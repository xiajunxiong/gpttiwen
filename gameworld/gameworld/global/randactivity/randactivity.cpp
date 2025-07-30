#include "world.h"
#include "gamelog.h"
#include "config/sharedconfig/sharedconfig.h"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"

#include "servercommon/activitydef.hpp"
#include "config/activityconfig/randactivityopencfg.hpp"
#include "global/usercache/usercache.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "protocol/msgactivity.hpp"
#include "randactivity.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"

RandActivity::RandActivity(RandActivityManager *rand_activity_manager, int activity_type)
: m_rand_activity_manager(rand_activity_manager), m_rand_activity_type(activity_type), m_rand_activity_status(ACTIVITY_STATUS_MAX), 
m_open_type(RAND_ACTIVITY_OPEN_TYPE_END), m_begin_time(0), m_end_time(0), m_is_dirty(false)
{

}

RandActivity::~RandActivity()
{

}

void RandActivity::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	this->SyncActivityStatusToGameworld();
	this->NotfiyRandActivityStatus(true);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 只有原服同步到隐藏服
		this->SendActivityDataToCross();
	}

	if(ACTIVITY_STATUS_OPEN == to_status)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				role->GetRoleModuleManager()->GetRoleActivityManager()->OnRAOpenOnlineSend(m_rand_activity_type);
			}
		}
		else
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				RoleCross* rolecross = role->GetRoleModuleManager()->GetRoleCross();
				UniqueServerID usid = UniqueServerID(rolecross->GetOriginPlatType(), rolecross->GetOriginServerId());
				if (usid != m_rand_activity_manager->GetUniqueServerID())
				{
					continue;
				}

				role->GetRoleModuleManager()->GetRoleActivityManager()->OnRAOpenOnlineSend(m_rand_activity_type);
			}
		}
	}
	else if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				role->GetRoleModuleManager()->GetRoleActivityManager()->OnRAClose(m_rand_activity_type);
			}
		}
		else
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				RoleCross* rolecross = role->GetRoleModuleManager()->GetRoleCross();
				UniqueServerID usid = UniqueServerID(rolecross->GetOriginPlatType(), rolecross->GetOriginServerId());
				if (usid != m_rand_activity_manager->GetUniqueServerID())
				{
					continue;
				}

				role->GetRoleModuleManager()->GetRoleActivityManager()->OnRAClose(m_rand_activity_type);
			}
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "OnSpecialActivityStatusChange::activity_type %d, from_status %d, to_status %d, begin %u, end %u", 
		m_rand_activity_type, from_status, to_status, m_begin_time, m_end_time);
}

void RandActivity::Init(const ActivityData &arg_data)
{
	UNSTD_STATIC_CHECK(sizeof(RandActivityData) <= sizeof(arg_data));
	const RandActivityData * data = &arg_data.rand_activity_data;

	m_begin_time = data->begin_time;
	m_end_time = data->end_time;
	m_open_type = data->activity_open_type;

	gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivity::Init type[%d] begin[%u] end[%u] open_type[%d]", m_rand_activity_type, m_begin_time, m_end_time, m_open_type);
}

void RandActivity::GetInitParam(ActivityData & arg_data)
{
	UNSTD_STATIC_CHECK(sizeof(RandActivityData) <= sizeof(arg_data));

	RandActivityData * data = &arg_data.rand_activity_data;

	data->begin_time = m_begin_time;
	data->end_time = m_end_time;
	data->activity_open_type = static_cast<char>(m_open_type);
}

