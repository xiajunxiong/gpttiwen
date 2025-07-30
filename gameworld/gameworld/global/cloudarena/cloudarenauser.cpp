#include "cloudarenauser.hpp"
#include "other/robot/robotconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "skill/skillpool.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/capability/capability.hpp"
#include "cloudarenaconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/protobuf/role_battle_data.pb.h"
#include "servercommon/protobufmanager.hpp"
#include "gamelog.h"
#include "cloudarena.hpp"
#include "other/partner/partnerconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/pet/pet.hpp"
#include "protocol/msgcloudarena.h"
#include "other/partner/partner.hpp"
#include "servercommon/errornum.h"
#include "protocol/msgformation.h"
#include "other/rolebattledatacapabilitycalculator.hpp"

CloudArenaUser::CloudArenaUser() : m_uid(0), m_level(0), m_show_level(0), m_profession(0), m_avatar(0), m_capability(0),
	m_score(0), m_remain_fighttimes(0), m_today_buy_fighttimes_times(0), m_next_recover_fighttimes_timestamp(0),
	m_last_time_win(false), m_last_time_lose(false), m_next_manual_refresh_timestamp(0),
	m_is_dirty(false), m_old_state(false), m_reach_score_timestamp(0u), m_last_season_rank(-1), m_ranking_reward_flag(0),
	m_reach_ranking_flag(0), m_season_final_rank(-1), m_season_initial_score(0), m_day_reward_status(0),
	m_season_timestamp(0u), m_hidden_server_idx(0), m_season_highest_rank(-1), m_season_highest_dan_id(0),
	m_last_day_score(0), m_achievement(this)
{
	memset(m_name, 0, sizeof(m_name));
	memset(m_target_uid_list, 0, sizeof(m_target_uid_list));
	m_today_challenge_list.reserve(CLOUD_ARENA_TODAY_CHALLENGE_LIST_SIZE);
}

CloudArenaUser::~CloudArenaUser()
{

}

void CloudArenaUser::Update(unsigned long interval, unsigned int now_second, ARG_OUT bool* has_change)
{
	if (this->IsRobot())
	{
		return;
	}

	//屏蔽每隔一段时间回复一次次数
	//bool is_send_info = false;
	//if (now_second >= m_next_recover_fighttimes_timestamp)
	//{
	//	m_next_recover_fighttimes_timestamp = now_second + LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().recovery_time_s;
	//	int max_times = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().max_store_fight_times;
	//	if (m_remain_fighttimes < max_times)
	//	{
	//		m_remain_fighttimes += 1;
	//		if (NULL != has_change)
	//		{
	//			*has_change = true;
	//		}
	//	}
	//
	//	is_send_info = true;
	//}

	//if (is_send_info)
	//{
	//	CloudArena::Instance().SendUserSelfInfo(m_uid, this);
	//}
}

void CloudArenaUser::OnDayChange(ARG_OUT bool* has_change)
{
	if (this->IsRobot())
	{
		return;
	}
	
	m_day_reward_status = CLOUD_ARENA_DAY_REWARD_NOT_CALIM;
	m_today_buy_fighttimes_times = 0;
	m_today_challenge_list.clear();

	m_remain_fighttimes = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().max_store_fight_times;
	
	this->ResetManualRefreshTimes();
	this->UpdateLastDayScore();

	if (NULL != has_change) *has_change = true;
	
	CloudArena::Instance().SendUserSelfInfo(m_uid, this);
}

void CloudArenaUser::UpdateLastDayScore()
{
	m_last_day_score = m_score;

	if (1 == m_other_param.new_season_first_time_update_last_day_score)
	{
		m_other_param.new_season_first_time_update_last_day_score = 0;
		m_last_day_score = m_other_param.last_season_final_score;
	}

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::UpdateLastDayScore user[%d,%s] last_day_score[%d]",
		m_uid, m_name, m_last_day_score);
}

void CloudArenaUser::CheckFormation(Role* role)
{
	for (int i = m_battle_data.pet_num - 1; i >= 0; --i)
	{
		int pet_id = m_battle_data.pet_character_data[i].battle_data.character_id;
		long long unique_id = m_battle_data.pet_character_data[i].unique_id;
		if (NULL == role->GetRoleModuleManager()->GetPet()->GetPetParam(pet_id, unique_id, NULL))
		{
			if (i == m_battle_data.fight_pet_idx)
			{
				m_battle_data.fight_pet_idx = -1;
			}

			if (i != m_battle_data.pet_num - 1)
			{
				m_battle_data.pet_character_data[i] = m_battle_data.pet_character_data[m_battle_data.pet_num - 1];
			}
			
			m_battle_data.pet_num -= 1;
		}
	}

	for (int i = m_battle_data.pet_helper_num - 1; i >= 0; --i)
	{
		int pet_id = m_battle_data.pet_helper_character_data[i].pet_data.battle_data.character_id;
		long long unique_id = m_battle_data.pet_helper_character_data[i].pet_data.unique_id;
		if (NULL == role->GetRoleModuleManager()->GetPet()->GetPetParam(pet_id, unique_id, NULL))
		{
			if (i != m_battle_data.pet_helper_num - 1)
			{
				m_battle_data.pet_helper_character_data[i] = m_battle_data.pet_helper_character_data[m_battle_data.pet_helper_num - 1];
			}

			m_battle_data.pet_helper_num -= 1;
		}
	}
}

