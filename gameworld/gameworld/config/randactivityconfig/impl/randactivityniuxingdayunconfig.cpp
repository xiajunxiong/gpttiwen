#include "randactivityniuxingdayunconfig.hpp"
#include "servercommon/activitydef.hpp"

RandActivityNiuXingDaYunConfig::RandActivityNiuXingDaYunConfig()
{
}

RandActivityNiuXingDaYunConfig::~RandActivityNiuXingDaYunConfig()
{
	for (std::map<long long, std::vector<RANiuXingDaYunConfig> >::iterator it = m_subject_group.begin(); it != m_subject_group.end(); it++)
	{
		std::vector<RANiuXingDaYunConfig>().swap(it->second);
	}
}

bool RandActivityNiuXingDaYunConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("gift_configure", InitNiuXingDaYunCfg);

	return true;
}

const RANiuXingDaYunConfig * RandActivityNiuXingDaYunConfig::RandSubject(int randactivity_openday) const
{
	for (std::map<long long, std::vector<RANiuXingDaYunConfig> >::const_iterator it = m_subject_group.begin(); it != m_subject_group.end(); it++)
	{
		int section_start = 0;
		int section_end = 0;
		GetSectionByKey(it->first, &section_start, &section_end);
		if (section_start <= randactivity_openday && section_end >= randactivity_openday)
		{
			if (it->second.size() > 0)
			{
				int rand_cfg = rand() % it->second.size();
				return &(*(it->second.begin() + rand_cfg));
			}
		}
	}
	return NULL;
}

const RANiuXingDaYunConfig * RandActivityNiuXingDaYunConfig::GetNXDYSubjectCfg(int id) const
{
	std::map<int, RANiuXingDaYunConfig>::const_iterator it =  m_type_subject_map.find(id);
	if (it != m_type_subject_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RandActivityNiuXingDaYunConfig::InitNiuXingDaYunCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < 0) return -1;

		RANiuXingDaYunConfig &node_cfg = m_type_subject_map[type];
		node_cfg.type = type;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0) return -2;
		if (!GetSubNodeValue(data_element, "customs_introduce_id", node_cfg.customs_introduce_id) || node_cfg.customs_introduce_id < 0) return -3;

		TiXmlElement * itemlist_element = data_element->FirstChildElement("reward_item_list");
		if (itemlist_element != NULL)
		{
			TiXmlElement * reward_itemt_element = itemlist_element->FirstChildElement("reward_item");
			while(reward_itemt_element != NULL)
			{
				ItemConfigData reward_cfg;
				if (!reward_cfg.ReadConfig(reward_itemt_element))
				{
					return -4;
				}
				node_cfg.reward_vec.push_back(reward_cfg);
				reward_itemt_element = reward_itemt_element->NextSiblingElement();
			}
		}

		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start < 0) return -2;
		int section_end = 0;
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) return -3;

		if (section_end == 0)
		{
			section_end = MAX_INT;
		}

		long long key = GetKeyBySection(section_start, section_end);

		m_subject_group[key].push_back(node_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long RandActivityNiuXingDaYunConfig::GetKeyBySection(int section_start, int section_end) const
{
	KeyChangeUn un_key;
	un_key.section[0] = section_start;
	un_key.section[1] = section_end;
	return un_key.key;
}

void RandActivityNiuXingDaYunConfig::GetSectionByKey(long long key, int * section_start, int * section_end) const
{
	KeyChangeUn un_key;
	un_key.key = key;
	*section_start = un_key.section[0];
	*section_end = un_key.section[1];
}
