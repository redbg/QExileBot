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
    Item(QDataStream &ItemData, int inventoryId = 0, int x = 0, int y = 0);
    Item(QDataStream *ItemData);

    ~Item() {}

public:
    // 基础信息
    int m_inventoryId;
    int m_x;
    int m_y;
    QJsonObject m_BaseItemType;
    QJsonObject m_Components;

    QJsonObject m_Base;
    // Stack
    QJsonObject m_Stack;
    // quint16 m_stackSize;

    // SkillGem
    QJsonObject m_SkillGem;
    // quint32 m_Experience; // 宝石经验

    // Sockets
    QJsonObject m_Sockets;
    // quint8 m_socketNumber;         // 插槽数量
    // QList<quint8> m_sockets;       // 插槽信息
    // QList<Item *> m_socketedItems; // 插槽物品数组
    // QList<quint8> m_socketLinks;   // 插槽链接信息

    // Mods
    QJsonObject m_Mods;
    // quint8 m_level;            // 装备等级
    // bool m_identified;         // 是否鉴定
    // quint8 m_frameType;        // 装备颜色
    // QJsonArray m_implicitMods; // 固定的词缀
    // QJsonArray m_explicitMods; // 基础词缀
    // QJsonArray m_enchantMods;  // 附魔词缀(涂油-花园)
    // QJsonArray m_craftedMods;  // 手工附魔词缀(工艺台)

    // Quality
    QJsonObject m_Quality;

    // Armour
    QJsonObject m_Armour;
    quint8 m_armourQuality;
    QJsonObject m_ArmourType; // 防具基础属性

    // Weapon
    QJsonObject m_Weapon;
    QJsonObject m_WeaponType; // 武器基础属性

    // AttributeRequirements
    QJsonObject m_AttributeRequirements;

    QJsonObject m_Charges;
    QJsonObject m_HeistBlueprint;
    QJsonObject m_Map;
    QJsonObject m_HeistContract;

    QJsonObject m_HeistEquipment;
    QJsonObject m_Usable;
    QJsonObject m_Flask;
    QJsonObject m_LocalStats;
    QJsonObject m_Shield;
    QJsonObject m_Quest;

    QJsonObject toJson()
    {
        QJsonObject JsonObject;

        JsonObject.insert("Components", m_Components);
        JsonObject.insert("BaseItemType", m_BaseItemType);

        return JsonObject;
    }

public:
    //解包组件
    Q_INVOKABLE QJsonObject *Base();
    Q_INVOKABLE QJsonObject *Mods();
    Q_INVOKABLE QJsonObject *HeistEquipment() { return &m_HeistEquipment; };
    Q_INVOKABLE QJsonObject *Usable() { return &m_Usable; };
    Q_INVOKABLE QJsonObject *Flask() { return &m_Flask; };
    Q_INVOKABLE QJsonObject *Stack();
    Q_INVOKABLE QJsonObject *HeistContract();
    Q_INVOKABLE QJsonObject *Map();
    Q_INVOKABLE QJsonObject *HeistBlueprint();
    Q_INVOKABLE QJsonObject *Quality();
    Q_INVOKABLE QJsonObject *LocalStats() { return &m_LocalStats; };
    Q_INVOKABLE QJsonObject *Shield() { return &m_Shield; };
    Q_INVOKABLE QJsonObject *Armour();
    Q_INVOKABLE QJsonObject *Weapon();
    Q_INVOKABLE QJsonObject *AttributeRequirements();
    Q_INVOKABLE QJsonObject *Charges();
    Q_INVOKABLE QJsonObject *Sockets();
    Q_INVOKABLE QJsonObject *SkillGem();
    Q_INVOKABLE QJsonObject *Quest() { return &m_Quest; }

    //
    void AlternateQualityTypes();
    QJsonObject fs_ItemTypeRegister_Mods();
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
