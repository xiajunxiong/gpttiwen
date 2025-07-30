#ifndef __MAZE_RACER_MANAGER_HPP__
#define __MAZE_RACER_MANAGER_HPP__

#include "servercommon/serverdef.h"
#include "global/activity/activity.hpp"

#include <map>
#include <vector>
#include <set>

static const int MAZE_RACER_COPY_ID_BEGIN = 100;		// ��������ƷID(SCENE_KEY) �� 100 ��ʼ
static const int MAZE_RACER_PASS_ID_BEGIN = 0;			// ͨ�ر�� �� 0 ��ʼ

struct TeamAmuletInfo		// ��������Ч������
{
	TeamAmuletInfo(UInt16 _id = -1, UInt16 _type = 0, UInt16 _val = 0, UInt32 _time = 0) : amulet_id(_id),
		effect_type(_type), effect_value(_val), effect_time(_time) {}

	UInt16 amulet_id;
	UInt16 effect_type;
	UInt16 effect_value;
	time_t effect_time;		// ������ʱ�� - ʣ�������ʧЧʱ���
};

struct TeamParamCommonData
{
	TeamParamCommonData();
	int GetAmuletIdByEffectType(int effect_type);
	int GetFirstAmuletId();
	TeamAmuletInfo * GetAmuletInfo(int effect_type);

	std::map<int, TeamAmuletInfo > team_mmulet_info;
};

struct TeamLocation		// ������������
{
	TeamLocation(int _copy_id = 0, int _layer = 0) : copy_id(_copy_id), layer(_layer) {};

	UInt32 copy_id;
	int layer;
};

struct MazeRacerRoleJoinInfo		// ��ҲμӼ�¼
{
	MazeRacerRoleJoinInfo()
	{
		role_id = 0;
		best_pass_time = 0;		//���ͨ��ʱ��
		get_fb_pass_reward_times = 0;
		join_times = 0;
	}

	int role_id;
	time_t best_pass_time;		//���ͨ��ʱ��
	int get_fb_pass_reward_times;
	int join_times;
};


class Team;
class SpecialLogicMazeRacer;
class MazeRacerManager
{
public:
	MazeRacerManager();
	virtual ~MazeRacerManager();

	static MazeRacerManager & Instance();

	void Update(time_t now_second);

	void InitWaitingHall();
	void KickWaitingHall();
	bool InitMazeCopy(int maze_id, int * copy_id, int * scene_id);
	int GetNextTiersScene(int copy_id, int layer);

	void OnEncounterMonster(int team_index);
	void OnTeamEnter(int copy_id, int layer, int team_index);
	void OnTeamPass(int copy_id, int maze_id, Team * team);
	void RefreshTeamStatus(int team_index);
	void OnSendRoleInfo(Role * role);

	void SendAllRankInfo(Role * role = NULL);
	void SendAllRankInfoHiddenToGame(int role_id);
	void SendEctypeRankInfo(int copy_id);		// ���͸���������Ϣ�������ڵ����

	void CheckIssueActivityAward(time_t now_second, int next_open_day);
	void OnActivityStandy();
	void OnActivityOpen();
	void OnActivityEnd();
	void Clear();

	SpecialLogicMazeRacer * GetSPMazeRacer(int copy_id, int layer);
	bool IsMazeRacerScene(SceneIndex scene_index);

	int & GetTeamResiAmulet(int team_index, int amulet_id);
	void AddAmuletToTeam(int team_index, int amulet_id, int num);
	void AddAmuletBuffToTeam(int user_team_index, int target_team_index, int amulet_id);
	TeamParamCommonData * GetTeamParamCommonData(int team_index);
	TeamAmuletInfo * GetTeamSufferAmulet(int team_index, int effect_type);	
	bool TeamHasSufferAmuletBuff(int team_index);

	void SendAmuletNumToTeam(int team_index);		// �����������֪ͨ
	void AmuletEffectMessage(int team_index);		// ��������Ч��֪ͨ

	void AddJoinTimes(int role_id);
	bool CanJoin(int role_id);
	MazeRacerRoleJoinInfo & GetRoleJoinInfo(int role_id);

	void GmSetActivityRankTimestamp();
private:
	void SendMazeRacerActivityRank(int maze_id, int role_id = 0);
	void MazeRacerIssueActivityAward();			// ���Ż�������� - ��ʽ���ʼ� - ʱ����ʱ�������ʱ

	void ForceStopMazeRacerBattle();
	void ReleaseSpLogicMap();

	// ��߼�
	time_t m_give_rank_award_timestamp;			// ��������������ʱ���
	std::set<UInt32> m_pass_id_list;			// ͨ�ر���б�
	std::map<int, std::map<UInt32, std::vector<MazeRacerTeamInfo> > > m_pass_rank_list;		// �ͨ�����а񵥣��� - �Թ�ID��ͨ��������ֵ - ����������Ϣ����

	// �����߼�
	std::map<UInt32, UInt32> m_begin_timestamp;												// ����������ʱ������� - ��������ID��ֵ��������ʱ���
	std::map<UInt32, UInt32> m_encounter_times;												// ���������ֵĴ������� - ���� Index��ֵ�����ִ���
	std::map<UInt32, TeamLocation> m_location_map;											// ���������������� - ���� Index��ֵ������
	std::map<UInt32, std::map<int, SceneIndex> > m_maze_list;								// �߼�ͳ��ӳ�䣺�� - ��������ID��������ֵ������ΨһIndex
	std::map<UInt32, std::map<UInt32, std::vector<MazeRacerTeamInfo> > > m_ectype_rank_list;// ��������ͨ�����У��� - ��������ID��ͨ��������ֵ��������Ϣ
	std::set<SceneIndex> m_need_del_scene_index;										// ��ɾ��

	std::map<int, std::map<int, int> > m_team_amulet_map;			// ������еķ����������� - ����ID������ID��ֵ - ��������
	std::map<int, TeamParamCommonData> m_team_common_data_map;							// �����ܵ�����Ч���������� - ����ID��ֵ - Ч������

	//��¼��ҲμӴ���
	std::map<int, MazeRacerRoleJoinInfo> m_role_to_join_info;
	int m_scenc_key;
};

#endif