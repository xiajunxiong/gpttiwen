#include "randactivitychengfengpolang.hpp"
#include "config/randactivityconfig/impl/randactivitychengfengpolangconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"

#include "world.h"
#include "obj/character/role.h"

RandActivityChengFengPoLang::RandActivityChengFengPoLang(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG), m_is_open(0)
{
}

RandActivityChengFengPoLang::~RandActivityChengFengPoLang()
{
}

void RandActivityChengFengPoLang::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo(user);
	}
}

void RandActivityChengFengPoLang::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (m_rand_activity_status != ACTIVITY_STATUS_OPEN) return;

	const RandActivityChengFengPoLangConfig * config = static_cast<const RandActivityChengFengPoLangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG));
	if (NULL == config) return;

	const tm * time_tm = EngineAdapter::Instance().LocalTime();
	if (NULL != time_tm)
	{
		int GAME_MIN = config->GetOtherCfg().time;			// 整点到游戏结束的分钟数
		if (m_is_open == 0 && time_tm->tm_min < GAME_MIN)
		{
			m_is_open = 1;
			this->RASendGameInfo();
		}

		if (m_is_open == 1 && time_tm->tm_min >= GAME_MIN)
		{
			m_is_open = 0;
			this->RASendGameInfo();
		}
	}
}

void RandActivityChengFengPoLang::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		this->RASendGameInfo();
	}
}

void RandActivityChengFengPoLang::RASendGameInfo(Role * role)
{
	Protocol::SCRAChengFengPoLangOpenInfo protocol;
	protocol.is_open = m_is_open;

	if (role == NULL)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
}