bool CloudArenaUser::OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req, Role* role)
{
	role->GetRoleModuleManager()->CollectRoleBattleData(&m_battle_data, false, true, false);

	m_battle_data.pet_num = 0;
	m_battle_data.partner_num = 0;
	m_battle_data.pet_helper_num = 0;

	int role_row = 0;
	if (PET_HELPER_FORMATION_GRID_ROLE == req->grid_list[7].type)
	{
		role_row = 1;
	}
	m_battle_data.role_stand_row = role_row;

	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		const Protocol::CSSetPetHelperFormation::GridInfo & curr= req->grid_list[i];
		if (PET_HELPER_FORMATION_GRID_INVALID == req->grid_list[i].type ||
			PET_HELPER_FORMATION_GRID_ROLE == req->grid_list[i].type)
		{
			continue;
		}
		else if (PET_HELPER_FORMATION_GRID_PET == req->grid_list[i].type)
		{			
			int pet_id = req->grid_list[i].id;
			long long unique_id = req->grid_list[i].unique_id;
			int pet_index = -1;
			if (NULL == role->GetRoleModuleManager()->GetPet()->GetPetParam(pet_id, unique_id, &pet_index))
			{
				continue;
			}

			if (i == 2 || i == 7)
			{
				PetBattleData& pet_data = m_battle_data.pet_character_data[0];
				if (!role->GetRoleModuleManager()->GetPet()->GetBattleCharacterData(pet_index, &pet_data, false))
				{
					continue;
				}
				m_battle_data.pet_num = 1;
				m_battle_data.fight_pet_idx = 0;

				pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
				pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;

				if (pet_data.auto_move_type != m_battle_data.auto_second_move_type ||
					pet_data.auto_skill_id != m_battle_data.auto_second_skill_id ||
					pet_data.auto_skill_level != m_battle_data.auto_second_skill_level)
				{
					pet_data.auto_move_type = req->auto_info.battle_auto_second_move_type;
					pet_data.auto_skill_id = req->auto_info.battle_auto_second_skill_id;
					pet_data.auto_skill_level = req->auto_info.battle_auto_second_skill_level;
				}
			}
			else if (m_battle_data.pet_helper_num < ARRAY_LENGTH(m_battle_data.pet_helper_character_data))
			{
				PetHelperBattleData& pet_helper_data = m_battle_data.pet_helper_character_data[m_battle_data.pet_helper_num];
				if (!role->GetRoleModuleManager()->GetPet()->GetBattleCharacterData(pet_index, &pet_helper_data.pet_data, false))
				{
					continue;
				}
				pet_helper_data.pet_data.battle_data.character_type = BATTLE_CHARACTER_TYPE_PET_SHADOW;
				pet_helper_data.battle_row = i / ROW_CHARACTER_NUM;
				pet_helper_data.battle_column = i % ROW_CHARACTER_NUM;
				pet_helper_data.pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
				pet_helper_data.pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;
				m_battle_data.pet_helper_num += 1;
			}
		}
		else if (PET_HELPER_FORMATION_GRID_PARTNER == req->grid_list[i].type)
		{
			if (m_battle_data.partner_num >= ARRAY_LENGTH(m_battle_data.partner_character_data))
			{
				continue;
			}

			PartnerBattleData& partner_data = m_battle_data.partner_character_data[m_battle_data.partner_num];
			if (!role->GetRoleModuleManager()->GetPartner()->GetBattleCharacterData(req->grid_list[i].id, &partner_data, NULL, 0, false))
			{
				continue;
			}
			partner_data.battle_row = i / ROW_CHARACTER_NUM;
			partner_data.battle_column = i % ROW_CHARACTER_NUM;
			m_battle_data.partner_num += 1;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(req->standby_list); ++i)
	{
		if (m_battle_data.pet_num >= ARRAY_LENGTH(m_battle_data.pet_character_data))
		{
			continue;
		}

		const Protocol::CSSetPetHelperFormation::PetStandbyInfo & curr = req->standby_list[i];
		int pet_id = req->standby_list[i].pet_id;
		long long unique_id = req->standby_list[i].unique_id;
		int pet_index = -1;
		if (NULL == role->GetRoleModuleManager()->GetPet()->GetPetParam(pet_id, unique_id, &pet_index))
		{
			continue;
		}

		PetBattleData& pet_data = m_battle_data.pet_character_data[m_battle_data.pet_num];
		if (!role->GetRoleModuleManager()->GetPet()->GetBattleCharacterData(pet_index, &pet_data, false))
		{
			continue;
		}
		pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
		pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;
		m_battle_data.pet_num += 1;
	}

	m_battle_data.auto_first_move_type = req->auto_info.battle_auto_first_move_type;
	m_battle_data.auto_first_skill_id = req->auto_info.battle_auto_first_skill_id;
	m_battle_data.auto_first_skill_level = req->auto_info.battle_auto_first_skill_level;
	m_battle_data.auto_second_move_type = req->auto_info.battle_auto_second_move_type;
	m_battle_data.auto_second_skill_id = req->auto_info.battle_auto_second_skill_id;
	m_battle_data.auto_second_skill_level = req->auto_info.battle_auto_second_skill_level;

	this->OnRoleBattleDataChanged();
	this->UpdateRoleBasicInfo(role);
	return true;
}

void CloudArenaUser::UpdateRoleBasicInfo(Role* role)
{
	if (NULL == role) return;

	m_level = role->GetLevel();
	F_STRNCPY(m_name, role->GetName(), sizeof(GameName));
	m_avatar = role->GetAvatarType();
	m_profession = role->GetProfession();
	m_appearance = role->GetRoleModuleManager()->GetAppearance();
}

bool CloudArenaUser::FindTargetByUid(int target_uid, ARG_OUT int* target_seq /* = NULL */)
{
	for (int i = 0; i < ARRAY_LENGTH(m_target_uid_list); ++i)
	{
		if (target_uid == m_target_uid_list[i])
		{
			if (NULL != target_seq)
			{
				*target_seq = i;
			}
			return true;
		}
	}

	return false;
}

void CloudArenaUser::OnFetchRankingReward(int rank_id)
{
	m_ranking_reward_flag |= (1 << rank_id);
}

