#include "config/logicconfigmanager.hpp"
#include "other/partner/partnerconfig.hpp"
#include "gameworld/skill/skillpool.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "global/platformbattle/platformbattle.h"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "robotconfig.hpp"
#include "servercommon/serverconfig/openserverconfig.hpp"
#include "checkresourcecenter.hpp"
#include "item/itempool.h"
#include "servercommon/namefilter.hpp"
#include "servercommon/performancecatch.hpp"
#include "task/taskpool.h"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "other/surface/surfaceconfig.hpp"

//#define ROBOT_CONFIG_PERFORMANCE_CHECK

RobotConfig::RobotConfig()
{	
	memset(m_robot_advance_times_list, 0, sizeof(m_robot_advance_times_list));
	memset(m_robot_partner_random_group_list, 0, sizeof(m_robot_partner_random_group_list));
	memset(m_team_type_robot_cfg, 0, sizeof(m_team_type_robot_cfg));
	memset(m_base_weapon_id_list, 0, sizeof(m_base_weapon_id_list));
	memset(m_robot_small_advance_times_list, 0, sizeof(m_robot_small_advance_times_list));
	memset(m_robot_advance_times_list, 0, sizeof(m_robot_advance_times_list));
	
	for (int i = 0; i <= MAX_ROLE_LEVEL; ++i)
	{
		for (int k = 0; k < ROBOT_CONFIG_USE_TYPE_MAX; ++k)
		{
			m_robot_pet_level_vec[k][i].reserve(50);
			m_mutate_pet_level_vec[k][i].reserve(25);
		}
	}
}

RobotConfig::~RobotConfig()
{
	for (int i = 0; i < TEAM_TYPE_MAX; ++i)
	{
		if (NULL != m_team_type_robot_cfg[i])
		{
			delete m_team_type_robot_cfg[i];
			m_team_type_robot_cfg[i] = NULL;
		}
	}

	for (RandomGroupMap::iterator it = m_partner_random_group_map.begin(); it != m_partner_random_group_map.end(); it++)
	{
		PartnerRandomGroup().swap(it->second);
	}

	for (StaticCreatPosiMap::iterator it = m_static_creat_posi_map.begin(); it != m_static_creat_posi_map.end(); it++)
	{
		StaticCreatPosiVec().swap(it->second);
	}

	for (int i = 0; i < ROBOT_CONFIG_USE_TYPE_MAX; i++)
	{
		for (int j = 0; j < (MAX_ROLE_LEVEL + 1); j++)
		{
			std::vector<RobotPetCfg>().swap(m_robot_pet_level_vec[i][j]);
			std::vector<RobotPetCfg>().swap(m_mutate_pet_level_vec[i][j]);
		}
	}

	for (int i = 0; i < ROBOT_LEVEL_GROUP_NUM_MAX; i++)
	{
		RobotLevelMap().swap(m_robot_level_map[i]);
	}
}

RobotConfig *g_robot_config = NULL;
RobotConfig * RobotConfig::Instance()
{
	if (NULL == g_robot_config)
	{
		g_robot_config = new RobotConfig();
	}
	return g_robot_config;
}

bool RobotConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);

	LOAD_CONFIG("weapon", InitRobotBaseWeaponID);

	LOAD_CONFIG("appearance", InitRobotAdvanceCfg);

	LOAD_CONFIG("pet_att", InitPetAttrCfg);

	LOAD_CONFIG("role", InitRoleRobotCfg);

	LOAD_CONFIG("pet_2", InitPetRobotCfg);

#if defined(_DEBUG) && defined(ROBOT_CONFIG_PERFORMANCE_CHECK)
	PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_ROBOT_CONFIG_LOAD);
#endif
	LOAD_CONFIG("parnter", InitPartnerRobotCfg);
#if defined(_DEBUG) && defined(ROBOT_CONFIG_PERFORMANCE_CHECK)
	long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_ROBOT_CONFIG_LOAD);
	printf("RobotConfig::Init InitPartnerRobotCfg Elapse[%lld ms]\n", elapse_us / 1000);
#endif

	LOAD_CONFIG("partner_random_group", InitRobotPartnerRandomGroupCfg);

	LOAD_CONFIG("partner_random", InitRobotPartnerRandomCfg);

	LOAD_CONFIG("run_robot", InitTeamTypeRobotCfg);

	LOAD_CONFIG("level_group", InitRobotLevelMap);

	LOAD_CONFIG("other", InitMissionCreatCfg);
	LOAD_CONFIG("auto_robot", InitRangerCreatCfg);
	LOAD_CONFIG("static_robot_num", InitStaticCfg);
	LOAD_CONFIG("static_robot_pos", InitStaticCreatPosiCfg);
	LOAD_CONFIG("invite_war", InitCapacityTargetValueCfg);
	LOAD_CONFIG("auto_talk", InitAutoTalkCfg);
	
	iRet = this->InitNameCfg(RootElement);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: InitNameConfig failed %d", configname.c_str(), iRet);
		*err = errinfo;
		return false;
	}
	
	iRet = this->InitRoleNameCfg(RootElement);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: InitRoleNameCfg failed %d", configname.c_str(), iRet);
		*err = errinfo;
		return false;
	}

	return true;
}

bool RobotConfig::Reload(const std::string & configname, std::string * err)
{
	RobotConfig *temp = new RobotConfig();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_robot_config)
	{
		delete g_robot_config;
	}

	g_robot_config = temp;

	return true;
}

const RobotPetCfg* RobotConfig::RandRobotPetByLevel(int level, int use_way)
{
	if (level < 0 || level > MAX_ROLE_LEVEL) return NULL;

	if (m_mutate_pet_level_vec[use_way][level].size() > 0 && rand() % 100 < m_other_cfg.robot_pet_mutate_rate)
	{
		size_t robot_idx = rand() % m_mutate_pet_level_vec[use_way][level].size();
		return &m_mutate_pet_level_vec[use_way][level][robot_idx];
	}

	if (m_robot_pet_level_vec[use_way][level].size() > 0)
	{
		size_t robot_idx = rand() % m_robot_pet_level_vec[use_way][level].size();
		return &m_robot_pet_level_vec[use_way][level][robot_idx];
	}

	return NULL;
}

const RobotPetCfg* RobotConfig::GetRobotPetByLevel(int pet_id, int level, int use_way)
{
	if (level < 0 || level > MAX_ROLE_LEVEL) return NULL;
	if (use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX) return NULL;

	RobotPetPtrMap::iterator it = m_pet_cfg_ptr_map[use_way][level].find(pet_id);
	if (it == m_pet_cfg_ptr_map[use_way][level].end()) return NULL;

	return it->second;
}

const RobotPartnerCfg* RobotConfig::GetPartner(int partner_id, int level, int use_way)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return NULL;
	if (partner_id <= 0 || partner_id >= PARTNER_NUM_MAX) return NULL;
	if (use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX) return NULL;

	return &m_partner_robot_list[use_way][level][partner_id];
}

const RobotAttrCon* RobotConfig::GetPetAttrByTypeAndLevel(int attr_type, int level, int use_way)
{
	if (attr_type <= 0 || attr_type >= ROBOT_PET_ATTR_NUM_MAX) return NULL;
	if (level <= 0 || level > MAX_ROLE_LEVEL) return NULL;
	if (use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX) return NULL;

	return &m_pet_attr_type_cfg_list[use_way][attr_type].attr_con_list[level];
}

const RobotRoleCfg* RobotConfig::RandRobotByLevel(int level, int use_way)
{
	if (level < 0 || level > MAX_ROLE_LEVEL) return NULL;
	if (use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX) return NULL;

	int rand_prof = RandomNum(1, PROFESSION_TYPE_NUM);

	return &m_robot_role_list[use_way][level][rand_prof];
}

const RobotRoleCfg* RobotConfig::RandRobotByLevelAndProf(int level, int prof, int use_way)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return NULL;
	if (prof <= 0 || prof >= PROFESSION_TYPE_NUM) return NULL;
	if (use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX) return NULL;

	return &m_robot_role_list[use_way][level][prof];
}

