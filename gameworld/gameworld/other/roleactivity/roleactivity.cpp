#include "roleactivity.hpp"

#include "engineadapter.h"

#include "item/itempool.h"
#include "item/itembase.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "config/randactivityconfig/randactivitybonusconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "protocol/msgrandactivity.hpp"
#include "gamelog.h"

RoleActivity::RoleActivity(int type)
	:m_role_module_mgr(NULL) , m_role_activity_type(type) , m_is_dirty(false) , m_is_new_create (false) , m_is_invalid(true)
{

}

RoleActivity::~RoleActivity()
{

}

void RoleActivity::ClearDirtyMark()
{
	m_is_dirty = false;
	m_is_new_create = false;
}

void RoleActivity::OnCheckInitBonus()
{
	const std::vector<RABonusCfg> * cfg = LOGIC_CONFIG->GetRandActivityBonusConfig()->GetBounsCfgByActivityId(m_role_activity_type);
	if (NULL == cfg) return;
	for (int i = 0; i < (int)cfg->size(); i++)
	{
		const RABonusCfg & node_cfg = (*cfg)[i];
		m_role_module_mgr->GetRoleActivityManager()->SetBonusFlag(node_cfg.seq, false);
	}
	m_role_module_mgr->GetRoleActivityManager()->SendBonusInfo();
}

void RoleActivity::OnSendResultNotice(short result_type, const std::vector<ItemConfigData> * item_list, const std::vector<RAMoneyInfo>* money_list, int param1, int param2, int exp) const
{
	if(result_type <= Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_BEGIN || result_type >= Protocol::SCRAResultNotice::RAND_ACTIVITY_RESULT_NOTICE_TYPE_MAX) return;

	static Protocol::SCRAResultNotice info;
	info.result_type = result_type;
	info.activity_type = m_role_activity_type;
	info.param1 = param1;
	info.param2 = param2;

	int list_num = 0;
	if (NULL != item_list && !item_list->empty())
	{
		for (int i = 0; i < (int)item_list->size() && list_num < Protocol::SCRAResultNotice::MAX_REWARD_ITEM_NUM; ++i)
		{
			info.item_list[list_num].Reset();

			info.item_list[list_num].item_id = (*item_list)[i].item_id;
			info.item_list[list_num].is_bind = (*item_list)[i].is_bind;
			info.item_list[list_num].count = (*item_list)[i].num;

			const ItemBase* item = ITEMPOOL->GetItem((*item_list)[i].item_id);
			if (NULL != item && item->GetItemType() == I_TYPE_CONVERT_ITEM)
			{
				int new_item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob((*item_list)[i].item_id, m_role_module_mgr->GetRole()->GetAppearanceProf());
				if (new_item_id > 0)
				{
					info.item_list[i].item_id = new_item_id;
				}
				else
				{
					continue;
				}
			}
			list_num += 1;
		}
	}
	if (NULL != money_list && list_num < Protocol::SCRAResultNotice::MAX_REWARD_ITEM_NUM && !money_list->empty())
	{
		for (int i = 0; i < (int)money_list->size() && list_num < Protocol::SCRAResultNotice::MAX_REWARD_ITEM_NUM; i++)
		{
			info.item_list[list_num].Reset();

			switch ((*money_list)[i].money_type)
			{
			case MONEY_TYPE_GOLD:
			{
				info.item_list[list_num].item_id = GOLD_ITEM_ID;
			}
			break;
			case MONEY_TYPE_COIN_BIND:
			{
				info.item_list[list_num].item_id = COIN_ITEM_ID;
			}
			break;
			default:
				break;
			}
			if(0 == info.item_list[list_num].item_id) continue;

			info.item_list[list_num].count = (*money_list)[i].money_count;
			list_num += 1;
		}
	}

	if (exp > 0 && list_num < Protocol::SCRAResultNotice::MAX_REWARD_ITEM_NUM)
	{
		info.item_list[list_num].item_id = EXP_ITEM_ID;
		info.item_list[list_num].count = exp;
		info.item_list[list_num].is_bind = 0;
		info.item_list[list_num].has_param = 0;
		info.item_list[list_num].param_length = 0;
		list_num += 1;
	}


	info.count = list_num;
	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &info);
}

bool RoleActivity::IsDataChange()
{
	if (m_is_dirty || m_is_new_create)
	{
		return true;
	}

	return false;
}

void RoleActivity::OnParticipateRA(const char* func)
{
	ROLE_LOG_QUICK6(LOG_TYPE_RAND_ACTIVITY_PARTICIPATE, m_role_module_mgr->GetRole(), m_role_activity_type, 0, func, NULL);
}

