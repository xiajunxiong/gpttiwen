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

	// 职业进阶
	bool RoleAdvanceCareer(int to_profession, int to_color, bool is_gm);
	void CheckAdvanceCond(int _cond_type, int _param1, int _param2);
	// 改变职业颜色
	void OnChangeColor(int _to_color);

	int GetSubLevel();	// 返回小阶
	int GetMainLevel(); // 返回大阶
private:
	void SendSubInfo();

	//角色登陆检测一遍战胜怪物组完成条件
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

	// 小阶段晋级
	bool RoleAdvanceCareerSub(bool _is_gm);
	// 大阶段晋级
	bool RoleAdvanceCareerMain(int _to_profession, int _to_color, bool _is_gm);
	// 传闻
	void AdvanceNotice(const CareerAdvanceCfg* _cfg);

	// 对老号的第一次上线做处理
	void InitHistoryData();

	RoleModuleManager * m_role_module_mgr;

	AdvanceCareerParam m_param;
};


#endif // !__ADVAMCE_CAREER_HPP__
