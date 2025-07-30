#include "MoveAssist.hpp"

#include "gamecommon.h"
#include "engineadapter.h"
#include "global/moveassistmanager/moveassistmanager.hpp"

#include "scene/scene.h"

#include "obj/character/character.h"
#include "obj/character/role.h"

static const int MOVE_ASSIXT_REQ_WAY_POINT_INTERVAL = 10;		//辅助移动 请求时间间隔
static const int MOVE_ASSIXT_MOVE_INTERVAL = 5;					//辅助移动 移动 重试时间间隔

void MoveAssist::Update(unsigned long interval, time_t now_second)
{
	switch (m_move_assixt_status)
	{
	case MOVE_ASSIST_STATUS_TYPE_NONE:
		{
			return;
		}
		break;
	case MOVE_ASSIST_STATUS_TYPE_WAIT_WAY_RET:
		{
			//第一次 或上次请求过长时间未返回
			if (now_second > m_next_move_assist_req_time)
			{
				this->SendMoveAssisReq();
				m_next_move_assist_req_time = now_second + MOVE_ASSIXT_REQ_WAY_POINT_INTERVAL;
			}
		}
		break;
	case MOVE_ASSIST_STATUS_TYPE_WAY_MOVE:
		{
			UNSTD_STATIC_CHECK(2 == Character::ACTION_COUNT);
			switch (m_this_cha->GetLogicResult())
			{
			//未移动 / 抵达拐点
			case Character::ACTION_IDLE:
				{
					if (0 > m_next_way_point_index
						|| m_way_point_count <= m_next_way_point_index
						|| MOVE_ASSIXT_WAY_POINT_NUM <= m_next_way_point_index)
					{//路径点错误 清空
						this->ReSetDestination();
						return;
					}

					Posi &target_pos = m_way_point_list[m_next_way_point_index];
					Posi now_pos = m_this_cha->GetPos();
					Posi deltaP = target_pos - now_pos;

					//校验坐标
					if (!deltaP.IsLongerThan(1))
					{//到达拐点
						m_next_way_point_index++;

						if (0 > m_next_way_point_index
							|| m_next_way_point_index >= m_way_point_count
							|| MOVE_ASSIXT_WAY_POINT_NUM <= m_next_way_point_index)
						{//已经到达目的地  清空
							this->ReSetDestination();
							return;
						}
					}
					//差距过大重新移动
					else if (m_next_move_assist_move_time >= now_second)
					{//防止频繁移动
						return;
					}
					m_next_move_assist_move_time = now_second + MOVE_ASSIXT_MOVE_INTERVAL;

					m_this_cha->SetMoveLogic(m_way_point_list[m_next_way_point_index]);

					//m_this_cha->Move(m_way_point_list[m_next_way_point_index].x, m_way_point_list[m_next_way_point_index].y, dir, distance, 0, false, true);
				}
				break;
			case Character::ACTION_RUN:
				{
					return;
				}
				break;
			default:
				return;
			}

		}
		break;
	case MOVE_ASSIST_STATUS_TYPE_WAIT_MOVE:
		{
			return;
		}
		break;
	default:
		return;
	}

	return;
	//测试
	Protocol::CSMoveAssistRet ret_info;
	this->OnMoveAssistRet(ret_info);
}

void MoveAssist::SetDestination(Posi target_pos, int assist_type)
{
	this->ReSetDestination();
	
	m_move_assixt_status = MOVE_ASSIST_STATUS_TYPE_WAIT_WAY_RET;
	m_assist_type = assist_type;

	m_target_posi = target_pos;

}

void MoveAssist::ReSetDestination()
{
	m_move_assixt_status = MOVE_ASSIST_STATUS_TYPE_NONE;

	m_assist_type = MOVE_ASSIST_REQ_TYPE_NONE;

	m_target_posi = Posi(-1, -1);

	m_way_point_count = 0;

	m_next_way_point_index = 0;

	m_next_move_assist_req_time = 0;

	m_next_move_assist_move_time = 0;
}

void MoveAssist::MovePause()
{
	if (MOVE_ASSIST_STATUS_TYPE_WAY_MOVE == m_move_assixt_status)
	{
		m_move_assixt_status = MOVE_ASSIST_STATUS_TYPE_WAIT_MOVE;
	}
}

void MoveAssist::MoveContinue()
{
	if (MOVE_ASSIST_STATUS_TYPE_WAIT_MOVE == m_move_assixt_status)
	{
		m_move_assixt_status = MOVE_ASSIST_STATUS_TYPE_WAY_MOVE;
	}
}

