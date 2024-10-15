#include "q-words-counter.h"

#include <QTextStream>
#include <QRegularExpression>

#include <algorithm>
#include <vector>


using WordCount = std::pair<QString, quint64>;
constexpr static bool cmpWordCount(
    const WordCount& left, const WordCount& right) {
    return left.second > right.second;
}

QWordsCounter::QWordsCounter(QObject *parent)
    : QObject{parent}
{}

QWordsCounter::~QWordsCounter() {
    cancelFileProcessing();
}

void QWordsCounter::cancelFileProcessing() {
    resumeFileProcessing();
    mFlagContinue.clear();
    if (mThread.joinable()) {
        mThread.join();
    }
    mWordsCounter.clear();
    mFile.reset();
    setReadCount(0);
}

bool QWordsCounter::pauseFileProcessing() {
    if (mLock || !mThread.joinable()) return false;
    mLock = std::make_unique<std::lock_guard<std::mutex>>(mMutex);
    return true;
}

bool QWordsCounter::resumeFileProcessing() {
    if (!mLock) return false;
    mLock.reset();
    return true;
}

bool QWordsCounter::startFileProcessing() {
    cancelFileProcessing();
    if (!mFile.isOpen()) return false;
    // set pos pointer to beginning of file
    mFlagContinue.test_and_set();
    mThread = std::thread([this]() {
        const QRegularExpression regExpWordSplitter(
            "[^" "а-яА-я" "a-zA-Z" "\\-_]");
        QString guessedWord;
        QTextStream fileStream(&mFile);
        while (!fileStream.atEnd() && mFlagContinue.test()) {
            fileStream >> guessedWord;
            auto words = guessedWord.split(
                regExpWordSplitter, Qt::SplitBehaviorFlags::SkipEmptyParts);
            for (const auto& word: words) {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
                std::lock_guard lockGuard(mMutex);
                ++mWordsCounter[word.toLower()];
            }
            setReadCount(fileStream.pos());
        }
        mFlagContinue.clear();
    });
    return true;
}

QVariant QWordsCounter::getFrequentWords(unsigned int count) {
    QList<QWordCount> ret;
    std::vector<WordCount> topWords;
    bool toResume = pauseFileProcessing();
    topWords.resize(qMin(static_cast<size_t>(count), mWordsCounter.size()));
    std::partial_sort_copy(mWordsCounter.begin(), mWordsCounter.end(),
                           topWords.begin(), topWords.end(), cmpWordCount);
    if (toResume) {
        resumeFileProcessing();
    }
    // qDebug() << "Top words: "<< topWords;
    for (auto& wordCount: topWords) {
        ret.append(QWordCount());
        ret.back().setName(std::move(wordCount.first));
        ret.back().setValue(std::move(wordCount.second));
    }
    return QVariant ::fromValue(ret);
    // return  QVariant::fromValue(QList<QWordCount>({
    //     QWordCount(),
    //     QWordCount(),
    //     QWordCount(),
    // }));
    // return QWordCount();
    // return  ({4,12, QWordCount(), 22});
}

QUrl QWordsCounter::fileName() {
    return mFile.fileName();
}

void QWordsCounter::setFileName(QUrl fileName) {
    // qDebug() << "File name " << fileName;
    cancelFileProcessing();
    mFile.close();
    mFile.setFileName(fileName.toLocalFile());
    using OpenFlags = QIODevice::OpenModeFlag;
    if (!mFile.open(OpenFlags::Text | OpenFlags::ReadOnly)) {
        // qDebug() << "Can not open" << mFile.fileName();
        mFile.setFileName(QString());
    }
    setFileSize(mFile.size());
    emit fileNameChanged();
}

quint64 QWordsCounter::progressTotal() {
    return mFileSize;
}

quint64 QWordsCounter::progressCurrent() {
    return mReadCount;
}

void QWordsCounter::setReadCount(quint64 count) {
    mReadCount = count;
    emit progressCurrentChanged();
}

void QWordsCounter::setFileSize(quint64 size) {
    mFileSize = size;
    emit progressTotalChanged();
}


QString QWordCount::name() const {
    return mName;
}

quint64 QWordCount::value() const {
    return mValue;
}

void QWordCount::setName(QString name) {
    mName = name;
}

void QWordCount::setValue(quint64 value) {
    mValue = value;
}
