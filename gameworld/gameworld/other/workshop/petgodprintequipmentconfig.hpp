#ifndef __PET_GOD_PRINT_EQUIPMENT_CONFIG_HPP__
#define __PET_GOD_PRINT_EQUIPMENT_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "equipment/petequipment.h"
#include "servercommon/workshopdef.h"

#include <map>
#include <set>
#include <vector>

struct PetGodPrintEquipMentOtherCfg
{
	PetGodPrintEquipMentOtherCfg() :need_coin_100_exp(0), exp_attenuation_100(0), exchange_item_id(0), free_num(0),
		exchange_get_item_id(0), exchange_get_item_id_1(0), swallow_gold_cost(0), pray_cost_item(0) {}

	int need_coin_100_exp;
	int	exp_attenuation_100;
	int exchange_item_id;
	int free_num;
	int exchange_get_item_id;
	int exchange_get_item_id_1;
	int swallow_gold_cost;
	int pray_cost_item;
};

struct PetGodPrintEquipMentBaseQualityCfg
{
	PetGodPrintEquipMentBaseQualityCfg() :main_attribute_group(0), deputy_attribute_num(0), deputy_attribute_att(0){}

	int	main_attribute_group;
	int	deputy_attribute_num;
	int deputy_attribute_att;
};

struct PetGodPrintEquipMentSuitCfg
{
	PetGodPrintEquipMentSuitCfg() : effect_type(0), param1(0), param2(0), score(0) {}
	enum EFFECT_TYPE
	{
		EFFECT_TYPE_NO = 0,		//无
		EFFECT_TYPE_ATTR = 1,	//属性  p1 属性类型 p2 属性数值
		EFFECT_TYPE_SP_ID = 2,	//特效	p1 1/2 (属性/战斗) p2 特效id
		EFFECT_TYPE_MAX,
	};
	int	effect_type;
	int	param1;
	int	param2;
	int score;
};

struct PetGodPrintEquipMentSuitListCfg
{
	PetGodPrintEquipMentSuitListCfg(){}

	PetGodPrintEquipMentSuitCfg suit_list[GOD_PRINT_INDEX_TYPE_MAX];
};

struct PetGodPrintEquipMentSuitListListCfg
{
	PetGodPrintEquipMentSuitListListCfg() {}
	PetGodPrintEquipMentSuitListCfg suilt_list[ItemBase::I_COLOR_MAX];
};

struct PetGodPrintEquipMentBaseAttrNumCfg
{
	PetGodPrintEquipMentBaseAttrNumCfg() :num(0), rate(0) {}
	int num;
	int rate;
};

struct PetGodPrintEquipMentBaseAttrNumGroupCfg
{
	PetGodPrintEquipMentBaseAttrNumGroupCfg() : weight(0) {}

	int weight;
	std::vector<PetGodPrintEquipMentBaseAttrNumCfg> m_attr_vec;
};

struct PetGodPrintdeputyAttrInfoCfg
{
	PetGodPrintdeputyAttrInfoCfg() : att_type(0), att_min(0), att_max(0) {}
	int att_type;
	int	att_min;
	int	att_max;
};

struct PetGodPrintdeputyAttrListCfg
{
	PetGodPrintdeputyAttrListCfg() {}
	
	std::vector<PetGodPrintdeputyAttrInfoCfg> m_attr_vec;
};

struct PetGodPrintMainAttrBaseCfg
{
	PetGodPrintMainAttrBaseCfg() : att_type(0), att_min(0), att_max(0), rate_10000(0) {}
	int att_type;
	int	att_min;
	int	att_max;
	int	rate_10000;
};

struct PetGodPrintMainAttrBaseGroupCfg
{
	PetGodPrintMainAttrBaseGroupCfg() : weight(0) {}

	int weight;
	std::vector<PetGodPrintMainAttrBaseCfg> m_main_attr_vec;
};

