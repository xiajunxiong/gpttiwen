#ifndef __ACTIVE_SYSTEM_CONFIG_HPP__
#define __ACTIVE_SYSTEM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/activesystemdef.hpp"


struct ActiveSystemCommonCfg
{
	ActiveSystemCommonCfg() :reward_coin(0), reward_energy(0), reward_exp(0), level_min(0), level_max(0),reward_num(0), open_day(0) 
	{}

	int reward_coin;			//����ͭ��
	int reward_energy;			//��������
	int reward_exp;				//��������
	int level_min;
	int level_max;
	int reward_num;				//������
	int open_day;
	ItemConfigData reward_list[REWARD_LIST_ITEM_NUM];	//�����б�
};

struct ActiveSystemCfg
{
	ActiveSystemCfg() :type(0), need_count(0), integral_active(0), is_every_time(0)
	{}

	int type;
	int need_count;				//����
	int integral_active;        //��ɽ������ܻ�Ծ��
	int is_every_time;			//������ɷ��Ż���ȫ����ɷ���
	ActiveSystemCommonCfg reward_cfg;
};

struct ActiveSystemListCfg
{
	ActiveSystemListCfg() {}

	std::vector<ActiveSystemCfg> m_reward_vec;
};

struct ActiveSystemSpecialCfg
{
	ActiveSystemSpecialCfg() : special_type(0), need_count(0)
	{}

	int special_type;
	int need_count;
	ActiveSystemCommonCfg reward_cfg;
};

struct ActiveSystemSpecialListCfg
{
	ActiveSystemSpecialListCfg() {}

	std::vector<ActiveSystemSpecialCfg> m_reward_vec;
};

struct ActiveSystemRewardCfg
{
	ActiveSystemRewardCfg():seq(0), integral_active(0), coin(0), gold(0), power(0), exp(0), count(0)
	{}

	int seq;
	int integral_active;             //�����Ծ��
	int coin;
	int gold;
	int power;
	int exp;
	int count;
	ItemConfigData reward[ACTIVE_SYSTEM_REWARD_MAX_NUM];
};

struct ActiveSystemLevelGroupCfg
{
	ActiveSystemLevelGroupCfg():level_min(0), level_max(0)
	{}

	int level_min;
	int level_max;

	ActiveSystemRewardCfg node_cfg[ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM];
};

struct ActiveSystemRelatedCfg
{
	ActiveSystemRelatedCfg() :seq(0), count(0)
	{}

	int seq;
	int count;
	ItemConfigData reward[ACTIVE_SYSTEM_REWARD_MAX_NUM];
};

struct ActiveSystemRelatedActCfg
{
	ActiveSystemRelatedActCfg()
	{}

	ActiveSystemRelatedCfg node_cfg[ACTIVE_SYSTEM_LEVEL_GROUP_MAX_NUM];
};

// �ܳ� ����
struct WeekActiveDegreeTaskCfg
{
	WeekActiveDegreeTaskCfg()
	{
		task_type = ACTIVE_DEGREE_TYPE_BEGIN;
		task_param = 0;
		week_active_degree = 0;
	}
	int task_type;			//��������
	int task_param;			//�������

	int week_active_degree;	//�� ��Ծ����
};

// �ܳ� - ��Ծ����
struct WeekActiveDegreeRewardCfg
{
	WeekActiveDegreeRewardCfg()
	{
		reward_index = 0;
		need_degree = 0;
	}
	int reward_index;
	int need_degree;

	std::vector<ItemConfigData> reward_item_vec;
};

// �ܳ� - ��Ծ�����ȼ��ֶ�
struct WeekActiveDegreeRewardLevelCfg
{
	WeekActiveDegreeRewardLevelCfg():level_max(0)
	{}

	int level_max;
	WeekActiveDegreeRewardCfg week_degree_reward_cfg[MAX_ACTIVE_DEGREE_WEEK_REWARD_NUM];
};

//ת�̴�������
struct TurntableTaskCfg
{
	TurntableTaskCfg() : seq(0), turntable_num(0) {}

	short seq;
	short param;
	short turntable_num;
};

