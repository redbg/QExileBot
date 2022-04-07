#pragma once
#include <QTcpSocket>
#include <QtEndian>
#include <cryptopp/salsa.h>
#include "PacketListModel.h"

class ExileSocket : public QTcpSocket
{
    Q_OBJECT

public:
    CryptoPP::Salsa20::Encryption m_Encryption;
    CryptoPP::Salsa20::Decryption m_Decryption;

    bool isCrypto;

    PacketListModel m_PacketListModel;

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
    qint64 write(QByteArray data, int type = QMetaType::QByteArray, QString name = QString());
    qint64 write(const char *data, qint64 len, int type = QMetaType::QByteArray, QString name = QString());
    qint64 write(QString data, QString name = QString());
    qint64 write(const char *data, QString name = QString());

    template <typename T>
    qint64 write(T data, QString name = QString())
    {
        data = qbswap(data);
        return this->write((char *)&data, sizeof(data), qMetaTypeId<T>(), name);
    }

    qint64 writeId(quint16 packetId, QString packetName = QString())
    {
        m_PacketListModel.m_PacketList.append(new Packet);
        m_PacketListModel.m_PacketList.last()->m_PacketType = "send";
        m_PacketListModel.m_PacketList.last()->m_PacketName = packetName;

        return this->write<quint16>(packetId, "PacketId");
    }

public:
    QByteArray read(qint64 maxlen, QString name = QString(), int type = QMetaType::QByteArray);
    QByteArray readAll(QString name = QString());
    QString readString(QString name = QString());

    template <typename T>
    T read(QString name = QString())
    {
        T data = *(T *)this->read(sizeof(T), name, qMetaTypeId<T>()).data();
        return qbswap(data);
    }

    quint16 readId()
    {
        m_PacketListModel.m_PacketList.append(new Packet);
        m_PacketListModel.m_PacketList.last()->m_PacketType = "recv";
        return this->read<quint16>("PacketId");
    }

public slots:
    void on_socket_disconnected() { isCrypto = false; }
};
