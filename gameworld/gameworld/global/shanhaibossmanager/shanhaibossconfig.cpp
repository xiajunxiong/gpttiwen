#include "shanhaibossconfig.hpp"
#include "servercommon/shanhaibossdef.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "config/logicconfigmanager.hpp"


ShanHaiBossConfig::ShanHaiBossConfig()
{

}

ShanHaiBossConfig::~ShanHaiBossConfig()
{
	std::vector<ShanHaiBossMonsterCfg>().swap(m_monster_cfg);
	for (std::map<int, std::vector<ShanHaiBossPosGroupCfg> >::iterator it = m_pos_cfg.begin(); it != m_pos_cfg.end(); it++)
	{
		std::vector<ShanHaiBossPosGroupCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<ShanHaiBossRewardCfg> >::iterator it = m_reward_cfg.begin(); it != m_reward_cfg.end(); it++)
	{
		std::vector<ShanHaiBossRewardCfg>().swap(it->second);
	}
}

bool ShanHaiBossConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("monster", InitMonsterCfg);
	LOAD_CONFIG("scene_group", InitSceneCfg);
	LOAD_CONFIG("pos_group", InitPosCfg);
	LOAD_CONFIG("reward_group", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("initiate_level", InitInitiateCfg);

	return true;
}

const ShanHaiBossMonsterCfg * ShanHaiBossConfig::GetMonsterCfg(int monster_npc_id) const
{
	for (std::vector<ShanHaiBossMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (it->npc_id == monster_npc_id)
		{
			return &(*it);
		}
	}
	return NULL;
}

const void ShanHaiBossConfig::GetMonsterListCfg(int wday, std::vector<ShanHaiBossMonsterCfg>& out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi> &out_pos_list) const
{
	if (0 > wday || wday >= 7) return;

	std::vector<int> type_list;
	for (std::vector<int>::const_iterator it = m_other_cfg.day_monster[wday].begin(); it != m_other_cfg.day_monster[wday].end(); ++it)
	{
		type_list.push_back(*it);
	}

	random_shuffle(type_list.begin(), type_list.end());		//¥Ú¬“≈≈–Ú

	int count = 0;
	for (std::vector<int>::const_iterator iter = type_list.begin(); iter != type_list.end() && count < m_other_cfg.monster_type_coune; ++iter, ++count)
	{
		this->GetMonsterCfgByType(*iter, out_boss_list, out_scene_list, out_scene_set_list, out_pos_list);
	}
}

const void ShanHaiBossConfig::GetMonsterCfgByType(int monster_type, std::vector<ShanHaiBossMonsterCfg>& out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi> &out_pos_list) const
{
	int scene_group_id = -1;
	int count = 0;
	for (std::vector<ShanHaiBossMonsterCfg>::const_iterator it = m_monster_cfg.begin(); it != m_monster_cfg.end(); ++it)
	{
		if (it->npc_type == monster_type && WorldStatus::Instance().GetWorldLevel() >= LOGIC_CONFIG->GetShanHaiBossConfig()->GetInitiateCfg(it->grade_type))
		{
			out_boss_list.push_back(*it);
			scene_group_id = it->scene_group_id;
			count++;
		}
	}

	if (0 >= count || 0 > scene_group_id) return;
	this->RandSceneCfg(scene_group_id, count, out_boss_list, out_scene_list, out_scene_set_list, out_pos_list);
}

const void ShanHaiBossConfig::RandSceneCfg(int scene_group_id, int count, std::vector<ShanHaiBossMonsterCfg>& out_boss_list, std::vector<int> &out_scene_list, std::set<int> &out_scene_set_list, std::vector<Posi>& out_pos_list) const
{
	std::map<int, ShanHaiBossSceneGroupListCfg>::const_iterator it = m_scene_cfg.find(scene_group_id);
	if (it == m_scene_cfg.end()) return;

	int posi_group_id = -1;
	int scene_id = 0;

	do 
	{
		int rand_weight = rand() % it->second.all_rate;

		for (std::vector<ShanHaiBossSceneGroupCfg>::const_iterator iter = it->second.node.begin(); iter != it->second.node.end(); ++iter)
		{
			rand_weight -= iter->rate;
			if (0 > rand_weight)
			{
				posi_group_id = iter->pos_group_id;
				scene_id = iter->scene_id;
				break;
			}
		}

	} while (!out_scene_set_list.insert(scene_id).second);

	if (0 > posi_group_id) return;
	this->RandPosiCfg(scene_id, posi_group_id, count, out_boss_list, out_scene_list, out_pos_list);
}

