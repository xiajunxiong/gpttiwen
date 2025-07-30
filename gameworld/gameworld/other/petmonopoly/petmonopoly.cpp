#include "other/petmonopoly/petmonopoly.hpp"
#include "protocol/msgpetmonopoly.h"
#include "other/rolemodulemanager.hpp"
#include "other/pet/petconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "other/event/eventhandler.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "item/knapsack.h"
#include "obj/character/role.h"
#include "battle/battle_manager_local.hpp"
#include "petmonopolyconfig.hpp"
#include "gamelog.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/levelcomplement/levelcomplement.h"
#include "global/worldstatus/worldstatus.hpp"

PetMonopoly::PetMonopoly() : m_mgr(NULL)
{

}

PetMonopoly::~PetMonopoly()
{

}

void PetMonopoly::Init(RoleModuleManager* mgr, const NewPetMonopolyParam& new_param, const PetMonopolyParam& param)
{
	m_mgr = mgr;

	if (new_param.old_param_flag == 0)
	{
		m_param.add_exp_times = param.add_exp_times;
		m_param.cur_grid_idx = param.cur_grid_idx;
		m_param.cur_level = param.cur_level;
		m_param.double_reward_surplus = param.double_reward_surplus;
		m_param.expect_gold_pet_id = param.expect_gold_pet_id;
		m_param.expect_silver_pet_id = param.expect_silver_pet_id;
		memcpy(m_param.fast_move_num, param.fast_move_num, sizeof(m_param.fast_move_num));
		memcpy(m_param.grid_list, param.grid_list, sizeof(m_param.grid_list));
		m_param.is_catch_pet_before = param.is_catch_pet_before;
		m_param.reward_record_num = param.reward_record_num;
		m_param.turntable_sector_idx = param.turntable_sector_idx;
		memcpy(m_param.turntable_sector_list, param.turntable_sector_list, sizeof(m_param.turntable_sector_list));
		memcpy(m_param.special_items, param.special_items, sizeof(m_param.special_items));
		for (int i=0;i<ARRAY_ITEM_COUNT(m_param.pet_list);++i)
		{
			m_param.pet_list[i] = param.reward_list[i];
		}

		m_param.old_param_flag = 1;
	}
	else
	{
		m_param = new_param;
	}

	if (0 == m_param.cur_level)
	{
		gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::Init role[%d,%s] init first time", mgr->GetRoleUID(), mgr->GetRoleName());
		this->GenerateGridData(1); // 从第1层开始
	}
	else
	{
		this->CheckGridVersion();
	}
}

void PetMonopoly::GetInitParam(ARG_OUT NewPetMonopolyParam* param)
{
	if (NULL != param)
	{
		*param = m_param;
	}
}

