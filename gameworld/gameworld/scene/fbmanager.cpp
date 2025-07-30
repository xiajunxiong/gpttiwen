#include "fbmanager.hpp"
#include "servercommon/servercommon.h"
#include "obj/character/role.h"
#include "other/rolestatusmanager.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "world.h"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "globalconfig/globalconfig.h"
#include "item/knapsack.h"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/braveground/activitybravegroundconfig.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/guildanswer/activityguildanswerconfig.hpp"
#include "config/activityconfig/mazeracer/activitymazeracerconfig.hpp"
#include "config/randactivityconfig/impl/randactivityanimalraceconfig.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "other/couragechallenge/couragechallengeconfig.h"
#include "other/miyi/miyiconfig.hpp"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolegatherfb/gatherfbconfig.hpp"
#include "other/skill/roleskill.hpp"
#include "other/funopen/funopen.hpp"
#include "other/challengefb/challengefbconfig.hpp"
#include "other/advancefb/advancefbconfig.hpp"
#include "speciallogicimpl/speciallogiccrystalfb.hpp"
#include "speciallogicimpl/speciallogicmazeracerwaitinghall.hpp"
#include "speciallogicimpl/speciallogicchallengefb.hpp"
#include "speciallogicimpl/speciallogicadvancefb.hpp"
#include "speciallogicimpl/speciallogicduchuangzeiying.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "config/otherconfig/duchuangzeiyingconfig.hpp"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/funcguide/funcguideconfig.h"
#include "global/guild/guild.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityanimalrace.hpp"
#include "obj/character/robot.h"
#include "other/roleactivity/roleactivity.hpp"
#include "scene/speciallogicimpl/speciallogicbraveground.hpp"
#include "other/medal/medalfbconfig.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "global/cross/crossmanager.h"
#include "scene/speciallogicimpl/mazeracermanager.hpp"
#include "task/taskrecorder.h"
#include "other/rolecross/rolecross.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/mentor/shitu.hpp"
#include "other/mentor/shituconfig.hpp"
#include "protocol/msgbraveground.h"
#include "other/roleactivity/roleactivitymanager.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityanimalraceconfig.hpp"
#include "other/wenxianglou/wenxiangloufbconfig.hpp"
#include "task/taskmanager.h"
#include "other/catchfox/catchfoxconfig.hpp"
#include "other/smxj/starkingconfig.hpp"
#include "other/dreamnoteschallenge/dreamnoteschallenge.hpp"
#include "config/randactivityconfig/impl/randactivityfortunemazeconfig.hpp"
#include "speciallogicimpl/speciallogicrafortunemaze.hpp"
#include "config/randactivityconfig/impl/randactivitymijingqiwenconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymijingqiwenconfig2.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen2.hpp"
#include "scene/speciallogicimpl/speciallogicreliclooting.hpp"
#include "scene/speciallogicimpl/speciallogicweddingfb.hpp"

#include "other/rolematerial/rolematerial.hpp"
#include "config/otherconfig/rolematerialconfig.hpp"
#include "other/catchfox/maintaskcatchfoxconfig.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "other/petgodfb/petgodfb.hpp"
#include "other/petgodfb/petgodfbconfig.hpp"
#include "other/petgodfb/speciallogicpetgodfb.hpp"
#include "other/dujie/dujie.hpp"
#include "other/dujie/dujieconfig.hpp"
#include "other/dujie/speciallogicfbdujie.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/wanlingfb/wanlingfbconfig.hpp"
#include "other/wanlingfb/speciallogicwanlingfb.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "qingyuan/qingyuanconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "qingyuan/weddingmanger.hpp"
#include "scene/speciallogicimpl/speciallogicweddingfb.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengconfig.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "other/cycleactivity/tuzikuaipao/tuzikuaipaoconfig.hpp"
#include "global/zhouqimanager/zhouqimanager.h"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "other/hongmengtianjie/hongmengtianjiemanager.hpp"
#include "other/hongmengtianjie/hongmengtianjieconfig.hpp"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"
#include "other/cycleactivity/gotoschool/gotoschoolconfig.hpp"

FBManager& FBManager::Instance()
{
	static FBManager inst;
	return inst;
}

