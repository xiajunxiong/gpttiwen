#ifndef __WAN_LING_FN_CONFIG_HPP__
#define __WAN_LING_FN_CONFIG_HPP__

#include "servercommon/wanlingfbdef.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include "gamedef.h"

namespace WanLingFB
{
	struct OtherCfg
	{
		OtherCfg()
		{
			day_battle_reward_time = 0;
			day_pass_reward_time = 0;
		}

		int day_battle_reward_time;	//每日战斗奖励次数
		int day_pass_reward_time;	//每日通关奖励次数
	};

	struct CreatNPCCfg
	{
		CreatNPCCfg()
		{
			npc_id = 0;
			npc_dir = 0;
		}
		int npc_id;
		//位置
		Posi npc_pos;
		Dir npc_dir;
	};

	struct TaskCfg
	{
		TaskCfg()
		{
			task_seq = 0;
			task_type = 0;
			target_npc_id = 0;

			npc_action = 0;
		}
		int task_seq;		//任务序号

		int task_type;		//任务类型
		int target_npc_id;	//任务目标npc_id

		int npc_action;
	};
	enum TaskType
	{//									npc_action
		TASK_TYPE_BATTLE = 1,	//战斗	  怪物组
		TASK_TYPE_TALK = 2,		//对话	  对话id

		TASK_TYPE_MAX,
	};
	struct FB_Cfg
	{
		FB_Cfg()
		{
			fb_index = 0;
			open_level = 0;
			scene_id = 0;

		}
		int fb_index;	//副本index

		int open_level;	//角色进入等级限制
		int scene_id;	//场景id

		Posi role_pos;	//角色出生点

		Posi leave_pos;	//离开场景 传送门位置

		//创建npc vec
		std::vector<CreatNPCCfg> creat_npc_vec;
		//任务vec[任务index]
		std::vector<TaskCfg> task_vec;
		//最后一场战斗的任务seq
		int final_fight_task_seq;

		//战斗奖励
		int battle_pass_coin_bind;	//铜币

		//通关奖励
		int pass_exp;	//经验
		std::vector<ItemConfigData> pass_reward_vec;
		//通关随机奖励组id
		std::vector<int> pass_drop_grop_id_vec;
	};

	struct DropCfg
	{
		DropCfg()
		{
			is_broadcast = 0;
		}
		int is_broadcast;
		std::vector<ItemConfigData> base_prof_drop_;	//奖励物品[基础职业]
	};
	//掉落池map<权重begin, 物品>
	typedef std::map<int, DropCfg> DropPool;

	struct DropGroupCfg
	{
		DropGroupCfg()
		{
			group_id = 0;
			total_power = 0;
		}
		int group_id;	//奖励组id

		int total_power;//总权重
		DropPool drop_pool;
	};

	//奖励组id_map<奖励组id, 奖励组信息>
	typedef std::map<int, DropGroupCfg> DropGroupIDMap;
};

class WanLingFBConfig : public ILogicConfig
{
public:
	WanLingFBConfig();
	virtual ~WanLingFBConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WanLingFB::OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	const WanLingFB::FB_Cfg * GetFBCfg(const int fb_index) const;
	const WanLingFB::FB_Cfg * GetFBByLevelCfg(int role_level) const;
	const WanLingFB::TaskCfg * GetTaskCfg(const int fb_index, const int task_seq) const;
	const ItemConfigData * GetDropItem(const int drop_group_id, const int prof, int &is_broadcast) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDropGroupCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitCreatNpcCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	WanLingFB::OtherCfg m_other_cfg;
	//副本配置[副本index]
	std::vector<WanLingFB::FB_Cfg> m_fb_cfg_vec;

	//奖励组map<奖励组id, 奖励组配置>
	WanLingFB::DropGroupIDMap m_drop_group_id_map;
};

#endif