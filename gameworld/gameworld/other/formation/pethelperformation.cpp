#include "pethelperformation.hpp"
#include "protocol/msgformation.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/event/eventhandler.hpp"
#include "other/pet/petconfig.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/realmbreakconfig.hpp"

PetHelperFormation::PetHelperFormation() : m_mgr(NULL)
{

}

PetHelperFormation::~PetHelperFormation()
{

}

void PetHelperFormation::Init(RoleModuleManager* mgr, const PetHelperFormationParam& param)
{
	m_mgr = mgr;
	m_param = param;
}

void PetHelperFormation::GetInitParam(ARG_OUT PetHelperFormationParam* param)
{
	*param = m_param;
}

void PetHelperFormation::OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req)
{
	if (!req->CheckDuplicate())
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!req->CheckStanbyPetNum())
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (req->system_type < 0 || req->system_type >= PET_HELPER_FORMATION_TYPE_MAX)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	switch (req->system_type)
	{
	case PET_HELPER_FORMATION_CLOUD_ARENA:
		{
			this->OnRoleSetCloudArenaFormation(req);
		}
		break;

	case PET_HELPER_FORMATION_COURAGE_CHALLENGE:
		{
			this->OnRoleSetCourageChallengeFormation(req);
		}
		break;

	case PET_HELPER_FORMATION_NETHERWORLD:
	case PET_HELPER_FORMATION_COLOSSEUM:
	case PET_HELPER_FORMATION_ENDLESS_TOWER:
	case PET_HELPER_FORMATION_GUILD_BOSS:
	case PET_HELPER_FORMATION_WORLD_ARENA:
	case PET_HELPER_FORMATION_YAO_XING_SHI_LIAN:
		{
			this->OnRoleSetFormationCommon(req);
		}
		break;
	case PET_HELPER_FORMATION_REALM_BREAK_FIGHT:
		{
			this->OnRoleSetRealmBreakFightFormation(req);
		}
		break; 

	default:
		{
			this->OnRoleSetFormationCommon(req);
		}
		break;
	}

	UNSTD_STATIC_CHECK(9 == PET_HELPER_FORMATION_TYPE_MAX);

	EventHandler::Instance().OnPetHelperFormationChanged(m_mgr->GetRole(), req->system_type);
}

bool PetHelperFormation::IsFormationValid(int system_type)
{
	if (system_type < 0 || system_type >= PET_HELPER_FORMATION_TYPE_MAX) return false;

	return m_param.formation_list[system_type].is_valid_formation > 0;
}

void PetHelperFormation::OnRoleQueryFormation(int system_type)
{
	if (system_type < 0 || system_type >= PET_HELPER_FORMATION_TYPE_MAX) return;

	if (!this->IsFormationValid(system_type))
	{
		this->SetInitialFormation(m_param.formation_list[system_type]);
	}

	this->SendFormation(system_type);
}