void PetMonopoly::OnOperateReq(int operate_type, int param1, int param2)
{
	switch (operate_type)
	{
	case Protocol::CSPetMonopolyOperate::OPER_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_REWARD_RECORD:
		{
			this->SendRewardRecordInfo();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_SET_EXPECT_PET:
		{
			this->SetExpectPet(param1);
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_CLEAR_EXPECT_PET:
		{
			bool is_gold = (1 == param1);
			this->ClearExpectPet(is_gold);
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_ROLL_DICE:
		{
			this->RollDice();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_FETCH_GRID_REWARD:
		{
			this->FetchGridReward();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_START_PET_BATTLE:
		{
			this->StartBattle();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_TURN_TABLE:
		{
			this->ShowTurnTableResult();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_ENTER_NEXT_LEVEL:
		{
			this->EnterNextLevel();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_SHOW_TRUNTABLE_REWARDS:
		{
			this->ShowTurnTableRewards();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_TURN_TABLE_END:
		{
			this->TurnTableEnd();
		}
		break;
	case Protocol::CSPetMonopolyOperate::OPER_TYPE_ADD_EXP:
		{
			if (m_param.add_exp_times > 0)
			{
				this->AddExp();
			}
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_SPECIAL_ITEM:
		{
			this->UseSpecialItem(param1, param2);
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_SPECIAL_ITEM_NUM:
		{
			this->SendSpecialInfo();
		}
		break;

	case Protocol::CSPetMonopolyOperate::OPER_TYPE_FAST_MOVE_REWARD:
		{
			this->SendFastMoveReward(param1);
		}
		break;
	}
}

void PetMonopoly::OnBattleFinish()
{
	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
	{
		return;
	}

	if (0 == m_param.is_catch_pet_before) // 第一次抓宠战斗结束后就赋值为1
	{
		m_param.is_catch_pet_before = 1;
	}

	m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
	this->SendCurGridChangeNotice();

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::OnBattleFinish role[%d,%s] grid[%d] BECOME NONE",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.cur_grid_idx);
}

void PetMonopoly::SendInfo()
{
	Protocol::SCPetMonopolyInfo msg;
	msg.expect_gold_pet_id = m_param.expect_gold_pet_id;
	msg.expect_silver_pet_id = m_param.expect_silver_pet_id;
	msg.cur_level = m_param.cur_level;
	msg.cur_grid_idx = m_param.cur_grid_idx;
	
	for (int grid_idx = 0; grid_idx < ARRAY_ITEM_COUNT(m_param.grid_list); ++grid_idx)
	{
		msg.grid_list[grid_idx].grid_type = m_param.grid_list[grid_idx].grid_type;
		msg.grid_list[grid_idx].reward_type = m_param.grid_list[grid_idx].reward_type;
		
		if (PET_MONOPOLY_GRID_REWARD_ITEM == msg.grid_list[grid_idx].reward_type ||
			PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == msg.grid_list[grid_idx].reward_type)
		{
			msg.grid_list[grid_idx].param1 = m_param.grid_list[grid_idx].grid_detail.item.item_id;
			msg.grid_list[grid_idx].param2 = m_param.grid_list[grid_idx].grid_detail.item.item_num;
		}
		else if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == msg.grid_list[grid_idx].reward_type)
		{
			msg.grid_list[grid_idx].param1 = m_param.grid_list[grid_idx].grid_detail.pet.pet_id;
			msg.grid_list[grid_idx].param2 = 0;
		}
		else if (PET_MONOPOLY_GRID_REWARD_OPPORTUNITY == msg.grid_list[grid_idx].reward_type)
		{
			msg.grid_list[grid_idx].param1 = m_param.grid_list[grid_idx].grid_detail.special_item.special_item_id;
			msg.grid_list[grid_idx].param2 = 1;
		}
		else
		{
			msg.grid_list[grid_idx].param1 = 0;
			msg.grid_list[grid_idx].param2 = 0;
		}
	}

	m_mgr->NetSend(&msg, sizeof(msg));
}

void PetMonopoly::SendSpecialInfo()
{	
	this->SendSpecialItemNum(-1);
}

void PetMonopoly::SendRewardRecordInfo()
{
	Protocol::SCPetMonopolyRewardRecord msg;
	msg.reward_num = 0;
	for (int i = 0; i < m_param.pet_record_num && msg.reward_num < ARRAY_ITEM_COUNT(msg.reward_list); ++i)
	{
		msg.reward_list[msg.reward_num].item_id = m_param.pet_list[i].item_id;
		msg.reward_list[msg.reward_num].item_num = m_param.pet_list[i].num;
		msg.reward_list[msg.reward_num].is_bind = m_param.pet_list[i].is_bind;
		msg.reward_list[msg.reward_num].param_length = 0;
		const ItemBase* item = ITEMPOOL->GetItem(m_param.pet_list[i].item_id);
		if (NULL != item && item->IsNetValueItem())
		{
			int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
			msg.reward_list[msg.reward_num].param_length = param_length;
			memcpy(msg.reward_list[msg.reward_num].data, m_param.pet_list[i].param_data, param_length);
		}
		msg.reward_num += 1;
	}

	for (int i = 0; i < m_param.reward_record_num && msg.reward_num < ARRAY_ITEM_COUNT(msg.reward_list); ++i)
	{
		msg.reward_list[msg.reward_num].item_id = m_param.reward_list[i].item_id;
		msg.reward_list[msg.reward_num].item_num = m_param.reward_list[i].num;
		msg.reward_list[msg.reward_num].is_bind = m_param.reward_list[i].is_bind;
		msg.reward_list[msg.reward_num].param_length = 0;
		msg.reward_num += 1;
	}

	int send_length = sizeof(msg) - sizeof(msg.reward_list) + msg.reward_num * sizeof(msg.reward_list[0]);
	EngineAdapter::Instance().NetSendSerializeMsg(m_mgr->GetRole()->GetNetId(), &msg);
}

void PetMonopoly::SendExpectPetChangeNotice()
{
	Protocol::SCPetMonopolyExpectPetChangeNotice msg;
	msg.gold_expect_pet_id = m_param.expect_gold_pet_id;
	msg.silver_expect_pet_id = m_param.expect_silver_pet_id;

	m_mgr->NetSend(&msg, sizeof(msg));
}

void PetMonopoly::SendCurGridChangeNotice()
{
	if (m_param.cur_grid_idx < 0 || m_param.cur_grid_idx >= PET_MONOPOLY_GRID_NUM)
	{
		return;
	}
	int grid_idx = m_param.cur_grid_idx;

	Protocol::SCPetMonopolyCurGridChangeNotice msg;
	msg.cur_grid_idx = grid_idx;

	msg.grid.grid_type = m_param.grid_list[grid_idx].grid_type;
	msg.grid.reward_type = m_param.grid_list[grid_idx].reward_type;

	if (PET_MONOPOLY_GRID_REWARD_ITEM == msg.grid.reward_type ||
		PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[grid_idx].grid_detail.item.item_id;
		msg.grid.param2 = m_param.grid_list[grid_idx].grid_detail.item.item_num;
	}
	else if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[grid_idx].grid_detail.pet.pet_id;
		msg.grid.param2 = 0;
	}
	else if (PET_MONOPOLY_GRID_REWARD_OPPORTUNITY == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[grid_idx].grid_detail.special_item.special_item_id;
		msg.grid.param2 = 1;
	}
	else
	{
		msg.grid.param1 = 0;
		msg.grid.param2 = 0;
	}

	m_mgr->NetSend(&msg, sizeof(msg));
}

void PetMonopoly::SendCurGridChangeNotice(int cur_grid_idx)
{
	if (cur_grid_idx < 0 || cur_grid_idx >= PET_MONOPOLY_GRID_NUM)
	{
		return;
	}

	Protocol::SCPetMonopolyCurGridChangeNotice msg;
	msg.cur_grid_idx = cur_grid_idx;

	msg.grid.grid_type = m_param.grid_list[cur_grid_idx].grid_type;
	msg.grid.reward_type = m_param.grid_list[cur_grid_idx].reward_type;

	if (PET_MONOPOLY_GRID_REWARD_ITEM == msg.grid.reward_type ||
		PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[cur_grid_idx].grid_detail.item.item_id;
		msg.grid.param2 = m_param.grid_list[cur_grid_idx].grid_detail.item.item_num;
	}
	else if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[cur_grid_idx].grid_detail.pet.pet_id;
		msg.grid.param2 = 0;
	}
	else if (PET_MONOPOLY_GRID_REWARD_OPPORTUNITY == msg.grid.reward_type)
	{
		msg.grid.param1 = m_param.grid_list[cur_grid_idx].grid_detail.special_item.special_item_id;
		msg.grid.param2 = 1;
	}
	else
	{
		msg.grid.param1 = 0;
		msg.grid.param2 = 0;
	}

	m_mgr->NetSend(&msg, sizeof(msg));
}

void PetMonopoly::SendEnterNextLevelNotice(int next_level)
{
	Protocol::SCPetMonopolyEnterNextLevelNotice notice;
	notice.next_level = next_level;

	m_mgr->NetSend(&notice, sizeof(notice));
}

void PetMonopoly::SendTurnTableRewards()
{
	Protocol::SCPetMonopolyTurntable table;
	for (int i = 0; i < PET_MONOPOLY_TURNTABLE_SECTOR_NUM; ++i)
	{
		table.sector_list[i].reward_type = m_param.turntable_sector_list[i].reward_type;
		if (PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT == table.sector_list[i].reward_type ||
			PET_MONOPOLY_GRID_REWARD_ITEM == table.sector_list[i].reward_type)
		{
			table.sector_list[i].param1 = m_param.turntable_sector_list[i].reward_detail.item.item_id;
			table.sector_list[i].param2 = m_param.turntable_sector_list[i].reward_detail.item.item_num;
			table.sector_list[i].reserve_sh = 0;
		}
		else if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == table.sector_list[i].reward_type)
		{
			table.sector_list[i].param1 = m_param.turntable_sector_list[i].reward_detail.pet.pet_id;
			table.sector_list[i].param2 = 0;
			table.sector_list[i].reserve_sh = 0;
		}
		else if (PET_MONOPOLY_GRID_REWARD_OPPORTUNITY == table.sector_list[i].reward_type)
		{
			table.sector_list[i].param1 = m_param.turntable_sector_list[i].reward_detail.special_item.special_item_id;
			table.sector_list[i].param2 = 1;
			table.sector_list[i].reserve_sh = 0;
		}
		else
		{
			table.sector_list[i].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
		}
	}

	m_mgr->NetSend(&table, sizeof(table));
}

void PetMonopoly::SendTurnTableResult()
{
	Protocol::SCPetMonopolyTurntableResult result;
	result.sector_idx = m_param.turntable_sector_idx;

	m_mgr->NetSend(&result, sizeof(result));
}

void PetMonopoly::SendSpecialItemNum(int item_type)
{
	Protocol::SCPetMonopolySpecialItem item_num;
	item_num.double_surplus = m_param.double_reward_surplus;
	item_num.use_item_type = item_type;

	for (int i = 0; i < PET_MONOPOLY_SPECIAL_ITEM_MAX; i++)
	{
		item_num.special_item_num[i] = m_param.special_items[i];
	}

	m_mgr->NetSend(&item_num,sizeof(item_num));
}

void PetMonopoly::SendFastMoveReward(int grid_index)
{
	if (!this->CheckFastMove(grid_index))
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.grid_list[grid_index].reward_type == PET_MONOPOLY_GRID_REWARD_OPPORTUNITY)
	{
		int i = m_param.grid_list[grid_index].grid_detail.special_item.special_item_id;

		int t_num = 1;
		if (m_param.double_reward_surplus > 0)
		{
			this->DoubleReward(t_num);
		}
		m_param.special_items[i] += t_num;

		m_param.add_exp_times++;

		if (m_param.add_exp_times > 0)
		{
			this->AddExp();
		}

		this->SendSpecialItemNum(-1);

		m_param.grid_list[grid_index].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;

		this->SendCurGridChangeNotice(grid_index);

		this->FastMoveConsume(grid_index);

		gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s role[%d,%s] obtain_special_item[%d num:%d] ", __FUNCTION__,
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(),i,t_num );

		return;
	}

	if (m_param.grid_list[grid_index].reward_type != PET_MONOPOLY_GRID_REWARD_ITEM &&
		m_param.grid_list[grid_index].reward_type != PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_GRID_NO_REWARD);
		return;
	}

	ItemConfigData item;
	item.item_id = m_param.grid_list[grid_index].grid_detail.item.item_id;
	item.num = m_param.grid_list[grid_index].grid_detail.item.item_num;
	item.is_bind = m_param.grid_list[grid_index].grid_detail.item.is_bind > 0;

	if (m_param.double_reward_surplus > 0)
	{
		this->DoubleReward(item.num);
	}

	++m_param.add_exp_times;
	if (m_param.add_exp_times > 0)
	{
		this->AddExp();
	}

	if (!m_mgr->GetKnapsack()->Put(item, PUT_REASON_PET_MONOPOLY))
	{
		m_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT);
	}

	m_param.grid_list[grid_index].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;

	this->AddRewardToRecord(item);

	this->SendCurGridChangeNotice(grid_index);

	this->FastMoveConsume(grid_index);

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s role[%d,%s] grid_idx[%d] item_id[%d] item_num[%d] is_bind[%d]",__FUNCTION__,
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), grid_index, item.item_id, item.num, item.is_bind);
}

void PetMonopoly::SetExpectPet(int pet_id)
{
	const PetCfg* cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!LOGIC_CONFIG->GetPetMonopolyConfig()->IsPetInPool(pet_id, cfg->quality))
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_PET_NOT_EXPECTABLE);
		return;
	}

	if (PET_QUALITY_SILVER == cfg->quality)
	{
		m_param.expect_silver_pet_id = pet_id;
	}
	else if (PET_QUALITY_GOLD == cfg->quality)
	{
		m_param.expect_gold_pet_id = pet_id;
	}
	else
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	this->SendExpectPetChangeNotice();

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::SetExpectPet role[%d,%s] expect_gold[%d] expect_silver[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.expect_gold_pet_id, m_param.expect_silver_pet_id);
}