bool CloudArenaUser::FillRobotData(int id)
{
	UserID robot_uid(CLOUD_ARENA_ROBOT_SERVER_ID, id);
	m_uid = UidToInt(robot_uid);
	const CloudArenaRobotCfg* robot_cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetRobotCfg(id);
	if (NULL == robot_cfg) return false;

	m_level = GetMax(robot_cfg->level_start, CloudArena::Instance().GetSeasonWorldLevel() - robot_cfg->world_level_diff);
	m_show_level = m_level;
	m_score = robot_cfg->score;

	RoleBattleData & battle_data = m_battle_data;
	battle_data.is_auto = 1;
	battle_data.role_stand_row = BATTLE_ROW_TYPE_BACK;

	const RobotRoleCfg* robot_role = ROBOTCONFIG->RandRobotByLevel(m_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_role)
	{
		return false;
	}

	int group_index = LOGIC_CONFIG->GetCloudArenaConfig()->GetRobotAttrAdjustGroupIndexByWorldLevel(m_level);
	if (group_index < 0 || group_index >= ARRAY_ITEM_COUNT(robot_cfg->attr_adjust_group_list))
	{
		return false;
	}

	int adjust_group_id = robot_cfg->attr_adjust_group_list[group_index];
	const CloudArenaRobotAttrAdjustCfg* adjust_cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetRobotAttrAdjustCfg(adjust_group_id);
	if (NULL == adjust_cfg)
	{
		return false;
	}

	m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	m_avatar = AppearanceConfig::Instance().RandAvatarByProfession(m_profession);

	GameName robot_name;
	ROBOTCONFIG->GetRandomName(SEX_ARRAY[m_avatar], robot_name, false);
	F_STRNCPY(m_name, robot_name, sizeof(GameName));

	m_appearance.Reset();
	m_appearance.advance_times = robot_role->advance_times;
	m_appearance.avatar_type = m_avatar;
	m_appearance.headshot_id = 0;
	m_appearance.color = rand() % INIT_APPREARANCE_COLOR_NUM;
	m_appearance.prof_base = robot_role->base_prof;
	m_appearance.weapon_id = robot_role->weapon_id;
	m_appearance.sub_weapon_id = 0;

	battle_data.appearance = m_appearance;

	BattleCharacterData & role_data = battle_data.role_character_data;
	role_data.character_id = m_uid;
	role_data.character_type = BATTLE_CHARACTER_TYPE_ROLE;
	role_data.level = m_level;
	F_STRNCPY(role_data.name, robot_name, sizeof(GameName));
	role_data.profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
	role_data.weapon_type = robot_role->weapon_type;

	const RobotAttrCon & robot_attr = robot_role->attr_con;
	role_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_attr.max_hp;
	role_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_attr.max_mp;
	memcpy(role_data.attr_list, robot_attr.basic_attr_list, sizeof(robot_attr.basic_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_attr.amend_attr_list, sizeof(robot_attr.amend_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_attr.resis_attr_list, sizeof(robot_attr.resis_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ELEMENT_BEGIN], robot_attr.element_list, sizeof(robot_attr.element_list));

	{
		this->AdjustRobotAttr(role_data.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->role_attack_adjust);
		this->AdjustRobotAttr(role_data.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->role_mental_adjust);
		this->AdjustRobotAttr(role_data.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->role_recover_adjust);
		this->AdjustRobotAttr(role_data.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->role_maxhp_adjust);
	}

	for (int i = 0; i < ROBOT_ROLE_SKILL_MAX && role_data.skill_num < MAX_BATTLE_SKILL_NUM; ++i)
	{
		const RobotSkillCon & robot_skill = robot_attr.skill_list[i];
		if (NULL == SkillPool::Instance().GetSkillCfg(robot_skill.skill_id) || robot_skill.skill_lv <= 0) continue;

		BattleCharacterData::SkillItem & skill = role_data.skill_list[role_data.skill_num++];
		skill.skill_id = robot_skill.skill_id;
		skill.skill_level = robot_skill.skill_lv;
	}

	// 宠物数据	
	PetBattleData& pet_data = battle_data.pet_character_data[0];
	if (!this->FillRobotPetData(pet_data, m_level, adjust_cfg, false))
	{
		return false;
	}
	battle_data.pet_num = 1;

	// 伙伴数据
	int partner_list[ROW_CHARACTER_NUM - 1] = { 0 };
	int partner_num = 0;
	battle_data.partner_num = 0;
	ROBOTCONFIG->RandRobotPartnerList(m_level, ROW_CHARACTER_NUM - 1, &partner_num, partner_list);
	for (int i = 0; i < partner_num && i < ARRAY_LENGTH(BATTLE_POS_RELATION) - 1; ++i)
	{
		int partner_id = partner_list[i];
		PartnerBattleData& partner_data = battle_data.partner_character_data[battle_data.partner_num];
		if (!this->FillRobotPartnerData(partner_data, m_level, partner_id, adjust_cfg))
		{
			continue;
		}
		partner_data.battle_row = BATTLE_ROW_TYPE_BACK;
		partner_data.battle_column = BATTLE_POS_RELATION[i + 1];
		battle_data.partner_num += 1;
	}

	// 宠物助战
	battle_data.pet_helper_num = 0;
	for (int i = 0; i < robot_cfg->pet_helper_num && i < ARRAY_LENGTH(BATTLE_POS_RELATION) - 1; ++i)
	{
		const RobotPetCfg* robot_pet = ROBOTCONFIG->RandRobotPetByLevel(m_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
		if (NULL == robot_pet) continue;

		PetHelperBattleData& pet_helper_data = battle_data.pet_helper_character_data[battle_data.pet_helper_num];
		if (!this->FillRobotPetHelperData(pet_helper_data, m_level, adjust_cfg))
		{
			continue;
		}
		pet_helper_data.battle_row = BATTLE_ROW_TYPE_FRONT;
		pet_helper_data.battle_column = BATTLE_POS_RELATION[i + 1];
		battle_data.pet_helper_num += 1;
	}

	m_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);

	return true;
}

bool CloudArenaUser::FillRobotPetData(PetBattleData& data, int robot_level, const CloudArenaRobotAttrAdjustCfg* adjust_cfg, bool is_pet_helper)
{
	const RobotPetCfg* robot_pet = ROBOTCONFIG->RandRobotPetByLevel(robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_pet) return false;

	BattleCharacterData& pet_data = data.battle_data;
	pet_data.character_id = robot_pet->pet_id;
	pet_data.character_type = BATTLE_CHARACTER_TYPE_PET;
	if (is_pet_helper)
	{
		pet_data.character_type = BATTLE_CHARACTER_TYPE_PET_SHADOW;
	}
	pet_data.level = m_level;
	pet_data.param = robot_pet->attr_con.strengthen_level;
	F_STRNCPY(pet_data.name, robot_pet->name, sizeof(GameName));
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(robot_pet->pet_id);
	if (NULL != pet_cfg)
	{
		pet_data.race = pet_cfg->race;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_WATER] = pet_cfg->element_water;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = pet_cfg->element_fire;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_WIND] = pet_cfg->element_wind;
		pet_data.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = pet_cfg->element_land;
	}

	// 属性用robot_pet_attr的
	pet_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_pet->attr_con.max_mp;
	pet_data.attr_list[BATTLE_ATTR_CUR_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_CUR_MP] = robot_pet->attr_con.max_mp;
	memcpy(pet_data.attr_list, robot_pet->attr_con.basic_attr_list, sizeof(robot_pet->attr_con.basic_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_pet->attr_con.amend_attr_list, sizeof(robot_pet->attr_con.amend_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_pet->attr_con.resis_attr_list, sizeof(robot_pet->attr_con.resis_attr_list));

	{
		this->AdjustRobotAttr(pet_data.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->pet_attack_adjust);
		this->AdjustRobotAttr(pet_data.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->pet_mental_adjust);
		this->AdjustRobotAttr(pet_data.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->pet_recover_adjust);
		this->AdjustRobotAttr(pet_data.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->pet_maxhp_adjust);
	}

	pet_data.skill_num = 0; // 宠物技能在战斗服加载

	return true;
}

bool CloudArenaUser::FillRobotPartnerData(PartnerBattleData& partner_data, int robot_level, int partner_id, const CloudArenaRobotAttrAdjustCfg* adjust_cfg)
{
	const RobotPartnerCfg* robot_partner = ROBOTCONFIG->GetPartner(partner_id, m_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_partner) return false;

	const PartnerBasicCfg* partner_basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == partner_basic_cfg) return false;

	BattleCharacterData& bcd = partner_data.battle_data;
	bcd.Reset();
	bcd.level = m_level;
	bcd.character_id = partner_id;
	bcd.profession = partner_basic_cfg->profession;
	bcd.character_type = BATTLE_CHARACTER_TYPE_PARTNER;
	F_STRNCPY(bcd.name, robot_partner->name, sizeof(bcd.name));
	bcd.race = partner_basic_cfg->race_type;
	bcd.is_long_range_unit = static_cast<char>(partner_basic_cfg->is_long_range_unit);

	CLEAR_PARTNER_CRYSTAL(bcd.attr_list);
	PARTNER_CRYSTAL_PUT(partner_basic_cfg->crystal_item, bcd.attr_list);

	bcd.attr_list[BATTLE_ATTR_MAX_HP] = robot_partner->attr_con.max_hp;
	bcd.attr_list[BATTLE_ATTR_MAX_MP] = robot_partner->attr_con.max_mp;
	bcd.attr_list[BATTLE_ATTR_CUR_HP] = robot_partner->attr_con.max_hp;
	bcd.attr_list[BATTLE_ATTR_CUR_MP] = robot_partner->attr_con.max_mp;
	memcpy(bcd.attr_list, robot_partner->attr_con.basic_attr_list, sizeof(robot_partner->attr_con.basic_attr_list));
	memcpy(&bcd.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_partner->attr_con.amend_attr_list, sizeof(robot_partner->attr_con.amend_attr_list));
	memcpy(&bcd.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_partner->attr_con.resis_attr_list, sizeof(robot_partner->attr_con.resis_attr_list));

	{
		this->AdjustRobotAttr(bcd.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->partner_attack_adjust);
		this->AdjustRobotAttr(bcd.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->partner_mental_adjust);
		this->AdjustRobotAttr(bcd.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->partner_recover_adjust);
		this->AdjustRobotAttr(bcd.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->partner_maxhp_adjust);
	}

	int skill_num = ARRAY_ITEM_COUNT(robot_partner->attr_con.skill_list);
	bcd.skill_num = 0;
	for (int skill_idx = 0; skill_idx < skill_num; ++skill_idx)
	{
		if (robot_partner->attr_con.skill_list[skill_idx].skill_id > 0 && robot_partner->attr_con.skill_list[skill_idx].skill_lv > 0)
		{
			bcd.skill_list[bcd.skill_num].skill_id = robot_partner->attr_con.skill_list[skill_idx].skill_id;
			bcd.skill_list[bcd.skill_num].skill_level = robot_partner->attr_con.skill_list[skill_idx].skill_lv;
			++bcd.skill_num;
		}
	}

	return true;
}

bool CloudArenaUser::FillRobotPetHelperData(PetHelperBattleData& pet_helper_data, int robot_level, const CloudArenaRobotAttrAdjustCfg* adjust_cfg)
{
	return this->FillRobotPetData(pet_helper_data.pet_data, robot_level, adjust_cfg, true);
}

bool CloudArenaUser::FillDataWithRole(Role* role)
{
	if (NULL == role) return false;

	m_uid = role->GetUID();
	m_level = role->GetLevel();
	F_STRNCPY(m_name, role->GetName(), sizeof(GameName));
	m_avatar = role->GetAvatarType();
	m_profession = role->GetProfession();
	m_appearance = role->GetRoleModuleManager()->GetAppearance();
	role->GetRoleModuleManager()->CollectRoleBattleData(&m_battle_data, true, true);
	m_capability = role->GetRoleModuleManager()->GetCapability()->GetRoleCapability();

	const CloudArenaOtherCfg& cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg();
	m_score = cfg.initial_score;
	m_season_initial_score = cfg.initial_score;
	m_remain_fighttimes = cfg.season_init_fight_times;
	m_today_buy_fighttimes_times = 0;
	m_next_recover_fighttimes_timestamp = 0;

	role->GetRoleModuleManager()->CollectRoleBattleData(&m_battle_data, true, true);
	
	m_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
	return true;
}

bool CloudArenaUser::FillDataWithUserData(const CloudArenaUserListParam::CloudArenaUserData& data)
{
	m_uid = data.uid;
	m_level = data.level;
	F_STRNCPY(m_name, data.name, sizeof(GameName));
	m_avatar = data.avatar;
	m_profession = data.profession;
	m_appearance = data.appearance;
	m_capability = data.capability;
	m_score = data.score;
	m_remain_fighttimes = data.remain_fight_times;
	memcpy(m_target_uid_list, data.target_uid_list, sizeof(m_target_uid_list));
	UNSTD_STATIC_CHECK(sizeof(m_target_uid_list) == sizeof(data.target_uid_list));
	m_today_buy_fighttimes_times = data.today_buy_fighttimes_times;
	m_next_recover_fighttimes_timestamp = data.next_recover_fighttimes_timtstamp;

	m_reach_score_timestamp = data.reach_score_timestamp;
	m_last_season_rank = data.last_season_rank;
	m_ranking_reward_flag = data.fetch_ranking_reward_flag;
	m_reach_ranking_flag = data.reach_ranking_flag;
	m_season_final_rank = data.season_final_rank;
	m_season_initial_score = data.season_initial_score;
	m_day_reward_status = data.day_reward_status;
	m_season_timestamp = data.season_timestamp;
	m_hidden_server_idx = data.hidden_server_idx;
	m_last_day_score = data.last_day_score;
	m_other_param = data.other_param;
	m_season_highest_rank = data.season_highest_rank;
	m_season_highest_dan_id = data.season_highest_dan_id;

	if (0 == m_other_param.season_highest_score_flag)
	{
		m_other_param.season_highest_score_flag = 1;
		m_other_param.season_highest_score = m_score;
	}

	PB_RoleBattleData * pb_data = (PB_RoleBattleData *)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
	if (NULL == pb_data) return false;
	
	if (!pb_data->ParseFromArray(data.battle_data_buff, data.battle_data_length))
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "CloudArenaUser::FillDataWithUserData parse protobuf failed! uid[%d]", data.uid);

#ifdef _DEBUG
		assert(0);
#endif
		return false;
	}

	m_battle_data.Reset();
	if (!m_battle_data.SetDataFromPB(*pb_data))
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "CloudArenaUser::FillDataWithUserData set from pb_data failed! uid[%d]",
			data.uid);

