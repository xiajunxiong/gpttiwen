#ifndef __RAND_ACTIVITY_TIAN_MING_DIVINATION_CONFIG_HPP__
#define __RAND_ACTIVITY_TIAN_MING_DIVINATION_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/ratianmingdivinationparam.hpp"
#include "item/itembase.h"

// 天命卜卦-奖池配置
struct RATianMingDivinationRewardCfg
{
	RATianMingDivinationRewardCfg() : seq(0), is_rare(0), weight(0), can_add_lot(false) {}

	int seq;
	ItemConfigData reward_item;
	int is_rare;
	int weight;
	bool can_add_lot;

};

struct RATianMingDivinationRewardSectionCfg
{
	RATianMingDivinationRewardSectionCfg() : section_start(0), section_end(0), tianming_reward_total_weight(0), tianming_reward_max_seq(0),
		tianming_reward_add_lot_seq_count(0)
	{
		memset(tianming_reward_add_lot_seq_list, 0, sizeof(tianming_reward_add_lot_seq_list));
	}

	int section_start;
	int section_end;

	int tianming_reward_total_weight;								// 天命卜卦，配置总权重
	int tianming_reward_max_seq;									// 天命卜卦, 奖池最大下标
	RATianMingDivinationRewardCfg tianming_reward_cfg_list[RAND_ACTIVITY_TIANMING_DIVINATION_REWARD_MAX_SEQ + 1]; // 天命卜卦奖励配置信息

	int tianming_reward_add_lot_seq_count;							// 天命卜卦，竹签数量
	int tianming_reward_add_lot_seq_list[RA_TIANMING_LOT_COUNT];	// 天命卜卦, 可加注奖品下标数组
};

// 天命卜卦-元宝消耗
struct RATianMingDivinationGoldConsumeCfg
{
	RATianMingDivinationGoldConsumeCfg() : lot_add_times(-1), add_consume_gold(-1), chou_consume_gold(0) {}

	int lot_add_times;
	int add_consume_gold;
	int chou_consume_gold;

};

struct RATianMingDivinationOtherCfg
{
	RATianMingDivinationOtherCfg() : day_times(0){}
	int day_times;
};

class RandActivityManager;

class RandActivityTianMingDivinationConfig : public RandActivityConfig
{
public:
	RandActivityTianMingDivinationConfig();
	virtual ~RandActivityTianMingDivinationConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int GetTianMingDivinationRewardMaxSeq(RandActivityManager* ramgr) const;

	int GetTianMingDivinationGoldConsumeMaxAddLotTimes()const { return m_tianming_gold_consume_max_add_lot_times; }
	const RATianMingDivinationRewardCfg * GetTianMingDivinationRewardCfg(RandActivityManager* ramgr, int seq)const;
	const RATianMingDivinationGoldConsumeCfg * GetTianMingDivinationGoldConsumeCfg(int times)const;
	int GetTianMingDivinationSeqWithLotIndex(RandActivityManager* ramgr, int lot_index)const;
	int GetTianMingDivinationLotIndexWithSeq(RandActivityManager* ramgr, int seq)const;
	int GetTianMingDivinationFreeChouTimes() const { return m_tianming_free_chou_times; }
	int GetTianMingDivinationTotalWeight(RandActivityManager* ramgr)const;

	int GetActOpenDay(RandActivityManager* ramgr, int activity_type)const; 				//当前时间	    到	活动开启时间天数
	int GetActRealOpenDay(RandActivityManager* ramgr, int activity_type)const;			//活动开启时间	到	开服时间天数

	const RATianMingDivinationOtherCfg* GetOtherCfg()const { return &m_other_cfg; }
private:
	int InitTianMingDivinationRewardCfg(TiXmlElement *RootElement);
	int InitTianMingDivinationGoldConsumeCfg(TiXmlElement *RootElement);
	int InitTianMingDivinationOtherCfg(TiXmlElement *RootElement);

	int m_tianming_free_chou_times;									// 天命卜卦, 每天免费卜卦次数

	int m_tianming_reward_section_count;
	RATianMingDivinationRewardSectionCfg m_tianming_reward_section_cfg_list[RAND_ACTIVITY_SECTION_MAX_COUNT];

	int m_tianming_gold_consume_max_add_lot_times;					// 天命卜卦, 最大加注次数
	RATianMingDivinationGoldConsumeCfg m_tianming_gold_consume_cfg_list[RAND_ACTIVITY_TIANMING_DIVINATION_TOTAL_MAX_ADD_LOT_TIMES + 1];	// 天命卜卦元宝消耗信息

	RATianMingDivinationOtherCfg m_other_cfg;
};

#endif	//__RAND_ACTIVITY_TIAN_MING_DIVINATION_CONFIG_HPP__