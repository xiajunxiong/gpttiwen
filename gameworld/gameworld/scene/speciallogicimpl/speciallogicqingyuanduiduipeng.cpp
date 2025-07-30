#include "speciallogicqingyuanduiduipeng.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "scene/scene.h"
#include "obj/character/role.h"
#include "npc/npcmanager.h"
#include "obj/otherobj/npcother.h"
#include "servercommon/noticenum.h"
#include "engineadapter.h"
#include "world.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/team/teammanager.hpp"

SpecialLogicQingYuanDuiDuiPeng::SpecialLogicQingYuanDuiDuiPeng(Scene * scene) : SpecialLogic(scene)
{
	m_is_init = false;

	begin_game_times = 0;
	end_game_times = 0;
	role_leave_times = 0;

	integral = 0;
	continuity = 0;
	succ_match = 0;

	reward_level = 0;
}

SpecialLogicQingYuanDuiDuiPeng::~SpecialLogicQingYuanDuiDuiPeng()
{
}

void SpecialLogicQingYuanDuiDuiPeng::Update(unsigned long interval, time_t now_second)
{
	if (!m_is_init)
	{
		m_is_init = true;

		std::map<int, QingYuanDuiDuiPengNpcCfg> npc_map = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetNpcMap();
		
		for (std::map<int, QingYuanDuiDuiPengNpcCfg>::iterator begin_it = npc_map.begin(); begin_it != npc_map.end(); begin_it++)
		{
			int npc_id = begin_it->first;

			
			NPCOther * npc = new NPCOther();
			npc->SetNpcId(npc_id);
			npc->SetPos(begin_it->second.pos);

			m_scene->AddObj(npc);

			NpcData npc_data(begin_it->second.seq, npc_id);
			m_npc_list[npc->GetId()] = npc_data;
		}
	}

	if (end_game_times > 0 && now_second > end_game_times)
	{
		this->OnFinish(true);
	}
	if (role_leave_times > 0 && now_second > role_leave_times)
	{
		this->DelayKickOutAllRole(CHANGE_SCENE_REASON_FB_FINISH);
	}
}

void SpecialLogicQingYuanDuiDuiPeng::OnRoleEnterScene(Role * role)
{
	for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
	{
		if(role_list[i].user_id != INVALID_USER_ID)
			continue;

		if (role_list[i].user_id == role->GetUserId())
			break;

		role_list[i].user_id = role->GetUserId();
		break;
	}

	if (!role->IsLeader())
	{
		TeamManager::Instance().OnTemporarilyPart(role);
	}
	
	this->SendFbInfo(role);
}

void SpecialLogicQingYuanDuiDuiPeng::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (end_game_times > 0)
	{
		this->OnFinish(false);

		Protocol::SCNoticeNum en;
		en.notice_num =	errornum::EN_QINGYUAN_DUIDUIPENG_LEAVE_SCENE;

		m_scene->SendToRole((const char *)&en, sizeof(en));
	}
}

// void SpecialLogicQingYuanDuiDuiPeng::OnNPCChosedReward(Role * role, ObjID npc_objid, int option_index)
// {
// 	if (NULL == role) return;
// 
// 	std::map<ObjID, NpcData>::iterator it = m_npc_list.find(npc_objid);
// 	if (it == m_npc_list.end())
// 		return;
// 
// 	for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
// 	{
// 		if(role->GetUserId() != role_list[i].user_id)
// 			continue;
// 
// 		role_list[i].npc_obj_id = npc_objid;
// 		role_list[i].seq = it->second.seq;
// 		role_list[i].npc_id = it->second.npc_id;
// 
// 		{
// 			int length_1 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_prom, it->second.npc_id);;
// 			if (length_1 > 0)
// 			{
// 				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_list[i].user_id, gamestring::GAMESTRING_BUF, length_1, GetMsgType(SYS_MSG_FLOAT));
// 			}
// 		}
// 		{
// 			int length_2 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_prom_1,
// 				role->GetUID(), role->GetName(), it->second.npc_id);;
// 			if (length_2 > 0)
// 			{
// 				int team_mate_seq = i == 0 ? 1 : 0;
// 				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_list[team_mate_seq].user_id, gamestring::GAMESTRING_BUF, length_2, GetMsgType(SYS_MSG_FLOAT));
// 			}
// 		}		
// 	}
// 
// 	this->SendRoleForm();
// }