RoleBattleData RobotConfig::GetRobotBattleData(int level, int use_way)
{
	RoleBattleData  battle_data;
	battle_data.Reset();

	battle_data.is_auto = 1;
	battle_data.role_stand_row = BATTLE_ROW_TYPE_BACK;

	const RobotRoleCfg * robot_role = this->RandRobotByLevel(level,use_way);
	if (NULL != robot_role)
	{
		int m_profession = robot_role->base_prof * PROFESSION_BASE + robot_role->advance_times;
		int m_avatar = AppearanceConfig::Instance().RandAvatarByProfession(m_profession);

		GameName robot_name;
		ROBOTCONFIG->GetRandomName(SEX_ARRAY[m_avatar], robot_name, false);

		battle_data.appearance.Reset();
		battle_data.appearance.advance_times = robot_role->advance_times;
		battle_data.appearance.avatar_type = m_avatar;
		battle_data.appearance.headshot_id = 0;
		battle_data.appearance.color = rand() % INIT_APPREARANCE_COLOR_NUM;
		battle_data.appearance.prof_base = robot_role->base_prof;
		battle_data.appearance.weapon_id = robot_role->weapon_id;
		battle_data.appearance.sub_weapon_id = 0;

		BattleCharacterData & role_data = battle_data.role_character_data;
		role_data.character_id = -1;
		role_data.character_type = BATTLE_CHARACTER_TYPE_ROLE_SHADOW;
		role_data.level = level;
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

		const RobotOtherCfg & other_cfg = ROBOTCONFIG->GetOtherCfg();
		for (int i = 0; i < ROBOT_ROLE_SKILL_MAX && role_data.skill_num < MAX_BATTLE_SKILL_NUM; ++i)
		{
			const RobotSkillCon & robot_skill = robot_attr.skill_list[i];
			if (NULL == SkillPool::Instance().GetSkillCfg(robot_skill.skill_id) || robot_skill.skill_lv <= 0) continue;

			BattleCharacterData::SkillItem & skill = role_data.skill_list[role_data.skill_num++];
			skill.skill_id = robot_skill.skill_id;
			skill.skill_level = robot_skill.skill_lv;
		}
	}

	// 宠物数据
	const RobotPetCfg * robot_pet = ROBOTCONFIG->RandRobotPetByLevel(level, use_way);
	if (NULL != robot_pet)
	{
		battle_data.pet_num = 1;
		BattleCharacterData & pet_data = battle_data.pet_character_data[0].battle_data;
		pet_data.character_id = robot_pet->pet_id;
		pet_data.character_type = BATTLE_CHARACTER_TYPE_PET;
		pet_data.level = level;
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
		pet_data.skill_num = 0; // 宠物技能在战斗服加载

	}

	return battle_data;
}

bool RobotConfig::GetRandomName(int sex, GameName out_name, bool need_prefix /*= true*/, bool need_infix /*= true*/, bool need_suffix /*= true*/, int name_type /*= ROBOT_NAME_TYPE_DEF*/)
{
	if (sex != MALE && sex != FEMALE) return false;
	if (!need_prefix && !need_infix && !need_suffix) return false;
	if (NULL == out_name) return false;

	const NameCfg::NameList *_first_name = NULL;
	const NameCfg::NameList *_middle_name = NULL;
	const NameCfg::NameList *_last_name = NULL;

	switch (name_type)
	{
	case ROBOT_NAME_TYPE_ROLE:
		{//模拟玩家名字  前--中后
		 //_first_name = need_prefix ? &m_role_name_cfg.first_name[sex] : NULL;
		 //_middle_name = need_infix ? &m_role_name_cfg.middle_name[FEMALE_MIDDLE] : NULL;
		 //_last_name = need_suffix ? &m_role_name_cfg.last_name[FEMALE_LAST] : NULL;
			_first_name = need_prefix ? &m_role_name_cfg.first_name[sex] : NULL;
			_middle_name = need_infix ? &m_role_name_cfg.middle_name[sex] : NULL;
			_last_name = need_suffix ? &m_role_name_cfg.last_name[sex] : NULL;
		}
		break;
	default:
		{
			const NameCfg& name_cfg = this->GetNameCfg();

			_first_name = need_prefix ? &name_cfg.first_name[sex] : NULL;
			_middle_name = need_infix ? &name_cfg.middle_name[sex] : NULL;
			_last_name = need_suffix ? &name_cfg.last_name[sex] : NULL;
		}
		break;
	}



	unsigned int rand_fir_length = NULL != _first_name ? (unsigned int)(_first_name->name_str_list.size()) : 0;
	unsigned int rand_mid_length = NULL != _middle_name ? (unsigned int)(_middle_name->name_str_list.size()) : 0;
	unsigned int rand_last_length = NULL != _last_name ? (unsigned int)(_last_name->name_str_list.size()) : 0;

	//long long name_str_id = 0;
	unsigned int rand_index;

	std::string name;
	name.reserve(sizeof(GameName));

	if (rand_fir_length > 0)
	{
		// 随机前缀
		rand_index = rand() % rand_fir_length;
		name += _first_name->name_str_list[rand_index].name_str;
	}

	if (rand_mid_length > 0)
	{
		// 随机中缀
		rand_index = rand() % rand_mid_length;
		name += _middle_name->name_str_list[rand_index].name_str;
	}

	if (rand_last_length > 0)
	{
		// 随机后缀
		rand_index = rand() % rand_last_length;
		name += _last_name->name_str_list[rand_index].name_str;
	}

	if (0 == name.length()) return false;

	F_STRNCPY(out_name, name.c_str(), sizeof(GameName));
	return true;
}

int RobotConfig::RandRobotLevelByGroup(int group_id, int level)
{
	if (group_id < 0 || group_id >= ROBOT_LEVEL_GROUP_NUM_MAX) return level;

	RobotLevelMap::iterator it = m_robot_level_map[group_id].lower_bound(level);
	if (it == m_robot_level_map[group_id].end()) return level;

	return RandomNum(it->first, it->second + 1);
}

const TeamTypeRobotCfg* RobotConfig::GetAutoTeamRobotCfg(int team_type)
{
	if (team_type < 0 || team_type >= TEAM_TYPE_MAX)
	{
		return NULL;
	}

	return m_team_type_robot_cfg[team_type];
}

int RobotConfig::GetRobotAdvanceTimes(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return 0;

	return m_robot_advance_times_list[level];
}

int RobotConfig::GetRobotSmallAdvanceTimes(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL) return 0;

	return m_robot_small_advance_times_list[level];
}

const RobotConfig::StaticCreatPosiVec * RobotConfig::GetStaticCreatPosiCfg(int scene_id) const
{
	StaticCreatPosiMap::const_iterator cit = m_static_creat_posi_map.find(scene_id);
	if (cit == m_static_creat_posi_map.end())
	{
		return NULL;
	}
	return &cit->second;
}