const void ShanHaiBossConfig::RandPosiCfg(int scene_id, int posi_group_id, int count, std::vector<ShanHaiBossMonsterCfg>& out_boss_list, std::vector<int> &out_scene_list, std::vector<Posi>& out_pos_list) const
{
	std::map<int, std::vector<ShanHaiBossPosGroupCfg> >::const_iterator it = m_pos_cfg.find(posi_group_id);
	if (it == m_pos_cfg.end()) return;

	std::vector<Posi> posi_list;
	for (std::vector<ShanHaiBossPosGroupCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		posi_list.push_back(vit->boss_pos);
	}

	//¥Ú¬“≈≈–Ú
	random_shuffle(posi_list.begin(), posi_list.end());

	if (count > (int)posi_list.size()) return;

	for (int i = 0; i < count; ++i)
	{
		out_scene_list.push_back(scene_id);
		out_pos_list.push_back(posi_list[i]);
	}
}

void ShanHaiBossConfig::GetRandRewardItem(int monster_npc_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward, int role_level, bool use_dec_reward)
{
	const ShanHaiBossMonsterCfg * cfg = this->GetMonsterCfg(monster_npc_id);
	if (cfg == NULL) return;

	if (use_dec_reward)
	{
		this->GetRandReward(cfg->poker_dec_reward_id, out_reward, coin, luck_coin, role_level);
	}
	else
	{
		for (int i = 0; i < (int)cfg->reward_id_vec.size(); ++i)
		{
			this->GetRandReward(cfg->reward_id_vec[i], out_reward, coin, luck_coin, role_level);
		}
	}
}

void ShanHaiBossConfig::GetPokerRewardItem(int monster_npc_id, int * coin, int * luck_coin, std::vector<ItemConfigData>& out_reward, bool use_dec_reward)
{
	const ShanHaiBossMonsterCfg * cfg = this->GetMonsterCfg(monster_npc_id);
	if (cfg == NULL) return;

	int reward_id = cfg->poker_reward_id;
	if (use_dec_reward)
	{
		reward_id = cfg->poker_dec_reward_id;
	}
	this->GetRandReward(reward_id, out_reward, coin, luck_coin, 0);
}

void ShanHaiBossConfig::GetRandReward(int reward_id, std::vector<ItemConfigData>& out_list, int * coin, int * luck_coin, int role_level)
{
	std::map<int, std::vector<ShanHaiBossRewardCfg> > ::iterator iter = m_reward_cfg.find(reward_id);
	if (iter == m_reward_cfg.end())
	{
		return;
	}

	/*int all_weight = 0;
	for (std::vector<ShanHaiBossRewardCfg>::const_iterator zit = iter->second.begin(); zit != iter->second.end(); ++zit)
	{
		if (zit->min_level <= role_level && role_level <= zit->max_level)
		{
			all_weight += zit->rate;
		}
	}

	if (0 >= all_weight)
	{
		return;
	}*/

	int rate = rand() % SHAN_HAI_REWARD_ALL_WEIGHT;

	for (std::vector<ShanHaiBossRewardCfg>::const_iterator it = iter->second.begin(); it != iter->second.end(); ++it)
	{
		if (it->min_level <= role_level && role_level <= it->max_level)
		{
			if (rate < it->rate)
			{
				if (0 == it->coin)
				{
					out_list.push_back(it->item);
				}
				else
				{
					*coin = it->coin;
					*luck_coin = it->luck;
				}
				break;
			}

			rate -= it->rate;
		}
	}
}

int ShanHaiBossConfig::GetInitiateCfg(int grade_type)const
{
	if (grade_type <= 0 || grade_type >= SHAN_HAI_BOSS_GRADE_TYPE_MAX)
	{
		return -1;
	}

	std::map<int, int>::const_iterator it = m_initiate_cfg.find(grade_type);
	if (it == m_initiate_cfg.end()) return -2;

	return it->second;
}

