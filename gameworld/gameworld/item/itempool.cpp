#include <memory.h>
#include "gamelog.h"
#include "itembase.h"
#include "itempool.h"
#include "checkresourcecenter.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/smartmountssystemdef.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "servercommon/huanshoudef.hpp"
#include "servercommon/partnerdef.h"
#include "equipment/equipment.h"
#include "equipment/jewelry.h"
#include "item/other/otheritem.hpp"
#include "item/gift/defgift.hpp"
#include "item/gift/randgift.hpp"
#include "item/pet/petitem.hpp"
#include "item/expense/norexitem.hpp"
#include "item/elementcrystal/elementcrystal.hpp"
#include "item/expcrystal/expcrystal.hpp"
#include "item/treasuremap/treasureitem.hpp"
#include "item/medalitem/medalitem.hpp"
#include "item/partnernatalcard/partnernatalcard.hpp"
#include "item/specialpet/specialpet.hpp"
#include "item/gift/optionalgift.hpp"
#include "item/huanshouskill/huanshouskillitem.hpp"
#include "item/item_convert_job_item/itemconvertbyjobitem.hpp"
#include "equipment/petequipment.h"
#include "other/treasure_map/treasuremapconfig.h"
#include "equipment/petsoulequipment.h"
#include "item/zhulingitem/zhulingitem.hpp"
#include "item/yuanhunzhuitem/yuanhunzhuitem.hpp"
#include "equipment/godprintequipment.h"
#include "item/spiritpact/spiritpactitem.hpp"
#include "equipment/accessoriesment.h"
#include "item/dianhuashiitem/dianhuashiitem.hpp"
#include "item/petbadgeitem/petbadgeitem.hpp"
#include "item/peaktokenitem/peaktokenitem.hpp"
#include "item/zaohua/zaohuaitem.hpp"
#include "item/shengqi/holyitem.h"
#include "item/shengqi/runestoneitem.h"
#include "item/fuyu/fuyuitem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOADEQUIPMENT(NODENAME, CLASSNAME, EQUIPMENTTYPE, CHECKREPEAT) \
	currentElement = RootElement->FirstChildElement(NODENAME);\
	if (!currentElement || !currentElement->FirstChildElement())\
	{\
		*err = configname + ": xml node error in getting node [" NODENAME "]. ";\
		return false;\
	}\
	else\
	{\
		TiXmlElement *subElement = currentElement->FirstChildElement("path");\
		while(subElement)\
		{\
			ItemBase* item = new CLASSNAME(EQUIPMENTTYPE);\
			std::string path;\
			\
			if (!GetNodeValue(subElement,path))\
			{\
				*err = configname + ": xml node error in node [itemmanager->" NODENAME "->path] ";\
				delete item;\
				Release();\
				return false;\
			}\
			\
			if (!item->Init((dir+path).c_str(), err))\
			{\
				delete item;\
				Release();\
				return false;\
			}\
			\
			if (m_item_pool[item->GetItemId()] != 0)\
			{\
				SNPRINTF(errinfo, sizeof(errinfo), "load path: %s error: item id[%d] repeat", path.c_str(),item->GetItemId());\
				*err = errinfo;\
				delete item;\
				Release();\
				return false;\
			}\
			m_item_pool[item->GetItemId()] = item;\
			subElement = subElement->NextSiblingElement();\
		}\
	}

//-------------------------------------------------------------------------------------------
#define LOADITEM(NODENAME, CLASSNAME) \
	currentElement = RootElement->FirstChildElement(NODENAME);\
	if (!currentElement)\
	{\
		*err = configname + ": xml node error in getting node [" NODENAME "]. ";\
		return false;\
	}\
	else\
	{\
		TiXmlElement *subElement = currentElement->FirstChildElement("path");\
		while(subElement)\
		{\
			ItemBase* item = new CLASSNAME();\
			std::string path;\
			\
			if (!GetNodeValue(subElement,path))\
			{\
				*err = configname + ": xml node error in node [itemmanager->" NODENAME "->path] ";\
				delete item;\
				Release();\
				return false;\
			}\
			\
			if (!item->Init((dir+path).c_str(), err))\
			{\
				delete item;\
				Release();\
				return false;\
			}\
			\
			if (m_item_pool[item->GetItemId()] != 0)\
			{\
				SNPRINTF(errinfo, sizeof(errinfo), "load path: %s error: item id[%d] repeat", path.c_str(),item->GetItemId());\
				*err = errinfo;\
				delete item;\
				Release();\
				return false;\
			}\
			m_item_pool[item->GetItemId()] = item;\
			subElement = subElement->NextSiblingElement();\
		}\
	}

//-------------------------------------------------------------------------------------------
#define LOADCONFIG \
	TiXmlDocument document;\
	char errinfo[1024];\
	\
	if (0 == path || !document.LoadFile(path))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load Item Config Error, %s.", path, document.ErrorDesc());\
		*err = errinfo;\
		return false;\
	}\
	\
	TiXmlElement *RootElement = document.RootElement( );\
	\
	if (!RootElement )\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in root.", path);\
		*err = errinfo;\
		return false;\
	}

//-------------------------------------------------------------------------------------------
#define GETNADEVALUE(NODENAME, VALUENAME) \
	if (!GetSubNodeValue(RootElement, NODENAME, VALUENAME) )\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s].", path, NODENAME);\
		*err = errinfo;\
		return false;\
	}

//-------------------------------------------------------------------------------------------
#define GET_INT_ARRAY_BY_SEPERATOR(NODENAME, VALUENAME, SEPERATOR) \
	if (!GetSubNodeIntArrayValueBySeperator(RootElement, NODENAME, VALUENAME, SEPERATOR))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s].", path, NODENAME);\
		*err = errinfo;\
		return false;\
	}

//-------------------------------------------------------------------------------------------
#define GETOPTIONALVALUE(NODENAME, VALUENAME) \
	GetSubNodeValue(RootElement, NODENAME, VALUENAME);

