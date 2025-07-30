#include "randactivityservercompetitionconfig.hpp"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"
#include "servercommon/struct/global/couragechallengerankdef.hpp"
#include "servercommon/rankdef.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "world.h"

RandActivityServerCompetitionConfig::RandActivityServerCompetitionConfig()
{
}

RandActivityServerCompetitionConfig::~RandActivityServerCompetitionConfig()
{
}

bool RandActivityServerCompetitionConfig::Init(TiXmlElement * RootElement, std::string * err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("timestamp", InitVersionTimestampCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("activity_reward", InitRewardCfg);

	return true;
}

const RASeverCompetitionRewardCfg * RandActivityServerCompetitionConfig::GetRewardCfg(RandActivityManager * ramgr, int rank_type, int seq) const
{
	if(NULL == ramgr || rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX || seq < 0 || seq >= MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM) return NULL;

	
	time_t real_server_open_timestamp = World::GetInstWorld()->GetRealServerOpenTime(ramgr->GetUniqueServerID());
	int version = this->GetVersion(real_server_open_timestamp);
	if (version < 0 || version >= ARRAY_ITEM_COUNT(m_competition_reward_list))
	{
		return NULL;
	}

	const std::map<long long, std::map<long long, RASeverCompetitionRewardCfg> > & reward_cfg = m_competition_reward_list[version];
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	for (std::map<long long, std::map<long long, RASeverCompetitionRewardCfg> >::const_iterator section_it = reward_cfg.begin(); section_it != reward_cfg.end(); ++section_it)
	{
		int section_start = 0, section_end = 0;
		this->GetInfoByKey(section_it->first, &section_start, &section_end);
		if (section_start <= act_real_open_day && section_end >= act_real_open_day)
		{
			long long type_key = this->GetTypeKey(rank_type, seq);
			std::map<long long, RASeverCompetitionRewardCfg>::const_iterator type_it = section_it->second.find(type_key);
			if (type_it != section_it->second.end())
			{
				return &type_it->second;
			}
		}
	}

	return NULL;
}

void RandActivityServerCompetitionConfig::GetRewardMapCfgByRankType(RandActivityManager * ramgr, int rank_type, std::vector<RASeverCompetitionRewardCfg> & rank_reward_list) const
{
	if (NULL == ramgr || rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX) return;

	time_t real_server_open_timestamp = World::GetInstWorld()->GetRealServerOpenTime(ramgr->GetUniqueServerID());
	int version = this->GetVersion(real_server_open_timestamp);
	if (version < 0 || version >= ARRAY_ITEM_COUNT(m_competition_reward_list))
	{
		return ;
	}

	const std::map<long long, std::map<long long, RASeverCompetitionRewardCfg> > & reward_cfg = m_competition_reward_list[version];
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	for (std::map<long long, std::map<long long, RASeverCompetitionRewardCfg> >::const_iterator section_it = reward_cfg.begin(); section_it != reward_cfg.end(); ++section_it)
	{
		int section_start = 0, section_end = 0;
		this->GetInfoByKey(section_it->first, &section_start, &section_end);
		if (section_start <= act_real_open_day && section_end >= act_real_open_day)
		{
			for (std::map<long long, RASeverCompetitionRewardCfg>::const_iterator type_it = section_it->second.begin(); type_it != section_it->second.end(); type_it++)
			{
				int get_rank_type = 0, seq = 0;
				this->GetInfoByKey(type_it->first, &get_rank_type, &seq);
				if(get_rank_type != rank_type) continue;

				rank_reward_list.push_back(type_it->second);
			}
		}
	}

	return;
}

const std::map<int, RASeverCompetitionTimeCfg>* RandActivityServerCompetitionConfig::GetCompetitionMap(RandActivityManager * ramgr) const
{
	if(NULL == ramgr) return NULL;

	time_t real_server_open_timestamp = World::GetInstWorld()->GetRealServerOpenTime(ramgr->GetUniqueServerID());
	int version = this->GetVersion(real_server_open_timestamp);
	if (version < 0 || version >= ARRAY_ITEM_COUNT(m_competition_map))
	{
		return NULL;
	}

	const std::map<long long, std::map<int, RASeverCompetitionTimeCfg> > & map_cfg = m_competition_map[version];
	int act_real_open_day = this->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	for (std::map<long long, std::map<int, RASeverCompetitionTimeCfg> >::const_iterator section_it = map_cfg.begin(); section_it != map_cfg.end(); ++section_it)
	{
		int section_start = 0, section_end = 0;
		this->GetInfoByKey(section_it->first, &section_start, &section_end);
		if (section_start <= act_real_open_day && section_end >= act_real_open_day)
		{
			return &section_it->second;
		}
	}

	return NULL;
}

int RandActivityServerCompetitionConfig::GetVersion(unsigned int timestamp) const
{
	std::map<unsigned int, int>::const_reverse_iterator it = m_competition_version_map.rbegin();
	for (; it != m_competition_version_map.rend(); ++it)
	{
		if (timestamp >= it->first)
		{
			return it->second;
		}
	}

	return -1;
}

bool RandActivityServerCompetitionConfig::IsHasVersion(int version) const
{
	std::map<unsigned int, int>::const_iterator it = m_competition_version_map.begin();
	for (; it != m_competition_version_map.end(); ++it)
	{
		if (it->second == version)
		{
			return true;
		}
	}

	return false;
}

int RandActivityServerCompetitionConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	
	TiXmlElement *list_element = data_element->FirstChildElement("day_reward_item_list");
	if (NULL == list_element)
	{
		return -2;
	}
	TiXmlElement *item_element = list_element->FirstChildElement("day_reward_item");
	if (NULL == item_element)
	{
		return -3;
	}
	while (NULL != item_element)
	{
		ItemConfigData item;
		if (!item.ReadConfig(item_element))
		{
			return -4;
		}

		m_other_cfg.day_reward_list.push_back(item);
		item_element = item_element->NextSiblingElement();
	}
	if (m_other_cfg.day_reward_list.empty())
	{
		return -5;
	}

	return 0;
}

int RandActivityServerCompetitionConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	std::set<int> version_set;
	while (NULL != data_element)
	{
		int version = 0;
		if (!GetSubNodeValue(data_element, "version", version) || version < 0 || version >= ARRAY_ITEM_COUNT(m_competition_reward_list)) return -777;


		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) return -1;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -2;
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}
		if (section_start > section_end) return -3;
		long long section_key = this->GetSectionKey(section_start, section_end);

		RASeverCompetitionRewardCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rank_type", node_cfg.rank_type) || node_cfg.rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || node_cfg.rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX) return -4;
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM) return -5;

		long long type_key = this->GetTypeKey(node_cfg.rank_type, node_cfg.seq);
		std::map<long long, std::map<long long, RASeverCompetitionRewardCfg> >::iterator it = m_competition_reward_list[version].find(section_key);
		if (it != m_competition_reward_list[version].end())
		{
			std::map<long long, RASeverCompetitionRewardCfg>::iterator type_it = it->second.find(type_key);
			if (type_it != it->second.end())
			{
				return -6;
			}
		}
		if (!GetSubNodeValue(data_element, "start_time", node_cfg.start_time) || node_cfg.start_time <= 0) return -7;
		if (!GetSubNodeValue(data_element, "continuou_times", node_cfg.continuou_times) || node_cfg.continuou_times <= 0) return -88;

		std::map<long long, std::map<int, RASeverCompetitionTimeCfg> >::iterator section_it = m_competition_map[version].find(section_key);
		if (section_it != m_competition_map[version].end())
		{
			std::map<int, RASeverCompetitionTimeCfg>::iterator rank_it = section_it->second.find(node_cfg.rank_type);
			if (rank_it != section_it->second.end() && (rank_it->second.start_time != node_cfg.start_time || rank_it->second.continuou_times != node_cfg.continuou_times))
			{
				return -8;
			}
		}
		RASeverCompetitionTimeCfg time_cfg;
		time_cfg.start_time = node_cfg.start_time;
		time_cfg.continuou_times = node_cfg.continuou_times;

		if (!GetSubNodeValue(data_element, "reward_type", node_cfg.reward_type) || node_cfg.reward_type <= SERVER_COMPETITION_REWARD_TYPE_BEGIN || 
			node_cfg.reward_type >= SERVER_COMPETITION_REWARD_TYPE_MAX) return -9;
		
		if (!GetSubNodeValue(data_element, "parameter_1", node_cfg.param1) || node_cfg.param1 <= 0) return -10;
		if (!GetSubNodeValue(data_element, "parameter_2", node_cfg.param2) || node_cfg.param2 < 0) return -11;

		UNSTD_STATIC_CHECK(MAX_COURAGE_CHALLENGE_RANK_NUM <= PERSON_RANK_MAX_SIZE);
		switch (node_cfg.reward_type)
		{
		case SERVER_COMPETITION_REWARD_TYPE_RANK:
		{
			if(node_cfg.param2 > MAX_COURAGE_CHALLENGE_RANK_NUM) return -12;		//当前排名存储最少的是锢魔之塔,排名不可超过该值
		}
		break;
		case SERVER_COMPETITION_REWARD_TYPE_SCORE:
		case SERVER_COMPETITION_REWARD_TYPE_COURAGE_CHALLENGE:
		{
			if(node_cfg.param1 > node_cfg.param2) return -13;
		}
		break;
		case SERVER_COMPETITION_REWARD_TYPE_COLOSSEUM_RANK:
		{
			if(node_cfg.param2 > node_cfg.param1) return -14;
		}
		break;
		default:
			return -15;
		}

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_node)
		{
			return -16;
		}

		TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -100;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}
		int is_broadcast = 0;
		if (!GetSubNodeValue(data_element, "is_broadcast", is_broadcast)) return -17;
		node_cfg.is_broadcast = (0 != is_broadcast) ? true : false;

		if (!GetSubNodeValue(data_element, "parameter_3", node_cfg.param3) || node_cfg.param3 < 0) return -18;

		m_competition_reward_list[version][section_key][type_key] = node_cfg;
		m_competition_map[version][section_key][node_cfg.rank_type] = time_cfg;
		version_set.insert(version);
		data_element = data_element->NextSiblingElement();
	}

	for (std::set<int>::const_iterator it = version_set.begin(); it != version_set.end(); ++it)
	{
		if (!this->IsHasVersion(*it))
		{
			return -888;
		}
	}

	return 0;
}

