#include "gladiator.hpp"
#include "world.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "global/usercache/usercache.hpp"
#include "protocol/msgcolosseum.h"
#include "scene/scenemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/colosseumconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/title/title.hpp"
#include "item/itempool.h"
#include "item/itembase.h"

#include "colosseummanager.hpp"
#include "gamelog.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/robot/robotconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "config/otherconfig/scoresystemconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/formation/pethelperformation.hpp"
#include "other/rolebattledatacapabilitycalculator.hpp"

Gladiator::Gladiator() : colosseum_clear_check(0), m_uid(0), m_level(0), m_show_level(0), m_profession(0), 
	m_avatar_type(0), m_today_fight_count(0), m_fetch_exp_flag(0), 
	m_worship_count(0), last_refresh_target_time(0), m_highest_rank(-1), 
	m_rank_award_fetch_flag(0), m_is_in_pool(false), m_pos(0), m_total_capability(0)
{
	memset(m_name, 0, sizeof(m_name));
	memset(m_target_list, -1, sizeof(m_target_list));
	colosseum_param.Reset();
}

Gladiator::~Gladiator()
{
}

/*
* \description 克隆一个Gladiator
* \note 当成员变量中存在指针时，要考虑加入深拷贝
*/
Gladiator* Gladiator::Clone()
{
	Gladiator* gladiator = new Gladiator();
	*gladiator = *this;  // 成员

	return gladiator;
}

void Gladiator::UserLogin(Role * role)
{
	if (m_target_list[0] < 0)
	{
		ColosseumManager::Instance().OnRefreshTarget(m_uid);
	}

	//检查清空标记

	if (colosseum_clear_check != 0)
	{
		this->ClearCheck();
	}
	if (NULL != role && !CrossConfig::Instance().IsHiddenServer())	//本服才设置
	{
		role->GetRoleModuleManager()->OnSetColosseumInfo(m_pos + 1);
	}

	this->SendEveryDayRankRewardInfo();
}

void Gladiator::SyncRoleData(Role * role)
{
	if (NULL == role) return;

	if (!role->GetRoleModuleManager()->GetPetHelperFormation()->CollectBattleData(BATTLE_MODE_SINGLE_ARENA, m_battle_data, false))
	{
		role->GetRoleModuleManager()->CollectRoleBattleData(&m_battle_data, true, true);
	}
	
	m_uid = role->GetUID();
	m_level = role->GetLevel();
	m_profession = role->GetProfession();
	m_avatar_type = role->GetAvatarType();
	m_appearance = role->GetRoleModuleManager()->GetAppearance();
	role->GetName(m_name);

	AssignShowLevel(this, m_pos);

	m_total_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
}

void Gladiator::SyncRoleBattleData(const RoleBattleData& role_battle_data)
{
	m_battle_data = role_battle_data;
	m_total_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
}

int Gladiator::GetTargetRank(int index) const
{
	if (index < 0 || index >= CHALLENGE_FIELD_OPPONENT_MAX) return -1;

	return m_target_list[index];
}

void Gladiator::SysReport(Gladiator* gladiator, bool is_win, bool is_gladiator_attacker)
{
	if (NULL != gladiator)
	{
		Report report;
		report.is_win = is_win;
		report.rank = gladiator->GetPos();
		report.is_attacker = is_gladiator_attacker ? 1 : 0;

		report.profession = gladiator->m_profession;
		report.level = gladiator->m_show_level;
		report.avatar_type = gladiator->m_avatar_type;
		report.headshot_id = gladiator->m_appearance.headshot_id;
		report.reserve_sh = 0;
		memcpy(report.name, gladiator->m_name, sizeof(GameName));
		m_report.push_back(report);
		if (static_cast<int>(m_report.size()) > COLOSSEUM_REPORT_NUM)
		{
			m_report.pop_front();
		}

		if (!this->IsRobot())
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(this->GetUserID());
			if (NULL != role)
			{
				Protocol::SCColosseumNewReportDetail notice;
				notice.report = report;
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
			}
			else
			{
				OfflineRegisterManager::Instance().OnColosseumNewReport(this->GetUserID());
			}
		}
	}
}

