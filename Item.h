#pragma once
#include <QObject>
#include <QByteArray>
#include "Helper.h"
#include <QList>
#include <QMetaProperty>
class Item : public QObject
{
    Q_OBJECT

    QDataStream *m_DataStream;

private:
public:
    Item(QByteArray ItemData);
    Item(QDataStream *ItemData);

    ~Item()
    {
        if (m_DataStream != nullptr)
        {
            delete m_DataStream;
            m_DataStream = nullptr;
        }
    }

public:
    int m_x;
    int m_y;
    int m_inventoryId;
    //Stack
    quint16 m_stackSize;
    quint16 m_maxStackSize;
    //SkillGem
    quint32 m_Experience; //宝石经验
    //Sockets
    quint8 m_socketNumber;//插槽数量
    quint8 m_socketLinkNumber; //插槽链接数量

    quint16 m_requirements_Level; //需求等级
    quint16 m_requirements_Str;   //需求力量
    quint16 m_requirements_Dex;   //需求敏捷
    quint16 m_requirements_Int;   //需求法力
    QString m_InheritsFrom;       //继承类
    QString m_baseTypeName;
    bool m_identified;       //是否鉴定
    char colour;             //颜色
    QList<Item *> m_sockets; //插槽数组

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

public:
    //解包组件
    Q_INVOKABLE void Base();
    Q_INVOKABLE void AlternateQualityTypes();
    Q_INVOKABLE void Mods();
    Q_INVOKABLE void HeistEquipment(){};
    Q_INVOKABLE void Usable(){};
    Q_INVOKABLE void Flask(){};
    Q_INVOKABLE void Stack();
    Q_INVOKABLE void HeistContract();
    Q_INVOKABLE void Map();
    Q_INVOKABLE void HeistBlueprint();
    Q_INVOKABLE void Quality();
    Q_INVOKABLE void LocalStats(){};
    Q_INVOKABLE void Shield(){};
    Q_INVOKABLE void Weapon(){};
    Q_INVOKABLE void AttributeRequirements(){};
    Q_INVOKABLE void Charges();
    Q_INVOKABLE void Armour();
    Q_INVOKABLE void Sockets();
    Q_INVOKABLE void SkillGem();

    //
    void fs_ItemTypeRegister_Mods();
    void HeistJobs();
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
};
