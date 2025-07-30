#include "worldarenauser.hpp"
#include "servercommon/robotdef.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/worldarena/worldarenaconfig.hpp"
#include "gamelog.h"
#include "protocol/msgworldarena.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/robot/robotconfig.hpp"
#include "skill/skillpool.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "worldarena.hpp"

WorldArenaUser::WorldArenaUser(bool old_state) : m_user_data(old_state)
{

}

WorldArenaUser::~WorldArenaUser()
{

}

void WorldArenaUser::OnDayChange()
{
	this->ClearTodayGiveNotice();
	this->ResetDailyRewardFetchFlag();

	if (this->IsRobot())
	{
		long long group_id = this->GetGroupID();
		int average_lv = WorldArena::Instance().CalculateAverageLevelInGroup(group_id);
		if (this->GetLevel() != average_lv)
		{
			this->UpdateRobotLevel(average_lv);
		}
	}
}

long long WorldArenaUser::GetUUID() const
{
	return m_user_data.GetUserData().signup_info.uuid;
}

int WorldArenaUser::GetUID() const
{
	return m_user_data.GetUserData().signup_info.uid;
}

const char* WorldArenaUser::GetName() const
{
	return m_user_data.GetUserData().signup_info.role_name;
}

void WorldArenaUser::SetSignUpInfo(const WorldArenaSignUpInfo& info)
{
	m_user_data.MutableUserData().signup_info = info;
}

void WorldArenaUser::SetSeasonIndex(short season_index)
{
	m_user_data.MutableUserData().signup_info.season_index = season_index;
}

void WorldArenaUser::SetRoleBattleData(const RoleBattleData& rbd)
{
	m_user_data.MutableUserData().battle_data = rbd;
}

void WorldArenaUser::SetGroupID(long long group_id)
{
	m_user_data.MutableUserData().schedule_info.group_id = group_id;
}

long long WorldArenaUser::GetGroupID() const
{
	return m_user_data.GetUserData().schedule_info.group_id;
}

void WorldArenaUser::SetScheduleID(int round_idx, long long schedule_id)
{
	assert(round_idx >= 0 && round_idx < WORLD_ARENA_ROUND_NUM);

	if (round_idx < 0 || round_idx >= WORLD_ARENA_ROUND_NUM) return;

	m_user_data.MutableUserData().schedule_info.schedule_list[round_idx] = schedule_id;
}

long long WorldArenaUser::GetScheduleID(int round_idx)
{
	if (round_idx < 0 || round_idx >= WORLD_ARENA_ROUND_NUM) return 0;

	return m_user_data.GetUserData().schedule_info.schedule_list[round_idx];
}

bool WorldArenaUser::IsRobot() const
{
	return 0 == LongLongToUniqueUserID(m_user_data.GetUserData().signup_info.uuid).plat_type;
}

const RoleBattleData& WorldArenaUser::GetRoleBattleData()
{
	return m_user_data.GetUserData().battle_data;
}