//-------------------------------------------------------------------------------------------
#define LOADITEMBASEATTR \
	GETNADEVALUE("id", m_item_id);\
	GETNADEVALUE("color", m_color);\
	if (m_color <= 0 || m_color >= ItemBase::I_COLOR_MAX)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [color] value.", path);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("isbind", m_is_bind);\
	GETNADEVALUE("candiscard", m_can_discard);\
	GETNADEVALUE("isbroadcast", m_is_broadcast);\
	GETNADEVALUE("sellprice", m_sell_price);\
	if (m_sell_price < 0)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [sellprice] must be >= 0.", path);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("pile_limit", m_pile_limit);\
	if (m_pile_limit <= 0)\
	{\
	SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [pile_limit] must be > 0.", path);\
	*err = errinfo;\
	return false;\
	}\
	GETNADEVALUE("isdroprecord", m_is_record);\
	GETNADEVALUE("time_length", m_time_length);\
	GETNADEVALUE("invalid_time", m_invalid_time);\
	GETNADEVALUE("limit_prof", m_limit_prof);\
	if (m_limit_prof < PROF_TYPE_PROF_NO_LIMIT || m_limit_prof >= PROF_TYPE_MAX || (m_limit_prof > PROF_TYPE_PROF_NO_LIMIT && m_limit_prof % PROFESSION_BASE != 0) \
		|| (m_limit_prof >= PROF_TYPE_INVALID && m_limit_prof < PROFESSION_BASE))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [limit_prof:%d] value.", path, m_limit_prof);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("limit_level", m_limit_level);\
	if (m_limit_level < 0 || m_limit_level > MAX_ROLE_LEVEL)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [limit_level] value.", path);\
		*err = errinfo;\
		return false;\
	}\
	std::string item_name;\
	GETNADEVALUE("name", item_name);\
	if (item_name.length() >= sizeof(m_item_name))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [name] too long.", path);\
		*err = errinfo;\
		return false;\
	}\
	memcpy(m_item_name, item_name.c_str(), (int)item_name.length());\
	m_item_name[item_name.length()] = '\0';\
	GETNADEVALUE("bag_type", m_item_column);\
	if (m_item_column < ItemNamespace::ITEM_COLUMN_TYPE_MIN || m_item_column >= ItemNamespace::ITEM_COLUMN_TYPE_MAX)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [bag_type] value.", path);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("bag_classification", m_bags_belong);\
	if (m_bags_belong < 0 || m_bags_belong >= B_BELONG_TYPE_MAX)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [bag_classification] value.", path);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("item_type", m_item_sub_type);\
	if (m_item_sub_type < 0 || m_item_sub_type >= ItemNamespace::ITEM_SUB_TYPE_MAX)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [item_sub_type] value.", path);\
		*err = errinfo;\
		return false;\
	}\
	GETNADEVALUE("item_trade_money_type", m_item_trade_money_type);\
	if (m_item_trade_money_type < ItemNamespace::ITEM_TRADE_MONEY_TYPE_INVALID || m_item_trade_money_type >= ItemNamespace::ITEM_TRADE_MONEY_TYPE_MAX)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [trade_money_type] value[%d]", path, m_item_trade_money_type);\
	}\
	GETNADEVALUE("trade_need_announcement", m_trade_need_announcement);\
	GET_INT_ARRAY_BY_SEPERATOR("min_price", m_item_trade_min_price, '|');\
	GET_INT_ARRAY_BY_SEPERATOR("max_price", m_item_trade_max_price, '|');\
	GET_INT_ARRAY_BY_SEPERATOR("recommended_price", m_item_trade_recommend_price, '|');\
	if (!CheckTradePrice(m_item_trade_min_price, m_item_trade_max_price, m_item_trade_recommend_price))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: trade price chek failed!", path);\
	}\
	GETNADEVALUE("search_type", m_trade_search_type);\
	if (m_trade_search_type < -1 || m_trade_search_type >= TRADE_ITEM_SEARCH_TYPE_MAX) \
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [search_type] value[%d]", path, m_trade_search_type);\
	}\
	GETNADEVALUE("group", m_trade_num_per_group);\
	if (m_trade_num_per_group < 1)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [group] value[%d]", path, m_trade_num_per_group);\
	}\
	GETOPTIONALVALUE("limit_drop", m_is_limit_drop);\
	GETOPTIONALVALUE("drop_cd_s", m_drop_cooldown_s);
	
//-------------------------------------------------------------------------------------------
#define LOADITEMUSECD() \
	GETNADEVALUE("colddown_id", m_colddown_id);\
	GETNADEVALUE("server_colddown", m_server_colddown);\
	if (m_server_colddown < 0)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml colddown error.", path);\
		*err = errinfo;\
		return false;\
	}

//-------------------------------------------------------------------------------------------
#define LOADCOMPOSEBASE \
	GETNADEVALUE("cancompose", m_can_compose);\
	GETNADEVALUE("need_coin", m_need_coin);\
	GETNADEVALUE("need_num", m_need_num);\
	GETNADEVALUE("product_item_id", m_product_item_id);\
	if (m_can_compose)\
	{\
	if (m_need_coin < 0 || m_need_num <= 0 || m_product_item_id <= 0)\
		{\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error ComposeItem need_coin:%d, need_num:%d, product_item_id:%d.", \
		path, (int)m_need_coin, (int)m_need_num, (int)m_product_item_id);\
		*err = errinfo;\
		return false;\
		}\
		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_item_id, m_product_item_id, 1, __FUNCTION__);\
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TreasureMapItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param1", m_param1);
	if (m_param1 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%d] value.", path, "param1");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param2", m_param2);
	if (m_param2 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%d] value.", path, "m_param2");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("is_sp_map", m_map_type);
	if (m_map_type <= 0 || m_map_type >= TREASURE_MAP_ITEM_RANK_TYPE_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%d] value.", path, "m_map_type");
		*err = errinfo;
		return false;
	}

	return true;
}

