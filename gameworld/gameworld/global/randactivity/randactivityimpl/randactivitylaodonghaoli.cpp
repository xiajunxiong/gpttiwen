#include "randactivitylaodonghaoli.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitylaodonghaoliconfig.hpp"
#include "world.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "gameworld/gamelog.h"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "obj/character/npc_gather.h"
#include "servercommon/string/gameworldstr.h"
#include "engineadapter.h"
#include "global/usercache/usercache.hpp"

#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitylaodonghaoli.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

RandActivityLaoDongHaoLi::RandActivityLaoDongHaoLi(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI)
{
	is_start_time = false;
}

RandActivityLaoDongHaoLi::~RandActivityLaoDongHaoLi()
{
}

void RandActivityLaoDongHaoLi::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);
	memcpy(&m_param, arg_data.rand_activity_data.data, sizeof(m_param));
}

void RandActivityLaoDongHaoLi::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(arg_data.rand_activity_data.data));
	memcpy(arg_data.rand_activity_data.data, &m_param, sizeof(m_param));
}

void RandActivityLaoDongHaoLi::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		this->SetBoxTime();
	}

	if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		this->ClearBox();			//活动关闭，回收所有宝箱
		m_scene_box_map.clear();
		m_param.Reset();
		is_start_time = false;
		this->SendInfo();
	}
}

void RandActivityLaoDongHaoLi::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (m_param.refresh_box_time >= m_param.close_box_time) return;

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && m_param.refresh_box_time <= now_second && m_param.close_box_time > now_second && !is_start_time)
	{
		this->RefreshBox();
		is_start_time = true;
		this->SendInfo();
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && m_param.next_refresh_timestamp <= now_second && is_start_time)
	{
		this->RefreshBox();
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && m_param.close_box_time <= now_second && is_start_time)
	{
		this->ClearBox();			//回收所有宝箱
		m_scene_box_map.clear();
		m_param.next_refresh_timestamp = 0;
		is_start_time = false;
		m_param.notice_tamp = 0;
		// 跑马灯
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_lao_dong_hao_li_end_broadcast);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE));
		}

		this->SendInfo();
	}

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN && m_param.notice_tamp > 0 && m_param.notice_tamp <= now_second && is_start_time)
	{
		// 跑马灯
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_lao_dong_hao_li_broadcast);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE));
		}
		m_param.notice_tamp = now_second + 600;//十分钟提示一次
	}
}

void RandActivityLaoDongHaoLi::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivity::OnDayChange(old_dayid, now_dayid);

	this->SetBoxTime();
}