void WorldArenaUser::AssembleRobotParam(RobotParam& param, int level)
{
	const WorldArenaUserInfo& info = m_user_data.GetUserData();
	param.avatar_type = info.signup_info.avatar_type;
	param.bound_battle_mode = 0;
	param.bound_team_type = 0;
	param.profession = info.signup_info.profession;
	param.level = level;
	param.plat_type = info.signup_info.plat_type;
	param.robot_id = info.signup_info.uid;
	F_STRNCPY(param.robot_name, info.signup_info.role_name, sizeof(param.robot_name));
	param.robot_type = ROBOT_TYPE_NORMAL;
	param.robot_ai_param.Reset();
	param.role_battle_data = info.battle_data;

	const WorldArenaRobotCfg* robot_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetRobotCfgByProfLevel(GetBaseProf(param.profession), level);
	if (NULL == robot_cfg)
	{
		assert(0);
		return;
	}

	RoleBattleData & battle_data = param.role_battle_data;
	battle_data.is_auto = 1;
	battle_data.role_stand_row = BATTLE_ROW_TYPE_BACK;

	const RobotRoleCfg* robot_role = ROBOTCONFIG->RandRobotByLevel(level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_role)
	{
		assert(0);
		return;
	}

	const WorldArenaRobotAttrAdjustCfg* adjust_cfg = LOGIC_CONFIG->GetWorldArenaConfig()->GetRobotAttrAdjustCfg(robot_cfg->attr_adjust_group);
	if (NULL == adjust_cfg)
	{
		assert(0);
		return;
	}

	BattleCharacterData & role_data = battle_data.role_character_data;

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
	if (!this->FillRobotPetData(pet_data, level, adjust_cfg, false))
	{
		assert(0);
		return ;
	}
	battle_data.pet_num = 1;

	// 伙伴数据
	int partner_list[ROW_CHARACTER_NUM - 1] = { 0 };
	int partner_num = 0;
	battle_data.partner_num = 0;
	ROBOTCONFIG->RandRobotPartnerList(level, ROW_CHARACTER_NUM - 1, &partner_num, partner_list);
	for (int i = 0; i < partner_num && i < ARRAY_LENGTH(BATTLE_POS_RELATION) - 1; ++i)
	{
		int partner_id = partner_list[i];
		PartnerBattleData& partner_data = battle_data.partner_character_data[battle_data.partner_num];
		if (!this->FillRobotPartnerData(partner_data, level, partner_id, adjust_cfg))
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
		const RobotPetCfg* robot_pet = ROBOTCONFIG->RandRobotPetByLevel(level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
		if (NULL == robot_pet) continue;

		PetHelperBattleData& pet_helper_data = battle_data.pet_helper_character_data[battle_data.pet_helper_num];
		if (!this->FillRobotPetHelperData(pet_helper_data, level, adjust_cfg))
		{
			continue;
		}
		pet_helper_data.battle_row = BATTLE_ROW_TYPE_FRONT;
		pet_helper_data.battle_column = BATTLE_POS_RELATION[i + 1];
		battle_data.pet_helper_num += 1;
	}
}

void WorldArenaUser::GetScheduleList(ARG_OUT long long* schedule_list, int max_num)
{
	int num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_user_data.GetUserData().schedule_info.schedule_list) && num < max_num; ++i)
	{
		schedule_list[num++] = m_user_data.GetUserData().schedule_info.schedule_list[i];
	}
}

int WorldArenaUser::GetAvatarType() const
{
	return m_user_data.GetUserData().signup_info.avatar_type;
}

const char* WorldArenaUser::GetGuildName() const
{
	return m_user_data.GetUserData().signup_info.guild_name;
}

int WorldArenaUser::GetHeadshotId() const
{
	return m_user_data.GetUserData().signup_info.headshot_id;
}

int WorldArenaUser::GetLevel() const
{
	return m_user_data.GetUserData().signup_info.level;
}

int WorldArenaUser::GetPlatType() const
{
	return m_user_data.GetUserData().signup_info.plat_type;
}

int WorldArenaUser::GetProfession() const
{
	return m_user_data.GetUserData().signup_info.profession;
}

short WorldArenaUser::GetSeasonIndex() const
{
	return m_user_data.GetUserData().signup_info.season_index;
}

const Appearance& WorldArenaUser::GetAppearance() const
{
	return m_user_data.GetUserData().battle_data.appearance;
}

const WorldArenaSignUpInfo* WorldArenaUser::GetSignUpInfoPtr() const
{
	return &m_user_data.GetUserData().signup_info;
}

const WorldArenaSignUpInfo& WorldArenaUser::GetSignUpInfoRef() const
{
	return m_user_data.GetUserData().signup_info;
}

bool WorldArenaUser::IsDirty() const
{
	return m_user_data.IsDirty();
}

void WorldArenaUser::ClearDirtyMark()
{
	m_user_data.SetOldState(true);

	if (m_user_data.GetLastSaveTimestamp_MS() > m_user_data.GetDirtyTimestamp_MS())
	{
		m_user_data.SetDirty(false);
		m_user_data.SetLastSaveTimestamp_MS(0ULL);
	}
}

void WorldArenaUser::SetLastSaveTimestampMS(unsigned long long timestamp_ms)
{
	m_user_data.SetLastSaveTimestamp_MS(timestamp_ms);
}

