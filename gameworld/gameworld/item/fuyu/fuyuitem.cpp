#include "fuyuitem.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/fuyu/fuyuconfig.hpp"
#include "gamelog.h"

FuYuItem::FuYuItem()
	:ItemBase(I_TYPE_FU_YU), m_fuyu_level(0)
{
}

int FuYuItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

void FuYuItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	if (NULL == param_data)
	{
		return;
	}

	FuYuParam & data = param_data->fu_yu_param;
	if (0 != data.has_net_value)
	{
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->RandFuYuParam(param_data, this->GetItemId(), LOGIC_CONFIG->GetFuYuConfig()->RandSpGroup()))
	{
		if (NULL != role)
		{
			gamelog::g_log_fu_yu.printf(LL_INFO, "%s, role[%d, %s] RandFuYuParam Failed! fu_yu_param[%s]", __FUNCTION__, role->GetUID(), role->GetName(), param_data->GetParamLog(I_TYPE_FU_YU));
		}
	}
}

void FuYuItem::RandomNetValueSpecial(Role * role, NetValueItemParam * param_data, int put_reason, int sp_group) const
{
	if (NULL == param_data)
	{
		return;
	}

	FuYuParam & data = param_data->fu_yu_param;
	if (0 != data.has_net_value)
	{
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->RandFuYuParam(param_data, this->GetItemId(), sp_group))
	{
		if (NULL != role)
		{
			gamelog::g_log_fu_yu.printf(LL_INFO, "%s, role[%d, %s] RandFuYuParam Failed! fu_yu_param[%s]", __FUNCTION__, role->GetUID(), role->GetName(), param_data->GetParamLog(I_TYPE_FU_YU));
		}
	}
}
