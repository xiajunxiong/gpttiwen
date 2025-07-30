#include "randactivitytianmingdivinationconfig.hpp"

#include "engineadapter.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "world.h"

RandActivityTianMingDivinationConfig::RandActivityTianMingDivinationConfig()
	:m_tianming_free_chou_times(0), m_tianming_reward_section_count(0), m_tianming_gold_consume_max_add_lot_times(0)
{

}

RandActivityTianMingDivinationConfig::~RandActivityTianMingDivinationConfig()
{

}

bool RandActivityTianMingDivinationConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("tianming_jiangchi", InitTianMingDivinationRewardCfg);
	LOAD_RA_CONFIG("tianming_consume", InitTianMingDivinationGoldConsumeCfg);
	LOAD_RA_CONFIG("other", InitTianMingDivinationOtherCfg);

	return true;
}

int RandActivityTianMingDivinationConfig::GetTianMingDivinationRewardMaxSeq(RandActivityManager* ramgr)const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	for (int j = 0; j < m_tianming_reward_section_count; ++j)
	{
		if (act_real_open_day >= m_tianming_reward_section_cfg_list[j].section_start &&
			act_real_open_day <= m_tianming_reward_section_cfg_list[j].section_end)
		{
			return m_tianming_reward_section_cfg_list[j].tianming_reward_max_seq;
		}
	}

	return 0;
}

const RATianMingDivinationRewardCfg * RandActivityTianMingDivinationConfig::GetTianMingDivinationRewardCfg(RandActivityManager* ramgr, int seq)const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	for (int j = 0; j < m_tianming_reward_section_count; ++j)
	{
		if (act_real_open_day >= m_tianming_reward_section_cfg_list[j].section_start &&
			act_real_open_day <= m_tianming_reward_section_cfg_list[j].section_end)
		{
			if (seq > 0 && seq <= m_tianming_reward_section_cfg_list[j].tianming_reward_max_seq && seq <= RAND_ACTIVITY_TIANMING_DIVINATION_REWARD_MAX_SEQ)
			{
				return &m_tianming_reward_section_cfg_list[j].tianming_reward_cfg_list[seq];
			}

			break;
		}
	}

	return NULL;
}

const RATianMingDivinationGoldConsumeCfg * RandActivityTianMingDivinationConfig::GetTianMingDivinationGoldConsumeCfg(int times)const
{
	if (times < 0 || times > m_tianming_gold_consume_max_add_lot_times || times > RAND_ACTIVITY_TIANMING_DIVINATION_TOTAL_MAX_ADD_LOT_TIMES)
	{
		return NULL;
	}

	return &m_tianming_gold_consume_cfg_list[times];
}

int RandActivityTianMingDivinationConfig::GetTianMingDivinationSeqWithLotIndex(RandActivityManager* ramgr, int lot_index)const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	for (int j = 0; j < m_tianming_reward_section_count; ++j)
	{
		if (act_real_open_day >= m_tianming_reward_section_cfg_list[j].section_start &&
			act_real_open_day <= m_tianming_reward_section_cfg_list[j].section_end)
		{
			if (lot_index >= 0 && lot_index < m_tianming_reward_section_cfg_list[j].tianming_reward_add_lot_seq_count && lot_index < RA_TIANMING_LOT_COUNT)
			{
				return m_tianming_reward_section_cfg_list[j].tianming_reward_add_lot_seq_list[lot_index];
			}

			break;
		}
	}

	return 0;
}

int RandActivityTianMingDivinationConfig::GetTianMingDivinationLotIndexWithSeq(RandActivityManager* ramgr, int seq) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	for (int j = 0; j < m_tianming_reward_section_count; ++j)
	{
		if (act_real_open_day >= m_tianming_reward_section_cfg_list[j].section_start &&
			act_real_open_day <= m_tianming_reward_section_cfg_list[j].section_end)
		{
			for (int lot_index = 0; lot_index < RA_TIANMING_LOT_COUNT; ++lot_index)
			{
				if (m_tianming_reward_section_cfg_list[j].tianming_reward_add_lot_seq_list[lot_index] == seq)
				{
					return lot_index;
				}
			}

			break;
		}
	}

	return -1;
}

int RandActivityTianMingDivinationConfig::GetTianMingDivinationTotalWeight(RandActivityManager* ramgr) const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);

	for (int j = 0; j < m_tianming_reward_section_count; ++j)
	{
		if (act_real_open_day >= m_tianming_reward_section_cfg_list[j].section_start &&
			act_real_open_day <= m_tianming_reward_section_cfg_list[j].section_end)
		{
			return m_tianming_reward_section_cfg_list[j].tianming_reward_total_weight;
		}
	}

	return 0;
}

