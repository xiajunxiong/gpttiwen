#include "funcguide.hpp"
#include "other/funcguide/funcguideconfig.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "protocol/msgfuncguide.h"
#include "other/pet/pet.hpp"
#include "battle/battle_manager_local.hpp"

FuncGuide::FuncGuide()
{
}

FuncGuide::~FuncGuide()
{
}

void FuncGuide::InitFuncGuideInfo(RoleModuleManager * role_mgr, const FuncGuideParam & param)
{
	m_mgr = role_mgr;
	m_param = param;
}

void FuncGuide::GetFuncGuideParam(FuncGuideParam * out_param)
{
	*out_param = m_param;
}

void FuncGuide::OnFuncGuideLogin()
{
	this->SendEventFlag();
	this->SendRolePetDeadSummonGuideInfo();
}

void FuncGuide::ChooseNewbiePet(int choice)
{
	// 根据需求变动(选3只宠物改为抽1只宠物），代码已没有用，并被删除了
}

void FuncGuide::SendEventFlag()
{
	Protocol::SCFuncGuideFlagInfo fgfi;
	fgfi.flag_info = m_param.guide_flag;
	m_mgr->NetSend((const void *)&fgfi, sizeof(fgfi));
}

void FuncGuide::OnChallengeColosseum()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_CHALLENGE_COLOSSEUM))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_CHALLENGE_COLOSSEUM);
		this->SendEventFlag();
	}
}

bool FuncGuide::HasChallengeColosseumBefore()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_CHALLENGE_COLOSSEUM);
}

void FuncGuide::OnDrawGuidePet()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_PET_CHOICE))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_NEWBIE_PET_CHOICE);
		this->SendEventFlag();
	}
}

bool FuncGuide::IsDrawGuidePet()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_PET_CHOICE);
}

void FuncGuide::OnDrawGuidePet2()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PET_2))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_NEWBIE_DRAW_PET_2);
		this->SendEventFlag();
	}
}

bool FuncGuide::IsDrawGuidePet2()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PET_2);
}

void FuncGuide::OnDrawGuidePartner()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PARTNER))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_NEWBIE_DRAW_PARTNER);
		this->SendEventFlag();
	}
}

bool FuncGuide::IsDrawGuidePartner()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PARTNER);
}

void FuncGuide::OnDrawGuidePartner2()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PARTNER_2))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_NEWBIE_DRAW_PARTNER_2);
		this->SendEventFlag();
	}
}

bool FuncGuide::IsDrawGuidePartner2()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_DRAW_PARTNER_2);
}

void FuncGuide::OnCatchPetSucc()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_CATCH_PET))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_NEWBIE_CATCH_PET);
		this->SendEventFlag();
	}
}

bool FuncGuide::HasCatchPetBefore()
{
	return m_param.guide_flag.IsBitSet(FUNC_GUIDE_NEWBIE_CATCH_PET);
}

void FuncGuide::StartCatchPetGuideBattle()
{
	if (this->HasCatchPetBefore())
	{
		m_mgr->NoticeNum(errornum::EN_ALREADY_CATCH_PET_BEFORE);
		return;
	}

	int monster_group_id = LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg().catch_pet_guide_monster_group_id;
	BattleManagerLocal::GetInstance().StartMonsterFightReq(m_mgr->GetRole(), monster_group_id, BATTLE_MODE_NORMAL_MONSTER, 0);
}

void FuncGuide::FinishCloudArenaGuide()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_CLOUD_ARENA))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_CLOUD_ARENA);
		this->SendEventFlag();
	}
}

void FuncGuide::SendRolePetDeadSummonGuideInfo()
{
	Protocol::SCPetDeadSummonGuideInfo info;
	for (int i = 0; i < PET_DEAD_SUMMON_GUIDE_RECORD_DAYS; ++i)
	{
		info.guide_trigger_times[i] = m_param.pet_dead_summon_guide_times_list[i];
	}
	m_mgr->NetSend(&info, sizeof(info));
}

void FuncGuide::OnRoleTriggerPetDeadSummonGuide()
{
	int create_days = m_mgr->GetCreateDays();
	if (create_days > 0) // GetCreateDays从1开始，但这里数据从0开始存储
	{
		create_days -= 1;
	}
	create_days = GetMin(create_days, PET_DEAD_SUMMON_GUIDE_RECORD_DAYS - 1);
	create_days = GetMax(create_days, 0);
	m_param.pet_dead_summon_guide_times_list[create_days] += 1;

	this->SendRolePetDeadSummonGuideInfo();
}

void FuncGuide::GmSetAllFlag()
{
	for (int i = 0; i < FUNC_GUIDE_FLAG_MAX; ++i)
	{
		m_param.guide_flag.SetBit(i);
	}

	this->SendEventFlag();
}

void FuncGuide::FinishLunhuiWangchuanGuide()
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_LUNHUI_WANGCHUAN))
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_LUNHUI_WANGCHUAN);
		this->SendEventFlag();
	}
}

void FuncGuide::OnFastDreamNotesChallenge(int level)
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_FAST_DREAM_NOTES_CHALLENGE) && 
		level == LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg().fast_dream_notes_level)
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_FAST_DREAM_NOTES_CHALLENGE);
		this->SendEventFlag();
	}
	
}

void FuncGuide::OnPassLunHuiWangChuan(int level)
{
	if (!m_param.guide_flag.IsBitSet(FUNC_GUIDE_PASS_LUN_HUI_WANG_CHUAN) && 
		level == LOGIC_CONFIG->GetFuncGuideConfig()->GetOtherCfg().pass_lunhui_wangchuan_level)
	{
		m_param.guide_flag.SetBit(FUNC_GUIDE_PASS_LUN_HUI_WANG_CHUAN);
		this->SendEventFlag();
	}
	
}
