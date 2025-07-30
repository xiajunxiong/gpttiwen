#include "config/logicconfigmanager.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "world.h"
#include "global/worldstatus/worldstatus.hpp"
#include "gamelog.h"
#include "global/usercache/usercache.hpp"
#include "other/rolemodulemanager.hpp"

#include "servercommon/zhouqidef.hpp"
#include "global/zhouqimanager/msgzhouqi.h"
#include "global/zhouqimanager/zhouqiconfig.hpp"
#include "zhouqimanager.h"

ZhouQiManager & ZhouQiManager::Instance()
{
	static ZhouQiManager am;
	return am;
}

void ZhouQiManager::OnServerStart()
{
	this->CalZhouQiTimeInfo();
}

void ZhouQiManager::Update(unsigned long interval, time_t now_second)
{

}

void ZhouQiManager::OnDayChange()
{
	this->CalZhouQiTimeInfo();
	this->SendZhouQiInfo();

	std::vector<int> online_user_list;
	UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
	for (int i = 0; i < (int)online_user_list.size(); ++i)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
		if (role)
		{
			role->GetRoleModuleManager()->CheckZhouQiInfo();
		}
	}
}

void ZhouQiManager::OnUserLogin(Role * role)
{
	this->SendZhouQiInfo(role);
}

int ZhouQiManager::GetZhouQiNum()
{
	return m_zhou_qi_num;
}

void ZhouQiManager::GetZhouQiInfo(int * ARG_OUT zhou_qi_num, unsigned int * ARG_OUT end_time, unsigned int * ARG_OUT begin_time)
{
	if (zhou_qi_num)
	{
		*zhou_qi_num = m_zhou_qi_num;
	}

	if (end_time)
	{
		*end_time = (unsigned int)m_end_time;
	}

	if (begin_time)
	{
		*begin_time = (unsigned int)m_begin_time;;
	}
}

bool ZhouQiManager::IsInZhouQi(int play_type)
{
	unsigned int hidden_open_server = GetZeroTime(WorldStatus::Instance().GetHiddenOpenServerTime());  //这个值有可能是0，一次都没连上隐藏服就是0
	time_t now = EngineAdapter::Instance().Time();

	const ZhouQiOpenLimitCfg * open_limit_cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiOpenLimitCfg(now);
	if (!open_limit_cfg)
	{
		return false;
	}

	if (hidden_open_server == 0 || now < hidden_open_server + (open_limit_cfg->need_hidden_open_server_day - 1) * SECOND_PER_DAY)
	{
		return false;
	}

	const ZhouQiCfg * cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiCfg(this->GetZhouQiNum());
	if (!cfg)
	{
		return false;
	}

	const ZhouQiPlayTypeCfg * play_cfg = MapValuesConstPtr(cfg->play_cfg, play_type);
	if (!play_cfg)
	{
		return false;
	}

	return true;
}

bool ZhouQiManager::IsDayGameOpen(int play_type)
{
	const ZhouQiCfg * cfg = LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiCfg(this->GetZhouQiNum());
	if (!cfg)
	{
		return false;
	}

	const ZhouQiPlayTypeCfg * play_cfg = MapValuesConstPtr(cfg->play_cfg, play_type);
	if (!play_cfg)
	{
		return false;
	}

	int day = GetDayIndex(m_begin_time, EngineAdapter::Instance().Time()) + 1;
	if (day < play_cfg->start_day || play_cfg->end_day < day)
	{
		return false;
	}

	time_t now = EngineAdapter::Instance().Time();
	unsigned int zero_times = (unsigned int)GetZeroTime(now);
	unsigned int day_times = (unsigned int)(now - zero_times);

	return (play_cfg->day_begin_time[0] <= day_times && day_times < play_cfg->day_end_time[0]) ||
		(play_cfg->day_begin_time[1] <= day_times && day_times < play_cfg->day_end_time[1]);
}

void ZhouQiManager::SendZhouQiInfo(Role * role)
{
	Protocol::SCZhouQiInfo protocol;
	protocol.zhou_qi_num = m_zhou_qi_num;
	protocol.begin_time = (unsigned int)m_begin_time;
	protocol.end_time = (unsigned int)m_end_time;

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((char *)&protocol, sizeof(protocol));
	}
}

ZhouQiManager::ZhouQiManager()
{
	m_zhou_qi_num = 0;
	m_begin_time = 0;
	m_end_time = 0;
}

ZhouQiManager::~ZhouQiManager()
{
}

bool ZhouQiManager::CalZhouQiTimeInfo()
{
	time_t now = EngineAdapter::Instance().Time();
	LOGIC_CONFIG->GetZhouQiConfig()->GetZhouQiTimeInfo(now, &m_zhou_qi_num, &m_begin_time, &m_end_time);

	gamelog::g_zhou_qi.printf(LL_INFO, "%s zhou_qi_num:%d begin_time:%ud end_time:%ud", __FUNCTION__, m_zhou_qi_num, (unsigned int)m_begin_time, (unsigned int)m_end_time);

	return true;
}
