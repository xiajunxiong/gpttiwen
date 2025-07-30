#ifndef __RAND_ACTIVITY_GOD_BEAST_ADVENT_CONFIG_HPP__
#define __RAND_ACTIVITY_GOD_BEAST_ADVENT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

#include <vector>

struct YSLSTaskRewardCfg
{
	YSLSTaskRewardCfg():like(0)
	{}

	int like;		// 好感度
	std::vector<ItemConfigData> reward_list;
};

struct YSLSTaskCfg
{
	YSLSTaskCfg():task_type(0),task_param(0)
	{}

	enum TASK_TYPE
	{
		TASK_TYPE_JING_JI_CHANG = 0,		//!< 竞技场
		TASK_TYPE_QIONG_DING_ZHI_ZHENG = 1,	//!< 穹顶之争
		TASK_TYPE_WU_FANG_LEI_TAI = 2,		//!< 五方擂台
		TASK_TYPE_XIAN_SHI_HUO_DONG = 3,	//!< 任意限时活动	[魔将入侵,家族答题,以一敌百,百鬼夜行,巨龙来袭,迷宫竞速,家族荣誉战,幻界战场,首席竞选,家族大乱斗,竞技对抗赛,勇闯地宫,丝绸之路]
		TASK_TYPE_CONSUME_TI_LI = 4,		//!< 使用体力
		TASK_TYPE_JING_YING_SHI_LIAN = 5,	//!< 精英试炼
		TASK_TYPE_YE_WAI_SHOU_LING = 6,		//!< 野外首领
		TASK_TYPE_YUAN_ZHENG_MO_ZHONG = 7,	//!< 远征魔冢
		TASK_TYPE_BEI_DOU_QI_XING = 8,		//!< 北斗七星
		TASK_TYPE_ONLINE_TIME = 9,			//!< 在线时长
		TASK_TYPE_WAN_LING_HUA_BEN = 10,	//!< 万灵话本

		TASK_TYPE_MAX,
	};

	int task_type;		//!< 任务类型
	int task_param;		//!< 任务完成条件
};

struct YSLSTaskGroupCfg
{
	YSLSTaskGroupCfg():task_group(0)
	{}

	int task_group;
	std::map<int, YSLSTaskCfg> task_cfg;				//任务配置, key-seq
	std::map<int, YSLSTaskRewardCfg> reward_cfg;		//奖励配置, key-seq 
};

struct YSLSTaskSectionCfg
{
	YSLSTaskSectionCfg():section_start(0),section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<YSLSTaskGroupCfg> task_group_cfg;
};

struct YSLSExchangeCfg
{
	YSLSExchangeCfg():unlock_integral(0),limit_type(0),limit_times(0),consume_item_id(0), consume_num(0)
	{}

	enum BUY_LIMIT
	{
		SERVER_LIMIT = 0,	// 全服限购
		DAY_LIMIT = 1,		// 每日限购
		ACTIVITY_LIMIT = 2,	// 活动限购
	};

	int unlock_integral;		// 解锁好感度条件
	int limit_type;				// 限购类型
	int limit_times;			// 限购次数
	int consume_item_id;		// 消耗道具id
	int consume_num;			// 消耗道具数量
	ItemConfigData reward;
};

struct YSLSExchangeSectionCfg
{
	YSLSExchangeSectionCfg():exchange_group(0)
	{}

	int exchange_group;
	std::map<int, YSLSExchangeCfg> exchange_cfg;		// 兑换配置,key-seq
};

struct YSLSFbSectionCfg
{
	YSLSFbSectionCfg() :section_start(0), section_end(0), secret_area_type(0)
	{}

	int section_start;
	int section_end;
	int secret_area_type;		// 副本
};

struct YSLSBuySectionCfg
{
	YSLSBuySectionCfg() :section_start(0), section_end(0),price(0)
	{}

	int section_start;
	int section_end;
	int price;				// 购买价格
};

struct YSLSRecycleCfg
{
	YSLSRecycleCfg():item_id(0),coin(0),gold(0),silver_coin(0),has_item(0)
	{}

	int item_id;		//要回收的道具
	int coin;			//铜币
	int gold;			//元宝
	int silver_coin;	//灵玉
	int has_item;		//判断列表是否有道具,0-item_list为空
	std::vector<ItemConfigData> item_list;	//道具
};

struct YSLSCycleCfg
{
	YSLSCycleCfg():type(0),start_time(0),end_time(0),exchange_group(0)
	{}

	int type;
	unsigned int start_time;			// 开始时间
	unsigned int end_time;			// 结束时间
	int exchange_group;		// 兑换组
};

struct YSLSOtherCfg
{
	YSLSOtherCfg() :is_more_change(0), integral_up(0)
	{}

	int is_more_change;		//全服限购数量上限
	int integral_up;		//好感度上限
};


class RandActivityManager;
class RandActivityYiShouLinShiConfig : public RandActivityConfig
{
public:
	RandActivityYiShouLinShiConfig();
	virtual ~RandActivityYiShouLinShiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetTaskParamCfg(int task_type)const;
	int GetTaskNumCfg()const { return (int)m_task_param_cfg.size(); }
	const YSLSTaskCfg* GetTaskCfg(RandActivityManager* ramgr, int seq, int task_group)const;
	const YSLSTaskRewardCfg* GetTaskRewardCfg(RandActivityManager* ramgr, int seq, int task_group)const;
	const YSLSExchangeCfg* GetExchangeCfg(int exchange_group, int seq)const;
	int GetFbType(RandActivityManager* ramgr)const;
	int GetBuyCfg(RandActivityManager* ramgr)const;
	const YSLSRecycleCfg* GetRecycleCfg(int item_id)const;		// 判断道具是不是活动内的道具
	const YSLSRecycleCfg* GetCurRecycleCfg(int exchange_group)const;	//获取对应奖励组周期的道具配置
	int GetStartTimeCfg(unsigned int& start_time, int& type)const;
	const YSLSOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const YSLSCycleCfg* GetCurCycleCfg(int seq)const;
	const std::map<int, YSLSRecycleCfg>& GetAllRecycleCfg()const { return m_recycle_cfg; }

private:
	int InitTaskCfg(TiXmlElement *RootElement);
	int InitExchangeCfg(TiXmlElement *RootElement);
	int InitFbCfg(TiXmlElement *RootElement);
	int InitBuyCfg(TiXmlElement *RootElement);
	int InitRecycleCfg(TiXmlElement *RootElement);
	int InitCycleCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<YSLSTaskSectionCfg> m_task_section_cfg;				// 任务配置
	std::map<int, int> m_task_param_cfg;					// key-任务类型,value--完成条件
	std::vector<YSLSExchangeSectionCfg> m_exchange_section_cfg;		// 兑换配置,key-seq
	std::vector<YSLSFbSectionCfg> m_fb_cfg;					// 副本配置
	std::vector<YSLSBuySectionCfg> m_buy_cfg;						// 购买配置
	std::map<int, YSLSRecycleCfg> m_recycle_cfg;			// 回收配置,key-兑换组
	std::vector<YSLSCycleCfg> m_cycle_time_cfg;						// 活动周期配置
	YSLSOtherCfg m_other_cfg;
};


#endif