#include "speciallogicshituchuangong.hpp"
#include "other/mentor/shitu.hpp"
#include "other/mentor/shituconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "scene/scene.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgshitu.h"

#include "servercommon/errornum.h"
#include "engineadapter.h"

#include "global/usercache/usercache.hpp"
#include "config/logicconfigmanager.hpp"


SpecialLogicShiTuChuanGong::SpecialLogicShiTuChuanGong(Scene * scene)
	: SpecialLogic(scene)
{
	m_is_end = 0;
	m_game_begin = 0;
	m_game_end = 0;

	m_next_send_game_info_time = 0;

	m_game_ready_count = 0;
	m_need_send_score = 0;
}

void SpecialLogicShiTuChuanGong::Update(unsigned long interval, time_t now_second)
{
	if(0 != m_is_end)
	{//结束
		return;
	}

	//
	if (m_next_send_game_info_time < now_second)
	{
		m_next_send_game_info_time = now_second + 1;
		return;
	}

	if (0 != m_need_send_score)
	{
		//积分通知
		m_need_send_score = 0;

		for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
		{
			RoleInfo *role_info = this->GetRoleInfoByType(shi_tu_type);
			if (NULL == role_info)
			{
				continue;
			}
			Role* temp_role = this->GetRole(role_info->role_uid);
			this->SendGameInfo(temp_role, Protocol::SCShiTuChuanGongGameInfo::INFO_TYPE_GAME_SCORE, (unsigned int)(role_info->other_score));
		}
	}

	if (0 != m_game_end
		&& m_game_end < now_second)
	{//游戏结束
		this->GameEnd();
	}

}

void SpecialLogicShiTuChuanGong::OnRoleEnterScene(Role * role)
{
	if (NULL == role)
	{
		this->DelayKickOutRole(role);
		return;
	}

	int uid = role->GetUID();
	//未注册
	RoleInfo *role_info = this->GetRoleInfo(uid);
	if (NULL == role_info)
	{
		role->NoticeNum(errornum::EN_RELICLOOTING_TEAM_TYPE_ERR);
		this->DelayKickOutRole(role);
		return;
	}

	this->SendSceneShowInfo(role);
}

void SpecialLogicShiTuChuanGong::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (NULL == role)
	{
		this->DelayKickOutRole(role);
		return;
	}

	int uid = role->GetUID();
	RoleInfo *role_info = this->GetRoleInfo(uid);
	if (NULL == role_info)
	{
		this->DelayKickOutRole(role);
		return;
	}

	//未结束
	if (0 == m_is_end)
	{//中断
		this->BreakOff(role);
	}

	this->DelayKickOutAllRole();
}

void SpecialLogicShiTuChuanGong::SetRoleInfo(int uid, int shi_tu_type)
{
	if (0 > shi_tu_type
		|| SHI_TU_SEEKING_TYPE_MAX <= shi_tu_type)
	{
		return;
	}

	RoleInfo & role_info = m_role_info_[shi_tu_type];
	role_info.role_uid = uid;
	role_info.type = shi_tu_type;
}

void SpecialLogicShiTuChuanGong::SetEnd()
{
	UNSTD_STATIC_CHECK(2 == SHI_TU_SEEKING_TYPE_MAX);
	//赋值 对方uid
	m_role_info_[SHI_TU_SEEKING_TYPE_APPRENTICE].other_uid = m_role_info_[SHI_TU_SEEKING_TYPE_MASTER].role_uid;
	m_role_info_[SHI_TU_SEEKING_TYPE_MASTER].other_uid = m_role_info_[SHI_TU_SEEKING_TYPE_APPRENTICE].role_uid;
}

void SpecialLogicShiTuChuanGong::OnReq(Role * role, int req_type, int p_1, int p_2)
{
	if (NULL == role)
	{
		return;
	}

	int uid = role->GetUID();
	RoleInfo *role_info = this->GetRoleInfo(uid);
	if (NULL == role_info)
	{
		return;
	}

	switch (req_type)
	{
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_GAME_READY:
		{
			this->RoleGameReady(role);
		}
		break;
	case Protocol::CSShiTuChuanGongFBReq::REQ_TYPE_GAME_INFO:
		{
			this->RoleGameSorceSet(role, p_1, p_2);
		}
		break;
	default:
		break;
	}
}

