#ifndef __SPECIAL_LOGIC_FISHING_GROUND_HPP__
#define __SPECIAL_LOGIC_FISHING_GROUND_HPP__

#include "scene/speciallogic.hpp"
#include "servercommon/servercommon.h"

class SpecialLogicFishingGround : public SpecialLogic
{
public:
	SpecialLogicFishingGround(Scene* scene);
	virtual ~SpecialLogicFishingGround();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual bool CanDestroy() { return true; }
	virtual bool CanFish(Role * role);
	virtual void OnFish(Role* role);
	virtual bool OnGatherActiveAdventures() { return false; }		//钓鱼暂时不触发奇遇任务
	
	void OnStartFish(Role * role);
	void OnEndFishing(Role * role);
	void OnStopFish(Role * role);

	void OnSetInfo(int fb_seq);

private:

	int m_fb_seq;
};



#endif