void PetMonopoly::ClearExpectPet(bool is_gold)
{
	if (is_gold)
	{
		m_param.expect_gold_pet_id = 0;
	}
	else
	{
		m_param.expect_silver_pet_id = 0;
	}

	this->SendExpectPetChangeNotice();

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::ClearExpectPet role[%d,%s] expect_gold[%d] expect_silver[%d]", 
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.expect_gold_pet_id, m_param.expect_silver_pet_id);
}

void PetMonopoly::RollDice(int fix_dice /* = 0 */)
{
	if (PET_MONOPOLY_GRID_NUM - 1 == m_param.cur_grid_idx)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_ON_ENDING_POINT);
		return;
	}

	if (m_param.cur_grid_idx < 0 || m_param.cur_grid_idx >= PET_MONOPOLY_GRID_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_NONE)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_CUR_GRID_HAS_REWARD);
		return;
	}
	
	if (!fix_dice)
	{
		if (!m_mgr->GetRoleCrystalFb()->UseCrystalFbEnergy(LOGIC_CONFIG->GetPetMonopolyConfig()->GetOtherCfg().dice_cost_energy, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_PET_MONOPOLY))
		{
			return;
		}
	}

	int dice = 1 + RandomNum(6);
	if (fix_dice >= 1 && fix_dice <= 6)
	{
		dice = fix_dice;
	}
	else if (this->TryFixDiceForCatchPetGuide(&fix_dice))
	{
		dice = fix_dice;
	}

	if (dice + m_param.cur_grid_idx >= PET_MONOPOLY_GRID_NUM)
	{
		dice = PET_MONOPOLY_GRID_NUM - m_param.cur_grid_idx - 1;
	}

	m_param.cur_grid_idx += dice;
	m_param.add_exp_times++;

	Protocol::SCPetMonopolyRollDice resp;
	resp.cur_grid_idx = m_param.cur_grid_idx;
	resp.dice = dice;

	m_mgr->NetSend(&resp, sizeof(resp));

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::RollDice role[%d,%s] dice[%d] cur_grid[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), dice, m_param.cur_grid_idx);

	
	WorldStatus::Instance().LogFunctionStats(m_mgr->GetRole(), "PetMonopoly");

	EventHandler::Instance().OnPassPetMonopoly(m_mgr->GetRole());
}

