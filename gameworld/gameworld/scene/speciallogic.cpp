#include "other/rolemodulemanager.hpp"
#include "protocol/msgteam.h"
#include "protocol/msgitem.h"
#include "global/team/team.hpp"
#include "global/usercache/usercache.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "other/partner/partner.hpp"
#include "other/pet/pet.hpp"
#include "item/money.h"
#include "obj/character/robot.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "obj/character/role.h"
#include "obj/character/character.h"
#include "obj/character/npc_monster.h"
#include "speciallogic.hpp"
#include "scene.h"
#include "engineadapter.h"
#include "world.h"
#include "gamelog.h"

#include "other/rolemodulemanager.hpp"
#include "other/duel/duel.hpp"


#include "scene/speciallogicimpl/speciallogiccommon.hpp"
#include "scene/speciallogicimpl/speciallogicguaji.hpp"
#include "scene/speciallogicimpl/speciallogiccouragechallenge.hpp"
#include "scene/speciallogicimpl/speciallogicmiyi.hpp"
#include "scene/speciallogicimpl/speciallogiccrystalfb.hpp"
#include "scene/speciallogicimpl/speciallogicteamfight.hpp"
#include "scene/speciallogicimpl/speciallogicgather.hpp"
#include "scene/speciallogicimpl/speciallogicbraveground.hpp"
#include "scene/speciallogicimpl/speciallogicchiefelection.hpp"
#include "scene/speciallogicimpl/speciallogicchiefpeakbattle.hpp"
#include "scene/speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "scene/speciallogicimpl/speciallogicadvancefb.hpp"
#include "scene/speciallogicimpl/speciallogicplatform.hpp"
#include "scene/speciallogicimpl/speciallogicguidefb.hpp"
#include "scene/speciallogicimpl/speciallogicmedal.hpp"
#include "scene/speciallogicimpl/speciallogicfantasyfb.hpp"
#include "scene/speciallogicimpl/speciallogicanimalrace.hpp"
#include "scene/speciallogicimpl/speciallogiconlyfight.hpp"
#include "other/robot/robotmanager.hpp"
#include "scene/speciallogicimpl/speciallogicfshitufb.hpp"
#include "scene/speciallogicimpl/speciallogicchiefelectionsea.hpp"
#include "scene/speciallogicimpl/speciallogicduchuangzeiying.hpp"
#include "scene/speciallogicimpl/speciallogiccatchfox.hpp"
#include "scene/speciallogicimpl/speciallogicstarking.hpp"
#include "scene/speciallogicimpl/speciallogicdreamnoteschallenge.hpp"
#include "scene/speciallogicimpl/speciallogicrafortunemaze.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen.hpp"
#include "scene/speciallogicimpl/speciallogicmijingqiwen2.hpp"
#include "scene/speciallogicimpl/speciallogicmaterial.hpp"
#include "scene/speciallogicimpl/speciallogicreliclooting.hpp"
#include "scene/speciallogicimpl/speciallogicsoulinzhen.hpp"
#include "scene/speciallogicimpl/speciallogicfishingground.hpp"
#include "other/petgodfb/speciallogicpetgodfb.hpp"
#include "other/wanlingfb/speciallogicwanlingfb.hpp"
#include "scene/speciallogicimpl/speciallogicweddingfb.hpp"
#include "scene/speciallogicimpl/speciallogicworldarenstandbyroom.hpp"
#include "scene/speciallogicimpl/speciallogicworldteamarena.hpp"
#include "scene/speciallogicimpl/speciallogicqingyuanshenghui.hpp"
#include "other/duel/duelconfig.hpp"
#include "scene/speciallogicimpl/speciallogicqingyuanduiduipeng.hpp"
#include "scene/speciallogicimpl/speciallogicsanxian.hpp"
#include "scene/speciallogicimpl/speciallogictuzikuaipao.hpp"
#include "scene/speciallogicimpl/speciallogicuta.hpp"
#include "scene/speciallogicimpl/speciallogichongmengtianjie.hpp"
#include "scene/speciallogicimpl/speciallogicgotoschool.hpp"

