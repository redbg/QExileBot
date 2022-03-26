#include "Account.h"

QJsonValue Account::toJson()
{
    QJsonObject JsonObject;

    for (int i = 0; i < this->metaObject()->propertyCount(); i++)
    {
        const char *key = this->metaObject()->property(i).name();
        QJsonValue value = QJsonValue::fromVariant(this->property(key));
        JsonObject.insert(key, value);
    }

    return JsonObject;
}

void Account::fromJson(QJsonValue JsonValue)
{
    if (JsonValue.isObject())
    {
        QJsonObject JsonObject = JsonValue.toObject();

        for (int i = 0; i < this->metaObject()->propertyCount(); i++)
        {
            const char *key = this->metaObject()->property(i).name();
            QJsonValue value = JsonObject.value(key);
            this->setProperty(key, value);
        }
    }
}