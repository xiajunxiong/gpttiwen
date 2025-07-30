#include "speciallogicreliclooting.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "servercommon/anecdotetaskdef.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "global/team/teammanager.hpp"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "protocol/msgscene.h"
#include "other/buffmanager/buffmanager.hpp"
#include "other/buffmanager/buffbase.hpp"
#include "other/rolemodulemanager.hpp"
#include "npc/npcmanager.h"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "obj/character/npc_gather.h"
#include "other/robot/robotmanager.hpp"
#include "servercommon/robotdef.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/noticenum.h"
#include "other/event/eventhandler.hpp"
#include "gamelog.h"

bool static SortFunc(const PlayerInfo &rank_item_1, const PlayerInfo &rank_item_2)
{
	return (rank_item_1.gongxun > rank_item_2.gongxun);
}

SpecialLogicRelicLootingFb::SpecialLogicRelicLootingFb(Scene * scene) : SpecialLogic(scene)
{
	check_out_follow_tamp = 0;
	all_member_ready = false;//是否所有玩家都准备好了
	member_enter_wait_tamp = 0;//进场等待10秒-等客户端返回玩家到位完毕才准备倒计时
	fb_ready_tamp = 0;// 准备时间
	m_fb_end_tamp = 0;//游戏结束时间戳
	m_kick_out_tamp = 0;
	m_drop_skill_tamp = 0;//掉落技能时间戳
	win_type = Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_DRAW;//胜利类型

	allow_move = true;//玩家是否允许移动

	defender_num = 0;
	memset(defender_list, 0, sizeof(defender_list));

	attacker_num = 0;
	memset(attacker_list, 0, sizeof(attacker_list));

	//m_team_recode_vec.clear();//队伍记录
	next_refresh_team_member_pos_tamp = 0;
}

SpecialLogicRelicLootingFb::~SpecialLogicRelicLootingFb()
{
}

void SpecialLogicRelicLootingFb::OnRoleEnterScene(Role * role)
{
	//给玩家加载预设技能
	ActivityRelicLootingConfig*  cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig();
	if (cfg == NULL)return;
	bool is_defender = false;
	PlayerInfo * player_info = this->GetPlayerInfo(role->GetUID(), &is_defender);
	if (player_info == NULL)return;
	player_info->on_line = 1;
	const std::map<int, RelicLootingSkillCfg> * all_skill_cfg = cfg->GetSkillInfoList();
	if (all_skill_cfg != NULL)
	{
		for (std::map<int, RelicLootingSkillCfg>::const_iterator it = all_skill_cfg->begin(); it != all_skill_cfg->end(); it++)//写入各阵营默认技能
		{
			if (player_info != NULL && is_defender)
			{
				if (it->second.skill_limit == RelicLootingSkillCfg::SKILL_LIMIT_1)
				{
					if (player_info->skill_list.common_skill_num >= PlayerSkillList::MAX_COMMON_SKILL_NUM)continue;
					player_info->skill_list.m_common_skill[player_info->skill_list.common_skill_num].sn = it->second.sn;
					player_info->skill_list.common_skill_num++;
				}
			}
			else if(player_info != NULL && !is_defender)
			{
				if (it->second.skill_limit == RelicLootingSkillCfg::SKILL_LIMIT_2)
				{
					if (player_info->skill_list.common_skill_num >= PlayerSkillList::MAX_COMMON_SKILL_NUM)continue;
					player_info->skill_list.m_common_skill[player_info->skill_list.common_skill_num].sn = it->second.sn;
					player_info->skill_list.common_skill_num++;
				}
			}
		}
	}
	this->SendPlayerSkillInfo(role);
	this->SendObjInfo(role);
	this->SendTeamInfo(role);
	if (fb_ready_tamp > 0)
	{
		this->SendSceneTampInfoToAll(Protocol::SCRelicLootingTampInfo::RELIC_LOOTING_TYPE_READY_START, fb_ready_tamp);
	}

	if (m_fb_end_tamp > 0)
	{
		this->SendSceneTampInfoToAll(Protocol::SCRelicLootingTampInfo::RELIC_LOOTING_TYPE_BEGAIN, m_fb_end_tamp);
	}

	if (check_out_follow_tamp == 0)
	{
		check_out_follow_tamp = (unsigned int)EngineAdapter::Instance().Time() + 1;
	}
}

void SpecialLogicRelicLootingFb::OnRoleLeaveScene(Role * role, bool is_logout)
{
	role->GetRoleStatusManager()->OnRoleStopMoveStatus();
	this->OnRoleStopMoveStatus(role->GetUID());
	PlayerInfo * player = this->GetPlayerInfo(role->GetUID());
	if (NULL != player)
	{
		player->on_line = 0;
		this->SendTeamInfo();
		this->OnCheckAllAttackerBeCatch();
	}
}

void SpecialLogicRelicLootingFb::OnRoleLeaveTeam(Role * role)
{
}

bool SpecialLogicRelicLootingFb::CanMoveOn(Role * role)
{
	if (!allow_move)
	{
		role->NoticeNum(errornum::EN_RELICLOOTING_CANNT_MOVE);
	}

	//检查人物移动状态
	int move_mod = role->GetRoleStatusManager()->GetRoleMoveStatus();
	if (move_mod == RoleStatusManager::ROLE_MOVE_STATUS_SHIFA)
	{
		return false;
	}

	PlayerInfo * player = this->GetPlayerInfo(role->GetUID());
	if (NULL != player)
	{
		if (player->stop_reason == PlayerInfo::STOP_MOVE_REASON_BE_CATCH)
		{
			//被抓住了不能动
			role->NoticeNum(errornum::EN_RELICLOOTING_BE_CATCH);//你被抓住了，无法动弹，请呼叫队友援助
			return false;
		}

		if (player->stop_reason == PlayerInfo::STOP_MOVE_REASON_BE_TRAP)
		{
			//被陷阱夹住了
			role->NoticeNum(errornum::EN_RELICLOOTING_BE_TRAP);//你被夹住了，无法动弹，请呼叫队友援助
			return false;
		}
	}

	return allow_move;
}

