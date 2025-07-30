#ifndef __RA_SHAN_HAI_MET_CONFIG_HPP__
#define __RA_SHAN_HAI_MET_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"

class RandActivityManager;

namespace RAShanHaiMet
{
	struct OtherCfg
	{
		OtherCfg()
		{
			high_reward_buy_money = 0;
			cfg_ver = 0;
		}
		int high_reward_buy_money;
		int cfg_ver;
	};
	//ÿ�ս�������
	struct DailyCfg
	{
		DailyCfg()
		{
			section_day_begin = 0;

			act_day_index = 0;
			task_group_id = 0;
		}
		int section_day_begin;	//�ֶο�ʼ�Ŀ������� (����Ϊ1)

		int act_day_index;		//�����index(����Ϊ0)
		int task_group_id;		//������id
	
		std::vector<ItemConfigData> normal_reward_vec;		//ͨ������
		std::vector<ItemConfigData> unfinish_reward_vec;	//δ��ɽ���
		std::vector<ItemConfigData> extra_reward_vec;		//���⽱��
	};
	//�׶�����map<�����index, ÿ������>
	typedef std::map<int, DailyCfg> SectionCfgDayMap;
	//����ý׶�map<��������begin, �׶�����>
	typedef std::map<int, SectionCfgDayMap> SectionMap;
	//��������
	enum DailyRewardType
	{
		DAILY_REWARD_TYPE_NONE = 0,	//��
		DAILY_REWARD_TYPE_NORMAL,	//ͨ������
		DAILY_REWARD_TYPE_UNFINISH,	//δ��ɽ���
		DAILY_REWARD_TYPE_EXTRA,	//���⽱��			(����)
		DAILY_REWARD_TYPE_ALL,		//ͨ������+���⽱��	(����)

		DAILY_REWARD_TYPE_MAX,
	};

	//��������
	struct TaskCfg
	{
		TaskCfg()
		{
			task_group_id = 0;
			task_seq = 0;

			task_type = 0;
			task_param_1 = 0;
			task_param_2 = 0;
		}

		int task_group_id;		//������id
		int task_seq;			//�������

		int task_type;			//��������

		int task_param_1;		//�������
		int task_param_2;		//�������
	};
	//��������Ϣ<��������, ������Ϣ>
	typedef std::map<int, TaskCfg> TaskGroupCfg;

	//������id map<������id, ��������Ϣ>
	typedef std::map<int, TaskGroupCfg> TaskGroupIDMap;


};

class RAShanHaiMetConfig : public RandActivityConfig
{
public:
	RAShanHaiMetConfig();
	virtual ~RAShanHaiMetConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiMet::OtherCfg & GetOtherCfg() const { return m_other_cfg; };
	//����
	const RAShanHaiMet::DailyCfg * GetDailyCfg(RandActivityManager* ramgr, int act_day_index) const;

	virtual int GetConfigVer() const;

	//�������
	//��������Ϣ
	const RAShanHaiMet::TaskGroupCfg * GetTaskGroupCfg(int task_group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitTaskGroupCfg(TiXmlElement *RootElement);
	int InitDailyCfg(TiXmlElement *RootElement);


	RAShanHaiMet::OtherCfg m_other_cfg;
	RAShanHaiMet::SectionMap m_section_map;
	RAShanHaiMet::TaskGroupIDMap m_task_group_id_cfg;
};

#endif		//__RA_SHAN_HAI_JU_BAO_CONFIG_HPP__