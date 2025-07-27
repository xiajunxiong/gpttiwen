#ifndef __BIG_CROSS_H_GAME_PROTOCOL_H__
#define __BIG_CROSS_H_GAME_PROTOCOL_H__

#include "servercommon/serverdef.h"
#include "servercommon/struct/bigcross/utachampionrecordparam.hpp"
#include "servercommon/msgserialize.hpp"
#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"
#include "servercommon/maildef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

#pragma pack(push)
#pragma pack(4)

// BigCross��HiddenGameWorld����Э��
namespace bigchgameprotocol
{
	// HGAME_BIGC  -> hiddengameworld����bigcross
	// BIGC_HGAME  -> bigcross����hiddengameworld
	enum _MsgType
	{
		MT_INVALID = 0,

		MT_BIGC_HGAME_PROTOCOL_BEGIN = MT_INVALID,
		MT_HGAME_BIGC_REGISTER_HGAME_SERVER,							// HGAME -> BIGC ע��hiddengameworld�� BIGC
		MT_HGAME_BIGC_SYNC_GAME_SERVER_INFO,							// HGAME -> BIGC ע��|ע�� ���ط��µ�������BIGC
		MT_BIGC_HGAME_REGISTER_SERVER_ACK,								// BIGC -> HGAME ע���ִ
		MT_BIGC_HGAME_HEART_BEAT,										// BIGC -> HGAME ������
		MT_HGAME_BIGC_SECRET_KEY,										// HGAME -> BIGC ��Կ��֤
		MT_BIGC_HGAME_SECRET_KEY_ACK,									// BIGC -> HGAME ��Կ��֤��Ӧ
		MT_BIGC_HGAME_SEND_MSG_TO_ROLE,									// BIGC -> HGAME ����Э�鵽���

		MT_HGAME_BIGC_GET_UTA_CHAMPION_RECORD,							// HGAME -> BIGC ��ȡ���µ�һ[���]�ھ��ֲ�
		MT_BIGC_HGAME_GET_UTA_CHAMPION_RECORD_ACK,						// BIGC -> HGAME ��ȡ���µ�һ[���]�ھ��ֲ� ��Ӧ 

		MT_HGAME_BIGC_ADD_UTA_CHAMPION_RECORD_GM_TEST,					// BIGC -> HGAME ��ȡ���µ�һ[���]�ھ��ֲ� ��Ӧ 

		MT_BIGC_HGAME_SYNC_UTA_BASE_DATA,								// BIGC -> HGAME ͬ�����µ�һϵͳ������Ϣ
		MT_BIGC_HGAME_UTA_REQUEST_SYNC_TEAM_INFO,						// BIGC -> HGAME Ҫ�����ط�ͬ�����µ�һ������������
		MT_HGAME_BIGC_UTA_SYNC_TEAM_INFO_LIST,							// HGAME -> BIGC ���ط������ͬ�����µ�һ��������

		MT_HGAME_BIGC_UTA_USER_REQUEST_TEAM_LIST,						// HGAME -> BIGC ���������������б�
		MT_BIGC_HGAME_MESSAGE_SEND_TO_USER,								// BIGC -> HGAME ���������ط���ҷ���Ϣ
		MT_HGAME_BIGC_UTA_USER_REQUEST_TEAM_DETAIL_INFO,				// HGAME -> BIGC �������ĳ�������ϸ��Ϣ

		MT_BIGC_HGAME_SYNC_MAIL_INFO,									// BIGC -> HGAME ͬ��Ҫ���͵��ʼ���Ϣ�����ط�
		MT_BIGC_HGAME_SYNC_ROLE_REQ_RET,								// BIGC -> HGAME ͬ����Ҳ�����������ط�[��Ҫ����ERRORNUM��֪ͨ]
		MT_HGAME_BIGC_UTA_GUESS_REQ,									// HGAME -> BIGC ͬ����ҵ�UTA���²���
		MT_BIGC_HGAME_GIVE_LOTTERY_TO_ROLE,								// BIGC -> HGAME ������Ҿ��±�

		MT_BIGC_HGAME_UTA_QUERY_MEMBER_RANK,							// BIGC -> HGAME �����������µ�һ�����Ա���а�
		MT_HGAME_BIGC_UTA_MEMBER_RANK_RESP,								// HGAME -> BIGC ���ش������µ�һ�����Ա���а�

		MT_HGAME_BIGC_UTA_FETCH_WIN_BOX,								// HGAME -> BIGC ���������ȡʤ������
		MT_BIGC_HGAME_UTA_GIVE_WIN_BOX_REWARD,							// BIGC -> HGAME ����ʤ�����佱��
		MT_BIGC_HGAME_UTA_GIVE_USER_ITEMS,								// BIGC -> HGAME ���µ�һ���� ϵͳ���������Ʒ

