#ifndef __MSG_UTA_H__
#define __MSG_UTA_H__

#include "servercommon/servercommon.h"
#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/userprotocal/msgcode.h"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCUTABaseInfo						//!< 5001 ������Ϣ
	{
	public:
		SCUTABaseInfo() : header(MT_UTA_BASE_INFO_SC) {};
		MessageHeader header;

		short cur_status;					//!< ��ǰ�״̬ 1:׼�� 2:�ʸ��� 3:���� 4:���� 5:����
		short round_idx;					//!< ��ǰ��������
		short season_idx;					//!< ������
		short is_in_progress;				//!< �Ƿ������  0:��  1:��
		unsigned int next_battle_timestamp;	//!< �³�ս����ʱ���
		unsigned int season_end_timestamp;	//!< ��������ʱ�� ����ǰ�������׶��ҳ�����ʱ���Ͳ���ʾͼ�꣩
	};
	
	struct MsgUTATeamMemberInfo
	{
		int plat_type;				//!< ������
		int role_id;				//!< ��ɫID
		GameName role_name;			//!< �����
		short level;				//!< �ȼ�
		short headshot_id;			//!< ͷ��id
		short prof_base_type;		//!< ְҵ��������[1, 9]
		short avatar_type;			//!< ģ������ [1, 6]
		int rank_prof;				//!< ְҵ���� -1����δ�ϰ�
		int rank_capability;		//!< �������� -1����δ�ϰ�
		int rank_level;				//!< �ȼ����� -1����δ�ϰ�
	};

	class SCUTATeamDetailInfo		//!< 5004 ��ѯ��������ذ�
	{
	public:
		SCUTATeamDetailInfo() : header(MT_UTA_TEAM_DETAIL_INFO_SC) {};
		MessageHeader header;

		MsgUTATeamMemberInfo member_list[UTA_TEAM_MEMBER_NUM];	// 5������
	};


	class SCUTAMyTeamQualificationSchedules	//!< 5003 �ҵĶ����ʸ�������
	{
	public:
		SCUTAMyTeamQualificationSchedules() : header(MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_SC) {};
		MessageHeader header;

		enum FIGHT_RESULT
		{
			FIGHT_RESULT_NOT_DONE = 0,
			FIGHT_RESULT_WIN = 1,
			FIGHT_RESULT_LOSE = 2,
			FIGHT_RESULT_GIVEUP = 3,
			FIGHT_RESULT_DRAW = 4,
		};

		short CalcFightResult(int schedule_result, bool is_attacker);

		struct ScheduleInfo
		{
			void Reset()
			{
				team_unique_id = 0;
				server_id = 0;
				memset(team_name, 0, sizeof(team_name));
				leader_headshot_id = 0;
				fight_result = 0;
			}

			long long team_unique_id;	// ����ΨһID
			GameName team_name;			// ս����
			short server_id;			// ���ڲ�ѯ��������
			short leader_avatar_type;	// �ӳ�ģ������
			short leader_headshot_id;	// �ӳ�ͷ��ID
			short fight_result;			// 0:δ��  1:ʤ  2:��  3:��Ȩ
		};

		ScheduleInfo schedule_list[UTA_QUALIFICATION_ROUND_NUM];
	};

	class CSUTATeamListReq	//!< 5051 ������������б�
	{
	public:
		CSUTATeamListReq() : header(MT_UTA_TEAM_LIST_CS) {}
		MessageHeader header;
	};

	class CSUTATeamDetailReq	//!< 5052 ����ĳ���������ϸ��Ϣ
	{
	public:
		CSUTATeamDetailReq() : header(MT_UTA_TEAM_DETAIL_CS) {}
		MessageHeader header;

		long long team_unique_id;	//!< ����ΨһID
	};

	class CSUTAMyTeamQualificationScheduleReq	//!< 5053 �����ҵĶ����ʸ�������
	{
	public:
		CSUTAMyTeamQualificationScheduleReq() : header(MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_CS) {}
		MessageHeader header;
	};

	class CSUTATeamScoreRankListReq	//!< 5055 �����ʸ����������а�
	{
	public:
		CSUTATeamScoreRankListReq() : header(MT_UTA_TEAM_QUALIFICATION_RANK_LIST_CS) {}
		MessageHeader header;
	};

	class SCUTAMyTeamBaseInfo	//!< 5005 �ҵĶ��������Ϣ
	{
	public:
		SCUTAMyTeamBaseInfo() : header(MT_UTA_MY_TEAM_BASE_INFO_SC) {}
		MessageHeader header;

		GameName team_name;				//!< �ҵĶ�����
		short qualification_score;		//!< ��ǰ����
		short cur_rank;					//!< ��ǰ����
		short win_num;					//!< ʤ����
		short lose_num;					//!< ������
		short draw_num;					//!< ƽ����
		short giveup_num;				//!< ��Ȩ��
		short today_reward_num;			//!< �����ѻ�ò��뽱����
		short today_total_reward_num;	//!< �������ɻ�ò��뽱����
		int win_box_fetch_flag;			//!< ʤ��������ȡ��ǣ������ƣ�
	};

	class CSUTAFetchWinBox		// 5054 ��ȡʤ������
	{
	public:
		CSUTAFetchWinBox() : header(MT_UTA_FETCH_WIN_BOX_CS) {}
		MessageHeader header;

		int sn;					// �������sn�ֶ�
	};

	struct MsgUTAMainScheduleTeamInfo
	{
		MsgUTAMainScheduleTeamInfo() : unique_team_id(0), plat_type(0), leader_server_id(0),
			leader_headshot_id(0), support_rate(0) 
		{
			memset(team_name, 0, sizeof(team_name));
		}
		long long unique_team_id;	//!< ����ΨһID  0��ʾû�ж���
		GameName team_name;			//!< ������
		int plat_type;				//!< ��������
		short leader_server_id;		//!< �ӳ�������ID
		short leader_avatar_type;	//!< �ӳ�ģ������
		short leader_headshot_id;	//!< �ӳ�ͷ��
		short support_rate;			//!< ֧���� �ٷֱ�
	};

	struct MsgUTAMainScheduleInfo
	{
		long long shedule_id;		//!< ����ID
		short type;					//!< 0 ʤ����   1 ������
		short fight_result;			//!< 0 δ�н��  1 ����ʤ  2 �ط�ʤ  3 �ѿ�ʼ���ȴ������
		MsgUTAMainScheduleTeamInfo atk_team_info;	//!< ����������Ϣ
		MsgUTAMainScheduleTeamInfo def_team_info;	//!< �ط�������Ϣ
		long long video_id;			//!< ¼��ID
	};

	class SCUTAMainSchedules	//!< 5007 ���������·�
	{
	public:
		SCUTAMainSchedules() : header(MT_UTA_MAIN_SCHEDULES_SC) {}
		MessageHeader header;

		short round_idx;	//!< ��X������ (0:32��16  1:16��8  2:8��4  3��4��2  4���ܾ���  
		short count;		//!< �б�����
		MsgUTAMainScheduleInfo schedule_list[UTA_MAIN_MATCH_NUM_PER_ROUND]; // ��󳤶�Ϊ16
	};

	class CSUTAMainSchedules	//!< 5056 ������������
	{
	public:
		CSUTAMainSchedules() : header(MT_UTA_MAIN_SCHEDULES_CS) {}
		MessageHeader header;

		int round_idx;	//!< ��X������ (0:32��16  1:16��8  2:8��4  3��4��2  4���ܾ���  
	};

	class CSUTAFinalRankList	//!< 5057 �������հ�
	{
	public:
		CSUTAFinalRankList() : header(MT_UTA_FINAL_RANK_LIST_CS) {}
		MessageHeader header;
	};

	class CSUTAQualificationSchedules //!< 5058 �����ʸ�������
	{
	public:
		CSUTAQualificationSchedules() : header(MT_UTA_QUALIFICATION_SCHEDULES_CS) {}
		MessageHeader header;

		int round_idx;  //!< ���� [0, 5)
	};

	class CSUTAMainSchedulesOverview	//!< 5059 ����������������
	{
	public:
		CSUTAMainSchedulesOverview() : header(MT_UTA_MAIN_SCHEDULES_OVERVIEW_CS) {}
		MessageHeader header;

		short overview_type;
		short is_last_season;		//!< 1:��ȡ�Ͻ�����  0:��ȡ��������
	};

	class SCUTASceneInfo		// 5011 ս�������ڵ���Ϣ
	{
	public:
		SCUTASceneInfo() : header(MT_UTA_SCENE_INFO_SC) {}
		MessageHeader header;

		char is_main_match;		// 0���ʸ���   1��������
		char is_win_group;		// ����������ʱ��Ч   0��������  1��ʤ����
		short round_idx;		// �ִ�
		unsigned int start_battle_timestamp;	// ������ʼʱ���
		long long opponent_team_unique_id;		// ���ֶ���ΨһID �����ڲ鿴�������ݣ�
		GameName opponent_team_name;			// ���ֶ�����
	};

	class SCUTABattleVSInfo		//!< 5012 ������Ϣ �����е���ʱ3�룩
	{
	public:
		SCUTABattleVSInfo() : header(MT_UTA_BATTLE_VS_INFO) {}
		MessageHeader header;

		struct MemberInfo
		{
			GameName member_name;	//!< �����
			short prof_base_type;	//!< ְҵ��������[1, 9]
			short avatar_type;		//!< ģ������ [1, 6]
			short level;			//!< �ȼ�
			short headshot_id;		//!< ͷ��
		};

		char is_main_match;		// 0���ʸ���   1��������
		char is_win_group;		// ����������ʱ��Ч   0��������  1��ʤ����
		short round_idx;		// �ִ�
		GameName team_name;		// �ҷ�������
		GameName opponent_team_name;	// �з�������

		MemberInfo ally_members[UTA_TEAM_MEMBER_NUM];	// �ҷ���Ա��Ϣ
		MemberInfo opponent_members[UTA_TEAM_MEMBER_NUM];	//�з���Ա��Ϣ
	};

	class CSUTAEnterScene	// 5060 ����ս������
	{
	public:
		CSUTAEnterScene() : header(MT_UTA_ENTER_SCENE_CS) {}
		MessageHeader header;
	};

	class SCUTAPrepareEnterSceneNotice	// 5013 ׼������֪ͨ
	{
	public:
		SCUTAPrepareEnterSceneNotice() : header(MT_UTA_PREPARE_ENTER_SCENE_SC) {}
		MessageHeader header;

		unsigned int start_prepare_time;		//!< ��ʼ��ʾ"׼����"��ʱ���
		unsigned int start_enter_scene_time;	//!< ��ʼ������ʱ���
		unsigned int stop_enter_scene_time;		//!< ֹͣ������ʱ���

		//!< \note ��ǰʱ�䳬��$start_prepare_timeʱ������ʾ"׼����"�İ�ť
		//!<       ��ǰʱ�䳬��$start_enter_scene_timeʱ������ʾ"������"�İ�ť
		//!<       ��ǰʱ�䳬��$stop_enter_scene_timeʱ���Թر���ʾ
	};

	class SCUTABattleResultShow	//!< 5014 ���µ�һս�������Ϣ��ʾ 
	{
	public:
		SCUTABattleResultShow() : header(MT_UTA_BATTLE_RESULT_SHOW_SC) {}
		MessageHeader header;

		char match_type;		//!< �������� 0:�ʸ���  1:������
		char is_win;			//!< 0:��  1:ʤ
		short win_reason;		//!< Ӯ��ԭ��  0:��Ӯ��  1:����δ׼ʱ�볡
		short modify_score;		//!< �ʸ������ֱ仯
		short cur_score;		//!< �仯����ʸ�������
	};

	class CSUTAWatchVideo		//!< 5061 �鿴ս��
	{
	public:
		CSUTAWatchVideo() : header(MT_UTA_WATCH_VIDEO_CS) {}
		MessageHeader header;

		long long video_id;		//!< ս����ID
	};

	class CSUTAStartOB			//!< 5062 ��ʼOB
	{
	public:
		CSUTAStartOB() : header(MT_UTA_START_OB_CS) {}
		MessageHeader header;

		long long schedule_id;	//!< �ۿ��ĳ��Σ����̺ţ�
	};
}