void RandActivity::Update(unsigned long interval, time_t now_second)
{
	if (0 == m_begin_time || 0 == m_end_time) 
	{
		if (ACTIVITY_STATUS_MAX == m_rand_activity_status)
		{
			m_rand_activity_status = ACTIVITY_STATUS_CLOSE;
		}

		return;
	}

	unsigned int now = static_cast<unsigned int>(now_second);

	if (ACTIVITY_STATUS_MAX == m_rand_activity_status)
	{
		if (m_begin_time <= now && now < m_end_time)
		{
			m_rand_activity_status = ACTIVITY_STATUS_OPEN;
		}
		else
		{
			m_rand_activity_status = ACTIVITY_STATUS_CLOSE;
		}
	}

	switch (m_rand_activity_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			if (m_begin_time <= now && now < m_end_time)
			{
				m_rand_activity_status = ACTIVITY_STATUS_OPEN;
				this->OnSpecialActivityStatusChange(ACTIVITY_STATUS_CLOSE, ACTIVITY_STATUS_OPEN);
			}
		}
		break;

	case ACTIVITY_STATUS_OPEN:
		{
			if (now < m_begin_time || m_end_time <= now)
			{
				m_rand_activity_status = ACTIVITY_STATUS_CLOSE;
				this->OnSpecialActivityStatusChange(ACTIVITY_STATUS_OPEN, ACTIVITY_STATUS_CLOSE);
			}
		}
		break;
	}

	// 活动开启了，却未设定开启类型，默认开到随机活动
	if (ACTIVITY_STATUS_OPEN == m_rand_activity_status && RAND_ACTIVITY_OPEN_TYPE_END == m_open_type)
	{
		m_open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;
		this->SetDirty();
	}
}

void RandActivity::OnUserLogin(Role *user)
{
	this->NotfiyRandActivityStatus(false, user);
}

bool RandActivity::CalActivityOpenTime(const RandActivityOpenDetail* open_cfg, std::string& error_msg)
{
	if (NULL == open_cfg)
	{
		error_msg = "Open Cfg Is NULL";
		return false;
	}

	if (ACTIVITY_STATUS_OPEN == m_rand_activity_status)
	{
		error_msg = "Activity Is Opening";
		return false;
	}

	int limit_open_days = LOGIC_CONFIG->GetRandActivityOpenCfg()->GetLimitOpenDay(m_rand_activity_type);
	unsigned int allow_set_begin_time = 0u;
	if (limit_open_days > 0 && !open_cfg->is_from_open_server_day_cfg) // 基于开服天数开启的活动，不受limit_open_days限制
	{
		unsigned int server_open_zero_time = World::GetInstWorld()
			->GetServerOpenZeroTime(m_rand_activity_manager->GetUniqueServerID());

		allow_set_begin_time = server_open_zero_time +
			(limit_open_days - 1) * SECOND_PER_DAY;
	}

	if (open_cfg->begin_timestamp < allow_set_begin_time)
	{
		error_msg = STRING_SPRINTF("Limit Open Days[%d]", limit_open_days);
		return false;
	}

	if (RAND_ACTIVITY_OPEN_TYPE_NORMAL == open_cfg->open_type)
	{
		if ((0 == m_end_time && 0 == m_begin_time) || (m_end_time < static_cast<unsigned int>(EngineAdapter::Instance().Time()))) // 未有活动开过，或活动已经结束
		{
			this->SetRandActivityTime(open_cfg->begin_timestamp, open_cfg->end_timestamp, open_cfg->open_type, "OnInit CalActivityOpenTime");
			this->NotfiyRandActivityStatus(false, NULL);
			return true;
		}
		else
		{
			error_msg = "Activity Is Opening_222";
			return false;
		}
	}
	else
	{
		error_msg = "Unknown Open Type";
		return false;
	}
}

void RandActivity::SetRandActivityTime(unsigned int begin_time, unsigned int end_time, int open_type, const char* reason)
{
	m_begin_time = begin_time;
	m_end_time = end_time;
	m_open_type = open_type;

	this->SetDirty();

	gamelog::g_log_rand_activity.printf(LL_INFO, "SetRandActivityTime::activity_type %d, begin %u, end %u", m_rand_activity_type, m_begin_time, m_end_time);
	//this->SendActivityDataToCross();

	LOG_QUICK8(gamelog::g_log_quick, LOG_TYPE_RAND_ACTIVITY, m_rand_activity_type, "SetRandActivityTime", reason ? reason : "NULL", m_begin_time, m_end_time, "-", "-");
}

