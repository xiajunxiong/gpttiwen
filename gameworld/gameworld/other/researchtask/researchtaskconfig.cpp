#include "researchtaskconfig.hpp"
#include "world.h"
#include "monster/monsterpool.hpp"
#include "scene/scenemanager.hpp"
#include "npc/npcpool.h"
#include "servercommon/commondata.hpp"

ResearchTaskConfig::ResearchTaskConfig()
{
}

ResearchTaskConfig::~ResearchTaskConfig()
{
	for (std::map<int, std::vector<RandSceneCon> >::iterator it = m_rand_scene_map.begin(); it != m_rand_scene_map.end(); it++) {
		std::vector<RandSceneCon>().swap(it->second);
	}
	for (std::map<int, std::vector<RandDemonCon> >::iterator it = m_rand_demon_map.begin(); it != m_rand_demon_map.end(); it++) {
		std::vector<RandDemonCon>().swap(it->second);
	}
	for (std::map<int, std::vector<RandPosCon> >::iterator it = m_rand_pos_map.begin(); it != m_rand_pos_map.end(); it++) {
		std::vector<RandPosCon>().swap(it->second);
	}
	for (std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator it = m_rand_award_map.begin(); it != m_rand_award_map.end(); it++) {
		for (std::map<ParamSection, std::vector<RandAwardCon> >::iterator node_it = it->second.begin(); node_it != it->second.end(); node_it++){
			std::vector<RandAwardCon>().swap(node_it->second);
		}
	}
	for (std::map<int, std::map<ParamSection, std::vector<RTRoundRewardCon> > >::iterator it = m_round_reward_map.begin(); it != m_round_reward_map.end(); it++) {
		for (std::map<ParamSection, std::vector<RTRoundRewardCon> >::iterator node_it = it->second.begin(); node_it != it->second.end(); node_it++) {
			std::vector<RTRoundRewardCon>().swap(node_it->second);
		}
	}

	for (std::map<int, std::vector<RTRandGroupCon> >::iterator it = m_back_item_group_map.begin(); it != m_back_item_group_map.end(); it++) {
		std::vector<RTRandGroupCon>().swap(it->second);
	}
}

bool ResearchTaskConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;
	
	LOAD_CONFIG("other", InitZTaskOtherCfg);
	LOAD_CONFIG("pos_group", InitPosGroupCfg);
	LOAD_CONFIG("scene_group", InitSceneGroupCfg);
	LOAD_CONFIG("monster_group", InitDemonGroupCfg);
	LOAD_CONFIG("reward", InitAwardGroupCfg);
	LOAD_CONFIG("mission_cfg", InitTasksBasicCfg);	// 需要检测 场景组随机集合 和 怪物组随机集合，必须放在这两者后面
	LOAD_CONFIG("mission_reward", InitTaskRewardCfg);
	LOAD_CONFIG("reward_group", InitBackGroupsCfg);
	LOAD_CONFIG("reward_find", InitRewardBackCfg);
	LOAD_CONFIG("reward_group_rate", InitOneRundRewardCfg);
	LOAD_CONFIG("times_reward_group", InitRundRewardGroupCfg);

	return true;
}

const ResearchTaskCfg * ResearchTaskConfig::GetTaskByLevel(int level)
{
	for (std::map<int, ResearchTaskCfg>::iterator loop_it = m_research_task_cfg.begin(); loop_it != m_research_task_cfg.end(); ++loop_it)
	{
		if (level >= loop_it->second.lv_limit_min && level <= loop_it->second.lv_limit_max) return & loop_it->second;
	}

	return NULL;
}

const ResearchTaskCfg * ResearchTaskConfig::GetTaskByTaskID(int task_id)
{
	std::map<int, ResearchTaskCfg>::iterator f_it = m_research_task_cfg.find(task_id);
	if (m_research_task_cfg.end() != f_it) return & f_it->second;
	
	return NULL;
}

const ResearchRewardCfg * ResearchTaskConfig::GetTaskReward(int role_level)
{
	for (std::map<ParamSection, ResearchRewardCfg>::iterator loop = m_task_reward_map.begin(); loop != m_task_reward_map.end(); ++loop)
	{
		if (role_level >= loop->first.section_min && role_level <= loop->first.section_max) return & loop->second;
	}

	return NULL;
}

