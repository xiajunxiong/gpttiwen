#include "activitymazeracerconfig.hpp"
#include "world.h"

ActivityMazeRacerConfig::ActivityMazeRacerConfig() : box_weight(0)
{
}

ActivityMazeRacerConfig::~ActivityMazeRacerConfig()
{
	for (std::map<int, std::vector<MazeRacerAwardCfg> >::iterator it = m_award_cfg.begin(); it != m_award_cfg.end(); it++)
	{
		std::vector<MazeRacerAwardCfg>().swap(it->second);
	}

	for (std::map<int, std::map<ParamSection, std::vector<ActivityRankRewardCfg> > >::iterator it = m_rank_award.begin(); it != m_rank_award.end(); it++){
		for (std::map<ParamSection, std::vector<ActivityRankRewardCfg> >::iterator node_it = it->second.begin(); node_it != it->second.end(); node_it++){
			std::vector<ActivityRankRewardCfg>().swap(node_it->second);
		}
	}

	for (std::map<int, std::vector<MazeRaceBoxRewardCfg> >::iterator it = m_box_reward_group_cfg.begin(); it != m_box_reward_group_cfg.end(); it++)
	{
		std::vector<MazeRaceBoxRewardCfg>().swap(it->second);
	}
}

bool ActivityMazeRacerConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitMazeRacerOtherInfor);
	LOAD_CONFIG("scene", InitMazeRacerSceneGroup);
	LOAD_CONFIG("reward", InitMazeRacerAwardGroup);

	LOAD_CONFIG("tier", InitMazeRacerTierConfig);				// 必须放在场景组配置后面，检查场景组是否存在
	LOAD_CONFIG("maze", InitMazeRacerBasicInfor);				// 必须放在层数配置后面，检查迷宫ID是否存在
	LOAD_CONFIG("act_rank_reward", InitMazeRacerRankReward);	// 必须放在层数配置和奖励组配置后面，检查迷宫ID和奖励组是否存在
	LOAD_CONFIG("maze_rank_reward", InitMazeRacerCopyReward);	// 必须放在层数配和奖励组配置后面，检查迷宫ID和奖励组是否存在
	LOAD_CONFIG("charm", InitMazeRacerAmuletsCfg);
	LOAD_CONFIG("box_reward", InitMazeRacerBoxRewardGroupCfg);
	LOAD_CONFIG("box", InitMazeRacerBoxCfg);					// 必须放在宝箱奖励组后面读取,检测宝箱奖励组ID

	{
		iRet = this->CheckSceneGroup();	// 检查场景组里的场景数量是否合法
		if (iRet)
		{
			sprintf(errinfo, "PartnerConfig::CheckSceneGroup failed %d", iRet);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

int ActivityMazeRacerConfig::GetARandAmulet()
{
	int rand_rate = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::map<int, MazeRacerAumletCfg>::iterator loop = m_aumlet_cfg.begin(); loop != m_aumlet_cfg.end(); ++loop)
	{
		if (rand_rate < loop->second.drop_rate) return loop->first;
		rand_rate -= loop->second.drop_rate;
	}

	return (-1);
}

int ActivityMazeRacerConfig::GetMaxTierByMazeID(int maze_id)
{
	std::map<int, std::map<int, int> >::iterator f_it = m_tier_cfg.find(maze_id);
	if (m_tier_cfg.end() != f_it) return f_it->second.rbegin()->first;

	return 0;
}

int ActivityMazeRacerConfig::RandASceneIDByMazeAndTier(int maze_id, int tier, std::set<int> & eliminate_list)
{
	std::map<int, std::map<int, int> >::iterator f_it = m_tier_cfg.find(maze_id);
	if (m_tier_cfg.end() == f_it) return 0;

	std::map<int, int>::iterator scene_group_it = f_it->second.find(tier);
	if (f_it->second.end() == scene_group_it) return 0;

	int group_id = scene_group_it->second;
	std::map<int, std::set<int> >::iterator group_finder = m_scene_group_cfg.find(group_id);
	if (m_scene_group_cfg.end() == group_finder) return 0;

	std::vector<int> choice_list;
	for (std::set<int>::iterator loop = group_finder->second.begin(); loop != group_finder->second.end(); ++loop)
	{
		if (eliminate_list.end() == eliminate_list.find(*loop)) choice_list.push_back(*loop);
	}

	int resule_idx = rand() % (int)choice_list.size();
	return choice_list[resule_idx];
}

void ActivityMazeRacerConfig::GetAllWaitingHallSceneID(std::set<int> & out_list)
{
	out_list.clear();
	for (std::map<ParamSection, MazeRacerSceneCfg>::iterator loop = m_scene_cfg.begin(); loop != m_scene_cfg.end(); ++loop)
	{
		out_list.insert(loop->second.wait_scene_id);
	}
}

bool ActivityMazeRacerConfig::RandActivityAward(int maze_id, int rank_posi, int role_level, std::vector<ItemConfigData> & out_list)
{
	std::map<int, std::map<ParamSection, std::vector<ActivityRankRewardCfg> > >::iterator maze_it = m_rank_award.find(maze_id);
	if(m_rank_award.end() == maze_it) return false;

	for (std::map<ParamSection, std::vector<ActivityRankRewardCfg> >::iterator loop = maze_it->second.begin(); loop != maze_it->second.end(); ++loop)
	{
		if (rank_posi > loop->first.section_max || rank_posi < loop->first.section_min) continue;

		bool is_break = false;
		for (int i = 0; i < (int)loop->second.size(); i++)
		{
			if (loop->second[i].min_level <= role_level && loop->second[i].max_level >= role_level)
			{
				out_list.assign(loop->second[i].reward_list.begin(), loop->second[i].reward_list.end());
				is_break = true;
				break;
			}
		}
		if (is_break)
		{
			break;
		}
	}
	if (out_list.empty())
	{
		return false;
	}

	/*out_list.clear(); std::map<int, std::map<bool, int> > tmp_item_list;

	std::map<int, std::map<ParamSection, std::vector<int> > >::iterator maze_it = m_rank_award.find(maze_id);
	if (m_rank_award.end() == maze_it) return false;

	std::vector<int> tmp_award_group;
	for (std::map<ParamSection, std::vector<int> >::iterator loop = maze_it->second.begin(); loop != maze_it->second.end(); ++loop)
	{
		if (rank_posi > loop->first.section_max || rank_posi < loop->first.section_min) continue;

		tmp_award_group.assign(loop->second.begin(), loop->second.end());
		break;
	}
	if (tmp_award_group.empty())
	{
		return false;
	}
	
	for (std::vector<int>::iterator award_id_it = tmp_award_group.begin(); award_id_it != tmp_award_group.end(); ++award_id_it)
	{
		std::map<int, std::vector<MazeRacerAwardCfg> >::iterator award_group_it = m_award_cfg.find(*award_id_it);
		if (m_award_cfg.end() == award_group_it) return false;

		int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<MazeRacerAwardCfg>::iterator award_it = award_group_it->second.begin(); award_it != award_group_it->second.end(); ++award_it)
		{
			if (rand_prob < award_it->award_prob)
			{
				tmp_item_list[award_it->award_item.item_id][award_it->award_item.is_bind] += award_it->award_item.num;
				break;		// 只随机一件
			}

			rand_prob -= award_it->award_prob;
		}
	}

	for (std::map<int, std::map<bool, int> >::iterator out_inserter = tmp_item_list.begin(); out_inserter != tmp_item_list.end(); ++out_inserter)
	{
		for (std::map<bool, int>::iterator binder_it = out_inserter->second.begin(); binder_it != out_inserter->second.end(); ++binder_it)
		{
			out_list.push_back(ItemConfigData(out_inserter->first, binder_it->first, binder_it->second));
		}
	}*/
	return true;
}

void ActivityMazeRacerConfig::RandCopyRankAward(int maze_id, int rank_posi,int level, std::vector<ItemConfigData> & out_list)
{
	out_list.clear(); std::map<int, std::map<bool, int> > tmp_item_list;

	std::map<int, RankAward>::iterator maze_it = m_copy_award.find(maze_id);
	if (m_copy_award.end() == maze_it) return;

	std::vector<int> tmp_award_group;
	for (std::map<ParamSection, RankAward_Rank>::iterator loop = maze_it->second.rank_posi.begin(); loop != maze_it->second.rank_posi.end(); ++loop)
	{
		if (rank_posi > loop->first.section_max || rank_posi < loop->first.section_min) continue;
		for (std::map<ParamSection, RankAward_Level>::iterator loop2 = loop->second.level_posi.begin();loop2 != loop->second.level_posi.end();loop2++)
		{
			if (level > loop2->first.section_max || level < loop2->first.section_min) continue;
			tmp_award_group.assign(loop2->second.m_rank_award_list.begin(), loop2->second.m_rank_award_list.end());
			break;
		}
	}

	for (std::vector<int>::iterator award_id_it = tmp_award_group.begin(); award_id_it != tmp_award_group.end(); ++award_id_it)
	{
		std::map<int, std::vector<MazeRacerAwardCfg> >::iterator award_group_it = m_award_cfg.find(*award_id_it);
		if (m_award_cfg.end() == award_group_it) return;

		int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<MazeRacerAwardCfg>::iterator award_it = award_group_it->second.begin(); award_it != award_group_it->second.end(); ++award_it)
		{
			if (rand_prob < award_it->award_prob)
			{
				tmp_item_list[award_it->award_item.item_id][award_it->award_item.is_bind] += award_it->award_item.num;
				break;		// 只随机一件
			}

			rand_prob -= award_it->award_prob;
		}
	}

	for (std::map<int, std::map<bool, int> >::iterator out_inserter = tmp_item_list.begin(); out_inserter != tmp_item_list.end(); ++out_inserter)
	{
		for (std::map<bool, int>::iterator binder_it = out_inserter->second.begin(); binder_it != out_inserter->second.end(); ++binder_it)
		{
			out_list.push_back(ItemConfigData(out_inserter->first, binder_it->first, binder_it->second));
		}
	}
}

