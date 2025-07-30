#include "speciallogichongmengtianjie.hpp"
#include "other/hongmengtianjie/hongmengtianjieconfig.hpp"
#include "other/hongmengtianjie/rolehongmengtianjie.hpp"
#include "other/hongmengtianjie/hongmengtianjiemanager.hpp"

#include "config/logicconfigmanager.hpp"
#include "npc/npcmanager.h"
#include "scene/scene.h"
#include "scene/map.h"
#include "protocol/msghongmengtianjie.hpp"
#include "battle/battle_manager_local.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "world.h"
#include "battle/battle_video_manager.hpp"

#include "obj/otherobj/specialtransport.h"
#include "scene/fbmanager.hpp"
#include "servercommon/errornum.h"

SpecialLogicHongMengTianJie::SpecialLogicHongMengTianJie(Scene * scene) :SpecialLogic(scene), 
m_fb_id(0), m_door_pos(0, 0), m_is_world_pass(false), m_role_leave_times(0), 
m_stage(0),m_monster_seq(0), m_fb_type(0), m_fb_seq(0)
{
}

SpecialLogicHongMengTianJie::~SpecialLogicHongMengTianJie()
{
}

// void SpecialLogicHongMengTianJie::Update(unsigned long interval, time_t now_second)
// {
// 	if (m_role_leave_times > 0 && now_second > m_role_leave_times)
// 	{
// 		this->DelayKickOutAllRole(CHANGE_SCENE_REASON_FB_FINISH);
// 	}
// }

void SpecialLogicHongMengTianJie::OnRoleEnterScene(Role * role)
{
	if (NULL == role) return;

	if (!this->IsTeamMember(role->GetUID()))
	{
		this->DelayKickOutRole(role);
	}

	this->SendSceneInfo(role);
	this->SendPassItemInfo(role);

	if (!role->GetRoleModuleManager()->GetRoleHongMengTianJie()->IsBossTimes(true))
	{
		role->NoticeNum(errornum::EN_HONGMENG_TIANJIE_NOT_REWARD_TIMES);
	}
}

void SpecialLogicHongMengTianJie::OnRoleLeaveScene(Role * role, bool is_logout)
{
}

void SpecialLogicHongMengTianJie::OnTransport(Role * role, SpecialTransport * transport_obj)
{
	if (NULL == role || NULL == transport_obj)
	{
		return;
	}

	if (role->IsLeader())
	{
		FBManager::Instance().LeaveFB(role);
	}
}

bool SpecialLogicHongMengTianJie::CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader)
{
	if (NULL == member) return false;
	if (!this->IsTeamMember(member->GetUID())) return false;

	if (!WorldHongMengTianJieManager::Instance().CanRoleEnterFB(member, m_fb_type, m_fb_seq))
	{
		return false;
	}

	return true;
}

bool SpecialLogicHongMengTianJie::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	if (NULL == role) return false;
	if (!this->IsTeamMember(role->GetUID())) return false;

	if (!WorldHongMengTianJieManager::Instance().CanRoleEnterFB(role, m_fb_type, m_fb_seq))
	{
		return false;
	}

	return true;
}

void SpecialLogicHongMengTianJie::OnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (BATTLE_MODE_HONGMENG_TIANJIE != ack->battle_mode || RESULT_TYPE_ATTACKER_WIN != ack->battle_result)
	{
		return;
	}

	int npc_seq_id = ack->business_data.monster_gen_id;
	this->OnKillAMonster(npc_seq_id);

	Team * team = NULL;
	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->attacker_list.role_list[i].uid));
		if (NULL != mem_role)
		{
			std::vector<ItemConfigData> item_list;
			mem_role->GetRoleModuleManager()->GetRoleHongMengTianJie()->SendBossReward(m_fb_id, m_stage, item_list);

			for (int j = 0; j < (int)item_list.size(); j++)
			{
				m_user_item_list[mem_role->GetUID()][item_list[j].item_id] += item_list[j].num;
			}

			if (NULL == team) team = mem_role->GetMyTeam(false);

			this->SendPassItemInfo(mem_role);
		}
	}

	m_stage += 1;

	if (!m_is_world_pass)
	{
		pass_monster_group[ack->monster_group_id] = ack->business_data.record_file_id;
	}

	// BOSS 是否都打完
	if (m_stage >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
	{
		if (!WorldHongMengTianJieManager::Instance().IsFirstPass(m_fb_id) && NULL != team)
		{
			WorldHongMengTianJieManager::Instance().SetFirstPassData(m_fb_id, team);

			// 记录视频
			for (std::map<int, int>::iterator begin_it = pass_monster_group.begin(); begin_it != pass_monster_group.end(); begin_it++)
			{
				BattleVideoMgr::Instance().TryRecordVideo(BATTLE_MODE_HONGMENG_TIANJIE, begin_it->first, 0, begin_it->second, false);
			}
		}

		this->CreatLeaveTransport();
		//m_role_leave_times = (unsigned int)EngineAdapter::Instance().Time() + 5;
	}
	else
	{
		this->CreateAMonster();		
	}

	this->SendSceneInfo();
}