bool FBManager::EnterFB(Role *role, short fb_type, int param1, int param2, bool is_client_req)
{
	if (NULL == role)
	{
		return false;
	}

	if (!role->CanEnterFB(true))
	{
		return false;
	}

	//检查队伍信息
	Team*  role_team = role->GetMyTeam();
	if (NULL != role_team)
	{
		switch (fb_type)
		{
		case FB_TYPE_GUILD_FIGHT:
			if (165 != role_team->GetTeamType())
			{
				role->NoticeNum(errornum::EN_FB_MANAGER_ENTER_TEAM_TYPE_ERR);
				return false;
			}
			break;
		}
	}

	// 默认单人和队伍都可以进副本，需要每个
	bool can_single_enter = true;
	bool can_team_enter = true;

	int target_scene_id = 0;
	int target_scene_key = 0;
	Posi target_pos(0, 0);

	switch (fb_type)
	{
	case FB_TYPE_GOTO_SCHOOL:
		{
			const GotoSchoolOtherConfig & other_cfg = LOGIC_CONFIG->GetGotoSchoolConfig()->GetOtherCfg();

			target_scene_id = other_cfg.scene_id;
			target_scene_key = role->GetUID();

		}
		break;
	case FB_TYPE_HONG_MENG_TIAN_JIE:
		{
			Team * team = role->GetMyTeam();
			if (!team)
			{
				return false;
			}

			//组队确认
			if (is_client_req)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			int type = param1;
			int fb_seq = param2;

			if (!CrossConfig::Instance().IsHiddenServer())
			{
				this->ProceedTeamEnterHiddenHongMengTianJie(role, type, fb_seq);
				return true;
			}

			if (!WorldHongMengTianJieManager::Instance().CanEnterFB(role, type, fb_seq))
				return false;

			const HongMengTianJieFbCfg * fb_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetFBCfg(type, fb_seq);
			if (NULL == fb_cfg)
				return false;

			target_scene_id = fb_cfg->scene_id;
			target_scene_key = team->GetTeamSceneKey();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);

			SpecialLogicHongMengTianJie * special_logic = 
				World::GetInstWorld()->GetSceneManager()->GetSpecialLogicHongMengTianJie(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			int difficultf = WorldHongMengTianJieManager::Instance().GetDifficultf(type, fb_seq);
			special_logic->InitScene(fb_cfg->fb_id, difficultf, fb_cfg->door_pos, type, fb_seq);
		
			int uid_list[TEAM_MEMBER_MAX_NUM] = { 0 };
			int member_num = 0;
			for (int i = 0; i < team->GetMemberCount()
				&& member_num < TEAM_MEMBER_MAX_NUM; ++i)
			{
				TeamMember* member = team->GetMemberInfo(i);
				if (NULL != member)
				{
					uid_list[member_num++] = member->uid;
				}
			}
			special_logic->SetTeamMemberList(member_num, uid_list);
		}
		break;
	case FB_TYPE_TU_ZI_KUAI_PAO:
		{
			const TuZiKuaiPaoOtherCfg & other_cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoOtherCfg();

			Team * team = role->GetMyTeam(true);
			if (!team || team->GetMemberCount() != 2)
			{
				return false;
			}

			if (!FunOpen::IsFunOpen(role, FUN_TYPE_ZHOU_QI))return false;
#ifndef  _DEBUG

			if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_TU_ZI_KUAI_PAO))
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}
#endif

			if (is_client_req && NULL != team && team->GetMemberCount() > 1)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			target_scene_id = other_cfg.scene_id;
			target_scene_key = this->GetSceneKey();
		}
		break;
	case FB_TYPE_SAN_XIAN_GUI_DONG:
	case FB_TYPE_TAO_QUAN_NA_JI:
		{
			if (is_client_req)return false;
			target_scene_id = param1;
			target_scene_key = role->GetUID();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);
		}
		break;
	case FB_TYPE_QINGYUAN_DUIDUIPENG:
		{
			if (!QingYuanDuiDuiPengManager::Instance().CanEnterFB(role))
				return false;

			Team * team = role->GetMyTeam();
			if (!team || team->GetMemberCount() != 2)
			{
				return false;
			}

			if (is_client_req)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			target_scene_id = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().scene_id;
			target_scene_key = team->GetTeamSceneKey();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);	
		}
		break;
	case FB_TYPE_QING_YUAN_SHENG_HUI:
		{
			if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_QING_YUAN_SHENG_HUI))
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			Team * team = role->GetMyTeam(true);
			if (!team || team->GetMemberCount() != 2)
			{
				return false;
			}

			target_scene_id = 323;
			target_scene_key = COMMON_FB_KEY;			
		}
		break;
	case FB_TYPE_SHI_TU_CHUAN_GONG:
		{
			int fb_index = param1;
			if (!role->IsLeader())
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}

			Team* my_team = role->GetMyTeam();
			if (NULL == my_team)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			//限制 师徒2人进入
			if (SHI_TU_SEEKING_TYPE_MAX !=  my_team->GetMemberCount())
			{
				role->NoticeNum(errornum::EN_SHITU_CHUAN_GONG_TEAM_FULL);
				return false;
			}
			
			int other_uid = 0;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				Role * member = my_team->GetMemberRoleByIndex(i);
				if (NULL == member)
				{
					continue;
				}

				if (role->GetUID() != member->GetUID())
				{
					other_uid = member->GetUID();
					break;
				}
			}

			//限制 徒弟带队
			if (0 == other_uid
				||!role->GetRoleModuleManager()->GetShiTu()->IsMaster(other_uid))
			{
				role->NoticeNum(errornum::EN_SHITU_CHUAN_GONG_TEAM_LEADER);
				return false;
			}
			
			//组队确认
			if (is_client_req)
			{
				my_team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			const ShiTuChuanGongOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetChuanGongOtherCfg();

			target_scene_id = other_cfg.scene_id;
			target_pos = other_cfg.born_p;
			target_scene_key = role->GetUID();

			SpecialLogicShiTuChuanGong * special_logic 
				= World::GetInstWorld()->GetSceneManager()
					->GetSpecialLogicShiTuChuanGong(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetRoleInfo(role->GetUID(), SHI_TU_SEEKING_TYPE_APPRENTICE);
			special_logic->SetRoleInfo(other_uid, SHI_TU_SEEKING_TYPE_MASTER);

			special_logic->SetEnd();
		}
		break;
	case FB_TYPE_WORLD_TEAM_ARENA:
		{
			Protocol::CSActivityEnterReq req;
			req.activity_type = ACTIVITY_TYPE_WORLD_TEAM_ARENA;
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq(&req);
			return true;
		}
		break;
	case FB_TYPE_UTA:
		{
			UTAShadow::Instance().TryEnterBattleScene(role);
			return true;
		}
		break;
	case FB_TYPE_WEDDING_FB:
		{
			//p_1: 婚宴id
			if (!CrossConfig::Instance().IsHiddenServer())
			{
				role->NoticeNum(errornum::EN_HIDDEN_REQ);
				return false;
			}
			WeddingInfo *wedding_info = WeddingManger::Instance().GetWeddingInfo(param1);
			if (NULL == wedding_info)
			{
				role->NoticeNum(errornum::EN_WEDDING_NULL);
				return false;
			}

			if (!wedding_info->GetHost(role->GetUID())
				&& !wedding_info->BeInvite(role->GetUID()))
			{
				role->NoticeNum(errornum::EN_WEDDING_APPLY_WAIT);
				return false;
			}

			const WeddingFB::BaseCfg& fb_base_cfg
				= LOGIC_CONFIG->GetQingYuanConfig()->GetFBBaseCfg();
			target_scene_id = fb_base_cfg.scene_id;
			target_scene_key = param1;
			target_pos = fb_base_cfg.role_born;

			SpecialLogicWeddingFB * special_logic = World::GetInstWorld()->GetSceneManager()
				->GetSpecialLogicWeddingFB(target_scene_id, target_scene_key, false);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			if (special_logic->CenEnterFB(role))
			{
				return false;
			}
		}
		break;
	case FB_TYPE_WAN_LING_FB:
		{
			int fb_index = param1;
			if (!role->IsLeader())
			{
				return false;
			}

			const WanLingFB::FB_Cfg* fb_cfg
				= LOGIC_CONFIG->GetWanLingFBConfig()->GetFBCfg(fb_index);
			if (NULL == fb_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			Team* my_team = role->GetMyTeam();
			if (NULL == my_team)
			{
				return false;
			}

			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				Role * member = my_team->GetMemberRoleByIndex(i);
				if (NULL == member)
				{
					continue;
				}

				if (member->GetLevel() < fb_cfg->open_level)
				{
					role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
					return false;
				}

				if (!member->GetRoleModuleManager()->GetRoleWanLingFB()->CanEnterFB(fb_index))
				{
					
					return false;
				}
			}

			target_scene_id = fb_cfg->scene_id;
			target_pos = fb_cfg->role_pos;
			target_scene_key = my_team->GetTeamSceneKey();

			SpecialLogicWanLingFB * special_logic 
				= World::GetInstWorld()->GetSceneManager()->GetSpecialLogicWanLingFB(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetFBInfo(fb_index);
		}
		break;
	case FB_TYPE_FISHING_GROUND:
		{
			can_team_enter = false;
			if (role->InTeam())
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return false;
			}
			const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfg(param1);
			if (NULL == level_cfg || NEW_LIFE_SKILL_TYPE_FISHING != level_cfg->need_skill_type)
			{
				return false;
			}
			if (level_cfg->need_skill_level > role->GetRoleModuleManager()->GetRoleNewLifeSkill()->GetNewLifeSkillByType(level_cfg->need_skill_type))
			{
				role->NoticeNum(errornum::EN_NEED_SKILL_LEVEL_NOT_ENOUGH);
				return false;
			}

			target_scene_id = level_cfg->scene_id;
			target_scene_key = COMMON_FB_KEY;
			target_pos = level_cfg->birth_pos;

			SpecialLogicFishingGround * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicFishingGround(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			special_logic->OnSetInfo(param1);
		}
		break;
	case FB_TYPE_DU_JIE:
		{
			//判断队长可进入
			if (!role->GetRoleModuleManager()->GetDuJie()->CanEnterFB(param1, false))
			{
				role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_TASK_LIMIT);
				return false;
			}
			target_scene_key = role->GetUID();

			if (role->InTeam())
			{//组队时判断队员等级
				if (!role->IsLeader())
				{
					return false;
				}

				const DuJieOtherCfg & other_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetOtherCfg();
				Team* m_team = role->GetMyTeam();
				if (NULL != m_team)
				{
					//判断队员可进入
					for (int i = 0; i < m_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
					{
						Role *temp_role = m_team->GetMemberRoleByIndex(i);
						if (NULL == temp_role) continue;
						
						if (temp_role->IsLeader())
						{
							continue;
						}

						//检测可挑战 或 可助战
						if (!temp_role->GetRoleModuleManager()->GetDuJie()->CanEnterFB(param1, true)
							&& !temp_role->GetRoleModuleManager()->GetDuJie()->CanEnterFB(param1, false))
						{
							role->NoticeNum(errornum::EN_MEMBER_CAN_NOT_ENTER_SCENE);
							temp_role->NoticeNum(errornum::EN_FUN_OPEN_ROLE_TASK_LIMIT);
							return false;
						}
					}
					target_scene_key = m_team->GetTeamSceneKey();
					if (!m_team->IsAllMemberLevelLess(m_team->GetLeaderLevel() + other_cfg.level_gap))
					{
						role->NoticeNum(errornum::EN_TEAM_MEMBER_LEVEL_LIMIT);
						return false;
					}

					if (is_client_req)
					{
						m_team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
						return false;
					}
				}
			}
			const DuJieFbCfg* fb_cfg = LOGIC_CONFIG->GetDuJieConfig()->GetFbCfg(param1);
			if (NULL == fb_cfg)
			{
				return false;
			}

			target_scene_id = fb_cfg->scene_id;
			target_pos = fb_cfg->role_pos;
			SpecialLogicFBDuJie * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicFBDuJie(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetFBInfo(param1);
		}
		break;
	case FB_TYPE_PET_GOD:
		{
			can_team_enter = false;
			if (role->InTeam())
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return false;
			}

			if (!role->GetRoleModuleManager()->GetPetGodFB()->CanEnterFB(param1))
			{
				//role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}
			const PetGodFbConfig::FBCfg * fb_cfg = LOGIC_CONFIG->GetPetGodFbConfig()->GetFBCfg(param1);
			if (NULL == fb_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			target_scene_id = fb_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = fb_cfg->role_pos;
			SpecialLogicPetGodFB * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicPetGodFB(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetFBInfo(target_scene_key, param1, role->GetLevel());
		}
		break;
	case FB_TYPE_SOUL_IN_ZHEN:
		{
			target_scene_id = param1;
			target_scene_key = role->GetUID();
		}
		break;
	case FB_TYPE_RELIC_LOOTING:
		{
			//检查看看是不是守卫者进场
			target_scene_id = param1;
			target_scene_key = param2;
			SpecialLogicRelicLootingFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicRelicLootingFb(target_scene_id, target_scene_key, false);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			if (role->InTeam())
			{
				Team* m_team = role->GetMyTeam();
				if (NULL != m_team)
				{
					for (int i = 0; i < m_team->GetMemberCount() && i < MAX_TEAM_MEMBER_NUM; i++)
					{
						Role *temp_role = m_team->GetMemberRoleByIndex(i);
						if (NULL == temp_role)continue;
						if (!special_logic->OnEnterFbBeforeSet(temp_role, is_client_req))
						{
							return false;
						}
					}
				}
			}
			else
			{
				if (!special_logic->OnEnterFbBeforeSet(role, is_client_req))
				{
					return false;
				}
			}
		}
		
		break;
	case FB_TYPE_MATERIAL_SCENE:
		{
			can_team_enter = false;
			if (role->InTeam())
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return false;
			}
			
			if (!role->GetRoleModuleManager()->GetRoleMaterial()->IsCanEnterFB(param1))
			{
				role->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_CHALLENGE);
				return false;
			}
			const MaterialOtherCfg & other_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetOtherCfg();
			if (role->GetRoleModuleManager()->GetRoleCrystalFb()->GetCrystalFbEnergy() < other_cfg.use_power)
			{
				role->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
				return false;
			}

			const MaterialSceneCfg * scene_cfg = LOGIC_CONFIG->GetMaterialConfig()->GetSceneCfg(param1);
			if (NULL == scene_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			if (!role->GetRoleModuleManager()->GetRoleMaterial()->IsCanEnterFBByTimes(scene_cfg->chapter_id, scene_cfg->level_seq))
			{
				role->NoticeNum(errornum::EN_MATERIAL_PASS_TIME_NOT_ENOUGH);
				return false;
			}

			target_scene_id = scene_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);
			SpecialLogicMaterial * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMaterial(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetInfo(scene_cfg->chapter_id, scene_cfg->level_seq);

		}
		break;
	case FB_TYPE_DREAM_NOTES_CHALLENGE:
		{
			can_team_enter = false;
			if (role->InTeam())
			{
				role->NoticeNum(errornum::EN_IN_TEAM_CAN_NOT);
				return false;
			}
			if (!role->GetRoleModuleManager()->GetDreamNotesChallenge()->IsCanChallenge(param1))
			{
				role->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_CHALLENGE);
				return false;
			}

			const DreamNotesChallengeCfg* scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(param1);
			if (NULL == scene_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			//没通关的关卡不需要次数与体力(2022-6-8 策划修改,没打过的也需要消耗体力)
			int pass_level = role->GetRoleModuleManager()->GetDreamNotesChallenge()->GetPassLevel();
// 			if (param1 != pass_level + 1)
// 			{
				if (param1 <= pass_level)
				{
					if (!role->GetRoleModuleManager()->GetDreamNotesChallenge()->IsHasTimes())
					{
						role->NoticeNum(errornum::EN_TODAY_CHALLENGE_TIMES_NOT_ENOUGH);
						return false;
					}
				}
				else if(param1 > pass_level + 1)
				{
					role->NoticeNum(errornum::EN_DREAM_NOTES_CHALLENGE_NOT_CHALLENGE);
					return false;
				}

				if (role->GetRoleModuleManager()->GetRoleCrystalFb()->GetCrystalFbEnergy() < scene_cfg->power_expend)
				{
					role->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
					return false;
				}
// 			}
			if (scene_cfg->recommend_level > role->GetLevel())
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_level_not_enough_not_challenge, scene_cfg->recommend_level);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(role, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
				}
				return false;
			}

			target_scene_id = scene_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);
			SpecialLogicDreamNotesChallengeFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicDreamNotesChallengeFb(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetInfo(scene_cfg->level);
		}
		break;
	case FB_TYPE_MI_JING_QI_WEN:
		{
			const RandActivityMiJingQiWenConfig * config = static_cast<const RandActivityMiJingQiWenConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE));
			if (NULL == config)
			{
				return false;
			}

			//检查队伍最高等级
			int top_level = role->GetLevel();
			if (role->InTeam())
			{
				Team * role_team = role->GetMyTeam();
				if (NULL != role_team)
				{
					top_level = role_team->GetFollowMemberMaxLevel();
				}
			}

			const MiJingQiWenFbLevelTypeCfg * level_cfg = config->GetLevelToCfg(top_level);
			if (level_cfg == NULL)return false;

			const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(level_cfg->type);

			if (cfg == NULL)
			{
				return false;
			}

			const RoleCrystalFbLevelCfg::LayerCfg * layer_cfg = cfg->GetLayerCfg(1);	//从每关第一层开始
			if (layer_cfg == NULL)
			{
				return false;
				//只有boss关卡
			}

			const CrystalFbSceneCfg::ItemCfg * scene_cfg = config->GetRandSceneCfg(layer_cfg->scene_id_group);
			if (scene_cfg == NULL)
			{
				return false;
			}

			target_scene_id = scene_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = scene_cfg->birth_pos;

			if (role->GetLevel() < cfg->need_role_level)
			{
				role->NoticeNum(errornum::EN_FB_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}

			can_team_enter = false;

			//bool is_need_comsume_item = true;
			
			{
				Team * team = role->GetMyTeam(false);
				if (NULL != team)
				{
					can_single_enter = false;
					can_team_enter = true;
					if (!team->IsLeader(role))
					{
						return false;
					}

					if (!team->IsAllMemberFollow())
					{
						return false;
					}

					for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
					{
						Role * member = team->GetMemberRoleByIndex(i);
						if (NULL == member)
						{
							continue;
						}

						if (member->GetLevel() < cfg->need_role_level)
						{
							role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
							return false;
						}

						if (!member->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
						{
							int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_team_member_item_not_enough_notice,
								member->GetName(), cfg->comsume_item_id);

							if (length > 0)
							{
								static Protocol::SCSystemMsg sm;
								int sendlen = 0;

								if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
								{
									team->SendToFollowMember((const char*)&sm, sendlen, true);
								}
							}
							return false;
						}
					}

					if (team->GetMemberCount() <= 1)
					{
						team->NoticeAllMember(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
						return false;
					}

					target_scene_key = team->GetTeamSceneKey();

					if (is_client_req)
					{
						team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
						return false;
					}
				}
				else
				{
					can_single_enter = true;
					can_team_enter = false;
					target_scene_key = role->GetUID();
				}
			}

			SpecialLogicMiJingQiWenFB * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMiJingQiWenFB(target_scene_id, target_scene_key);
			if (special_logic == NULL)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			special_logic->SetInfo(1, level_cfg->type, scene_cfg->transport_pos);
		}
		break;
	case FB_TYPE_MI_JING_QI_WEN_2:
		{
				const RandActivityMiJingQiWenConfig2* config = static_cast<const RandActivityMiJingQiWenConfig2*>
					(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
				if (NULL == config)
				{
					return false;
				}

				//检查队伍最高等级
				int top_level = role->GetLevel();
				if (role->InTeam())
				{
					Team * role_team = role->GetMyTeam();
					if (NULL != role_team)
					{
						top_level = role_team->GetFollowMemberMaxLevel();
					}
				}

				const MiJingQiWenFbLevelTypeCfg2 * level_cfg = config->GetLevelToCfg(top_level);
				if (level_cfg == NULL)return false;

				const RoleCrystalFbLevelCfg * cfg = config->GetRoleCrystalFbLevelCfg(level_cfg->type);

				if (cfg == NULL)
				{
					return false;
				}

				const RoleCrystalFbLevelCfg::LayerCfg * layer_cfg = cfg->GetLayerCfg(1);	//从每关第一层开始
				if (layer_cfg == NULL)
				{
					return false;
					//只有boss关卡
				}

				const CrystalFbSceneCfg::ItemCfg * scene_cfg = config->GetRandSceneCfg(layer_cfg->scene_id_group);
				if (scene_cfg == NULL)
				{
					return false;
				}

				target_scene_id = scene_cfg->scene_id;
				target_scene_key = role->GetUID();
				target_pos = scene_cfg->birth_pos;

				if (role->GetLevel() < cfg->need_role_level)
				{
					role->NoticeNum(errornum::EN_FB_ROLE_LEVEL_NOT_ENOUTH);
					return false;
				}

				can_team_enter = false;

				//bool is_need_comsume_item = true;

				{
					Team * team = role->GetMyTeam(false);
					if (NULL != team)
					{
						can_single_enter = false;
						can_team_enter = true;
						if (!team->IsLeader(role))
						{
							return false;
						}

						if (!team->IsAllMemberFollow())
						{
							return false;
						}

						for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
						{
							Role * member = team->GetMemberRoleByIndex(i);
							if (NULL == member)
							{
								continue;
							}

							if (member->GetLevel() < cfg->need_role_level)
							{
								role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
								return false;
							}

							if (!member->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
							{
								int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_team_member_item_not_enough_notice,
											member->GetName(), cfg->comsume_item_id);

								if (length > 0)
								{
									static Protocol::SCSystemMsg sm;
									int sendlen = 0;

									if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
									{
										team->SendToFollowMember((const char *)&sm, sendlen, true);
									}
								}
								return false;
							}
						}

						if (team->GetMemberCount() <= 1)
						{
							team->NoticeAllMember(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
							return false;
						}

						target_scene_key = team->GetTeamSceneKey();

						if (is_client_req)
						{
							team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
							return false;
						}
					}
					else
					{
						can_single_enter = true;
						can_team_enter = false;
						target_scene_key = role->GetUID();
					}
				}

				SpecialLogicMiJingQiWenFB2 * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMiJingQiWenFB2(target_scene_id, target_scene_key);
				if (special_logic == NULL)
				{
					role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
					return false;
				}

				special_logic->SetInfo(1, level_cfg->type, scene_cfg->transport_pos);
		}
		break;
	case FB_TYPE_FORTUNE_MAZE:
		{
			if (is_client_req)return false;
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
			if (NULL == ramgr)
			{
				return false;
			}

			if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_FORTUNE_MAZE))
			{
				return false;
			}

			if (role->InTeam())
			{
				role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
				return false;
			}
			const RandActivityFortuneMazeConfig * config = static_cast<const RandActivityFortuneMazeConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (NULL == config)return false;
			int real_scene_id = 0;
			int real_scene_npc_id = 0;
			int real_box_reward_id = 0;
			Posi box_pos = Posi(0, 0);

			const OtherCfg * old_cfg = config->GetCfgBySceneId(param1, ramgr);
			if (old_cfg != NULL)
			{
				real_scene_id = old_cfg->scene_id;
				real_scene_npc_id = old_cfg->npc_id;
				real_box_reward_id = old_cfg->reward_group_id;
				box_pos = old_cfg->box_pos;
			}
			else
			{
				const OtherCfg * scene_cfg = config->GetRandSceneId(ramgr);
				if (NULL == scene_cfg)return false;
				real_scene_id = scene_cfg->scene_id;
				real_scene_npc_id = scene_cfg->npc_id;
				real_box_reward_id = scene_cfg->reward_group_id;
				box_pos = scene_cfg->box_pos;
			}

			target_scene_id = real_scene_id;
			target_scene_key = role->GetUID();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);
			SpecialLogicFortuneMaze * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicFortuneMaze(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->CreateBox(real_scene_npc_id, real_box_reward_id, box_pos);
			RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
				(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
			if (old_cfg == NULL && NULL != fortune_maze)//旧图存在的时候
			{
				fortune_maze->SetRoleOldSceneId(real_box_reward_id);
			}
		}
		break;
	case FB_TYPE_CATCH_FOX:
		{
			const CatchFoxFindFoxConfig * other_cfg = LOGIC_CONFIG->GetCatchFoxFbConfig()->GetFindFoxConfig();
			if (other_cfg == NULL)return false;
			
			target_scene_id = other_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = Posi(other_cfg->pos_x1,other_cfg->pos_y1);
		}
		break;

	case FB_TYPE_MAIN_TASK_CATCH_FOX:
		{
			const MainTaskCatchFoxFindFoxConfig* cfg = LOGIC_CONFIG->GetMainTaskCatchFoxFbConfig()->GetFindFoxConfig();
			if (NULL == cfg) return false;

			target_scene_id = cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = Posi(cfg->pos_x1, cfg->pos_y1);
		}
		break;
	case FB_TYPE_STAR_KING:
		{
			const StarKingConfig *other_cfg = LOGIC_CONFIG->GetStarKingFbConfig()->GetConfig();
			if (other_cfg == NULL)return false;

			target_scene_id = other_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = Posi(other_cfg->pos_x1, other_cfg->pos_y1);
		}
		break;
	case FB_TYPE_WEN_XIANG_LOU:
		{
			Team * team = role->GetMyTeam();

			if (team != NULL)
			{
				return false;
			}

			const OtherConfig * other_cfg = LOGIC_CONFIG->GetWenXiangLouFbConfig()->GetOtherCfg();
			if (other_cfg == NULL)return false;

			if (role->GetRoleModuleManager()->GetTaskManager()->GetWenXiangLouParam().has_finish_fb == 1)//完成之后无法进入
			{
				return false;
			}

			target_scene_id = other_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);

			SpecialLogicWenXiangLouFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicWenXiangLouFb(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->InitFbScene(role);
		}
		break;
	case FB_TYPE_DU_CHUANG_ZEI_YING:
		{
			Team *team = role->GetMyTeam();
			if (NULL != team)
			{
				const DuChuangZeiYingOtherCfg & other_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetOtherCfg(); 
				if (team->GetFollowMemberCount() > other_cfg.max_team_num)
				{
					role->NoticeNum(errornum::EN_TEAM_TOO_MANY_PEOPLE);
					return false;
				}
			}
			
			const DuChuangZeiYingSceneCfg * scene_cfg = LOGIC_CONFIG->GetDuChuangZeiYingConfig()->GetSceneCfg(0);
			if (NULL == scene_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			target_scene_id = scene_cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = scene_cfg->born_pos;

			SpecialLogicDuChuangZeiYing * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicDuChuangZeiYing(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			special_logic->SetInfo(1, scene_cfg->transport_pos);
		}
		break;
	case FB_TYPE_SHI_TU_FB:
		{
			const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();

			Team * team = role->GetMyTeam();

			if (team == NULL)
			{
				return false;
			}

			if (!team->IsLeader(role))
			{
				return false;
			}

			if (team->HasDismissMember())
			{
				role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}

			int mem_count = team->GetMemberCount();
			if (mem_count < other_cfg.shi_tu_fb_role_num)
			{
				team->SendLeaderIncompatibleNotice(0, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOT_ENOUGH_MEMBER_NUM, other_cfg.shi_tu_fb_role_num);
				return false;
			}

			for (int i = 0; i < mem_count; ++i)
			{
				TeamMember * member = team->GetMemberInfoByIndex(i);
				if (member == NULL || !member->IsFollow())
				{
					continue;
				}

				if (!role->GetRoleModuleManager()->GetShiTu()->IsApprentice(member->uid) && role->GetUID() != member->uid)
				{
					team->SendLeaderIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOT_TU_DI);
					return false;
				}
			}

			target_scene_id = other_cfg.shi_tu_fb_scene_id;
			target_scene_key = team->GetTeamSceneKey();
			target_pos = World::GetInstWorld()->GetSceneManager()->GetTownPos(target_scene_id);

			SpecialLogicShiTuFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicShiTuFb(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_logic->SetInfo(role->GetUID(), role->GetLevel(), team->GetTeamIndex());
		}
		break;
	case FB_TYPE_ONLY_FIGHT:
		{
			Protocol::CSActivityEnterReq req;
			req.activity_type = ACTIVITY_TYPE_ONLY_FIGHT;
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq(&req);
			return true;
		}
		break;
	case FB_TYPE_ANIMAL_RACE:
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());
			if (NULL == ramgr) return false;

			RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
			if (rand_activity == NULL) return false;

			if (rand_activity->GetStatus() != ACTIVITY_STATUS_OPEN)
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}
			can_single_enter = true;
			can_team_enter = false;

			int index = rand_activity->GetInvaildIndex();

			const RandActivityAnimalRaceConfig * config = static_cast<const RandActivityAnimalRaceConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
			if (NULL == config)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			const RARolePlaceCfg * place_cfg = config->GetRolePlaceCfg(index);
			if (NULL == place_cfg) return false;
			const RAOtherCfg & other_cfg = config->GetOtherCfg();

			target_scene_id = other_cfg.scene_id;
			target_scene_key = COMMON_FB_KEY;
			target_pos = place_cfg->role_pos;
		}
		break;
	case FB_TYPE_FANTASY_FB:
		{
			can_single_enter = false;
			can_team_enter = true;

			Team *team = role->GetMyTeam();
			if (NULL == team)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}
			if (!team->IsLeader(role))
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}
			if (!team->IsAllMemberFollow()) 
			{
				role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}
			const FantasyFBOtherCfg * other_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetOtherCfg();
			if (NULL == other_cfg) return false;

			if (team->GetFollowMemberCount() < other_cfg->min_team_num)
			{
				role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
				return false;
			}
			if (team->GetAverageLevel() < other_cfg->min_level)
			{
				role->NoticeNum(errornum::EN_TEAM_AVG_LEVEL_NOT_ENOUGH);
				return false;
			}
			const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_FANTASY_FB);
			if (NULL == cfg) return false;

			for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < team->GetMemberCount(); i++)
			{
				TeamMember * member = team->GetMemberInfoByIndex(i);
				if (NULL == member) continue;

				if (member->IsFollow())
				{
					if (cfg->limit_level > member->level)
					{
						role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
						return false;
					}
				}
			}
			std::map<int, int> m_layer_seq;
			const FantasyFBSceneCfg * scene_cfg = LOGIC_CONFIG->GetFantasyFbConfig()->GetFBSceneCfg(team->GetAverageLevel(), &m_layer_seq);
			if (NULL == scene_cfg) return false;

			target_scene_id = scene_cfg->scene_id;
			target_scene_key = team->GetTeamSceneKey();
			target_pos = scene_cfg->birth_pos;

			SpecialLogicFantasyFb * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicFantasyFb(target_scene_id, target_scene_key);
			if (NULL == special_logic)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			
			special_logic->SetInfo(1, scene_cfg->transport_pos, &m_layer_seq);

			/*if (is_client_req && NULL != team &&  team->GetMemberCount() >= other_cfg->min_team_num)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}*/
		}
		break;
	case FB_TYPE_MEDAL:
		{
			can_single_enter = true;
			can_team_enter = true;

			const MedalFbLevelCfg * level_cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLevelCfg(param1);
			if (level_cfg == NULL)
			{
				return false;
			}

			const bool is_single = LOGIC_CONFIG->GetMedalFbConfig()->IsSingleFb(param1);

			const MedalFbLayerCfg * cfg = LOGIC_CONFIG->GetMedalFbConfig()->GetMedalFbLayerCfg(param1, 1);
			if (cfg == NULL)
			{
				return false;
			}

			Team * team = role->GetMyTeam();	
			if (!is_single)
			{
				team = role->GetMyTeam(true);
				if (team == NULL)
				{
					role->NoticeNum(errornum::EN_MEDAL_FB_LESS_MEMBER);
					return false;
				}

				if (!role->IsLeader())
				{
					return false;
				}

				if (team->GetFollowMemberCount() < level_cfg->min_enter_role_num)
				{
					role->NoticeNum(errornum::EN_MEDAL_FB_LESS_MEMBER);
					return false;
				}

				if (team->GetFollowMemberCount() > level_cfg->max_enter_role_num)
				{
					role->NoticeNum(errornum::EN_MEDAL_FB_OVER_MEMBER);
					return false;
				}

				if (is_client_req && NULL != team && team->GetMemberCount() > 1)
				{
					team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
					return false;
				}
			}
			else
			{
				if (team != NULL && team->GetMemberCount() > 1)
				{
					role->NoticeNum(errornum::EN_MEDAL_FB_OVER_MEMBER);
					return false;
				}
			}

		/*	if (team == NULL)
			{
				team = role->GetMyTeam(true);
			}*/

			target_scene_id = cfg->scene_id;
			target_scene_key = is_single ? role->GetUID() : team->GetTeamSceneKey();
			target_pos = cfg->birth_pos;
			target_pos = cfg->birth_pos;

			SpecialLogicMedal * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMedal(target_scene_id, target_scene_key);
			if (special_lobgic == NULL)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			special_lobgic->SetInfo(param1, 1);

			bool comsume_succ = role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(level_cfg->need_item_id, level_cfg->need_item_num, __FUNCTION__, 0, true);
			
			if (!comsume_succ)
			{
				return false;
			}
		}
		break;
	case FB_TYPE_GUIDE_FB:
		{
			const GuideFBLevelCfg * cfg = LOGIC_CONFIG->GetFuncGuideConfig()->GetGuideFBLevelCfg(1);
			if (cfg == NULL)
			{
				return false;
			}

			target_scene_id = cfg->scene_id;
			target_scene_key = role->GetUID();
		}
		break;
	case FB_TYPE_PLATFORM_BATTLE:
		{
			if (!role->IsInCross())
			{
				if (!CrossManager::Instance().IsCrossServerConnected())
				{
					role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
					return false;
				}


				CrossData_Business business_data;
				business_data.auto_join_type = CROSS_AUTO_JION_TYPE_FB;
				business_data.auto_join_type_param1 = FB_TYPE_PLATFORM_BATTLE;
				business_data.param1 = param1;
				business_data.param2 = param2;
				business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
				role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);

				return true;
			}

			const PlatformBattleSceneCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetSceneCfg(param1);
			if (cfg == NULL)
			{
				return false;
			}

			target_scene_id = cfg->scene_id;
			target_scene_key = COMMON_FB_KEY;
			target_pos = cfg->enter_pos;
		}
		break;
	case FB_TYPE_BRAVE_GROUND:
		{
			Team * team = role->GetMyTeam(true);
			if (team == NULL)
			{
				return false;
			}

			if (!role->IsLeader())
			{
				return false;
			}

			if (!role->IsInCross())
			{
				if (!CrossManager::Instance().IsCrossServerConnected())
				{
					role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
					return false;
				}

				TeamManager::Instance().OnTeamGotoHidden(role, ACTIVITY_TYPE_BRAVE_GROUND, param1, param2);
				return true;
			}

			can_single_enter = false;

			const BraveGroundLevelCfg * cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelCfg(1, team->GetMemberCount() <= 1 || !is_client_req);  //进入时候获取坐标
			if (cfg == NULL)
			{
				return false;
			}

			if (!team->IsAllMemberFollow())
			{
				return false;
			}

			const BraveGroundOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetOtherCfg();

			ActivityBraveGround * activity = (ActivityBraveGround *) ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND);
			if (activity == NULL)
			{
				return false;
			}

			if (!activity->IsActivityOpen())
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			int can_enter_max_level = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLastLevel();
			if (can_enter_max_level < 1)
			{
				role->NoticeNum(errornum::EN_BRAVE_GROUND_WORLD_LEVEL_NOT_OPEN);
				return false;
			}

			int min_level = INT_MAX;  //最小关卡
			int max_role_level = 1;
			Protocol::SCBraveGroundTeamInfo send_team_info;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				Role * member = team->GetMemberRoleByIndex(i);
				if (NULL == member)
				{
					continue;
				}

				if (member->GetLevel() < other_cfg.need_role_level || !FunOpen::IsFunOpen(role, FUN_TYPE_BRAVE_GROUND, false))
				{
					role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
					return false;
				}

				max_role_level = std::max(max_role_level, role->GetLevel());

				const CommonDataParam & common_data = member->GetRoleModuleManager()->GetCommonData();

				int temp = std::max(activity->GetRoleReachedLevel(member->GetUID()),
						    LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetSaoDangRewardLevel(common_data.brave_ground_max_reached_level, member->GetLevel()));
				if (temp < min_level)
				{
					min_level = temp;
				}
				
				send_team_info.info[i].role_id = member->GetUID();
				send_team_info.info[i].reached_level = temp;
			}

			team->SendToMember(&send_team_info, sizeof(send_team_info));

			if (is_client_req && team->GetMemberCount() > 1)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			int record_key = activity->GetRecordKey();
			target_scene_id = cfg->scene_id;
			target_scene_key = record_key;
			target_pos = cfg->birth_pos;

			SpecialLogicBraveGround * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicBraveGround(target_scene_id, target_scene_key);
			if (special_lobgic == NULL)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_lobgic->SetInfo(team->GetTeamIndex(), record_key, cfg->transport_pos, min_level, max_role_level, true);
		}
		break;
	case FB_TYPE_COURAGE_CHALLENGE:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_COURAGE_CHALLENGE, false))
			{
				role->NoticeNum(errornum::EN_CURR_LEVEL_NOT_ENTER_SCENE);
				return false;
			}

			Team *team = role->GetMyTeam();
			if (NULL != team)
			{
				role->NoticeNum(errornum::EN_ENTER_SCENE_NOT_TEAM);
				return false;
			}

			target_scene_id = LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().scene_id;
			target_scene_key = role->GetUID();

			can_team_enter = false;
		}
		break;

	case FB_TYPE_MIYI:
		{
			//printf("[进入试炼之地]\n ");
			const MiYiLevelCfg * cfg = LOGIC_CONFIG->GetMiYiConfig()->GetMiYiLevelCfgBySceneId(param1);
			if (cfg == NULL) return false;

			std::vector<Role *> role_done; role_done.clear();  // 已完成前置的列表
			std::vector<Role *> role_non_done; role_non_done.clear();  // 未完成前置的列表

			if (!FunOpen::IsFunOpen(role, FUN_TYPE_MIYI, true))
			{
				return false;
			}

			target_scene_id = cfg->scene_id;
			target_scene_key = role->GetUID();
			target_pos = cfg->enter_pos;

			if (role->GetLevel() < cfg->need_role_level)
			{
				return false;
			}
			
			do																																									
			{
				if (cfg->pre_task == 0)
				{
					break;
				} 
				else if (!role->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(cfg->pre_task))
				{
					role_non_done.push_back(role);
				}
				else
				{
					role_done.push_back(role);
				}
			} while (0);
			

			Team* team = role->GetMyTeam();
			if (NULL != team)
			{	
				if (!team->IsLeader(role))
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
					return false;
				}
				
				if (!team->IsAllMemberFollow())
				{
					role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
					return false;
				}
				target_scene_key = team->GetTeamSceneKey();

				for (int index_member = 0; index_member < MAX_TEAM_MEMBER_NUM; ++index_member)
				{
					Role* member = team->GetMemberRoleByIndex(index_member);
					if (NULL == member) continue;

					if (member->GetLevel() < cfg->need_role_level)
					{
						role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
						return false;
					}

					if (member == role) continue;

					if (cfg->pre_task == 0)
					{
						continue;
					}
					else if (!member->GetRoleModuleManager()->GetTaskRecorder()->HaveFinished(cfg->pre_task))
					{
						role_non_done.push_back(member);
					}
					else
					{
						role_done.push_back(member);
					}
				}
			}

			// 没完成前置的给自己飘字
			for (int i_no = 0; i_no < (int)role_non_done.size(); ++i_no)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_enter_miyi, cfg->pre_task);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
					{
						EngineAdapter::Instance().NetSend(role_non_done[i_no]->GetNetId(), (const char*) &sm, sendlen);
					}
				}
				for (int i_yes = 0; i_yes < (int)role_done.size(); ++i_yes)
				{
					int length_2 = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_enter_miyi_2, role_non_done[i_no]->GetName(), cfg->pre_task);
					if (length_2 > 0)
					{
						Protocol::SCSystemMsg sm_2;
						int sendlen_2 = 0;
						if (SceneManager::CreateSystemMsg(&sm_2, &sendlen_2, gamestring::GAMESTRING_BUF, length_2, GetMsgType(SYS_MSG_FLOAT)))
						{
							EngineAdapter::Instance().NetSend(role_done[i_yes]->GetNetId(), (const char*) &sm_2, sendlen_2);
						}
					}
				}
				if (i_no == (int)role_non_done.size() - 1)
				{
					return false;
				}
			}

			if (NULL != team && is_client_req && team->GetMemberCount() > 1) 
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}
		}
		break;

	case FB_TYPE_CRYSTAL_FB:
		{
			const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(param1);

			Posi transport_pos(0, 0);
			if (cfg != NULL)
			{
				if (cfg->layer_cfg.size() != 0)
				{
					const RoleCrystalFbLevelCfg::LayerCfg * layer_cfg = cfg->GetLayerCfg(1);	//从每关第一层开始
					if (layer_cfg == NULL)
					{
						return false;
					}

					const CrystalFbSceneCfg::ItemCfg * scene_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandSceneCfg(layer_cfg->scene_id_group);
					if (scene_cfg == NULL)
					{
						return false;
					}

					target_scene_id = scene_cfg->scene_id;
					target_scene_key = role->GetUID();
					target_pos = scene_cfg->birth_pos;
					transport_pos = scene_cfg->transport_pos;
				}
				else
				{
					//boss关卡
					target_scene_id = cfg->last_layer_scene_id;
					target_scene_key = role->GetUID();
					target_pos = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandPosi(cfg->boss_layer_sceneborn_group_id);
				}
				
			}
			else
			{
				return false;
			}

			if (role->GetLevel() < cfg->need_role_level)
			{
				role->NoticeNum(errornum::EN_FB_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}

			can_team_enter = false;

			bool is_need_comsume_item = true;
			int coster = 0;
			int team_max_level = 0;
			switch (cfg->type)
			{
				case ROLE_CRYSTAL_FB_TYPE_SPECIAL:
				{
						if (role->GetRoleModuleManager()->GetRoleCrystalFb()->HasFristPassCrystalFb(cfg->seq))//这个副本只允许进一次
						{
							return false;
						}
						can_team_enter = true;
				}
				break;
				case ROLE_CRYSTAL_FB_TYPE_NORMAL:		//普通副本为单人副本，只可单人进入，进入时消耗相关道具					
					{
						if (!FunOpen::IsFunOpen(role, FUN_TYPE_CRYSTAL_FB, true))
						{
							return false;
						}

						if (cfg->is_fly_up_fb && role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag() < FlyUpSystemParam::FLY_UP_STAG_2)
						{
							role->NoticeNum(errornum::EN_ROLE_FLY_UP_STAG_NOT_ENOUGH);
							return false;
						}

						if (!role->GetRoleModuleManager()->GetRoleCrystalFb()->CanChallengeNormalFB(cfg->seq))
						{
							role->NoticeNum(errornum::EN_CRYSTAL_FB_NORMAL_TIMES_LIMIT);
							return false;
						}

						if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
						{
							is_need_comsume_item = false; // 尝试使用体力

							int use_energy = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().use_power;
							int cur_energy = role->GetRoleModuleManager()->GetRoleCrystalFb()->GetCrystalFbEnergy();
							if (cur_energy < use_energy) // 体力和石头都没有，报错
							{
								int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_item_not_enough_notie_content2, cfg->comsume_item_id);
								World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
								return false;
							}			
						}

						Team * team = role->GetMyTeam();
						if (NULL != team)
						{
							role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
							return false;
						}
					}
					break;
				case ROLE_CRYSTAL_FB_TYPE_XI_YOU:		//稀有副本为组队副本，进入时只需要队长消耗相关道具											
					{
						can_single_enter = false;
						can_team_enter = true;
						is_need_comsume_item = false;
						Team * team = role->GetMyTeam(true);
						if (NULL == team)
						{
							return false;
						}

						if (!team->IsLeader(role))
						{
							return false;
						}

						if (!team->IsAllMemberFollow())
						{
							return false;
						}
						int member_num = team->GetMemberCount();

						for (int i = 0; i < MAX_TEAM_MEMBER_NUM && i < member_num; ++i)
						{
							TeamMember* team_member = team->GetMemberInfo(i);
							if(NULL == team_member) continue;

							Role * member = team->GetMemberRoleByIndex(i);
							if (NULL == member)
							{
								continue;
							}					

							if (member->GetLevel() < cfg->need_role_level || !FunOpen::IsFunOpen(member, FUN_TYPE_CRYSTAL_FB, false))
							{
								role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
								return false;
							}
						}

						target_scene_key = team->GetTeamSceneKey();
					}
					break;
				case ROLE_CRYSTAL_FB_TYPE_JING_YING:		//精英副本组队进入，进入时只需要队长消耗相关道具					
					{
						can_single_enter = false;
						can_team_enter = true;

						Team * team = role->GetMyTeam(true);
						if (NULL == team)
						{
							return false;
						}

						if (!team->IsLeader(role))
						{
							return false;
						}

						if (!team->IsAllMemberFollow())
						{
							return false;
						}

						for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
						{
							Role* member = team->GetMemberRoleByIndex(i);
							if (NULL == member)
							{
								continue;
							}

							if (member->GetLevel() < cfg->need_role_level || !FunOpen::IsFunOpen(member, FUN_TYPE_CRYSTAL_FB, false))
							{
								role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
								return false;
							}

							if (cfg->is_fly_up_fb && member->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag() < FlyUpSystemParam::FLY_UP_STAG_2)
							{
								role->NoticeNum(errornum::EN_TEAM_NEED_ROLE_FEI_SHENG);
								member->NoticeNum(errornum::EN_ROLE_FLY_UP_STAG_NOT_ENOUGH);
								return false;
							}
							
							if (member->GetLevel() > team_max_level) team_max_level = member->GetLevel();
						}
						coster = role->GetUID();
						target_scene_key = team->GetTeamSceneKey();
					}
					break;
				default:
					return false;
			}

			SpecialLogicCrystalFB * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicCrystalFB(target_scene_id, target_scene_key);
			if (special_lobgic == NULL)
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			special_lobgic->SetInfo(1, param1, transport_pos, coster, team_max_level);

			if (is_need_comsume_item)
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
				{
					int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_item_not_enough_notie_content,
						cfg->comsume_item_id);

					if (length > 0)
					{
						static Protocol::SCSystemMsg sm;
						int sendlen = 0;

						if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
						{
							EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*) &sm, sendlen);
						}
					}
					return false;
				}
			}

			Team * team = role->GetMyTeam();
			if (is_client_req && NULL != team &&  team->GetMemberCount() > 1)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}
		}
		break;
	case FB_TYPE_GATHER_FB:
		{
			can_team_enter = false;

			const RoleGatherFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleGatherFbConfig()->GetLevelCfg(param1);
			if (level_cfg == NULL || level_cfg->need_skill_type == NEW_LIFE_SKILL_TYPE_FISHING)
			{
				return false;
			}

			/*	if (role->GetLevel() < level_cfg->need_role_level)
				{
					role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
					return false;
				}*/
			if (level_cfg->need_skill_level > role->GetRoleModuleManager()->GetRoleNewLifeSkill()->GetNewLifeSkillByType(level_cfg->need_skill_type))
			{
				role->NoticeNum(errornum::EN_NEED_SKILL_LEVEL_NOT_ENOUGH);
				return false;
			}

			/*const LifeSkillInfo * life_skill_info = role->GetRoleModuleManager()->GetRoleSkill()->GetLifeSkillInfoByLifeSkillType(level_cfg->need_skill_type);
			if (NULL == life_skill_info || life_skill_info->skill_level < level_cfg->need_skill_level)
			{
				role->NoticeNum(errornum::EN_NEED_SKILL_LEVEL_NOT_ENOUGH);
				return false;
			}*/

			target_scene_id = level_cfg->scene_id;
			target_scene_key = COMMON_FB_KEY;
			target_pos = level_cfg->birth_pos;
		}
		break;
	case FB_TYPE_MAZE_RACER:		// 传送进大厅
		{
			if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_MAZE_RACER))
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			Team * team = role->GetMyTeam();
			if (NULL == team)		// 必须组队
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			if (role->GetUID() != team->GetLeaderUID())
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}

			int mem_count = team->GetMemberCount();
			const MazeRacerOthersCfg & mr_other_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetOthersConfig();

			if (mem_count < mr_other_cfg.min_member_need)
			{
				role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
				return false;
			}

			const MazeRacerSceneCfg * scene_cfg = LOGIC_CONFIG->GetActivityMazeRacerConfig()->GetSceneConfig(team->GetAverageLevel());
			if (NULL == scene_cfg)
			{
				role->NoticeNum(errornum::EN_FB_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}

			if (team->GetAverageLevel() < scene_cfg->min_lv)
			{
				role->NoticeNum(errornum::EN_TEAM_AVG_LEVEL_NOT_ENOUGH);
				return false;
			}

			if (team->GetAverageLevel() > scene_cfg->max_lv)
			{
				role->NoticeNum(errornum::EN_TEAM_AVG_LEVEL_TOO_HIGH);
				return false;
			}

			for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)
			{
				Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
				if (NULL == mem_role) continue;

				if (!FunOpen::IsFunOpen(mem_role, FUN_TYPE_MAZE_RACE, false))
				{
					role->NoticeNum(errornum::EN_MEMBER_FUN_OPEN_LIMIT);
					return false;
				}
			}

			if (!team->IsAllMemberFollow())
			{
				return false;
			}


			bool can_join = true;
			for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
			{
				TeamMember *  info = team->GetMemberInfo(i);
				if (info != NULL)
				{
					if (!MazeRacerManager::Instance().CanJoin(info->uid))
					{
						can_join = false;
						team->SendAllMemberIncompatibleNotice(i, Protocol::TEAM_MEMBER_INCOMPATIBLE_NOTICE_TYPE_JOIN_TIMES);
					}
				}
			}

			if (!can_join)
			{
				return false;
			}

			if (!role->IsInCross())
			{
				if (!CrossManager::Instance().IsCrossServerConnected())
				{
					role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
					return false;
				}

				TeamManager::Instance().OnTeamGotoHidden(role, ACTIVITY_TYPE_MAZE_RACER, param1, param2);
				return true;
			}
			else
			{
				SpecialLogicMazeRacerWaitingHall * waiting_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicMazeRacerWaitingHall(scene_cfg->wait_scene_id, COMMON_FB_KEY, false);
				if (NULL == waiting_hall) return false;

				target_scene_id = scene_cfg->wait_scene_id;
				target_scene_key = COMMON_FB_KEY;
				waiting_hall->GetScene()->GetTownPoint(&target_pos);
			}
		}
		break;
	case FB_TYPE_GUILD_FIGHT:
		{
			Protocol::CSActivityEnterReq req;
			req.activity_type = ACTIVITY_TYPE_GUILD_FIGHT;
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq(&req);
			return true;
			/*
			if (!ActivityManager::Instance().IsActivityStandy(ACTIVITY_TYPE_GUILD_FIGHT))
			{
				role->NoticeNum(errornum::EN_ACTIVITY_ISNT_IN_STANDY);
				return false;
			}

			GuildID guild_id = role->GetGuildID();
			Guild * guild = GuildManager::Instance().GetGuild(guild_id);
			if (NULL == guild)
			{
				role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
				return false;
			}

			if (!FunOpen::IsFunOpen(role, FUN_TYPE_GUILD_LUANDOU, true))
			{
				return false;
			}

			Team * team = role->GetMyTeam();
			if (NULL != team)	// 如果是组队状态，检测队员家族是否跟队长相同
			{
				if (role->GetUID() != team->GetLeaderUID())
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
					return false;
				}

				int mem_count = team->GetMemberCount();
				for (int mem_idx = 0; mem_idx < mem_count; ++mem_idx)
				{
					Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
					if (NULL == mem_role) continue;		// 机器人不检测

					if (guild_id != mem_role->GetGuildID())
					{
						role->NoticeNum(errornum::EN_MEMBER_GUILD_NOT_UNIFORM);
						return false;
					}

					if (!FunOpen::IsFunOpen(mem_role, FUN_TYPE_GUILD_LUANDOU, true))
					{
						role->NoticeNum(errornum::EN_MEMBER_FUN_OPEN_LIMIT);
						return false;
					}
				}
			}

			const GuildFightOtherCfg & gf_other = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg();
			SpecialLogicGuildFight * fight_hall = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildFight(gf_other.scene_id, COMMON_FB_KEY, false);
			if (NULL == fight_hall) return false;

			target_scene_id = gf_other.scene_id;
			target_scene_key = COMMON_FB_KEY;
			fight_hall->GetScene()->GetTownPoint(&target_pos);
			*/
		}
		break;
	case FB_TYPE_CHALLENGE_FB:
		{
			const ChallChapterInfor * fb_cfg = LOGIC_CONFIG->GetChallengeFbConfig()->GetChapterInfor(param1, param2);
			if (NULL == fb_cfg)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}

			Team * team = role->GetMyTeam();
			if (NULL == team)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}

			if (role->GetUID() != team->GetLeaderUID())
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}

			int mem_num = team->GetMemberCount();

			const ChallengeFbOthers & chall_fb_other = LOGIC_CONFIG->GetChallengeFbConfig()->GetChallFbOtherCfg();
			if (mem_num < chall_fb_other.min_mem_num)
			{
				role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
				return false;
			}

			for (int mem_idx = 0; mem_idx < mem_num; ++mem_idx)
			{
				Role * mem_role = team->GetMemberRoleByIndex(mem_idx);
				if (NULL == mem_role) continue;	// 机器人不检测

				if (mem_role->GetLevel() < fb_cfg->level_limit)
				{
					role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
					return false;
				}
			}

			SpecialLogicChallengeFb * sp_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicChallengeFb(fb_cfg->scene_id, team->GetTeamSceneKey());
			if (NULL == sp_logic) 
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}
			sp_logic->InitChallengeScene(fb_cfg->fb_id, fb_cfg->chapter);
			target_scene_id = fb_cfg->scene_id;
			target_scene_key = team->GetTeamSceneKey();
			sp_logic->GetScene()->GetTownPoint(&target_pos);
		}
		break;
	case FB_TYPE_GUILD_ANSWER:
		{
			Protocol::CSActivityEnterReq req;
			req.activity_type = ACTIVITY_TYPE_GUILD_ANSWER;
			role->GetRoleModuleManager()->GetRoleActivityManager()->OnActivityEnterReq(&req);
			return true;

			/*
			if (ActivityManager::Instance().IsActivityClose(ACTIVITY_TYPE_GUILD_ANSWER))
			{
				role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
				return false;
			}

			Guild * guild = role->GetGuild();
			if (NULL == guild)
			{
				role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
				return false;
			}

			Team * team = role->GetMyTeam();
			if (NULL != team)
			{
				int mem_count = team->GetMemberCount();
				for (int i = 0; i < mem_count; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL != member && NULL == member->GetGuild())
					{
						role->NoticeNum(errornum::EN_MEMBER_GUILD_NOT_EXIST);
						return false;
					}
				}
			}

			const GuildAnswerOthers & ga_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
			SpecialLogicGuildAnswer * sp_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildAnswer(ga_cfg.scene_id, COMMON_FB_KEY, false);
			if (NULL == sp_logic) return false;

			target_scene_id = ga_cfg.scene_id;
			target_scene_key = COMMON_FB_KEY;
			sp_logic->GetScene()->GetTownPoint(&target_pos);

			*/
		}
		break;
	case FB_TYPE_ADVANCE_FB:
		{
			/*if (param1 < 0 || param1 >= ADV_TYPE_MAX)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return false;
			}*/

			/*Team * team = role->GetMyTeam();
			if (NULL == team)
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}*/

			/*if (role->GetUID() != team->GetLeaderUID())
			{
				role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}*/

			/*const AdvFbOtherCfg & adv_fb_other = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbOtherCfg();
			if (team->GetMemberCount() < adv_fb_other.min_team_num[param1])
			{
				role->NoticeNum(errornum::EN_FOLLOW_MEMBER_NOT_ENOUGH);
				return false;
			}*/
			const AdvFbInfoCfg  * scene_cfg = LOGIC_CONFIG->GetAdvanceFbConfig()->GetAdvFbInfoCfg(param1, 1);
			if (NULL == scene_cfg) return false;
			target_scene_id = scene_cfg->scene_id;
			target_scene_key = role->GetUID();
			Team * team = role->GetMyTeam();
			if (NULL != team)
			{
				if (team->GetLeaderUID() != role->GetUID())
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
					return false;
				}

				int mem_count = team->GetMemberCount();
				for (int i = 0; i < mem_count; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL != member && member->GetLevel() < scene_cfg->min_level)
					{
						role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
						return false;
					}
				}
			} 
			else
			{
				if (role->GetLevel() < scene_cfg->min_level)
				{
					role->NoticeNum(errornum::EN_MEMBER_LEVEL_NOT_ENOUGH);
					return false;
				}
			}

			if (is_client_req && NULL != team &&  team->GetMemberCount() > 1)
			{
				team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, fb_type, param1, param2);
				return false;
			}

			SpecialLogicAdvanceFb * sp_af = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicAdvanceFb(target_scene_id, target_scene_key);
			if (NULL == sp_af) 
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			sp_af->InitAdvanceFbScene(param1, 0, 1);
			target_pos = scene_cfg->enter_pos;
		}
		break;
	case FB_TYPE_GUILD_STATION:
		{
			Guild * guild = role->GetGuild();
			if (NULL == guild)
			{
				role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
				return false;
			}

			const GuildOtherCfg & guild_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
			target_scene_id = guild_cfg.station_scene_id;
			target_scene_key = role->GetGuildID();

			Team * team = role->GetMyTeam();
			if (NULL != team)
			{
				if (team->GetLeaderUID() != role->GetUID())
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
					return false;
				}

				int mem_count = team->GetMemberCount();
				for (int i = 0; i < mem_count; ++i)
				{
					Role * member = team->GetMemberRoleByIndex(i);
					if (NULL != member && member->GetGuildID() != target_scene_key)
					{
						role->NoticeNum(errornum::EN_MEMBER_GUILD_NOT_UNIFORM);
						return false;
					}
				}
			}

			SpecialLogicGuildStation * sp_gs = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildStation(target_scene_id, target_scene_key);
			if (NULL == sp_gs) 
			{
				role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return false;
			}

			sp_gs->GetScene()->GetTownPoint(&target_pos);
		}
		break;
	case FB_TYPE_SHAN_HAI_BOSS:
		{
			//can_single_enter = false;
			//can_team_enter = true;

			//Team *team = role->GetMyTeam();
			//if (NULL == team)
			//{
			//	role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
			//	return false;
			//}
			//if (!team->IsLeader(role))
			//{
			//	role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
			//	return false;
			//}

			//if (0 == LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterGroupCfg(param1, team->GetFollowMemberMaxLevel()))
			//{
			//	return false;
			//}

			//const ShanHaiBossBossCfg * boss_cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetBossFromNpcCfg(param1, team->GetFollowMemberMaxLevel());
			//if (boss_cfg == NULL)
			//{
			//	return false;
			//}
			//const ItemID key_id = LOGIC_CONFIG->GetShanHaiBossConfig()->GetKeyIDFromNpcCfg(boss_cfg->npc_id);
			//if (0 >= key_id)
			//{
			//	return false;
			//}
			//if (0 >= role->GetRoleModuleManager()->GetKnapsack()->Count(key_id))
			//{
			//	role->NoticeNum(errornum::EN_SHAN_HAI_KEY_NOT_ENOUGH);
			//	return false;
			//}
			//
			//if (team->HasDismissMember())
			//{
			//	role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			//	return false;
			//}
			//if (team->GetMemberCount() < MAX_TEAM_MEMBER_NUM)
			//{
			//	role->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_ENGOUH);
			//	return false;
			//}
			//if (team->GetFollowMemberMaxLevel() - team->GetFollowMemberMinLevel() >= 10)
			//{
			//	role->NoticeNum(errornum::EN_TEAM_LEVEL_MORE_TEN);
			//	return false;
			//}

			///*for (int idx = 0; idx < team->GetMemberCount(); ++idx)
			//{
			//	Role * member = team->GetMemberRoleByIndex(idx);
			//	if (NULL == member) return false;

			//	if (!FunOpen::IsFunOpen(role, FUN_TYPE_SHAN_HAI_BOSS, true))
			//	{
			//		return false;
			//	}
			//}*/

			//if (is_client_req && NULL != team)
			//{
			//	team->SetEnterFbInfo(role, AGREE_BEFORE_ENTER_TYPE_FB, FB_TYPE_SHAN_HAI_BOSS, param1, param2);
			//	return false;
			//}

			//target_scene_id = boss_cfg->scene_id;
			//target_scene_key = team->GetTeamSceneKey();
			//target_pos = boss_cfg->role_pos;

			//SpecialLogicShanHaiBoss * special_logic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicShanHaiBoss(target_scene_id, target_scene_key);
			//if (NULL == special_logic)
			//{
			//	role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			//	return false;
			//}

			//if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(key_id, 1, __FUNCTION__))
			//{
			//	role->NoticeNum(errornum::EN_SHAN_HAI_KEY_NOT_ENOUGH);
			//	return false;
			//}

			//special_logic->SetInfo(boss_cfg->npc_id, team->GetFollowMemberMaxLevel());
		}
		break;

	case FB_TYPE_NEW_COURAGE_CHALLENGE:
		{
			if (!FunOpen::IsFunOpen(role, FUN_TYPE_NEW_COURAGE_CHALLENGE, false))
			{
				role->NoticeNum(errornum::EN_CURR_LEVEL_NOT_ENTER_SCENE);
				return false;
			}

			Team *team = role->GetMyTeam();
			if (NULL != team)
			{
				role->NoticeNum(errornum::EN_ENTER_SCENE_NOT_TEAM);
				return false;
			}

			target_scene_id = LOGIC_CONFIG->GetCourageChallengeConfig()->GetOtherCfg().new_scene_id;
			target_scene_key = role->GetUID();

			can_team_enter = false;
		}
		break;
	}

	if (0 == target_scene_id || 0 == target_scene_key)
	{
		role->NoticeNum(errornum::EN_FB_NOT_EXIST);
		return false;
	}

	if (role->InTeam())
	{
		Team * m_team = role->GetMyTeam();
		if (NULL != m_team)
		{
			for (int member_idx = 0; member_idx < m_team->GetMemberCount(); member_idx++)
			{
				WorldStatus::Instance().LogFunctionStats(role, "FBManagerEnter", fb_type, param1, param2);
			}
		}
	}
	else
	{
		WorldStatus::Instance().LogFunctionStats(role, "FBManagerEnter", fb_type, param1, param2);
	}

	bool ret = false;
	
	if (!can_team_enter)
	{
		ret = this->ProceedSingleEnter(role, target_scene_id, target_pos, target_scene_key, fb_type, param1, param2);
	} 
	else if (!can_single_enter)
	{
		ret = this->ProceedTeamEnter(role, target_scene_id, target_pos, target_scene_key, fb_type, param1, param2);
	} 
	else
	{
		if (role->InTeam())
		{
			ret = this->ProceedTeamEnter(role, target_scene_id, target_pos, target_scene_key, fb_type, param1, param2);
		} 
		else
		{
			ret = this->ProceedSingleEnter(role, target_scene_id, target_pos, target_scene_key, fb_type, param1, param2);
		}
	}

	return ret;
}

