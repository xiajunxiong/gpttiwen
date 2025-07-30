#ifndef __ROLE_GUILD_TASK_CONFIG_HPP__
#define __ROLE_GUILD_TASK_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

struct RoleGuildTaskItemCfg
{
	int task_id;
	int finish_task_id;
	int need_role_level;
	int type;
	int rate;
	int can_help;

	int guild_exp;
	int guild_gong_xian;
	int reward_group_id;

	int give_guild_exp;		
	int give_guild_gong_xian;
	int give_reward_group_id;
};

struct RoleGuildTaskRewardGroupItemCfg
{
	int group_id;
	int rate;


	std::vector<ItemConfigData> rewards;
};

struct RoleGuildTaskRewardGroupCfg
{
	RoleGuildTaskRewardGroupCfg()
	{
		rate_count = 0;
	}

	int group_id;

	int rate_count;
	std::vector<RoleGuildTaskRewardGroupItemCfg> group_list;
};

struct RoleGuildTaskOtherCfg
{
	int max_finish_times;
	int finish_cd;
	int give_up_cd;
	int refresh_comsume_value;
};


class RoleGuildTaskConfig : public ILogicConfig
{
public:
	RoleGuildTaskConfig();
	virtual ~RoleGuildTaskConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleGuildTaskItemCfg * GetRoleGuildTaskItemCfg(int task_id);
	const RoleGuildTaskItemCfg * GetRandRoleGuildTaskItemCfg(int role_level);
	const RoleGuildTaskRewardGroupItemCfg * RandGuildTaskRewardGroupItemCfg(int group_id);
	const RoleGuildTaskOtherCfg & GetOtherCfg() const;
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitRoleGuildTaskItemCfg(TiXmlElement * RootElement);
	int InitRoleGuildTaskRewardGroupCfg(TiXmlElement * RootElement);

	int m_task_rate_count;
	std::map<int,RoleGuildTaskItemCfg> m_task_cfg;
	RoleGuildTaskOtherCfg m_other_cfg;
	std::map<int, RoleGuildTaskRewardGroupCfg> m_reward_group_cfg;
};

#endif