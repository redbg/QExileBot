#include "ExileClient.h"

// public slots:

void ExileClient::connectToLoginServer(const QString &hostName, quint16 port, const QString &Email, const QString &Password)
{
    qDebug() << QString("连接登录服务器:[%1:%2]").arg(hostName).arg(port);

    this->m_Email = Email;
    this->m_Password = Password;
    this->connectToHost(hostName, port);
}

void ExileClient::on_client_connected()
{
    qDebug() << "on_client_connected";
    this->SendPublicKey();
}

void ExileClient::on_client_disconnected()
{
    qDebug() << "on_client_disconnected";
}

void ExileClient::on_client_errorOccurred(QAbstractSocket::SocketError socketError)
{
    qWarning() << socketError << this->errorString();
}

void ExileClient::on_client_readyRead()
{
    while (this->size())
    {
        quint16 PacketId = this->read<quint16>();

        switch ((MSG_SERVER)PacketId)
        {
        case MSG_SERVER::ServerPublicKey:
            this->RecvPublicKey();
            this->SendLogin(m_Email, m_Password);
            break;
        case MSG_SERVER::LoginResult:
            if (this->RecvLoginResult())
            {
                // 登录成功,获取赛区列表
                SendGetLeagueList();
            }
            break;
        case MSG_SERVER::LeagueList:
            RecvLeagueList();
            break;
        case MSG_SERVER::CharacterList:
            this->RecvCharacterList();
            if (this->m_CharacterModel.m_CharacterList.size())
            {
                // 有角色,选择角色进入游戏
                SendSelectCharacter();
            }
            else
            {
                // 没有角色,创建角色
                SendCreateCharacter(Helper::GetRandomString(20), Global::league, Global::classType);
            }
            break;
        case MSG_SERVER::CreateCharacterResult:
            RecvCreateCharacterResult();
            break;
        case MSG_SERVER::SelectCharacterResult:
            RecvSelectCharacterResult();
            break;
        case MSG_SERVER::CloseSocket:
            RecvCloseSocket();
            break;
        default:
            qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                              .arg(QString::number(PacketId, 16))
                              .arg(this->readAll().toHex(' '));
            break;
        }
    }
}

// PublicKey

void ExileClient::SendPublicKey()
{
    // qDebug() << "发送公钥";

    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::DH dh(Global::p, Global::q, Global::g);
    dh.GenerateKeyPair(rng, this->m_PrivateKey, this->m_PublicKey);

    this->writeId((quint16)MSG_CLIENT::ClientPublicKey, "发送公钥");   // PacketId
    this->write<quint16>(sizeof(this->m_PublicKey));                   // PublicKeySize
    this->write((char *)this->m_PublicKey, sizeof(this->m_PublicKey)); // PublicKey
    this->write<quint16>(0);                                           // DsaKeySize
}

void ExileClient::RecvPublicKey()
{
    // qDebug() << "收到公钥";

    CryptoPP::DH dh(Global::p, Global::q, Global::g);

    QByteArray serverPublicKey = this->read(this->read<quint16>()); // serverPublicKey
    QByteArray serverDsaKey = this->read(this->read<quint16>());    // serverDsaKey

    // 生成密钥
    CryptoPP::byte agreedValue[0x80] = {};
    dh.Agree(agreedValue, this->m_PrivateKey, (CryptoPP::byte *)serverPublicKey.data());

    QByteArray key((char *)Global::KeyHeader, sizeof(Global::KeyHeader));
    key.append((char *)agreedValue, sizeof(agreedValue));

    QByteArray keyHash = QCryptographicHash::hash(key, QCryptographicHash::Sha512);

    // 开启加密
    this->SetKeyWithIV(keyHash);
    this->EnableCrypto();
}

// 登录

void ExileClient::SendLogin(const QString &Email, const QString &Password)
{
    qDebug() << QString("登录 Email:%1 Password:%2").arg(Email).arg(Password);

    // PasswordHash
    QByteArray PasswordHash = QCryptographicHash::hash(
        QByteArray((char *)Password.data(), Password.size() * 2), QCryptographicHash::Sha256);

    // MacAddressHash
    QByteArray MacAddressHash = QCryptographicHash::hash(
        QByteArray((char *)Email.data(), Email.size() * 2), QCryptographicHash::Sha256);

    this->writeId((quint16)MSG_CLIENT::Login, "登录");                   // PacketId
    this->write<quint32>(0x2);                                           // ??
    this->write(Email);                                                  // Email
    this->write((char *)Global::ClientHash, sizeof(Global::ClientHash)); // ClientHash
    this->write(PasswordHash);                                           // PasswordHash
    this->write(MacAddressHash);                                         // MacAddressHash
    this->write<quint8>(1);                                              // ??
    this->write<quint8>(0);                                              // 1 = 是否使用记住的密码
    this->write<quint8>(0x40);                                           // ??
}

bool ExileClient::RecvLoginResult()
{
    qDebug() << "收到登录结果";

    quint16 Result = this->read<quint16>(); // LoginResult

    if (Result != 0)
    {
        this->readAll();
        emit signal_BackendError(Result);
        return false;
    }

    this->readString();                 // ??
    this->read(0x20);                   // 保存的密码
    this->read<quint8>();               // ??
    m_AccountName = this->readString(); // AccountName

    qDebug() << QString("登录成功! AccountName:%1").arg(m_AccountName);

    emit this->LoginSuccess(m_AccountName);

    return true;
}

