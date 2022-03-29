#pragma once
#include "Character.h"
#include <QAbstractTableModel>
#include <QBrush>
#include <QList>
#include <QMetaProperty>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

class CharacterModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    QList<Character *> m_CharacterList;
    quint32 m_LastSelectIndex;
    quint8 m_Unknown1;

public:
    explicit CharacterModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}
    virtual ~CharacterModel() {}

public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return m_CharacterList.size();
    }
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return Character::staticMetaObject.propertyCount();
    }
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            const char *name = Character::staticMetaObject.property(index.column()).name();
            return m_CharacterList.at(index.row())->property(name);
        }
        case Qt::ForegroundRole:
        {
            return index.row() == m_LastSelectIndex ? QBrush(Qt::darkGreen) : QVariant();
        }
        default:
            return QVariant();
        }
    }
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            return Character::staticMetaObject.property(section).name();
        }
        return QVariant();
    }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    // virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
    // {
    //     switch (role)
    //     {
    //     case Qt::EditRole:
    //     {
    //         const char *name = Character::staticMetaObject.property(index.column()).name();
    //         m_CharacterList.at(index.row())->setProperty(name, value);
    //         return true;
    //     }
    //     default:
    //         return false;
    //     }
    // }

    void append(Character *character)
    {
        beginInsertRows(QModelIndex(), m_CharacterList.size(), m_CharacterList.size());
        m_CharacterList.append(character);
        endInsertRows();
    }

    void clear()
    {
        m_CharacterList.clear();
    }

public:
    QJsonValue toJson()
    {
        QJsonArray JsonArray;

        for (int i = 0; i < m_CharacterList.size(); i++)
        {
            JsonArray.append(m_CharacterList.at(i)->toJson());
        }

        return JsonArray;
    }

    Character *currentCharacter()
    {
        return m_CharacterList.at(m_LastSelectIndex);
    }
};
