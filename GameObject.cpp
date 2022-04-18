#include "GameObject.h"

void GameObject::readHead()
{
    // Head
    this->readData<quint8>();

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

void GameObject::readPositioned()
{
    // Positioned
    quint32 x = this->readData<quint32>();
    quint32 y = this->readData<quint32>();

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
        {    //fs_BuffDefinitions
          quint16 id =  readData<quint16>();

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
            if(!buffer.isEmpty())
            {
                    if(buffer.value("Unknown43").toInteger() & 16777216 == 16777216)
                    {
                        readData<quint16>();
                    }     

                    if(buffer.value("IsRecovery").toBool())
                    {
                        for (size_t i = 0; i < buffer.value("Unknown41").toArray().size(); i++)
                        {
                            readData<quint32>();
                        }
                        
                    } 
                    
            }

                    quint32 size =  readData<quint32>();
                    for (quint32 i = 0; i < size; i++)
                    {
                        readData<quint32>();
                    }
        }
    }
}