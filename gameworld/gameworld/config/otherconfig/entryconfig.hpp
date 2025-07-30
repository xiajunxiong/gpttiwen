#ifndef __ENTRY_CONFIG_HPP__
#define __ENTRY_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "gameworld/item/itembase.h"
#include <vector>
#include <map>
#include <set>

struct NumGroupCfg
{
	NumGroupCfg() : num_group(0), num(0), rate(0) {}
	int num_group;
	int num;
	int rate;
};

struct NumGroupListCfg
{
	NumGroupListCfg() :weight_count(0) {}
	int weight_count;
	std::vector<NumGroupCfg> mun_group_vec;
};

struct RandGroupCfg
{
	RandGroupCfg() : att_group(0), att_type(-1), quality(0), rate(0) {}
	int att_group;
	int att_type;
	int quality;
	int rate;
};

struct RandGroupListCfg
{
	RandGroupListCfg() : weight_count(0) {}
	int weight_count;
	std::vector<RandGroupCfg> rand_group_vec;
};

struct AttValueCfg
{
	AttValueCfg() : level(0), quality(0), att_type(0), max(0), min(0) {}
	int level;
	int quality;
	int att_type;
	int max;
	int min;
};

struct MaxAttrNumCfg
{
	MaxAttrNumCfg() : level(0), role_level(0), quality(0), max_num(0) {}
	int level;
	int role_level;
	int quality;
	int max_num;
};

struct AttValueListCfg
{
	AttValueListCfg() {}
	AttValueCfg m_att_list[BATTLE_ATTR_MAX];
};

struct WorkshopLockPrice
{
	WorkshopLockPrice() : level(0), gold(0), coin(0), ex_coin(0) {}

	int level;
	int gold;
	int coin;
	int ex_coin;
};

struct WorkshopSmeltPrice
{
	WorkshopSmeltPrice() : smelt_price(0), price_type(0) {}

	int smelt_price;
	int price_type;
};

struct GodEquipAttrNumConfig
{
	GodEquipAttrNumConfig() : max_num(0), role_level(0) {}
	int max_num;
	int role_level;
};

struct GodEquipActiveAttrNumConfig
{
	GodEquipActiveAttrNumConfig() : godequip_level(0), role_level(0) {}
	int godequip_level;
	int role_level;
};

struct QuenChingExpendCfg
{
	QuenChingExpendCfg() : equip_num_quality(0), item_id(0), consume_num(0) {}
	int equip_num_quality;
	int	item_id;
	int	consume_num;

};

struct  QuenChingAddCfg
{
	QuenChingAddCfg() : equip_num_quality(0), att_type(0), once_add(0) {}
	int equip_num_quality;
	int	att_type;
	int	once_add;
};

static const int MAX_EQUIP_LOCK_NUM = 6;

struct EntryOtherConfig
{
	EntryOtherConfig() : coin_smelt_times(0) {}
	int coin_smelt_times;
};

class EntryConfig : public ILogicConfig
{
public:
	EntryConfig();
	virtual ~EntryConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetEntryNumConfig(int group_id);
	int GetEntryRandConfig(int group_id, int entry_num, RandGroupCfg entry_type[EquipmentParam::EXA_ATTR_NUM_MAX], int entry_aw[EquipmentParam::EXA_ATTR_NUM_MAX] = NULL);
	bool EntryGroupHasAttrTypr(int group_id, int attr_type);
	const AttValueCfg * GetEntryAttrValueConfig(int level, int quality, int attr_type);
	const AttValueCfg * GetEntryAttrValueConfig2(int level, int quality, int attr_type);
	const AttValueCfg * GetEntryAttrValueConfig3(int level, int quality, int attr_type);
	const AttValueCfg * GetEntryAttrValueConfig4(int level, int quality, int attr_type);

	//词条反向查询
	const AttValueCfg * GetEntryAttrValueConfigBack(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfigBack2(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfigBack3(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfigBack4(int level, int attr_type, int attr_value);

	const AttValueCfg * GetEntryAttrValueConfigByLevelAndTypeAndValue(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfig2ByLevelAndTypeAndValue(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfig3ByLevelAndTypeAndValue(int level, int attr_type, int attr_value);
	const AttValueCfg * GetEntryAttrValueConfig4ByLevelAndTypeAndValue(int level, int attr_type, int attr_value);

	const MaxAttrNumCfg * GetEntryAttrMaxNumConfig(int level, int quality);

	const GodEquipAttrNumConfig* GetGodEquipmentNumCfg(int strengthen_level, int quality);
	const GodEquipActiveAttrNumConfig* GetGodEquipmentAttrVocCfg(int quality, int voc_idx);
	const WorkshopLockPrice * GetLockPriceCfg(int lock_num);
	const WorkshopSmeltPrice * GetSmeltPriceCfg() { return &smelt; }
	const QuenChingExpendCfg * GetQuenChingExpendCfg(int voc_color);
	const QuenChingAddCfg * GetQuenChingAddCfg(int voc_color, int voc_attr_type);
	const EntryOtherConfig & GetEntryOtherCfg()const { return m_other_cfg; }
private:
	int InitOtherConfig(TiXmlElement* RootElement);
	int InitNumGroupConfig(TiXmlElement* RootElement);
	int InitRandGroupConfig(TiXmlElement* RootElement);
	int InitAttValueConfig(TiXmlElement* RootElement);
	int InitAttValueConfig2(TiXmlElement* RootElement);
	int InitAttValueConfig3(TiXmlElement* RootElement);
	int InitAttValueConfig4(TiXmlElement* RootElement);
	int InitMaxNumConfg(TiXmlElement* RootElement);
	int InitLockPriceCfg(TiXmlElement * RootElement);
	int InitSmeltPriceCfg(TiXmlElement * RootElement);
	int InitGodEquipCfg(TiXmlElement * RootElement);
	int InitGodEquipVocActiveCfg(TiXmlElement * RootElement);
	int InitGodEquipVocUpLevelExpendCfg(TiXmlElement * RootElement);
	int InitGodEquipVocUpLevelAddCfg(TiXmlElement * RootElement);

	int GetAttValueKeyByLevelAndQuality(int level, int quality) { return level * 10000 + quality; }

	EntryOtherConfig m_other_cfg;

	std::map<int, NumGroupListCfg> m_num_group_config;
	std::map<int, RandGroupListCfg> m_rand_group_config;

	std::map<int, AttValueListCfg> m_attvalue_config;		//装备词条
	std::map<int, AttValueListCfg> m_attvalue_config2;		//灵饰词条

	std::map<int, AttValueListCfg> m_attvalue_config3;		//(新)装备词条
	std::map<int, AttValueListCfg> m_attvalue_config4;		//(新)灵饰词条2

	std::map<int, MaxAttrNumCfg> m_max_attr_num_config;
	std::map<int, std::map<int, GodEquipAttrNumConfig> > m_god_equip_cfg;		//神装词条 key1:quality key2:god_equip_level second:词条数量
	GodEquipActiveAttrNumConfig m_god_equip_voc_active_cfg[ItemBase::I_COLOR_MAX][EquipmentParam::ATTR_NUM_MAX];
	WorkshopLockPrice lock_list[MAX_EQUIP_LOCK_NUM];
	WorkshopSmeltPrice smelt;
	QuenChingExpendCfg m_god_equip_expend[ItemBase::I_COLOR_MAX];
	QuenChingAddCfg* m_god_equip_add_config[ItemBase::I_COLOR_MAX][BATTLE_ATTR_MAX];
	std::vector<QuenChingAddCfg> m_add_cfg_vec;
};

#endif