void SpecialLogicShiTuChuanGong::SendSceneShowInfo(Role * role)
{
	if (NULL == role)
	{
		return;
	}
	UNSTD_STATIC_CHECK(2 == SHI_TU_SEEKING_TYPE_MAX);

	static Protocol::SCShiTuChuanGongSceneInfo info;
	info.count = SHI_TU_SEEKING_TYPE_MAX;

	for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
	{
		RoleInfo *role_info = this->GetRoleInfoByType(shi_tu_type);
		if (NULL == role_info)
		{
			continue;
		}

		UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_info->role_uid);
		if (NULL == user_node)
		{
			continue;
		}

		info.host_list[shi_tu_type].uid = user_node->uid;
		user_node->GetName(info.host_list[shi_tu_type].role_name);
		info.host_list[shi_tu_type].appearance = user_node->appearance;
	}

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void SpecialLogicShiTuChuanGong::SendGameInfo(Role * role, int type, unsigned int param)
{
	static Protocol::SCShiTuChuanGongGameInfo info;
	info.type = type;
	info.param = param;

	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, sizeof(info));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

void SpecialLogicShiTuChuanGong::RoleGameReady(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	int uid = role->GetUID();
	RoleInfo *role_info = this->GetRoleInfo(uid);
	if (NULL == role_info)
	{
		return;
	}
	if (0 != role_info->game_ready)
	{
		return;
	}
	role_info->game_ready = 1;
	m_game_ready_count++;
	
	if (SHI_TU_SEEKING_TYPE_MAX > m_game_ready_count)
	{
		return;
	}

	const ShiTuChuanGongOtherCfg& other_cfg
		= LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongOtherCfg();

	//开始准备
	m_game_begin = EngineAdapter::Instance().Time() + other_cfg.game_ready_time;
	m_game_end = m_game_begin + other_cfg.game_time + 2;
	{//准备通知
		this->SendGameInfo(NULL, Protocol::SCShiTuChuanGongGameInfo::INFO_TYPE_GAME_BEGIN, (unsigned int)m_game_begin);
	}
}

void SpecialLogicShiTuChuanGong::RoleGameSorceSet(Role * role, int base_score, int game_score)
{
	if (0 != m_is_end)
	{
		return;
	}

	if (NULL == role)
	{
		return;
	}

	int uid = role->GetUID();
	RoleInfo *role_info = this->GetRoleInfo(uid);
	if (NULL == role_info)
	{
		return;
	}
	//设置

	for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
	{
		RoleInfo *role_info = this->GetRoleInfoByType(shi_tu_type);
		if (NULL == role_info)
		{
			continue;
		}

		int other_uid = role_info->role_uid;
		if (other_uid == uid)
		{//自己
			role_info->game_score = game_score;
			role_info->base_score = base_score;
		}
		else
		{//他人
			RoleInfo *other_role_info = this->GetRoleInfo(other_uid);
			if (NULL == other_role_info)
			{
				continue;
			}
			other_role_info->other_score = base_score;
		}
	}

	m_need_send_score = true;
}

void SpecialLogicShiTuChuanGong::GameEnd()
{
	if (0 != m_is_end)
	{
		return;
	}
	m_is_end = 1;

	for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
	{
		RoleInfo *role_info = this->GetRoleInfoByType(shi_tu_type);
		if (NULL == role_info)
		{
			continue;
		}

		Role* role = this->GetRole(role_info->role_uid);
		if (NULL == role)
		{
			continue;
		}
		role->GetRoleModuleManager()->GetShiTu()->FinishChuanGong(
			role_info->other_uid,
			role_info->game_score);
	}
}

void SpecialLogicShiTuChuanGong::BreakOff(Role *role)
{
	if (0 != m_is_end)
	{
		return;
	}
	m_is_end = 1;

	if (NULL == role)
	{
		return;
	}
	//对方飘字
	for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
	{
		if (m_role_info_[shi_tu_type].role_uid != role->GetUID())
		{
			Role *other_role = this->GetRole(m_role_info_[shi_tu_type].role_uid);
			if (NULL != other_role)
			{
				other_role->NoticeNum(errornum::EN_SHITU_CHUAN_GONG_BREAK_OFF);
			}
		}
	}

}

Role * SpecialLogicShiTuChuanGong::GetRole(int uid)
{
	return m_scene->GetRoleByUID(uid);
}

SpecialLogicShiTuChuanGong::RoleInfo * SpecialLogicShiTuChuanGong::GetRoleInfo(int uid)
{
	for (int shi_tu_type = 0; shi_tu_type < SHI_TU_SEEKING_TYPE_MAX; shi_tu_type++)
	{
		if (m_role_info_[shi_tu_type].role_uid == uid)
		{
			return &m_role_info_[shi_tu_type];
		}
	}
	return NULL;
}

SpecialLogicShiTuChuanGong::RoleInfo * SpecialLogicShiTuChuanGong::GetRoleInfoByType(int shi_tu_type)
{
	if (0 > shi_tu_type
		|| SHI_TU_SEEKING_TYPE_MAX <= shi_tu_type)
	{
		return NULL;
	}
	return &m_role_info_[shi_tu_type];
}

