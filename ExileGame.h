#pragma once
#include "ExileSocket.h"
#include "Global.h"
#include "Helper.h"
#include "ExileClient.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "GameObjectScene.h"
#include "GameObject.h"
#include <QGraphicsView>
#include <QJSEngine>
#include <QTimer>

class ExileGame : public ExileSocket
{
    Q_OBJECT

public:
    ExileClient *m_ExileClient;

    quint32 m_Ticket1;
    quint32 m_Ticket2;

    GameObjectScene m_Scene;

    QJSEngine *m_JSEngine;
    QTimer m_Tick;

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
        : ExileSocket(parent), m_JSEngine(new QJSEngine(this))
    {
        connect(this, &ExileSocket::connected, this, &ExileGame::on_game_connected, Qt::DirectConnection);
        connect(this, &ExileSocket::disconnected, this, &ExileGame::on_game_disconnected, Qt::DirectConnection);
        connect(this, &ExileSocket::errorOccurred, this, &ExileGame::on_game_errorOccurred, Qt::DirectConnection);
        connect(this, &ExileSocket::readyRead, this, &ExileGame::on_game_readyRead, Qt::DirectConnection);

        connect(
            &m_Scene, &GameObjectScene::move, this, [=](QPointF pos)
            {
                qDebug() << pos;
                this->SendSkill(pos.x(), pos.y(), 0x2909, 0x408);
            });

        m_JSEngine->installExtensions(QJSEngine::ConsoleExtension);
        m_JSEngine->globalObject().setProperty("game", m_JSEngine->newQObject(this));
        m_JSEngine->evaluate(Helper::File::ReadAll("script/script.js"), "script/script.js");
        connect(&m_Tick, &QTimer::timeout, this, [=]()
                { m_JSEngine->globalObject().property("tick").call(); });
        m_Tick.start(100);
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

    Q_INVOKABLE void SendSkill(qint32 x, qint32 y, quint16 skill, quint16 u)
    {
        static quint16 count = 0;

        this->writeId(0x18, "释放技能");

        this->write<qint32>(x);
        this->write<qint32>(y);

        this->write<quint16>(skill);
        this->write<quint16>(count);
        this->write<quint16>(u);

        count++;
    }

public:
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