void SpecialLogicRelicLootingFb::Update(unsigned long interval, time_t now_second)
{
	if (check_out_follow_tamp > 0 && check_out_follow_tamp <= now_second)
	{
		check_out_follow_tamp = 0;
		//this->CheckAllTeamHasOutFollow();
	}

	if (member_enter_wait_tamp > 0 && member_enter_wait_tamp <= now_second)
	{
		member_enter_wait_tamp = 0;
		//加载时间到了，开始倒计时
		fb_ready_tamp = (unsigned int)now_second + 3;
		this->SendSceneTampInfoToAll(Protocol::SCRelicLootingTampInfo::RELIC_LOOTING_TYPE_READY_START, fb_ready_tamp);
	}

	if (fb_ready_tamp > 0 && fb_ready_tamp <= now_second)
	{
		fb_ready_tamp = 0;
		//倒计时结束，开始游戏
		this->OnGameBegin();
		m_fb_end_tamp = (unsigned int)now_second + 480;
		this->SendSceneTampInfoToAll(Protocol::SCRelicLootingTampInfo::RELIC_LOOTING_TYPE_BEGAIN, m_fb_end_tamp);
	}

	if (m_fb_end_tamp > 0 && m_fb_end_tamp <= (unsigned int)now_second)
	{
		//副本游戏结束
		m_fb_end_tamp = 0;
		//结算
		win_type = Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_D;
		this->OnPlayEnd(win_type);
		m_kick_out_tamp = (unsigned int)now_second + 10;//10秒后踢出副本
	}

	if (m_kick_out_tamp > 0 && m_kick_out_tamp < (unsigned int)now_second)
	{
		this->DelayKickOutAllRole();
	}

	if (m_drop_skill_tamp > 0 && m_drop_skill_tamp < (unsigned int)now_second)
	{
		this->RefreshSkillBox();
		const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
		m_drop_skill_tamp = (unsigned int)now_second + other_cfg.props_time;
	}

	if (next_refresh_team_member_pos_tamp < now_second)
	{
		this->SendTeamMemberPos();
		next_refresh_team_member_pos_tamp = now_second + 1;
	}

	for (std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = skill_belog_map.begin(); it != skill_belog_map.end(); it++)
	{
		if(it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_TRAP)
		{
			if (it->second.param > 0 && it->second.param <= now_second)
			{
				std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it_obj = m_obj_map.find(it->second.param2);
				if (it_obj != m_obj_map.end())
				{
					Obj * obj = m_scene->GetObj(it_obj->first);
					if (NULL != obj && obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
					{
						m_scene->DeleteObj(obj->GetId());
						m_obj_map.erase(it_obj);
					}
				}
				skill_belog_map.erase(it);
				break;
			}
			ObjID obj_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM + RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//场景最大人数
			//bool catch_member = false;
			const RelicLootingSkillCfg * skill_detail = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(RelicLootingSkillCfg::SKILL_ID_11);
			if (skill_detail != NULL)
			{
				int count = m_scene->GetZoneMatrix()->GetObjByCircle(it->second.obj_pos, skill_detail->param3, obj_list, sizeof(obj_list) / sizeof(obj_list[0]));
				bool effect = false;
				for (int i = 0; i < count && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM + RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; ++i)
				{
					Role *target_role = m_scene->GetRoleByObjID(obj_list[i]);
					if (NULL != target_role && it->first != target_role->GetUID())
					{
						bool is_defender = false;
						PlayerInfo * target = this->GetPlayerInfo(target_role->GetUID(), &is_defender);
						if (target != NULL && target->stop_reason == PlayerInfo::STOP_MOVE_REASON_NORMAL)
						{
							//直接夹住这个玩家
							target->stop_reason = PlayerInfo::STOP_MOVE_REASON_BE_TRAP;
							target->param = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param2;
							effect = true;
							break;
						}
					}
				}
				if (effect)
				{
					std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it_obj = m_obj_map.find(it->second.param2);
					if (it_obj != m_obj_map.end())
					{
						Obj * obj = m_scene->GetObj(it_obj->first);
						if (NULL != obj && obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
						{
							m_scene->DeleteObj(obj->GetId());
							m_obj_map.erase(it_obj);
						}
					}
					//使用效果完毕，陷阱移除
					skill_belog_map.erase(it);
					break;
				}
			}
		}
		else if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_ROBOT)
		{
			if (it->second.param > 0 && it->second.param <= now_second)
			{
				//玩家回到原点
				Role *target_role = m_scene->GetRoleByUID(UidToInt(it->first));
				if (target_role != NULL)
				{
					this->Transmt(target_role, &it->second.obj_pos);
				}
				std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it_obj = m_obj_map.find(it->second.param2);
				if (it_obj != m_obj_map.end())
				{
					Obj * obj = m_scene->GetObj(it_obj->first);
					if (NULL != obj && obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
					{
						m_scene->DeleteObj(obj->GetId());
						m_obj_map.erase(it_obj);
					}
				}
				skill_belog_map.erase(it);
				break;
			}
		}
		else if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_DOLL)
		{
			if (it->second.param > 0 && it->second.param <= now_second)
			{
				Robot * robot = ROBOTMANAGER.GetRobot(it->second.param2);
				if (NULL != robot)
				{
					ROBOTMANAGER.RemoveRobot(robot, __FUNCTION__);
				}
				skill_belog_map.erase(it);
				break;
			}
		}
	}

	for (int attack_idx = 0; attack_idx < attacker_num && attack_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attack_idx++)
	{
		PlayerInfo &node = attacker_list[attack_idx];
		RelicLootingFBSkillInfo &skill_node = node.skill_list.m_item_skill;
		if (node.stop_reason == PlayerInfo::STOP_MOVE_REASON_BE_TRAP && node.param <= now_second)
		{
			node.stop_reason = PlayerInfo::STOP_MOVE_REASON_NORMAL;
			node.param = 0;
			Role * role = m_scene->GetRoleByUID(node.uid);
			if (role != NULL)
			{
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
			}
		}
		if (skill_node.UsingSkill())
		{
			if ((skill_node.casting_tamp > 0 && skill_node.casting_tamp <= (unsigned int)now_second) || (skill_node.guide_over_tamp > 0 && skill_node.guide_over_tamp <= (unsigned int)now_second))
			{
				//施法或引导结束了，释放技能
				bool is_delete = false;
				Role * role = m_scene->GetRoleByUID(node.uid);
				if (role != NULL)
				{
					this->SkillEffect(role, &skill_node, &is_delete);
				}

				if (is_delete)
				{
					skill_node.sn = 0;
					skill_node.cool_down_tamp = 0;
				}
				skill_node.casting_tamp = 0;
				skill_node.guide_over_tamp = 0;
			}
		}
		else
		{
			if (skill_node.cool_down_tamp > 0 && skill_node.cool_down_tamp <= now_second)
			{
				skill_node.cool_down_tamp = 0;
			}
			if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_7)//轻身符
			{
				if (skill_node.special_param > 0)//需要在加速结束后触发第二个减益buff
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.special_param = 0;
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (role != NULL)
						{
							const RelicLootingSkillCfg * skill_info = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(RelicLootingSkillCfg::SKILL_ID_7);
							if (skill_info != NULL)
							{
								BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(role->GetRoleModuleManager(), (unsigned int)EngineAdapter::Instance().Time(), (unsigned int)EngineAdapter::Instance().Time() + skill_info->param4);
								buff->SetMoveSpeed(skill_info->param5);
								buff->SetSave(false);
								role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
								this->OnRoleStartSoftBodyEnd(role->GetUID());
								this->OnRoleStopMoveStatus(role->GetUID());
							}
							this->SendPlayerSkillInfo(role);
						}
					}
				}
			}
			else if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_8)//稻草人
			{
				if (skill_node.special_param > 0)
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (NULL != role)this->SendPlayerSkillInfo(role);
					}
				}
			}
			else if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_13)//隐身衣
			{
				if (skill_node.special_param > 0)//隐身衣结束
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.special_param = 0;
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (role != NULL)
						{
							role->GetRoleStatusManager()->OnRoleStopMoveStatus();
							this->SendPlayerSkillInfo(role);
						}
					}
				}
			}
		}
		for (int skill_idx = 0; skill_idx < node.skill_list.common_skill_num && skill_idx < PlayerSkillList::MAX_COMMON_SKILL_NUM; skill_idx++)
		{
			RelicLootingFBSkillInfo &skill_node = node.skill_list.m_common_skill[skill_idx];
			if (skill_node.UsingSkill())
			{
				if ((skill_node.casting_tamp > 0 && skill_node.casting_tamp <= (unsigned int)now_second) || (skill_node.guide_over_tamp > 0 && skill_node.guide_over_tamp <= (unsigned int)now_second))
				{
					//施法或引导结束了，释放技能
					Role * role = m_scene->GetRoleByUID(node.uid);
					if (role != NULL)
					{
						this->SkillEffect(role, &skill_node);
					}
					skill_node.casting_tamp = 0;
					skill_node.guide_over_tamp = 0;
				}
			}
			else
			{
				if (skill_node.cool_down_tamp > 0 && skill_node.cool_down_tamp <= now_second)
				{
					skill_node.cool_down_tamp = 0;
				}
				if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_6)//神隐技能
				{
					if (skill_node.special_param > 0)//神隐引导结束后需要检查神隐的结束时间并且把潜行模式关掉
					{
						if (skill_node.special_param <= (unsigned int)now_second)
						{
							skill_node.ClearMove();
							Role * role = m_scene->GetRoleByUID(node.uid);
							if (role != NULL)
							{
								role->GetRoleStatusManager()->OnRoleStopMoveStatus();
								this->SendPlayerSkillInfo(role);
							}
						}
					}
				}
			}
		}
	}

	for (int defend_idx = 0; defend_idx < defender_num && defend_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defend_idx++)
	{
		PlayerInfo &node = defender_list[defend_idx];
		RelicLootingFBSkillInfo &skill_node = node.skill_list.m_item_skill;
		if (node.stop_reason == PlayerInfo::STOP_MOVE_REASON_BE_TRAP && node.param <= now_second)
		{
			node.stop_reason = PlayerInfo::STOP_MOVE_REASON_NORMAL;
			node.param = 0;
			Role * role = m_scene->GetRoleByUID(node.uid);
			if (role != NULL)
			{
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
			}
		}
		if (skill_node.UsingSkill())
		{
			if ((skill_node.casting_tamp > 0 && skill_node.casting_tamp <= (unsigned int)now_second) || (skill_node.guide_over_tamp > 0 && skill_node.guide_over_tamp <= (unsigned int)now_second))
			{
				//施法或引导结束了，释放技能
				bool is_delete = false;
				Role * role = m_scene->GetRoleByUID(node.uid);
				if (role != NULL)
				{
					this->SkillEffect(role, &skill_node, &is_delete);
				}
				if (is_delete)
				{
					skill_node.sn = 0;
					skill_node.cool_down_tamp = 0;
				}
				skill_node.casting_tamp = 0;
				skill_node.guide_over_tamp = 0;
			}
		}
		else
		{
			if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_7)//轻身符
			{
				if (skill_node.special_param > 0)//需要在加速结束后触发第二个减益buff
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.special_param = 0;
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (role != NULL)
						{
							const RelicLootingSkillCfg * skill_info = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(RelicLootingSkillCfg::SKILL_ID_7);
							if (skill_info != NULL)
							{
								BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(role->GetRoleModuleManager(), (unsigned int)EngineAdapter::Instance().Time(), (unsigned int)EngineAdapter::Instance().Time() + skill_info->param4);
								buff->SetMoveSpeed(skill_info->param5);
								buff->SetSave(false);
								role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
								this->OnRoleStartSoftBodyEnd(role->GetUID());
								this->OnRoleStopMoveStatus(role->GetUID());
							}
							this->SendPlayerSkillInfo(role);
						}
					}
				}
			}
			else if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_8)//稻草人
			{
				if (skill_node.special_param > 0)
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (NULL != role)this->SendPlayerSkillInfo(role);
					}
				}
			}
			else if (skill_node.sn == RelicLootingSkillCfg::SKILL_ID_13)//隐身衣
			{
				if (skill_node.special_param > 0)//隐身衣结束
				{
					if (skill_node.special_param <= (unsigned int)now_second)
					{
						skill_node.special_param = 0;
						skill_node.ClearSkill();
						Role * role = m_scene->GetRoleByUID(node.uid);
						if (role != NULL)
						{
							role->GetRoleStatusManager()->OnRoleStopMoveStatus();
							this->SendPlayerSkillInfo(role);
						}
						
					}
				}
			}
		}
		for (int skill_idx = 0; skill_idx < node.skill_list.common_skill_num && skill_idx < PlayerSkillList::MAX_COMMON_SKILL_NUM; skill_idx++)
		{
			RelicLootingFBSkillInfo &skill_node = node.skill_list.m_common_skill[skill_idx];
			if (skill_node.UsingSkill())
			{
				if ((skill_node.casting_tamp > 0 && skill_node.casting_tamp <= (unsigned int)now_second) || (skill_node.guide_over_tamp > 0 && skill_node.guide_over_tamp <= (unsigned int)now_second))
				{
					//施法或引导结束了，释放技能
					Role * role = m_scene->GetRoleByUID(node.uid);
					if (role != NULL)
					{
						this->SkillEffect(role, &skill_node);
					}
					skill_node.casting_tamp = 0;
					skill_node.guide_over_tamp = 0;
				}
			}
		}
	}
}