void WorldArenaUser::SetUserInfoForInit(const WorldArenaUserInfo& info)
{
	m_user_data.MutableUserData().signup_info = info.signup_info;
	m_user_data.MutableUserData().schedule_info = info.schedule_info;
	m_user_data.MutableUserData().score_info = info.score_info;
	m_user_data.MutableUserData().battle_data = info.battle_data;

	m_user_data.SetDirty(false);
}

int WorldArenaUser::GetScore() const
{
	return m_user_data.GetUserData().score_info.total_score;
}

int WorldArenaUser::GetWinNum() const
{
	return m_user_data.GetUserData().score_info.win_num;
}

int WorldArenaUser::GetLoseNum() const
{
	return m_user_data.GetUserData().score_info.lose_num;
}

int WorldArenaUser::GetDrawNum() const
{
	return m_user_data.GetUserData().score_info.draw_num;
}

int WorldArenaUser::GetTotalFightNum() const
{
	const WorldArenaScoreInfo& score_info = m_user_data.GetUserData().score_info;
	return score_info.win_num + score_info.lose_num + score_info.draw_num + score_info.giveup_num;
}

int WorldArenaUser::GetTotalBattleRound() const
{
	return m_user_data.GetUserData().score_info.total_battle_round;
}

bool WorldArenaUser::HasFetchSignupReward() const
{
	return m_user_data.GetUserData().score_info.has_fetch_signup_reward > 0;
}

void WorldArenaUser::OnFetchSignupReward()
{
	m_user_data.MutableUserData().score_info.has_fetch_signup_reward = 1;
}

bool WorldArenaUser::CanFetchDailyReward() const
{
	return WORLD_ARENA_DAILY_REWARD_FLAG_FETCHABLE == m_user_data.GetUserData().score_info.daily_reward_fetch_flag;
}

void WorldArenaUser::ResetDailyRewardFetchFlag()
{
	m_user_data.MutableUserData().score_info.daily_reward_fetch_flag = WORLD_ARENA_DAILY_REWARD_FLAG_UNFETCHABLE;
}

void WorldArenaUser::OnFetchDailyRerward()
{
	m_user_data.MutableUserData().score_info.daily_reward_fetch_flag = WORLD_ARENA_DAILY_REWARD_FLAG_ALREADY_FETCHED;
}

char WorldArenaUser::GetFetchDailyRewardFlag() const
{
	return m_user_data.GetUserData().score_info.daily_reward_fetch_flag;
}

void WorldArenaUser::SetDailyRewardAvaliable()
{
	m_user_data.MutableUserData().score_info.daily_reward_fetch_flag = WORLD_ARENA_DAILY_REWARD_FLAG_FETCHABLE;
}

int WorldArenaUser::GetWinBoxRewardFlag()
{
	return m_user_data.GetUserData().score_info.win_box_reward_flag;
}

void WorldArenaUser::OnFetchWinBox(int sn)
{
	m_user_data.MutableUserData().score_info.win_box_reward_flag |= (1 << sn);
}

void WorldArenaUser::OnSetFightResult(bool is_win, bool is_draw, bool is_lose, bool is_giveup, int round_num)
{
	if (is_win)
	{
		m_user_data.MutableUserData().score_info.win_num += 1;
		m_user_data.MutableUserData().score_info.total_score += LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().win_score;
	}
	else if (is_draw)
	{
		m_user_data.MutableUserData().score_info.draw_num += 1;
		m_user_data.MutableUserData().score_info.total_score += LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().draw_score;
	}
	else if (is_lose)
	{
		m_user_data.MutableUserData().score_info.lose_num += 1;
		m_user_data.MutableUserData().score_info.total_score += LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().lose_score;
	}
	else if (is_giveup)
	{
		m_user_data.MutableUserData().score_info.giveup_num += 1;
		m_user_data.MutableUserData().score_info.total_score += LOGIC_CONFIG->GetWorldArenaConfig()->GetOtherCfg().give_up_score;
		m_user_data.MutableUserData().score_info.today_giveup_notice = 1;
		this->SendGiveupNoticeMail();
	}

	m_user_data.MutableUserData().score_info.total_battle_round += round_num;

	const WorldArenaUserInfo& user_info = m_user_data.GetUserData();
	gamelog::g_log_world_arena.printf(LL_INFO, "%s %s[%d,%s] win_num[%d] draw_num[%d] lose_num[%d] giveup_num[%d] score[%d] total_round[%d]",
		__FUNCTION__, this->IsRobot() ? "robot" : "role", user_info.signup_info.uid, user_info.signup_info.role_name, user_info.score_info.win_num,
		user_info.score_info.draw_num, user_info.score_info.lose_num, user_info.score_info.giveup_num, user_info.score_info.total_score,
		user_info.score_info.total_battle_round);
}

