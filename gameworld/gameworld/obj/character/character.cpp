#include "character.h"
#include "scene/scene.h"
#include "scene/map.h"
#include "globalconfig/globalconfig.h"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "protocol/msgscene.h"
#include "other/event/eventhandler.hpp"
#include "protocol/msgrole.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"

#include "global/moveassistmanager/moveassistmanager.hpp"
#include "obj/MoveAssist/MoveAssist.hpp"
#include "gamecommon.h"

#include "gamelog.h"
static const int TEST_UID = 84956060; //84956060;// 83927485;

Character::Character(ObjType type) : Obj(type), m_last_check_valid_time(0u), m_last_check_pos(0, 0), m_speed_rate(0), m_speed(0),
	m_action_status(ACTION_IDLE), m_logic_result(LOGIC_NONE), m_old_dir(0.0f), m_old_cos(0.0f), m_old_sin(0.0f)
{
	m_move_assist = NULL;
}

Character::Character(ObjType type, const GameName _name) : Obj(type, INVALID_OBJ_ID, _name), m_last_check_valid_time(0u), 
	m_last_check_pos(0, 0), m_speed_rate(0), m_speed(0), m_action_status(ACTION_IDLE), m_logic_result(LOGIC_NONE), m_old_dir(0.0f),
	m_old_cos(0.0f), m_old_sin(0.0f)
{
	m_move_assist = NULL;
}

Character::~Character()
{
	if (NULL != m_move_assist)
	{
		delete m_move_assist;
	}
}

void Character::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	switch (m_action_status)
	{
	case ACTION_IDLE:
		break;

	case ACTION_RUN:
		MoveLogic(interval);
		break;

	default:
		break;
	}

	if (NULL != m_move_assist)
	{
		m_move_assist->Update(interval, now_second);
	}
}


int Character::SetSpeedRate(int rate)
{
	m_speed_rate = rate;
	return m_speed_rate;
}

int Character::GetSpeed() const
{
	return m_speed;
}

int Character::SetSpeed(int speed)
{
	int old = m_speed;
	m_speed = speed;

	if (NULL != m_scene && old != m_speed)
	{
		Protocol::SCOjbSpeedChangeNotice p;
		p.obj_id = this->GetId();
		p.move_speed = m_speed;
		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&p, sizeof(p));
	}

	return m_speed;
}

