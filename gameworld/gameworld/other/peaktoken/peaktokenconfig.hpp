#ifndef __PEAK_TOKEN_CONFIG_HPP__
#define __PEAK_TOKEN_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/peaktokendef.hpp"

#include "gamedef.h"



class PeakTokenConfig : public ILogicConfig
{
public:
	//����
	struct OtherCfg
	{
		OtherCfg()
		{
			act_duration_s = 0;
			notice_interval_s = 0;

			game_time_s = 0;
			role_reward_times = 0;

			item_can_use_times = 0;
		}

		int act_duration_s;		//�����ʱ��(s)
		int notice_interval_s;	//����ʱ����(s)

		int game_time_s;		//��Ϸʱ��(s)
		int role_reward_times;	//��ҽ�������

		int item_can_use_times;	//��Ʒ��ʹ�ô���
	};

	//������
	struct RewardCfg
	{
		RewardCfg()
		{
			group_id = 0;
			power = 0;
			is_broadcast = 0;
		}
		int group_id;

		int power;				//Ȩ��
		ItemConfigData item;	//����
		int is_broadcast;		//�㲥
	};

	//������ ����map<Ȩ��_begin, ��������Ʒ>
	typedef std::map<int, RewardCfg>  RewardPool;
	struct GroupCfg
	{
		GroupCfg()
		{
			group_id = 0;

			total_power = 0;
		}
		int group_id;

		int total_power;	//������Ȩ��
		RewardPool item_pool;
	};
	//������id map<������id, ������>
	typedef std::map<int, GroupCfg>  GroupIDMap;

public:
	PeakTokenConfig();
	virtual ~PeakTokenConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const OtherCfg & GetOtherCfg() const { return m_other_cfg; };

	const RewardCfg * GetGroupRandCfg(int group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);


	OtherCfg m_other_cfg;

	GroupIDMap m_group_id_map;
};
#endif
