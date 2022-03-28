#pragma once
#include <QObject>
#include <QThread>
#include <QJsonValue>
#include <QJsonObject>
#include <QMetaProperty>
#include "ExileClient.h"

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

    ExileClient *m_ExileClient;

public:
    explicit Account(QObject *parent = nullptr)
        : QThread(parent)
    {
    }

    virtual ~Account()
    {
        this->quit();
        this->wait();
    }

public:
    QJsonValue toJson();
    void fromJson(QJsonValue JsonValue);

protected:
    void run() override
    {
        m_ExileClient = new ExileClient(this);
        connect(m_ExileClient, &ExileSocket::errorOccurred, this, &QThread::quit, Qt::DirectConnection);

        m_ExileClient->connectToLoginServer(Global::hostName, Global::port, m_Email, m_Password);

        this->exec();

        delete m_ExileClient;
    }
};