bool FBManager::LeaveFB(Role *role)
{
	if (NULL == role) return false;

	Scene *scene = role->GetScene();
	if ((NULL == scene || scene->IsInStaticScene())) return false;

	scene->GetSpecialLogic()->OnLeaveFB(role);

	Posi last_scene_pos(0, 0);
	int last_scene_id = role->GetLastScene(&last_scene_pos);

	if (!Scene::IsStaticScene(World::GetInstWorld()->GetSceneManager()->GetSceneTypeBySceneID(last_scene_id)))
	{
		last_scene_id = GLOBALCONFIG->GetCenterCityBornPos(last_scene_pos);
		Scene* center_scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(last_scene_id, COMMON_SCENE_KEY);
		if (NULL != center_scene)
		{
			center_scene->GetTownPoint(&last_scene_pos);
		}

		Posi temp_pos;
		gamelog::g_log_scene.printf(LL_ERROR, "SpecialLogic::DelayKickOutRole role last scene ERROR, user[%d,%s] cur_scene[%d] last_scene[%d]", role->GetUID(), role->GetName(), scene->GetSceneID(), role->GetLastScene(&temp_pos));
	}

	bool ret = false;
	if (role->InTeam())
	{
		Team* team = role->GetMyTeam();
		if (NULL == team) return false;

		if (team->GetLeaderUID() == role->GetUID())
		{
			ret = this->ProceedTeamLeave(role, last_scene_id, last_scene_pos);
		} 
		else
		{
			TeamManager::Instance().OnLeaveFB(role);
			ret = this->ProceedSingleLeave(role, last_scene_id, last_scene_pos);
		}
	} 
	else
	{
		ret = this->ProceedSingleLeave(role, last_scene_id, last_scene_pos);
	}
	
	return ret;
}

