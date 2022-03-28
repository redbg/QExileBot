#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaProperty>
#include "Helper.h"

class Character : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_Name)
    Q_PROPERTY(QString league MEMBER m_League)
    Q_PROPERTY(int level MEMBER m_Level)
    Q_PROPERTY(quint32 experience MEMBER m_Experience)
    Q_PROPERTY(int classId MEMBER m_ClassId)
    // 自定义字段
    Q_PROPERTY(QString classType READ GetClassType)
    Q_PROPERTY(QString className READ GetClassName)
    Q_PROPERTY(QString MetadataId READ GetMetadataId)

public:
    enum ClassType : quint8
    {
        StrDexInt, // 貴族
        Str,       // 野蠻人    特性：力量，近戰
        Dex,       // 遊俠      特性：敏捷，遠距離
        Int,       // 女巫      特性：智慧，法術
        StrDex,    // 決鬥者
        StrInt,    // 聖堂武僧
        DexInt,    // 暗影刺客
    };
    Q_ENUM(ClassType)

public:
    explicit Character(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~Character() {}

public:
    QString m_Name;       // 名字
    QString m_League;     // 赛区名
    quint8 m_LeagueId;    // 赛区Id
    quint8 m_Unknown1;    // ??
    quint8 m_Level;       // 等级
    quint32 m_Experience; // 经验
    quint8 m_ClassId;     // 职业Id
    quint16 m_Unknown2;   // ??
    quint8 m_Unknown3;    // ??
    quint8 m_Unknown4;    // ??

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

    QString GetClassType()
    {
        return GetClassTypeById(m_ClassId);
    }

    QString GetClassName()
    {
        return GetClassNameById(m_ClassId);
    }

    QString GetMetadataId()
    {
        return GetMetadataIdById(m_ClassId);
    }

    static QString GetClassTypeById(quint8 classId)
    {
        QMetaEnum MetaEnum = QMetaEnum::fromType<Character::ClassType>();
        return MetaEnum.valueToKey(classId);
    }

    static QString GetClassNameById(quint8 classId)
    {
        return Helper::Data::GetCharacter(classId).value("Name").toString();
    }

    static QString GetMetadataIdById(quint8 classId)
    {
        return Helper::Data::GetCharacter(classId).value("Id").toString();
    }
};