		MT_HGAME_BIGC_GM_COMMAND,										// HGAME -> BIGC ��Ϸ���������GM����
		MT_HGAME_BIGC_UTA_MY_TEAM_QUALIFICATION_SHEDULES,				// HGAME -> BIGC ��Ϸ��������������µ�һ�ҵĶ����ʸ�������
		MT_HGAME_BIGC_UTA_TEAM_QUALIFICATION_RANK_LIST,					// HGAME -> BIGC ��Ϸ��������������µ�һ�ʸ������а�
		MT_HGAME_BIGC_UTA_MAIN_SCHEDULES,								// HGAME -> BIGC ��Ϸ�������������������
		MT_HGAME_BIGC_UTA_FINAL_RANK_LIST,								// HGAME -> BIGC ��Ϸ��������������հ�
		MT_HGAME_BIGC_UTA_QUALIFICATION_SCHEDULES,						// HGAME -> BIGC ��Ϸ������������ʸ�������
		MT_HGAME_BIGC_UTA_MAIN_SHCEDULES_OVERVIEW,						// HGAME -> BIGC ���µ�һ������������
		MT_BIGC_HGAME_UTA_SYNC_SCHEDULES,								// BIGC  -> HGAME���µ�һ����ͬ��������Ϣ
		MT_HGAME_BIGC_UTA_SYNC_BATTLE_DATA,								// HGAME -> BIGC ���µ�һ����ͬ��ս�����ݵ�����
		MT_HGAME_BIGC_UTA_SCHEDULE_TEAMMEMBER_DETAIL_REQ,				// HGAME -> BIGC UTA��������VS��Ϣ
		MT_BIGC_HGAME_UTA_SCHEDULE_TEAMMEMBER_DETAIL_RESP,				// BIGC  -> HGAME��������VS��Ϣ
		MT_BIGC_HGAME_TRANSFER_MSG_FROM_BATTLE_TO_HIDDEN,				// BIGC  -> HGAME ������ս����Э��ת�������ط�
		MT_HGAME_BIGC_TRANSFER_MSG_FROM_HIDDEN_TO_BATTLE,				// HGAME -> BIGC ���ط�ת��ս��Э�鵽������ս����
		MT_HGAME_BIGC_USER_REQUEST_WATCH_VIDEO,							// HGAME -> BIGC ��ս��
		MT_BIGC_HGAME_SYNC_VIDEO_DATA,									// BIGC  -> HGAME ͬ��ս��¼������
		MT_BIGC_HGAME_UTA_SYNC_ROUND_RESULT_FOR_OB,						// BIGC  -> HGAME ͬ��OB�غ�ս������
		MT_BIGC_HGAME_UTA_SYNC_BATTLE_INFO_FOR_OB,						// BIGC  -> HGAME ͬ��OB�غ���ʼ����
		MT_BIGC_HGAME_UTA_BATTLE_END_STOP_OB,							// BIGC  -> HGAME ս������ֹͣOB
		MT_HGAME_BIGC_USER_LOGIN_HIDDEN_NOTICE,							// HGAME -> BIGC  ��ҵ�¼���ط�ʱ֪ͨ����
		MT_HGAME_BIGC_USER_LOGIN_ORIGIN_NOTICE,							// HGAME -> BIGC  ��ҵ�¼ԭ��ʱ֪ͨ����
		MT_BIGC_HGAME_UTA_CHAMPION_RECORD_REQUEST_UPDATE_DATA,			// BIGC -> HGAME  UTA�ھ�����������������Ϣ
		MT_HGAME_BIGC_SYNC_USER_DATA_FOR_UTA_CHAMPION_RECORD,			// HGAME -> BIGC  UTA�ھ����������Ϣ������
		MT_BIGC_HGAME_USER_ENTER_SCENE_CONTINUE_UTA_BATTLE,				// BIGC -> HGAME  ����ҵ�UTAս��������
		MT_BIGC_HGAME_UTA_BATTLE_END_NOTICE_TEAM,						// BIGC -> HGAME  ����ս������֪ͨ���ط�
		MT_HGAME_BIGC_CHECK_USER_IN_BIGCROSS_BATTLE,					// HGAME -> BIGC  �������Ƿ��ڴ���ս����
		MT_HGAME_BIGC_USER_ACTIVELY_LEAVE_UTA_SCENE_BEFORE_BATTLE,		// HGAME -> BIGC  UTAս����ʼǰ��������뿪����
		MT_BIGC_HGAME_UTA_GIVE_SERVER_ITEMS,							// BIGC -> HGAME  ���������������
		MT_BIGC_HGAME_MAX,
	};