int FBManager::GetSceneKey()
{
	return m_scene_key++;
}


bool FBManager::ConsumeBeforeRoleEnter(Role* role, short fb_type, int param1, int param2, int target_scene_id, int target_scene_key, bool only_check)
{
	// 注意在only check的情况下只检查，不实际消耗
	switch (fb_type)
	{
	case FB_TYPE_DREAM_NOTES_CHALLENGE:
		{
			if (only_check)
			{
				return true;
			}
			const DreamNotesChallengeCfg * scene_cfg = LOGIC_CONFIG->GetDreamNotesChallengeConfig()->GetChallengeFbCfg(param1);
			if (NULL == scene_cfg)
			{
				return false;
			}
			int pass_level = role->GetRoleModuleManager()->GetDreamNotesChallenge()->GetPassLevel();
			//if (param1 != pass_level + 1)	//没通关过的才需要消耗体力
			{
				if (!role->GetRoleModuleManager()->GetRoleCrystalFb()->UseCrystalFbEnergy(scene_cfg->power_expend, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_ADVANCE_DREAM_NOTES))
				{
					return false;
				}
				role->GetRoleModuleManager()->GetDreamNotesChallenge()->OnRoleEnterFBUseEnergy(scene_cfg->power_expend);
			}
		}
		break;
	case FB_TYPE_MI_JING_QI_WEN:
		{
			if (only_check)
			{
				return true;
			}
			const RandActivityMiJingQiWenConfig * config = static_cast<const RandActivityMiJingQiWenConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE));
			if (NULL == config)
			{
				return false;
			}
			const RoleCrystalFbLevelCfg  * cfg = config->GetRoleCrystalFbLevelCfg(param1);
			if (cfg == NULL)
			{
				return false;
			}

			//检查
			if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}

			if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter mijingqiwen fb"))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}
		}
		break;
	case FB_TYPE_MI_JING_QI_WEN_2:
		{
			if (only_check)
			{
				return true;
			}
			const RandActivityMiJingQiWenConfig2 * config = static_cast<const RandActivityMiJingQiWenConfig2 *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANECDOTE_2));
			if (NULL == config)
			{
				return false;
			}
			const RoleCrystalFbLevelCfg * cfg = config->GetRoleCrystalFbLevelCfg(param1);
			if (cfg == NULL)
			{
				return false;
			}

			//检查
			if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}

			if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter mijingqiwen fb"))
			{
				role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return false;
			}
		}
		break;
	case FB_TYPE_CRYSTAL_FB:
		{
			if (only_check)
			{
				return true;
			}

			const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(param1);
			if (cfg == NULL)
			{
				return false;
			}

			SpecialLogicCrystalFB * special_lobgic = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicCrystalFB(target_scene_id, target_scene_key);
			if (special_lobgic == NULL)
			{
				return false;
			}
			//bool is_need_comsume_item = true;
			
			switch (cfg->type)
			{
				case ROLE_CRYSTAL_FB_TYPE_SPECIAL:		//特殊副本为组队，进入时只需要队长消耗相关道具										
				{
					Team * team = role->GetMyTeam();
					if (NULL == team)
					{
						return false;
					}

					if (team->IsLeader(role))
					{
						if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter crystal fb"))
						{
							role->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
							return false;
						}
					}
				}
				break;
				case ROLE_CRYSTAL_FB_TYPE_NORMAL:		//普通副本为单人副本，只可单人进入，进入时消耗相关道具					
					{
						if (role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter crystal fb"))
						{
							// 优先使用石头
							ItemConfigData item(cfg->comsume_item_id, true, cfg->comsume_item_num);
							role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleEnterFBUseItem(item);
						}
						else if (role->GetRoleModuleManager()->GetRoleCrystalFb()->UseCrystalFbEnergy(LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().use_power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_ADVANCE_CRYSTAL))
						{
							role->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleEnterFBUseEnergy(LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().use_power);
						}
						else
						{
							int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_item_not_enough_notie_content2, cfg->comsume_item_id);
							World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
							return false;
						}
						
						role->GetRoleModuleManager()->GetRoleCrystalFb()->AddPuTongJingYingPlayTimes();
					}
					break;
				case ROLE_CRYSTAL_FB_TYPE_XI_YOU:		//稀有副本为组队，进入时只需要队长消耗相关道具-->修改为不扣材料										
					{
						/*Team * team = role->GetMyTeam();
						if (NULL == team)
						{
							return false;
						}

						if (team->IsLeader(role))
						{
							if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter crystal fb"))
							{
								role->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
								return false;
							}
						}*/
					}
					break;
				case ROLE_CRYSTAL_FB_TYPE_JING_YING:		//精英副本组队进入，进入时只需要队长消耗相关道具					
					{
						Team * team = role->GetMyTeam();
						if (NULL == team)
						{
							return false;
						}

						if (team->IsLeader(role))
						{
							if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter crystal fb"))
							{
								role->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
								return false;
							}
							special_lobgic->OnAddConsumeUid(role->GetUID());
							//黄征洋策划要这样改，给侠义值逻辑是大于 normal_reward_times，防止队长第5次进入没有获得侠义值。所以等于normal_reward_times时候增加一次
							int normal_reward_times = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().normal_reward_time;
							if (role->GetRoleModuleManager()->GetRoleCrystalFb()->GetJingYingPlayTimes() == normal_reward_times)
							{
								role->GetRoleModuleManager()->GetRoleCrystalFb()->AddJingYingPlayTimes();
							}
						}
						else
						{
							role->GetRoleModuleManager()->GetRoleCrystalFb()->AddJingYingPlayTimes();
							if (role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num, "enter crystal fb"))
							{
								special_lobgic->OnAddConsumeUid(role->GetUID());
							}
						}
					}
					break;
				default:
					return false;
			}
		}
		break;

	default:
		{

		}
		break;
	}

	return true;
}