void MoveAssist::OnMoveToOther()
{
	UNSTD_STATIC_CHECK(4 == MoveAssist::MOVE_ASSIST_STATUS_TYPE_MAX);
	if (MoveAssist::MOVE_ASSIST_STATUS_TYPE_NONE != m_move_assixt_status)
	{//非移动辅助的移动操作 //清空移动辅助
		this->ReSetDestination();
	}
}

void MoveAssist::OnMoveAssistRet(Protocol::CSMoveAssistRet & ret_info)
{
	//校验状态
	if (MOVE_ASSIST_STATUS_TYPE_WAIT_WAY_RET != m_move_assixt_status)
	{
		return;
	}

	if (m_assist_type != MOVE_ASSIST_REQ_TYPE_AROND)
	{
		if (m_target_posi.x == -1
			|| m_target_posi.y == -1)
		{
			return;
		}
	}

	if (ret_info.move_assist_id != m_move_assist_id)
	{
		return;
	}

	Scene * scene = m_this_cha->GetScene();
	if (NULL == scene)
	{
		return;
	}
	if (ret_info.scene_id != scene->GetSceneID())
	{
		return;
	}
	if (ret_info.target_pos_x != m_target_posi.x
		|| ret_info.target_pos_y != m_target_posi.y)
	{//目的地改变 舍弃
		return;
	}
	Posi cur_pos(ret_info.cur_pos_x, ret_info.cur_pos_y);
	if ((m_this_cha->GetPos() - cur_pos).IsLongerThan(5))
	{//位置改变  舍弃
		return;
	}

	UNSTD_STATIC_CHECK(Protocol::CSMoveAssistRet::WayPoint::WAY_POINT_MAX_NUM == MOVE_ASSIXT_WAY_POINT_NUM)
		static Posi way_point_list[MOVE_ASSIXT_WAY_POINT_NUM];
	int way_point_count = 0;

	for (; way_point_count < ret_info.way_point_count
		&& way_point_count < MOVE_ASSIXT_WAY_POINT_NUM
		; way_point_count++)
	{
		//距离过长
		//if ( < m_this_cha->GetPosf().Dist(last_check_point))
		//{
		//	return;
		//}
		if (0 > ret_info.way_point_list[way_point_count].pos_x
			|| 0 > ret_info.way_point_list[way_point_count].pos_y)
		{
			return;
		}
		way_point_list[way_point_count].x = ret_info.way_point_list[way_point_count].pos_x;
		way_point_list[way_point_count].y = ret_info.way_point_list[way_point_count].pos_y;
	}

	this->SetWayPointList(ret_info.way_point_count, way_point_list);
}

void MoveAssist::SendMoveAssisReq()
{
	Scene * scene = m_this_cha->GetScene();
	if (NULL == scene)
	{
		return;
	}

	int role_count = scene->RoleNum();
	if (0 >= role_count)
	{
		return;
	}

	for (int i = 0; i < 3; i++)
	{
		Role* role = scene->GetRoleByIndex(rand() % role_count);
		if (NULL != role)
		{
			unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
			if (!MoveAssistManager::Instance().CheckModifyRoleMoveAssistCD(role->GetUID(), now_second))
			{
				continue;
			}
			// 发送移动协助请求
			static Protocol::SCMoveAssistReq msg;
			msg.move_assist_id = m_move_assist_id;
			msg.scene_id = scene->GetSceneID();
			msg.obj_id = m_this_cha->GetId();
			msg.assist_type = m_assist_type;
			msg.speed = (short)m_this_cha->GetSpeedRate();
			msg.cur_pos_x = m_this_cha->GetPos().x;
			msg.cur_pos_y = m_this_cha->GetPos().y;
			msg.target_pos_x = (short)m_target_posi.x;
			msg.target_pos_y = (short)m_target_posi.y;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));

			break;
		}
	}
}

void MoveAssist::SetWayPointList(int way_point_num, const Posi * way_point_list)
{
	this->ReSetDestination();

	if (0 >= way_point_num
		|| NULL == way_point_list)
	{
		return;
	}
	m_target_posi = way_point_list[0];

	m_move_assixt_status = MOVE_ASSIST_STATUS_TYPE_WAY_MOVE;

	m_way_point_count = way_point_num;
	m_next_way_point_index = 0;

	for (int i = 0; i < way_point_num && i < MOVE_ASSIXT_WAY_POINT_NUM; ++i)
	{
		m_way_point_list[i] = way_point_list[i];
	}
}

MoveAssist::MoveAssist(Character * self_cha, unsigned int move_assist_id)
{
	m_this_cha = self_cha;
	m_move_assist_id = move_assist_id;

	memset(m_way_point_list, -1, sizeof(m_way_point_list));

	//寻路结束需要清空的放里面
	this->ReSetDestination();
}

MoveAssist::~MoveAssist()
{
}
