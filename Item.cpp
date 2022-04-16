#include "Item.h"

#define HIBYTE(v) (v >> 8) & 0xff

Item::Item(QDataStream &ItemData, int inventoryId, int x, int y)
	: m_DataStream(&ItemData), m_inventoryId(inventoryId), m_x(x), m_y(y)
{
	// Hash
	qint32 Hash = readData<qint32>();
	readData<quint8>();

	// 获取物品基本信息
	m_BaseItemType = Helper::Data::GetBaseItemType(Hash);

	QString Id = m_BaseItemType["Id"].toString();
	QString Name = m_BaseItemType["Name"].toString();
	QString InheritsFrom = m_BaseItemType["InheritsFrom"].toString();

	qDebug() << "----" << Name << Id << "->" << InheritsFrom << Hash;

	// 获取组件列表
	QJsonArray ItemComponents = Helper::Data::GetItemComponent(InheritsFrom);

	for (auto v : ItemComponents)
	{
		qDebug() << "--------" << v.toString();

		QJsonObject *temp = nullptr;
		this->metaObject()->invokeMethod(this,
										 v.toString().toLatin1().data(),
										 Qt::DirectConnection,
										 Q_RETURN_ARG(QJsonObject *, temp));

		m_Components.insert(v.toString(), *temp);

		if (temp != nullptr && !(temp->isEmpty()))
		{
			qDebug() << "===========" << *temp;
		}
	}
}

QJsonObject *Item::Base()
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
		Item(*this->m_DataStream);
	}
	if ((v22 & 0x80) != 0)
	{
		quint8 size = readData<quint8>();
		for (int i = 0; i < size; i++)
		{
			readData<quint32>();
			Helper::DataHelper::ReadVarint(*(this->m_DataStream));
		}
	}

	return &m_Base;
}

QJsonObject *Item::Mods()
{
	m_Mods.insert("m_level", readData<quint8>());
	readData<quint8>();
	quint8 v15 = readData<quint8>();
	if ((v15 & 0x20) != 0)
	{
		readData<quint16>();
		readData<quint16>();
		readData<quint8>();
	}
	AlternateQualityTypes();

	return &m_Mods;
}

QJsonObject *Item::Stack()
{
	m_Stack.insert("m_stackSize", readData<quint16>());

	return &m_Stack;
}

QJsonObject *Item::HeistContract()
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

	return &m_HeistContract;
}

QJsonObject *Item::Map()
{
	readData<quint8>();
	readData<quint8>();

	return &m_Map;
}

QJsonObject *Item::HeistBlueprint()
{
	readData<quint32>();
	readData<quint8>();
	readData<quint8>();

	quint8 size = readData<quint8>();

	for (size_t i = 0; i < size; i++)
	{
		HeistJobs();
	}

	return &m_HeistBlueprint;
}

QJsonObject *Item::Quality()
{
	quint8 Quality = readData<quint8>();

	m_Quality.insert("m_Quality", Quality);

	return &m_Quality;
}

QJsonObject *Item::Charges()
{
	Helper::DataHelper::ReadVarint1(*(this->m_DataStream));
	return &m_Charges;
}

QJsonObject *Item::AttributeRequirements()
{
	m_AttributeRequirements = Helper::Data::GetComponentAttributeRequirement(m_BaseItemType["Id"].toString());

	return &m_AttributeRequirements;
}

QJsonObject *Item::Armour()
{
	quint8 Quality = readData<quint8>();

	m_Armour.insert("m_Quality", Quality);
	m_Armour.insert("m_ArmourType", Helper::Data::GetArmourType(m_BaseItemType["_rid"].toInt()));
	return &m_Armour;
}

QJsonObject *Item::Weapon()
{
	m_Weapon.insert("m_WeaponType", Helper::Data::GetWeaponType(m_BaseItemType["_rid"].toInt()));
	return &m_Weapon;
}

