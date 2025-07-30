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
	std::vector<std::pair<int, ItemConfigData> > reward_limit_cfg;	// <抽奖次数, 达成奖励道具>
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
	std::vector<std::pair<int, int> > chong_zhi_cfg;	// <充值金额(配置元宝数), 达成给予抽奖次数>
};

class RandActivityHunQiFanLiConfig : public RandActivityConfig
{

public:
	RandActivityHunQiFanLiConfig();
	virtual ~RandActivityHunQiFanLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	// @当获得返回值不为空时才会把used_flag[i]位置1, 注意这个操作可能会改变数据,可以传临时变量计算完成再赋值给m_param
	const ItemConfigData* RandRewardCfg(RandActivityManager* ramgr, BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP>& used_flag, int* turntable_length = NULL) const;

	const std::pair<int, ItemConfigData>* GetPhaseRewardCfg(RandActivityManager* ramgr, int seq) const;
	const std::vector<std::pair<int, int> >* GetChongZhiCfg(RandActivityManager* ramgr) const;

private:
	int InitBasisCfg(TiXmlElement *RootElement, std::string& err);
	int InitPhaseRewardCfg(TiXmlElement *RootElement, std::string& err);
	int InitChongZhiCfg(TiXmlElement *RootElement, std::string& err);

	std::vector<RAHunQiFanLiBasicSectionCfg> m_basic_section_cfg;	// 抽奖奖池配置
	std::vector<RAHunQiFanLiPhaseSectionCfg> m_phase_reward_cfg;	// 抽奖次数阶段奖励
	std::vector<RAHunQiFanLiChongZhiSectionCfg> m_chong_zhi_cfg;	// 充值配置
};

#endif