bool Jewelry::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	/*if (0 != m_limit_level % 10)		// 灵饰等级必须要能被 10 整除 - 重锻限定
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node limit_level[%d] can not divided evenly by 10.", path, m_limit_level);
		*err = errinfo;
		return false;
	}*/

	GETNADEVALUE("equip_level", m_equipment_level);
	if (m_equipment_level < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node equip_level[%d] <= 0.", path, m_equipment_level);
		*err = errinfo;
		return false;
	}
	
	GETNADEVALUE("attr_type", m_attr_add_type);
	if (m_attr_add_type < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_type[%d] < 0.", path, m_attr_add_type);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("attr_min", m_default_attr_range.min_val);
	if (m_default_attr_range.min_val < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_min[%d] < 0.", path, m_default_attr_range.min_val);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("attr_max", m_default_attr_range.max_val);
	if (m_default_attr_range.max_val < m_default_attr_range.min_val)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_max[%d] < attr_min[%d].", path, m_default_attr_range.max_val, m_default_attr_range.min_val);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("attr_type_2", m_attr_add_type2);
	if (m_attr_add_type2 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_type[%d] < 0.", path, m_attr_add_type2);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("attr_min_2", m_default_attr_range2.min_val);
	if (m_default_attr_range2.min_val < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_min[%d] < 0.", path, m_default_attr_range2.min_val);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("attr_max_2", m_default_attr_range2.max_val);
	if (m_default_attr_range2.max_val < m_default_attr_range2.min_val)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node attr_max[%d] < attr_min[%d].", path, m_default_attr_range2.max_val, m_default_attr_range2.min_val);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("entry_num", m_entry_num);
	if (m_entry_num < 0 || m_entry_num >= JewelryParam::EXA_ATTR_NUM_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node entry_num[%d] illegal.", path, m_entry_num);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("entry_group", m_entry_group);
	if (m_entry_group < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node entry_group[%d] illegal.", path, m_entry_group);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color <= 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	for (int attr_idx = 0; attr_idx < EQUIP_JEWELRY_ATTR_NUM; attr_idx++)
	{
		std::string read_path = STRING_SPRINTF("added_attr_type%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_attr_type[attr_idx]);
		if (m_attr_type < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_attr_type[%d] illegal.", path, m_attr_type[attr_idx]);
			*err = errinfo;
			return false;
		}
		read_path = STRING_SPRINTF("min_value%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_min_value[attr_idx]);
		if (m_min_value[attr_idx] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_min_value[%d] illegal.", path, m_min_value[attr_idx]);
			*err = errinfo;
			return false;
		}
		read_path = STRING_SPRINTF("max_value%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_max_value[attr_idx]);
		if (m_max_value[attr_idx] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_max_value[%d] illegal.", path, m_max_value[attr_idx]);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("min_jg_common", m_min_jg_common);
	GETNADEVALUE("max_jg_common", m_max_jg_common);
	GETNADEVALUE("min_jg_added", m_min_jg_added);
	GETNADEVALUE("max_jg_added", m_max_jg_added);

	GETNADEVALUE("special_equip", m_special_equip);
	GETNADEVALUE("suitgroup_id", m_suit_random_group);
	GETNADEVALUE("is_star", star_group);

	if (m_max_value < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_special_equip[%d] illegal.", path, m_special_equip);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("ornament_type", m_ornament_type);
	if (m_ornament_type < J_TYPE_GEM_RING || m_ornament_type >= J_TYPE_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node ornament_type[%d] illegal.", path, m_ornament_type);
		*err = errinfo;
		return false;
	}
	
	int without_level_limit = 0;
	GETNADEVALUE("without_level_limit", without_level_limit);
	m_is_unlimit_level = without_level_limit > 0 ? true : false;

	return true;
}

bool Equipment::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("limit_prof_up", m_limit_prof_grade);
	if (m_limit_prof_grade < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node limit_prof_up[%d] <= 0.", path, m_limit_prof_grade);
		*err = errinfo;
		return false;
	}

	int without_level_limit = 0;
	GETNADEVALUE("without_level_limit", without_level_limit);
	m_is_unlimit_level = without_level_limit > 0 ? true : false;

	GETNADEVALUE("equip_level", m_equip_level);
	if (m_equip_level <= 0 || m_equip_level > ItemNamespace::EQUIPMENT_MAX_LEVEL)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node equip_level[%d] <= 0.", path, m_equip_level);
		*err = errinfo;
		return false;
	}

	if (E_TYPE_WEAPON == m_equipment_type)
	{
		GETNADEVALUE("weapon_type", m_weapon_type);
		if (m_weapon_type < WEAPON_TYPE_NONE || m_weapon_type >= WEAPON_TYPE_MAX)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node weapon_type[%d] < 0 || >= WEAPON_TYPE_MAX.", path, m_weapon_type);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("effect_group", m_effect_random_group);
	if (m_effect_random_group != 0)
	{
		CheckResourceCenter::Instance().GetSpecialEffectCheck()->Add(m_effect_random_group, __FUNCTION__);
	}

	
	GETNADEVALUE("equip_type2", m_equipment_class);
	if (m_equipment_class < E_CLASS_MIN || m_equipment_class >= E_CLASS_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node equip_type2[%d] illegal.", path, m_equipment_class);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("entry_num", m_entry_num);
	if (m_entry_num < 0 || m_entry_num >= EquipmentParam::EXA_ATTR_NUM_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node entry_num[%d] illegal.", path, m_entry_num);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("entry_group", m_entry_group);
	if (m_entry_group < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node entry_group[%d] illegal.", path, m_entry_group);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	for (int attr_idx = 0; attr_idx < EQUIP_JEWELRY_ATTR_NUM; attr_idx++)
	{
		std::string read_path = STRING_SPRINTF("added_attr_type%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_attr_type[attr_idx]);
		if (m_attr_type[attr_idx] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_attr_type[%d] illegal.", path, m_attr_type[attr_idx]);
			*err = errinfo;
			return false;
		}
		read_path = STRING_SPRINTF("min_value%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_min_value[attr_idx]);
		if (m_min_value[attr_idx] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_min_value[%d] illegal.", path, m_min_value[attr_idx]);
			*err = errinfo;
			return false;
		}
		read_path = STRING_SPRINTF("max_value%d", attr_idx);
		GETNADEVALUE(read_path.c_str(), m_max_value[attr_idx]);
		if (m_max_value[attr_idx] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_max_value[%d] illegal.", path, m_max_value[attr_idx]);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("min_jg_common", m_min_jg_common);
	GETNADEVALUE("max_jg_common", m_max_jg_common);
	GETNADEVALUE("min_jg_added", m_min_jg_added);
	GETNADEVALUE("max_jg_added", m_max_jg_added);

	GETNADEVALUE("special_equip", m_special_equip);
	GETNADEVALUE("suitgroup_id", m_suit_random_group);

	GETNADEVALUE("is_star", star_group);

	if (m_max_value < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_special_equip[%d] illegal.", path, m_special_equip);
		*err = errinfo;
		return false;
	}
	
	const char* ATTR_STR_LIST[] = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "jingshen", "huifu",
								"bisha", "bisha_dmg", "jianren", "fanji", "mingzhong", "shanduo",
								"def_zhongdu", "def_shihua", "def_hunshui", "def_jiuzui", "def_hunluan", "mogong", "mokang" };
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(ATTR_STR_LIST) == EQUIP_ATTR_IDX_MAX);

	char tmp_min[20];
	char tmp_max[20];

	for (int i = EQUIP_ATTR_IDX_MIN; i < EQUIP_ATTR_IDX_MAX; ++i)
	{
		memset(tmp_min, 0, sizeof(tmp_min));
		memset(tmp_max, 0, sizeof(tmp_max));

		SNPRINTF(tmp_min, sizeof(tmp_min), "min_%s", ATTR_STR_LIST[i]);
		GETNADEVALUE(tmp_min, m_attr_list[i].min_val);

		SNPRINTF(tmp_max, sizeof(tmp_max), "max_%s", ATTR_STR_LIST[i]);
		GETNADEVALUE(tmp_max, m_attr_list[i].max_val);
		if (m_attr_list[i].max_val < m_attr_list[i].min_val)
		{
			SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] < [%s].", path, tmp_max, tmp_min);
			*err = errinfo;
			return false;
		}
	}

	if (E_TYPE_WEAPON == m_equipment_type)
	{
		int weapon_type = GetBattleWeaponType(m_equipment_class);
		ITEMPOOL->AddBaseWeaponID(weapon_type, m_item_id);
	}

	return true;
}

bool ZhuLingItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	return true;
}

bool YuanHunZhuItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	return true;
}

bool SpiritPactItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("partner_id", m_partner_id);
	if (0 > m_partner_id
		|| PARTNER_NUM_MAX <= m_partner_id)
	{
		SNPRINTF(errinfo, sizeof(errinfo),
			"%s: xml node error in node partner_id[%d] < 0 || partner_id > PARTNER_NUM_MAX[%d].",
			path, m_partner_id, PARTNER_NUM_MAX);

		*err = errinfo;
		return false;
	}

	GETNADEVALUE("star_level", m_star);
	if (0 > m_star
		|| PARTNER_SPIRIT_PACT_MAX_STAR < m_star)
	{
		SNPRINTF(errinfo, sizeof(errinfo), 
			"%s: xml node error in node star_level[%d] <= 0 || m_star > MAX_STAR[%d].",
			path, m_partner_id, PARTNER_SPIRIT_PACT_MAX_STAR);

		*err = errinfo;
		return false;
	}

	GETNADEVALUE("promote_expens", m_up_star_cost_id);
	if (0 != m_up_star_cost_id)
	{
		CheckResourceCenter::Instance().GetItemValidCheck()
			->Add(m_item_id, m_up_star_cost_id, 1, __FUNCTION__);
	}


	GETNADEVALUE("expens_num", m_up_star_cost_num);
	if (0 > m_partner_id)
	{
		SNPRINTF(errinfo, sizeof(errinfo), 
			"%s: xml node error in node expens_num[%d] < 0.",
			path, m_up_star_cost_num);

		*err = errinfo;
		return false;
	}

	GETNADEVALUE("promote_products", m_up_star_out_id);
	if (0 != m_up_star_cost_id)
	{
		if (0 != m_up_star_out_id)
		{
			CheckResourceCenter::Instance().GetItemValidCheck()
				->Add(m_item_id, m_up_star_out_id, 1, __FUNCTION__);
		}
		else
		{//有消耗 没产物
			SNPRINTF(errinfo, sizeof(errinfo),
				"%s: xml node error in node promote_expens[%d] promote_products[%d].",
				path, m_up_star_cost_id, m_up_star_out_id);

			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("attr_up", m_attr_up_per_mil);
	if (0 >= m_attr_up_per_mil)
	{
		SNPRINTF(errinfo, sizeof(errinfo),
			"%s:  xml node error in node attr_up[%d] <= 0.",
			path, m_up_star_cost_num);

		*err = errinfo;
	}
	
	CheckResourceCenter::Instance().GetPartnerPactCheckStruct()
		->Add(m_item_id, __FUNCTION__);

	return true;
}

bool PetEquipment::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("equip_level", m_equip_level);
	if (m_equip_level <= 0 || m_equip_level > ItemNamespace::EQUIPMENT_MAX_LEVEL)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node equip_level[%d] <= 0.", path, m_equip_level);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("break_item", m_break_item);
	if (m_break_item < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node break_item[%d] illegal.", path, m_break_item);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("break_num", m_break_num);
	if (m_break_num < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node break_num[%d] illegal.", path, m_break_num);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("number_id", m_number_id);
	if (m_number_id < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node entry_num[%d] illegal.", path, m_number_id);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("skill_group", m_skill_group);
	if (m_skill_group < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node skill_group[%d] illegal.", path, m_skill_group);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("skill_group_2", m_skill_group_2);
	if (m_skill_group_2 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node skill_group_2[%d] illegal.", path, m_skill_group_2);
		*err = errinfo;
		return false;
	}

	return true;
}

bool GodPrintEquipment::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param1", m_param1);
	if (m_param1 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node param1[%d] illegal.", path, m_param1);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param2", m_param2);
	if (m_param2 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node param2[%d] illegal.", path, m_param2);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param3", m_param3);
	if (m_param3 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node param3[%d] illegal.", path, m_param3);
		*err = errinfo;
		return false;
	}

	return true;
}

bool NorexItem::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("use_type", m_use_type);
	if (m_use_type <= I_NOREX_ITEM_INVALID || m_use_type >= I_NOREX_ITEM_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s] value.", path, "use_type");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param1", m_param1);
	if (m_param1 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s] value.", path, "param1");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param2", m_param2);
	if (m_param2 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s] value.", path, "param2");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param3", m_param3);
	if (m_param3 < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "param3");
		*err = errinfo;
		return false;
	}

	switch (m_use_type)
	{
	case I_NOREX_ITEM_GIFT:
		{
			// m_param1为宠物类的强化等级
			// m_param2为奖池id 对应 gift_randitem.xml 
			// m_param3为1时是自选礼包 可以自己挑选一个奖池里面的宠物
		}
		break;
	case I_NOREX_ITEM_IMMORTAL_COIN_CARD:
		{
			// 获得的仙玉数量
			if (m_param1 <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml [%s] Error. not be less-than 0 ", path, "param1");
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_PET_EGGS:
		{
			// m_param1为获得的宠物id
			char check_str[1024] = { 0 };
			SNPRINTF(check_str, sizeof(check_str), "%s: xml [%s]", path, "param1");
			CheckResourceCenter::Instance().GetPetCheck()->Add(m_param1, __FUNCTION__);
			// m_param2为宠物的强化等级
			if (m_param2 <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml [%s] Error. not <= 0 ", path, "param2");
				*err = errinfo;
				return false;
			}
			// 当拥有的宠物大于m_param3时不能再获得宠物
			if (m_param3 < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml [%s] Error. not < 0 ", path, "param3");
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_PET_SKILL:
		{
			// m_param1为技能id
			if (m_param1 < 0 || m_param1 >= PET_SKILL_LEVLE_PARAM)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml [%s] Error. not < 0 or >= %d", path, "param1", PET_SKILL_LEVLE_PARAM);
				*err = errinfo;
				return false;
			}
			// m_param2为技能等级
			// m_param3为技能学习等级
		}
		break;

	case I_NOREX_ITEM_HP_RECOVER:
	case I_NOREX_ITEM_RECOVER_MP:
	case I_NOREX_ITEM_HP_PERCENT_RECOVER:
	case I_NOREX_ITEM_MP_PERCENT_RECOVER:
		{
			if (m_param1 <= 0 || m_param2 < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml param Error. param1[%d] or param2[%d] <= 0", path, m_param1, m_param2);
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_BUY_GIFT_ITEM:
		{
			if (m_param1 <= 0 || m_param1 >= BUY_GIFT_ITEM_TYPE_MAX)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param1[%d] <= 0 or >= max[%d]", path, m_param1, BUY_GIFT_ITEM_TYPE_MAX);
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_CONVERT_ITEM:
		{
			char check_str[1024] = { 0 };
			SNPRINTF(check_str, sizeof(check_str), "%s: xml [%s]", path, "param1");
			CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_item_id, m_param1, m_param2, __FUNCTION__);
			if (m_param2 <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml [%s] Error. not <= 0 ", path, "param2");
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_REALIVE:
		{
			if (m_param1 < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param1[%d] < 0", path, m_param1);
				*err = errinfo;
				return false;
			}
			if (m_param2 < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param2[%d] < 0", path, m_param2);
				*err = errinfo;
				return false;
			}
			if (m_param1 <= 0 && m_param2 <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param1[%d] <= 0 and param2[%d] <= 0", path, m_param1, m_param2);
				*err = errinfo;
				return false;
			}
		}
		break;
	case I_NOREX_ITEM_SMART_MOUNTS_SKILL:
		{
			CheckResourceCenter::Instance().GetPassiveSkillCheckStruct()->Add(m_param1, __FUNCTION__);
			if (m_param2 < 0 || m_param2 >= MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param2[%d] < 0 or >= %d", path, m_param2, MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM);
				*err = errinfo;
				return false;
			}
			if (m_param3 <= 0 || m_param3 >= MAX_BATTLE_SKILL_LEVEL)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param3[%d] <= 0 or >= %d", path, m_param3, MAX_BATTLE_SKILL_LEVEL);
				*err = errinfo;
				return false;
			}
		}
		break;	
	case I_NOREX_ITEM_HUANSHOU_EGG:
		{
			if (m_param1 < 0 || m_param1 >= MAX_HUAN_SHOU_NUM)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml param Error. param1[%d] < 0 or >= %d", path, m_param1, MAX_HUAN_SHOU_NUM);
				*err = errinfo;
				return false;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

bool MedalItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node medal attrribute.", path);
	*err = errinfo;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("att_group", m_attr_group);
	GETNADEVALUE("sp_group", m_sp_group);
	GETNADEVALUE("initial_rank", m_initial_rank);

	return true;
}

bool PetSoulEquipment::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("pet_weapon_seq", pet_weapon_seq);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pet_weapon_seq[%d] illegal.", path, pet_weapon_seq);
		*err = errinfo;
		return false;
	}

	return true;
}

bool PetItem::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	return true;
}

bool ExpCrystal::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	return true;
}

bool PartnerNatalCard::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("partner_equipment_type", m_equip_type);
	if (m_equip_type < 0 || m_equip_type >= PE_TYPE_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value. ItemID: %d | CurValue: %d", path, "partner_equipment_type", m_item_id, m_equip_type);
		*err = errinfo;
		return false;
	}

	return true;
}

bool OtherItem::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("sub_type", m_sub_type);
	if (m_sub_type < I_SUB_TYPE_INVALID || m_sub_type >= OTHER_ITEM_SUB_TYPE_NUM)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s] value.", path, "sub_type");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("value", m_value);
	if (m_value < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [%s] value.", path, "value");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("refresh_time", m_day_invalid_time);

	return true;
}

bool DefGift::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

// 	if (!m_is_bind)
// 	{
// 		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value. gift must be bind", path, "is_bind");
// 		*err = errinfo;
// 		return false;
// 	}

	GETNADEVALUE("coin_bind", m_coin_bind);
	if (m_coin_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "coin_bind");
		*err = errinfo;
		return false;
	}
	
	GETNADEVALUE("gold_bind", m_gold_bind);
	if (m_gold_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "gold_bind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("gold", m_gold);
	if (m_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "gold");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("is_check_prof", m_is_check_prof);
	GETNADEVALUE("item_num", m_item_num);
	if (m_item_num < 0 || m_item_num > GIFT_ITEM_MAX_NUM)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "item_num");
		*err = errinfo;
		return false;
	}

	char strNode[32] = "";
	for (int i = 0; i < m_item_num; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode),"item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (m_gift_item_list[i].item_id <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		if (m_gift_item_list[i].item_id == m_item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: def gift cannot cantain self %d.", path, m_item_id);
			*err = errinfo;
			return false;
		}

		SNPRINTF(strNode, sizeof(strNode),"item_%d_num", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].num);
		if (m_gift_item_list[i].num <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_item_id, m_gift_item_list[i].item_id, m_gift_item_list[i].num, __FUNCTION__);

		SNPRINTF(strNode, sizeof(strNode), "is_bind_%d", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].is_bind);
	}

	for (int i = m_item_num; i < GIFT_ITEM_MAX_NUM; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode), "item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (0 != m_gift_item_list[i].item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] not zero.", path, strNode);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("need_gold", m_need_gold);
	if (m_need_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "need_gold");
		*err = errinfo;
		return false;
	}

	//GETNADEVALUE("need_gold_bind", m_need_bind_gold);
	//if (m_need_bind_gold < 0)
	//{
	//	SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "need_gold_bind");
	//	*err = errinfo;
	//	return false;
	//}

	//只有一个有值
	if (m_need_gold && m_need_bind_gold)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node m_need_gold or m_need_bind_gold", path);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("pet_animation", m_is_pet_gift);

	return true;
}

bool RandGift::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

// 	if (!m_is_bind)
// 	{
// 		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value. gift must be bind", path, "is_bind");
// 		*err = errinfo;
// 		return false;
// 	}

	GETNADEVALUE("coin_bind", m_coin_bind);
	if (m_coin_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "coin_bind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("gold_bind", m_gold_bind);
	if (m_gold_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "gold_bind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("gold", m_gold);
	if (m_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "gold");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("need_gold", m_need_gold);
	if (m_need_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "need_gold");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("need_gold_bind", m_need_bind_gold);
	if (m_need_bind_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "need_gold_bind");
		*err = errinfo;
		return false;
	}

	//只有一个有值
	if (m_need_gold && m_need_bind_gold)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node m_need_gold or m_need_bind_gold", path);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("is_check_prof", m_is_check_prof);
	GETNADEVALUE("item_num", m_item_num);
	if (m_item_num < 0 || m_item_num > GIFT_ITEM_MAX_NUM)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "item_num");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("rand_num", m_rand_num);
	if (m_rand_num < 0 || m_rand_num >= m_item_num)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, "rand_num");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("item_column_type", m_reward_column_type);

	m_sum_rate = 0;
	int has_rate_item = 0;
	char strNode[32] = "";
	for (int i = 0; i < m_item_num; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode), "item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (m_gift_item_list[i].item_id <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		if (m_gift_item_list[i].item_id == m_item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: rand gift cannot cantain self %d.", path, m_item_id);
			*err = errinfo;
			return false;
		}

		SNPRINTF(strNode, sizeof(strNode), "item_%d_num", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].num);
		if (m_gift_item_list[i].num <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_item_id, m_gift_item_list[i].item_id, m_gift_item_list[i].num, __FUNCTION__);

		SNPRINTF(strNode, sizeof(strNode), "is_bind_%d", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].is_bind);

		SNPRINTF(strNode, sizeof(strNode), "item_%d_rate", i + 1);
		GETNADEVALUE(strNode, m_item_rate_list[i]);
		if (m_item_rate_list[i] < 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}
		else if (m_item_rate_list[i] > 0)
		{
			has_rate_item += 1;
		}

		m_sum_rate += m_item_rate_list[i];
	}

	if (m_sum_rate <= 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error m_sum_rate <= 0.", path);
		*err = errinfo;
		return false;
	}

	//允许概率为0的道具,那么此时添加判断有概率道具数量与随出的数量(随机道具逻辑不会重复)
	if (has_rate_item < m_rand_num)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error has_rate_item <= m_rand_num.", path);
		*err = errinfo;
		return false;
	}

	for (int i = m_item_num; i < GIFT_ITEM_MAX_NUM; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode), "item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (0 != m_gift_item_list[i].item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error in node [%s] not zero.", path, strNode);
			*err = errinfo;
			return false;
		}
	}

	UNSTD_STATIC_CHECK(5 == RAND_GIFT_BROADCAST_ITEM_NUM);
	GETNADEVALUE("boardcast_item_1", m_boardcast_item_list[0]);
	GETNADEVALUE("boardcast_item_2", m_boardcast_item_list[1]);
	GETNADEVALUE("boardcast_item_3", m_boardcast_item_list[2]);
	GETNADEVALUE("boardcast_item_4", m_boardcast_item_list[3]);
	GETNADEVALUE("boardcast_item_5", m_boardcast_item_list[4]);

	for (int i = 0; i < RAND_GIFT_BROADCAST_ITEM_NUM; i++)
	{
		if (0 == m_boardcast_item_list[i]) continue;

		bool has_found = false;
		for (int k = 0; k < GIFT_ITEM_MAX_NUM; k++)
		{
			if (0 == m_gift_item_list[k].item_id) break;

			if (m_boardcast_item_list[i] == m_gift_item_list[k].item_id)
			{
				has_found = true;
				break;
			}
		}

		if (!has_found)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error broadcast_item not found item_id = %d.", path, (int)m_boardcast_item_list[i]);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("boardcast_string_id", m_boardcast_string_id);
	if (m_boardcast_item_list[0] > 0 || m_boardcast_item_list[1] > 0 || m_boardcast_item_list[2] > 0 || m_boardcast_item_list[3] > 0 || m_boardcast_item_list[4] > 0)
	{
		if (m_boardcast_string_id <= 0 || m_boardcast_string_id > OPEN_RANDGIFT_BROADCAST_ID_MAX_NUM)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error boardcast_string_id error", path);
			*err = errinfo;
			return false;
		}
	}
	else
	{
		if (m_boardcast_string_id > 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml error boardcast_string_id no need", path);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("pet_animation", m_is_pet_gift);

	return true;
}

bool OptionalGift::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("coin_bind", m_coin_bind);
	if (m_coin_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "coin_bind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("gold_bind", m_gold_bind);
	if (m_gold_bind < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "gold_bind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("gold", m_gold);
	if (m_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "gold");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("is_check_prof", m_is_check_prof);
	GETNADEVALUE("item_num", m_item_num);
	if (m_item_num < 0 || m_item_num > GIFT_ITEM_MAX_NUM)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "item_num");
		*err = errinfo;
		return false;
	}

	char strNode[32] = "";
	for (int i = 0; i < m_item_num; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode), "item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (m_gift_item_list[i].item_id <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		if (m_gift_item_list[i].item_id == m_item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo),  "%s: def gift cannot cantain self %d.", path, m_item_id);
			*err = errinfo;
			return false;
		}

		SNPRINTF(strNode, sizeof(strNode), "item_%d_num", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].num);
		if (m_gift_item_list[i].num <= 0)
		{
			SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, strNode);
			*err = errinfo;
			return false;
		}

		CheckResourceCenter::Instance().GetItemValidCheck()->Add(m_item_id, m_gift_item_list[i].item_id, m_gift_item_list[i].num, __FUNCTION__);

		SNPRINTF(strNode, sizeof(strNode), "is_bind_%d", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].is_bind);
	}

	for (int i = m_item_num; i < GIFT_ITEM_MAX_NUM; ++i)
	{
		SNPRINTF(strNode, sizeof(strNode), "item_%d_id", i + 1);
		GETNADEVALUE(strNode, m_gift_item_list[i].item_id);
		if (0 != m_gift_item_list[i].item_id)
		{
			SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] not zero.", path, strNode);
			*err = errinfo;
			return false;
		}
	}

	GETNADEVALUE("need_gold", m_need_gold);
	if (m_need_gold < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "need_gold");
		*err = errinfo;
		return false;
	}
	
	GETNADEVALUE("select_num", m_optional_num);
	if (m_optional_num <= 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml error in node [%s] value.", path, "select_num");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("pet_animation", m_is_pet_gift);

	return true;
}

