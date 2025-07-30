#include "npc_gather.h"
#include "scene/scene.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityskydropgift.hpp"
#include "gameworld/other/rolegatherfb/dropgather.hpp"
#include "global/randactivity/randactivityimpl/randactivitytianjianghaoli.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "task/taskmanager.h"

NPCGather::NPCGather() : NPC(Obj::OBJ_TYPE_NPC_GATHERS), m_gather_need_time(0), m_award_count(0), m_already_gathered(false), m_remove_flag(true), m_team_gather(true)
{
	m_delete_time_s = 0;

	m_sub_type = SUB_TYPE_DEF;

	m_param1 = 0;
	m_param2 = 0;
	m_param3 = 0;
	memset(m_award_group, 0, sizeof(m_award_group));

}

NPCGather::~NPCGather()
{

}

int NPCGather::InitNPCGather(int need_time, int award_count, const int * award_group_list)
{
	m_gather_need_time = need_time;
	m_award_count = 0;
	if (award_count > 0 && award_group_list != NULL)
	{
		m_award_count = award_count;
		memcpy(m_award_group, award_group_list, sizeof(m_award_group));
	}

	return 0;
}

void NPCGather::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	NPC::Update(interval, now_second, now_dayid);

	//自删
	if (0 != m_delete_time_s
		&& now_second > m_delete_time_s)
	{
		this->GetScene()->DeleteObj(m_obj_id);
	}
}

bool NPCGather::CanGather(Role * gatherer)
{
	if (NULL == gatherer || m_already_gathered)
	{
		return false;
	}
	if (!m_team_gather && gatherer->InTeam())
	{
		gatherer->GetRoleModuleManager()->NoticeNum(errornum::EN_ACTIVITY_SKY_DROP_GATHER_ERR);
		return false;
	}

	if (gatherer->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().lyjz_gahter_flag == 1 && m_param1 == -100)
	{
		return false;
	}

	return true;
}

bool NPCGather::GatherThis(Role * gatherer)
{
	if (NULL == gatherer || m_already_gathered) return false;

	ServerNPCGatherParam gather_param; gather_param.Reset();
	gather_param.award_item_count = m_award_count;
	memcpy(gather_param.gather_item_group_list, m_award_group, sizeof(gather_param.gather_item_group_list));
	if (!gatherer->OnGiveGatherReward(gather_param))
	{
		return false;
	}

	m_already_gathered = true;

	// 奖励已给，注意不能中途返回
	EventHandler::Instance().OnSuccsGather(gatherer, m_npc_id, m_npc_seq, m_posi);
	DropGather::Instance().OnGather(m_scene, this);
	
	EventHandler::Instance().OnNpcGatherGatherThis(gatherer, m_scene, this);
	if (m_param1 == -100)
	{
		gatherer->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().lyjz_gahter_flag = 1;
	}
	else if(m_param1 == -101)
	{
		gatherer->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().yinyuanyixianqian_flag = 1;
	}
	
	if (m_remove_flag)
	{
		this->RemoveAfterGather();
	}
	Scene* gahter_scene = this->GetScene();
	if (NULL != gahter_scene)
	{
		for (int i = 0; i < gahter_scene->RoleNum(); i++)
		{
			Role * gather_role = gahter_scene->GetRoleByIndex(i);
			if(NULL == gather_role || gather_role->GetUID() == gatherer->GetUID() || gather_role->GetGatherSeqId() != this->GetNpcSeq()) continue;

			gather_role->NoticeNum(errornum::EN_GATHER_FAILED_BY_SLOW);
			gather_role->OnStopGathering(false);
		}
	}

	if (m_param1 <= -100 && !m_remove_flag)
	{
		m_already_gathered = false;//永久为未被采集状态
		if (gahter_scene != NULL)//常驻NPC假的移除
		{
			Protocol::SCVisibleObjLeave leave_msg;
			leave_msg.obj_id = this->GetId();
			leave_msg.reserved = 0;
			EngineAdapter::Instance().NetSend(gatherer->GetNetId(), &leave_msg, sizeof(leave_msg));

			static Protocol::SCSceneObjList msg;
			msg.notify_reason = Protocol::SCSceneObjList::NOTIFY_REASON_DELETE;
			msg.obj_num = 1;
			msg.obj_list[0].pos_x = this->GetPos().x;
			msg.obj_list[0].pos_y = this->GetPos().y;
			msg.obj_list[0].obj_id = this->GetId();
			msg.obj_list[0].obj_type = this->GetObjType();

			int send_len = sizeof(msg) - (Protocol::SCSceneObjList::MAX_OBJ_NUM - msg.obj_num) * sizeof(msg.obj_list[0]);
			EngineAdapter::Instance().NetSend(gatherer->GetNetId(), (const char *)&msg, send_len);
		}
	}
	
	return true;
}

void NPCGather::SetRemoveAfterGatherFlag(bool remove_flag)
{
	m_remove_flag = remove_flag;
}

void NPCGather::SetParam(const int * p1, const int * p2, const int * p3)
{
	if (p1 != NULL)
	{
		m_param1 = *p1;
	}

	if (p2 != NULL)
	{
		m_param2 = *p2;
	}

	if (p3 != NULL)
	{
		m_param3 = *p3;
	}
}

void NPCGather::GetParam(int *ARG_OUT p1, int *ARG_OUT p2, int *ARG_OUT p3)
{
	if (p1 != NULL)
	{
		*p1 = m_param1;
	}

	if (p2 != NULL)
	{
		*p2 = m_param2;
	}

	if (p3 != NULL)
	{
		*p3 = m_param3;
	}
}

bool NPCGather::CanSee(Role * role)
{
	if (m_param1 == -100)//灵药救主常驻采集物
	{
		if (role->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().lyjz_gahter_flag == 1)
		{
			return false;
		}
	}
	if (m_param1 == -101)//姻缘一线牵常驻采集物
	{
		if (role->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().yinyuanyixianqian_flag == 1)
		{
			return false;
		}
	}
	return true;
}

void NPCGather::RemoveAfterGather()
{
	m_scene->DeleteObj(m_obj_id);
}