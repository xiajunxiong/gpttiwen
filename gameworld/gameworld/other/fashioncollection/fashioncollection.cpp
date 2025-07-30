#include "fashioncollection.hpp"
#include "protocol/msgfashioncollection.h"
#include "other/surface/surface.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamelog.h"
#include "gameworld/engineadapter.h"
#include "obj/character/role.h"
#include "other/fashioncollection/fashioncollectionconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/struct/itemlistparam.h"

FashionCollection::FashionCollection():m_role_module(NULL)
{
}

FashionCollection::~FashionCollection()
{
}

void FashionCollection::Init(RoleModuleManager * role_module_manager, const FashionCollectionParam & param)
{
	m_role_module = role_module_manager;
	m_param = param;
}

void FashionCollection::GetOtherInitParam(FashionCollectionParam * param)
{
	*param = m_param;
}

const AttributeList & FashionCollection::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();

		// 基础属性
		for (int surface_id = 0; surface_id < ARRAY_LENGTH(m_param.m_fashion_collection); surface_id++)
		{
			if(m_param.m_fashion_collection[surface_id] <= 0) continue;

			const FashionCollectionLevelUpCfg* level_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetLevelUpCfg
			(m_role_module->GetRole()->GetBaseProfession(), surface_id, m_param.m_fashion_collection[surface_id]);
			if (NULL == level_cfg) continue;

			m_attrs_add.AddAttrCfg(level_cfg->attr_list);
		}

		// 专精属性
		for (int level = m_param.m_specialize; level > 0; level--)
		{
			const FashionCollectionStarAddCfg* star_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetStarCfg(m_role_module->GetRole()->GetBaseProfession(), level);
			if (NULL != star_cfg)
			{
				m_attrs_add.AddAttrCfg(star_cfg->attr_list);
			}
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void FashionCollection::ChangeProfession(int old_prof, int new_prof)
{
	m_role_module->ReCalcAttr(AttributeList::RECALC_REASON_FASHION_COLLECTION, __FUNCTION__);
}

void FashionCollection::OnClientRequest(const void * msg)
{
	if (NULL == msg) return;

	Protocol::CSFashionCollectionRequest * req = (Protocol::CSFashionCollectionRequest *)msg;

	switch (req->req_type)
	{
	case Protocol::FASHION_COLLECTION_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
	case Protocol::FASHION_COLLECTION_REQ_TYPE_LEVEL_UP:
		{
			this->OnLevelUp(req->param1);
		}
		break;
	case Protocol::FASHION_COLLECTION_REQ_TYPE_SPECIALIZE:
		{
			this->OnSpecializeActivation();
		}
		break;
	}

}

void FashionCollection::OnRoleLogin()
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.m_fashion_collection); i++)
	{
		if (m_role_module->GetSurface()->IsActiveSurface(i) && (m_param.m_fashion_collection[i] <= 0))
		{
			this->OnActivation(i);
		}
	}

	this->SendInfo();
}