void SpecialLogicRelicLootingFb::OnAOIEnterRole(Role * role, Role * target_role)
{
	RoleSpecialEffect * info = this->GetRoleSpecialEffect(target_role->GetUID());
	if (info == NULL)return;
	Protocol::SCRelicLootingEffectInfo msg;
	msg.uid = info->uid;
	msg.special_effect_type = info->effect_type;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, sizeof(msg));
}

int SpecialLogicRelicLootingFb::GetXYDistend(int x1, int y1, int x2, int y2)
{
	double _x = x1 - x2;
	double _y = y1 - y2;
	if (_x < 0)_x *= -1;
	if (_y < 0)_y *= -1;
	return (int)sqrt((_x*_x) + (_y*_y));
}

bool SpecialLogicRelicLootingFb::OnEnterFbBeforeSet(Role * role, bool is_defender)
{
	if (is_defender)
	{
		if (defender_num >= RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM || defender_num < 0)return false;//满人了，出现这个基本是BUG
		//检查一下是否有这个玩家了
		if (this->HasUser(role->GetUID()))
		{
			//玩家已经在了
			return false;
		}
		role->GetName(defender_list[defender_num].player_name);
		defender_list[defender_num++].uid = role->GetUID();
	}
	else
	{
		if (attacker_num >= RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM || attacker_num < 0)return false;//满人了，出现这个基本是BUG
		//检查一下是否有这个玩家了
		if (this->HasUser(role->GetUID()))
		{
			//玩家已经在了
			return false;
		}
		role->GetName(attacker_list[attacker_num].player_name);
		attacker_list[attacker_num++].uid = role->GetUID();
	}

	return true;
}

void SpecialLogicRelicLootingFb::TeamRecord(Team * team_info)
{
	/*if (team_info == NULL)return;
	RelicLootingTeamRecodeInfo node;
	node.min_level_limit = team_info->GetLimitLevelLow();
	node.max_level_limit = team_info->GetLimitLevelHigh();
	node.leader_uid = team_info->GetLeaderUID();
	int team_member_count = team_info->GetMemberCount();
	for (int team_idx = 0; team_idx < team_member_count && team_idx < TEAM_MEMBER_MAX_NUM - 1; team_idx++)
	{
		Role* role = team_info->GetMemberRoleByIndex(team_idx);
		if (NULL != role)
		{
			node.member_uid_list[node.member_count++] = role->GetUID();
		}
	}
	//m_team_recode_vec.push_back(node);*/
}

void SpecialLogicRelicLootingFb::GetSkillItem(Role * role, int param1)
{
	std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = m_obj_map.find(param1);
	if (it != m_obj_map.end())
	{
		const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
		const RelicLootingSkillCfg * skill_detail = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(it->second.param);
		int distance = this->GetXYDistend(it->second.obj_pos.x, it->second.obj_pos.y, role->GetPos().x, role->GetPos().y);
		if (distance > other_cfg.duration)return;
		PlayerInfo * player = this->GetPlayerInfo(role->GetUID());
		if (skill_detail != NULL && NULL != player)
		{
			int skill_id = -2;
			if (player->skill_list.m_item_skill.special_param != 0 && player->skill_list.m_item_skill.sn != 0)
			{
				skill_id = -1;//检查通用技能使用时，需要检查是否有特殊参数，也算在使用技能中
			}
			if (skill_id == -1 || player->skill_list.UsingSkill(&skill_id))
			{
				role->NoticeNum(errornum::EN_RELICLOOTING_HAS_ITEM_SKILL);
				return;
			}
			player->skill_list.m_item_skill.sn = skill_detail->sn;
			Obj *obj = m_scene->GetObj(it->first);
			
			if (obj != NULL)m_scene->DeleteObj(obj->GetId());
			m_obj_map.erase(it);
			this->SendPlayerSkillInfo(role);
			this->SendObjInfo(role);
			
		}
	}
}

void SpecialLogicRelicLootingFb::GiveUpSkillItem(Role * role)
{
	PlayerInfo * player = this->GetPlayerInfo(role->GetUID());
	if (NULL != player)
	{
		if (player->skill_list.m_item_skill.UsingSkill())
		{
			return;
		}
		else
		{
			if (player->skill_list.m_item_skill.special_param != 0)return;
			player->skill_list.m_item_skill.sn = 0;
		}
	}
	this->SendPlayerSkillInfo(role);
}

