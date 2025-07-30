#include "rolecross.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "protocol/msgactivity.hpp"
#include "engineadapter.h"
#include "global/cross/crossmanager.h"
#include "servercommon/crossdef.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "global/team/teammanager.hpp"
#include "gamelog.h"
#include "world.h"
#include "chat/chatmanager.h"
#include "battle/battle_manager_local.hpp"
#include "protocol/msgserver.h"
#include "global/team/team.hpp"

RoleCross::RoleCross() : m_module_mgr(NULL), m_is_preparing_for_cross(false)
{

}

RoleCross::~RoleCross()
{

}

void RoleCross::Init(RoleModuleManager *mgr, const RoleCrossParam &p)
{
	m_module_mgr = mgr;
	m_param = p;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_param.origin_server_id = LocalConfig::Instance().GetMergeServerId();
		m_param.origin_uuid = m_module_mgr->GetRole()->GetUUID();
		F_STRNCPY(m_param.origin_role_name, m_module_mgr->GetRole()->GetName(), sizeof(GameName));
		m_param.origin_plat_type = LocalConfig::Instance().GetPlatType();
		F_STRNCPY(m_param.origin_pname, m_module_mgr->GetRole()->GetPlatName(), sizeof(PlatName));
	}
}

void RoleCross::GetInitParam(RoleCrossParam *p)
{
	if (NULL == p) return;

	*p = m_param;
}

void RoleCross::Update(time_t now_second)
{

}

bool RoleCross::OnStartCrossReq(const CrossData_Business& business_data)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return false;
	}

	if (!CrossManager::Instance().IsCrossServerConnected())
	{
		m_module_mgr->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return false;
	}

	if (!m_module_mgr->GetRole()->GetRoleStatusManager()->CanStartCross())
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_CAN_NOT_CROSS_NOW);
		return false;
	}

	if (!CrossManager::Instance().CanStartCross(m_module_mgr->GetRole()))
	{
		return false;
	}

	const RoleBattleInfo* rbi = BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(m_module_mgr->GetRole()->GetUID());
	if (NULL != rbi)
	{
		if (rbi->is_ob)
		{
			BattleManagerLocal::GetInstance().ForceStopRoleOB(m_module_mgr->GetRole());
		}
	}
	
	//检查玩家本地群组同步跨服情况
	ChatManager::Instance().OnRoleStarCrossSendLocalGroup(m_module_mgr->GetRole());

	//确保先保存成功后再跨服
	if (!m_module_mgr->GetRole()->Save(false)) //如果后续出现进入跨服比保存更快从而丢失数据，则应该保存成功确认后才跨服
	{
		m_module_mgr->NoticeNum(errornum::EN_ROLE_CAN_NOT_CROSS_NOW);
		return false;
	}

	m_is_preparing_for_cross = true;

	gamelog::g_log_rolecross.printf(LL_INFO, "RoleCross::OnStartCrossReq STEP_1 rolecross[%d,%s]", m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName());

	static crossgameprotocal::GameCrossStartCrossReq req;
	req.plat_type = LocalConfig::Instance().GetPlatType();
	req.server_id = LocalConfig::Instance().GetMergeServerId();
	req.uid = m_module_mgr->GetRole()->GetUID();
	F_STRNCPY(req.pname, m_module_mgr->GetRole()->GetPlatName(), sizeof(req.pname));
	req.cross_business_data = business_data;

	return InternalComm::Instance().SendToCross((const char*)&req, sizeof(req));
}

void RoleCross::OnStartCrossAck(crossgameprotocal::CrossGameStartCrossAck* cgsca)
{
	if (cgsca->result != 0)
	{
		m_is_preparing_for_cross = false;
		return;
	}
	// 特殊处理。 如果玩家正在观战，则强制退出观战
	const RoleBattleInfo* rbi = BattleManagerLocal::GetInstance().GetRoleBattleInfoConst(m_module_mgr->GetRole()->GetUID());
	if (NULL != rbi)
	{
		if (rbi->is_ob)
		{
			BattleManagerLocal::GetInstance().ForceStopRoleOB(m_module_mgr->GetRole());
		}
	}

	gamelog::g_log_rolecross.printf(LL_INFO, "RoleCross::OnStartCrossAck STEP_5 rolecross[%d,%s] host[%s] port[%d]", 
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), cgsca->gateway_hostname, cgsca->gateway_port);

	static Protocol::SCCrossStartAck ack;

	F_STRNCPY(ack.gateway_hostname, cgsca->gateway_hostname, sizeof(ack.gateway_hostname));
	ack.gateway_port = cgsca->gateway_port;

	F_STRNCPY(ack.role_name, cgsca->role_name, sizeof(ack.role_name));
	ack.new_uid = cgsca->new_uid;
	ack.scene_id = cgsca->scene_id;
	ack.last_scene_id = cgsca->last_scene_id;
	ack.time = cgsca->time;

	F_STRNCPY(ack.session_key, cgsca->session_key, sizeof(ack.session_key));

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&ack, sizeof(ack));
}

void RoleCross::OnRoleChangeName()
{
	F_STRNCPY(m_param.origin_role_name, m_module_mgr->GetRole()->GetName(), sizeof(GameName));
}

