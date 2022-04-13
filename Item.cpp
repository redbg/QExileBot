#include "Item.h"
#define HIBYTE(v) (v >> 8) & 0xff

Item::Item(QByteArray ItemData)
    : m_DataStream(new QDataStream(ItemData))
{
    qDebug() << ItemData.toHex(' ');
    Item::Item(m_DataStream);
}

Item::Item(QDataStream *ItemData)
    : m_DataStream(ItemData)
{
    quint32 Hash = readData<quint32>();
    QJsonArray array = Helper::Data::GetItemComponent(Helper::Data::GetBaseItemType(Hash)["InheritsFrom"].toString());
    for (auto v : array)
    {
        qDebug() << "------------:" + v.toString();
        this->metaObject()->invokeMethod(this, v.toString().toLatin1().data());
    }
}

Q_INVOKABLE void Item::Base()
{
    readData<quint16>();
    readData<quint8>();
    readData<quint8>();
    quint16 v22 = readData<quint16>();
    readData<quint8>();
    if ((v22 & 1) != 0)
    {
        readData<quint8>();
    }
    quint16 v23 = HIBYTE(v22);
    if ((v23 & 0x10) != 0)
    {
        readData<quint32>();
        readData<quint32>();
    }

    if ((v22 & 4) != 0)
    {
        quint32 size = readData<quint32>();
        if (size > 0)
        {
            readData(2 * size);
        }
    }
    else if ((v22 & 2) != 0)
    {
        readData<quint16>();
    }

    if ((v22 & 8) != 0)
    {
        readData<quint32>();
    }

    if ((v22 & 0x10) != 0)
    {
        readData(readData<quint32>() * 2);
    }
    if ((v22 & 0x20) != 0)
    {
        quint8 v57 = readData<quint8>();

        for (int i = 0; i < v57; i++)
        {
            readData<quint32>();
            readData<quint32>();
        }
    }

    if ((v22 & 0x40) != 0)
    {
        Item(this->m_DataStream);
        //self:ItemTypeRegister(buf, result)
    }
    if ((v22 & 0x80) != 0)
    {
        quint8 size = readData<quint8>();
        for (int i = 0; i < size; i++)
        {
            readData<quint32>();
            //fs_componentRead(buf, result)
            Helper::DataHelper::ReadVarint(*(this->m_DataStream));
        }
    }
}

Q_INVOKABLE void Item::AlternateQualityTypes()
{
    quint16 size;
    quint16 v8 = readData<quint16>();//是否鉴定,颜色
    readData<quint8>();
    if ((v8 & 0x200) != 0)
    {
        // 类型id
        readData<quint8>();
    }
    if ((v8 & 0x10) != 0)
    {
        size = readData<quint8>();

        for (int i = 0; i < size; i++)
        {
            fs_ItemTypeRegister_Mods();
        }
    }

    if ((v8 & 0x40) != 0)
    {
        size = readData<quint8>();
        for (int i = 0; i < size; i++)
        {
            fs_ItemTypeRegister_Mods();
        }
    }

    if ((v8 & 0x400) != 0)
    {
        size = readData<quint8>();
        for (int i = 0; i < size; i++)
        {
            fs_ItemTypeRegister_Mods();
        }
    }
    bool v22 = (v8 & 8) != 0;
    if (v22)
    {
        if ((v8 & 0x80) == 0)
        {
            if ((v8 & 0x100) != 0)
            {
                size = readData<quint8>();
                for (int i = 0; i < size; i++)
                {
                    readData<quint32>();
                }
            }
        }
        else
        {
            readData(readData<quint32>() * 2);
        }
        if ((v8 & 0x20) != 0)
        {
            size = readData<quint8>();

            for (int i = 0; i < size; i++)
            {
                fs_ItemTypeRegister_Mods();
            }
        }
    }
}

void Item::fs_ItemTypeRegister_Mods()
{
    quint16 HASH16 = readData<quint16>();
    QJsonObject mod = Helper::Data::GetMods(HASH16);

    quint16 count = 0;
    if (mod["StatsKey1"].isNull())
    {
        count += 1;
    }
    if (mod["StatsKey2"].isNull())
    {
        count += 1;
    }
    if (mod["StatsKey3"].isNull())
    {
        count += 1;
    }
    if (mod["StatsKey4"].isNull())
    {
        count += 1;
    }
    if (mod["StatsKey5"].isNull())
    {
        count += 1;
    }
    if (mod["StatsKey6"].isNull())
    {
        count += 1;
    }
    if (mod["Heist_StatsKey0"].isNull())
    {
        count += 1;
    }
    if (mod["Heist_StatsKey1"].isNull())
    {
        count += 1;
    }
    for (size_t i = 0; i < count; i++)
    {
        Helper::DataHelper::ReadVarint1(*(this->m_DataStream));
    }
}

Q_INVOKABLE void Item::Mods()
{
    readData<quint8>();
    readData<quint8>();
    quint8 v15 = readData<quint8>();
    if ((v15 & 0x20) != 0)
    {
        readData<quint16>();
        readData<quint16>();
        readData<quint8>();
    }
    AlternateQualityTypes();
}

Q_INVOKABLE void Item::Stack()
{
    m_stackSize = readData<quint16>();
}

Q_INVOKABLE void Item::HeistContract()
{
    readData<quint32>();
    readData<quint8>();
    readData<quint8>();
    quint8 v14 = readData<quint8>();
    quint8 v68 = readData<quint8>();

    for (size_t i = 0; i < v14; i++)
    {
        readData<quint8>();
        readData<quint8>();
    }

    for (size_t i = 0; i < v68; i++)
    {
        readData<quint8>();
    }
}

Q_INVOKABLE void Item::Map()
{
    readData<quint8>();
    readData<quint8>();
}

Q_INVOKABLE void Item::HeistBlueprint()
{
    readData<quint32>();
    readData<quint8>();
    readData<quint8>();
    quint8 size = readData<quint8>();
    for (size_t i = 0; i < size; i++)
    {
        HeistJobs();
    }
}
void Item::HeistJobs()
{
    quint8 size = readData<quint8>();
    for (size_t i = 0; i < size >> 1; i++)
    {
        readData<quint8>();
        readData<quint8>();
    }

    size = readData<quint8>();
    for (size_t i = 0; i < size; i++)
    {
        readData<quint8>();
    }

    size = readData<quint8>();
    for (size_t i = 0; i < size; i++)
    {
        readData<quint8>();
    }
}

Q_INVOKABLE void Item::Quality()
{
    readData<quint8>();
}

Q_INVOKABLE void Item::Charges()
{
    Helper::DataHelper::ReadVarint1(*(this->m_DataStream));
}

Q_INVOKABLE void Item::Armour()
{
    readData<quint8>();
}

Q_INVOKABLE void Item::Sockets()
{
    quint8 v31 = readData<quint8>();
    if ((v31 & 1) == 0 && (v31 & 2) == 0)
    {
        quint8 a26 = (v31 >> 2) & 7;
        quint8 v35 = v31 >> 5;
        if (a26 > 0)
        {
            //result.doc = result.doc .. "插槽数量：" .. a26 .. "\n"
            for (size_t i = 0; i < a26; i++)
            {

                quint8 v40 = readData<quint8>();
                if ((v40 & 1) != 0)
                {
                    m_sockets.push_back(new Item(m_DataStream));
                }
            }
        }
        else
        {
        }

        for (size_t i = 0; i < v35; i++)
        {
            readData<quint8>();
        }
    }
}

Q_INVOKABLE void Item::SkillGem()
{
    m_Experience =  readData<quint32>();
    readData<quint32>();
    readData<quint8>();
}