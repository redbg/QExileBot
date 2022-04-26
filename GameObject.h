#pragma once
#include "Helper.h"
#include <Buffer.h>
#include <QGraphicsTextItem>
#include <QJsonArray>
#include <QList>
#include <QMap>

class GameObject
    : public QGraphicsTextItem,
      Buffer
{
    Q_OBJECT

public:
    quint32 m_Id;
    quint32 m_Hash;
    QString m_MetadataId;

    QString m_typeName;
    QString m_name;
    static QJsonArray m_ComponentArray;
    //
    quint32 m_hp;

public:
    GameObject(quint32 Id,
               quint32 Hash,
               QByteArray ComponentsData,
               QJsonObject componentArray)
        : m_Id(Id), m_Hash(Hash)
    {
        qDebug() << ComponentsData.toHex(' ');
        this->readData<quint8>();
        readHead();

        quint8 v17 = readData<quint8>();
        for (quint8 i = 0; i < v17; i++)
        {
            readData<quint16>();
        }

        for (auto i = componentArray.begin(); i != componentArray.end(); i++)
        {
            m_typeName = i.key();
            for (auto v : i.value().toArray())
            {
                this->metaObject()->invokeMethod(
                    this, v.toString().toLatin1().data(), Qt::DirectConnection);
            }
        }

        this->setPlainText(QString::number(m_Id));
        this->setDefaultTextColor(Qt::green);
    }

    ~GameObject() {}

    //解析数据段虚函数
    Q_INVOKABLE void readHead();
    Q_INVOKABLE void Positioned();
    Q_INVOKABLE void Stats();
    Q_INVOKABLE void Pathfinding(){};
    Q_INVOKABLE void WorldItem(){};
    Q_INVOKABLE void Buffs();
    Q_INVOKABLE void Life();
    Q_INVOKABLE void Animated();
    Q_INVOKABLE void Player();
    Q_INVOKABLE void AreaTransition();
    Q_INVOKABLE void PlayerClass(){/*空*/};
    Q_INVOKABLE void Inventories();
    Q_INVOKABLE void Actor();
    Q_INVOKABLE void LimitedLifespan(){/*空*/};
    Q_INVOKABLE void Render(){/*空*/};
    Q_INVOKABLE void ObjectMagicProperties();
    Q_INVOKABLE void BaseEvents(){};

    //
    bool fs_componentPlayerUnknown(unsigned char *buffer, int len, unsigned __int64 a2);
    void fs_ActorA0();
    void fs_ActiveSkills();
    void fs_ActiveSkills1(quint8 size);
    void fs_ActiveSkills3();
    void fs_ActiveSkills_0();
    void fs_GrantedEffectsPerLevel();
    void fs_AlternateQualityTypes();
    void fs_Data_Mods();
};