QJsonObject *Item::Sockets()
{
	quint8 v31 = readData<quint8>();

	if ((v31 & 1) == 0 && (v31 & 2) == 0)
	{
		//  插槽数量
		quint8 socketNumber = (v31 >> 2) & 7;
		quint8 v35 = v31 >> 5;

		QJsonArray sockets;
		QJsonArray socketItems;
		QJsonArray socketLinks;
		QJsonArray socketColors;

		for (size_t i = 0; i < socketNumber; i++)
		{
			quint8 socketInfo = readData<quint8>();
			quint8 isItem = socketInfo & 1;
			quint8 color = socketInfo >> 1; // 1 = 红色, 2 = 绿色, 3 = 蓝色, 4 = 白色
			socketColors.append(color);

			sockets.append(socketInfo);

			if (socketInfo & 1)
			{
				socketItems.append(Item(*m_DataStream).toJson());
			}
		}

		for (size_t i = 0; i < v35; i++)
		{
			socketLinks.append(readData<quint8>());
		}

		m_Sockets.insert("sockets", sockets);
		m_Sockets.insert("socketItems", socketItems);
		m_Sockets.insert("socketLinks", socketLinks);
		m_Sockets.insert("socketColors", socketColors);
	}

	return &m_Sockets;
}

QJsonObject *Item::SkillGem()
{
	qint32 Experience = readData<qint32>();

	m_SkillGem.insert("m_Experience", Experience);

	return &m_SkillGem;
}

void Item::AlternateQualityTypes()
{
	QJsonArray implicitMods;
	QJsonArray explicitMods;
	QJsonArray enchantMods;
	QJsonArray craftedMods;

	quint16 size = 0;
	quint16 v8 = readData<quint16>(); // 是否鉴定,颜色
	readData<quint8>();

	// 是否已鉴定
	quint8 identified = v8 & 0b1000;

	// 装备颜色
	quint8 frameType = v8 & 0b0111;

	if (v8 & 0x200)
	{
		readData<quint8>();
		readData<quint8>();
	}

	// implicitMods 固定的词条,未鉴定的也会有
	if (v8 & 0x10)
	{
		size = readData<quint8>();

		for (int i = 0; i < size; i++)
		{
			implicitMods.append(fs_ItemTypeRegister_Mods());
		}
	}

	// enchantMods 附魔的词条
	if (v8 & 0x40)
	{
		size = readData<quint8>();

		for (int i = 0; i < size; i++)
		{
			enchantMods.append(fs_ItemTypeRegister_Mods());
		}
	}

	// craftedMods 手工附魔词缀(工艺台)
	if ((v8 & 0x400) != 0)
	{
		size = readData<quint8>();

		for (int i = 0; i < size; i++)
		{
			craftedMods.append(fs_ItemTypeRegister_Mods());
		}
	}

	// 判断是否已鉴定
	if (identified)
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

		// explicitMods 鉴定后会出现的词条
		if (v8 & 0x20)
		{
			size = readData<quint8>();

			for (int i = 0; i < size; i++)
			{
				explicitMods.append(fs_ItemTypeRegister_Mods());
			}
		}
	}

	m_Mods.insert("implicitMods", implicitMods);
	m_Mods.insert("explicitMods", explicitMods);
	m_Mods.insert("enchantMods", enchantMods);
	m_Mods.insert("craftedMods", craftedMods);
}

QJsonObject Item::fs_ItemTypeRegister_Mods()
{
	quint16 HASH16 = readData<quint16>();
	QJsonObject mod = Helper::Data::GetMods(HASH16);

	QJsonObject MyMod;
	QJsonObject stats;

	if (!mod["StatsKey1"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey1").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["StatsKey2"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey2").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["StatsKey3"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey3").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["StatsKey4"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey4").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["StatsKey5"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey5").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["StatsKey6"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("StatsKey6").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["Heist_StatsKey0"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("Heist_StatsKey0").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}
	if (!mod["Heist_StatsKey1"].isNull())
	{
		QJsonObject stat = Helper::Data::GetStat(mod.value("Heist_StatsKey1").toInt());
		stats.insert(stat.value("Text").toString(), Helper::DataHelper::ReadVarint1(*this->m_DataStream));
	}

	MyMod.insert(mod.value("CorrectGroup").toString(), stats);

	return MyMod;
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