struct PetGodPrintIntensifyDetailCfg
{
	PetGodPrintIntensifyDetailCfg() : intensify_att_min(0), intensify_att_max(0) {}
	enum PET_GOD_PRINT_TYPE
	{
		PET_GOD_PRINT_TYPE_MAIN = 1,// 主属性
		PET_GOD_PRINT_TYPE_DEPUTY = 2,//副属性
	};
	int intensify_att_min;
	int	intensify_att_max;
};

struct PetGodPrintIntensifyInfoCfg
{
	PetGodPrintIntensifyInfoCfg() : level(0), exp(0), main_attribute_up(0), deputy_attribute_up(0) {}

	int level;
	int exp;
	int	main_attribute_up;
	int	deputy_attribute_up;
};

struct PetGodPrintIntensifyLevelCfg
{
	PetGodPrintIntensifyLevelCfg() {}

	std::vector<PetGodPrintIntensifyInfoCfg> m_intensify_vec;
};

struct PetGodPrintResolveExpCfg
{
	PetGodPrintResolveExpCfg() : item_exp(0) {}

	int item_exp;
};

struct PetGodPrintWishCfg
{
	PetGodPrintWishCfg() : pray_seq(0), item_group(0), expend_coin(0), expend_gold(0), rate_10000(0) {}
	int pray_seq;
	int	item_group;
	int	expend_coin;
	int expend_gold;
	int	rate_10000;
};

struct PetGodPrintWishRewardCfg
{
	PetGodPrintWishRewardCfg() : item_group(0), item_seq(0), rate_10000(0), god_print_item(0), is_broadcast(0) {}
	int item_group;
	int item_seq;
	int rate_10000;
	int god_print_item;
	int is_broadcast;
	std::vector<ItemConfigData> m_reward_vec;
};

struct PetGodPrintWishRewardGroupCfg
{
	PetGodPrintWishRewardGroupCfg() : weight(0){}
	int weight;
	std::vector<PetGodPrintWishRewardCfg> m_group_vec;
};

struct PetGodPrintWishExchangeCfg
{
	PetGodPrintWishExchangeCfg() :exchange_seq(0), expend(0), exchange_group(0) {}
	int exchange_seq;
	int	expend;
	int	exchange_group;
};

struct PetGodPrintWishExchangeItemCfg
{
	PetGodPrintWishExchangeItemCfg() :exchange_group_seq(0), rate_10000(0), is_broadcast(0) {}
	int exchange_group_seq;
	int	rate_10000;
	int is_broadcast;
	std::vector<ItemConfigData> reward_vec;
};

struct PetGodPrintWishExchangeGroupCfg
{
	PetGodPrintWishExchangeGroupCfg() :weight(0) {}
	int weight;
	std::vector<PetGodPrintWishExchangeItemCfg> m_group_vec;
};

struct GodPrintItemToExpenseItem
{
	GodPrintItemToExpenseItem() : pet_god_item_id(0), expense_item_id(0) {}
	int pet_god_item_id;
	int expense_item_id;
};

struct GodPrintIntensifyItemCfg
{
	GodPrintIntensifyItemCfg() : intensify_item_id(0), intensify_item_exp(0) {}
	int intensify_item_id;
	int intensify_item_exp;
};

