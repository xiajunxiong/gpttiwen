#ifndef __TRANSACTION_TASK_CONFIG_HPP__
#define __TRANSACTION_TASK_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/transactiontaskdef.hpp"


struct TransactionTaskSingleCfg
{
	TransactionTaskSingleCfg():seq(-1), rate(0), is_has_item_reward(false), money_num(0)
	{}

	int seq;						
	int rate;						//权重
	ItemConfigData need_item;		//所需道具
	ItemConfigData reward;			//奖励
	bool is_has_item_reward;
	int money_num;					//灵玉奖励
};

struct TransactionTaskLevelGroupCfg
{
	TransactionTaskLevelGroupCfg():type(0), min_level(0), max_level(0), version_reset(0)
	{
		memset(group_id, 0, sizeof(group_id));
	}

	int type;
	int min_level;		
	int max_level;
	int group_id[TRANSACTION_TASK_REWARD_MAX_NUM];		//奖励组id
	int version_reset;									//版本重置
};

struct TransactionTaskRewardCfg
{
	TransactionTaskRewardCfg():level(0), exp(0), coin(0), count(0)
	{}

	int level;
	int exp;
	int coin;
	int count;
	std::vector<ItemConfigData> task_reward;		//任务奖励
};

struct TransactionTaskOtherCfg
{
	TransactionTaskOtherCfg():random(0), task_id(0)
	{}

	int random;
	int task_id;	
};

struct TransactionTaskRewardFindCfg
{
	TransactionTaskRewardFindCfg() : seq(0), min_level(0), max_level(0), find_type(0), group_id(0), exp(0), coin(0), family_gongxian(0)
	{}

	int seq;
	int min_level;
	int max_level;
	int find_type;
	int group_id;
	int exp;
	int coin;
	int family_gongxian;
};

struct TransactionTaskFindRewardGroupCfg
{
	TransactionTaskFindRewardGroupCfg() : rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

class TransactionTaskConfig : public ILogicConfig
{
public:
	TransactionTaskConfig();
	virtual ~TransactionTaskConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool GetRewardListCfg(int level, int* list, short *type, short *version);
	int GetGroupByIndex(int type, int index);
	const TransactionTaskSingleCfg * GetTaskSingleRewardCfg(int group_id, int seq);
	const TransactionTaskRewardCfg* GetTaskRewardCfg(int level);
	const TransactionTaskOtherCfg & GetTaskOtherCfg() { return m_other_cfg; }
	const TransactionTaskLevelGroupCfg* GetTaskLevelGroupCfg(int level);
	const int GetRewardType(int _role_level);
	const TransactionTaskRewardFindCfg * GetFindRewardCfg(int find_type, int level);
	const TransactionTaskFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id);


private:
	int InitTaskLevelGroupCfg(TiXmlElement *RootElement);
	int InitTaskRewardGroupCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskRewardCfg(TiXmlElement *RootElement);
	int InitTaskRewardFindCfg(TiXmlElement *RootElement);
	int InitTaskFindRewardGroupCfg(TiXmlElement *RootElement);

	TransactionTaskOtherCfg m_other_cfg;
	std::vector<TransactionTaskLevelGroupCfg> m_task_level_group_cfg;
	//std::map<int, std::vector<TransactionTaskSingleCfg> > m_group_cfg;    //以group_id为键
	TransactionTaskSingleCfg m_group_cfg[MAX_GROUP_ID_NUM][MAX_GROUP_SEQ_NUM];	

	std::map<int, TransactionTaskRewardCfg> m_task_reward_map;			//任务奖励 以level为键

	std::vector<TransactionTaskRewardFindCfg> m_rfcfg_v;							// 一键完成
	std::map<int, std::map<int, TransactionTaskRewardFindCfg> > m_transaction_task_find_cfg;	// 奖励找回表：键 - 等级区间、找回类型
	std::map<int, std::vector<TransactionTaskFindRewardGroupCfg> > m_find_reward_group_map;			// 找回道具组：键 - 组ID
};



#endif