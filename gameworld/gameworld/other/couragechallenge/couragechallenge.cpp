#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "item/money.h"

#include "global/couragechallengerank/couragechallengerank.hpp"

#include "other/event/eventhandler.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/couragechallenge/couragechallengeconfig.h"
#include "protocol/msgcouragechallenge.h"
#include "couragechallenge.h"
#include "gameworld/gamelog.h"
#include "servercommon/logdef.h"
#include "servercommon/errornum.h"


CourageChallenge::CourageChallenge() :m_role_module_mgr(NULL)
{
	for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
	{
		score_data[i].Reset();
	}
	m_level_data_map.clear();
	m_dirty_group_map.clear();
	m_snap_dirty_group_map.clear();
}

CourageChallenge::~CourageChallenge()
{
}

bool SortData(const LayerLevelNode & a, const LayerLevelNode & b)
{
	if (-1 == a.layer_level) return false;
	if (-1 == b.layer_level) return true;

	if (a.layer_level != b.layer_level) return a.layer_level < b.layer_level;
	
	return a.level_seq < b.level_seq; 
}

/*bool SortScoreData(const ScoreLayerLevelNode & a, const ScoreLayerLevelNode & b)
{

	
}*/

void CourageChallenge::Init(RoleModuleManager * mgr, const CourageChallengeParam & param, const RoleCourageChallengeLevelParamList & param_list)
{
	m_role_module_mgr = mgr;
	m_param = param;

	if (0 == m_param.new_get_timestamp)
	{
		if (0 != m_param.old_get_timestamp)
		{
			m_param.new_get_timestamp = m_param.old_get_timestamp;
			m_param.old_get_timestamp = 0;
		}
		else
		{
			m_param.new_get_timestamp = static_cast<long long>(EngineAdapter::Instance().Time());
		}
	}

	//该标识不可再改变,如果改变需要写适应代码,否则会造成数据库此时会有多条相同关卡数据
	if (1 != m_param.is_change_data_storage_flag)
	{
		for (int i = 0; i < m_param.count && i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
		{
			CourageChallengeKey level_key(m_param.data_node[i].layer_level, m_param.data_node[i].level_seq);
			if (this->IsInvalid(level_key)) continue;

			m_level_data_map[level_key] = m_param.data_node[i].flag;
			m_snap_dirty_group_map[level_key] = structcommon::CS_INSERT;
			//不存在的配置不计算评分
			if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(level_key.layer_level, level_key.level_seq) || m_param.data_node[i].flag <= 0)
			{
				continue;
			}

			for (int index = 0; index < COURAGE_CHALLENGE_CONDITION_MAX_NUM; index++)
			{
				if (m_param.data_node[i].flag & (1 << index))
				{
					score_data[level_key.layer_level - 1].star++;
				}
			}
		}
		for (int i = 0; i < m_param.score_layer_count && i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			ScoreLayerLevelNode & score_data_node = this->GetScoreNode(i);
			if (this->IsScoreInvalid(score_data_node.layer_level)) continue;

			m_param.score_flag[score_data_node.layer_level - 1] = score_data_node.flag;
		}
		m_param.DataReset();
		m_param.is_change_data_storage_flag = 1;
	}

	for (int i = 0; i < param_list.count && i < MAX_COURAGE_CHALLENGE_LEVEL_NUM; i++)
	{
		CourageChallengeKey level_key(param_list.data_list[i].info.layer_level, param_list.data_list[i].info.level_seq);
		if (this->IsInvalid(level_key)) continue;

		m_level_data_map[level_key] = param_list.data_list[i].info.flag;
		//不存在的配置不计算评分
		if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(level_key.layer_level, level_key.level_seq) || param_list.data_list[i].info.flag <= 0)
		{
			continue;
		}
		
		for (int index = 0; index < COURAGE_CHALLENGE_CONDITION_MAX_NUM; index++)
		{
			if (param_list.data_list[i].info.flag & (1 << index))
			{
				score_data[level_key.layer_level - 1].star++;
			}
		}
	}
	for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
	{
		score_data[i].layer_level = i + 1;
		score_data[i].flag = m_param.score_flag[i];
	}

	/*if (m_param.change_reset_flag != 6)
	{
		m_param.Reset();
		m_param.change_reset_flag = 6;
		m_param.is_set_new_layer_level_data = 1;
		m_param.is_set_old_data_version_flag = 1;
		m_param.new_get_timestamp = static_cast<long long>(EngineAdapter::Instance().Time());
		return;
	}

	std::set<int> delete_layer_set;
	for (int i = 0; i < m_param.score_layer_count && i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
	{
		ScoreLayerLevelNode & score_data_node = this->GetScoreNode(i);
		if (score_data_node.layer_level == -1) continue;

		const CourageChallengeMaxScoreRewardCfg * max_score_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetMaxScoreRewardCfg(score_data_node.layer_level);
		if (NULL == max_score_cfg || (1 == m_param.is_set_old_data_version_flag && m_param.layer_data_node_version[i] != max_score_cfg->version))
		{
			delete_layer_set.insert(score_data_node.layer_level);
			score_data_node.Reset();
		}
		else
		{
			m_param.layer_data_node_version[i] = max_score_cfg->version;
			m_layer_level_star_map[score_data_node.layer_level] = i;
		}
	}

	m_param.is_set_old_data_version_flag = 1;
	//检测到有版号修改或删除的分层等级
	if (!delete_layer_set.empty())
	{	
		//没有数据全部重置
		if (m_layer_level_star_map.empty())
		{
			m_param.DataReset();
		}
		else
		{
			//重置一些相关数据
			int delete_data_num = 0;
			for (int i = 0; i < m_param.count && i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
			{
				if (m_param.data_node[i].layer_level == -1 || m_param.data_node[i].level_seq == -1 ||
					delete_layer_set.find(m_param.data_node[i].layer_level) == delete_layer_set.end()
					) continue;

				m_param.data_node[i].Reset();
				delete_data_num++;
			}
			if (delete_data_num > 0)
			{
				std::sort(m_param.data_node, m_param.data_node + m_param.count, SortData);
				m_param.count -= delete_data_num;
			}
			m_layer_level_star_map.clear();
			memset(m_param.layer_data_node_version, 0, sizeof(m_param.layer_data_node_version));
			int index = 0;
			//排序并校准版本号
			for (int i = 0; i < m_param.score_layer_count && i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
			{
				ScoreLayerLevelNode & score_data_node = this->GetScoreNode(i);
				if (score_data_node.layer_level != -1)
				{
					ScoreLayerLevelNode & score_data = this->GetScoreNode(index);
					score_data = score_data_node;
					if (index != i)
					{
						score_data_node.Reset();
					}
					const CourageChallengeMaxScoreRewardCfg * max_score_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetMaxScoreRewardCfg(score_data.layer_level);
					if (NULL != max_score_cfg)
					{
						m_param.layer_data_node_version[index] = max_score_cfg->version;
					}
					index++;
				}
			}
			m_param.score_layer_count -= (short)delete_layer_set.size();

			//排序
			ScoreLayerLevelNode score_layer_level_node[COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM];
			UNSTD_STATIC_CHECK(sizeof(score_layer_level_node) >= sizeof(m_param.score_data_node) + sizeof(m_param.score_1_data_node));
			memcpy(score_layer_level_node, m_param.score_data_node, sizeof(m_param.score_data_node));
			memcpy(score_layer_level_node + ARRAY_LENGTH(m_param.score_data_node), m_param.score_1_data_node, sizeof(m_param.score_1_data_node));
			std::sort(score_layer_level_node, score_layer_level_node + ARRAY_LENGTH(score_layer_level_node), SortScoreData);

			memcpy(m_param.score_data_node, score_layer_level_node, sizeof(m_param.score_data_node));
			memcpy(m_param.score_1_data_node, score_layer_level_node + ARRAY_LENGTH(m_param.score_data_node), sizeof(m_param.score_1_data_node));
		}
		m_param.is_add_rank = 0;
	}

	for (int i= 0; i < m_param.count && i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
	{
		if(m_param.data_node[i].layer_level == -1 || m_param.data_node[i].level_seq == -1) continue;

		CourageChallengeKey data_node(m_param.data_node[i].layer_level, m_param.data_node[i].level_seq);
		m_data_node_map[data_node] = i;
	}
	if (m_layer_level_star_map.empty())
	{
		for (int i = 0; i < m_param.score_layer_count && i < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM; i++)
		{
			ScoreLayerLevelNode & score_data_node = this->GetScoreNode(i);
			if (score_data_node.layer_level == -1) continue;

			m_layer_level_star_map[score_data_node.layer_level] = i;
		}
	}
	if (0 == m_param.new_get_timestamp)
	{
		if (0 != m_param.old_get_timestamp)
		{
			m_param.new_get_timestamp = m_param.old_get_timestamp;
			m_param.old_get_timestamp = 0;
		}
		else
		{
			m_param.new_get_timestamp = static_cast<long long>(EngineAdapter::Instance().Time());
		}
	}
	if (1 != m_param.is_set_new_layer_level_data)
	{
		if (m_param.score_layer_count >= 0)
		{
			std::set<int> layer_set;
			for (int i = 0; i < m_param.count && i < COURAGE_CHALLENGE_SEQ_MAX_NUM; i++)
			{	
				if (m_param.data_node[i].layer_level == -1 || m_param.data_node[i].level_seq == -1) continue;

				LayerLevelNode & data_item = m_param.data_node[i];
				std::map<int, short>::iterator it = m_layer_level_star_map.find(data_item.layer_level);
				if (it == m_layer_level_star_map.end())		//遗漏数据
				{	
					if(m_param.score_layer_count >= COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM + COURAGE_CHALLENGE_LAYER_LEVEL_NEW_ADD_MAX_NUM) continue;

					ScoreLayerLevelNode node;
					node.layer_level = data_item.layer_level;
					for (int k = 0; k < COURAGE_CHALLENGE_CONDITION_MAX_NUM; k++)
					{
						if (data_item.flag & (1 << k))
						{
							++node.star;
						}
					}
					m_layer_level_star_map[node.layer_level] = m_param.score_layer_count;
					ScoreLayerLevelNode & layer_node = this->GetScoreNode(m_param.score_layer_count);
					layer_node = node;
					++m_param.score_layer_count;
					layer_set.insert(data_item.layer_level);
				}
				else
				{
					std::set<int>::iterator layer_it = layer_set.find(data_item.layer_level);
					if (layer_it != layer_set.end())
					{
						int add_star = 0;
						for (int k = 0; k < COURAGE_CHALLENGE_CONDITION_MAX_NUM; k++)
						{
							if (data_item.flag & (1 << k))
							{
								++add_star;
							}
						}
						ScoreLayerLevelNode & layer_node = this->GetScoreNode(it->second);
						layer_node.star += add_star;
					}
				}
			}
		}
		m_param.is_set_new_layer_level_data = 1;
	}*/
}