bool Character::Move(Axis _x, Axis _y, Dir _dir, Scalar distance, short height, bool check_valid /*= false*/, bool is_move_assist /*= false*/)
{
	if (NULL == m_scene) return false;

/*
#ifdef _DEBUG
	Role* pRole = NULL;
	if (Obj::OBJ_TYPE_ROLE == this->GetObjType())
	{
		Role* role = (Role*)this;
		if (role->GetUID() == TEST_UID)
		{
			pRole = role;
		}
	}
#endif
*/

	//SendPosiInfo();

	// 合法性验证
	if ((check_valid && !m_scene->GetMap()->CheckValidate(m_obj_type, _x, _y)) || (fabs(_dir - Dir_S) > 1e-6 && fabs(_dir) > Dir_Max) /* _dir >= Dir_Count*/
		|| (OBJ_TYPE_ROLE == m_obj_type && !m_scene->GetMap()->IsInsideMap(_x, _y)))
	{

/*
#ifdef _DEBUG
		if (NULL != pRole)
		{
			gamelog::g_log_world.printf(LL_ERROR, "Move Fail_1, role[%d,%s] cur_pos[%d,%d] dir[%.2f] distance[%.2f]\n", pRole->GetUID(), pRole->GetName(), _x, _y, _dir, distance);
		}
#endif
*/
		return false;
	}

	//printf("Move:  _x:%d _y:%d  _dir:%.f\n", _x, _y, distance);

	// 如果是客户端发来的消息，则需要验证与当前位置的距离是否合法
	bool is_check_valid = GLOBALCONFIG->GetOtherConfig_IsCheckValid();
	if (check_valid && is_check_valid)
	{
		const static unsigned int max_move_speed = GLOBALCONFIG->GetOtherConfig_MaxMoveSpeed();
		const static Axis move_check_length = (Axis)GLOBALCONFIG->GetOtherConfig_MoveCheck_Length();

		Posi to_pos(_x, _y);

		{
			// 检查当前位置偏差，允许在一定范围内
			Posi delta_pos = m_posi - to_pos;
			//printf("delta_pos=[%d,%d] my_pos[%d,%d] cli_pos[%d,%d]  \n", delta_pos.x, delta_pos.y, m_posi.x, m_posi.y, 
			//	to_pos.x, to_pos.y);

			if (delta_pos.x > move_check_length || delta_pos.x < -move_check_length
				|| delta_pos.y > move_check_length || delta_pos.y < -move_check_length)
			{
				// 又是个挂
				gamelog::g_log_world.printf(LL_WARNING, "cur:%d,%d  to:%d,%d  name[%s]     ****1\n", m_posi.x, m_posi.y, to_pos.x, to_pos.y, this->GetName());
				if (m_obj_type == Obj::OBJ_TYPE_ROLE && this->CheckWaiGuaMove())
				{
					Role* role = (Role*)this;
					role->ResetPos(m_posi.x, m_posi.y);
				}
				return false;
			}
		}

		{
			// 如果过了当前位置偏差的检查，则防止短时间内发送大量在允许偏差范围内的包
			// 此时则需要检查速度
			unsigned int now = EngineAdapter::Instance().GetGameTime();
			unsigned int time_dis = now - m_last_check_valid_time;

			/*printf("Move:  _x:%d _y:%d  _dir:%f\n _time:%d _now:%d last_check: %d\n", _x, _y, distance , time_dis,
			now, m_last_check_valid_time);*/


			// 直接用当前速度判断可能会有问题，因为在一次矢量运动过程中速度可能改变
			// 所以需要由策划配置一个最大速度来判断
			Axis dis = Axis(time_dis * float(max_move_speed) / 100000.0f);
			dis > 1000 ? (dis = 1000) : 0;

			//float check_dis = (m_last_check_pos).Dist(to_pos);
			//printf("%d+%d=%d %f\n", dis, move_check_length, dis + move_check_length, check_dis);

			if ((m_last_check_pos - to_pos).IsLongerThan(dis + move_check_length))
			{
				// 这小子用挂，拦截掉
				printf("last:%d,%d  to:%d,%d dis:%d      ****2\n", m_last_check_pos.x, m_last_check_pos.y, to_pos.x, to_pos.y, dis);
				if (m_obj_type == Obj::OBJ_TYPE_ROLE && this->CheckWaiGuaMove())
				{
					((Role*)this)->ResetPos(m_posi.x, m_posi.y);
				}
				return false;
			}

			if (m_last_check_valid_time != now || !this->CheckMoveFrequency())
			{
				// 同一帧内收到多条移动协议是有可能的，这时只记录第一次
				m_last_check_valid_time = now;
				m_last_check_pos = to_pos;
			}
		}
	}

	if (check_valid && !CanCharacterAction(true, true))
	{

/*
#ifdef _DEBUG
		if (NULL != pRole)
		{
			gamelog::g_log_world.printf(LL_ERROR, "Move Fail_2, role[%d,%s] cur_pos[%d,%d] dir[%.2f] distance[%.2f]\n", pRole->GetUID(), pRole->GetName(), _x, _y, _dir, distance);
		}
#endif
*/

		return false;
	}

	m_posf.Init((float)_x, (float)_y);
	m_posi.Init(_x, _y);
	m_dir = _dir;
	m_dir_distance = distance;
	m_height = height;
	m_scene->GetZoneMatrix()->MoveObser(m_obser_handle);

	int old_status = m_action_status;
	m_action_status = (m_dir == Dir_S ? ACTION_IDLE : ACTION_RUN);

	SendPosiInfo();

/*
#ifdef _DEBUG
	if (NULL != pRole)
	{
		gamelog::g_log_world.printf(LL_ERROR, "Move Succ, role[%d,%s] cur_pos[%d,%d] dir[%.2f] distance[%.2f]\n", pRole->GetUID(), pRole->GetName(), _x, _y, _dir, distance);
	}
#endif
*/

	if (Obj::OBJ_TYPE_ROLE == this->GetObjType())
	{
		if (ACTION_IDLE == old_status && ACTION_RUN == m_action_status)
		{
			Role* role = (Role*)this;
			EventHandler::Instance().OnRoleStartMove(role);
		}
		else if (ACTION_RUN == old_status && ACTION_IDLE == m_action_status)
		{
			Role* role = (Role*)this;
			EventHandler::Instance().OnRoleStopMove(role);
		}
	}

	//移动辅助
	if (!is_move_assist 
		&& NULL != m_move_assist)
	{//非移动辅助的移动操作 //清空移动辅助
		m_move_assist->OnMoveToOther();
	}

	return true;
}

void Character::ActionStop()
{
	bool not_static = false;
	if (!IsStatic())
	{
		not_static = true;
	}

	// m_dir = Dir_S;

	m_dir_distance = 0;
	m_action_status = ACTION_IDLE;

	if (not_static)
	{
		SendPosiInfo();
	}
}

void Character::MoveStop(bool is_notice /* = false */)
{
	if (m_action_status == ACTION_RUN && !IsStatic())
	{
		m_dir_distance = 0;
		m_action_status = ACTION_IDLE;

		if (is_notice)
		{
			SendPosiInfo();
		}

		if (Obj::OBJ_TYPE_ROLE == this->GetObjType())
		{
			Role* role = (Role*)this;
			EventHandler::Instance().OnRoleStopMove(role);
		}
	}
}

MoveAssist * Character::GetMoveAssist(bool can_creat)
{
	if (!can_creat)
	{
		return m_move_assist;
	}
	
	return this->CreatMoveAssist();
}