bool PetHelperFormation::AssembleBattleData(int system_type, ARG_OUT RoleBattleData& battle_data, bool is_use_system_food)
{
	if (system_type < 0 || system_type >= PET_HELPER_FORMATION_TYPE_MAX) return false;
	if (!this->IsFormationValid(system_type)) return false;

	PetHelperFormationInfo& info = m_param.formation_list[system_type];

	battle_data.Reset();
	
	m_mgr->CollectRoleBattleData(&battle_data, false, true, false);

	battle_data.pet_num = 0;
	battle_data.partner_num = 0;
	battle_data.pet_helper_num = 0;

	int role_row = 0;
	if (PET_HELPER_FORMATION_GRID_ROLE == info.grid_list[7].character_type)
	{
		role_row = 1;
	}
	battle_data.role_stand_row = role_row;

	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		const PetHelperFormationGrid & curr = info.grid_list[i];
		if (PET_HELPER_FORMATION_GRID_INVALID == info.grid_list[i].character_type ||
			PET_HELPER_FORMATION_GRID_ROLE == info.grid_list[i].character_type)
		{
			continue;
		}
		else if (PET_HELPER_FORMATION_GRID_PET == info.grid_list[i].character_type)
		{
			int pet_id = info.grid_list[i].character_id;
			long long unique_id = info.grid_list[i].unique_id;
			int pet_index = -1;
			if (NULL == m_mgr->GetPet()->GetPetParam(pet_id, unique_id, &pet_index))
			{
				continue;
			}

			if (i == 2 || i == 7)
			{
				PetBattleData& pet_data = battle_data.pet_character_data[0];
				if (!m_mgr->GetPet()->GetBattleCharacterData(pet_index, &pet_data, false))
				{
					continue;
				}
				battle_data.pet_num = 1;
				battle_data.fight_pet_idx = 0;
				pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
				pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;

				if (pet_data.auto_move_type != battle_data.auto_second_move_type ||
					pet_data.auto_skill_id != battle_data.auto_second_skill_id ||
					pet_data.auto_skill_level != battle_data.auto_second_skill_level)
				{
					pet_data.auto_move_type = m_param.formation_list[system_type].battle_auto_second_move_type;
					pet_data.auto_skill_id = m_param.formation_list[system_type].battle_auto_second_skill_id;
					pet_data.auto_skill_level = m_param.formation_list[system_type].battle_auto_second_skill_level;
				}
			}
			else if (battle_data.pet_helper_num < ARRAY_LENGTH(battle_data.pet_helper_character_data))
			{
				PetHelperBattleData& pet_helper_data = battle_data.pet_helper_character_data[battle_data.pet_helper_num];
				if (!m_mgr->GetPet()->GetBattleCharacterData(pet_index, &pet_helper_data.pet_data, false))
				{
					continue;
				}
				pet_helper_data.pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
				pet_helper_data.pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;
				pet_helper_data.pet_data.battle_data.character_type = BATTLE_CHARACTER_TYPE_PET_SHADOW;
				pet_helper_data.battle_row = i / ROW_CHARACTER_NUM;
				pet_helper_data.battle_column = i % ROW_CHARACTER_NUM;
				battle_data.pet_helper_num += 1;
			}
		}
		else if (PET_HELPER_FORMATION_GRID_PARTNER == info.grid_list[i].character_type)
		{
			if (battle_data.partner_num >= ARRAY_LENGTH(battle_data.partner_character_data))
			{
				continue;
			}

			PartnerBattleData& partner_data = battle_data.partner_character_data[battle_data.partner_num];
			if (!m_mgr->GetPartner()->GetBattleCharacterData(info.grid_list[i].character_id, &partner_data, NULL, 0, false))
			{
				continue;
			}
			partner_data.battle_row = i / ROW_CHARACTER_NUM;
			partner_data.battle_column = i % ROW_CHARACTER_NUM;
			battle_data.partner_num += 1;
		}
	}

	for (int i = 0; i < ARRAY_LENGTH(info.standby_list); ++i)
	{
		if (battle_data.pet_num >= ARRAY_LENGTH(battle_data.pet_character_data))
		{
			continue;
		}

		const StandbyPetData & curr = info.standby_list[i];
		int pet_id = info.standby_list[i].pet_id;
		long long unique_id = info.standby_list[i].pet_unique_id;
		int pet_index = -1;
		if (NULL == m_mgr->GetPet()->GetPetParam(pet_id, unique_id, &pet_index))
		{
			continue;
		}

		PetBattleData& pet_data = battle_data.pet_character_data[battle_data.pet_num];
		if (!m_mgr->GetPet()->GetBattleCharacterData(pet_index, &pet_data, false))
		{
			continue;
		}
		pet_data.pet_exclusive_skill_disable_flag = curr.pet_exclusive_skill_disable_flag;
		pet_data.lian_yao_exclusive_skill_disable_flag = curr.lian_yao_exclusive_skill_disable_flag;
		battle_data.pet_num += 1;
	}

	battle_data.auto_first_move_type = m_param.battle_auto_first_move_type;
	battle_data.auto_first_skill_id = m_param.battle_auto_first_skill_id;
	battle_data.auto_first_skill_level = m_param.battle_auto_first_skill_level;
	battle_data.auto_second_move_type = m_param.formation_list[system_type].battle_auto_second_move_type;
	battle_data.auto_second_skill_id = m_param.formation_list[system_type].battle_auto_second_skill_id;
	battle_data.auto_second_skill_level = m_param.formation_list[system_type].battle_auto_second_skill_level;

	if (is_use_system_food)
	{
		m_mgr->MinusOriginFoodAttr(&battle_data.role_character_data);
		m_mgr->AddFoodAttr(&battle_data.role_character_data);
	}

	return true;
}

