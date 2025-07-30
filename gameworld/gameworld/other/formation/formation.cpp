#include "formation.hpp"
#include "servercommon/commondata.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "servercommon/errornum.h"
#include "protocol/msgother.h"
#include "protocol/msgformation.h"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "pethelperformation.hpp"

RoleFormation::RoleFormation() : m_mgr(NULL)
{

}

RoleFormation::~RoleFormation()
{

}

void RoleFormation::Init(RoleModuleManager* mgr, const FormationParam& param)
{
	m_mgr = mgr;
	m_param = param;

	this->CheckSkillLevel();

	if (0 == m_param.valid_formation_num)
	{
		m_param.valid_formation_num = LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().formation_start_num;
		m_param.cur_formation_idx = 0;
		this->SyncFormationFromRole(m_param.cur_formation_idx);
	}
}

void RoleFormation::GetInitParam(FormationParam* out_param)
{
	if (NULL == out_param) return;

	*out_param = m_param;
}

void RoleFormation::RenameFormationReq(int formation_seq, GameName new_name)
{
	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	
	new_name[sizeof(GameName) - 1] = 0;

	if (strlen(new_name) == 0)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (formation_seq >= m_param.valid_formation_num)
	{
		m_mgr->NoticeNum(errornum::EN_FORMATION_NOT_AVAILABLE);
		return;
	}

	FormationInfo& fi = m_param.info_list[formation_seq];
	F_STRNCPY(fi.formation_name, new_name, sizeof(GameName));

	this->SendFormationRenameNotice(formation_seq);
}

