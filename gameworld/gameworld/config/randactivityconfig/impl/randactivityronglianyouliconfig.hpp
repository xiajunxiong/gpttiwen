#ifndef __RAND_ACTIVITY_RONG_LIAN_YOU_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_RONG_LIAN_YOU_LI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

namespace RARongLianYouLi
{
	enum RONG_LIAN_TYPE
	{
		RONG_LIAN_TYPE_PURPLE = 0,			// ��ɫ����
		RONG_LIAN_TYPE_ORANGE = 1,			// ��ɫ����
		RONG_LIAN_TYPE_RED	= 2,			// ��ɫ����

		RONG_LIAN_TYPE_MAX,
	};

};

struct RARongLianYouLiRewardCfg
{
	RARongLianYouLiRewardCfg():seq(0),attribute_type(0),parameter(0),reward_times(0)
	{}

	int seq;
	int attribute_type;			// ��������
	int parameter;
	int reward_times;			// ����������
	ItemConfigData reward;
};

struct RARongLianYouLiRewardSectionCfg
{
	RARongLianYouLiRewardSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RARongLianYouLiRewardCfg> reward;
};

struct RARongLianYouLiStageRewardCfg
{
	RARongLianYouLiStageRewardCfg():rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reawrd;			// �̶����� + �������
};

struct RARongLianYouLiStageRewardGroupCfg
{
	RARongLianYouLiStageRewardGroupCfg(): reward_group(0), task_num(0), total_rate(0)
	{}

	int reward_group;
	int task_num;
	int total_rate;
	std::vector<RARongLianYouLiStageRewardCfg> reward_lsit;
};

struct RARongLianYouLiStageRewardSectionCfg
{
	RARongLianYouLiStageRewardSectionCfg() : section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RARongLianYouLiStageRewardGroupCfg> reward_group;
};


class RandActivityManager;
class RandActivityRongLianYouLiConfig : public RandActivityConfig
{
public:
	RandActivityRongLianYouLiConfig();
	virtual ~RandActivityRongLianYouLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RARongLianYouLiRewardCfg> * GetRewardCfg(RandActivityManager * ramgr)const;
	const RARongLianYouLiStageRewardCfg * GetRandStageRewardCfg(RandActivityManager * ramgr, int task_num)const;
	const std::vector<int> GetTaskNumCfg(RandActivityManager * ramgr)const;

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitStageRewardCfg(TiXmlElement *RootElement);

	std::vector<RARongLianYouLiRewardSectionCfg> m_reward_section_cfg;
	std::vector<RARongLianYouLiStageRewardSectionCfg> m_stage_reward_section_cfg;
};

#endif