const MazeRacerAumletCfg * ActivityMazeRacerConfig::GetAmuletConfig(int amulet_id)
{
	std::map<int, MazeRacerAumletCfg>::iterator f_it = m_aumlet_cfg.find(amulet_id);
	if (m_aumlet_cfg.end() != f_it) return & f_it->second;

	return NULL;
}

const MazeRacerSceneCfg * ActivityMazeRacerConfig::GetSceneConfig(int level)
{
	for (std::map<ParamSection, MazeRacerSceneCfg>::iterator loop = m_scene_cfg.begin(); loop != m_scene_cfg.end(); ++loop)
	{
		if (level >= loop->first.section_min && level <= loop->first.section_max) return & loop->second;
	}
	
	return NULL;
}

const MazeRacerWaitingHall * ActivityMazeRacerConfig::GetWaitingHallInfoBySceneID(int scene_id)
{
	std::map<int, MazeRacerWaitingHall>::iterator f_it = m_hall_info_cfg.find(scene_id);
	if (m_hall_info_cfg.end() != f_it) return & f_it->second;

	return NULL;
}

const MazeRaceBoxCfg * ActivityMazeRacerConfig::GetRandMazeRaceBoxCfg()
{
	if(m_box_cfg.empty()) return NULL;
	int rate_num = rand() % box_weight;
	for (std::map<int, MazeRaceBoxCfg>::const_iterator it = m_box_cfg.begin(); it != m_box_cfg.end(); ++it)
	{
		if (rate_num < it->second.rate)
		{
			return &it->second;
		}
		rate_num -= it->second.rate;
	}
	
	return NULL;
}