void WorldArenaUser::AssembleMsgRankListUserInfo(ARG_OUT Protocol::MsgRankListUserInfo* info)
{
	if (NULL == info) return;

	info->plat_type = this->GetPlatType();
	info->uid = this->GetUID();
	F_STRNCPY(info->name, this->GetName(), sizeof(info->name));
	info->profession = this->GetProfession();
	info->score = this->GetScore();
	info->win_num = this->GetWinNum();
	info->lose_num = this->GetLoseNum();
	info->draw_num = this->GetDrawNum();
	info->total_num = this->GetTotalFightNum();
}

bool WorldArenaUser::HasTodayGiveupNotice() const
{
	return m_user_data.GetUserData().score_info.today_giveup_notice > 0;
}

void WorldArenaUser::ClearTodayGiveNotice()
{
	m_user_data.MutableUserData().score_info.today_giveup_notice = 0;
}

void WorldArenaUser::UpdateUserSignupInfo(const WorldArenaSignUpInfo& info)
{
	WorldArenaUserInfo& mutable_info = m_user_data.MutableUserData();
	mutable_info.signup_info.avatar_type = info.avatar_type;
	mutable_info.signup_info.headshot_id = info.headshot_id;
	mutable_info.signup_info.level = info.level;
	mutable_info.signup_info.profession = info.profession;
	mutable_info.signup_info.top_level = info.top_level;
	F_STRNCPY(mutable_info.signup_info.guild_name, info.guild_name, sizeof(GuildName));
	F_STRNCPY(mutable_info.signup_info.role_name, info.role_name, sizeof(GameName));
}

void WorldArenaUser::UpdateRobotLevel(int level)
{
	m_user_data.MutableUserData().signup_info.level = level;
}

void WorldArenaUser::SendGiveupNoticeMail()
{
	const tm* localtime = EngineAdapter::Instance().LocalTime();
	assert(NULL != localtime);

	static MailContentParam contentparam;
	contentparam.Reset();
	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_arena_fight_result_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_arena_not_show_up, localtime->tm_mon + 1, localtime->tm_mday);
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(IntToUid(this->GetUID()), MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM);
	}
}

void WorldArenaUser::AdjustRobotAttr(BattleAttr& attr, int adjust_percent)
{
	if (0 == adjust_percent) return;

	int add_attr = (int)(1.0 * attr * adjust_percent / WORLD_ARENA_ROBOT_ATTR_ADJUST_PERCENT);
	attr += add_attr;

	GetMax(attr, 1);
}

bool WorldArenaUser::FillRobotPetData(PetBattleData& data, int robot_level, const WorldArenaRobotAttrAdjustCfg* adjust_cfg, bool is_pet_helper)
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
	pet_data.level = robot_level;
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

bool WorldArenaUser::FillRobotPartnerData(PartnerBattleData& partner_data, int robot_level, int partner_id, const WorldArenaRobotAttrAdjustCfg* adjust_cfg)
{
	const RobotPartnerCfg* robot_partner = ROBOTCONFIG->GetPartner(partner_id, robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_partner) return false;

	const PartnerBasicCfg* partner_basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == partner_basic_cfg) return false;

	BattleCharacterData& bcd = partner_data.battle_data;
	bcd.Reset();
	bcd.level = robot_level;
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

bool WorldArenaUser::FillRobotPetHelperData(PetHelperBattleData& pet_helper_data, int robot_level, const WorldArenaRobotAttrAdjustCfg* adjust_cfg)
{
	return this->FillRobotPetData(pet_helper_data.pet_data, robot_level, adjust_cfg, true);
}

