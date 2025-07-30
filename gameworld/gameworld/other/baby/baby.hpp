#ifndef __BABY_HPP__
#define __BABY_HPP__

#include "servercommon/babydef.hpp"
#include "other/attributelist.hpp"
#include <set>

class Role;
class RoleModuleManager;

class Baby
{
public:
	Baby();
	~Baby();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager, const RoleBabyParam & param);
	void GetOtherInitParam(RoleBabyParam *param);
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);
	const AttributeList & ReCalcAttr(AttributeList & base_add, bool is_recalc);
	void OnRoleLogin();
	
	void ActiveBabySingle(char * baby_name);
	void ActiveBabyDuet(char * baby_name);
	void DuetCreateRegStage1();
	void RemoveBaby(int baby_index);	// ÒÅÆú±¦±¦
	void SendInfo();
	void AddPlayJiFen(int add_ji_fen);
	void UpLevel(int index);
	void ReName(int index,char * new_name);
	void Draw();
	void Contribution(int type);
	void GiveUpCurrCreateType();
	void SelectCreateType(int create_type);

	void OnDivorce();
	
	int GetBabyNum();
	void AddBaby(long long baby_uuid);
	bool IsMaxBaby();
	bool CanStartPlay();

	void OnStopDuetCreate();

	void GmOper(int type, int p1, int p2, int p3);
private:
	bool GetFreeBabyItemIndex(int & out_index);
	void GiveUpDuetCreate();
	RoleBabyParam & GetBabyParam();

	RoleModuleManager * m_role_module_mgr;

	RoleBabyParam m_param;
	AttributeList m_attrs_add;
};


#endif // !__BABY_HPP__
