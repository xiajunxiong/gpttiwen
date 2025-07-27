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
	class SCUTABaseInfo						//!< 5001 赛季信息
	{
	public:
		SCUTABaseInfo() : header(MT_UTA_BASE_INFO_SC) {};
		MessageHeader header;

		short cur_status;					//!< 当前活动状态 1:准备 2:资格赛 3:正赛 4:结算 5:休赛
		short round_idx;					//!< 当前比赛轮数
		short season_idx;					//!< 赛季号
		short is_in_progress;				//!< 是否进行中  0:否  1:是
		unsigned int next_battle_timestamp;	//!< 下场战斗的时间戳
		unsigned int season_end_timestamp;	//!< 赛季结束时间 （当前在休赛阶段且超过该时间后就不显示图标）
	};
	
	struct MsgUTATeamMemberInfo
	{
		int plat_type;				//!< 渠道号
		int role_id;				//!< 角色ID
		GameName role_name;			//!< 玩家名
		short level;				//!< 等级
		short headshot_id;			//!< 头像id
		short prof_base_type;		//!< 职业基础类型[1, 9]
		short avatar_type;			//!< 模型类型 [1, 6]
		int rank_prof;				//!< 职业排名 -1代表未上榜
		int rank_capability;		//!< 评分排名 -1代表未上榜
		int rank_level;				//!< 等级排名 -1代表未上榜
	};

	class SCUTATeamDetailInfo		//!< 5004 查询队伍详情回包
	{
	public:
		SCUTATeamDetailInfo() : header(MT_UTA_TEAM_DETAIL_INFO_SC) {};
		MessageHeader header;

		MsgUTATeamMemberInfo member_list[UTA_TEAM_MEMBER_NUM];	// 5人数组
	};


	class SCUTAMyTeamQualificationSchedules	//!< 5003 我的队伍资格赛赛程
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

			long long team_unique_id;	// 队伍唯一ID
			GameName team_name;			// 战队名
			short server_id;			// 用于查询服务器名
			short leader_avatar_type;	// 队长模型类型
			short leader_headshot_id;	// 队长头像ID
			short fight_result;			// 0:未赛  1:胜  2:负  3:弃权
		};

		ScheduleInfo schedule_list[UTA_QUALIFICATION_ROUND_NUM];
	};

	class CSUTATeamListReq	//!< 5051 请求参赛队伍列表
	{
	public:
		CSUTATeamListReq() : header(MT_UTA_TEAM_LIST_CS) {}
		MessageHeader header;
	};

	class CSUTATeamDetailReq	//!< 5052 请求某个队伍的详细信息
	{
	public:
		CSUTATeamDetailReq() : header(MT_UTA_TEAM_DETAIL_CS) {}
		MessageHeader header;

		long long team_unique_id;	//!< 队伍唯一ID
	};

	class CSUTAMyTeamQualificationScheduleReq	//!< 5053 请求我的队伍资格赛赛程
	{
	public:
		CSUTAMyTeamQualificationScheduleReq() : header(MT_UTA_MY_TEAM_QUALIFICATION_SCHEDULES_CS) {}
		MessageHeader header;
	};

	class CSUTATeamScoreRankListReq	//!< 5055 请求资格赛积分排行榜
	{
	public:
		CSUTATeamScoreRankListReq() : header(MT_UTA_TEAM_QUALIFICATION_RANK_LIST_CS) {}
		MessageHeader header;
	};

	class SCUTAMyTeamBaseInfo	//!< 5005 我的队伍基本信息
	{
	public:
		SCUTAMyTeamBaseInfo() : header(MT_UTA_MY_TEAM_BASE_INFO_SC) {}
		MessageHeader header;

		GameName team_name;				//!< 我的队伍名
		short qualification_score;		//!< 当前积分
		short cur_rank;					//!< 当前排名
		short win_num;					//!< 胜场数
		short lose_num;					//!< 负场数
		short draw_num;					//!< 平局数
		short giveup_num;				//!< 弃权数
		short today_reward_num;			//!< 今日已获得参与奖励数
		short today_total_reward_num;	//!< 今日最大可获得参与奖励数
		int win_box_fetch_flag;			//!< 胜场奖励领取标记（二进制）
	};

	class CSUTAFetchWinBox		// 5054 领取胜场宝箱
	{
	public:
		CSUTAFetchWinBox() : header(MT_UTA_FETCH_WIN_BOX_CS) {}
		MessageHeader header;

		int sn;					// 配置里的sn字段
	};

	struct MsgUTAMainScheduleTeamInfo
	{
		MsgUTAMainScheduleTeamInfo() : unique_team_id(0), plat_type(0), leader_server_id(0),
			leader_headshot_id(0), support_rate(0) 
		{
			memset(team_name, 0, sizeof(team_name));
		}
		long long unique_team_id;	//!< 队伍唯一ID  0表示没有队伍
		GameName team_name;			//!< 队伍名
		int plat_type;				//!< 渠道类型
		short leader_server_id;		//!< 队长服务器ID
		short leader_avatar_type;	//!< 队长模型类型
		short leader_headshot_id;	//!< 队长头像
		short support_rate;			//!< 支持率 百分比
	};

	struct MsgUTAMainScheduleInfo
	{
		long long shedule_id;		//!< 赛事ID
		short type;					//!< 0 胜者组   1 败者组
		short fight_result;			//!< 0 未有结果  1 攻方胜  2 守方胜  3 已开始，等待结果中
		MsgUTAMainScheduleTeamInfo atk_team_info;	//!< 攻方队伍信息
		MsgUTAMainScheduleTeamInfo def_team_info;	//!< 守方队伍信息
		long long video_id;			//!< 录像ID
	};

	class SCUTAMainSchedules	//!< 5007 正赛赛程下发
	{
	public:
		SCUTAMainSchedules() : header(MT_UTA_MAIN_SCHEDULES_SC) {}
		MessageHeader header;

		short round_idx;	//!< 第X轮正赛 (0:32进16  1:16进8  2:8进4  3：4进2  4：总决赛  
		short count;		//!< 列表数量
		MsgUTAMainScheduleInfo schedule_list[UTA_MAIN_MATCH_NUM_PER_ROUND]; // 最大长度为16
	};

	class CSUTAMainSchedules	//!< 5056 请求正赛赛程
	{
	public:
		CSUTAMainSchedules() : header(MT_UTA_MAIN_SCHEDULES_CS) {}
		MessageHeader header;

		int round_idx;	//!< 第X轮正赛 (0:32进16  1:16进8  2:8进4  3：4进2  4：总决赛  
	};

	class CSUTAFinalRankList	//!< 5057 请求最终榜单
	{
	public:
		CSUTAFinalRankList() : header(MT_UTA_FINAL_RANK_LIST_CS) {}
		MessageHeader header;
	};

	class CSUTAQualificationSchedules //!< 5058 请求资格赛赛程
	{
	public:
		CSUTAQualificationSchedules() : header(MT_UTA_QUALIFICATION_SCHEDULES_CS) {}
		MessageHeader header;

		int round_idx;  //!< 轮数 [0, 5)
	};

	class CSUTAMainSchedulesOverview	//!< 5059 请求正赛赛程总览
	{
	public:
		CSUTAMainSchedulesOverview() : header(MT_UTA_MAIN_SCHEDULES_OVERVIEW_CS) {}
		MessageHeader header;

		short overview_type;
		short is_last_season;		//!< 1:获取上届总览  0:获取本届总览
	};

	class SCUTASceneInfo		// 5011 战斗场景内的信息
	{
	public:
		SCUTASceneInfo() : header(MT_UTA_SCENE_INFO_SC) {}
		MessageHeader header;

		char is_main_match;		// 0：资格赛   1：晋级赛
		char is_win_group;		// 仅当晋级赛时有效   0：败者组  1：胜者组
		short round_idx;		// 轮次
		unsigned int start_battle_timestamp;	// 比赛开始时间戳
		long long opponent_team_unique_id;		// 对手队伍唯一ID （用于查看对手阵容）
		GameName opponent_team_name;			// 对手队伍名
	};

	class SCUTABattleVSInfo		//!< 5012 对阵信息 （自行倒计时3秒）
	{
	public:
		SCUTABattleVSInfo() : header(MT_UTA_BATTLE_VS_INFO) {}
		MessageHeader header;

		struct MemberInfo
		{
			GameName member_name;	//!< 玩家名
			short prof_base_type;	//!< 职业基础类型[1, 9]
			short avatar_type;		//!< 模型类型 [1, 6]
			short level;			//!< 等级
			short headshot_id;		//!< 头像
		};

		char is_main_match;		// 0：资格赛   1：晋级赛
		char is_win_group;		// 仅当晋级赛时有效   0：败者组  1：胜者组
		short round_idx;		// 轮次
		GameName team_name;		// 我方队伍名
		GameName opponent_team_name;	// 敌方队伍名

		MemberInfo ally_members[UTA_TEAM_MEMBER_NUM];	// 我方人员信息
		MemberInfo opponent_members[UTA_TEAM_MEMBER_NUM];	//敌方人员信息
	};

	class CSUTAEnterScene	// 5060 进入战斗场景
	{
	public:
		CSUTAEnterScene() : header(MT_UTA_ENTER_SCENE_CS) {}
		MessageHeader header;
	};

	class SCUTAPrepareEnterSceneNotice	// 5013 准备进场通知
	{
	public:
		SCUTAPrepareEnterSceneNotice() : header(MT_UTA_PREPARE_ENTER_SCENE_SC) {}
		MessageHeader header;

		unsigned int start_prepare_time;		//!< 开始显示"准备中"的时间戳
		unsigned int start_enter_scene_time;	//!< 开始进场的时间戳
		unsigned int stop_enter_scene_time;		//!< 停止进场的时间戳

		//!< \note 当前时间超过$start_prepare_time时可以显示"准备中"的按钮
		//!<       当前时间超过$start_enter_scene_time时可以显示"进场中"的按钮
		//!<       当前时间超过$stop_enter_scene_time时可以关闭显示
	};

	class SCUTABattleResultShow	//!< 5014 天下第一战斗结果信息显示 
	{
	public:
		SCUTABattleResultShow() : header(MT_UTA_BATTLE_RESULT_SHOW_SC) {}
		MessageHeader header;

		char match_type;		//!< 比赛类型 0:资格赛  1:晋级赛
		char is_win;			//!< 0:负  1:胜
		short win_reason;		//!< 赢的原因  0:打赢了  1:对手未准时入场
		short modify_score;		//!< 资格赛积分变化
		short cur_score;		//!< 变化后的资格赛积分
	};

	class CSUTAWatchVideo		//!< 5061 查看战报
	{
	public:
		CSUTAWatchVideo() : header(MT_UTA_WATCH_VIDEO_CS) {}
		MessageHeader header;

		long long video_id;		//!< 战报的ID
	};

	class CSUTAStartOB			//!< 5062 开始OB
	{
	public:
		CSUTAStartOB() : header(MT_UTA_START_OB_CS) {}
		MessageHeader header;

		long long schedule_id;	//!< 观看的场次（赛程号）
	};
}