SpecialLogic::SpecialLogic(Scene *scene) : m_scene(scene), m_last_check_monster_status_time(0)
{
}

int SpecialLogic::GetSceneType()
{
	return m_scene->GetSceneType();
}

SpecialLogic* SpecialLogic::CreateSpecialLogic(Scene* scene, int scene_type)
{
	SpecialLogic* sp = NULL;
	switch (scene_type)
	{
	case Scene::SCENE_TYPE_DU_JIE:
		sp = new SpecialLogicFBDuJie(scene);
		break;
	case Scene::SCENE_TYPE_PET_GOD_FB:
		sp = new SpecialLogicPetGodFB(scene);
		break;
	case Scene::SCENE_TYPE_SOUL_IN_ZHEN:
		sp = new SpecialLogicSoulInZhen(scene);
		break;
	case Scene::SCENE_TYPE_RELIC_LOOTING:
		sp = new SpecialLogicRelicLootingFb(scene);
		break;
	case Scene::SCENE_TYPE_COMMON_SCENE:
		sp = new SpecialLogicCommon(scene);
		break;
	case Scene::SCENE_TYPE_GUAJI:
		sp = new SpecialLogicGuaji(scene);
		break;
	case Scene::SCENE_TYPE_COURAGE_CHALLENGE:
		sp = new SpecialLogicCourageChallenge(scene);
		break;
	case Scene::SCENE_TYPE_MIYI:
		sp = new SpecialLogicMiYi(scene);
		break;
	case Scene::SCENE_TYPE_CRYSTAL_FB:
		sp = new SpecialLogicCrystalFB(scene);
		break;
	case Scene::SCENE_TYPE_TEAM_FIGHT:
		sp = new SpecialLogicTeamFight(scene);
		break;
	case Scene::SCENE_TYPE_MAZE_RACER:
		sp = new SpecialLogicMazeRacer(scene);
		break;
	case Scene::SCENE_TYPE_GATHER_FB:
		sp = new SpecialLogicGatherFB(scene);
		break;
	case Scene::SCENE_TYPE_MAZE_RACER_WAITING_HALL:
		sp = new SpecialLogicMazeRacerWaitingHall(scene);
		break;
	case Scene::SCENE_TYPE_BRAVE_GROUND:
		sp = new SpecialLogicBraveGround(scene);
		break;
	case Scene::SCENE_TYPE_GUILD_FIGHT:
		sp = new SpecialLogicGuildFight(scene);
		break;
	case Scene::SCENE_TYPE_CHIEF_ELECTION:
		sp = new SpecialLogicChiefElection(scene);
		break;
	case Scene::SCENE_TYPE_CHIEF_PEAK_BATTLE:
		sp = new SpecialLogicChiefPeakBattle(scene);
		break;
	case Scene::SCENE_TYPE_GUILD_HONOR_BATTLE:
		sp = new SpecialLogicGuildHonorBattle(scene);
		break;
	case Scene::SCENE_TYPE_GUILD_ANSWER:
		sp = new SpecialLogicGuildAnswer(scene);
		break;
	case Scene::SCENE_TYPE_CHALLENGE_FB:
		sp = new SpecialLogicChallengeFb(scene);
		break;
	case Scene::SCENE_TYPE_ADVANCE_FB:
		sp = new SpecialLogicAdvanceFb(scene);
		break;
	case Scene::SCENE_TYPE_PLATFORM_BATTLE:
		sp = new SpecialLogicPlatformBattle(scene);
		break;
	case Scene::SCENE_TYPE_GUILD_STATION:
		sp = new SpecialLogicGuildStation(scene);
		break;
	case Scene::SCENE_TYPE_GUIDE_FB:
		sp = new SpecialLogicGuideFb(scene);
		break;
	case Scene::SCENE_TYPE_MEDAL_FB:
		sp = new SpecialLogicMedal(scene);
		break;
	case Scene::SCENE_TYPE_FANTASY_FB:
		sp = new SpecialLogicFantasyFb(scene);
		break;
	case Scene::SCENE_TYPE_ANIMAL_RACE:
		sp = new SpecialLogicAnimalRace(scene);
		break;
	case Scene::SCENE_TYPE_ONLY_FIGHT:
		sp = new SpecialLogicOnlyFight(scene);
		break;
	case Scene::SCENE_TYPE_SHI_TU:
		sp = new SpecialLogicShiTuFb(scene);
		break;
	case Scene::SCENE_TYPE_CHIEF_ELECTION_SEA:
		sp = new SpecialLogicChiefElectionSea(scene);
		break;
	case Scene::SCENE_TYPE_SHAN_HAI_BOSS:
		sp = new SpecialLogicShanHaiBoss(scene);
		break;
	case Scene::SCENE_TYPE_DU_CHUANG_ZEI_YING:
		sp = new SpecialLogicDuChuangZeiYing(scene);
		break;
	case Scene::SCENE_TYPE_WEN_XIANG_LOU:
		sp = new SpecialLogicWenXiangLouFb(scene);
		break;
	case Scene::SCENE_TYPE_CATCH_FOX:
		sp = new SpecialLogicCatchFoxFb(scene);
		break;
	case Scene::SCENE_TYPE_STAR_KING:
		sp = new SpecialLogicStarKingFb(scene);
		break;
	case Scene::SCENE_TYPE_DREAM_NOTES_CHALLENGE:
		sp = new SpecialLogicDreamNotesChallengeFb(scene);
		break;
	case Scene::SCENE_TYPE_FORTUNE_MAZE:
		sp = new SpecialLogicFortuneMaze(scene);
		break;
	case Scene::SCENE_TYPE_MI_JING_QI_WEN:
		sp = new SpecialLogicMiJingQiWenFB(scene);
		break;
	case Scene::SCENE_TYPE_MATERIAL_FB:
		sp = new SpecialLogicMaterial(scene);
		break;
	case Scene::SCENE_TYPE_MI_JING_QI_WEN_2:
		sp = new SpecialLogicMiJingQiWenFB2(scene);
		break;
	case Scene::SCENE_TYPE_WAN_LING_FB:
		sp = new SpecialLogicWanLingFB(scene);
		break;
	case Scene::SCENE_TYPE_FISHING_GROUND:
		sp = new SpecialLogicFishingGround(scene);
		break;
	case Scene::SCENE_TYPE_WEDDING_FB:
		sp = new SpecialLogicWeddingFB(scene);
		break;
	case Scene::SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM:
		sp = new SpecialLogicWorldArenaStandbyRoom(scene);
		break;
	case Scene::SCENE_TYPE_HUASHENG_MIJING_WAIT:
		sp = new SpecialLogicCommon(scene);
		break;
	case Scene::SCENE_TYPE_WORLD_TEAM_ARENA:
		sp = new SpecialLogicWorldTeamArena(scene);
		break;
	case Scene::SCENE_TYPE_QING_YUAN_SHENG_HUI:
		sp = new SpecialLogicQingYuanShengHui(scene);
		break;
	case Scene::SCENE_TYPE_SHI_TU_CHUAN_GONG:
		sp = new SpecialLogicShiTuChuanGong(scene);
		break;
	case Scene::SCENE_TYPE_QINGYUAN_DUIDUIPENG:
		sp = new SpecialLogicQingYuanDuiDuiPeng(scene);
		break;
	case Scene::SCENE_TYPE_EMPRTY_SCENE:
	case Scene::SCENE_TYPE_TAO_QUAN_NA_JI:
		sp = new SpecialLogicSanXianFb(scene);
		break;
	case Scene::SCENE_TYPE_TU_ZI_KUAI_PAO:
		sp = new SpecialLogicTuZiKuaiPao(scene);
		break;
	case Scene::SCENE_TYPE_UTA:
		sp = new SpecialLogicUTA(scene);
		break;
	case Scene::SCENE_TYPE_HONGMENG_TIANJIE:
		sp = new SpecialLogicHongMengTianJie(scene);
		break;
	case Scene::SCENE_TYPE_GOTO_SCHOOL:
		sp = new SpecialLogicGotoSchool(scene);
		break;
	}
	UNSTD_STATIC_CHECK(55 == Scene::SCENE_TYPE_COUNT);

	return sp;
}