	struct MessageHeader
	{
		explicit MessageHeader() : msg_type(MT_INVALID) {}
		explicit MessageHeader(int _msg_type) : msg_type(_msg_type) {}

		int msg_type;
	};

	struct HGameBigCRegisterHGameServer									// HGAME -> BIGC ע��hiddengameworld�� bigcross
	{
		HGameBigCRegisterHGameServer() : header(MT_HGAME_BIGC_REGISTER_HGAME_SERVER) {}
		MessageHeader header;

		UniqueHiddenServerID  unique_hidden_server_id;
		unsigned int server_open_time;									// ���ط��Ŀ���ʱ��[������]
	};

	enum SYNC_GAME_SERVER_INFO_REQ_TYPE
	{
		SYNC_GAME_SERVER_INFO_REQ_TYPE_REGISTER,						// ע��
		SYNC_GAME_SERVER_INFO_REQ_TYPE_UNSUBSCRIBE,						// ע��
	};

	struct HGameBigCSyncGameServerInfo									// HGAME -> BIGC ע��|ע�� ���ط��µ�������BIGC
	{
		static const int MAX_SERVER_ID_NUM = 1024;

		HGameBigCSyncGameServerInfo() : header(MT_HGAME_BIGC_SYNC_GAME_SERVER_INFO) {}
		MessageHeader header;

		UniqueHiddenServerID  unique_hidden_server_id;					// �����ĸ����ط�
		int req_type;
		int count;
		int	server_id_list[MAX_SERVER_ID_NUM];
	};


	struct BigCHGameRegisterServerAck									// BIGC -> HGAME ע���ִ
	{
		BigCHGameRegisterServerAck() : header(MT_BIGC_HGAME_REGISTER_SERVER_ACK) {}
		MessageHeader header;

		// ��Э�� , �����������ط�ע��ɹ���
	};

	struct BigCHGameHeartBeat											// BIGC -> HGAME ������
	{
		BigCHGameHeartBeat() : header(MT_BIGC_HGAME_HEART_BEAT) {}
		MessageHeader header;
	};

	// ֻ��һ�λ��� , ��֤ʧ�ܻ�ֱ�ӶϿ�����
	struct HGameBigCSecretKey											// HGAME -> BIGC ��Կ��֤
	{
		HGameBigCSecretKey() : header(MT_HGAME_BIGC_SECRET_KEY) {}
		MessageHeader header;

		UniqueHiddenServerID  unique_hidden_server_id;
		char key[128];														// ��Կ�ַ���
	};

	struct BigCHGameSecretKeyAck											// BIGC -> HGAME ��Կ��֤��Ӧ
	{
		BigCHGameSecretKeyAck() : header(MT_BIGC_HGAME_SECRET_KEY_ACK) {}
		MessageHeader header;
	};

	struct BigCHGameSendMsgToRole											// BIGC -> HGAME ����Э�������Ӧ�����
	{
		BigCHGameSendMsgToRole() : header(MT_BIGC_HGAME_SEND_MSG_TO_ROLE) {}
		MessageHeader header;

		int target_role_uid;												// Ŀ�����uid
		// ����Ӷ�Ӧ��Э���
	};


	struct HGameBigCGetUTAChampionRecord												// HGAME -> BIGC ��ȡ���µ�һ[���] �ھ��ֲ�
	{
		HGameBigCGetUTAChampionRecord() : header(MT_HGAME_BIGC_GET_UTA_CHAMPION_RECORD) {}
		MessageHeader header;

		int uid;															// ��ȡ��Ϣ�����uid, 
		UniqueHiddenServerID  unique_hidden_server_id;						// �����ĸ������
		int begin_index;
		int end_index;
	};

	struct BigCHGameGetUTAChampionRecordAck												// BIGC -> HGAME ��ȡ���µ�һ[���] �ھ��ֲ� ��Ӧ
	{
		BigCHGameGetUTAChampionRecordAck() : header(MT_BIGC_HGAME_GET_UTA_CHAMPION_RECORD_ACK) {}
		MessageHeader header;

		int uid;
		int length;		// ����ӵ���Ϣ����

	};

	struct HGameBigCAddUTAChampionRecordGmTest
	{
		HGameBigCAddUTAChampionRecordGmTest() :header(MT_HGAME_BIGC_ADD_UTA_CHAMPION_RECORD_GM_TEST) {}
		MessageHeader header;

