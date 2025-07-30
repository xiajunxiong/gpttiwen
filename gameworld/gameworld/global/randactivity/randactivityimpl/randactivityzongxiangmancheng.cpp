#include "randactivityzongxiangmancheng.hpp"
#include "config/randactivityconfig/impl/randactivityzongxiangmanchengconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "world.h"
#include "obj/character/role.h"

RandActivityZongXiangManCheng::RandActivityZongXiangManCheng(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG)
{
}

RandActivityZongXiangManCheng::~RandActivityZongXiangManCheng()
{
}

void RandActivityZongXiangManCheng::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo(user);
	}
}

void RandActivityZongXiangManCheng::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (m_rand_activity_status != ACTIVITY_STATUS_OPEN) return;
}

void RandActivityZongXiangManCheng::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo();
	}
}

void RandActivityZongXiangManCheng::RASendGameInfo(Role * role)
{
	/*Protocol::SCRAChengFengPoLangOpenInfo protocol;
	protocol.is_open = 0;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}*/
}