#ifdef _DEBUG
		assert(0);
#endif

		return false;
	}

	m_fight_record.Init(data.fight_record_head, data.fight_record_tail, data.fight_record_data_length, data.fight_record_data_buff);
	m_achievement.Init(data.achievement_param);

	m_today_challenge_list.reserve(ARRAY_ITEM_COUNT(data.today_challenge_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(data.today_challenge_list); ++i)
	{
		if (data.today_challenge_list[i] > 0)
		{
			m_today_challenge_list.push_back(data.today_challenge_list[i]);
		}
	}

	m_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
	this->OnScoreModifiedCheckRankingFlag();

	return true;
}

int CloudArenaUser::CalcCapability()
{
	int total_capability = 0;
	total_capability += this->CalcRoleCapability(m_battle_data.role_character_data);

	if (m_battle_data.fight_pet_idx >= 0 && m_battle_data.fight_pet_idx < m_battle_data.pet_num && m_battle_data.fight_pet_idx < ARRAY_LENGTH(m_battle_data.pet_character_data))
	{
		// 宠物的战力计算会有所偏差，因为主角的宠物战力计算不是直接用属性来计算的
		total_capability += this->CalcPetCapability(m_battle_data.pet_character_data[m_battle_data.fight_pet_idx].battle_data);
	}

	for (int i = 0; i < m_battle_data.partner_num && i < ARRAY_ITEM_COUNT(m_battle_data.partner_character_data); ++i)
	{
		total_capability += this->CalcPartnerCapability(m_battle_data.partner_character_data[i].battle_data);
	}

	for (int i = 0; i < m_battle_data.pet_helper_num && i < ARRAY_ITEM_COUNT(m_battle_data.pet_helper_character_data); ++i)
	{
		total_capability += this->CalcPetCapability(m_battle_data.pet_helper_character_data[i].pet_data.battle_data);
	}

	return total_capability;	
}

