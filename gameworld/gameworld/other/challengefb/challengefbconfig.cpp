#include "challengefbconfig.hpp"
#include "world.h"
#include "gameworld/npc/npcpool.h"

ChallengeFbConfig::ChallengeFbConfig()
{
}

ChallengeFbConfig::~ChallengeFbConfig()
{
}

bool ChallengeFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitChallOthersCfg)
	LOAD_CONFIG("fb_expend_num", InitItemUseLimitCfg)
	LOAD_CONFIG("monster_cfg", InitMonsterInfoCfg)
	LOAD_CONFIG("challenge_content", InitChallInformCfg)
	LOAD_CONFIG("stage_reward", InitStageRewardCfg)

	return true;
}

const ChallChapterInfor * ChallengeFbConfig::GetChapterInfor(int fb_id, int chapter)
{
	std::map<int, std::map<int, ChallChapterInfor> >::iterator fb_it = m_chapter_infor_map.find(fb_id);
	if (m_chapter_infor_map.end() == fb_it) return NULL;

	std::map<int, ChallChapterInfor>::iterator chapter_it = fb_it->second.find(chapter);
	if (fb_it->second.end() != chapter_it) return & chapter_it->second;
	
	return NULL;
}

const ChallEctypeReward * ChallengeFbConfig::GetEctypeReward(int fb_id)
{
	std::map<int, ChallEctypeReward>::iterator fb_it = m_ectype_reward_map.find(fb_id);
	if (m_ectype_reward_map.end() != fb_it) return & fb_it->second;

	return NULL;
}

const ChallMonsterCfg * ChallengeFbConfig::RandAMonsterCfg(int scene_id)
{
	std::map<int, std::set<ChallMonsterCfg> >::iterator scene_it = m_monster_map.find(scene_id);
	if (m_monster_map.end() == scene_it) return NULL;

	std::vector<int> tmp_rand_vec;
	for (std::set<ChallMonsterCfg>::iterator cfg_it = scene_it->second.begin(); cfg_it != scene_it->second.end(); ++cfg_it)
	{
		tmp_rand_vec.push_back(cfg_it->npc_id);
	}

	int rand_idx = rand() % (int)tmp_rand_vec.size();
	std::set<ChallMonsterCfg>::iterator result = scene_it->second.find(ChallMonsterCfg(tmp_rand_vec[rand_idx]));
	if (scene_it->second.end() != result) return &*result;

	return NULL;
}

bool ChallengeFbConfig::GetMonsterCfg(int scene_id, std::set<ChallMonsterCfg> & out_list)
{
	out_list.clear();

	std::map<int, std::set<ChallMonsterCfg> >::iterator scene_it = m_monster_map.find(scene_id);
	if (m_monster_map.end() == scene_it) return false;

	for (std::set<ChallMonsterCfg>::iterator mon_cfg_it = scene_it->second.begin(); mon_cfg_it != scene_it->second.end(); ++mon_cfg_it)
	{
		out_list.insert(*mon_cfg_it);
	}

	return true;
}

int ChallengeFbConfig::GetItemUseTimesLimit(int fb_id, int chapter, int use_type)
{
	const ChallChapterInfor * chapter_cfg = this->GetChapterInfor(fb_id, chapter);
	if (NULL == chapter_cfg) return 0;		// 如果获取不到副本配置，默认 0；

	std::map<int, std::map<int, int> >::iterator group_it = m_item_use_limit_map.find(chapter_cfg->item_limit_group);
	if (m_item_use_limit_map.end() == group_it) return MAX_INT;		// 如果找不到，认为无限制；

	std::map<int, int>::iterator type_it = group_it->second.find(use_type);
	if (group_it->second.end() != type_it) return type_it->second;

	return MAX_INT;
}

int ChallengeFbConfig::InitChallOthersCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	if (!GetSubNodeValue(root_element, "player_num", m_chall_other.min_mem_num) || m_chall_other.min_mem_num < 0 || m_chall_other.min_mem_num > MAX_TEAM_MEMBER_NUM)
	{
		return -1;
	}

	return 0;
}

