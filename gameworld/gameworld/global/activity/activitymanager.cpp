#include <memory>

#include "gamelog.h"
#include "activity.hpp"
#include "activitymanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/localconfig.hpp"

#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activityimpl/activityworldboss2.hpp"
#include "global/activity/activityimpl/activityworldboss3.hpp"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "global/activity/activityimpl/activitymazeracer.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "global/activity/activityimpl/activityguildhonorbattle.hpp"
#include "global/activity/activityimpl/activityguildanswer.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "global/activity/activityimpl/activityhundredghost.hpp"
#include "global/activity/activityimpl/activitysilkroads.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "global/activity/activityimpl/activityqingyuanshenghui.hpp"

ActivityManager::ActivityManager()
{
	UNSTD_STATIC_CHECK(19 == ACTIVITY_TYPE_MAX - ACTIVITY_TYPE_INVALID);

	memset(m_activity_list, 0, sizeof(m_activity_list));

	m_activity_list[ACTIVITY_TYPE_WORLD_BOSS] = new ActivityWorldBoss(this);
	m_activity_list[ACTIVITY_TYPE_TEAM_FIGHT] = new ActivityTeamFight(this);
	m_activity_list[ACTIVITY_TYPE_MAZE_RACER] = new ActivityMazeRacer(this);
	m_activity_list[ACTIVITY_TYPE_BRAVE_GROUND] = new ActivityBraveGround(this);
	m_activity_list[ACTIVITY_TYPE_GUILD_FIGHT] = new ActivityGuildFight(this);
	m_activity_list[ACTIVITY_TYPE_CHIEF_ELECTION] = new ActivityChiefElection(this);
	m_activity_list[ACTIVITY_TYPE_CHIEF_PEAK_BATTLE] = new ActivityChiefPeakBattle(this);
	m_activity_list[ACTIVITY_TYPE_GUILD_HONOR_BATTLE] = new ActivityGuildHonorBattle(this);
	m_activity_list[ACTIVITY_TYPE_GUILD_ANSWER] = new ActivityGuildAnswer(this);
	m_activity_list[ACTIVITY_TYPE_WORLD_BOSS_2] = new ActivityWorldBoss2(this);
	m_activity_list[ACTIVITY_TYPE_ONLY_FIGHT] = new ActivityOnlyFight(this);
	m_activity_list[ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG] = new ActivityHuanJieZhanChang(this);
	m_activity_list[ACTIVITY_TYPE_RELIC_LOOTING] = new ActivityRelicLooting(this);
	m_activity_list[ACTIVITY_TYPE_HUNDRED_GHOST] = new ActivityHundredGhost(this);
	m_activity_list[ACTIVITY_TYPE_SILK_ROADS] = new ActivitySilkRoads(this);
	m_activity_list[ACTIVITY_TYPE_WORLD_TEAM_ARENA] = new ActivityWorldTeamArena(this);
	m_activity_list[ACTIVITY_TYPE_QING_YUAN_SHENG_HUI] = new ActivityQingYuanShengHui(this);
	m_activity_list[ACTIVITY_TYPE_WORLD_BOSS_3] = new ActivityWorldBoss3(this);
}

ActivityManager::~ActivityManager()
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_activity_list[i])
		{
			delete m_activity_list[i];
		}
	}

	memset(m_activity_list, 0, sizeof(m_activity_list));
}

void ActivityManager::SaveRoleData()
{
	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->SaveRoleData(true);
		}
	}
}


ActivityManager & ActivityManager::Instance()
{
	static ActivityManager am;
	return am;
}