#pragma pack(pop)


namespace Protocol
{
	struct MsgUTATeamInfo
	{
		long long team_unique_id;	//!< 队伍唯一ID
		GameName team_name;
		int plat_type;
		int server_id;				//!< 队长的服务器ID
		int world_team_arena_score;	//!< 英雄会武积分
		int qualification_score;	//!< 淘汰赛积分
		short zhanji_type;			//!< 当前战绩类型（0:淘汰赛  1:正赛)
		short zhanji_value;			//!< 淘汰赛第N名， 或 正赛XX强(读枚举）
	};


	class SCUTATeamList : public IMessageSerializer	//!< 5002 参赛队伍列表（已有序）
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
		long long team_unique_id;	//!< 队伍唯一ID
		GameName team_name;
		int plat_type;
		int server_id;				//!< 队长的服务器ID
		short qualification_score;	//!< 淘汰赛积分
		short win_times;			//!< 胜场数
		short battle_times;			//!< 总场数
		short is_enter_main_phase;	//!< 是否获得了正赛资格 0:否  1:是
	};

	class SCUTATeamScoreRankList : public IMessageSerializer	//!< 5006 战队当前积分排名（已有序）
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

		long long team_unique_id;	//!< 队伍唯一ID
		int plat_type;				//!< 渠道号
		int leader_server_id;		//!< 服务器名
		short total_win_num;		//!< 总胜场数
		short total_battle_num;		//!< 总战斗场数
		GameName team_name;			//!< 队伍名
	};

	class SCUTAFinalRankList : public IMessageSerializer	//!< 5008 最终榜单(已有序)
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

		int season_index;	// 赛季号
		std::vector<MsgUTAFinalRankTeamInfo> final_list;
	};


	enum MSG_UTA_MAIN_OVERVIEW_TYPE
	{
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP32 = 0,			//!< 三十二强
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP16 = 1,			//!< 十六强
		MSG_UTA_MAIN_OVERVIEW_TYPE_TOP8_TO_FINAL = 2,	//!< 八强至总决赛
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_1 = 3,	//!< 败者组第一轮
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_2 = 4,	//!< 败者组第二轮
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_3 = 5,	//!< 败者组第三轮
		MSG_UTA_MAIN_OVERVIEW_TYPE_FAIL_GROUP_4 = 6,	//!< 败者组第四轮
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

		long long atk_team_unique_id;	//!< 攻方队伍ID
		long long def_team_unique_id;	//!< 守方队伍ID
		GameName atk_team_name;			//!< 攻方队名
		GameName def_team_name;			//!< 守方队名
		int fight_result;				//!< 0:未打  1:攻方胜  2:守方胜  3:在打
		long long video_id;				//!< 用于观看录像
	};

	class SCUTAMainOverviewSchedules : public IMessageSerializer	//!< 5009 晋级赛赛况总览
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

		short overview_type;	//!< 总览类型 \see MSG_UTA_MAIN_OVERVIEW_TYPE
		short is_last_season;	//!< 1:获取上届总览  0:获取本届总览
		std::vector<MsgMainOverviewSchedule> schedule_list;
	};

	struct MsgQualificationScheduleTeam
	{
		MsgQualificationScheduleTeam() : team_unique_id(0LL), plat_type(0), leader_server_id(0),
			leader_headshot_id(0), cur_score(0) 
		{
			memset(team_name, 0, sizeof(team_name));
		}

		long long team_unique_id;		//!< 队伍ID
		GameName team_name;				//!< 队名
		int plat_type;					//!< 渠道号
		short leader_server_id;			//!< 服务器ID
		short leader_avatar_type;		//!< 队长模型类型
		short leader_headshot_id;		//!< 队长头像
		short cur_score;				//!< 当前积分
	};

	struct MsgQualificationScheduleInfo
	{
		MsgQualificationScheduleInfo() : schedule_id(0LL), fight_result(0), video_id(0LL) {}

		long long schedule_id;	//!< 赛程ID
		int fight_result;		//!< 0 未打   1 攻方胜  2 守方胜   3 在打
		long long video_id;		//!< 录像ID

		MsgQualificationScheduleTeam atk_team;	//!< 攻方队伍
		MsgQualificationScheduleTeam def_team;	//!< 守方队伍
	};

	class SCUTAQualificationSchedules : public IMessageSerializer	//!< 5010 淘汰赛赛程
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

		int round_idx;	//!< 轮数 [0,5)
		std::vector<MsgQualificationScheduleInfo> schedule_list;
	};
}

#endif