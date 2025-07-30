#ifndef __RA_COLORFUL_DOUBLE_CONFIG_HPP__
#define __RA_COLORFUL_DOUBLE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"

class RandActivityManager;

namespace RAColorfulDouble
{

	struct OtherCfg
	{
		OtherCfg()
		{
			act_type = 0;

			act_cost_type = 0;
			act_cost_num = 0;
			cfg_ver = 0;

			box_act_reward_type = 0;
			box_act_reward_num = 0;
		}
		//���伤������
		int act_type;		//��������

		int act_cost_type;	//������������
		int act_cost_num;	//������������
		int cfg_ver;		//ֱ�����ð汾

		int box_act_reward_type;//���������
		int box_act_reward_num;	//���������

		//��������
		enum ActType
		{
			ACT_TYPE_BOX = 1,	//���������
			ACT_TYPE_TOTAL,		//ȫ������

			ACT_TYPE_MAX,
		};
		//������������
		enum ActCostType
		{
			ACT_COST_TYPE_GOLD = 0,			//Ԫ��
			ACT_COST_TYPE_CMD_BUY = 1,		//ֱ��
			ACT_COST_TYPE_SILVER_COIN = 2,	//����(����)

			ACT_COST_TYPE_MAX,
		};

		//��������
		enum ActRewardType
		{
			ACT_REWARD_TYPE_NONE = 0,	//�޼����
			ACT_REWARD_TYPE_GOLD,		//Ԫ��
			ACT_REWARD_TYPE_SILVER_COIN,//����(����)
			ACT_REWARD_TYPE_COIN_BIND,	//ͭ��

			ACT_REWARD_TYPE_MAX,
		};
	};

	//��ѽ���
	struct FreeRewardCfg
	{
		FreeRewardCfg()
		{
			section_day_begin = 0;
		}

		int section_day_begin;	//�ֶο�ʼ�Ŀ������� (����Ϊ1)
		//��ѽ���
		std::vector<ItemConfigData> free_reward_vec;
	};
	//ÿ����ѽ����׶�map<�ֶο�ʼ�Ŀ�������, ����>
	typedef std::map<int, FreeRewardCfg> FreeSectionMap;

	//���佱��
	struct BosRewardCfg
	{
		BosRewardCfg()
		{
			box_seq = 0;
			section_day_begin = 0;

			need_act_open_day = 0;
			need_finish_task_num = 0;
		}
		int box_seq;	//����seq

		int section_day_begin;	//�ֶο�ʼ�Ŀ������� (����Ϊ1)

		//���佱��
		std::vector<ItemConfigData> box_reward_vec;

		//����
		int need_act_open_day;		//���������(��һ��Ϊ1)
		int need_finish_task_num;	//��ڼ����������
	};
	//����׶���Ϣ<������Ŀ���ʱ��, ���佱��>
	typedef std::map<int, BosRewardCfg> BoxSectionDayMap;
	//������Ϣmap<����seq, ����׶���Ϣ>
	typedef std::map<int, BoxSectionDayMap> BoxSeqMap;

	//������Ϣ
	struct TaskCfg
	{
		TaskCfg()
		{
			act_open_day = 0;
			task_index = 0;

			task_type = 0;
			task_param = 0;
		}

		int act_open_day;	//���������(��һ��Ϊ1)

		int task_index;		//����λ��

		int task_type;		//��������
		int task_param;		//�������
	};
	typedef std::vector<TaskCfg> TaskVec;

	//��������-����map<��������, ��������vec>
	typedef std::map<int, TaskVec> TaskTypeMap;

	//������Ϣ-�����map<�����, ��������-����map>
	typedef std::map<int, TaskTypeMap> TaskDayMap;

};

class RAColorfulDoubleConfig : public RandActivityConfig
{
public:
	RAColorfulDoubleConfig();
	virtual ~RAColorfulDoubleConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAColorfulDouble::OtherCfg& GetOtherCfg() const { return m_other_cfg; };

	const RAColorfulDouble::FreeRewardCfg* GetFreeRewardCfg(RandActivityManager* ramgr) const;
	const RAColorfulDouble::BosRewardCfg* GetBoxRewardCfg(RandActivityManager * ramgr, 
		unsigned int act_begin_time, int box_seq) const;

	const RAColorfulDouble::TaskVec* GetTaskVecByType(RandActivityManager* ramgr, int act_open_day, int task_type) const;


private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitFreeRewardCfg(TiXmlElement *RootElement);
	int InitBoxCfg(TiXmlElement *RootElement);
	int InitTaskCfg(TiXmlElement *RootElement);

	RAColorfulDouble::OtherCfg m_other_cfg;

	//ÿ����ѽ����׶�map<�ֶο�ʼ�Ŀ�������, ����>
	RAColorfulDouble::FreeSectionMap m_free_section_day_map;

	//������Ϣmap<����seq, ����׶���Ϣ>
	RAColorfulDouble::BoxSeqMap m_box_seq_map;

	//������Ϣ-�����map<�����, ��������-����map>
	RAColorfulDouble::TaskDayMap m_task_day_map;
};


#endif