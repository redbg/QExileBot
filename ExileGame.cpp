#include "ExileGame.h"

void ExileGame::connectToGameServer(quint32 Ticket1, quint32 WorldAreaId, quint32 Ticket2, quint16 Port, quint32 Address, QByteArray Key)
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

void ExileGame::on_game_readyRead()
{
    while (this->size())
    {
        quint16 PacketId = this->read<quint16>();

        switch (PacketId)
        {
        case 0x5:
        {
            // 开启加密
            this->EnableCrypto();
            break;
        }
        case 0x10:
        {
            // 收到地图信息
            m_WorldAreaId = this->read<quint16>();
            m_LeagueName = this->readString();
            m_Seed = this->read<quint32>();
            this->readAll();

            QNetworkAccessManager *mgr = new QNetworkAccessManager;
            QUrl url(QString("http://127.0.0.1:6112/world?id=%1&seed=%2").arg(m_WorldAreaId).arg(m_Seed));
            mgr->get(QNetworkRequest(url));
            connect(mgr, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
                    {
                        uint TileHash = reply->rawHeader("TileHash").toUInt();
                        uint DoodadHash = reply->rawHeader("DoodadHash").toUInt();
                        this->SendTileHash(TileHash, DoodadHash); // <<<<<<<<<< SendTileHash
                        reply->deleteLater();
                        mgr->deleteLater(); });
            break;
        }
        case 0x13:
        {
            quint16 size = this->read<quint16>();
            this->read<quint16>();
            this->read(size * 2);
            this->read(size);
            break;
        }
        case 0x19:
        {
            // 游戏服务器错误
            quint16 BackendError = this->read<quint16>(); // error Id
            this->readString();                           // ??
            emit signal_BackendError(BackendError);
        }
        case 0x21a:
            // 服务器心跳
            break;
        default:
            qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                              .arg(QString::number(PacketId, 16))
                              .arg(this->readAll().toHex(' '));
            break;
        }
    }
}