struct TurntableRewardCfg
{
	TurntableRewardCfg() : seq(0) {}

	short seq;

	ItemConfigData item;
};

struct ActiveSystemOtherCfg
{
	ActiveSystemOtherCfg():one_click_level(0), one_click_open_time(0)
	{}

	int one_click_level;		// һ����ɿ����ȼ�
	int one_click_open_time;	//һ����ɿ���ʱ�俪��
};


class RandActivityManager;
class ActiveSystemConfig : public ILogicConfig
{
public:
	ActiveSystemConfig();
	virtual ~ActiveSystemConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const ActiveSystemCfg* GetActiveSystemCfg(int type, int level);
	const ActiveSystemSpecialCfg* GetSpecialActiveCfg(int type, int level);
	const ActiveSystemRewardCfg* GetActiveSystemRewardCfg(int level, int index);
	void GetActiveSystemRelatedCfgNum(RandActivityManager* ramgr, int seq, int count, const ItemConfigData * reward_num, std::vector<ItemConfigData> * reward_list);

	const WeekActiveDegreeTaskCfg* GetWeekActiveDegreeTaskCfg(int task_type) const;
	const WeekActiveDegreeRewardCfg* GetWeekActiveDegreeRewardCfg(int role_level, int reward_seq) const;

	const TurntableTaskCfg* GetTurntableActivityCfg(int activity_type);
	const std::vector<TurntableTaskCfg> & GetTurntableWeekCfg() { return m_turntable_week_task_cfg; }
	const TurntableRewardCfg* GetTurntableRewardCfg(int role_leve);

	int GetOneClickConsumeGold(int activie_type)const;
	const ActiveSystemOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitActiveSystemCfg(TiXmlElement *RootElement);
	int InitActiveLevelGroupCfg(TiXmlElement *RootElement);
	int InitActiveRelatedCfg(TiXmlElement *RootElement);

	int InitActiveCommonCfg(TiXmlElement *root_element, ActiveSystemCommonCfg & node_cfg);

	int InitWeekActiveDegreeTaskCfg(TiXmlElement *RootElement);
	int InitWeekActiveDegreeRewardfg(TiXmlElement *RootElement);

	int InitTurntableTaskCfg(TiXmlElement *RootElement);
	int InitTurntableLevelCfg(TiXmlElement *RootElement);
	int InitTurntableRewardCfg(TiXmlElement *RootElement);

	int InitOneClickListCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	ActiveSystemListCfg m_active_system_cfg[ACTIVE_SYSTEM_TYPE_MAX];				//��typeΪ�±�
	ActiveSystemSpecialListCfg m_special_active_cfg[ACTIVITY_SYSTEM_SPECIAL_TYPE_MAX];
	std::vector<ActiveSystemLevelGroupCfg> m_active_level_group_cfg;
	
	std::map<int, ActiveSystemRelatedActCfg> m_active_related_cfg;			//��ʱ����Ӫ���Ҫ���ض����� ��act_typeΪkey

	//�ܳ� -������Ϣ[��������]
	WeekActiveDegreeTaskCfg m_week_degree_task_cfg[ACTIVE_DEGREE_TYPE_MAX];

	//�ܳ� -��Ծ�Ƚ׶ν���[����seq - 1]
// 	int m_week_degree_reward_cfg_num;
	std::vector<WeekActiveDegreeRewardLevelCfg> m_week_degree_reward_level_cfg;

	std::map<int, TurntableTaskCfg> m_turntable_day_task_cfg;								// ÿ�ո��µĲ�������
	std::vector<TurntableTaskCfg> m_turntable_week_task_cfg;								// �ܻ�Ծ
	std::map<short, short> m_turntable_level_cfg;											// <��С�ȼ�, ������>
	std::map<short, std::map<int, TurntableRewardCfg> > m_turntable_reward_cfg;				//<������, <rate, cfg> >

	std::map<int, int> m_one_click_list_cfg;	// һ�����������Ҫ����Ԫ����, key-see: Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE, value-����Ԫ��
	ActiveSystemOtherCfg m_other_cfg;
};



#endif