#ifndef __GATHER_FB_CONFIG_HPP__
#define __GATHER_FB_CONFIG_HPP__

#include <vector>
#include <map>

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

static const int MAX_FISHING_TIME_LIMIT_NUM = 2;

struct RoleGatherFbLevelCfg
{
	RoleGatherFbLevelCfg()
	{
		seq = 0;
		scene_id = 0;
		gather_id = 0;
		expend_active = 0;
		pos_group_id = 0;
		need_role_level = 0;
		birth_pos = Posi(0, 0);
		need_skill_type = 0;
		need_skill_level = 0;
		reward_group_id = 0;
	}

	int seq;
	int scene_id;
	int gather_id;
	int expend_active;
	int pos_group_id;
	int need_role_level;
	Posi birth_pos;
	int need_skill_type;
	int need_skill_level;
	int reward_group_id;
};

struct RoleGatherFbOtherCfg
{
	RoleGatherFbOtherCfg()
	{
		create_interval = 0;
		gather_power_max = 0;
		gather_comsume_power = 0;
		every_add_gather_power_value = 0;
		one_way_time = 0;
	}

	int create_interval;
	int gather_power_max;
	int gather_comsume_power;
	int every_add_gather_power_value;
	int one_way_time;					//指针单程时间
};

struct RoleGatherFbPosCfg
{
	RoleGatherFbPosCfg()
	{
	}

	std::vector<Posi> create_pos;
};

struct FieldGatherCfg
{
	FieldGatherCfg():seq(0),time_second(0),min_num(0),max_num(0), limit_num(0),scene_id(0),gather_id(0),pos_group(0)
	{
	}
	int seq;
	int time_second;
	int min_num;
	int max_num;
	int limit_num;
	int scene_id;
	int gather_id;
	int pos_group;
};

struct GatherFbRewardGroupCfg
{
	static const int MAX_GATHER_REWARD_RATE_COUNT = 10000;

	GatherFbRewardGroupCfg() : rate(0)
	{}

	int rate;
	ItemConfigData item;
};

struct GatherFbSkillLevelCfg
{
	GatherFbSkillLevelCfg() : multiple(0), special_reward_group(0)
	{}

	int multiple;
	int special_reward_group;
};

struct GatherFbSpecialRewardGroupCfg
{
	static const int MAX_SPECIAL_REWARD_RATE_COUNT = 10000;
	GatherFbSpecialRewardGroupCfg()
	{}

	int rate;
	ItemConfigData reward;
};

// -----------------------------------------------------------------------------------
class RoleGatherFbConfig : public ILogicConfig
{
public:
	RoleGatherFbConfig();
	virtual ~RoleGatherFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleGatherFbLevelCfg * GetLevelCfg(int seq);
	const RoleGatherFbLevelCfg * GetLevelCfgBySceneId(int scene_id);
	const RoleGatherFbPosCfg * GetPosCfg(int pos_group_id);
	const RoleGatherFbOtherCfg & GetOtherCfg();
	const FieldGatherCfg * GetFieldGatherCfg(int seq);
	const std::map<int, FieldGatherCfg> * GetFieldGatherList() { return &m_field_cfg; }
	const GatherFbRewardGroupCfg * GetRewardGroupRandCfg(int reward_group_id);

	const GatherFbSkillLevelCfg * GetSkillLevelCfg(int scene_id, int skill_level);
	const GatherFbSpecialRewardGroupCfg * GetRandSpecialRewardCfg(int special_reward_group_id);	//不参与倍率

private:
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitPosCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitfieldCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitSkillLevelCfg(TiXmlElement *RootElement, std::string& errormsg);
	int InitSpecialRewardGroupCfg(TiXmlElement *RootElement, std::string& errormsg);

	std::vector<RoleGatherFbLevelCfg> m_level_cfg;
	RoleGatherFbOtherCfg m_other_cfg;
	std::map<int, RoleGatherFbPosCfg> m_pos_cfg;
	std::map<int, FieldGatherCfg> m_field_cfg;

	std::map<int, std::vector<GatherFbRewardGroupCfg> > m_gather_reward_group_cfg;			//奖励组配置 

	std::map<int, std::map<int, GatherFbSkillLevelCfg> > m_skill_level_cfg;					//技能等级关联配置 key1:场景ID key2:生活技能等级
	std::map<int, std::vector<GatherFbSpecialRewardGroupCfg> > m_special_reward_group_cfg;	//特殊奖励组配置---不参与倍率
};

#endif