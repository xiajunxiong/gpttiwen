#include "newcouragechallenge.hpp"
#include "newcouragechallenge.hpp"
#include "newcouragechallenge.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "item/money.h"

#include "other/event/eventhandler.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/couragechallenge/couragechallengeconfig.h"
#include "protocol/msgnewcouragechallenge.h"
#include "newcouragechallenge.hpp"
#include "gameworld/gamelog.h"
#include "servercommon/logdef.h"
#include "servercommon/errornum.h"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/rank/rankmanager.hpp"

NewCourageChallenge::NewCourageChallenge() : m_mgr(NULL), m_total_star(0), m_max_layer(0)
{
	m_level_data_map.clear();
	m_dirty_map.clear();
	m_temp_dirty_vec.clear();
}

NewCourageChallenge::~NewCourageChallenge()
{
}

void NewCourageChallenge::Init(RoleModuleManager * mgr, const NewCourageChallengeParam & param, const NewCourageChallengeLevelParamList & param_list)
{
	m_mgr = mgr;
	m_param = param;

	for (int i = 0; i < param_list.count && i < MAX_NEW_COURAGE_CHALLENGE_LIST_NUM; i++)
	{
		NewCourageChallengeKey level_key(param_list.data_list[i].info.layer_level, param_list.data_list[i].info.level_seq);
		if (this->IsInvalid(level_key)) continue;
		m_level_data_map[level_key] = param_list.data_list[i].info.flag;
	}
	this->CalcStarCount();
}

void NewCourageChallenge::GetInitParam(NewCourageChallengeParam * param, NewCourageChallengeLevelParamList * list)
{
	*param = m_param;

	NewDirtyGroupMap::iterator it_begin = m_dirty_map.begin();
	NewDirtyGroupMap::iterator it_end = m_dirty_map.end();
	for (; it_begin != it_end; it_begin++)
	{
		NewLevelInfoMap::iterator level_it = m_level_data_map.find(it_begin->first);
		if (level_it != m_level_data_map.end())
		{
			NewLayerLevelNode info;
			info.layer_level = level_it->first.layer_level;
			info.level_seq = level_it->first.level_seq;
			info.flag = level_it->second;
			list->data_list[list->count].info = info;
			list->data_list[list->count++].change_state = it_begin->second;
		}

		it_begin->second = structcommon::CS_NONE;
		m_temp_dirty_vec.push_back(it_begin->first);
	}
}

void NewCourageChallenge::ClearDirtyMark()
{
	std::vector<NewCourageChallengeKey>::const_iterator it_begin = m_temp_dirty_vec.begin();
	std::vector<NewCourageChallengeKey>::const_iterator it_end = m_temp_dirty_vec.end();
	for (; it_begin != it_end; it_begin++)
	{
		if (structcommon::CS_NONE == m_dirty_map[*it_begin])
		{
			m_dirty_map.erase(*it_begin);
		}
	}

	m_temp_dirty_vec.clear();
}

void NewCourageChallenge::SendPassReward(int monster_group_id, unsigned char flag)
{
	const NewCourageChallengeLevelCfg * level_cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewLevelCfgByMonster(monster_group_id);
	if (level_cfg == NULL || this->IsInvalid(level_cfg->layer_level, level_cfg->level_seq))
	{
		return;
	}

	bool is_first_kill = true;
	NewCourageChallengeKey level_key(level_cfg->layer_level, level_cfg->level_seq);
	NewLevelInfoMap::iterator it = m_level_data_map.find(level_key);
	if (it == m_level_data_map.end())
	{
		m_level_data_map[level_key] = 0;
	}
	else
	{
		if (it->second & (1 << NEW_COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM))
		{
			is_first_kill = false;
		}
	}

	if (is_first_kill)
	{
		std::vector<ItemConfigData> reward_list;
		if (!level_cfg->pass_reward.empty())
		{
			reward_list.insert(reward_list.end(), level_cfg->pass_reward.begin(), level_cfg->pass_reward.end());
		}
		if (m_mgr->GetRole()->GetLevel() <= level_cfg->level + LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().extra_reward_level)
		{
			if (!level_cfg->extra_reward.empty())
			{
				reward_list.insert(reward_list.end(), level_cfg->extra_reward.begin(), level_cfg->extra_reward.end());
			}
			m_level_data_map[level_key] |= (1 << NEW_COURAGE_CHALLENGE_EXTER_REWARD_BIT_NUM);
		}

		if (!reward_list.empty())
		{
			if (!m_mgr->GetKnapsack()->PutList((short)reward_list.size(), &reward_list[0], PUT_REASON_NEW_COURAGE_CHALLENGE))
			{
				m_mgr->GetKnapsack()->SendMail(&reward_list[0], (int)reward_list.size(), SEND_MAIL_TYPE_NEW_COURAGE_CHALLENGE, true);
			}
		}
		m_level_data_map[level_key] |= (1 << COURAGE_CHALLENGE_FIRST_PASS_BIT_NUM);
		m_dirty_map[level_key] = structcommon::CS_INSERT;
	}

	this->SetLayerLevelCondition(flag, level_key);
	this->SendLevelSingleInfo(level_key);
}