bool ElementCrystal::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("element_water", m_element_water);
	if (m_element_water < 0 || m_element_water > 10)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "element_water");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("element_fire", m_element_fire);
	if (m_element_fire < 0 || m_element_fire > 10)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "element_fire");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("element_wind", m_element_wind);
	if (m_element_wind < 0 || m_element_wind > 10)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "element_wind");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("element_land", m_element_earth);
	if (m_element_earth < 0 || m_element_earth > 10)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "element_land");
		*err = errinfo;
		return false;
	}

	if (m_element_earth + m_element_fire + m_element_water + m_element_wind != 10)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml value error, element sum not 10", path);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("num_use", m_limit_num);
	if (m_limit_num <= 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in node [%s] value.", path, "num_use");
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("element_type", m_element_type);
	switch (m_element_type)
	{	
		case ELEMENT_TYPE_WATER:
		{
			if (m_element_water <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml element_type[%d], element_water[%d] <= 0?", path, m_element_type, m_element_water);
				*err = errinfo;
				return false;
			}
		}
		break;
		case ELEMENT_TYPE_FIRE:
		{
			if (m_element_fire <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml element_type[%d], element_fire[%d] <= 0?", path, m_element_type, m_element_fire);
				*err = errinfo;
				return false;
			}
		}
		break;
		case ELEMENT_TYPE_WIND:
		{
			if (m_element_wind <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml element_type[%d], element_wind[%d] <= 0?", path, m_element_type, m_element_wind);
				*err = errinfo;
				return false;
			}
		}
		break;
		case ELEMENT_TYPE_EARTH:
		{
			if (m_element_earth <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml element_type[%d], element_land[%d] <= 0?", path, m_element_type, m_element_earth);
				*err = errinfo;
				return false;
			}
		}
		break;
		case ELEMENT_TYPE_INVALID:
		break;
		default:
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml element_type[%d] must in [%d, %d].", path, m_element_type, ELEMENT_TYPE_INVALID, ELEMENT_TYPE_EARTH);
			*err = errinfo;
			return false;
		}
		break;
	}
	
	return true;
}