bool PetHelperFormation::CollectBattleData(int battle_mode, ARG_OUT RoleBattleData& battle_data, bool is_use_system_food)
{
	//cloud_arena因历史原因已有其他接口，不需要加在这里

	if ((BATTLE_MODE_COURAGE_CHALLENGE == battle_mode || BATTLE_MODE_NEW_COURAGE_CHALLENGE == battle_mode) && this->IsFormationValid(PET_HELPER_FORMATION_COURAGE_CHALLENGE))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_COURAGE_CHALLENGE]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_COURAGE_CHALLENGE, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_NETHER_WORLD_PURGATORY == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_NETHERWORLD))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_NETHERWORLD]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_NETHERWORLD, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_SINGLE_ARENA == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_COLOSSEUM))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_COLOSSEUM]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_COLOSSEUM, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_ENDLESS_TOWER == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_ENDLESS_TOWER))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_ENDLESS_TOWER]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_ENDLESS_TOWER, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_GUILD_BOSS == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_GUILD_BOSS))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_GUILD_BOSS]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_GUILD_BOSS, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_WORLD_ARENA == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_WORLD_ARENA))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_WORLD_ARENA]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_WORLD_ARENA, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_YAO_XING_SHI_LIAN == battle_mode && this->IsFormationValid(PET_HELPER_FORMATION_YAO_XING_SHI_LIAN))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_YAO_XING_SHI_LIAN]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_YAO_XING_SHI_LIAN, battle_data, is_use_system_food);
	}
	else if (BATTLE_MODE_REALM_BREAK_FIGHT == battle_mode &&  this->IsFormationValid(PET_HELPER_FORMATION_REALM_BREAK_FIGHT))
	{
		this->CheckFormationRemoveInvalidGrid(m_param.formation_list[PET_HELPER_FORMATION_REALM_BREAK_FIGHT]);
		return this->AssembleBattleData(PET_HELPER_FORMATION_REALM_BREAK_FIGHT, battle_data, is_use_system_food);
	}

	UNSTD_STATIC_CHECK(9 == PET_HELPER_FORMATION_TYPE_MAX);

	return false;
}

void PetHelperFormation::SynAutoSkill(FormationAutoInfo & info, int pet_id)
{
	m_param.battle_auto_first_move_type = (char)info.battle_auto_first_move_type;
	m_param.battle_auto_first_skill_level = (char)info.battle_auto_first_skill_level;
	m_param.battle_auto_first_skill_id = (UInt16)info.battle_auto_first_skill_id;

	for (int i = 0; i < PET_HELPER_FORMATION_TYPE_MAX; i++)
	{
		for (int grid = 0; grid < SIDE_CHARACTER_NUM; grid++)
		{
			if (!m_param.formation_list[i].grid_list[grid].IsValid()) continue;

			if (grid == 2 || grid == 7)
			{
				if (m_param.formation_list[i].grid_list[grid].character_type == PET_HELPER_FORMATION_GRID_PET && m_param.formation_list[i].grid_list[grid].character_id == pet_id)
				{
					m_param.formation_list[i].battle_auto_second_move_type = (char)info.battle_auto_second_move_type;
					m_param.formation_list[i].battle_auto_second_skill_level = (char)info.battle_auto_second_skill_level;
					m_param.formation_list[i].battle_auto_second_skill_id = (UInt16)info.battle_auto_second_skill_id;
				}
			}
		}
	}
	
	//for (int sys_type = 0; sys_type < PET_HELPER_FORMATION_TYPE_MAX; sys_type++)
	//{
	//	this->SendFormation(sys_type);
	//}
}

