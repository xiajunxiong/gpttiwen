#ifndef __GEM_STONE_CONFIG_HPP__
#define __GEM_STONE_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"

#include <map>
#include <set>
#include <vector>

static const int GEM_ATTR_COUNT_MAX = 2;		// 宝石属性条数最大值

struct GemstoneOtherInfoCfg
{
	GemstoneOtherInfoCfg() : compose_price_once(0) {}

	int compose_price_once;
};

struct GemstoneBasicInfoCfg 
{
	GemstoneBasicInfoCfg() : gemstone_type(0) , level(0), level_limit(0), godequip_level(0){}

	int gemstone_type;
	int level;
	int level_limit;
	int godequip_level;
	BattleAttrPair attr_add[GEM_ATTR_COUNT_MAX];
};

struct SlotOpenConfig
{
	SlotOpenConfig() :equiplimit_lv(0), godequiplimit_lv(0) {}

	int equiplimit_lv;
	int godequiplimit_lv;

};

enum GemstoneType
{										
	GEMSTONE_TYPE_HELMET = 0,		// 头盔
	GEMSTONE_TYPE_ARMOUR,			// 盔甲
	GEMSTONE_TYPE_WEAPON,			// 武器
	GEMSTONE_TYPE_SHIELD,			// 盾牌
	GEMSTONE_TYPE_SHOE,				// 鞋子
	GEMSTONE_TYPE_RING,				// 戒指
	GEMSTONE_TYPE_BRACELET,			// 手镯
	GEMSTONE_TYPE_PENDANTS,			// 坠饰
	GEMSTONE_TYPE_MAX,
};

class GemstoneConfig : public ILogicConfig
{
public:
	GemstoneConfig();
	virtual ~GemstoneConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool CheckGemstoneCanMount(int equip_type, int gem_type);		// 判断宝石是否符合装备类型
	bool CheckGemMountSlotOpen(int slot_id, int equip_limit, bool isgodequip );			// 判断宝石镶嵌槽是否开启
	int GetGemstoneItemID(int gem_type, int gem_level);				// 根据宝石类型和等级获取对应宝石的道具ID

	int GetGemstoneByType(int equip_type,int gem_type);

	const GemstoneOtherInfoCfg & GetGemstoneOtherInfoCfg() { return m_gemstone_other_cfg; }
	const GemstoneBasicInfoCfg * GetGemstoneBasicInfoCfg(int gem_item_id);

private:
	int InitGemstoneOtherCfg(TiXmlElement * RootElement);
	int InitGemstoneAttrsAdd(TiXmlElement * RootElement);
	int InitGemstoneCanMount(TiXmlElement * RootElement);
	int InitGemstoneSlotOpen(TiXmlElement * RootElement);

	GemstoneOtherInfoCfg m_gemstone_other_cfg;

	std::map<int, GemstoneBasicInfoCfg> m_gem_attrs_cfg;		// 宝石属性配置 - 键 宝石ID
	std::set<int> m_can_mount_cfg[GEMSTONE_TYPE_MAX];			// 装备可镶嵌宝石类型配置 - 键 装备类型
	std::map<int, SlotOpenConfig> m_slot_open_cfg;			// 镶嵌孔开放等级信息配置 - 键 镶嵌孔ID；值 装备等级限制||神装阶数限制

	std::map<int, std::map<int, ItemID> > m_gemstone_map;		// 宝石 类型 -> 等级 -> 道具ID 映射表
};

#endif