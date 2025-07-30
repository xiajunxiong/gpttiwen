#include "otheritem.hpp"

#include "obj/character/role.h"
#include "engineadapter.h"
#include "servercommon/errornum.h"

int OtherItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	role->NoticeNum(errornum::EN_KNAPSACK_USE_NOT_DIRECT);

	return -1;
}

UInt32 OtherItem::CalInvalidTime() const
{
	UInt32 tmp_time = 0;

	if (m_day_invalid_time >= 0)
	{
		int h = m_day_invalid_time / 100;
		int min = m_day_invalid_time % 100;
		tmp_time = (unsigned int)EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(h, min, 0);
	}
	else
	{
		tmp_time = ItemBase::CalInvalidTime();
	}
		
	return tmp_time;
}

bool OtherItem::HasInvalidTime() const
{
	return m_day_invalid_time >= 0 || ItemBase::HasInvalidTime();
}