void PetHelperFormation::SynBattleAutoSkill(bool is_first_move, short move_type, UInt16 skill_id, short skill_level, int pet_id, long long pet_unique_id)
{
	if (is_first_move)
	{
		m_param.battle_auto_first_move_type = (char)move_type;
		m_param.battle_auto_first_skill_level = (char)skill_level;
		m_param.battle_auto_first_skill_id = (UInt16)skill_id;
	}
	else
	{
		for (int i = 0; i < PET_HELPER_FORMATION_TYPE_MAX; i++)
		{
			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_param.formation_list[i].grid_list) == 10);

			PetHelperFormationGrid& grid2 = m_param.formation_list[i].grid_list[2];
			PetHelperFormationGrid& grid7 = m_param.formation_list[i].grid_list[7];
			if (this->CheckGridIsPet(grid2, pet_id, pet_unique_id) 
				|| this->CheckGridIsPet(grid7, pet_id, pet_unique_id))
			{
				m_param.formation_list[i].battle_auto_second_move_type = (char)move_type;
				m_param.formation_list[i].battle_auto_second_skill_level = (char)skill_level;
				m_param.formation_list[i].battle_auto_second_skill_id = (UInt16)skill_id;
			}			
		}
	}

	this->SendFormation(PET_HELPER_FORMATION_REALM_BREAK_FIGHT);
	//for (int sys_type = 0; sys_type < PET_HELPER_FORMATION_TYPE_MAX; sys_type++)
	//{
	//	this->SendFormation(sys_type);
	//}
	//m_mgr->GetRole()->SendBattleAutoInfo();
}

void PetHelperFormation::GmRandomFormation(int system_type)
{
	if (system_type < 0 || system_type >= PET_HELPER_FORMATION_TYPE_MAX) return;

	static Protocol::CSSetPetHelperFormation req;
	memset(req.grid_list, 0, sizeof(req.grid_list));
	memset(req.standby_list, 0, sizeof(req.standby_list));
	req.system_type = system_type;

	static const int ROLE_GRID[2] = { 2, 7 };
	int role_grid = ROLE_GRID[rand() % 2];
	req.grid_list[role_grid].id = m_mgr->GetRoleUID();
	req.grid_list[role_grid].type = PET_HELPER_FORMATION_GRID_ROLE;

	std::vector<int> partner_list = m_mgr->GetPartner()->GetActiveParnerList();
	int PARNER_COLUMN[ROW_CHARACTER_NUM - 1] = { 1, 3, 0, 4 };
	RandomShuffle(PARNER_COLUMN, ARRAY_LENGTH(PARNER_COLUMN));
	if (!partner_list.empty())
	{
		RandomShuffle(&partner_list[0], partner_list.size());
	}
	for (int i = 0; i < ARRAY_LENGTH(PARNER_COLUMN) && i < partner_list.size(); ++i)
	{
		int grid = PARNER_COLUMN[i];
		if (rand() % 2 > 0)
		{
			grid = (grid + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
		}
		req.grid_list[grid].id = partner_list[i];
		req.grid_list[grid].type = PET_HELPER_FORMATION_GRID_PARTNER;
	}

	int pet_count = m_mgr->GetPet()->GetPetCount();
	std::vector<int> permuatation = RandomPermutation(0, pet_count);

	static const int PET_COLUMN[ROW_CHARACTER_NUM] = { 2, 1, 3, 0, 4 };
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(PET_COLUMN) == 5);
	int standby_num = 0;
	for (int i = 0; i < pet_count && standby_num < ARRAY_LENGTH(req.standby_list) - 1; ++i)
	{
		if (i < ARRAY_ITEM_COUNT(PET_COLUMN))
		{
			int grid = PET_COLUMN[i];
			if (req.grid_list[grid].id > 0)
			{
				grid = (grid + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
			}

			const ItemGridData* pet_data = m_mgr->GetPet()->GetPetGridData(permuatation[i]);
			if (NULL == pet_data) continue;

			req.grid_list[grid].id = pet_data->item_id;
			req.grid_list[grid].unique_id = pet_data->item_unique_id;
			req.grid_list[grid].type = PET_HELPER_FORMATION_GRID_PET;
		}
		else
		{
			const ItemGridData* pet_data = m_mgr->GetPet()->GetPetGridData(permuatation[i]);
			if (NULL == pet_data) continue;

			req.standby_list[standby_num].pet_id = pet_data->item_id;
			req.standby_list[standby_num].unique_id = pet_data->item_unique_id;
			standby_num += 1;
		}
	}

	this->OnRoleSetFormation(&req);
}

void PetHelperFormation::OnRoleSetCloudArenaFormation(Protocol::CSSetPetHelperFormation* req)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_mgr->NoticeNum(errornum::EN_CLOUD_ARENA_ONLY_IN_CROSS);
		return;
	}

	if (CloudArena::Instance().OnRoleSetFormation(req, m_mgr->GetRole()))
	{
		this->AssembleFormationFromProtocol(req);
	}
}