bool RoleFormation::SaveFormationReq(Protocol::CSFormationSave* req)
{
	int formation_seq = req->formation_seq;
	int pet_id = -1;
	if (req->is_save_new_format > 0)
	{
		//找一个空位
		int empty_slot = -1;
		for (int i = 1; i < m_param.valid_formation_num && i < ARRAY_LENGTH(m_param.info_list); ++i)
		{
			if (0 == m_param.info_list[i].is_valid)
			{
				empty_slot = i;
				break;
			}
		}
		if (-1 == empty_slot)
		{
			m_mgr->NoticeNum(errornum::EN_NO_EMPTY_SLOT_FOR_NEW_FORMATION);
			return false;
		}
		else
		{
			formation_seq = empty_slot;
		}
	}

	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (req->role_row != BATTLE_ROW_TYPE_BACK && req->role_row != BATTLE_ROW_TYPE_FRONT)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	const ItemGridData* pet_grid = NULL;
	if (-1 != req->pet_idx)
	{
		pet_grid = m_mgr->GetPet()->GetPetGridData(req->pet_idx);
		if (NULL == pet_grid)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		if (m_mgr->GetPet()->IsPetTraning(req->pet_idx))
		{
			m_mgr->NoticeNum(errornum::EN_PET_IS_TRAINNING_OPER_FAIL);
			return false;
		}
	}

	if (formation_seq >= m_param.valid_formation_num)
	{
		m_mgr->NoticeNum(errornum::EN_FORMATION_NOT_AVAILABLE);
		return false;
	}

	int role_pos = BATTLE_POS_RELATION[0] + ROW_CHARACTER_NUM * req->role_row;	
	int pet_pos = (role_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
	if (role_pos < 0 || role_pos >= SIDE_CHARACTER_NUM
		|| pet_pos < 0 || pet_pos >= SIDE_CHARACTER_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return false;
	}
	
	FormationGrid grid_list[SIDE_CHARACTER_NUM];
	grid_list[role_pos].character_id = m_mgr->GetRole()->GetUID();
	grid_list[role_pos].character_type = BATTLE_CHARACTER_TYPE_ROLE;
	grid_list[role_pos].unique_id = 0;
	grid_list[role_pos].reserve_ch = 0;
	grid_list[role_pos].reserve_sh = 0;

	if (-1 != req->pet_idx && NULL != pet_grid)
	{
		grid_list[pet_pos].character_id = pet_grid->param->pet_param.pet_id;
		grid_list[pet_pos].character_type = BATTLE_CHARACTER_TYPE_PET;
		grid_list[pet_pos].unique_id = pet_grid->item_unique_id;
		grid_list[pet_pos].reserve_ch = 0;
		grid_list[pet_pos].reserve_sh = 0;

		pet_id = pet_grid->param->pet_param.pet_id;
	}

	for (int i = 0; i < req->partner_num; ++i)
	{
		int partner_id = req->partner_list[i].partner_id;

		// 避免有重复的伙伴
		bool has_found = false;
		for (int k = 0; k < i; ++k)
		{
			if (req->partner_list[k].partner_id == partner_id)
			{
				has_found = true;
				break;
			}
		}
		if (has_found)
		{
			continue;
		}

		int partner_pos = req->partner_list[i].partner_pos;
		if (partner_pos < 0 || partner_pos >= SIDE_CHARACTER_NUM)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		if (partner_pos == role_pos || partner_pos == pet_pos)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		int partner_pos_opposite = (partner_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
		if (grid_list[partner_pos].IsValid() || grid_list[partner_pos_opposite].IsValid())
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		if (!m_mgr->GetPartner()->IsPartnerAlreadyActive(partner_id))
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}

		grid_list[partner_pos].character_id = partner_id;
		grid_list[partner_pos].character_type = BATTLE_CHARACTER_TYPE_PARTNER;
		grid_list[partner_pos].unique_id = 0;
		grid_list[partner_pos].reserve_ch = 0;
		grid_list[partner_pos].reserve_sh = 0;
	}

	FormationInfo& fi = m_param.info_list[formation_seq];
	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		fi.grid_list[i] = grid_list[i];
	}
	fi.is_valid = 1;
	if (req->is_save_new_format > 0)		//新阵容直接使用玩家当前设置即可
	{
		m_param.info_auto_list[formation_seq].battle_auto_first_move_type = m_mgr->GetCommonData().battle_auto_first_move_type;
		m_param.info_auto_list[formation_seq].battle_auto_first_skill_id = m_mgr->GetCommonData().battle_auto_first_skill_id;
		m_param.info_auto_list[formation_seq].battle_auto_first_skill_level = m_mgr->GetCommonData().battle_auto_first_skill_level;
		m_param.info_auto_list[formation_seq].battle_auto_second_move_type = m_mgr->GetCommonData().battle_auto_second_move_type;
		m_param.info_auto_list[formation_seq].battle_auto_second_skill_id = m_mgr->GetCommonData().battle_auto_second_skill_id;
		m_param.info_auto_list[formation_seq].battle_auto_second_skill_level = m_mgr->GetCommonData().battle_auto_second_skill_level;
	}
	else
	{
		m_param.info_auto_list[formation_seq] = req->auto_info;
	}

	// 同步到当前阵容
	if (formation_seq != 0)
	{
		FormationInfo& cur_fi = m_param.info_list[0];
		m_param.info_auto_list[0] = m_param.info_auto_list[formation_seq];
		for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
		{
			cur_fi.grid_list[i] = fi.grid_list[i];
		}
	}
	
	this->SynAutoSkill(req->auto_info, pet_id);
	m_mgr->GetPetHelperFormation()->SynAutoSkill(req->auto_info, pet_id);

	this->SyncFormationToRole(0, 0 != req->is_save_new_format);

	if (req->is_save_new_format > 0)
	{
		this->SendSimpleInfo();
		this->SendDetailInfo(formation_seq, Protocol::SCFormationDetailInfo::REASON_TYPE_SAVE_NEW_FORMATION);
	}
	else
	{
		this->SendDetailInfo(formation_seq, Protocol::SCFormationDetailInfo::REASON_TYPE_SAVE);
	}
	
	if (formation_seq != 0)
	{
		this->SendDetailInfo(0, Protocol::SCFormationDetailInfo::REASON_TYPE_SAVE);
	}
	return true;
}

