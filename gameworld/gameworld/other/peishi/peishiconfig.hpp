#ifndef __PEISHI_CONFIG_HPP__
#define __PEISHI_CONFIG_HPP__
#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include <map>
#include <vector>
#include "equipment/accessoriesment.h"

struct PeishiOtherCfg
{
	PeishiOtherCfg()
	{
		open_level = 0;
		entry_exp = 0;
		tianmingdan_id = 0;
		tianmingdan_exp = 0;
		feisheng_open = 0;
		need_feisheng = 0;
		displace_card = 0;
		displace_card_feisheng = 0;
	}
	int open_level;
	int	entry_exp;
	int	tianmingdan_id;
	int	tianmingdan_exp;
	int feisheng_open;
	int	need_feisheng;
	int	displace_card;
	int	displace_card_feisheng;
};

struct PeishiEntryCfg
{
	PeishiEntryCfg()
	{
		suit_type = 0;
		peishi_color = 0;
		seq = 0;
	}
	int suit_type;
	int	peishi_color;
	int	seq;
};

struct EntryRewardCfg
{
	EntryRewardCfg()
	{
		seq = 0;
		att_type = 0;
		att_min = 0;
		att_max = 0;
	}

	int seq;
	int	att_type;
	int	att_min;
	int	att_max;
};

struct PeishiSkillCfg
{
	PeishiSkillCfg()
	{
		suit_type = 0;
		min_color = 0;
		skill_1 = 0;
		skill1_profession = 0;
		skill_2 = 0;
		skill2_profession = 0;
		skill_level = 0;
	}
	int suit_type;
	int	min_color;
	int	skill_1;
	int	skill1_profession;
	int	skill_2;
	int	skill2_profession;
	int	skill_level;
};

struct ChuanChengCfg
{
	ChuanChengCfg()
	{
		entry_num = 0;
		chuancheng_entry_num = 0;
	}
	int entry_num;
	int	chuancheng_entry_num;
};

struct ChuanChengCostCfg
{
	ChuanChengCostCfg() : Material_level(0), Material_quality(0), coin(0) {}
	int Material_level;
	int	Material_quality;
	int	coin;
};

struct PeiShiStrengthenCfg
{
	PeiShiStrengthenCfg() :peishi_id(0), peishi_level(0), role_level(0), att_type_1(0), attr_value_1(0), att_type_2(0), attr_value_2(0) {}
	int peishi_id;
	int	peishi_level;
	int	role_level;
	int att_type_1;
	int	attr_value_1;
	int	att_type_2;
	int	attr_value_2;
	std::vector<ItemConfigData> cost_item_list;
};

struct PeiShiChangeCfg
{
	PeiShiChangeCfg() :displace_quality(0), entry(0), cost_num(0) {}
	int displace_quality;
	int	entry;
	int	cost_num;
};
const static int MAX_PEISHI_SPECIAL_EQUIP_ATTR_NUM = 6;//仙饰 - 基础属性最大限制
struct PeiShiSpecialEquipCfg
{
	PeiShiSpecialEquipCfg():type(SPECIAL_TYPE_NO),level(0),spec_value_2(0)
	{
		memset(attr_type_list, 0, sizeof(attr_type_list));
		memset(attr_value_list, 0, sizeof(attr_value_list));
	}
	enum SPECIAL_TYPE//千分比
	{
		SPECIAL_TYPE_NO = 0,
		SPECIAL_TYPE_1 = 1,//配饰强化属性加成
		SPECIAL_TYPE_2 = 2,	//铸灵石加成
		SPECIAL_TYPE_MAX,
	};
	int type;
	int level;
	int spec_value_2;

	int attr_type_list[MAX_PEISHI_SPECIAL_EQUIP_ATTR_NUM];
	int attr_value_list[MAX_PEISHI_SPECIAL_EQUIP_ATTR_NUM];

	std::vector<ItemConfigData> cost_vec;
};

const static int MAX_PEISHI_SEQ = 10;
const static int MAX_PEISHI_STRENGTHEN_LEVEL = 50;

class PeiShiConfig : public ILogicConfig
{
public:
	PeiShiConfig();
	virtual ~PeiShiConfig();
	virtual bool Init(const std::string &configname, std::string *err);
	const PeishiOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const PeishiEntryCfg * GetEntryCfg(int suit_type, int color);
	const EntryRewardCfg * RandEntryRewardCfg(int seq);
	const EntryRewardCfg * GetEntryRewardCfg(int seq, int attr_type);
	const PeishiSkillCfg * GetPeiShiSkillCfg(int suit_type, int color);
	const ChuanChengCfg * GetChuanChengCfg(int num);
	const ChuanChengCostCfg * GetChuanChengCostCfg(int level, int color);
	const PeiShiStrengthenCfg * GetPeiShiStrengthenCfg(int item_id, int level);
	const PeiShiChangeCfg * GetChangeCfg(int quality, int entry_num);
	const PeiShiSpecialEquipCfg * GetSpecialEquip(int type, int level);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitEntryCfg(TiXmlElement *RootElement);
	int InitEntryRewardCfg(TiXmlElement *RootElement);
	int InitPeishiSkillCfg(TiXmlElement *RootElement);
	int InitChuanChengCfg(TiXmlElement *RootElement);
	int InitChuanChengCostCfg(TiXmlElement *RootElement);
	int InitStrengthenCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitChangeCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitSpecialEquipCfg(TiXmlElement * RootElement, std::string& errormsg);

	PeishiOtherCfg m_other_cfg;
	PeishiEntryCfg * point_entry_cfg[ACCESSORIES_SUIT_TYPE_MAX][ItemBase::I_COLOR_MAX];
	std::vector<PeishiEntryCfg> m_entry_cfg;
	EntryRewardCfg * point_entry_reward_cfg[MAX_PEISHI_SEQ][BATTLE_ATTR_MAX];
	std::map<int, std::vector<EntryRewardCfg> > m_entry_reward_cfg;
	PeishiSkillCfg * point_peishi_skill_cfg[ACCESSORIES_SUIT_TYPE_MAX][ItemBase::I_COLOR_MAX];
	std::vector<PeishiSkillCfg> m_peishi_skill_cfg;

	std::vector<ChuanChengCfg> m_chuancheng_cfg;
	std::vector<ChuanChengCostCfg> m_chuancheng_cost_cfg;

	PeiShiStrengthenCfg * strengthen_level_item_list[MAX_UINT16][MAX_PEISHI_STRENGTHEN_LEVEL];
	std::vector<PeiShiStrengthenCfg> m_strengthen_cfg;

	PeiShiChangeCfg * change_list[ItemBase::I_COLOR_MAX][MAX_ACCESSORIES_VOC_NUM];
	std::vector<PeiShiChangeCfg> m_change_cfg;

	std::map<int, std::map<int, PeiShiSpecialEquipCfg> > special_equip_map;
};

#endif // !__PRESTIGE_CONFIG_HPP__
