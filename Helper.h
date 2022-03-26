#include <QRandomGenerator>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Helper
{
    static QString GetRandomString(int randomStringLength)
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
        static QByteArray ReadAll(QString fileName)
        {
            QFile file(fileName);
            Q_ASSERT(file.open(QFile::ReadOnly) != false);
            return file.readAll();
        }
    }

    namespace Data
    {
        static QJsonArray GetDataArray(QString fileName)
        {
            QJsonDocument JsonDocument = QJsonDocument::fromJson(Helper::File::ReadAll(fileName));
            Q_ASSERT(JsonDocument.isArray() != false);
            return JsonDocument.array();
        }

        static QJsonObject GetBackendError(quint16 index)
        {
            return GetDataArray(":/Data/BackendErrors.json").at(index).toObject();
        }

        static QJsonObject GetCharacter(quint8 ClassId)
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
    };
};