bool SpecialPet::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("pet_id", m_pet_id);
	CheckResourceCenter::Instance().GetPetCheck()->Add(m_pet_id, __FUNCTION__);

	GETNADEVALUE("max_con", m_max_bp[ADD_POINT_TYPE_VITALITY]);
	GETNADEVALUE("min_con", m_min_bp[ADD_POINT_TYPE_VITALITY]);
	if (m_max_bp[ADD_POINT_TYPE_VITALITY] < m_min_bp[ADD_POINT_TYPE_VITALITY])
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: max_con < min_con", path);
		*err = errinfo;
		return false;
	}
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_max_bp[ADD_POINT_TYPE_VITALITY], ADD_POINT_TYPE_VITALITY, path);
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_min_bp[ADD_POINT_TYPE_VITALITY], ADD_POINT_TYPE_VITALITY, path);

	GETNADEVALUE("max_str", m_max_bp[ADD_POINT_TYPE_STRENGTH]);
	GETNADEVALUE("min_str", m_min_bp[ADD_POINT_TYPE_STRENGTH]);
	if (m_max_bp[ADD_POINT_TYPE_STRENGTH] < m_min_bp[ADD_POINT_TYPE_STRENGTH])
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: max_str < min_str", path);
		*err = errinfo;
		return false;
	}
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_max_bp[ADD_POINT_TYPE_STRENGTH], ADD_POINT_TYPE_STRENGTH, path);
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_min_bp[ADD_POINT_TYPE_STRENGTH], ADD_POINT_TYPE_STRENGTH, path);

	GETNADEVALUE("max_def", m_max_bp[ADD_POINT_TYPE_POWER]);
	GETNADEVALUE("min_def", m_min_bp[ADD_POINT_TYPE_POWER]);
	if (m_max_bp[ADD_POINT_TYPE_POWER] < m_min_bp[ADD_POINT_TYPE_POWER])
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: max_def < min_def", path);
		*err = errinfo;
		return false;
	}
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_max_bp[ADD_POINT_TYPE_POWER], ADD_POINT_TYPE_POWER, path);
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_min_bp[ADD_POINT_TYPE_POWER], ADD_POINT_TYPE_POWER, path);

	GETNADEVALUE("max_agi", m_max_bp[ADD_POINT_TYPE_AGILITY]);
	GETNADEVALUE("min_agi", m_min_bp[ADD_POINT_TYPE_AGILITY]);
	if (m_max_bp[ADD_POINT_TYPE_AGILITY] < m_min_bp[ADD_POINT_TYPE_AGILITY])
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: max_agi < min_agi", path);
		*err = errinfo;
		return false;
	}
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_max_bp[ADD_POINT_TYPE_AGILITY], ADD_POINT_TYPE_AGILITY, path);
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_min_bp[ADD_POINT_TYPE_AGILITY], ADD_POINT_TYPE_AGILITY, path);

	GETNADEVALUE("max_magic", m_max_bp[ADD_POINT_TYPE_MAGIC]);
	GETNADEVALUE("min_magic", m_min_bp[ADD_POINT_TYPE_MAGIC]);
	if (m_max_bp[ADD_POINT_TYPE_MAGIC] < m_min_bp[ADD_POINT_TYPE_MAGIC])
	{
		SNPRINTF(errinfo, sizeof(errinfo),  "%s: max_magic < min_magic", path);
		*err = errinfo;
		return false;
	}
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_max_bp[ADD_POINT_TYPE_MAGIC], ADD_POINT_TYPE_MAGIC, path);
	CheckResourceCenter::Instance().GetPetCheck()->AddAttrCheck(m_pet_id, m_min_bp[ADD_POINT_TYPE_MAGIC], ADD_POINT_TYPE_MAGIC, path);

	return true;
}

