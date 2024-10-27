#include "q-words-counter.h"

#include <QTextStream>
#include <QRegularExpression>
#include <QWriteLocker>

#include <algorithm>
#include <vector>

#define delayForDemo() (std::this_thread::sleep_for(std::chrono::milliseconds(4)));


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
    mFlagContinue = false;
    if (mFlieProcessingWorker.joinable()) {
        mFlieProcessingWorker.join();
    }
    mWordsCounter.clear();
    mFile.reset();
    setReadCount(0);
}

bool QWordsCounter::pauseFileProcessing() {
    if (mLocker || !mFlieProcessingWorker.joinable()) return false;
    mLocker.reset(new QReadLocker(&mReadWriteLock));
    return true;
}

bool QWordsCounter::resumeFileProcessing() {
    if (!mLocker) return false;
    mLocker.reset();
    return true;
}

bool QWordsCounter::startFileProcessing() {
    cancelFileProcessing();
    if (!mFile.isOpen()) return false;
    mFlagContinue = true;
    mFlieProcessingWorker = std::thread([this]() {
        const QRegularExpression regExpWordSplitter(
            "[^" "а-яА-я" "a-zA-Z" "\\-_]");
        QString guessedWord;
        QTextStream fileStream(&mFile);
        while (!fileStream.atEnd() && mFlagContinue) {
            fileStream >> guessedWord;
            auto words = guessedWord.split(
                regExpWordSplitter, Qt::SplitBehaviorFlags::SkipEmptyParts);
            for (const auto& word: words) {
                delayForDemo();
                QWriteLocker writeLocker(&mReadWriteLock);
                ++mWordsCounter[word.toLower()];
            }
            setReadCount(fileStream.pos());
        }
        mFlagContinue = false;
    });
    return true;
}

QVariant QWordsCounter::getFrequentWords(unsigned int count) {
    std::vector<WordCount> topWords;
    bool toResume = pauseFileProcessing();
    topWords.resize(qMin(static_cast<size_t>(count), mWordsCounter.size()));
    std::partial_sort_copy(mWordsCounter.begin(), mWordsCounter.end(),
                           topWords.begin(), topWords.end(), cmpWordCount);
    if (toResume) {
        resumeFileProcessing();
    }
    return QVariant::fromValue(QList<QWordCount>(
        topWords.begin(), topWords.end()));
}

QUrl QWordsCounter::fileName() {
    return mFile.fileName();
}

void QWordsCounter::setFileName(QUrl fileName) {
    cancelFileProcessing();
    mFile.close();
    mFile.setFileName(fileName.toLocalFile());
    using OpenFlags = QIODevice::OpenModeFlag;
    if (!mFile.open(OpenFlags::Text | OpenFlags::ReadOnly)) {
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

QWordCount::QWordCount(const QString &name, quint64 value)
    : mName(name), mValue(value) {}

QWordCount::QWordCount(const std::pair<QString, quint64> &pair)
    : QWordCount(pair.first, pair.second) {}
