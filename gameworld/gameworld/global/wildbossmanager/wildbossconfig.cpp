#include "wildbossconfig.hpp"
#include "servercommon/wildbossdef.hpp"

#include "item/itempool.h"
#include "npc/npcpool.h"
#include "checkresourcecenter.hpp"

WildBossConfig::WildBossConfig()
{
}

WildBossConfig::~WildBossConfig()
{
	std::vector<WildBossItemCfg>().swap(m_boss_item_cfg);
}

bool WildBossConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("level_group", InitLevelGroupCfg);
	LOAD_CONFIG("reward_group", InitRewardCfg);
	LOAD_CONFIG("time_group", InitRefreshTimeCfg);
	LOAD_CONFIG_2("monster_list", InitMonsterCfg);	//怪物列表与怪物配置需要放在一些配置读取之后
	LOAD_CONFIG_2("monster", InitWildBossCfg);
	LOAD_CONFIG("other", InitOtherCfg);	
	
	return true;
}

const std::vector<WildBossItemCfg> & WildBossConfig::GetAllWildBossItemCfg()
{
	return m_boss_item_cfg;
}

const WildBossItemCfg * WildBossConfig::GetWildBossItemCfg(int seq)
{
	if(seq < 0 || seq >= (int)m_boss_item_cfg.size() || m_boss_item_cfg.empty()) return NULL;

	return &m_boss_item_cfg[seq];
}

const WildBossOtherCfg & WildBossConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const WildBossMonsterCfg * WildBossConfig::GetMonsterCfg(int npc_number)
{
	std::map<int, WildBossMonsterCfg>::const_iterator it = m_monster_list.find(npc_number);
	if (it == m_monster_list.end())
	{
		return NULL;
	}
	return &it->second;
}

const WildBossMonsterCfg * WildBossConfig::GetMonsterCfgByNpcId(int npc_monster_id)
{
	for (std::map<int, WildBossMonsterCfg>::const_iterator it = m_monster_list.begin(); it != m_monster_list.end(); ++it)
	{
		if (it->second.npc_monster_id == npc_monster_id)
		{
			return &it->second;
		}
	}
	return NULL;
}

bool WildBossConfig::IsItemNotice(int item_id)
{
	if(m_other_cfg.check_item_boss_notice_list.empty()) return false;

	if (m_other_cfg.check_item_boss_notice_list.end() == m_other_cfg.check_item_boss_notice_list.find(item_id))
	{
		return false;
	}

	return true;
}

void WildBossConfig::GetRandRewardItemByRewardIdList(std::vector<int> reward_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward)
{
	for (int i = 0; i < (int) reward_id.size(); ++i)
	{
		this->GetRandReward(reward_id[i], out_reward, coin, luck_coin);
	}
}

void WildBossConfig::GetRandRewardItem(int scene_id, int monster_npc_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward, bool use_dec_reward)
{
	const WildBossMonsterCfg * cfg = this->GetMonsterCfgByNpcId(monster_npc_id);
	if (cfg == NULL)
	{
		return ;
	}

	const std::vector<int > * reward_id = &cfg->reward_id_vec;
	if (use_dec_reward)
	{
		reward_id = &cfg->dec_reward_id_vec;
	}

	this->GetRandRewardItemByRewardIdList(*reward_id, coin, luck_coin, out_reward);
}

void WildBossConfig::GetPokerRewardItem(int scene_id, int monster_npc_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward, bool use_dec_reward)
{
	const WildBossMonsterCfg * cfg = this->GetMonsterCfgByNpcId(monster_npc_id);
	if (cfg == NULL) return;

	int reward_id = cfg->poker_reward;
	if (use_dec_reward)
	{
		reward_id = cfg->dec_poker_reward;
	}
	this->GetPokerRewardItemByRewardId(reward_id, coin, luck_coin, out_reward);
}