void SpecialLogic::DelayKickOutRole(int role_id, int reason /* = CHANGE_SCENE_REASON_INVALID */, int delay_ms /* = 0 */)
{
	Role * role = m_scene->GetRoleByUID(role_id);
	if (NULL != role)
	{
		this->DelayKickOutRole(role, reason, delay_ms);
	}
}

void SpecialLogic::DelayKickOutRole(Role *role, int reason /* = CHANGE_SCENE_REASON_INVALID */, int delay_ms /* = 0 */)
{
	if (NULL == role) return;

	Posi target_pos(0, 0);
	int target_scene_id = role->GetLastScene(&target_pos);

	ChangeSceneDelayer *timer = new ChangeSceneDelayer(World::GetInstWorld()->GetSceneManager(), m_scene->GetIndex(), role->GetId(),
		role->GetUserId(), target_scene_id, 0, target_pos, reason);

	if (0 == delay_ms)
	{
		EngineAdapter::Instance().CreateTimer(1, timer);
	}
	else
	{
		EngineAdapter::Instance().CreateTimer(delay_ms, timer);
	}
}

void SpecialLogic::DelayKickOutAllRole(int reason /* = CHANGE_SCENE_REASON_INVALID */, int delay_ms /* = 0 */)
{
	for (int i = 0; i < (int)m_scene->RoleNum(); ++i)
	{
		Role *role = m_scene->GetRoleByIndex(i);
		if (NULL != role)
		{
			this->DelayKickOutRole(role, reason, delay_ms);
		}
	}
}

