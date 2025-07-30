#ifndef __ACTIVITY_WORLD_BOSS3_HPP__
#define __ACTIVITY_WORLD_BOSS3_HPP__

#include "global/activity/activity.hpp"
#include <map>
#include <set>
#include "servercommon/internalprotocal/crossgameprotocal.h"

struct WorldBossRoleInfo3
{
	WorldBossRoleInfo3() : uid(0), hurt(0), heal(0), play_times(0)
	{
	}

	int uid;
	int hurt;
	int heal;
	int play_times;
};

class Role;
class ActivityWorldBoss3 : public Activity
{
public:
	ActivityWorldBoss3(ActivityManager *activity_manager);
	~ActivityWorldBoss3();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnUserLogin(Role *role);
	virtual void OnUserLogout(Role *role);

	int GetBossMaxHp() const { return m_boss_max_hp; }
	int GetBossCurHp() const { return m_boss_cur_hp; }
	void SetBossCurHp(int cur_hp);
	void SyncBossCurHp(int cur_hp, int modify_hp);
	void OnKillBoss();

	void AddJoinRecord(int uid, int add_hurt, int add_heal);
	void SendReward();
	void HiddenGameSendRankResult(crossgameprotocal::HiddenGameWorldBoss3SyncRankResultMsg* msg);
	void HiddenGameSyncUidList(crossgameprotocal::HiddenGameWorldBoss3SyncParticipateUidList* msg);
	void SendBossInfo(Role * role = NULL);
	void CancelBossInfo(Role * role);

	void SendBossPosInfo(Role * role = NULL);
	void SendRoleInfo(Role * role = NULL);

	int GetWorldBossPlayTimes(int uid);
	void AddWorldBossPlayTimes(int uid, int play_times = 1);

	void SyncInfoToLocalServer(bool is_send_boss_pos = false);
	void SetInfo(crossgameprotocal::CrossGameSyncWorldBoss3Info* msg);

	void Settlement();		//���������ǿ�ƽ���������ս���󡣺����������л���㽱��

	void SendRankInfo(Role * role = NULL);
private:
	void Broadcast(int minute);
	void ActBossBroadcast(bool is_kill = false);
	bool CreateBoss();
	void ClearBoss();
	void GameClearBossInfo();

	std::vector<WorldBossRoleInfo3> GetLucklist();
	void UpdateRank();

	int m_boss_npc_id;
	int m_boss_npc_seq;
	int m_send_mail_boss_npc_id;

	int m_boss_scene_id;
	int m_boss_scene_key;
	Posi m_boss_pos;

	int m_boss_cur_hp;
	int m_boss_max_hp;

	int m_open_day;			//�㲥ʱ��ֵ����Ϊ�㲥 Ҫ˵��ʱ�ĸ������ֹ�㲥��open_day�����仯�������ʼ�����Ĺ�����㲥�Ĳ�ͬ
	bool m_is_kill;

	std::map<int, WorldBossRoleInfo3> m_role_info;
	std::set<int> m_req_boss_info_role;
	std::set<int> m_send_rank_result_uid_set;

	time_t m_next_broadcast_time;
	bool m_has_send_reward;

	time_t m_last_update_rank_time;
	bool m_is_need_update_rank;
	std::vector<WorldBossRoleInfo3> m_hurt_rank;
	std::vector<WorldBossRoleInfo3> m_heal_rank;

	//ÿ30�룬�۳�boss����Ѫ������1%��Ѫ����	��boss��ǰ �պ�ʣѪ������1 % ��Ѫ���� ����Ѫ������1 % ��Ѫ��ʱ����ִ�и��߼�����֤��ҵĻ�ɱ����
	time_t m_reduce_hp_begin_time;

};

#endif