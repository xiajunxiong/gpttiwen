#include "buffmanager.hpp"
#include "engineadapter.h"
#include "gameworld/protocol/msgother.h"
#include "buffbase.hpp"
#include "other/rolemodulemanager.hpp"
BuffManager::BuffManager()
{
	m_move_speed_per = 0;
	m_exp_up_per = 0;

	m_lase_update_time = 0;
}

BuffManager::~BuffManager()
{
	for (BuffList::iterator it = m_buff_list.begin(); m_buff_list.end() != it; ++it)
	{
		BuffBase* buff_base = *it;
		if (NULL != buff_base)
		{
			this->FreeBuff(buff_base);
		}
	}
}

void BuffManager::Init(RoleModuleManager * mgr, const BuffListParam & param)
{
	m_role_module_mgr = mgr;
	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
	for (int i = 0; i < param.buff_count&& i < ROLE_BUFF_MAX_SAVE_NUM; i++)
	{
		const BuffNode& node = param.buff_list[i];
		if (node.end_time <= now_time) continue;

		switch (node.buff_type)
		{
		case ROLE_BUFF_CHANGE_ATTR:
			{
				BuffChangeAttr *buff = new BuffChangeAttr(m_role_module_mgr, node.begin_time, node.end_time, node.origin_type);
				for (int attr = 0; attr < node.data.change_attr.count && attr < ROLE_BUFF_ATTR_NUM_MAX; attr++)
				{
					buff->AddAttr(node.data.change_attr.attr_type[attr], node.data.change_attr.attr_value[attr]);
				}
				buff->SetSave(true);
				this->AddBuff(buff, false);
			}
			break;
		case ROLE_BUFF_EXP_UP:
			{
				BuffExpUp *buff = new BuffExpUp(m_role_module_mgr, node.begin_time, node.end_time, node.origin_type);
				buff->SetExpUpPer(node.data.exp_up.up_per);
				buff->SetSave(true);
				this->AddBuff(buff, false);
			}
			break;
		case ROLE_BUFF_CHANGE_MOVE_SPEED:
			{
				BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(m_role_module_mgr, node.begin_time, node.end_time, node.origin_type);
				buff->SetMoveSpeed(node.data.change_move_speed.change_per);
				buff->SetSave(true);
				this->AddBuff(buff, false);
			}
			break;
		}
	}

}

void BuffManager::GetInitParam(BuffListParam * param)
{
	param->buff_count = 0;
	for (BuffList::const_iterator it = m_buff_list.begin(), end = m_buff_list.end();
		 it != end && param->buff_count < ROLE_BUFF_MAX_SAVE_NUM; ++it)
	{
		BuffBase* buff_base = *it;
		if (NULL == buff_base) continue;

		if (!buff_base->IsSave()) continue;

		if (buff_base->IsClearScene()) continue;

		if (param->buff_count < 0 || param->buff_count >= ROLE_BUFF_MAX_SAVE_NUM) continue;

		if (!buff_base->GetParam(&(param->buff_list[param->buff_count])))continue;

		param->buff_count++;
	}
}

void BuffManager::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (m_lase_update_time >= now_second) return;
	bool has_change_buff = false;
	for (BuffList::iterator it = m_buff_list.begin(); it != m_buff_list.end(); )
	{
		BuffBase* buff_base = *it;
		if (NULL == buff_base) continue;

		if (buff_base->IsOvertime(now_second))
		{
			m_buff_list.erase(it++);
			buff_base->Move();
			this->FreeBuff(buff_base);
			has_change_buff = true;
		}
		else
		{
			++it;
		}
	}
	if(has_change_buff)this->SendBuffList();
	m_lase_update_time = now_second;
}

const AttributeList & BuffManager::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		for (BuffList::const_iterator it = m_buff_list.begin(), end = m_buff_list.end(); it != end; ++it)
		{
			BuffBase* buff_base = *it;
			if (NULL == buff_base) continue;

			if (ROLE_BUFF_CHANGE_ATTR != buff_base->GetBuffType()) continue;

			BuffChangeAttr* buff_attr = (BuffChangeAttr*)buff_base;

			buff_attr->ReCalcAttr(m_attrs_add);
		}
	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void BuffManager::OnLogin()
{
	//this->SendBuffList();
	bool has_effect = false;
	for (BuffList::iterator it = m_buff_list.begin(); it != m_buff_list.end(); it++)
	{
		BuffBase* buff_base = *it;
		if (buff_base != NULL)
		{
			buff_base->Effect();
			has_effect = true;
		}
	}
}