void PetHelperFormation::OnRoleSetCourageChallengeFormation(Protocol::CSSetPetHelperFormation* req)
{
	int pet_helper_num = m_mgr->GetCourageChallenge()->GetPetHelperNum();
	int pet_helper_num_in_formation = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(req->grid_list); ++i)
	{
		if (2 == i || 7 == i) continue;
		if (PET_HELPER_FORMATION_GRID_PET != req->grid_list[i].type) continue;

		pet_helper_num_in_formation += 1;
	}

	if (pet_helper_num_in_formation > pet_helper_num)
	{
		m_mgr->NoticeNum(errornum::EN_PET_HELPER_NUM_OVER_LIMIT);
		return;
	}

	this->AssembleFormationFromProtocol(req);
	this->SendFormation(PET_HELPER_FORMATION_COURAGE_CHALLENGE);
}

void PetHelperFormation::OnRoleSetFormationCommon(Protocol::CSSetPetHelperFormation* req)
{
	int pet_helper_num_in_formation = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(req->grid_list); ++i)
	{
		if (2 == i || 7 == i) continue;
		if (PET_HELPER_FORMATION_GRID_PET != req->grid_list[i].type) continue;

		pet_helper_num_in_formation += 1;
	}

	if (pet_helper_num_in_formation > 5)
	{
		m_mgr->NoticeNum(errornum::EN_PET_HELPER_NUM_OVER_LIMIT);
		return;
	}

	this->AssembleFormationFromProtocol(req);
	this->SendFormation(req->system_type);
}

void PetHelperFormation::OnRoleSetRealmBreakFightFormation(Protocol::CSSetPetHelperFormation * req)
{
	int pet_helper_num = LOGIC_CONFIG->GetRealmBreakConfig()->GetFightCanPetNum(m_mgr->GetCreateDays());
	int pet_helper_num_in_formation = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(req->grid_list); ++i)
	{
		if (2 == i || 7 == i) continue;
		if (PET_HELPER_FORMATION_GRID_PET != req->grid_list[i].type) continue;

		pet_helper_num_in_formation += 1;
	}

	if (pet_helper_num_in_formation > pet_helper_num)
	{
		m_mgr->NoticeNum(errornum::EN_PET_HELPER_NUM_OVER_LIMIT);
		return;
	}

	this->AssembleFormationFromProtocol(req);
	this->SendFormation(PET_HELPER_FORMATION_REALM_BREAK_FIGHT);
}