void SpecialLogic::ClearAllNpcMonster()
{
	for (int i = 0; i < (int)m_scene->NPCMonsterNum(); ++i)
	{
		NPCMonster * monster = m_scene->GetMonsterByIndex(i);
		if (NULL != monster) m_scene->DeleteObj(monster->GetId());
	}
}

bool SpecialLogic::CanEnterTeam(Role * role, Team * team, bool is_from_create)
{
	return m_scene->IsInStaticScene();
}

long long SpecialLogic::GetMineMonsterEncounterInterval(Role * role)
{
	return m_scene->RandMineMonsterEncounterTimeMS();
}

const AttributeList & SpecialLogic::ReCalcAttrPer(AttributeList & base_add, const AttributeList & cpy_attr_list)
{
	m_attr_inc.Reset();					// 通常场景不会给玩家加属性；
	base_add.Add(m_attr_inc.m_attrs);
	return m_attr_inc;
}

void SpecialLogic::UseGoldRecoveryHp(Role * role, int param1, int param2, int param3)
{
	const SundryOtherCfg & cfg = LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg();
	if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(cfg.recovery_need_gold))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	Protocol::SCTeamMemberAddHpMpNotice help_add_notice_protocol;
	help_add_notice_protocol.is_to_use_pet = 0;
	help_add_notice_protocol.partner_id = 0;
	help_add_notice_protocol.item_id =0;

	switch (param1)
	{
		case 0:		//加自己
			{
				if (!role->GetRoleModuleManager()->ChangeHp(INT_MAX))
				{
					role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
					return;
				}
			}
			break;
		case 1:		//加自己宠物 p2:pet_index
			{
				if (!role->GetRoleModuleManager()->GetPet()->ChangeHP(param2, INT_MAX, true))
				{
					return;
				}
			}
			break;
		case 2:		//加自己伙伴  p2:partner_id
			{
				if (!role->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CHP, INT_MAX))
				{
					return;
				}
			}
			break;
		case 3:		//给队友加,p2: team_member_index | p3:是否对宠物
			{
				Team * team = role->GetMyTeam();
				if (team != NULL)
				{
					TeamMember * team_member = team->GetMemberInfo(param2);
					if (team_member == NULL)
					{
						return;
					}

					if (!team_member->IsRobot())
					{
						Role* member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(team_member->uid));
						if (NULL != member_role)
						{
							if (param3 == 0)
							{
								if (!member_role->GetRoleModuleManager()->ChangeHp(INT_MAX))
								{
									role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
									return;
								}
							}
							else
							{
								if (!member_role->GetRoleModuleManager()->GetPet()->ChangeHP(member_role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX], INT_MAX))
								{
									return;
								}
							}

							help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
							help_add_notice_protocol.is_to_use_pet = param3 != 0;
							EngineAdapter::Instance().NetSend(member_role->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
						}					
					}
					else
					{
						Robot* member_robot = RobotManager::Instance().GetRobot(team_member->uid);
						if (NULL != member_robot)
						{
							if (param3 == 0)
							{
								if (!member_robot->ChangeHp(INT_MAX))
								{
									role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
									return;
								}
							}
							else
							{
								if (!member_robot->ChangePetHp(INT_MAX))
								{
									role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
									return;
								}
							}
						}

						
					}
				}
			}
			break;
		case 4:		//给队长的伙伴加,p2: 伙伴id
			{
				Team * team = role->GetMyTeam();
				if (team == NULL)
				{
					return;
				}

				Role * leader = team->GetLeaderRole();
				if (leader == NULL)
				{
					return;
				}

				if (!leader->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CHP, INT_MAX))
				{
					return;
				}

				help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
				help_add_notice_protocol.partner_id = param2;
				EngineAdapter::Instance().NetSend(leader->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));

			}
			break;
	}

	role->GetRoleModuleManager()->GetMoney()->UseGold(cfg.recovery_need_gold, __FUNCTION__);

	Protocol::SCUseRecoveryNotice use_notice;		//发给使用者 飘字
	use_notice.item_id =0;
	use_notice.use_gold_type = 1;
	use_notice.param1 = param1;
	use_notice.param2 = param2;
	use_notice.param3 = param3;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &use_notice, sizeof(use_notice));
}

