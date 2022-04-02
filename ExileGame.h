#pragma once
#include "ExileSocket.h"
#include "Global.h"
#include "Helper.h"
#include "ExileClient.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class ExileGame : public ExileSocket
{
    Q_OBJECT

public:
    ExileClient *m_ExileClient;

    quint32 m_Ticket1;
    quint32 m_Ticket2;

    quint32 m_WorldAreaId;
    QString m_LeagueName;
    quint32 m_Seed;

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
    void connectToGameServer(quint32 Ticket1, quint32 WorldAreaId, quint32 Ticket2, quint16 Port, quint32 Address, QByteArray Key);

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

    void on_game_readyRead();

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

    void SendTileHash(quint32 tileHash, quint32 doodadHash)
    {
        qDebug() << QString("SendTileHash(%1, %2)").arg(tileHash).arg(doodadHash);
        this->write<quint16>(0x53);
        this->write(tileHash);
        this->write(doodadHash);
    }

signals:
    void signal_BackendError(quint16 BackendError);
};
