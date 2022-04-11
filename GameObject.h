#pragma once
#include <QGraphicsTextItem>

class GameObject : public QGraphicsTextItem
{
    Q_OBJECT

public:
    quint32 m_Id;
    quint32 m_Hash;
    QByteArray m_ComponentsData;
    QString m_MetadataId;

public:
    GameObject(quint32 Id, quint32 Hash, QByteArray ComponentsData)
        : m_Id(Id), m_Hash(Hash), m_ComponentsData(ComponentsData)
    {
        QDataStream DataStream(m_ComponentsData);
        readHead(DataStream);
        readPositioned(DataStream);

        m_MetadataId = Helper::Data::GetObjectType(Hash).value("Path").toString();
        qDebug() << m_Id << Hash << m_MetadataId;

        this->setPlainText(QString::number(m_Id));
        this->setDefaultTextColor(Qt::green);
    }
    ~GameObject() {}

    void readHead(QDataStream &DataStream)
    {
        // Head
        this->readData<quint8>(DataStream);

        quint8 size = this->readData<quint8>(DataStream);

        for (size_t i = 0; i < size; i++)
        {
            this->readData<quint32>(DataStream);
            quint8 v17 = this->readData<quint8>(DataStream);
            this->readData<quint8>(DataStream);

            quint8 v21 = this->readData<quint8>(DataStream);

            switch (v21)
            {
            case 1:
            case 4:
            case 5:
                for (quint8 i = 0; i < v17; i++)
                {
                    this->readData<quint32>(DataStream);
                }
                break;
            case 3:
                for (quint8 i = 0; i < v17; i++)
                {
                    this->readData<quint32>(DataStream);
                    this->readData<quint32>(DataStream);
                }
                break;
            case 6:
                for (quint8 i = 0; i < v17; i++)
                {
                    this->readData<quint8>(DataStream);
                }
                break;
            case 7:
                for (quint8 i = 0; i < v17; i++)
                {
                    quint32 size = this->readData<quint32>(DataStream);
                    this->readData(DataStream, size);
                }
                break;
            default:
                break;
            }
        }
    }

    void readPositioned(QDataStream &DataStream)
    {
        // Positioned
        quint32 x = this->readData<quint32>(DataStream);
        quint32 y = this->readData<quint32>(DataStream);

        this->setPos(x, y);
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
};
