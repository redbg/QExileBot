#pragma once
#include <QGraphicsTextItem>
#include "Helper.h"
class GameObject : public QGraphicsTextItem
{
    Q_OBJECT

public:
    quint32 m_Id;
    quint32 m_Hash;
    QString m_MetadataId;
    QDataStream *m_DataStream;

    template <typename T>
    T readData()
    {
        QByteArray data(sizeof(T), 0);
        m_DataStream->readRawData(data.data(), sizeof(T));
        return *(T *)data.data();
    }

    QByteArray readData(quint64 size)
    {
        QByteArray data(size, 0);
        m_DataStream->readRawData(data.data(), size);
        return data;
    }

public:
    GameObject(quint32 Id, quint32 Hash, QByteArray ComponentsData)
        : m_Id(Id), m_Hash(Hash), m_DataStream(new QDataStream(ComponentsData))
    {
        readHead();
        readPositioned();

        m_MetadataId = Helper::Data::GetObjectType(Hash).value("Path").toString();
        qDebug() << m_Id << Hash << m_MetadataId;

        this->setPlainText(QString::number(m_Id));
        this->setDefaultTextColor(Qt::green);
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

    ~GameObject() {}

    Q_INVOKABLE void readHead();
    Q_INVOKABLE void readPositioned();
    Q_INVOKABLE void Stats();
    Q_INVOKABLE void Pathfinding(){};
    Q_INVOKABLE void WorldItem(){};
    Q_INVOKABLE void Buffs();

};