void ActivityMazeRacerConfig::GetMazeRaceBoxReward(int box_id, std::vector<ItemConfigData>& out_reward_list, bool is_leader)
{
	std::map<int, MazeRaceBoxCfg>::const_iterator box_it = m_box_cfg.find(box_id);
	if(box_it == m_box_cfg.end()) return;

	if (is_leader)
	{
		if(box_it->second.captain_reward_num <= 0) return;

		for (int i = 0; i < box_it->second.captain_reward_num && i < MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM; ++i)
		{
			std::map<int, std::vector<MazeRaceBoxRewardCfg> >::const_iterator reward_it = m_box_reward_group_cfg.find(box_it->second.captain_reward_group_id_list[i]);
			if(reward_it == m_box_reward_group_cfg.end()) continue;

			int rate_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			for (int k = 0; k < (int)reward_it->second.size(); ++k)
			{
				if (rate_num < reward_it->second[k].rate)
				{
					out_reward_list.push_back(reward_it->second[k].item_reward);
					break;
				}
				rate_num -= reward_it->second[k].rate;
			}
		}
	}
	else
	{
		if(box_it->second.reward_num <= 0) return;

		for (int i = 0; i < box_it->second.reward_num && i < MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM; ++i)
		{
			std::map<int, std::vector<MazeRaceBoxRewardCfg> >::const_iterator reward_it = m_box_reward_group_cfg.find(box_it->second.reward_group_id_list[i]);
			if (reward_it == m_box_reward_group_cfg.end()) continue;

			int rate_num = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			for (int k = 0; k < (int)reward_it->second.size(); ++k)
			{
				if (rate_num < reward_it->second[k].rate)
				{
					out_reward_list.push_back(reward_it->second[k].item_reward);
					break;
				}
				rate_num -= reward_it->second[k].rate;
			}
		}
	}
}

