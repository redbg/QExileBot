#include "GameObject.h"
 QJsonArray m_ComponentArray;
void GameObject::readHead()
{
    // Head

    quint8 size = this->readData<quint8>();

    for (size_t i = 0; i < size; i++)
    {
        this->readData<quint32>();
        quint8 v17 = this->readData<quint8>();
        this->readData<quint8>();

        quint8 v21 = this->readData<quint8>();

        switch (v21)
        {
        case 1:
        case 4:
        case 5:
            for (quint8 i = 0; i < v17; i++)
            {
                this->readData<quint32>();
            }
            break;
        case 3:
            for (quint8 i = 0; i < v17; i++)
            {
                this->readData<quint32>();
                this->readData<quint32>();
            }
            break;
        case 6:
            for (quint8 i = 0; i < v17; i++)
            {
                this->readData<quint8>();
            }
            break;
        case 7:
            for (quint8 i = 0; i < v17; i++)
            {
                quint32 size = this->readData<quint32>();
                this->readData(size);
            }
            break;
        default:
            break;
        }
    }
}

void GameObject::Positioned()
{
    // Positioned
    quint32 x = this->readData<quint32>();
    quint32 y = this->readData<quint32>();
    readData<quint32>();
    readData<quint8>();
    quint16 v16 = readData<quint16>();
    quint16 v18 = v16 >> 8;
    quint16 v84 = v16 >> 8;
    if ((v16 & 0x20) != 0)
    {
        readData<quint32>();
        readData<quint32>();
        readData<quint32>();
    }

    if ((v18 & 0x8) != 0)
    {
        readData<quint32>();
    }

    if ((v16 & 0x4) != 0)
    {
        readData<quint32>();
        readData<quint32>();
    }

    if ((v16 & 0x40) != 0)
    {
        readData<quint32>();
        readData<quint32>();
    }

    //::LABEL_33::
    if ((v84 & 0x1) != 0)
    {
        readData<quint32>();
        readData<quint32>();
        readData<quint8>();
        readData<quint32>();
        readData<quint8>();
    }

    if ((v84 & 0x2) != 0)
    {
        readData<quint32>();
    }

    if ((v84 & 0x4) != 0)
    {
        readData<quint8>();
        readData<quint8>();
    }
    if ((v84 & 0x10) != 0)
    {
        readData<quint8>();
    }
    if ((v84 & 0x20) != 0)
    {
        readData<quint32>();
    }

    this->setPos(x, y);
}

void GameObject::Stats()
{
    quint32 v5 = Helper::DataHelper::ReadVarint(*m_DataStream);
    for (quint32 i = 0; i < v5; i++)
    {
        Helper::DataHelper::ReadVarint(*m_DataStream);
        Helper::DataHelper::ReadVarint1(*m_DataStream);
    }
    readData<quint32>();
    readData<quint8>();
    readData<quint8>();
}

Q_INVOKABLE void GameObject::Buffs()
{
    quint16 size = readData<quint16>();
    for (quint16 i = 0; i < size; i++)
    {
        readData<quint16>();
        readData<quint8>();
        { //fs_BuffDefinitions
            quint16 id = readData<quint16>();

            readData<quint16>();
            readData<quint32>();
            readData<quint32>();
            readData<quint16>();
            readData<quint32>();
            readData<quint16>();
            readData<quint16>();
            readData<quint16>();
            readData<quint32>();
            readData<quint8>();

            QJsonObject buffer = Helper::Data::GetBuffDefinitions(id);
            if (!buffer.isEmpty())
            {
                if (buffer.value("Unknown43").toInteger() & 16777216 == 16777216)
                {
                    readData<quint16>();
                }

                if (buffer.value("IsRecovery").toBool())
                {
                    for (size_t i = 0; i < buffer.value("Unknown41").toArray().size(); i++)
                    {
                        readData<quint32>();
                    }
                }
            }

            quint32 size = readData<quint32>();
            for (quint32 i = 0; i < size; i++)
            {
                readData<quint32>();
            }
        }
    }
}

void GameObject::Life()
{
    m_hp = readData<quint32>();
    readData<quint32>();
    readData<quint16>();
    readData<quint32>();

    readData<quint32>();

    readData<quint32>();
    readData<quint32>();
    readData<qint16>();
    readData<quint32>();

    readData<quint32>();
    readData<quint32>();
    readData<quint16>();
    readData<quint32>();

    readData<quint32>();

    readData<quint8>();
}