void RoleFormation::FormationDetailInfoReq(int formation_seq)
{
	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (formation_seq >= m_param.valid_formation_num)
	{
		m_mgr->NoticeNum(errornum::EN_FORMATION_NOT_AVAILABLE);
		return;		
	}

	if (formation_seq == m_param.cur_formation_idx)
	{
		this->SyncFormationFromRole(formation_seq);
	}
	else if (0 != m_param.info_list[formation_seq].is_valid)
	{
		// 检查宠物是否尚在
		int pet_pos = BATTLE_POS_RELATION[0];
		do
		{
			if (BATTLE_CHARACTER_TYPE_PET != m_param.info_list[formation_seq].grid_list[pet_pos].character_type)
			{
				pet_pos = (pet_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
				if (BATTLE_CHARACTER_TYPE_PET != m_param.info_list[formation_seq].grid_list[pet_pos].character_type)
				{
					// 没有宠物在阵上
					break;
				}
			}

			const PetParam* pet_param = m_mgr->GetPet()->GetPetParam(
				m_param.info_list[formation_seq].grid_list[pet_pos].character_id,
				m_param.info_list[formation_seq].grid_list[pet_pos].unique_id,
				NULL);
			if (NULL == pet_param) // 找不到这宠物了，那就清掉数据
			{
				m_param.info_list[formation_seq].grid_list[pet_pos].Reset();
				m_param.info_auto_list[formation_seq].battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				m_param.info_auto_list[formation_seq].battle_auto_second_skill_id = 0;
				m_param.info_auto_list[formation_seq].battle_auto_second_skill_level = 0;
			}

		} while (0);
		
	}

	this->SendDetailInfo(formation_seq, Protocol::SCFormationDetailInfo::REASON_TYPE_INFO);
}

void RoleFormation::DeleteFormationReq(int formation_seq)
{
	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (0 == formation_seq || formation_seq == m_param.cur_formation_idx)
	{
		m_mgr->NoticeNum(errornum::EN_CUR_FORMATION_CANNOT_DELETE);
		return;
	}

	if (formation_seq >= m_param.valid_formation_num)
	{
		m_mgr->NoticeNum(errornum::EN_FORMATION_NOT_AVAILABLE);
		return;
	}

	m_param.info_list[formation_seq].Reset();

	this->SendSimpleInfo();
}

void RoleFormation::OnChangeProf(int old_profession, int new_profession)
{
	if (GetBaseProfession(old_profession) == GetBaseProfession(new_profession))
	{
		return;
	}

	this->OnChangeProfResetFormationAutoMove();
	if (this->IsCurFormationSeqValid())
	{
		this->SyncFormationToRole(m_param.cur_formation_idx, false);
	}
}

bool RoleFormation::IsHasSetAutoSkill()
{
	if(m_param.cur_formation_idx < 0 || m_param.cur_formation_idx >= ARRAY_ITEM_COUNT(m_param.info_auto_list))
	{
		return false;
	}
	if (BATTLE_MOVE_TYPE_NORMAL_ATTACK == m_param.info_auto_list[m_param.cur_formation_idx].battle_auto_first_move_type && 
		BATTLE_MOVE_TYPE_NORMAL_ATTACK == m_param.info_auto_list[m_param.cur_formation_idx].battle_auto_second_move_type)
	{
		return false;
	}

	return true;
}

void RoleFormation::SynAutoSkill(FormationAutoInfo & info, int pet_id)
{
	for (int i = 0; i < MAX_FORMATION_NUM; i++)
	{
		if(!m_param.info_list[i].is_valid) continue;

		bool is_same_pet = false;
		for (int grid = 0; grid < SIDE_CHARACTER_NUM; grid++)
		{
			if (m_param.info_list[i].grid_list[grid].character_type == BATTLE_CHARACTER_TYPE_PET  && m_param.info_list[i].grid_list[grid].character_id == pet_id)
			{
				is_same_pet = true;
			}
		}

		m_param.info_auto_list[i].battle_auto_first_move_type = info.battle_auto_first_move_type;
		m_param.info_auto_list[i].battle_auto_first_skill_id = info.battle_auto_first_skill_id;
		m_param.info_auto_list[i].battle_auto_first_skill_level = info.battle_auto_first_skill_level;
		if (is_same_pet)
		{
			m_param.info_auto_list[i].battle_auto_second_move_type = info.battle_auto_second_move_type;
			m_param.info_auto_list[i].battle_auto_second_skill_id = info.battle_auto_second_skill_id;
			m_param.info_auto_list[i].battle_auto_second_skill_level = info.battle_auto_second_skill_level;
		}
		//this->SendDetailInfo(i, Protocol::SCFormationDetailInfo::REASON_TYPE_SAVE_NEW_FORMATION);		
	}

	{	// 布阵界面同步玩家修改后的技能
		Protocol::SCBattleAutoInfo info;

		info.is_auto = m_mgr->GetCommonData().battle_is_auto;
		info.first_move_type = m_param.info_auto_list[0].battle_auto_first_move_type;
		info.first_skill_id = m_param.info_auto_list[0].battle_auto_first_skill_id;
		info.first_skill_level = m_param.info_auto_list[0].battle_auto_first_skill_level;
		info.second_move_type = m_param.info_auto_list[0].battle_auto_second_move_type;
		info.second_skill_id = m_param.info_auto_list[0].battle_auto_second_skill_id;
		info.second_skill_level = m_param.info_auto_list[0].battle_auto_second_skill_level;

		m_mgr->NetSend(&info, sizeof(info));
	}
}

void RoleFormation::SynBattleAutoSkill(bool is_first_move, short move_type, UInt16 skill_id, short skill_level, int pet_id, long long pet_unique_id)
{
	for (int i = 0; i < MAX_FORMATION_NUM; i++)
	{
		if (is_first_move)
		{
			m_param.info_auto_list[i].battle_auto_first_move_type = move_type;
			m_param.info_auto_list[i].battle_auto_first_skill_id = skill_id;
			m_param.info_auto_list[i].battle_auto_first_skill_level = skill_level;
		}
		else
		{
			UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_param.info_list[i].grid_list) == 10);
			FormationGrid& grid2 = m_param.info_list[i].grid_list[2];
			FormationGrid& grid7 = m_param.info_list[i].grid_list[7];
			if (this->CheckGridIsPet(grid2, pet_id, pet_unique_id)
				|| this->CheckGridIsPet(grid7, pet_id, pet_unique_id))
			{
				m_param.info_auto_list[i].battle_auto_second_move_type = move_type;
				m_param.info_auto_list[i].battle_auto_second_skill_id = skill_id;
				m_param.info_auto_list[i].battle_auto_second_skill_level = skill_level;
			}
		}
		//this->SendDetailInfo(i, Protocol::SCFormationDetailInfo::REASON_TYPE_SAVE_NEW_FORMATION);
	}

	//this->SyncFormationToRole(0, false);
}