bool ItemConvertByJobItem::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	return true;
}

bool Accessoriesment::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("color", m_color);
	if (m_color < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_color[%d] illegal.", path, m_color);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("fenjie_tianming", fenjie_tianming);
	if (fenjie_tianming < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node fenjie_tianming[%d] illegal.", path, fenjie_tianming);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("feisheng_level", m_fly_flag);
	if (m_fly_flag < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node feisheng_level[%d] illegal.", path, m_fly_flag);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("entry_num", m_entry_num);
	if (m_entry_num < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node entry_num[%d] illegal.", path, m_entry_num);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("suit_type", m_suit_type);
	if (m_suit_type < 0 || m_suit_type >= ACCESSORIES_SUIT_TYPE_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node suit_type[%d] illegal.", path, m_suit_type);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("equip_type", m_equip_type);

	if (A_TYPE_MIN + 1 > m_equip_type || m_equip_type >= A_TYPE_MAX + 1)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node equip_type[%d] illegal.", path, m_equip_type);
		*err = errinfo;
		return false;
	}

	for (int i = 0; i < MAX_ACCESSORIES_BASE_VOC_NUM; ++i)
	{
		std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
		GETNADEVALUE(read_path.c_str(), base_attr_list[i].attr_type);

		read_path = STRING_SPRINTF("attr_value_%d", i + 1);
		GETNADEVALUE(read_path.c_str(), base_attr_list[i].attr_value);
	}

	return true;
}

bool DianHuaShiItem::Init(const char* path, std::string *err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("effect_type", effect_type);
	if (effect_type < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node effect_type[%d] illegal.", path, effect_type);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("effect_level", effect_level);
	if (effect_level < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node effect_level[%d] illegal.", path, effect_level);
		*err = errinfo;
		return false;
	}

	return true;
}

bool PetBadgeItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	return true;
}

bool PeakTokenItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;
	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1 illegal.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("param1", m_reward_group_id);
	if (0 >= m_reward_group_id)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node param1 m_reward_group_id[%d] <= 0 illegal.", path, m_reward_group_id);
		*err = errinfo;
		return false;
	}

	CheckResourceCenter::Instance().GetPeakTokenCheckStruct()
		->Add(m_item_id, m_reward_group_id, __FUNCTION__);

	return true;
}

bool ZaoHuaItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("equip_type", m_equip_type);
	if (m_equip_type < ZaoHuaItem::Z_TYPE_MIN || m_equip_type >= ZaoHuaItem::Z_TYPE_MAX)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node m_equip_type[%d] illegal.", path, m_equip_type);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("equip_level", m_equip_level);
	if (m_equip_level <= 0 || m_equip_level > ItemNamespace::EQUIPMENT_MAX_LEVEL)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node equip_level[%d] <= 0.", path, m_equip_level);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("suit_type", m_suit_id);
	GETNADEVALUE("return_item", return_item);
	GETNADEVALUE("return_num", return_num);
	if (return_num < 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node return_num[%d] illegal.", path, return_num);
		*err = errinfo;
		return false;
	}

	for (int i = 0; i < MAX_ZAO_HUA_BASE_VOC_NUM; ++i)
	{
		std::string read_path = STRING_SPRINTF("attr_type_%d", i + 1);
		GETNADEVALUE(read_path.c_str(), m_attr_list[i].attr_type);

		read_path = STRING_SPRINTF("attr_value_%d", i + 1);
		GETNADEVALUE(read_path.c_str(), m_attr_list[i].attr_value);
	}

	return true;
}

