#ifndef __UTA_SHADOW_HPP__
#define __UTA_SHADOW_HPP__

#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"

struct UTAScheduleShadow
{
	UTAScheduleShadow() : schedule_id(0), round_idx(0), is_main_match(false), is_win_group(false), 
		atk_team_unique_id(0LL), atk_plat_type(0), atk_hidden_idx(0),
		def_team_unique_id(0LL), def_plat_type(0), def_hidden_idx(0)
	{
		memset(atk_team_name, 0, sizeof(atk_team_name));
		memset(def_team_name, 0, sizeof(def_team_name));
		memset(atk_member_uid_list, 0, sizeof(atk_member_uid_list));
		memset(def_member_uid_list, 0, sizeof(def_member_uid_list));
	}

	bool IsTeamMember(int uid, bool is_attacker) const; 

	long long schedule_id;
	int round_idx;
	bool is_main_match;
	bool is_win_group;

	long long atk_team_unique_id;
	int atk_plat_type;
	int atk_hidden_idx;
	GameName atk_team_name;
	int atk_member_uid_list[UTA_TEAM_MEMBER_NUM];

	long long def_team_unique_id;
	int def_plat_type;
	int def_hidden_idx;
	GameName def_team_name;
	int def_member_uid_list[UTA_TEAM_MEMBER_NUM];
};

struct UTAUserScheduleHash
{
	UTAUserScheduleHash(long long _schedule_id, bool _is_attacker, int _scene_key) : 
		schedule_id(_schedule_id), is_attacker(_is_attacker), scene_key(_scene_key) {}

	long long schedule_id;
	bool is_attacker;
	int scene_key;
};

namespace bigchgameprotocol
{
	struct BigCHGameSyncSchedules;
	struct BigCHGameUTAScheduleTeamMemberDetailResp;
}

namespace Protocol
{
	class SCUTAPrepareEnterSceneNotice;
	class SCUTABaseInfo;
}

class Role;
class UTASceneManager;
class SpecialLogicUTA;
class UTAShadow
{
public:
	typedef std::map<long long, UTAScheduleShadow> ScheduleMap;
	typedef std::map<UniqueUserID, UTAUserScheduleHash> UserScheduleHashMap;

public:
	UTAShadow();
	~UTAShadow();

	void Update(unsigned int now);
	void OnUserLogin(const UserID& user_id);
	void OnUserLoginOrigin(const UserID& user_id);

	static UTAShadow& Instance();

	void OnSyncDataFromBigCross(short cur_status, short season_idx, short round_idx, 
		unsigned int next_battle_timestamp, unsigned int active_reward_begin_timestamp,
		unsigned int active_reward_end_timestamp, unsigned int season_end_timestamp);
	void OnSyncTeamInfoRequest();

	void OnTeamListReq(Role* role);
	void OnTeamDetailInfoReq(Role* role, long long team_unique_id);
	void OnBigCrossQueryTeamMemberRankReq(long long team_unique_id, int member_num, int* uid_list, int* profession_list);
	void OnFetchWinBoxReq(Role* role, int sn);
	void OnMyTeamQualificationSchedulesReq(Role* role);
	void OnTeamQualificationRankListReq(Role* role);
	void OnMainSchedulesReq(Role* role, int round_idx);
	void OnFinalRankListReq(Role* role);
	void OnGetQualificationSchedulesReq(Role* role, int round_idx);
	void OnGetMainSchedulesOverviewReq(Role* role, int overview_type, bool is_last_season);
	void OnUserWatchVideoReq(const UserID& user_id, long long video_id);
	void OnGiveWinBoxResp(int uid, int sn);
	void OnUTAGiveItemsToUser(int role_id, int uta_put_reason_type, const std::vector<ItemDataWrapper>& itemlist, bool only_send_mail, const std::string& mail_subject, const std::string& mail_content);
	void OnUTAGiveItemsToServer(const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content);
	void TryBestToGiveUserItems(int role_id, int put_reason_type, const std::vector<ItemConfigData>& itemlist, const std::string& mail_subject, const std::string& mail_content, bool only_send_mail = false);
	void TryBestToGiveUserItems(int role_id, int put_reason_type, const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content, bool only_send_mail = false);
	bool GiveUserItemsByMail(int role_id, const std::vector<ItemDataWrapper>& itemlist, const std::string& mail_subject, const std::string& mail_content);
	void LogGiveUserItemsResult(int role_id, int put_reason_type, const std::vector<ItemDataWrapper>& itemlist, const std::string& msg);
	void OnUTASyncSchedules(bigchgameprotocol::BigCHGameSyncSchedules* msg);
	void OnUTAScheduleTeamMemberDetailResp(bigchgameprotocol::BigCHGameUTAScheduleTeamMemberDetailResp* msg);
	bool IsInBattleTime(unsigned int now);
	bool IsInBattleStandbyTime(unsigned int now);
	void TryEnterBattleScene(Role* role);
	bool TryReEnterBattleScene(int uid, int* out_scene_id, int* out_fb_key);
	bool CanGiveGuessCoinForActive();
	void OnBattleEndNoticeTeam(long long schedule_id, long long team_unique_id);
	const UTAScheduleShadow* GetScheduleShadow(long long schedule_id);
private:
	UTAShadow(const UTAShadow&);
	UTAShadow& operator=(const UTAShadow&);

	bool HasSyncDataFromBigCross();
	bool HasCreateScene();
	void OnCreateScene();
	void CreateSceneForSchedules();
	void EnterScene(Role* role);
	SpecialLogicUTA* GetSpecialLogic(Role* role);
	void TryEnterSceneInOrigin(Role* role);
	const UTAScheduleShadow* GetUserSchedule(Role* role, bool* is_attacker);
	const UTAScheduleShadow* GetUserSchedule(int plat_type, int role_id, bool* is_attacker);
	bool SendPrepareNoticeToAllParticipants();
	void AssemblePrepareNoticeMsg(Protocol::SCUTAPrepareEnterSceneNotice* notice);
	void CheckSendPrepareNoticeToParticipants(unsigned int now);
	void CheckSendBroadCastNoticeToAll(unsigned int now);
	void BroadcastPrepareNotice();
	void BroadcastBattleStartNotice();
	void SendUTABaseInfo(const UserID& user_id);
	void SendUTABaseInfoToAll();
	void AssembleSCUTABaseInfo(Protocol::SCUTABaseInfo* msg);
	void SendPrepareEnterSceneNotice(const UserID& user_id);

	short m_cur_status;
	short m_round_idx;
	short m_season_idx;
	unsigned int m_next_battle_timestamp;
	unsigned int m_season_end_timestamp;

	ScheduleMap m_schedule_map;
	UserScheduleHashMap m_user_hash_map;
	bool m_has_create_scene;
	unsigned int m_broadcast_prepare_notice_timestamp;
	unsigned int m_broadcast_battle_start_timestamp;
	unsigned int m_send_notice_to_participants_timestamp;
	
	UTASceneManager* m_uta_scene_mgr;
};

#endif