void RoleFormation::SynPetAutoSkill()
{
	m_param.info_auto_list[0].battle_auto_second_move_type = m_mgr->GetCommonData().battle_auto_second_move_type;
	m_param.info_auto_list[0].battle_auto_second_skill_id = m_mgr->GetCommonData().battle_auto_second_skill_id;
	m_param.info_auto_list[0].battle_auto_second_skill_level = m_mgr->GetCommonData().battle_auto_second_skill_level;

	for (int grid = 0; grid < SIDE_CHARACTER_NUM; grid++)
	{
		if (m_param.info_list[0].grid_list[grid].character_type == BATTLE_CHARACTER_TYPE_PET)
		{
			this->SynAutoSkill(m_param.info_auto_list[0], m_param.info_list[0].grid_list[grid].character_id);
			m_mgr->GetPetHelperFormation()->SynAutoSkill(m_param.info_auto_list[0], m_param.info_list[0].grid_list[grid].character_id);
		}
	}
}

void RoleFormation::SendSimpleInfo()
{
	Protocol::SCFormationSimpleInfo info;
	info.valid_formation_num = m_param.valid_formation_num;
	info.active_formation_num = 0;

	int num = 0;
	for (int i = 0; i < m_param.valid_formation_num && i < MAX_FORMATION_NUM && num < MAX_FORMATION_NUM; ++i)
	{
		if (0 == m_param.info_list[i].is_valid) continue;

		info.formation_list[num].seq = i;
		F_STRNCPY(info.formation_list[num].formation_name, m_param.info_list[i].formation_name, sizeof(GameName));
		
		num += 1;
	}

	info.active_formation_num = num;

	int send_len = sizeof(info) - sizeof(info.formation_list) + info.active_formation_num * sizeof(info.formation_list[0]);
	m_mgr->NetSend(&info, send_len);
}

