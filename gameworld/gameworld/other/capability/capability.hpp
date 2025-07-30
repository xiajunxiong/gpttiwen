#ifndef __CAPABILITY_HPP__
#define __CAPABILITY_HPP__

#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Capability
{
public:
	Capability();
	~Capability();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int capability, int max_capability, int huashen_capability, int huashen_max_capability);
	void GetCapabilityInitParam(int *capability, int *max_capability, int *huashen_capability, int * huashen_max_capability);
	//本尊
	void GetAttrListParam(AttributeList & attr_List);
	void CalcRoleCapability();						// 计算本尊战力

	int GetRoleCapability() { return m_capability; }
	int GetRoleMaxCapability() { return m_max_capability; }
	int GetRoleExtraCapability() { return m_extra_capability; }

	//化身
	void GetHuaShenAttrListParam(AttributeList & attr_List);
	void CalcHuaShenCapability();						// 计算化身战力

	int GetRoleHuaShenCapability() { return m_huashen_capability; }
	int GetRoleHuaShenMaxCapability() { return m_huashen_max_capability; }

	AttributeList & GetAttributeList(int role_type = ROLE_TYPE_BENZUN);		//获得本尊或化身属性列表
	int CalcCapabityByAttrList(AttributeList & attr_list);

	int GetCalcCapByAttrList(AttributeList & attr_list);					//由该属性通过公式计算评分

	void OnSendMaxChangeInfo();
	int GmCalcCapability() { return this->CalcCapablity(); }
private:
	int CalcCapablity(int role_type = ROLE_TYPE_BENZUN); //计算战力
	int CalcExtraCapability();

	RoleModuleManager *m_module_mgr;

	AttributeList m_attr_list;					//玩家本尊总属性
	AttributeList m_huashen_attr_list;			//玩家化身总属性
	int m_capability;							//玩家本尊当前战力
	int m_max_capability;						//玩家本尊历史最高战力
	int m_extra_capability;						//玩家除战斗数据外的战力（不存库）（通过配置直接加的战力）

	int m_huashen_capability;					//玩家化身当前战力
	int m_huashen_max_capability;				//玩家化身历史最高战力
};


#endif
