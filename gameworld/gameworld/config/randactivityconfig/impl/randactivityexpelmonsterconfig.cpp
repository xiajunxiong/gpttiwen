#include "randactivityexpelmonsterconfig.hpp"

RandActivityExpelMonsterConfig::RandActivityExpelMonsterConfig()
{

}

RandActivityExpelMonsterConfig::~RandActivityExpelMonsterConfig()
{
	std::vector<ExpelMonsterRewardSectionCfg>().swap(m_reward_section_cfg);
}

bool RandActivityExpelMonsterConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("dispel_consume", InitDispelConsumeCfg);
	LOAD_RA_CONFIG("dispel_reward", InitDispelRewardCfg);
	LOAD_RA_CONFIG("monster_coordinate", InitMonsterCoordinate);

	return true;
}

const ExpelMonsterConsumeCfg * RandActivityExpelMonsterConfig::GetConsumeCfg(int seq) const
{
	std::map<int, ExpelMonsterConsumeCfg>::const_iterator it = m_consume_cfg.find(seq);
	if (it != m_consume_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const ExpelMonsterRewardCfg * RandActivityExpelMonsterConfig::GetRewardCfg(int role_level, RandActivityManager * ramgr)const
{
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT);
	std::vector<ExpelMonsterRewardSectionCfg>::const_reverse_iterator it = m_reward_section_cfg.rbegin();
	for (; m_reward_section_cfg.rend() != it; ++it)
	{
		if (it->section_end >= act_real_open_day && act_real_open_day >= it->section_start)
		{
			break;
		}
	}
	if (m_reward_section_cfg.rend() == it)
	{
		it = m_reward_section_cfg.rbegin();
	}

	int act_open_day = this->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_DISCOUNT_STORE);
	std::vector<ExpelMonsterRewardOpenDayCfg>::const_reverse_iterator it2 = it->open_day_cfg.rbegin();
	for (; it->open_day_cfg.rend() != it2; ++it2)
	{
		if (it2->day_end >= act_open_day && act_open_day >= it2->day_start)
		{
			break;
		}
	}
	if (it->open_day_cfg.rend() == it2)
	{
		it2 = it->open_day_cfg.rbegin();
	}

	static std::vector<ExpelMonsterRewardCfg> temp_reward;
	int total_rate = 0;
	temp_reward.clear();
	std::vector<ExpelMonsterRewardCfg>::const_reverse_iterator it3 = it2->reward_cfg.rbegin();
	for (; it2->reward_cfg.rend() != it3; ++it3)
	{
		if (it3->grade_start <= role_level && it3->grade_end >= role_level)
		{
			temp_reward.push_back(*it3);
			total_rate += it3->rate;
		}
	}

	int rand_num = RandomNum(total_rate);
	for (std::vector<ExpelMonsterRewardCfg>::const_iterator reward_it = temp_reward.begin(); temp_reward.end() != reward_it;++reward_it)
	{
		if (rand_num < reward_it->rate)
		{
			return &(*reward_it);
		}
		rand_num -= reward_it->rate;
	}

	return NULL;
}

const ExpelMonsterCoordinateCfg * RandActivityExpelMonsterConfig::GetMonsterCoordinateCfg(int seq) const
{
	std::map<int, ExpelMonsterCoordinateCfg>::const_iterator it = m_cooddinate_cfg.find(seq);
	if (it != m_cooddinate_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

int RandActivityExpelMonsterConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "reward_frequency", m_other_cfg.reward_frequency)) return -1;

	return 0;
}

int RandActivityExpelMonsterConfig::InitDispelConsumeCfg(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		ExpelMonsterConsumeCfg node_cfg;

		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(root_element, "consume_type", node_cfg.consume_type) || node_cfg.consume_type < 0 || node_cfg.consume_type > 1) return -2;
		if (!GetSubNodeValue(root_element, "need_gold", node_cfg.need_gold) || node_cfg.need_gold <= 0) return -3;

		TiXmlElement *node_element = root_element->FirstChildElement("item");
		if (node_element == NULL)return -3;

		node_cfg.item.ReadConfig(node_element);

		m_consume_cfg[node_cfg.seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityExpelMonsterConfig::InitDispelRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)return -999;

	int last_section = 0;
	int last_time = 0;
	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -999;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -998;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}

		if (section_end < section_start) return -997;

		if (section_start != last_section)
		{
			m_reward_section_cfg.push_back(ExpelMonsterRewardSectionCfg(section_start, section_end));
			last_section = section_start;
			last_time = 0;
		}

		int start_time = 0, end_time= 0;
		if (!GetSubNodeValue(data_element, "start_time", start_time) || start_time <= 0) return -889;
		if (!GetSubNodeValue(data_element, "end_time", end_time) || end_time < 0) return -888;
		if (0 == end_time)
		{
			end_time = MAX_INT;
		}

		if (end_time < start_time) return -887;

		if (start_time != last_time)
		{
			m_reward_section_cfg.rbegin()->open_day_cfg.push_back(ExpelMonsterRewardOpenDayCfg(start_time, end_time));
			last_time = start_time;
		}

		ExpelMonsterRewardCfg node_cfg;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "grade_start", node_cfg.grade_start)) return -2;
		if (!GetSubNodeValue(data_element, "grade_end", node_cfg.grade_end)) return -3;
		if (!GetSubNodeValue(data_element, "seq1", node_cfg.seq1) || node_cfg.seq1 < 0) return -4;
		if (!GetSubNodeValue(data_element, "hearsay", node_cfg.hearsay) || node_cfg.hearsay < 0) return -5;
		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0) return -6;

		if (node_cfg.grade_end == 0)
		{
			node_cfg.grade_end = MAX_INT;
		}

		LOAD_ITEM_VETOR_NODE("reward_item_list", "reward_item", node_cfg.item);

		m_reward_section_cfg.rbegin()->open_day_cfg.rbegin()->reward_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityExpelMonsterConfig::InitMonsterCoordinate(TiXmlElement *RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;

	while (NULL != root_element)
	{
		ExpelMonsterCoordinateCfg node_cfg;

		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq)) return -1;
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id)) return -2;
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id)) return -3;
		if (!GetSubNodeValue(root_element, "x", node_cfg.monster_pos.x)) return -4;
		if (!GetSubNodeValue(root_element, "y", node_cfg.monster_pos.y)) return -5;

		m_cooddinate_cfg[node_cfg.seq] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

