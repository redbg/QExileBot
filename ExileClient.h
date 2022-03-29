#pragma once
#include "ExileSocket.h"
#include "Global.h"
#include "Helper.h"
#include "CharacterModel.h"

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

public:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;

    CharacterModel m_CharacterModel;

    QString m_BackendError;

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
        connect(this, &ExileSocket::errorOccurred, this, &ExileClient::on_client_errorOccurred, Qt::DirectConnection);
        connect(this, &ExileSocket::readyRead, this, &ExileClient::on_client_readyRead, Qt::DirectConnection);
    }

    virtual ~ExileClient() {}

public slots:
    void on_client_connected();
    void on_client_disconnected();
    void on_client_errorOccurred(QAbstractSocket::SocketError socketError);
    void on_client_readyRead();

public:
    void connectToLoginServer(const QString &hostName, quint16 port, const QString &Email, const QString &Password);

    // 公钥
    void SendPublicKey();
    void RecvPublicKey();

    // 登录
    void SendLogin(const QString &Email, const QString &Password);
    bool RecvLoginResult();

    // 请求赛区列表
    void SendGetLeagueList();
    void RecvLeagueList();

    // 创建角色
    void SendCreateCharacter(QString Name, QString League, Character::ClassType classType);
    bool RecvCreateCharacterResult();

    // 角色列表
    void RecvCharacterList();

    // 选择角色进入游戏
    void SendSelectCharacter();
    void RecvSelectCharacterResult();

    // 进入游戏成功关闭连接
    void RecvCloseSocket();

signals:
    void LoginSuccess(const QString &AccountName);
    void SelectCharacterSuccess(ExileClient *client, quint32 Ticket1, quint32 WorldAreaId, quint32 Ticket2, quint16 Port, quint32 Address, QByteArray Key);
};