bool PetMonopoly::TryFixDiceForCatchPetGuide(ARG_OUT int* fix_dice)
{
	if (m_param.cur_level != 1) return false; // 只有第一层才尝试帮玩家踩到宠物格点
	if (m_param.is_catch_pet_before != 0) return false; // 只有未进入过抓宠战斗的才帮其踩到宠物格点

	int cur_grid_idx = m_param.cur_grid_idx;
	for (int offset = 1; offset <= 6 && cur_grid_idx + offset < PET_MONOPOLY_GRID_NUM; ++offset)
	{
		int grid_idx = cur_grid_idx + offset;
		if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == m_param.grid_list[grid_idx].reward_type)
		{
			*fix_dice = offset;
			return true;
		}
	}

	return false;
}

void PetMonopoly::FetchGridReward()
{
	if (m_param.grid_list[m_param.cur_grid_idx].reward_type == PET_MONOPOLY_GRID_REWARD_OPPORTUNITY)
	{
		int i = m_param.grid_list[m_param.cur_grid_idx].grid_detail.special_item.special_item_id;

		int t_num = 1;
		if (m_param.double_reward_surplus > 0)
		{
			this->DoubleReward(t_num);
		}
		m_param.special_items[i] += t_num;

		if (m_param.add_exp_times > 0)
		{
			this->AddExp();
		}

		this->SendSpecialItemNum(-1);

		m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;

		this->SendCurGridChangeNotice();

		gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s role[%d,%s] obtain_special_item[%d num:%d] ", __FUNCTION__,
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), i, t_num);

		return;
	}

	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_ITEM &&
		m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_GRID_NO_REWARD);
		return;
	}

	ItemConfigData item;
	item.item_id = m_param.grid_list[m_param.cur_grid_idx].grid_detail.item.item_id;
	item.num = m_param.grid_list[m_param.cur_grid_idx].grid_detail.item.item_num;
	item.is_bind = m_param.grid_list[m_param.cur_grid_idx].grid_detail.item.is_bind > 0;

	if (m_param.double_reward_surplus > 0)
	{
		this->DoubleReward(item.num);
	}

	if (!m_mgr->GetKnapsack()->Put(item, PUT_REASON_PET_MONOPOLY))
	{
		m_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT);
	}

	if (m_param.add_exp_times > 0)
	{
		this->AddExp();
	}
	m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;

	this->AddRewardToRecord(item);

	this->SendCurGridChangeNotice();

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::FetchGridReward role[%d,%s] grid_idx[%d] item_id[%d] item_num[%d] is_bind[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.cur_grid_idx, item.item_id, item.num, item.is_bind);
}

void PetMonopoly::StartBattle()
{
	if (m_mgr->GetRole()->GetRoleStatusManager()->IsInBattleStatus())
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_BATTLE);
		return;
	}

	if (m_mgr->GetRole()->InTeam())
	{
		m_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return;
	}

	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_GRID_NO_PET_TO_CATCH);
		return;
	}

	int monster_group_id = m_param.grid_list[m_param.cur_grid_idx].grid_detail.pet.monster_group_id;
	if (1 == m_param.cur_level && 0 == m_param.is_catch_pet_before)
	{
		monster_group_id = LOGIC_CONFIG->GetPetMonopolyConfig()->GetOtherCfg().guide_pet_monster_group;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_mgr->GetRole(), BATTLE_MODE_PET_MONOPOLY, monster_group_id, 0, 0, true);

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::StartBattle role[%d,%s] cur_grid[%d] monster_group_id[%d] pet_id[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.cur_grid_idx,
		m_param.grid_list[m_param.cur_grid_idx].grid_detail.pet.monster_group_id,
		m_param.grid_list[m_param.cur_grid_idx].grid_detail.pet.pet_id);
}

void PetMonopoly::GenerateGridData(int level)
{
	const PetMonopolyLayerCfg* level_cfg = LOGIC_CONFIG->GetPetMonopolyConfig()->GetLowerBoundLayerCfg(level);
	if (NULL == level_cfg)
	{
		return;
	}

	for (int grid_idx = 0; grid_idx < PET_MONOPOLY_GRID_NUM; ++grid_idx)
	{
		bool ret = this->FillGridData(level_cfg->grid_list[grid_idx], m_param.grid_list[grid_idx], m_param.expect_silver_pet_id, m_param.expect_gold_pet_id);
		if (!ret) // 失败，就把这个格子置为空奖励
		{
			m_param.grid_list[grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
		}
	}

	m_param.cur_grid_idx = 0;
	m_param.cur_level = level;
	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::GenerateGridData role[%d,%s] start level [%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), level);

	for (int grid = 0; grid < PET_MONOPOLY_GRID_NUM; ++grid)
	{
		gamelog::g_log_pet_monopoly.printf(LL_INFO, "grid[%d] grid_type[%d] reward_type[%d] reward_detail[%s]",
			grid, m_param.grid_list[grid].grid_type, m_param.grid_list[grid].reward_type, 
			m_param.grid_list[grid].grid_detail.GetDetailDataLog(m_param.grid_list[grid].reward_type));
	}
}