void SpecialLogicRelicLootingFb::OnRoleUseSkill(Role * role, int param1, int param2, int param3, int param4)
{
	PlayerInfo * node_palyer = this->GetPlayerInfo(role->GetUID());
	if (node_palyer == NULL)return;
	RelicLootingFBSkillInfo *  node_skill_info = node_palyer->skill_list.GetSkillInfo(param1);//param1是技能id
	if (node_skill_info == NULL)return;
	const RelicLootingSkillCfg * skill_detail = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(param1);
	if (skill_detail == NULL)return;

	int using_skill_idx = -2;
	if (skill_detail->skill_limit == RelicLootingSkillCfg::SKILL_LIMIT_0)
	{
		if (node_palyer->skill_list.m_item_skill.special_param != 0 && node_palyer->skill_list.m_item_skill.sn != 0)
		{
			using_skill_idx = -1;//检查通用技能使用时，需要检查是否有特殊参数，也算在使用技能中
		}
	}

	if (using_skill_idx == -1 || node_palyer->skill_list.UsingSkill(&using_skill_idx))
	{
		//还在用技能，强制打断现在使用的技能然后进入冷却时间
		if (using_skill_idx == -1)//道具技能打断就没有了
		{
			if (node_skill_info->sn == node_palyer->skill_list.m_item_skill.sn)return;//同一个，直接返回
			node_palyer->skill_list.m_item_skill.ClearSkill();
		}
		else if(using_skill_idx >= 0 && using_skill_idx <= PlayerSkillList::MAX_COMMON_SKILL_NUM && using_skill_idx < node_palyer->skill_list.common_skill_num)
		{
			RelicLootingFBSkillInfo &using_skill = node_palyer->skill_list.m_common_skill[using_skill_idx];
			if (node_skill_info->sn == using_skill.sn)return;//同一个，直接返回
			const RelicLootingSkillCfg * use_skill_detail = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(using_skill.sn);
			if (use_skill_detail != NULL)
			{
				switch (use_skill_detail->sn)
				{
				case RelicLootingSkillCfg::SKILL_ID_1:
				case RelicLootingSkillCfg::SKILL_ID_6:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param3;
					break;
				case RelicLootingSkillCfg::SKILL_ID_2:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param4;
					break;
				case RelicLootingSkillCfg::SKILL_ID_3:
				case RelicLootingSkillCfg::SKILL_ID_4:
				case RelicLootingSkillCfg::SKILL_ID_5:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param2;
					break;
				default:
					break;
				}
				using_skill.casting_tamp = 0;
				using_skill.guide_over_tamp = 0;
			}
		}
		role->GetRoleStatusManager()->OnRoleStopMoveStatus();
		this->OnRoleStopMoveStatus(role->GetUID());
		this->SendPlayerSkillInfo(role);
	}

	if (node_skill_info->cool_down_tamp > EngineAdapter::Instance().Time())return;
	
	switch (skill_detail->sn)
	{
	case RelicLootingSkillCfg::SKILL_ID_1:
		{
			//玩家状态进入引导，并且引导结束后以玩家所在为半径直接抓捕敌方单位
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->OnRoleStartCatch(role->GetUID());
			/*node_skill_info->special_param = param2;//玩家objid*/
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_2:
		{
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->OnRoleStartSprint(role->GetUID());
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_3:
		{
			role->GetRoleStatusManager()->OnRoleStartShiFa();
			this->OnRoleStartTransfer(role->GetUID());
			node_skill_info->casting_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_4:
		{
			role->GetRoleStatusManager()->OnRoleStartShiFa();
			this->OnRoleStartUnlock(role->GetUID());
			node_skill_info->special_param = param2;//param2是对应箱子的objid
			node_skill_info->casting_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_5:
		{
			role->GetRoleStatusManager()->OnRoleStartShiFa();
			this->OnRoleStartHelp(role->GetUID());
			node_skill_info->casting_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
			node_skill_info->special_param = param2;//param2是对应玩家的objid
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_6:
		{
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->OnRoleStartSneack(role->GetUID());
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_7:
		{
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->OnRoleStartSoftBody(role->GetUID());
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_8:
		{
			if (node_skill_info->special_param > 0)//稻草人已经被激活
			{
				std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = skill_belog_map.find(role->GetUserId());
				if (it != skill_belog_map.end() && it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_ROBOT)
				{
					this->Transmt(role, &it->second.obj_pos);
					std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it_obj = m_obj_map.find(it->second.param2);
					if (it_obj != m_obj_map.end())
					{
						Obj * obj = m_scene->GetObj(it_obj->first);
						if (NULL != obj && obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
						{
							m_scene->DeleteObj(obj->GetId());
							m_obj_map.erase(it_obj);
						}
					}
					skill_belog_map.erase(it);
				}
				node_skill_info->ClearSkill();
				this->OnRoleStopMoveStatus(role->GetUID());
				break;//跳出
			}
			this->HasUserSkillBelong(role);
			role->GetRoleStatusManager()->OnRoleStartShiFa();
			this->OnRoleStartDaoCaoRen(role->GetUID());
			node_skill_info->casting_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_9:
		{
			role->GetRoleStatusManager()->OnRoleStartShiFa();
			this->OnRoleStartTransfer(role->GetUID());
			node_skill_info->casting_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_10:
		{
			//询问队友是否传送过来
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->ChuanSongFaZhen(role, skill_detail);
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_11:
		{
			this->HasUserSkillBelong(role);
			NPCGather * monster = NpcManager::Instance().CreateNPCGather(skill_detail->param3, m_scene, role->GetPos());
			if (monster != NULL)
			{
				Protocol::SCRelicLootingObjInfo::Objinfo &node = skill_belog_map[role->GetUserId()];//这操作会顶替上一个道具技能的belong
				node.obj_pos = monster->GetPos();
				node.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_TRAP;
				node.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
				node.param = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param2;
				node.param2 = monster->GetId();

				Protocol::SCRelicLootingObjInfo::Objinfo &node2 = m_obj_map[monster->GetId()];
				node2.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_TRAP;
				node2.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
				node2.obj_pos = monster->GetPos();
			}
			node_skill_info->ClearSkill();
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_12:
		{
			this->OnRoleStartDoll(role->GetUID());
			RobotParam robot_param;
			if (!ROBOTMANAGER.ConstructRobotParamByLevel(role->GetLevel(), ROBOT_CONFIG_USE_TYPE_ROLE_SHADOW, false, &robot_param, role->GetUserId().db_index))
			{
				return;
			}
			robot_param.robot_type = ROBOT_TYPE_NORMAL;
			F_STRNCPY(robot_param.robot_name, role->GetName(), sizeof(GameName));
			F_STRNCPY(robot_param.role_battle_data.role_character_data.name, role->GetName(), sizeof(GameName));
			robot_param.role_battle_data.appearance = role->GetRoleModuleManager()->GetAppearance();

			Robot* robot = ROBOTMANAGER.CreateRobotToScene(m_scene, role->GetPos(), robot_param, __FUNCTION__);
			if (robot != NULL)
			{
				this->HasUserSkillBelong(role);
				Protocol::SCRelicLootingObjInfo::Objinfo &node = skill_belog_map[role->GetUserId()];
				node.obj_pos = robot->GetPos();
				node.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_DOLL;
				node.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
				node.param = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
				node.param2 = robot->GetRobotID();
				node_skill_info->ClearSkill();
			}
			this->OnRoleStopMoveStatus(role->GetUID());
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_13:
		{
			role->GetRoleStatusManager()->OnRoleStartYinDao();
			this->OnRoleStartSneack(role->GetUID());
			node_skill_info->guide_over_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_detail->param1;
		}
		break;
	case RelicLootingSkillCfg::SKILL_ID_14:
		{
			this->OnRoleStartRefresh(role->GetUID());
			int distance = this->GetXYDistend(role->GetPos().x, role->GetPos().y, param2, param3);//param2，x param3，y
			if (distance <= skill_detail->param1)
			{
				Posi new_pos(param2, param3);
				this->RefreshPos(role, &new_pos, distance);
			}
			node_skill_info->ClearSkill();
			this->OnRoleStopMoveStatus(role->GetUID());
		}
		break;
	default:
		break;
	}
	this->SendPlayerSkillInfo(role);
}

void SpecialLogicRelicLootingFb::GiveUpSkillStatus(Role * role)
{
	PlayerInfo * node_palyer = this->GetPlayerInfo(role->GetUID());
	if (node_palyer == NULL)return;
	int using_skill_idx = -2;
	if (node_palyer->skill_list.UsingSkill(&using_skill_idx))
	{
		//还在用技能，强制打断现在使用的技能然后进入冷却时间
		if (using_skill_idx == -1)//道具技能打断就没有了
		{
			node_palyer->skill_list.m_item_skill.sn = 0;
			node_palyer->skill_list.m_item_skill.casting_tamp = 0;
			node_palyer->skill_list.m_item_skill.guide_over_tamp = 0;
			node_palyer->skill_list.m_item_skill.cool_down_tamp = 0;
		}
		else if (using_skill_idx >= 0 && using_skill_idx <= PlayerSkillList::MAX_COMMON_SKILL_NUM && using_skill_idx < node_palyer->skill_list.common_skill_num)
		{
			RelicLootingFBSkillInfo &using_skill = node_palyer->skill_list.m_common_skill[using_skill_idx];
			const RelicLootingSkillCfg * use_skill_detail = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(using_skill.sn);
			if (use_skill_detail != NULL)
			{
				switch (use_skill_detail->sn)
				{
				case RelicLootingSkillCfg::SKILL_ID_1:
				case RelicLootingSkillCfg::SKILL_ID_6:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param3;
					break;
				case RelicLootingSkillCfg::SKILL_ID_2:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param4;
					break;
				case RelicLootingSkillCfg::SKILL_ID_3:
				case RelicLootingSkillCfg::SKILL_ID_4:
				case RelicLootingSkillCfg::SKILL_ID_5:
					using_skill.cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + use_skill_detail->param2;
					break;
				default:
					break;
				}
				using_skill.casting_tamp = 0;
				using_skill.guide_over_tamp = 0;
			}
		}
		this->HasUserSkillBelong(role);
		role->GetRoleStatusManager()->OnRoleStopMoveStatus();
		this->OnRoleStopMoveStatus(role->GetUID());
		this->SendPlayerSkillInfo(role);
	}
}

void SpecialLogicRelicLootingFb::OnRoleTs(Role * role, int member_uid)
{
	PlayerInfo * player = this->GetPlayerInfo(member_uid);
	if (player != NULL)
	{
		if (player->skill_list.m_item_skill.sn == RelicLootingSkillCfg::SKILL_ID_10)
		{
			Role * target = m_scene->GetRoleByUID(member_uid);
			if (NULL != target)
			{
				Posi target_pos = target->GetPos();
				this->Transmt(role, &target_pos);
			}
			
		}
	}
}

void SpecialLogicRelicLootingFb::SendMake(Role * role, int mark_type, int pos_x, int pos_y)
{
	Protocol::SCRelicLootingMarkInfo info;
	info.marker_uid = role->GetUID();
	info.mark_type = mark_type;
	info.mark_pos.x = pos_x;
	info.mark_pos.y = pos_y;
	bool is_defender = false;
	if (this->GetPlayerInfo(role->GetUID(), &is_defender))
	{
		if (is_defender)
		{
			for (int i = 0; i < defender_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM;i++)
			{
				Role * member = m_scene->GetRoleByUID(defender_list[i].uid);
				if (NULL != member)EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&info, sizeof(info));
			}
		}
		else
		{
			for (int i = 0; i < attacker_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
			{
				Role * member = m_scene->GetRoleByUID(attacker_list[i].uid);
				if (NULL != member)EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&info, sizeof(info));
			}
		}
	}
}

void SpecialLogicRelicLootingFb::InquiryTransmt(Role *role, Role * member, unsigned int end_tamp)
{
	Protocol::SCRelicLootingMemberTransmitInfo info;
	info.uid = role->GetUID();
	info.end_tamp = end_tamp;
	F_STRNCPY(info.player_name, role->GetName(), sizeof(GameName));
	EngineAdapter::Instance().NetSend(member->GetNetId(), (const char*)&info, sizeof(info));
}

void SpecialLogicRelicLootingFb::SendObjInfo(Role * role)
{
	Protocol::SCRelicLootingObjInfo info;
	info.obj_count = 0;
	memset(info.obj_list, 0, sizeof(info.obj_list));
	for (std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = m_obj_map.begin(); it != m_obj_map.end() && info.obj_count < Protocol::SCRelicLootingObjInfo::MAX_OBJ_NUM; it++)
	{
		if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_BOX)
		{
			info.obj_list[info.obj_count].obj_type = it->second.obj_type;
			info.obj_list[info.obj_count].status = it->second.status;
			info.obj_list[info.obj_count].param = it->second.param;
			info.obj_list[info.obj_count].obj_pos = it->second.obj_pos;
			info.obj_count += 1;
		}
		else if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_SKILL)
		{
			info.obj_list[info.obj_count].obj_type = it->second.obj_type;
			info.obj_list[info.obj_count].status = it->second.status;
			info.obj_list[info.obj_count].param = it->second.param;
			info.obj_list[info.obj_count].obj_pos = it->second.obj_pos;
			info.obj_count += 1;
		}
	}

	if (role != NULL)
	{
		std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = skill_belog_map.find(role->GetUserId());
		if (it != skill_belog_map.end())
		{
			info.obj_list[info.obj_count].obj_type = it->second.obj_type;
			info.obj_list[info.obj_count].status = it->second.status;
			info.obj_list[info.obj_count].param = it->second.param;
			info.obj_list[info.obj_count].obj_pos = it->second.obj_pos;
			info.obj_count += 1;
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info) - (Protocol::SCRelicLootingObjInfo::MAX_OBJ_NUM - info.obj_count) * sizeof(Protocol::SCRelicLootingObjInfo::Objinfo));
	}
	else
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
}

void SpecialLogicRelicLootingFb::SendTeamMemberPos()
{
	static GSNetID send_netid[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	int send_num = 0;
	Protocol::SCRelicLootingMemberPosInfo info;
	memset(info.member_list, 0, sizeof(info.member_list));
	info.member_num = 0;
	//盗宝
	for (int i = 0; i < attacker_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
	{
		info.member_list[info.member_num].uid = attacker_list[i].uid;
		Role * role = m_scene->GetRoleByUID(attacker_list[i].uid);
		if (role != NULL)
		{
			send_netid[send_num++] = role->GetNetId();
			info.member_list[info.member_num++].pos = role->GetPos();
		}
	}
	EngineAdapter::Instance().NetMulSend(send_netid, send_num, (const char*)&info, sizeof(info) - (Protocol::SCRelicLootingMemberPosInfo::MAX_TEAM_INFO_MEMBER_NUM - info.member_num) * sizeof(Protocol::SCRelicLootingMemberPosInfo::MemberPos));
	//守卫
	static GSNetID send_netid2[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];
	int send_num2 = 0;
	memset(info.member_list, 0, sizeof(info.member_list));
	info.member_num = 0;
	for (int i = 0; i < defender_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
	{
		info.member_list[info.member_num].uid = defender_list[i].uid;
		Role * role = m_scene->GetRoleByUID(defender_list[i].uid);
		if (role != NULL)
		{
			send_netid2[send_num2++] = role->GetNetId();
			info.member_list[info.member_num++].pos = role->GetPos();
		}
	}
	EngineAdapter::Instance().NetMulSend(send_netid2, send_num2, (const char*)&info, sizeof(info) - (Protocol::SCRelicLootingMemberPosInfo::MAX_TEAM_INFO_MEMBER_NUM - info.member_num) * sizeof(Protocol::SCRelicLootingMemberPosInfo::MemberPos));
}

void SpecialLogicRelicLootingFb::SendTeamInfo(Role * role)
{
	Protocol::SCRelicLootingTeamInfoReq info;

	for (int i = 0; i < attacker_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
	{
		info.attack_team_list[i].uid = attacker_list[i].uid;
		info.attack_team_list[i].team_type = Protocol::SCRelicLootingTeamInfoReq::TEAM_TYPE_ATTACKER;
		info.attack_team_list[i].gongxian = attacker_list[i].gongxun;
		info.attack_team_list[i].stop_reason = attacker_list[i].stop_reason;
		info.attack_team_list[i].on_line = attacker_list[i].on_line;
		F_STRNCPY(info.attack_team_list[i].play_name, attacker_list[i].player_name, sizeof(GameName));
	}

	for (int i = 0; i < defender_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
	{
		info.defend_team_list[i].uid = defender_list[i].uid;
		info.defend_team_list[i].team_type = Protocol::SCRelicLootingTeamInfoReq::TEAM_TYPE_DEFENDER;
		info.defend_team_list[i].gongxian = defender_list[i].gongxun;
		info.defend_team_list[i].stop_reason = defender_list[i].stop_reason;
		info.defend_team_list[i].on_line = defender_list[i].on_line;
		F_STRNCPY(info.defend_team_list[i].play_name, defender_list[i].player_name, sizeof(GameName));
	}

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
	else
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
}

void SpecialLogicRelicLootingFb::OnMemberAllEnterFb()
{
	member_enter_wait_tamp = (unsigned int)EngineAdapter::Instance().Time() + 2;

	//把阵营玩家迁移到对应位置等待
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();

	for (int defender_idx = 0; defender_idx < defender_num && defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defender_idx++)
	{
		PlayerInfo &player = defender_list[defender_idx];
		Role * role = m_scene->GetRoleByUID(player.uid);
		if (NULL != role)
		{
			role->ResetPos(other_cfg.defender_pos_list[defender_idx].x, other_cfg.defender_pos_list[defender_idx].y);
			RoleSpecialEffect & node = m_special_effect_map[role->GetUID()];
			node.uid = role->GetUID();
			node.effect_type = RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_NORMAL;
		}
	}

	for (int attacker_idx = 0; attacker_idx < attacker_num && attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_idx++)
	{
		PlayerInfo &player = attacker_list[attacker_idx];
		Role * role = m_scene->GetRoleByUID(player.uid);
		if (NULL != role)
		{
			role->ResetPos(other_cfg.attacker_pos_list[attacker_idx].x, other_cfg.attacker_pos_list[attacker_idx].y);
			RoleSpecialEffect & node = m_special_effect_map[role->GetUID()];
			node.uid = role->GetUID();
			node.effect_type = RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_NORMAL;
		}
	}
	all_member_ready = false;
	allow_move = false;
	this->SendSceneTampInfoToAll(Protocol::SCRelicLootingTampInfo::RELIC_LOOTING_TYPE_ROLE_LOGIN_STATUS, member_enter_wait_tamp);
	this->SendTeamInfo();
}

void SpecialLogicRelicLootingFb::OnGameBegin()
{
	allow_move = true;
	all_member_ready = true;
	play_time = EngineAdapter::Instance().Time();
	m_drop_skill_tamp = play_time + LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg().first_props;
	this->CreateBox();

	for (int defender_index = 0; defender_index < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defender_index++)
	{
		if (defender_list[defender_index].uid != 0)
		{
			Role * role = m_scene->GetRoleByUID(defender_list[defender_index].uid);
			if (NULL != role)
			{
				EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_RELIC_LOOTING, __FUNCTION__);
				Team* m_team = role->GetMyTeam();
				if (m_team != NULL)
				{
					if (!m_team->IsLeader(role))
					{
						TeamMember* member_info = m_team->GetMemberInfo(role);
						if (NULL != member_info && member_info->IsFollow())m_team->OnTemporarilyPart(role);//开始游戏时需要全员暂离
					}
				}
			}
			else
			{
				defender_list[defender_index].on_line = 0;
			}
			gamelog::g_log_fb_other.printf(LL_INFO, "%s defender[%d,%s] idx[%d]", __FUNCTION__, defender_list[defender_index].uid,
				defender_list[defender_index].player_name, defender_index);
		}
	}

	for (int attacker_index = 0; attacker_index < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_index++)
	{
		if (attacker_list[attacker_index].uid != 0)
		{
			Role * role = m_scene->GetRoleByUID(attacker_list[attacker_index].uid);
			if (NULL != role)
			{
				EventHandler::Instance().OnParticipateLimitActivity(role, ACTIVITY_TYPE_RELIC_LOOTING, __FUNCTION__);
				Team* m_team = role->GetMyTeam();
				if (m_team != NULL)
				{
					if (!m_team->IsLeader(role))
					{
						TeamMember* member_info = m_team->GetMemberInfo(role);
						if (NULL != member_info && member_info->IsFollow())m_team->OnTemporarilyPart(role);
					}
				}
			}
			else
			{
				attacker_list[attacker_index].on_line = 0;
			}
			gamelog::g_log_fb_other.printf(LL_INFO, "%s attacker[%d,%s] idx[%d]", __FUNCTION__, attacker_list[attacker_index].uid,
				attacker_list[attacker_index].player_name, attacker_index);
		}
	}

	//检查一次是否有盗宝者
	this->OnCheckAllAttackerBeCatch();
}

void SpecialLogicRelicLootingFb::CreateBox()
{
	const std::vector<DropPositionCfg> * box_list = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetDropPositionVec();
	if (box_list != NULL)
	{
		for (std::vector<DropPositionCfg>::const_iterator it = box_list->begin(); it != box_list->end(); it++)
		{
			if (it->npc_id > 0)
			{
				NPCGather * monster = NpcManager::Instance().CreateNPCGather(it->npc_id, m_scene, Posi(it->box_x, it->box_y));
				if (monster != NULL)
				{
					monster->SetDir((Dir)it->boss_dir);
					Protocol::SCRelicLootingObjInfo::Objinfo &node = m_obj_map[monster->GetId()];
					node.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_BOX;
					node.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
					node.obj_pos = Posi(it->box_x, it->box_y);
				}
			}
		}
	}
	this->SendObjInfo();
}

void SpecialLogicRelicLootingFb::RefreshSkillBox()
{
	
	for (std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = m_obj_map.begin(); it != m_obj_map.end();)
	{
		if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_SKILL)
		{
			//删掉道具
			Obj * node_obj = m_scene->GetObj(it->first);
			if (NULL != node_obj && node_obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
			{
				m_scene->DeleteObj(it->first);
			}
			m_obj_map.erase(it++);
			continue;
		}
		it++;
	}
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();

	for (int i = 0; i < other_cfg.props_num; i++)
	{
		Protocol::SCRelicLootingObjInfo::Objinfo obj_info;
		const SkillDropCfg* skill_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->RandSkill();
		if (skill_cfg != NULL)
		{
			NPCGather * monster = NpcManager::Instance().CreateNPCGather(skill_cfg->npc_id, m_scene, m_scene->GetARandPosi());
			if (NULL != monster)
			{
				obj_info.param = skill_cfg->sn;
				obj_info.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_SKILL;
				obj_info.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
				obj_info.obj_pos = monster->GetPos();
				m_obj_map.insert(std::pair<int, Protocol::SCRelicLootingObjInfo::Objinfo>(monster->GetId(), obj_info));
			}
		}
	}

	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;

		role->NoticeNum(noticenum::NT_RELIC_SKILL_BOX_REFRESH_SUCC);
	}

	this->SendObjInfo();
}

void SpecialLogicRelicLootingFb::ClearAllPayerEffect()
{
	int role_num = m_scene->RoleNum();
	for (int role_idx = 0; role_idx < role_num; ++role_idx)
	{
		Role * role = m_scene->GetRoleByIndex(role_idx);
		if (NULL == role) continue;

		role->GetRoleStatusManager()->OnRoleStopMoveStatus();
		this->OnRoleStopMoveStatus(role->GetUID());
	}
}

bool SpecialLogicRelicLootingFb::HasUser(int uid, bool * is_defender)
{
	for (int defender_idx = 0; defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && defender_idx < defender_num; defender_idx++)
	{
		if (defender_list[defender_idx].uid == uid)
		{
			if (NULL != is_defender)*is_defender = true;
			return true;
		}
	}
	for (int attacker_idx = 0; attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && attacker_idx < attacker_num; attacker_idx++)
	{
		if (attacker_list[attacker_idx].uid == uid)
		{
			if (NULL != is_defender)*is_defender = true;
			return true;
		}
	}
	return false;
}

void SpecialLogicRelicLootingFb::OnGameEndMakeRoleInTeam()
{
	/*for (std::vector<RelicLootingTeamRecodeInfo>::iterator it = m_team_recode_vec.begin(); it != m_team_recode_vec.end(); it++)
	{
		Role * role = m_scene->GetRoleByUID(it->leader_uid);
		if (NULL == role)continue;
		if (role->InTeam())
		{
			//解散队伍
			TeamManager::Instance().DismissTeam(role);
		}
		Team * role_team = TeamManager::Instance().CreateTeam(role, it->min_level_limit, it->max_level_limit, TEAM_TYPE_RELIC_LOOTING, false);
		if (NULL == role_team)
		{
			continue;
		}

		for (int member_idx = 0; member_idx < TEAM_MEMBER_MAX_NUM - 1 && member_idx < it->member_count; member_idx++)
		{
			Role * member_role = m_scene->GetRoleByUID(it->member_uid_list[member_idx]);
			if (member_role->InTeam())
			{
				TeamManager::Instance().ExitTeam(member_role);
			}
			role_team->AddMember(member_role);
		}
	}*/
	//游戏结束重新把玩家移动到初始位置，重新组队准备离场
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();

	for (int defender_idx = 0; defender_idx < defender_num && defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defender_idx++)
	{
		PlayerInfo &player = defender_list[defender_idx];
		Role * role = m_scene->GetRoleByUID(player.uid);
		if (NULL != role)
		{
			role->ResetPos(other_cfg.defender_pos_list[defender_idx].x, other_cfg.defender_pos_list[defender_idx].y);
		}
	}

	for (int attacker_idx = 0; attacker_idx < attacker_num && attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_idx++)
	{
		PlayerInfo &player = attacker_list[attacker_idx];
		Role * role = m_scene->GetRoleByUID(player.uid);
		if (NULL != role)
		{
			role->ResetPos(other_cfg.attacker_pos_list[attacker_idx].x, other_cfg.attacker_pos_list[attacker_idx].y);
		}
	}

	int role_num = m_scene->RoleNum();
	for (int i = 0; i < role_num; i++)
	{
		Role * role = m_scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		if (role->InTeam())
		{
			Team* m_team = role->GetMyTeam();
			if (m_team != NULL && !m_team->IsLeader(role))
			{
				TeamMember* member_info = m_team->GetMemberInfo(role);
				if (NULL != member_info && !member_info->IsFollow())m_team->OnTemporarilyPart(role);
			}
		}
	}
}

void SpecialLogicRelicLootingFb::SendSceneTampInfoToAll(int tamp_type, unsigned int tamp, Role * role)
{
	Protocol::SCRelicLootingTampInfo info;
	info.reason_type = tamp_type;
	info.tamp = tamp;
	if (role == NULL)
	{
		m_scene->SendToRole((const char*)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
	}
}

void SpecialLogicRelicLootingFb::OnPlayEnd(int win_type)
{
	allow_move = false;
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
	play_time = m_fb_end_tamp - play_time;//换算成游戏时长
	int play_time_score = 0;
	if (play_time <= 3 * 60)
	{
		play_time_score = other_cfg.time_score_3;
	}
	else if (play_time <= 5 * 60)
	{
		play_time_score = other_cfg.time_score_5;
	}
	else if (play_time_score <= 8 * 60)
	{
		play_time_score = other_cfg.time_score_8;
	}

	m_fb_end_tamp = 0;
	m_kick_out_tamp = (unsigned int)EngineAdapter::Instance().Time() + 10;
	allow_move = false;
	Protocol::SCRelicLootingFBFinishInfo info;
	info.win_type = win_type;
	
	ActivityRelicLooting * activity = (ActivityRelicLooting *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
	if (NULL != activity)
	{
		{
			//盗宝者功勋奖励排序
			std::vector<PlayerInfo> rank;
			for (int i = 0; i < attacker_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
			{
				rank.push_back(attacker_list[i]);
			}
			std::sort(rank.begin(), rank.end(), &SortFunc);
			int play_idx = 0;
			for (std::vector<PlayerInfo>::iterator it = rank.begin(); it != rank.end(); it++)
			{
				activity->OnUserPlayEnd(it->uid);
				bool is_win = false;
				int win_score = 0;
				ActivityRoleData * data_role = activity->GetRoleData(it->uid);
				if (win_type == Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_A)
				{
					if (data_role != NULL)
					{
						win_score = data_role->relic_looting_role_data.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] * other_cfg.continu_score;
					}
					switch (play_idx + 1)
					{
					case 1:
						win_score = win_score + other_cfg.first_score + play_time_score + other_cfg.winner_score;
						break;
					case 2:
						win_score = win_score + other_cfg.second_score + play_time_score + other_cfg.winner_score;
						break;
					case 3:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.winner_score;
						break;
					}
					is_win = true;
				}
				else if (win_type == Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_D)
				{
					switch (play_idx + 1)
					{
					case 1:
						win_score = other_cfg.first_score + play_time_score + other_cfg.loser_score;
						break;
					case 2:
						win_score = other_cfg.second_score + play_time_score + other_cfg.loser_score;
						break;
					case 3:
						win_score = other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					}
				}
				else
				{
					switch (play_idx + 1)
					{
					case 1:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					case 2:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					case 3:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					}
				}
				Role * role = m_scene->GetRoleByUID(it->uid);
				if (NULL != role)activity->OnRoleRank(role, win_score + it->gongxun, is_win);
				else activity->OnUserIdRank(it->uid, win_score + it->gongxun, is_win);
				
				info.attack_team_list[play_idx].uid = it->uid;
				info.attack_team_list[play_idx].team_type = Protocol::SCRelicLootingFBFinishInfo::TEAM_TYPE_ATTACKER;
				info.attack_team_list[play_idx].gongxian = it->gongxun;
				info.attack_team_list[play_idx].add_score = win_score + it->gongxun;
				info.attack_team_list[play_idx].on_line = it->on_line;
				F_STRNCPY(info.attack_team_list[play_idx].play_name, it->player_name, sizeof(GameName));
				play_idx++;
				gamelog::g_log_fb_other.printf(LL_INFO, "%s attacker[%d,%s] idx[%d]", __FUNCTION__, it->uid,
					it->player_name, play_idx);
			}
		}

		{
			//守卫者功勋奖励排序
			std::vector<PlayerInfo> rank;
			for (int i = 0; i < defender_num && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; i++)
			{
				rank.push_back(defender_list[i]);
			}
			std::sort(rank.begin(), rank.end(), &SortFunc);
			int play_idx = 0;
			for (std::vector<PlayerInfo>::iterator it = rank.begin(); it != rank.end(); it++)
			{
				activity->OnUserPlayEnd(it->uid);
				bool is_win = false;
				int win_score = 0;
				ActivityRoleData * data_role = activity->GetRoleData(it->uid);
				if (win_type == Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_D)
				{
					if (data_role != NULL)
					{
						win_score = data_role->relic_looting_role_data.param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_1] * other_cfg.continu_score;
					}
					switch (play_idx + 1)
					{
					case 1:
						win_score = win_score + other_cfg.first_score + play_time_score + other_cfg.winner_score;
						break;
					case 2:
						win_score = win_score + other_cfg.second_score + play_time_score + other_cfg.winner_score;
						break;
					case 3:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.winner_score;
						break;
					}
					is_win = true;
				}
				else if (win_type == Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_A)
				{
					switch (play_idx + 1)
					{
					case 1:
						win_score = other_cfg.first_score + play_time_score + other_cfg.loser_score;
						break;
					case 2:
						win_score = other_cfg.second_score + play_time_score + other_cfg.loser_score;
						break;
					case 3:
						win_score = other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					}
				}
				else
				{
					switch (play_idx + 1)
					{
					case 1:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					case 2:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					case 3:
						win_score = win_score + other_cfg.third_score + play_time_score + other_cfg.loser_score;
						break;
					}
				}
				Role * role = m_scene->GetRoleByUID(it->uid);
				if(NULL != role)activity->OnRoleRank(role, win_score + it->gongxun, is_win);
				else activity->OnUserIdRank(it->uid, win_score + it->gongxun, is_win);
				info.defend_team_list[play_idx].uid = it->uid;
				info.defend_team_list[play_idx].team_type = Protocol::SCRelicLootingFBFinishInfo::TEAM_TYPE_DEFENDER;
				info.defend_team_list[play_idx].gongxian = it->gongxun;
				info.defend_team_list[play_idx].add_score = win_score + it->gongxun;
				info.defend_team_list[play_idx].on_line = it->on_line;
				F_STRNCPY(info.defend_team_list[play_idx].play_name, it->player_name, sizeof(GameName));
				play_idx++;
				gamelog::g_log_fb_other.printf(LL_INFO, "%s defender[%d,%s] idx[%d]", __FUNCTION__, it->uid,
					it->player_name, play_idx);
			}
		}
	}
	m_scene->SendToRole((const char*)&info, sizeof(info));

	this->ClearAllPayerEffect();
	this->OnGameEndMakeRoleInTeam();
}

void SpecialLogicRelicLootingFb::OnCheckAllBoxBeSteal()
{
	int besteal_num = 0;
	for (std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = m_obj_map.begin(); it != m_obj_map.end(); it++)
	{
		if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_BOX)
		{
			if (it->second.status == Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NO)
			{
				besteal_num += 1;
			}
		}
	}
	const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
	if (besteal_num >= other_cfg.win_box_num)
	{
		this->OnAllBoxBeSteal();
	}
}

void SpecialLogicRelicLootingFb::OnAllBoxBeSteal()
{
	if (m_fb_end_tamp == 0)return;
	win_type = Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_A;
	this->OnPlayEnd(win_type);
}

void SpecialLogicRelicLootingFb::OnCheckAllAttackerBeCatch()
{
	for (int attacker_idx = 0; attacker_idx < attacker_num && attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_idx++)
	{
		if (attacker_list[attacker_idx].on_line != 0 && attacker_list[attacker_idx].uid != 0 && attacker_list[attacker_idx].stop_reason != PlayerInfo::STOP_MOVE_REASON_BE_CATCH)
		{
			return;
		}
	}
	this->OnAllAttackerBeCATCH();
}

void SpecialLogicRelicLootingFb::OnAllAttackerBeCATCH()
{
	if (m_fb_end_tamp == 0)return;
	win_type = Protocol::SCRelicLootingFBFinishInfo::RELIC_LOOTING_WIN_TYPE_WIN_D;
	this->OnPlayEnd(win_type);
}

void SpecialLogicRelicLootingFb::SendPlayerSkillInfo(Role * role)
{
	Protocol::SCRelicLootingFbSkillInfo info;
	PlayerInfo * node_player = this->GetPlayerInfo(role->GetUID());
	if (node_player == NULL)return;
	info.stop_reason = node_player->stop_reason;
	memcpy(&info.m_skill, &node_player->skill_list, sizeof(node_player->skill_list));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void SpecialLogicRelicLootingFb::HasUserSkillBelong(Role * role)
{
	std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = skill_belog_map.find(role->GetUserId());
	if (it != skill_belog_map.end())//有技能的，先把前一个技能结算了
	{
		if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_TRAP)
		{
			skill_belog_map.erase(it);
		}
		else if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_ROBOT)
		{
			//玩家回到原点
			Role *target_role = m_scene->GetRoleByUID(UidToInt(it->first));
			if (target_role != NULL)
			{
				this->Transmt(target_role, &it->second.obj_pos);
			}
			skill_belog_map.erase(it);
		}
		else if (it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_DOLL)
		{
			Robot * robot = ROBOTMANAGER.GetRobot(it->second.param2);
			if (NULL != robot)
			{
				ROBOTMANAGER.RemoveRobot(robot, __FUNCTION__);
			}
		}
	}
}

PlayerInfo * SpecialLogicRelicLootingFb::GetPlayerInfo(int uid, bool* is_defender)
{
	for (int i = 0; i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && i < defender_num; i++)
	{
		if (defender_list[i].uid == uid)
		{
			if (NULL != is_defender)*is_defender = true;
			return &defender_list[i];
		}
	}

	for (int i = 0; i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM && i < attacker_num; i++)
	{
		if (attacker_list[i].uid == uid)
		{
			if (NULL != is_defender)*is_defender = false;
			return &attacker_list[i];
		}
	}

	return NULL;
}

RoleSpecialEffect * SpecialLogicRelicLootingFb::GetRoleSpecialEffect(int uid)
{
	std::map<int, RoleSpecialEffect>::iterator it = m_special_effect_map.find(uid);
	if (it != m_special_effect_map.end())
	{
		return &it->second;
	}
	return NULL;
}

void SpecialLogicRelicLootingFb::OnRoleStartCatch(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_1);
}

void SpecialLogicRelicLootingFb::OnRoleStartBeCatch(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_2);
}

void SpecialLogicRelicLootingFb::OnRoleStartSprint(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_3);
}

void SpecialLogicRelicLootingFb::OnRoleStartTransfer(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_4);
}

void SpecialLogicRelicLootingFb::OnRoleStartTransferBack(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_5);
}

void SpecialLogicRelicLootingFb::OnRoleStartUnlock(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_6);
}

void SpecialLogicRelicLootingFb::OnRoleStartHelp(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_7);
}

void SpecialLogicRelicLootingFb::OnRoleStartBeHelp(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_8);
}

void SpecialLogicRelicLootingFb::OnRoleStartSneack(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_9);
}

void SpecialLogicRelicLootingFb::OnRoleStartDaoCaoRen(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_10);
}

void SpecialLogicRelicLootingFb::OnRoleStartSoftBody(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_11);
}

