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
        m_PacketListModel.append();
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
        m_PacketListModel.append();
        m_PacketListModel.m_PacketList.last()->m_PacketType = "recv";
        return this->read<quint16>("PacketId");
    }

    // 无符号
    quint32 ReadVarint(QString name = QString())
    {
        quint8 v = this->read<quint8>(name);

        if ((v & 0x80) == 0)
        {
            return v;
        }

        if ((v & 0xC0) == 0x80)
        {
            return this->read<quint8>(name) | (v & 0x3f) << 8;
        }
        else if ((v & 0xE0) == 0xC0)
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
        }
        else if ((v & 0xF0) == 0xE0)
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            quint8 v3 = this->read<quint8>(name);

            return v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
        }
        else
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            quint8 v3 = this->read<quint8>(name);
            quint8 v4 = this->read<quint8>(name);
            return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
        }
    }

    // 有符号
    qint32 ReadVarint1(QString name = QString())
    {
        quint8 v = this->read<quint8>(name);

        if ((v & 0x80) == 0)
        {
            if ((v & 0x40) == 0)
            {
                return v;
            }
            else
            {
                return v | 0xffffff80;
            }
        }

        if ((v & 0xC0) == 0x80)
        {
            return this->read<quint8>(name) | (v & 0x3f) << 8;
        }
        else if ((v & 0xE0) == 0xC0)
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
        }
        else if ((v & 0xF0) == 0xE0)
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            quint8 v3 = this->read<quint8>(name);

            qint64 result = v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
            result = v & 8 ? result : result | 0xF0000000;
            return result;
        }
        else
        {
            quint8 v1 = this->read<quint8>(name);
            quint8 v2 = this->read<quint8>(name);
            quint8 v3 = this->read<quint8>(name);
            quint8 v4 = this->read<quint8>(name);
            return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
        }
    }

public slots:
    void on_socket_disconnected() { isCrypto = false; }
};