void WildBossConfig::GetPokerRewardItemByRewardId(int & reward_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward)
{
	this->GetRandReward(reward_id, out_reward, coin, luck_coin);
}

int WildBossConfig::GetMonsterIdBySceneId(int scene_id)
{
	return m_scens_to_npc[scene_id];
}


void WildBossConfig::GetRandReward(int reward_id, std::vector<ItemConfigData>& out_list, int *coin, int *luck_coin)
{
	std::map<int, WildBossRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_id);
	if (iter == m_rewards_cfg.end())
	{
		return;
	}

	int r = rand() % WildBossRewardCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
	{
		if (r < iter->second.group_reward[i].rate)
		{
			if (iter->second.group_reward[i].coin == 0)
			{
				out_list.push_back(iter->second.group_reward[i].reward);
			}
			else
			{
				*coin += iter->second.group_reward[i].coin;
				*luck_coin += iter->second.group_reward[i].luck_coin;
			}
			break;
		}

		r -= iter->second.group_reward[i].rate;
	}
}

Posi WildBossConfig::GetRandPosi(int rand_pos_group_id)
{
	std::map<int, WildBossPosCfg > ::iterator iter = m_pos_cfg.find(rand_pos_group_id);
	if (iter == m_pos_cfg.end())
	{
		return Posi();
	}

	int r = rand() % WildBossPosCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
	{
		if (r < iter->second.group_cfg[i].rate)
		{
			return iter->second.group_cfg[i].pos;
		}

		r -= iter->second.group_cfg[i].rate;
	}

	return Posi();
}

int WildBossConfig::GetRefreshInterval(int group_id)
{
	std::map<int, WildBossRefresgTimeCfg > ::iterator iter = m_refresh_time_cfg.find(group_id);
	if (iter == m_refresh_time_cfg.end())
	{
		return 3600;
	}

	int r = rand() % WildBossRefresgTimeCfg::MAX_RATE_COUNT;

	for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
	{
		if (r < iter->second.group_cfg[i].rate)
		{
			return iter->second.group_cfg[i].refresh_interval;
		}

		r -= iter->second.group_cfg[i].rate;
	}

	return 3600;
}

int WildBossConfig::GetLevelGroupCfg(int level_group_id)
{
	std::map<int, int>::iterator it = m_level_group_map.find(level_group_id);
	if (it == m_level_group_map.end()) return -1;

	return it->second;
}

int WildBossConfig::InitWildBossCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	memset(m_scens_to_npc, 0, sizeof(m_scens_to_npc));

	int next_seq = 0;
	while (NULL != root_element)
	{
		WildBossItemCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq != next_seq)
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, [seq] != next_seq[%d]?", cfg.seq, next_seq);
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id) || cfg.scene_id <= 0)
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, scene_id[%d] <= 0?", cfg.seq, cfg.scene_id);
			return -2;
		}
		CheckResourceCenter::Instance().GetSceneCheck()->Add(cfg.scene_id, __FUNCTION__);
		if (!GetSubNodeValue(root_element, "level_group", cfg.level_group_id) || m_level_group_map.end() == m_level_group_map.find(cfg.level_group_id))
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "ordinary_number", cfg.ordinary_number) || m_monster_list.end() == m_monster_list.find(cfg.ordinary_number))
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, ordinary_number[%d] not find?", cfg.seq, cfg.ordinary_number);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "wild_number", cfg.wild_number) || (0 != cfg.wild_number && m_monster_list.end() == m_monster_list.find(cfg.wild_number)))
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, wild_number[%d] not find?", cfg.seq, cfg.wild_number);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "wild_rate", cfg.wild_rate) || cfg.wild_rate < 0 || cfg.wild_rate > WildBossItemCfg::MAX_WILD_BOSS_RATE_COUNT)
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, wild_rate[%d] < 0 || > %d?", cfg.seq, cfg.wild_rate, WildBossItemCfg::MAX_WILD_BOSS_RATE_COUNT);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "refresh_time", cfg.refresh_time_group_id) || m_refresh_time_cfg.end() == m_refresh_time_cfg.find(cfg.refresh_time_group_id))
		{
			errormsg = STRING_SPRINTF("[seq=%d] error, refresh_time[%d] not find?", cfg.seq, cfg.refresh_time_group_id);
			return -9;
		}

		m_boss_item_cfg.push_back(cfg);
		m_scens_to_npc[cfg.scene_id] = 1;
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_boss_item_cfg.size() > MAX_WILD_BOSS_NUM)
	{
		return -10080;
	}

	return 0;
}

