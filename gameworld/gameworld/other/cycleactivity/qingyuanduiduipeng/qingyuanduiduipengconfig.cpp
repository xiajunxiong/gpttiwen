#include "qingyuanduiduipengconfig.hpp"
#include "npc/npcpool.h"
#include "checkresourcecenter.hpp"

QingYuanDuiDuiPengConfig::QingYuanDuiDuiPengConfig()
{
}

QingYuanDuiDuiPengConfig::~QingYuanDuiDuiPengConfig()
{
}

bool QingYuanDuiDuiPengConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("play_config", InitNpcCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("rank_reward", InitRankCfg);
	LOAD_CONFIG_2("reward_level", InitRewardLevelCfg);

	return true;
}

const QingYuanDuiDuiPengRankCfg * QingYuanDuiDuiPengConfig::GetRankCfg(int rank_index)
{
	if (rank_index < 0 || rank_index >= MAX_QINGYUAN_DUIDUIPENG_RANK_NUM)
		return NULL;

	std::map<int, QingYuanDuiDuiPengRankCfg>::iterator it = m_rank_cfg.upper_bound(rank_index);
	if (it == m_rank_cfg.end())
		return NULL;

	return &it->second;
}

const int QingYuanDuiDuiPengConfig::GetRewardTimeCfg(int match_couple)
{
	std::map<int, int>::iterator it = m_reward_cfg.find(match_couple);
	if (it == m_reward_cfg.end())
		return -1;

	return it->second;
}

int QingYuanDuiDuiPengConfig::InitNpcCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id) || npc_id <= 0)
		{
			errormsg = STRING_SPRINTF("[npc_id=%d] error, npc_id <= 0 ?", npc_id);
			return -1;
		}

		QingYuanDuiDuiPengNpcCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq < 0)
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, seq < 0 ?", cfg.seq);
			return -2;
		}
		if (!GetSubNodeValue(root_element, "pos_x", cfg.pos.x) || cfg.pos.x <= 0)
		{
			errormsg = STRING_SPRINTF("[pos_x=%d] error, pos_x <= 0 ?", cfg.pos.x);
			return -3;
		}
		if (!GetSubNodeValue(root_element, "pos_y", cfg.pos.y) || cfg.pos.y <= 0)
		{
			errormsg = STRING_SPRINTF("[pos_y=%d] error, pos_y <= 0 ?", cfg.pos.y);
			return -3;
		}

		m_npc_cfg[npc_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanDuiDuiPengConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "start_time", m_other_cfg.start_time) || m_other_cfg.start_time < 0)
	{
		errormsg = STRING_SPRINTF("[start_time=%d] error, is start_time < 0?", m_other_cfg.start_time);
		return -1;
	}

	if (!GetSubNodeValue(root_element, "end_time", m_other_cfg.end_time) || m_other_cfg.end_time > 24)
	{
		errormsg = STRING_SPRINTF("[end_time=%d] error, is end_time > 24?", m_other_cfg.start_time);
		return -2;
	}

	{
		TiXmlElement * list_element = root_element->FirstChildElement("reward_1_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[reward_1_list] error, NULL == list_element ?");
			return -3;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("reward_1");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -4;

			m_other_cfg.reward_1.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}
	{
		TiXmlElement * list_element = root_element->FirstChildElement("reward_2_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[reward_2_list] error, NULL == list_element ?");
			return -3;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("reward_2");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -4;

			m_other_cfg.reward_2.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}

	{
		TiXmlElement * list_element = root_element->FirstChildElement("reward_3_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[reward_3_list] error, NULL == list_element ?");
			return -3;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("reward_3");
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -4;

			m_other_cfg.reward_3.push_back(item);
			item_element = item_element->NextSiblingElement();
		}
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id))
	{
		errormsg = STRING_SPRINTF("[scene_id=%d] error, is not find data?", m_other_cfg.scene_id);
		return -5;
	}
	CheckResourceCenter::Instance().GetSceneCheck()->Add(m_other_cfg.scene_id, __FUNCTION__);

	if (!GetSubNodeValue(root_element, "during_time", m_other_cfg.during_time) || m_other_cfg.during_time <= 0)
	{
		errormsg = STRING_SPRINTF("[during_time=%d] error, m_other_cfg.during_time <= 0?", m_other_cfg.during_time);
		return -6;
	}

	if (!GetSubNodeValue(root_element, "reward_times", m_other_cfg.reward_times) || m_other_cfg.reward_times <= 0)
	{
		errormsg = STRING_SPRINTF("[reward_times=%d] error, m_other_cfg.reward_times <= 0?", m_other_cfg.reward_times);
		return -6;
	}

	return 0;
}

int QingYuanDuiDuiPengConfig::InitRankCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int rank_range_max = 0;
		if (!GetSubNodeValue(root_element, "rank_range_max", rank_range_max) || rank_range_max <= 0 || rank_range_max > MAX_QINGYUAN_DUIDUIPENG_RANK_NUM)
		{
			errormsg = STRING_SPRINTF("[rank_range_max=%d] error, is rank_range_max <= 0 || rank_range_max > 100?", rank_range_max);
			return -1;
		}

		TiXmlElement * list_element = root_element->FirstChildElement("item_list_list");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[item_list] error, NULL == list_element ?");
			return -2;
		}

		TiXmlElement * item_element = list_element->FirstChildElement("item_list");

		QingYuanDuiDuiPengRankCfg cfg;
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
				return -3;

			cfg.reward.push_back(item);

			item_element = item_element->NextSiblingElement();
		}

		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id) || cfg.title_id < 0)
		{
			errormsg = STRING_SPRINTF("[title_id=%d] error, title_id < 0 ?", cfg.title_id);
			return -4;
		}

		m_rank_cfg[rank_range_max] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanDuiDuiPengConfig::InitRewardLevelCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int play_time = 0;
		if (!GetSubNodeValue(root_element, "play_time", play_time) || play_time <= 0)
		{
			errormsg = STRING_SPRINTF("[play_time=%d] error, is play_time <= 0?", play_time);
			return -1;
		}

		int match_couple = 0;
		if (!GetSubNodeValue(root_element, "match_couple", match_couple) || match_couple <= 0)
		{
			errormsg = STRING_SPRINTF("[match_couple=%d] error, is match_couple <= 0?", match_couple);
			return -2;
		}

		m_reward_cfg[match_couple] = play_time;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
