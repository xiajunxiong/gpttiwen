#ifndef __WORLD_STATUS_HPP__
#define __WORLD_STATUS_HPP__

#include "iclockmodule.h"
#include "inetworkmodule.h"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msghuanshou.hpp"

#include <vector>

namespace crossgameprotocal
{
	struct HiddenGameSyncWorldStatus;
};

struct FakeAnnouncementCfg;
struct FakeAnnouncementInfo
{
	FakeAnnouncementInfo() : min_level(0), max_level(0), announcement_type(0), is_random_name(false), item_id(0)
	{}

	void SetData(const FakeAnnouncementCfg * data);

	int min_level;
	int max_level;
	int announcement_type;
	bool is_random_name;
	int item_id;
	std::string descripte_1;
	std::string descripte_2;
	std::string descripte_3;
};


enum WORLDSTATE_DATA_STATUS
{			
	WORLDSTATE_DATA_STATUS_INVALID = 0,						// ��ʼ״̬
	WORLDSTATE_DATA_STATUS_LOADING,							// ���ݼ����� 
	WORLDSTATE_DATA_STATUS_LOAD_FINISH,						// ���ݼ������ 
	WORLDSTATE_DATA_STATUS_MAX,
};

static const int WORLDSTATUS_SAVE_INTERVAL = 30;			// ����ӻ�дһ��
class Role;
class WorldStatus
{
	friend class RMIInitWorldStatusBackObjectImpl;
	friend class RMIInitActivityDataBackImpl;
	friend class RMIInitActivityRoleDataBackObjectImpl;

public:
	static WorldStatus & Instance();

	void Init(const WorldStatusParam &worldstatus_param);

	void OnServerStart();
	void OnServerStop();

	void OnUserLogin(Role * role);
	void OnGameServerConnect(std::set<int> server_id);

	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnTwoWeekChange();
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);

	bool IsLoadFinish() const;
	void InitLoadFinish();

	unsigned int GetLastUpdateDayId();
	void SetLastUpdateDayId(unsigned int dayid);

	unsigned int GetLastResetRoleModuleDataDayId() { return m_common_data.last_reset_role_module_dayid; }
	void SetLastResetRoleModuleDataDayId(unsigned int dayid) { m_common_data.last_reset_role_module_dayid = dayid; }
	unsigned int GetLastResetRoleModuleDataTimestamp() { return m_common_data.last_reset_role_modlue_timestamp; }
	void SetLastResetRoleModuleDataTimestamp(unsigned int timestamp) { m_common_data.last_reset_role_modlue_timestamp = timestamp; }

	unsigned int GetStartTime() { return m_start_time; }


	WorldStatusCommonDataParam & GetCommonData() { return m_common_data; }
	WorldHonorRollDataParam & GetHonorRollData() { return m_honor_roll_data; }
	bool IsResetComCapRank() { return m_common_data.is_reset_complex_cap_rank_flag != 0;}
	void SetResetComCapRankFlag(bool flag) { m_common_data.is_reset_complex_cap_rank_flag = flag ? 1 : 0; }

	void SetCourageChallengeFirstKill(int level, Role * role);
	void SendCourageChallengeFirstKillInfo(Role * role = NULL);

	int GetWorldLevel(bool is_cal = false);
	int GetBraveGroundWorldLevel();
	void SendWorldLevelInfo(Role * role = NULL);
	void OnCalcCrossOpenDay();
	unsigned int GetCrossOpenTimestamp()const { return m_common_data.open_cross_server_zero_timestamp; }

	bool IsChiefElectionWinner(int role_id,int prof);
	bool OnChiefElectionFinish(int role_id,int prof);
	bool GmsetChiefElection(int role_id, int prof);
	void ClearChiefElectionInfo(bool notice = true);
	void SetChiefElectionInfo(const ChiefElectionDataParam & chief_election_data);
	void SendChiefElectionInfo(Role * role = NULL);
	std::vector<int> GetChiefElectionTopRole();

	void OnChiefPeakBattleFinish(int role_id,int match_round);
	void ClearPeakBattleInfo(bool notice = true);
	void SetChiefPeakBattleInfo(const ChiefPeakBattleDataParam & chief_peak_battle_data);

	int GetChiefPeakBattleMatchRound(int role_id);
	void SendChiefPeakBattleInfo(Role * role = NULL);

	void GetServerSchoolTask(int* out_list, int daily_num);

	void OnConnectCrossServerSucc();

	void SyncWorldStatusHiddenToGameworld(int server_id = 0);			//���ط�ĳЩ����״̬�ı�ͬ������Ϸ��
	void SetWorldStatus(const crossgameprotocal::HiddenGameSyncWorldStatus * msg);
	
	//*************************�������
	bool IsInWorldAnswerTime() { return m_common_data.answer_end_tamp > 0; }//��������Ƿ�����
	bool IsAnswerTime() { return m_common_data.answer_tamp > 0; }//�Ƿ��������ʱ��
	void RefreshWorldQuestion(time_t now_time,bool is_begin = false);//ˢ���������
	bool RoleAnswerQuestion(Role *role, int answer_id);//�ش�����
	void OnRefreshAnswerSubject(time_t now_time);//�������
	void SendWorldAnswerInfo(Role * role = NULL);
	//----------------------��һ�а�(���˰������Ҷ�)-----------------------
	void SendTitleToRole(int title_id, int uid, const char * reason);		//��������ųƺ�
	void OnlyFightStatusChange(int to_status, bool is_enter = false);								//�״̬�ı�
	void CheckOnlyFightSceneRoleNum();										//�����һ�а�������������

	int GetOnlyFightRoleRank(int role_uid);
	int GetOnlyFightRankTitleId(int rank);									
	void GetOnlyFightRankTitleList(std::vector<int>* title_list);			
	void AddOnlyFightRecord(int length, int * id_list);						//��¼������Ϣ
	void HiddenSynOnlyFightDataToAllGame();									//���ط���������Ϸ��ͬ������
	void HiddenGameOnlyFightDataSyn(OnlyFightDataParam * param);			//ԭ���յ����ط�ͬ������������
	void GameHiddenOnlyFightDataSynReq(int server_id);						//���ط��յ�ԭ�������ͬ�����ݻ�ԭ��
	void ClearOnlyFightRankInfo();							
	void OnReCalOnlyFightMap();												//���¼���Map����ӳ��				

	//�ٹ���
	void OnCalcFakeAnnouncement(unsigned int now_second);
	void OnCheckFakeAnnouncement(unsigned int now_second);

	//���޹���
	void AddHuanShouAnnInfo(Role * role, int egg_item_id, int huanshou_id);
	void AddHuanShouAnnInfo(HuanShouAnnouncementInfo ann_info);
	void OnSendHuanShouAnnInfo(Role * role);
	void OnHiddenToGameHuanShouAnnInfo(int uid);
	void GetHuanShouAnnInfo(Protocol::SCHuanShouAnnouncementInfo & info);

	//֪ͨ��Ϸ����log
	void SendNoticeLog(int uid, const GameName role_name, const PlatName plat_name, int activity_type, int param1, const char * reason_str);
	
	//GM����
	void SetWorldQuestionStatus(int type);

	//*************************�������

	//----------------------ȫ���Ź�-----------------------
	int GetRAQuanMinDiscountFirstMark();
	void SetRAQuanMinDiscountFirstMark();

	////////////////////////// ��ʱ��һؽ���////////////////////////////
	void SetFindEndTimestamp(int special_logic_activity_type);
	unsigned int GetFindEndTimestamp(int special_logic_activity_type);
	void GmSetFindEndTimestamp(int special_logic_activity_type, int end_day);
	

	unsigned int GetColosseumRefreshTamp() { return m_common_data.colosseum_refresh_tamp; }
	void SetColosseumRefreshTamp(unsigned int tamp) { m_common_data.colosseum_refresh_tamp = tamp; }

	unsigned int GetLunHuiWangChuanRefreshTamp() { return m_common_data.lun_hui_wang_chuan_refresh_tamp; }
	void SetLunHuiWangChuanRefreshTamp(unsigned int tamp) { m_common_data.lun_hui_wang_chuan_refresh_tamp = tamp; }

	void LogFunctionStats(Role * role, const char* function_name, int param1 = 0, int param2 = 0, int param3 = 0);

	unsigned int GetTopLevelRefreshTimestamp();	
	void OnReCalcTopLevelRefreshTimestamp(bool is_reset = false);

	void GmSendEndlessRankReward();
	void GmSetEndlessRankClear() { m_common_data.clear_person_rank_flag = 0; }

	//�����װ����ͨ����Ϣ
	void RandBroadcastInfo();

	//��Ե ����id
	WeddingID CreatWeddingID();

	unsigned int GetHiddenOpenServerTime();
	//ͬ�����ط�����ʱ��,���ڻ�õ�
	void SetSyncHiddenOpenServerTime(unsigned int hidden_open_time);
	void HiddenToGameSyncHiddenOpenServerTime(int server_id);
	void SendHiddenInfo(Role * role = NULL);