int RandActivityServerCompetitionConfig::InitVersionTimestampCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int version = 0;
		if (!GetSubNodeValue(data_element, "version", version) || version < 0 || version >= ARRAY_ITEM_COUNT(m_competition_reward_list)) return -1;

		unsigned int ver_timestamp = 0;
		if (!GetSubNodeValue(data_element, "ver_timestamp", ver_timestamp) || ver_timestamp < 0) return -2;

		m_competition_version_map[ver_timestamp] = version;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityServerCompetitionConfig::GetSectionKey(int section_start, int section_end)
{
	RAServerCompetitionKey info_key;
	info_key.param[0] = section_start;
	info_key.param[1] = section_end;

	return info_key.unique_key;
}

long long RandActivityServerCompetitionConfig::GetTypeKey(int rank_type, int seq)const
{
	RAServerCompetitionKey info_key;
	info_key.param[0] = rank_type;
	info_key.param[1] = seq;

	return info_key.unique_key;
}

void RandActivityServerCompetitionConfig::GetInfoByKey(long long _info_key, int * info_param1, int * info_param2) const
{
	if(NULL == info_param1 || NULL == info_param2) return;

	RAServerCompetitionKey info_key;
	info_key.unique_key = _info_key;
	*info_param1 = info_key.param[0];
	*info_param2 = info_key.param[1];
}