void RobotConfig::GetPlatformBattleRotbotCfg(int platform_number, int robot_index, PlatformBattleRobotInfo ARG_OUT * robot_info_item_ptr)
{
	if (robot_info_item_ptr ==NULL)
	{
		return;
	}

	PlatformBattleRobotInfo & robot_info_item = *robot_info_item_ptr;

	const PlatformBattleOtherCfg & other_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetOtherCfg();
	int num = LOGIC_CONFIG->GetPlatformBattleConfig()->GetMaxAllowableNum(platform_number);
	int robot_role_level = LOGIC_CONFIG->GetPlatformBattleConfig()->GetRandRobotRoleLevel(platform_number, robot_index);
	int show_level = other_cfg.robot_show_level;

	std::vector<int >partner_id_list= ROBOTCONFIG->RandRobotPartnerList2(robot_role_level, num);
	robot_info_item.partner_robot_data.reserve(partner_id_list.size());
	robot_info_item.pet_robot_data.reserve(num);

	for (int cur_num = 0; cur_num < num; ++cur_num)
	{
		bool exchange_pos = rand() % 2 > 0;//打乱位置

		//宠物
		int min_pet_level = robot_role_level - 5;
		min_pet_level = min_pet_level < 0 ? 0 : min_pet_level;
		int max_pet_level = robot_role_level + 5;
		max_pet_level = max_pet_level < min_pet_level ? min_pet_level + 1 : max_pet_level;

		int robot_pet_level = RandomNum(min_pet_level, robot_role_level);
		const RobotPetCfg * robot_pet_cfg = RandRobotPetByLevel(robot_pet_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);

		if (robot_pet_cfg != NULL)
		{
			PlatformBattleRobotPetItem pet_item;
			pet_item.pet_capability = robot_pet_cfg->GetCapability();

			BattleCharacterData & bcd = pet_item.pet_data.battle_data;
			bcd.character_id = robot_pet_cfg->pet_id;
			bcd.character_type = BATTLE_CHARACTER_TYPE_PET;
			bcd.level = show_level;
			bcd.param = robot_pet_cfg->attr_con.strengthen_level;
			SNPRINTF(bcd.name, sizeof(bcd.name), "%s", robot_pet_cfg->name);
			bcd.is_use_cur_hp_mp = 0;

			const PetCfg * pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(robot_pet_cfg->pet_id);
			if (NULL != pet_cfg)
			{
				bcd.race = pet_cfg->race;
				bcd.attr_list[BATTLE_ATTR_ELEMENT_WATER] = pet_cfg->element_water;
				bcd.attr_list[BATTLE_ATTR_ELEMENT_FIRE] = pet_cfg->element_fire;
				bcd.attr_list[BATTLE_ATTR_ELEMENT_WIND] = pet_cfg->element_wind;
				bcd.attr_list[BATTLE_ATTR_ELEMENT_EARTH] = pet_cfg->element_land;
			}

			// 属性用robot_pet_attr的
			robot_pet_cfg->GetAttr(bcd.attr_list);

			// 技能用robot_pet的
			int skill_num = ARRAY_LENGTH(robot_pet_cfg->attr_con.skill_list);
			bcd.skill_num = 0;
			for (int i = 0; i < skill_num; ++i)
			{
				if (robot_pet_cfg->attr_con.skill_list[i].skill_id > 0 && robot_pet_cfg->attr_con.skill_list[i].skill_lv > 0)
				{
					bcd.skill_list[bcd.skill_num].skill_id = robot_pet_cfg->attr_con.skill_list[i].skill_id;
					bcd.skill_list[bcd.skill_num].skill_level = robot_pet_cfg->attr_con.skill_list[i].skill_lv;
					++bcd.skill_num;
				}
			}

			pet_item.pos = exchange_pos ? cur_num : cur_num + PLATFORM_BATTLE_PET_PARTNER_MAX;

			robot_info_item.pet_robot_data.push_back(pet_item);
		}

		if (cur_num < (int)partner_id_list.size())
		{
			int partner_id = partner_id_list[cur_num];

			const RobotPartnerCfg* partner_data_cfg = this->GetPartner(partner_id, robot_role_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
			if (partner_data_cfg == NULL)
			{
				continue;
			}		

			const PartnerBasicCfg * partner_basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
			if (NULL == partner_basic_cfg)
			{
				continue;
			}

			static PlatformBattleRobotPartnerItem partner_item;
			partner_item.Reset();
			BattleCharacterData * out_data = &partner_item.partner_data.battle_data;
			partner_data_cfg->GetAttr(out_data->attr_list);
			out_data->attr_list[BATTLE_ATTR_CUR_HP] = out_data->attr_list[BATTLE_ATTR_MAX_HP];
			out_data->attr_list[BATTLE_ATTR_CUR_MP] = out_data->attr_list[BATTLE_ATTR_MAX_MP];
			CLEAR_PARTNER_CRYSTAL( out_data->attr_list);
			PARTNER_CRYSTAL_PUT( partner_basic_cfg->crystal_item, out_data->attr_list);
			partner_item.partner_capability = partner_data_cfg->GetCapability(out_data->attr_list);

			out_data->character_id = partner_data_cfg->partner_id;
			out_data->character_type = BATTLE_CHARACTER_TYPE_PARTNER;
			out_data->level = show_level;
			SNPRINTF(out_data->name, sizeof(out_data->name), partner_data_cfg->name);
			out_data->profession = partner_data_cfg->profession;
			out_data->weapon_type = WEAPON_TYPE_NONE;	// 伙伴一律空手
			out_data->weapon_id = 0;
			out_data->is_use_cur_hp_mp = 0;
			out_data->skill_num = 0; 	// 伙伴技能
			for (int t_s_num = 0; t_s_num < PARTNER_COMMON_SKILL_MAX && t_s_num < ARRAY_ITEM_COUNT(partner_data_cfg->attr_con.skill_list); ++t_s_num)
			{
				BattleCharacterData::SkillItem & od_sl = out_data->skill_list[t_s_num];
				od_sl.skill_id = partner_data_cfg->attr_con.skill_list[t_s_num].skill_id;
				od_sl.skill_level = partner_data_cfg->attr_con.skill_list[t_s_num].skill_lv;
				out_data->skill_num += 1;
			}
			partner_item.pos = exchange_pos ? cur_num + PLATFORM_BATTLE_PET_PARTNER_MAX : cur_num;

			robot_info_item.partner_robot_data.push_back(partner_item);
		}
	}

	robot_info_item.robot_role_level = show_level;
}

void RobotConfig::RandRobotPartnerList(int level, int max_num, int* out_num, int* out_list)
{
	if (NULL == out_num || NULL == out_list) return;

	if (level <= 0 || level > MAX_ROLE_LEVEL || max_num <= 0)
	{
		*out_num = 0;
		return;
	}

	int group = m_robot_partner_random_group_list[level];
	RandomGroupMap::iterator it = m_partner_random_group_map.find(group);
	if (it == m_partner_random_group_map.end())
	{
		*out_num = 0;
		return;
	}

	PartnerRandomGroup& partner_list = it->second;
	std::random_shuffle(partner_list.begin(), partner_list.end());
	int num = 0;
	for (int i = 0; i < (int)partner_list.size() && i < max_num; ++i)
	{
		out_list[num++] = partner_list[i];
	}
	*out_num = num;
}

int RobotConfig::GetRandAutoGroupsId(int key_hash, int account_role_num)
{
	UNSTD_STATIC_CHECK(MAX_AUTO_TALK_TALK_GROUPS_NUM == 5);
	//此时是固定5组,配置已经检测过了,对应5个角色
	int rand_index = (account_role_num - 1) % MAX_AUTO_TALK_TALK_GROUPS_NUM;	//account_role_num[1,5] 需要先减1,否则第五个获取的是第一个
	int i = 0;
	for (std::set<int>::const_iterator it = m_auto_talk_groups_set.begin(); it != m_auto_talk_groups_set.end(); ++it, ++i)
	{
		if (rand_index == i)
		{
			return *it;
		}
	}

	return 0;
}

const AutoTalkCfg * RobotConfig::GetAutoTalkCfg(int auto_talk_group_id, int talk_id)
{
	if(auto_talk_group_id <= 0 || talk_id <= 0) return NULL;

	int talk_key = auto_talk_group_id * MAX_AUTO_TALK_TALK_GROUPS_BASE_NUM + talk_id;
	std::map<int, AutoTalkCfg>::const_iterator it = m_auto_talk_map.find(talk_key);
	if(it != m_auto_talk_map.end())
	{
		return &it->second;
	}

	return NULL;
}

std::vector<int> RobotConfig::RandRobotPartnerList2(int level, int max_num)
{
	std::vector<int> ret;

	if (level <= 0 || level > MAX_ROLE_LEVEL || max_num <= 0)
	{
		return ret;
	}

	int group = m_robot_partner_random_group_list[level];
	RandomGroupMap::iterator it = m_partner_random_group_map.find(group);
	if (it == m_partner_random_group_map.end())
	{
		return ret;
	}

	PartnerRandomGroup& partner_list = it->second;
	std::random_shuffle(partner_list.begin(), partner_list.end());

	for (int i = 0; i < (int)partner_list.size() && i < max_num; ++i)
	{
		ret.push_back(partner_list[i]);
	}

	return ret;
}

void RobotConfig::AddBaseWeaponID(int weapon_type, ItemID weapon_id)
{
	if (weapon_type < 0 || weapon_type >= WEAPON_TYPE_MAX) return;

	if (m_base_weapon_id_list[weapon_type] > 0) return;

	m_base_weapon_id_list[weapon_type] = weapon_id;
}

ItemID RobotConfig::GetBaseWeaponID(int weapon_type)
{
	if (weapon_type < 0 || weapon_type >= WEAPON_TYPE_MAX) return 0;

	return m_base_weapon_id_list[weapon_type];
}

int RobotConfig::InitRobotBaseWeaponID(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	memset(m_base_weapon_id_list, 0, sizeof(m_base_weapon_id_list));
	while (NULL != root_element)
	{
		int weapon_type = 0;
		if (!GetSubNodeValue(root_element, "weapon_type", weapon_type))
		{
			return -1;
		}
		int weapon_id = 0;
		if (!GetSubNodeValue(root_element, "id", weapon_id))
		{
			return -2;
		}
		this->AddBaseWeaponID(weapon_type, weapon_id);
		root_element = root_element->NextSiblingElement();
	}

	for (int i = 1; i < WEAPON_TYPE_MAX; ++i)
	{
		if (0 == m_base_weapon_id_list[i])
		{
			return -5000;
		}
	}
	UNSTD_STATIC_CHECK(WEAPON_TYPE_MAX == 9);

	return 0;
}

int RobotConfig::InitRoleRobotCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	char skill_reader[128] = { 0 };
	int last_level = 0;
	int last_prof = 0;
	int last_use_way = -1;
	while (NULL != root_element)
	{
		RobotRoleCfg tmp_con; RobotAttrCon & tmp_attr_con = tmp_con.attr_con;

		if (!GetSubNodeValue(root_element, "use_way", tmp_con.use_way)
			|| tmp_con.use_way < 0 || tmp_con.use_way >= ROBOT_CONFIG_USE_TYPE_MAX 
			|| (tmp_con.use_way != last_use_way + 1 && tmp_con.use_way != last_use_way))
		{
			return -1;
		}
		if (last_use_way != tmp_con.use_way)
		{
			last_use_way = tmp_con.use_way;
			last_level = 0;
			last_prof = 0;
		}

		if (!GetSubNodeValue(root_element, "level", tmp_con.level) || tmp_con.level <= 0 || tmp_con.level > MAX_ROLE_LEVEL || (last_level + 1 != tmp_con.level && last_level != tmp_con.level))
		{
			return -2;
		}
		if (last_level != tmp_con.level)
		{
			last_level = tmp_con.level;
			last_prof = 0;
		}

		if (!GetSubNodeValue(root_element, "prof", tmp_con.base_prof) || tmp_con.base_prof <= 0 || tmp_con.base_prof >= PROFESSION_TYPE_NUM || tmp_con.base_prof != last_prof + 1)
		{
			return -3;
		}
		last_prof = tmp_con.base_prof;

		tmp_con.advance_times = this->GetRobotAdvanceTimes(tmp_con.level);
		tmp_con.small_advance_times = this->GetRobotSmallAdvanceTimes(tmp_con.level);

		if (!GetSubNodeValue(root_element, "weapon_type", tmp_con.weapon_type) || tmp_con.weapon_type < 0 || tmp_con.weapon_type >= WEAPON_TYPE_MAX)
		{
			return -4;
		}

		tmp_con.weapon_id = this->GetBaseWeaponID(tmp_con.weapon_type);

		if (!GetSubNodeValue(root_element, "maxhp", tmp_attr_con.max_hp) || tmp_attr_con.max_hp <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "maxmp", tmp_attr_con.max_mp) || tmp_attr_con.max_mp <= 0)
		{
			return -6;
		}

		const char * base_attr_reader[BATTLE_BASE_ATTR_NUM] = { "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int i = 0; i < BATTLE_BASE_ATTR_NUM; ++i)
		{
			if (!GetSubNodeValue(root_element, base_attr_reader[i], tmp_attr_con.basic_attr_list[i]) || tmp_attr_con.basic_attr_list[i] < 0)
			{
				return -(100 + i);
			}
		}

		const char * amen_attr_reader[] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "bisha_dmg", "jianren" };
		for (int i = 0; i < BATTLE_AMEN_ATTR_NUM && i < ARRAY_LENGTH(amen_attr_reader); ++i)
		{
			if (!GetSubNodeValue(root_element, amen_attr_reader[i], tmp_attr_con.amend_attr_list[i]) || tmp_attr_con.amend_attr_list[i] < 0)
			{
				return -(200 + i);
			}
		}

		const char * resi_attr_reader[] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int i = 0; i < ARRAY_LENGTH(resi_attr_reader) && i < ARRAY_LENGTH(tmp_attr_con.resis_attr_list); ++i)
		{
			if (!GetSubNodeValue(root_element, resi_attr_reader[i], tmp_attr_con.resis_attr_list[i]) || tmp_attr_con.resis_attr_list[i] < 0)
			{
				return -(300 + i);
			}
		}

		const char * element_reader[BATTLE_ELEMENT_ATTR_NUM] = { "water", "fire", "wind", "earth" };
		for (int i = 0; i < BATTLE_ELEMENT_ATTR_NUM; ++i)
		{
			if (!GetSubNodeValue(root_element, element_reader[i], tmp_attr_con.element_list[i]) || tmp_attr_con.element_list[i] < 0 || tmp_attr_con.element_list[i] > BATTLE_ELEMENT_MAX)
			{
				return -(400 + i);
			}
		}

		for (int idx = 0; idx < ROBOT_ROLE_SKILL_MAX; ++idx)
		{
			RobotSkillCon & skill_con = tmp_attr_con.skill_list[idx];

			SNPRINTF(skill_reader, sizeof(skill_reader), "skill_id_%d", idx);
			if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_id) || (0 != skill_con.skill_id && NULL == SkillPool::Instance().GetSkillCfg(skill_con.skill_id)))
			{
				return -1000;
			}

			SNPRINTF(skill_reader, sizeof(skill_reader), "skill_level_%d", idx);
			if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_lv) || skill_con.skill_lv < 0 || skill_con.skill_lv > MAX_BATTLE_SKILL_LEVEL)
			{
				return -1001;
			}
		}

		if (!GetSubNodeValue(root_element, "surface_id", tmp_con.surface_id) || tmp_con.surface_id < 0 ||
			(tmp_con.surface_id > 0 && NULL == LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(tmp_con.surface_id)))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "surface_arms_id", tmp_con.surface_arms_id) || tmp_con.surface_arms_id < 0 ||
			(tmp_con.surface_arms_id > 0 && NULL == LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(tmp_con.surface_arms_id)))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "mount_id", tmp_con.mount_id) || tmp_con.mount_id < 0 ||
			(tmp_con.mount_id > 0 && NULL == LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(tmp_con.mount_id)))
		{
			return -9;
		}

		m_robot_role_list[tmp_con.use_way][tmp_con.level][tmp_con.base_prof] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitPetRobotCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	char skill_reader[128] = { 0 };
	int last_use_way = -1;
	while (NULL != root_element)
	{
		RobotPetCfg tmp_con; 
		RobotAttrCon& tmp_attr_con = tmp_con.attr_con;
		if (!GetSubNodeValue(root_element, "use_way", tmp_con.use_way) || tmp_con.use_way < 0 || tmp_con.use_way >= ROBOT_CONFIG_USE_TYPE_MAX || (tmp_con.use_way != last_use_way && tmp_con.use_way != last_use_way + 1))
		{
			return -1;
		}
		if (last_use_way != tmp_con.use_way)
		{
			last_use_way = tmp_con.use_way;
		}

		if (!GetSubNodeValue(root_element, "pet_id", tmp_con.pet_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "type", tmp_con.pet_attr_type))
		{
			return -33;
		}

		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(tmp_con.pet_id);
		if (NULL == pet_cfg)
		{
			return -4;
		}
		tmp_con.pet_race = pet_cfg->race;
		tmp_con.is_long_range_unit = pet_cfg->is_long_range_unit;
		F_STRNCPY(tmp_con.name, pet_cfg->name, sizeof(tmp_con.name));

		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = pet_cfg->element_water;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = pet_cfg->element_fire;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = pet_cfg->element_wind;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = pet_cfg->element_land;
		
		for (int idx = 0; idx < 2; ++idx) // 现在只有2个技能了
		{
			RobotSkillCon & skill_con = tmp_attr_con.skill_list[idx];

			SNPRINTF(skill_reader, sizeof(skill_reader), "skill_id_%d", idx);
			if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_id) || (0 != skill_con.skill_id && NULL == SkillPool::Instance().GetSkillCfg(skill_con.skill_id)))
			{
				return -1000;
			}

			skill_con.skill_lv = 1;

			//SNPRINTF(skill_reader, sizeof(skill_reader), "skill_level_%d", idx);
			//if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_lv) || skill_con.skill_lv < 0 || skill_con.skill_lv > MAX_BATTLE_SKILL_LEVEL)
			//{
			//	return -1001;
			//}
		}

		for (int lv = 1; lv <= MAX_ROLE_LEVEL; ++lv)
		{
			tmp_con.level = lv;
			const RobotAttrCon* attr_cfg = this->GetPetAttrByTypeAndLevel(tmp_con.pet_attr_type, lv, tmp_con.use_way);
			if (NULL != attr_cfg)
			{
				tmp_attr_con.strengthen_level = attr_cfg->strengthen_level;
				tmp_attr_con.max_hp = attr_cfg->max_hp;
				tmp_attr_con.max_mp = attr_cfg->max_mp;
				memcpy(tmp_attr_con.basic_attr_list, attr_cfg->basic_attr_list, sizeof(tmp_attr_con.basic_attr_list));
				memcpy(tmp_attr_con.amend_attr_list, attr_cfg->amend_attr_list, sizeof(tmp_attr_con.amend_attr_list));
				memcpy(tmp_attr_con.resis_attr_list, attr_cfg->resis_attr_list, sizeof(tmp_attr_con.resis_attr_list));
			}
			else
			{
#ifdef _DEBUG
				assert(0);
#endif
			}

			if (pet_cfg->pet_mutant_times)
			{
				m_mutate_pet_level_vec[tmp_con.use_way][lv].push_back(tmp_con);
				m_pet_cfg_ptr_map[tmp_con.use_way][lv][tmp_con.pet_id] = &m_mutate_pet_level_vec[tmp_con.use_way][lv].back();
			}
			else
			{
				m_robot_pet_level_vec[tmp_con.use_way][lv].push_back(tmp_con);
				m_pet_cfg_ptr_map[tmp_con.use_way][lv][tmp_con.pet_id] = &m_robot_pet_level_vec[tmp_con.use_way][lv].back();
			}
		}
		
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitPetAttrCfg(TiXmlElement* RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -10000;

	int last_type = 0;
	int last_level = 0;
	int last_use_way = -1;
	while (data_element)
	{
		int use_way = 0;
		if (!GetSubNodeValue(data_element, "use_way", use_way) || use_way < 0 || use_way >= ROBOT_CONFIG_USE_TYPE_MAX || (use_way != last_use_way && use_way != last_use_way + 1))
		{
			return -111;
		}
		if (last_use_way != use_way)
		{
			last_use_way = use_way;
			last_type = 0;
			last_level = 0;
		}

		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) 
			|| type >= ROBOT_PET_ATTR_NUM_MAX 
			|| (type != last_type + 1 && type != last_type))
		{
			return -1;
		}
		if (type == last_type + 1)
		{
			last_level = 0;
		}
		last_type = type;

		RobotPetAttrCfg& cfg = m_pet_attr_type_cfg_list[use_way][type];
		cfg.attr_type = type;

		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) 
			|| level > MAX_ROLE_LEVEL 
			|| (level != last_level + 1))
		{
			return -2;
		}
		last_level = level;

		RobotAttrCon& attr_cfg = cfg.attr_con_list[level];

		if (!GetSubNodeValue(data_element, "strengthen_level", attr_cfg.strengthen_level))
		{
			return -4;
		}

		if (!GetSubNodeValue(data_element, "maxhp", attr_cfg.max_hp) || attr_cfg.max_hp <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(data_element, "maxmp", attr_cfg.max_mp) || attr_cfg.max_mp <= 0)
		{
			return -6;
		}

		const char * base_attr_reader[BATTLE_BASE_ATTR_NUM] = { "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int i = 0; i < BATTLE_BASE_ATTR_NUM; ++i)
		{
			if (!GetSubNodeValue(data_element, base_attr_reader[i], attr_cfg.basic_attr_list[i]) || attr_cfg.basic_attr_list[i] < 0)
			{
				return -(100 + i);
			}
		}

		const char * amen_attr_reader[] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "bisha_dmg", "jianren" };
		for (int i = 0; i < BATTLE_AMEN_ATTR_NUM && i < ARRAY_LENGTH(amen_attr_reader); ++i)
		{
			if (!GetSubNodeValue(data_element, amen_attr_reader[i], attr_cfg.amend_attr_list[i]) || attr_cfg.amend_attr_list[i] < 0)
			{
				return -(200 + i);
			}
		}

		const char * resi_attr_reader[] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int i = 0; i < ARRAY_LENGTH(resi_attr_reader) && i < ARRAY_LENGTH(attr_cfg.resis_attr_list); ++i)
		{
			if (!GetSubNodeValue(data_element, resi_attr_reader[i], attr_cfg.resis_attr_list[i]) || attr_cfg.resis_attr_list[i] < 0)
			{
				return -(300 + i);
			}
		}

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitRobotAdvanceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -10000;

	while (data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		int advance_times = 0;
		if (!GetSubNodeValue(data_element, "advanced_num", advance_times) || advance_times < 0 || advance_times > PROFESSION_ADVANCE_TIMES)
		{
			return -2;
		}
		m_robot_advance_times_list[level] = advance_times;

		int small_advance_times = 0;
		if (!GetSubNodeValue(data_element, "low_num", small_advance_times) || small_advance_times < 0 || small_advance_times > 9)
		{
			return -3;
		}
		m_robot_small_advance_times_list[level] = small_advance_times;

		data_element = data_element->NextSiblingElement();
	}

	{
		int cur_times = 0;
		for (int lv = 1; lv <= MAX_ROLE_LEVEL; ++lv)
		{
			if (m_robot_advance_times_list[lv] != 0)
			{
				cur_times = m_robot_advance_times_list[lv];
			}
			else
			{
				m_robot_advance_times_list[lv] = cur_times;
			}
		}
	}
	
	{
		int cur_times = 0;
		for (int lv = 1; lv <= MAX_ROLE_LEVEL; ++lv)
		{
			if (m_robot_small_advance_times_list[lv] != 0)
			{
				cur_times = m_robot_small_advance_times_list[lv];
			}
			else
			{
				m_robot_small_advance_times_list[lv] = cur_times;
			}
		}
	}

	return 0;
}

int RobotConfig::InitRobotPartnerRandomGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -10000;

	while (data_element)
	{
		int index = 0;
		if (!GetSubNodeValue(data_element, "num", index) || index < 0)
		{
			return -1;
		}

		int partnet_id = 0;
		if (!GetSubNodeValue(data_element, "partner_id", partnet_id) || partnet_id < 0)
		{
			return -2;
		}
		CheckResourceCenter::Instance().GetPartnerCheck()->Add(partnet_id, __FUNCTION__);

		m_partner_random_group_map[index].push_back(partnet_id);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitRobotPartnerRandomCfg(TiXmlElement* RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -10000;

	while (data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "min_level", level) || level <= 0 || level > MAX_ROLE_LEVEL)
		{
			return -1;
		}

		int group = 0;
		if (!GetSubNodeValue(data_element, "partner_group", group) || m_partner_random_group_map.find(group) == m_partner_random_group_map.end())
		{
			return -2;
		}
		m_robot_partner_random_group_list[level] = group;

		data_element = data_element->NextSiblingElement();
	}

	int cur_group = 0;
	for (int lv = 1; lv <= MAX_ROLE_LEVEL; ++lv)
	{
		if (m_robot_partner_random_group_list[lv] != 0)
		{
			cur_group = m_robot_partner_random_group_list[lv];
		}
		else
		{
			m_robot_partner_random_group_list[lv] = cur_group;
		}
	}

	return 0;
}

