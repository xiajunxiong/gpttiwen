#ifndef __DU_JIE_CONFIG_HPP__
#define __DU_JIE_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/dujiedef.hpp"

#include "gamedef.h"

struct DuJieOtherCfg
{
	DuJieOtherCfg() :level_gap(0)
	{}

	int level_gap;		//队员等级差距

	//助战
	int day_assist_times;		//每日助战奖励次数
	std::vector<ItemConfigData> assist_reward;	//助战奖励
};

//////渡劫
//关卡配置
struct DuJieFbCfg
{
	static const int MAX_REWARD_NUM = 5;

	DuJieFbCfg()
	{
		fb_index = 0;
		front_task_type = 0;
		front_task_param_1 = 0;
		front_task_param_2 = 0;

		scene_id = 0;

		boss_dir = 0;
		boss_npc_id = 0;

		reward_num = 0;

		reward_title_id = 0;
	}

	int fb_index;	//关卡index

	//前置任务
	int front_task_type;	//任务类型
	int front_task_param_1;	//任务参数
	int front_task_param_2;	//任务参数

	//副本信息
	int scene_id;	//场景id

	//怪物信息
	Posi boss_pos;
	Dir boss_dir;
	int boss_npc_id;

	//角色出生点
	Posi role_pos;

	//奖励
	int reward_num;
	ItemConfigData reward_list_[MAX_REWARD_NUM];
	UInt16 reward_title_id;

	////助战
	//int day_assist_times;		//每日助战奖励次数
	//std::vector<ItemConfigData> assist_reward;	//助战奖励
};

class DuJieConfig : public ILogicConfig
{
public:
	DuJieConfig();
	virtual ~DuJieConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DuJieOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const DuJieFbCfg* GetFbCfg(int fb_index) const;

	int GetTaskShanHaiBossHard() const { return m_task_shan_hai_boss_hard; };
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFBCfg(TiXmlElement *RootElement);

	//副本信息[副本level]
	DuJieOtherCfg m_other_cfg;
	int m_fb_count;
	DuJieFbCfg m_fb_cfg_[DU_JIE_MAX_FB_NUM];

	//任务记录的山海boss难度
	int m_task_shan_hai_boss_hard;

};

#endif
