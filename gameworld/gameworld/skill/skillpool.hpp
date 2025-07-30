#ifndef __SKILL_POOL_HPP__
#define __SKILL_POOL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/configcommon.h"

#include <map>

static const int MAX_LIFE_SKILL_LEVEL = 10;

struct LifeSkillCfg
{
	LifeSkillCfg() : life_skill_id(0), life_skill_type(-1), life_skill_class(0), max_level(0) {}

	struct SkillParam
	{
		SkillParam() : upgrade_need_exp(0), param1(0), param2(0), param3(0), param4(0) {}

		int upgrade_need_exp;

		int param1;
		int param2;
		int param3;
		int param4;
	};

	int GetUpgradeNeedExp(int cur_level) const
	{
		if (cur_level < 0 || cur_level >= max_level || cur_level >= MAX_LIFE_SKILL_LEVEL)
		{
			return MAX_INT;
		}

		return skill_param[cur_level].upgrade_need_exp;
	}

	UInt16 life_skill_id;							//!< 生活技能id，只与职业可学技能配置联用
	UInt16 life_skill_type;							//!< 生活技能类型 \see LIFE_SKILL_TYPE
	UInt16 life_skill_class;						//!< 生活技能种类 \see LIFE_SKILL_CLASS

	UInt16 max_level;								//!< 最大等级
	SkillParam skill_param[MAX_LIFE_SKILL_LEVEL + 1]; //!< 每升一级属性
};

struct SkillLevelCfg
{
	SkillLevelCfg() : uplevel_need_exp(0), add_exp(0), bind_coin_consume(0), skill_learn_level(0), coin_consume(0)
	{}

	int uplevel_need_exp;
	int add_exp;
	int bind_coin_consume;
	int skill_learn_level;
	int coin_consume;
};

struct SkillCfg
{
	SkillCfg() : skill_id(INVALID_BATTLE_SKILL_ID), max_level(0) {}

	int GetUplevelNeedExp(int _level) const;
	int GetAddExp(int _level) const;
	int GetBindCoinConsume(int _level) const;
	int GetCoinConsume(int _level) const;
	long long GetCoinConsumeCount(int cur_level, int to_level) const;
	int GetRoleLevelLimit(int _level) const;
	int GetMaxLevel() const;
	int GetMaxLevelForRoleLevel(int role_level) const;
	const SkillLevelCfg* GetLevelCfg(int _level) const;

	UInt16 skill_id;
	int max_level;

	SkillLevelCfg level_cfg_list[MAX_BATTLE_SKILL_LEVEL + 1];
};

struct MakeEquipExpInfo
{
	int equip_level;
	int add_exp;
};

struct LifeSkillOtherCfg
{
	LifeSkillOtherCfg() : dig_add_exp(0), lumber_add_exp(0), pick_flower_add_exp(0), make_equip_add_exp_list_size(0)
	{
		memset(make_equip_add_exp_list, 0, sizeof(make_equip_add_exp_list));
	}

	int MakeEquipAddExp(int equip_level) const
	{
		for (int i = 0; i < make_equip_add_exp_list_size && i < MAKE_EQUIP_ADD_EXP_LIST_SIZE; ++i)
		{
			if (equip_level == make_equip_add_exp_list[i].equip_level)
			{
				return make_equip_add_exp_list[i].add_exp;
			}
		}

		return 0;
	}

	int dig_add_exp;
	int lumber_add_exp;
	int pick_flower_add_exp;

	static const int  MAKE_EQUIP_ADD_EXP_LIST_SIZE = 20;
	int make_equip_add_exp_list_size;
	MakeEquipExpInfo make_equip_add_exp_list[MAKE_EQUIP_ADD_EXP_LIST_SIZE];
};

