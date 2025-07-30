#include "activityhuanjiezhanchangtester.hpp"
#include "global/activity/activitymanager.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "servercommon/robotdef.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/robot/robotconfig.hpp"

TesterActivityHuanJieZhanChang::TesterActivityHuanJieZhanChang() : 
	ActivityHuanJieZhanChang(&ActivityManager::Instance()),
	m_fake_roleid_generator(10000), m_is_auto_testing(false), m_auto_testing_end_time(0u),
	m_next_add_team_time(0u), m_add_team_num_each_time(0), m_add_team_internal_s(0),
	m_next_match_time(0u), m_match_interval_s(0)
{

}

TesterActivityHuanJieZhanChang::~TesterActivityHuanJieZhanChang()
{

}

void TesterActivityHuanJieZhanChang::Update(unsigned long interval, time_t now_second)
{
	if (m_is_auto_testing)
	{
		unsigned int now = static_cast<unsigned int>(now_second);
		if (now > m_auto_testing_end_time)
		{
			m_is_auto_testing = false;
			m_match_set.clear();
			return;
		}

		if (now > m_next_add_team_time)
		{
			m_next_add_team_time = now + m_add_team_internal_s;
			this->AddSomeFakeMatchTeam(m_add_team_num_each_time);
		}

		if (now > m_next_match_time)
		{
			m_next_match_time = now + m_match_interval_s;
			this->MatchHelp1(true);
		}
	}
}

void TesterActivityHuanJieZhanChang::AddSomeFakeMatchTeam(int num)
{
	for (int i = 0; i < num; ++i)
	{
		this->AddFakeMatchTeam();
	}
}

void TesterActivityHuanJieZhanChang::AddFakeMatchTeam()
{
	int team_member_num = RandomNum(1, ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM + 1);
	ActivityHuanJieTeamInfo team;
	for (int i = 0; i < team_member_num; ++i)
	{
		team.AddRoleItem(this->MakeFakeRoleData());
	}
	m_match_set.push_front(team);
}

HuanJieZhanChangTesterRoleData TesterActivityHuanJieZhanChang::MakeFakeRoleData()
{
	HuanJieZhanChangTesterRoleData tmp;
	tmp.role_id = this->GenerateFakeRoleId();
	tmp.role_server_id = IntToUid(tmp.role_id).db_index;
	SNPRINTF(tmp.role_name, sizeof(tmp.role_name), "Fake_%d", IntToUid(tmp.role_id).role_id);
	tmp.role_level = RandomNum(90, 120);
	tmp.is_finish_ding_ji_sai = (rand() % 2 == 1);
	tmp.is_leader = 0;

	RoleBattleData robot_battle_data = ROBOTCONFIG->GetRobotBattleData(tmp.role_level, ROBOT_CONFIG_USE_TYPE_JJC_AND_PLATFORM);
	F_STRNCPY(robot_battle_data.role_character_data.name, tmp.role_name, sizeof(GameName));
	robot_battle_data.role_character_data.character_id = tmp.role_id;
	
	tmp.avatar = robot_battle_data.appearance.avatar_type;
	tmp.profession = robot_battle_data.partner_character_data->battle_data.profession;
	tmp.headshort_id = robot_battle_data.appearance.headshot_id;
	tmp.rank_score = RandomNum(500, 3000);
	
	tmp.battle_data = robot_battle_data;
	return tmp;
}

void TesterActivityHuanJieZhanChang::TestMatch(int team_num)
{
	m_match_set.clear();

	this->AddSomeFakeMatchTeam(team_num);

	this->MatchHelp1(true);

	m_match_set.clear();
}

void TesterActivityHuanJieZhanChang::TestMatch2(int total_time_s, int interval_s, int add_team_num, int match_interval_s)
{
	if (m_is_auto_testing)
	{
		printf("TestMatch2 Is Auto Testing, Oper Failed!\n");
		return;
	}

	m_is_auto_testing = true;

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_auto_testing_end_time = now + total_time_s;
	m_next_add_team_time = now;
	m_add_team_num_each_time = add_team_num;
	m_add_team_internal_s = interval_s;
	m_next_match_time = now + match_interval_s;
	m_match_interval_s = match_interval_s;

	printf("TestMatch2 Start, total_test_time=%d second, add_team_interval=%d second, add_team_num=%d each time, match_interval=%d second\n",
		total_time_s, interval_s, add_team_num, match_interval_s);
}

int TesterActivityHuanJieZhanChang::GenerateFakeRoleId()
{
	int tmp = m_fake_roleid_generator++;
	if (m_fake_roleid_generator >= 1000000)
	{
		m_fake_roleid_generator = 10000;
	}
	int server_id = LocalConfig::Instance().GetMergeServerId();
	UserID user_id(server_id, tmp);
	return UidToInt(user_id);
}