void Gladiator::SendEveryDayRankRewardInfo()
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(this->GetUserID());
	if (NULL != role)
	{
		CommonDataParam& com_data = role->GetRoleModuleManager()->GetCommonData();
		static Protocol::SCColosseumEveryDayRewardInfo info;
		info.count = 0;
		for (int i = 0; i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST; i++)
		{
			info.rank_pos_list[i] = com_data.gladiator_everyday_reward_list[i];
			info.count++;
		}

		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(m_uid), &info, sizeof(info));
	}
}

void Gladiator::OnResetColosseunmModuleData( bool is_in_pool)
{
	this->SendEveryDayRankReward();

	m_today_fight_count = 0;
	m_fetch_exp_flag = 0;

	gamelog::g_log_colosseum.printf(LL_INFO, "Gladiator::OnResetColosseunmModuleData role[%d,%s] pos[%d]", m_uid, m_name, m_pos + 1);
}

void Gladiator::SendEveryDayRankReward()
{
	if (!this->IsRobot())
	{
		int rank = this->GetPos() + 1;
		//记录玩家今天排名并且替换旧的奖励历史或增加
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(this->GetUserID());
		if (NULL == role)
		{
			OfflineRegisterManager::Instance().OnGladiatorGetEveryDayReward(this->GetUserID(), rank);
		}
		else
		{
			CommonDataParam& com_data = role->GetRoleModuleManager()->GetCommonData();
			/*if (com_data.gladiator_everyday_reward_count >= OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST)
			{
				//替换顶去最旧的，把新的排到最末尾
				for (int j = 0; j < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1; j++)
				{
					com_data.gladiator_everyday_reward_list[j] = com_data.gladiator_everyday_reward_list[j + 1];
				}
				com_data.gladiator_everyday_reward_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1] = rank;
			}
			else
			{
				com_data.gladiator_everyday_reward_list[com_data.gladiator_everyday_reward_count++] = rank;
			}*/
			com_data.gladiator_everyday_reward_list[0] = rank;
			com_data.gladiator_everyday_reward_count = 1;
			this->SendEveryDayRankRewardInfo();
		}
		/*const ColosseumAwardCfg * award_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRandRewardCfg(rank);
		if (award_cfg != NULL)
		{
			MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = award_cfg->award_coin;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS] = award_cfg->colosseum_credits;

			int item_count = 0;
			for (int i = 0; i < (int)award_cfg->award_list.size() && item_count < ARRAY_LENGTH(contentparam.item_list); ++i)
			{
				ItemConfigData reward = award_cfg->award_list[i];
				const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
				if (item_base != NULL)
				{
					contentparam.item_list[item_count].item_id = reward.item_id;
					contentparam.item_list[item_count].num = reward.num;
					contentparam.item_list[item_count].is_bind = (reward.is_bind) ? 1 : 0;
					contentparam.item_list[item_count].invalid_time = item_base->CalInvalidTime();
				}
			}

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_colosseum_rank_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_colosseum_rank_reward_content, rank);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser(this->GetUid(), MAIL_REASON_DEFAULT, contentparam);
			}
		}*/
	}
}

void Gladiator::OnSeasonChange()
{
	if (this->IsRobot()) return;

	m_rank_award_fetch_flag = 0;
	m_highest_rank = -1;
	memset(m_target_list, -1, sizeof(m_target_list));
}

void Gladiator::OnAddInPool()
{

}

