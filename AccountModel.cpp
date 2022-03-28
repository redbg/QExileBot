#include "AccountModel.h"

int AccountModel::rowCount(const QModelIndex &parent) const
{
    return m_AccountList.size();
}

int AccountModel::columnCount(const QModelIndex &parent) const
{
    return Account::staticMetaObject.propertyCount();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        const char *name = Account::staticMetaObject.property(index.column()).name();
        return m_AccountList.at(index.row())->property(name);
    }
    case Qt::ForegroundRole:
    {
        return m_AccountList.at(index.row())->isRunning() ? QBrush(Qt::darkGreen) : QVariant();
    }
    default:
        return QVariant();
    }
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        return Account::staticMetaObject.property(section).name();
    }
    return QVariant();
}

Qt::ItemFlags AccountModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool AccountModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        const char *name = Account::staticMetaObject.property(index.column()).name();
        m_AccountList.at(index.row())->setProperty(name, value);
        return true;
    }
    default:
        return false;
    }
}

bool AccountModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        m_AccountList.insert(row, new Account(this));
    }

    endInsertRows();
    return true;
}

bool AccountModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);

    qDeleteAll(m_AccountList.begin() + row, m_AccountList.begin() + row + count); // delete
    m_AccountList.remove(row, count);                                             // remove

    endRemoveRows();
    return true;
}

QJsonValue AccountModel::toJson()
{
    QJsonArray JsonArray;

    for (int i = 0; i < m_AccountList.size(); i++)
    {
        JsonArray.append(m_AccountList.at(i)->toJson());
    }

    return JsonArray;
}

void AccountModel::fromJson(QJsonValue JsonValue)
{
    if (JsonValue.isArray())
    {
        QJsonArray JsonArray = JsonValue.toArray();

        for (int i = 0; i < JsonArray.size(); i++)
        {
            Account *account = new Account(this);
            account->fromJson(JsonArray.at(i));
            m_AccountList.append(account);
        }
    }
}

void AccountModel::start(const QModelIndex &index)
{
    m_AccountList.at(index.row())->start();
}

void AccountModel::quit(const QModelIndex &index)
{
    m_AccountList.at(index.row())->quit();
}

Account *AccountModel::at(int row)
{
    return m_AccountList.at(row);
}