#pragma once

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QVariant>
#include <QScopedPointer>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QAtomicInteger>

#include <map>
#include <thread>


class QWordCount {
    Q_GADGET
    
protected:
    QString mName;
    quint64 mValue = 0;
    
public:    
    Q_PROPERTY(QString name MEMBER mName);
    Q_PROPERTY(quint64 value MEMBER mValue);
    
public:
    QWordCount() = default;
    QWordCount(const QString& name, quint64 value);
    QWordCount(const std::pair<QString, quint64>& pair);;
};
Q_DECLARE_METATYPE(QWordCount);


class QWordsCounter : public QObject
{
    Q_OBJECT;
        
private:
    QFile mFile;
    quint64 mFileSize = 0;
    quint64 mReadCount = 0;
    QReadWriteLock mReadWriteLock;
    QScopedPointer<const QReadLocker> mLocker;
    QAtomicInteger<bool> mFlagContinue;
    
    std::thread mFlieProcessingWorker;
    std::map<QString, quint64> mWordsCounter;
    
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