bool PetMonopoly::FillGridData(const PetMonopolyLayerGridCfg& cfg, PetMonopolyGrid& grid, int expect_silver_pet_id, int expect_gold_pet_id)
{
	grid.grid_type = cfg.grid_type;
	grid.reward_type = cfg.reward_type;
	grid.version = cfg.version;
	grid.grid_detail.Reset();

	switch (cfg.reward_type)
	{
	case PET_MONOPOLY_GRID_REWARD_ITEM:
		{
			int reward_group_id = cfg.param;
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandItemByRewardGroupId(reward_group_id, &grid.grid_detail.item))
			{
				return false;
			}
		}
		break;

	case PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT:
		{
			int pet_quality = cfg.param;
			int specific_pet = cfg.param2;
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandPetFragmentByPetQuality(pet_quality, specific_pet, expect_silver_pet_id, expect_gold_pet_id, &grid.grid_detail.item))
			{
				return false;
			}
		}
		break;

	case PET_MONOPOLY_GRID_REWARD_PET_MONSTER:
		{
			int silver_pet_change = cfg.param;
			int specific_pet = cfg.param2;
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandPetMonsterByPetQuality(silver_pet_change, specific_pet, expect_silver_pet_id, expect_gold_pet_id, &grid.grid_detail.pet))
			{
				return false;
			}
		}
		break;

	case PET_MONOPOLY_GRID_REWARD_TURNTABLE:
		{
			int turntable_group_id = cfg.param;
			grid.grid_detail.turntable.reward_group_id = turntable_group_id;
		}
		break;

	case PET_MONOPOLY_GRID_REWARD_OPPORTUNITY:
		{
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandSpecialItem(&grid.grid_detail.special_item))
			{
				return false;
			}
		}
		break;
	}

	return true;
}

void PetMonopoly::CheckGridVersion()
{
	const PetMonopolyLayerCfg* level_cfg = LOGIC_CONFIG->GetPetMonopolyConfig()->GetLowerBoundLayerCfg(m_param.cur_level);
	if (NULL == level_cfg)
	{
		return;
	}

	for (int grid_idx = 0; grid_idx < PET_MONOPOLY_GRID_NUM; ++grid_idx)
	{
		if (PET_MONOPOLY_GRID_REWARD_NONE == m_param.grid_list[grid_idx].reward_type) continue;

		if (m_param.grid_list[grid_idx].version >= level_cfg->grid_list[grid_idx].version) continue;
		
		bool ret = this->FillGridData(level_cfg->grid_list[grid_idx], m_param.grid_list[grid_idx], m_param.expect_silver_pet_id, m_param.expect_gold_pet_id);
		if (!ret) // 失败，就把这个格子置为空奖励
		{
			m_param.grid_list[grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
			continue;
		}

		if (PET_MONOPOLY_GRID_NUM - 1 == grid_idx &&
			PET_MONOPOLY_GRID_REWARD_TURNTABLE == m_param.grid_list[grid_idx].reward_type)
		{
			// 如果是最后一格 且最后一格是转盘，则重置转盘奖励
			m_param.turntable_sector_idx = -1;
			this->GenerateTurnTableRewards();
		}
	}
}

void PetMonopoly::AddRewardToRecord(const ItemConfigData& item)
{
	for (int i = 0; i < m_param.reward_record_num; ++i)
	{
		if (item.item_id == m_param.reward_list[i].item_id
			&& (item.is_bind ? 1 : 0) == m_param.reward_list[i].is_bind)
		{
			m_param.reward_list[i].num += item.num;

			gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::AddRewardToRecord Merge role[%d,%s] item[%d] num[%d] isbind[%d]",
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(), item.item_id, item.num, item.is_bind ? 1 : 0);
			return;
		}
	}

	if (m_param.reward_record_num >= ARRAY_ITEM_COUNT(m_param.reward_list))
	{
		return;
	}

	m_param.reward_list[m_param.reward_record_num].Reset();
	m_param.reward_list[m_param.reward_record_num].item_id = item.item_id;
	m_param.reward_list[m_param.reward_record_num].is_bind = item.is_bind ? 1 : 0;
	m_param.reward_list[m_param.reward_record_num].num = item.num;
	m_param.reward_record_num += 1;

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::AddRewardToRecord role[%d,%s] item[%d] num[%d] isbind[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), item.item_id, item.num, item.is_bind ? 1 : 0);
}

void PetMonopoly::GmRollDice(int fix_dice)
{
	this->RollDice(fix_dice);
}

void PetMonopoly::GmJumpToLayer(int layer)
{
	int enter_level = layer;
	this->GenerateGridData(enter_level); // 进入下一层
	m_param.reward_record_num = 0; // 进入下层后清空奖励列表
	m_param.turntable_sector_idx = -1;
	EventHandler::Instance().OnEnterPetMonopoly(m_mgr->GetRole(), m_param.cur_level);

	this->SendEnterNextLevelNotice(enter_level);
	this->SendInfo();
}

void PetMonopoly::GmUseSpecialItem(int special_item_id, int item_param)
{
	for (int i=0;i< PET_MONOPOLY_SPECIAL_ITEM_MAX ;++i)
	{
		m_param.special_items[i] += 10;
	}

	this->UseSpecialItem(special_item_id, item_param);
}

void PetMonopoly::AddPetToRecord(const ItemDataWrapper& item_wrapper)
{
	if (m_param.pet_record_num >= ARRAY_ITEM_COUNT(m_param.pet_list))
	{
		return;
	}

	m_param.pet_list[m_param.pet_record_num] = item_wrapper;
	m_param.pet_record_num += 1;

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::AddPetToRecord role[%d,%s] pet[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), item_wrapper.item_id);
}

void PetMonopoly::EnterNextLevel()
{
	if (m_param.cur_grid_idx != PET_MONOPOLY_GRID_NUM - 1)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_NOT_ON_ENDING_POINT);
		return;
	}

	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_NONE)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_CUR_GRID_HAS_REWARD);
		return;
	}

	int remain_add_exp_times = m_param.add_exp_times;
	if (remain_add_exp_times > 0)
	{
		gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::EnterNextLevel Remain_Add_Exp_Times > 0, role[%d,%s] remain_times[%d]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName(), remain_add_exp_times);
		for (int i = 0; i < remain_add_exp_times; ++i)
		{
			this->AddExp();
		}
	}
	
	int enter_level = m_param.cur_level + 1;
	this->GenerateGridData(enter_level); // 进入下一层
	m_param.reward_record_num = 0; // 进入下层后清空奖励列表
	EventHandler::Instance().OnEnterPetMonopoly(m_mgr->GetRole(), m_param.cur_level);

	this->SendEnterNextLevelNotice(enter_level);
	this->SendInfo();
}