void RoleCross::OnBackToOriginServerReq(ARG_IN const ReturnOriginData_Business* in_business_data /* = NULL */)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (m_module_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_module_mgr->NoticeNum(errornum::EN_RETURN_ORIGIN_SERVER_IN_BATTLE_LIMIT);
		return;
	}

	static ReturnOriginData_Business business;
	business.Reset();
	business.business_type = ReturnOriginData_Business::BUSINESS_TYPE_INVALID;

	if (NULL != in_business_data)
	{
		business = *in_business_data;
	}

	do
	{
		Team* team = m_module_mgr->GetRole()->GetMyTeam();
		if (NULL != team)
		{
			if (team->GetLeaderUID() == m_module_mgr->GetRole()->GetUID())
			{
				// 是队长 获取一同回原服的玩家列表
				business.business_type = ReturnOriginData_Business::BUSINESS_TYPE_TEAM_SWITCH_TO_ORIGIN;
				business.level_limit_high = team->GetLimitLevelHigh();
				business.level_limit_low = team->GetLimitLevelLow();
				business.team_type = team->GetTeamType();
				
				int num = 0;
				const UniqueServerID* leader_usid = World::GetInstWorld()->GetMotherServerId(UniqueServerID(this->GetOriginPlatType(), this->GetOriginServerId()));
				if (NULL == leader_usid)
				{
					// 找不到usid，不太可能发生，但仍可做容错
					TeamManager::Instance().ExitTeam(m_module_mgr->GetRole());
					break;
				}

				for (int i = 0; i < MAX_TEAM_MEMBER_NUM && num < ARRAY_ITEM_COUNT(business.teammate_list); ++i)
				{
					TeamMember* member = team->GetMemberInfoByIndex(i);
					if (NULL == member) continue;
					if (member->uid == m_module_mgr->GetRole()->GetUID()) continue;
					if (!member->is_follow) continue;
					if (member->IsRobot()) continue;

					Role* member_role = team->GetMemberRoleByIndex(i);
					if (NULL == member_role) continue;

					const UniqueServerID* member_usid = World::GetInstWorld()->GetMotherServerId(
						UniqueServerID(member_role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), 
							member_role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId()
						));
					if (NULL == member_usid) continue;
					if (leader_usid != member_usid) continue;

					business.teammate_list[num++] = member_role->GetUID();
				}

				// 集体退队
				TeamManager::Instance().OnLeaderExitTeamWithSomeMembers(m_module_mgr->GetRole(), num, business.teammate_list);
			}
			else
			{
				// 不是队长 直接退队
				TeamManager::Instance().ExitTeam(m_module_mgr->GetRole());
			}
		}

	} while (0);

	crossgameprotocal::HiddenGameRoleReturnOriginServerReq req;
	req.role_id = m_module_mgr->GetRole()->GetUID();
	req.business_data = business;

	InternalComm::Instance().SendToGameThroughCross(this->GetOriginPlatType(), this->GetOriginServerId(), (const char*)&req, sizeof(req));

}

void RoleCross::OnBackToOriginServerAck(crossgameprotocal::GameHiddenRoleReturnOriginServerAck* ack)
{
	if (ack->result != 0)
	{
		gamelog::g_log_rolecross.printf(LL_WARNING, "OnBackToOriginServerAck FAILED! role[%d,%s] result[%d]",
			ack->role_id, m_module_mgr->GetRole()->GetName(), ack->result);
		return;
	}

	if (m_module_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_module_mgr->NoticeNum(errornum::EN_RETURN_ORIGIN_SERVER_IN_BATTLE_LIMIT);
		return;
	}

	Scene* scene = m_module_mgr->GetRole()->GetScene();
	if (NULL != scene)
	{
		int scene_id = scene->GetSceneID();
		int last_scene_id = m_module_mgr->GetRole()->GetLastScene(NULL);
		if (!scene->IsInStaticScene())
		{
			scene_id = last_scene_id;
		}

		unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int origin_uid = UidToInt(LongLongToUniqueUserID(m_param.origin_uuid).user_id);
		static SessionKey session_key;
		GetSessionKey(InternalComm::Instance().m_session_key_base, scene_id, 0,
			last_scene_id, origin_uid, m_param.origin_role_name, now_time, 1, m_param.origin_pname, session_key);

		UniqueServerID origin_usid(LocalConfig::Instance().GetPlatType(), IntToUid(origin_uid).db_index);
		const World::MotherServerInfo* msi = World::GetInstWorld()->GetMotherServerInfo(origin_usid);
		if (NULL == msi)
		{
			m_module_mgr->NoticeNum(errornum::EN_RETURN_ORIGIN_SERVER_FAIL);
			return;
		}

		Protocol::SCBackToOriginServer msg;
		msg.last_scene_id = last_scene_id;
		msg.scene_id = scene_id;
		msg.time = now_time;
		F_STRNCPY(msg.session_key, session_key, sizeof(msg.session_key));
		F_STRNCPY(msg.gateway_hostname, msi->gw_hostname.c_str(), sizeof(msg.gateway_hostname));
		msg.gateway_port = msi->gw_port;

		m_module_mgr->NetSend(&msg, sizeof(msg));

		m_module_mgr->GetRole()->Save(); //!< \note 立刻保存一次先（离线时会再保存一次，如果卡的话再把这里屏蔽吧
	}
}

void RoleCross::SendCrossConnectInfo()
{
	Protocol::SCCrossConnectInfo info;
	info.is_cross = CrossConfig::Instance().IsHiddenServer() ? 1 : 0;
	if (1 == info.is_cross)
	{
		info.is_connected_cross = 1;
	}
	else
	{
		info.is_connected_cross = CrossManager::Instance().IsCrossServerConnected() ? 1 : 0;
	}
	info.reserve_sh = 0;

	EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), &info, sizeof(info));
}
