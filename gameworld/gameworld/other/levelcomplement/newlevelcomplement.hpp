#ifndef __NEW_LEVEL_COMPLEMENT_HPP__
#define __NEW_LEVEL_COMPLEMENT_HPP__

#include "other/rolemodulemanager.hpp"
#include "servercommon/levelcomplementdef.hpp"

class RoleNewLevelComplement
{
public:
	RoleNewLevelComplement();
	~RoleNewLevelComplement();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleNewLevelComplementParam & param);
	void GetInitParam(RoleNewLevelComplementParam * param);
	void OnRoleLogin();
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLevelUp();

	//由日常触发,助力经验
	void OnAddExp(long long add_exp, const char * reason);
	//提取助力经验到角色经验
	void OnBuyExp();

	void SendInfo();

private:

	//获取等级差值
	int GetLevelDifference();
	//获取平均等级
	int GetAverageLevel();
	//补充助力经验
	void ComplementHelpExp();
	//检测当前累计经验是否符合规范
	void CheckAccExpConform();
	//是否满足开启该功能
	bool IsOpen();
	//获取倍率下标
	int GetMultipleIndex();

	RoleModuleManager * m_module_mgr;

	RoleNewLevelComplementParam m_param;
};


#endif