int CloudArenaUser::CalcRoleCapability(const BattleCharacterData& role_data)
{
	int app_prof = role_data.profession / PROFESSION_BASE;
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	const BattleAttr* attr_list = role_data.attr_list;
	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(role_data.attr_list); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;

		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += attr_list[i] * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += attr_list[i] * prof_cap_cfg->cap_base[i];
		}
	}

	int skill_capability = 0;
	const BattleCharacterData::SkillItem* skill_list = role_data.skill_list;
	for (int i = 0; i < m_battle_data.role_character_data.skill_num; ++i)
	{
		int add_skill_score = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfSkillScore(ProfSkillScoreKey(app_prof, skill_list[i].skill_level));
		if (add_skill_score > 0)
		{
			skill_capability += add_skill_score;
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + skill_capability + special_capability;
}

int CloudArenaUser::CalcPartnerCapability(const BattleCharacterData& partner_data)
{
	int app_prof = partner_data.profession; // 伙伴的prof不需要除以BASE_PROFESSION
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(partner_data.attr_list); i++)
	{
		if (prof_cap_cfg->cap_base[i] == 0) continue;
		
		if (LOGIC_CONFIG->GetScoreSystemConfig()->IsSpecialAttrType(i))
		{
			special_capability += partner_data.attr_list[i] * 1.0 * prof_cap_cfg->cap_base[i];
		}
		else
		{
			capability += partner_data.attr_list[i] * 1.0 * prof_cap_cfg->cap_base[i];
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + special_capability;
}

int CloudArenaUser::CalcPetCapability(const BattleCharacterData& pet_data)
{
	int pet_base_capability = 0;
	const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_data.character_id);
	if (NULL != pet_cfg)
	{
		pet_base_capability = pet_cfg->base_score;
	}

	double gongjixishu_1 = 1 + pet_data.attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.00081 + (pet_data.attr_list[BATTLE_ATTR_CRITICAL] + pet_data.attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE]) *
		0.000242 + pet_data.attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.0054;
	double gongjixishu_2 = 1 + (pet_data.attr_list[BATTLE_ATTR_HIT] + pet_data.attr_list[BATTLE_ATTR_DODGE]) * 0.001;
	double jingshenxishu_1 = 1 + 1.2 * pet_data.attr_list[BATTLE_ATTR_MAGIC_ATTACK] / (pet_data.attr_list[BATTLE_ATTR_MAGIC_ATTACK] + 220) + 0.8 * pet_data.attr_list[BATTLE_ATTR_MAGIC_DEFENSE] /
		(pet_data.attr_list[BATTLE_ATTR_MAGIC_DEFENSE] + 300);

	int attr_capability = (int)(pet_data.attr_list[BATTLE_ATTR_MAX_MP] * 1.6 + pet_data.attr_list[BATTLE_ATTR_MAX_HP] * 0.25 + pet_data.attr_list[BATTLE_ATTR_ATTACK] * 1.55 * gongjixishu_1 * gongjixishu_2 +
		pet_data.attr_list[BATTLE_ATTR_DEFENSE] * 1.2 + pet_data.attr_list[BATTLE_ATTR_MENTAL] * 1.95 * jingshenxishu_1 + pet_data.attr_list[BATTLE_ATTR_RECOVERY] * 1.85 +
		pet_data.attr_list[BATTLE_ATTR_AGILITY] * 2.2);

	int skill_capability = 0;
	const PetSkillAddCapabilityCfg * add_capability_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetSkillAddCapabilityCfg(pet_data.level);
	if (NULL != add_capability_cfg)
	{
		for (int i = 0; i < pet_data.skill_num; ++i)
		{
			const GamePassiveSkillCfg * passive_skill = SkillPool::Instance().GetPassiveSkillCfg(pet_data.skill_list[i].skill_id);
			if (passive_skill == NULL)
			{
				continue;
			}
			switch (passive_skill->skill_priority)
			{
			case PET_SKILL_PRIORITY_TYPE_HIGH:
				{
					skill_capability += add_capability_cfg->high_skill_add;
				}
				break;
			case PET_SKILL_PRIORITY_TYPE_SUPER:
				{
					int skill_level = pet_data.skill_list[i].skill_level;
					if (0 <= skill_level && skill_level < add_capability_cfg->super_skill_add.size())
					{
						skill_capability += add_capability_cfg->super_skill_add[skill_level];
					}
				}
				break;
			default:
				{
					skill_capability += add_capability_cfg->low_skill_add;
				}
				break;
			}
		}
	}

	return pet_base_capability + attr_capability + skill_capability;
}