// 获取赛区列表

void ExileClient::SendGetLeagueList()
{
    qDebug() << "请求赛区列表";
    this->writeId((quint16)MSG_CLIENT::GetLeagueList, "请求赛区列表");
}

void ExileClient::RecvLeagueList()
{
    qDebug() << "收到赛区列表";

    this->read(8); // ??

    quint32 size = this->read<quint32>();

    for (size_t i = 0; i < size; i++)
    {
        qDebug() << "[" << i << "]" << this->readString(); // LeagueName
        this->readString();

        this->readString();
        this->readString();

        this->read(8);
        this->read(8);
        this->read(8);
        this->read(2);
        this->read(1);
        this->read(1);
    }
}

// 创建角色

void ExileClient::SendCreateCharacter(QString Name, QString League, Character::ClassType classType)
{
    QString ClassTypeString = Character::GetClassTypeById(classType);
    QString ClassNameString = Character::GetClassNameById(classType);

    qDebug() << QString("创建角色 name:%1 league:%2 classType:%3 className:%4")
                    .arg(Name)
                    .arg(League)
                    .arg(ClassTypeString)
                    .arg(ClassNameString);

    this->writeId((quint16)MSG_CLIENT::CreateCharacter, "创建角色"); // PacketId
    this->write(Name);                                               // 角色名
    this->write(League);                                             // 赛区名
    this->write<quint32>(0);                                         // ??
    this->write<quint32>(0);                                         // ??
    this->write(ClassTypeString + "Default");                        // 职业
    this->write(QByteArray(0x20, 0));                                // ??
}

bool ExileClient::RecvCreateCharacterResult()
{
    qDebug() << "收到创建角色结果";

    quint16 Result = this->read<quint16>();
    this->read<quint8>();

    if (Result != 0)
    {
        emit signal_BackendError(Result);
        return false;
    }

    qDebug() << "创建角色成功";
    return true;
}

// 收到角色列表

void ExileClient::RecvCharacterList()
{
    qDebug() << "收到角列表";

    m_CharacterModel.clear();

    quint32 CharacterSize = this->read<quint32>(); // Size

    for (quint32 i = 0; i < CharacterSize; i++)
    {
        Character *character = new Character(&m_CharacterModel);

        character->m_Name = this->readString();          // Name
        character->m_League = this->readString();        // League
        character->m_LeagueId = this->read<quint8>();    // LeagueId
        character->m_Unknown1 = this->read<quint8>();    // ??
        character->m_Level = this->read<quint8>();       // Level
        character->m_Experience = this->read<quint32>(); // Experience
        character->m_ClassId = this->read<quint8>();     // ClassId
        character->m_Unknown2 = this->read<quint16>();   // ??
        character->m_Unknown3 = this->read<quint8>();    // ??
        character->m_Unknown4 = this->read<quint8>();    // ??

        m_CharacterModel.append(character);

        qDebug() << "[" << i << "]" << character->toJson();
    }

    m_CharacterModel.m_LastSelectIndex = this->read<quint32>(); // LastSelectIndex
    m_CharacterModel.m_Unknown1 = this->read<quint8>();         // ??
}

// 选择角色进入游戏

void ExileClient::SendSelectCharacter()
{
    quint32 Index = m_CharacterModel.m_LastSelectIndex;

    qDebug() << QString("选择角色,进入游戏 Index:%1").arg(Index);

    this->writeId((quint16)MSG_CLIENT::SelectCharacter, "选择角色,进入游戏"); // PacketId
    this->write<quint8>(0);                                                   // 语言
    this->write<quint32>(Index);                                              // 角色下标
}

void ExileClient::RecvSelectCharacterResult()
{
    qDebug() << "收到选择角色结果,正在进入游戏...";

    quint32 Ticket1 = this->read<quint32>();
    quint32 WorldAreaId = this->read<quint32>();
    quint32 Ticket2 = this->read<quint32>(); // 可能是地图实例Id

    quint8 size = this->read<quint8>();

    quint16 Port = 0;
    quint32 Address = 0;

    for (quint8 i = 0; i < size; i++)
    {
        this->read<quint16>();
        Port = this->read<quint16>();
        Address = this->read<quint32>();
        this->read(0x14);
    }

    QByteArray Key = this->read(0x40);

    // 连接游戏服务器
    qDebug() << QString("收到游戏服务器连接地址:[%1:%2] Ticket1:[%3] WorldAreaId:[%4] Ticket2:[%5]")
                    .arg(QHostAddress(Address).toString())
                    .arg(Port)
                    .arg(Ticket1)
                    .arg(WorldAreaId)
                    .arg(Ticket2);

    emit SelectCharacterSuccess(Ticket1, WorldAreaId, Ticket2, Port, Address, Key);
}

void ExileClient::RecvCloseSocket()
{
    qDebug() << "进入游戏成功,ClientSocket关闭";
    this->read<quint16>();
    this->disconnectFromHost();
}