void GameObject::Animated()
{
    quint8 v8 = readData<quint8>();
    if ((v8 & 1) != 0)
    {
        if ((v8 & 2) != 0)
        {
            readData<quint32>();
        }
        else
        {

            quint32 size = readData<quint32>();
            if (size * 2 > m_DataStreamSize)
            {
                return;
            }
            readData(2 * size);
        }
    }
    if ((v8 & 4) != 0)
    {
        readData<quint32>();
        readData(readData<quint32>() * 2);
        readData<quint8>();
        readData<quint8>();
        readData<quint32>();
        readData<quint32>();
        readData<quint32>();
    }

    if ((v8 & 8) != 0)
    {
        readData<quint32>();
        readData<quint32>();
        readData<quint32>();
        if ((v8 & 0x20) != 0)
        {
            readData<quint32>();
            readData<quint32>();
        }
    }

    if ((v8 & 0x10) != 0)
    {

        readData<quint32>();
    }
}

void GameObject::Player()
{
  QByteArray nameBit = readData(readData<quint32>() * 2);
  m_name =
    QString::fromUtf16((const char16_t*)nameBit.data(), nameBit.size() / 2);
  qDebug() << m_name;
  readData<quint8>();
  readData<quint32>();
  readData<quint32>();
  readData<quint16>();
  readData<quint8>();

  readData(readData<quint8>() * 9);
  QByteArray data = readData(readData<quint8>() * 9);
  readData<quint32>();
  readData<quint16>();
  readData<quint16>();

  readData<quint64>();
  readData<quint64>();

  if (!data.isEmpty() && fs_componentPlayerUnknown(
                           (unsigned char*)data.data(), data.size(), 0x409)) {
    readData<quint64>();
    readData<quint64>();
  }

  readData<quint8>();
  readData<quint8>();
  readData<quint8>();
  readData<quint8>();
  readData<quint8>();
}

bool __fastcall fs_componentPlayerUnknown1(unsigned __int8 *a1, unsigned __int8 a2)
{

    return *a1 < a2;
}
bool GameObject::fs_componentPlayerUnknown(unsigned char *buffer, int len, unsigned __int64 a2)
{
    unsigned char *end;   // r8
    unsigned char *begin; // rsi
    char v4;              // r14
    unsigned __int64 v5;  // r15
    unsigned __int64 v7;  // rbx

    end = buffer + len;
    begin = buffer;
    v4 = a2;
    v5 = a2 >> 6;
    v7 = (end - buffer) / 9;
    if ((__int64)v7 > 0)
    {
        do
        {
            if (fs_componentPlayerUnknown1(&begin[8 * (v7 >> 1) + (v7 >> 1)], v5))
            {
                begin += 8 * (v7 >> 1) + (v7 >> 1) + 9;
                v7 += 0xFFFFFFFFFFFFFFFFui64 - (v7 >> 1);
            }
            else
            {
                v7 >>= 1;
            }
        } while ((__int64)v7 > 0);
        end = buffer + len;
    }
    return begin != end && *begin == (unsigned char)v5 && ((unsigned __int8)(1 << (v4 & 7)) & begin[((unsigned __int64)(v4 & 0x3F) >> 3) + 1]) != 0;
}

void GameObject::AreaTransition()
{
    readData<quint8>();
    readData<quint16>();
}

void GameObject::Inventories()
{
    quint8 size = readData<quint8>();
    for (quint8 i = 0; i < size; i++)
    {
        //fs_ItemVisualIdentity
        {
            readData<quint8>(); //位置id
            quint8 v10 = readData<quint8>();
            if (v10 > 0)
            {
                readData<quint16>(); //ItemVisualIdentity::Unknown5
                readData<quint16>();
                readData<quint16>();
                readData<quint16>();
                readData<quint8>(); //ItemStances_Id
                readData<quint8>();
            }
        }
    }
}

void GameObject::Actor()
{
    readData(readData<quint32>() * 2);
    readData<quint16>();
    readData<quint8>();
    if (m_hp <= 0)
    {
        readData<quint8>();
        readData<quint8>();
    }

    readData<quint32>();
    readData<quint32>();
    fs_ActorA0();
}

void GameObject::fs_ActorA0()
{
    quint16 v6 = readData<quint16>();
    if ((v6 & 0x40) != 0)
    {

        fs_ActiveSkills();
    }
    else
    {
        if ((v6 & 0x20) != 0)
        {
            fs_ActiveSkills_0();
        }
    }
}