bool RandActivityLaoDongHaoLi::IsReceiveBox(Role* role, NPCGather* gather)
{
	if (NULL == role || NULL == gather)return false;

	int m_scene_id = role->GetScene()->GetSceneID();

	std::map<int, std::vector<ObjID> >::iterator it = m_scene_box_map.find(m_scene_id);
	if (it != m_scene_box_map.end())
	{
		for (std::vector<ObjID>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
		{
			if (*vit == gather->GetId())return true;
		}
	}
	return false;
}

bool RandActivityLaoDongHaoLi::ReceiveBoxOp(Role* role, NPCGather* gather)
{
	const RandActivityLaoDongHaoLiConfig * config = static_cast<const RandActivityLaoDongHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI));
	if (NULL == config)
	{
		return false;
	}

	const RALaoDongHaoLiOtherCfg & other_cfg = config->GetOtherCfg();
	const RALaoDongHaoLiRewardCfg * reward_cfg = config->GetRewardCfg();
	if (NULL == reward_cfg)
	{
		return false;
	}

	RoleActivityLaoDongHaoLi * role_activity = static_cast<RoleActivityLaoDongHaoLi *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity (RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI));
	if (NULL == role_activity)
	{
		return false;
	}

	int receive_count = role_activity->GetGtherCount();
	if (receive_count >= other_cfg.reward_num)
	{
		role->NoticeNum(errornum::EN_RAND_ACTIVITY_MAX_RECEIVE_NUM);
		return false;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(reward_cfg->reward_item.item_id, reward_cfg->reward_item.num))
	{
		role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	Scene* scene = role->GetScene();
	if (NULL == scene)
	{
		return false;
	}

	//删除采集物
	bool is_delete = false;
	std::map<int, std::vector<ObjID> >::iterator it = m_scene_box_map.find(scene->GetSceneID());
	if (it != m_scene_box_map.end())
	{
		for (std::vector<ObjID>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
		{
			if (*vit == gather->GetId())
			{
				Obj* box_obj = scene->GetObj(*vit);
				if (NULL == box_obj)return false;
				if (box_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS)return false;
				if (!scene->DeleteObj(*vit))
				{
					//gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityLaoDongHaoLi::ClearBox() activity_type %d, clear_err[scene_id:%d , objid:%d]",
					//	RandActivityManager::Instance().ForceGetRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI)->GetActivityType(), it->first, *vit);
					return false;
				}

				it->second.erase(vit);
				is_delete = true;
				break;
			}
		}
	}

	if (is_delete)
	{
		//给奖励物品
		role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->reward_item, PUT_REASON_RA_TIAN_JIANG_HAO_LI);
		role_activity->RATianJiangHaoLiAddReceiveCount();

		//传闻
		if (1 == reward_cfg->hearsay)
		{
			const ItemBase * item_base = ItemPool::Instance()->GetItem(reward_cfg->reward_item.item_id);
			if (NULL != item_base)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_tian_jiang_hao_li_hearsay,
								role->GetRoleModuleManager()->GetRole()->GetName(), item_base->GetItemId(), RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
					{
						World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool RandActivityLaoDongHaoLi::IsRefreshBoxTime()
{
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (m_param.refresh_box_time <= now_time && now_time < m_param.close_box_time)
	{
		return true;
	}
	return false;
}

void RandActivityLaoDongHaoLi::GmSetTime(int begin_time, int end_time)
{
	//活动开启状态下使用，因为活动开启会设置时间为表里的时间
	if (begin_time >= end_time) return;

	m_param.refresh_box_time = this->ForHourMinuteGetTimestamp((int)begin_time);
	m_param.close_box_time = this->ForHourMinuteGetTimestamp(end_time);
}

void RandActivityLaoDongHaoLi::SetBoxTime()
{
	const RandActivityLaoDongHaoLiConfig * config = static_cast<const RandActivityLaoDongHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI));
	if (NULL == config)
	{
		return;
	}

	const RALaoDongHaoLiOtherCfg & other_cfg = config->GetOtherCfg();

	if (other_cfg.start_time >= other_cfg.end_time)return;

	m_param.refresh_box_time = this->ForHourMinuteGetTimestamp(other_cfg.start_time);
	m_param.close_box_time = this->ForHourMinuteGetTimestamp(other_cfg.end_time);
	m_param.notice_tamp = m_param.refresh_box_time;
}

void RandActivityLaoDongHaoLi::RefreshBox()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	const RandActivityLaoDongHaoLiConfig * config = static_cast<const RandActivityLaoDongHaoLiConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI));
	if (NULL == config)
	{
		return;
	}

	const std::vector<RALaoDongHaoLiPointCfg> & point_cfg = config->GetAllPointCfg();
	const RALaoDongHaoLiOtherCfg & other_cfg = config->GetOtherCfg();


	SceneManager * scene_manager = World::GetInstWorld()->GetSceneManager();
	if (scene_manager != NULL)
	{
		for (int i = 0; i < other_cfg.map_case_num; ++i)
		{
			int point_num = point_cfg.size();

			if(point_num > 0)
			{
				int index = rand() % point_num;
				const RALaoDongHaoLiPointCfg *node_cfg = &point_cfg[index];
				std::map<int, std::vector<ObjID> >::iterator it = m_scene_box_map.find(node_cfg->scene_id);
				if (it != m_scene_box_map.end() && (int)it->second.size() >= other_cfg.map_case_num)break;		//某一场景超过宝箱数量上限

				Scene* scene = scene_manager->GetSceneById(node_cfg->scene_id, COMMON_SCENE_KEY);
				if (scene != NULL)
				{
					NPCGather* gather = NpcManager::Instance().CreateNPCGather(node_cfg->npc_gather_id, scene, scene->GetARandPosi());
					if (gather == NULL)continue;

					if (it == m_scene_box_map.end())
					{
						m_scene_box_map[node_cfg->scene_id].push_back(gather->GetId());
					}
					else
					{
						it->second.push_back(gather->GetId());
					}
				}
			}
		}
	}
	m_param.next_refresh_timestamp = EngineAdapter::Instance().Time() + other_cfg.refresh_time;	//把第一个的时间作为刷新间隔时间
}

void RandActivityLaoDongHaoLi::ClearBox()
{
	SceneManager* scene_manager = World::GetInstWorld()->GetSceneManager();
	if (NULL == scene_manager)return;

	for (std::map<int, std::vector<ObjID> >::iterator it = m_scene_box_map.begin(); it != m_scene_box_map.end(); ++it)
	{
		Scene* scene = scene_manager->GetSceneById(it->first, COMMON_SCENE_KEY);
		if (NULL != scene)
		{
			for (std::vector<ObjID>::iterator obj_it = it->second.begin(); obj_it != it->second.end(); ++obj_it)
			{
				Obj* box_obj = scene->GetObj(*obj_it);
				if (NULL == box_obj)continue;

				if (box_obj->GetObjType() != Obj::OBJ_TYPE_NPC_GATHERS)continue;

				if (!scene->DeleteObj(*obj_it))
				{
					//gamelog::g_log_rand_activity.printf(LL_INFO, "RandActivityLaoDongHaoLi::ClearBox() activity_type %d, clear_err[scene_id:%d , objid:%d]",
					//	RandActivityManager::Instance().ForceGetRandActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI)->GetActivityType(), it->first, *obj_it);
				}
			}
		}
	}
}

void RandActivityLaoDongHaoLi::SendInfo()
{
	std::vector<int> online_user_list;
	int count = UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
	for (int i = 0; i < count; i++)
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
		if (NULL == role) continue;

		RoleActivityLaoDongHaoLi * role_activity = static_cast<RoleActivityLaoDongHaoLi *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI));
		if (NULL == role_activity)
		{
			continue;
		}

		role_activity->SendRATianJiangHaoLiInfo();
	}
}

/**
* 输入格式：1100
* 输出时间戳
*/
time_t RandActivityLaoDongHaoLi::ForHourMinuteGetTimestamp(int hour_minute)
{
	int t_hour = hour_minute / 100;
	int t_minute = hour_minute % 100;

	if (0 > t_hour || t_hour >= 24) return 0;
	if (0 > t_minute || t_minute >= 60) return 0;

	time_t now_time = EngineAdapter::Instance().Time();
	int next_day_zeor_time = EngineAdapter::Instance().NextDayInterval(0, 0, 0);

	int cut_time = ((24 - t_hour) * 3600) - (t_minute * 60);

	time_t r_timestamp = now_time + next_day_zeor_time - cut_time;

	return r_timestamp;
}
