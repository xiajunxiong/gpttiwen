#include "elementelf.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "config/otherconfig/elementelfconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "protocol/msgother.h"
#include "gamelog.h"

ElementElf::ElementElf()
{

}

ElementElf::~ElementElf()
{

}

void ElementElf::OnDayChange()
{
	m_param.today_showup_times = 0;
	gamelog::g_log_debug.printf(LL_INFO, "ElementElf::OnDayChange role[%d,%s] today_showup[%d]",
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.today_showup_times);
}

void ElementElf::Init(RoleModuleManager* mgr, const ElementElfParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void ElementElf::GetInitParam(ElementElfParam* out_param)
{
	if (NULL == out_param) return;

	*out_param = m_param;
}

void ElementElf::OnFightFinish(bool is_win, bool is_fly_away, const BattleData_Business& business)
{
	if (0 == business.gen_element_elf_monster) return;
	if (0 == business.element_elf_monster_id) return;

	this->OnElementElfShowUp();

	if (!is_win || is_fly_away) return;
	if (0 == business.has_kill_element_elf_monster) return;

	const ElementElfRewardCfg* cfg =
		LOGIC_CONFIG->GetElementElfConfig()
		->RandRewardCfg(business.element_elf_reward_group);
	if (NULL == cfg)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "ElementElf::OnFightFinish role[%d,%s] reward_group[%d]",
			m_mgr->GetUid(), m_mgr->GetRoleName(), business.element_elf_reward_group);
		return;
	}

	if (m_mgr->GetKnapsack()->CheckForPutList2(cfg->rewards))
	{
		m_mgr->GetKnapsack()->PutVec(cfg->rewards, PUT_REASON_ELEMENT_ELF_REWARD);
	}
	else
	{
		m_mgr->GetKnapsack()->SendMail(cfg->rewards);
	}

	Protocol::SCElementElfRewards msg;
	msg.reward_seq = cfg->seq;
	m_mgr->NetSend(&msg, sizeof(msg));
}

void ElementElf::OnElementElfShowUp()
{
	m_param.today_showup_times += 1;
	gamelog::g_log_debug.printf(LL_INFO, "ElementElf::OnElementElfShowUp role[%d,%s] today_showup[%d]",
		m_mgr->GetUid(), m_mgr->GetRoleName(), m_param.today_showup_times);
}