void PetHelperFormation::AssembleFormationFromProtocol(Protocol::CSSetPetHelperFormation* req)
{
	//此函数不做任何合法性判断，需要上层逻辑自行判断
	PetHelperFormationInfo& formation = m_param.formation_list[req->system_type];
	formation.is_valid_formation = 1;
	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		formation.grid_list[i].character_id = req->grid_list[i].id;
		formation.grid_list[i].character_type = req->grid_list[i].type;
		formation.grid_list[i].unique_id = req->grid_list[i].unique_id;
		formation.grid_list[i].reserve_ch = 0;
		formation.grid_list[i].pet_exclusive_skill_disable_flag = req->grid_list[i].pet_exclusive_skill_disable_flag;
		formation.grid_list[i].lian_yao_exclusive_skill_disable_flag = req->grid_list[i].lian_yao_exclusive_skill_disable_flag;
	}
	UNSTD_STATIC_CHECK(
		ARRAY_ITEM_COUNT(req->grid_list) == SIDE_CHARACTER_NUM &&
		ARRAY_ITEM_COUNT(formation.grid_list) == SIDE_CHARACTER_NUM
	);

	for (int i = 0; i < FORMATION_STANDBY_PET_NUM; ++i)
	{
		formation.standby_list[i].pet_id = req->standby_list[i].pet_id;
		formation.standby_list[i].pet_unique_id = req->standby_list[i].unique_id;
		formation.standby_list[i].pet_exclusive_skill_disable_flag = req->standby_list[i].pet_exclusive_skill_disable_flag;
		formation.standby_list[i].lian_yao_exclusive_skill_disable_flag = req->standby_list[i].lian_yao_exclusive_skill_disable_flag;

	}
	UNSTD_STATIC_CHECK(
		ARRAY_ITEM_COUNT(req->standby_list) == FORMATION_STANDBY_PET_NUM &&
		ARRAY_ITEM_COUNT(formation.standby_list) == FORMATION_STANDBY_PET_NUM
	);

	m_param.battle_auto_first_move_type = req->auto_info.battle_auto_first_move_type;
	m_param.battle_auto_first_skill_id = req->auto_info.battle_auto_first_skill_id;
	m_param.battle_auto_first_skill_level = req->auto_info.battle_auto_first_skill_level;
	formation.battle_auto_second_move_type = req->auto_info.battle_auto_second_move_type;
	formation.battle_auto_second_skill_id = req->auto_info.battle_auto_second_skill_id;
	formation.battle_auto_second_skill_level = req->auto_info.battle_auto_second_skill_level;

	{
		int pet_id = 0;
		long long pet_unique_id = 0LL;
		if (formation.grid_list[2].IsValid() && 
			formation.grid_list[2].character_type == PET_HELPER_FORMATION_GRID_PET)
		{
			pet_id = formation.grid_list[2].character_id;
			pet_unique_id = formation.grid_list[2].unique_id;
		}
		else if (formation.grid_list[7].IsValid() && 
			formation.grid_list[7].character_type == PET_HELPER_FORMATION_GRID_PET)
		{
			pet_id = formation.grid_list[7].character_id;
			pet_unique_id = formation.grid_list[7].unique_id;
		}

		const PetParam* pet_param = m_mgr->GetPet()->GetPetParam(pet_id, pet_unique_id, NULL);
		if (NULL != pet_param && 
			BATTLE_MOVE_TYPE_PERFORM_SKILL == formation.battle_auto_second_move_type)
		{
			PetConfig* pet_config = LOGIC_CONFIG->GetPetConfig();
			bool has_skill = pet_config->IsPetHasExclusiveSkill(pet_param->pet_id, pet_param->str_level, formation.battle_auto_second_skill_id, formation.battle_auto_second_skill_level);
			bool has_skill2 = pet_config->IsPetHasExclusiveSkill(pet_param->lian_yao_pet_id, pet_param->lian_yao_pet_str_lv, formation.battle_auto_second_skill_id, formation.battle_auto_second_skill_level);
			if (!has_skill && !has_skill2)
			{
				formation.battle_auto_second_move_type = pet_param->pet_auto_move_type;
				formation.battle_auto_second_skill_id = pet_param->pet_auto_skill_id;
				formation.battle_auto_second_skill_level = pet_param->pet_auto_skill_level;
			}
		}

		//同步自动到每个布阵中
		FormationAutoInfo info;
		info.battle_auto_first_move_type = (short)m_param.battle_auto_first_move_type;
		info.battle_auto_first_skill_id = (UInt16)m_param.battle_auto_first_skill_id;
		info.battle_auto_first_skill_level = (short)m_param.battle_auto_first_skill_level;
		info.battle_auto_second_move_type = (short)formation.battle_auto_second_move_type;
		info.battle_auto_second_skill_id = (UInt16)formation.battle_auto_second_skill_id;
		info.battle_auto_second_skill_level = (short)formation.battle_auto_second_skill_level;
		if (pet_id > 0)
		{
			this->SynAutoSkill(info, pet_id);
			m_mgr->GetRoleFormation()->SynAutoSkill(info, pet_id);
		}
	}
}