void SpecialLogic::UseGoldRecoveryMp(Role * role, int param1, int param2, int param3)
{
	const SundryOtherCfg & cfg = LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg();

	if (!role->GetRoleModuleManager()->GetMoney()->GoldMoreThan(cfg.recovery_need_gold))
	{
		role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}

	Protocol::SCTeamMemberAddHpMpNotice help_add_notice_protocol;
	help_add_notice_protocol.from_team_member_index = -1;
	help_add_notice_protocol.is_to_use_pet = 0;
	help_add_notice_protocol.partner_id = 0;
	help_add_notice_protocol.item_id = 0;

	switch (param1)
	{
		case 0:		//加自己魔
			{
				if (!role->GetRoleModuleManager()->ChangeMp(INT_MAX))
				{
					role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
					return;
				}
			}
			break;
		case 1:		//加自己宠物魔 p2:pet_index
			{
				if (!role->GetRoleModuleManager()->GetPet()->ChangeMp(param2, INT_MAX, true))
				{
					return;
				}
			}
			break;
		case 2:		//加自己伙伴 p2:partner_id
			{
				if (!role->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CMP, INT_MAX))
				{
					return;
				}
			}
			break;
		case 3:		//给队友加魔,p2: team_member_index | p3:是否对宠物
			{
				Team * team = role->GetMyTeam();
				if (team == NULL)
				{
					return;
				}
				TeamMember * team_member = team->GetMemberInfo(param2);

				if (team_member == NULL)
				{
					return;
				}

				if (!team_member->IsRobot())
				{
					Role* member_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(team_member->uid));
					if (NULL != member_role)
					{
						if (param3 == 0)
						{
							if (!member_role->GetRoleModuleManager()->ChangeMp(INT_MAX))
							{
								role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
								return;
							}
						}
						else
						{
							if (!member_role->GetRoleModuleManager()->GetPet()->ChangeMp(member_role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX], INT_MAX))
							{
								return;
							}
						}

						help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
						help_add_notice_protocol.is_to_use_pet = param3 != 0;
						EngineAdapter::Instance().NetSend(member_role->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));
					}			
				}
				else
				{
					Robot* robot = RobotManager::Instance().GetRobot(team_member->uid);
					if (NULL != robot)
					{
						if (param3 == 0)
						{

							if (!robot->ChangeMp(INT_MAX))
							{
								role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
								return;
							}
						}
						else
						{
							if (!robot->ChangePetMp(INT_MAX))
							{
								role->NoticeNum(errornum::EN_ATTR_HAS_FULL_ERR);
								return;
							}
						}
					}				
				}

			}
			break;
		case 4:		//给队长的伙伴加魔 , p2: 伙伴id
			{
				Team * team = role->GetMyTeam();
				if (team == NULL)
				{
					return;
				}

				Role * leader = team->GetLeaderRole();
				if (leader == NULL)
				{
					return;
				}

				if (!leader->GetRoleModuleManager()->GetPartner()->AlterPartnerHPOrMPValue(param2, MAX_TYPE_ONLY_CMP, INT_MAX))
				{
					return;
				}

				help_add_notice_protocol.from_team_member_index = team->GetMemberIndex(role);
				help_add_notice_protocol.partner_id = param2;
				EngineAdapter::Instance().NetSend(leader->GetNetId(), &help_add_notice_protocol, sizeof(help_add_notice_protocol));

			}
			break;
		default:
			return;
	}

	role->GetRoleModuleManager()->GetMoney()->UseGold(cfg.recovery_need_gold,__FUNCTION__);

	Protocol::SCUseRecoveryNotice use_notice;		//发给使用者 飘字
	use_notice.item_id = 0;
	use_notice.use_gold_type = 2;
	use_notice.param1 = param1;
	use_notice.param2 = param2;
	use_notice.param3 = param3;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &use_notice, sizeof(use_notice));
}