bool PetMonopoly::GenerateTurnTableRewards()
{
	if (m_param.turntable_sector_idx != -1)
	{
		gamelog::g_log_pet_monopoly.printf(LL_WARNING, "GenerateTurnTableRewards Already Gen Rewards, role[%d,%s]",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName());
		return true;
	}

	int grid_idx = PET_MONOPOLY_GRID_NUM - 1; // 不用当前格点，客户端要求在任何时候都可以看到奖励
	if (m_param.grid_list[grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_TURNTABLE)
	{
		gamelog::g_log_pet_monopoly.printf(LL_WARNING, "GenerateTurnTableRewards Error role[%d,%s] last grid not turntable",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName());
		return false;
	}

	int turntable_group_id = m_param.grid_list[grid_idx].grid_detail.turntable.reward_group_id;
	const PetMonopolyTurnTableGroup* group = LOGIC_CONFIG->GetPetMonopolyConfig()->GetTurntableGroup(turntable_group_id);
	if (NULL == group)
	{
		gamelog::g_log_pet_monopoly.printf(LL_WARNING, "GenerateTurnTableRewards Error role[%d,%s] turntable group not found",
			m_mgr->GetRoleUID(), m_mgr->GetRoleName());
		return false;
	}

	int previous_rate_sum = 0;
	int r = RandomNum(group->total_rate);
	int bingo_idx = -1;
	for (int i = 0; i < group->sector_num; ++i)
	{
		if (PET_MONOPOLY_TURNTABLE_REWARD_ITEM == group->sector_list[i].reward_type)
		{
			m_param.turntable_sector_list[i].reward_type = PET_MONOPOLY_GRID_REWARD_ITEM;
			m_param.turntable_sector_list[i].reserve_sh = 0;
			m_param.turntable_sector_list[i].reward_detail.item.item_id = group->sector_list[i].item.item_id;
			m_param.turntable_sector_list[i].reward_detail.item.item_num = group->sector_list[i].item.num;
			m_param.turntable_sector_list[i].reward_detail.item.is_bind = group->sector_list[i].item.is_bind ? 1 : 0;
			m_param.turntable_sector_list[i].reward_detail.item.reserve_ch = 0;
			m_param.turntable_sector_list[i].reward_detail.item.reserve_sh = 0;

			if (bingo_idx == -1 && (r < group->sector_list[i].rate + previous_rate_sum || i == group->sector_num - 1))
			{
				bingo_idx = i;
			}
		}
		else if (PET_MONOPOLY_TURNTABLE_REWARD_PET_FRAGMENT == group->sector_list[i].reward_type)
		{
			int pet_quality = group->sector_list[i].param;
			PetMonopolyGridItem item;
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandPetFragmentByPetQuality(pet_quality, 0, m_param.expect_silver_pet_id, m_param.expect_gold_pet_id, &item))
			{
				gamelog::g_log_pet_monopoly.printf(LL_WARNING, "GenerateTurnTableRewards Error role[%d,%s] pet fragment random failed!",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName());
				return false; // 即使在一半的时候返回也可以在下次登录时重新转盘
			}
			m_param.turntable_sector_list[i].reward_type = PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT;
			m_param.turntable_sector_list[i].reserve_sh = 0;
			if (m_param.double_reward_surplus > 0)
			{
				this->DoubleReward((int &)item.item_num);
			}
			m_param.turntable_sector_list[i].reward_detail.item = item;

			if (bingo_idx == -1 && (r < group->sector_list[i].rate + previous_rate_sum || i == group->sector_num - 1))
			{
				bingo_idx = i;
			}
		}
		else if (PET_MONOPOLY_TURNTABLE_REWARD_PET_MONSTER == group->sector_list[i].reward_type)
		{
			int silver_pet_chance = group->sector_list[i].param;
			PetMonopolyGridPet pet;
			if (!LOGIC_CONFIG->GetPetMonopolyConfig()->RandPetMonsterByPetQuality(silver_pet_chance, 0, m_param.expect_silver_pet_id, m_param.expect_gold_pet_id, &pet))
			{
				gamelog::g_log_pet_monopoly.printf(LL_WARNING, "GenerateTurnTableRewards Error role[%d,%s] pet monster random failed!",
					m_mgr->GetRoleUID(), m_mgr->GetRoleName());
				return false;
			}
			m_param.turntable_sector_list[i].reward_type = PET_MONOPOLY_GRID_REWARD_PET_MONSTER;
			m_param.turntable_sector_list[i].reserve_sh = 0;
			m_param.turntable_sector_list[i].reward_detail.pet = pet;

			if (bingo_idx == -1 && (r < group->sector_list[i].rate + previous_rate_sum || i == group->sector_num - 1))
			{
				bingo_idx = i;
			}
		}

		previous_rate_sum += group->sector_list[i].rate;
	}

	m_param.turntable_sector_idx = bingo_idx;

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::GenerateTurnTableRewards role[%d,%s] sector_idx[%d]", m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.turntable_sector_idx);

	for (int i = 0; i < PET_MONOPOLY_TURNTABLE_SECTOR_NUM; ++i)
	{
		int reward_type = m_param.turntable_sector_list[i].reward_type;
		gamelog::g_log_pet_monopoly.printf(LL_INFO, "Sector[%d] RewardType[%d] Reward[%s]",
			i, reward_type, m_param.turntable_sector_list[i].reward_detail.GetDetailDataLog(reward_type));
	}

	return true;
}

void PetMonopoly::ShowTurnTableRewards()
{
	if (-1 == m_param.turntable_sector_idx)
	{
		if (!this->GenerateTurnTableRewards())
		{
			return;
		}
	}

	this->SendTurnTableRewards();
}

void PetMonopoly::ClearTurnTableData()
{
	m_param.turntable_sector_idx = -1;
	for (int i = 0; i < PET_MONOPOLY_TURNTABLE_SECTOR_NUM; ++i)
	{
		m_param.turntable_sector_list[i].reward_type = 0;
		m_param.turntable_sector_list[i].reserve_sh = 0;
		m_param.turntable_sector_list[i].reward_detail.Reset();
	}
}

void PetMonopoly::ShowTurnTableResult()
{
	if (-1 == m_param.turntable_sector_idx)
	{
		if (!this->GenerateTurnTableRewards())
		{
			m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_TURNTABLE_GEN_FAILED);
			return;
		}
	}

	this->SendTurnTableResult();
}

