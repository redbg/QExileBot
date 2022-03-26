#pragma once
#include <QTcpSocket>
#include <QtEndian>
#include <cryptopp/salsa.h>

class ExileSocket : public QTcpSocket
{
    Q_OBJECT

private:
    CryptoPP::Salsa20::Encryption m_Encryption;
    CryptoPP::Salsa20::Decryption m_Decryption;

    bool isCrypto;

public:
    explicit ExileSocket(QObject *parent = nullptr)
        : QTcpSocket(parent), isCrypto(false)
    {
        connect(this, &QTcpSocket::disconnected, this, &ExileSocket::on_socket_disconnected, Qt::DirectConnection);
    }

    virtual ~ExileSocket() {}

public:
    void SetKeyWithIV(QByteArray key);
    void EnableCrypto();

public:
    qint64 write(QByteArray data);
    qint64 write(const char *data, qint64 len);
    qint64 write(QString data);
    qint64 write(const char *data);

    template <typename T>
    qint64 write(T data)
    {
        data = qbswap(data);
        return this->write((char *)&data, sizeof(data));
    }

public:
    QByteArray read(qint64 maxlen);
    QByteArray readAll();
    QString readString();

    template <typename T>
    T read()
    {
        T data = *(T *)this->read(sizeof(T)).data();
        return qbswap(data);
    }

public slots:
    void on_socket_disconnected() { isCrypto = false; }
};