bool ActivityManager::IsCrossActivity(int activity_type)
{
	bool is_cross = false;
	switch (activity_type)
	{
		case ACTIVITY_TYPE_TEAM_FIGHT:
		case ACTIVITY_TYPE_MAZE_RACER:
		case ACTIVITY_TYPE_BRAVE_GROUND:
		case ACTIVITY_TYPE_GUILD_FIGHT:
		case ACTIVITY_TYPE_CHIEF_ELECTION:
		case ACTIVITY_TYPE_CHIEF_PEAK_BATTLE:
		case ACTIVITY_TYPE_GUILD_HONOR_BATTLE:
		case ACTIVITY_TYPE_GUILD_ANSWER:
		case ACTIVITY_TYPE_ONLY_FIGHT:
		case ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG:
		case ACTIVITY_TYPE_WORLD_BOSS:
		case ACTIVITY_TYPE_WORLD_BOSS_2:
		case ACTIVITY_TYPE_WORLD_BOSS_3:
		case ACTIVITY_TYPE_RELIC_LOOTING:
		//case ACTIVITY_TYPE_HUNDRED_GHOST:
		case ACTIVITY_TYPE_SILK_ROADS:
		case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
		//case ACTIVITY_TYPE_QING_YUAN_SHENG_HUI: TODO gfp
		{
			is_cross = true;
		}
		break;
	}
	UNSTD_STATIC_CHECK(19 == ACTIVITY_TYPE_MAX);

	return is_cross;
}
 
void ActivityManager::Update(unsigned long interval, time_t now_second)
{
	if (!WorldStatus::Instance().IsLoadFinish())
	{
		return;
	}

	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (this->IsCrossActivity(i) && !CrossConfig::Instance().IsHiddenServer())
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->Update(interval, now_second);
		}
	}
}

void ActivityManager::OnMonthChange()
{
	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (this->IsCrossActivity(i) && !CrossConfig::Instance().IsHiddenServer())
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnMonthChange();
		}
	}
}

void ActivityManager::OnWeekChange()
{
	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (this->IsCrossActivity(i) && !CrossConfig::Instance().IsHiddenServer())
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnWeekChange();
		}
	}
}

void ActivityManager::OnDayChange()
{
	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (this->IsCrossActivity(i) && !CrossConfig::Instance().IsHiddenServer())
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnDayChange();
		}
	}
}

void ActivityManager::InitActivity(int activity_type, const ActivityData & data)
{
	Activity * activity = this->ForceGetActivity(activity_type);

	if (activity != NULL && activity->IsNeedSave())
	{
		activity->Init(data);
	}
}

void ActivityManager::GetChangedActivityData(ActivityDataListParam * list_param)
{
	for (int i = 0; i < ACTIVITY_TYPE_MAX && list_param->count < ActivityDataListParam::LIST_COUNT; ++i)
	{
		if (NULL != m_activity_list[i] && m_activity_list[i]->IsDirty() && m_activity_list[i]->IsNeedSave())
		{
			list_param->data_list[list_param->count].Reset(i);
			list_param->data_list[list_param->count].activity_type = i;
			m_activity_list[i]->GetInitParam(list_param->data_list[list_param->count].activity_data);
			list_param->count++;
		}
	}
}

void ActivityManager::GetActivityData(ActivityDataListParam * list_param)
{
	for (int i = 0; i < ACTIVITY_TYPE_MAX && list_param->count < ActivityDataListParam::LIST_COUNT; ++i)
	{
		if (NULL != m_activity_list[i] && m_activity_list[i]->IsNeedSave())
		{
			list_param->data_list[list_param->count].Reset(i);
			list_param->data_list[list_param->count].activity_type = i;
			m_activity_list[i]->GetInitParam(list_param->data_list[list_param->count].activity_data);
			list_param->count++;
		}
	}
}

void ActivityManager::ClearDirtyMark()
{
	for (int i = 0; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->ClearDirtyMark();
		}
	}
}

void ActivityManager::InitActivityRoleData(const ActivityRoleDataParam::DBActivityRoleDataItem & role_data_item)
{
	Activity * activity = ForceGetActivity(role_data_item.activity_type);
	if (NULL != activity)
	{
		activity->InitActivityRoleData(role_data_item.role_id, role_data_item.data);
	}
}