bool FBManager::ProceedSingleEnter(Role* role, int target_scene_id, Posi target_pos, int target_scene_key, short fb_type, int param1, int param2)
{
	if (role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return false;
	}

	if (!role->GetRoleStatusManager()->CanTransportTo(target_scene_id))
	{
		role->NoticeNum(errornum::EN_SCENE_TRANSPORT_LIMIT_BY_ROLE_STATUS);
		return false;
	}

	if (!role->GetRoleModuleManager()->CanEnterScene(target_scene_id))
	{
		return false;
	}

	if (!this->ConsumeBeforeRoleEnter(role, fb_type, param1, param2, target_scene_id, target_scene_key, false))
	{
		return false;
	}

	int uid = role->GetUID();
	GameName user_name;
	role->GetName(user_name);

	bool ret = World::GetInstWorld()->GetSceneManager()->GoTo(role, target_scene_id, target_scene_key, target_pos);
	if (!ret)
	{
		role->NoticeNum(errornum::EN_FB_ENTER_FB_ERROR);
		return false;
	}

	gamelog::g_log_scene.printf(LL_DEBUG, "FBManager::EnterFB: role[%d,%s] ret:%d scene_id[%d] pos[%d,%d] scene_key[%d]",
		uid, user_name, (ret ? 1 : 0), target_scene_id, target_pos.x, target_pos.y, target_scene_key);

	return ret;
}