bool SpecialLogicHongMengTianJie::InitScene(int fb_id, int difficulty, Posi door_pos, int fb_type, int fb_seq)
{
	m_fb_id = fb_id;
	m_fb_type = fb_type;
	m_fb_seq = fb_seq;
	m_difficulty = difficulty;
	m_door_pos = door_pos;
	m_is_world_pass = WorldHongMengTianJieManager::Instance().IsFirstPass(fb_id);

	return this->CreateAMonster();
}

void SpecialLogicHongMengTianJie::SetTeamMemberList(int member_num, int* uid_list)
{
	m_uid_list.clear();
	for (int i = 0; i < member_num; ++i)
	{
		m_uid_list.insert(uid_list[i]);
	}
}

bool SpecialLogicHongMengTianJie::IsTeamMember(int uid) const
{
	return m_uid_list.find(uid) != m_uid_list.end();
}

void SpecialLogicHongMengTianJie::OnReq(Role * role, void * msg)
{
	if (NULL == msg) return;

	Protocol::CSHongMengTianJieReq * req = (Protocol::CSHongMengTianJieReq *)msg;

	switch (req->req_type)
	{
	case Protocol::CSHongMengTianJieReq::REQ_TYPE_TALK:
		{
			this->SendTalkInfo(req->p1, req->p2);
		}
		break;
	case Protocol::CSHongMengTianJieReq::REQ_TYPE_SEND_REWARD:
		{
			this->SendPassItemInfo(role);
		}
		break;
	}
}

void SpecialLogicHongMengTianJie::SendSceneInfo(Role * role)
{
	Protocol::SCHongMengTianJieSceneInfo info;
	info.stage = m_stage;
	info.fb_id = m_fb_id;

	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, sizeof(info));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}

void SpecialLogicHongMengTianJie::SendTalkInfo(int talk_id, int talk_seq)
{
	Protocol::SCHongMengTianJieTalkInfo info;
	info.talk_id = talk_id;
	info.talk_seq = talk_seq;

	m_scene->SendToRole((const char *)&info, sizeof(info));
}

void SpecialLogicHongMengTianJie::SendPassItemInfo(Role * role)
{
	if (NULL == role) return;

	Protocol::SCHongMengTianJieRewardInfo info;
	info.list_num = 0;
	std::map<int, std::map<ItemID, int> >::const_iterator user_it = m_user_item_list.find(role->GetUID());
	if (user_it != m_user_item_list.end())
	{
		for (std::map<ItemID, int>::const_iterator loop = user_it->second.begin(); loop != user_it->second.end(); ++loop)
		{
			if (info.list_num >= ARRAY_LENGTH(info.item_list)) break;
			info.item_list[info.list_num++] = ItemPairCon(loop->first, loop->second);
		}
	}

	int send_len = sizeof(info) - sizeof(info.item_list) + info.list_num * sizeof(info.item_list[0]);
	if (send_len > 0) EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, send_len);
}

bool SpecialLogicHongMengTianJie::CreateAMonster()
{
	const HongMengTianJieNpcCfg * npc_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetNpcCfg(m_fb_id);
	if (NULL == npc_cfg)
		return false;

	if (m_stage >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
		return false;

	NPCMonster * monster = NpcManager::Instance().CreateNPCMonster(npc_cfg->npc_param[m_stage].npc_id, m_scene, npc_cfg->npc_param[m_stage].pos, BATTLE_MODE_HONGMENG_TIANJIE);
	if (NULL == monster) return false;

	m_monster_seq = monster->GetNpcSeq();

	return true;
}

void SpecialLogicHongMengTianJie::OnKillAMonster(int seq_id)
{
	NPCMonster * npc_monster = NpcManager::Instance().GetNPCMonster(seq_id);
	if (NULL == npc_monster) return;

	m_scene->DeleteObj(npc_monster->GetId());
}

void SpecialLogicHongMengTianJie::CreatLeaveTransport()
{
	SpecialTransport * transport_obj = new SpecialTransport();
	if (transport_obj == NULL)
	{
		return;
	}

	transport_obj->SetPos(m_door_pos);
	transport_obj->GetExtraInfo().param1 = 0;
	transport_obj->GetExtraInfo().send_param1 = 0;
	transport_obj->GetExtraInfo().send_param2 = 0;
	m_scene->AddObj(transport_obj);
}