void NewCourageChallenge::SetLayerLevelCondition(unsigned char flag, NewCourageChallengeKey level_key)
{
	if (flag <= 0 || this->IsInvalid(level_key)) return;

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

	bool max_layer_change = false;
	if (level_key.layer_level > m_max_layer)
	{
		m_max_layer = level_key.layer_level;
		max_layer_change = true;
	}

	if (0 < add_star)
	{
		m_total_star += add_star;
	}

	//更新排行榜
	if (0 < add_star || max_layer_change)
	{
		RankManager::Instance().SyncPersonRankInfo(m_mgr->GetRole(), PERSON_RANK_TYPE_NEW_COURAGE_CHALLENGE);
		gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s user[%d, %s] all_star[%lld] max_layer[%d]", __FUNCTION__, m_mgr->GetUid(), m_mgr->GetRoleName(), m_total_star, m_max_layer);
	}

	m_dirty_map[level_key] = m_dirty_map[level_key] == structcommon::CS_INSERT ? structcommon::CS_INSERT : structcommon::CS_UPDATE;
	EventHandler::Instance().OnPassFBNewCourageChallenge(m_mgr->GetRole());
	return;
}

bool NewCourageChallenge::OnFetchCommomReward(int seq, bool is_notify)
{
	if (0 > seq || seq >= MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM)
	{
		if(is_notify) m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (m_param.common_reward_flag.IsBitSet(seq))
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return false;
	}

	const NewCourageChallengeRewardCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewRewardCfg(seq);
	if (NULL == cfg)
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (cfg->star_num > (int)m_total_star)
	{
		m_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return false;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(cfg->reward))
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	if (!m_mgr->GetKnapsack()->PutVec(cfg->reward, PUT_REASON_NEW_COURAGE_CHALLENGE))
	{
		return false;
	}

	m_param.common_reward_flag.SetBit(seq);

	std::string gamelog_str;
	for (int i = 0; i < (int)cfg->reward.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", cfg->reward[i].item_id, cfg->reward[i].num);
	}

	if (is_notify) this->SendRewardInfo();
	gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s user[%d, %s] common seq[%d] is_notify[%d] get_reward[%s]", __FUNCTION__,
		m_mgr->GetUid(), m_mgr->GetRoleName(), seq, is_notify ? 1 : 0, gamelog_str.c_str());

	return true;
}

bool NewCourageChallenge::OnFetchTokenReward(int seq, bool is_notify)
{
	if (0 > seq || seq >= MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM)
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (1 != m_param.token_is_buy)
	{
		return false;
	}

	if (m_param.token_reward_flag.IsBitSet(seq))
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return false;
	}

	const NewCourageChallengeRewardCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewRewardCfg(seq);
	if (NULL == cfg)
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (cfg->star_num > (int)m_total_star)
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return false;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(cfg->token_reward))
	{
		if (is_notify) m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	if (!m_mgr->GetKnapsack()->PutVec(cfg->token_reward, PUT_REASON_NEW_COURAGE_CHALLENGE))
	{
		return false;
	}

	m_param.token_reward_flag.SetBit(seq);

	std::string gamelog_str;
	for (int i = 0; i < (int)cfg->token_reward.size(); ++i)
	{
		gamelog_str += STRING_SPRINTF("{item[%d, %d]} ", cfg->token_reward[i].item_id, cfg->token_reward[i].num);
	}

	this->SendRewardInfo();
	gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s user[%d, %s] token seq[%d] is_notify[%d] get_reward[%s]", __FUNCTION__,
		m_mgr->GetUid(), m_mgr->GetRoleName(), seq, is_notify ? 1 : 0, gamelog_str.c_str());

	return true;
}

void NewCourageChallenge::OnFetchOneKey()
{
	bool is_has_fetch = false;
	int size = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewRewardCfgSize();
	for (int i = 0; i < size && i < MAX_NEW_COURAGE_CHALLENGE_REWARD_NUM; ++i)
	{
		const NewCourageChallengeRewardCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewRewardCfg(i);
		if (NULL == cfg) continue;

		if (cfg->star_num > (int)m_total_star)
		{
			break;
		}

		if (!m_param.common_reward_flag.IsBitSet(i))
		{
			if (this->OnFetchCommomReward(i, false))
			{
				is_has_fetch = true;
			}
		}

		if (1 == m_param.token_is_buy && !m_param.token_reward_flag.IsBitSet(i))
		{
			if (this->OnFetchTokenReward(i, false))
			{
				is_has_fetch = true;
			}
		}
	}

	if (is_has_fetch) this->SendRewardInfo();
}

void NewCourageChallenge::SendAllInfo()
{
	this->SendLevelListInfo();
	this->SendRewardInfo();
}

