#include "custominstruction.hpp"
#include "protocol/msgother.h"
#include "servercommon/servercommon.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "battle/battle_manager_local.hpp"

CustomInstruction::CustomInstruction() : m_mgr(NULL)
{

}

CustomInstruction::~CustomInstruction()
{

}

void CustomInstruction::Init(RoleModuleManager * module_mgr, const CustomInstructionParam & param)
{
	m_mgr = module_mgr;
	m_param = param;
}

void CustomInstruction::GetInitParam(CustomInstructionParam * param)
{
	if (NULL != param) *param = m_param;
}

void CustomInstruction::AddCustomInstructionReq(const char* istr, int istr_len)
{
	if (m_param.instruction_num >= CUSTOM_INSTRUCTION_NUM)
	{
		m_mgr->NoticeNum(errornum::EN_INSTRUCTION_FULL);
		return;
	}

	int len = GetMin(ARRAY_ITEM_COUNT(m_param.instruction_list[0].istr), istr_len);
	F_STRNCPY(m_param.instruction_list[m_param.instruction_num].istr, istr, len);
	m_param.instruction_num += 1;
	
	this->SendInstructionInfo();
}

void CustomInstruction::RemoveCustomInstructionReq(int index)
{
	if (index < 0 || index >= m_param.instruction_num)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	for (int i = index + 1; i < m_param.instruction_num; ++i)
	{
		m_param.instruction_list[i - 1] = m_param.instruction_list[i];
	}
	m_param.instruction_num -= 1;

	this->SendInstructionInfo();
}

void CustomInstruction::BatchRemoveCustomInstructionReq(int num, const short* index_list)
{
	for (int i = 0; i < num; ++i)
	{
		int index = index_list[i];
		if (index < 0 || index >= m_param.instruction_num)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	for (int i = 0; i < num; ++i)
	{
		int index = index_list[i];
		m_param.instruction_list[index].Reset();
	}

	int temp_num = 0;
	for (int i = 0; i < m_param.instruction_num; ++i)
	{
		if (strcmp(m_param.instruction_list[i].istr, "") != 0)
		{
			m_param.instruction_list[temp_num] = m_param.instruction_list[i];
			temp_num += 1;
		}
	}

	m_param.instruction_num = temp_num;

	this->SendInstructionInfo();
}

void CustomInstruction::OnBattleSetCustomInstructionReq(short index, short grid_idx)
{
	if (index < 0 || index >= m_param.instruction_num)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	BattleManagerLocal::GetInstance().RoleSetCustomInstructionReq(m_mgr->GetRole(), m_param.instruction_list[index].istr, grid_idx);
}

void CustomInstruction::SendInstructionInfo()
{
	Protocol::SCCustomInstructionInfo info;
	info.instruction_num = 0;
	for (int i = 0; i < m_param.instruction_num && info.instruction_num < ARRAY_ITEM_COUNT(info.instruction_list); ++i)
	{
		F_STRNCPY(info.instruction_list[info.instruction_num], m_param.instruction_list[i].istr, sizeof(info.instruction_list[0]));
		info.instruction_num += 1;
	}

	int len = sizeof(info) - sizeof(info.instruction_list) + info.instruction_num * sizeof(info.instruction_list[0]);
	m_mgr->NetSend(&info, len);
}

