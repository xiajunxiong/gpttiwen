#ifndef __HUAN_JIE_ZHAN_CHANG_HPP__
#define __HUAN_JIE_ZHAN_CHANG_HPP__

#include <set>
#include <vector>
#include <list>

#include "global/activity/activity.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/struct/battle/battle_def.hpp"

class Role;
class CommonSave;
struct RoleBattleData;
namespace Protocol
{
	class SCHuanJieZhanChangRankInfo;
	class  SCHuanJieZhanChangRoleInfo;
	class SCHuanJieZhanChangRoleRankListing;
	struct HuanJieZhanChangOpponentInfoItem;
}

struct ActivityHuanJieZhanChangShangHaiInfo
{
	ActivityHuanJieZhanChangShangHaiInfo()
	{
		shanghai = 0;
		zhi_liao = 0;
		chengshou_shanghai = 0;
	}

	long long shanghai;
	long long zhi_liao;
	long long chengshou_shanghai;
};

struct QueryHuanJieInfo
{
	QueryHuanJieInfo() : rank_score(0), is_finish_ding_ji(0) {}
	int rank_score;
	char is_finish_ding_ji;
};

struct HuanJieZhanChangTesterRoleData;
class HuanJieZhanChangRoleItem
{
public:
	HuanJieZhanChangRoleItem(const HuanJieZhanChangTesterRoleData& role_data);
	HuanJieZhanChangRoleItem(Role * role,int score,int is_finish_ding_ji_sai_arg);
	HuanJieZhanChangRoleItem(const RoleBattleData & arg_robot_battle_data,int arg_server_id, int score, int is_finish_ding_ji_sai_arg);
	void GetData(Protocol::HuanJieZhanChangOpponentInfoItem & ARG_OUT outdata) const;
	void GetData2(HuanJieZhanChangRecordRoleItem & ARG_OUT outdata) const;

	int role_id;
	int role_level;
	int rank_score;
	GameName role_name;
	short avatar_type;
	short prof;
	short headshot_id;
	short is_leader;
	int is_finish_ding_ji_sai;
	int role_server_id;

	RoleBattleData robot_battle_data;
};

class ActivityHuanJieTeamInfo
{
public:
	ActivityHuanJieTeamInfo();

	int GetAverageRankScore() const;
	int GetAverageLevel() const;
	int GetRoleLeaderServerId() const;
	bool operator<(const ActivityHuanJieTeamInfo & arg) const;
	bool AddTeamInfo(ActivityHuanJieTeamInfo  team_info);
	bool AddRoleItem(const HuanJieZhanChangRoleItem & role_item);
	void AutoSetLeader();
	bool HasRoleDingJi() const;
	int GetLackRoleNum() const;
	std::set<int> GetRoleIdList() const;
	inline int GetRoleNum() const;
	void CalcMatchRankSection(unsigned int now);
	int GetHighestRankScore() const;
	
	unsigned int begin_time;				
	int max_role_num;
	std::vector<HuanJieZhanChangRoleItem> role_list;

	int match_rank_min;
	int match_rank_max;
};

class HuanJieZhanChangFightItem
{
public:
	HuanJieZhanChangFightItem(const ActivityHuanJieTeamInfo & attacker_item, const ActivityHuanJieTeamInfo & defender_item);
	void GetFightRoleInfo(CommonSaveHuanJieTianTiReceordData ARG_OUT & record, std::map<int, ActivityHuanJieZhanChangShangHaiInfo> shanghai_info) const;
	int GetAverageRankScore(bool is_attacker) const;
	void MakeLog();

	time_t start_fight_time;
	ActivityHuanJieTeamInfo attacker;
	ActivityHuanJieTeamInfo defender;
};

struct ActivityHuanJieZhanChangRankItem : public ActivityHuanJieZhanChangRankItemCommon //���а���
{
	ActivityHuanJieZhanChangRankItem(int uid, const ActivityHuanJieZhanChangRoleData & role_data);
	bool operator<(const ActivityHuanJieZhanChangRankItem & other) const;

	time_t last_join_time;
};


struct ActivityHuanJieZhanChangRoleRankInfo  //��¼(Snapshot��)�ϰ���Ҷ�Ӧ������������ѯ���ñ����������а�
{
	ActivityHuanJieZhanChangRoleRankInfo()
	{
		hide_rank_list_pos = 0;
		rank_list_pos = 0;
	}

	int hide_rank_list_pos;		//���ط�����  1��ʼ
	int rank_list_pos;			//���ڷ�����  1��ʼ
};