void SpecialLogicRelicLootingFb::OnRoleStartSoftBodyEnd(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_12);
}

void SpecialLogicRelicLootingFb::OnRoleStartBeTrap(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_13);
}

void SpecialLogicRelicLootingFb::OnRoleStartDoll(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_14);
}

void SpecialLogicRelicLootingFb::OnRoleStartRefresh(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_15);
}

void SpecialLogicRelicLootingFb::OnRoleStopMoveStatus(int uid)
{
	this->SetSpecialEffect(uid, RoleSpecialEffect::ROLE_SPECIAL_EFFECT_TYPE_NORMAL);
}

void SpecialLogicRelicLootingFb::SetSpecialEffect(int uid, int effect_type)
{
	Role * role = m_scene->GetRoleByUID(uid);
	std::map<int, RoleSpecialEffect>::iterator it = m_special_effect_map.find(uid);
	if (role != NULL && it != m_special_effect_map.end())
	{
		it->second.effect_type = effect_type;
		this->SendRoleSpecialEffect(role, &it->second);
	}
}

void SpecialLogicRelicLootingFb::SendRoleSpecialEffect(Role * role, RoleSpecialEffect * info)
{
	if (info == NULL || role == NULL)return;
	Protocol::SCRelicLootingEffectInfo msg;
	msg.uid = role->GetUID();
	msg.special_effect_type = info->effect_type;
	if (NULL != m_scene) m_scene->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), &msg, sizeof(msg));
}