const AwardBackCon * ResearchTaskConfig::GetRewardBack(int role_level, int back_type)
{
	for (std::map<ParamSection, std::map<int, AwardBackCon> >::iterator level_it = m_reward_back_map.begin(); level_it != m_reward_back_map.end(); ++level_it)
	{
		if (role_level < level_it->first.section_min || role_level > level_it->first.section_max) continue;

		std::map<int, AwardBackCon>::iterator type_it = level_it->second.find(back_type);
		if (level_it->second.end() != type_it) return & type_it->second;
	}

	return NULL;
}

const ItemConfigData * ResearchTaskConfig::RandBackItem(int group_id)
{
	std::map<int, std::vector<RTRandGroupCon> >::iterator group_it = m_back_item_group_map.find(group_id);
	if (m_back_item_group_map.end() == group_it) return NULL;

	int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::vector<RTRandGroupCon>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
	{
		if (rand_prob < item_it->prob) return & item_it->item;
		rand_prob -= item_it->prob;
	}

	return NULL;
}

bool ResearchTaskConfig::GetRandSceneAndDemonByTaskID(int task_id, int * scene_id, int * demon_id, int * image_id, ARG_OUT Posi* pos)
{
	std::map<int, ResearchTaskCfg>::iterator f_it = m_research_task_cfg.find(task_id);
	if (m_research_task_cfg.end() == f_it) return false;

	int pos_group_id = 0;
	
	{
		std::map<int, std::vector<RandSceneCon> >::iterator scene_rand_it = m_rand_scene_map.find(f_it->second.scene_group);
		if (m_rand_scene_map.end() == scene_rand_it) return false;
	
		int scene_rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<RandSceneCon>::iterator scene_loop_it = scene_rand_it->second.begin(); scene_loop_it != scene_rand_it->second.end(); ++scene_loop_it)
		{
			if (scene_rand_prob < scene_loop_it->rand_rate)
			{
				*scene_id = scene_loop_it->scene_id;
				pos_group_id = scene_loop_it->pos_group_id;
				break;
			}

			scene_rand_prob -= scene_loop_it->rand_rate;
		}
	}

	{
		bool found_pos = false;
		std::map<int, std::vector<RandPosCon> >::iterator it = m_rand_pos_map.find(pos_group_id);
		if (it != m_rand_pos_map.end())
		{
			int prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			std::vector<RandPosCon>::iterator iter = it->second.begin();
			for (; iter != it->second.end(); ++iter)
			{
				if (prob < iter->rand_rate)
				{
					pos->x = iter->pos_x;
					pos->y = iter->pos_y;
					found_pos = true;
					break;
				}

				prob -= iter->rand_rate;
			}
		}

#ifdef _DEBUG
		assert(found_pos);
#endif
	}

	{
		std::map<int, std::vector<RandDemonCon> >::iterator demon_rand_it = m_rand_demon_map.find(f_it->second.demon_group);
		if (m_rand_demon_map.end() == demon_rand_it) return false;
		
		int demon_rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
		for (std::vector<RandDemonCon>::iterator demon_loop_it = demon_rand_it->second.begin(); demon_loop_it != demon_rand_it->second.end(); ++demon_loop_it)
		{
			if (demon_rand_prob < demon_loop_it->rand_rate)
			{
				*demon_id = demon_loop_it->battle_group_id;
				*image_id = demon_loop_it->img_id;
				break;
			}

			demon_rand_prob -= demon_loop_it->rand_rate;
		}
	}

	return true;
}

void ResearchTaskConfig::GetAwardTimesListByFinishTimes(int finish_times, std::set<int> & out_list)
{
	out_list.clear();

	for (std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator loop_it = m_rand_award_map.begin(); loop_it != m_rand_award_map.end(); ++loop_it)
	{
		if (finish_times >= loop_it->first) out_list.insert(loop_it->first);
	}
}

int ResearchTaskConfig::GetAwardSeqByAwardNeedTimes(int need_times)
{
	std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator f_it = m_rand_award_map.find(need_times);
	if (m_rand_award_map.end() != f_it) return std::distance(m_rand_award_map.begin(), f_it);

	return -1;
}