int ChallengeFbConfig::InitChallInformCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		ChallChapterInfor tmp_con;
		if (!GetSubNodeValue(root_element, "fuben_id", tmp_con.fb_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "chapter", tmp_con.chapter))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "open_level", tmp_con.level_limit) || tmp_con.level_limit <= 0 || tmp_con.level_limit > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "scene_id", tmp_con.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(tmp_con.scene_id))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "play_type", tmp_con.play_type) || tmp_con.play_type < 0 || tmp_con.play_type >= CF_TYPE_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "param_2", tmp_con.param2))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "use_item_num", tmp_con.item_limit_group) || (0 != tmp_con.item_limit_group && m_item_use_limit_map.end() == m_item_use_limit_map.find(tmp_con.item_limit_group)))
		{
			return -7;
		}

		TiXmlElement * param1_list = root_element->FirstChildElement("param_1");
		if (NULL == param1_list) return -100;

		std::set<int> tmp_param_list_checker;
		TiXmlElement * param_node = param1_list->FirstChildElement("node");
		while (NULL != param_node)
		{
			if (tmp_con.param1_list_num >= PARAM_1_LIST_COUNT) return -150;
			if (!GetNodeValue(param_node, tmp_con.param1_list[tmp_con.param1_list_num]) || tmp_param_list_checker.end() != tmp_param_list_checker.find(tmp_con.param1_list[tmp_con.param1_list_num]))
			{
				return -200;
			}

			tmp_param_list_checker.insert(tmp_con.param1_list[tmp_con.param1_list_num]);
			++tmp_con.param1_list_num;
			param_node = param_node->NextSiblingElement();
		}

		TiXmlElement * item_list = root_element->FirstChildElement("chapter_reward_list");
		if (NULL == item_list) return -300;

		TiXmlElement * item_node = item_list->FirstChildElement("chapter_reward");
		while (NULL != item_node)
		{
			if (tmp_con.reward_num >= ITEM_REWARD_LIST_COUNT) return -350;
			if (!tmp_con.chapter_reward[tmp_con.reward_num++].ReadConfig(item_node))
			{
				return -400;
			}

			item_node = item_node->NextSiblingElement();
		}

		UNSTD_STATIC_CHECK(3 == CF_TYPE_MAX)
		switch (tmp_con.play_type)
		{
		case CF_TYPE_BASICAL_PLAY_TYPE:
			{
				std::map<int, std::set<ChallMonsterCfg> >::iterator mon_it = m_monster_map.find(tmp_con.scene_id);
				if (m_monster_map.end() == mon_it) return -1000;	// 如果没有配置怪物，报错

				if (1 != (int)mon_it->second.size()) return -1001;	// 如果明雷数量不等于 1，报错
			}
			break;
		case CF_TYPE_KILL_APPO_MON_NUM:
			if (tmp_con.param2 <= 0) return -2001;
			break;
		case CF_TYPE_KILL_MON_IN_ORDER:
			{
				if (tmp_con.param1_list_num <= 0) return -3000;	// 如果没有配置击杀顺序，报错

				std::map<int, std::set<ChallMonsterCfg> >::iterator mon_it = m_monster_map.find(tmp_con.scene_id);
				if (m_monster_map.end() == mon_it) return -3001;	// 如果没有配置怪物

				// 检查顺序内的怪物ID 是否重复 - 重复会出问题
				std::set<int> tmp_checker;
				for (int i = 0; i < tmp_con.param1_list_num; ++i)
				{
					if (tmp_checker.end() != tmp_checker.find(tmp_con.param1_list[i])) return -3002;
					tmp_checker.insert(tmp_con.param1_list[i]);
				}

				// 检查怪物有没有在配置里
				for (int kill_order = 0; kill_order < tmp_con.param1_list_num && kill_order < PARAM_1_LIST_COUNT; ++kill_order)
				{
					if (mon_it->second.end() == mon_it->second.find(ChallMonsterCfg(tmp_con.param1_list[kill_order], 0, 0))) return -3003;
				}
			}
			break;
		}

		m_chapter_infor_map[tmp_con.fb_id][tmp_con.chapter] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ChallengeFbConfig::InitStageRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int fb_id = 0; ChallEctypeReward tmp_con;
		if (!GetSubNodeValue(root_element, "fuben_id", fb_id))
		{
			return -1;
		}

		TiXmlElement * item_list = root_element->FirstChildElement("stage_reward_list");
		if (NULL == item_list) return -10000;

		TiXmlElement * item_node = item_list->FirstChildElement("stage_reward");
		while (NULL != item_node)
		{
			if (tmp_con.reward_num >= ITEM_REWARD_LIST_COUNT) return -500;
			if (!tmp_con.ectype_reward[tmp_con.reward_num++].ReadConfig(item_node))
			{
				return -200;
			}

			item_node = item_node->NextSiblingElement();
		}

		m_ectype_reward_map[fb_id] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ChallengeFbConfig::InitMonsterInfoCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int scene_id = 0; ChallMonsterCfg tmp_con;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "npc_id", tmp_con.npc_id))
		{
			return -2;
		}

		const ServerNPCInfoCfg * cfg = NPCPOOL->GetServerNPCInfo(tmp_con.npc_id);
		if (NULL == cfg || SERVER_NPC_TYPE_MONSTER != cfg->server_npc_type) return -3;

		if (!GetSubNodeValue(root_element, "x", tmp_con.posi.x))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "y", tmp_con.posi.y))
		{
			return -5;
		}

		std::map<int, std::set<ChallMonsterCfg> >::iterator scene_it = m_monster_map.find(scene_id);
		if (m_monster_map.end() != scene_it)
		{
			scene_it->second.insert(tmp_con);
		}
		else
		{
			std::set<ChallMonsterCfg> tmp_list; tmp_list.insert(tmp_con);
			m_monster_map[scene_id] = tmp_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ChallengeFbConfig::InitItemUseLimitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int limit_group_id = 0, item_type = 0, limit_times = 0;
		if (!GetSubNodeValue(root_element, "limit_item_group_id", limit_group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "use_type", item_type))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "xz_num", limit_times) || limit_times < 0)
		{
			return -3;
		}

		m_item_use_limit_map[limit_group_id][item_type] = limit_times;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}