#include "activityteamfightconfig.hpp"
#include <algorithm>
#include "global/mail/mailmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "item/money.h"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include "gameworld/item/knapsack.h"

ActivityTeamFightConfig::ActivityTeamFightConfig() : 
	m_max_rank(0), m_match_teammate_rule_num(0), m_match_enemy_rule_num(0)
{
}

ActivityTeamFightConfig::~ActivityTeamFightConfig()
{
	std::map<int, std::vector<ActTeamFightRankCfg> >().swap(m_rank_cfg);
	std::map<int, std::vector<ActTeamFightBoxCfg> >().swap(m_box_cfg);

	for (int i = 0; i < ARRAY_LENGTH(m_reward_group_vec_list); i++)
	{
		RewardGroupVec().swap(m_reward_group_vec_list[i]);
	}

	std::vector<TeamFightJoinAwardCfg>().swap(m_join_award_vec);
	for (std::map<int, std::vector<TeamFightFindRewardGroupCfg> >::iterator it = m_find_reward_group_map.begin(); it != m_find_reward_group_map.end(); it++)
	{
		std::vector<TeamFightFindRewardGroupCfg>().swap(it->second);
	}
}

bool ActivityTeamFightConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("reward", InitRankCfg);
	LOAD_CONFIG("box_reward", InitBoxCfg);
	//LOAD_CONFIG("reward_group", InitRewardGroup);
	LOAD_CONFIG("teammate_match", InitTeamMateMatchRule);
	LOAD_CONFIG("opponent_match", InitEnemyMatchRule);
	LOAD_CONFIG("participation_reward", InitTeamFightJoinAwardCfg);		// 参与奖励
	LOAD_CONFIG("reward_find_group", InitTeamFightFindAwardGroupCfg);	// 找回奖励组
	LOAD_CONFIG("reward_find", InitTeamFightFindAwardCfg);				// 找回奖励

	return true;
}

static int _rank = 0;
bool RankCfg(const ActTeamFightRankCfg & cfg)
{
	return _rank >= cfg.min_rank && _rank <= cfg.max_rank;
}

const ActTeamFightRankCfg * ActivityTeamFightConfig::GetRankCfg(int rank, int role_level)const
{
	_rank = rank;

	for (std::map<int, std::vector<ActTeamFightRankCfg> >::const_iterator level_it = m_rank_cfg.begin(); level_it != m_rank_cfg.end(); level_it++)
	{
		if (level_it->first / 1000 <= role_level && level_it->first % 1000 >= role_level)
		{
			std::vector<ActTeamFightRankCfg>::const_iterator it = std::find_if(level_it->second.begin(), level_it->second.end(), RankCfg);
			if (it != level_it->second.end())
			{
				return &(*it);
			}
		}
	}

	return NULL;
}

bool ActivityTeamFightConfig::SendBoxReward(Role * role, int type, int role_level)const
{
	if(NULL == role) return false;

	std::map<int, std::vector<ActTeamFightBoxCfg> >::const_iterator type_it = m_box_cfg.find(type);
	if (type_it == m_box_cfg.end()) return false;

	for (std::vector<ActTeamFightBoxCfg>::const_iterator it = type_it->second.begin(); it != type_it->second.end(); ++it)
	{
		if (role_level >= it->min_level && role_level <= it->max_level)
		{
			const ItemBase *item_base = ITEMPOOL->GetItem(it->item.item_id);
			if (NULL != item_base)
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(it->item.item_id, it->item.num))
				{
					role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
					return false;
				}
				if (!role->GetRoleModuleManager()->GetKnapsack()->Put(it->item, PUT_REASON_TEAM_FIGHT))
				{
					role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
					return false;
				}
			}

			role->AddExp(it->exp, ADD_EXP_TYPE_TEAM_FIGHT, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(it->coin, __FUNCTION__);
			gamelog::g_log_team_fight.printf(LL_INFO, "%s Succ user[%d] item[%d %d] exp[%d]", __FUNCTION__, role->GetUID(), (int)it->item.item_id, it->item.num, it->exp);

			break;
		}
	}

	return true;
}