int RobotConfig::InitPartnerRobotCfg(TiXmlElement* RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	char skill_reader[128] = { 0 };
	int last_level = 0;
	int last_partner_id = 0;
	int last_use_way = -1;
	while (NULL != root_element)
	{
		RobotPartnerCfg tmp_con; RobotAttrCon & tmp_attr_con = tmp_con.attr_con;

		if (!GetSubNodeValue(root_element, "use_way", tmp_con.use_way) || tmp_con.use_way < 0 || tmp_con.use_way >= ROBOT_CONFIG_USE_TYPE_MAX || (tmp_con.use_way != last_use_way && tmp_con.use_way != last_use_way + 1))
		{
			return -1;
		}
		if (tmp_con.use_way != last_use_way)
		{
			last_use_way = tmp_con.use_way;
			last_level = 0;
			last_partner_id = 0;
		}

		if (!GetSubNodeValue(root_element, "level", tmp_con.level) || tmp_con.level <= 0 || tmp_con.level > MAX_ROLE_LEVEL || (tmp_con.level != last_level && tmp_con.level != last_level + 1))
		{
			return -2;
		}
		if (tmp_con.level != last_level)
		{
			last_level = tmp_con.level;
			last_partner_id = 0;
		}

		if (!GetSubNodeValue(root_element, "partner_id", tmp_con.partner_id) || tmp_con.partner_id != last_partner_id + 1)
		{
			return -3;
		}
		last_partner_id = tmp_con.partner_id;

		if (tmp_con.partner_id <= 0 || tmp_con.partner_id >= PARTNER_NUM_MAX)
		{
			return -33333;
		}

		if (!GetSubNodeValue(root_element, "prof", tmp_con.profession))
		{
			return -33;
		}

		const PartnerBasicCfg* partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(tmp_con.partner_id);
		if (NULL == partner_cfg)
		{
			return -333;
		}

		std::string name;
		if (!GetSubNodeValue(root_element, "name", name))
		{
			return -4;
		}
		STRNCPY(tmp_con.name, name.c_str(), sizeof(tmp_con.name));
		
		if (!GetSubNodeValue(root_element, "maxhp", tmp_attr_con.max_hp) || tmp_attr_con.max_hp <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "maxmp", tmp_attr_con.max_mp) || tmp_attr_con.max_mp <= 0)
		{
			return -6;
		}

		const char * base_attr_reader[BATTLE_BASE_ATTR_NUM] = { "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int i = 0; i < BATTLE_BASE_ATTR_NUM; ++i)
		{
			if (!GetSubNodeValue(root_element, base_attr_reader[i], tmp_attr_con.basic_attr_list[i]) || tmp_attr_con.basic_attr_list[i] < 0)
			{
				return -(100 + i);
			}
		}

		const char * amen_attr_reader[] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "bisha_dmg", "jianren" };
		for (int i = 0; i < BATTLE_AMEN_ATTR_NUM && i < ARRAY_LENGTH(amen_attr_reader); ++i)
		{
			if (!GetSubNodeValue(root_element, amen_attr_reader[i], tmp_attr_con.amend_attr_list[i]) || tmp_attr_con.amend_attr_list[i] < 0)
			{
				return -(200 + i);
			}
		}

		const char * resi_attr_reader[] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int i = 0; i < ARRAY_LENGTH(resi_attr_reader) && i < ARRAY_LENGTH(tmp_attr_con.resis_attr_list); ++i)
		{
			if (!GetSubNodeValue(root_element, resi_attr_reader[i], tmp_attr_con.resis_attr_list[i]) || tmp_attr_con.resis_attr_list[i] < 0)
			{
				return -(300 + i);
			}
		}

		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_NULL;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_NULL;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_NULL;
		tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_NULL;
		switch (partner_cfg->crystal_item)
		{
		case PARTNER_CRYSTAL_ID_WATER:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_PURE;
			break; 
			
		case PARTNER_CRYSTAL_ID_FIRE:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_PURE;
			break; 
			
		case PARTNER_CRYSTAL_ID_WIND:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_PURE;
			break; 
			
		case PARTNER_CRYSTAL_ID_EARTH:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_PURE;
			break; 
			
		case PARTNER_CRYSTAL_ID_WATER_AND_FIRE:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			break; 
			
		case PARTNER_CRYSTAL_ID_EARTH_AND_WATER:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WATER - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			break; 
			
		case PARTNER_CRYSTAL_ID_WIND_AND_EARTH:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_EARTH - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			break; 
			
		case PARTNER_CRYSTAL_ID_FIRE_AND_WIND:
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_FIRE - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX; 
			tmp_attr_con.element_list[BATTLE_ATTR_ELEMENT_WIND - BATTLE_ATTR_ELEMENT_BEGIN] = PARTNER_CRYSTAL_MIX;
			break; 
		}

		for (int idx = 0; idx < ROBOT_ROLE_SKILL_MAX; ++idx)
		{
			RobotSkillCon & skill_con = tmp_attr_con.skill_list[idx];

			SNPRINTF(skill_reader, sizeof(skill_reader), "skill_id_%d", idx);
			if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_id) || (0 != skill_con.skill_id && NULL == SkillPool::Instance().GetSkillCfg(skill_con.skill_id)))
			{
				return -1000 - idx;
			}

			SNPRINTF(skill_reader, sizeof(skill_reader), "skill_level_%d", idx);
			if (!GetSubNodeValue(root_element, skill_reader, skill_con.skill_lv) || skill_con.skill_lv < 0 || skill_con.skill_lv > MAX_BATTLE_SKILL_LEVEL)
			{
				return -1100 - idx;
			}
		}

		m_partner_robot_list[tmp_con.use_way][tmp_con.level][tmp_con.partner_id] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitRobotLevelMap(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "group_id", group_id) || group_id < 0 || group_id >= ROBOT_LEVEL_GROUP_NUM_MAX)
		{
			return -1;
		}

		int min_level = 0;
		if (!GetSubNodeValue(dataElement, "min_level", min_level) || min_level < 0 || min_level > MAX_ROLE_LEVEL)
		{
			return -2;
		}

		int max_level = 0;
		if (!GetSubNodeValue(dataElement, "max_level", max_level) || max_level < min_level || max_level > MAX_ROLE_LEVEL)
		{
			return -3;
		}

		m_robot_level_map[group_id].insert(RobotLevelMap::value_type(min_level, max_level));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitTeamTypeRobotCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int team_type = 0;
		if (!GetSubNodeValue(dataElement, "team_type", team_type) || team_type < TEAM_TYPE_INVALID || team_type >= TEAM_TYPE_MAX)
		{
			return -1;
		}
		TeamTypeRobotCfg* cfg = new TeamTypeRobotCfg();
		m_team_type_robot_cfg[team_type] = cfg;
		cfg->team_type = team_type;
		
		int is_run = 0;
		if (!GetSubNodeValue(dataElement, "is_run_robot", is_run) || is_run < 0 || is_run > 1)
		{
			return -2;
		}
		cfg->is_run_robot = is_run > 0;

		int is_user_create_team = 0;
		if (!GetSubNodeValue(dataElement, "is_user_create_team", is_user_create_team) || is_user_create_team < 0 || is_user_create_team > 1)
		{
			return -3;
		}
		cfg->is_user_create_team = is_user_create_team > 0;

		if (!GetSubNodeValue(dataElement, "add_robot_time", cfg->add_robot_seconds) || cfg->add_robot_seconds <= 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "level_group_id", cfg->level_group_id) || cfg->level_group_id < 0 || cfg->level_group_id >= ROBOT_LEVEL_GROUP_NUM_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "battle_mode", cfg->battle_mode) || cfg->battle_mode < BATTLE_MODE_INVALID || cfg->battle_mode >= BATTLE_MODE_MAX)
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "add_min_interval", cfg->min_add_robot_interval_s) || cfg->min_add_robot_interval_s < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "add_max_interval", cfg->max_add_robot_interval_s) || cfg->max_add_robot_interval_s < cfg->min_add_robot_interval_s)
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "robot_limit", cfg->robot_limit))
		{
			return -8;
		}

		int robot_new = 0;
		if (!GetSubNodeValue(dataElement, "robot_new", robot_new))
		{
			return -9;
		}
		cfg->use_real_role_robot = robot_new > 0;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitNameCfg(TiXmlElement * RootElement)
{
	int ret = this->InitNameCfgImpl(RootElement, m_name_cfg.first_name[FEMALE_FIRST], "female_first");
	if (0 != ret)
	{
		printf("InitNameCfg female_first failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.middle_name[FEMALE_MIDDLE], "female_middle");
	if (0 != ret)
	{
		printf("InitNameCfg female_middle failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.last_name[FEMALE_LAST], "female_last");
	if (0 != ret)
	{
		printf("InitNameCfg female_last failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.first_name[MALE_FIRST], "male_first");
	if (0 != ret)
	{
		printf("InitNameCfg male_first failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.middle_name[MALE_MIDDLE], "male_middle");
	if (0 != ret)
	{
		printf("InitNameCfg male_middle failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.last_name[MALE_LAST], "male_last");
	if (0 != ret)
	{
		printf("InitNameCfg male_last failed! ret=%d\n", ret);
		return ret;
	}

	return 0;
}

int RobotConfig::InitNameCfgImpl(TiXmlElement * RootElement, NameCfg::NameList & name_list, const char * sheet_name)
{
	if (NULL == sheet_name) return -99;

	GameName game_name;
	bool ret = true;

	{
		TiXmlElement *role_element = RootElement->FirstChildElement(sheet_name);
		if (NULL == role_element)
		{
			return -1000;
		}

		// 允许没有数据
		TiXmlElement *data_element = role_element->FirstChildElement("data");
		if (NULL == data_element)
		{
			return 0;
		}

		bool has_reserved = false;
		while (NULL != data_element)
		{
			memset(game_name, 0, sizeof(game_name));
			int id = 0;
			// 不允许个数大于1000
			if (!GetSubNodeValue(data_element, "id", id) || 0 >= id || MAX_NAME_CFG_NUM <= id)
			{
				return -1;
			}

			if (!GetSubNodeValue(data_element, "name", game_name) || NameFilter::Instance().HasSensitiveWord(sizeof(game_name), game_name))
			{
				ret = false;
				printf("NameFilter check pass fail on sheet[%s]\tid[%d]\n", sheet_name, id);
			}

			static NameCfg::Name name;
			name.name_id = id;
			F_STRNCPY(name.name_str, game_name, sizeof(name.name_str));

			name_list.name_str_list.push_back(name);
			if (!has_reserved && name_list.name_str_list.size() > 10)
			{
				has_reserved = true;
				name_list.name_str_list.reserve(200);
			}

			data_element = data_element->NextSiblingElement();
		}
	}

#ifndef _DEBUG
	if (!ret)
	{
		assert(0);
	}
#endif

	return 0;
}

int RobotConfig::InitRoleNameCfg(TiXmlElement * RootElement)
{
	int ret = this->InitNameCfgImpl(RootElement, m_role_name_cfg.first_name[FEMALE_FIRST], "robot_female_first");
	if (0 != ret)
	{
		printf("InitRoleNameCfg robot_female_first failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_role_name_cfg.first_name[MALE_FIRST], "robot_male_first");
	if (0 != ret)
	{
		printf("InitRoleNameCfg robot_male_first failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_role_name_cfg.last_name[FEMALE_MIDDLE], "robot_female_last");
	if (0 != ret)
	{
		printf("InitRoleNameCfg robot_female_last failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_role_name_cfg.last_name[MALE_FIRST], "robot_male_last");
	if (0 != ret)
	{
		printf("InitRoleNameCfg robot_male_last failed! ret=%d\n", ret);
		return ret;
	}

	return 0;
}

int RobotConfig::InitAutoTalkCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	int last_talk_groups_id = 0;
	int last_talk_id = 0;
	while (NULL != dataElement)
	{
		AutoTalkCfg node_cfg;
		if (!GetSubNodeValue(dataElement, "talk_groups", node_cfg.talk_groups_id) || node_cfg.talk_groups_id <= 0)
		{
			return -1;
		}
		if (last_talk_groups_id != node_cfg.talk_groups_id)
		{
			last_talk_groups_id = node_cfg.talk_groups_id;
			last_talk_id = 0;
		}
		if (!GetSubNodeValue(dataElement, "talk_id", node_cfg.talk_id) || node_cfg.talk_id <= 0 || node_cfg.talk_id >= MAX_AUTO_TALK_TALK_GROUPS_BASE_NUM ||
			node_cfg.talk_id != last_talk_id + 1)
		{
			return -2;
		}
		int talk_key = node_cfg.talk_groups_id * MAX_AUTO_TALK_TALK_GROUPS_BASE_NUM + node_cfg.talk_id;
		if(m_auto_talk_map.end() != m_auto_talk_map.find(talk_key))
		{
			return -3;
		}
		last_talk_id = node_cfg.talk_id;
		if (!GetSubNodeValue(dataElement, "interval", node_cfg.interval) || node_cfg.interval <= 0)
		{
			return -4;
		}
		

		m_auto_talk_groups_set.insert(node_cfg.talk_groups_id);
		m_auto_talk_map[talk_key] = node_cfg;
		dataElement = dataElement->NextSiblingElement();
	}
	if (m_auto_talk_groups_set.size() != MAX_AUTO_TALK_TALK_GROUPS_NUM)		//角色有5个,需要最少5个话本  暂时先固定5组.之后有时间再优化
	{
		return -999;
	}

	return 0;
}

int RobotConfig::InitMissionCreatCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "robot_show_days", m_mission_creat_cfg.robot_show_days)
		|| 0 > m_mission_creat_cfg.robot_show_days)
	{
		return -1;
	}
	if (!GetSubNodeValue(dataElement, "task_robot_refresh_cd", m_mission_creat_cfg.task_robot_refresh_cd)
		|| 100 > m_mission_creat_cfg.task_robot_refresh_cd)
	{
		return -2;
	}
	if (!GetSubNodeValue(dataElement, "final_task_id", m_mission_creat_cfg.final_task_id)
		|| NULL == TASKPOOL->GetTaskCfgByID(m_mission_creat_cfg.final_task_id))
	{
		return -3;
	}
	if (!GetSubNodeValue(dataElement, "scene_id", m_mission_creat_cfg.scene_id)
		|| !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(m_mission_creat_cfg.scene_id))
	{
		return -4;
	}
	if (!GetSubNodeValue(dataElement, "pos_x", m_mission_creat_cfg.pos_x)
		|| 0 > m_mission_creat_cfg.pos_x)
	{
		return -5;
	}
	if (!GetSubNodeValue(dataElement, "pos_y", m_mission_creat_cfg.pos_y)
		|| 0 > m_mission_creat_cfg.pos_y)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "invite_task", m_other_cfg.start_capacity_target_task_id))
	{
		return -7;
	}

	if (!GetSubNodeValue(dataElement, "invite_count_down", m_other_cfg.capacity_target_time) || m_other_cfg.capacity_target_time < 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(dataElement, "invite_robot_level", m_other_cfg.capacity_target_robot_level))
	{
		return -9;
	}

	if (!GetSubNodeValue(dataElement, "invite_robot_level", m_other_cfg.capacity_target_robot_level))
	{
		return -10;
	}

	if (::ReadItemConfigDataList(dataElement, "victor_reward", m_other_cfg.capacity_target_win_reward) != 0)
	{
		return -11;
	}

	if (::ReadItemConfigDataList(dataElement, "fail_reward", m_other_cfg.capacity_target_lose_reward) != 0)
	{
		return -12;
	}

	return 0;
}

int RobotConfig::InitRangerCreatCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id)
			|| !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -1;
		}
		RangerRobotCreatCfg &cfg_info = m_ranger_creat_map[scene_id];
		cfg_info.scene_id = scene_id;

		if (!GetSubNodeValue(root_element, "level", cfg_info.level)
			|| 0 >= cfg_info.level)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "interval_time", cfg_info.base_interval_time_s)
			|| 0 >= cfg_info.base_interval_time_s)
		{
			return -3;
		}
		cfg_info.base_interval_time_s = cfg_info.base_interval_time_s * SECOND_PER_HOUR;

		if (!GetSubNodeValue(root_element, "robot_num", cfg_info.robot_num)
			|| 0 >= cfg_info.robot_num)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RobotConfig::InitStaticCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id)
			|| !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -1;
		}
		StaticRobotCreatCfg &cfg_info = m_static_creat_map[scene_id];
		cfg_info.scene_id = scene_id;

		if (!GetSubNodeValue(root_element, "robot_num", cfg_info.robot_num)
			|| 0 >= cfg_info.robot_num)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level", cfg_info.level)
			|| 0 >= cfg_info.level)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "interval_time", cfg_info.range_interval_time_s)
			|| 0 >= cfg_info.range_interval_time_s)
		{
			return -4;
		}
		cfg_info.range_interval_time_s = cfg_info.range_interval_time_s * SECOND_PER_HOUR;


		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RobotConfig::InitStaticCreatPosiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		int scene_id = 0;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id)
			|| !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -1;
		}

		StaticRobotPosiCfg cfg_info;
		cfg_info.scene_id = scene_id;

		if (!GetSubNodeValue(root_element, "pos_x", cfg_info.pos_x)
			|| 0 > cfg_info.pos_x)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "pos_y", cfg_info.pos_y)
			|| 0 > cfg_info.pos_y)
		{
			return -3;
		}

		cfg_info.posi_index = m_static_creat_posi_map[scene_id].size();
		m_static_creat_posi_map[scene_id].push_back(cfg_info);

		root_element = root_element->NextSiblingElement();
	}

	//检测
	StaticCreatMap::iterator static_it = m_static_creat_map.begin();
	for (; static_it != m_static_creat_map.end(); static_it++)
	{
		if (m_static_creat_posi_map.find(static_it->first) == m_static_creat_posi_map.end())
		{
			return -200;
		}
	}
	return 0;
}

