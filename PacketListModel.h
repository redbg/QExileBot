#pragma once
#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include <QMetaProperty>
#include "Packet.h"

class PacketListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    QList<Packet *> m_PacketList;

public:
    PacketListModel(QObject *parent = nullptr) : QAbstractItemModel(parent) {}
    ~PacketListModel() { qDeleteAll(m_PacketList); }

public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        if (parent.isValid())
        {
            return createIndex(row, column, m_PacketList.at(parent.row())->m_Fields.at(row));
        }

        return createIndex(row, column, m_PacketList.at(row));
    }

    virtual QModelIndex parent(const QModelIndex &child) const override
    {
        if (child.isValid())
        {
            QObject *item = (QObject *)child.internalPointer();
            if (m_PacketList.indexOf(item->parent()) != -1)
            {
                return createIndex(m_PacketList.indexOf(item->parent()), 0, item->parent());
            }
        }

        return QModelIndex();
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        if (parent.isValid())
        {
            return m_PacketList.at(parent.row())->m_Fields.size();
        }

        return m_PacketList.size();
    }

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return Packet::staticMetaObject.propertyCount();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            QObject *item = (QObject *)index.internalPointer();
            const char *name = Packet::staticMetaObject.property(index.column()).name();
            return item->property(name);
        }
        default:
            return QVariant();
        }
    }

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            return Packet::staticMetaObject.property(section).name();
        }
        return QVariant();
    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    virtual bool hasChildren(const QModelIndex &parent) const override
    {
        if (m_PacketList.indexOf(parent.internalPointer()) == -1 && parent.isValid())
        {
            return false;
        }

        return true;
    }
};
