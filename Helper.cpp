#include "Helper.h"

namespace Helper
{
    QString GetRandomString(int randomStringLength)
    {
        const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        QString randomString;

        for (int i = 0; i < randomStringLength; ++i)
        {
            qint64 randomInt = QRandomGenerator::global()->bounded(possibleCharacters.size());
            randomString.append(possibleCharacters.at(randomInt));
        }

        return randomString;
    }

    namespace File
    {
        QByteArray ReadAll(QString fileName)
        {
            QFile file(fileName);
            bool isOpen = file.open(QFile::ReadOnly);
            Q_ASSERT(isOpen != false);
            return file.readAll();
        }
    }

    namespace Data
    {
        QJsonArray GetDataArray(QString fileName)
        {
            QJsonDocument JsonDocument = QJsonDocument::fromJson(Helper::File::ReadAll(fileName));
            Q_ASSERT(JsonDocument.isArray() != false);
            return JsonDocument.array();
        }

        QJsonObject GetBackendError(quint16 index)
        {
            return GetDataArray(":/Data/BackendErrors.json").at(index).toObject();
        }

        QJsonObject GetCharacter(quint8 ClassId)
        {
            QJsonArray JsonArray = GetDataArray(":/Data/Characters.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                QJsonObject JsonObject = JsonArray.at(i).toObject();

                if (JsonObject.value("IntegerId").toInt() == ClassId)
                {
                    return JsonObject;
                }
            }

            return QJsonObject();
        }

        QJsonObject GetInventorie(quint32 InventoryId)
        {
            return GetDataArray(":/Data/Inventories.json").at(InventoryId).toObject();
        }
    };
};
