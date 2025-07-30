#ifndef __SPECIAL_LOGIC_ANIMAL_RACE_HPP__
#define __SPECIAL_LOGIC_ANIMAL_RACE_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicAnimalRace : public SpecialLogic
{
public:
	SpecialLogicAnimalRace(Scene * scene);
	virtual ~SpecialLogicAnimalRace();

	virtual void OnRoleEnterScene(Role *role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����

	virtual bool CanDestroy() { return false; }

	void OnActivityClose();

private:

	//std::map<int, Appearance> activity_scene_role_appearance;		//��uidΪkey   ����Ϊ19  ��������Э�鷢��
};

#endif
