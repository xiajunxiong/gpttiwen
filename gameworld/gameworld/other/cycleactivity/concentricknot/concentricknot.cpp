#include "concentricknot.hpp"
#include "concentricknotconfig.hpp"

#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "global/team/team.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "protocol/cycleactivity/msgconcentricknot.hpp"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"
#include "gameworld/gameworld/other/route/mailroute.hpp"
#include "servercommon/noticenum.h"
#include "other/funopen/funopen.hpp"

ConcentricKnot::ConcentricKnot()
{
}

ConcentricKnot::~ConcentricKnot()
{
}

void ConcentricKnot::Init(RoleModuleManager * mgr, const ConcentricKnotParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void ConcentricKnot::GetInitParam(ConcentricKnotParam * param)
{
	*param = m_param;
}

void ConcentricKnot::OnDayChange()
{
	m_param.Reset();
}

void ConcentricKnot::OnLogout()
{
	if (m_param.game_end_times > 0)
	{
		Team * team = m_mgr->GetRole()->GetMyTeam();
		if (NULL == team)
			return;

		Protocol::SCConcentricKnotEndGame info;
		info.is_finish = 0;

		for (int i = 0; i < team->GetMemberCount(); ++i)
		{
			Role* member = team->GetMemberRoleByIndex(i);
			if (NULL == member) continue;

			member->GetRoleModuleManager()->GetConcentricKnot()->SetUseItem(0);
			member->GetRoleModuleManager()->GetConcentricKnot()->SetEndGameTimes(0);

			if (member->GetUID() != m_mgr->GetUid())
			{
				member->GetRoleModuleManager()->NetSend(&info, sizeof(info));
			}
		}
	}
}

void ConcentricKnot::OnClientReq(void * msg)
{
	if (NULL == msg) return;

	Protocol::CSConcentricKnotReq * req = (Protocol::CSConcentricKnotReq *)msg;

	switch (req->req_type)
	{
	case Protocol::CSConcentricKnotReq::REQ_TYPE_ENTER:
		{
			this->OnCanGame();
		}
		break;
	case Protocol::CSConcentricKnotReq::REQ_TYPE_AGREE:
		{
			this->OnAgree(req->p1);
		}
		break;
	case Protocol::CSConcentricKnotReq::REQ_TYPE_START:
		{
			this->OnStartGame(req->p1);
		}
		break;
	case Protocol::CSConcentricKnotReq::REQ_TYPE_OPEN_CARD:
		{
			this->OnOpenCard(req->p1);
		}
		break;
	case Protocol::CSConcentricKnotReq::REQ_TYPE_END:
		{
			this->OnEndGame(req->p1);
		}
		break;
	case Protocol::CSConcentricKnotReq::REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
	}
}

void ConcentricKnot::OnAddActiveReward(int active)
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_CONCENTRIC_KNOT) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_CONCENTRIC_KNOT))
	{
		return;
	}

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHOU_QI))return;

	int rate = LOGIC_CONFIG->GetConcentricKnotConfig()->GetActiveRateCfg(active);
	if (rate <= 0) return;

	if (LOGIC_CONFIG->GetConcentricKnotConfig()->IsEndActiveRate(active))		// 是否是最后一个活跃奖励
	{
		if (m_param.is_get_concentric_knot == 0) rate = 10000;					// 如果之前都没有拿过同心结， 那么最后一个必给
	}

	int rand_rate = RandomNum(10000);
	if (rand_rate < rate)
	{
		int rand_index = RandomNum((int)LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list.size());

		static ItemDataWrapper temp_item_wrapper;
		temp_item_wrapper.item_id = LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list[rand_index].item_id;
		temp_item_wrapper.num = LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list[rand_index].num;
		temp_item_wrapper.is_bind = (LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list[rand_index].is_bind) ? 1 : 0;

		unsigned int now_times = EngineAdapter::Instance().Time();
		temp_item_wrapper.invalid_time = now_times + EngineAdapter::Instance().NextDayInterval(0, 0, 0);

		if (m_mgr->GetKnapsack()->CheckForPut(temp_item_wrapper.item_id, temp_item_wrapper.num))
		{
			m_mgr->GetKnapsack()->Put(temp_item_wrapper, PUT_REASON_CONCENTRIC_KNOT);
		}
		else
		{
			static MailContentParam contentparam; contentparam.Reset();
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);

			contentparam.item_list[0] = temp_item_wrapper;

			MailRoute::MailToUser(m_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
			
			m_mgr->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);
		}

		m_param.is_get_concentric_knot = 1;

		gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s] active[%d] reward_item[%d, %d]", __FUNCTION__,
			m_mgr->GetUid(), m_mgr->GetRoleName(), active, temp_item_wrapper.item_id, temp_item_wrapper.num);
	}
}

void ConcentricKnot::SendInfo()
{
	Protocol::SCConcentricKnotInfo info;
	info.reward_times = m_param.reward_times;

	m_mgr->NetSend(&info, sizeof(info));
}

