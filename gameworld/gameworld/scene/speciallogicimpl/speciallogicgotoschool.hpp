#ifndef __SPECIAL_GOTO_SCHOOL_HPP__
#define __SPECIAL_GOTO_SCHOOL_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

#include "servercommon/tuzikuaipaodef.hpp"

class SpecialLogicGotoSchool : public SpecialLogic
{
public:
	SpecialLogicGotoSchool(Scene* scene);
	virtual ~SpecialLogicGotoSchool();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����
	virtual bool CanDestroy();
	
	void Fight(Role * role, int monster_group_id);
private:
};

#endif