//主动技能相关
void GameObject::fs_ActiveSkills()
{
    quint8 size = readData<quint8>();
    fs_ActiveSkills1(size);
    size = readData<quint8>(); //数量
    for (quint8 i = 0; i < size; i++)
    {
        readData<quint32>();
        readData<quint32>();
        readData<quint32>();
    }
    size = readData<quint8>(); //数量
    for (quint8 i = 0; i < size; i++)
    {
        readData<quint16>();
        quint32 size2 = readData<quint32>();
        for (quint32 i = 0; i < size2; i++)
        {
            readData<quint32>();
            readData<quint32>();
            readData<quint32>();
            readData<quint32>();
        }
    }
}

void GameObject::fs_ActiveSkills1(quint8 size)
{
    for (quint8 i = 0; i < size; i++)
    {
        fs_ActiveSkills3();
    }
}

void GameObject::fs_ActiveSkills3()
{
  readData<quint16>(); //技能id
  fs_GrantedEffectsPerLevel();
  quint8 v89 = readData<quint8>();
  if ((v89 & 1) != 0) {
    readData<quint32>();
    readData<quint16>();
  }
  quint8 v12 = readData<quint8>();

  for (quint8 i = 0; i < v12; i++) {
    fs_GrantedEffectsPerLevel();
    readData<quint32>();
    readData<quint32>();
  }

  quint32 size1 = Helper::DataHelper::ReadVarint(*m_DataStream);
  for (quint32 i = 0; i < 0; i++) {
    Helper::DataHelper::ReadVarint(*m_DataStream);
    Helper::DataHelper::ReadVarint1(*m_DataStream);
  }
}

void GameObject::fs_ActiveSkills_0()
{
}

void GameObject::fs_GrantedEffectsPerLevel()
{
    quint8 v8 = readData<quint8>();
    if (v8 > 0)
    {
        readData<quint8>();
    }

    readData<quint32>(); //GrantedEffectsPerLevelId "GrantedEffectsPerLevel.GrantedEffectsKey -> GrantedEffects.ActiveSkill -> ActiveSkills"
}

void GameObject::ObjectMagicProperties()
{
    fs_AlternateQualityTypes();
    quint8 v7 = readData<quint8>();
    if ((v7 & 2) != 0)
    {
        readData<quint16>();
    }
}

void GameObject::fs_AlternateQualityTypes()
{
    quint32 size;
    quint16 v8 = readData<quint16>();
    readData<quint8>();
    bool v78 = (v8 & 8) != 0;

    if ((v8 & 0x200) != 0)
    {
        readData<quint8>();
        readData<quint8>();
    }

    if ((v8 & 0x10) != 0)
    {
        size = readData<quint8>();
        for (quint32 i = 0; i < size; i++)
        {
            fs_Data_Mods();
        }
    }

    if ((v8 & 0x40) != 0)
    {
        size = readData<quint8>();
        for (quint32 i = 0; i < size; i++)
        {
            fs_Data_Mods();
        }
    }

    if ((v8 & 0x400) != 0)
    {
        size = readData<quint8>();
        for (quint32 i = 0; i < size; i++)
        {
            fs_Data_Mods();
        }
    }

    if (v78)
    {
        if ((v8 & 0x80) == 0)
        {
            if ((v8 & 0x100) != 0)
            {
                size = readData<quint8>();
                for (quint8 i = 0; i < size; i++)
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
            readData<quint8>();
        }
    }
}

void GameObject::fs_Data_Mods()
{
    quint16 HASH16 = readData<quint16>();
    QJsonObject mod = Helper::Data::GetMods(HASH16);
    quint32 size = 0;
    if (mod.value("StatsKey1").toBool())
    {
        size += 1;
    }
    if (mod.value("StatsKey2").toBool())
    {
        size += 1;
    }
    if (mod.value("StatsKey3").toBool())
    {
        size += 1;
    }
    if (mod.value("StatsKey4").toBool())
    {
        size += 1;
    }
    if (mod.value("StatsKey5").toBool())
    {
        size += 1;
    }
    if (mod.value("StatsKey6").toBool())
    {
        size += 1;
    }

    if (mod.value("Heist_StatsKey0").toBool())
    {
        size += 1;
    }
    if (mod.value("Heist_StatsKey1").toBool())
    {
        size += 1;
    }

    for (quint32 i = 0; i < size; i++)
    {
        Helper::DataHelper::ReadVarint(*m_DataStream);
    }
}