int WildBossConfig::InitMonsterCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	std::set<int> npc_monster_list;
	while (NULL != root_element)
	{
		int npc_number = 0;
		if (!GetSubNodeValue(root_element, "npc_number", npc_number) || npc_number <= 0 || m_monster_list.end() != m_monster_list.find(npc_number))
		{
			errormsg = STRING_SPRINTF("[id=%d] error, [npc_number] <= 0 || repeat?", npc_number);
			return -1;
		}
		WildBossMonsterCfg node_cfg;
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_monster_id))
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, [npc_id] not find?", npc_number);
			return -2;
		}
		const ServerNPCInfoCfg* cfg = NPCPOOL->GetServerNPCInfo(node_cfg.npc_monster_id);
		if (NULL == cfg || SERVER_NPC_TYPE_MONSTER != cfg->server_npc_type)
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, npc_id[%d] not find || npc_type not monster?", npc_number, node_cfg.npc_monster_id);
			return -3;
		}

		{
			TiXmlElement * clear_reward = root_element->FirstChildElement("reward_group_id");
			if (NULL != clear_reward)
			{
				TiXmlElement *node_element = clear_reward->FirstChildElement("node");
				while (node_element)
				{
					int reward_id = 0;
					if (!GetNodeValue(node_element, reward_id) || m_rewards_cfg.end() == m_rewards_cfg.find(reward_id))
					{
						errormsg = STRING_SPRINTF("[npc_number=%d] error, reward_group_id[%d] not find", npc_number, reward_id);
						return -20001;
					}

					node_cfg.reward_id_vec.push_back(reward_id);
					node_element = node_element->NextSiblingElement();
				}
			}
			if (node_cfg.reward_id_vec.empty())
			{
				errormsg = STRING_SPRINTF("[npc_number=%d] error, [reward_group_id] is empty?", npc_number);
				return -4;
			}
		}

		{
			TiXmlElement * clear_reward = root_element->FirstChildElement("reward_group_2");
			if (NULL != clear_reward)
			{
				TiXmlElement *node_element = clear_reward->FirstChildElement("node");
				while (node_element)
				{
					int reward_id = 0;
					if (!GetNodeValue(node_element, reward_id) || m_rewards_cfg.end() == m_rewards_cfg.find(reward_id))
					{
						errormsg = STRING_SPRINTF("[npc_number=%d] error, reward_group_2[%d] not find", npc_number, reward_id);
						return -20002;
					}

					node_cfg.dec_reward_id_vec.push_back(reward_id);
					node_element = node_element->NextSiblingElement();
				}
			}
			if (node_cfg.dec_reward_id_vec.empty())
			{
				errormsg = STRING_SPRINTF("[npc_number=%d] error, [reward_group_2] is empty?", npc_number);
				return -5;
			}
		}

		if (!GetSubNodeValue(root_element, "flop_reward", node_cfg.poker_reward) || m_rewards_cfg.end() == m_rewards_cfg.find(node_cfg.poker_reward))
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, flop_reward[%d] not find?", npc_number, node_cfg.poker_reward);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "flop_decay_reward", node_cfg.dec_poker_reward) || m_rewards_cfg.end() == m_rewards_cfg.find(node_cfg.dec_poker_reward))
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, flop_decay_reward[%d] not find?", npc_number, node_cfg.dec_poker_reward);
			return -7;
		}

		if (!GetSubNodeValue(root_element, "live_time", node_cfg.alive_time) || node_cfg.alive_time <= 0)
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, live_time[%d] <= 0?", npc_number, node_cfg.alive_time);
			return -8;
		}
		if (npc_monster_list.end() != npc_monster_list.find(node_cfg.npc_monster_id))
		{
			errormsg = STRING_SPRINTF("[npc_number=%d] error, npc_id[%d] repeat?", npc_number, node_cfg.npc_monster_id);
			return -9;
		}

		npc_monster_list.insert(node_cfg.npc_monster_id);
		m_monster_list[npc_number] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int WildBossConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "weekly_time", m_other_cfg.get_reward_times) || m_other_cfg.get_reward_times <= 0)
	{
		return -1;
	}
	TiXmlElement * boss_notice_element = root_element->FirstChildElement("boss_notice");
	if (NULL == boss_notice_element)
	{
		return -2;
	}
	TiXmlElement * node_element = boss_notice_element->FirstChildElement("node");	//允许空,则代表没传闻
	while (NULL != node_element)
	{
		int check_item_id = 0;
		if (!GetNodeValue(node_element, check_item_id) || NULL == ITEMPOOL->GetItem(check_item_id))
		{
			return -4;
		}
		if (m_other_cfg.check_item_boss_notice_list.end() != m_other_cfg.check_item_boss_notice_list.find(check_item_id))
		{
			return -5;
		}

		m_other_cfg.check_item_boss_notice_list.insert(check_item_id);
		node_element = node_element->NextSiblingElement();
	}

	return 0;
}

int WildBossConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		WildBossRewardCfg & cfg = m_rewards_cfg[group_id];
		WildBossRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "coin", item_cfg.coin) || item_cfg.coin < 0)
		{
			return -3;
		}
		if (item_cfg.coin == 0)
		{
			TiXmlElement * clear_reward = root_element->FirstChildElement("reward");
			if (!item_cfg.reward.ReadConfig(clear_reward))
			{
				return -104;
			}
		}
		else
		{
			if (!GetSubNodeValue(root_element, "luck", item_cfg.luck_coin) || item_cfg.luck_coin < 0)
			{
				return -4;
			}
		}

		cfg.group_reward.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rewards_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, WildBossRewardCfg >::iterator iter = m_rewards_cfg.begin(); iter != m_rewards_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_reward.size(); ++i)
		{
			count += iter->second.group_reward[i].rate;
		}

		if (count > WildBossRewardCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int WildBossConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		WildBossPosCfg & cfg = m_pos_cfg[group_id];
		WildBossPosCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "x", item_cfg.pos.x) )
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "y", item_cfg.pos.y))
		{
			return -4;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_pos_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, WildBossPosCfg >::iterator iter = m_pos_cfg.begin(); iter != m_pos_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != WildBossPosCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int WildBossConfig::InitRefreshTimeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		WildBossRefresgTimeCfg & cfg = m_refresh_time_cfg[group_id];
		WildBossRefresgTimeCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "refresh_time", item_cfg.refresh_interval) || item_cfg.refresh_interval < 0)
		{
			return -3;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_refresh_time_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, WildBossRefresgTimeCfg >::iterator iter = m_refresh_time_cfg.begin(); iter != m_refresh_time_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != WildBossRefresgTimeCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int WildBossConfig::InitLevelGroupCfg(TiXmlElement* RootElement)
{
	TiXmlElement* root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int level_group_id = 0;
		if (!GetSubNodeValue(root_element, "id", level_group_id) || m_level_group_map.end() != m_level_group_map.find(level_group_id))
		{
			return -1;
		}
		int min_Level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_Level) || min_Level <= 0 || min_Level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level < min_Level)
		{
			return -3;
		}
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL < 1000);

		m_level_group_map[level_group_id] = min_Level * 1000 + max_level;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
