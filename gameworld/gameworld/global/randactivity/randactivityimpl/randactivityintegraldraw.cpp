#include "randactivityintegraldraw.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "obj/character/role.h"
#include "protocol/msgrandactivity.hpp"
#include "engineadapter.h"
#include "world.h"

RandActivityIntegralDraw::RandActivityIntegralDraw(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_INTEGRAL_DRAW), m_is_game_open(0)
{
}

RandActivityIntegralDraw::~RandActivityIntegralDraw()
{
}

void RandActivityIntegralDraw::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (CrossConfig::Instance().IsHiddenServer())
		return;

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo(user);
	}
}

void RandActivityIntegralDraw::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (!CrossConfig::Instance().IsHiddenServer() && to_status == ACTIVITY_STATUS_OPEN)
	{

		this->RASendGameInfo();
	}
}

void RandActivityIntegralDraw::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	memcpy(&m_is_game_open, arg_data.rand_activity_data.data, sizeof(m_is_game_open));
}

void RandActivityIntegralDraw::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	UNSTD_STATIC_CHECK(sizeof(m_is_game_open) <= sizeof(arg_data.rand_activity_data.data));
	memcpy(arg_data.rand_activity_data.data, &m_is_game_open, sizeof(m_is_game_open));
}

void RandActivityIntegralDraw::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (m_rand_activity_status != ACTIVITY_STATUS_OPEN) return;

	const tm * time_tm = EngineAdapter::Instance().LocalTime();
	if (NULL != time_tm)
	{
		int GAME_MIN = 15;				// 整点 到游戏结束的 分钟数
		if (m_is_game_open == 0 && time_tm->tm_min < GAME_MIN)
		{
			m_is_game_open = 1;
			this->RASendGameInfo();
		}

		if (m_is_game_open == 1 && time_tm->tm_min >= GAME_MIN)
		{
			m_is_game_open = 0;
			this->RASendGameInfo();
		}
	}
}

void RandActivityIntegralDraw::RASendGameInfo(Role * role)
{
	Protocol::SCRAIntegralDrawGameInfo protocol;
	protocol.is_game_open = m_is_game_open;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}