bool FBManager::ProceedTeamEnter(Role* role, int target_scene_id, Posi target_pos, int target_scene_key, short fb_type, int param1, int param2)
{
	if (!role->InTeam())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
		return false;
	}

	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}

	if (team->GetLeaderUID() != role->GetUID())
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return false;
	}

	if (team->HasDismissMember())
	{
		role->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
		return false;
	}

	/* 暂时貌似组队进入，也可以用队长的uid做scene_key，所以暂时先屏蔽
	if (COMMON_SCENE_KEY != target_scene_key)
	{
		Team* team = role->GetMyTeam();
		if (NULL == team)
		{
			role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return false;
		}
		target_scene_key = team->GetTeamSceneKey(); // 当需要组队进入，scene_key应用玩家队伍的scene_key
	}
	*/

	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	Robot* robot_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int role_num = 0;
	int robot_num = 0;
	int member_num = team->GetMemberCount();
	for (int i = 0; i < member_num && role_num < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember* member = team->GetMemberInfo(i);
		if (NULL == member || member->Invalid() || !member->is_follow || !member->is_online) continue;

		if (!member->IsRobot())
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (member_role == NULL)
			{
				continue;
			}

			if (!member_role->GetRoleStatusManager()->CanTransportTo(target_scene_id))
			{
				member_role->NoticeNum(errornum::EN_SCENE_TRANSPORT_LIMIT_BY_ROLE_STATUS);
				return false;
			}

			if (!member_role->GetRoleModuleManager()->CanEnterScene(target_scene_id))
			{
				return false;
			}

			// 先判断是否有足够的门票
			if (!this->ConsumeBeforeRoleEnter(member_role, fb_type, param1, param2, target_scene_id, target_scene_key, true))
			{
				//TODO 通报给队长or全队
				role->NoticeNum(errornum::EN_TEAM_MEMBER_LACK_OF_TICKET);
				return false;
			}

			role_list[role_num++] = member_role;
		} 
		else
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL != robot)
			{
				robot_list[robot_num++] = robot;
			}
		}
	}

	for (int i = 0; i < role_num; ++i)
	{
		this->ConsumeBeforeRoleEnter(role_list[i], fb_type, param1, param2, target_scene_id, target_scene_key, false);
	}

	World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, target_scene_id, target_scene_key, target_pos);

	return true;
}

