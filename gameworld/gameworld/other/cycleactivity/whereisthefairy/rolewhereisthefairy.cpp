#include "rolewhereisthefairy.hpp"
#include "gamelog.h"

#include "item/knapsack.h"
#include "obj/character/role.h"

#include "protocol/cycleactivity/msgwhereisthefairy.hpp"

#include "servercommon/errornum.h"
#include "servercommon/zhouqidef.hpp"

#include "config/logicconfigmanager.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/cycleactivity/whereisthefairy/whereisthefairyconfig.hpp"

RoleWhereIsTheFairy::RoleWhereIsTheFairy()
{
}

RoleWhereIsTheFairy::~RoleWhereIsTheFairy()
{
}

void RoleWhereIsTheFairy::Init(RoleModuleManager * module_mgr)
{
	m_role_module_mgr = module_mgr;
}

void RoleWhereIsTheFairy::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	m_role_module_mgr->GetCommonData().is_day_fetch_where_is_the_fairy_flag = 0;
	this->SendAllInfo();
}

void RoleWhereIsTheFairy::OnFinishGame()
{
	if (0 != m_role_module_mgr->GetCommonData().is_day_fetch_where_is_the_fairy_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_HAS_GET_REWARD);
		return;
	}

	const WhereIsTheFairyRewardCfg & reward_cfg = LOGIC_CONFIG->GetWhereIsTheFairyConfig()->GetRewardCfg();
	if (!reward_cfg.reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg.reward_list))
		{
			if (m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg.reward_list, PUT_REASON_WHERE_IS_THE_FAIRY))
			{
				m_role_module_mgr->GetKnapsack()->NotifyGetItemListVec(reward_cfg.reward_list, GET_REASON_WHERE_IS_THE_FAIRY);
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg.reward_list, 1);
		}
	}

	m_role_module_mgr->GetCommonData().is_day_fetch_where_is_the_fairy_flag = 1;
	this->SendAllInfo();

	gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s] play_type [%d] Finish Game", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		ZHOU_QI_PLAY_TYPE_WHERE_IS_THE_FAIRY);
}

void RoleWhereIsTheFairy::SendAllInfo()
{
	Protocol::SCRoleWhereIsTheFairyInfo info;
	info.is_day_fetch_where_is_the_fairy_flag = m_role_module_mgr->GetCommonData().is_day_fetch_where_is_the_fairy_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}
