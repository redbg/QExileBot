#pragma once
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>

class GameObjectScene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(quint32 Id MEMBER m_WorldAreaId)
    Q_PROPERTY(QString League MEMBER m_League)
    Q_PROPERTY(quint32 Seed MEMBER m_Seed)
    Q_PROPERTY(quint32 TileHash MEMBER m_TileHash)
    Q_PROPERTY(quint32 DoodadHash MEMBER m_DoodadHash)
    Q_PROPERTY(quint32 PlayerId MEMBER m_PlayerId)

public:
    // WorldArea
    quint32 m_WorldAreaId;
    QString m_League;
    quint32 m_Seed;
    quint32 m_TileHash;
    quint32 m_DoodadHash;

    quint32 m_TerrainWidth;
    quint32 m_TerrainHeight;
    QByteArray m_TerrainData;
    QImage m_Terrain;
    QGraphicsPixmapItem m_TerrainItem;
    QGraphicsTextItem m_TextItem;

    quint32 m_PlayerId;

public:
    GameObjectScene()
    {
        this->addItem(&m_TerrainItem);
        this->addItem(&m_TextItem);
    }
    ~GameObjectScene() {}

    void Refresh()
    {
        m_Terrain = GetTerrain();
        m_TerrainItem.setPixmap(QPixmap::fromImage(m_Terrain));
        RefreshText();
    }

    void RefreshText()
    {
        m_TextItem.setPlainText(QString(QJsonDocument(this->toJson().toObject()).toJson()));
        m_TextItem.setDefaultTextColor(Qt::white);
    }

    void SetPlayerId(quint32 PlayerId)
    {
        m_PlayerId = PlayerId;
        RefreshText();
    }

    QImage GetTerrain()
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

    QJsonValue toJson()
    {
        QJsonObject JsonObject;

        for (int i = 0; i < this->metaObject()->propertyCount(); i++)
        {
            const char *name = this->metaObject()->property(i).name();
            JsonObject.insert(name, QJsonValue::fromVariant(this->property(name)));
        }

        return JsonObject;
    }
};