bool FBManager::ProceedSingleLeave(Role* role, int target_scene_id, Posi target_pos)
{
	if (role->InTeam()) return false;

	gamelog::g_log_debug.printf(LL_DEBUG, "FBManager::LeaveFB: %s[%d] [scene_id:%d]", role->GetName(), UidToInt(role->GetUserId()), role->GetScene()->GetSceneID());

	World::GetInstWorld()->GetSceneManager()->GoTo(role, target_scene_id, 0, target_pos);	// 只能往普通场景传送

	return true;
}

bool FBManager::ProceedTeamLeave(Role* role, int target_scene_id, Posi target_pos)
{
	if (!role->InTeam()) return false;

	Team* team = role->GetMyTeam();
	if (NULL == team || team->GetLeaderUID() != role->GetUID()) return false;

	// 获取role列表
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int role_num = 0;

	Robot* robot_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int robot_num = 0;

	int member_num = team->GetMemberCount();
	for (int i = 0; i < member_num; ++i)
	{
		TeamMember* member = team->GetMemberInfo(i);
		if (NULL != member && !member->Invalid())
		{
			if (!member->IsRobot())
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL == member_role)
				{
					continue;
				}

				if (member->is_online && member->is_follow)
				{
					role_list[role_num++] = member_role;
				}
			} 
			else
			{
				Robot* robot = team->GetMemberRobotByIndex(i);
				if (NULL != robot)
				{
					robot_list[robot_num++] = robot;
				}
			}
		}
	}

	bool is_one_dragon_leave_fantasy_fb = false;
	Scene* scene = role->GetScene();
	if (Scene::SCENE_TYPE_FANTASY_FB == scene->GetSceneType() && TEAM_TYPE_ONE_DRAGON == team->GetTeamType())
	{
		is_one_dragon_leave_fantasy_fb = true;
	}

	switch (scene->GetSceneType())
	{//不踢机器人的场景
	case Scene::SCENE_TYPE_WAN_LING_FB:
		{
			is_one_dragon_leave_fantasy_fb = true;
		}
		break;
	default:
		break;
	}

	if (is_one_dragon_leave_fantasy_fb)
	{
		// 一条龙离开四象幻境 不踢机器人
		World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, target_scene_id, 0, target_pos);
		TeamManager::Instance().OnTeamSwitchToStaticScene(team);
	}
	else
	{
		for (int i = 0; i < role_num; ++i)
		{
			Role* member = role_list[i];

			World::GetInstWorld()->GetSceneManager()->GoTo(member, target_scene_id, 0, target_pos);
		}

		TeamManager::Instance().OnTeamSwitchToStaticScene(team);

		for (int i = 0; i < robot_num; ++i)
		{
			Robot* member = robot_list[i];
			team->LeaveTeam(member->GetRobotUserID()); // 机器人在离开副本后离开队伍 离开队伍时会自动delete掉，无需手动操作
		}
		if (robot_num > 0 && team->GetMemberCount() == 1)
		{
			team->DismissTeamHelper();
		}
	}

	return true;
}