void Gladiator::SetPos(int pos, bool is_from_battle /* = false */)
{
	if (pos != m_pos)
	{
		m_pos = pos;

		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(m_uid);
		if (role != NULL)
		{
			role->GetRoleModuleManager()->GetTitle()->CalColosseumTitle();
			if (!CrossConfig::Instance().IsHiddenServer())
			{
				role->GetRoleModuleManager()->OnSetColosseumInfo(m_pos + 1);
			}
		}
		else
		{
			if (!CrossConfig::Instance().IsHiddenServer() && is_from_battle)
			{
				crossgameprotocal::GameHiddenColosseumRankInfoSyn ghcris;
				ghcris.role_id = m_uid;
				ghcris.colosseum_rank = m_pos + 1;

				InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghcris, sizeof(ghcris));
			}
		}

		if (this->IsRobot())
		{
			this->AdjustRobotAttr();
			m_total_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
		}
		else if (0 == m_total_capability)
		{
			m_total_capability = RoleBattleDataCapabilityCalculator::CalcCapability(m_battle_data);
		}
	}
}

void Gladiator::AssignShowLevel(Gladiator* gladiator, int rank_pos)
{
	if (NULL == gladiator) return;

	if (!gladiator->IsRobot())
	{
		gladiator->m_show_level = gladiator->m_level;
	}
	else
	{
		int open_server_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
		gladiator->m_show_level = LOGIC_CONFIG->GetColosseumConfig()->GetRobotShowLevelByRankAndOpenServerDays(rank_pos + 1, open_server_days);
	}
}

void AdjustAttr(BattleAttr& attr, int adjust_percent)
{
	if (0 == adjust_percent) return;

	int add_attr = (int)(1.0 * attr * adjust_percent / COLOSSEUM_ROBOT_ATTR_ADJUST_PERCENT);
	attr += add_attr;

	GetMax(attr, 1);
}

