#include "minigame_niulang_config.hpp"
#include "servercommon/servercommon.h"

MiniGameNiuLangConfig::MiniGameNiuLangConfig() : m_level_num(0)
{

}

MiniGameNiuLangConfig::~MiniGameNiuLangConfig()
{

}

bool MiniGameNiuLangConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("level_list", InitLevelCfg);

	return true;
}

int MiniGameNiuLangConfig::GetNpcSeq(int seq)
{
	if (seq >= 0 && seq < m_level_num)
	{
		return m_level_cfg[seq].npc_demo_seq;
	}

	return 0;
}

int MiniGameNiuLangConfig::GetMaxStage() const
{
	if (m_level_num > 0 && m_level_num <= MAX_LEVEL_CFG_NUM)
	{
		return m_level_cfg[m_level_num - 1].seq;
	}
	return 0;
}

int MiniGameNiuLangConfig::InitLevelCfg(TiXmlElement* root_element, std::string& err)
{
	TiXmlElement* dataElement = root_element->FirstChildElement("data");
	if (!dataElement) return -10000;

	int last_seq = 0;
	while (dataElement)
	{
		int seq = 0;
		if (!GetSubNodeValue(dataElement, "seq", seq) || last_seq + 1 != seq || seq >= MAX_LEVEL_CFG_NUM)
		{
			return -1;
		}
		last_seq = seq;
		MiniGameNiuLangLevelCfg& cfg = m_level_cfg[seq];
		cfg.seq = seq;
		if (!GetSubNodeValue(dataElement, "fight_list", cfg.npc_demo_seq))
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "monster_group_id", cfg.monster_group_id))
		{
			return -3;
		}

		dataElement = dataElement->NextSiblingElement();

		m_level_num = GetMax(seq + 1, m_level_num);
	}

	return 0;
}