bool ResearchTaskConfig::RandAwardList(int level, int need_times, int * out_list_len, ItemConfigData * out_award_list)
{
	*out_list_len = 0;
	std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator f_it = m_rand_award_map.find(need_times);
	if (m_rand_award_map.end() == f_it) return false;

	for (std::map<ParamSection, std::vector<RandAwardCon> >::iterator level_it = f_it->second.begin(); level_it != f_it->second.end(); ++level_it)
	{
		if (level < level_it->first.section_min || level > level_it->first.section_max) continue;

		for (std::vector<RandAwardCon>::iterator loop_it = level_it->second.begin(); loop_it != level_it->second.end(); ++loop_it)
		{
			int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
			if (rand_prob < loop_it->give_rate) out_award_list[(*out_list_len)++] = loop_it->award_item;
		}
	}

	return true;
}

const AwardBackCon* ResearchTaskConfig::GetFindType(int _role_level)
{
	for (std::map<ParamSection, std::map<int, AwardBackCon> >::iterator level_it = m_reward_back_map.begin(); level_it != m_reward_back_map.end(); ++level_it)
	{
		if (_role_level < level_it->first.section_min || _role_level > level_it->first.section_max) continue;

		std::map<int, AwardBackCon>::iterator type_it = m_seq_to_rrcfg.find(level_it->first.seq);
		if (m_seq_to_rrcfg.end() != type_it) return &type_it->second;
	}

	return NULL;
}

const std::vector<RTRoundRewardCon>* ResearchTaskConfig::RandRewardListRoundCfg(int need_times, int role_level)
{
	std::map<int, std::map<ParamSection, std::vector<RTRoundRewardCon> > >::iterator it = m_round_reward_map.find(need_times);
	if (it != m_round_reward_map.end())
	{
		for (std::map<ParamSection, std::vector<RTRoundRewardCon> >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (role_level < it2->first.section_min || role_level > it2->first.section_max) continue;
			
			return &it2->second;
		}
	}
	return NULL;
}

const RTRoundRewardCfg * ResearchTaskConfig::RandRoundRewardCfg(int group_id)
{
	std::map<int, RTRoundRewardGroupCfg>::iterator it = m_round_reward_group_map.find(group_id);
	if (it != m_round_reward_group_map.end())
	{
		int rand_num = 1;
		if (it->second.weight > 0)
		{
			rand_num = rand() % it->second.weight;
		}

		for (std::vector<RTRoundRewardCfg>::iterator it2 = it->second.m_reward_vec.begin(); it2 != it->second.m_reward_vec.end(); it2++)
		{
			if (it2->rate >= rand_num)
			{
				return &(*it2);
			}
			rand_num -= it2->rate;
		}
	}
	return NULL;
}

int ResearchTaskConfig::InitZTaskOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "refresh_time", m_zzt_other_cfg.daily_refresh_time) || !this->IsTimerCfg(m_zzt_other_cfg.daily_refresh_time))
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "fast_complete_cost", m_zzt_other_cfg.one_key_cost_per_times) || m_zzt_other_cfg.one_key_cost_per_times < 0)
		{
			return -2;
		}
		
		if (!GetSubNodeValue(root_element, "fast_complete_num", m_zzt_other_cfg.one_key_auto_max_times) || m_zzt_other_cfg.one_key_auto_max_times < 0)
		{
			return -3;
		}
		
		if (!GetSubNodeValue(root_element, "daily_times", m_zzt_other_cfg.max_daily_task_times) || m_zzt_other_cfg.max_daily_task_times < 0)
		{
			return -4;
		}
		
		if (!GetSubNodeValue(root_element, "min_team_num", m_zzt_other_cfg.team_need_member_min) || m_zzt_other_cfg.team_need_member_min < 0 || m_zzt_other_cfg.team_need_member_min > MAX_TEAM_MEMBER_NUM)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "reward_power", m_zzt_other_cfg.gather_power_award) || m_zzt_other_cfg.gather_power_award < 0)
		{
			return -6;
		}
		
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int ResearchTaskConfig::InitTasksBasicCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		ResearchTaskCfg t_rtc;
		if (!GetSubNodeValue(root_element, "task_id", t_rtc.task_id) || t_rtc.task_id <= 0 || m_research_task_cfg.end() != m_research_task_cfg.find(t_rtc.task_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", t_rtc.lv_limit_min) || t_rtc.lv_limit_min <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", t_rtc.lv_limit_max) || t_rtc.lv_limit_max < t_rtc.lv_limit_min)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "task_npc", t_rtc.task_npc_seq) || NULL == NPCPOOL->GetNPCInformaCfg(t_rtc.task_npc_seq))
		{
			return -6;	// 真中必定是客户端 NPC
		}

		if (!GetSubNodeValue(root_element, "scene_group_id", t_rtc.scene_group) || m_rand_scene_map.end() == m_rand_scene_map.find(t_rtc.scene_group))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "monster_group_id", t_rtc.demon_group) || m_rand_demon_map.end() == m_rand_demon_map.find(t_rtc.demon_group))
		{
			return -8;
		}
		
		m_research_task_cfg.insert(std::pair<int, ResearchTaskCfg>(t_rtc.task_id, t_rtc));
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int ResearchTaskConfig::InitSceneGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		int scene_group = 0; RandSceneCon t_rsc;
		if (!GetSubNodeValue(root_element, "scene_group_id", scene_group))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "scene_id", t_rsc.scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(t_rsc.scene_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", t_rsc.rand_rate) || t_rsc.rand_rate <= 0 || t_rsc.rand_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "pos_group", t_rsc.pos_group_id) || t_rsc.pos_group_id <= 0)
		{
			return -4;
		}
		if (m_rand_pos_map.find(t_rsc.pos_group_id) == m_rand_pos_map.end())
		{
			return -44;
		}

		std::map<int, std::vector<RandSceneCon> >::iterator f_it = m_rand_scene_map.find(scene_group);
		if (m_rand_scene_map.end() != f_it)		// 有对应组
		{
			f_it->second.push_back(t_rsc);
		}
		else		// 无对应组
		{
			std::vector<RandSceneCon> t_vec_rgc; t_vec_rgc.push_back(t_rsc);
			m_rand_scene_map.insert(std::pair<int, std::vector<RandSceneCon> >(scene_group, t_vec_rgc));
		}
		
		root_element = root_element->NextSiblingElement();
	}
	
	// 检测随机组的概率总和是否等于 10000
	for (std::map<int, std::vector<RandSceneCon> >::iterator loop_it = m_rand_scene_map.begin(); loop_it != m_rand_scene_map.end(); ++loop_it)
	{
		int rand_total = 0;
		for (std::vector<RandSceneCon>::iterator vec_it = loop_it->second.begin(); vec_it != loop_it->second.end(); ++vec_it)
		{
			rand_total += vec_it->rand_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -50;
	}

	return 0;
}