void CourageChallenge::GetInitParam(CourageChallengeParam * param, RoleCourageChallengeLevelParamList * list)
{
	*param = m_param;

	m_dirty_group_map.insert(m_snap_dirty_group_map.begin(), m_snap_dirty_group_map.end());
	DirtyGroupMap::iterator it_begin = m_snap_dirty_group_map.begin();
	DirtyGroupMap::iterator it_end = m_snap_dirty_group_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		LevelInfoMap::iterator level_it = m_level_data_map.find(it_begin->first);
		if (level_it != m_level_data_map.end())
		{
			LayerLevelNode info;
			info.layer_level = level_it->first.layer_level;
			info.level_seq = level_it->first.level_seq;
			info.flag = level_it->second;
			list->data_list[list->count].info = info;
			list->data_list[list->count++].change_state = it_begin->second;
		}

		it_begin->second = structcommon::CS_NONE;
	}
}

void CourageChallenge::ClearDirtyMark()
{
	DirtyGroupMap::iterator it_begin = m_dirty_group_map.begin();
	DirtyGroupMap::iterator it_end = m_dirty_group_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		if (structcommon::CS_NONE == m_snap_dirty_group_map[it_begin->first])
		{
			m_snap_dirty_group_map.erase(it_begin->first);
		}
	}

	m_dirty_group_map.clear();
}