private:
	void GetInitHuanShouAnnParam(HuanShouAnnouncementParam * param);

private:
	WorldStatus();
	~WorldStatus();

	bool LoadWorldStatus(long long id_fram);
	void LoadWorldStatusSucc();
	bool SaveWorldStatus(); 

	bool LoadActivityData(long long id_from);
	void LoadActivityDataSucc();
	bool SaveActivityData(bool is_save_all = false);

	bool LoadActivityRoleData(long long inc_id_from);
	void LoadActivityRoleDataSucc();
	void SaveActivityRoleData();

	int m_data_state; 
	int m_activity_data_state;
	int m_activity_role_data_state;

	unsigned int m_start_time;							
	int m_world_time;									
	unsigned int m_last_update_dayid;	

	unsigned int m_last_pet_broadcast_timestamp;
	unsigned int m_last_godequip_broadcast_timestamp;

	bool world_gather_init;			// ��פ�ɼ���
	int cross_server_open_day;		// �����������������

	std::map<int, bool> m_world_answer_role_map;			//���������һش��б�uid - ֻ�лش���ȷ�Ż�����
	std::map<int, int> m_only_fight_rank_map;				//��һ�а�����ӳ��  key:role_uid second:���� [1,3]

	unsigned int m_next_calc_fake_announcement_timestamp;		// �´μ���ٹ����ʱ���
	std::vector<FakeAnnouncementInfo> m_fake_announcement_list;	// ����ٹ�������
	std::map<unsigned int, int> m_fake_announcement_time_map;	// �����ٹ������� key:ʱ��� second:����ٹ��������±�

	std::list<HuanShouAnnouncementInfo> m_huanshou_announcement_list;	//���޹����б�

	WorldStatusCommonDataParam m_common_data;
	WorldHonorRollDataParam m_honor_roll_data;
	CourageChallengeDataParam m_courage_challenge_data;
	ChiefElectionDataParam m_chief_election_data;
	ChiefPeakBattleDataParam m_chief_peak_battle_data;
	OnlyFightDataParam m_only_fight_data;
	ShanHaiBossParam m_shan_hai_boss_data;
	WorldStatusGuildFightParam m_guild_fight_param;
	WeddingFB::HiddenGlobalParam m_wedding_global_param;

};

#endif // __WORLD_STATUS_HPP__