bool SpecialLogic::CanChangeProf(Role * role)
{
	return true;
}

bool SpecialLogic::IsAllMonsterDead()
{
	//TODO 关底怪可以尝试用隐藏怪逻辑实现
	//if (m_scene->HasHiddenMonster())
	//{
	//	return false;
	//}

	if (m_scene->NPCMonsterNum() <= 0) return true;

	return false;
}

void SpecialLogic::CheckSceneMonsterStatus(unsigned long interval, time_t now_second)
{
	if (now_second >= m_last_check_monster_status_time + 1)
	{
		m_last_check_monster_status_time = static_cast<unsigned int>(now_second);

		if (this->IsAllMonsterDead()) this->OnAllMonsterDead();
	}
}

void SpecialLogic::SyncHpMp(const char * sync_data)
{
	BattleFBData* data = (BattleFBData*)sync_data;
	for (int i = 0; i < data->role_count; ++i)
	{
		Role* role = m_scene->GetRoleByUID(data->role_list[i].role_id);
		if (NULL != role)
		{
			role->GetSceneStatus()->UpdateRole(data->role_list[i].role_cur_hp, data->role_list[i].role_cur_mp);

			for (int k = 0; k < data->role_list[i].pet_count; ++k)
			{
				BattleFBData::PetInfo& pet_info = data->role_list[i].pet_list[k];
				role->GetSceneStatus()->UpdatePet(pet_info.pet_id, pet_info.pet_origin_index, pet_info.pet_cur_hp, pet_info.pet_cur_mp);
			}
		}
	
		else if (RobotManager::IsRobot(data->role_list[i].role_id))
		{
			Robot* robot = RobotManager::Instance().GetRobot(data->role_list[i].role_id);
			if (NULL != robot)
			{
				robot->UpdateHpMp(data->role_list[i].role_cur_hp, data->role_list[i].role_cur_mp);
				for (int k = 0; k < data->role_list[i].pet_count; ++k)
				{
					BattleFBData::PetInfo& pet_info = data->role_list[i].pet_list[k];
					robot->UpdatePetHpMp(pet_info.pet_id, pet_info.pet_origin_index, pet_info.pet_cur_hp, pet_info.pet_cur_mp);
				}
			}
		}
	}

	Role* partner_owner_role = m_scene->GetRoleByUID(data->partner_belong_uid);
	if (NULL != partner_owner_role)
	{
		for (int i = 0; i < data->partner_count; ++i)
		{
			partner_owner_role->GetSceneStatus()->UpdatePartner(data->partner_list[i].partner_id,
				data->partner_list[i].partner_cur_hp, data->partner_list[i].partner_cur_mp);
		}
	}
}

