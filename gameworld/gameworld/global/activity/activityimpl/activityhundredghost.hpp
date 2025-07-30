#ifndef __ACTIVITY_HUNDRED_GHOST_HPP__
#define __ACTIVITY_HUNDRED_GHOST_HPP__

#include "global/activity/activity.hpp"
#include <map>
#include <set>

class Role;
struct HundredGhostNpcInfo
{
	HundredGhostNpcInfo() { this->Reset(); }

	void Reset()
	{
		scene_id = 0;
		npc_id = 0;
		npc_seq = -1;
		npc_pos = Posi(0, 0);
	}

	int scene_id;
	int npc_id;
	int npc_seq;
	Posi npc_pos;
};

class ActivityHundredGhost : public Activity
{
public:
	ActivityHundredGhost(ActivityManager *activity_manager);
	~ActivityHundredGhost();

	virtual bool IsNeedSave();
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);
	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);

	void OnKillMonster(int npc_seq);
	void SendMonsterInfo(Role *role, bool send_all = false);
	void HiddenSendBossInfoToGameRole(int uid);

private:
	bool CreateMonster(bool is_death);
	bool CheckCreateMonsterBoss(int type);			//检测创建隐藏鬼王或者鬼皇
	void ClearMonster();
	void ReSetData();

	std::vector<HundredGhostNpcInfo> m_monster_list;	//当前在场景创建的NPC信息列表
	int m_death_num;									//已死亡数量
	std::vector<int> m_ghost_king_npc_id;				//已经生成的鬼皇NPC_ID
};

#endif
