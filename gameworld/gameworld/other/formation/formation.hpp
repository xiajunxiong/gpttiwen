#ifndef __FORMATION_HPP__
#define __FORMATION_HPP__

#include "servercommon/formationdef.hpp"

namespace Protocol {
	class CSFormationSave;
}

class RoleModuleManager;
class RoleFormation
{
public:
	RoleFormation();
	~RoleFormation();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const FormationParam& param);
	void GetInitParam(FormationParam* out_param);

	void RenameFormationReq(int formation_seq, GameName new_name);
	bool SaveFormationReq(Protocol::CSFormationSave* req);
	void FormationDetailInfoReq(int formation_seq);
	void DeleteFormationReq(int formation_seq);
	void OnChangeProf(int old_profession, int new_profession);
	bool IsHasSetAutoSkill();

	void SynAutoSkill(FormationAutoInfo& info,int pet_id);
	void SynBattleAutoSkill(bool is_first_move, short move_type, UInt16 skill_id, short skill_level, int pet_id, long long pet_unique_id);	// is_first_move == true时,pet_id pet_unique_id无效
	void SynPetAutoSkill();		// 在宠物界面修改出战宠物后,技能id同步过来

	void SendSimpleInfo();
	void SendDetailInfo(int formation_seq, int reason);

	FormationInfo* GetCurFormationInfo();
	void GetCurFormationPartnerPosList(int max_num, ARG_OUT int* out_num, ARG_OUT int* out_partner_list, ARG_OUT int* out_pos_list);

	void OnPartnerFightStateChange();
private:
	void SyncFormationFromRole(int formation_seq);
	void SyncFormationToRole(int formation_seq, bool is_new_formation);
	void SendFormationRenameNotice(int formation_seq);
	void CheckSkillLevel();
	bool CheckGridIsPet(const FormationGrid& grid, int pet_id, long long pet_unique_id);
	bool IsCurFormationSeqValid() const;
	void OnChangeProfResetFormationAutoMove();

	RoleModuleManager* m_mgr;
	FormationParam m_param;
};

#endif