void SpecialLogic::DuelOnFightFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	if (NULL == ack)
	{
		return;
	}

	if (BATTLE_MODE_DUEL != ack->battle_mode)
	{
		return;
	}

	std::set<int> tmp_atk_uid_list, tmp_def_uid_list;
	for (int atk_idx = 0; atk_idx < ack->attacker_list.role_count; ++atk_idx)
	{
		tmp_atk_uid_list.insert(ack->attacker_list.role_list[atk_idx].uid);
	}

	for (int def_idx = 0; def_idx < ack->defender_list.role_count; ++def_idx)
	{
		tmp_def_uid_list.insert(ack->defender_list.role_list[def_idx].uid);
	}

	int att_idx = ack->business_data.param3 % 10;
	int def_idx = ack->business_data.param3 / 10;

	int att_uid = ack->attacker_list.role_list[att_idx].uid;
	int def_uid = ack->defender_list.role_list[def_idx].uid;

	bool att_is_team = false, def_is_team = false;
	bool att_is_red_name = false, def_is_red_name = false;

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		Role * att_role = m_scene->GetRoleByUID(ack->attacker_list.role_list[i].uid);
		if (NULL != att_role)
		{
			if (att_role->GetRoleModuleManager()->GetDuel()->IsRedName())
			{
				att_is_red_name = true;
			}

			Team * att_team = att_role->GetMyTeam();
			if (NULL != att_team)
			{
				att_is_team = true;
			}
		}
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		Role * def_role = m_scene->GetRoleByUID(ack->defender_list.role_list[i].uid);
		if (NULL != def_role)
		{
			if (def_role->GetRoleModuleManager()->GetDuel()->IsRedName())
			{
				def_is_red_name = true;
			}

			Team * def_team = def_role->GetMyTeam();
			if (NULL != def_team)
			{
				def_is_team = true;
			}
		}
	}

	GameName att_name = { 0 };
	GameName def_name = { 0 };
	GameName win_name = { 0 };
	GameName lose_name = { 0 };

	Int64 reduce_coin = LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg().reduce_coin;
	Int64 att_coin = reduce_coin;
	Int64 def_coin = reduce_coin;
	Int64 win_coin = 0;
	Int64 lose_coin = 0;

	Role* att_role = World::GetInstWorld()->GetSceneManager()->GetRole(att_uid);
	if (NULL != att_role)
	{
		att_role->GetName(att_name);
		if (att_role->GetRoleModuleManager()->GetMoney()->GetCoinBind() < reduce_coin)
		{
			att_coin = att_role->GetRoleModuleManager()->GetMoney()->GetCoinBind();
		}
	}
	else
	{
		UserCacheNode * att_cache = UserCacheManager::Instance().GetUserNode(att_uid);
		if (NULL != att_cache)
		{
			att_cache->GetName(att_name);
		}
	}

	Role* def_role = World::GetInstWorld()->GetSceneManager()->GetRole(def_uid);
	if (NULL != def_role)
	{
		def_role->GetName(def_name);
		if (def_role->GetRoleModuleManager()->GetMoney()->GetCoinBind() < reduce_coin)
		{
			def_coin = def_role->GetRoleModuleManager()->GetMoney()->GetCoinBind();
		}
	}
	else
	{
		UserCacheNode * def_cache = UserCacheManager::Instance().GetUserNode(def_uid);
		if (NULL != def_cache)
		{
			def_cache->GetName(def_name);
		}
	}

	switch (ack->battle_result)
	{
		case RESULT_TYPE_ATTACKER_WIN:
		{
			this->OnDuelFightOver(tmp_atk_uid_list, ack->business_data.param1, true, true, ack->business_data.param2, def_is_team, def_is_red_name, def_name, def_coin);
			this->OnDuelFightOver(tmp_def_uid_list, ack->business_data.param1, false, false, ack->business_data.param2, att_is_team, att_is_red_name, att_name, def_coin);
			F_STRNCPY(win_name, att_name, sizeof(GameName));
			F_STRNCPY(lose_name, def_name, sizeof(GameName));
			win_coin = att_coin;
			lose_coin = def_coin;
		}
		break;
		case RESULT_TYPE_DEFENDER_WIN:
		{
			this->OnDuelFightOver(tmp_atk_uid_list, ack->business_data.param1, true, false, ack->business_data.param2, def_is_team, def_is_red_name, def_name, att_coin);
			this->OnDuelFightOver(tmp_def_uid_list, ack->business_data.param1, false, true, ack->business_data.param2, att_is_team, att_is_red_name, att_name, att_coin);
			F_STRNCPY(win_name, def_name, sizeof(GameName));
			F_STRNCPY(lose_name, att_name, sizeof(GameName));
			win_coin = def_coin;
			lose_coin = att_coin;
		}
		break;
		case RESULT_TYPE_DRAW:
		break;
		case RESULT_TYPE_FORCED_FINISH:		// 平局或战斗中断，算双方均失败；
		case RESULT_TYPE_EXCEED_MAX_ROUND:
		case RESULT_TYPE_EXCEED_MAX_TIME:
		{

		}
		break;
	}

	if (DUEL_BATTLE_TYPE_DUEL == ack->business_data.param1)
	{
		int length = 0;
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_end_duel_battle,
			win_name, lose_name, lose_name, lose_name, (int)lose_coin);

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
				0, 0, 0, 0);
		}
	}

	if (DUEL_BATTLE_TYPE_ADJUDICATION == ack->business_data.param1)
	{
		if (RESULT_TYPE_ATTACKER_WIN == ack->battle_result)
		{
			int length = 0;
			if (def_is_team)
			{
				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_begin_duel_two_battle_team, att_name, def_name);
			}
			else
			{
				length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_begin_duel_two_battle, att_name, def_name);
			}
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
					0, 0, 0, 0);
			}
		}
		else if(RESULT_TYPE_DEFENDER_WIN == ack->battle_result)
		{
			int length = 0;
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::duel_lose_duel_two, att_name, def_name, def_name);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
					0, 0, 0, 0);
			}
		}
	}

	gamelog::g_log_duel.printf(LL_INFO, "%s DUEL_END att[%s][%d %d %d %d %d] def[%s][%d %d %d %d %d] duel_type[%d] battle_result[%d] level_diff[%d] param3[%d]", __FUNCTION__,
		att_name, ack->attacker_list.role_list[0].uid, ack->attacker_list.role_list[1].uid, ack->attacker_list.role_list[2].uid, ack->attacker_list.role_list[3].uid, ack->attacker_list.role_list[4].uid,
		def_name, ack->defender_list.role_list[0].uid, ack->defender_list.role_list[1].uid, ack->defender_list.role_list[2].uid, ack->defender_list.role_list[3].uid, ack->defender_list.role_list[4].uid,
		ack->business_data.param1, ack->battle_result, ack->business_data.param2, ack->business_data.param3);
}

void SpecialLogic::OnDuelFightOver(std::set<int> uid_list, int duel_type, bool is_attack, bool is_win, int level_diff, bool target_is_team, bool target_is_red, GameName name, UInt64 coin)
{
	for (std::set<int>::iterator it = uid_list.begin(); it != uid_list.end(); ++it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*it);
		if (NULL == role)
		{
			OfflineRegisterManager::Instance().OnSetDuelFight(*it, duel_type, is_attack, is_win, level_diff);
			continue;
		}

		int ret = role->GetRoleModuleManager()->GetDuel()->OnBattleFinish(duel_type, is_attack, is_win, level_diff, m_scene->GetSceneID(), coin);

		role = World::GetInstWorld()->GetSceneManager()->GetRole(*it);
		if (NULL == role) continue;
		role->GetRoleModuleManager()->GetDuel()->NoticeResult(duel_type, is_attack, is_win, abs(ret), target_is_team, name, target_is_red, coin);
	}
}

