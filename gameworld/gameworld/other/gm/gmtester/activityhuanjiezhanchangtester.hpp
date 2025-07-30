#ifndef __ACTIVITY_HUANJIE_ZHANCHANG_TESTER_HPP__
#define __ACTIVITY_HUANJIE_ZHANCHANG_TESTER_HPP__

#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"

struct HuanJieZhanChangTesterRoleData
{
	int role_id;
	int rank_score;
	GameName role_name;
	int role_level;
	int avatar;
	int profession;
	int headshort_id;
	int role_server_id;
	bool is_finish_ding_ji_sai;
	bool is_leader;

	RoleBattleData battle_data;
};

class TesterActivityHuanJieZhanChang : public ActivityHuanJieZhanChang
{
public:
	TesterActivityHuanJieZhanChang();
	virtual ~TesterActivityHuanJieZhanChang();

	virtual void Update(unsigned long interval, time_t now_second);

	void AddSomeFakeMatchTeam(int num);
	void AddFakeMatchTeam();
	HuanJieZhanChangTesterRoleData MakeFakeRoleData();
	void TestMatch(int team_num);
	void TestMatch2(int total_time_s, int interval_s, int add_team_num, int match_interval_s);
protected:
	int GenerateFakeRoleId();

	int m_fake_roleid_generator;

	bool m_is_auto_testing;
	unsigned int m_auto_testing_end_time;
	unsigned int m_next_add_team_time;
	int m_add_team_num_each_time;
	int m_add_team_internal_s;
	unsigned int m_next_match_time;
	int m_match_interval_s;
};

#endif