#pragma pack(pop)


namespace Protocol
{
	struct MsgUTATeamInfo
	{
		long long team_unique_id;	//!< ����ΨһID
		GameName team_name;
		int plat_type;
		int server_id;				//!< �ӳ��ķ�����ID
		int world_team_arena_score;	//!< Ӣ�ۻ������
		int qualification_score;	//!< ��̭������
		short zhanji_type;			//!< ��ǰս�����ͣ�0:��̭��  1:����)
		short zhanji_value;			//!< ��̭����N���� �� ����XXǿ(��ö�٣�
	};


	class SCUTATeamList : public IMessageSerializer	//!< 5002 ���������б�������
	{
	public:
		SCUTATeamList() : header(MT_UTA_TEAM_LIST_SC) {};
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(static_cast<int>(team_list.size()));
			if (!result) return false;

			for (size_t i = 0; i < team_list.size(); ++i)
			{
				result = WRITE_LL(team_list[i].team_unique_id)
					&& WRITE_STRN(team_list[i].team_name, sizeof(GameName))
					&& WRITE_INT(team_list[i].plat_type)
					&& WRITE_INT(team_list[i].server_id)
					&& WRITE_INT(team_list[i].world_team_arena_score)
					&& WRITE_INT(team_list[i].qualification_score)
					&& WRITE_SHORT(team_list[i].zhanji_type)
					&& WRITE_SHORT(team_list[i].zhanji_value);
				if (!result) return false;
			}

