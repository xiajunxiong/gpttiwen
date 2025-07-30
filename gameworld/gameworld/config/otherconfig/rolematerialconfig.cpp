#include "rolematerialconfig.hpp"
#include "checkresourcecenter.hpp"

#include "npc/npcpool.h"

MaterialConfig::MaterialConfig()
{
}

MaterialConfig::~MaterialConfig()
{
	std::vector<MaterialSceneCfg>().swap(m_scene_vec);
	for (std::map<int, std::vector<MaterialSceneCfg> >::iterator it = m_scene_map.begin(); it != m_scene_map.end(); it++)
	{
		std::vector<MaterialSceneCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<MaterialRewardGroupCfg> >::iterator it = m_reward_group_cfg.begin(); it != m_reward_group_cfg.end(); it++)
	{
		std::vector<MaterialRewardGroupCfg>().swap(it->second);
	}
	std::vector<MaterialBoxRewardCfg>().swap(m_box_reward_cfg);
}

bool MaterialConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward_group", InitRewardGroupCfg);
	LOAD_CONFIG("scene_config", InitSceneCfg);
	LOAD_CONFIG("box_reward", InitBoxRewardCfg);
	//LOAD_CONFIG("exp", InitExpCfg);		//经验奖励使用水晶副本的经验配置
	//LOAD_CONFIG("num_breach", InitBuyCfg);//购买额外次数使用其他分页配置
	
	return true;
}

const MaterialSceneCfg * MaterialConfig::GetSceneCfg(int seq)
{
	if(seq < 0 || seq >= (int)m_scene_vec.size()) return NULL;

	return &m_scene_vec[seq];
}

const MaterialSceneCfg * MaterialConfig::GetSceneCfg(int chapter_id, int level_seq)
{
	std::map<int, std::vector<MaterialSceneCfg> >::iterator it = m_scene_map.find(chapter_id);
	if(it == m_scene_map.end()) return NULL;

	if(level_seq < 0 || level_seq >= (int)it->second.size()) return NULL;  

	return &it->second[level_seq];
}

int MaterialConfig::GetChapterLevelNum(int chapter_id)
{
	std::map<int, std::vector<MaterialSceneCfg> >::iterator it = m_scene_map.find(chapter_id);
	if(it == m_scene_map.end()) return -1;

	return (int)it->second.size();
}

void MaterialConfig::GetPassReward(std::vector<int> reward_group_list, std::vector<ItemConfigData> * reward_list)
{
	if(reward_group_list.empty() || NULL == reward_list) return;

	for (int i = 0; i < (int)reward_group_list.size(); i++)
	{
		std::map<int, std::vector<MaterialRewardGroupCfg> >::iterator group_it = m_reward_group_cfg.find(reward_group_list[i]);
		if(group_it == m_reward_group_cfg.end()) continue;
		
		int rate_num = rand() % MaterialRewardGroupCfg::MAX_RATE_COUNT_NUM;
		for (int k = 0; k < (int)group_it->second.size(); k++)
		{
			if (rate_num < group_it->second[k].rate)
			{
				reward_list->push_back(group_it->second[k].reward);
				break;
			}
			rate_num -= group_it->second[k].rate;
		}
	}
	return;
}

const MaterialBoxRewardCfg * MaterialConfig::GetBoxRewardCfg(int seq)
{
	if(seq < 0 || seq >= (int)m_box_reward_cfg.size()) return NULL;


	return &m_box_reward_cfg[seq];
}

int MaterialConfig::GetExpCfg(int role_level)
{
	if(role_level <= 0 || role_level > MAX_ROLE_LEVEL) return 0;

	for (std::map<int, int>::iterator it = m_exp_cfg.begin(); it != m_exp_cfg.end(); it++)
	{
		if (role_level >= it->first / 1000 && role_level <= it->first % 1000)
		{
			return it->second;
		}
	}

	return 0;
}

int MaterialConfig::GetBuyCfg(int buy_times)
{
	std::map<int, int>::iterator it = m_buy_cfg.find(buy_times);
	if(it == m_buy_cfg.end()) return 0;

	return it->second;
}

int MaterialConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "pass_time_max", m_other_cfg.pass_time_max) || m_other_cfg.pass_time_max <= 0) return -1;
	if (!GetSubNodeValue(root_element, "buy_times_max", m_other_cfg.buy_times_max) || m_other_cfg.buy_times_max <= 0) return -2;
	if (!GetSubNodeValue(root_element, "extra_buy_consume", m_other_cfg.extra_buy_consume) || m_other_cfg.extra_buy_consume <= 0) return -3;
	if (!GetSubNodeValue(root_element, "use_power", m_other_cfg.use_power) || m_other_cfg.use_power <= 0) return -4;

	return 0;
}

int MaterialConfig::InitRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group_id) || reward_group_id <= 0) return -1;

		MaterialRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0 || node_cfg.rate > MaterialRewardGroupCfg::MAX_RATE_COUNT_NUM) return -2;

		TiXmlElement * reward = root_element->FirstChildElement("reward_item");
		if (reward != NULL)
		{
			if (!node_cfg.reward.ReadConfig(reward))
			{
				return -201;
			}
		}
		else
		{
			return -202;
		}
		m_reward_group_cfg[reward_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	//策划说可以有随机不到的奖励
	for (std::map<int, std::vector<MaterialRewardGroupCfg> >::iterator it = m_reward_group_cfg.begin(); it != m_reward_group_cfg.end(); it++)
	{
		int rate_count = 0;
		for (int i = 0; i < (int)it->second.size(); i++)
		{
			rate_count += it->second[i].rate;
		}
		if (rate_count > MaterialRewardGroupCfg::MAX_RATE_COUNT_NUM)
		{
			return -(it->first * 1000);
		}
	}

	return 0;
}

