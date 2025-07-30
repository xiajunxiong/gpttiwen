#ifndef __RA_SHAN_HAI_JU_BAO_CONFIG_HPP__
#define __RA_SHAN_HAI_JU_BAO_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"
#include "servercommon/roleactivity/rashanhaijubaoparam.hpp"

class RandActivityManager;

namespace RAShanHaiJuBao
{
	struct OtherCfg
	{
		OtherCfg()
		{
			draw_cost_item_id = 0;
			cost_num = 0;
		}
		ItemID draw_cost_item_id;
		int cost_num;
	};
	//奖品配置
	struct DrawItemCfg
	{
		DrawItemCfg()
		{
			section_day_begin = 0;

			phase_seq = 0;
			phase_act_day_begin = 0;

			draw_power = 0;
			is_brocast = 0;

			draw_index = 0;
			show_type = 0;
		}
		int section_day_begin;	//分段开始的开服天数

		int phase_seq;				//活动阶段
		int phase_act_day_begin;	//活动阶段 起始活动天数

		int draw_power;				//权重
		int is_brocast;				//传闻

		int draw_index;				//奖励序号
		ItemConfigData draw_item;	//奖励

		int show_type;				//客户端用
	};
	//奖池权重map<权重begin, 奖品配置>
	typedef std::map<int, DrawItemCfg> DrawPoolPowerMap;
	//奖池配置
	struct DrawPool
	{
		DrawPool()
		{
			phase_seq = 0;
			phase_act_day_begin = 0;

			power_count = 0;
		}
		int phase_seq;				//活动阶段
		int phase_act_day_begin;	//活动阶段 起始活动天数

		int power_count;
		DrawPoolPowerMap draw_pool;
	};
	//活动阶段奖池map<活动阶段, 奖池>
	typedef std::map<int, DrawPool> PhaseDrawPoolMap;
	//阶段配置
	struct SectionCfg
	{
		//奖池[活动阶段]
		PhaseDrawPoolMap phase_draw_pool_map;
	};
	//阶段配置map<开服天数begin, 奖品配置>
	typedef std::map<int, SectionCfg> SectionCfgMap;

	//额外奖励
	struct ExtraRewardCfg
	{
		ExtraRewardCfg()
		{
			extra_reward_index = 0;
			need_draw_num = 0;
		}
		int extra_reward_index;	//额外奖励index
		int need_draw_num;		//需求抽奖次数

		//奖励
		std::vector<ItemConfigData> reward_item_vec;
	};
	//额外奖励阶段配置
	struct ExtraPhaseCfg
	{
		ExtraPhaseCfg()
		{
			m_extra_reward_count = 0;
		}
		int m_extra_reward_count;
		ExtraRewardCfg m_extra_reward_cfg_[SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX];
	};
	
};

class RAShanHaiJuBaoConfig : public RandActivityConfig
{
public:
	RAShanHaiJuBaoConfig();
	virtual ~RAShanHaiJuBaoConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAShanHaiJuBao::OtherCfg & GetOtherCfg() const { return m_other_cfg; };
	const RAShanHaiJuBao::DrawItemCfg * GetRandDrawCfg(RandActivityManager* ramgr, int phase_seq) const;
	const RAShanHaiJuBao::ExtraRewardCfg * GetExtraRewardCfg(int phase_seq, int extra_reward_index) const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDrawCfg(TiXmlElement *RootElement);
	int InitExtraRewardCfg(TiXmlElement *RootElement);


	RAShanHaiJuBao::OtherCfg m_other_cfg;
	RAShanHaiJuBao::SectionCfgMap m_section_map;

	//额外奖励阶段配置[活动阶段]
	RAShanHaiJuBao::ExtraPhaseCfg m_extra_phase_cfg_[SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX + 1];
};

#endif		//__RA_SHAN_HAI_JU_BAO_CONFIG_HPP__