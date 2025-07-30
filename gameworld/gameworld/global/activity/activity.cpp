#include "activity.hpp"

#include "globalconfig/globalconfig.h"
#include "engineadapter.h"
#include "world.h"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "protocol/msgactivity.hpp"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "servercommon/activitydef.hpp"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/activity/activitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "config/logicconfigmanager.hpp"

Activity::Activity(ActivityManager *activity_manager, int activity_type) : m_activity_manager(activity_manager), m_activity_type(activity_type), 
	m_activity_status(ACTIVITY_STATUS_CLOSE), m_next_status_switch_time(0),m_next_open_wday(0), m_is_dirty(false), m_already_notify_standy(false), 
	m_next_role_data_save_time(0), m_is_role_data_change(false)
{
	m_role_data_iter = m_role_data.end();
}

Activity::~Activity()
{

}

void Activity::Init(const ActivityData & arg_data)
{
	//UNSTD_STATIC_CHECK  (sizeof(NormalActivityData) <= sizeof(arg_data));
	//const NormalActivityData * data = (const NormalActivityData *)&arg_data;

	

}

void Activity::GetInitParam(ActivityData & arg_data)
{
	//UNSTD_STATIC_CHECK(sizeof(NormalActivityData) <= sizeof(arg_data));
	//NormalActivityData * data = (NormalActivityData*)&arg_data;


}

void Activity::OnBattleServerDisconnect(int battle_server_id)
{
}

void Activity::SetDirty()
{
	m_is_dirty = true;
}

void Activity::OnActivityStatusChange(int from_status, int to_status)
{
	static ActivityStatus ggas;
	ggas.activity_type = (short)m_activity_type;
	ggas.status = (short)to_status;
	ggas.activity_begin_time = 0;

	switch (to_status)
	{
	case ACTIVITY_STATUS_STANDY:
		{
			ggas.activity_begin_time = m_next_status_switch_time;
			ggas.activity_end_time = ggas.activity_begin_time + (unsigned int)m_statustime_list_cfg.statustime[ACTIVITY_STATUS_OPEN].dur_s;
		}
		break;

	case  ACTIVITY_STATUS_OPEN:
		{
			if (m_statustime_list_cfg.statustime[ACTIVITY_STATUS_STANDY].Invalid())
			{
				ggas.activity_begin_time = m_next_status_switch_time;
			}
		}
		break;

	case ACTIVITY_STATUS_CLOSE:
		{
			m_already_notify_standy = false;

			this->OnCalcNextSwtichTimeByClose();
		}
		break;
	}

	if (from_status != m_activity_status)
	{
		this->SendActivtyNotify(m_activity_status);
	}
	
	World::GetInstWorld()->GetSceneManager()->OnActivityStatusChange(&ggas);

	this->NotfiyActivityStatus(from_status != m_activity_status);
	this->SyncActivityStatusHiddenToGameworld();
}

void Activity::Update(unsigned long interval, time_t now_second)
{
	if (0 == m_next_status_switch_time)
	{
		this->OnCalcNextSwtichTimeByClose();
		this->SyncActivityStatusHiddenToGameworld();
	}
	else
	{
		if (now_second >= m_next_status_switch_time)
		{
			int old_activity_status = m_activity_status;
			m_activity_status = GLOBALCONFIG->GetDynamicConfig().GetActivtyNextStatus(m_activity_type, m_activity_status, m_next_open_wday, &m_statustime_list_cfg);
			m_next_status_switch_time = GLOBALCONFIG->GetDynamicConfig().GetActivityNextSwitchTime(m_activity_type, m_activity_status, m_next_open_wday, &m_statustime_list_cfg, true);
			this->OnActivityStatusChange(old_activity_status, m_activity_status);
		}
	}

	if (now_second >= m_next_role_data_save_time)
	{
		m_next_role_data_save_time = now_second + 60;
		this->SaveRoleData();
	}
}

