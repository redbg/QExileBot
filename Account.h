#pragma once
#include <QObject>
#include <QThread>
#include <QJsonValue>
#include <QJsonObject>
#include <QMetaProperty>

class Account : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString Email MEMBER m_Email)
    Q_PROPERTY(QString Password MEMBER m_Password)
    Q_PROPERTY(QString AccountName MEMBER m_AccountName)
    Q_PROPERTY(QString POESESSID MEMBER m_POESESSID)

public:
    QString m_Email;
    QString m_Password;
    QString m_AccountName;
    QString m_POESESSID;

public:
    explicit Account(QObject *parent = nullptr) : QThread(parent) {}
    virtual ~Account() {}

public:
    QJsonValue toJson();
    void fromJson(QJsonValue JsonValue);
};
