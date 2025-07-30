#include "tiangongxunbao.hpp"
#include "protocol/cycleactivity/msgtiangongxunbao.hpp"
#include "tiangongxunbaoconfig.hpp"
#include "gamelog.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"

#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/zhouqimanager/zhouqiconfig.hpp"

RoleTianGongXunBao::RoleTianGongXunBao()
{
}

RoleTianGongXunBao::~RoleTianGongXunBao()
{
}

void RoleTianGongXunBao::Init(RoleModuleManager * module_mgr)
{
	m_module_mgr = module_mgr;
}

void RoleTianGongXunBao::Update(unsigned int now_second)
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO)) return;

	const ZhouQiStartPointCfg * ZQstart_cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiStartPointCfg(now_second);
	if (NULL == ZQstart_cfg) return;
	if (m_module_mgr->GetCommonData().tian_gong_xun_bao_start_timestamp != ZQstart_cfg->start_time)
	{
		m_module_mgr->GetCommonData().tian_gong_xun_bao_cur_grid = 0;
		m_module_mgr->GetCommonData().tian_gong_xun_bao_start_timestamp = ZQstart_cfg->start_time;
	}
}

void RoleTianGongXunBao::OnRoleLogin()
{
	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_TIAN_GONG_XUN_BAO))
	{
		return;
	}

	this->SendInfo();
}

void RoleTianGongXunBao::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	m_module_mgr->GetCommonData().tian_gong_xun_bao_free_flag = 0;

	this->SendInfo();
}

void RoleTianGongXunBao::StartAction()
{
	const TGXBOtherCfg& other_cfg = LOGIC_CONFIG->GetTianGongXunBaoConfig()->GetOtherCfg();

	if (m_module_mgr->GetCommonData().tian_gong_xun_bao_free_flag != 0 && !m_module_mgr->GetKnapsack()->HasItem(other_cfg.item_id, other_cfg.consume, false))	//判断道具数量
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	int next_grid = m_module_mgr->GetCommonData().tian_gong_xun_bao_cur_grid;

	const TGXBRewardCfg * reward_cfg = LOGIC_CONFIG->GetTianGongXunBaoConfig()->GetNextReward(next_grid); 
	if (NULL == reward_cfg) return;

	if (m_module_mgr->GetCommonData().tian_gong_xun_bao_free_flag != 0)
	{
		m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.item_id, other_cfg.consume, __FUNCTION__);
	}

	if (m_module_mgr->GetKnapsack()->CheckForPut(reward_cfg->reward.item_id, reward_cfg->reward.num))
	{
		m_module_mgr->GetKnapsack()->Put(reward_cfg->reward, PUT_REASON_TIAN_GONG_XUN_BAO, NULL, NULL, 0, 0, false);
	}
	else
	{
		m_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward, 1, SEND_MAIL_TYPE_TIAN_GONG_XUN_BAO_REWARD);
	}

	if (reward_cfg->is_broadcast)
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_tian_gong_xun_bao_broadcast, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), reward_cfg->reward.item_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	m_module_mgr->GetCommonData().tian_gong_xun_bao_cur_grid = next_grid;
	m_module_mgr->GetCommonData().tian_gong_xun_bao_free_flag = 1;

	gamelog::g_log_cycleactivity.printf(LL_INFO, "%s role[%d, %s] cur_grid[%d] reward[%d, %d]", __FUNCTION__,
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), next_grid, reward_cfg->reward.item_id, reward_cfg->reward.num);

	this->SendInfo();
}

void RoleTianGongXunBao::SendInfo()
{
	Protocol::SCTianGongXunBaoGridInfo msg;
	msg.cur_grid = m_module_mgr->GetCommonData().tian_gong_xun_bao_cur_grid;
	msg.used_free = m_module_mgr->GetCommonData().tian_gong_xun_bao_free_flag;

	m_module_mgr->NetSend(&msg, sizeof(msg));
}
