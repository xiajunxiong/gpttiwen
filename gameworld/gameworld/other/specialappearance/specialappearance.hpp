#ifndef __SPECIAL_APPEARANCE_HPP__
#define __SPECIAL_APPEARANCE_HPP__

#include "servercommon/specialappearancedef.hpp"

class Role;
class RoleModuleManager;
class SpecialAppearance
{
public:
	SpecialAppearance();
	~SpecialAppearance();

	void* operator new(size_t c);
	void operator delete(void*);

	void Init(RoleModuleManager* mgr, const SpecialAppearanceParam& param);

	void GetInitParam(SpecialAppearanceParam* out_param);

	void Update(unsigned long interval, unsigned int now_second);

	short GetSpecialAppearanceType() { return m_show_appearance_type; }
	int GetSpecialAppearanceParam() { return m_show_appearance_param; }

	void SendAppearance(bool is_broadcast);

private:
	void CalcAppearance();

	RoleModuleManager *m_mgr;

	short m_show_appearance_type;
	int m_show_appearance_param;

	SpecialAppearanceParam m_param;
};

#endif