void ConcentricKnot::OnCanGame()
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_CONCENTRIC_KNOT) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_CONCENTRIC_KNOT))
	{
		m_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}

	if (!FunOpen::IsFunOpen(m_mgr->GetRole(), FUN_TYPE_ZHOU_QI))return;

	Team * team = m_mgr->GetRole()->GetMyTeam();
	if (!team || team->GetMemberCount() != 2)
	{
		return;
	}

	Role* team_member = NULL;
	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member) continue;

// 		if (member->GetRoleModuleManager()->GetConcentricKnot()->IsReward())
// 		{
// 			m_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_BE_ALLOW_ENTER_BATTLE);
// 			return;
// 		}

		if (m_mgr->GetRole()->GetUID() != member->GetUID())
		{
			team_member = member;
		}
	}

	if (NULL == team_member) return;

	std::vector<int> item_count;
	for (unsigned int i = 0; i < LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list.size(); i++)
	{
		int item_id = LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().list[i].item_id;
		int count = m_mgr->GetKnapsack()->Count(item_id);
		if (count > 0)
		{
			item_count.push_back(item_id);
		}
	}

	// 检查双方物品消耗
	std::vector<int> use_item_list;
	for (unsigned int i = 0; i < item_count.size(); i++)
	{
		int count = team_member->GetRoleModuleManager()->GetKnapsack()->Count(item_count[i]);
		if (count <= 0)
			continue;

		use_item_list.push_back(item_count[i]);
	}

	if (use_item_list.empty())
	{
		m_mgr->NoticeNum(errornum::EN_TEAM_MEMBEI_NOT_CANENTER_ITEM);
		return;
	}

	int rand_index = RandomNum((int)use_item_list.size());
	m_param.use_item = use_item_list[rand_index];
	team_member->GetRoleModuleManager()->GetConcentricKnot()->SetUseItem(use_item_list[rand_index]);

	Protocol::SCConcentricKnotAgreeNotice an_info;
	an_info.uid = m_mgr->GetUid();
	an_info.is_agree = 1;

	team->SendToMember(&an_info, sizeof(an_info));
}

void ConcentricKnot::OnAgree(int is_agree)
{
	Team * team = m_mgr->GetRole()->GetMyTeam();
	if (NULL == team)
		return;

	Protocol::SCConcentricKnotAgreeNotice an_info;
	an_info.uid = m_mgr->GetUid();
	an_info.is_agree = is_agree;

	team->SendToMember(&an_info, sizeof(an_info));
}

void ConcentricKnot::OnStartGame(int cards_group)
{
	Team * team = m_mgr->GetRole()->GetMyTeam();
	if (NULL == team)
		return;

	unsigned int begin_game_times = EngineAdapter::Instance().Time() + 5;
	unsigned int game_end_times = begin_game_times + LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().play_time;

	Protocol::SCConcentricKnotStart info;
	info.cards_group = cards_group;
	info.begin_times = begin_game_times;
	info.end_times = game_end_times;

	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member) continue;

		member->GetRoleModuleManager()->GetConcentricKnot()->SetEndGameTimes(game_end_times);
		member->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

void ConcentricKnot::OnOpenCard(int card_id)
{
	Team * team = m_mgr->GetRole()->GetMyTeam();
	if (NULL == team)
		return;

	Protocol::SCConcentricKnotOpenCard info;
	info.uid = m_mgr->GetUid();
	info.card_id = card_id;

	team->SendToMember(&info, sizeof(info));
}

void ConcentricKnot::OnEndGame(bool is_finish)
{
	if (!is_finish)
	{
		m_param.use_item = 0;

		Team * team = m_mgr->GetRole()->GetMyTeam();
		if (NULL == team)
			return;

		for (int i = 0; i < team->GetMemberCount(); ++i)
		{
			Role* member = team->GetMemberRoleByIndex(i);
			if (NULL == member) continue;

			member->GetRoleModuleManager()->GetConcentricKnot()->SetUseItem(0);
			member->GetRoleModuleManager()->GetConcentricKnot()->SetEndGameTimes(0);
		}

		Protocol::SCConcentricKnotEndGame info;
		info.is_finish = 0;

		team->SendToMember(&info, sizeof(info));
		return;
	}
	
	m_mgr->GetKnapsack()->ConsumeItem(m_param.use_item, 1, __FUNCTION__);

	if (!this->IsReward())
	{
		if (m_mgr->GetKnapsack()->CheckForPutList2(LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().reward))
		{
			m_mgr->GetKnapsack()->PutVec(LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().reward, PUT_REASON_CONCENTRIC_KNOT);
		}
		else
		{
			m_mgr->GetKnapsack()->SendMail(LOGIC_CONFIG->GetConcentricKnotConfig()->GetOtherCfg().reward, SNED_MAIL_TYPE_DEFAULT, true);
		}
	}

	gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s] use_item_id[%d]", __FUNCTION__,
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.use_item);

	m_param.use_item = 0;
	m_param.game_end_times = 0;
	m_param.reward_times = 1;

	Protocol::SCConcentricKnotEndGame info;
	info.is_finish = 1;

	m_mgr->NetSend(&info, sizeof(info));

	this->SendInfo();
}