int MaterialConfig::InitBoxRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int next_seq = 0;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq || seq >= MAX_MATERIAL_BOX_FLAG_NUM) return -1;
		next_seq += 1;

		MaterialBoxRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "chapter", node_cfg.chapter_id)) return -2;

		std::map<int, std::vector<MaterialSceneCfg> >::iterator it = m_scene_map.find(node_cfg.chapter_id);
		if (it == m_scene_map.end())
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "max_score", node_cfg.max_score) || node_cfg.max_score <= 0) return -4;
		if (node_cfg.max_score > (int)it->second.size() * 3)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "coin_reward", node_cfg.coin_reawrd) || node_cfg.coin_reawrd < 0) return -6;

		//允许没奖励
		TiXmlElement * reward_list = root_element->FirstChildElement("reward_list");
		if (NULL != reward_list)
		{
			TiXmlElement * reward_element = reward_list->FirstChildElement("reward");
			while (reward_element != NULL)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(reward_element))
				{
					return -101;
				}
				node_cfg.reward_list.push_back(reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_box_reward_cfg.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int MaterialConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	bool is_first = true;
	int next_seq = 0;
	int last_chapter_id = 0;
	int next_level_seq = 0;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq != next_seq) return -1;
		next_seq += 1;

		MaterialSceneCfg node_cfg;
		if (!GetSubNodeValue(root_element, "chapter", node_cfg.chapter_id) || node_cfg.chapter_id < 0 || node_cfg.chapter_id >= MAX_MATERIAL_CHAPTER_NUM) return -2;
		if (!GetSubNodeValue(root_element, "chapter_level_limit", node_cfg.level_limit) || node_cfg.level_limit <= 0 || node_cfg.level_limit >= MAX_ROLE_LEVEL) return -3;
		if (is_first)
		{
			if(0 != node_cfg.chapter_id) return -88;
			is_first = false;
		}
		if (last_chapter_id != node_cfg.chapter_id)
		{
			last_chapter_id = node_cfg.chapter_id;
			next_level_seq = 0;
		}

		if (!GetSubNodeValue(root_element, "level", node_cfg.level_seq) || node_cfg.level_seq < 0 || node_cfg.level_seq >= MAX_MATERIAL_CHECKPOINT_NUM || next_level_seq != node_cfg.level_seq) return -4;
		next_level_seq += 1;

		char condition[64] = { 0 };
		for (int i = 1; i <= MAX_MATERIAL_CONDITON_NUM; i++)
		{
			SNPRINTF(condition, sizeof(condition), "condition_%d", i);
			if (!GetSubNodeValue(root_element, condition, node_cfg.condition[i - 1]) || node_cfg.condition[i - 1] < 0) return -100 - i;
		}

	/*	char achievement_reward[64] = { 0 };
		for (int i = 1; i <= MAX_MATERIAL_ACHIEVENMENT_CONDITION_NUM; i++)
		{
			SNPRINTF(condition, sizeof(condition), "achievement_condition_%d", i);
			if (!GetSubNodeValue(root_element, condition, node_cfg.achievement_condition[i - 1]) || node_cfg.achievement_condition[i - 1] < 0) return -200 - i;

			SNPRINTF(achievement_reward, sizeof(achievement_reward), "achievement_reward_%d", i);
			TiXmlElement * reward = root_element->FirstChildElement(achievement_reward);
			if (reward != NULL)
			{
				if (!node_cfg.achievement_reward[i - 1].ReadConfig(reward))
				{
					return -300 - i;
				}
			}
			else
			{
				return -310 - i;
			}
		}*/
		if (!GetSubNodeValue(root_element, "scene_id", node_cfg.scene_id)) return -5;
		CheckResourceCenter::Instance().GetSceneCheck()->Add(node_cfg.scene_id, __FUNCTION__);

		if (!GetSubNodeValue(root_element, "boss_id", node_cfg.boss_id)) return -6;
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(node_cfg.boss_id);
		if (NULL == npc_cfg || SERVER_NPC_TYPE_MONSTER != npc_cfg->server_npc_type) return -7;

		if (!GetSubNodeValue(root_element, "boss_x", node_cfg.boss_pos.x) || node_cfg.boss_pos.x < 0) return -8;
		if (!GetSubNodeValue(root_element, "boss_y", node_cfg.boss_pos.y) || node_cfg.boss_pos.y < 0) return -9;
		if (!GetSubNodeValue(root_element, "boss_dir", node_cfg.boss_dir) || node_cfg.boss_dir < 0) return -10;

		TiXmlElement * first_reward_list = root_element->FirstChildElement("first_reward_list");
		if (NULL == first_reward_list)
		{
			return -400;
		}
		TiXmlElement * first_reward = first_reward_list->FirstChildElement("first_reward");
		while (first_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(first_reward))
			{
				return -401;
			}
			node_cfg.first_reward_list.push_back(reward);
			first_reward = first_reward->NextSiblingElement();
		}

		TiXmlElement * reward_group_list = root_element->FirstChildElement("reward_group");
		if (NULL == reward_group_list)
		{
			return -500;
		}
		TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
		while (node_element != NULL)
		{
			int reward_group_id = 0;
			if (!GetNodeValue(node_element, reward_group_id) || m_reward_group_cfg.end() == m_reward_group_cfg.find(reward_group_id))
			{
				return -501;
			}
				
			node_cfg.common_reward_group_list.push_back(reward_group_id);
			node_element = node_element->NextSiblingElement();
		}

		//npc_1可以为0
		if (!GetSubNodeValue(root_element, "npc_1_id", node_cfg.npc_1_id) || node_cfg.npc_1_id < 0) return -11;
		if (!GetSubNodeValue(root_element, "npc_1_x", node_cfg.npc_1_pos.x) || node_cfg.npc_1_pos.x < 0) return -12;
		if (!GetSubNodeValue(root_element, "npc_1_y", node_cfg.npc_1_pos.y) || node_cfg.npc_1_pos.y < 0) return -13;
		if (!GetSubNodeValue(root_element, "npc_1_dir", node_cfg.npc_1_dir) || node_cfg.npc_1_dir < 0) return -14;
		if (!GetSubNodeValue(root_element, "is_need_talk1", node_cfg.is_need_talk_1)) return -15;
		node_cfg.is_need_talk_1 = (0 != node_cfg.is_need_talk_1) ? 1 : 0;

		if (0 != node_cfg.npc_1_id)
		{
			const ServerNPCInfoCfg* npc_1_cfg = NPCPOOL->GetServerNPCInfo(node_cfg.npc_1_id);
			if (NULL == npc_1_cfg || SERVER_NPC_TYPE_CHOSED != npc_1_cfg->server_npc_type)
			{
				return -30;
			}
		}

		//npc_2可以为0
		if (!GetSubNodeValue(root_element, "npc_2_id", node_cfg.npc_2_id) || node_cfg.npc_2_id < 0) return -16;
		if (!GetSubNodeValue(root_element, "npc_2_x", node_cfg.npc_2_pos.x) || node_cfg.npc_2_pos.x < 0) return -17;
		if (!GetSubNodeValue(root_element, "npc_2_y", node_cfg.npc_2_pos.y) || node_cfg.npc_2_pos.y < 0) return -18;
		if (!GetSubNodeValue(root_element, "npc_2_dir", node_cfg.npc_2_dir) || node_cfg.npc_2_dir < 0) return -19;
		if (!GetSubNodeValue(root_element, "is_need_talk2", node_cfg.is_need_talk_2)) return -20;
		node_cfg.is_need_talk_2 = (0 != node_cfg.is_need_talk_2) ? 1 : 0;

		if (0 != node_cfg.npc_2_id)
		{
			const ServerNPCInfoCfg* npc_2_cfg = NPCPOOL->GetServerNPCInfo(node_cfg.npc_2_id);
			if (NULL == npc_2_cfg || SERVER_NPC_TYPE_CHOSED != npc_2_cfg->server_npc_type) 
			{
				return -31;
			}
		}

		//npc_3可以为0
		if (!GetSubNodeValue(root_element, "npc_3_id", node_cfg.npc_3_id) || node_cfg.npc_3_id < 0) return -21;
		if (!GetSubNodeValue(root_element, "npc_3_x", node_cfg.npc_3_pos.x) || node_cfg.npc_3_pos.x < 0) return -22;
		if (!GetSubNodeValue(root_element, "npc_3_y", node_cfg.npc_3_pos.y) || node_cfg.npc_3_pos.y < 0) return -23;
		if (!GetSubNodeValue(root_element, "npc_3_dir", node_cfg.npc_3_dir) || node_cfg.npc_3_dir < 0) return -24;
		if (!GetSubNodeValue(root_element, "is_need_talk3", node_cfg.is_need_talk_3)) return -25;
		node_cfg.is_need_talk_3 = (0 != node_cfg.is_need_talk_3) ? 1 : 0;

		if (0 != node_cfg.npc_3_id)
		{
			const ServerNPCInfoCfg* npc_3_cfg = NPCPOOL->GetServerNPCInfo(node_cfg.npc_3_id);
			if (NULL == npc_3_cfg || SERVER_NPC_TYPE_CHOSED != npc_3_cfg->server_npc_type) return -32;
		}

		m_scene_vec.push_back(node_cfg);
		m_scene_map[node_cfg.chapter_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int MaterialConfig::InitExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	while (NULL != root_element)
	{
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(root_element, "min_level", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(root_element, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL) return -2;
		int exp = 0;
		if (!GetSubNodeValue(root_element, "exp", exp) || exp <= 0) return -3;

		int level_key = min_level * 1000 + max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);
		
		m_exp_cfg[level_key] = exp;
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int MaterialConfig::InitBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	int next_buy_times = 1;
	while (NULL != root_element)
	{
		int buy_times = 0;
		if (!GetSubNodeValue(root_element, "buy_times", buy_times) || buy_times != next_buy_times || buy_times > m_other_cfg.buy_times_max) return -1;
		next_buy_times++;

		int price = 0;
		if (!GetSubNodeValue(root_element, "price", price) || price <= 0) return -2;

		m_buy_cfg[buy_times] = price;
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_buy_cfg.size() != m_other_cfg.buy_times_max)
	{
		return -3;
	}

	return 0;
}
