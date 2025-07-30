#ifndef __SCORE_SYSTEM_CONFIG_HPP__
#define __SCORE_SYSTEM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/medaldef.h"
#include "servercommon/struct/battle/battle_def.hpp"

struct MedalSpeScoreCfg		//令牌特效评分
{
	MedalSpeScoreCfg()
	{
		memset(score_priority, 0, sizeof(score_priority));
	}
	int score_priority[MAX_MEDAL_MAX_GRADE];
};

struct CharacterScoreCfg
{
	CharacterScoreCfg() :job(0), level(0), score_s(0), score_a(0), score_b(0) {}

	int job;
	int level;
	int score_s;
	int score_a;
	int score_b;
};

struct PetScoreCfg
{
	PetScoreCfg() : level(0), score_s(0), score_a(0), score_b(0) {}

	int level;
	int score_s;
	int score_a;
	int score_b;
};

struct EquipmentScoreCfg
{
	EquipmentScoreCfg() : item_id(0), attr_type(-1), score_s(0), score_a(0), score_b(0) {}
	int item_id;
	int attr_type;
	int score_s;
	int score_a;
	int score_b;
};

static const int CAPABILITY_ATTR_BASE_NUM = 100;		//此处配置扩大后系数，计算出结果后需除以该系数
static const int CAPABILITY_BASE_NUM = 4;		//计算出结果后还需要乘以该系数
struct CapabilityCalCfg		// 角色或伙伴战力计算系数  
{
	CapabilityCalCfg()
	{
		memset(cap_base, 0, sizeof(cap_base));
	}

	int cap_base[BATTLE_ATTR_MAX];	//属性系数
};

struct ProfSkillScoreKey
{
	ProfSkillScoreKey() : job_type(0), skill_level(0) {}
	ProfSkillScoreKey(short _job_type, short _skill_level) : job_type(_job_type), skill_level(_skill_level) {}
		
	bool operator< (const ProfSkillScoreKey& index) const
	{
		if (job_type != index.job_type)
		{
			return job_type < index.job_type;
		}
		else
		{
			return skill_level < index.skill_level;
		}
	}

	short job_type;
	short skill_level;
};

struct TrainSkillScoreKey
{
	TrainSkillScoreKey() : train_skill_type(0), train_skill_level(0) {}
	TrainSkillScoreKey(short _train_skill_type, short _train_skill_level) 
		: train_skill_type(_train_skill_type), train_skill_level(_train_skill_level) {}

	bool operator< (const TrainSkillScoreKey& index) const
	{
		if (train_skill_type != index.train_skill_type)
		{
			return train_skill_type < index.train_skill_type;
		}
		else
		{
			return train_skill_level < index.train_skill_level;
		}
	}

	short train_skill_type;
	short train_skill_level;
};

struct SkillScoreCfg
{
	SkillScoreCfg() : skill_id(0), skill_level(0), skill_score(0) {}

	int skill_id;
	int skill_level;
	int skill_score;
};

struct ScoreTaskCfg
{
	ScoreTaskCfg() :level(0), role_score(0),money(0)
	{}

	int level;
	int role_score;
	int money;
};

class ScoreSystemConfig : public ILogicConfig
{
public:
	ScoreSystemConfig();
	virtual ~ScoreSystemConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const MedalSpeScoreCfg * GetMedalSpeScroeCfg(int level);
	const CharacterScoreCfg * GetPartnerScoreCfg(int job,int level);
	const CharacterScoreCfg * GetRoleScoreCfg(int job,int level);
	const PetScoreCfg * GetPetScoreCfg(int level);
	const EquipmentScoreCfg * GetEquipmentScoreCfg(ItemID item_id);
	const CapabilityCalCfg * GetProfCapabilityCalCfg(int app_prof);
	bool IsSpecialAttrType(int attr_type);
	int GetProfSkillScore(ProfSkillScoreKey job_key);
	int GetTrainSkillScore(int train_skill_type, int train_skill_level);
	const SkillScoreCfg * GetSkillScpreCfg(int skill_id, int skill_level);
	const std::vector<ScoreTaskCfg>& GetScoreTask() { return m_score_task; }
	int GetHolyEquipScoreCfg(int attr_type)const;

private:
	int InitMedalSpeScoreCfg(TiXmlElement *RootElement);
	int InitPartnerScoreCfg(TiXmlElement *RootElement);
	int InitRoleScoreCfg(TiXmlElement *RootElement);
	int InitPetScoreCfg(TiXmlElement *RootElement);
	int InitEquipmentScoreCfg(TiXmlElement *RootElement);
	int InitCapbilityBaseCfg(TiXmlElement *RootElement);
	int InitProfSkillScoreCfg(TiXmlElement *RootElement);
	int InitTrainSkillScoreCfg(TiXmlElement *RootElement);
	int InitSkillScoreCfg(TiXmlElement *RootElement);
	int InitScoreTaskCfg(TiXmlElement *RootElement);
	int InitHolyEquipScoreCfg(TiXmlElement *RootElement);

	MedalSpeScoreCfg m_medal_spe_score_cfg[MAX_ROLE_LEVEL];			//令牌特效评分配置
	CharacterScoreCfg m_partner_score_cfg[PROFESSION_TYPE_NUM][MAX_ROLE_LEVEL + 1];
	CharacterScoreCfg m_role_score_cfg[PROFESSION_TYPE_NUM][MAX_ROLE_LEVEL + 1];
	PetScoreCfg m_pet_score_cfg[MAX_ROLE_LEVEL + 1];
	std::map<ItemID, EquipmentScoreCfg> m_equipment_score_cfg;
	CapabilityCalCfg capability_base_cfg[PROFESSION_TYPE_NUM];		//角色(伙伴)属性计算系数
	std::map<ProfSkillScoreKey, int> m_prof_skill_score_cfg;		//角色技能评分
	std::map<TrainSkillScoreKey, int> m_train_skill_score_cfg;		//角色修炼技能评分

	std::vector<SkillScoreCfg> m_skill_score_vec;				// 被动技能评分

	std::vector<ScoreTaskCfg> m_score_task;			// 角色评分任务

	std::map<int, int> m_holy_equip_score_cfg;		// 圣器属性评分
};

#endif