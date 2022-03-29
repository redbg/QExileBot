#pragma once
#include <QObject>
#include <QThread>
#include <QJsonValue>
#include <QJsonObject>
#include <QMetaProperty>
#include "ExileClient.h"

class Account : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString Email MEMBER m_Email)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QString POESESSID MEMBER m_POESESSID)

public:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;
    QString m_POESESSID;

    ExileClient *m_ExileClient;

    QString m_LastError;

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
    void run() override
    {
        m_ExileClient = new ExileClient;
        connect(m_ExileClient, &ExileClient::errorOccurred, this, [=](QAbstractSocket::SocketError socketError)
                { m_LastError = m_ExileClient->errorString(); this->quit(); });

        connect(m_ExileClient, &ExileClient::LoginSuccess, this, [=](const QString &AccountName)
                { m_AccountName = AccountName; });

        // 连接登录服务器
        m_ExileClient->connectToLoginServer(Global::hostName, Global::port, m_Email, m_Password);

        this->exec();

        delete m_ExileClient;
    }
};