void PetMonopoly::TurnTableEnd()
{
	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_TURNTABLE)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_CUR_GRID_NO_TURNTABLE);
		return;
	}

	if (-1 == m_param.turntable_sector_idx)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	PetMonopolySectorInfo& sector = m_param.turntable_sector_list[m_param.turntable_sector_idx];
	switch (sector.reward_type)
	{
	case PET_MONOPOLY_GRID_REWARD_PET_MONSTER:
		{
			m_param.grid_list[m_param.cur_grid_idx].grid_detail = sector.reward_detail;
			m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_PET_MONSTER;

			this->SendCurGridChangeNotice();
		}
		break;

	case PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT:
	case PET_MONOPOLY_GRID_REWARD_ITEM:
		{
			ItemConfigData item;
			item.item_id = sector.reward_detail.item.item_id;
			item.num = sector.reward_detail.item.item_num;
			if (m_param.double_reward_surplus > 0)
			{
				this->DoubleReward(item.num);
			}
			item.is_bind = sector.reward_detail.item.is_bind > 0;
			if (!m_mgr->GetKnapsack()->Put(item, PUT_REASON_PET_MONOPOLY))
			{
				m_mgr->GetKnapsack()->SendMail(&item, 1);
			}

			m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
			m_param.grid_list[m_param.cur_grid_idx].grid_detail.Reset();

			this->SendCurGridChangeNotice();
		}
		break;
	case PET_MONOPOLY_GRID_REWARD_OPPORTUNITY:
		{

			int i = m_param.grid_list[m_param.cur_grid_idx].grid_detail.special_item.special_item_id;
			
			int item_num = 1;
			if (m_param.double_reward_surplus > 0)
			{
				this->DoubleReward(item_num);
			}
			m_param.special_items[i] += item_num;

			this->SendSpecialItemNum(-1);

			m_param.grid_list[m_param.cur_grid_idx].reward_type = PET_MONOPOLY_GRID_REWARD_NONE;
			m_param.grid_list[m_param.cur_grid_idx].grid_detail.Reset();

			gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s role[%d,%s] obtain_special_item[%d num:%d] ", __FUNCTION__,
				m_mgr->GetRoleUID(), m_mgr->GetRoleName(), i, item_num);

			this->SendCurGridChangeNotice();
		}
		break;

	default:
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		}
		break;
	}

	this->ClearTurnTableData();
}

void PetMonopoly::AddExp()
{
	{	// 策划要求屏蔽加经验
		m_param.add_exp_times = 0;
		return;		
	}

	if (m_param.add_exp_times <= 0)
	{
		m_mgr->NoticeNum(errornum::EN_CLOUD_ARENA_NO_ADD_EXP_TIMES);
		gamelog::g_log_pet_monopoly.printf(LL_WARNING, "PetMonopoly::AddExp No Times Stil Request, role[%d,%s]", m_mgr->GetRoleUID(), m_mgr->GetRoleName());
		return;
	}

	m_param.add_exp_times -= 1;

	//int add_exp_num = LOGIC_CONFIG->GetPetMonopolyConfig()->GetAddExpNum(m_mgr->GetRole()->GetLevel());
	int add_exp_num = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetExpCfg(m_mgr->GetRole()->GetLevel()) * LOGIC_CONFIG->GetPetMonopolyConfig()->GetOtherCfg().dice_cost_energy;
	long long actual_exp = m_mgr->GetRole()->AddExp(add_exp_num, ADD_EXP_TYPE_PET_MONOPOLY, __FUNCTION__);
	m_mgr->GetLevelComplement()->OnAddExp(actual_exp);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_mgr->GetRole(), add_exp_num, __FUNCTION__);

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::AddExp role[%d,%s] role_level[%d] add_exp_num[%d] remain_times[%d]",
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_mgr->GetRole()->GetLevel(), add_exp_num, m_param.add_exp_times);

	{
		ItemConfigData item;
		item.item_id = ItemNamespace::VIRTUAL_ITEM_EXP;
		item.num = static_cast<int>(actual_exp);
		item.is_bind = false;
		this->AddRewardToRecord(item);
	}
}



void PetMonopoly::UseSpecialItem(int special_item_id,int item_parpam /*= 0*/)
{
	if (special_item_id < 0 || special_item_id >= PET_MONOPOLY_SPECIAL_ITEM_MAX)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_param.special_items[special_item_id] <= 0)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_LACK_SPECIAL_ITEM);
		return;
	}

	switch (special_item_id)
	{
	case PET_MONOPOLY_SPECIAL_ITEM_LUBANDICE:
		{
			this->UseLubanDice(item_parpam);
		}
		break;

	case PET_MONOPOLY_SPECIAL_ITEM_DOUBLECARD:
		{
			this->UseDoubleCard();
		}
		break;

	case PET_MONOPOLY_SPECIAL_ITEM_FASTMOVEPAPER:
		{
			this->UseFastMovePaper();
		}
		break;

	case PET_MONOPOLY_SPECIAL_ITEM_WISHCARD:
		{
			this->UseWishCard(item_parpam);
		}
		break;

	case PET_MONOPOLY_SPECIAL_ITEM_PORTAL:
		{
			this->UsePortal();
		}
		break;
	}
}

void PetMonopoly::SpecialItemConsume(int special_item_type)
{
	--m_param.special_items[special_item_type];
	this->SendSpecialItemNum(special_item_type);

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s[item_type:%d] role[%d,%s] item_surplus[%d]",__FUNCTION__,
		special_item_type, m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.special_items[special_item_type]);
}

void PetMonopoly::UseLubanDice(int fix_dice)
{
	if ( fix_dice < 1 || fix_dice>6 )
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (PET_MONOPOLY_GRID_NUM - 1 == m_param.cur_grid_idx)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_ON_ENDING_POINT);
		return;
	}

	this->SpecialItemConsume(PET_MONOPOLY_SPECIAL_ITEM_LUBANDICE);
	this->RollDice(fix_dice);
}

