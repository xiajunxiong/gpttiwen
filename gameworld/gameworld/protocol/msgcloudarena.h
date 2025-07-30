#ifndef __MSG_CLOUD_ARENA_H__
#define __MSG_CLOUD_ARENA_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/cloudarenadef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSCloudArenaOperate	//!< 3675 云端竞技操作请求
	{
	public:
		enum OPER_TYPE
		{
			OPER_TYPE_SELF_INFO = 0,		//!< 获取自身信息
			OPER_TYPE_TARGET_INFO = 1,		//!< 获取目标信息
			OPER_TYPE_REFRESH_TARGET = 2,	//!< 刷新目标列表
			OPER_TYPE_START_FIGHT = 3,		//!< 开始挑战  param1:挑战第X个目标 X=[0, 4)
			OPER_TYPE_BUY_FIGHT_TIMES = 4,	//!< 购买挑战次数（已废弃）
			OPER_TYPE_RECORD_LIST = 5,		//!< 请求战报列表  param1:从第几条战报开始  param2:请求多少条战报
			OPER_TYPE_RANK_LIST = 6,		//!< 请求排行榜  param1:排行榜类型  (0:总榜  1:本服榜  2:段位榜)
			OPER_TYPE_FETCH_RANKING_REWARD = 7,	//!< 领取段位奖励  param1:段位id (段位配置ranking字段)
			OPER_TYPE_OPEN_UI = 8,			//!< 打开界面UI
			OPER_TYPE_ACHIEVEMENT_INFO = 9,	//!< 获取成就信息
			OPER_TYPE_FETCH_ACHIEVE = 10,	//!< 领取成就奖励 param1:成就类型[0,10)  \see CLOUD_ARENA_ACHIEVEMENT
			OPER_TYPE_FETCH_DAY_REWARD = 11,//!< 领取日奖励
			OPER_TYPE_QUERY_FORMATION = 12,	//!< 查询阵容
		};

	public:
		CSCloudArenaOperate();
		MessageHeader header;

		int operate_type;
		int param1;
		int param2;
	};

	class SCCloudArenaSelfInfo	//!< 3676 自身信息返回
	{
	public:
		SCCloudArenaSelfInfo();
		MessageHeader header;

		int cur_rank;							//!< 当前排名
		int cur_score;							//!< 当前积分
		int capability;							//!< 评分
		short remain_fight_times;				//!< 剩余挑战次数
		char day_reward_status;					//!< 每日奖励状态 0:不可领取  1:可领取  2:已领取
		char has_today_fight;					//!< 今天是否挑战过 0:否 1:是
		unsigned int season_finish_timestamp;	//!< 赛季结束时间戳
		unsigned int next_recover_fighttimes_timestamp; //!< 下次挑战次数恢复时间戳
		int reach_ranking_flag;					//!< 达成段位标记 (以二进制的形式表示每个段位是否达成）
		int fetch_ranking_reward_flag;			//!< 领取段位奖励标记（以二进制的形式表示每个段位达成奖励是否已领取）
		int season_idx;							//!< 当前第几赛季 从1开始	
		int today_buy_times;					//!< 今天已购买挑战机会的次数
		int last_day_score;						//!< 前一天的积分
	};

	class SCCloudArenaTargetInfo	//!< 3677 对手信息返回
	{
	public:
		SCCloudArenaTargetInfo();
		MessageHeader header;

		struct TargetInfo
		{
			int rank;				//!< 排名
			int score;				//!< 积分
			int capability;			//!< 综合评分
			int top_level;			//!< 巅峰等级
			short level;			//!< 等级
			short is_robot;			//!< 是否机器人(1:是 0:否)
			GameName name;			//!< 名字
			Appearance appearance;	//!< 外观
		};

		unsigned int next_manual_refresh_timestamp;	//!< 下次可手动刷新时间戳
		int today_already_refresh_times;			//!< 今天已手动刷新次数
		int count;
		TargetInfo target_list[CLOUD_ARENA_OPPONENT_NUM];
	};

	class SCCloudArenaFightRecordList	//!< 3679 玩家战报下发
	{
	public:
		SCCloudArenaFightRecordList();
		MessageHeader header;

		int offset;			//!< 从第几条战报开始
		int count;			//!< 下发了多少条战报
		CloudArenaFightRecord record_list[CLOUD_ARENA_USER_FIGHT_RECORD_NUM];
	};

	class SCCloudArenaNewFightRecordNotice	//!< 3678 玩家新战报通知
	{
	public:
		SCCloudArenaNewFightRecordNotice();
		MessageHeader header;
	};

	struct RankInfo
	{
		RankInfo() : rank(0), uid(0), level(0), is_robot(0), profession(0), score(0)
		{
			memset(name, 0, sizeof(name));
		}

		int rank;
		int uid;
		int top_level;			//巅峰等级
		short level;
		short is_robot;
		int profession;
		int score;
		GameName name;
	};

	class SCCloudArenaRankList				//!< 3680 排行榜类型
	{
	public:
		static const int MAX_RANK_NUM = 100;

	public:
		SCCloudArenaRankList();
		MessageHeader header;

		short rank_type;					//!< rank类型
		short rank_num;						//!< rank_list数量
		RankInfo my_rank;					//!< 我的排名
		RankInfo rank_list[MAX_RANK_NUM];	//!< 排名列表
	};

	class SCCloudArenaNoticeSeasonInitialScore	//!< 3681 通知赛季初始段位
	{
	public:
		SCCloudArenaNoticeSeasonInitialScore();
		MessageHeader header;

		int season_initial_score;
	};

	class SCCloudArenaNoticeDanChange	//!< 3682 通知大段位变更
	{
	public:
		SCCloudArenaNoticeDanChange();
		MessageHeader header;

		short old_dan;		//!< 旧的段位
		short new_dan;		//!< 新的段位
	};

	class SCCloudArenaAchievementInfo	//!< 3683 成就信息下发
	{
	public:
		SCCloudArenaAchievementInfo();
		MessageHeader header;

		struct AchievementItem
		{
			int progress_num;
			short cur_level;
			short reward_status;
		};

		AchievementItem achieve_list[CLOUD_ARENA_ACHIEVEMENT_MAX];
	};

	class SCCloudArenaAchievementUpdateNotice	//!< 3684 单条成就信息更新
	{
	public:
		SCCloudArenaAchievementUpdateNotice();
		MessageHeader header;

		int achieve_type;
		int progress_num;
		short cur_level;
		short reward_status;
	};

	class SCCloudArenaRoleSeasonHighestData		//!< 3685 玩家赛季最高数据下发
	{
	public:
		SCCloudArenaRoleSeasonHighestData();
		MessageHeader header;

		int season_highest_rank;		//!< 赛季最高排名 排名从0开始，-1表示没有排名
		short season_highest_dan_id;	//!< 赛季最高段位id  
		short season_highest_score;		//!< 赛季最高分数
		int is_show_new_shop;			//!< 是否显示新的穹顶商店
	};

	class SCCloudArenaRealTimeInfo	//!< 3686 返回实时积分排名数据
	{
	public:
		SCCloudArenaRealTimeInfo();
		MessageHeader header;

		int cur_rank;			//!< 当前排名，客户端要+1
		int cur_score;			//!< 当前积分
	};

	class CSCloudArenaRealTimeInfo //!< 3687 请求实时积分排名
	{
	public:
		CSCloudArenaRealTimeInfo();
		MessageHeader header;
	};

	class CSCloudArenaSetFormation	//!< 3688 设置阵容
	{
	public:
		CSCloudArenaSetFormation();
		MessageHeader header;

		struct GridInfo
		{
			int id;					// 主角uid 或 宠物id 或 伙伴id 
			short type;				// 0为空  1为主角  2为宠物  3为伙伴
			short reserve_sh;
			long long unique_id;    // 为宠物时，宠物的唯一ID
		};

		struct PetStandbyInfo
		{
			int pet_id;				// 待机宠物id 值为0则没有
			long long unique_id;	// 待机宠物唯一ID
		};

		enum FORMATION_GRID_TYPE
		{
			FORMATION_GRID_INVALID = 0,
			FORMATION_GRID_ROLE = 1,
			FORMATION_GRID_PET = 2,
			FORMATION_GRID_PARTNER = 3,
			FORMATION_GRID_MAX
		};

		bool CheckStanbyPetNum()
		{
			int standby_num = 0;
			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list); ++i)
			{
				if (standby_list[i].pet_id > 0) standby_num += 1;
			}

			if (standby_num >= CLOUD_ARENA_STANDBY_PET_MAX_NUM)
			{
				if (grid_list[2].type == FORMATION_GRID_PET ||
					grid_list[7].type == FORMATION_GRID_PET)
				{
					return false;
				}
			}

			return true;
		}

		bool CheckDuplicate()
		{
			for (int i = 0; i < ARRAY_ITEM_COUNT(grid_list) - 1; ++i)
			{
				if (FORMATION_GRID_INVALID == grid_list[i].type)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(grid_list); ++k)
				{
					if (grid_list[i].type == grid_list[k].type &&
						grid_list[i].id == grid_list[k].id)
					{
						if (FORMATION_GRID_PET == grid_list[i].type)
						{
							if (grid_list[i].unique_id == grid_list[k].unique_id)
							{
								return false;
							}
						}
						else
						{
							return false;
						}
					}
				}

				if (FORMATION_GRID_PET == grid_list[i].type)
				{
					for (int k = 0; k < ARRAY_ITEM_COUNT(standby_list); ++k)
					{
						if (grid_list[i].id == standby_list[k].pet_id &&
							grid_list[i].unique_id == standby_list[k].unique_id)
						{
							return false;
						}
					}
				}
			}

			for (int i = 0; i < ARRAY_ITEM_COUNT(standby_list) - 1; ++i)
			{
				if (0 == standby_list[i].pet_id)
				{
					continue;
				}

				for (int k = i + 1; k < ARRAY_ITEM_COUNT(standby_list); ++k)
				{
					if (standby_list[i].pet_id == standby_list[k].pet_id &&
						standby_list[i].unique_id == standby_list[k].unique_id)
					{
						return false;
					}
				}
			}

			return true;
		}

		GridInfo grid_list[SIDE_CHARACTER_NUM]; 
		PetStandbyInfo standby_list[CLOUD_ARENA_STANDBY_PET_MAX_NUM];
	};

	class SCCloudArenaFormationInfo	//!< 3689 阵容下发
	{
	public:
		SCCloudArenaFormationInfo();
		MessageHeader header;

		struct GridInfo
		{
			GridInfo() { this->Reset(); }

			void Reset()
			{
				id = 0;
				type = 0;
				reserve_sh = 0;
				unique_id = 0;
			}

			int id;					// 主角uid 或 宠物id 或 伙伴id 
			short type;				// 0为空  1为主角  2为宠物  3为伙伴
			short reserve_sh;
			long long unique_id;    // 为宠物时，宠物的唯一ID
		};

		struct PetStandbyInfo
		{
			PetStandbyInfo() { this->Reset(); }
			void Reset()
			{
				pet_id = 0;
				unique_id = 0;
			}

			int pet_id;				// 待机宠物id 值为0则没有
			long long unique_id;	// 待机宠物唯一ID
		};

		void Reset()
		{
			for (int i = 0; i < ARRAY_LENGTH(grid_list); ++i)
			{
				grid_list[i].Reset();
			}

			for (int i = 0; i < ARRAY_LENGTH(standby_list); ++i)
			{
				standby_list[i].Reset();
			}
		}

		GridInfo grid_list[SIDE_CHARACTER_NUM]; 
		PetStandbyInfo standby_list[CLOUD_ARENA_STANDBY_PET_MAX_NUM];
	};
}

#pragma pack(pop)

#endif