const ActTeamFightBoxCfg * ActivityTeamFightConfig::GetBoxReward(int type, int role_level) const
{
	std::map<int, std::vector<ActTeamFightBoxCfg> >::const_iterator type_it = m_box_cfg.find(type);
	if (type_it == m_box_cfg.end()) return NULL;

	for (std::vector<ActTeamFightBoxCfg>::const_iterator it = type_it->second.begin(); it != type_it->second.end(); ++it)
	{
		if (role_level >= it->min_level && role_level <= it->max_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

bool ActivityTeamFightConfig::MailReissueBoxReward(int box_type, int role_level, int uid) const
{
	const ActTeamFightBoxCfg * box_cfg = this->GetBoxReward(box_type, role_level);
	if(NULL == box_cfg) return false;

	static MailContentParam contentparam; contentparam.Reset();
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = box_cfg->exp;
	contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = box_cfg->coin;
	const ItemBase * item_cfg = ITEMPOOL->GetItem(box_cfg->item.item_id);
	if (NULL != item_cfg)
	{
		contentparam.item_list[0] = ItemDataWrapper(box_cfg->item.item_id, box_cfg->item.num, box_cfg->item.is_bind);
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_team_fight_reissue_box_reward_subject);
	int length = 0;
	if (ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN == box_type)
	{
		length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_team_fight_reissue_first_box_reward_content);
	}
	else
	{
		length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_team_fight_reissue_three_box_reward_content);
	}
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(IntToUid(uid), MAIL_REASON_DEFAULT, contentparam);
		return true;
	}

	return false;
}

const ActTeamFightRewardGroupCfg* ActivityTeamFightConfig::GetRewardGroupCfg(int group_id, int role_level) const
{
	if (group_id < 0 || group_id >= ACT_TEAM_FIGHT_REWARD_GROUP_NUM) return NULL;

	const RewardGroupVec& vec = m_reward_group_vec_list[group_id];
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (role_level >= vec[i].min_level && role_level <= vec[i].max_level)
		{
			return &vec[i];
		}
	}

	return NULL;
}

int ActivityTeamFightConfig::GetTeamMateLevelDiffByWaitSeconds(int wait_sec)
{
	for (int i = 0; i < m_match_teammate_rule_num; ++i)
	{
		if (wait_sec >= m_match_teammate_rule_cfg_list[i].match_sec_min
			&& wait_sec <= m_match_teammate_rule_cfg_list[i].match_sec_max)
		{
			return m_match_teammate_rule_cfg_list[i].average_level_diff;
		}
	}

	return 10; // 找不到配置，则返回一个合理的估算值
}

int ActivityTeamFightConfig::GetOpponentLevelDiffByWaitSeconds(int wait_sec)
{
	for (int i = 0; i < m_match_enemy_rule_num; ++i)
	{
		if (wait_sec >= m_match_enemy_rule_cfg_list[i].match_sec_min
			&& wait_sec <= m_match_enemy_rule_cfg_list[i].match_sec_max)
		{
			return m_match_enemy_rule_cfg_list[i].average_level_diff;
		}
	}

	return 10; // 找不到配置，则返回一个合理的估算值
}

const TeamFightJoinAwardCfg * ActivityTeamFightConfig::GetTeamFightJoinAwardCfg(int role_level)
{
	for (int i = 0; i < (int)m_join_award_vec.size(); i++)
	{
		if (m_join_award_vec[i].min_level <= role_level && m_join_award_vec[i].max_level >= role_level)
		{
			return &m_join_award_vec[i];
		}
	}

	return NULL;
}

const TeamFightFindCfg * ActivityTeamFightConfig::GetFindCfg(int find_type, int role_level) const
{
	if (find_type < 0 || find_type >= RB_TYPE_MAX) return NULL;

	const TeamFightFindGroupCfg & node_cfg = m_find_cfg[find_type];
	for (int i = 0; i < (int)node_cfg.node_list.size(); i++)
	{
		if (node_cfg.node_list[i].min_level <= role_level && node_cfg.node_list[i].max_level >= role_level)
		{
			return &node_cfg.node_list[i];
		}
	}
	return NULL;
}

const TeamFightFindRewardGroupCfg * ActivityTeamFightConfig::GetFindRewardGroupCfg(int group_id) const
{
	std::map<int, std::vector<TeamFightFindRewardGroupCfg> >::const_iterator it = m_find_reward_group_map.find(group_id);
	if (it == m_find_reward_group_map.end()) return NULL;

	int rate = rand() & RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (int i = 0; i < (int)it->second.size();i++)
	{
		if (rate < it->second[i].rate)
		{
			return &it->second[i];
		}
		rate -= it->second[i].rate;
	}

	return NULL;
}

int ActivityTeamFightConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		if (!GetSubNodeValue(dataElement, "scene_id", m_other_cfg.scene_id))return -1;
		if (!GetSubNodeValue(dataElement, "open_level", m_other_cfg.level))return -2;
		if (!GetSubNodeValue(dataElement, "x", m_other_cfg.pos.x))return -3;
		if (!GetSubNodeValue(dataElement, "y", m_other_cfg.pos.y))return -4;
		if (!GetSubNodeValue(dataElement, "battle_countdown", m_other_cfg.start_battle_countdown)) return -5;
		if (!GetSubNodeValue(dataElement, "max_round", m_other_cfg.limit_max_round)) return -6;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityTeamFightConfig::InitRankCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_max_rank = 0;
	int last_rank = 0;
	while (NULL != dataElement)
	{
		ActTeamFightRankCfg node_cfg;
		int min_level = 0, max_level = 0;
		if (!GetSubNodeValue(dataElement, "level_min", min_level) || min_level <= 0 || min_level > MAX_ROLE_LEVEL)
		{
			return -200;
		}

		if (!GetSubNodeValue(dataElement, "level_max", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)
		{
			return -201;
		}
		int level_key = min_level * 1000 + max_level;
		UNSTD_STATIC_CHECK(MAX_ROLE_LEVEL <= 1000);

		if (!GetSubNodeValue(dataElement, "min_rank", node_cfg.min_rank) || node_cfg.min_rank <= 0)return -1;
		if (!GetSubNodeValue(dataElement, "max_rank", node_cfg.max_rank) || node_cfg.max_rank <= node_cfg.min_rank)return -2;
		if (m_rank_cfg.find(level_key) == m_rank_cfg.end())
		{
			last_rank = 0;
		}
		if (last_rank + 1 != node_cfg.min_rank) return -100;
		last_rank = node_cfg.max_rank;
		m_max_rank = node_cfg.max_rank > m_max_rank ? node_cfg.max_rank : m_max_rank;

		if (!GetSubNodeValue(dataElement, "exp", node_cfg.exp))return -3;
		if (!GetSubNodeValue(dataElement, "bind_coin", node_cfg.bind_coin))return -4;


		TiXmlElement * item_list_Element = dataElement->FirstChildElement("reward_list");
		if (NULL == item_list_Element) return -15;

		TiXmlElement * item_Element = item_list_Element->FirstChildElement("reward");
		if (NULL == item_Element) return -16;

		while (NULL != item_Element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_Element))
			{
				return -18;
			}
			node_cfg.reward_list.push_back(item);
			item_Element = item_Element->NextSiblingElement();
		}

		if (!GetSubNodeValue(dataElement, "title", node_cfg.title_id)) return -19;

		m_rank_cfg[level_key].push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityTeamFightConfig::InitBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int type;
		if (!GetSubNodeValue(dataElement, "type", type) || (type != ACT_TEAM_FIGHT_BOX_TYPE_FIRST_WIN && type != ACT_TEAM_FIGHT_BOX_TYPE_THIRD_BATTLE))
		{
			return -1;
		}
			
		ActTeamFightBoxCfg node_cfg;

		TiXmlElement * itemElement = dataElement->FirstChildElement("reward");
		ItemID item_id = 0;
		if (itemElement != NULL && GetSubNodeValue(itemElement, "item_id", item_id) && item_id != 0)
		{
			if (!node_cfg.item.ReadConfig(itemElement))
			{
				return -3;
			}
		}

		if (!GetSubNodeValue(dataElement, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "max_level", node_cfg.max_level) || node_cfg.max_level < 0 || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -7;
		}

		std::string temp_txt;
		if (!GetSubNodeValue(dataElement, "box_name", temp_txt) || temp_txt.size() <= 0)
		{
			return -8;
		}
		if (temp_txt.length() >= sizeof(node_cfg.mail_txt))
		{
			return -9;
		}
		strncpy(node_cfg.mail_txt, temp_txt.c_str(), sizeof(node_cfg.mail_txt));

		m_box_cfg[type].push_back(node_cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityTeamFightConfig::InitRewardGroup(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "group_id", group_id) || group_id < 0 || group_id >= ACT_TEAM_FIGHT_REWARD_GROUP_NUM)
		{
			return -1;
		}
		RewardGroupVec& vec = m_reward_group_vec_list[group_id];
		
		ActTeamFightRewardGroupCfg cfg;
		cfg.group_id = group_id;

		if (!GetSubNodeValue(dataElement, "min_level", cfg.min_level) || cfg.min_level < 0 || cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "max_level", cfg.max_level) || cfg.max_level < 0 || cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (cfg.max_level < cfg.min_level)
		{
			return -33;
		}

		TiXmlElement * item_Element = dataElement->FirstChildElement("reward");
		if (NULL == item_Element)
		{
			return -4;
		}

		if (!cfg.item.ReadConfig(item_Element))
		{
			return -5;
		}

		vec.push_back(cfg);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int ActivityTeamFightConfig::InitTeamMateMatchRule(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_match_teammate_rule_num = 0;
	int last_time_max = -1;
	while (NULL != dataElement)
	{
		if (m_match_teammate_rule_num >= ARRAY_ITEM_COUNT(m_match_teammate_rule_cfg_list))
		{
			return -999;
		}

		int match_time_min = 0;
		if (!GetSubNodeValue(dataElement, "match_time_min", match_time_min)
			|| match_time_min < 0
			|| last_time_max + 1 != match_time_min)
		{
			return -1;
		}

		int match_time_max = 0;
		if (!GetSubNodeValue(dataElement, "match_time_max", match_time_max)
			|| match_time_max <= match_time_min)
		{
			return -2;
		}
		last_time_max = match_time_max;

		int role_level = 0;
		if (!GetSubNodeValue(dataElement, "role_level", role_level)
			|| role_level < 0 || role_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		m_match_teammate_rule_cfg_list[m_match_teammate_rule_num].match_sec_min = match_time_min;
		m_match_teammate_rule_cfg_list[m_match_teammate_rule_num].match_sec_max = match_time_max;
		m_match_teammate_rule_cfg_list[m_match_teammate_rule_num].average_level_diff = role_level;
		m_match_teammate_rule_num += 1;

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int ActivityTeamFightConfig::InitEnemyMatchRule(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	m_match_enemy_rule_num = 0;
	int last_time_max = -1;
	while (NULL != dataElement)
	{
		if (m_match_enemy_rule_num >= ARRAY_ITEM_COUNT(m_match_enemy_rule_cfg_list))
		{
			return -999;
		}

		int match_time_min = 0;
		if (!GetSubNodeValue(dataElement, "match_time_min", match_time_min)
			|| match_time_min < 0
			|| last_time_max + 1 != match_time_min)
		{
			return -1;
		}

		int match_time_max = 0;
		if (!GetSubNodeValue(dataElement, "match_time_max", match_time_max)
			|| match_time_max <= match_time_min)
		{
			return -2;
		}
		last_time_max = match_time_max;

		int role_level = 0;
		if (!GetSubNodeValue(dataElement, "team_level", role_level)
			|| role_level < 0 || role_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		m_match_enemy_rule_cfg_list[m_match_enemy_rule_num].match_sec_min = match_time_min;
		m_match_enemy_rule_cfg_list[m_match_enemy_rule_num].match_sec_max = match_time_max;
		m_match_enemy_rule_cfg_list[m_match_enemy_rule_num].average_level_diff = role_level;
		m_match_enemy_rule_num += 1;

		dataElement = dataElement->NextSiblingElement();
	}
	return 0;
}

int ActivityTeamFightConfig::InitTeamFightJoinAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		TeamFightJoinAwardCfg node_cfg;
		if (!GetSubNodeValue(root_element, "level_min", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "level_max", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -4;
		}

		TiXmlElement * award_list = root_element->FirstChildElement("reward_list");
		if (NULL != award_list)
		{
			TiXmlElement * award_node = award_list->FirstChildElement("reward");
			if (NULL == award_node) return -100;

			while (NULL != award_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(award_node))
				{
					return -200;
				}
				node_cfg.reward_list.push_back(item);
				award_node = award_node->NextSiblingElement();
			}
		}
		m_join_award_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int ActivityTeamFightConfig::InitTeamFightFindAwardGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id < 0)
		{
			return -1;
		}
		TeamFightFindRewardGroupCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate < 0)
		{
			return -2;
		}
		TiXmlElement * reward_node = root_element->FirstChildElement("reward");
		if (NULL == reward_node) return -222;

		if (!node_cfg.reward.ReadConfig(reward_node))
		{
			return -3;
		}

		m_find_reward_group_map[group_id].push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int ActivityTeamFightConfig::InitTeamFightFindAwardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int find_type = 0;
		if (!GetSubNodeValue(root_element, "find_type", find_type) || find_type < 0 || find_type >= RB_TYPE_MAX)
		{
			return -1;
		}
		TeamFightFindGroupCfg & group_cfg = m_find_cfg[find_type];
		TeamFightFindCfg node_cfg;
		if (!GetSubNodeValue(root_element, "min_level", node_cfg.min_level) || node_cfg.min_level < 0 || node_cfg.min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "max_level", node_cfg.max_level) || node_cfg.max_level < node_cfg.min_level || node_cfg.max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "exp", node_cfg.exp) || node_cfg.exp < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "coin", node_cfg.coin) || node_cfg.coin < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "family_gongxian", node_cfg.family_gongxian) || node_cfg.family_gongxian < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "group_id", node_cfg.group_id) || (node_cfg.group_id != -1 && m_find_reward_group_map.end() == m_find_reward_group_map.find(node_cfg.group_id)))
		{
			return -6;
		}

		group_cfg.node_list.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