enum GAME_PASSIVE_SKILL_PRIORITY_TYPE
{
	GAME_PASSIVE_SKILL_PRIORITY_TYPE_INVALID = -1,
	GAME_PASSIVE_SKILL_PRIORITY_TYPE_COMMMON = 0,		//普通
	GAME_PASSIVE_SKILL_PRIORITY_TYPE_HIGH = 1,			//高级
	GAME_PASSIVE_SKILL_PRIORITY_TYPE_SUPER = 2,			//超级

	GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX
};

struct GamePassiveSkillCfg
{
	static const int MAX_ELEMENTAL_SKILL_TYPE = 99;

	bool IsElementalSkillType() const
	{
		if (skill_type <= MAX_ELEMENTAL_SKILL_TYPE)
		{
			return true;
		}

		return false;
	}

	GamePassiveSkillCfg() : skill_id(0), sp_id(0), skill_type(0), skill_priority(0), is_level_spid(0)
	{
		memset(level_spid_list, 0, sizeof(level_spid_list));
	}

	int skill_id;
	int sp_id;
	int skill_type;
	int skill_priority;

	bool is_level_spid;
	int level_spid_list[MAX_BATTLE_SKILL_LEVEL + 1];
};

class SkillPool
{
public:
	static SkillPool& Instance();

	bool Init(const std::string &skill_dir_name, std::string *err);
	static bool Reload(const std::string &skill_dir_name, std::string *err);

	bool RandAttr(int life_skill_type, int skill_lv, std::vector<AttrInfo> & out_list);

	const SkillCfg * GetSkillCfg(UInt16 skill_id);
	const LifeSkillCfg * GetLifeSkillCfg(int life_skill_type);
	const LifeSkillCfg * GetLifeSkillById(UInt16 life_skill_id);
	const LifeSkillOtherCfg & GetLifeSkillOtherCfg() { return m_life_skill_other_cfg; }
	const GamePassiveSkillCfg* GetPassiveSkillCfg(short skill_id);
	long long GetSkillGoBackRetCoin(int skill_id, int skill_level, int cur_level_exp, int profession, int advance_sub_level, int go_back_level = 1);
private:
	bool InitSkill(const std::string &config_name, std::string *err, short skill_type);
	bool InitLifeSkill(const std::string& config_name, std::string* err);
	bool InitPassiveSkill(const std::string &config_name, std::string *err);
	int InitPassiveSkillLevelCfg(TiXmlElement *RootElement);

	int InitSkillListCfg(TiXmlElement *RootElement, short skill_type, std::string *err);
	int InitLifeSkillCfg(TiXmlElement *RootElement);
	int InitLifeSkillOtherCfg(TiXmlElement *RootElement);
	int InitMakeEquipExpCfg(TiXmlElement* RootElement);
	int InitAttrTypeRandGroup(TiXmlElement* RootElement);
	int InitAttrSegeRandGroup(TiXmlElement* RootElement);
	int InitAttrNumbRandGroup(TiXmlElement* RootElement);
	int InitPassiveSkillCfg(TiXmlElement *RootElement);

	bool RandSingleAttr(int group_id, int skill_lv, AttrInfo * out_info);

	SkillCfg * m_skill_list[MAX_BATTLE_SKILL_ID];
	LifeSkillCfg m_life_skill_list[LIFE_SKILL_TYPE_MAX];
	LifeSkillOtherCfg m_life_skill_other_cfg;

	std::map<int, std::map<int, int> > m_attr_numb_rand_map;			// 键-值：组ID、数量、概率
	static const int RAND_GROUP_NUM = 10;
	int m_attr_type_rand_map[RAND_GROUP_NUM][ADD_POINT_TYPE_MAX];
	std::map<int, std::map<int, AttrSegment> > m_attr_segement_map;		// 键-值：技能等级、属性类型、属性区间

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(SkillPool);

	typedef std::map<int, GamePassiveSkillCfg> GamePassiveSkillCfgMap;
	GamePassiveSkillCfgMap m_passive_skill_cfg_map;
};

#endif