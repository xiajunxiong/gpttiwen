#ifndef __ADVAMCE_CAREER_HPP__
#define __ADVAMCE_CAREER_HPP__

#include "servercommon/struct/advancecareerparam.hpp"
#include "protocol/msgadvancecareer.h"
#include "config/otherconfig/professionconfig.hpp"

class Role;
class RoleModuleManager;

class AdvanceCareer
{
public:
	AdvanceCareer();
	~AdvanceCareer();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager* GetRoleModuleManager() { return m_role_module_mgr; }
	void Init(RoleModuleManager * _role_module_manager, const AdvanceCareerParam & _param);
	void GetOtherInitParam(AdvanceCareerParam * _param);
	void OnLogin();

	// ְҵ����
	bool RoleAdvanceCareer(int to_profession, int to_color, bool is_gm);
	void CheckAdvanceCond(int _cond_type, int _param1, int _param2);
	// �ı�ְҵ��ɫ
	void OnChangeColor(int _to_color);

	int GetSubLevel();	// ����С��
	int GetMainLevel(); // ���ش��
private:
	void SendSubInfo();

	//��ɫ��½���һ��սʤ�������������
	void RoleLoginFightMonsterTypeCheck();
	void RoleLoginHongMengTianJieTypeCheck();

	// 1. p1: == 
	void AdvanceCondChangeHelpType1(int _cond_type, int _param_1);
	// 2. p1: >=
	void AdvanceCondChangeHelpType2(int _cond_type, int _param_1);
	// 3. p1: == p2: ==
	void AdvanceCondChangeHelpType3(int _cond_type, int _param_1, int _param_2);
	// 4. p1: >  p1: == p2: >=  
	void AdvanceCondChangeHelpType4(int _cond_type, int _param_1, int _param_2);

	// С�׶ν���
	bool RoleAdvanceCareerSub(bool _is_gm);
	// ��׶ν���
	bool RoleAdvanceCareerMain(int _to_profession, int _to_color, bool _is_gm);
	// ����
	void AdvanceNotice(const CareerAdvanceCfg* _cfg);

	// ���Ϻŵĵ�һ������������
	void InitHistoryData();

	RoleModuleManager * m_role_module_mgr;

	AdvanceCareerParam m_param;
};


#endif // !__ADVAMCE_CAREER_HPP__
