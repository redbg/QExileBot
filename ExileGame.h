#pragma once
#include "ExileSocket.h"
#include "Global.h"
#include "Helper.h"
#include "ExileClient.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QGraphicsScene>

class ExileGame : public ExileSocket
{
    Q_OBJECT

public:
    ExileClient *m_ExileClient;

    quint32 m_Ticket1;
    quint32 m_Ticket2;

    // WorldArea
    quint32 m_WorldAreaId;
    QString m_League;
    quint32 m_Seed;
    quint32 m_TileHash;
    quint32 m_DoodadHash;

    // Terrain
    quint32 m_TerrainWidth;
    quint32 m_TerrainHeight;
    QByteArray m_TerrainData;
    QImage m_MiniMap;
    QGraphicsScene m_Scene;

    quint32 m_PlayerId;

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

        this->writeId(0x3, "SendTicket");

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
        this->writeId(0x53, "SendTileHash");
        this->write(tileHash);
        this->write(doodadHash);
    }

public:
    QImage miniMap()
    {
        QImage map(m_TerrainWidth, m_TerrainHeight, QImage::Format_Indexed8);

        for (int i = 0; i < 256; i++)
        {
            map.setColor(i, qRgb(0, 0, 0));
        }

        map.setColor('1', qRgb(255, 255, 255));

        for (quint32 y = 0; y < m_TerrainHeight; y++)
        {
            for (quint32 x = 0; x < m_TerrainWidth; x++)
            {
                map.setPixel(x, y, m_TerrainData.at((y * m_TerrainWidth) + x + y));
            }
        }

        return map;
    }

    template <typename T>
    T readData(QDataStream &DataStream)
    {
        QByteArray data(sizeof(T), 0);
        DataStream.readRawData(data.data(), sizeof(T));
        return *(T *)data.data();
    }

    QByteArray readData(QDataStream &DataStream, quint64 size)
    {
        QByteArray data(size, 0);
        DataStream.readRawData(data.data(), size);
        return data;
    }

signals:
    void signal_BackendError(quint16 BackendError);
};