void SpecialLogicQingYuanDuiDuiPeng::OnClientReq(Role * role, void * msg)
{
	if (NULL == role || NULL == msg)
		return;

	CSQingYuanDuiDuiPengReq * req = (CSQingYuanDuiDuiPengReq *)msg;

	switch (req->req_type)
	{
	case Protocol::CSQingYuanDuiDuiPengReq::REQ_TYPE_START:
		{
			this->OnStartGame(role);
		}
		break;
	case Protocol::CSQingYuanDuiDuiPengReq::REQ_TYPE_GOTO_COMPARE:
		{
			this->CompareForm();
		}
		break;
	case Protocol::CSQingYuanDuiDuiPengReq::REQ_TYPE_NPC_FORM:
		{
			this->OnNpcForm(role, req->p1);
		}
		break;
	}
}

void SpecialLogicQingYuanDuiDuiPeng::OnStartGame(Role * role)
{
	if (NULL == role) return;

// 	for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
// 	{
// 		if (role->GetUserId() != role_list[i].user_id)
// 			continue;
// 
// 		if (role_list[i].start_game) return;		// 已经准备了 就直接返回
// 		role_list[i].start_game = true;
// 
// 		int team_mate_seq = i == 0 ? 1 : 0;
// 		if (!role_list[team_mate_seq].start_game)			// 如果队友没准备就提示一下 如果准备了就直接开始
// 		{
// 			Role * team_role = m_scene->GetRoleByUID(UidToInt(role_list[team_mate_seq].user_id));
// 			if (NULL != team_role)
// 			{
// 				team_role->NoticeNum(noticenum::NT_QINGYUAN_DUIDUIPENG_START_GAME);
// 			}
// 		}
// 
// 		break;
// 	}

	if (begin_game_times <= 0)
	{
		begin_game_times = EngineAdapter::Instance().Time() + 10;
		end_game_times = begin_game_times + LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().during_time;

		this->SendGameStart();
	}
}

void SpecialLogicQingYuanDuiDuiPeng::CompareForm()
{
	bool is_fb_finish = false;
	//积分计算规则：每验证成功加10分，连续验证成功，每次额外增加{（X-1）*5}分，X为当前连续验证成功的次数。
	if (role_list[0].npc_id != role_list[1].npc_id && role_list[0].seq == role_list[1].seq && role_list[0].seq != 0)
	{
		// 配对成功
		int reward_integral = (continuity + 1 - 1) * 5;
		int add_integral = 10 + reward_integral;

		int length = 0;
		if (continuity == 0)
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_jifen, add_integral);
		}
		else
		{
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_jifen_1, 
				add_integral, reward_integral);
		}
		
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgScene(m_scene, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}

		integral += add_integral;
		continuity += 1;
		succ_match += 1;

		m_scene->DeleteObj(role_list[0].npc_obj_id);
		m_scene->DeleteObj(role_list[1].npc_obj_id);
		m_npc_list.erase(role_list[0].npc_obj_id);
		m_npc_list.erase(role_list[1].npc_obj_id);
		
		int reward_times = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetRewardTimeCfg(succ_match);
		if (reward_times > 0)
		{
			int times = EngineAdapter::Instance().Time() - begin_game_times;
			if (times <= reward_times) reward_level += 1;
		}

		if (m_npc_list.empty()) is_fb_finish = true;
		this->SendFbInfo();
	}
	else
	{
		// 配对失败
		continuity = 0;

		Protocol::SCNoticeNum scnn;
		scnn.notice_num = errornum::EN_QINGYUAN_DUIDUIPNEG_PEIDUI_NOT;

		m_scene->SendToRole((const char *)&scnn, sizeof(scnn));
	}

	for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
	{
		role_list[i].ResetForm();
	}

	this->SendRoleForm();

	if (is_fb_finish) this->OnFinish(true);
}

