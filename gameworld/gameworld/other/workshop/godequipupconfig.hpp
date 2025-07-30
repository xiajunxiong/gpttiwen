#ifndef __GOD_EQUIP_CONFIG_HPP__
#define __GOD_EQUIP_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/workshopdef.h"

#include <map>
#include <vector>

const static int MAX_GOD_STRENGTHEN_ATTR_LIST = 6;//强化属性列表最大种类

struct StrengthenCfg
{
	StrengthenCfg() : id(0), level(0), need_rolelevel(0), equip_type(0), color(0), limit_prof(0), suit_id(0), strengthen_id(0)
	{
		memset(attr_type_list, 0, sizeof(attr_type_list));
		memset(attr_add_list, 0, sizeof(attr_add_list));
	}
	int id;
	int	level;
	int	need_rolelevel;
	int	equip_type;
	int color;
	int	limit_prof;
	int	attr_type_list[MAX_GOD_STRENGTHEN_ATTR_LIST];
	int	attr_add_list[MAX_GOD_STRENGTHEN_ATTR_LIST];
	int suit_id;
	int strengthen_id;
	std::vector<ItemConfigData> consume_item_list;
	std::vector<ItemConfigData> consume_item_list_2;
};

struct GodEquipTurnBackNode
{
	GodEquipTurnBackNode() {}
	int uid;
	int back_item;
	std::vector<ItemConfigData> return_item_list;
};

struct GodEquipTurnBackListCfg
{
	GodEquipTurnBackListCfg() {}
	int uid;
	std::vector<GodEquipTurnBackNode> back_item_vec;
};

struct GodEquipResonanceCfg
{
	GodEquipResonanceCfg() {}
	int limit_prof;
	int	level;
	int	addition_level;
	int attr_type_1;
	int	attr_add_1;
	int	attr_type_2;
	int	attr_add_2;
};

struct GodEquipOtherCfg
{
	GodEquipOtherCfg():broadcast_interval(0), start_broadcast_time(0), world_rank(0)
	{}

	int broadcast_interval;
	int start_broadcast_time;
	int world_rank;			// 世界等级高于这个值不再发公告
};

#define GODEQUIP_CONFIG GodEquipUpConfig::Instance()

class GodEquipUpConfig
{
public:
	GodEquipUpConfig();
	virtual ~GodEquipUpConfig();
	
	static GodEquipUpConfig * Instance();

	bool Init(const std::string &configname, std::string *err);
	bool Reload(const std::string &configname, std::string *err);

	const StrengthenCfg *GetStrengthenCfg(ItemID item_id,int level);
	const StrengthenCfg *GetStrengthenByJob(int job,int level, int color, int equip_type);//返回的是前一阶的属性
	const StrengthenCfg *GetStrengBackCfg(ItemID item_id, int level);
	const GodEquipTurnBackListCfg * GetTurnBackUserId(int uid);
	const GodEquipTurnBackListCfg * GetTurnBackUserId2(int uid);
	const GodEquipResonanceCfg * GetResonanceCfg(int prof, int level);

	const ItemID RandGodEquipId()const;
	const GodEquipOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitGodEquipUpCfg(TiXmlElement * RootElement);
	int InitGodJewelryUpCfg(TiXmlElement * RootElement);
	int InitGodItemTurnBackCfg(TiXmlElement * RootElement);
	int InitGodItemTurnBack2Cfg(TiXmlElement * RootElement);
	int InitGodEquipResonanceCfg(TiXmlElement * RootElement);
	int InitGodEquipBroadCastCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	StrengthenCfg * item_point[MAX_UINT16][MAX_GOD_EQUIP_MAX_LEVEL + 1];
	StrengthenCfg * job_point[PROFESSION_TYPE_NUM][MAX_GOD_EQUIP_MAX_LEVEL + 1][ItemBase::I_COLOR_MAX][ULTIMATE_EQUIP_TYPE_MAX];
	StrengthenCfg * turn_back_item_point[MAX_UINT16][MAX_GOD_EQUIP_MAX_LEVEL + 1];//神装倒退,物品id为I_COLOR_GOD品质的对应，指向为品质为I_COLOR_STAR的配置
	std::map<int ,GodEquipTurnBackListCfg> turn_back_user;
	std::map<int, GodEquipTurnBackListCfg> turn_back_user2;
	std::vector<StrengthenCfg> m_strengthen_vec;

	GodEquipResonanceCfg * resonance_list[PROFESSION_TYPE_NUM][MAX_GOD_EQUIP_MAX_LEVEL + 1];
	std::vector<GodEquipResonanceCfg> m_resonance_vec;

	std::vector<ItemID> m_broadcast_list;
	GodEquipOtherCfg m_other_cfg;
};

#endif