void CourageChallenge::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
}

void CourageChallenge::OnRoleLogin()
{
	//上线重新计算星数,所以需要更新排行榜
	//if (m_param.is_add_rank != 4)
	{
		this->UpdateRoleRank(true);
	//	m_param.is_add_rank = 4;
	}
}

void CourageChallenge::SendPassReward(int monster_group_id, unsigned char flag)
{
	const CourageChallengeLevelCfg * level_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfgByMonsterGroupId(monster_group_id);
	if (level_cfg == NULL || this->IsInvalid(level_cfg->layer_level, level_cfg->level_seq))
	{
		return;
	}
	bool is_first_kill = true;
	CourageChallengeKey level_key(level_cfg->layer_level, level_cfg->level_seq);
	LevelInfoMap::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end())
	{
		m_level_data_map[level_key] = 0;
	}
	else
	{
		if (it->second & (1 << COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM))
		{
			is_first_kill = false;
		}
	}
	if (is_first_kill)
	{
		m_role_module_mgr->GetMoney()->AddCoinBind(level_cfg->coin, __FUNCTION__);
		std::vector<ItemConfigData> reward_list;
		if (!level_cfg->pass_reward.empty())
		{
			reward_list.insert(reward_list.end(), level_cfg->pass_reward.begin(), level_cfg->pass_reward.end());
		}
		if (m_role_module_mgr->GetRole()->GetLevel() <= level_cfg->level + LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().extra_reward_level)
		{
			if (!level_cfg->extra_reward.empty())
			{
				reward_list.insert(reward_list.end(), level_cfg->extra_reward.begin(), level_cfg->extra_reward.end());
			}
			m_level_data_map[level_key] |= (1 << COURAGE_CHALLENGE_EXTER_REWARD_BIT_NUM);
		}

		if (!reward_list.empty())
		{
			if (!m_role_module_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_SYSTEM_COURAGE_CHALLENGE))
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_COURAGE_CHALLENGE, true);
			}
		}
		m_level_data_map[level_key] |= (1 << COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM);
		m_snap_dirty_group_map[level_key] = structcommon::CS_INSERT;
	}
	ROLE_LOG_QUICK6(LOG_TYPE_COURAGECHALLENGE, m_role_module_mgr->GetRole(), level_key.layer_level, level_key.level_seq, __FUNCTION__, NULL);
	this->SetLayerLevelCondition(flag, level_key);
	this->SendSingleInfo(level_key);
	m_role_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_COURAGE_CHALLENGE);
}

