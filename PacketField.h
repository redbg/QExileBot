#pragma once
#include <QObject>
#include <QMetaProperty>

class PacketField : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Type READ GetType)
    Q_PROPERTY(QString Name MEMBER m_Name)
    Q_PROPERTY(QVariant Data READ GetData)

public:
    int m_Type;
    QString m_Name;
    QByteArray m_Data;

public:
    PacketField(QString name, QByteArray data, int type, QObject *parent = nullptr)
        : m_Name(name),
          m_Data(data),
          m_Type(type),
          QObject(parent)
    {
    }
    ~PacketField() {}

public:
    QString GetType()
    {
        return QMetaType(m_Type).name();
    }

    QVariant GetData()
    {
        if (m_Type == QMetaType::QString)
        {
            return QString::fromUtf16((char16_t *)m_Data.data(), m_Data.size() / 2);
        }

        return m_Data.toHex();
    }
};