class PetGodPrintEquipMentConfig : public ILogicConfig
{
public:
	PetGodPrintEquipMentConfig();
	virtual ~PetGodPrintEquipMentConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const PetGodPrintEquipMentOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const PetGodPrintEquipMentBaseQualityCfg * GetBaseCfg(int seq, int quality);
	const PetGodPrintEquipMentSuitCfg * GetSuitListCfg(int suit_id, int star_level, int suit_num);
	const PetGodPrintEquipMentBaseAttrNumCfg * RandDeputyAttrNum(int num_group);
	bool GetMaxDeputyAttrNum(int num_group, int* attr_max, int* attr_min);//获取改组最小词条数
	const PetGodPrintdeputyAttrInfoCfg * RandDeputyAttrBaseInfo(int seq, int attr_list[MAX_PET_GOD_PRONT_ATTR_NUM]);
	const PetGodPrintdeputyAttrInfoCfg * GetDeputyAttrBaseInfo(int seq, int attr_type);
	const PetGodPrintMainAttrBaseCfg * RandMainBaseAttrCfg(int group_id);
	const PetGodPrintIntensifyDetailCfg * GetIntensifyCfg(int god_print_type, int quality, int attr_type);
	const PetGodPrintIntensifyInfoCfg * GetIntensifyInfoCfg(int quality, int level);
	const PetGodPrintResolveExpCfg * GetResolveExpCfg(int quality);
	const PetGodPrintWishCfg * GetWishCfg(int seq);
	const PetGodPrintWishRewardCfg * RandRewardByGroup(int group_id);
	const PetGodPrintWishExchangeCfg * GetExchangeSeq(int seq);
	const PetGodPrintWishExchangeItemCfg * RandExchangeReward(int group_id);
	const int *GetExpenseIdByGodPrintId(int item_id);
	const int *GetGodPrintIdByExpenseId(int item_id);
	const GodPrintIntensifyItemCfg * GetGodPrintIntensifyItem(int item_id);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitBaseCfg(TiXmlElement * RootElement);
	int InitSuitCfg(TiXmlElement * RootElement);
	int InitDeputyAttrNumGroupCfg(TiXmlElement * RootElement);
	int InitDeputyAttrValueCfg(TiXmlElement * RootElement);
	int InitMainAttrBaseGroupCfg(TiXmlElement * RootElement);
	int InitAttrIntensiftCfg(TiXmlElement * RootElement);
	int InitAttrIntensiftInfoCfg(TiXmlElement * RootElement);
	int InitResolveExpCfg(TiXmlElement * RootElement);
	int InitWishCfg(TiXmlElement * RootElement);
	int InitWishGroupCfg(TiXmlElement * RootElement);
	int InitWishExchangeCfg(TiXmlElement * RootElement);
	int InitWishExchangeGroupCfg(TiXmlElement * RootElement);
	int InitGodPrintIdExchangeExpenseId(TiXmlElement * RootElement);
	int InitGodPrintIntensifyItemCfg(TiXmlElement * RootElement);

	PetGodPrintEquipMentOtherCfg m_other_cfg;
	PetGodPrintEquipMentBaseQualityCfg m_base_cfg[GOD_PRINT_INDEX_TYPE_MAX + 1][ItemBase::I_COLOR_MAX + 1];
	std::map<int, PetGodPrintEquipMentSuitListListCfg> m_suit_map;
	std::map<int, PetGodPrintEquipMentBaseAttrNumGroupCfg> m_base_attr_map;
	std::map<int, PetGodPrintdeputyAttrListCfg> m_deputy_attr_base_list;
	std::map<int, PetGodPrintMainAttrBaseGroupCfg> m_main_attr_map;
	PetGodPrintIntensifyDetailCfg m_intensify_cfg[PetGodPrintIntensifyDetailCfg::PET_GOD_PRINT_TYPE_DEPUTY][ItemBase::I_COLOR_MAX][BATTLE_ATTR_MAX];
	PetGodPrintIntensifyLevelCfg m_intensify_info_cfg[ItemBase::I_COLOR_MAX];
	PetGodPrintResolveExpCfg m_resolve_exp[ItemBase::I_COLOR_MAX];

	std::map<int, PetGodPrintWishCfg> m_wish_cfg;
	std::map<int, PetGodPrintWishRewardGroupCfg> m_wish_group_map;

	std::map<int, PetGodPrintWishExchangeCfg> m_wish_exchange_map;
	std::map<int, PetGodPrintWishExchangeGroupCfg> m_wish_exchange_group_map;

	std::vector<GodPrintItemToExpenseItem> m_exchange_item_id_vec;//主动消耗道具id-》对应神印真实道具
	int *m_god_print_item_id[MAX_UINT16];
	int *m_expense_item_id[MAX_UINT16];

	std::vector<GodPrintIntensifyItemCfg> m_god_print_intensify_vec;
};

#endif
