#pragma once
#include <QRandomGenerator>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Helper
{
    QString GetRandomString(int randomStringLength);

    namespace File
    {
        QByteArray ReadAll(QString fileName);
    }

    namespace Data
    {
        QJsonArray GetDataArray(QString fileName);
        QJsonObject GetJsonObj(QString fileName);
        QJsonObject GetBackendError(quint16 index);
        QJsonObject GetCharacter(quint8 ClassId);
        QJsonObject GetInventorie(quint32 InventoryId);
        QJsonObject GetObjectType(quint32 Hash);
        QJsonObject GetBaseItemType(qint32 Hash);
        QJsonArray GetItemComponent(QString InheritsFrom);
        QJsonObject GetMods(quint16 HASH16);
        QJsonObject GetStat(quint16 _rid);
        QJsonObject GetComponentAttributeRequirement(QString itemTypeId);
        QJsonObject GetArmourType(quint64 BaseItemTypesKey);
        QJsonObject GetWeaponType(quint64 BaseItemTypesKey);
        QJsonObject GetBuffDefinitions(quint16 id);


    };

    namespace DataHelper
    {
        template <typename T>
        T readData(QDataStream &DataStream)
        {
            QByteArray data(sizeof(T), 0);
            DataStream.readRawData(data.data(), sizeof(T));
            return *(T *)data.data();
        }

        static QByteArray readData(QDataStream &DataStream, quint64 size)
        {
            QByteArray data(size, 0);
            DataStream.readRawData(data.data(), size);
            return data;
        }

        // 无符号
        static quint32 ReadVarint(QDataStream &data)
        {
            quint8 v = readData<quint8>(data);

            if ((v & 0x80) == 0)
            {
                return v;
            }

            if ((v & 0xC0) == 0x80)
            {
                return readData<quint8>(data) | (v & 0x3f) << 8;
            }
            else if ((v & 0xE0) == 0xC0)
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
            }
            else if ((v & 0xF0) == 0xE0)
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                quint8 v3 = readData<quint8>(data);

                return v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
            }
            else
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                quint8 v3 = readData<quint8>(data);
                quint8 v4 = readData<quint8>(data);
                return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
            }
        }

        // 有符号
        static qint32 ReadVarint1(QDataStream &data)
        {
            quint8 v = readData<quint8>(data);

            if ((v & 0x80) == 0)
            {
                if ((v & 0x40) == 0)
                {
                    return v;
                }
                else
                {
                    return v | 0xffffff80;
                }
            }

            if ((v & 0xC0) == 0x80)
            {
                return readData<quint8>(data) | (v & 0x3f) << 8;
            }
            else if ((v & 0xE0) == 0xC0)
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                return v2 | v1 << 8 | (v & 0x1f) << (8 * 2);
            }
            else if ((v & 0xF0) == 0xE0)
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                quint8 v3 = readData<quint8>(data);

                qint64 result = v3 | v2 << 8 | v1 << (8 * 2) | (v & 0xf) << (8 * 3);
                result = v & 8 ? result : result | 0xF0000000;
                return result;
            }
            else
            {
                quint8 v1 = readData<quint8>(data);
                quint8 v2 = readData<quint8>(data);
                quint8 v3 = readData<quint8>(data);
                quint8 v4 = readData<quint8>(data);
                return v4 | v3 << 8 | v2 << (8 * 2) | v1 << (8 * 3);
            }
        }

    }
};