void BuffManager::OnFightEnd()
{
}

void BuffManager::OnRoleLeaveScene(bool is_logout)
{
	bool has_change_buff = false;
	for (BuffList::iterator it = m_buff_list.begin(); it != m_buff_list.end(); )
	{
		BuffBase* buff_base = *it;
		if (NULL == buff_base) continue;
		if (buff_base->IsClearScene())// 是否场景buff
		{
			m_buff_list.erase(it++);
			buff_base->Move();
			this->FreeBuff(buff_base);
			has_change_buff = true;
		}
		else
		{
			++it;
		}
	}
	if (has_change_buff)this->SendBuffList();
}

void BuffManager::OnRoleEnterScene(int now_scene)
{
	bool has_change_buff = false;
	for (BuffList::iterator it = m_buff_list.begin(); it != m_buff_list.end(); )
	{
		BuffBase* buff_base = *it;
		if (NULL == buff_base) continue;
		if (buff_base->GetSceneBuffType() > 0 && buff_base->GetSceneBuffType() != now_scene)// 是否场景buff
		{
			m_buff_list.erase(it++);
			buff_base->Move();
			this->FreeBuff(buff_base);
			has_change_buff = true;
		}
		else
		{
			++it;
		}
	}
	if (has_change_buff)this->SendBuffList();
}

void BuffManager::AddBuff(BuffBase * buff, bool is_effect)
{
	if (NULL == buff) return;
	m_buff_list.push_back(buff);
	if (is_effect)
	{
		buff->Effect();
		m_role_module_mgr->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_BUFF_MGR, __FUNCTION__);
	}
	this->SendBuffList();
}

int BuffManager::ChangeMoveSpeedPer(int per)
{
	m_move_speed_per += per;
	return m_move_speed_per;
}

int BuffManager::GetMoveSpeedPer()
{
	return m_move_speed_per;
}

int BuffManager::GetSpeedChangeRate()
{
	return this->GetMoveSpeedPer();
}

int BuffManager::AddExpUpPer(int per)
{
	m_exp_up_per += per;
	return m_exp_up_per;
}

int BuffManager::GetExpUpPer()
{
	return m_exp_up_per;
}

void BuffManager::SendBuffList()
{
	Protocol::SCBuffInfo info;

	info.buff_count = 0;
	for (BuffList::const_iterator it = m_buff_list.begin(), end = m_buff_list.end();
		it != end; ++it)
	{
		BuffBase* buff_base = *it;
		if (NULL == buff_base) continue;
		info.buff_list[info.buff_count].origin = buff_base->GetOrigin();
		info.buff_list[info.buff_count].type = buff_base->GetBuffType();
		BuffNode buff_param;
		buff_base->GetParam(&buff_param);
		info.buff_list[info.buff_count].begin_time = buff_param.begin_time;
		info.buff_list[info.buff_count].end_time = buff_param.end_time;
		info.buff_list[info.buff_count].scene_buff = buff_param.fb_type;
		memcpy(info.buff_list[info.buff_count].data, buff_param.data.data, sizeof(NetValueBuffParamData));
		info.buff_count += 1;
	}
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void BuffManager::FreeBuff(BuffBase * buff_base)
{
	switch (buff_base->GetBuffType())
	{
	case ROLE_BUFF_CHANGE_ATTR:
		{
			BuffChangeAttr *buff = (BuffChangeAttr*)buff_base;
			delete buff;
			buff = NULL;
		}
		break;
	case ROLE_BUFF_EXP_UP:
		{
			BuffExpUp *buff = (BuffExpUp*)buff_base;
			delete buff;
			buff = NULL;
		}
		break;
	case ROLE_BUFF_CHANGE_MOVE_SPEED:
		{
			BuffChangeMoveSpeed *buff = (BuffChangeMoveSpeed*)buff_base;
			delete buff;
			buff = NULL;
		}
		break;
	}
}
