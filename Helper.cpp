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
            static QJsonArray JsonArray = GetDataArray(":/Data/BackendErrors.json");
            return JsonArray.at(index).toObject();
        }

        QJsonObject GetCharacter(quint8 ClassId)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/Characters.json");

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
            static QJsonArray JsonArray = GetDataArray(":/Data/Inventories.json");
            return JsonArray.at(InventoryId).toObject();
        }

        QJsonObject GetObjectType(quint32 Hash)
        {
            static QJsonArray JsonArray = GetDataArray(":/Data/ObjectTypes.json");

            for (int i = 0; i < JsonArray.size(); i++)
            {
                if (JsonArray.at(i).toObject().value("Hash").toInteger() == Hash)
                {
                    return JsonArray.at(i).toObject();
                }
            }

            return QJsonObject();
        }
    };
};
