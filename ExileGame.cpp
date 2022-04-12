#include "ExileGame.h"

void ExileGame::connectToGameServer(quint32 Ticket1, quint32 WorldAreaId, quint32 Ticket2, quint16 Port, quint32 Address, QByteArray Key)
{
    m_Ticket1 = Ticket1;
    m_Scene.m_WorldAreaId = WorldAreaId;
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
        quint16 PacketId = this->readId();
        // qDebug() << PacketId;

        switch (PacketId)
        {
        case 0x5:
        {
            // 开启加密
            this->EnableCrypto();
            break;
        }
        case 0xa:
        {
            readString("name");
            readString("text");
            readString();

            read<quint16>();
            read<quint8>();
            read<quint8>();

            quint8 size = read<quint8>("itemSize");

            for (size_t i = 0; i < size; i++)
            {
                read<quint32>("itemIndex");
                read(read<quint16>("size"), "itemData");
            }
            break;
        }
        case 0xb:
        {
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint32>();
            {
                quint8 size1 = this->read<quint8>();
                quint8 size2 = this->read<quint8>();

                for (size_t i = 0; i < size1; i++)
                {
                    this->read<quint8>();
                    this->read<quint32>();
                }

                for (size_t i = 0; i < size2; i++)
                {
                    this->readString();
                }
            }
            break;
        }
        case 0x10:
        {
            // 收到地图信息
            m_Scene.m_WorldAreaId = this->read<quint16>("WorldAreaId");
            m_Scene.m_League = this->readString("League");
            m_Scene.m_Seed = this->read<quint32>("Seed");
            this->readAll("未知");

            QNetworkAccessManager *mgr = new QNetworkAccessManager;
            QUrl url(QString("http://127.0.0.1:6112/world?id=%1&seed=%2").arg(m_Scene.m_WorldAreaId).arg(m_Scene.m_Seed));
            mgr->get(QNetworkRequest(url));
            connect(mgr, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
                    {
                        m_Scene.m_TileHash = reply->rawHeader("TileHash").toUInt();
                        m_Scene.m_DoodadHash = reply->rawHeader("DoodadHash").toUInt();
                        m_Scene.m_TerrainWidth = reply->rawHeader("TerrainWidth").toUInt();
                        m_Scene.m_TerrainHeight = reply->rawHeader("TerrainHeight").toUInt();
                        m_Scene.m_Radar = QJsonDocument::fromJson(reply->rawHeader("Radar")).object();
                        m_Scene.m_TerrainData = reply->readAll();
                        m_Scene.Refresh();
                        this->SendTileHash(m_Scene.m_TileHash, m_Scene.m_DoodadHash); // <<<<<<<<<< SendTileHash
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
        case 0x14:
        {
            {
                quint16 size = this->read<quint16>();

                for (quint16 i = 0; i < size; i++)
                {
                    this->read<quint32>();
                    this->read<quint32>();
                    this->readString();
                    this->read<quint16>();

                    quint8 v5_1 = this->read<quint8>();
                    quint8 v5_2 = this->read<quint8>();

                    this->read<quint16>();
                    this->read<quint8>();
                    this->read<quint8>();
                    this->read<quint8>();

                    if ((v5_1 & 0x80) != 0)
                    {
                        this->read<quint64>();
                    }

                    if ((v5_1 & 0x40) != 0)
                    {
                        this->read<quint8>();
                    }

                    if ((v5_2 & 1) != 0)
                    {
                        this->read<quint32>();
                    }
                }
            }
            this->readString();
            this->read<quint16>();
            this->read<quint8>();
            break;
        }
        case 0x15:
        {
            m_Scene.SetPlayerId(this->read<quint32>("PlayerId"));
            qDebug() << "PlayerId:" << m_Scene.m_PlayerId;
            break;
        }
        case 0x19:
        {
            // 游戏服务器错误
            quint16 BackendError = this->read<quint16>(); // error Id
            this->readString();                           // ??
            emit signal_BackendError(BackendError);
            break;
        }
        case 0x24:
        {
            this->read<quint16>();
            this->read<quint16>();
            this->read<quint16>();

            this->read<quint8>();
            this->read<quint8>();

            if ((this->read<quint8>() & 2) == 0)
            {
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
                this->read(this->read<quint16>() * 2);
            }

            break;
        }
        case 0x2b:
        {
            this->read<quint16>();
            this->read<quint16>();

            if ((this->read<quint8>() & 2) == 0)
            {
                quint16 size = this->read<quint16>();
                this->read(size * 2);
            }

            break;
        }
        case 0x39:
        {
            this->read<quint32>();
            this->read(0x13);
            break;
        }
        case 0x3e:
        {
            this->read(0x1a);
            this->read<quint16>();
            this->read<quint16>();
            break;
        }
        case 0x6e:
        {
            this->readString();
            this->readString();

            quint8 v4 = this->read<quint8>();

            if (v4 != 0)
            {
                this->readString();

                this->read<quint8>();
                this->read<quint8>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint16>();
                this->read<quint8>();

                this->readString();

                this->read<quint8>();
                this->read<quint16>();
                this->read<quint8>();
                this->read<quint8>();
            }
            else
            {
                this->read<quint64>();
            }

            break;
        }
        case 0x84:
        {
            this->read<quint8>();
            quint32 inventoryId = this->read<quint32>();

            {
                this->read<quint32>();
                quint32 size = this->read<quint32>();

                for (quint32 i = 0; i < size; i++)
                {
                    this->read<quint32>();
                }

                quint32 itemSize = this->read<quint32>();

                for (quint32 i = 0; i < itemSize; i++)
                {
                    this->read<quint32>();
                    this->read<quint8>(); // x
                    this->read<quint8>(); // y

                    // item info
                    this->read(this->read<quint16>());
                }

                quint8 v35 = this->read<quint8>();
                if (v35 != 0)
                {
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                }
            }
            break;
        }
        case 0x85:
        {
            // inventory 库存
            this->read<quint8>();
            quint32 inventoryId = this->read<quint32>("id");

            qDebug() << inventoryId << Helper::Data::GetInventorie(inventoryId - 1).value("Id").toString();

            {
                // sub_15F00C0
                this->read<quint8>();
                this->read<quint8>();
                this->read<quint8>("Width");
                this->read<quint8>("Height");
                quint8 v16 = this->read<quint8>();
                if ((v16 & 0x8) != 0)
                {
                    this->read<quint64>();
                }

                if ((v16 & 0x10) != 0)
                {
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                    this->read<quint32>();
                }

                {
                    quint32 size = this->read<quint32>("itemSize");

                    for (quint32 i = 0; i < size; i++)
                    {
                        this->read<quint32>("Index");
                        this->read<quint8>("x"); // x
                        this->read<quint8>("y"); // y

                        // item info
                        this->read(this->read<quint16>());
                    }
                }
            }

            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x8e:
        {
            this->read<quint8>();
            this->readString();
            this->readString();
            break;
        }
        case 0xe2:
        {
            this->read<quint16>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x143:
        {
            this->read(0xe);
            this->read(0xe);
            this->read(0xe);

            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            quint8 result = this->read<quint8>();

            {
                this->read(this->read<quint8>() * 2);

                if ((result & 1) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
                if ((result & 2) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
                if ((result & 4) != 0)
                {
                    this->read(this->read<quint8>() * 2);
                }
            }
            break;
        }
        case 0x148:
        {
            // 释放技能
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint16 v4 = this->read<quint16>();
            if ((v4 & 0x80) != 0)
            {
                this->read<quint32>();
            }

            qint32 currentX = this->ReadVarint1("currentX");
            qint32 currentY = this->ReadVarint1("currentY");
            qint32 targetX = this->ReadVarint1("targetX");
            qint32 targetY = this->ReadVarint1("targetY");

            this->read<quint16>("Skill_Id");
            this->read<quint16>();

            m_Scene.SetPositioned(id, currentX, currentY);
            break;
        }
        case 0x149:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        case 0x150:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint32 size = this->ReadVarint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVarint();
                this->ReadVarint1();
            }

            break;
        }
        case 0x151:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            quint32 size = this->ReadVarint();

            for (quint32 i = 0; i < size; i++)
            {
                this->ReadVarint();
                this->ReadVarint1();
            }

            break;
        }
        case 0x153:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            break;
        }
        case 0x15e:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            this->read<quint16>();

            break;
        }
        case 0x184:
        {
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();
            this->read<quint32>();

            this->read<quint64>();
            this->read<quint8>();
            this->read<quint32>();
            break;
        }
        case 0x1a9:
        {
            this->read<quint8>();
            this->read<quint8>();
            this->read<quint8>();
            break;
        }
        case 0x1f5:
        {
            this->read<quint8>();
            quint8 size = this->read<quint8>();

            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint16>();
                this->read<quint8>();
            }

            break;
        }
        case 0x17c:
        {
            quint8 size = this->read<quint8>();

            for (quint8 i = 0; i < size; i++)
            {
                this->read<quint8>();
                this->read<quint32>();
                this->read<quint64>();
            }

            this->read<quint8>();
            break;
        }
        case 0x212:
        {
            // Data/ArchnemesisMods.dat
            for (int i = 0; i < (0x610 / 0x18); i++)
            {
                quint8 v15 = this->read<quint8>();
                if (v15 != 0xff)
                {
                    this->read<quint8>();
                }
            }

            break;
        }
        case 0x21a:
        {
            // 服务器心跳
            break;
        }
        case 0x21e:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint32 Hash = this->read<quint32>("Hash");                    // GameObjectHashId
            QByteArray ComponentsData = this->read(this->read<quint16>()); // Components Data

            m_Scene.addItem(new GameObject(id, Hash, ComponentsData));
            break;
        }
        case 0x21f:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();

            quint16 size = this->read<quint16>();
            QByteArray data = this->read(size);
            break;
        }
        case 0x220:
        {
            quint32 id = this->read<quint32>(); // GameObjectId
            this->read<quint32>();
            this->read<quint16>();
            break;
        }
        default:
            qWarning() << QString("[!] UnknownPacket PacketId:[0x%1] Data:[%2]")
                              .arg(QString::number(PacketId, 16))
                              .arg(this->readAll().toHex(' '));
            break;
        }
    }
}