void CloudArenaUser::SetOpponentList(const CloudArena::OpponentList& opponent_list)
{
	memset(m_target_uid_list, 0, sizeof(m_target_uid_list));
	for (size_t i = 0; i < CLOUD_ARENA_OPPONENT_NUM && i < static_cast<int>(opponent_list.size()); ++i)
	{
		m_target_uid_list[i] = opponent_list[i].uid;
	
#ifdef _DEBUG
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::SetOpponentList role[%d,%s] cur_score[%d]  opponent uid[%d] score[%d]", m_uid, m_name, m_score, opponent_list[i].uid, opponent_list[i].score);
#endif 
	}
}

void CloudArenaUser::GetOpponentList(int max_num, ARG_OUT int* opponent_num, ARG_OUT int* opponent_list) const
{
	if (NULL == opponent_num || NULL == opponent_list) return;

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_target_uid_list) && (*opponent_num) < max_num; ++i)
	{
		opponent_list[*opponent_num] = m_target_uid_list[i];
		(*opponent_num) += 1;
	}
}

int CloudArenaUser::GetOpponentUid(int idx) const
{
	if (idx < 0 || idx >= CLOUD_ARENA_OPPONENT_NUM) return 0;

	return m_target_uid_list[idx];
}

bool CloudArenaUser::HasOpponent() const
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_target_uid_list); ++i)
	{
		if (m_target_uid_list[i] > 0)
		{
			return true;
		}
	}

	return false;
}

void CloudArenaUser::SetLastTimeFightResult(bool is_win)
{
	if (is_win)
	{
		m_last_time_win = true;
		m_last_time_lose = false;
	}
	else
	{
		m_last_time_win = false;
		m_last_time_lose = true;
	}
}

void CloudArenaUser::ModifyScore(int score)
{
	m_score += score;

	int min_score = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().min_score;
	if (m_score < min_score)
	{
		m_score = min_score;
	}

	m_reach_score_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	
	this->OnScoreModifiedCheckRankingFlag();
}

void CloudArenaUser::SetRoleBattleData(const RoleBattleData& battle_data)
{
	m_battle_data = battle_data;

	this->OnRoleBattleDataChanged();
}

