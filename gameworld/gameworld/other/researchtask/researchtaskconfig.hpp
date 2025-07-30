#ifndef __RESEARCH_TASK_CONFIG_HPP__
#define __RESEARCH_TASK_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"

#include <algorithm>

#include <map>
#include <set>
#include <vector>

static const int RESEARCH_TASK_TIMES_PER_ROUND = 10;			// д����ʮ��һ��
static const int RESEARCH_TASK_AWARD_MAX_COUNT = 5;
static const int RESEARCH_TASK_AWARD_MAX_SEQID = 32;

struct ResearchTaskOtherCfg		// ��������
{
	ResearchTaskOtherCfg() : daily_refresh_time(0), one_key_cost_per_times(0), one_key_auto_max_times(0), 
		max_daily_task_times(0), team_need_member_min(0), gather_power_award(0) {}

	int daily_refresh_time;			// ÿ��ˢ��ʱ�� - Ĭ��ÿ�����ˢ��
	int one_key_cost_per_times;		// һ�����ÿ������
	int one_key_auto_max_times;		// һ������Զ�����
	int max_daily_task_times;		// ÿ�ս��������������
	int team_need_member_min;		// ��С�������
	int gather_power_award;			// ��������������ֵ
};

struct ResearchTaskCfg		// ��������
{
	ResearchTaskCfg() : task_id(0), lv_limit_min(0), lv_limit_max(0), 
		task_npc_seq(0), scene_group(0), demon_group(0) {}

	int task_id;			// ����ID
	int lv_limit_min;		// ��С�ȼ�����
	int lv_limit_max;		// ���ȼ�����
	int task_npc_seq;		// �������NPC���������ύ��ͬһ��NPC�����һ�ֲ��ύ��

	int scene_group;		// ���������
	int demon_group;		// ���������
};

struct ResearchRewardCfg	// ��������������
{
	ResearchRewardCfg() : experience(0), coin_bind(0) {}

	int experience;
	int coin_bind;
};

struct RandSceneCon		// ���������
{
	RandSceneCon() : scene_id(0), rand_rate(0), pos_group_id(0) {}

	int scene_id;		// ����ID
	int rand_rate;		// �������
	int pos_group_id;	// ��Ӧ������
};

struct RandDemonCon		// ��������� 
{
	RandDemonCon() : img_id(0), battle_group_id(0), rand_rate(0) {}

	int img_id;				// ���ڳ�������ʾ�Ĺ���ID
	int battle_group_id;	// ����ս���Ĺ�����ID
	int rand_rate;			// �������
};

struct RandPosCon
{
	RandPosCon() : pos_group_id(0), pos_x(0), pos_y(0), rand_rate(0) {}

	int pos_group_id;
	int pos_x;
	int pos_y;
	int rand_rate;
};

struct RandAwardCon		// �ִν�����
{
	RandAwardCon() : give_rate(0) {}

	int give_rate;					// �������
	ItemConfigData award_item;		// ��������
};

struct RTRandGroupCon
{
	RTRandGroupCon() : prob(0) {}

	int prob;
	ItemConfigData item;
};

struct AwardBackCon
{
	AwardBackCon() : experience(0), coin_bind(0), family_contribution(0), item_group(0), find_type(0){}

	int experience;
	int coin_bind;
	int family_contribution;
	int item_group;
	int find_type;
};

struct RTRoundRewardCon
{
	RTRoundRewardCon() : reward_group(0), rate(0) {}
	int reward_group;
	int rate;
};

struct RTRoundRewardCfg
{
	RTRoundRewardCfg() : rate(0) {}

	int rate;
	ItemConfigData reward;
};

struct RTRoundRewardGroupCfg
{
	RTRoundRewardGroupCfg() : weight(0) {}

	int weight;
	std::vector<RTRoundRewardCfg> m_reward_vec;
};

class ResearchTaskConfig : public ILogicConfig
{
public:
	ResearchTaskConfig();
	virtual ~ResearchTaskConfig();

	virtual bool Init(const std::string & configname, std::string * err);

	const ResearchTaskOtherCfg & GetOtherConfig() { return m_zzt_other_cfg; }

	const ResearchTaskCfg * GetTaskByLevel(int level);
	const ResearchTaskCfg * GetTaskByTaskID(int task_id);
	const ResearchRewardCfg * GetTaskReward(int role_level);
	const AwardBackCon * GetRewardBack(int role_level, int back_type);
	const ItemConfigData * RandBackItem(int group_id);

	bool GetRandSceneAndDemonByTaskID(int task_id, int * scene_id, int * demon_id, int * image_id, ARG_OUT Posi* pos);

	void GetAwardTimesListByFinishTimes(int finish_times, std::set<int> & out_list);
	int GetAwardSeqByAwardNeedTimes(int need_times);	// �� 0 ��ʼ
	bool RandAwardList(int level, int need_times, int * out_list_len, ItemConfigData * out_award_list);
	const AwardBackCon* GetFindType(int _role_level);

	const std::vector<RTRoundRewardCon> * RandRewardListRoundCfg(int need_times, int role_level);
	const RTRoundRewardCfg * RandRoundRewardCfg(int group_id);
private:
	int InitZTaskOtherCfg(TiXmlElement * RootElement);
	int InitTasksBasicCfg(TiXmlElement * RootElement);
	int InitSceneGroupCfg(TiXmlElement * RootElement);
	int InitDemonGroupCfg(TiXmlElement * RootElement);
	int InitPosGroupCfg(TiXmlElement * RootElement);
	int InitAwardGroupCfg(TiXmlElement * RootElement);
	int InitTaskRewardCfg(TiXmlElement * RootElement);
	int InitBackGroupsCfg(TiXmlElement * RootElement);
	int InitRewardBackCfg(TiXmlElement * RootElement);
	int InitOneRundRewardCfg(TiXmlElement * RootElement);
	int InitRundRewardGroupCfg(TiXmlElement * RootElement);

	bool IsTimerCfg(int check_time);

	ResearchTaskOtherCfg m_zzt_other_cfg;
	std::map<int, ResearchTaskCfg> m_research_task_cfg;				// �� - ����ID

	std::map<int, std::vector<RandSceneCon> > m_rand_scene_map;
	std::map<int, std::vector<RandDemonCon> > m_rand_demon_map;
	std::map<int, std::vector<RandPosCon> > m_rand_pos_map;
	std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > > m_rand_award_map;
	std::map<ParamSection, ResearchRewardCfg> m_task_reward_map;

	std::map<int, std::map<ParamSection, std::vector<RTRoundRewardCon> > > m_round_reward_map;
	std::map<int, RTRoundRewardGroupCfg> m_round_reward_group_map;

	// �����һ�
	std::map<int, std::vector<RTRandGroupCon> > m_back_item_group_map;			// �һص����飺�� - ��ID
	std::map<ParamSection, std::map<int, AwardBackCon> > m_reward_back_map;		// �����һر��� - �ȼ����䡢�һ�����
	std::map<int, AwardBackCon> m_seq_to_rrcfg;
};

#endif