void Gladiator::AdjustRobotAttr()
{
	if (!this->IsRobot()) return;

	int server_open_days = World::GetInstWorld()->GetOpenServerDays_0(LocalConfig::Instance().GetUniqueServerID());
	const ColosseumRobotLevelCfg* level_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRobotLevelCfgByRankAndOpenServerDays(m_pos + 1, server_open_days);
	if (NULL == level_cfg) return;

	int robot_level = RandomNum(level_cfg->level_min, level_cfg->level_max + 1);
	int show_level = level_cfg->show_level;

	int adjust_group_id = level_cfg->attr_adjust_group;
	const ColosseumRobotAttrAdjustCfg* adjust_cfg = LOGIC_CONFIG->GetColosseumConfig()->GetRobotAttrAdjustCfg(adjust_group_id);
	if (NULL == adjust_cfg) return;

	const RobotRoleCfg* robot_role = ROBOTCONFIG->RandRobotByLevelAndProf(robot_level, GetBaseProf(m_profession), ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_role) return;

	BattleCharacterData & role_data = m_battle_data.role_character_data;
	role_data.level = show_level;

	// 更新主角属性
	const RobotAttrCon & robot_attr = robot_role->attr_con;
	role_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_attr.max_hp;
	role_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_attr.max_mp;
	memcpy(role_data.attr_list, robot_attr.basic_attr_list, sizeof(robot_attr.basic_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_attr.amend_attr_list, sizeof(robot_attr.amend_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_attr.resis_attr_list, sizeof(robot_attr.resis_attr_list));
	memcpy(&role_data.attr_list[BATTLE_ATTR_ELEMENT_BEGIN], robot_attr.element_list, sizeof(robot_attr.element_list));

	{
		AdjustAttr(role_data.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->role_attack_adjust);
		AdjustAttr(role_data.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->role_mental_adjust);
		AdjustAttr(role_data.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->role_recover_adjust);
		AdjustAttr(role_data.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->role_maxhp_adjust);
	}

	// 更新宠物属性
	BattleCharacterData & pet_data = m_battle_data.pet_character_data[0].battle_data;
	const RobotPetCfg* robot_pet = ROBOTCONFIG->GetRobotPetByLevel(pet_data.character_id, robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	if (NULL == robot_pet) return;

	pet_data.param = robot_pet->attr_con.strengthen_level;
	pet_data.level = show_level;

	pet_data.attr_list[BATTLE_ATTR_MAX_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_MAX_MP] = robot_pet->attr_con.max_mp;
	pet_data.attr_list[BATTLE_ATTR_CUR_HP] = robot_pet->attr_con.max_hp;
	pet_data.attr_list[BATTLE_ATTR_CUR_MP] = robot_pet->attr_con.max_mp;
	memcpy(pet_data.attr_list, robot_pet->attr_con.basic_attr_list, sizeof(robot_pet->attr_con.basic_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_pet->attr_con.amend_attr_list, sizeof(robot_pet->attr_con.amend_attr_list));
	memcpy(&pet_data.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_pet->attr_con.resis_attr_list, sizeof(robot_pet->attr_con.resis_attr_list));

	{
		AdjustAttr(pet_data.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->pet_attack_adjust);
		AdjustAttr(pet_data.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->pet_mental_adjust);
		AdjustAttr(pet_data.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->pet_recover_adjust);
		AdjustAttr(pet_data.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->pet_maxhp_adjust);
	}

	// 更新伙伴属性
	for (int i = 0; i < m_battle_data.partner_num; ++i)
	{
		int partner_id = m_battle_data.partner_character_data[i].battle_data.character_id;
		const RobotPartnerCfg* robot_partner = ROBOTCONFIG->GetPartner(partner_id, robot_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
		if (NULL == robot_partner) continue;

		const PartnerBasicCfg* partner_basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
		if (NULL == partner_basic_cfg) continue;

		BattleCharacterData& bcd = m_battle_data.partner_character_data[i].battle_data;
		bcd.level = show_level;

		// 属性用robot_pet_attr的
		bcd.attr_list[BATTLE_ATTR_MAX_HP] = robot_partner->attr_con.max_hp;
		bcd.attr_list[BATTLE_ATTR_MAX_MP] = robot_partner->attr_con.max_mp;
		bcd.attr_list[BATTLE_ATTR_CUR_HP] = robot_partner->attr_con.max_hp;
		bcd.attr_list[BATTLE_ATTR_CUR_MP] = robot_partner->attr_con.max_mp;
		memcpy(bcd.attr_list, robot_partner->attr_con.basic_attr_list, sizeof(robot_partner->attr_con.basic_attr_list));
		memcpy(&bcd.attr_list[BATTLE_ATTR_CORRECTED_VALUE_BEGIN], robot_partner->attr_con.amend_attr_list, sizeof(robot_partner->attr_con.amend_attr_list));
		memcpy(&bcd.attr_list[BATTLE_ATTR_ANTI_VALUE_BEGIN], robot_partner->attr_con.resis_attr_list, sizeof(robot_partner->attr_con.resis_attr_list));

		{
			AdjustAttr(bcd.attr_list[BATTLE_ATTR_ATTACK], adjust_cfg->partner_attack_adjust);
			AdjustAttr(bcd.attr_list[BATTLE_ATTR_MENTAL], adjust_cfg->partner_mental_adjust);
			AdjustAttr(bcd.attr_list[BATTLE_ATTR_RECOVERY], adjust_cfg->partner_recover_adjust);
			AdjustAttr(bcd.attr_list[BATTLE_ATTR_MAX_HP], adjust_cfg->partner_maxhp_adjust);
		}
	}
}

int Gladiator::CalcRoleCapability(const RoleBattleData& role_battle_data)
{
	int app_prof = role_battle_data.role_character_data.profession / PROFESSION_BASE;
	const CapabilityCalCfg * prof_cap_cfg = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfCapabilityCalCfg(app_prof);
	if (NULL == prof_cap_cfg) return 0;

	const BattleAttr* attr_list = role_battle_data.role_character_data.attr_list;
	double capability = 0;
	double special_capability = 0;
	for (int i = 0; i < BATTLE_ATTR_ANTI_VALUE_END && i < ARRAY_LENGTH(prof_cap_cfg->cap_base) && i < ARRAY_LENGTH(role_battle_data.role_character_data.attr_list); i++)
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
	const BattleCharacterData::SkillItem* skill_list = role_battle_data.role_character_data.skill_list;
	for (int i = 0; i < role_battle_data.role_character_data.skill_num; ++i)
	{
		int add_skill_score = LOGIC_CONFIG->GetScoreSystemConfig()->GetProfSkillScore(ProfSkillScoreKey(app_prof, skill_list[i].skill_level));
		if (add_skill_score > 0)
		{
			skill_capability += add_skill_score;
		}
	}

	return static_cast<int>(capability * 1.0F * CAPABILITY_BASE_NUM / CAPABILITY_ATTR_BASE_NUM) + skill_capability + special_capability;
}

int Gladiator::CalcPartnerCapability(const BattleCharacterData& partner_data)
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

int Gladiator::CalcPetCapability(const BattleCharacterData& pet_data)
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

int Gladiator::CalcCapability(const RoleBattleData& role_battle_data)
{
	int total_capability = 0;
	total_capability += CalcRoleCapability(role_battle_data);

	if (role_battle_data.fight_pet_idx >= 0 && role_battle_data.fight_pet_idx < role_battle_data.pet_num && role_battle_data.fight_pet_idx < ARRAY_LENGTH(role_battle_data.pet_character_data))
	{
		// 宠物的战力计算会有所偏差，因为主角的宠物战力计算不是直接用属性来计算的
		total_capability += CalcPetCapability(role_battle_data.pet_character_data[role_battle_data.fight_pet_idx].battle_data);
	}

	for (int i = 0; i < role_battle_data.partner_num && i < ARRAY_ITEM_COUNT(role_battle_data.partner_character_data); ++i)
	{
		total_capability += CalcPartnerCapability(role_battle_data.partner_character_data[i].battle_data);
	}

	for (int i = 0; i < role_battle_data.pet_helper_num && i < ARRAY_ITEM_COUNT(role_battle_data.pet_helper_character_data); ++i)
	{
		total_capability += CalcPetCapability(role_battle_data.pet_helper_character_data[i].pet_data.battle_data);
	}

	return total_capability;
}

void Gladiator::SetClearCheck()
{
	colosseum_clear_check = 1;
	this->ClearCheck();
}

void Gladiator::ClearCheck()
{
	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_uid));
	if (role != NULL)
	{
		role->GetRoleModuleManager()->GetCommonData().colosseum_task_flag.Reset();
		colosseum_clear_check = 0;
		colosseum_param.Reset();
	}
}

void Gladiator::SetColosseumParam(int colosseum_type, int param)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	if (colosseum_type < COLOSSEUN_PARAM_TYPE_0 || colosseum_type >= COLOSSEUN_PARAM_TYPE_MAX)
	{
		return;
	}
	int &param_node = colosseum_param.colosseum_param[colosseum_type];
	//int old_param = param_node;
	switch (colosseum_type)
	{
	case COLOSSEUN_PARAM_TYPE_0://排名第一是：0，进入的值预先+1，这个是倒序的
		if (param_node != 0 && param_node <= param)return;
		param_node = param;
		break;
	case COLOSSEUN_PARAM_TYPE_4:
		param_node = param;
		break;
	case COLOSSEUN_PARAM_TYPE_5:
	case COLOSSEUN_PARAM_TYPE_6:
		if (param_node >= param)return;
		param_node = param;
		break;
	case COLOSSEUN_PARAM_TYPE_1:
	case COLOSSEUN_PARAM_TYPE_2:
	case COLOSSEUN_PARAM_TYPE_3:
	case COLOSSEUN_PARAM_TYPE_7:
	case COLOSSEUN_PARAM_TYPE_8:
	case COLOSSEUN_PARAM_TYPE_9:
	case COLOSSEUN_PARAM_TYPE_10:
	case COLOSSEUN_PARAM_TYPE_11:
	case COLOSSEUN_PARAM_TYPE_12:
	case COLOSSEUN_PARAM_TYPE_13:
	case COLOSSEUN_PARAM_TYPE_14:
	case COLOSSEUN_PARAM_TYPE_15:
	case COLOSSEUN_PARAM_TYPE_16:
	case COLOSSEUN_PARAM_TYPE_17:
	case COLOSSEUN_PARAM_TYPE_18:
	case COLOSSEUN_PARAM_TYPE_19:
	case COLOSSEUN_PARAM_TYPE_20:
	case COLOSSEUN_PARAM_TYPE_21:
		if (param_node + param >= MAX_INT)
		{
			param_node = MAX_INT;
		}
		param_node += param;
		break;
	default:
		break;
	}
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_uid));
	if(NULL != role)this->SendColosseumInfo(role);
	//printf("Gladiator::SetColosseumParam [type:%d] [param:%d] old_param_node[%d] new_param_node[%d]\n", colosseum_type, param, old_param, param_node);
}

