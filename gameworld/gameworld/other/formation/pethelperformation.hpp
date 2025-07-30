#ifndef __PET_HELPER_FORMATION_HPP__
#define __PET_HELPER_FORMATION_HPP__

#include "servercommon/pethelperformationdef.hpp"
#include "formation.hpp"

namespace Protocol
{
	class CSSetPetHelperFormation;
}

class RoleModuleManager;
class Role;
class PetHelperFormation
{
public:
	PetHelperFormation();
	~PetHelperFormation();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const PetHelperFormationParam& param);
	void GetInitParam(ARG_OUT PetHelperFormationParam* param);

	void OnRoleSetFormation(Protocol::CSSetPetHelperFormation* req);
	bool IsFormationValid(int system_type);
	void OnRoleQueryFormation(int system_type);
	bool AssembleBattleData(int system_type, ARG_OUT RoleBattleData& battle_data, bool is_use_system_food);
	bool CollectBattleData(int battle_mode, ARG_OUT RoleBattleData& battle_data, bool is_use_system_food);

	void SynAutoSkill(FormationAutoInfo& info, int pet_id);
	void SynBattleAutoSkill(bool is_first_move, short move_type, UInt16 skill_id, short skill_level, int pet_id, long long pet_unique_id);	// is_first_move == true ±,pet_id pet_unique_idŒﬁ–ß

	void GmRandomFormation(int system_type);
private:
	void OnRoleSetCloudArenaFormation(Protocol::CSSetPetHelperFormation* req);
	void OnRoleSetCourageChallengeFormation(Protocol::CSSetPetHelperFormation* req);
	void OnRoleSetFormationCommon(Protocol::CSSetPetHelperFormation* req);
	void OnRoleSetRealmBreakFightFormation(Protocol::CSSetPetHelperFormation* req);
	void AssembleFormationFromProtocol(Protocol::CSSetPetHelperFormation* req);
	void SendFormation(int system_type);
	void CheckFormationRemoveInvalidGrid(PetHelperFormationInfo& info);
	void SetInitialFormation(PetHelperFormationInfo& info);
	bool CheckGridIsPet(const PetHelperFormationGrid& grid, int pet_id, long long pet_unique_id);

	RoleModuleManager* m_mgr;
	PetHelperFormationParam m_param;
};

#endif