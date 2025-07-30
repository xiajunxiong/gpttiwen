#ifndef __SPECIAL_LOGIC_GATHER_HPP__
#define __SPECIAL_LOGIC_GATHER_HPP__

#include "scene/speciallogic.hpp"
#include <map>
#include <set>

class SpecialLogicGatherFB : public SpecialLogic
{
public:
	SpecialLogicGatherFB(Scene* scene);
	virtual ~SpecialLogicGatherFB();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role *role);	//!< 主角进入场景
	virtual bool CanDestroy() { return true; }

	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_DUEL; }
	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	virtual bool CanGather(Role* role, NPCGather* gather);
	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos);
	virtual void OnRemoveObj(Obj *obj);
	virtual void OnGetItemByGather(Role * role, ItemID item_id, int num);
	virtual bool OnGatherActiveAdventures() { return true; }
private:
	void CreateGather();
	void SendGatherListInfo(Role * role);

	struct GatherInfo
	{
		GatherInfo()
		{
			gather_id = 0;
			next_create_time = 0;
			gather_obj_id = INVALID_OBJ_ID;
			pos = Posi(0, 0);
		}

		int gather_id;
		time_t next_create_time;
		ObjID gather_obj_id;
		Posi pos;
	};

	bool m_is_init;
	time_t m_begin_timestamp;
	std::vector<GatherInfo> m_gather_info;
	std::map<int, std::map<ItemID, int> > m_gather_list;			// 本次采集获取的道具 key1:role_id
};

#endif