int RobotConfig::InitCapacityTargetValueCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -10000;

	while (NULL != root_element)
	{
		if (!GetSubNodeValue(root_element, "robot_rank", m_other_cfg.capacity_target_robot_value))  //只要最后一个
		{
			return -1;
		}


		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RobotConfig::InitOtherCfg(TiXmlElement* RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	if (!GetSubNodeValue(dataElement, "change_rate", m_other_cfg.robot_pet_mutate_rate) || m_other_cfg.robot_pet_mutate_rate < 0)
	{
		return -1;
	}

	if (!GetSubNodeValue(dataElement, "robot_weak_percent", m_other_cfg.jjc_robot_weaken_attr_percent) || m_other_cfg.jjc_robot_weaken_attr_percent < 0 || m_other_cfg.jjc_robot_weaken_attr_percent >= 100)
	{
		return -2;
	}

	if (!GetSubNodeValue(dataElement, "off_auto_talk", m_other_cfg.off_auto_talk_level) || m_other_cfg.off_auto_talk_level < 0 || m_other_cfg.off_auto_talk_level >= MAX_ROLE_LEVEL)
	{
		return -3;
	}

	if (!GetSubNodeValue(dataElement, "match_level_down", m_other_cfg.match_level_down))
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "match_level_top", m_other_cfg.match_level_up))
	{
		return -4;
	}

	if (!GetSubNodeValue(dataElement, "robot_name", m_other_cfg.real_role_robot_suffix))
	{
		return -5;
	}

	if (::ReadItemConfigData2(dataElement, "help_reward", m_other_cfg.real_role_robot_rewards) != 0)
	{
		return -6;
	}

	if (!GetSubNodeValue(dataElement, "double_robot", m_other_cfg.match_real_role_robot_level))
	{
		return -7;
	}

	return 0;
}

