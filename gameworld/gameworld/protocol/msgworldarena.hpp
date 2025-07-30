#ifndef __MSG_WORLD_ARENA_HPP__
#define __MSG_WORLD_ARENA_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/worldarenaparam.hpp"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCWorldArenaInfo							//!< 4400 赛季信息
	{
	public:
		SCWorldArenaInfo();
		MessageHeader header;

		int already_signup_num;						//!< 已报名人数
		short cur_status;							//!< 当前状态
		short reserve_sh;							//!< 保留字
		unsigned int next_status_timestamp;			//!< 下次状态变化时间戳
		short season_index;							//!< 赛季号  从1开始
		short total_group_num;						//!< 总分组数
	};

	class SCWorldArenaUserSignupInfo				//!< 4401 玩家报名信息
	{
	public:
		SCWorldArenaUserSignupInfo();
		MessageHeader header;

		short has_signup;
		short reserve_sh;
	};

	class CSWorldArenaOperate						//!< 4402 玩家操作请求
	{
	public:
		CSWorldArenaOperate();
		MessageHeader header;

		enum OPER_TYPE
		{
			OPER_TYPE_INFO = 0,
			OPER_TYPE_SIGNUP = 1,			//!< 报名 
			OPER_TYPE_SCHEDULE_INFO = 2,	//!< 查看赛程信息
			OPER_TYPE_USER_BASE_INFO = 3,	//!< 查看主角的基本信息
			OPER_TYPE_ENTER_SCENE = 4,		//!< 前往赛场
			OPER_TYPE_FETCH_WIN_BOX = 5,	//!< 领取胜场宝箱 param1=sn (配置中的序号sn字段)
			OPER_TYPE_GROUP_RANK_LIST = 6,	//!< 查询组别排行榜 param1=group_idx (组别序号[0, 总分组数])
			OPER_TYPE_STANDBY_FOR_FIGHT = 7,//!< 在赛场中准备 param1=[1:准备 0:取消准备] （废弃）
			OPER_TYPE_FETCH_SIGNUP_REWARD = 8,	//!< 领取报名奖励
			OPER_TYPE_FETCH_DAILY_REWARD = 9,	//!< 领取每日奖励
		};

		short oper_type;
		short reserve_sh;

		int param1;
		int param2;
	};

	struct MsgWorldArenaOneSchedule
	{
		void Reset()
		{
			plat_type = 0;
			uid = 0;
			memset(name, 0, sizeof(name));
			avatar_type = 0;
			headshot_id = 0;
			profession = 0;
			memset(guild_name, 0, sizeof(guild_name));
			rank = 0;
			round_idx = 0;
		}

		void SetFightResult(int real_result, bool is_attacker)
		{
			switch (real_result)
			{
			case WORLD_ARENA_RESULT_ATTACKER_WIN:
				{
					if (is_attacker) result = 1;
					else result = 2;
				}
				break;

			case WORLD_ARENA_RESULT_DEFENDER_WIN:
				{
					if (is_attacker) result = 2;
					else result = 1;
				}
				break;

			case WORLD_ARENA_RESULT_DRAW:
				{
					result = 3;
				}
				break;

			case WORLD_ARENA_RESULT_BOTH_GIVEUP:
				{
					result = 2;
				}
				break;

			default:
				{
					result = 0;
				}
				break;
			}
		}

		int plat_type;							//!< spid   (用于查玩家详细数据）
		int uid;								//!< 角色ID (用于查玩家详细数据）
		GameName name;							//!< 角色名字
		short level;							//!< 等级
		short avatar_type;						//!< 模型
		int headshot_id;						//!< 头像ID
		int profession;							//!< 职业 (基础职业与进阶次数结合）
		GuildName guild_name;					//!< 家族名
		int rank;								//!< 全服排行
		int round_idx;							//!< 轮数 取值范围[0, 19)   
		short result;							//!< 结果 [0:未出结果  1:胜  2:负  3:平]
		short reserve_sh;						//!< 保留字
	};

	class SCWorldArenaUserScheduleInfo			//!< 4403 玩家赛程信息
	{
	public:
		SCWorldArenaUserScheduleInfo();
		MessageHeader header;

		short has_schedule;						//!< 是否已分组（1:是  0:否) 
		short group_idx;						//!< 分组序号，取值范围是[0, 总分组数)

		MsgWorldArenaOneSchedule schedule_list[WORLD_ARENA_ROUND_NUM];	//!< \see MsgWorldArenaOneSchedule
	};

	class SCWorldArenaUserBaseInfo		//!< 4404 玩家基础信息
	{
	public:
		SCWorldArenaUserBaseInfo();
		MessageHeader header;

		char has_signup;				//!< 是否报名
		char has_fetch_signup_reward;	//!< 是否已领取报名奖励
		short cur_rank;					//!< 当前排名 从0开始
		short group_index;				//!< 组别  [0, 总分组数)
		short win_num;					//!< 胜场数
		short score;					//!< 积分
		char daily_reward_fetch_flag;	//!< 每日奖励领取标记( 0:不可领取 1:可领取 2:已领取 )
		char reserve_ch;				//!< 保留字
		int win_box_reward_flag;		//!< 胜场宝箱领取标记 (二进制）
	};

	struct MsgRankListUserInfo
	{
		int plat_type;
		int uid;
		GameName name;			
		int profession;			//!< 职业
		short win_num;			//!< 胜场数
		short lose_num;			//!< 负场数
		short draw_num;			//!< 平局数
		short total_num;		//!< 总场数 (总场数 = 胜 + 平 + 负 + 弃权数）
		int score;				//!< 积分
	};

	struct MsgTopThreeAdditionInfo
	{		
		Appearance appearance;
	};

	class SCWorldArenaGroupRankList		//!< 4405 组内排行榜信息（已有序，按顺序显示即可）
	{
	public:
		SCWorldArenaGroupRankList();
		MessageHeader header;

		int group_idx;	//!< 组别  [0, 总分组数)
		int user_num;	//!< 角色数
		MsgTopThreeAdditionInfo top3_info_list[3];	//!< 这里必定发3个
		MsgRankListUserInfo user_list[WORLD_ARENA_FIGHTER_NUM_PER_GROUP];   //!< 最多20人，按user_num读
	};

	class SCWorldArenaCountDownToFight	//!< 4406 倒计时进入战斗
	{
	public:
		SCWorldArenaCountDownToFight();
		MessageHeader header;

		int count_down_s;			//!< 倒数秒数
	};

	class SCWorldArenaStartEnterSceneNotice		//!< 4407 开始进场通知
	{
	public:
		SCWorldArenaStartEnterSceneNotice();
		MessageHeader header;

		unsigned int enter_scene_end_timestamp;	//!< 进场结束时间戳
		short start_round_index;				//!< 开始的是第几轮战斗
		short has_finish_this_round;			//!< 该轮战斗是否已完成
	};

	struct MsgWorldArenaShowUpRoleInfo
	{	
		MsgWorldArenaShowUpRoleInfo() : plat_type(0), uid(0), level(0), profession(0),
			avatar_type(0), headshot_id(0)
		{
			memset(name, 0, sizeof(name));
		}

		int plat_type;				//!< 渠道号
		int uid;					//!< 主角id  （加上渠道号才可唯一判定是哪个玩家）
		int level;					//!< 等级
		int profession;				//!< 职业
		short avatar_type;			//!< 模型
		short headshot_id;			//!< 头像
		GameName name;				//!< 名字		
	};

	class SCWorldArenaShowUpInfo		//!< 4408 论剑到场信息
	{
	public:
		SCWorldArenaShowUpInfo();
		MessageHeader header;

		short is_attacker_show_up;		//!< 攻方是否到场 0:否 1:是
		short is_defender_show_up;		//!< 守方是否到场

		MsgWorldArenaShowUpRoleInfo attacker_info;
		MsgWorldArenaShowUpRoleInfo defender_info;
	};
}

#pragma pack(pop)


#endif