void RoleFormation::SendDetailInfo(int formation_seq, int reason)
{
	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM)
	{
		return;
	}

	Protocol::SCFormationDetailInfo info;
	info.formation_seq = formation_seq;
	info.reason = reason;
	info.partner_num = 0;
	info.pet_idx = -1;

	if (0 == m_param.info_list[formation_seq].is_valid)
	{
		info.is_valid = 0;
		info.role_row = 0;
		info.pet_idx = -1;

		// 空陣容直接發
		int len = sizeof(info) - sizeof(info.partner_list) + info.partner_num * sizeof(info.partner_list[0]);
		m_mgr->NetSend(&info, len);

		return; 
	}
	else
	{
		info.is_valid = 1;
	}

	int role_pos = -1;
	int pet_pos = -1;
	FormationInfo& fi = m_param.info_list[formation_seq];
	for (int i = 0; i < SIDE_MAX_ROLE_NUM; ++i)
	{
		int pos = BATTLE_POS_RELATION[i];
		int opposite_pos = (pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
		if (i == 0)
		{
			if (fi.grid_list[pos].IsValid() &&
				BATTLE_CHARACTER_TYPE_ROLE == fi.grid_list[pos].character_type)
			{
				role_pos = pos;
			}
			else if (fi.grid_list[opposite_pos].IsValid() &&
				BATTLE_CHARACTER_TYPE_ROLE == fi.grid_list[opposite_pos].character_type)
			{
				role_pos = opposite_pos;
			}

			if (fi.grid_list[pos].IsValid() && 
				BATTLE_CHARACTER_TYPE_PET == fi.grid_list[pos].character_type)
			{
				pet_pos = pos;
			}
			else if (fi.grid_list[opposite_pos].IsValid() && 
				BATTLE_CHARACTER_TYPE_PET == fi.grid_list[opposite_pos].character_type)
			{
				pet_pos = opposite_pos;
			}
		}
		else if (info.partner_num < ARRAY_ITEM_COUNT(info.partner_list))
		{
			if (fi.grid_list[pos].IsValid() &&
				BATTLE_CHARACTER_TYPE_PARTNER == fi.grid_list[pos].character_type)
			{
				info.partner_list[info.partner_num].partner_id = fi.grid_list[pos].character_id;
				info.partner_list[info.partner_num].partner_pos = pos;
				info.partner_list[info.partner_num].reserve_sh = 0;
				info.partner_num += 1;
			}
			else if (fi.grid_list[opposite_pos].IsValid() &&
				BATTLE_CHARACTER_TYPE_PARTNER == fi.grid_list[opposite_pos].character_type)
			{
				info.partner_list[info.partner_num].partner_id = fi.grid_list[opposite_pos].character_id;
				info.partner_list[info.partner_num].partner_pos = opposite_pos;
				info.partner_list[info.partner_num].reserve_sh = 0;
				info.partner_num += 1;
			}
		}
	}

	if (role_pos == -1)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	info.role_row = role_pos / ROW_CHARACTER_NUM;
	if (-1 != pet_pos)
	{
		int pet_id = fi.grid_list[pet_pos].character_id;
		long long unique_id = fi.grid_list[pet_pos].unique_id;
		int pet_idx = -1;
		if (NULL != m_mgr->GetPet()->GetPetParam(pet_id, unique_id, &pet_idx))
		{
			info.pet_idx = static_cast<short>(pet_idx);
			if (m_mgr->GetPet()->IsPetTraning(info.pet_idx))
			{
				info.pet_idx = -1; // 正在训练中，不显示宠物
			}
		}
	}
	info.auto_info = m_param.info_auto_list[formation_seq];

	int len = sizeof(info) - sizeof(info.partner_list) + info.partner_num * sizeof(info.partner_list[0]);
	m_mgr->NetSend(&info, len);
}

FormationInfo* RoleFormation::GetCurFormationInfo()
{
	if (m_param.cur_formation_idx >= 0 && m_param.cur_formation_idx < MAX_FORMATION_NUM)
	{
		if (m_param.info_list[m_param.cur_formation_idx].is_valid > 0)
		{
			return &m_param.info_list[m_param.cur_formation_idx];
		}
	}

	return NULL;
}

void RoleFormation::GetCurFormationPartnerPosList(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_partner_list, ARG_OUT int* out_pos_list)
{
	if (NULL == out_num || NULL == out_pos_list || NULL == out_partner_list) return;

	FormationInfo* fi = this->GetCurFormationInfo();
	if (NULL == fi) return;

	// 从1开始，因为0是主角
	*out_num = 0;
	for (int i = 1; i < ARRAY_LENGTH(BATTLE_POS_RELATION) && *out_num < max_num; ++i)
	{
		int grid_idx = BATTLE_POS_RELATION[i];
		if (fi->grid_list[grid_idx].IsValid() && BATTLE_CHARACTER_TYPE_PARTNER == fi->grid_list[grid_idx].character_type)
		{
			out_partner_list[*out_num] = fi->grid_list[grid_idx].character_id;
			out_pos_list[*out_num] = i;
			*out_num += 1;
		}
		else if (fi->grid_list[grid_idx + ROW_CHARACTER_NUM].IsValid() && BATTLE_CHARACTER_TYPE_PARTNER == fi->grid_list[grid_idx + ROW_CHARACTER_NUM].character_type)
		{
			out_partner_list[*out_num] = fi->grid_list[grid_idx + ROW_CHARACTER_NUM].character_id;
			out_pos_list[*out_num] = i;
			*out_num += 1;
		}
	}
}

void RoleFormation::OnPartnerFightStateChange()
{
	this->SyncFormationFromRole(0);
}