void CourageChallenge::SetLayerLevelCondition(unsigned char flag, CourageChallengeKey level_key)
{
	if(flag <= 0 || this->IsInvalid(level_key)) return;

	int add_star = 0;
	for (int i = 0; i < COURAGE_CHALLENGE_CONDITION_MAX_NUM; i++)
	{
		if ((m_level_data_map[level_key] & (1 << i)) || (!(flag & (1 << i))))
		{
			continue;
		}

		++add_star;
		m_level_data_map[level_key] |= (1 << i);
	}

	if (add_star <= 0)
	{
		return;
	}

	score_data[level_key.layer_level - 1].star += add_star;
	m_snap_dirty_group_map[level_key] = m_snap_dirty_group_map[level_key] == structcommon::CS_INSERT ? structcommon::CS_INSERT : structcommon::CS_UPDATE;
	m_param.new_get_timestamp = static_cast<long long>(EngineAdapter::Instance().Time());
	this->UpdateRoleRank(true);
	EventHandler::Instance().OnPassFBCourageChallenge(m_role_module_mgr->GetRole());
	
	this->CalculatePetHelperNum();
	return;
}

void CourageChallenge::OnRoleOpenUI()
{
	this->CalculatePetHelperNum();
}

void CourageChallenge::FetchEveryDayReward(int layer_level, int layer_Level_seq)
{
	const CourageChallengeScoreRewardCfg * score_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetScoreRewardCfg(layer_level, layer_Level_seq);
	if (score_cfg == NULL || this->IsScoreInvalid(layer_level))
	{
		return;
	}
	
	int seq = layer_level - 1;
	ScoreLayerLevelNode & node = score_data[seq];
	if (node.flag.IsBitSet(layer_Level_seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;	
	}

	if (score_cfg->star > node.star)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COURAGE_CHALLENGE_STAR_ENOUGH);
		return;
	}

	if (!score_cfg->score_reward.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)score_cfg->score_reward.size(), &score_cfg->score_reward[0], PUT_REASON_SYSTEM_COURAGE_CHALLENGE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
	node.flag.SetBit(layer_Level_seq);
	m_param.score_flag[seq].SetBit(layer_Level_seq);
	this->SendScoreSingleInfo(layer_level);
}

