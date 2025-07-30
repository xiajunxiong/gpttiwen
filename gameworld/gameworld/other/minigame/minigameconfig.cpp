#include "minigameconfig.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/minigamedef.hpp"

MiniGameConfig::MiniGameConfig()
{

}

MiniGameConfig::~MiniGameConfig()
{

}

bool MiniGameConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("minigame_award", InitIndexCfg);
	LOAD_CONFIG_2("award_cfg_1", InitStageRewardCfg);
	LOAD_CONFIG_2("award_cfg_2", InitPhaseRewardCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

const MiniGameIndexCfg* MiniGameConfig::GetIndexCfg(int minigame_id) const
{
	IndexCfgMap::const_iterator it = m_index_cfg_map.find(minigame_id);
	if (it != m_index_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

bool MiniGameConfig::HasNextStage(int minigame_id, int clear_stage_num)
{
	const MiniGameIndexCfg* cfg = this->GetIndexCfg(minigame_id);
	if (NULL == cfg) return false;

	return NULL != this->GetStageRewardCfg(cfg->stage_reward_group, clear_stage_num + 1);
}

int MiniGameConfig::InitIndexCfg(TiXmlElement* root_element, std::string& err)
{
	TiXmlElement* data_element = root_element->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (data_element)
	{
		MiniGameIndexCfg cfg;
		if (!GetSubNodeValue(data_element, "minigame_ID", cfg.mini_game_id) || cfg.mini_game_id <= 0 || cfg.mini_game_id >= MINI_GAME_MAX)
		{
			err = STRING_SPRINTF("InitIndexCfg minigame_ID not found or invalid, minigame_ID[%d]", cfg.mini_game_id);
			return -1;
		}

		if (!GetSubNodeValue(data_element, "group_1", cfg.stage_reward_group) || cfg.stage_reward_group < 0)
		{
			err = STRING_SPRINTF("InitIndexCfg group_1 not found or invalid, group_1[%d]", cfg.stage_reward_group);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "group_2", cfg.phase_reward_group) || cfg.phase_reward_group < 0)
		{
			err = STRING_SPRINTF("InitIndexCfg group_2 not found or invalid, group_2[%d]", cfg.phase_reward_group);
			return -3;
		}

		if (!GetSubNodeValue(data_element, "minigame_name", cfg.mini_game_name))
		{
			err = STRING_SPRINTF("InitIndexCfg minigame_name not found or invalid, minigame_name[%s]", cfg.mini_game_name.c_str());
			return -4;
		}

		if (NULL != this->GetIndexCfg(cfg.mini_game_id))
		{
			err = STRING_SPRINTF("InitIndexCfg minigame_id[%d] duplicate", cfg.mini_game_id);
			return -5; 
		}
		
		m_index_cfg_map.insert(IndexCfgMap::value_type(cfg.mini_game_id, cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int MiniGameConfig::InitStageRewardCfg(TiXmlElement* root_element, std::string& err)
{
	TiXmlElement* data_element = root_element->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (data_element)
	{
		int minigame_group = 0;
		if (!GetSubNodeValue(data_element, "minigame_group", minigame_group) || minigame_group <= 0)
		{
			err = STRING_SPRINTF("InitStageRewardCfg [minigame_group] not found or invalid, minigame_group[%d]", minigame_group);
			return -1;
		}

		int stage_id = 0;
		if (!GetSubNodeValue(data_element, "seq", stage_id) || stage_id <= 0)
		{
			err = STRING_SPRINTF("InitStageRewardCfg [seq] not found or invalid, seq[%d]", stage_id);
			return -2;
		}

		MiniGameStageRewardCfg cfg;
		cfg.group_id = minigame_group;
		cfg.stage_id = stage_id;	

		TiXmlElement *list_element = data_element->FirstChildElement("item_list_list");
		if (NULL == list_element)
		{
			return -1000;
		}

		TiXmlElement *item_element = list_element->FirstChildElement("item_list");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -1002;
			}
			cfg.reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		StageKey stage_key(minigame_group, stage_id);
		if (m_stage_reward_map.find(stage_key) != m_stage_reward_map.end())
		{
			err = STRING_SPRINTF("InitStageRewardCfg stage_key duplicate, minigame_group[%d] seq[%d]", minigame_group, stage_id);
			return -2000;
		}
		m_stage_reward_map.insert(StageRewardMap::value_type(stage_key, cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int MiniGameConfig::InitPhaseRewardCfg(TiXmlElement* root_element, std::string& err)
{
	TiXmlElement* data_element = root_element->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (data_element)
	{
		int minigame_group = 0;
		if (!GetSubNodeValue(data_element, "minigame_group", minigame_group) || minigame_group <= 0)
		{
			err = STRING_SPRINTF("InitPhaseRewardCfg [minigame_group] not found or invalid, minigame_group[%d]", minigame_group);
			return -1;
		}

		int phase_seq = 0;
		if (!GetSubNodeValue(data_element, "seq", phase_seq) || phase_seq <= 0 || phase_seq >= MINI_GAME_PHASE_REWARD_MAX_SEQ)
		{
			err = STRING_SPRINTF("InitPhaseRewardCfg [seq] not found or invalid, seq[%d]", phase_seq);
			return -2;
		}

		int clear_stage_num = 0;
		if (!GetSubNodeValue(data_element, "need_stage_seq", clear_stage_num) || clear_stage_num <= 0)
		{
			err = STRING_SPRINTF("InitPhaseRewardCfg [seq] not found or invalid, need_stage_seq[%d]", clear_stage_num);
			return -2;
		}

		MiniGamePhaseRewardCfg cfg;
		cfg.group_id = minigame_group;
		cfg.phase_seq = phase_seq;
		cfg.clear_stage_num = clear_stage_num;

		TiXmlElement *item_element = data_element->FirstChildElement("reward_item");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				return -1002;
			}
			cfg.reward_list.push_back(item);
			item_element = item_element->NextSiblingElement();
		}

		PhaseKey phase_key(minigame_group, phase_seq);
		if (m_phase_reward_map.find(phase_key) != m_phase_reward_map.end())
		{
			err = STRING_SPRINTF("InitPhaseRewardCfg phase_key duplicate, minigame_group[%d] phase_seq[%d]", minigame_group, phase_seq);
			return -2000;
		}
		m_phase_reward_map.insert(PhaseRewardMap::value_type(phase_key, cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int MiniGameConfig::InitOtherCfg(TiXmlElement* root_element, std::string& err)
{
	TiXmlElement* data_element = root_element->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "mail_award", m_other_cfg.mail_award_stage_num) || m_other_cfg.mail_award_stage_num < 0)
	{
		return -1;
	}

	return 0;
}

const MiniGameStageRewardCfg* MiniGameConfig::GetStageRewardCfg(int mini_game_id, int stage_id)
{
	const MiniGameIndexCfg* index_cfg = this->GetIndexCfg(mini_game_id);
	if (NULL == index_cfg) return NULL;

	StageKey key(index_cfg->stage_reward_group, stage_id);
	return this->GetStageRewardCfg(key);
}

const MiniGameStageRewardCfg* MiniGameConfig::GetStageRewardCfg(const StageKey& key)
{
	StageRewardMap::iterator it = m_stage_reward_map.find(key);
	if (it != m_stage_reward_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const MiniGamePhaseRewardCfg* MiniGameConfig::GetPhaseRewardCfg(int mini_game_id, int phase_id)
{
	PhaseKey key(mini_game_id, phase_id);
	return this->GetPhaseRewardCfg(key);
}

const MiniGamePhaseRewardCfg* MiniGameConfig::GetPhaseRewardCfg(const PhaseKey& key)
{
	PhaseRewardMap::iterator it = m_phase_reward_map.find(key);
	if (it != m_phase_reward_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const MiniGameOtherCfg& MiniGameConfig::GetOtherCfg() const
{
	return m_other_cfg;
}

int MiniGameConfig::GetAwardStageNum()
{
	return m_other_cfg.mail_award_stage_num;
}

