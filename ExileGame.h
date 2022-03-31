#pragma once
#include "ExileSocket.h"
#include "Global.h"
#include "Helper.h"
#include "ExileClient.h"

class ExileGame : public ExileSocket
{
    Q_OBJECT

public:
    ExileClient *m_ExileClient;

    quint32 m_Ticket1;
    quint32 m_Ticket2;
    quint32 m_WorldAreaId;

public:
    enum MSG_CLIENT : quint16
    {
    };
    Q_ENUM(MSG_CLIENT)

    enum MSG_SERVER : quint16
    {
    };
    Q_ENUM(MSG_SERVER)

public:
    explicit ExileGame(QObject *parent = nullptr)
        : ExileSocket(parent)
    {
        connect(this, &ExileSocket::connected, this, &ExileGame::on_game_connected, Qt::DirectConnection);
        connect(this, &ExileSocket::disconnected, this, &ExileGame::on_game_disconnected, Qt::DirectConnection);
        connect(this, &ExileSocket::errorOccurred, this, &ExileGame::on_game_errorOccurred, Qt::DirectConnection);
        connect(this, &ExileSocket::readyRead, this, &ExileGame::on_game_readyRead, Qt::DirectConnection);
    }

    virtual ~ExileGame() {}

public slots:
    void connectToGameServer(quint32 Ticket1, quint32 WorldAreaId, quint32 Ticket2, quint16 Port, quint32 Address, QByteArray Key)
    {
        m_Ticket1 = Ticket1;
        m_WorldAreaId = WorldAreaId;
        m_Ticket2 = Ticket2;
        this->SetKeyWithIV(Key);

        this->connectToHost(QHostAddress(Address), Port);

        qDebug() << QString("connectToGameServer:[%1:%2] Ticket1:[%3] WorldAreaId:[%4] Ticket2:[%5]")
                        .arg(QHostAddress(Address).toString())
                        .arg(Port)
                        .arg(Ticket1)
                        .arg(WorldAreaId)
                        .arg(Ticket2);
    }

    void on_game_connected()
    {
        // qDebug() << "on_game_connected";
        SendTicket();
    }

    void on_game_disconnected()
    {
        // qDebug() << "on_game_disconnected";
    }

    void on_game_errorOccurred(QAbstractSocket::SocketError socketError)
    {
        qWarning() << socketError << this->errorString();
    }

    void on_game_readyRead()
    {
        while (this->size())
        {
            quint16 PacketId = this->read<quint16>();

            switch (PacketId)
            {
            case 0x5:
                // 开启加密
                this->EnableCrypto();
                break;
            case 0x13:
            {
                quint16 size = this->read<quint16>();
                this->read<quint16>();
                this->read(size * 2);
                this->read(size);
            }
            break;
            default:
                qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                                  .arg(QString::number(PacketId, 16))
                                  .arg(this->readAll().toHex(' '));
                break;
            }
        }
    }

public:
    void SendTicket()
    {
        qDebug() << "SendTicket";

        this->write<quint16>(0x3);

        this->write<quint32>(this->m_Ticket2);
        this->write<quint16>(0x2);
        this->write(m_ExileClient->m_CharacterModel.currentCharacter()->m_Name);
        this->write<quint32>(this->m_Ticket1);

        this->write<quint32>(0x00);
        this->write<quint8>(0x40);
        this->write<quint8>(0x00);
        this->write<quint8>(0x00);
        this->write<quint8>(0x01);
    }
};
