#ifndef __ACTIVITY_ONLY_FIGHT_CONFIG__
#define __ACTIVITY_ONLY_FIGHT_CONFIG__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/commondata.hpp"
#include <set>

static const int ONLY_FIGHT_AWARD_MAX_COUNT = 10;
static const int ONLY_FIGHT_BOX_MAX_AWARD = 5;
static const int ONLY_FIGHT_BORN_POSI_MAX_NUM = 8;


struct OnlyFightDeleteData
{
	OnlyFightDeleteData() :box_id(0), obj_id(0) {}
	OnlyFightDeleteData(int _box_id, ObjID _obj_id) : box_id(_obj_id), obj_id(_obj_id) {}
	int box_id;
	ObjID obj_id;
};

struct OnlyFightSceneCfg
{
	OnlyFightSceneCfg():min_level(0), max_level(0), scene_id(0)
	{}

	int min_level;
	int max_level;
	int scene_id;
};

struct OnlyFightOtherCfg
{
	OnlyFightOtherCfg() : max_fail_times(0), max_rounds_num(0), wins_score(0),lose_score(0), 
	draw_score(0), rank_refresh_interval(0), num(0), max_num(0), countdown(0) {}

	int max_fail_times;				// ���ʧ�ܴ���
	int max_rounds_num;				// ����ս�����غ���
	int wins_score;					// ʤ�����û���
	int lose_score;					// ʧ�����û���
	int draw_score;					// ƽ�����û���
	int rank_refresh_interval;		// ����������ˢ�¼�� - ��λ:��(s)
	int time;						// ˢ��ʱ��
	int num;						// һ��ˢ�±������
	int max_num;					// ͬʱ���ڱ������
	int countdown;					// ��������ʱ
};

struct OnlyFightParticipationAward		//���뽱��
{
	OnlyFightParticipationAward() : coin_bind(0), experience(0) {}

	int coin_bind;			// ͭ�ҽ���
	int experience;			// ���齱��
	std::vector<ItemConfigData> item_reward;	//�����޵��߽���
};

struct OnlyFightRankAward		// ��һ�а�(���˰������Ҷ�)��������
{
	OnlyFightRankAward() : title(0), coin(0) {}

	int title;			// �ƺŽ���
	int coin;			// ͭ�ҽ���
	std::vector<ItemConfigData> item_reward;		//�����޵��߽���
};

struct OnlyFightRoleLevelRankAward		
{
	OnlyFightRoleLevelRankAward()
	{
		min_role_level = 0;
		max_role_level = 0;
	}

	int min_role_level;
	int max_role_level;
	std::map<ParamSection, OnlyFightRankAward> rank_reward;
};

struct OnlyFightSurvAward		// ����
{
	OnlyFightSurvAward() : seq(0), coin_bind(0), experience(0) {}

	int seq;
	int coin_bind;			// ͭ�ҽ���
	int experience;			// ���齱��
	std::vector<ItemConfigData> item_reward;	//�����޵��߽���
};

struct OnlyFightBoxCfg
{
	OnlyFightBoxCfg() : box_id(0), rate(0), reward_id_num(0)
	{
		memset(reward_id_list, 0, sizeof(reward_id_list));
	}

	int box_id;						// ����ID
	int rate;						// Ȩ��

	int reward_id_num;				// ��ȡ������
	int reward_id_list[ONLY_FIGHT_BOX_MAX_AWARD];		// ���佱����ID
};

struct OnlyFightBoxRewardCfg
{
	OnlyFightBoxRewardCfg(): reward_id(0), rate(0)
	{}

	int reward_id;				//���佱����ID
	int rate;
	ItemConfigData reward_item;
};

struct OnlyFightFindRewardGroupCfg
{
	OnlyFightFindRewardGroupCfg() :rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

struct OnlyFightFindCfg
{
	OnlyFightFindCfg() :min_level(0), max_level(0), exp(0), coin(0), family_gongxian(0), group_id(0)
	{}

