#include "randactivitybonusconfig.hpp"

#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"

RandActivityBonusConfig::RandActivityBonusConfig()
{
}

RandActivityBonusConfig::~RandActivityBonusConfig()
{
	std::vector<RABonusCfg>().swap(m_bonus_cfg_vec);
	for (std::map<int, std::vector<RABonusCfg> >::iterator it = n_bonus_cfg_map.begin(); it != n_bonus_cfg_map.end(); it++)
	{
		std::vector<RABonusCfg>().swap(it->second);
	}
}

bool RandActivityBonusConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("bonus_cfg", InitBonusCfg);
//	LOAD_CONFIG("other", InitOtherCfg);

	return true;
}

const RABonusCfg * RandActivityBonusConfig::GetBonusCfgBySeq(int seq)
{
	if(seq < 0 || seq >= (int)m_bonus_cfg_vec.size()) return NULL;

	return &m_bonus_cfg_vec[seq];
}

const std::vector<RABonusCfg>* RandActivityBonusConfig::GetBounsCfgByActivityId(int activity_id)
{
	std::map<int, std::vector<RABonusCfg> >::iterator it = n_bonus_cfg_map.find(activity_id);
	if(it != n_bonus_cfg_map.end()) return &it->second;

	return NULL;
}

const RABonusCfg * RandActivityBonusConfig::GetBonusCfg(int activity_id, int type)
{
	std::map<int, std::vector<RABonusCfg> >::iterator it = n_bonus_cfg_map.find(activity_id);
	if (it != n_bonus_cfg_map.end())
	{
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			if (type == it->second[i].type)
			{
				return &it->second[i];
			}
		}
	}

	return NULL;
}

int RandActivityBonusConfig::InitBonusCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	int last_seq = -1;
	while (NULL != data_element)
	{
		RABonusCfg node;
		if (!GetSubNodeValue(data_element, "seq", node.seq) || node.seq < 0 || node.seq >= MAX_BONUS_COUNT_NUM || node.seq != last_seq + 1) return -1;
		last_seq = node.seq;
		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0) return -2;
		if (!GetSubNodeValue(data_element, "activity_id", node.activity_id) || node.activity_id <= RAND_ACTIVITY_TYPE_BEGIN || node.activity_id >= RAND_ACTIVITY_TYPE_END) return -3;
		if (!GetSubNodeValue(data_element, "type_item", node.type_item) || node.type_item < 0) return -4;
		TiXmlElement *reward_element = data_element->FirstChildElement("reward_item");
		if (NULL != reward_element)
		{
			if (!node.reward.ReadConfig(reward_element)) return -5;
		}
		switch (node.activity_id)
		{
			case RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN:
			{
				if(node.type < JIAN_DAN_QIN_XIN_TYPE_COMMON || node.type >= JIAN_DAN_QIN_XIN_TYPE_MAX) return -100;
			}
			break;
		}
		if (!GetSubNodeValue(data_element, "section_start", node.section_start) || node.section_start <= 0) return -6;
		if (!GetSubNodeValue(data_element, "section_end", node.section_end) || node.section_end < 0) return -7;
		if (0 == node.section_end)
		{
			node.section_end = INT_MAX;
		}
		if (node.section_start > node.section_end)
		{
			return -8;
		}

		m_bonus_cfg_vec.push_back(node);
		n_bonus_cfg_map[node.activity_id].push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityBonusConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "reset", m_other_cfg.version_id) || m_other_cfg.version_id < 0) return -1;

	return 0;
}