void SpecialLogicRelicLootingFb::CheckAllTeamHasOutFollow()
{
	for (int defender_index = 0; defender_index < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defender_index++)
	{
		if (defender_list[defender_index].uid != 0)
		{
			Role * role = m_scene->GetRoleByUID(defender_list[defender_index].uid);
			if (NULL != role)
			{
				Team* m_team = role->GetMyTeam();
				if (m_team != NULL)
				{
					if (!m_team->IsLeader(role))
					{
						TeamMember* member_info = m_team->GetMemberInfo(role);
						if (NULL != member_info && member_info->IsFollow())m_team->OnTemporarilyPart(role);//全员暂离
					}
				}
			}
		}
	}

	for (int attacker_index = 0; attacker_index < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_index++)
	{
		if (attacker_list[attacker_index].uid != 0)
		{
			Role * role = m_scene->GetRoleByUID(attacker_list[attacker_index].uid);
			if (NULL != role)
			{
				Team* m_team = role->GetMyTeam();
				if (m_team != NULL)
				{
					if (!m_team->IsLeader(role))
					{
						TeamMember* member_info = m_team->GetMemberInfo(role);
						if (NULL != member_info && !member_info->IsFollow())m_team->OnTemporarilyPart(role);//全员暂离
					}
				}
			}
		}
	}
}