bool HolyItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node shengqi attrribute.", path);
	*err = errinfo;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("shengqi_type", m_shengqi_type);

	return true;
}

bool RuneStoneItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node fushi attrribute.", path);
	*err = errinfo;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("fushi_type",m_runestone_type );
	GETNADEVALUE("fushi_level", m_runestone_level);

	return true;
}

bool FuYuItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node medal attrribute.", path);
	*err = errinfo;

	if (1 != m_pile_limit)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node pile_limit[%d] != 1.", path, m_pile_limit);
		*err = errinfo;
		return false;
	}

	GETNADEVALUE("fuyu_level", m_fuyu_level);

	return true;
}

bool HuanShouSkillItem::Init(const char * path, std::string * err)
{
	LOADCONFIG;
	LOADITEMBASEATTR;

	GETNADEVALUE("skill_id", m_skill_id);
	CheckResourceCenter::Instance().GetPassiveSkillCheckStruct()->Add(m_skill_id, __FUNCTION__);

	return true;
}

//---------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ItemPool::Init(const std::string &dir, const std::string &configname, std::string *err)
{
	char errinfo[1024] = {0};

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(dir+configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load ItemManager Config Error, %s.", (dir+configname).c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	TiXmlElement *currentElement = NULL;

	LOADEQUIPMENT("helmet", Equipment, Equipment::E_TYPE_HELMET, true);
	LOADEQUIPMENT("armour", Equipment, Equipment::E_TYPE_ARMOUR, true);
	LOADEQUIPMENT("mainweapon", Equipment, Equipment::E_TYPE_WEAPON, true);
	LOADEQUIPMENT("subweapon", Equipment, Equipment::E_TYPE_SHIELD, true);
	LOADEQUIPMENT("shoe", Equipment, Equipment::E_TYPE_SHOE, true);

	
	LOADITEM("ExpenseNorex", NorexItem);
	LOADITEM("Other", OtherItem);
	LOADITEM("pet", PetItem);

	LOADITEM("DefGift", DefGift);
	LOADITEM("RandGift", RandGift);
	LOADITEM("SelectGift", OptionalGift);

	LOADITEM("element_crystal", ElementCrystal);
	LOADITEM("exp_crystal", ExpCrystal);
	LOADITEM("partner_equipment", PartnerNatalCard);
	LOADITEM("pet_badges", PetBadgeItem);

	LOADEQUIPMENT("ring", Jewelry, Jewelry::J_TYPE_GEM_RING, true);
	LOADEQUIPMENT("bracelet", Jewelry, Jewelry::J_TYPE_BRACELET, true);
	LOADEQUIPMENT("pendant", Jewelry, Jewelry::J_TYPE_PENDANTS, true);

	LOADEQUIPMENT("equipment", PetEquipment, PetEquipment::PET_EQUIPMENT_TYPE_0, true);

	LOADITEM("treasure_map", TreasureMapItem);

	LOADITEM("medal", MedalItem);
	LOADITEM("special_pet", SpecialPet);
	LOADITEM("Change", ItemConvertByJobItem);

	LOADEQUIPMENT("pet_weapon", PetSoulEquipment, PetSoulEquipment::PET_SOUL_EQUIPMENT_TYPE_0, true);

	LOADITEM("spirit_stone", ZhuLingItem);
	LOADITEM("yuan_soul_item", YuanHunZhuItem);

	LOADEQUIPMENT("pet_god_print", GodPrintEquipment, 0, true);
	
	LOADITEM("star_deed", SpiritPactItem);

	LOADEQUIPMENT("peishi", Accessoriesment, 0, true);

	LOADEQUIPMENT("dianhua_props", DianHuaShiItem, 0, true);

	LOADITEM("supreme_token", PeakTokenItem);

	LOADITEM("equip_list", ZaoHuaItem);

	LOADITEM("shengqi", HolyItem);
	LOADITEM("fushi", RuneStoneItem);

	LOADITEM("fuyu", FuYuItem);
	LOADITEM("skill_book", HuanShouSkillItem);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemPool *gs_item_pool_config = NULL;

bool ItemPool::Reload(const std::string &dir, const std::string &filename, std::string *err)
{
	ItemPool *temp = new ItemPool();

	if (!temp->Init(dir, filename, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_item_pool_config) delete gs_item_pool_config;

	gs_item_pool_config = temp;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RELOAD_ITEM(RELOAD_TYPE, CLASS_XML_DIR, CLASSNAME) \
	if (RELOAD_TYPE == item_type)\
	{\
		SNPRINTF(tmp_str, sizeof(tmp_str), "%s/item/%s/%d.xml", dir.c_str(), CLASS_XML_DIR, item_id);\
		TiXmlDocument m_document;\
		if (!m_document.LoadFile(tmp_str))\
		{\
			SNPRINTF(tmp_str, sizeof(tmp_str), "ReloadItem error LoadFile item_id:%d!!!", item_id);\
			*err = tmp_str;\
			return false;\
		}\
		ItemBase *tmp_item = new CLASSNAME();\
		if (!tmp_item->Init(tmp_str, err))\
		{\
			SNPRINTF(tmp_str, sizeof(tmp_str), "ReloadItem error ItemInit item_id:%d!!!", item_id);\
			*err = tmp_str;\
			delete tmp_item;\
			return false;\
		}\
		if (NULL != m_item_pool[item_id])\
		{\
			delete m_item_pool[item_id];\
		}\
		m_item_pool[item_id] = tmp_item;\
		return true;\
	}

bool ItemPool::ReloadOneItem(const std::string &dir, int item_type, ItemID item_id, std::string *err)
{
	static char tmp_str[1024] = {0};

	RELOAD_ITEM(RELOAD_I_TYPE_RANDGIFT, "gift/rand", RandGift);
	RELOAD_ITEM(RELOAD_I_TYPE_EXPENSE, "expense/norex", NorexItem);
	RELOAD_ITEM(RELOAD_I_TYPE_OTHER, "other", OtherItem);

	SNPRINTF(tmp_str, sizeof(tmp_str), "ReloadItem error cannot find item:%d!", item_id);
	*err = tmp_str;

	return false;
}

ItemPool * ItemPool::Instance()
{
	if (NULL == gs_item_pool_config)
	{
		gs_item_pool_config = new ItemPool();
	}

	return gs_item_pool_config;
}

ItemPool::ItemPool()
{
	memset(m_item_pool, 0, sizeof(m_item_pool));
	memset(m_base_weapon_id_list, 0, sizeof(m_base_weapon_id_list));
}

ItemPool::~ItemPool()
{
	this->Release();
}

void ItemPool::Release()
{
	for (int i = 0; i < MAX_ITEM_ID; ++i)
	{
		if (NULL != m_item_pool[i])
		{
			delete m_item_pool[i];
		}
	}
	memset(m_item_pool, 0, sizeof(m_item_pool));
}

const ItemBase * ItemPool::GetItem(ItemID item_id)
{
	if (item_id >= MAX_ITEM_ID) return NULL;

	return m_item_pool[item_id];
}

bool ItemPool::CheckItemLegal(ItemID item_id, int price, const char * reason_str)
{
	if (item_id >= MAX_ITEM_ID) return false;

	if(NULL == m_item_pool[item_id]) return false;

	NetValueItemParam param;
	if (m_item_pool[item_id]->CanSell(&param))
	{
		if (m_item_pool[item_id]->GetSellPrice(&param) > price)
		{
			gamelog::g_log_world.printf(LL_ERROR, "%s is faild, buy_price < sold_price! item_id:%d reason_str[%s]", __FUNCTION__, item_id, reason_str);
			return false;
		}
	}

	return true;
}

short ItemPool::GetItemColumn(ItemID item_id)
{
	if (item_id >= MAX_ITEM_ID) return ItemNamespace::ITEM_COLUMN_TYPE_MIN;

	return m_item_pool[item_id]->GetItemColumn();
}

long long ItemPool::ConvertToLongLong(short param1, short param2, short param3, char param4, char param5)
{
	union ParamToLongLong
	{
		struct
		{
			short m_param1;
			short m_param2;										
			short m_param3;									
			char m_param4;					
			char m_param5;
		};

		long long val;
	};

	ParamToLongLong ll;

	ll.m_param1 = param1;
	ll.m_param2 = param2;
	ll.m_param3 = param3;
	ll.m_param4 = param4;
	ll.m_param5 = param5;

	return ll.val;
}

const char * ItemPool::ItemDataWrapperLog(const ItemDataWrapper *item_wrapper, int index)
{
	if (index < 0 || index >= LOG_MAX_ITEM_NUM) return "NULL";

	static char item_wrapper_log[LOG_BASE_LENGTH * LOG_MAX_ITEM_NUM] = { 0 };
	char *p = item_wrapper_log + index * LOG_BASE_LENGTH;

	if (NULL == item_wrapper)
	{
		SNPRINTF(p, LOG_BASE_LENGTH, "NULL");
	}
	else
	{
		SNPRINTF(p, LOG_BASE_LENGTH,
			"item_id:%d, num:%d, is_bind:%d, has_param:%d, invalid_time:%d, item_param[%s]",
			(int)item_wrapper->item_id,
			(int)item_wrapper->num,
			(int)item_wrapper->is_bind,
			(int)item_wrapper->has_param,
			(int)item_wrapper->invalid_time); // 
	}

	return p;
}

void ItemPool::AddBaseWeaponID(int weapon_type, ItemID weapon_id)
{
	if (weapon_type < 0 || weapon_type >= WEAPON_TYPE_MAX) return;

	if (m_base_weapon_id_list[weapon_type] > 0) return;

	m_base_weapon_id_list[weapon_type] = weapon_id;
}

ItemID ItemPool::GetBaseWeaponID(int weapon_type)
{
	if (weapon_type < 0 || weapon_type >= WEAPON_TYPE_MAX) return 0;

	return m_base_weapon_id_list[weapon_type];
}