bool ActivityManager::IsActivityStandy(int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return false;
	if (NULL == m_activity_list[activity_type]) return false;

	return (ACTIVITY_STATUS_STANDY == m_activity_list[activity_type]->GetActivityStatus());
}

bool ActivityManager::IsActivityOpen(int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return false;
	if (NULL == m_activity_list[activity_type]) return false;

	return (ACTIVITY_STATUS_OPEN == m_activity_list[activity_type]->GetActivityStatus());
}

bool ActivityManager::IsActivityClose(int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return true;
	if (NULL == m_activity_list[activity_type]) return true;

	return (ACTIVITY_STATUS_CLOSE == m_activity_list[activity_type]->GetActivityStatus());
}

void ActivityManager::OnUserLogin(Role *role)
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnUserLogin(role);
		}
	}
}

void ActivityManager::OnUserLogout(Role *role)
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnUserLogout(role);
		}
	}
}

void ActivityManager::OnUserLoginOriginGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (!this->IsCrossActivity(i))
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnUserLoginOriginGame(role_id);
		}
	}
}

void ActivityManager::OnConnectCrossServerSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SyncAllActivityStatusHiddenToGameworld();
	}

	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnConnectCrossServerSucc();
		}
	}
}

void ActivityManager::OnGameServerConnect(std::set<int> server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (!this->IsCrossActivity(i))
		{
			continue;
		}

		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnGameServerConnect(server_id);
		}
	}

	if (!server_id.empty())
	{
		this->SyncAllActivityStatusHiddenToGameworld(*server_id.begin());
	}	
}

void ActivityManager::OnUserEnterScene(Role *role)
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnUserEnterScene(role);
		}
	}
}

void ActivityManager::OnUserLeaveScene(Role *role)
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnUserLeaveScene(role);
		}
	}
}

void ActivityManager::OnQueryRoomStatusReq(Role *role, int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return;

	Activity *activity = m_activity_list[activity_type];
	if (NULL != activity)
	{
		activity->OnQueryRoomStatusReq(role);
	}
}

void ActivityManager::OnForceToNextState(int activity_type, unsigned int * next_status_switch_timestamp)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) 
	{
		gamelog::g_log_activity.printf(LL_ERROR, "%s activity_type[%d] is error! ", __FUNCTION__, activity_type);
		return;
	}
	if (NULL != m_activity_list[activity_type])
	{
		if (IsCrossActivity(activity_type))
		{
			if (CrossConfig::Instance().IsHiddenServer())
			{
				m_activity_list[activity_type]->ForceToNextState(next_status_switch_timestamp);
			}
			else
			{
				crossgameprotocal::HiddenGameActivityForceToNextState protocol;
				protocol.activity_type = activity_type;
				if (NULL != next_status_switch_timestamp)
				{
					protocol.next_status_switch_timestamp = *next_status_switch_timestamp;
				}
				else
				{
					protocol.next_status_switch_timestamp = 0;
				}
				InternalComm::Instance().SendToHiddenThroughCross((const char*)&protocol, sizeof(protocol));
			}
		}
		else
		{
			m_activity_list[activity_type]->ForceToNextState(next_status_switch_timestamp);
		}
	}
}

Activity * ActivityManager::ForceGetActivity(int activity_type)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX) return NULL;

	return m_activity_list[activity_type];
}

void ActivityManager::OnBattleServerDisconnect(int battle_server_id)
{
	for (int i = ACTIVITY_TYPE_INVALID; i < ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->OnBattleServerDisconnect(battle_server_id);
		}
	}
}

void ActivityManager::SyncAllActivityStatusHiddenToGameworld(int server_id)
{
	for (int i = ACTIVITY_TYPE_INVALID + 1; i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_activity_list[i])
		{
			m_activity_list[i]->SyncActivityStatusHiddenToGameworld(server_id);
		}
	}
}