int ResearchTaskConfig::InitDemonGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		int demon_group = 0; RandDemonCon t_rdc;
		if (!GetSubNodeValue(root_element, "group_id", demon_group) || demon_group <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "monster_id", t_rdc.img_id) || !MonsterPool::GetInstance().IsMonsterExist(t_rdc.img_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "monster_group_id", t_rdc.battle_group_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "rate", t_rdc.rand_rate) || t_rdc.rand_rate <= 0 || t_rdc.rand_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -4;
		}

		std::map<int, std::vector<RandDemonCon> >::iterator f_it = m_rand_demon_map.find(demon_group);
		if (m_rand_demon_map.end() != f_it)		// 有对应组
		{
			f_it->second.push_back(t_rdc);
		}
		else		// 无对应组
		{
			std::vector<RandDemonCon> t_vec_rgc; t_vec_rgc.push_back(t_rdc);
			m_rand_demon_map.insert(std::pair<int, std::vector<RandDemonCon> >(demon_group, t_vec_rgc));
		}
		
		root_element = root_element->NextSiblingElement();
	}
	
	// 检测随机组的概率总和是否等于 10000
	for (std::map<int, std::vector<RandDemonCon> >::iterator loop_it = m_rand_demon_map.begin(); loop_it != m_rand_demon_map.end(); ++loop_it)
	{
		int rand_total = 0;
		for (std::vector<RandDemonCon>::iterator vec_it = loop_it->second.begin(); vec_it != loop_it->second.end(); ++vec_it)
		{
			rand_total += vec_it->rand_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -50;
	}

	return 0;
}

int ResearchTaskConfig::InitPosGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0)
		{
			return -1;
		}

		int pos_x = 0;
		if (!GetSubNodeValue(root_element, "x", pos_x) || pos_x <= 0)
		{
			return -2;
		}

		int pos_y = 0;
		if (!GetSubNodeValue(root_element, "y", pos_y) || pos_y <= 0)
		{
			return -3;
		}

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate < 0)
		{
			return -4;
		}

		static RandPosCon con;
		con.pos_group_id = group_id;
		con.pos_x = pos_x;
		con.pos_y = pos_y;
		con.rand_rate = rate;

		m_rand_pos_map[group_id].push_back(con);

		root_element = root_element->NextSiblingElement();
	}

	// 检测随机组的概率总和是否等于 10000
	for (std::map<int, std::vector<RandPosCon> >::iterator loop_it = m_rand_pos_map.begin(); loop_it != m_rand_pos_map.end(); ++loop_it)
	{
		int rand_total = 0;
		for (std::vector<RandPosCon>::iterator vec_it = loop_it->second.begin(); vec_it != loop_it->second.end(); ++vec_it)
		{
			rand_total += vec_it->rand_rate;
		}

		if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -50;
	}

	return 0;
}

