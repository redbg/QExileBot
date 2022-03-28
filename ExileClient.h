#pragma once
#include "ExileSocket.h"
#include "Global.h"

#include <cryptopp/osrng.h>
#include <cryptopp/dh.h>

#include <QCryptographicHash>

class ExileClient : public ExileSocket
{
    Q_OBJECT

public:
    enum MSG_CLIENT : quint16
    {
        ClientPublicKey = 0x2,
        Login = 0x3,
        SelectCharacter = 0xd,
        CreateCharacter = 0x11,
        GetLeagueList = 0x16,
    };
    Q_ENUM(MSG_CLIENT)

    enum MSG_SERVER : quint16
    {
        ServerPublicKey = 0x2,
        LoginResult = 0x4,
        CreateCharacterResult = 0x12,
        SelectCharacterResult = 0x13,
        CharacterList = 0x14,
        CloseSocket = 0x15,
        LeagueList = 0x19,
    };
    Q_ENUM(MSG_SERVER)

private:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;

    CryptoPP::byte m_PrivateKey[0x80];
    CryptoPP::byte m_PublicKey[0x80];

public:
    explicit ExileClient(QObject *parent = nullptr)
        : ExileSocket(parent)
    {
        // this->setObjectName("client");
        // QMetaObject::connectSlotsByName(this);
        connect(this, &ExileSocket::connected, this, &ExileClient::on_client_connected, Qt::DirectConnection);
        connect(this, &ExileSocket::disconnected, this, &ExileClient::on_client_disconnected, Qt::DirectConnection);
        connect(this, &ExileSocket::readyRead, this, &ExileClient::on_client_readyRead, Qt::DirectConnection);
        connect(this, &ExileSocket::errorOccurred, this, &ExileClient::on_client_errorOccurred, Qt::DirectConnection);
    }

    virtual ~ExileClient() {}

public slots:
    void on_client_connected();
    void on_client_disconnected();
    void on_client_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_client_readyRead();

public:
    void connectToLoginServer(const QString &hostName, quint16 port, const QString &Email, const QString &Password)
    {
        qDebug() << "连接到登录服务器...";

        this->m_Email = Email;
        this->m_Password = Password;
        this->connectToHost(hostName, port);
    }

public:
    void SendPublicKey();
    void RecvPublicKey();

    void SendLogin(const QString &Email, const QString &Password);
};
