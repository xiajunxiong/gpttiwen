#include "speciallogicfishingground.hpp"

#include "scene/scene.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"

#include "protocol/msgfishground.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolegatherfb/rolegatherfb.hpp"
#include "other/rolegatherfb/gatherfbconfig.hpp"

SpecialLogicFishingGround::SpecialLogicFishingGround(Scene* scene) 
	:SpecialLogic(scene)
{
}

SpecialLogicFishingGround::~SpecialLogicFishingGround()
{
}

void SpecialLogicFishingGround::Update(unsigned long interval, time_t now_second)
{
}

void SpecialLogicFishingGround::OnRoleEnterScene(Role * role)
{
}

bool SpecialLogicFishingGround::CanFish(Role * role)
{
	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfgBySceneId(m_scene->GetSceneID());
	if (NULL == level_cfg)
	{
		return false;
	}
	if (role->GetRoleModuleManager()->GetRoleGatherFb()->GetGatherPower() < level_cfg->expend_active)
	{
		return false;
	}

	return true;
}

void SpecialLogicFishingGround::OnFish(Role * role)
{
	//×îÉÙÏûºÄ1
	int consume = 0;
	const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfgBySceneId(m_scene->GetSceneID());
	if (NULL != level_cfg)
	{
		consume = level_cfg->expend_active;
	}
	if (consume <= 0)
	{
		consume = 1;
	}

	role->GetRoleModuleManager()->GetRoleGatherFb()->AddGatherPower(-consume, true);
}

void SpecialLogicFishingGround::OnStartFish(Role * role)
{
	if(NULL == role) return; 

	if (!this->CanFish(role))
	{
		role->NoticeNum(errornum::EN_GATHER_POWER_NOT_ENOUGH);
		return;
	}

	role->OnStartFish(m_fb_seq);
}

void SpecialLogicFishingGround::OnEndFishing(Role * role)
{
	if(NULL == role) return; 

	role->OnEndFishing(m_fb_seq);
}

void SpecialLogicFishingGround::OnStopFish(Role * role)
{
	if(NULL == role) return;

	role->OnStopFishing(Protocol::SCStopFishingNotify::STOP_REASON_TYPE_FAIL);
}

void SpecialLogicFishingGround::OnSetInfo(int fb_seq)
{
	m_fb_seq = fb_seq;
}