void SpecialLogicQingYuanDuiDuiPeng::OnFinish(bool is_finish)
{
	if (reward_level <= 0)
		is_finish = false;

	if (is_finish)
	{
		//最后剩余时间转化为积分，总时间为180秒，则最后积分=验证成功积分*7+额外增加积分+剩余时间积分
		int add_integral = end_game_times - EngineAdapter::Instance().Time();
		if (add_integral > 0) integral += add_integral;
	
		for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
		{
			Role * role = m_scene->GetRoleByUID(UidToInt(role_list[i].user_id));
			if (NULL == role) continue;

			// 发奖励
			if (role->GetRoleModuleManager()->GetCommonData().qingyuan_duiduipeng_times < LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().reward_times)
			{
				role->GetRoleModuleManager()->GetCommonData().qingyuan_duiduipeng_times += 1;

				std::vector<ItemConfigData> reward;

				switch (reward_level)
				{
				case 1:
					{
						reward = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().reward_1;
					}
					break;
				case 2:
					{
						reward = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().reward_2;
					}
					break;
				case 3:
					{
						reward = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().reward_3;
					}
					break;
				}

				if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList2(reward, false))
				{
					role->GetRoleModuleManager()->GetKnapsack()->PutVec(reward, PUT_REASON_QINGYUAN_DUIDUIPENG);
				}
				else
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(reward, SNED_MAIL_TYPE_DEFAULT, true);
				}
			}

			int pass_times = EngineAdapter::Instance().Time() - begin_game_times;

			// 更新排行榜
			if (CrossConfig::Instance().IsHiddenServer())
			{
				GameName name; 	role->GetName(name);		
				QingYuanDuiDuiPengManager::Instance().OnRoleUpDate(role->GetUID(), name, integral, pass_times);
			}
			else
			{
				static crossgameprotocal::GameHiddenQingYuanDuiDuiPengUpDataRankInfo info;
				info.role_id = role->GetUID();
				role->GetName(info.name);
				info.jifen = integral;
				info.pass_times = pass_times;

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&info, sizeof(info));
			}
		}
	}
	
	end_game_times = 0;
	role_leave_times = EngineAdapter::Instance().Time() + 5;

	// 发送副本结算通知
	Protocol::SCQingYuanDuiDuiFinish info;
	info.is_finish = is_finish;

	m_scene->SendToRole((const char *)&info, sizeof(info));
}

void SpecialLogicQingYuanDuiDuiPeng::OnNpcForm(Role * role, ObjID npc_objid)
{
	if (NULL == role) return;

	std::map<ObjID, NpcData>::iterator it = m_npc_list.find(npc_objid);
	if (it == m_npc_list.end())
		return;

	for (int i = 0; i < ARRAY_LENGTH(role_list); i++)
	{
		if (role->GetUserId() != role_list[i].user_id)
			continue;

		role_list[i].npc_obj_id = npc_objid;
		role_list[i].seq = it->second.seq;
		role_list[i].npc_id = it->second.npc_id;

		{
			int length_1 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_prom, it->second.npc_id);;
			if (length_1 > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_list[i].user_id, gamestring::GAMESTRING_BUF, length_1, GetMsgType(SYS_MSG_FLOAT));
			}
		}
		{
			int length_2 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_qingyuan_duiduipeng_prom_1,
				role->GetUID(), role->GetName(), it->second.npc_id);;
			if (length_2 > 0)
			{
				int team_mate_seq = i == 0 ? 1 : 0;
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_list[team_mate_seq].user_id, gamestring::GAMESTRING_BUF, length_2, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT));
			}
		}
		break;
	}

	this->SendRoleForm();
}

void SpecialLogicQingYuanDuiDuiPeng::SendGameStart()
{
	Protocol::SCQingYuanDuiDuiPengStartNotice info;
	info.begin_game_times = begin_game_times;
	info.end_game_times = end_game_times;

	m_scene->SendToRole((const char *)&info, sizeof(info));
}

void SpecialLogicQingYuanDuiDuiPeng::SendRoleForm()
{
	Protocol::SCQingYuanDuiDuiRoleForm info;
	for (int i = 0; i < ARRAY_LENGTH(role_list) && i < ARRAY_LENGTH(info.role_form); i++)
	{
		info.role_form[i].uid = UidToInt(role_list[i].user_id);
		info.role_form[i].form = role_list[i].npc_id;
	}

	m_scene->SendToRole((const char *)&info, sizeof(info));
}

void SpecialLogicQingYuanDuiDuiPeng::SendFbInfo(Role * role)
{
	Protocol::SCQingYuanDuiDuiFbInfo info;
	info.integral = integral;
	info.succ_match = succ_match;
	info.reward_level = reward_level;

	if (NULL == role)
	{
		m_scene->SendToRole((const char *)&info, sizeof(info));
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
}
