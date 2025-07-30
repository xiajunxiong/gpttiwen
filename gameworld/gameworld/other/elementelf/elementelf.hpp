#ifndef __ELEMENT_ELF_HPP__
#define __ELEMENT_ELF_HPP__

#include "servercommon/elementelfdef.hpp"

class RoleModuleManager;
struct BattleData_Business;
class ElementElf
{
public:
	ElementElf();
	~ElementElf();

	void* operator new(size_t c);
	void operator delete(void* m);

	void OnDayChange();

	void Init(RoleModuleManager* mgr, const ElementElfParam& param);
	void GetInitParam(ElementElfParam* out_param);

	void OnFightFinish(bool is_win, bool is_fly_away, const BattleData_Business& business);
	inline int GetTodayShowUpTimes() const { return m_param.today_showup_times; }
	void OnElementElfShowUp();
private:
	RoleModuleManager* m_mgr;
	
	ElementElfParam m_param;
};

#endif