		int index;
		int count;
		UTAChampionRecordUserInfo user_info[MAX_TEAM_MEMBER_NUM];
	};

	struct BigCHGameUTABaseInfoSync
	{
		BigCHGameUTABaseInfoSync() : header(MT_BIGC_HGAME_SYNC_UTA_BASE_DATA) {}
		MessageHeader header;

		short cur_status;
		short season_index;
		short round_idx;
		short reserve_sh;
		unsigned int next_battle_timestamp;
		unsigned int active_reward_begin_timestamp;
		unsigned int active_reward_end_timestamp;
		unsigned int season_end_timestamp;
	};

	struct BigCHGameUTARequestSyncTeamInfo
	{
		BigCHGameUTARequestSyncTeamInfo() : header(MT_BIGC_HGAME_UTA_REQUEST_SYNC_TEAM_INFO) {}
		MessageHeader header;
	};

	struct HGameBigCUserRequestUTATeamList
	{
		HGameBigCUserRequestUTATeamList() : header(MT_HGAME_BIGC_UTA_USER_REQUEST_TEAM_LIST) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct BigCHGameMessageToUser
	{
		BigCHGameMessageToUser() : header(MT_BIGC_HGAME_MESSAGE_SEND_TO_USER) {}
		MessageHeader header;

		int uid;	// uidΪ0ʱ�����������

		// ���������û�Э������
	};

	struct HGameBigCUserRequestUTATeamDetailInfo
	{
		HGameBigCUserRequestUTATeamDetailInfo() : header(MT_HGAME_BIGC_UTA_USER_REQUEST_TEAM_DETAIL_INFO) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		long long team_unique_id;
	};


	struct BigCHGameSyncMailInfo			// BIGC -> HGAME ͬ��Ҫ���͵��ʼ���Ϣ�����ط�
	{
		BigCHGameSyncMailInfo() : header(MT_BIGC_HGAME_SYNC_MAIL_INFO) {}
		MessageHeader header;

		int uid;							// �����ʼ������
		MailContentParam	param;
	};


	struct BigCHGameSyncRoleReqRet			// BIGC -> HGAME ͬ����Ҳ�����������ط�[��Ҫ����ERRORNUM��֪ͨ]
	{
		BigCHGameSyncRoleReqRet() : header(MT_BIGC_HGAME_SYNC_ROLE_REQ_RET) {}
		MessageHeader header;

		int uid;							// ������Ϣ�����uid
		int error_num;						// ��Ҫ����ERRORNUM��֪ͨ , 0��ʾ�����ɹ��ޱ���
	};

	struct HGameBigCUTAGuessReq			// HGAME -> BIGC ͬ����ҵ�UTA���²���
	{
		HGameBigCUTAGuessReq() : header(MT_HGAME_BIGC_UTA_GUESS_REQ) {}
		MessageHeader header;

		UniqueUserID uuid;													// ��ȡ��Ϣ�����uuid, 
		UniqueHiddenServerID  unique_hidden_server_id;						// �����ĸ������

		int req_type;														// ��������
		long long param_1;
		long long param_2;
		int param_3;
	};

	enum GIVE_LOTTERY_REASON_TYPE
	{
		GIVE_LOTTERY_REASON_TYPE_BET_FAILD = 0,		// ��עʧ�ܷ���
		GIVE_LOTTERY_REASON_TYPE_GET_GUESS_REWARD = 1,	// �ֶ���ȡ���½���
	};

	struct BigCHGameGiveLotteryToRole			// BIGC -> HGAME ������Ҿ��±�
	{
		BigCHGameGiveLotteryToRole() : header(MT_BIGC_HGAME_GIVE_LOTTERY_TO_ROLE) {}
		MessageHeader header;

		int target_uid;														// Ŀ�����
		int num;															// ����
		int reason;															// ����
	};

	struct BigCHGameUTARequestTeamMemberRank
	{
		BigCHGameUTARequestTeamMemberRank() : header(MT_BIGC_HGAME_UTA_QUERY_MEMBER_RANK) {}
		MessageHeader header;

		long long unique_team_id;
		int member_uid_list[UTA_TEAM_MEMBER_NUM];
		int member_profession_list[UTA_TEAM_MEMBER_NUM];
	};

	struct HGameBigCUTATeamMemberRankResp
	{
		HGameBigCUTATeamMemberRankResp() : header(MT_HGAME_BIGC_UTA_MEMBER_RANK_RESP) {}
		MessageHeader header;

		long long unique_team_id;