void CourageChallenge::FetchMaxScoreReward(int layer_level)
{
	const CourageChallengeMaxScoreRewardCfg * max_score_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetMaxScoreRewardCfg(layer_level);
	if (NULL == max_score_cfg || this->IsScoreInvalid(layer_level))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	int seq = layer_level - 1;
	ScoreLayerLevelNode & node = score_data[seq];
	if (node.flag.IsBitSet(COURAGE_CHALLENGE_MAX_SCORE_FLAG_BIT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	if (node.star < max_score_cfg->max_star)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_COURAGE_CHALLENGE_STAR_ENOUGH);
		return;
	}
	if (!max_score_cfg->max_score_reward.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)max_score_cfg->max_score_reward.size(), &max_score_cfg->max_score_reward[0], PUT_REASON_SYSTEM_COURAGE_CHALLENGE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}
		
	node.flag.SetBit(COURAGE_CHALLENGE_MAX_SCORE_FLAG_BIT);
	m_param.score_flag[seq].SetBit(COURAGE_CHALLENGE_MAX_SCORE_FLAG_BIT);
	this->SendScoreSingleInfo(layer_level);
}

int CourageChallenge::GetLayerScoreNum(int layer_level)
{
	if(this->IsScoreInvalid(layer_level)) return 0;
	
	return score_data[layer_level - 1].star;
}

int CourageChallenge::GetReachStarScoreCount()
{
	int star = 0;
	for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
	{
		if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfgByLayerLevel(i + 1)) continue;

		star += score_data[i].star;
	}

	return star;
}

int CourageChallenge::GetPassMaxLayerLevel()
{
	//默认最低分层
	int layer_level = LOGIC_CONFIG->GetCourageChallengeConfig()->GetMinLayerLevel();
	for (int i = 0; i < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
	{
		if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfgByLayerLevel(i + 1) || score_data[i].star <= 0) continue;

		layer_level = layer_level >= score_data[i].layer_level ? layer_level : score_data[i].layer_level;
	}

	return layer_level;
}

int CourageChallenge::GetPassMaxSeq()
{
	int max_clear_layer_level = 0;
	int max_clear_level_seq = 0;

	LevelInfoMap::iterator it = m_level_data_map.begin();
	for (; it != m_level_data_map.end(); ++it)
	{
		if (max_clear_layer_level < it->first.layer_level)
		{
			max_clear_layer_level = it->first.layer_level;
			max_clear_level_seq = it->first.level_seq;
		}
		else if (max_clear_layer_level == it->first.layer_level)
		{
			max_clear_level_seq = it->first.level_seq;
		}
	}

	const CourageChallengeLevelCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(max_clear_layer_level, max_clear_level_seq);
	if (NULL == cfg)
	{
		return 0;
	}

	return cfg->seq;
}

bool CourageChallenge::IsFinishCurLayer(int layer_level)
{
	const std::vector<CourageChallengeLevelCfg> * layer_level_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfgByLayerLevel(layer_level);
	if (NULL == layer_level_cfg)
	{
		return false;
	}
	for (int i = 0; i < (int)layer_level_cfg->size(); i++)
	{
		CourageChallengeKey level_key((*layer_level_cfg)[i].layer_level, (*layer_level_cfg)[i].level_seq);
		LevelInfoMap::iterator it = m_level_data_map.find(level_key);
		if (it == m_level_data_map.end() || !(it->second & (1 << COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM)))
		{
			return false;
		}
	}

	return true;
}

void CourageChallenge::SendAllInfo()
{
	Protocol::SCCourageChallengeAllInfo info;
	info.count = 0;
	for (LevelInfoMap::iterator it = m_level_data_map.begin(); it != m_level_data_map.end() && info.count < MAX_COURAGE_CHALLENGE_LEVEL_NUM; it++)
	{
		if(NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(it->first.layer_level, it->first.level_seq)) continue;

		LayerLevelNode level_node;
		level_node.layer_level = it->first.layer_level;
		level_node.level_seq = it->first.level_seq;
		level_node.flag = it->second;
		info.layer_data_node[info.count++] = level_node;
	}

	info.score_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(info.score_level_node) && i < MAX_COURAGE_CHALLENGE_SCORE_NUM && info.score_count < MAX_COURAGE_CHALLENGE_SCORE_NUM; i++)
	{
		if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfgByLayerLevel(i + 1) || score_data[i].star <= 0) continue;

		info.score_level_node[info.score_count++] = score_data[i];
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

