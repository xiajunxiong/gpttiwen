#include "randactivityjoyseeksfavorsconfig.hpp"

RandActivityJoySeeksFavorsConfig::RandActivityJoySeeksFavorsConfig()
{

}

RandActivityJoySeeksFavorsConfig::~RandActivityJoySeeksFavorsConfig()
{

}

bool RandActivityJoySeeksFavorsConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("seek_pets", InitSeekPetsCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);

	return true;
}

const JoySeeksFavorsCfg * RandActivityJoySeeksFavorsConfig::GetJoySeeksFavorsCfg(int level, bool is_win) const
{
	for (std::map<int, JoySeeksFavorsWDCfg>::const_iterator it = m_joy_seeks_favor_cfg.begin(); it != m_joy_seeks_favor_cfg.end(); ++it)
	{
		if (it->second.grade_start <= level && it->second.grade_end > level)
		{
			if (is_win)
			{
				return &it->second.wd_list[JoySeeksFavorsWDCfg::DW_TYPE_WIN];
			}
			else
			{
				return &it->second.wd_list[JoySeeksFavorsWDCfg::DW_TYPE_LOSE];
			}
		}
	}

	return NULL;
}

int RandActivityJoySeeksFavorsConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	if (NULL == RootElement)return -10000;

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)return -20000;

	if (!GetSubNodeValue(data_element, "start_time1", m_ohter_cfg.start_time1))return -1;
	if (!GetSubNodeValue(data_element, "end_time1", m_ohter_cfg.end_time1))return -2;

	if (!GetSubNodeValue(data_element, "start_time2", m_ohter_cfg.start_time2))return -3;
	if (!GetSubNodeValue(data_element, "end_time2", m_ohter_cfg.end_time2))return -4;

	if (!GetSubNodeValue(data_element, "dr_time", m_ohter_cfg.dr_time))return -5;


	return 0;
}

int RandActivityJoySeeksFavorsConfig::InitSeekPetsCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)return -10000;

	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)return -20000;

	while (NULL != data_element)
	{
		int grade_start = 0;
		if (!GetSubNodeValue(data_element, "grade_start", grade_start))return -1;

		int grade_end = 0;
		if (!GetSubNodeValue(data_element, "grade_end", grade_end))return -2;

		int key = this->GetKeyByStartAndEnd(grade_start, grade_end);

		JoySeeksFavorsWDCfg & node = m_joy_seeks_favor_cfg[key];
		node.grade_start = grade_start;

		if (grade_end <= 0)
		{
			grade_end = MAX_INT;
		}

		node.grade_end = grade_end;

		int victory_or_defeat = 0;
		if (!GetSubNodeValue(data_element, "victory_or_defeat", victory_or_defeat) || victory_or_defeat < JoySeeksFavorsWDCfg::DW_TYPE_WIN || victory_or_defeat > JoySeeksFavorsWDCfg::DW_TYPE_LOSE)return -3;
		JoySeeksFavorsCfg & n_node = node.wd_list[victory_or_defeat];
		n_node.victory_or_defeat = victory_or_defeat;

		if (!GetSubNodeValue(data_element, "seq", n_node.seq))return -4;

		if (!GetSubNodeValue(data_element, "seq1", n_node.seq1))return -5;

		TiXmlElement * reward_element = data_element->FirstChildElement("reward_item");
		if (reward_element == NULL)return -6;
		n_node.reward.ReadConfig(reward_element);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