MoveAssist * Character::CreatMoveAssist()
{
	if (NULL != m_move_assist)
	{
		return m_move_assist;
	}
	unsigned int creat_id = MoveAssistManager::Instance().GetCreatMoveAssistID();
	m_move_assist = new MoveAssist(this, creat_id);
	return m_move_assist;
}

void Character::SetMoveLogic(Posi _target_posi)
{
	if (!this->CanCharacterAction(false, true))
	{
		return;
	}

	Scalar _distance = 0.0f;
	Posf _deltaP_f = Posf(_target_posi.x, _target_posi.y) - this->GetPosf();
	Dir _dir = gamecommon::GetMoveDirFloat(_deltaP_f, &_distance);

	this->ActionStop();
	m_dir = _dir;
	m_dir_distance = _distance;
	m_action_status = (m_dir == Dir_S ? ACTION_IDLE : ACTION_RUN);

	this->SendPosiInfo();
}

int Character::MoveLogic(unsigned long interval)
{
/*
#ifdef _DEBUG
	Role* pRole = NULL;
	if (Obj::OBJ_TYPE_ROLE == this->GetObjType())
	{
		Role* role = (Role*)this; // 此处代码仅用于断点
		if (role->GetUID() == TEST_UID)
		{
			pRole = role;
		}
	}

	if (pRole != NULL)
	{
		gamelog::g_log_world.printf(LL_ERROR, "Enter MoveLogic cur_pos[%d,%d] dir[%.2f] distance[%.2f]", 
			m_posi.x, m_posi.y, m_dir, m_dir_distance);
	}
#endif
*/

	if (NULL == m_scene) return MOVE_STATIC;

	if (m_dir == Dir_S)
	{
		m_action_status = ACTION_IDLE;
		return MOVE_STATIC;
	}

	static const float COMMON_SPEED = LOGIC_CONFIG->GetSundryConfig()->GetConstantCfg().move_speed * 5 / 10.0f; // 只初始一次

	float SPEED = COMMON_SPEED;
	if (this->GetObjType() == Obj::OBJ_TYPE_TU_ZI_KAI_PAO_TU_ZI)
	{
		SPEED = m_speed;
	}

	float speed_per = (m_speed_rate + 100) / 100.0f;
	if (speed_per < 0) speed_per = 0;

	Scalar distance = (Scalar)SPEED * speed_per * (interval / 1000.0f);
	distance = (distance < m_dir_distance ? distance : m_dir_distance);

	if (fabs(m_old_dir - m_dir) > 1e-6)
	{
		m_old_dir = m_dir;
		m_old_cos = cos(m_dir);
		m_old_sin = sin(m_dir);
	}
	else if (fabs(m_old_cos) < 1e-6 && fabs(m_old_sin) < 1e-6)
	{
		// old_cos与old_sin不会同时为0，否则就是静止状态
		m_old_cos = cos(m_dir);
		m_old_sin = sin(m_dir);
	}

	Posf pos = m_posf + Posf(m_old_cos, m_old_sin) * distance;
	Posi old_pos = m_posi;

	bool check_map_validate = this->CheckMapValidate();
	if (!check_map_validate || m_scene->GetMap()->CheckValidate(m_obj_type, (Axis)pos.x, (Axis)pos.y))
	{
		distance = std::min(m_dir_distance, distance);

		m_posf = pos;
		m_posi.Init((Axis)m_posf.x, (Axis)m_posf.y);
		m_dir_distance -= distance;

/*
#ifdef _DEBUG
		if (NULL != pRole)
		{		
			gamelog::g_log_world.printf(LL_ERROR, "MoveLogic After, new_pos[%d,%d] new_distance[%.2f]\n", m_posi.x, m_posi.y, m_dir_distance);
		}
#endif
*/

		m_scene->GetZoneMatrix()->MoveObser(m_obser_handle);
		if (m_dir_distance <= 0)
		{
			m_action_status = ACTION_IDLE;
		}

		if (Obj::OBJ_TYPE_ROLE == this->GetObjType()) // 只要移动了，就要更新AOIHandle
		{
			Role* role = (Role*)this;
			role->GetScene()->GetZoneMatrix()->MoveAOI(role->GetAOIHandle());
		}
	}
	else
	{
		MoveStop();

/*
#ifdef _DEBUG
		if (NULL != pRole)
		{
			gamelog::g_log_world.printf(LL_ERROR, "Stop Move cur_pos[%d,%d]\n", m_posi.x, m_posi.y);
		}
#endif
*/

		return MOVE_STATIC;
	}

	return MOVE_RUN;
}

void Character::SendPosiInfo()
{
	Protocol::SCObjMove scum;

	scum.dir = m_dir;
	scum.obj_id = m_obj_id;
	scum.pos_x = (short)m_posi.x;
	scum.pos_y = (short)m_posi.y;
	scum.distance = m_dir_distance;
	//scum.height = m_height;
	scum.reserve_sh = 0;

	if (NULL != m_scene) m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&scum, sizeof(Protocol::SCObjMove));
}