int PlatformBattlePartnerRobotCfg::GetCapability() const
{
	int capability = 0;

	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == basic_cfg)
	{
		return capability;
	}

	switch (basic_cfg->profession)
	{
		case PARTNER_PROF_PUGILIST:
		case PARTNER_PROF_SWORDMAN:
		case PARTNER_PROF_ROMAHAWK:
		case PARTNER_PROF_MARKSMAN:
			{
				capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.71 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
						    attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 0.3 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
						    attr_list[BATTLE_ATTR_MAX_MP] * 0.06 + attr_list[BATTLE_ATTR_CRITICAL] * 1.3 + attr_list[BATTLE_ATTR_HIT] * 1.1 +
						    attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 1.1 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.12 +
						    attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 1.3 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
						    (attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
						     attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
			}
			break;
		case PARTNER_PROF_HORSEMAN:
			{
				capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.71 + attr_list[BATTLE_ATTR_DEFENSE] * 0.5 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
						    attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 0.3 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
						    attr_list[BATTLE_ATTR_MAX_MP] * 0.06 + attr_list[BATTLE_ATTR_CRITICAL] * 1.3 + attr_list[BATTLE_ATTR_HIT] * 1.1 +
						    attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 1.3 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.12 +
						    attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 1.3 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
						    (attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
						     attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
			}
			break;
		case PARTNER_PROF_MAGICIAN:
			{
				capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.1 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
						    attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 1.2 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
						    attr_list[BATTLE_ATTR_MAX_MP] * 0.18 + attr_list[BATTLE_ATTR_CRITICAL] * 0.2 + attr_list[BATTLE_ATTR_HIT] * 0.2 +
						    attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.2 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.8 +
						    attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 10 * 0.2 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
						    (attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
						     attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
			}
			break;
		case PARTNER_PROF_MINISTER:
		case PARTNER_PROF_SORCERER:
			{
				capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.3 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
						    attr_list[BATTLE_ATTR_RECOVERY] * 1.2 + attr_list[BATTLE_ATTR_MENTAL] * 1 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
						    attr_list[BATTLE_ATTR_MAX_MP] * 0.18 + attr_list[BATTLE_ATTR_CRITICAL] * 0.2 + attr_list[BATTLE_ATTR_HIT] * 0.2 +
						    attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.2 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.8 +
						    attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.2 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
						    (attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
						     attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
			}
			break;
		default:
			break;
	}

	return 0;
}

int RobotPartnerCfg::GetCapability(Attribute(ARG_IN &attr_list)[BATTLE_ATTR_MAX]) const
{
	int capability = 0;

	const PartnerBasicCfg * basic_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
	if (NULL == basic_cfg)
	{
		return capability;
	}

	switch (basic_cfg->profession)
	{
	case PARTNER_PROF_PUGILIST:
	case PARTNER_PROF_SWORDMAN:
	case PARTNER_PROF_ROMAHAWK:
	case PARTNER_PROF_MARKSMAN:
		{
			capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.71 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
				attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 0.3 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
				attr_list[BATTLE_ATTR_MAX_MP] * 0.06 + attr_list[BATTLE_ATTR_CRITICAL] * 1.3 + attr_list[BATTLE_ATTR_HIT] * 1.1 +
				attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 1.1 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.12 +
				attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 1.3 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
				(attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
					attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
		}
		break;
	case PARTNER_PROF_HORSEMAN:
		{
			capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.71 + attr_list[BATTLE_ATTR_DEFENSE] * 0.5 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
				attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 0.3 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
				attr_list[BATTLE_ATTR_MAX_MP] * 0.06 + attr_list[BATTLE_ATTR_CRITICAL] * 1.3 + attr_list[BATTLE_ATTR_HIT] * 1.1 +
				attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 1.3 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.12 +
				attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 1.3 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
				(attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
					attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
		}
		break;
	case PARTNER_PROF_MAGICIAN:
		{
			capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.1 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
				attr_list[BATTLE_ATTR_RECOVERY] * 0.6 + attr_list[BATTLE_ATTR_MENTAL] * 1.2 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
				attr_list[BATTLE_ATTR_MAX_MP] * 0.18 + attr_list[BATTLE_ATTR_CRITICAL] * 0.2 + attr_list[BATTLE_ATTR_HIT] * 0.2 +
				attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.2 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.8 +
				attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 10 * 0.2 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
				(attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
					attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
		}
		break;
	case PARTNER_PROF_MINISTER:
	case PARTNER_PROF_SORCERER:
		{
			capability = (int)((attr_list[BATTLE_ATTR_ATTACK] * 0.3 + attr_list[BATTLE_ATTR_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_AGILITY] * 0.45 +
				attr_list[BATTLE_ATTR_RECOVERY] * 1.2 + attr_list[BATTLE_ATTR_MENTAL] * 1 + attr_list[BATTLE_ATTR_MAX_HP] * 0.08 +
				attr_list[BATTLE_ATTR_MAX_MP] * 0.18 + attr_list[BATTLE_ATTR_CRITICAL] * 0.2 + attr_list[BATTLE_ATTR_HIT] * 0.2 +
				attr_list[BATTLE_ATTR_DODGE] * 1.1 + attr_list[BATTLE_ATTR_COUNTER_ATTACK] * 0.2 + attr_list[BATTLE_ATTR_MAGIC_ATTACK] * 0.8 +
				attr_list[BATTLE_ATTR_MAGIC_DEFENSE] * 0.45 + attr_list[BATTLE_ATTR_CRITICAL_INC_VALUE] * 0.2 + attr_list[BATTLE_ATTR_CRITICAL_DEC_VALUE] * 1.1 +
				(attr_list[BATTLE_ATTR_ANTI_POISON] + attr_list[BATTLE_ATTR_ANTI_FREEZE] + attr_list[BATTLE_ATTR_ANTI_PETRIFY] + attr_list[BATTLE_ATTR_ANTI_CHAOS] +
					attr_list[BATTLE_ATTR_ANTI_DRUNK] * 0.5)) * 4);
		}
		break;
	default:
		break;
	}

	return capability;
}
