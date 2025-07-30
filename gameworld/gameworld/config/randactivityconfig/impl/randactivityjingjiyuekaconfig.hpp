#ifndef __RAND_ACTIVITY_JINGJIYUEKA_CONFIG_HPP__
#define __RAND_ACTIVITY_JINGJIYUEKA_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/roleactivity/rajingjiyuekaparam.hpp"
#include <set>

struct RandActivityJingJiYueKaOtherCfg
{
	RandActivityJingJiYueKaOtherCfg() :buy_gold(0), day_task_refresh_gold(0), is_open(false), open_day(0), buy_price(0), cfg_ver(0), open_role_level(0)
	{}

	int buy_gold;						//����ȼ�����Ԫ��
	int day_task_refresh_gold;			//ÿ������ˢ������Ԫ��
	bool is_open;						
	int open_day;						//����������ڼ�������ȼ�����
	int buy_price;						//������(������) -- ֱ��
	int cfg_ver;						
	int open_role_level;				//��ɫ�����ȼ�
};

struct RandActivityJingJiYueKaBaseCfg
{
	RandActivityJingJiYueKaBaseCfg() : upgrade_exp(0), is_has_base_reward(false)
	{}

	int upgrade_exp;
	bool is_has_base_reward;
	ItemConfigData base_item;
	std::vector<ItemConfigData> extra_reward_list;		//ֱ��������ʸ���ȡ
};

struct RandActivityJingJiYueKaTaskCfg
{
	RandActivityJingJiYueKaTaskCfg() : task_id(0), task_type(0), param1(0), task_exp(0), rate(0), is_can_rand(false)
	{}
	
	int task_id;
	int task_type;
	int param1;
	int task_exp;
	int rate;
	bool is_can_rand;			//�Ƿ���������������
};

struct RandActivityJingJiYueKaTaskGroupCfg
{
	RandActivityJingJiYueKaTaskGroupCfg() : rate_count(0), can_rand_rate_count(0), can_rand_task_count(0)
	{}

	int rate_count;
	int can_rand_rate_count;
	int can_rand_task_count;
	std::vector<RandActivityJingJiYueKaTaskCfg> task_list;
};

struct RandActivityJingJiYueKaTaskCountCfg
{
	RandActivityJingJiYueKaTaskCountCfg()
	{
		memset(task_num_list, 0, sizeof(task_num_list));
	}

	int task_num_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX];
};

union RandActivityJingJiYueKaKey
{
	int param[2];
	long long unique_key;
};

class RandActivityManager;
class RandActivityJingJiYueKaConfig : public RandActivityConfig
{
public:
	RandActivityJingJiYueKaConfig();
	virtual ~RandActivityJingJiYueKaConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RandActivityJingJiYueKaOtherCfg & GetOtherCfg()const { return m_other_cfg; }
	const RandActivityJingJiYueKaBaseCfg * GetBaseCfg(int active_role_level, int card_level)const;
	const std::map<int, RandActivityJingJiYueKaBaseCfg> * GetBaseMapCfg(int active_role_level) const;
	int GetMaxCardLevel(int active_role_level)const;
	
	const RandActivityJingJiYueKaTaskCfg * GetTaskCfg(int task_groupd_type, int task_id)const;
	bool GetSingleRandDayTask(std::set<int> accept_task_list, int & new_task_id, int & new_task_type)const;
	void GetRandTaskByType(int task_type, int active_role_level, short & has_task_count, int max_count, RAJingJiYueKaTaskInfo * task_info_list)const;


	virtual int GetConfigVer() const;
private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg);
	int InitBaseCfg(TiXmlElement * RootElement, std::string & errormsg);
	int InitTaskCfg(TiXmlElement * RootElement, std::string & errormsg);
	int InitTaskCountCfg(TiXmlElement * RootElement, std::string & errormsg);

	long long GetKey(int level_down, int level_up) const;
	void GetInfoByKey(long long key, int * level_down, int * level_up) const;

	RandActivityJingJiYueKaOtherCfg m_other_cfg;	
	std::map<long long, std::map<int, RandActivityJingJiYueKaBaseCfg> > m_base_cfg;		//�������� key1:GetBaseKey key2:card_level
	RandActivityJingJiYueKaTaskGroupCfg m_task_group_list[RA_JING_JI_YUE_KA_TASK_GROUP_TYPE_MAX];	//����������
	std::map<long long, RandActivityJingJiYueKaTaskCountCfg> m_task_count_cfg;			//������������
};


#endif