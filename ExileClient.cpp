#include "ExileClient.h"

void ExileClient::on_client_connected()
{
    // qDebug() << "on_client_connected";
    this->SendPublicKey();
}

void ExileClient::on_client_disconnected()
{
    // qDebug() << "on_client_disconnected";
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
            this->RecvLoginResult();
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

    this->write<quint16>((quint16)MSG_CLIENT::ClientPublicKey);        // PacketId
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

// Login

void ExileClient::SendLogin(const QString &Email, const QString &Password)
{
    qDebug() << QString("登录 Email:%1 Password:%2").arg(Email).arg(Password);

    // PasswordHash
    QByteArray PasswordHash = QCryptographicHash::hash(
        QByteArray((char *)Password.data(), Password.size() * 2), QCryptographicHash::Sha256);

    // MacAddressHash
    QByteArray MacAddressHash = QCryptographicHash::hash(
        QByteArray((char *)Email.data(), Email.size() * 2), QCryptographicHash::Sha256);

    this->write<quint16>((quint16)MSG_CLIENT::Login);                    // PacketId
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

    quint16 LoginResult = this->read<quint16>(); // LoginResult

    if (LoginResult != 0)
    {
        quint16 BackendErrorIndex = LoginResult - 1;
        QJsonObject BackendError = Helper::Data::GetBackendError(BackendErrorIndex);
        QString errorString = BackendError.value("Id").toString();

        this->readAll();
        this->setErrorString(errorString);
        emit errorOccurred(SocketError::RemoteHostClosedError);

        return false;
    }

    this->readString();                 // ??
    this->read(0x20);                   // 保存的密码
    this->read<quint8>();               // ??
    m_AccountName = this->readString(); // AccountName

    qDebug() << QString("登录成功! AccountName:%1").arg(m_AccountName);

    return true;
}