void Activity::OnCalcNextSwtichTimeByClose()
{
	bool is_find = false;
	GLOBALCONFIG->GetDynamicConfig().GetActivityNextCfg(m_activity_type, &m_next_open_wday, &m_statustime_list_cfg, &is_find);
	m_next_status_switch_time = GLOBALCONFIG->GetDynamicConfig().GetActivityNextSwitchTime(m_activity_type, m_activity_status, m_next_open_wday, &m_statustime_list_cfg, is_find);
}

void Activity::OnUserLogin(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	this->NotfiyActivityStatus(false, role);
}

void Activity::OnUserLogout(Role *role)
{
	if (NULL == role)
	{
		return;
	}
}

void Activity::OnUserEnterScene(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	if (ACTIVITY_STATUS_OPEN != m_activity_status && ACTIVITY_STATUS_STANDY != m_activity_status) 
	{
		return;
	}
}

void Activity::OnUserLeaveScene(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	if (ACTIVITY_STATUS_OPEN != m_activity_status && ACTIVITY_STATUS_STANDY != m_activity_status) 
	{
		return;
	}
}

void Activity::OnConnectCrossServerSucc()
{
}

void Activity::OnUserLoginOriginGame(int role_id)
{

}

void Activity::OnGameServerConnect(std::set<int> server_id)
{

}

void Activity::NotfiyActivityStatus(bool is_broadcast, Role *role)
{
	static Protocol::SCActivityStatus as;
	as.activity_type = (short)m_activity_type;
	as.status = static_cast<char>(m_activity_status);
	as.is_broadcast = is_broadcast ? 1 : 0;
	as.next_status_switch_time = m_next_status_switch_time;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&as, sizeof(as));
	}
	else 
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&as, sizeof(as));
	}
}

void Activity::GetChangeRoleDataList(ActivityRoleDataParam & ARG_OUT role_data_list, bool is_clear_new_and_invalid_item)
{
	role_data_list.count = 0;

	for (std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.begin(); iter != m_role_data.end();)
	{
		if (role_data_list.count >= ARRAY_LENGTH(role_data_list.data_list))
		{
			break;
		}

		const ActivityPerRoleDataNode & role_data_node = iter->second;
		if (role_data_node.is_dirty)
		{
			ActivityRoleDataParam::DBActivityRoleDataItem role_data_item;
			role_data_item.activity_type = this->GetActivityType();
			role_data_item.role_id = iter->first;

			if (role_data_node.is_invalid)
			{
				if (role_data_node.is_new_create)
				{
					if (is_clear_new_and_invalid_item)
					{
						m_role_data.erase(iter++);
					}

					continue;
				}

				role_data_item.change_state = structcommon::CS_DELETE;
				role_data_list.data_list[role_data_list.count++] = role_data_item;
			}
			else
			{
				role_data_item.change_state = !role_data_node.is_new_create ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				role_data_item.data = role_data_node.data;

				role_data_list.data_list[role_data_list.count++] = role_data_item;
			}
		}

		++iter;
	}
}

void Activity::ClearActivityRoleChangeFlag(const ActivityRoleDataParam & role_data_param)
{
	for (int i = 0; i < role_data_param.count && i < ARRAY_LENGTH(role_data_param.data_list); ++i)
	{
		int role_id = role_data_param.data_list[i].role_id;
		int change_state = role_data_param.data_list[i].change_state;
		if (change_state == structcommon::CS_DELETE)
		{
			m_role_data.erase(role_id);
		}
		else
		{
			ActivityPerRoleDataNode & role_data_node = m_role_data[role_id];
			role_data_node.is_dirty = false;
			role_data_node.is_new_create = false;
		}
	}
}

