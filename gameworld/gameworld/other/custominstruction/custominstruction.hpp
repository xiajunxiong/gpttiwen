#ifndef __CUSTOM_INSTRUCTION_HPP__
#define __CUSTOM_INSTRUCTION_HPP__

#include "servercommon/custominstructiondef.hpp"

class RoleModuleManager;
class CustomInstruction
{
public:
	CustomInstruction();
	~CustomInstruction();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const CustomInstructionParam & param);
	void GetInitParam(CustomInstructionParam * param);

	void AddCustomInstructionReq(const char* istr, int istr_len);
	void RemoveCustomInstructionReq(int index);
	void BatchRemoveCustomInstructionReq(int num, const short* index_list);
	void OnBattleSetCustomInstructionReq(short index, short grid_idx);

	void SendInstructionInfo();
private:
	RoleModuleManager* m_mgr;
	CustomInstructionParam m_param;
};

#endif