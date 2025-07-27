#ifndef __BATTLE_SPECIAL_EFFECT_POOL_HPP__
#define __BATTLE_SPECIAL_EFFECT_POOL_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_halo_def.hpp"
#include <map>

static const int EFFECT_DETAIL_NUM = 5;
static const int EFFECT_DETAIL_PARAM_NUM = 4;
static const int SPECIAL_EFFECT_MAX_NUM = 15000;

struct SpecialEffectKey
{
	SpecialEffectKey() : effect_type(0), effect_id(0) {}

	bool operator < (const SpecialEffectKey& _key) const
	{
		if (effect_type != _key.effect_type)
		{
			return effect_type < _key.effect_type;
		}

		return effect_id < _key.effect_id;
	}

	short effect_type;
	short effect_id;
};

struct SpecialEffectCfg
{
	SpecialEffectCfg() : sp_id(0), effect_num(0), max_trigger_times(-1), invalid_on_silent(false), 
		is_heart_skill(false), is_jieyi(false), jieyi_only_pve_active(false), is_show_on_list(false)
	{
		sp_type = 0;
		sp_priority = 0;
		prof_type = 0;
		medal_grade = 0;
	}

	int sp_id;
	short effect_num;
	short max_trigger_times;
	SpecialEffectKey effect_detail_list[EFFECT_DETAIL_NUM];
	int sp_type;
	int sp_priority;
	int prof_type;		// 职业类型[0,8]
	int medal_grade;
	bool invalid_on_silent;
	bool is_heart_skill;	// 是否是心法技能
	bool is_jieyi;
	bool jieyi_only_pve_active; // 结义只PVE生效
	bool is_qingyuan;
	bool is_show_on_list;
};

struct EffectDetailCfg
{
	EffectDetailCfg() : effect_id(0), timing(0), effect_sub_type(0), param_num(0),
		cond_type(0), cond_param1(0), cond_param2(0), cond_param3(0), cond_param4(0), valid_round(0)
	{
		memset(param_list, 0, sizeof(param_list));
	}

	short effect_id;
	short timing; // 只有基于触发时机的特效才有该字段
	short effect_sub_type; 
	short param_num;
	int param_list[EFFECT_DETAIL_PARAM_NUM];

	int cond_type;		// 暂时只有技能命中特效有触发条件
	int cond_param1;
	int cond_param2;
	int cond_param3;
	int cond_param4;

	int valid_round;	//有效回合数(0:则表示不受限制)(基于时机触发才有该字段)
};

struct EffectGroupCfg
{
	struct EffectPair
	{
		EffectPair() : sp_id(0), rate(0) {}

		int sp_id;
		int rate;
	};

	void AddEffect(int sp_id, int rate)
	{
		if (effect_num >= MAX_EFFECT_NUM_IN_ONE_GROUP) return;

		effect_list[effect_num].sp_id = sp_id;
		effect_list[effect_num].rate = total_rate + rate;

		total_rate += rate;
		effect_num += 1;
	}

	static const int MAX_EFFECT_NUM_IN_ONE_GROUP = 50;

	EffectGroupCfg() : group_id(0), total_rate(0), effect_num(0) {}

	int group_id;
	int total_rate;
	
	int effect_num;
	EffectPair effect_list[MAX_EFFECT_NUM_IN_ONE_GROUP];
};

struct BattleHaloCfg
{
	int halo_id;
	int effect_type;
	int range_type;
	int target_side;
	HaloEffectParam param;
};

struct JieyiOriginSpid
{
	bool operator < (const JieyiOriginSpid& other) const
	{
		if (origin_spid != other.origin_spid)
		{
			return origin_spid < other.origin_spid;
		}
		else
		{
			return jieyi_num < other.jieyi_num;
		}
	}

	int origin_spid;
	int jieyi_num;
};

class BattleSpecialEffectPool
{
public:
	typedef std::map<SpecialEffectKey, EffectDetailCfg> EffectDetailMap;
	typedef std::map<int, EffectGroupCfg> EffectGroupMap;
	typedef std::map<int, BattleHaloCfg> HaloCfgMap;
	typedef std::map<JieyiOriginSpid, int> JieyiSpidHash;
	typedef std::map<int, std::vector<int> > OnlyEffectOnPerformSkillMap; // <sp_id, skill_id_list>
public:
	static BattleSpecialEffectPool& GetInstance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	const SpecialEffectCfg* GetSpecialEffectCfg(int sp_id);
	const EffectDetailCfg* GetEffectDetailCfg(const SpecialEffectKey& key);
	const EffectGroupCfg* GetEffectGroupCfg(int group_id);
	const BattleHaloCfg* GetHaloCfg(int halo_id);
	int GetMedalSpid(int sp_id,int grade);

	int RandomSpecialEffect(int group_id);
	bool CheckSpidMatchGroupid(int sp_id, int group_id);

	bool CheckOnlyForSkillPerformBeforeAdd(int sp_id, std::vector<int>* out_skill_list);
private:
	BattleSpecialEffectPool();
	~BattleSpecialEffectPool();

	//禁止拷贝复制（请不要实现以下这两个函数）
	BattleSpecialEffectPool(const BattleSpecialEffectPool&);
	BattleSpecialEffectPool& operator=(const BattleSpecialEffectPool &);
	
	void Release();

	int InitSpecialEffectList(TiXmlElement *RootElement);
	int InitSpecialEffectDetail(TiXmlElement *RootElement, short effect_type);
	int InitSpecialEffectGroup(TiXmlElement *RootElement);
	int InitHaloList(TiXmlElement *RootElement);
	int InitJieyiSpidHash(TiXmlElement *RootElement);
	int InitOnlyEffectOnPerformSkillMap(TiXmlElement *RootElement);

	bool CheckEffectDetail();
	bool CheckEffectGroup();
	static bool CheckEffectSubType(short effect_type, short effect_sub_type);

	SpecialEffectCfg* m_special_effect_list[SPECIAL_EFFECT_MAX_NUM];

	EffectDetailMap m_effect_detail_map;
	EffectGroupMap m_effect_group_map;
	HaloCfgMap m_halo_cfg_map;
	JieyiSpidHash m_jieyi_spid_hash;
	OnlyEffectOnPerformSkillMap m_only_effect_on_perform_skill_map;
};

#endif