void Activity::SendActivtyNotify(int notify_status)
{
	if (m_activity_type == ACTIVITY_TYPE_WORLD_BOSS || m_activity_type == ACTIVITY_TYPE_WORLD_BOSS_2 || ACTIVITY_TYPE_WORLD_BOSS_3 == m_activity_type) return;
	if (m_activity_type == ACTIVITY_TYPE_GUILD_HONOR_BATTLE && ACTIVITY_STATUS_OPEN == notify_status) return;

	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_activity_state, m_activity_type, notify_status);
	if (length <= 0) return;

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	SysMsgType msg_type = GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT);	// 跑马灯和系统栏都要显示
	if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, msg_type, 0, 0, this->GetActivityLimitLevel(m_activity_type)))
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);		// 通知所有玩家
	}

	if (ActivityManager::IsCrossActivity(m_activity_type) && CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToAllGameUserThroughCross((const char *)&sm, sendlen);
	}
}

void Activity::SetActivityState(const SyncActivityStatus & as)
{
	m_activity_status = as.activity_status;
	m_next_status_switch_time = as.next_status_switch_time;
	this->NotfiyActivityStatus();
}

void Activity::ForceToNextState(unsigned int * next_status_switch_timestamp)
{
	if (NULL != next_status_switch_timestamp)
	{
		m_next_status_switch_time = *next_status_switch_timestamp;
	}
	else
	{
		m_next_status_switch_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
}

void Activity::ForceToCloseState()
{
	int old_activity_status = m_activity_status;
	m_activity_status = ACTIVITY_STATUS_CLOSE;
	this->OnActivityStatusChange(old_activity_status, m_activity_status);
}

void Activity::OnQueryRoomStatusReq(Role *role)
{
	if (NULL == role)
	{
		return;
	}

}

unsigned int Activity::GetNextActivityBeginTime()
{
	switch (m_activity_status)
	{
	case ACTIVITY_STATUS_CLOSE:
	{
		return m_next_status_switch_time + (unsigned int)m_statustime_list_cfg.statustime[ACTIVITY_STATUS_STANDY].dur_s;
	}
	break;

	case ACTIVITY_STATUS_STANDY:
	{
		return m_next_status_switch_time;
		
	}
	break;

	case ACTIVITY_STATUS_OPEN:
	{
		return 0u;
	}
	break;

	}

	return 0u;
}

int Activity::GetActivityLimitLevel(int activity_type)
{
	UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX == 19);
	int fun_open_type = 0;
	switch (activity_type)
	{
		case ACTIVITY_TYPE_WORLD_BOSS:
			fun_open_type = FUN_TYPE_WORLD_BOSS;
			break;
		case ACTIVITY_TYPE_TEAM_FIGHT:
			fun_open_type = FUN_TYPE_SQUARD_FIGHT;
			break;
		case ACTIVITY_TYPE_MAZE_RACER:
			fun_open_type = FUN_TYPE_MAZE_RACE;
			break;
		case ACTIVITY_TYPE_BRAVE_GROUND:
			fun_open_type = FUN_TYPE_BRAVE_GROUND;
			break;
		case ACTIVITY_TYPE_GUILD_FIGHT:
			fun_open_type = FUN_TYPE_GUILD_LUANDOU;
			break;
		case ACTIVITY_TYPE_CHIEF_ELECTION:
			fun_open_type = FUN_TYPE_CHIEF_ELETION;
			break;
		case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
			fun_open_type = FUN_TYPE_CHIEF_PEAK;
			break;
		case ACTIVITY_TYPE_GUILD_HONOR_BATTLE:
			fun_open_type = FUN_TYPE_GUILD_HONOR_BATTLE;
			break;
		case ACTIVITY_TYPE_GUILD_ANSWER:
			fun_open_type = FUN_TYPE_GUILD_ANSWER;
			break;
		case ACTIVITY_TYPE_WORLD_BOSS_2:
			fun_open_type = FUN_TYPE_WORLD_BOSS_2;
			break;
		case ACTIVITY_TYPE_ONLY_FIGHT:
			fun_open_type = FUN_TYPE_ONLY_FIGHT;
			break;
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
			fun_open_type = FUN_TYPE_HUANJIEZHANCHANG;
			break;
		case ACTIVITY_TYPE_HUNDRED_GHOST:
			fun_open_type = FUN_TYPE_HUNDRED_GHOST;
			break;
		case ACTIVITY_TYPE_SILK_ROADS:
			fun_open_type = FUN_TYPE_SILK_ROADS;
			break;
		case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
			fun_open_type = FUN_TYPE_WORLD_TEAM_ARENA;
			break;
		case ACTIVITY_TYPE_QING_YUAN_SHENG_HUI:
			fun_open_type = FUN_TYPE_QING_YUAN_SHENG_HUI;
			break;
		case ACTIVITY_TYPE_WORLD_BOSS_3:
			fun_open_type = FUN_TYPE_WORLD_BOSS_3;
			break;
		default:
			return 0;
	}
	const FunOpenCfg* fun_cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(fun_open_type);
	if (NULL == fun_cfg)
	{
		return 0;
	}

	return fun_cfg->limit_level;
}