void PetHelperFormation::SendFormation(int system_type)
{
	if (system_type < 0 || system_type >= PET_HELPER_FORMATION_TYPE_MAX) return;
	if (!this->IsFormationValid(system_type)) return;

	Protocol::SCPetHelperFormationInfo info;
	info.Reset();
	info.system_type = system_type;
	
	PetHelperFormationInfo& formation = m_param.formation_list[system_type];
	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		info.grid_list[i].id = formation.grid_list[i].character_id;
		info.grid_list[i].type = formation.grid_list[i].character_type;
		info.grid_list[i].pet_exclusive_skill_disable_flag = formation.grid_list[i].pet_exclusive_skill_disable_flag;
		info.grid_list[i].lian_yao_exclusive_skill_disable_flag = formation.grid_list[i].lian_yao_exclusive_skill_disable_flag;
		info.grid_list[i].unique_id = formation.grid_list[i].unique_id;
	}
	UNSTD_STATIC_CHECK(
		ARRAY_ITEM_COUNT(info.grid_list) == SIDE_CHARACTER_NUM &&
		ARRAY_ITEM_COUNT(formation.grid_list) == SIDE_CHARACTER_NUM
	);

	for (int i = 0; i < FORMATION_STANDBY_PET_NUM; ++i)
	{
		info.standby_list[i].pet_id = formation.standby_list[i].pet_id;
		info.standby_list[i].unique_id = formation.standby_list[i].pet_unique_id;
		info.standby_list[i].pet_exclusive_skill_disable_flag = formation.standby_list[i].pet_exclusive_skill_disable_flag;
		info.standby_list[i].lian_yao_exclusive_skill_disable_flag = formation.standby_list[i].lian_yao_exclusive_skill_disable_flag;
	}
	UNSTD_STATIC_CHECK(
		ARRAY_ITEM_COUNT(info.standby_list) == FORMATION_STANDBY_PET_NUM &&
		ARRAY_ITEM_COUNT(formation.standby_list) == FORMATION_STANDBY_PET_NUM
	);

	info.auto_info.battle_auto_first_move_type = m_param.battle_auto_first_move_type;
	info.auto_info.battle_auto_first_skill_id = m_param.battle_auto_first_skill_id;
	info.auto_info.battle_auto_first_skill_level = m_param.battle_auto_first_skill_level;
	info.auto_info.battle_auto_second_move_type = m_param.formation_list[system_type].battle_auto_second_move_type;
	info.auto_info.battle_auto_second_skill_id = m_param.formation_list[system_type].battle_auto_second_skill_id;
	info.auto_info.battle_auto_second_skill_level = m_param.formation_list[system_type].battle_auto_second_skill_level;

	m_mgr->NetSend(&info, sizeof(info));
}

