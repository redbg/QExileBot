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
    void run() override
    {
        m_BackendError.clear();

        // ExileClient
        m_ExileClient = new ExileClient;
        // m_ExileGame
        m_ExileGame = new ExileGame;
        m_ExileGame->m_ExileClient = m_ExileClient;

        // errorOccurred
        connect(
            m_ExileClient, &ExileClient::errorOccurred, this, [=](QAbstractSocket::SocketError socketError)
            { m_BackendError = m_ExileClient->m_BackendError; this->quit(); },
            Qt::DirectConnection);

        connect(
            m_ExileGame, &ExileGame::errorOccurred, this, [=](QAbstractSocket::SocketError socketError)
            { m_BackendError = m_ExileClient->m_BackendError; this->quit(); },
            Qt::DirectConnection);

        // 登录成功
        connect(
            m_ExileClient, &ExileClient::LoginSuccess, this, [=](const QString &AccountName)
            { m_AccountName = AccountName; },
            Qt::DirectConnection);

        // 选择角色成功,进入游戏
        connect(m_ExileClient, &ExileClient::SelectCharacterSuccess,
                m_ExileGame, &ExileGame::connectToGameServer,
                Qt::DirectConnection);

        // 连接登录服务器
        m_ExileClient->connectToLoginServer(Global::hostName, Global::port, m_Email, m_Password);

        // Loop
        this->exec();

        // delete
        delete m_ExileGame;
        delete m_ExileClient;
    }
};
