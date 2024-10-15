#pragma once

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QVariant>

#include <map>
#include <thread>
#include <mutex>
#include <atomic>



class QWordCount {
    Q_GADGET
    
protected:
    QString mName;
    quint64 mValue = 0;
    
public:
    QWordCount& operator=(const QWordCount& other) {
        mName = other.mName;
        mValue = other.mValue;
        return *this;
    }
    
    Q_PROPERTY(QString name READ name WRITE setName);
    Q_PROPERTY(quint64 value READ value WRITE setValue);
    
public:
    QString name() const;
    quint64 value() const;
    void setName(QString name);
    void setValue(quint64 value);
};
Q_DECLARE_METATYPE(QWordCount);


class QWordsCounter : public QObject
{
    Q_OBJECT;
    
protected:
    QFile mFile;
    quint64 mFileSize = 0;
    quint64 mReadCount = 0;
    
    std::thread mThread;
    std::map<QString, quint64> mWordsCounter;
    std::mutex mMutex;
    std::atomic_flag mFlagContinue;
    std::unique_ptr<std::lock_guard<std::mutex>> mLock;
    
public:
    explicit QWordsCounter(QObject *parent = nullptr);
    virtual ~QWordsCounter() override;
    
    Q_PROPERTY(QUrl fileName READ fileName WRITE setFileName NOTIFY fileNameChanged);
    Q_PROPERTY(quint64 progressCurrent READ progressCurrent NOTIFY progressCurrentChanged);
    Q_PROPERTY(quint64 progressTotal READ progressTotal NOTIFY progressTotalChanged);
    
    Q_INVOKABLE void cancelFileProcessing();
    Q_INVOKABLE bool pauseFileProcessing();
    Q_INVOKABLE bool resumeFileProcessing();
    Q_INVOKABLE bool startFileProcessing();
    Q_INVOKABLE QVariant getFrequentWords(unsigned count);
    
public:
    QUrl fileName();
    void setFileName(QUrl fileName);
    quint64 progressTotal();
    quint64 progressCurrent();
    void setReadCount(quint64 count);
    void setFileSize(quint64 size);
        
signals:
    void fileNameChanged();
    void progressCurrentChanged();
    void progressTotalChanged();
};