void NewCourageChallenge::SendLevelListInfo()
{
	Protocol::SCNewCourageChallengeLevelListInfo info;
	info.count = 0;

	for (NewLevelInfoMap::const_iterator it = m_level_data_map.begin(); it != m_level_data_map.end() && info.count < MAX_NEW_COURAGE_CHALLENGE_LIST_NUM; ++it)
	{
		if (NULL == LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewLevelCfg(it->first.layer_level, it->first.level_seq)) continue;

		NewLayerLevelNode level_node;
		level_node.layer_level = it->first.layer_level;
		level_node.level_seq = it->first.level_seq;
		level_node.flag = it->second;
		info.list[info.count++] = level_node;
	}

	int len = sizeof(info) - sizeof(info.list[0]) * (MAX_NEW_COURAGE_CHALLENGE_LIST_NUM - info.count);
	m_mgr->NetSend(&info, len);
}

void NewCourageChallenge::SendLevelSingleInfo(NewCourageChallengeKey level_key)
{
	Protocol::SCNewCourageChallengeLevelSignleInfo info;
	info.layer_level = level_key.layer_level;
	info.layer_level_seq = level_key.level_seq;
	info.flag = m_level_data_map[level_key];

	m_mgr->NetSend(&info, sizeof(info));
}

void NewCourageChallenge::SendRewardInfo()
{
	Protocol::SCNewCourageChallengeRewardInfo info;
	info.token_is_buy = m_param.token_is_buy;
	memset(info.sh_reserve, 0, sizeof(info.sh_reserve));
	info.common_reward_flag = m_param.common_reward_flag;
	info.token_reward_flag = m_param.token_reward_flag;

	m_mgr->NetSend(&info, sizeof(info));
}

void NewCourageChallenge::OnBuyItemCheck()
{
	if (0 != m_param.token_is_buy)
	{
		return;
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}

	RoleCommonSave * rcs = m_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE);
	if (rcs == NULL)
	{
		return;
	}

	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	if (now < (time_t)buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		m_mgr->NoticeNum(errornum::EN_HAS_ORDER);

		return;
	}

	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), log_str1.c_str());
	}

	buy_item->cfg_vesion = 0;
	buy_item->need_gold = LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().new_token_reward_prize * 10;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	rcs->SetDataChange(0);
	m_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_NEW_COURAGE_CHALLENGE;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = unique_key;

	m_mgr->NetSend(&info, sizeof(info));
	return;
}

int NewCourageChallenge::OnCmdBuyItem(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE, unique_id);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}

	gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s line:%d | role[%d %s] %s", __FUNCTION__, __LINE__, m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
		::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}

	m_param.token_is_buy = 1;

	this->SendRewardInfo();

	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE, unique_id, gamelog::g_log_new_courage_challenge, __FUNCTION__);
	m_mgr->GetRole()->Save();

	return 0;
}

int NewCourageChallenge::OnBuyGiftItem(ARG_OUT int * price)
{
	if (1 == m_param.token_is_buy)
	{
		return BUY_GIFT_ITEM_RET_TYPE_2;
	}

	if (NULL != price)
	{
		*price = LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().new_token_reward_prize * 10;
		return 0;
	}

	m_param.token_is_buy = 1;
	this->SendRewardInfo();

	return 0;
}

int NewCourageChallenge::GetPassMaxSeq()
{
	int max_clear_layer_level = 0;
	int max_clear_level_seq = 0;

	NewLevelInfoMap::iterator it = m_level_data_map.begin();
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

	const NewCourageChallengeLevelCfg* cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewLevelCfg(max_clear_layer_level, max_clear_level_seq);
	if (NULL == cfg)
	{
		return 0;
	}

	return cfg->seq;
}

void NewCourageChallenge::CalcStarCount()
{
	m_total_star = 0;
	for (NewLevelInfoMap::const_iterator it = m_level_data_map.begin(); it != m_level_data_map.end(); ++it)
	{
		if (0 >= it->second) continue;
		//不存在的配置不计算星星
		const NewCourageChallengeLevelCfg * cfg = LOGIC_CONFIG->GetCourageChallengeConfig()->GetNewLevelCfg(it->first.layer_level, it->first.level_seq);
		if (NULL == cfg) continue;

		for (int index = 0; index < COURAGE_CHALLENGE_CONDITION_MAX_NUM; index++)
		{
			if (it->second & (1 << index))
			{
				m_total_star++;
			}
		}

		if (it->first.layer_level > m_max_layer)
		{
			m_max_layer = it->first.layer_level;
		}
	}
	gamelog::g_log_new_courage_challenge.printf(LL_INFO, "%s user[%d, %s] all_star[%lld] max_layer[%d]", __FUNCTION__,
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_total_star, m_max_layer);
}

bool NewCourageChallenge::IsInvalid(NewCourageChallengeKey level_key)
{
	return level_key.layer_level <= 0 || level_key.level_seq < 0;
}

bool NewCourageChallenge::IsInvalid(int layer_level, int level_seq)
{
	return layer_level <= 0 || level_seq < 0;
}