			return result;
		}

		std::vector<MsgUTATeamInfo> team_list;	
	};

	struct MsgUTATeamScoreRankInfo
	{
		long long team_unique_id;	//!< ����ΨһID
		GameName team_name;
		int plat_type;
		int server_id;				//!< �ӳ��ķ�����ID
		short qualification_score;	//!< ��̭������
		short win_times;			//!< ʤ����
		short battle_times;			//!< �ܳ���
		short is_enter_main_phase;	//!< �Ƿ����������ʸ� 0:��  1:��
	};

	class SCUTATeamScoreRankList : public IMessageSerializer	//!< 5006 ս�ӵ�ǰ����������������
	{
	public:
		SCUTATeamScoreRankList() : header(MT_UTA_TEAM_SCORE_RANK_LIST_SC) {};
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(my_team_unique_id)
				&& WRITE_INT(static_cast<int>(team_list.size()));

			if (!result) return false;

			for (size_t i = 0; i < team_list.size(); ++i)
			{
				result = WRITE_LL(team_list[i].team_unique_id)
					&& WRITE_STRN(team_list[i].team_name, sizeof(GameName))
					&& WRITE_INT(team_list[i].plat_type)
					&& WRITE_INT(team_list[i].server_id)
					&& WRITE_SHORT(team_list[i].qualification_score)
					&& WRITE_SHORT(team_list[i].win_times)
					&& WRITE_SHORT(team_list[i].battle_times)
					&& WRITE_SHORT(team_list[i].is_enter_main_phase);
				if (!result) return false;
			}

			return result;
		}

		long long my_team_unique_id;
		std::vector<MsgUTATeamScoreRankInfo> team_list;
	};

	struct MsgUTAFinalRankTeamInfo
	{
		MsgUTAFinalRankTeamInfo() : team_unique_id(0), leader_server_id(0), total_win_num(0), 
			total_battle_num(0) 
		{
			memset(team_name, 0, sizeof(team_name));
		}

		long long team_unique_id;	//!< ����ΨһID
		int plat_type;				//!< ������
		int leader_server_id;		//!< ��������
		short total_win_num;		//!< ��ʤ����
		short total_battle_num;		//!< ��ս������
		GameName team_name;			//!< ������
	};

	class SCUTAFinalRankList : public IMessageSerializer	//!< 5008 ���հ�(������)
	{
	public:
		SCUTAFinalRankList() : header(MT_UTA_FINAL_RANK_LIST_SC) {};
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(season_index)
				&& WRITE_INT(final_list.size());

			if (!result) return false;

			for (size_t i = 0; i < final_list.size(); ++i)
			{
				result = WRITE_LL(final_list[i].team_unique_id)
					&& WRITE_STRN(final_list[i].team_name, sizeof(GameName))
					&& WRITE_INT(final_list[i].plat_type)
					&& WRITE_INT(final_list[i].leader_server_id)
					&& WRITE_SHORT(final_list[i].total_win_num)
					&& WRITE_SHORT(final_list[i].total_battle_num);

				if (!result) return false;
			}

			return result;
		}

		int season_index;	// ������
		std::vector<MsgUTAFinalRankTeamInfo> final_list;
	};


	enum MSG_UTA_MAIN_OVERVIEW_TYPE
	{
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP32 = 0,			//!< ��ʮ��ǿ
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP16 = 1,			//!< ʮ��ǿ
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP8_TO_FINAL = 2,	//!< ��ǿ���ܾ���
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_1 = 3,	//!< �������һ��
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_2 = 4,	//!< ������ڶ���
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_3 = 5,	//!< �����������
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_4 = 6,	//!< �����������
		MSG_UTA_MAIN_OVERVIEW_TYPE_MAX
	};

	struct MsgMainOverviewSchedule
	{
		MsgMainOverviewSchedule() : atk_team_unique_id(0), def_team_unique_id(0), 
			fight_result(0), video_id(0) 
		{
			memset(atk_team_name, 0, sizeof(atk_team_name));
			memset(def_team_name, 0, sizeof(def_team_name));
		}

		long long atk_team_unique_id;	//!< ��������ID
		long long def_team_unique_id;	//!< �ط�����ID
		GameName atk_team_name;			//!< ��������
		GameName def_team_name;			//!< �ط�����
		int fight_result;				//!< 0:δ��  1:����ʤ  2:�ط�ʤ  3:�ڴ�
		long long video_id;				//!< ���ڹۿ�¼��
	};

	class SCUTAMainOverviewSchedules : public IMessageSerializer	//!< 5009 ��������������
	{
	public:
		SCUTAMainOverviewSchedules() : header(MT_UTA_OVERVIEW_SCHEDULES_SC) {};
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(overview_type)
				&& WRITE_INT(schedule_list.size());

			if (!result) return false;

			for (size_t i = 0; i < schedule_list.size(); ++i)
			{
				result = WRITE_LL(schedule_list[i].atk_team_unique_id)
					&& WRITE_LL(schedule_list[i].def_team_unique_id)
					&& WRITE_STRN(schedule_list[i].atk_team_name, sizeof(GameName))
					&& WRITE_STRN(schedule_list[i].def_team_name, sizeof(GameName))
					&& WRITE_INT(schedule_list[i].fight_result)
					&& WRITE_LL(schedule_list[i].video_id);

				if (!result) return false;
			}

			return result;
		}

		short overview_type;	//!< �������� \see MSG_UTA_MAIN_OVERVIEW_TYPE
		short is_last_season;	//!< 1:��ȡ�Ͻ�����  0:��ȡ��������
		std::vector<MsgMainOverviewSchedule> schedule_list;
	};

	struct MsgQualificationScheduleTeam
	{
		MsgQualificationScheduleTeam() : team_unique_id(0LL), plat_type(0), leader_server_id(0),
			leader_headshot_id(0), cur_score(0) 
		{
			memset(team_name, 0, sizeof(team_name));
		}

		long long team_unique_id;		//!< ����ID
		GameName team_name;				//!< ����
		int plat_type;					//!< ������
		short leader_server_id;			//!< ������ID
		short leader_avatar_type;		//!< �ӳ�ģ������
		short leader_headshot_id;		//!< �ӳ�ͷ��
		short cur_score;				//!< ��ǰ����
	};

	struct MsgQualificationScheduleInfo
	{
		MsgQualificationScheduleInfo() : schedule_id(0LL), fight_result(0), video_id(0LL) {}

		long long schedule_id;	//!< ����ID
		int fight_result;		//!< 0 δ��   1 ����ʤ  2 �ط�ʤ   3 �ڴ�
		long long video_id;		//!< ¼��ID

		MsgQualificationScheduleTeam atk_team;	//!< ��������
		MsgQualificationScheduleTeam def_team;	//!< �ط�����
	};

	class SCUTAQualificationSchedules : public IMessageSerializer	//!< 5010 ��̭������
	{
	public:
		SCUTAQualificationSchedules() : header(MT_UTA_QUALIFICATION_SCHEDULES_SC) {};
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(round_idx)
				&& WRITE_INT(schedule_list.size());

			if (!result) return false;

			for (size_t i = 0; i < schedule_list.size(); ++i)
			{
				result = WRITE_LL(schedule_list[i].schedule_id)
					&& WRITE_INT(schedule_list[i].fight_result)
					&& WRITE_LL(schedule_list[i].video_id);
				if (!result) return false;

				result = WRITE_LL(schedule_list[i].atk_team.team_unique_id)
					&& WRITE_STRN(schedule_list[i].atk_team.team_name, sizeof(GameName))
					&& WRITE_INT(schedule_list[i].atk_team.plat_type)
					&& WRITE_SHORT(schedule_list[i].atk_team.leader_server_id)
					&& WRITE_SHORT(schedule_list[i].atk_team.leader_avatar_type)
					&& WRITE_SHORT(schedule_list[i].atk_team.leader_headshot_id)
					&& WRITE_SHORT(schedule_list[i].atk_team.cur_score);
				if (!result) return false;

				result = WRITE_LL(schedule_list[i].def_team.team_unique_id)
					&& WRITE_STRN(schedule_list[i].def_team.team_name, sizeof(GameName))
					&& WRITE_INT(schedule_list[i].def_team.plat_type)
					&& WRITE_SHORT(schedule_list[i].def_team.leader_server_id)
					&& WRITE_SHORT(schedule_list[i].def_team.leader_avatar_type)
					&& WRITE_SHORT(schedule_list[i].def_team.leader_headshot_id)
					&& WRITE_SHORT(schedule_list[i].def_team.cur_score);
				if (!result) return false;
			}

			return result;
		}

		int round_idx;	//!< ���� [0,5)
		std::vector<MsgQualificationScheduleInfo> schedule_list;
	};
}

#endif