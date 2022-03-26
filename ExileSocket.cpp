#include "ExileSocket.h"

void ExileSocket::SetKeyWithIV(QByteArray key)
{
    Q_ASSERT(key.size() == 0x40);
    this->m_Encryption.SetKeyWithIV((CryptoPP::byte *)key.data(), 0x20,
                                    (CryptoPP::byte *)&key.data()[0x20], 8);
    this->m_Decryption.SetKeyWithIV((CryptoPP::byte *)key.data(), 0x20,
                                    (CryptoPP::byte *)&key.data()[0x30], 8);
}

void ExileSocket::EnableCrypto()
{
    this->isCrypto = true;
}

// ========== write ==========

qint64 ExileSocket::write(QByteArray data)
{
    if (data.size() == 0)
    {
        return 0;
    }

    if (this->isCrypto == true)
    {
        this->m_Encryption.ProcessData((CryptoPP::byte *)data.data(),
                                       (CryptoPP::byte *)data.data(),
                                       data.size());
    }

    return QTcpSocket::write(data);
}

qint64 ExileSocket::write(const char *data, qint64 len)
{
    return this->write(QByteArray(data, len));
}

qint64 ExileSocket::write(QString data)
{
    qint64 size = this->write<quint16>(data.size());
    size += this->write((char *)data.data(), data.size() * sizeof(char16_t));
    return size;
}

qint64 ExileSocket::write(const char *data)
{
    return this->write(QString(data));
}

// ========== read ==========

QByteArray ExileSocket::read(qint64 maxlen)
{
    QByteArray data;

    if (maxlen == 0)
    {
        return data;
    }

    do
    {
        data.append(QTcpSocket::read(maxlen - data.size()));
    } while (data.size() != maxlen && this->waitForReadyRead());

    if (this->isCrypto == true)
    {
        this->m_Decryption.ProcessData((CryptoPP::byte *)data.data(),
                                       (CryptoPP::byte *)data.data(),
                                       data.size());
    }

    return data;
}

QByteArray ExileSocket::readAll()
{
    return this->read(QTcpSocket::size());
}

QString ExileSocket::readString()
{
    quint16 size = this->read<quint16>();
    QByteArray data = this->read(size * sizeof(char16_t));
    return data.size() == size * sizeof(char16_t) ? QString::fromUtf16((char16_t *)data.data(), size) : "";
}
