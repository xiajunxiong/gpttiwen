#ifndef __WORKSHOP_CONFIG_HPP__
#define __WORKSHOP_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/workshopdef.h"
#include "equipment/equipment.h"
#include "equipment/jewelry.h"

#include <map>
#include <set>

static const int FAILURE_RETURN_NUM_MAX = 2;		// 失败返还道具最大数
static const int COMPOUND_MATE_KIND_MAX = 5;		// 合成材料种类最大数
UNSTD_STATIC_CHECK(COMPOUND_MATE_KIND_MAX < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM);

struct MaterialCon		// 合成材料容器
{
	MaterialCon() : mate_id(0), can_return(WORKSHOP_RETURN_NONE), need_num(0), can_decompose(false), min_value(0), max_value(0) {}

	ItemID mate_id;			// 材料 ID
	short can_return;		// 失败是否返还
	int need_num;			// 合成所需数量

	bool can_decompose;		// 分解是否返回
	int min_value;
	int max_value;
};

struct WorkshopItemCompound		// 物品合成表 - 键值对 值，键 是 合成ID
{
	WorkshopItemCompound(const ItemID & p_comp_id = 0) : composition(p_comp_id), fortifier_id(0),
		level_limit(0), active_sign(0), need_coin(0), craft_need(0), succ_prob(0), resolve_return_lingyu(0) {}

	ItemID composition;				// 合成物
	ItemID fortifier_id;			// 强化打造石
	int level_limit;				// 等级限制
	int active_sign;				// 激活码
	int need_coin;					// 合成所需铜币
	int craft_need;					// 进行强化打造时，需要的打造石数量
	int succ_prob;					// 合成成功率

	ItemPairCon fail_return_item[FAILURE_RETURN_NUM_MAX];	// 失败返还道具
	MaterialCon need_mate[COMPOUND_MATE_KIND_MAX];			// 合成所需材料

	int resolve_return_lingyu;
};

struct WorkshopCraftAttrAdd		// 装备强化打造属性提升表 - 键值对 值，键是装备ID
{
	WorkshopCraftAttrAdd() : effect_prob_inc(0)
	{
		memset(min_val_instead, 0, sizeof(min_val_instead));
	}

	int effect_prob_inc;									// 特效出现概率提升
	int min_val_instead[Equipment::EQUIP_ATTR_IDX_MAX];		// 属性最小值替换值
};

struct WorkshopItemExchange		// 物品兑换表
{
	WorkshopItemExchange() : exchange_type(0), param1(0), param2(0), need_num(0), give_num(0), succ_rate(0), fail_return_item(0), fail_return_num(0)
	{}

	int exchange_type;		// 兑换类型	- 0:技能书兑换	1:封印卡兑换 2:法宝碎片分解 3:银钥匙合成  4:金钥匙合成 5:超级洗髓露合成
	int param1;				// 参数1		- 0:兑换物品组	1:材料物品ID
	int param2;				// 参数2		- 0:结果物品组	1:结果物品ID
	int need_num;			// 需求数量
	int give_num;			// 兑换数量
	int succ_rate;			// 成功几率
	int need_coin;			// 所需铜币
	int fail_return_item;	// 失败返还道具
	int fail_return_num;	// 失败返还数量
};

struct WorkshopExRate
{
	WorkshopExRate(const ItemID & _item_id = 0) : item_id(_item_id), ex_rate(0) {}

	ItemID item_id;
	UInt16 ex_rate;

	bool operator == (const WorkshopExRate & cobj) const
	{
		return this->item_id == cobj.item_id;
	}

	bool operator < (const WorkshopExRate & cobj) const
	{
		return this->item_id < cobj.item_id;
	}
};

struct WorkshopUltimateCfg
{
	WorkshopUltimateCfg() :equip_type(0), color(0), limit_level(0) {}

	int equip_type;
	int	color;
	int	limit_level;
	
	std::vector<ItemConfigData> common_consume_item;
	std::vector<ItemConfigData> added_consume_item;
};

struct WorkShopUltimateOtherCfg
{
	WorkShopUltimateOtherCfg() : jg_extent_min(0), jg_extent_max(0), rate(0){}

	int jg_extent_min;
	int	jg_extent_max;
	int	rate;
};

struct WorkShopUltimateOtherList
{
	WorkShopUltimateOtherList() : weight_count(0) {}

	int weight_count;
	WorkShopUltimateOtherCfg value_ultimate[5];
};

struct WorlshopGodEquipReturn
{
	WorlshopGodEquipReturn() : color(0), level(0), reset(0) {}

	int color;
	int	level;
	int	reset;
	std::vector<ItemConfigData> turn_back_reward_vec;
};

class WorkshopConfig : public ILogicConfig
{
public:
	WorkshopConfig();
	virtual ~WorkshopConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WorkshopItemCompound * GetItemCompoundCfg(int comp_seq);
	const WorkshopItemCompound* GetItemCompoundCfgByItemId(ItemID _item_id);
	const WorkshopCraftAttrAdd * GetCraftAttrAddCfg(int equip_id);
	const WorkshopItemExchange * GetItemExchangeCfg(int exch_seq);

	const ItemPairCon * GetFastComposeGemCon(int gemstone_id);
	bool IsItemBelongsToGroup(int group_id, int item_id);
	const ItemID RandAExchangeItem(int group_id);
	const WorkshopUltimateCfg * GetUltimateCfg(int equip_type, int color, int limit_level);
	const WorkShopUltimateOtherCfg * GetRandUlitmateCfg();
	const WorlshopGodEquipReturn * GetGodEquipReturnCfg(int color, int level);
	int GetNowGodEquipReturnVersion() { return god_equip_version_id; }
private:
	int InitCompoundList(TiXmlElement * RootElement);
	int InitCraftAttrAdd(TiXmlElement * RootElement);
	int InitItemExchange(TiXmlElement * RootElement);
	int InitConvertGroup(TiXmlElement * RootElement);
	int InitUltimateConsume(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement * RootElement);
	int InitGodEquipReturn(TiXmlElement *RootElement);

	std::map<int, WorkshopItemCompound> m_compound_items_map;	// 物品合成表
	std::map<int, WorkshopItemCompound> m_target_to_wiccfg;     // 通过物品id获得物品合成表
	std::map<int, WorkshopCraftAttrAdd> m_craft_attr_add_map;	// 强化打造表
	std::map<int, WorkshopItemExchange> m_exchange_props_map;	// 道具兑换表
	std::map<int, std::set<WorkshopExRate> > m_convert_group_map;	// 兑换组配置

	std::map<int, ItemPairCon> m_gemstone_compose_search_map;	// 宝石快速合成查询映射

	WorkshopUltimateCfg m_ultimate_consume[ULTIMATE_EQUIP_TYPE_MAX][ItemBase::I_COLOR_MAX][MAX_ROLE_LEVEL + 1];//精工消耗-装备类型，装备品质，装备等级
	WorkShopUltimateOtherList m_ultimate_weight;//精工区间
	int god_equip_version_id;
	std::vector<WorlshopGodEquipReturn> m_god_equip_return_vec;//神装返还
};

#endif