void RandActivity::OnForceToNextState()
{
	if (ACTIVITY_STATUS_OPEN == m_rand_activity_status)
	{
		m_end_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
	else
	{
		int duration_day =0;
		const RandActivityCfg * cfg = GLOBALCONFIG->GetDynamicConfig().GetRandActivityCfg(m_rand_activity_type);
		if (cfg != NULL)
		{
			duration_day = cfg->duration_day;
		}

		if (duration_day <= 0) // 不允许持续天数为0，这个是强制开启的接口
		{
			duration_day = 7;
		}

		m_begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_end_time = m_begin_time + EngineAdapter::Instance().NextDayInterval(0, 0, 0) + (duration_day - 1) * 24 * 3600;
	}

	this->SetDirty();
}

void RandActivity::SyncActivityStatusToGameworld()
{
	static ActivityStatus ggas;
	ggas.activity_type = (short)m_rand_activity_type;
	ggas.status = m_rand_activity_status;
	ggas.activity_begin_time = m_begin_time;
	ggas.activity_end_time = m_end_time;
	World::GetInstWorld()->GetSceneManager()->OnActivityStatusChange(&ggas);
}

void RandActivity::SetEndTime(unsigned int end_timestamp, bool is_notice, bool is_broadcast)
{
	m_end_time = end_timestamp;
	if (is_notice)
	{
		this->NotfiyRandActivityStatus(is_broadcast);
		this->SendActivityDataToCross();
	}
}

void RandActivity::NotfiyRandActivityStatus(bool is_broadcast, Role *user)
{
	if (NULL != user 
		&& CrossConfig::Instance().IsHiddenServer() 
		&& !this->CanSyncActivityDataToCross())
	{
		// 原服活动不会同步活动信息到跨服，需要向原服请求活动信息
		this->QueryOriginServerRAStatus(user);
		return;
	}

	static Protocol::SCActivityStatus as;
	this->AssembleActivityStatusMsg(&as);	
	as.is_broadcast = is_broadcast ? 1 : 0;

	if (NULL != user)
	{
		EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&as, sizeof(as));
	}
	else 
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			World::GetInstWorld()->SendToAllGateway((const char *)&as, sizeof(as));
		}
		else
		{
			this->SendRAStatusToSpecificServerUsers(&as);
		}
	}

	if (is_broadcast
		&& NULL == user
		&& !CrossConfig::Instance().IsHiddenServer()
		&& !this->CanSyncActivityDataToCross())
	{
		this->NotifyOriginServerRAStatusToUsersInHidden();
	}
}

void RandActivity::AssembleActivityStatusMsg(Protocol::SCActivityStatus* msg)
{
	msg->activity_open_type = m_open_type;
	msg->activity_type = (short)m_rand_activity_type;
	msg->status = static_cast<char>(m_rand_activity_status);
	msg->is_broadcast = 0;
	msg->next_status_switch_time = (ACTIVITY_STATUS_OPEN == m_rand_activity_status ? m_end_time : m_begin_time);
	msg->param_1 = m_begin_time;
	msg->param_2 = m_end_time;
}

void RandActivity::QueryOriginServerRAStatus(Role* user)
{
	crossgameprotocal::HiddenGameQueryOriginServerRAStatus msg;
	msg.query_activity_type = this->GetActivityType();
	msg.query_uid = user->GetUID();
	int plat_type = LocalConfig::Instance().GetPlatType();
	int server_id = user->GetUserId().db_index;
	InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, sizeof(msg));
}

void RandActivity::SendRAStatusToSpecificServerUsers(Protocol::SCActivityStatus* msg)
{
	static GSNetID gsnetid[256];
	int rolenum = 0;

	std::vector<int> online_user_list;
	UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
	for (int i = 0; i < (int)online_user_list.size(); i++)
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
		if (NULL == role) continue;

		RoleCross* rolecross = role->GetRoleModuleManager()->GetRoleCross();
		UniqueServerID usid = UniqueServerID(rolecross->GetOriginPlatType(), rolecross->GetOriginServerId());
		if (usid != m_rand_activity_manager->GetUniqueServerID())
		{
			continue;
		}

		gsnetid[rolenum++] = role->GetNetId();
		if (rolenum >= ARRAY_ITEM_COUNT(gsnetid))
		{
			EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, (const char*)msg, sizeof(Protocol::SCActivityStatus));
			rolenum = 0;
		}
	}

	if (rolenum > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, (const char*)msg, sizeof(Protocol::SCActivityStatus));
	}
}