bool ShanHaiBossConfig::IsGetItemNotice(int item_id)
{
	for (std::vector<int>::const_iterator it = m_other_cfg.notice_id_vec.begin(); it != m_other_cfg.notice_id_vec.end(); ++it)
	{
		if (item_id == *it)
		{
			return true;
		}
	}
	return false;
}


int ShanHaiBossConfig::InitMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int monster_type = 0;
	int scene_group_id = 0;
	int same_count = 0;

	while (NULL != root_element)
	{
		ShanHaiBossMonsterCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || 0 > cfg.seq)
		{
			return -1;
		}
		if (cfg.seq != (int)m_monster_cfg.size() || cfg.seq >= MAX_UINT8)
		{
			return -99;
		}
		if (!GetSubNodeValue(root_element, "npc_id", cfg.npc_id) || cfg.npc_id <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "monster_type", cfg.npc_type) || cfg.npc_type <= 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "level_group", cfg.monster_level) || cfg.monster_level <= 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "monster_group", cfg.monster_group_id) || cfg.monster_group_id <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "fight_times", cfg.fight_times) || cfg.fight_times <= 0)
		{
			return -6;
		}
		if (!GetSubNodeValue(root_element, "scene_group_id", cfg.scene_group_id) || cfg.scene_group_id < 0)
		{
			return -7;
		}
		if (!GetSubNodeValue(root_element, "flop_reward", cfg.poker_reward_id) || cfg.poker_reward_id <= 0)
		{
			return -8;
		}
		if (!GetSubNodeValue(root_element, "weaken_flop_reward", cfg.poker_dec_reward_id) || cfg.poker_dec_reward_id <= 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "grade_type", cfg.grade_type) || cfg.grade_type <= SHAN_HAI_BOSS_GRADE_TYPE_INVAILD || cfg.grade_type >= SHAN_HAI_BOSS_GRADE_TYPE_MAX)
		{
			return -10;
		}

		TiXmlElement * reward_list = root_element->FirstChildElement("reward_group_id");
		if (NULL != reward_list)
		{
			TiXmlElement *node_element = reward_list->FirstChildElement("node");
			while (node_element)
			{
				int reward_id = 0;
				if (!GetNodeValue(node_element, reward_id))
				{
					return -100;
				}

				cfg.reward_id_vec.push_back(reward_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		if (cfg.npc_type == monster_type)
		{
			if (cfg.scene_group_id != scene_group_id) return -111;

			same_count++;
			if (same_count > SHAN_HAI_MONSTER_ONE_TYPE_MAX) return -112;
		}
		else
		{
			same_count = 1;
		}
		monster_type = cfg.npc_type;
		scene_group_id = cfg.scene_group_id;

		m_monster_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShanHaiBossConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int scene_group_id = 0;
		if (!GetSubNodeValue(root_element, "scene_group_id", scene_group_id) || scene_group_id < 0)
		{
			return -1;
		}

		ShanHaiBossSceneGroupListCfg& data = m_scene_cfg[scene_group_id];
		ShanHaiBossSceneGroupCfg cfg;

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id) || cfg.scene_id <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "pos_group_id", cfg.pos_group_id) || cfg.pos_group_id < 0)
		{
			return -4;
		}

		data.all_rate += cfg.rate;
		data.node.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShanHaiBossConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int pos_group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", pos_group_id) || pos_group_id < 0)
		{
			return -1;
		}

		std::vector<ShanHaiBossPosGroupCfg>& data = m_pos_cfg[pos_group_id];
		ShanHaiBossPosGroupCfg cfg;

		if (!GetSubNodeValue(root_element, "x", cfg.boss_pos.x) || cfg.boss_pos.x <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "y", cfg.boss_pos.y) || cfg.boss_pos.y <= 0)
		{
			return -3;
		}

		data.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShanHaiBossConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0)
		{
			return -1;
		}
		std::vector<ShanHaiBossRewardCfg> & data = m_reward_cfg[group_id];
		ShanHaiBossRewardCfg cfg;

		if (!GetSubNodeValue(root_element, "coin", cfg.coin) || cfg.coin < 0)
		{
			return -2;
		}

		if (0 == cfg.coin)
		{
			TiXmlElement * item_element = root_element->FirstChildElement("reward");
			if (NULL != item_element)
			{
				if (!cfg.item.ReadConfig(item_element)) return -100;
			}
		}
		else
		{
			if (!GetSubNodeValue(root_element, "luck", cfg.luck) || cfg.luck < 0)
			{
				return -3;
			}
		}

		if (!GetSubNodeValue(root_element, "rate", cfg.rate) || cfg.rate <= 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "min_level", cfg.min_level) || cfg.min_level <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "max_level", cfg.max_level) || cfg.max_level < cfg.min_level)
		{
			return -6;
		}

		data.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShanHaiBossConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ShanHaiBossOtherCfg & cfg = m_other_cfg;

		if (!GetSubNodeValue(root_element, "day_time", cfg.day_time) || cfg.day_time <= 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "open_time1", cfg.close_begin_time) || cfg.close_begin_time < 0 || cfg.close_begin_time >= 24)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "open_time2", cfg.close_end_time) || cfg.close_end_time < 0 || cfg.close_end_time >= 24)
		{
			return -3;
		}
		if (cfg.close_begin_time >= cfg.close_end_time)
		{
			return -99;
		}
		if (!GetSubNodeValue(root_element, "monster_amount", cfg.monster_type_coune) || cfg.monster_type_coune <= 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(root_element, "live_time", cfg.live_time) || cfg.live_time <= 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(root_element, "limit_team_people", cfg.limit_team_people) || cfg.limit_team_people <= 0)
		{
			return -6;
		}
		TiXmlElement * day_monstar_type_list_1 = root_element->FirstChildElement("Monday_monster");
		if (NULL != day_monstar_type_list_1)
		{
			TiXmlElement *node_element = day_monstar_type_list_1->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -101;
				}

				cfg.day_monster[1].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_2 = root_element->FirstChildElement("tuesday_monster");
		if (NULL != day_monstar_type_list_2)
		{
			TiXmlElement *node_element = day_monstar_type_list_2->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -102;
				}

				cfg.day_monster[2].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_3 = root_element->FirstChildElement("wednesday_monster");
		if (NULL != day_monstar_type_list_3)
		{
			TiXmlElement *node_element = day_monstar_type_list_3->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -103;
				}

				cfg.day_monster[3].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_4 = root_element->FirstChildElement("thursday_monster");
		if (NULL != day_monstar_type_list_4)
		{
			TiXmlElement *node_element = day_monstar_type_list_4->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -104;
				}

				cfg.day_monster[4].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_5 = root_element->FirstChildElement("friday_monster");
		if (NULL != day_monstar_type_list_5)
		{
			TiXmlElement *node_element = day_monstar_type_list_5->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -105;
				}

				cfg.day_monster[5].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_6 = root_element->FirstChildElement("saturday_monster");
		if (NULL != day_monstar_type_list_6)
		{
			TiXmlElement *node_element = day_monstar_type_list_6->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -106;
				}

				cfg.day_monster[6].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}
		TiXmlElement * day_monstar_type_list_7 = root_element->FirstChildElement("sunday_monster");
		if (NULL != day_monstar_type_list_7)
		{
			TiXmlElement *node_element = day_monstar_type_list_7->FirstChildElement("node");
			while (node_element)
			{
				int monster_type = 0;
				if (!GetNodeValue(node_element, monster_type))
				{
					return -100;
				}

				cfg.day_monster[0].push_back(monster_type);
				node_element = node_element->NextSiblingElement();
			}
		}

		TiXmlElement * notice_list = root_element->FirstChildElement("notice");
		if (NULL != notice_list)
		{
			TiXmlElement *node_element = notice_list->FirstChildElement("node");
			while (node_element)
			{
				int notice_id = 0;
				if (!GetNodeValue(node_element, notice_id))
				{
					return -100;
				}

				cfg.notice_id_vec.push_back(notice_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ShanHaiBossConfig::InitInitiateCfg(TiXmlElement *RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int grade_type = 0;
		if (!GetSubNodeValue(root_element, "grade_type", grade_type)||grade_type<=0||grade_type> SHAN_HAI_MONSTER_ONE_TYPE_MAX)
		{
			return -1;
		}

		int initiate_level = 0;
		if (!GetSubNodeValue(root_element, "initiate_level", initiate_level) || initiate_level <= 0)
		{
			return - 2;
		}

		m_initiate_cfg[grade_type] = initiate_level;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