void PetHelperFormation::CheckFormationRemoveInvalidGrid(PetHelperFormationInfo& info)
{
	if (0 == info.is_valid_formation) return;

	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		if (PET_HELPER_FORMATION_GRID_PET == info.grid_list[i].character_type)
		{
			long long unique_id = info.grid_list[i].unique_id;
			int pet_id = info.grid_list[i].character_id;
			if (NULL == m_mgr->GetPet()->GetPetParam(pet_id, unique_id, NULL))
			{
				info.grid_list[i].character_type = PET_HELPER_FORMATION_GRID_INVALID;
				info.grid_list[i].character_id = 0;
				info.grid_list[i].unique_id = 0;
			}
		}
	}

	for (int i = FORMATION_STANDBY_PET_NUM - 1; i >= 0; --i)
	{
		long long unique_id = info.standby_list[i].pet_unique_id;
		int pet_id = info.standby_list[i].pet_id;
		if (0 == pet_id || 0 == unique_id) continue;

		if (NULL == m_mgr->GetPet()->GetPetParam(pet_id, unique_id, NULL))
		{
			info.standby_list[i].pet_id = 0;
			info.standby_list[i].pet_unique_id = 0;
			for (int k = FORMATION_STANDBY_PET_NUM - 1; k > i; --k)
			{
				if (0 == info.standby_list[k].pet_id || 0 == info.standby_list[k].pet_unique_id)
				{
					continue;
				}
				info.standby_list[i].pet_id = info.standby_list[k].pet_id;
				info.standby_list[i].pet_unique_id = info.standby_list[k].pet_unique_id;
				break;
			}
		}
	}
}

void PetHelperFormation::SetInitialFormation(PetHelperFormationInfo& info)
{
	info.is_valid_formation = 1;

	int role_grid = 2;
	int pet_grid = 7;
	if (BATTLE_ROW_TYPE_FRONT == m_mgr->GetCommonData().battle_position)
	{
		role_grid = 7;
		pet_grid = 2;
	}
	info.grid_list[role_grid].Reset();
	info.grid_list[role_grid].character_id = m_mgr->GetRoleUID();
	info.grid_list[role_grid].character_type = PET_HELPER_FORMATION_GRID_ROLE;

	const ItemGridData* pet = m_mgr->GetPet()->GetPetGridData(m_mgr->GetPet()->GetFightIndex());
	if (NULL != pet)
	{
		info.grid_list[pet_grid].Reset();
		info.grid_list[pet_grid].character_id = pet->item_id;
		info.grid_list[pet_grid].character_type = PET_HELPER_FORMATION_GRID_PET;
		info.grid_list[pet_grid].unique_id = pet->item_unique_id;

		info.battle_auto_second_move_type = (char)m_mgr->GetCommonData().battle_auto_second_move_type;
		info.battle_auto_second_skill_level = (char)m_mgr->GetCommonData().battle_auto_second_skill_level;
		info.battle_auto_second_skill_id = (UInt16)m_mgr->GetCommonData().battle_auto_second_skill_id;
	}

	static int fight_partner_list[SIDE_MAX_ROLE_NUM - 1];
	int fight_partner_num = 0;
	m_mgr->GetPartner()->GetSynFightPartnerIdList(ARRAY_LENGTH(fight_partner_list), fight_partner_list, &fight_partner_num);

	for (int i = 0; i < fight_partner_num && i < ARRAY_LENGTH(BATTLE_POS_RELATION) - 1; ++i)
	{
		const NewPartnerBaseInfo* pbi = m_mgr->GetPartner()->GetPartnerBaseInfoById(fight_partner_list[i]);
		if (NULL == pbi) continue;

		int partner_id = fight_partner_list[i];
		int column = BATTLE_POS_RELATION[i + 1];
		int grid_idx = BATTLE_ROW_TYPE_BACK * SIDE_CHARACTER_NUM + column;
		if (grid_idx < 0 || grid_idx >= ARRAY_LENGTH(info.grid_list) || grid_idx == 2 || grid_idx == 7)
		{
			continue;
		}
		info.grid_list[grid_idx].Reset();
		info.grid_list[grid_idx].character_id = partner_id;
		info.grid_list[grid_idx].character_type = PET_HELPER_FORMATION_GRID_PARTNER;
	}
}

bool PetHelperFormation::CheckGridIsPet(const PetHelperFormationGrid& grid, int pet_id, long long pet_unique_id)
{
	return grid.IsValid()
		&& grid.character_type == PET_HELPER_FORMATION_GRID_PET
		&& grid.character_id == pet_id
		;//&& grid.unique_id == pet_unique_id;
}

