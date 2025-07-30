#include "roleattrpellet.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "servercommon/errornum.h"

#include "item/knapsack.h"
#include "obj/character/role.h"

#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/partner/partnerconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/roleattrpelletconfig.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "protocol/msgother.h"

RoleAttrPellet::RoleAttrPellet() : m_module_mgr(NULL)
{
	m_attrs_add.Reset();
}

RoleAttrPellet::~RoleAttrPellet()
{
}

void RoleAttrPellet::Init(RoleModuleManager * mgr, const RoleAttrPelletParam & param)
{
	m_module_mgr = mgr;
	m_param = param;
}

void RoleAttrPellet::GetOtherInitParam(RoleAttrPelletParam * out_param)
{
	*out_param = m_param;
}

const AttributeList & RoleAttrPellet::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(m_param.role_use_attr_list); ++i)
		{
			if(m_param.role_use_attr_list[i] <= 0) continue;

			int attr_type = GetAttrTypeByPelletType(i);
			if(attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

			int max_count = GetMin(LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + m_module_mgr->GetFlyUpSystem()->GetAttrPelletNum(), (int)m_param.role_use_attr_list[i]);
			const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_ROLE, m_module_mgr->GetRole()->GetAppearanceProf(), attr_type);
			if (NULL != attr_pellet_cfg)
			{
				m_attrs_add.m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
			}
		}
	}
	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void RoleAttrPellet::ReCalcAttrPartner(AttributeList & base_add, int partner_id, int profession)
{
	if(partner_id < 0 || partner_id >= PARTNER_NUM_MAX) return;

	for (int i = 0; i < ATTR_PELLET_TYPE_MAX && i < ARRAY_LENGTH(m_param.partner_use_attr_list[partner_id]); ++i)
	{
		if (m_param.partner_use_attr_list[partner_id][i] <= 0) continue;

		int attr_type = GetAttrTypeByPelletType(i);
		if (attr_type < 0 || attr_type >= BATTLE_ATTR_MAX) continue;

		int max_count = GetMin(m_module_mgr->GetPartner()->GetUseMaxAttrPellet(partner_id), (int)m_param.partner_use_attr_list[partner_id][i]);
		const RoleAttrPelletAttrListCfg * attr_pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetAttrPelletCfg(ROLE_ATTR_PELLET_TYPE_PARTNER, profession, attr_type);
		if (NULL != attr_pellet_cfg)
		{
			base_add.m_attrs[attr_type] += attr_pellet_cfg->attr_value * max_count;
		}
	}
 }

void RoleAttrPellet::GetRolePelletUseList(int max_num, short * role_use_pellet_count)
{
	if(max_num <= 0 || NULL == role_use_pellet_count) return;

	max_num = GetMin(max_num, ARRAY_LENGTH(m_param.role_use_attr_list));
	memcpy(role_use_pellet_count, m_param.role_use_attr_list, sizeof(short) * max_num);
}

void RoleAttrPellet::OnUseAttrPellet(int pellet_id, int num)
{
	const RoleAttrPelletAttrListCfg * pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetPelletListCfg(pellet_id, m_module_mgr->GetRole()->GetAppearanceProf());
	if (NULL == pellet_cfg || pellet_cfg->role_attr_pellet_type != ROLE_ATTR_PELLET_TYPE_ROLE)
	{
		return;
	}
	UNSTD_STATIC_CHECK(ATTR_PELLET_TYPE_MAX <= MAX_ATTR_PELLET_TYPE_NUM);
	int attr_pellet_type = GetPelletTypeByAttrType(pellet_cfg->attr_type);
	if (attr_pellet_type < 0 || attr_pellet_type >= ATTR_PELLET_TYPE_MAX)
	{
		return;
	}
	
	if (m_param.role_use_attr_list[attr_pellet_type] >= LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + m_module_mgr->GetFlyUpSystem()->GetAttrPelletNum())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_PELLET_USE_LIMIT);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(pellet_id);
	if (NULL == item_base || item_base->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_ATTR_PELLET)
	{
		return;
	}

	if (num + m_param.role_use_attr_list[attr_pellet_type] > LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetOtherCfg().use_limit + m_module_mgr->GetFlyUpSystem()->GetAttrPelletNum())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_PELLET_USE_LIMIT);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(pellet_id, num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	m_param.role_use_attr_list[attr_pellet_type] += num;
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_ATTR_PELLET, __FUNCTION__);
	this->OnSendSingleInfo(ROLE_ATTR_PELLET_TYPE_ROLE, m_param.role_use_attr_list[attr_pellet_type], attr_pellet_type, 0);
	gamelog::g_log_role_attr_pellet.printf(LL_INFO, "%s user[%d, %s] use_pellet_id[%d] use_num[%d] attr_pellet_type[%d] cur_num[%d] role_pellet_type[%d]", __FUNCTION__, m_module_mgr->GetUid(), 
		m_module_mgr->GetRoleName(), pellet_id, num, attr_pellet_type, m_param.role_use_attr_list[attr_pellet_type], ROLE_ATTR_PELLET_TYPE_ROLE);
}