void FBManager::ProceedTeamEnterHiddenHongMengTianJie(Role* role, int fb_type, int fb_seq)
{
	if (NULL == role) return;

	Team* team = role->GetMyTeam();
	if (NULL == team || !team->IsLeader(role)) return;

	static CrossData_Business business_data;
	business_data.Reset();
	business_data.origin_team_index = team->GetTeamIndex();
	business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_TEAM_ENTER_HONG_MENG_TIAN_JIE;
	business_data.level_limit_low = team->GetLimitLevelLow();
	business_data.level_limit_high = team->GetLimitLevelHigh();
	business_data.team_type = team->GetTeamType();
	business_data.allown_red_join = team->TeamIsAllowRedNameJoin() ? 1 : 0;
	business_data.param1 = fb_type;
	business_data.param2 = fb_seq;
	int other_member_count = 0;
	for (int i = 0; i < team->GetMemberCount() && other_member_count < 4; ++i)
	{
		TeamMember* tmb = team->GetMemberInfoByIndex(i);
		if (NULL == tmb || tmb->IsRobot() || !tmb->IsFollow() 
			|| tmb->uid == role->GetUID()) continue;

		business_data.teammate_list[other_member_count++] = tmb->uid;

#ifdef _DEBUG
		gamelog::g_log_debug.printf(LL_INFO, "ProceedTeamEnterHiddenHongMengTianJie role[%d,%s] member_uid[%d]",
			role->GetUID(), role->GetName(), tmb->uid);
#endif
	}

	if (role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data))
	{
		team->SetNotice(false, Team::AUTO_JOIN_LIMIT_TIME);
		//team->SetChangeLeaderNotNoticeFlag();
	}
}

FBManager::FBManager()
{
	m_scene_key = 1000;
}

FBManager::~FBManager()
{

}