struct ActivityHuanJieZhanChangRankRolePos  //��¼��ң�Snapshotǰ��λ��
{
	ActivityHuanJieZhanChangRankRolePos()
	{
	}

	std::map<int, int> role_to_rank_pos;  //role_id |  pos 1��ʼ
};

enum ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE
{
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_NONE,		//����ƥ��
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_MATCHING,	//ƥ����
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_MATCH_STATE_READY_FIHGT,	//ƥ�䵽���֣�׼������
};

enum ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT
{
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_WIN,
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL,
	ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW,
};

const static int ACTIVITY_HUAN_JIE_MAX_TEAM_MEMBER_NUM = 3;


struct ActivityHuanMatchRoleNumRequireItem
{
	bool operator<(const ActivityHuanMatchRoleNumRequireItem & other) const;
	bool IsMeetRequirements(std::map<int, int> & has_role_num) const;

	std::map<int, int> need_role_num;
}; 

struct ActivityHuanJieMatchRoleNumRequire	
{	
	ActivityHuanJieMatchRoleNumRequire();
	std::vector< ActivityHuanMatchRoleNumRequireItem> GetActivityHuanMatchRoleNumRequireItem(int group_num, std::map<int, int> has_role_num) const;

	std::vector<ActivityHuanMatchRoleNumRequireItem> need_role_num_list;	
};

class ActivityHuanJieZhanChang: public Activity
{
public:
	static int const SEASON_NUM_BEGIN = 1;

	ActivityHuanJieZhanChang(ActivityManager *activity_manager);
	virtual ~ActivityHuanJieZhanChang();

	virtual bool IsNeedSave();
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);
	virtual void OnUserLogin(Role * role);
	virtual void OnUserLogout(Role * role);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnActivityStatusChange(int from_status, int to_status);

	virtual void OnDayChange();
	virtual void OnMonthChange();

	virtual void OnConnectCrossServerSucc();
	virtual void OnUserLoginOriginGame(int role_id);
	virtual void OnGameServerConnect(std::set<int> server_id);	//���ط� ���� �ǿ��� �Ż��

	//����������
	void  OnRoleMatchReq(Role * role);			//����ƥ��
	void  OnRoleCancelMatchReq(int arg_role_id);		//ȡ��ƥ��
	void QueryRoleRankInfo(Role * role, std::set<int> query_role_id_list);	//��ѯ���а�

	//
	void SetRoleMatchState(int role_id, int state);
	int GetRoleMatchState(int role_id);
	void SendRoleMatchSate(int role_id);
	void SendRoleMatchSate(Role * role);
	void SendOpponentInfo(const HuanJieZhanChangFightItem & ready_fight, int send_role_id = 0);
	void SetFightResult(int role_id, int result, int fight_round, int fight_seq, bool is_attacker, const std::map<int, ActivityHuanJieZhanChangShangHaiInfo> & shang_hai_info);
	void RemoveFightItemInfo(int fight_seq);

	void SendRoleInfo(Role * role);
	void SendRoleInfoByRoleId(int role_id);
	void SendRankInfo(Role * role, int server_id);
	int AddScore(int role_id, int add_value, int total_times, int winning_streak_times, int win_times, int draw_times, bool is_finish_qualifying_match, int add_huan_jie_coin, int add_today_win_times);
	int GetRoleScore(int role_id);
	QueryHuanJieInfo GetRoleQueryInfo(int role_id);

	//------------------------------------------------------------------------
	void FillProtocol(Protocol::SCHuanJieZhanChangRankInfo * protocol_ptr, int server_id);
	void FillProtocol(Protocol::SCHuanJieZhanChangRoleInfo * protocol_ptr, int role_id);
	void FillProtocol(Protocol::SCHuanJieZhanChangRoleRankListing * protocol_ptr,const std::set<int> & query_role_id_list);

	void HiddenToGameSendRankInfo(int role_id, int server_id);
	void HiddenToGameSendRoleInfo(int role_id);
	void HiddenToGameQueryRoleRankInfo(int role_id, std::set<int> query_role_id_list);
	void HiddenToGameSyncRoleInfo(int role_id, int rank_score, int is_finish_ding_ji);
	//----------------------------
	int GetSeasonNum() const;
	void SetSeasonNum(int season_num);
	void SyncHuanJieActivityStatusHiddenToGameworld(int server_id = 0);
	//--------------------------------------------
	void SyncSinleRecordAck(int role_id, int record_id);
	//-----------------------------------------------
	void OnRoleDataChange(int role_id,const ActivityHuanJieZhanChangRoleData  & role_data);
	//-----���������ƺ�-----
	void SetSeasonTopRole(std::vector<int> & vec, bool is_notice = false);//�����ı��ͬ�������ı�ʱΪtrue,����ͬ��Ϊfalse			
	int GetTopRoleTitle(int role_id);

	//�����˵�������
	void AdjustRobotAttr(BattleAttr & attr, int adjust_percent);
	static bool IsHuanJieRobotId(int role_id);

	//��ȡ����ʤ�����佱��
	void FetchTodayWinReward(Role * role,int seq);

	void GmOp(Role * role,int op_type,int p1,int p2);

