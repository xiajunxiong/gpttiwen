#ifndef __MSG_TASK_H__
#define __MSG_TASK_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/taskdef.h"
#include "servercommon/taskchaindef.h"
#include "servercommon/anecdotetaskdef.hpp"
#include "servercommon/prestigetaskdef.hpp"
#include "gamedef.h"
#include "servercommon/treasuretaskdef.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSTaskInfoSingleHandle
	{
	public:
		CSTaskInfoSingleHandle();		//!< 1720 - 单个任务操作请求
		MessageHeader header;

		enum TASK_OP_TYPE
		{
			TASK_ACCEPT = 0,		//!< 接受任务
			TASK_GIVEUP = 1,		//!< 放弃任务
			TASK_COMMIT = 2,		//!< 提交任务
			TASK_DEMAND = 3,		//!< 查询任务状态		0 - 未进行；	1 - 已完成；	2 - 进行中
			TASK_PATROL = 4,		//!< 巡逻任务			0 - 请求下发    1 - 接取任务    2-任务提交
			TASK_SKIP_BATTLE = 5,	//!< 任务跳过战斗
		};

		UInt16 op_type;			//!< ushort 操作类型

		TaskID task_id;			//!< ushort 任务ID
	};
	
	class SCTaskDetailedInfoList
	{
	public:
		SCTaskDetailedInfoList();			//!< 1721 - 可接和已接的信息总表
		MessageHeader header;

		struct TaskProCon
		{
			TaskInfoPro task_con;
			int list_type;			//!< 任务列表类型 - 0：已接列表 | 1：可接列表
		};

		int list_count;		//!< 任务数量
		TaskProCon task_list_info[MAX_TASK_LIST_LEN_NUM];
	};

	class SCTaskDeleteNotifyList
	{
	public:
		SCTaskDeleteNotifyList();			//!< 1722 - 从总表中移除任务信息
		MessageHeader header;

		struct TaskRemoveCon
		{
			short remove_reason;		//!< 移除原因 0 - 其它原因 1 - 任务完成
			unsigned short task_id;
		};

		int list_count;
		TaskRemoveCon info_list[MAX_TASK_LIST_LEN_NUM];
	};

	class CSTalkWithAppointedNPC
	{
	public:
		CSTalkWithAppointedNPC();		//!< 1723 - 与任务指定的 NPC 交谈
		MessageHeader header;

		TaskID task_id;		//!< ushort 任务ID
		UInt16 npc_id;		//!< ushort NPC ID
	};
	
	class CSGiveNPCTaskNeedItems
	{
	public:
		CSGiveNPCTaskNeedItems();		//!< 1724 - 提交道具(列表)
		MessageHeader header;

		TaskID task_id;		//!< ushort 任务ID
		UInt16 list_count;	//!< ushort 提交的道具种数

		ItemContainer commit_list[MAX_ITEM_NUM_IN_GROUP];	//!< 道具表
	};

	class SCTaskCompleteTaskList
	{
	public:
		SCTaskCompleteTaskList();		//!< 1725 - 已完成的任务列表
		MessageHeader header;

		int list_count;			//!< 列表长度
		TaskID task_list[MAX_TASK_RECORD_NUM];	//!< 任务ID
	};

	class SCTaskCanAbandonedList
	{
	public:
		SCTaskCanAbandonedList();		//!< 1726 - 任务能否被放弃	0 - 不能； 1 - 可以
		MessageHeader header;

		int list_num;		//!< 列表长度
		ItemContainer can_abandon_list[MAX_ACCEPTED_TASK_NUM];	//!< 信息列表
	};

	class SCTaskSingleStatusInfo
	{
	public:
		SCTaskSingleStatusInfo();		//!< 1727 - 单个任务当前状态   0 - 未进行； 1 - 已完成； 2 - 进行中
		MessageHeader header;

		ItemContainer info_con;		//!< 状态容器
	};

	class CSTaskChainOp				//!<1728	任务链操作
	{
	public:
		CSTaskChainOp();
		MessageHeader header;

		enum OP_TYEP
		{
			ACCEPT = 0,			//!< 接取
			GIVEUP = 1,			//!< 放弃
			DELIVERY = 2,		//!< 交货	column|index
			TALK = 3,			//!< 交谈
			SKIP = 4,			//!< 跳过
			DELIVERY_PET = 5,	//!< 上交宠物 pet_index
			FETCH_REWARD = 6,	//!< 领取转盘奖励
			WAIT_ITEM = 7,		//!< 领取代送道具
		};

		int type;
		int param1;
		int param2;
	};

	class SCTaskChainInfo		//!<1729	任务链信息
	{
	public:
		SCTaskChainInfo();
		MessageHeader header;

		int pass_count;								//!< 完成了多少次
		int task_type;								//!< 当前任务类型
		int task_param[TASK_CHAIN_PARAM_COUNT];		//!< 任务的参数
		int can_skip;								//!< 能否跳过
		int price_type;								//!< 花费类型
		int price;									//!< 花费金额
		int task_count;								//!< 阶段
		short is_new;								//!< 是否显示为新任务
		short completion_times;						//!< 玩家当前所能做的总环数
		long long task_limit_timestamp;				//!< 特殊时间限制戳
		short task_pass_count;						//!< 本阶段的任务进度
		short cur_task_max_count;					//!< 本阶段的最大环数
		int accept_task_level;						//!< 每次接取任务的等级
	};
	UNSTD_STATIC_CHECK(10 == TASK_CHAIN_PARAM_COUNT);

	class CSSetupTaskTraceStatus
	{
	public:
		CSSetupTaskTraceStatus();		//!< 2008 - 设置任务追踪状态
		MessageHeader header;

		int list_num;
		unsigned int param_list[TASK_TRACE_MAX_NUM];
	};

	class SCTaskTraceInformation
	{
	public:
		SCTaskTraceInformation();		//!< 2009 - 下发任务追踪设置
		MessageHeader header;

		int list_num;
		unsigned int param_list[TASK_TRACE_MAX_NUM];
	};

	class SCTaskChainMonsterInfo
	{
	public:
		SCTaskChainMonsterInfo();		//!< 2016 - 任务链 - 刷怪通知
		MessageHeader header;

		int monster_id;
		int monster_group_id;
		int scene_id;
		int pos_x;
		int pos_y;
	};

	class CSTaskGameReq				//!< 2026 - 任务小游戏
	{
	public:
		CSTaskGameReq();
		MessageHeader header;
		int task_game_type;
		int param1;
		int param2;
	};

	class CSTaskCommitMoneyReq				//!< 2029 - 提交货币
	{
	public:
		CSTaskCommitMoneyReq();
		MessageHeader header;
		
		int task_id;
	};

	
	class CSTaskSkipReq				//!< 4434 - 跳过任务-通用
	{
	public:
		CSTaskSkipReq();
		MessageHeader header;

		int task_id;
	};

	class SCPatrolTaskInfo
	{
	public:
		SCPatrolTaskInfo();		//!< 2020 - 巡逻任务
		MessageHeader header;

		char today_flag;				//是否领取过巡逻任务
		char today_reward;				// 是否获取过巡逻任务奖励
		short task_progress;			//任务进度
		short task_status;				//领取状态  0-不可领取 1-可领取(但未领取) 2-已领取
		unsigned int task_coin;			//领取铜币
	};

	class SCBountyTaskTurnTableRewardInfo	//!< 2184 赏金任务转盘奖励信息
	{
	public:
		SCBountyTaskTurnTableRewardInfo();
		MessageHeader header;

		short reward_idx;		//!< 转盘指向的奖励 值为-1时无效
		short group_id;			//!< 奖励组ID
	};

	class CSBountyTaskTurnTableRewardFetch  //!< 2183 领取赏金任务转盘奖励
	{
	public:
		CSBountyTaskTurnTableRewardFetch();
		MessageHeader header;
	};

	class SCBountyTaskInfo		// 2185 赏金任务进度信息
	{
	public:
		SCBountyTaskInfo();
		MessageHeader header;

		short cur_finish_task_num;	//!< 当前任务进度
		short today_task_num;		//!< 今天总共要做得任务数
		short has_finish_first_task;//!< 是否完成了第一个任务
		short reserve_sh;			//!< 保留字
	};

	class CSTaskChosedReq		// 3234 剧情选项记录请求
	{
	public:
		enum TASK_CHOSED_REQ_TYPE
		{
			TASK_CHOSED_REQ_TYPE_INFO = 0,	//!< 请求下发
			TASK_CHOSED_REQ_TYPE_SET = 1,	//!< 设置
		};
		CSTaskChosedReq();
		MessageHeader header;

		unsigned short req_type;
		unsigned short param1;
		unsigned short param2;
		unsigned short param3;
	};

	class SCTaskChosedInfo		// 3235 剧情选项记录下发
	{
	public:
		SCTaskChosedInfo();
		MessageHeader header;

		int info_count;
		TaskChosedInfo chosed_list[MAX_TASK_CHOSED_RECORD_NUM];
	};

	class SCTaskChainTurnTableInfo	//!< 3236 任务链信息转盘下发
	{
	public:
		SCTaskChainTurnTableInfo();
		MessageHeader header;

		short turn_table_index;				// 转盘随机的奖励下标
		short turn_table_group_id;			// 转盘奖励组
	};

	struct AnecdoteTaskMsgInfo
	{
		short anecdote_task_type;	//!< 奇闻任务类型 \see ANECDOTE_TASK_TYPE
		unsigned short cur_task_id;	//!< 当前任务ID
		short has_finish;			//!< 是否已完成该类型奇闻任务
		short accept_status;		//!< 当前任务的接受状态 0:可接受 1:已接受
		int progress;				//!< 任务进度
	};

	class SCAnecdoteTaskInfo	//!< 3490 单个奇闻任务信息下发 （改变数据时下发）
	{
	public:
		SCAnecdoteTaskInfo();
		MessageHeader header;

		AnecdoteTaskMsgInfo task_info;
	};

	class SCAnecdoteTaskInfoList	//!< 3491 奇闻任务列表  上线时下发
	{
	public:
		SCAnecdoteTaskInfoList();
		MessageHeader header;

		int count;
		AnecdoteTaskMsgInfo task_info_list[ANECDOTE_TASK_TYPE_MAX];
	};

	class SCAnecdoteBigEventInfo		//!< 3499 奇闻大事件信息下发
	{
	public:
		SCAnecdoteBigEventInfo();
		MessageHeader header;

		char reward_version;				//!< 奖励版本号
		char is_big_event_open_now;			//!< 大事件是否开启中
		short anecdote_finish_num;			//!< 奇闻完成数量
		unsigned int end_timestamp;			//!< 大事件结束时间戳
		unsigned int reward_fetch_status;	//!< 大事件奖励领取标记（二进制形式）
	};

	class SCAnecdoteTaskBattleFail		//!< 3492 奇闻任务战斗失败
	{
	public:
		SCAnecdoteTaskBattleFail();
		MessageHeader header;

		short anecdote_task_type;
		unsigned short task_id;
	};

	class CSAnecdoteTaskOperate			//!< 3494 奇闻任务请求
	{
	public:
		CSAnecdoteTaskOperate();
		MessageHeader header;

		enum OPERATE_TYPE
		{
			OPERATE_TYPE_ACCEPT_TASK = 0, //!< 接受任务  p1 任务ID
			OPERATE_TYPE_COMMIT_TASK = 1, //!< 提交任务  p1 任务ID
			OPERATE_TYPE_CLAIM_REWARD = 2,//!< 领取任务奖励 p1 奇闻任务类型 \see ANECDOTE_TASK_TYPE
			OPERATE_TYPE_FETCH_BIG_EVENT_REWARD = 3, //!< 领取大事件奖励  p1 奖励seq
		};

		short operate_type;		//!< 操作类型
		unsigned short param1;	//!< 参数1
	};

	class SCAnecdoteTaskCompleteNotice	//!< 3493 奇闻任务完成通知
	{
	public:
		SCAnecdoteTaskCompleteNotice();
		MessageHeader header;

		unsigned short task_id;
		short anecdote_task_type;
	};

	class SCAnecdoteTaskSpecialInfo	//!< 3495 奇闻任务-其他特殊下发
	{
	public:
		SCAnecdoteTaskSpecialInfo();
		MessageHeader header;

		int special_value[ANECDOTE_TASK_TYPE_MAX];
	};

	class CSAnecdoteTaskSpecialReq	//!< 3496 奇闻任务-其他特殊设置
	{
	public:
		CSAnecdoteTaskSpecialReq();
		MessageHeader header;

		int req_type;//奇闻类型
		int req_value;
	};

	class CSNewTaskAction		//!< 3500 新的任务行为请求
	{
	public:
		CSNewTaskAction();
		MessageHeader header;

		short action_type;		//!< 行为类型 \see NEW_TASK_ACTION 
		short reserve_sh;		//!< 保留字
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class CSNewTaskGiveNPCTaskNeedItems
	{
	public:
		CSNewTaskGiveNPCTaskNeedItems();		//!< 3504 - 提交道具(列表)
		MessageHeader header;

		TaskID task_id;		//!< ushort 任务ID
		UInt16 list_count;	//!< ushort 提交的道具种数

		ItemContainer commit_list[MAX_ITEM_NUM_IN_GROUP];	//!< 道具表
	};
	
	class SCNewTaskPartnerSoarInfo		 //!< 3505 伙伴飞升任务相关信息
	{
	public:
		SCNewTaskPartnerSoarInfo();
		MessageHeader header;

		char is_cur_accept_partner_soar_task;	// 是否是当天接取的伙伴飞升任务 1:当天 0:初始状态 2:不是当天
		char reserve_ch;					
		short reserve_sh;			
	};

	class CSNormalTaskFinishAnedoteGame	//!< 3497 普通任务完成奇闻小游戏（非奇闻任务）
	{
	public:
		CSNormalTaskFinishAnedoteGame();
		MessageHeader header;

		TaskID task_id;
		short anedote_type;
	};

	class SCResearchTaskNoticInfo	//!< 3498 三界除魔结算界面
	{
	public:
		SCResearchTaskNoticInfo();
		MessageHeader header;
		const static int MAX_REWARD_LIST = 10;//最多10种
		struct Item_info
		{
			Item_info() : item_id(0), num(0), is_bind(0) {}
			ItemID item_id;
			short num;
			int is_bind;
		};
		long long add_exp;
		
		int reward_num;
		Item_info reward_list[MAX_REWARD_LIST];
	};

	class SCPrestigeTaskInfo	//!< 2290 声望任务数据下发
	{
	public:
		struct TaskData
		{
			UInt16 task_id;
			short star;
		};

		SCPrestigeTaskInfo();
		MessageHeader header;

		short village_id;				//!< 今天选择的村庄ID (0标示未选择)
		short is_accept_task;			//!< 是否接取任务
		short today_finish_task_num;	//!< 今天已完成的任务数
		short today_task_num;			//!< 今天的任务数
		TaskData today_task_list[MAX_PRESTIGE_TASK_NUM_PER_DAY]; //!< 今天任务列表 (数组按today_task_num读取）
	};

	class CSPrestigeTaskSelectVillage	//!< 2291 选择声望任务村庄
	{
	public:
		CSPrestigeTaskSelectVillage();
		MessageHeader header;

		int village_id;					//!< 选择的村庄id
	};

	class SCPrestigeTaskFinalNotice		//!< 2292 声望任务最后通知
	{
	public:
		SCPrestigeTaskFinalNotice();
		MessageHeader header;

		int village_id;					//!< 选择的村庄id
		int total_add_prestige;			//!< 总计获得声望
	};

	class CSTreasureTaskReq					// 4349
	{
	public:
		CSTreasureTaskReq();
		MessageHeader header;

		short type;
		short param;
	};

	class SCTreasureTaskInfo				// 4350
	{
	public:
		SCTreasureTaskInfo();
		MessageHeader header;

		short level;					// 接任务时的等级
		short order_id;					// 当前任务ID
		short route;					// 路线
		short pos_id;					// 坐标点ID

		short pool[TREASURE_TASK_POOL_NNM];		// 奖池 8
	};

	class CSPrestigeTaskReq	//!< 2293 声望任务请求
	{
	public:
		CSPrestigeTaskReq();
		MessageHeader header;

		short type;
		short param;
	};

	class SCTaskWaitingList		// 1352 等待任务列表
	{
	public:
		SCTaskWaitingList();
		MessageHeader header;

		struct WaitingTask
		{
			TaskID task_id;		// 任务id
			short reserve_sh;	// 保留字
			unsigned int wait_until_timestamp; // 任务开放的时间戳
		};

		int count;
		WaitingTask task_list[100];
	};

	class CSSpecialAnecdoteReq		// 1354 奇闻特殊任务开始
	{
	public:
		CSSpecialAnecdoteReq();
		MessageHeader header;

		int task_type;
		int task_seq;
	};

	class SCSpecialAnecdoteList		// 1355 奇闻特殊任务列表下发
	{
	public:
		SCSpecialAnecdoteList();
		MessageHeader header;

		SpecialTaskInfo task_list[MAX_SPECIAL_ANECDOTE_TASK_TYPE];
	};

	class SCSpecialAnecdoteSingleList		// 1356 奇闻特殊任务下发 - 单个
	{
	public:
		SCSpecialAnecdoteSingleList();
		MessageHeader header;
		int task_type;
		SpecialTaskInfo task_info;
	};
}

#pragma pack(pop)
#endif