	int min_level;
	int max_level;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
};

struct OnlyFightFindGroupCfg
{
	OnlyFightFindGroupCfg()
	{}

	std::vector<OnlyFightFindCfg> node_list;
};

class ActivityOnlyFightConfig : public ILogicConfig
{
public:
	ActivityOnlyFightConfig();
	virtual ~ActivityOnlyFightConfig();
	
	virtual bool Init(const std::string &configname, std::string *err);
	void GetChestIDList(std::vector<int> & out_list);
	void GetSurvivalCheckNumList(std::set<int> & out_list);

	const OnlyFightOtherCfg & GetOnlyFightOtherCfg() { return m_other_cfg; }
	const OnlyFightParticipationAward * GetOnlyFightParCfg(int level);
	const OnlyFightRankAward * GetOnlyFightRankCfg(int rank, int role_level);
	const OnlyFightSurvAward * GetOnlyFightSurvAward(int level, int check_num);
	const OnlyFightBoxCfg * GetOnlyFightBoxCfg(int box_id);
	bool RandAwardListByBoxID(int chest_id, std::vector<ItemConfigData> & out_award_list);
	const OnlyFightBoxCfg * GetOnlyFightBoxCfg();
	const OnlyFightSceneCfg * GetOnlyFightSceneCfgByLevel(int level);
	const std::vector<OnlyFightSceneCfg> * GetOnlyFightSceneCfgVec() { return &m_scene_cfg; }

	Posi GetRandBornPosi();
	const std::vector<int> * GetRankTitleListCfg() { return &m_rank_reward_title_list_cfg;}

	const OnlyFightFindCfg * GetFindCfg(int find_type, int role_level)const;
	const OnlyFightFindRewardGroupCfg * GetFindRewardGroupCfg(int group_id)const;

private:
	int InitOnlyFightOthersCfg(TiXmlElement * RootElement);
	int InitOnlyFightParticicpationCfg(TiXmlElement * RootElement);
	int InitOnlyFightRankAward(TiXmlElement * RootElement);
	int InitOnlyFightSurvAward(TiXmlElement * RootElement);
	int InitOnlyFightBoxCfg(TiXmlElement * RootElement);
	int InitOnlyFightBoxRewardCfg(TiXmlElement * RootElement);
	int InitOnlyFightScecnCfg(TiXmlElement * RootElement);
	int InitOnlyFightBornPosiCfg(TiXmlElement * RootElement);
	int InitOnlyFightFindAwardGroupCfg(TiXmlElement * RootElement);
	int InitOnlyFightFindAwardCfg(TiXmlElement * RootElement);

	OnlyFightOtherCfg m_other_cfg;
	std::map<ParamSection, OnlyFightParticipationAward> m_particicpation_map;		// ���뽱����    �� - �ȼ�����
	std::vector<OnlyFightRoleLevelRankAward> m_rank_award;						// �����������	�� - ��������
	std::map<int, std::map<ParamSection, OnlyFightSurvAward> > m_survival_award;	// �Ҵ��߽�����	�� - �Ҵ����ٽ��������ȼ�����
	int chest_weight;
	std::map<int, OnlyFightBoxCfg> m_chest_cfg;										// �������ã�	�� - ����ID
	std::map<int, std::vector<OnlyFightBoxRewardCfg> > m_item_group_cfg;			// ���佱���飺	�� - ��ID

	std::vector<OnlyFightSceneCfg> m_scene_cfg;										// �������� : ������ҵȼ���

	int m_born_count;
	Posi m_born_list[ONLY_FIGHT_BORN_POSI_MAX_NUM];									// ������λ
	std::vector<int> m_rank_reward_title_list_cfg;									// �洢���������ĳƺ�

	std::map<int, std::vector<OnlyFightFindRewardGroupCfg> > m_find_reward_group_map;		// �һص�����:key - group_id
	OnlyFightFindGroupCfg m_find_cfg[RB_TYPE_MAX];											// �����һ�
};



#endif