void RoleFormation::SyncFormationFromRole(int formation_seq)
{
	FormationInfo& fi = m_param.info_list[formation_seq];

	int role_pos = BATTLE_POS_RELATION[0] + (m_mgr->GetCommonData().battle_position) * ROW_CHARACTER_NUM;

#ifdef _DEBUG
	assert(role_pos == 2 || role_pos == 7);
#else
	if (role_pos != 2 && role_pos != 7)
	{
		return;
	}
#endif

	for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
	{
		fi.grid_list[i].Reset();
	}

	// 人物
	fi.grid_list[role_pos].character_id = m_mgr->GetRole()->GetUID();
	fi.grid_list[role_pos].character_type = BATTLE_CHARACTER_TYPE_ROLE;
	fi.grid_list[role_pos].unique_id = 0;
	fi.grid_list[role_pos].reserve_ch = 0;
	fi.grid_list[role_pos].reserve_sh = 0;

	// 出战宠物
	int pet_pos = (role_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
	int fight_pet_idx = m_mgr->GetPet()->GetFightIndex();
	const ItemGridData* pet_grid = m_mgr->GetPet()->GetPetGridData(fight_pet_idx);
	if (NULL != pet_grid)
	{
		fi.grid_list[pet_pos].character_id = pet_grid->param->pet_param.pet_id;
		fi.grid_list[pet_pos].character_type = BATTLE_CHARACTER_TYPE_PET;
		fi.grid_list[pet_pos].unique_id = pet_grid->item_unique_id;
		fi.grid_list[pet_pos].reserve_ch = 0;
		fi.grid_list[pet_pos].reserve_sh = 0;
	}
	
	// 伙伴
	static int fight_partner_list[SIDE_MAX_ROLE_NUM - 1];
	int max_partner_num = SIDE_MAX_ROLE_NUM - 1;

	int fight_partner_num = 0;
	m_mgr->GetPartner()->GetSynFightPartnerIdList(max_partner_num, fight_partner_list, &fight_partner_num);
	for (int i = 0; i < fight_partner_num && i < SIDE_MAX_ROLE_NUM - 1; ++i)
	{
		if (fight_partner_list[i] <= 0) continue;

		const NewPartnerBaseInfo* pbi = m_mgr->GetPartner()->GetPartnerBaseInfoById(fight_partner_list[i]);
		if (NULL == pbi) continue;

		int partner_pos = BATTLE_POS_RELATION[i + 1];
		if (pbi->battle_pos > 0)
		{
			partner_pos += ROW_CHARACTER_NUM;
		}

		fi.grid_list[partner_pos].character_id = fight_partner_list[i];
		fi.grid_list[partner_pos].character_type = BATTLE_CHARACTER_TYPE_PARTNER;
		fi.grid_list[partner_pos].unique_id = 0;
		fi.grid_list[partner_pos].reserve_sh = 0;
		fi.grid_list[partner_pos].reserve_ch = 0;
	}

	if (0 == fi.is_valid)
	{
		fi.is_valid = 1;
	}

	EventHandler::Instance().OnRoleFormationChange(m_mgr->GetRole(), 0 == formation_seq);
}

void RoleFormation::SyncFormationToRole(int formation_seq, bool is_new_formation)
{
	FormationInfo& fi = m_param.info_list[formation_seq];
	FormationAutoInfo & auto_info = m_param.info_auto_list[formation_seq];
	if (0 == fi.is_valid)
	{
		return;
	}

	int role_pos = BATTLE_POS_RELATION[0];
	if (!fi.grid_list[role_pos].IsValid() ||
		fi.grid_list[role_pos].character_type != BATTLE_CHARACTER_TYPE_ROLE)
	{
		role_pos = (role_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
		if (!fi.grid_list[role_pos].IsValid() ||
			fi.grid_list[role_pos].character_type != BATTLE_CHARACTER_TYPE_ROLE)
		{
			// 找不到主角的信息，一定有问题
#ifdef _DEBUG
			assert(0);
#endif
			return;
		}
	}

	int battle_position = role_pos / ROW_CHARACTER_NUM;
	m_mgr->GetRole()->RoleSetBattlePosition(static_cast<char>(battle_position));

	int pet_pos = (role_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
	if (!fi.grid_list[pet_pos].IsValid()) // 该空格没有单位，则把玩家当前出战宠物设为待机
	{
		int cur_fight_idx = m_mgr->GetPet()->GetFightIndex();
		if (cur_fight_idx != -1)
		{
			m_mgr->GetPet()->SetPetState(cur_fight_idx, SET_STANDBY_INDEX1);
		}
	}
	else  // 该空格有单位
	{
#ifdef _DEBUG
		assert(BATTLE_CHARACTER_TYPE_PET == fi.grid_list[pet_pos].character_type);
#endif

		int cur_fight_idx = -1;
		int standby_idx1 = -1;
		int standby_idx2 = -1;
		int standby_idx3 = -1;
		int standby_idx4 = -1;
		int standby_idx5 = -1;
		m_mgr->GetPet()->GetFightIndex(&cur_fight_idx, &standby_idx1, &standby_idx2, &standby_idx3, &standby_idx4, &standby_idx5);
		if (-1 != cur_fight_idx)
		{
			const ItemGridData* p = m_mgr->GetPet()->GetPetGridData(cur_fight_idx);
			if (NULL != p)
			{
				// 当前出战宠物与布阵的出战宠物不一样，改为待机或休息
				if (p->param->pet_param.pet_id != fi.grid_list[pet_pos].character_id ||
					p->item_unique_id != fi.grid_list[pet_pos].unique_id)
				{
					if (-1 == standby_idx1)
					{
						m_mgr->GetPet()->SetPetState(cur_fight_idx, SET_STANDBY_INDEX1);
					}
					else if (-1 == standby_idx2)
					{
						m_mgr->GetPet()->SetPetState(cur_fight_idx, SET_STANDBY_INDEX2);
					}
					else if (-1 == standby_idx3)
					{
						m_mgr->GetPet()->SetPetState(cur_fight_idx, SET_STANDBY_INDEX3);
					}
					else if (-1 == standby_idx4)
					{
						m_mgr->GetPet()->SetPetState(cur_fight_idx, SET_STANDBY_INDEX4);
					}
					else if (-1 == standby_idx5)
					{
						m_mgr->GetPet()->SetPetState(standby_idx5, SET_STANDBY_INDEX5);
					}
				}
			}
		}

		int new_fight_idx = -1;
		if (NULL != m_mgr->GetPet()->GetPetParam(fi.grid_list[pet_pos].character_id, fi.grid_list[pet_pos].unique_id, &new_fight_idx))
		{
			if (cur_fight_idx != new_fight_idx)
			{
				m_mgr->GetPet()->SetPetState(new_fight_idx, SET_FIGHT_INDEX);
			}
		}
		m_mgr->GetPet()->ModifyAutoMove(new_fight_idx, auto_info.battle_auto_second_move_type, auto_info.battle_auto_second_skill_id, auto_info.battle_auto_second_skill_level, true);
	}

	static int fight_partner_list[SIDE_MAX_ROLE_NUM - 1];
	static int battle_row_list[SIDE_MAX_ROLE_NUM - 1];
	int max_partner_num = SIDE_MAX_ROLE_NUM - 1;
	memset(fight_partner_list, -1, sizeof(fight_partner_list));
	memset(battle_row_list, 0, sizeof(battle_row_list));
	for (int i = 0; i < SIDE_MAX_ROLE_NUM - 1; ++i)
	{
		int partner_pos = BATTLE_POS_RELATION[i + 1];
		int partner_pos_1 = (partner_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;

		//暂时不考虑伙伴前后排
		
		if (!fi.grid_list[partner_pos].IsValid() || BATTLE_CHARACTER_TYPE_PARTNER != fi.grid_list[partner_pos].character_type)
		{
			if (!fi.grid_list[partner_pos_1].IsValid() || BATTLE_CHARACTER_TYPE_PARTNER != fi.grid_list[partner_pos_1].character_type)
			{
				fight_partner_list[i] = -1;
			}	
			else
			{
				fight_partner_list[i] = fi.grid_list[partner_pos_1].character_id;
				battle_row_list[i] = BATTLE_ROW_TYPE_FRONT;
			}
		}
		else
		{
			fight_partner_list[i] = fi.grid_list[partner_pos].character_id;
			battle_row_list[i] = BATTLE_ROW_TYPE_BACK;
		}
	}
	if (!is_new_formation)
	{
		m_mgr->GetRole()->SetBattleAutoMove(true, auto_info.battle_auto_first_move_type, auto_info.battle_auto_first_skill_id, auto_info.battle_auto_first_skill_level, false, false, true);
		m_mgr->GetRole()->SetBattleAutoMove(false, auto_info.battle_auto_second_move_type, auto_info.battle_auto_second_skill_id, auto_info.battle_auto_second_skill_level, false, true, true);
	}

	if (0 == formation_seq)
	{
		m_mgr->GetPartner()->SyncFightListFromFormation(fight_partner_list, battle_row_list, max_partner_num);	
	}
	
	EventHandler::Instance().OnRoleFormationChange(m_mgr->GetRole(), 0 == formation_seq);
}

void RoleFormation::SendFormationRenameNotice(int formation_seq)
{
	if (formation_seq < 0 || formation_seq >= MAX_FORMATION_NUM || formation_seq >= m_param.valid_formation_num)
	{
		return;
	}

	Protocol::SCFormationRename msg;
	msg.formation_seq = formation_seq;
	F_STRNCPY(msg.new_name, m_param.info_list[formation_seq].formation_name, sizeof(GameName));

	m_mgr->NetSend(&msg, sizeof(msg));
}

void RoleFormation::CheckSkillLevel()
{
	for (int i = 0; i < MAX_FORMATION_NUM && i < m_param.valid_formation_num; ++i)
	{
		if (0 == m_param.info_auto_list[i].battle_auto_first_skill_level
			&& m_param.info_auto_list[i].battle_auto_first_skill_id > 0)
		{
			m_param.info_auto_list[i].battle_auto_first_skill_level = 1;
		}

		if (0 == m_param.info_auto_list[i].battle_auto_second_skill_level
			&& m_param.info_auto_list[i].battle_auto_second_skill_id > 0)
		{
			m_param.info_auto_list[i].battle_auto_second_skill_level = 1;
		}
	}
}

bool RoleFormation::CheckGridIsPet(const FormationGrid& grid, int pet_id, long long pet_unique_id)
{
	return grid.IsValid()
		&& (grid.character_type == BATTLE_CHARACTER_TYPE_PET || grid.character_type == BATTLE_CHARACTER_TYPE_PET_SHADOW)
		&& grid.character_id == pet_id
		;// && grid.unique_id == pet_unique_id;
}

bool RoleFormation::IsCurFormationSeqValid() const
{
	return m_param.cur_formation_idx >= 0
		&& m_param.cur_formation_idx < m_param.valid_formation_num
		&& m_param.cur_formation_idx < MAX_FORMATION_NUM;
}

void RoleFormation::OnChangeProfResetFormationAutoMove()
{
	RoleSkill::SkillItem * skill = m_mgr->GetRoleSkill()->GetRandSkill();
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.info_auto_list); i++)
	{
		if (0 == m_param.info_list[i].is_valid) continue;

		FormationAutoInfo & auto_info = m_param.info_auto_list[i];
		if (BATTLE_MOVE_TYPE_PERFORM_SKILL == auto_info.battle_auto_first_move_type ||
			BATTLE_MOVE_TYPE_AI_SKILL == auto_info.battle_auto_first_move_type)
		{
			if (NULL != skill)
			{
				auto_info.battle_auto_first_skill_id = skill->skill_id;
				auto_info.battle_auto_first_skill_level = skill->skill_level;
			}
			else
			{
				auto_info.battle_auto_first_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				auto_info.battle_auto_first_skill_id = 0;
				auto_info.battle_auto_first_skill_level = 0;
			}
		}

		int pet_pos = BATTLE_POS_RELATION[0];
		bool is_has_pet = true;
		if (BATTLE_CHARACTER_TYPE_PET != m_param.info_list[i].grid_list[pet_pos].character_type)
		{
			pet_pos = (pet_pos + ROW_CHARACTER_NUM) % SIDE_CHARACTER_NUM;
			if (BATTLE_CHARACTER_TYPE_PET != m_param.info_list[i].grid_list[pet_pos].character_type)
			{
				is_has_pet = false;
			}
		}

		if (is_has_pet)
		{
			const PetParam* pet_param = m_mgr->GetPet()->GetPetParam(
				m_param.info_list[i].grid_list[pet_pos].character_id,
				m_param.info_list[i].grid_list[pet_pos].unique_id,
				NULL);
			if (NULL == pet_param) // 找不到这宠物了，那就清掉数据
			{
				m_param.info_list[i].grid_list[pet_pos].Reset();
				m_param.info_auto_list[i].battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				m_param.info_auto_list[i].battle_auto_second_skill_id = 0;
				m_param.info_auto_list[i].battle_auto_second_skill_level = 0;
			}
		}
		else if (BATTLE_MOVE_TYPE_PERFORM_SKILL == auto_info.battle_auto_second_move_type ||
			BATTLE_MOVE_TYPE_AI_SKILL == auto_info.battle_auto_second_move_type)
		{
			if (NULL != skill)
			{
				auto_info.battle_auto_second_skill_id = skill->skill_id;
				auto_info.battle_auto_second_skill_level = skill->skill_level;
			}
			else
			{
				auto_info.battle_auto_second_move_type = BATTLE_MOVE_TYPE_NORMAL_ATTACK;
				auto_info.battle_auto_second_skill_id = 0;
				auto_info.battle_auto_second_skill_level = 0;
			}
		}
	}
}