bool Activity::IsActivityOpen()
{
	return ACTIVITY_STATUS_OPEN == m_activity_status;
}

bool Activity::IsActivityStandy()
{
	return ACTIVITY_STATUS_STANDY == m_activity_status;
}

bool Activity::IsActivityClose()
{
	return ACTIVITY_STATUS_CLOSE == m_activity_status;
}

void Activity::OnDayChange()
{
}

void Activity::OnWeekChange()
{
}

void Activity::OnMonthChange()
{
}

void Activity::InitActivityRoleData(int role_id, const ActivityRoleData & role_data)
{
	if (role_id == 0)
	{
		return;
	}

	ActivityPerRoleDataNode data_node;
	data_node.data = role_data;
	m_role_data[role_id] = data_node;
}

void Activity::SaveRoleData(bool is_stop_server)
{
	if (m_is_role_data_change)
	{
		int loop_count = 0;
		while (loop_count++ < 150)
		{
			ActivityRoleDataParam activity_role_data_param;
			activity_role_data_param.count = 0;
			this->GetChangeRoleDataList(activity_role_data_param);

			if (activity_role_data_param.count > 0)
			{
				RMISaveActivityRoleDataBackObjectImpl * impl = new RMISaveActivityRoleDataBackObjectImpl();
				RMIGlobalClient ac;
				ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
				if (ac.SaveActivityRoleDataAsyn(activity_role_data_param, impl))
				{
					if (activity_role_data_param.count < ARRAY_LENGTH(activity_role_data_param.data_list))
					{
						m_is_role_data_change = false;
					}

					this->ClearActivityRoleChangeFlag(activity_role_data_param);
				}
			}
			else
			{
				m_is_role_data_change = false;
			}

			if (is_stop_server == false || m_is_role_data_change == false)		//关服要保存所有数据，否则一次最多保存100条
			{
				break;
			}
		}
	}
}

void Activity::SetRoleData(int role_id, const ActivityRoleData & data)
{
	bool is_new_create = true;

	std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.find(role_id);
	if (iter != m_role_data.end())
	{
		is_new_create = iter->second.is_new_create;
	}

	ActivityPerRoleDataNode & data_node = m_role_data[role_id];
	data_node.data = data;

	data_node.is_dirty = true;
	data_node.is_invalid = false;
	data_node.is_new_create = is_new_create;
	m_is_role_data_change = true;
}