int RandActivityTianMingDivinationConfig::GetActOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	if (NULL == ramgr)
	{
		return 0;
	}

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	int act_real_open_day = GetDayIndex(activity_begin_time, cur_time) + 1; // 活动开启天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityTianMingDivinationConfig::GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	if (NULL == ramgr)
	{
		return 0;
	}

	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int act_real_open_day = GetDayIndex(open_server_timestamp, activity_begin_time) + 1; // 活动开启时的开服天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityTianMingDivinationConfig::InitTianMingDivinationRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}
	m_tianming_reward_section_count = 0;
	int pre_end_day = 0; // 写死，从第1天开始配
	while (NULL != data_element)
	{
		int section_start = 0;
		int section_end = 0;
		bool has_section_cfg = true;

		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}

		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
			pre_end_day = 0;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		int pre_index = m_tianming_reward_section_count - 1;
		if (pre_index < 0)
		{
			pre_index = 0;
		}

		if (section_start != m_tianming_reward_section_cfg_list[pre_index].section_start ||
			section_end != m_tianming_reward_section_cfg_list[pre_index].section_end)
		{
			if (section_start != pre_end_day + 1 || section_end < section_start)
			{
				return -1;
			}

			if (m_tianming_reward_section_count >= RAND_ACTIVITY_SECTION_MAX_COUNT)
			{
				return -1002;
			}

			pre_end_day = section_end;
			++m_tianming_reward_section_count;
		}

		RATianMingDivinationRewardSectionCfg &section_cfg = m_tianming_reward_section_cfg_list[m_tianming_reward_section_count - 1];
		section_cfg.section_start = section_start;
		section_cfg.section_end = section_end;

		if (section_cfg.tianming_reward_max_seq >= RAND_ACTIVITY_TIANMING_DIVINATION_REWARD_MAX_SEQ)
		{
			return -1000;
		}

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq != section_cfg.tianming_reward_max_seq + 1)
		{
			return -1;
		}

		RATianMingDivinationRewardCfg & tianming_reward_cfg = section_cfg.tianming_reward_cfg_list[seq];
		tianming_reward_cfg.seq = seq;
		section_cfg.tianming_reward_max_seq = seq;

		TiXmlElement *reward_item_element = data_element->FirstChildElement("reward_item");
		if (NULL == reward_item_element)
		{
			return -2;
		}

		if (!tianming_reward_cfg.reward_item.ReadConfig(reward_item_element))
		{
			return -3;
		}

		if (!GetSubNodeValue(data_element, "is_rare", tianming_reward_cfg.is_rare) || tianming_reward_cfg.is_rare < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "weight", tianming_reward_cfg.weight) || tianming_reward_cfg.weight <= 0)
		{
			return -4;
		}

		section_cfg.tianming_reward_total_weight += tianming_reward_cfg.weight;

		int can_add_lot = -1;
		if (!GetSubNodeValue(data_element, "can_add_lot", can_add_lot) || can_add_lot < 0 || can_add_lot > 1)
		{
			return -5;
		}
		tianming_reward_cfg.can_add_lot = (0 != can_add_lot);

		if (tianming_reward_cfg.can_add_lot)
		{
			if (section_cfg.tianming_reward_add_lot_seq_count >= RA_TIANMING_LOT_COUNT)
			{
				return -3000;
			}

			section_cfg.tianming_reward_add_lot_seq_list[section_cfg.tianming_reward_add_lot_seq_count] = tianming_reward_cfg.seq;
			++section_cfg.tianming_reward_add_lot_seq_count;
		}

		data_element = data_element->NextSiblingElement();
	}

	for (int i = 0; i < m_tianming_reward_section_count; ++i)
	{
		if (m_tianming_reward_section_cfg_list[i].tianming_reward_total_weight <= 0)
		{
			return -2000;
		}
	}

	return 0;
}

int RandActivityTianMingDivinationConfig::InitTianMingDivinationGoldConsumeCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	m_tianming_gold_consume_max_add_lot_times = -1;
	int last_add_consume_gold = 0;
	int last_chou_consume_gold = 0;

	while (NULL != data_element)
	{
		if (m_tianming_gold_consume_max_add_lot_times >= RAND_ACTIVITY_TIANMING_DIVINATION_TOTAL_MAX_ADD_LOT_TIMES)
		{
			return -2000;
		}

		int times = -1;
		if (!GetSubNodeValue(data_element, "lot_add_times", times) || times != m_tianming_gold_consume_max_add_lot_times + 1)
		{
			return -1;
		}

		RATianMingDivinationGoldConsumeCfg & tianming_gold_consume_cfg = m_tianming_gold_consume_cfg_list[times];
		tianming_gold_consume_cfg.lot_add_times = times;
		m_tianming_gold_consume_max_add_lot_times = times;

		if (!GetSubNodeValue(data_element, "add_consume_gold", tianming_gold_consume_cfg.add_consume_gold) || tianming_gold_consume_cfg.add_consume_gold < 0 ||
			tianming_gold_consume_cfg.add_consume_gold < last_add_consume_gold)
		{
			return -2;
		}
		last_add_consume_gold = tianming_gold_consume_cfg.add_consume_gold;

		if (!GetSubNodeValue(data_element, "chou_consume_gold", tianming_gold_consume_cfg.chou_consume_gold) || tianming_gold_consume_cfg.chou_consume_gold <= 0 ||
			tianming_gold_consume_cfg.chou_consume_gold < last_chou_consume_gold)
		{
			return -3;
		}
		last_chou_consume_gold = tianming_gold_consume_cfg.chou_consume_gold;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityTianMingDivinationConfig::InitTianMingDivinationOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		if (!GetSubNodeValue(data_element, "day_times", m_other_cfg.day_times) || m_other_cfg.day_times <= 0) return -1;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}
