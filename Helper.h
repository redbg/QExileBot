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
        QJsonObject GetBackendError(quint16 index);
        QJsonObject GetCharacter(quint8 ClassId);
        QJsonObject GetInventorie(quint32 InventoryId);
        QJsonObject GetObjectType(quint32 Hash);
    };
};
