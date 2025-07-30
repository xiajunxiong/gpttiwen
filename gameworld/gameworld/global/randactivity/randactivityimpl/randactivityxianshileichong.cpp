#include "randactivityxianshileichong.hpp"

#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "world.h"
#include "global/randactivity/randactivitymanager.hpp"

RandActivityXianShiLeiChong::RandActivityXianShiLeiChong(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG)
{


}

RandActivityXianShiLeiChong::~RandActivityXianShiLeiChong()
{

}

void RandActivityXianShiLeiChong::Init(const ActivityData &arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy(&m_param, ra_data->data, sizeof(m_param));
}

void RandActivityXianShiLeiChong::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivityXianShiLeiChong::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	if (ACTIVITY_STATUS_OPEN == to_status && !CrossConfig::Instance().IsHiddenServer())
	{
		// 活动开启时才重算  , 因为活动关闭后可能也会用到
		time_t open_server_time = World::GetInstWorld()->GetRealServerOpenTime(m_rand_activity_manager->GetUniqueServerID());
		m_param.open_day = GetDayIndex(GetZeroTime(open_server_time), EngineAdapter::Instance().Time()) + 1;

		if (m_param.open_day < 1)
			m_param.open_day = 1;
	}

	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
}

void RandActivityXianShiLeiChong::SyncActivityDataFromCross(CrossRandActivityDataParam &param)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_begin_time = param.open_time;
		m_end_time = param.close_time;
		m_open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;

		UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(param.data));

		memcpy(&m_param, param.data, sizeof(m_param));
	}
}

int RandActivityXianShiLeiChong::GetOpenDay()
{
	return  m_param.open_day < 1 ? 1 : m_param.open_day;
}
