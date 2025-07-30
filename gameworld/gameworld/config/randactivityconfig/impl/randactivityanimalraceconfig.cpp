#include "randactivityanimalraceconfig.hpp"

RandActivityAnimalRaceConfig::RandActivityAnimalRaceConfig()
{

}

RandActivityAnimalRaceConfig::~RandActivityAnimalRaceConfig()
{

}

bool RandActivityAnimalRaceConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("track_events", InitAnimalRaceCfg);
	LOAD_RA_CONFIG("open_time", InitTimeCfg);
	LOAD_RA_CONFIG("position", InitRolePlaceCfg);

	return true;
}

const RARolePlaceCfg * RandActivityAnimalRaceConfig::GetRolePlaceCfg(int index) const
{
	if (index < 0 || index >= MAX_ANIMAL_RACE_SC_NUM) return NULL;

	return &m_role_place_cfg[index];
}

int RandActivityAnimalRaceConfig::InitAnimalRaceCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	int seq = 0;
	if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0) return -1;
	m_animal_race_cfg.seq = seq;

	if (!GetSubNodeValue(data_element, "item_consume", m_animal_race_cfg.item_consume_id) || m_animal_race_cfg.item_consume_id < 0) return -2;

	return 0;
}

int RandActivityAnimalRaceConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "bet_upper_limit", m_other_cfg.bet_upper_limit) || m_other_cfg.bet_upper_limit <= 0) return -1;
	if (!GetSubNodeValue(data_element, "scene_id", m_other_cfg.scene_id)) return -2;

	return 0;
}

int RandActivityAnimalRaceConfig::InitTimeCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}

	if (!GetSubNodeValue(data_element, "start_time1", m_time_cfg.start_time1) || m_time_cfg.start_time1 < 0) return -1;
	if (!GetSubNodeValue(data_element, "end_time1", m_time_cfg.end_time1) || m_time_cfg.end_time1 <= m_time_cfg.start_time1) return -2;
	if (!GetSubNodeValue(data_element, "start_time2", m_time_cfg.start_time2) || m_time_cfg.start_time2 < 0 || m_time_cfg.start_time2 <= m_time_cfg.end_time1) return -3;
	if (!GetSubNodeValue(data_element, "end_time2", m_time_cfg.end_time2) || m_time_cfg.end_time2 < 0 || m_time_cfg.end_time2 < m_time_cfg.start_time2 || m_time_cfg.end_time2 < m_time_cfg.end_time1) return -4;

	return 0;
}

int RandActivityAnimalRaceConfig::InitRolePlaceCfg(TiXmlElement *RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -888;
	}
	while (NULL != data_element)
	{
		int id = 0;
		if (!GetSubNodeValue(data_element, "id", id) || id <= 0 || id > MAX_ANIMAL_RACE_SC_NUM) return -1;
		RARolePlaceCfg& node = m_role_place_cfg[id - 1];

		if (!GetSubNodeValue(data_element, "born_x", node.role_pos.x) || node.role_pos.x < 0) return -2;
		if (!GetSubNodeValue(data_element, "born_y", node.role_pos.y) || node.role_pos.y < 0) return -3;
		if (!GetSubNodeValue(data_element, "dir_y", node.role_dir)) return -4;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

