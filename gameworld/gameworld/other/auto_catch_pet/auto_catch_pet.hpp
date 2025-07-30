#ifndef __AUTO_CATCH_PET_HPP__
#define __AUTO_CATCH_PET_HPP__

#include "servercommon/auto_catch_pet_def.hpp"
#include "protocol/msgother.h"

class RoleModuleManager;
class AutoCatchPet
{
public:
	AutoCatchPet();
	~AutoCatchPet();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const AutoCatchPetParam& param);
	void GetInitParam(ARG_OUT AutoCatchPetParam* out_param);

	const AutoCatchPetParam& GetAutoCatchParam() const { return m_param; }

	void SetAutoCatchPetInfo(Protocol::CSSetAutoCatchPet* req);
	void SendAutoCatchInfo();

	void OnAutoCatchPet(int monster_id, ItemID seal_card_item_id, bool is_succ);

private:
	RoleModuleManager* m_mgr;
	AutoCatchPetParam m_param;
};

#endif