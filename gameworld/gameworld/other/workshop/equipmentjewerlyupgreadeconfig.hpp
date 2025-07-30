#ifndef _EQUIPMENT_JEWERLY_UPGREADE_CONFIG_HPP_
#define _EQUIPMENT_JEWERLY_UPGREADE_CONFIG_HPP_

#include <vector>
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

const static int EQUIPMENT_JEWERLY_HOLE_LEVEL_MAX = 61;//单个孔位最大等级为 EQUIPMENT_JEWERLY_HOLE_LEVEL_MAX - 1
const static int EQUIPMENT_JEWERLY_HOLE_STRENGTHEN_LEVEL_MAX = 15;//单个孔位最大强化等级为 EQUIPMENT_JEWERLY_HOLE_STRENGTHEN_LEVEL_MAX - 1

struct PositionCfg
{
	PositionCfg() : is_legal(false), hole_type(0), hole_level(0), grade(0), need_equip_level(0), need_godequip_level(0)
	{
		memset(attr_type, 0, sizeof(attr_type));
		memset(attr_add, 0, sizeof(attr_add));
	}
	
	const static int ATTR_NUM = 6;
	bool is_legal;
	int hole_type;
	int	hole_level;
	int grade;
	int	need_equip_level;// 修改为 装备等级
	int need_godequip_level;
	int	attr_type[ATTR_NUM];
	int	attr_add[ATTR_NUM];
	std::vector<ItemConfigData> consume_item_list;
};

struct PositionListCfg
{
	PositionListCfg() {}
	PositionCfg position_cfg[EQUIP_JEWELRY_TYPE_MAX][EQUIPMENT_JEWERLY_HOLE_LEVEL_MAX];
};

struct PositionSuitCfg
{
	PositionSuitCfg() {}
	const static int MAX_ATTR_NUM = 3;
	int hole_level_all;
	int	attr_type[MAX_ATTR_NUM];
	int	attr_add[MAX_ATTR_NUM];
	int sp_id;
};

struct EquipJewelryStrengthenCfg
{
	EquipJewelryStrengthenCfg():limit_prof(0),hole_type(0),grade(0)
	{
		memset(attr_type, 0, sizeof(attr_type));
		memset(attr_add, 0, sizeof(attr_add));
	}
	const static int ATTR_NUM = 6;
	int limit_prof;
	int	hole_type;
	int	grade;
	int	attr_type[ATTR_NUM];
	int	attr_add[ATTR_NUM];
	std::vector<ItemConfigData> consume_item_list;
};

struct PositionStrengthenListCfg
{
	PositionStrengthenListCfg() {}
	EquipJewelryStrengthenCfg position_cfg[EQUIP_JEWELRY_TYPE_MAX][EQUIPMENT_JEWERLY_HOLE_STRENGTHEN_LEVEL_MAX];
};

class EquipmentJewerlyUpgreadeConfig : public ILogicConfig
{
public:
	EquipmentJewerlyUpgreadeConfig();
	virtual ~EquipmentJewerlyUpgreadeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PositionCfg * GetPositionCfg(int profession, int hole_idx, int hole_level);
	const std::vector<PositionSuitCfg> * GetSuitListCfg(int profession);
	const EquipJewelryStrengthenCfg * GetStrengthenCfg(int profession, int hole_idx, int hole_grade);
private:
	int InitPositionCfg(TiXmlElement * RootElement);
	int InitSuitCfg(TiXmlElement * RootElement);
	int InitStrengthenCfg(TiXmlElement * RootElement);

	std::map<int, PositionListCfg> m_position_map;
	std::map<int, std::vector<PositionSuitCfg> > m_suit_map;
	std::map<int, PositionStrengthenListCfg> m_strengthen_map;
};

#endif // _EQUIPMENT_JEWERLY_UPGREADE_CONFIG_HPP_