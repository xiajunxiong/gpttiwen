#ifndef __SPECIAL_LOGIC_ANIMAL_RACE_HPP__
#define __SPECIAL_LOGIC_ANIMAL_RACE_HPP__

#include "scene/speciallogic.hpp"

class SpecialLogicAnimalRace : public SpecialLogic
{
public:
	SpecialLogicAnimalRace(Scene * scene);
	virtual ~SpecialLogicAnimalRace();

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< 主角离开场景

	virtual bool CanDestroy() { return false; }

	void OnActivityClose();

private:

	//std::map<int, Appearance> activity_scene_role_appearance;		//以uid为key   上限为19  用来配置协议发送
};

#endif