void SpecialLogicRelicLootingFb::SkillEffect(Role * role ,RelicLootingFBSkillInfo * effect_skill, bool *is_delete)
{
	if (effect_skill == NULL || NULL == role)return;
	const RelicLootingSkillCfg * skill_info = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetSkillDetail(effect_skill->sn);
	if (skill_info != NULL)
	{
		switch (skill_info->sn)
		{
		case RelicLootingSkillCfg::SKILL_ID_1:
			{
				this->CatchPlayer(role, skill_info, effect_skill);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->ClearMove();
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param3;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_2:
			{
				this->Galloping(role, skill_info);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->ClearMove();
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param4;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_3:
			{
				this->Transmt(role);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearMove();
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_4:
			{
				//检查玩家周围的宝箱信息
				this->OpenLock(role, skill_info, effect_skill);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->ClearMove();
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_5:
			{
				this->HelpTeamMember(role, skill_info, effect_skill);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->ClearMove();
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_6:
			{
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearMove();
				role->GetRoleStatusManager()->OnRoleStartSneak();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->special_param = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
				effect_skill->cool_down_tamp = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param3;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_7:
			{
				this->SoftBody(role, skill_info);
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->ClearMove();
				effect_skill->special_param = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param3;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_8:
			{
				this->DaoCaoRen(role, skill_info);
				this->OnRoleStopMoveStatus(role->GetUID());
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearMove();
				effect_skill->special_param = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_9:
			{
				this->Transmt(role);
				this->OnRoleStopMoveStatus(role->GetUID());
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearSkill();
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_10:
			{
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearSkill();
			}
			break;
		case RelicLootingSkillCfg::SKILL_ID_13:
			{
				role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				effect_skill->ClearMove();
				role->GetRoleStatusManager()->OnRoleStartSneak();
				this->OnRoleStopMoveStatus(role->GetUID());
				effect_skill->special_param = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
			}
			break;
		default:
			break;
		}
	}
	this->SendPlayerSkillInfo(role);
}

void SpecialLogicRelicLootingFb::OpenLock(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill)
{
	Obj *target_obj = m_scene->GetObj(skill->special_param);
	if (NULL != target_obj)
	{
		if (target_obj->GetObjType() == Obj::OBJ_TYPE_NPC_GATHERS)
		{
			std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo>::iterator it = m_obj_map.find(skill->special_param);
			if (it != m_obj_map.end() && it->second.obj_type == Protocol::SCRelicLootingObjInfo::OBJ_TYPE_BOX)
			{
				if (this->GetXYDistend(it->second.obj_pos.x, it->second.obj_pos.y, role->GetPos().x, role->GetPos().y) > skill_info->param3)//超出范围了，开箱失败
				{
					return;
				}
				it->second.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NO;
				//给贡献
				PlayerInfo * player = this->GetPlayerInfo(role->GetUID());
				if (player != NULL)
				{
					const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
					player->gongxun += other_cfg.box_score;

					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
						gamestring::g_reliclooting_open_box_notice, other_cfg.box_score);
					if (length > 0)
					{
						static Protocol::SCSystemMsg sm;
						int sendlen = 0;
						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
						{
							EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
						}
					}
					
					this->OnCheckAllBoxBeSteal();
				}
				m_scene->DeleteObj(target_obj->GetId());
				this->SendObjInfo();
				this->SendTeamInfo();
			}
		}
	}
}

void SpecialLogicRelicLootingFb::HelpTeamMember(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill)
{
	Obj *target_obj = m_scene->GetObj(skill->special_param);
	if (NULL != target_obj)
	{
		if (target_obj->GetObjType() == Obj::OBJ_TYPE_ROLE)
		{
			Role * target_role = (Role *)target_obj;
			bool is_defender = false;
			PlayerInfo *player_info = this->GetPlayerInfo(target_role->GetUID(), &is_defender);
			if (NULL != player_info && !is_defender)
			{
				if (role->GetUID() == player_info->uid)return;
				player_info->stop_reason = PlayerInfo::STOP_MOVE_REASON_NORMAL;
				target_role->GetRoleStatusManager()->OnRoleStopMoveStatus();
				this->OnRoleStopMoveStatus(target_role->GetUID());
				this->SendPlayerSkillInfo(target_role);
				this->SendTeamInfo();
				role->NoticeNum(noticenum::NT_RELIC_LOOTING_HELP_MEMBER_SUCC);
			}
		}
	}
}

void SpecialLogicRelicLootingFb::CatchPlayer(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill)
{
	ObjID obj_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM + RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//场景最大人数
	bool catch_member = false;
	int count = m_scene->GetZoneMatrix()->GetObjByCircle(role->GetPos(), skill_info->param2, obj_list, sizeof(obj_list) / sizeof(obj_list[0]));
	for (int i = 0; i < count && i < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM + RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; ++i)
	{
		Role *target_role = m_scene->GetRoleByObjID(obj_list[i]);
		if (NULL != target_role)
		{
			bool is_defender = false;
			PlayerInfo * target = this->GetPlayerInfo(target_role->GetUID(), &is_defender);
			if (target != NULL)
			{
				if (!is_defender)
				{
					if (target->stop_reason == PlayerInfo::STOP_MOVE_REASON_BE_CATCH)continue;//抓住了的跳过
					//抓到盗宝者了
					if (target->skill_list.m_item_skill.UsingSkill())
					{
						//使用中的技能一律中断
						target->skill_list.m_item_skill.casting_tamp = 0;
						target->skill_list.m_item_skill.guide_over_tamp = 0;
					}

					for (int common_skill_idx = 0; common_skill_idx < target->skill_list.common_skill_num && common_skill_idx < target->skill_list.MAX_COMMON_SKILL_NUM; common_skill_idx++)
					{
						if (target->skill_list.m_common_skill[common_skill_idx].UsingSkill())
						{
							//使用中的技能一律中断
							target->skill_list.m_common_skill[common_skill_idx].casting_tamp = 0;
							target->skill_list.m_common_skill[common_skill_idx].guide_over_tamp = 0;
						}
					}
					target_role->GetRoleStatusManager()->OnRoleStopMoveStatus();
					target->stop_reason = PlayerInfo::STOP_MOVE_REASON_BE_CATCH;
					catch_member = true;
					this->OnRoleStartBeCatch(target_role->GetUID());
					this->SendPlayerSkillInfo(target_role);
					break;//抓住一个就跳出
				}
			}
		}
	}

	if (catch_member == true)
	{
		const RelicLootingOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityRelicLootingConfig()->GetOtherCfg();
		PlayerInfo * player_role = this->GetPlayerInfo(role->GetUID());
		if (player_role != NULL)player_role->gongxun += other_cfg.capture_score;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_reliclooting_catch_player_notice, other_cfg.capture_score);

		if (length > 0)
		{
			static Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		this->SendTeamInfo();
		this->OnCheckAllAttackerBeCatch();
	}
}

void SpecialLogicRelicLootingFb::Galloping(Role *role, const RelicLootingSkillCfg * skill_info)
{
	BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(role->GetRoleModuleManager(), (unsigned int)EngineAdapter::Instance().Time(), (unsigned int)EngineAdapter::Instance().Time() + skill_info->param3);
	buff->SetMoveSpeed(skill_info->param2);
	buff->SetSave(false);
	role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
}

void SpecialLogicRelicLootingFb::Transmt(Role *role, Posi* transmt_pos)
{
	if (transmt_pos != NULL)
	{
		role->ResetPos(transmt_pos->x, transmt_pos->y);
	}
	else
	{
		Posi rand_pos = m_scene->GetARandPosi();
		role->ResetPos(rand_pos.x, rand_pos.y);
	}
	this->OnRoleStartTransferBack(role->GetUID());
	this->OnRoleStopMoveStatus(role->GetUID());
}

void SpecialLogicRelicLootingFb::RefreshPos(Role * role, Posi * transmt_pos, int distance)
{
	role->ResetPos(transmt_pos->x, transmt_pos->y);
	
	Protocol::SCObjMove scum;

	scum.dir = role->GetDir();
	scum.obj_id = role->GetId();
	scum.pos_x = (short)role->GetPos().x;
	scum.pos_y = (short)role->GetPos().y;
	scum.distance = (Scalar)distance;
	//scum.height = m_height;
	scum.reserve_sh = 4;//写死闪现

	if (NULL != m_scene) m_scene->GetZoneMatrix()->NotifyAreaMsg(role->GetObserHandle(), (void *)&scum, sizeof(Protocol::SCObjMove));
}

void SpecialLogicRelicLootingFb::SoftBody(Role *role, const RelicLootingSkillCfg * skill_info)
{
	BuffChangeMoveSpeed *buff = new BuffChangeMoveSpeed(role->GetRoleModuleManager(), (unsigned int)EngineAdapter::Instance().Time(), (unsigned int)EngineAdapter::Instance().Time() + skill_info->param3);
	buff->SetMoveSpeed(skill_info->param2);
	buff->SetSave(false);
	role->GetRoleModuleManager()->GetBuffManager()->AddBuff(buff);
}

void SpecialLogicRelicLootingFb::DaoCaoRen(Role *role, const RelicLootingSkillCfg * skill_info)
{
	NPCGather * monster = NpcManager::Instance().CreateNPCGather(skill_info->param3, m_scene, role->GetPos());
	if (monster != NULL)
	{
		Protocol::SCRelicLootingObjInfo::Objinfo &node = skill_belog_map[role->GetUserId()];//这操作会顶替上一个道具技能的belong
		node.obj_pos = monster->GetPos();
		node.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_ROBOT;
		node.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
		node.param = (unsigned int)EngineAdapter::Instance().Time() + skill_info->param2;
		node.param2 = monster->GetId();

		Protocol::SCRelicLootingObjInfo::Objinfo &node2 = m_obj_map[monster->GetId()];
		node2.obj_type = Protocol::SCRelicLootingObjInfo::OBJ_TYPE_BOX;
		node2.status = Protocol::SCRelicLootingObjInfo::OBJ_STATUS_NORMAL;
		node2.obj_pos = monster->GetPos();
	}
}

void SpecialLogicRelicLootingFb::ChuanSongFaZhen(Role *role, const RelicLootingSkillCfg * skill_info)
{
	bool is_defender = false;
	PlayerInfo * player = this->GetPlayerInfo(role->GetUID(), &is_defender);
	if (player == NULL)return;
	if (is_defender)
	{
		for (int defender_idx = 0; defender_idx < defender_num && defender_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; defender_idx++)
		{
			if (role->GetUID() == defender_list[defender_idx].uid)continue;
			Role *member_ans = m_scene->GetRoleByUID(defender_list[defender_idx].uid);
			if (NULL != member_ans)
			{
				this->InquiryTransmt(role, member_ans, player->skill_list.m_item_skill.guide_over_tamp);
			}
		}
	}
	else
	{
		for (int attacker_idx = 0; attacker_idx < attacker_num && attacker_idx < RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM; attacker_idx++)
		{
			if (role->GetUID() == attacker_list[attacker_idx].uid)continue;
			Role *member_ans = m_scene->GetRoleByUID(attacker_list[attacker_idx].uid);
			if (NULL != member_ans)
			{
				this->InquiryTransmt(role, member_ans, player->skill_list.m_item_skill.guide_over_tamp);
			}
		}
	}
}