void PetMonopoly::UseFastMovePaper()
{
	int forward_grid = LOGIC_CONFIG->GetPetMonopolyConfig()->GetSpecialItemParam(PET_MONOPOLY_SPECIAL_ITEM_FASTMOVEPAPER);

	
	if (PET_MONOPOLY_GRID_NUM - 1 == m_param.cur_grid_idx)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_ON_ENDING_POINT);
		return;
	}

	if (m_param.cur_grid_idx < 0 || m_param.cur_grid_idx >= PET_MONOPOLY_GRID_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (m_param.grid_list[m_param.cur_grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_NONE)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_CUR_GRID_HAS_REWARD);
		return;
	}

	this->SpecialItemConsume(PET_MONOPOLY_SPECIAL_ITEM_FASTMOVEPAPER);

	int i = 0;
	while (forward_grid--)
	{
		m_param.cur_grid_idx += 1;
			
		m_param.fast_move_num[i++] = m_param.cur_grid_idx;

		//遇到宠物封印或者到达终点都停止
		if (PET_MONOPOLY_GRID_REWARD_PET_MONSTER == m_param.grid_list[m_param.cur_grid_idx].reward_type) break;   
		if (PET_MONOPOLY_GRID_NUM - 1 == m_param.cur_grid_idx) break;
	}

	for (int i = 0; i < PET_MONOPOLY_MAX_FAST_MOVE_GRID; ++i)
	{
		if (m_param.fast_move_num[i] == m_param.cur_grid_idx)
		{
			m_param.fast_move_num[i] = -1;
		}
	}

	m_param.add_exp_times += 1;

	Protocol::SCPetMonopolyRollDice resp;
	resp.cur_grid_idx = m_param.cur_grid_idx;
	resp.dice = -2;

	m_mgr->NetSend(&resp, sizeof(resp));
}

void PetMonopoly::UseWishCard(int wish_item_seq)
{
	if (wish_item_seq <= 0 || wish_item_seq > PetMonopolyConfig::MAX_WISH_CARD_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const PetMonopolyWishCard* t_wish_item = LOGIC_CONFIG->GetPetMonopolyConfig()->GetWishCardReward(wish_item_seq);
	if (NULL == t_wish_item)
	{
		return;
	}

	int grid_num = LOGIC_CONFIG->GetPetMonopolyConfig()->GetSpecialItemParam(PET_MONOPOLY_SPECIAL_ITEM_WISHCARD);

	Protocol::SCPetMonopolyWishCardGrid wish_card; // 3670

	std::vector<int> reward_grid_allidx;
	for (int grid_idx = 1; grid_idx < PET_MONOPOLY_GRID_NUM-1; ++grid_idx)
	{
		if (m_param.grid_list[grid_idx].reward_type != PET_MONOPOLY_GRID_REWARD_NONE)
		{
			if (grid_idx == m_param.cur_grid_idx) continue;	
			reward_grid_allidx.push_back(grid_idx);
		}
	}

	if (0 == reward_grid_allidx.size())
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_NO_REWARD_GRID);
		return;
	}

	this->SpecialItemConsume(PET_MONOPOLY_SPECIAL_ITEM_WISHCARD);

	int i = 0;	//计数器
	while (grid_num > 0)
	{
		if (0 == reward_grid_allidx.size())
		{
			break;
		}
		int rand_idx = RandomNum(reward_grid_allidx.size());
		int reward_grid_idx = reward_grid_allidx[rand_idx];		//随机出来的格子idx

		wish_card.grid_idx[i] = reward_grid_idx;

		wish_card.item_grid[i].reward_type = m_param.grid_list[reward_grid_idx].reward_type = t_wish_item->reward_type; 
		wish_card.item_grid[i].grid_type = m_param.grid_list[reward_grid_idx].grid_type;
		wish_card.item_grid[i].param1 = m_param.grid_list[reward_grid_idx].grid_detail.item.item_id = t_wish_item->item.item_id;
		wish_card.item_grid[i].param2 = m_param.grid_list[reward_grid_idx].grid_detail.item.item_num = t_wish_item->item.num;

		std::vector<int>::iterator it = reward_grid_allidx.begin() + rand_idx;
		reward_grid_allidx.erase(it);

		++i;
		--grid_num;
	}
	
	wish_card.replace_num = i;

	m_mgr->NetSend(&wish_card, sizeof(wish_card));
}

void PetMonopoly::UsePortal()
{
	if ((PET_MONOPOLY_GRID_NUM - 1) == m_param.cur_grid_idx)
	{
		m_mgr->NoticeNum(errornum::EN_PET_MONOPOLY_ON_ENDING_POINT);
		return;
	}

	this->SpecialItemConsume(PET_MONOPOLY_SPECIAL_ITEM_PORTAL);

	int portal_grid_idx = 1+ RandomNum(m_param.cur_grid_idx, PET_MONOPOLY_GRID_NUM - 1) ;

	Protocol::SCPetMonopolyRollDice resp;
	resp.cur_grid_idx = m_param.cur_grid_idx = portal_grid_idx;
	resp.dice = -4;

	m_param.add_exp_times++;

	m_mgr->NetSend(&resp, sizeof(resp));
}

void PetMonopoly::UseDoubleCard()
{
	int double_reward_amount = LOGIC_CONFIG->GetPetMonopolyConfig()->GetSpecialItemParam(PET_MONOPOLY_SPECIAL_ITEM_DOUBLECARD);

	m_param.double_reward_surplus += double_reward_amount;

	this->SpecialItemConsume(PET_MONOPOLY_SPECIAL_ITEM_DOUBLECARD);
}

bool PetMonopoly::DoublePetReward()
{
	if (m_param.double_reward_surplus > 0)
	{
		--m_param.double_reward_surplus;
		this->SendSpecialItemNum(-1);
		return true;
	}

	return false;
}

void PetMonopoly::DoubleReward(int& item_num)
{
	--m_param.double_reward_surplus;
	item_num *= 2;
	
	this->SendSpecialItemNum(-1);

	gamelog::g_log_pet_monopoly.printf(LL_INFO, "PetMonopoly::%s role[%d,%s] double_reward_surplus[%d]", __FUNCTION__,
		m_mgr->GetRoleUID(), m_mgr->GetRoleName(), m_param.double_reward_surplus);
}

bool PetMonopoly::CheckFastMove(int grid_index)
{
	for (int i = 0; i < PET_MONOPOLY_MAX_FAST_MOVE_GRID; ++i)
	{
		if (grid_index == m_param.fast_move_num[i])
		{
			return true;
		}
	}

	return false;
}

void PetMonopoly::FastMoveConsume(int grid_index)
{
	for (int i = 0; i < PET_MONOPOLY_MAX_FAST_MOVE_GRID; ++i)
	{
		if (grid_index == m_param.fast_move_num[i])
		{
			m_param.fast_move_num[i] = -1;
		}
	}
}
