#ifndef __RAND_ACTIVITY_HUN_QI_FAN_LI_CONFIG_HPP__
#define __RAND_ACTIVITY_HUN_QI_FAN_LI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"

struct RAHunQiFanLiBasicCfg
{
	RAHunQiFanLiBasicCfg():rate(0)
	{}
	RAHunQiFanLiBasicCfg(int _rate, ItemConfigData _reward) :rate(_rate), reward(_reward)
	{}

	int rate;
	ItemConfigData reward;
};

struct RAHunQiFanLiBasicSectionCfg
{
	RAHunQiFanLiBasicSectionCfg():section_start(0), section_end(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	std::vector<RAHunQiFanLiBasicCfg> basic_cfg;
};

struct RAHunQiFanLiPhaseSectionCfg
{
	RAHunQiFanLiPhaseSectionCfg() :section_start(0), section_end(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	std::vector<std::pair<int, ItemConfigData> > reward_limit_cfg;	// <�齱����, ��ɽ�������>
};

struct RAHunQiFanLiChongZhiSectionCfg
{
	RAHunQiFanLiChongZhiSectionCfg() :section_start(0), section_end(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	std::vector<std::pair<int, int> > chong_zhi_cfg;	// <��ֵ���(����Ԫ����), ��ɸ���齱����>
};

class RandActivityHunQiFanLiConfig : public RandActivityConfig
{

public:
	RandActivityHunQiFanLiConfig();
	virtual ~RandActivityHunQiFanLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	// @����÷���ֵ��Ϊ��ʱ�Ż��used_flag[i]λ��1, ע������������ܻ�ı�����,���Դ���ʱ������������ٸ�ֵ��m_param
	const ItemConfigData* RandRewardCfg(RandActivityManager* ramgr, BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP>& used_flag, int* turntable_length = NULL) const;

	const std::pair<int, ItemConfigData>* GetPhaseRewardCfg(RandActivityManager* ramgr, int seq) const;
	const std::vector<std::pair<int, int> >* GetChongZhiCfg(RandActivityManager* ramgr) const;

private:
	int InitBasisCfg(TiXmlElement *RootElement, std::string& err);
	int InitPhaseRewardCfg(TiXmlElement *RootElement, std::string& err);
	int InitChongZhiCfg(TiXmlElement *RootElement, std::string& err);

	std::vector<RAHunQiFanLiBasicSectionCfg> m_basic_section_cfg;	// �齱��������
	std::vector<RAHunQiFanLiPhaseSectionCfg> m_phase_reward_cfg;	// �齱�����׶ν���
	std::vector<RAHunQiFanLiChongZhiSectionCfg> m_chong_zhi_cfg;	// ��ֵ����
};

#endif