void FashionCollection::OnActivation(int surface_id)
{
	if (surface_id < 0 || surface_id >= SURFACE_MAX_NUM) return;

	if (m_param.m_fashion_collection[surface_id] != 0) return;

	if (!m_role_module->GetSurface()->IsActiveSurface(surface_id))
	{
		m_role_module->NoticeNum(errornum::EN_HEADSHOT_SHIZHUANG_LIMIT);
		return;
	}

	// 激活默认查询1级
	const FashionCollectionLevelUpCfg* level_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetLevelUpCfg(m_role_module->GetRole()->GetBaseProfession(), surface_id, 1);
	if (NULL == level_cfg)
	{
		m_role_module->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.m_fashion_collection[surface_id] = 1;
	m_role_module->ReCalcAttr(AttributeList::RECALC_REASON_FASHION_COLLECTION, __FUNCTION__);
	this->SendInfo();
	gamelog::g_log_surface.printf(LL_INFO, "%s role[%d, %s] surface_id[%d] Activation succ", __FUNCTION__, m_role_module->GetUid(), m_role_module->GetRoleName(), surface_id);
}

void FashionCollection::Expired(int surface_id)
{
	if (surface_id < 0 || surface_id >= SURFACE_MAX_NUM) return;

	if (m_param.m_fashion_collection[surface_id] == 0) return;

	if (m_role_module->GetSurface()->IsActiveSurface(surface_id))
	{
		return;
	}

	m_param.m_fashion_collection[surface_id] = 0;

	m_role_module->ReCalcAttr(AttributeList::RECALC_REASON_FASHION_COLLECTION, __FUNCTION__);
	this->SendInfo();
	gamelog::g_log_surface.printf(LL_INFO, "%s role[%d, %s] surface_id[%d] Expired", __FUNCTION__, m_role_module->GetUid(), m_role_module->GetRoleName(), surface_id);
}

int FashionCollection::DecomposeSurface(short column, short index, int num)
{
	ItemGridData* item_grid = m_role_module->GetKnapsack()->GetItemGridData2(column, index);
	if (NULL == item_grid) return -1;

	if (num < item_grid->num)
	{
		m_role_module->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -2;
	}

	const std::pair<int, int>* decompose_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetDecomposeCfg(item_grid->item_id);
	if (NULL == decompose_cfg) return -3;

	if (!m_role_module->GetKnapsack()->CheckForPut(decompose_cfg->first, decompose_cfg->second * num))
	{
		m_role_module->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -4;
	}

	if (!m_role_module->GetKnapsack()->ConsumeItemByIndex(column, index, num, __FUNCTION__))
	{
		m_role_module->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -5;
	}

	m_role_module->GetKnapsack()->Put2(decompose_cfg->first, decompose_cfg->second * num, PUT_REASON_FASHION_COLLECTION_DECOMPOSE);

	gamelog::g_log_surface.printf(LL_INFO, "%s role[%d, %s] column[%d] index[%d] consume_num[%d] get_item[%d, %d]", __FUNCTION__,
		m_role_module->GetUid(), m_role_module->GetRoleName(), column, index, num, decompose_cfg->first, decompose_cfg->second * num);
	return 0;
}

void FashionCollection::OnLevelUp(int surface_id)
{
	if (surface_id < 0 || surface_id >= SURFACE_MAX_NUM) return;

	if (!m_role_module->GetSurface()->IsActiveSurface(surface_id))
	{
		m_role_module->NoticeNum(errornum::EN_HEADSHOT_SHIZHUANG_LIMIT);
		return;
	}

	if (m_param.m_fashion_collection[surface_id] <= 0 ||
		m_param.m_fashion_collection[surface_id] >= LOGIC_CONFIG->GetFashionCollectionConfig()->GetOtherCfg().max_level)
	{
		m_role_module->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const FashionCollectionLevelUpCfg* level_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetLevelUpCfg
		(m_role_module->GetRole()->GetBaseProfession(), surface_id, m_param.m_fashion_collection[surface_id] + 1);
	if (NULL == level_cfg)
	{
		m_role_module->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	std::map<int, int> consume_list;
	for (size_t i = 0; i < level_cfg->consume_list.size(); i++)
	{
		consume_list.insert(std::make_pair(level_cfg->consume_list[i].item_id, level_cfg->consume_list[i].num));
	}

	if (!m_role_module->GetKnapsack()->HasItemByMap(consume_list, false))
	{
		m_role_module->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module->GetKnapsack()->ConsumeItemByMap(consume_list, __FUNCTION__))
	{
		m_role_module->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	m_param.m_fashion_collection[surface_id] += 1;

	this->SendInfo();
	m_role_module->ReCalcAttr(AttributeList::RECALC_REASON_FASHION_COLLECTION, __FUNCTION__);
	gamelog::g_log_surface.printf(LL_INFO, "%s role[%d, %s] surface_id[%d] cur_level[%d]", __FUNCTION__,
		m_role_module->GetUid(), m_role_module->GetRoleName(), surface_id, m_param.m_fashion_collection[surface_id]);
}

void FashionCollection::OnSpecializeActivation()
{
	if (m_param.m_specialize >= LOGIC_CONFIG->GetFashionCollectionConfig()->GetOtherCfg().max_star_level)
	{
		m_role_module->NoticeNum(errornum::EN_FASHION_COLLECTION_MAX_SPECIALIZE_LEVEL);
		return;
	}

	const FashionCollectionStarAddCfg* star_cfg = LOGIC_CONFIG->GetFashionCollectionConfig()->GetStarCfg(m_role_module->GetRole()->GetBaseProfession(), m_param.m_specialize + 1);
	if (NULL == star_cfg) return;

	int total_star = 0;
	for (int i = 0; i < ARRAY_LENGTH(m_param.m_fashion_collection); i++)
	{
		const SurfaceCfg* surface_cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(i);
		if(NULL == surface_cfg || surface_cfg->duration != 0) continue;

		total_star += m_param.m_fashion_collection[i];
	}

	if (total_star < star_cfg->star_level)
	{
		m_role_module->NoticeNum(errornum::EN_FASHION_COLLECTION_STAR_NOT_ENOUGH);
		return;
	}

	m_param.m_specialize += 1;
	this->SendInfo();
	m_role_module->ReCalcAttr(AttributeList::RECALC_REASON_FASHION_COLLECTION, __FUNCTION__);
	gamelog::g_log_surface.printf(LL_INFO, "%s role[%d, %s] m_specialize[%d]", __FUNCTION__, m_role_module->GetUid(), m_role_module->GetRoleName(), m_param.m_specialize);
}

void FashionCollection::SendInfo()
{
	static Protocol::SCFashionCollectionInfo msg;
	msg.m_specialize_level = m_param.m_specialize;
	memcpy(msg.m_fashion_collection, m_param.m_fashion_collection, sizeof(msg.m_fashion_collection));

	m_role_module->NetSend(&msg, sizeof(msg));
}
