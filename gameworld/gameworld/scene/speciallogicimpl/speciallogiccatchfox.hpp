#ifndef __SPECIAL_LOGIC_CATCH_FOX_FB_HPP__
#define __SPECIAL_LOGIC_CATCH_FOX_FB_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include <vector>
#include <map>

class SpecialLogicCatchFoxFb : public SpecialLogic
{
public:
	SpecialLogicCatchFoxFb(Scene * scene);
	virtual ~SpecialLogicCatchFoxFb();
	virtual bool CanDestroy() { return true; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack);
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack);

	void OnCatchFox(int x,int y, int dir_y);
	inline bool IsMainTaskCatchFox() const { return m_is_main_task_catch_fox; }
private:
	bool m_is_main_task_catch_fox;
};

#endif