protected:
	void ClearMatching();		//���ƥ���е��ˣ�������͹ر�ʱ����

	void BuildRankList(time_t now_second,bool is_force = false);		//��������ݱ����ɫ���ݹ������а�
	void UpdateRank(int server_id, const ActivityHuanJieZhanChangRankItem & rank_item);	//��ɫ�����仯ʱ����
	void UpdateSnapshot(time_t now_second, bool is_force = false);

	void UpdateReadyFight(time_t now_second);
	void UpdateMatch(time_t now_second);
	void MatchHelp1(bool is_log = false);
	void MatchRobot(std::map<int, std::vector<ActivityHuanJieTeamInfo> > & need_robot_role_num_to_team_info,bool is_log);
	static ActivityHuanJieTeamInfo GetMaxRoleNumTeamInfo(std::map<int, std::vector<ActivityHuanJieTeamInfo> > & role_num_to_team_info, std::vector<ActivityHuanJieTeamInfo> ARG_OUT * combine_team_info);

	void AdjustRobotAttr(RoleBattleData * role_battle_data);
	int GetPlusNumber(int value);

	void CheckReachedRankReward(int role_id,ActivityHuanJieZhanChangRoleData & role_data);
	ActivityHuanJieZhanChangRoleData * GetActRoleData(int role_id, bool is_auto_create = true, bool set_dirty = false);
	CommonSaveHuanJieData * GetSaveData(bool set_dirty = false);

	//----------------------------------------
	int GetFightSeq();
	int GetRecordKey(int role_id);
	void AddTianTiRecord(int role_id, const CommonSaveHuanJieTianTiReceordData & data);
	void SyncTianTiRecrodToGameRole(int role_id,int record_id, const CommonSaveHuanJieTianTiReceordData & data);
	void CheckTianTiRecord(Role * role);
	void CheckTianTiRecord(int role_id);

protected:
	bool m_has_sync_season_num;
	int m_season_num;
	time_t m_season_end_time;	//��ʱûʲô�ã�����󷽱��·����ͻ���

	//----------------����ƥ���ֶ�------------------
	time_t m_last_update_match_time;
	std::list<ActivityHuanJieTeamInfo> m_match_set;
	std::list<HuanJieZhanChangFightItem> m_ready_fight_list;
	std::map<int, int> m_role_match_state;
	//std::map<int, std::list<ActivityHuanJieTeamInfo> > m_role_id_to_match_team_info;
	bool m_order_flag;

	ActivityHuanJieMatchRoleNumRequire m_require_role_num;

	//---------------------------------------���а�---------------------------------
	int GetUserRecordIndex(int server_id, int role_id);
	void SetUserRecordIndex(int server_id, int role_id, int pos);
	static const int  RANK_LIST_HIDDEN_SERVER_ID = 0;

	bool m_need_build_rank;
	bool m_need_snapshot;
	time_t m_last_rebuild_rank_time;
	time_t m_last_rank_snapshot_time;
	std::map<int, std::vector<ActivityHuanJieZhanChangRankItem> > m_server_rank_list;  //�����������а�
	std::map<int, ActivityHuanJieZhanChangRankRolePos> m_rank_list_role_to_pos;            //key server_id,value ���id��Ӧλ��	

	std::map<int, std::vector<ActivityHuanJieZhanChangRankItem> > m_server_rank_list_snapshot;  //�����������а�(Snapshot)
	std::map<int, ActivityHuanJieZhanChangRoleRankInfo> m_role_rank_list_info;   //role_id ��������ϰ���Ϣ

	//---------------------ս������ֶ�----------------------------------------------	
	int m_fight_seq;	
	std::map<int, HuanJieZhanChangFightItem> m_fight_item_info;			//key:m_fight_seq ����¼��ҿ�ʼ��սʱ����Ϣ��ս����������ս��
	CommonSave * m_single_record_data;
	CommonSave * m_save_data;
};

#endif