int ResearchTaskConfig::InitAwardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		int finish_times = 0; ParamSection level_sec; RandAwardCon t_rac;
		if (!GetSubNodeValue(root_element, "times", finish_times) || finish_times < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", level_sec.section_min) || level_sec.section_min <= 0 || level_sec.section_min > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_sec.section_max) || level_sec.section_max < level_sec.section_min || level_sec.section_max > MAX_ROLE_LEVEL)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "rate", t_rac.give_rate) || t_rac.give_rate <= 0 || t_rac.give_rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * reward_node = root_element->FirstChildElement("reward");
		if (NULL == reward_node) return -3;

		if (!t_rac.award_item.ReadConfig(reward_node))
		{
			return -4;
		}

		std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator f_it = m_rand_award_map.find(finish_times);
		if (m_rand_award_map.end() != f_it)
		{
			bool found = false;
			for (std::map<ParamSection, std::vector<RandAwardCon> >::iterator level_it = f_it->second.begin(); level_it != f_it->second.end(); ++level_it)
			{
				if (level_it->first.section_min == level_sec.section_min && level_it->first.section_max == level_sec.section_max)
				{
					found = true;
					level_it->second.push_back(t_rac);
				}
			}

			if (!found)
			{
				std::vector<RandAwardCon> t_vec_rac; t_vec_rac.push_back(t_rac);
				m_rand_award_map[finish_times][level_sec] = t_vec_rac;
			}
		}
		else
		{
			std::vector<RandAwardCon> t_vec_rac; t_vec_rac.push_back(t_rac);
			m_rand_award_map[finish_times][level_sec] = t_vec_rac;
		}
		
		root_element = root_element->NextSiblingElement();
	}
	
	if ((int)m_rand_award_map.size() > RESEARCH_TASK_AWARD_MAX_SEQID) return -30;

	for (std::map<int, std::map<ParamSection, std::vector<RandAwardCon> > >::iterator loop_it = m_rand_award_map.begin(); loop_it != m_rand_award_map.end(); ++loop_it)
	{
		for (std::map<ParamSection, std::vector<RandAwardCon> >::iterator level_it = loop_it->second.begin(); level_it != loop_it->second.end(); ++level_it)
		{
			if (level_it->second.size() >= RESEARCH_TASK_AWARD_MAX_COUNT) return -20;
		}
	}

	return 0;
}

int ResearchTaskConfig::InitTaskRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		ParamSection tmp_ps; ResearchRewardCfg tmp_con;
		if (!GetSubNodeValue(root_element, "min_level", tmp_ps.section_min) || tmp_ps.section_min <= 0 || tmp_ps.section_min > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", tmp_ps.section_max) || tmp_ps.section_max < tmp_ps.section_min)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.experience) || tmp_con.experience < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin_bind) || tmp_con.coin_bind < 0)
		{
			return -4;
		}

		m_task_reward_map[tmp_ps] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ResearchTaskConfig::InitBackGroupsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	while (NULL != root_element)		// 允许空表
	{
		int group_id = 0; RTRandGroupCon tmp_con;
		if (!GetSubNodeValue(root_element, "group_id", group_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "rate", tmp_con.prob) || tmp_con.prob <= 0 || tmp_con.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL == item_node) return -10000;

		if (!tmp_con.item.ReadConfig(item_node))
		{
			return -3;
		}

		std::map<int, std::vector<RTRandGroupCon> >::iterator group_it = m_back_item_group_map.find(group_id);
		if (m_back_item_group_map.end() != group_it)
		{
			group_it->second.push_back(tmp_con);
		}
		else
		{
			std::vector<RTRandGroupCon> tmp_list; tmp_list.push_back(tmp_con);
			m_back_item_group_map[group_id] = tmp_list;
		}

		root_element = root_element->NextSiblingElement();
	}

	for (std::map<int, std::vector<RTRandGroupCon> >::iterator group_it = m_back_item_group_map.begin(); group_it != m_back_item_group_map.end(); ++group_it)
	{
		int prob_total = 0;
		for (std::vector<RTRandGroupCon>::iterator item_it = group_it->second.begin(); item_it != group_it->second.end(); ++item_it)
		{
			prob_total += item_it->prob;
		}

		if (prob_total > RAND_UNIVERSAL_RATIO_BASE_NUM) return -500;
	}

	return 0;
}

