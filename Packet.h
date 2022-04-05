#pragma once
#include <QObject>
#include <QList>
#include <QMetaProperty>
#include <QByteArray>
#include "PacketField.h"

class Packet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Type MEMBER m_PacketType)
    Q_PROPERTY(QString Name MEMBER m_PacketName)
    Q_PROPERTY(QString Data READ PacketData)

public:
    QString m_PacketType;
    QString m_PacketName;
    QList<PacketField *> m_Fields;

public:
    Packet(QObject *parent = nullptr) : QObject(parent) {}
    ~Packet() { qDeleteAll(m_Fields); }

public:
    QString PacketData()
    {
        QString data;

        for (size_t i = 0; i < m_Fields.size(); i++)
        {
            data.append(m_Fields.at(i)->GetData().toString());

            if (m_Fields.at(i) != m_Fields.last())
            {
                data.append(' ');
            }
        }

        return data;
    }
};