void RoleAttrPellet::OnUsePetAttrPellet(int pellet_id, int num, int pet_index)
{
	m_module_mgr->GetPet()->OnUsePetAttrPellet(pellet_id, num, pet_index);
}

void RoleAttrPellet::OnUsePartnerAttrPellet(int pellet_id, int num, int partner_id)
{
	const NewPartnerBaseInfo* partner_info = m_module_mgr->GetPartner()->GetPartnerBaseInfoById(partner_id);
	if (NULL == partner_info)
	{
		return;
	}
	if (LOCK_STATE_UNLOCK != partner_info->partner_list.is_unlocked)
	{
		m_module_mgr->GetRole()->NoticeNum(errornum::EN_PARTNER_STATUS_IS_LOCKED);
		return;
	}

	const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == partner_cfg) return;

	const RoleAttrPelletAttrListCfg * pellet_cfg = LOGIC_CONFIG->GetRoleAttrPelletConfig()->GetPelletListCfg(pellet_id, partner_cfg->profession);
	if (NULL == pellet_cfg || pellet_cfg->role_attr_pellet_type != ROLE_ATTR_PELLET_TYPE_PARTNER)
	{
		return;
	}
	UNSTD_STATIC_CHECK(ATTR_PELLET_TYPE_MAX <= MAX_ATTR_PELLET_TYPE_NUM);
	int attr_pellet_type = GetPelletTypeByAttrType(pellet_cfg->attr_type);
	if (attr_pellet_type < 0 || attr_pellet_type >= ATTR_PELLET_TYPE_MAX)
	{
		return;
	}

	int can_use_max_attr_pellet = m_module_mgr->GetPartner()->GetUseMaxAttrPellet(partner_id);
	if (m_param.partner_use_attr_list[partner_id][attr_pellet_type] >= can_use_max_attr_pellet)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_PELLET_USE_LIMIT);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(pellet_id);
	if (NULL == item_base || item_base->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_ATTR_PELLET)
	{
		return;
	}

	if (num + m_param.partner_use_attr_list[partner_id][attr_pellet_type] > can_use_max_attr_pellet)
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_ATTR_PELLET_USE_LIMIT);
		return;
	}
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(pellet_id, num, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	m_param.partner_use_attr_list[partner_id][attr_pellet_type] += num;
	m_module_mgr->GetPartner()->OnAttrPelletChange(partner_id);
	this->OnSendSingleInfo(ROLE_ATTR_PELLET_TYPE_PARTNER, m_param.partner_use_attr_list[partner_id][attr_pellet_type], attr_pellet_type, partner_id);
	gamelog::g_log_role_attr_pellet.printf(LL_INFO, "%s user[%d, %s] use_pellet_id[%d] use_num[%d] attr_pellet_type[%d] cur_num[%d] role_pellet_type[%d] partner[%d] max_num[%d]",
	 __FUNCTION__, m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), pellet_id, num, attr_pellet_type, m_param.partner_use_attr_list[partner_id][attr_pellet_type], 
		ROLE_ATTR_PELLET_TYPE_PARTNER, partner_id, can_use_max_attr_pellet);
}

void RoleAttrPellet::OnSendAllInfo()
{
	Protocol::SCRoleAttrPelletAllInfo info;
	for (int i = 0; i < ARRAY_LENGTH(info.role_use_attr_list) && i < ARRAY_LENGTH(m_param.role_use_attr_list); ++i)
	{
		info.role_use_attr_list[i] = m_param.role_use_attr_list[i];
	}
	for (int i = 0; i < PARTNER_NUM_MAX; ++i)
	{
		for (int k = 0; k < MAX_ATTR_PELLET_TYPE_NUM; ++k)
		{
			info.partner_use_attr_list[i][k] = m_param.partner_use_attr_list[i][k];
		}
	}
	
	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleAttrPellet::OnSendSingleInfo(short role_attr_pellet_type, short use_count, int attr_pellet_type, int param1)
{
	Protocol::SCRoleAttrPelletSingleInfo info;
	info.role_attr_pellet_type = role_attr_pellet_type;
	info.use_count = use_count;
	info.attr_pellet_type = attr_pellet_type;
	info.param1 = param1;

	m_module_mgr->NetSend(&info, sizeof(info));
}
