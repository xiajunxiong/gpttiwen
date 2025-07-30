#ifndef __RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_CONFIG_HPP__
#define __RAND_ACTIVITY_DUAN_YANG_XIANG_NANG_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "item/itempool.h"

struct DYXNBasicCfg
{
	DYXNBasicCfg():task_type(0), param(0)
	{}

	enum TASK_TYPE
	{
		TASK_TYPE_JING_JI_CHANG = 0,		//!< 竞技场
		TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 穹顶之争
		TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< 五方擂台
		TASK_TYPE_XIAN_SHI_HUO_DONG = 3,	//!< 任意限时活动	[魔将入侵,家族答题,以一敌百,百鬼夜行,巨龙来袭,迷宫竞速,家族荣誉战,幻界战场,首席竞选,家族大乱斗,竞技对抗赛,勇闯地宫,丝绸之路,万灵话本]
		TASK_TYPE_CONSUME_TI_LI = 4,		//!< 使用体力
		TASK_TYPE_JING_YING_SHI_LIAN = 5,	//!< 精英试炼
		TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< 野外首领
		TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< 远征魔冢
		TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< 北斗七星
		TASK_TYPE_ONLINE_TIME = 9,			//!< 在线时长
		TASK_TYPE_WAN_LING_HUA_BEN = 10,	//!< 万灵话本

		TASK_TYPE_MAX,
	};

	int task_type;
	int param;
	std::vector<ItemConfigData> reward_list;
};

struct DYXNBasicSectionCfg
{
	DYXNBasicSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<DYXNBasicCfg> task_list;
};

struct DYXNRewardCfg
{
	DYXNRewardCfg():rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct DYXNRewardSectionCfg
{
	DYXNRewardSectionCfg() :section_start(0), section_end(0),total_rate(0)
	{}

	int section_start;
	int section_end;
	int total_rate;
	std::vector<DYXNRewardCfg> reward_cfg;
};


class RandActivityManager;
class RandActivityDuanYangXiangNangConfig : public RandActivityConfig
{
public:
	RandActivityDuanYangXiangNangConfig();
	virtual ~RandActivityDuanYangXiangNangConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const DYXNBasicCfg* GetTaskCfg(RandActivityManager* ramgr, int seq)const;
	int GetTaskNumCfg(RandActivityManager* ramgr)const;
	const DYXNRewardCfg* RandRewardCfg(RandActivityManager* ramgr)const;
	const std::vector<std::pair<ItemID, int> >& GetMakeCfg()const { return m_make_cfg; }

private:
	int InitBasicCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitMakeCfg(TiXmlElement *RootElement);

	std::vector<DYXNBasicSectionCfg> m_task_section_cfg;		// 任务配置
	std::vector<DYXNRewardSectionCfg> m_reward_section_cfg;		// 制作完成奖励配置
	std::vector<std::pair<ItemID, int> > m_make_cfg;	// 制作配置, 道具id和数量
};


#endif