void CloudArenaUser::GetUserData(ARG_OUT CloudArenaUserListParam::CloudArenaUserData* data) const
{
	if (NULL == data) return;

	data->uid = m_uid;
	data->level = m_level;
	data->avatar = m_avatar;
	data->profession = m_profession;
	F_STRNCPY(data->name, m_name, sizeof(data->name));
	data->appearance = m_appearance;
	data->capability = m_capability;
	data->score = m_score;
	data->remain_fight_times = m_remain_fighttimes;
	memcpy(data->target_uid_list, m_target_uid_list, sizeof(data->target_uid_list));
	UNSTD_STATIC_CHECK(sizeof(data->target_uid_list) == sizeof(m_target_uid_list));
	data->today_buy_fighttimes_times = m_today_buy_fighttimes_times;
	data->next_recover_fighttimes_timtstamp = m_next_recover_fighttimes_timestamp;

	data->reach_score_timestamp = m_reach_score_timestamp;
	data->last_season_rank = m_last_season_rank;
	data->reach_ranking_flag = m_reach_ranking_flag;
	data->fetch_ranking_reward_flag = m_ranking_reward_flag;
	data->season_final_rank = m_season_final_rank;
	data->season_initial_score = m_season_initial_score;
	data->day_reward_status = m_day_reward_status;
	data->season_timestamp = m_season_timestamp;
	data->hidden_server_idx = m_hidden_server_idx;
	data->season_highest_rank = m_season_highest_rank;
	data->season_highest_dan_id = m_season_highest_dan_id;
	data->last_day_score = m_last_day_score;
	data->other_param = m_other_param;

	memset(data->battle_data_buff, 0, sizeof(data->battle_data_buff));
	data->battle_data_length = 0;

	{
		PB_RoleBattleData* pb_data = (PB_RoleBattleData*)ProtobufMgr::Instance()->GetProtoMessage(PROTO_ROLE_BATTLE_DATA);
		if (NULL == pb_data)
		{
			return;
		}

		m_battle_data.GetPBData(pb_data);
		if (!pb_data->SerializeToArray(data->battle_data_buff, sizeof(data->battle_data_buff)))
		{
#ifdef _DEBUG
			assert(0);
#endif
			return;
		}
		data->battle_data_length = pb_data->GetCachedSize();
	}

	m_fight_record.GetRecordData(&data->fight_record_head, &data->fight_record_tail, &data->fight_record_data_length, data->fight_record_data_buff, sizeof(data->fight_record_data_buff));
	m_achievement.GetInitParam(&data->achievement_param);

	int num = 0;
	memset(data->today_challenge_list, 0, sizeof(data->today_challenge_list));
	for (size_t i = 0; i < m_today_challenge_list.size() && num < ARRAY_ITEM_COUNT(data->today_challenge_list); ++i)
	{
		if (m_today_challenge_list[i] == 0) continue;

		data->today_challenge_list[num++] = m_today_challenge_list[i];
	}
}

bool CloudArenaUser::AddFightTimes(int times)
{
	int max_times = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().max_store_fight_times;
	if (m_remain_fighttimes + times <= max_times)
	{
		m_remain_fighttimes += times;
		return true;
	}

	return false;
}

bool CloudArenaUser::UseFightTimes(int times)
{
	if (m_remain_fighttimes >= times)
	{
		m_remain_fighttimes -= times;
		
		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::UseFightTimes user[%d,%s] \
			use_times[%d] remain_times[%d]", m_uid, m_name, times, m_remain_fighttimes);
		return true;
	}

	gamelog::g_log_cloud_arena.printf(LL_WARNING, "CloudArenaUser::UseFightTimes Times Not Enough ERROR! user[%d,%s] \
			use_times[%d] remain_times[%d]", m_uid, m_name, times, m_remain_fighttimes);
	return false;
}

void CloudArenaUser::OnBuyFightTimes()
{
	this->AddFightTimes(1);

	m_today_buy_fighttimes_times += 1;

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::OnBuyFightTimes user[%d,%s] remain_times[%d] today_buy_times[%d]",
		m_uid, m_name, m_remain_fighttimes, m_today_buy_fighttimes_times);
}

void CloudArenaUser::OnStartNewSeason()
{
	int final_score = m_score;
	int final_rank = m_season_final_rank;

	m_score = this->CalculateSeasonInitialScore(m_score);
	m_season_initial_score = m_score;
	m_remain_fighttimes = LOGIC_CONFIG->GetCloudArenaConfig()->GetOtherCfg().season_init_fight_times;
	m_last_season_rank = m_season_final_rank;
	m_season_final_rank = -1;
	m_fight_record.ClearAllRecord();
	m_other_param.last_season_final_score = final_score;
	m_other_param.new_season_first_time_update_last_day_score = 1;
	m_reach_ranking_flag = 0;
	m_ranking_reward_flag = 0;
	m_season_highest_rank = -1;
	m_season_highest_dan_id = 0;
	m_other_param.season_highest_score = m_season_initial_score;
	this->OnScoreModifiedCheckRankingFlag();

	m_season_timestamp = CloudArena::Instance().GetSeasonTimestamp();
	m_hidden_server_idx = CloudArena::Instance().GetHiddenServerIdx();
	m_achievement.OnNewSeasonStart();
	CloudArena::Instance().SendAchievementInfo(m_uid, this);

	CloudArena::Instance().OnUserScoreChangedUpdateScoreHash(final_score, this);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::OnStartNewSeason user[%d,%s] final_score[%d] final_rank[%d] new_season_init_score[%d]",
		m_uid, m_name, final_score, final_rank, m_season_initial_score);

	this->TryUpdateHighestDanAndRank(false);

	this->SyncCommonDataToRole();
}

void CloudArenaUser::OnFetchAchievementReward(int achieve_type)
{
	if (achieve_type < 0 || achieve_type >= CLOUD_ARENA_ACHIEVEMENT_MAX) return;

	m_achievement.OnFetchAchievementReward(achieve_type);
}

void CloudArenaUser::OnManualRefreshOpponent()
{
	m_other_param.manual_refresh_times += 1;
}

void CloudArenaUser::ResetManualRefreshTimes()
{
	m_other_param.manual_refresh_times = 0;
}

void CloudArenaUser::GetFightRecordUserInfo(ARG_OUT CloudArenaFightRecordUserInfo* info) const
{
	info->uid = m_uid;
	info->level = m_level;
	info->avatar = m_avatar;
	info->profession = m_profession;
	info->headshot_id = m_appearance.headshot_id;
	F_STRNCPY(info->name, m_name, sizeof(info->name));
}

void CloudArenaUser::AddFightRecord(const CloudArenaFightRecord& record)
{
	m_fight_record.AddFightRecord(record);
}