int ResearchTaskConfig::InitRewardBackCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	while (NULL != root_element)		// 允许空表
	{
		ParamSection level_sec; int find_type = 0; AwardBackCon tmp_con;
		if (!GetSubNodeValue(root_element, "min_level", level_sec.section_min) || level_sec.section_min <= 0 || level_sec.section_min > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_sec.section_max) || level_sec.section_max < level_sec.section_min || level_sec.section_max > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "find_type", tmp_con.find_type) || tmp_con.find_type < 0 || tmp_con.find_type >= RB_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.experience) || tmp_con.experience < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", tmp_con.coin_bind) || tmp_con.coin_bind < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "family_gongxian", tmp_con.family_contribution) || tmp_con.family_contribution < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "reward_group", tmp_con.item_group) || (tmp_con.item_group >= 0 && m_back_item_group_map.end() == m_back_item_group_map.find(tmp_con.item_group)) 
			|| (tmp_con.item_group < 0 && tmp_con.item_group != -1))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "seq", level_sec.seq) || level_sec.seq < 0)
		{
			return -8;
		}

		m_seq_to_rrcfg[level_sec.seq] = tmp_con;
		m_reward_back_map[level_sec][tmp_con.find_type] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ResearchTaskConfig::InitOneRundRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int finish_times = 0; ParamSection level_sec; RTRoundRewardCon t_rac;
		if (!GetSubNodeValue(root_element, "times", finish_times) || finish_times < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "min_level", level_sec.section_min) || level_sec.section_min <= 0 || level_sec.section_min > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "max_level", level_sec.section_max) || level_sec.section_max < level_sec.section_min || level_sec.section_max > MAX_ROLE_LEVEL)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "group_rate", t_rac.rate) || t_rac.rate <= 0 || t_rac.rate > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "group", t_rac.reward_group) || t_rac.reward_group < 0)
		{
			return -8;
		}

		std::map<int, std::map<ParamSection, std::vector<RTRoundRewardCon> > >::iterator f_it = m_round_reward_map.find(finish_times);
		if (m_round_reward_map.end() != f_it)
		{
			bool found = false;
			for (std::map<ParamSection, std::vector<RTRoundRewardCon> >::iterator level_it = f_it->second.begin(); level_it != f_it->second.end(); ++level_it)
			{
				if (level_it->first.section_min == level_sec.section_min && level_it->first.section_max == level_sec.section_max)
				{
					found = true;
					level_it->second.push_back(t_rac);
				}
			}

			if (!found)
			{
				std::vector<RTRoundRewardCon> t_vec_rac; t_vec_rac.push_back(t_rac);
				m_round_reward_map[finish_times][level_sec] = t_vec_rac;
			}
		}
		else
		{
			std::vector<RTRoundRewardCon> t_vec_rac; t_vec_rac.push_back(t_rac);
			m_round_reward_map[finish_times][level_sec] = t_vec_rac;
		}

		root_element = root_element->NextSiblingElement();
	}

	if ((int)m_round_reward_map.size() > RESEARCH_TASK_AWARD_MAX_SEQID) return -30;

	return 0;
}

int ResearchTaskConfig::InitRundRewardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group = 0;
		if (!GetSubNodeValue(root_element, "group", group) || group < 0)
		{
			return -1;
		}
		RTRoundRewardGroupCfg &node_cfg = m_round_reward_group_map[group];
		RTRoundRewardCfg node;
		if (!GetSubNodeValue(root_element, "rate", node.rate) || node.rate <= 0)
		{
			return -2;
		}
		node_cfg.weight += node.rate;
		TiXmlElement * reward_node = root_element->FirstChildElement("reward");
		if (NULL == reward_node) return -3;

		if (!node.reward.ReadConfig(reward_node))
		{
			return -3;
		}
		node_cfg.m_reward_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

bool ResearchTaskConfig::IsTimerCfg(int check_time)
{
	int _hour = check_time / 100;
	int _min = check_time % 100;
	return _hour >= 0 && _hour < 24 && _min >= 0 && _min < 60;
}