		int rank_prof[UTA_TEAM_MEMBER_NUM];			//!< ְҵ���� -1����δ�ϰ�
		int rank_capability[UTA_TEAM_MEMBER_NUM];	//!< �������� -1����δ�ϰ�
		int rank_level[UTA_TEAM_MEMBER_NUM];		//!< �ȼ����� -1����δ�ϰ�
	};

	struct HGameBigCUTAFetchWinBox
	{
		HGameBigCUTAFetchWinBox() : header(MT_HGAME_BIGC_UTA_FETCH_WIN_BOX) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		int box_seq;
	};

	struct HGameBigCUTAMyTeamQualificationSchedules
	{
		HGameBigCUTAMyTeamQualificationSchedules() : header(MT_HGAME_BIGC_UTA_MY_TEAM_QUALIFICATION_SHEDULES) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct HGameBigCUTATeamQualificationRankList
	{
		HGameBigCUTATeamQualificationRankList() : header(MT_HGAME_BIGC_UTA_TEAM_QUALIFICATION_RANK_LIST) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct BigCHGameUTAGiveWinBoxReward
	{
		BigCHGameUTAGiveWinBoxReward() : header(MT_BIGC_HGAME_UTA_GIVE_WIN_BOX_REWARD) {}
		MessageHeader header;

		int role_id;
		int box_seq;
	};

	struct HGameBigCGmCommand
	{
		HGameBigCGmCommand() : header(MT_HGAME_BIGC_GM_COMMAND) {}
		MessageHeader header;

		int plat_type;
		int hidden_idx;
		int role_id;
		char command[64];
		char params[256];
	};

	struct HGameBigCUserRequestUTAMainSchedules
	{
		HGameBigCUserRequestUTAMainSchedules() : header(MT_HGAME_BIGC_UTA_MAIN_SCHEDULES) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		int round_idx;
	};

	struct HGameBigCUserRequestUTAFinalRankList
	{
		HGameBigCUserRequestUTAFinalRankList() : header(MT_HGAME_BIGC_UTA_FINAL_RANK_LIST) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct HGameBigCUserRequestUTAQualificationSchedules
	{
		HGameBigCUserRequestUTAQualificationSchedules() : header(MT_HGAME_BIGC_UTA_QUALIFICATION_SCHEDULES) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		int round_idx;
	};

	struct HGameBigCUserRequestUTAMainSchedulesOverview
	{
		HGameBigCUserRequestUTAMainSchedulesOverview() : header(MT_HGAME_BIGC_UTA_MAIN_SHCEDULES_OVERVIEW) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		short overview_type;
		short is_last_season;
	};

	struct HGameBigCUTASyncScheduleTeamMemberDetailReq
	{
		HGameBigCUTASyncScheduleTeamMemberDetailReq() : header(MT_HGAME_BIGC_UTA_SCHEDULE_TEAMMEMBER_DETAIL_REQ) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		long long schedule_id;
	};

	struct MsgUTAScheduleTeamMememberDetail
	{
		MsgUTAScheduleTeamMememberDetail() : profession(0), avatar_type(0), level(0), headshot_id(0)
		{
			memset(member_name, 0, sizeof(member_name));
		}

		GameName member_name;
		int profession;
		short avatar_type;
		short level;
		short headshot_id;
		short reserve_sh;
	};

	struct BigCHGameUTAScheduleTeamMemberDetailResp
	{
		BigCHGameUTAScheduleTeamMemberDetailResp() : header(MT_BIGC_HGAME_UTA_SCHEDULE_TEAMMEMBER_DETAIL_RESP) {}
		MessageHeader header;

		long long schedule_id;
		MsgUTAScheduleTeamMememberDetail attackers[UTA_TEAM_MEMBER_NUM];
		MsgUTAScheduleTeamMememberDetail defenders[UTA_TEAM_MEMBER_NUM];
	};

	struct BigCHGameTransferMsgFromBattleToHidden
	{
		BigCHGameTransferMsgFromBattleToHidden() : header(MT_BIGC_HGAME_TRANSFER_MSG_FROM_BATTLE_TO_HIDDEN) {}
		MessageHeader header;

		int length;
		char data[500000];
	};

	struct HGameBigCTransferMsgFromHiddenToBattle
	{
		HGameBigCTransferMsgFromHiddenToBattle() : header(MT_HGAME_BIGC_TRANSFER_MSG_FROM_HIDDEN_TO_BATTLE) {}
		MessageHeader header;

		int uid;
		int length;
		char data[500000];
	};

	struct HGameBigCUserRequestWatchVideo
	{
		HGameBigCUserRequestWatchVideo() : header(MT_HGAME_BIGC_USER_REQUEST_WATCH_VIDEO) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		long long video_id;
	};

	struct BigCHGameSyncVideoData
	{
		BigCHGameSyncVideoData() : header(MT_BIGC_HGAME_SYNC_VIDEO_DATA) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		long long video_id;
		int length;
		char data[500000];
	};

	struct BigCHGameUTASyncRoundResultForOB
	{
		BigCHGameUTASyncRoundResultForOB() : header(MT_BIGC_HGAME_UTA_SYNC_ROUND_RESULT_FOR_OB) {}
		MessageHeader header;

		long long schedule_id;
		int data_length;
		char data_buffer[100000];
	};

	struct BigCHGameUTABattleEndStopOB
	{
		BigCHGameUTABattleEndStopOB() : header(MT_BIGC_HGAME_UTA_BATTLE_END_STOP_OB) {}
		MessageHeader header;

		long long schedule_id;
	};

	struct HGameBigCUserLoginHiddenNotice
	{
		HGameBigCUserLoginHiddenNotice() : header(MT_HGAME_BIGC_USER_LOGIN_HIDDEN_NOTICE) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct HGameBigCUserLoginOriginNotice
	{
		HGameBigCUserLoginOriginNotice() : header(MT_HGAME_BIGC_USER_LOGIN_ORIGIN_NOTICE) {}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct BigCHGameUTAChampionRecordRequestUpdateData
	{
		BigCHGameUTAChampionRecordRequestUpdateData() : header(MT_BIGC_HGAME_UTA_CHAMPION_RECORD_REQUEST_UPDATE_DATA){}
		MessageHeader header;

		int role_id;
	};

	struct MsgUTAChampionRecordUserInfo
	{
		int uid;										//!< ���UID
		GameName role_name;								//!< �������
		int plat_id;									//!< ����ƽ̨ID
		int server_id;									//!< ����������ID

		short level;									//!< �ȼ�
		short fly_flag;									//!< �����׶�
		int top_level;									//!< �۷�ȼ�
		GuildName guild_name;							//!< ��������
		GuildBanner	guild_banner;						//!< �������
		int title_id;									//!< �ƺ�
		int facescore_id;								//!< ��ֵϵͳͷ����ԴID 

		char prof_base;									//!< ְҵ[0,9)
		char advance_times;								//!< ���״���[0,6]
		char avatar_type;								//!< ��ɫģ��[1,6]
		char color;										//!< ��ɫ[0,4)
		ItemID weapon_id;								//!< ����ID
		ItemID sub_weapon_id;							//!< ������ID

		short headshot_id;								//!< ͷ��ID 
		short special_appearance_type;					//!< ������������
		int special_appearance_param;					//!< �����������

		short wear_surface[SURFACE_TYPE_MAX];			//!< ��װ���ĵĻû���Դid �����±�Ϊ���ñ��type

		char smart_mounts_index;						//!< ��ǰ��˵����� -1:δ���
		char is_use_advanced_flag;						//!< ������Ϣʹ�ñ�ʶ 0:δʹ�ý������� 1:ʹ�ý�������
		short reserve_sh;								//!< ����
	};

	struct HGameBigCUserSyncDataForUTAChampionRecord
	{
		HGameBigCUserSyncDataForUTAChampionRecord() : header(MT_HGAME_BIGC_SYNC_USER_DATA_FOR_UTA_CHAMPION_RECORD){}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		
		MsgUTAChampionRecordUserInfo info;
	};

	struct BigCHGameUserEnterSceneContinueUTABattle
	{
		BigCHGameUserEnterSceneContinueUTABattle() : header(MT_BIGC_HGAME_USER_ENTER_SCENE_CONTINUE_UTA_BATTLE) {}
		MessageHeader header;

		int role_id;
	};

	struct BigCHGameUTABattleEndNoticeTeam
	{
		BigCHGameUTABattleEndNoticeTeam() : header(MT_BIGC_HGAME_UTA_BATTLE_END_NOTICE_TEAM) {}
		MessageHeader header;

		long long schedule_id;
		long long team_unique_id;
	};

	struct HGameBigCCheckUserInBigCrossBattle
	{
		HGameBigCCheckUserInBigCrossBattle() : header(MT_HGAME_BIGC_CHECK_USER_IN_BIGCROSS_BATTLE){}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
	};

	struct HGameBigCUserActivelyLeaveUTASceneBeforeBattle
	{
		HGameBigCUserActivelyLeaveUTASceneBeforeBattle() : header(MT_HGAME_BIGC_USER_ACTIVELY_LEAVE_UTA_SCENE_BEFORE_BATTLE){}
		MessageHeader header;

		int plat_type;
		int hidden_server_idx;
		int role_id;
		long long schedule_id;
		long long team_unique_id;
	};
}

#pragma pack(pop)

namespace bigchgameprotocol
{
	//ADDMARK 
	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len);

	struct MsgUTASyncTeamInfo
	{
		MsgUTASyncTeamInfo() : plat_type(0), hidden_idx(0), rank(0), team_id_in_hidden(0LL)
		{
			memset(team_name, 0, sizeof(team_name));
		}

		bool Serialize(char* buffer, int buffer_size, int* out_length);
		bool Deserialize(const char* data, int data_length, int* out_offset);

		int plat_type;
		int hidden_idx;
		int rank;		// ��0��ʼ
		long long team_id_in_hidden;
		GameName team_name;
		UTATeamMemberInfo member_list[UTA_TEAM_MEMBER_NUM];
	};

	struct HGameBigcUTASyncTeamInfoList : public IMessageSerializer, public IMessageDeserializer
	{
		HGameBigcUTASyncTeamInfoList() : header(MT_HGAME_BIGC_UTA_SYNC_TEAM_INFO_LIST) {}
		MessageHeader header;

		virtual int GetMessageType() const;

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		std::vector<MsgUTASyncTeamInfo> team_list;
	};

	struct BigCHGameUTAGiveItemsToUser : public IMessageDeserializer, public IMessageSerializer
	{
		BigCHGameUTAGiveItemsToUser() : header(MT_BIGC_HGAME_UTA_GIVE_USER_ITEMS) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_SHORT(uta_put_reason_type)
				&& WRITE_SHORT(only_send_mail)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_SHORT(uta_put_reason_type)
				&& READ_SHORT(only_send_mail)
				&& READ_INT(itemlist_size) 
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;

			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}
				itemlist.push_back(wrapper);
			}

			return ret;
		}

		int role_id;
		short uta_put_reason_type;
		short only_send_mail;
		std::vector<ItemDataWrapper> itemlist;
		std::string mail_subject;
		std::string mail_content;
	};