//layer_level_index由外部判断确保不会出错
void CourageChallenge::SendSingleInfo(CourageChallengeKey level_key)
{
	Protocol::SCCourageChallenegeSingleInfo info;
	info.layer_level = level_key.layer_level;
	info.layer_level_seq = level_key.level_seq;
	info.flag = m_level_data_map[level_key];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

//score_index由外部判断确保不会出错
void CourageChallenge::SendScoreSingleInfo(int layer_level)
{
	Protocol::SCCourageChallengeScoreSingleInfo info;
	ScoreLayerLevelNode & node = score_data[layer_level - 1];
	info.layer_level = node.layer_level;
	info.score_flag = node.flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

bool CourageChallenge::IsInvalid(CourageChallengeKey level_key)
{
	return level_key.layer_level <= 0 || level_key.layer_level > MAX_COURAGE_CHALLENGE_SCORE_NUM || level_key.level_seq < 0 || level_key.level_seq >= MAX_COURAGE_CHALLENGE_LEVEL_SEQ;
}

bool CourageChallenge::IsInvalid(int layer_level, int level_seq)
{
	return layer_level <= 0 || layer_level > MAX_COURAGE_CHALLENGE_SCORE_NUM || level_seq < 0 || level_seq >= MAX_COURAGE_CHALLENGE_LEVEL_SEQ;
}

bool CourageChallenge::IsScoreInvalid(int layer_level)
{
	return layer_level <= 0 || layer_level > MAX_COURAGE_CHALLENGE_SCORE_NUM;
}

ScoreLayerLevelNode & CourageChallenge::GetScoreNode(int score_index)
{
	if (score_index < COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM)
	{
		return m_param.score_data_node[score_index];
	}

	return  m_param.score_1_data_node[score_index - COURAGE_CHALLENGE_LAYER_LEVEL_MAX_NUM];
}

void CourageChallenge::CalculatePetHelperNum()
{
	int max_clear_layer_level = 0;
	int max_clear_level_seq = 0;

	LevelInfoMap::iterator it = m_level_data_map.begin();
	for (; it != m_level_data_map.end(); ++it)
	{
		if (max_clear_layer_level < it->first.layer_level)
		{
			max_clear_layer_level = it->first.layer_level;
			max_clear_level_seq = it->first.level_seq;
		}
		else if (max_clear_layer_level == it->first.layer_level)
		{
			max_clear_level_seq = it->first.level_seq;
		}
	}

	const CourageChallengeLevelCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetLevelCfg(max_clear_layer_level, max_clear_level_seq);
	if (NULL == cfg)
	{
		return;
	}

	int pet_helper_num = LOGIC_CONFIG->GetCourageChallengeConfig()->GetPetHelperNum(cfg->guanqia_num);
	if (this->UpdatePetHelperNum(pet_helper_num))
	{
		this->SendPetHelperNumUpdateNotice();
	}
}

bool CourageChallenge::UpdatePetHelperNum(int pet_helper_num)
{
	if (pet_helper_num > m_param.pet_helper_num)
	{
		m_param.pet_helper_num = pet_helper_num;
		return true;
	}

	return false;
}

void CourageChallenge::SendPetHelperNumUpdateNotice()
{
	Protocol::SCCourageChallengePetHelperNotice notice;
	notice.pet_helper_num = m_param.pet_helper_num;

	m_role_module_mgr->NetSend(&notice, sizeof(notice));
}

void CourageChallenge::UpdateRoleRank(bool is_change)
{
	static SynCourageChallengeRankInfo role_info;

	role_info.uid = m_role_module_mgr->GetUid();
	F_STRNCPY(role_info.user_name, m_role_module_mgr->GetRoleCross()->GetOriginRoleName(), sizeof(role_info.user_name));
	role_info.avatar_type = m_role_module_mgr->GetRole()->GetAvatarType();
	role_info.headshot_id = m_role_module_mgr->GetRole()->GetHeadshotID();
	role_info.star = this->GetReachStarScoreCount();
	role_info.layer_level = this->GetPassMaxLayerLevel();
	role_info.get_timestamp = m_param.new_get_timestamp;
	role_info.prof = m_role_module_mgr->GetRole()->GetProfession();
	role_info.reserve_sh = 0;

	CourageChallengeRankManager::Instance().RoleInfoChange(&role_info);

	if (is_change)
	{
		EventHandler::Instance().OnRoleCourageChallengeChange(m_role_module_mgr->GetRole(), role_info.star);
	}
}
