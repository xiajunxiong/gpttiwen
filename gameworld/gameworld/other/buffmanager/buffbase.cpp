#include "buffbase.hpp"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "buffmanager.hpp"
#include "protocol/msgother.h"

BuffBase::BuffBase(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin)
{
	m_role_module_mgr = role_module_mgr;
	m_begin_time = begin_time;
	m_end_time = end_time;
	m_origin = origin;
	m_is_save = false;
	m_is_leave_clear = false;
	m_fb_type = 0;
	m_data.Reset();
}

BuffBase::~BuffBase()
{
}

bool BuffBase::IsOvertime(time_t now_second)
{
	if (m_end_time == 0 && m_begin_time == 0)return false;//开始结束时间都为0的时候即，无限时长buff
	return now_second >= m_end_time;
}

bool BuffBase::GetParam(BuffNode * param)
{
	if (NULL == param) return false;

	param->begin_time = m_begin_time;
	param->end_time = m_end_time;
	param->buff_type = this->GetBuffType();
	param->origin_type = this->GetOrigin();
	param->fb_type = m_fb_type;
	memcpy(param->data.data, m_data.data, sizeof(NetValueBuffParamData));

	return true;
}

BuffChangeAttr::BuffChangeAttr(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin) :
	BuffBase(role_module_mgr, begin_time, end_time, origin)
{
}

BuffChangeAttr::~BuffChangeAttr()
{
}

bool BuffChangeAttr::Effect()
{
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_BUFF, __FUNCTION__);
	return true;
}

bool BuffChangeAttr::Move()
{
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_BUFF, __FUNCTION__);
	return true;
}

void BuffChangeAttr::AddAttr(int type, int value)
{
	BuffParamChangeAttr &data = m_data.change_attr;
	if (data.count >= ROLE_BUFF_ATTR_NUM_MAX) return;
	if (type < BATTLE_ATTR_MIN || type >= BATTLE_ATTR_MAX) return;

	data.attr_type[data.count] = type;
	data.attr_value[data.count] = value;
	data.count += 1;
}

void BuffChangeAttr::ReCalcAttr(AttributeList & base_add)
{
	BuffParamChangeAttr &data = m_data.change_attr;
	for (int i = 0; i < ROLE_BUFF_ATTR_NUM_MAX && i < data.count; i++)
	{
		if (data.attr_type[i] < BATTLE_ATTR_MIN || data.attr_type[i] >= BATTLE_ATTR_MAX) continue;

		base_add.m_attrs[data.attr_type[i]] += data.attr_value[i];
	}
}

BuffChangeMoveSpeed::BuffChangeMoveSpeed(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin) :
	BuffBase(role_module_mgr, begin_time, end_time, origin)
{
}

BuffChangeMoveSpeed::~BuffChangeMoveSpeed()
{
}

bool BuffChangeMoveSpeed::Effect()
{
	m_role_module_mgr->GetBuffManager()->ChangeMoveSpeedPer(m_data.change_move_speed.change_per);
	m_role_module_mgr->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_BUFF_MGR, __FUNCTION__);
	return true;
}

bool BuffChangeMoveSpeed::Move()
{
	m_role_module_mgr->GetBuffManager()->ChangeMoveSpeedPer(-m_data.change_move_speed.change_per);
	m_role_module_mgr->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_BUFF_MGR, __FUNCTION__);
	return true;
}

void BuffChangeMoveSpeed::SetMoveSpeed(int change_per)
{
	m_data.change_move_speed.change_per = change_per;
}

BuffExpUp::BuffExpUp(RoleModuleManager * role_module_mgr, unsigned int begin_time, unsigned int end_time, int origin) :
	BuffBase(role_module_mgr, begin_time, end_time, origin)
{
}

BuffExpUp::~BuffExpUp()
{
}

bool BuffExpUp::Effect()
{
	m_role_module_mgr->GetBuffManager()->AddExpUpPer(m_data.exp_up.up_per);
	return true;
}

bool BuffExpUp::Move()
{
	m_role_module_mgr->GetBuffManager()->AddExpUpPer(-m_data.exp_up.up_per);
	return true;
}

void BuffExpUp::SetExpUpPer(int per)
{
	m_data.exp_up.up_per = per;
}