int ActivityMazeRacerConfig::InitMazeRacerOtherInfor(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "reward_time", m_mr_other_cfg.act_reward_time) || !this->IsTimerCfg(m_mr_other_cfg.act_reward_time))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "time", m_mr_other_cfg.waiting_interval) || m_mr_other_cfg.waiting_interval < 0)
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "wait_num", m_mr_other_cfg.waiting_num_max) || m_mr_other_cfg.waiting_num_max < 0)
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "npc_id", m_mr_other_cfg.trans_npc_id) || m_mr_other_cfg.trans_npc_id <= 0)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "min_team", m_mr_other_cfg.min_member_need) || m_mr_other_cfg.min_member_need <= 0 || m_mr_other_cfg.min_member_need > MAX_TEAM_MEMBER_NUM)
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "refresh_time", m_mr_other_cfg.box_refresh_time) || m_mr_other_cfg.box_refresh_time <= 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "num", m_mr_other_cfg.box_refresh_num) || m_mr_other_cfg.box_refresh_num <= 0)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "max_num", m_mr_other_cfg.box_max_num) || m_mr_other_cfg.box_max_num < m_mr_other_cfg.box_refresh_num)
	{
		return -8;
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerBasicInfor(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		MazeRacerSceneCfg t_mrsc;
		if (!GetSubNodeValue(root_element, "min_level", t_mrsc.min_lv) || t_mrsc.min_lv < 0 || t_mrsc.min_lv > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", t_mrsc.max_lv) || t_mrsc.max_lv < t_mrsc.min_lv || t_mrsc.max_lv > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		ParamSection t_ps(t_mrsc.min_lv, t_mrsc.max_lv);
		if (m_scene_cfg.end() != m_scene_cfg.find(t_ps))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "maze_id", t_mrsc.maze_id) || m_tier_cfg.end() == m_tier_cfg.find(t_mrsc.maze_id))
		{
			return -4;
		}
		
		if (!GetSubNodeValue(root_element, "wait_scene", t_mrsc.wait_scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(t_mrsc.wait_scene_id))
		{
			return -5;
		}

		if (m_hall_info_cfg.end() != m_hall_info_cfg.find(t_mrsc.wait_scene_id))		// 等待场景不能重复
		{
			return -6;
		}

		m_scene_cfg[t_ps] = t_mrsc;
		m_hall_info_cfg[t_mrsc.wait_scene_id] = MazeRacerWaitingHall(t_mrsc.maze_id, t_ps.section_min, t_ps.section_max);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerTierConfig(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int maze_id = 0, tier = 0, scene_group = 0;
		if (!GetSubNodeValue(root_element, "maze_id", maze_id) || maze_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "num", tier) || tier <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "scene_group", scene_group) || m_scene_group_cfg.end() == m_scene_group_cfg.find(scene_group))
		{
			return -3;
		}

		std::map<int, std::map<int, int> >::iterator f_it = m_tier_cfg.find(maze_id);
		if (m_tier_cfg.end() != f_it)
		{
			if (f_it->second.rbegin()->first + 1 != tier) return -4;	// 必须连续
		}
		else
		{
			if (MAZE_RACER_MIN_LAYER != tier) return -5;		// 必须从第 1 层开始
		}

		m_tier_cfg[maze_id][tier] = scene_group;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerSceneGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int scene_group_id = 0, scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_group", scene_group_id) || scene_group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "scene_id", scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -2;
		}

		std::map<int, std::set<int> >::iterator f_it = m_scene_group_cfg.find(scene_group_id);
		if (m_scene_group_cfg.end() != f_it)
		{
			if (f_it->second.end() != f_it->second.find(scene_id)) return -3;
			f_it->second.insert(scene_id);
		}
		else
		{
			std::set<int> tmp_set; tmp_set.insert(scene_id);
			m_scene_group_cfg.insert(std::pair<int, std::set<int> >(scene_group_id, tmp_set));
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerAwardGroup(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	std::map<int, int> tmp_ratio_total_map;
	while (NULL != root_element)
	{
		int award_group_id; MazeRacerAwardCfg t_mrac;
		if (!GetSubNodeValue(root_element, "reward_group", award_group_id) || award_group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", t_mrac.award_prob) || t_mrac.award_prob <= 0 || t_mrac.award_prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_element = root_element->FirstChildElement("reward");
		if (NULL == item_element) return -3;		// 必须要有奖励

		if (!t_mrac.award_item.ReadConfig(item_element))
		{
			return -4;
		}

		std::map<int, std::vector<MazeRacerAwardCfg> >::iterator f_it = m_award_cfg.find(award_group_id);
		if (m_award_cfg.end() != f_it)
		{
			f_it->second.push_back(t_mrac);
		}
		else
		{
			std::vector<MazeRacerAwardCfg> tmp_vec; tmp_vec.push_back(t_mrac);
			m_award_cfg.insert(std::pair<int, std::vector<MazeRacerAwardCfg> >(award_group_id, tmp_vec));
		}

		tmp_ratio_total_map[award_group_id] += t_mrac.award_prob;
		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, int>::iterator loop = tmp_ratio_total_map.begin(); loop != tmp_ratio_total_map.end(); ++loop)
	{
		if (loop->second > RAND_UNIVERSAL_RATIO_BASE_NUM) return -3000;
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerRankReward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int maze_id = 0; ParamSection rank_section; ActivityRankRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "maze_id", maze_id) || m_tier_cfg.end() == m_tier_cfg.find(maze_id))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "rank_min", rank_section.section_min) || rank_section.section_min <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rank_max", rank_section.section_max) || rank_section.section_max < rank_section.section_min)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level <= 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		TiXmlElement * reward_show_list = root_element->FirstChildElement("reward_show_list");
		if (NULL == reward_show_list) return -1000;

		TiXmlElement * reward_show = reward_show_list->FirstChildElement("reward_show");
		while (NULL != reward_show)
		{
			ItemConfigData item;
			if (!item.ReadConfig(reward_show))
			{
				return -1001;
			}

			node_cfg.reward_list.push_back(item);
			reward_show = reward_show->NextSiblingElement();
		}

		std::map<int, std::map<ParamSection, std::vector<ActivityRankRewardCfg> > >::iterator f_it = m_rank_award.find(maze_id);
		if (m_rank_award.end() != f_it)
		{
			if (f_it->second.empty() && 1 != rank_section.section_min) return -40;		// 写死，必须从第一名开始
			if (!f_it->second.empty())
			{
				if (rank_section.section_min <= f_it->second.rbegin()->first.section_max)
				{
					if (!f_it->second.rbegin()->second.empty())
					{
						if (f_it->second.rbegin()->second.rbegin()->min_level == node_cfg.min_level)
						{
							return -50;
						}
					}
				}
			}
			m_rank_award[maze_id][rank_section].push_back(node_cfg);
		}
		else
		{
			if(1 != rank_section.section_min) return -40;		// 写死，必须从第一名开始
				
			m_rank_award[maze_id][rank_section].push_back(node_cfg);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerCopyReward(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int maze_id = 0; ParamSection rank_section,level_section;
		if (!GetSubNodeValue(root_element, "maze_id", maze_id) || m_tier_cfg.end() == m_tier_cfg.find(maze_id))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "rank_min", rank_section.section_min) || rank_section.section_min <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rank_max", rank_section.section_max) || rank_section.section_max < rank_section.section_min)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "min_level", level_section.section_min) || level_section.section_min <= 0)
		{
			return -21;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_section.section_max) || level_section.section_max < level_section.section_min)
		{
			return -31;
		}

		TiXmlElement * reward_group_id_list = root_element->FirstChildElement("reward_group");
		if (NULL == reward_group_id_list) return -1000;

		std::vector<int> tmp_award_group_id_list;
		TiXmlElement * reward_group_id = reward_group_id_list->FirstChildElement("node");
		while (NULL != reward_group_id)
		{
			int tmp_award_group_id = 0;
			if (!GetNodeValue(reward_group_id, tmp_award_group_id) || m_award_cfg.end() == m_award_cfg.find(tmp_award_group_id))
			{
				return -2000;
			}

			tmp_award_group_id_list.push_back(tmp_award_group_id);
			reward_group_id = reward_group_id->NextSiblingElement();
		}

		std::map<int, RankAward>::iterator f_it = m_copy_award.find(maze_id);
		if (m_copy_award.end() != f_it)
		{
			std::map<ParamSection, RankAward_Rank>::iterator s_it = f_it->second.rank_posi.find(rank_section);
			if (s_it != f_it->second.rank_posi.end())
			{
				std::map<ParamSection, RankAward_Level>::iterator t_it = s_it->second.level_posi.find(level_section);
				if (t_it != s_it->second.level_posi.end())
				{
					return -2001;
				}
				else
				{
					s_it->second.level_posi[level_section].m_rank_award_list = tmp_award_group_id_list;
				}
			}
			else
			{
				f_it->second.rank_posi[rank_section].level_posi[level_section].m_rank_award_list = tmp_award_group_id_list;
			}
		}
		else
		{
			//if (1 != rank_section.section_min) return -50;
			m_copy_award[maze_id].rank_posi[rank_section].level_posi[level_section].m_rank_award_list = tmp_award_group_id_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerAmuletsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int drop_rate_total = 0;
	while (NULL != root_element)
	{
		int amulet_id = 0; MazeRacerAumletCfg t_mrac;
		if (!GetSubNodeValue(root_element, "charm_id", amulet_id) || amulet_id < 0 || amulet_id >= ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE || m_aumlet_cfg.end() != m_aumlet_cfg.find(amulet_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "effect_type", t_mrac.effect_type) || t_mrac.effect_type < 0 || t_mrac.effect_type >= MAZE_AMULET_EFFECT_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "effect_val", t_mrac.effect_val))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "duration", t_mrac.duration) || t_mrac.duration <= 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "target", t_mrac.target_type) || t_mrac.target_type < 0 || t_mrac.target_type >= MAZE_AMULET_TARGET_TYPE_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "rate", t_mrac.drop_rate) || t_mrac.drop_rate < 0 || t_mrac.drop_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -6;
		}
		drop_rate_total += t_mrac.drop_rate;

		m_aumlet_cfg[amulet_id] = t_mrac;
		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_aumlet_cfg.size() > ACTIVITY_MAZE_RACER_AMULET_MAX_TYPE) return -100;
	if (drop_rate_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -101;	// 如果符文掉落概率总和大于 10000，报错

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	box_weight = 0;
	while (NULL != root_element)
	{
		int box_id = 0;
		if (!GetSubNodeValue(root_element, "box_id", box_id) || box_id <= 0)
		{
			return -1;
		}
		const ServerNPCInfoCfg* box_cfg = NPCPOOL->GetServerNPCInfo(box_id);
		if (NULL == box_cfg || SERVER_NPC_TYPE_GATHERS != box_cfg->server_npc_type)
		{
			return -2;
		}
		if (m_box_cfg.end() != m_box_cfg.find(box_id))
		{
			return -3;
		}

		MazeRaceBoxCfg node_cfg;
		node_cfg.box_id = box_id;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0)
		{
			return -4;
		}

		{
			TiXmlElement * award_list = root_element->FirstChildElement("reward_group_id");
			if (NULL == award_list) return -100;

			TiXmlElement * award_node = award_list->FirstChildElement("node");
			while (NULL != award_node)
			{
				if (node_cfg.reward_num >= MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM) return -(110 + node_cfg.reward_num);
				if (!GetNodeValue(award_node, node_cfg.reward_group_id_list[node_cfg.reward_num]) || m_box_reward_group_cfg.end() == m_box_reward_group_cfg.find(node_cfg.reward_group_id_list[node_cfg.reward_num]))
				{
					return -(150 + node_cfg.reward_num);
				}

				++node_cfg.reward_num;
				award_node = award_node->NextSiblingElement();
			}
		}

		{
			TiXmlElement * captian_award_list = root_element->FirstChildElement("captain_reward_group_id");
			if (NULL == captian_award_list) return -200;

			TiXmlElement * captian_award_node = captian_award_list->FirstChildElement("node");
			while (NULL != captian_award_node)
			{
				if (node_cfg.captain_reward_num >= MAX_MAZE_RACE_BOX_REWARD_GROUP_NUM) return -(210 + node_cfg.captain_reward_num);
				if (!GetNodeValue(captian_award_node, node_cfg.captain_reward_group_id_list[node_cfg.captain_reward_num]) || m_box_reward_group_cfg.end() == m_box_reward_group_cfg.find(node_cfg.captain_reward_group_id_list[node_cfg.captain_reward_num]))
				{
					return -(250 + node_cfg.captain_reward_num);
				}

				++node_cfg.captain_reward_num;
				captian_award_node = captian_award_node->NextSiblingElement();
			}
		}
		box_weight += node_cfg.rate;
		m_box_cfg[box_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityMazeRacerConfig::InitMazeRacerBoxRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_id", reward_group_id) || reward_group_id <= 0)
		{
			return -1;
		}
		MazeRaceBoxRewardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * reward_element = root_element->FirstChildElement("reward_item");
		if (NULL == reward_element)
		{	
			return -100;
		}
		if (!node_cfg.item_reward.ReadConfig(reward_element))
		{
			return -101;
		}

		m_box_reward_group_cfg[reward_group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	for (std::map<int, std::vector<MazeRaceBoxRewardCfg> >::iterator it = m_box_reward_group_cfg.begin(); it != m_box_reward_group_cfg.end(); ++it)
	{
		int rate_count = 0;
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			rate_count += it->second[i].rate;
		}
		if (rate_count != RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -(10000 + it->first);
		}
	}

	return 0;
}

bool ActivityMazeRacerConfig::IsTimerCfg(int check_time)
{
	int _hour = check_time / 100;
	int _min = check_time % 100;
	return _hour >= 0 && _hour < 24 && _min >= 0 && _min < 60;
}

int ActivityMazeRacerConfig::CheckSceneGroup()
{
	int max_tier = 0;
	for (std::map<int, std::map<int, int> >::iterator loop = m_tier_cfg.begin(); loop != m_tier_cfg.end(); ++loop)
	{
		max_tier = GetMax(max_tier, this->GetMaxTierByMazeID(loop->first));
	}

	for (std::map<int, std::set<int> >::iterator scene_group_checker = m_scene_group_cfg.begin(); scene_group_checker != m_scene_group_cfg.end(); ++scene_group_checker)
	{
		if ((int)scene_group_checker->second.size() < max_tier) return -1;
	}

	return 0;
}
