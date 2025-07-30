#ifndef __FUNC_GUIDE_H__
#define __FUNC_GUIDE_H__

#include "servercommon/funcguidedef.h"

class RoleModuleManager;
class FuncGuide		// 功能引导
{
public:
	FuncGuide();
	~FuncGuide();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_mgr; }

	void InitFuncGuideInfo(RoleModuleManager * role_mgr, const FuncGuideParam & param);
	void GetFuncGuideParam(FuncGuideParam * out_param);

	void OnFuncGuideLogin();
	int GetPetChoice() const { return m_param.choose_pet_seq; }

	void ChooseNewbiePet(int choice);
	void OnChallengeColosseum();
	bool HasChallengeColosseumBefore();

	void OnDrawGuidePet();
	bool IsDrawGuidePet();

	void OnDrawGuidePet2();
	bool IsDrawGuidePet2();

	void OnDrawGuidePartner();
	bool IsDrawGuidePartner();

	void OnDrawGuidePartner2();
	bool IsDrawGuidePartner2();

	void OnCatchPetSucc();
	bool HasCatchPetBefore();
	void StartCatchPetGuideBattle();

	void FinishCloudArenaGuide();
	void FinishLunhuiWangchuanGuide();
	void OnFastDreamNotesChallenge(int level);
	void OnPassLunHuiWangChuan(int level);

	void OnRoleTriggerPetDeadSummonGuide();
	
	void GmSetAllFlag();
private:
	RoleModuleManager * m_mgr;
	FuncGuideParam m_param;

	void SendEventFlag();
	void SendRolePetDeadSummonGuideInfo();
};

#endif