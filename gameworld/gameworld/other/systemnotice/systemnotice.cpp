#include "systemnotice.hpp"
#include "systemnoticeconfig.hpp"
#include "protocol/msgother.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"

SystemNotice::SystemNotice()
{
}

SystemNotice::~SystemNotice()
{
}

void SystemNotice::Init(RoleModuleManager * role_module_manager)
{
	m_role_module_mgr = role_module_manager;
}

void SystemNotice::GetAward(int seq)
{
	// 本功能已废弃
	/*
	const SystemNoticeAwardCon * award_cfg = LOGIC_CONFIG->GetSystemNoticeConfig()->GetSystemNoticeAwardCfg(seq);
	if (NULL == award_cfg)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < award_cfg->need_level)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return;
	}

	if (0 != (m_role_module_mgr->GetCommonData().system_notice_fetch_flag & (1 << seq)))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	
	if (award_cfg->award_num > 0 && !m_role_module_mgr->GetKnapsack()->CheckForPutList(award_cfg->award_num, award_cfg->award_list))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	// 检测通过，给予
	if (award_cfg->award_num > 0 && !m_role_module_mgr->GetKnapsack()->PutList(award_cfg->award_num, award_cfg->award_list, PUT_REASON_SYSTEM_NOTICE_REWARD)) return;

	m_role_module_mgr->GetCommonData().system_notice_fetch_flag |= (1 << seq);
	this->SendFetchFlagInfo();
	*/
}

void SystemNotice::SendFetchFlagInfo()
{
	static Protocol::SCSystemNoticeFlagInfo snfi;

	snfi.fetch_flag = m_role_module_mgr->GetCommonData().system_notice_fetch_flag;

	m_role_module_mgr->NetSend((const char *)&snfi, sizeof(snfi));
}