void CloudArenaUser::GetFightRecord(int offset, int count, ARG_OUT CloudArenaFightRecord* record_list, ARG_OUT int* record_num, int max_num) const
{
	m_fight_record.GetFightRecord(offset, count, record_list, record_num, max_num);
}

void CloudArenaUser::OnNoticeSeasonInitialScore()
{
	gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::OnNoticeSeasonInitialScore role[%d,%s] season_initial_score[%d]",
		m_uid, m_name, m_season_initial_score);

	m_season_initial_score = 0;
}

void CloudArenaUser::OnRankUpdate(int cur_rank)
{
	if (!this->IsRobot())
	{
		m_achievement.OnRankUpdate(cur_rank);
	}
}

void CloudArenaUser::OnFightFinishUpdateAchievement(bool is_challenge_other, bool is_win, bool is_higher_dan, int total_damage, int round_num, int use_medicine_num, int realive_num)
{
	if (!this->IsRobot())
	{
		m_achievement.OnFightFinish(is_challenge_other, is_win, is_higher_dan, total_damage, round_num, use_medicine_num, realive_num);
	}
}

const CloudArenaAchievementItem* CloudArenaUser::GetAchievementItem(int achieve_type) const
{
	return m_achievement.GetAchievementItem(achieve_type);
}

void CloudArenaUser::OnFetchDayReward()
{
	m_day_reward_status = CLOUD_ARENA_DAY_REWARD_ALREADY_CALIM;
}

void CloudArenaUser::AddTodayChallengeList(int target_uid)
{
	m_today_challenge_list.push_back(target_uid);
}

void CloudArenaUser::AdjustRobotAttr(BattleAttr& attr, int adjust_percent)
{
	if (0 == adjust_percent) return;

	int add_attr = (int)(1.0 * attr * adjust_percent / CLOUD_ARENA_ROBOT_ATTR_ADJUST_PERCENT);
	attr += add_attr;

	GetMax(attr, 1);
}

void CloudArenaUser::OnScoreModifiedCheckRankingFlag()
{
	const CloudArenaRankCfg* rank_cfg = LOGIC_CONFIG->GetCloudArenaConfig()->GetRankCfg(m_score);
	if (NULL == rank_cfg) return;

	int mask = (1 << (rank_cfg->rank_id + 1)) - 1;
	m_reach_ranking_flag |= mask;
}

int CloudArenaUser::CalculateSeasonInitialScore(int last_season_score)
{
	return LOGIC_CONFIG->GetCloudArenaConfig()->GetSeasonReduceToScore(last_season_score);
}

void CloudArenaUser::OnRoleBattleDataChanged()
{
	m_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
}

void CloudArenaUser::SyncCommonDataToRole()
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_uid));
	if (NULL != role)
	{
		role->GetRoleModuleManager()->UpdateCloudArenaData(CloudArena::Instance().GetSeasonTimestamp(), m_last_season_rank, m_season_highest_dan_id, m_season_highest_rank, m_other_param.season_highest_score);
	}
	else if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameSyncCloudArenaUserDataResp resp;
		resp.uid = m_uid;
		resp.season_timestamp = CloudArena::Instance().GetSeasonTimestamp();
		resp.last_season_rank = m_last_season_rank;
		resp.season_highest_dan_id = m_season_highest_dan_id;
		resp.season_highest_rank = m_season_highest_rank;
		resp.season_highest_score = m_other_param.season_highest_score;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&resp, sizeof(resp));
	}
}

bool CloudArenaUser::CheckSeasonTimestamp()
{
	if (m_season_timestamp == CloudArena::Instance().GetSeasonTimestamp()) return false;
	
	if (m_hidden_server_idx != CloudArena::Instance().GetHiddenServerIdx())
	{
		unsigned int old_season_timestamp = m_season_timestamp;
		int old_hidden_server_idx = m_hidden_server_idx;

		m_season_timestamp = CloudArena::Instance().GetSeasonTimestamp();
		m_hidden_server_idx = CloudArena::Instance().GetHiddenServerIdx();

		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::CheckSeasonTimestamp HiddenServerIdx different, Just Sync! \
				user[%d,%s] old_season[%u] new_season[%u] old_hidden_idx[%d] new_hidden_idx[%d]",
			m_uid, m_name, old_season_timestamp, m_season_timestamp, old_hidden_server_idx, m_hidden_server_idx);
	}
	else
	{
		unsigned int old_season_timestamp = m_season_timestamp;
		int old_hidden_server_idx = m_hidden_server_idx;

		m_season_final_rank = m_last_season_rank; // 调用OnStartNewSeason后 将对m_last_season_rank赋值
		this->OnStartNewSeason();

		gamelog::g_log_cloud_arena.printf(LL_INFO, "CloudArenaUser::CheckSeasonTimestamp Timestamp Not Match, Force Start New Season! \
				user[%d,%s] old_season[%u] new_season[%u] old_hidden_idx[%d] new_hidden_idx[%d]",
			m_uid, m_name, old_season_timestamp, m_season_timestamp, old_hidden_server_idx, m_hidden_server_idx);
	}

	return true;
}

void CloudArenaUser::TryUpdateHighestDanAndRank(bool is_sync_to_game /* = true */)
{
	if (this->IsRobot()) return;

	bool has_change = false;
	int cur_dan_id = LOGIC_CONFIG->GetCloudArenaConfig()->GetDanIdByScore(m_score);
	if (cur_dan_id > m_season_highest_dan_id)
	{
		m_season_highest_dan_id = cur_dan_id;
		has_change = true;
	}

	int rank = CloudArena::Instance().GetUserRank(m_uid);
	if (-1 == m_season_highest_rank || rank < m_season_highest_rank)
	{
		m_season_highest_rank = rank;
		has_change = true;
	}

	if (m_score > m_other_param.season_highest_score)
	{
		m_other_param.season_highest_score = m_score;
		has_change = true;
	}

	if (has_change && is_sync_to_game)
	{
		this->SyncCommonDataToRole();
	}
}

