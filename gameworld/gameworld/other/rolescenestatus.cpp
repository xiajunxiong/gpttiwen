#include "rolescenestatus.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/commondata.hpp"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "other/partner/partnerconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "scene/speciallogic.hpp"

RoleSceneStatus::RoleSceneStatus() : m_role(NULL), pre_scene_type(0)
{

}

void RoleSceneStatus::Init(Role* role)
{
	m_role = role;
}

void RoleSceneStatus::OnRoleEnterScene()
{
	if (this->IsInAutoRecoverScene())
	{
		// 当进入非特殊副本的场景时，重置数据
		m_role->GetRoleModuleManager()->GetCommonData().is_in_no_recover_fb = 0;
		this->RecoverRole();
		this->RecoverPet();
		this->RecoverPartner();
	}
	else if (0 == m_role->GetRoleModuleManager()->GetCommonData().is_in_no_recover_fb)
	{
		//当进入特殊副本场景且之前的场景不是特殊副本场景时，获取最新的血量魔量最大值
		m_role->GetRoleModuleManager()->GetCommonData().is_in_no_recover_fb = 1;
		this->RecoverRole();
		this->RecoverPet();
		this->RecoverPartner();	
	}

	if (pre_scene_type != 0 && pre_scene_type != m_role->GetScene()->GetSceneType())
	{
		m_scene_buff_list.clear();
	}
}

void RoleSceneStatus::OnRoleLeaveScene()
{
	pre_scene_type = m_role->GetScene()->GetSceneType();
}

void RoleSceneStatus::UpdateRole(int remain_hp, int remain_mp)
{
	if (remain_hp <= 0) remain_hp = 1;
	if (remain_mp <= 0) remain_mp = 0;

	m_role->GetRoleModuleManager()->SetCurHpMp(remain_hp, remain_mp);
}

void RoleSceneStatus::UpdatePet(int pet_id, int pet_index, int remain_hp, int remain_mp)
{
	if (remain_hp <= 0) remain_hp = 1;
	if (remain_mp <= 0) remain_mp = 0;

	m_role->GetRoleModuleManager()->GetPet()->SetCurHpMp(pet_id, pet_index, remain_hp, remain_mp);
}

void RoleSceneStatus::UpdatePartner(int partner_id, int remain_hp, int remain_mp)
{
	if (remain_hp <= 0) remain_hp = 1;
	if (remain_mp <= 0) remain_mp = 0;

	m_role->GetRoleModuleManager()->GetPartner()->SetPartnerHPOrMPValue(partner_id, remain_hp, remain_mp);
}

bool RoleSceneStatus::IsInAutoRecoverScene()
{
	Scene* scene = m_role->GetScene();
	if (NULL != scene)
	{
		SpecialLogic* spl = scene->GetSpecialLogic();
		if (NULL != spl)
		{
			return spl->IsAutoRecover();
		}
	}
	return false;
}

void RoleSceneStatus::RecoverRole()
{
	m_role->GetRoleModuleManager()->Recover();
}

void RoleSceneStatus::RecoverPet()
{
	Pet* pet = m_role->GetRoleModuleManager()->GetPet();
	if (NULL != pet)
	{
		for (int i = 0; i < ItemNamespace::MAX_PET_GRID_NUM; ++i)
		{
			pet->Recover(i);
		}
	}
}

void RoleSceneStatus::RecoverPartner()
{
	Partner* partner = m_role->GetRoleModuleManager()->GetPartner();
	if (NULL != partner)
	{
		std::vector<int> partner_vec;
		partner->GetPartnerList(&partner_vec);
		for (size_t seq = 0; seq < partner_vec.size(); ++seq)
		{
			int pid = partner_vec[seq];
			partner->MaxPartnerHPOrMP(pid, MAX_TYPE_BOTH_TWO);
		}
	}
}

void RoleSceneStatus::SetSceneBuff(BuffEffect buff_param)
{
	if (buff_param.buff_id <= 0 || (buff_param.buff_end_tamp == 0 && buff_param.buff_times == 0))return;
	
	if (m_scene_buff_list.find(buff_param.buff_id) != m_scene_buff_list.end())
	{
		//有同类型是buff，刷新持续时间
	}
	else
	{
		//插入新buff效果
		m_scene_buff_list.insert(std::pair<int, BuffEffect>(buff_param.buff_id,buff_param));
	}
}