void RandActivity::NotifyOriginServerRAStatusToUsersInHidden()
{
	crossgameprotocal::GameHiddenSendOriginServerRAStatus msg;
	msg.open_type = m_open_type;
	msg.activity_type = (short)m_rand_activity_type;
	msg.status = static_cast<short>(m_rand_activity_status);
	msg.next_status_switch_time = 
		(ACTIVITY_STATUS_OPEN == m_rand_activity_status ? m_end_time : m_begin_time);
	msg.param_1 = m_begin_time;
	msg.param_2 = m_end_time;

	msg.send_to_plat_type = LocalConfig::Instance().GetPlatType();
	msg.send_to_server_id = LocalConfig::Instance().GetMergeServerId();

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
}

void RandActivity::SyncActivityDataFromCross(CrossRandActivityDataParam &param)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		unsigned int old_begin_time = m_begin_time;
		unsigned int old_end_time = m_end_time;

		m_begin_time = param.open_time;
		m_end_time = param.close_time;
		m_open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;

		gamelog::g_log_rand_activity.printf(LL_INFO, "SyncActivityDataFromCross activity[%d] new_begin_time[%u] new_end_time[%u] old_begin_time[%u] old_end_time[%u] old_state[%s] ",
			this->GetActivityType(), m_begin_time, m_end_time, 
			old_begin_time, old_end_time, 
			ACTIVITY_STATUS_OPEN == m_rand_activity_status ? "OPEN" : "CLOSE");
	}
}

void RandActivity::SendActivityDataToCross()
{
	crossgameprotocal::GameCrossRandActivityInfoSync sync;
	sync.is_all_sync = 0;
	sync.count = 0;
	sync.plat_type = LocalConfig::Instance().GetPlatType();
	sync.server_id = LocalConfig::Instance().GetMergeServerId();
	if (this->CanSyncActivityDataToCross())
	{
		static ActivityData  act_data; act_data.Reset(m_rand_activity_type);
		this->GetInitParam(act_data);
		RandActivityData & data = act_data.rand_activity_data;
		sync.param_list[sync.count].open_time = data.begin_time;
		sync.param_list[sync.count].close_time = data.end_time;
		sync.param_list[sync.count].activity_type = m_rand_activity_type;
		memcpy(sync.param_list[sync.count].data, data.data, sizeof(data.data));
		++sync.count;
	}

	InternalComm::Instance().SendToCross((char *)&sync, sizeof(sync));
}

void RandActivity::GmNotfiyRandActivityStatus()
{
	this->NotfiyRandActivityStatus();
}

void RandActivity::GetRandActivityStausInfo(std::string & out_str)
{
	char temp_str[128] = { 0 };
	SNPRINTF(temp_str, 128, "%d", m_rand_activity_type);
	out_str = temp_str ;
	out_str += " ";

	switch (m_rand_activity_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			out_str += "rand_activity_status:close[";
		}
		break;
	case ACTIVITY_STATUS_STANDY:
		{
			out_str += "rand_activity_status:standy[";
		}
		break;
	case ACTIVITY_STATUS_OPEN:
		{
			out_str += "rand_activity_status:open[";
		}
		break;
	}
	SNPRINTF(temp_str, 128, "%d", m_rand_activity_status);
	out_str += temp_str;
	out_str += "] ";

	switch (m_open_type)
	{
	case RAND_ACTIVITY_OPEN_TYPE_NORMAL:
		{
			out_str += "open_type:Normal[" ;
		}
		break;
	}
	SNPRINTF(temp_str, 128, "%d", m_open_type);
	out_str += temp_str;
	out_str += "] begin_times:";
	SNPRINTF(temp_str, 128, "%d", m_begin_time);
	out_str += temp_str;
	out_str += "  end_times:";
	SNPRINTF(temp_str, 128, "%d", m_begin_time);
	out_str += temp_str;
}

void RandActivity::OnRoleParticipateRA(Role* role, const char* func)
{
	if (NULL == role) return;

	ROLE_LOG_QUICK6(LOG_TYPE_RAND_ACTIVITY_PARTICIPATE, role, this->GetActivityType(), 0, func, NULL);
}

void RandActivity::NotifyRandActivityStatusToHiddenUser(int uid)
{
	Protocol::SCActivityStatus as;
	this->AssembleActivityStatusMsg(&as);

	InternalComm::Instance().SendToHiddenUserThroughCross(uid, &as, sizeof(as));
}
