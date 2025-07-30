#ifndef __RAND_ACTIVITY_TIGER_AUSPICIOUS_SNOW_CONFIG_HPP__
#define __RAND_ACTIVITY_TIGER_AUSPICIOUS_SNOW_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"

// ��������
struct RATigerSpSonwRewardCfg
{
	RATigerSpSonwRewardCfg():seq(0), rate(0)
	{}

	int seq;
	int rate;
	ItemConfigData reward;
};

struct RATigerSpSonwRewardSectionCfg
{
	RATigerSpSonwRewardSectionCfg():section_start(0), section_end(0), total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<RATigerSpSonwRewardCfg> reward_list;
};

// �һ�����
struct RATigerSpSonwRechargeSectionCfg
{
	RATigerSpSonwRechargeSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<ItemConfigData> reward;
};

// ѩ������
struct RATigerSpSonwDriftCfg
{
	RATigerSpSonwDriftCfg() :seq(0), rate(0)
	{}

	int seq;
	int rate;
	std::vector<ItemConfigData> v_reward;
};

struct RATigerSpSonwDriftSectionCfg
{
	RATigerSpSonwDriftSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<RATigerSpSonwDriftCfg> reward_list;
};

// ��������
struct RAtigerSpSnowSceneCfg
{
	RAtigerSpSnowSceneCfg():scene_id(0), npc_id(0)
	{}

	int scene_id;
	int npc_id;
};

// ��������
struct RATigerSpSnowOtherCfg
{
	RATigerSpSnowOtherCfg():refresh_times(0), first_num(0), collect_times(0),
		consume_item_id_1(0), consume_item_id_2(0), consume_item_id_3(0), consume_item_id_4(0), consume_num(0), snowball_id(0)
	{}

	int refresh_times;			// ѩ��ˢ��ʱ��
	int first_num;				// ѩ��ˢ������
	int collect_times;			// ÿ�տ�ʰȡ����
	int consume_item_id_1;		// �һ����ĵ���
	int consume_item_id_2;
	int consume_item_id_3;
	int consume_item_id_4;
	int consume_num;			// ��������
	int snowball_id;			// ѩ��id
};


class RandActivityManager;
class RandActivityTigerAuspiciousSnowConfig : public RandActivityConfig
{
public:
	RandActivityTigerAuspiciousSnowConfig();
	virtual ~RandActivityTigerAuspiciousSnowConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const ItemConfigData * GetRandRewardCfg(RandActivityManager* ramgr, ARG_OUT int & seq)const;
	const RATigerSpSonwRechargeSectionCfg * GetRechargeCfg(RandActivityManager* ramgr)const;
	const RATigerSpSonwDriftCfg * GetDriftCfg(RandActivityManager* ramgr)const;
	const std::vector<RAtigerSpSnowSceneCfg> & GetAllSceneCfg()const { return m_scene_cfg; }
	const RATigerSpSnowOtherCfg & GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitRechargeCfg(TiXmlElement *RootElement);
	int InitSnowDriftCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RATigerSpSonwRewardSectionCfg> m_reward_section_cfg;
	std::vector<RATigerSpSonwRechargeSectionCfg> m_recharge_section_cfg;
	std::vector<RATigerSpSonwDriftSectionCfg> m_drift_section_cfg;
	std::vector<RAtigerSpSnowSceneCfg> m_scene_cfg;
	RATigerSpSnowOtherCfg m_other_cfg;
};

#endif