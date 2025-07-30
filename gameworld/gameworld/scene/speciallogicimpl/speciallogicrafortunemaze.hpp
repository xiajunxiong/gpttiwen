#ifndef __SPECIAL_LOGIC_RA_FORTUNE_MAZE_HPP__
#define __SPECIAL_LOGIC_RA_FORTUNE_MAZE_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include <vector>
#include <map>

class SpecialLogicFortuneMaze : public SpecialLogic
{
public:
	SpecialLogicFortuneMaze(Scene * scene);
	virtual ~SpecialLogicFortuneMaze();
	virtual void OnRoleEnterScene(Role *role);					//!< 主角进入场景
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanDestroy() { return true; }
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj);
	void CreateBox(int npc_id, int reward_group_id, Posi box_pos);
private:
	void SendBoxInfo(Role *role);
	unsigned int kick_out_tamp;
	int box_num;
};

#endif