	struct BigCHGameUTAGiveItemsToServer : public IMessageDeserializer, public IMessageSerializer
	{
		BigCHGameUTAGiveItemsToServer() : header(MT_BIGC_HGAME_UTA_GIVE_SERVER_ITEMS) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(itemlist_size)
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;

			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}
				itemlist.push_back(wrapper);
			}

			return ret;
		}

		std::vector<ItemDataWrapper> itemlist;
		std::string mail_subject;
		std::string mail_content;
	};

	struct MsgScheduleInfo
	{
		MsgScheduleInfo() : schedule_id(0), is_main_match(0), is_win_group(0),
			atk_team_unique_id(0LL), atk_plat_type(0), atk_hidden_server_idx(0),
			def_team_unique_id(0LL), def_plat_type(0), def_hidden_server_idx(0)
		{
			memset(atk_team_name, 0, sizeof(GameName));
			memset(def_team_name, 0, sizeof(GameName));
			memset(atk_member_uid_list, 0, sizeof(atk_member_uid_list));
			memset(def_member_uid_list, 0, sizeof(def_member_uid_list));
		}

		long long schedule_id;
		short is_main_match;
		short is_win_group;
		
		long long atk_team_unique_id;
		int atk_plat_type;
		int atk_hidden_server_idx;
		GameName atk_team_name;
		int atk_member_uid_list[UTA_TEAM_MEMBER_NUM];

		long long def_team_unique_id;
		int def_plat_type;
		int def_hidden_server_idx;
		GameName def_team_name;
		int def_member_uid_list[UTA_TEAM_MEMBER_NUM];
	};

	struct BigCHGameSyncSchedules : public IMessageDeserializer, public IMessageSerializer
	{
		BigCHGameSyncSchedules() : header(MT_BIGC_HGAME_UTA_SYNC_SCHEDULES) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(cur_status)
				&& WRITE_INT(round_idx)
				&& WRITE_INT((int)schedule_list.size());
			if (!ret) return false;

			for (size_t i = 0; i < schedule_list.size(); ++i)
			{
				ret = WRITE_LL(schedule_list[i].schedule_id)
					&& WRITE_SHORT(schedule_list[i].is_main_match)
					&& WRITE_SHORT(schedule_list[i].is_win_group)
					&& WRITE_LL(schedule_list[i].atk_team_unique_id)
					&& WRITE_INT(schedule_list[i].atk_plat_type)
					&& WRITE_INT(schedule_list[i].atk_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].atk_team_name, sizeof(schedule_list[i].atk_team_name))
					&& WRITE_LL(schedule_list[i].def_team_unique_id)
					&& WRITE_INT(schedule_list[i].def_plat_type)
					&& WRITE_INT(schedule_list[i].def_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].def_team_name, sizeof(schedule_list[i].def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].atk_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].def_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].def_member_uid_list[k]);
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int schdule_list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(cur_status)
				&& READ_INT(round_idx)
				&& READ_INT(schdule_list_size);
			if (!ret) return false;

			schedule_list.reserve(schdule_list_size);
			for (int i = 0; i < schdule_list_size; ++i)
			{
				MsgScheduleInfo schedule;
				ret = READ_LL(schedule.schedule_id)
					&& READ_SHORT(schedule.is_main_match)
					&& READ_SHORT(schedule.is_win_group)
					&& READ_LL(schedule.atk_team_unique_id)
					&& READ_INT(schedule.atk_plat_type)
					&& READ_INT(schedule.atk_hidden_server_idx)
					&& READ_STRN(schedule.atk_team_name, sizeof(schedule.atk_team_name), sizeof(schedule.atk_team_name))
					&& READ_LL(schedule.def_team_unique_id)
					&& READ_INT(schedule.def_plat_type)
					&& READ_INT(schedule.def_hidden_server_idx)
					&& READ_STRN(schedule.def_team_name, sizeof(schedule.def_team_name), sizeof(schedule.def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.atk_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.def_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.def_member_uid_list[k]);
					if (!ret) return false;
				}
				schedule_list.push_back(schedule);
			}

			return ret;
		}

		int cur_status;
		int round_idx;
		std::vector<MsgScheduleInfo> schedule_list;
	};

	struct HGameBigCSyncBattleData : public IMessageDeserializer, public IMessageSerializer
	{
		HGameBigCSyncBattleData() : header(MT_HGAME_BIGC_UTA_SYNC_BATTLE_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(schedule_id)
				&& WRITE_INT(plat_type)
				&& WRITE_INT(hidden_server_idx)
				&& WRITE_INT(uid)
				&& WRITE_LL(team_unique_id);
			if (!ret) return false;

			ret = rbd.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(schedule_id)
				&& READ_INT(plat_type)
				&& READ_INT(hidden_server_idx)
				&& READ_INT(uid)
				&& READ_LL(team_unique_id);
			if (!ret) return false;

			ret = rbd.Deserialize(data, data_length, out_offset);

			return ret;
		}

		long long schedule_id;
		int plat_type;
		int hidden_server_idx;
		int uid;
		long long team_unique_id;
		RoleBattleData rbd;
	};

	struct BigCHGameUTASyncBattleInfoForOB : public IMessageDeserializer, public IMessageSerializer
	{
		BigCHGameUTASyncBattleInfoForOB() : header(MT_BIGC_HGAME_UTA_SYNC_BATTLE_INFO_FOR_OB) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(schedule_id)
				&& WRITE_INT(length_sc_battle_info)
				&& WRITE_INT(length_sc_buff_list)
				&& WRITE_INT(length_sc_special_effect_list)
				&& WRITE_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = WRITE_STRN(sc_battle_info_data, length_sc_battle_info)
				&& WRITE_STRN(sc_buff_list_data, length_sc_buff_list)
				&& WRITE_STRN(sc_special_effect_list_data, length_sc_special_effect_list)
				&& WRITE_STRN(sc_halo_list_data, length_sc_halo_list);
			if (!ret) return false;

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(schedule_id)
				&& READ_INT(length_sc_battle_info)
				&& READ_INT(length_sc_buff_list)
				&& READ_INT(length_sc_special_effect_list)
				&& READ_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = READ_STRN(sc_battle_info_data, length_sc_battle_info, sizeof(sc_battle_info_data))
				&& READ_STRN(sc_buff_list_data, length_sc_buff_list, sizeof(sc_buff_list_data))
				&& READ_STRN(sc_special_effect_list_data, length_sc_special_effect_list, sizeof(sc_special_effect_list_data))
				&& READ_STRN(sc_halo_list_data, length_sc_halo_list, sizeof(sc_halo_list_data));
			if (!ret) return false;

			return ret;
		}

		long long schedule_id;

		int length_sc_battle_info;
		char sc_battle_info_data[5000];

		int length_sc_buff_list;
		char sc_buff_list_data[2000];

		int length_sc_special_effect_list;
		char sc_special_effect_list_data[2000];

		int length_sc_halo_list;
		char sc_halo_list_data[2000];
	};
}


#endif	// __BIG_CROSS_H_GAME_PROTOCOL_H__