ActivityRoleData * Activity::GetRoleData(int role_id, bool is_auto_create)
{
	ActivityRoleData * ret = NULL;

	std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.find(role_id);
	if (iter == m_role_data.end())
	{
		if (is_auto_create)
		{
			m_is_role_data_change = true;

			ActivityPerRoleDataNode data_node;
			data_node.data.Reset(this->GetActivityType());
			data_node.is_dirty = true;
			data_node.is_new_create = true;
			
			std::pair<std::map<int, ActivityPerRoleDataNode>::iterator, int> ret_iter = m_role_data.insert(std::make_pair(role_id, data_node));
			ret = &ret_iter.first->second.data;
		}
	}
	else
	{
		if (iter->second.is_invalid == false)
		{
			ret = &iter->second.data;
		}
		else
		{
			if (is_auto_create)
			{
				m_is_role_data_change = true;

				ActivityPerRoleDataNode data_node;
				data_node.data.Reset(this->GetActivityType());
				data_node.is_dirty = true;
				data_node.is_invalid = false;
				data_node.is_new_create = iter->second.is_new_create;
				iter->second = data_node;

				ret = &iter->second.data;
			}
		}
	}

	return ret;
}

void Activity::SetRoleDataChange(int role_id)
{
	std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.find(role_id);
	if (iter != m_role_data.end())
	{
		ActivityPerRoleDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		m_is_role_data_change = true;
	}
}

void Activity::ClearAllRoleData()
{
	for (std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.begin(); iter != m_role_data.end(); ++iter)
	{
		ActivityPerRoleDataNode & role_data_node = iter->second;
		role_data_node.is_dirty = true;
		role_data_node.is_invalid = true;
	}

	m_is_role_data_change = true;
}

void Activity::RemoveRoleData(int role_id)
{
	std::map<int, ActivityPerRoleDataNode>::iterator iter = m_role_data.find(role_id);
	if (iter != m_role_data.end())
	{
		ActivityPerRoleDataNode & data_node = iter->second;
		data_node.is_dirty = true;
		data_node.is_invalid = true;
		m_is_role_data_change = true;
	}
}

void Activity::BeginRoleDataForeach()
{
	m_role_data_iter = m_role_data.begin();
}

bool Activity::GetNextRoleData(RetActivityRoleData ARG_OUT & iter, bool is_reset)
{
	if (is_reset)
	{
		m_role_data_iter = m_role_data.begin();
	}

	while (m_role_data_iter != m_role_data.end())
	{
		if (m_role_data_iter->second.is_invalid)
		{
			++m_role_data_iter;
			continue;
		}

		iter.role_id = m_role_data_iter->first;
		iter.data = m_role_data_iter->second.data;
		++m_role_data_iter;

		return true;
	}

	return false;
}

void Activity::SyncActivityStatusHiddenToGameworld(int server_id)
{
	if (!ActivityManager::IsCrossActivity(m_activity_type) || !CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameSyncActivityStatus msg;
	msg.activity_type = m_activity_type;
	msg.activity_status = m_activity_status;
	msg.next_status_switch_time = m_next_status_switch_time;
	if (server_id == 0)
	{
		InternalComm::Instance().SendToGameThroughCross(0, 0, (const char *)&msg, sizeof(msg));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&msg, sizeof(msg));
	}
}

void Activity::GetActivityStausInfo(std::string & out_str)
{
	char temp_str[128] = { 0 };
	SNPRINTF(temp_str, 128, "%d", m_activity_type);
	out_str = temp_str;
	out_str += " ";

	switch (m_activity_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			out_str += "activity_status:close[";
		}
		break;
	case ACTIVITY_STATUS_STANDY:
		{
			out_str += "activity_status:standy[";
		}
		break;
	case ACTIVITY_STATUS_OPEN:
		{
			out_str += "activity_status:open[";
		}
		break;
	default:
		break;
	}
	SNPRINTF(temp_str, 128, "%d", m_activity_status);
	out_str += temp_str;
	out_str += "] next_status_switch_time:";
	SNPRINTF(temp_str, 128, "%d", m_next_status_switch_time);
	out_str += temp_str;
}

void RMISaveActivityRoleDataBackObjectImpl::SaveActivityRoleDataRet(int ret)
{
	if (0 != ret)
	{
		printf("%s Failed ret:%d\n", __FUNCTION__, ret);
	}
}