int Gladiator::GetColosseumParam(int colosseum_type)
{
	if (colosseum_type < COLOSSEUN_PARAM_TYPE_0 || colosseum_type >= COLOSSEUN_PARAM_TYPE_MAX)
	{
		return -1;
	}
	
	return colosseum_param.colosseum_param[colosseum_type];
}

void Gladiator::SendColosseumInfo(Role *role)
{
	static Protocol::SCColosseumDetalInfo info;
	info.colosseum_task_flag = colosseum_param;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void Gladiator::OnHistoryHighestRankChanged(int old_history_pos, int new_history_pos)
{
	gamelog::g_log_colosseum.printf(LL_INFO, "OnHistoryHighestRankChanged_1 role[%d,%s] old[%d] new[%d]",
		this->GetUid(), this->GetName(), old_history_pos, new_history_pos);


	int MAX_RANK = LOGIC_CONFIG->GetColosseumConfig()->GetMaxHistoryRank();
	int change_num = 0;
	if (-1 == old_history_pos)
	{
		old_history_pos = MAX_RANK;
		change_num = GetMax(0, MAX_RANK - new_history_pos);
	}
	else
	{
		old_history_pos = GetMin(MAX_RANK, old_history_pos);
		change_num = old_history_pos - new_history_pos;
	}

	if (change_num > 0)
	{
		int add_jjc_coin = LOGIC_CONFIG->GetColosseumConfig()->CalcHistoryRankReward(old_history_pos, change_num);
		if (add_jjc_coin > 0)
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(this->GetUserID());
			if (NULL != role)
			{
				role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(add_jjc_coin, MONEY_TYPE_COLOSSEUM, __FUNCTION__);
			}
			else
			{
				OfflineRegisterManager::Instance().OnColosseumHistoryRankReward(this->GetUserID(), add_jjc_coin);
			}
		}

		Protocol::SCHistoryHighestRankChangeNotice notice;
		notice.cur_rank = new_history_pos;
		notice.change_rank_num = change_num;

		this->SendMsgToUser(&notice, sizeof(notice));

		gamelog::g_log_colosseum.printf(LL_INFO, "OnHistoryHighestRankChanged_2 role[%d,%s] old[%d] new[%d] change_num[%d]",
			this->GetUid(), this->GetName(), old_history_pos, new_history_pos, change_num);
	}
}

void Gladiator::SendMsgToUser(void* data, unsigned int length)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(this->GetUserID());
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), data, length);
	}
	else if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameUserThroughCross(this->GetUid(), data, length);
	}
	else
	{
		InternalComm::Instance().SendToHiddenUserThroughCross(this->GetUid(), data, length);
	}
}

bool Gladiator::CheckHighestRankReward()
{
	assert(!this->IsRobot());
	if (!this->IsRobot())
	{
		int old_history_pos = colosseum_param.history_highest_rank_for_reward;
		if (-1 == old_history_pos)
		{
			old_history_pos = LOGIC_CONFIG->GetColosseumConfig()->GetMaxHistoryRank();
		}
		
		int new_history_pos = m_highest_rank;
		colosseum_param.history_highest_rank_for_reward = new_history_pos;

		if (new_history_pos != old_history_pos)
		{
			this->OnHistoryHighestRankChanged(old_history_pos, new_history_pos);

			return true;
		}
	}

	return false;
}
