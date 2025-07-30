#ifndef _SHAN_HAI_BOSS_MANAGER_HPP_
#define _SHAN_HAI_BOSS_MANAGER_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/shanhaibossdef.hpp"
#include <map>

struct ShanHaiNpcInfo
{
	ShanHaiNpcInfo() { this->Reset(); }

	void Reset()
	{
		scene_id = 0;
		npc_id = 0;
		npc_seq = -1;
		npc_pos = Posi(0, 0);
		death_num = 0;
	}

	int scene_id;
	int npc_id;
	int npc_seq;
	Posi npc_pos;
	int death_num;
};

//山海降妖
class ShanHaiBossManager
{
public:
	static ShanHaiBossManager & Instance();

	void Init(const ShanHaiBossParam &param);
	void GetInitParam(ShanHaiBossParam *param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void Update(unsigned long interval, time_t now_second);

	void OnRoleLogin(Role *role);

	void CreateLastBoss();			//生成上一次的NPC（NPC生成期间重启或宕机）
	void DeleteBoss();

	void SendRoleInfo(Role *role);
	void HiddenSendRoleInfoToGameRole(int uid, int param);
	void SendASignleAllUser(int index);
	void SendBossInfo(Role *role, bool send_all = false);
	void HiddenSendBossInfoToGameRole(int uid, int param);

	void OnKillMonster(int npc_seq_id, int npc_id);

	unsigned int GetNextRefreshTime()const { return m_param.next_refresh_timestamp; }
	void SetNoCreateTime();

	void GmRefreshShanHai();
private:
	ShanHaiBossManager();
	~ShanHaiBossManager();

	ShanHaiBossParam m_param;
	ShanHaiNpcInfo m_monster_info[SHAN_HAI_BOSS_MAXNUM];

	unsigned int m_no_create_monster_begin_time;
	unsigned int m_no_create_monster_end_time;
	int m_no_create_monster_mark;						//1:不允许创建怪物标志
};

#endif


