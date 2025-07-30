#include "minigame.hpp"
#include "servercommon/servercommon.h"
#include "protocol/msgminigame.hpp"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/minigame/minigameconfig.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "obj/character/role.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/minigame/minigame_niulang_config.hpp"

MiniGame::MiniGame() : m_mgr(NULL)
{

}

MiniGame::~MiniGame()
{

}

void MiniGame::Init(RoleModuleManager* mgr, const MiniGameParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void MiniGame::GetInitParam(MiniGameParam* out_param)
{
	if (NULL != out_param) *out_param = m_param;
}

void MiniGame::OnFinishNextStageReq()
{
	int mini_game_id = m_mgr->GetMiniGameId();
	if (mini_game_id == MINI_GAME_NIU_LANG_ZHI_NV)
	{
		if (m_param.clear_stage_num >= LOGIC_CONFIG->GetMiniGameNiuLangConfig()->GetStageNum())
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}
	else
	{
		if (!LOGIC_CONFIG->GetMiniGameConfig()->HasNextStage(mini_game_id, m_param.clear_stage_num))
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		const MiniGameStageRewardCfg* cfg = LOGIC_CONFIG->GetMiniGameConfig()->GetStageRewardCfg(mini_game_id, m_param.clear_stage_num + 1);
		if (NULL == cfg)
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (!m_mgr->GetKnapsack()->PutVec(cfg->reward_list, PUT_REASON_MINI_GAME_STAGE_REWARD))
		{
			if (!m_mgr->GetKnapsack()->SendMail(cfg->reward_list))
			{
				m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}
	}

	m_param.clear_stage_num += 1;

	this->SendInfo();

	gamelog::g_log_other.printf(LL_INFO, "%s  role[%d,%s] clear_stage_num[%d]",
		__FUNCTION__, m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.clear_stage_num);
}

void MiniGame::OnFetchPhaseRewardReq(int phase_seq)
{
	int mini_game_id = m_mgr->GetMiniGameId();
	const MiniGamePhaseRewardCfg* cfg = LOGIC_CONFIG->GetMiniGameConfig()->GetPhaseRewardCfg(mini_game_id, phase_seq);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.clear_stage_num < cfg->clear_stage_num)
	{
		m_mgr->NoticeNum(errornum::EN_MINI_GAME_PHASE_REWARD_CLEAR_STAGE_NUM_LIMIT);
		return;
	}

	if (m_param.phase_reward_flag.IsBitSet(phase_seq))
	{
		m_mgr->NoticeNum(errornum::EN_MINI_GAME_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!m_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_list))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_mgr->GetKnapsack()->PutVec(cfg->reward_list, PUT_REASON_MINI_GAME_PHASE_REWARD))
	{
		m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_param.phase_reward_flag.SetBit(phase_seq);

	this->SendInfo();

	gamelog::g_log_other.printf(LL_INFO, "%s  role[%d,%s] phase_seq[%d]", 
		__FUNCTION__, m_mgr->GetUid(), m_mgr->GetRoleName(), phase_seq);
}

void MiniGame::OnStartMiniGameBattleReq(int seq, bool has_pet, bool has_partner, bool has_skill)
{
	Team* team = m_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	if (!m_mgr->GetRole()->GetRoleStatusManager()->CanRoleStartFight(BATTLE_MODE_MINI_GAME_BATTLE))
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return;
	}

	int npc_seq = this->GetNpcSeq(seq);
	if (npc_seq == 0)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	BattleData_Business business;
	business.Reset();

	business.mini_game_has_pet = has_pet ? 1 : 0;
	business.mini_game_has_partner = has_partner ? 1 : 0;
	business.mini_game_has_skill = has_skill ? 1 : 0;

	int battle_mode = BATTLE_MODE_MINI_GAME_BATTLE;	
	BattleManagerLocal::GetInstance().StartDemoFightReq(m_mgr->GetRole(), npc_seq, battle_mode, &business);
}

void MiniGame::OnSetMiniGameIdReq(int mini_game_id)
{
	if (MINI_GAME_NONE == m_mgr->GetMiniGameId() && mini_game_id > 0 && mini_game_id < MINI_GAME_MAX)
	{
		int old_game_id = m_mgr->GetMiniGameId();
		m_mgr->SetMiniGameId(mini_game_id);
		gamelog::g_log_other.printf(LL_INFO, "MiniGame::OnSetMiniGameIdReq role[%d,%s] old_game_id[%d] new_game_id[%d]",
			m_mgr->GetUid(), m_mgr->GetRoleName(), old_game_id, mini_game_id);
	}
}

void MiniGame::OnLogin()
{
	this->SendInfo();

	if (m_param.has_init_for_the_first_time > 0) return;

	m_param.has_init_for_the_first_time = 1;
	
	if (m_mgr->GetRole()->GetAccountUserNum() != 1) return; // 不是首角色，从头开始打

	int award_stage_num = LOGIC_CONFIG->GetMiniGameConfig()->GetOtherCfg().mail_award_stage_num;
	if (award_stage_num <= 0) return;

	const MiniGameIndexCfg* index_cfg = LOGIC_CONFIG->GetMiniGameConfig()->GetIndexCfg(m_mgr->GetMiniGameId());
	if (NULL == index_cfg) return;

	static MailContentParam contentparam;
	contentparam.Reset();
	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_mini_game_subject, index_cfg->mini_game_name.c_str());
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_mini_game_content, index_cfg->mini_game_name.c_str());
	if (lensub <= 0 || length <= 0) return;

	int item_index = 0;
	for (int i = 0; i < award_stage_num; ++i)
	{
		int stage_id = i + 1;
		const MiniGameStageRewardCfg* stage_cfg = LOGIC_CONFIG->GetMiniGameConfig()->GetStageRewardCfg(m_mgr->GetMiniGameId(), stage_id);
		if (NULL == stage_cfg) continue;

		for (size_t k = 0; k < stage_cfg->reward_list.size(); ++k)
		{
			if (item_index >= ARRAY_ITEM_COUNT(contentparam.item_list))
			{
				MailRoute::Instance().MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				contentparam.ClearAttachment();
				item_index = 0;
			}
			contentparam.item_list[item_index].item_id = stage_cfg->reward_list[k].item_id;
			contentparam.item_list[item_index].num = stage_cfg->reward_list[k].num;
			contentparam.item_list[item_index].is_bind = stage_cfg->reward_list[k].is_bind ? 1 : 0;
			item_index += 1;
		}
	}

	if (item_index > 0)
	{
		MailRoute::Instance().MailToUser(m_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}

	m_param.clear_stage_num = award_stage_num;
	this->SendInfo();

	gamelog::g_log_other.printf(LL_INFO, "%s role[%d,%s] clear_stage_num[%d]", __FUNCTION__, m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.clear_stage_num);
}

void MiniGame::SendInfo()
{
	Protocol::SCMiniGameInfo msg;
	msg.phase_reward_flag = m_param.phase_reward_flag;
	msg.finish_stage_num = m_param.clear_stage_num;
	m_mgr->NetSend(&msg, sizeof(msg));
}

int MiniGame::GetNpcSeq(int seq)
{
	return LOGIC_CONFIG->GetMiniGameNiuLangConfig()->GetNpcSeq(seq);
}

