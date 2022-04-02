#pragma once
#include <QObject>
#include <QThread>
#include <QJsonValue>
#include <QJsonObject>
#include <QMetaProperty>
#include "ExileClient.h"
#include "ExileGame.h"

class Account : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString Email MEMBER m_Email)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QString POESESSID MEMBER m_POESESSID)
    Q_PROPERTY(QString BackendError MEMBER m_BackendError)

public:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;
    QString m_POESESSID;

    ExileClient *m_ExileClient;
    ExileGame *m_ExileGame;

    QString m_BackendError;

public:
    explicit Account(QObject *parent = nullptr)
        : QThread(parent)
    {
    }

    virtual ~Account()
    {
        this->quit();
        this->wait();
    }

public:
    QJsonValue toJson();
    void fromJson(QJsonValue JsonValue);

protected:
    void run() override;

public slots:
    void on_BackendError(quint16 Result)
    {
        quint16 BackendErrorIndex = Result - 1;
        QJsonObject BackendError = Helper::Data::GetBackendError(BackendErrorIndex);
        m_BackendError = BackendError.value("Id").toString();

        qWarning() << "BackendError:" << m_BackendError;

        this->quit();
    }
};
