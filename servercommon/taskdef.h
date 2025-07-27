#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#include "engineadapter.h"

#pragma pack(push, 4)

static const int MAX_ACCEPTED_TASK_NUM = 350;		// 最多同时拥有的任务数量
static const int MAX_ACCEPTED_TASK_NUM_ALLOW = 300; // 最多允许非主线，进阶任务可接上限
static const int MAX_TASK_LIST_LEN_NUM = 2048;		// 单次发送的任务列表的长度
static const int MAX_ITEM_NUM_IN_GROUP = 10;		// 每个道具组里最多拥有的道具种数
static const int TASK_EXPIRE_CHECK_INTERVAL = 3;	// 任务超时检测间隔
static const int TASK_TRACE_MAX_NUM = 350;			// 最大任务追踪数量
UNSTD_STATIC_CHECK(TASK_TRACE_MAX_NUM >= MAX_ACCEPTED_TASK_NUM);

static const int MAX_TASK_RECORD_NUM = 4990;
static const int MAX_TASK_RECORD_LEN = sizeof(short) + sizeof(short) * MAX_TASK_RECORD_NUM;
static const int MAX_TASK_CHOSED_RECORD_NUM = 512;

static const int TASK_NEED_HAVE_PROPERTY_LIST_REQUIRE_NUM = 1;

enum TASK_TYPE		// 任务类型
{
	TASK_TYPE_MAIN_QUEST = 0,				// 主线任务
	TASK_TYPE_SIDE_QUEST = 1,				// 支线任务
	TASK_TYPE_CHALLENGES = 2,				// 挑战任务
	TASK_TYPE_STAGE_TASK = 3,				// 进阶任务
	TASK_TYPE_MEDAL_TASK = 4,				// 勋章任务
	TASK_TYPE_TRMAP_TASK = 5,				// 宝图任务
	TASK_TYPE_PROFS_TASK = 6,				// 门派任务(职业任务 / 学院任务)
	TASK_TYPE_CHAIN_TASK = 7,				// 周常任务 - 任务链
	TASK_TYPE_GUILD_TASK = 8,				// 家族任务
	TASK_TYPE_RESEARCHES = 9,				// 除魔任务(真中的研究)
	TASK_TYPE_ADVENTURES = 10,				// 奇遇任务
	TASK_TYPE_SPECIAL_QUEST = 11,			// 特殊任务（给策划用的）推荐任务
	TASK_TYPE_BOUNTY_TASK = 12,				// 赏金任务
	TASK_TYPE_TRANSACTION_TASK = 13,		 //购买任务 - 丝绸之路
	TASK_TYPE_ANECDOTE_TASK = 14,			// 奇闻任务
	TASK_TYPE_PRESTIGE_TASK = 15,			// 声望任务
	TASK_TYPE_FLY_UP_TASK = 16,				// 角色飞升任务
	TASK_TYPE_PARTNER_SOAR_TASK = 17,		// 伙伴飞升任务
	TASK_TYPE_PARTNER_SOAR_FRONT_TASK = 18,	// 伙伴飞升前置任务
	TASK_TYPE_PET_FEI_SHENG_TASK = 19,		// 宠物飞升任务
	TASK_TYPE_FLY_UP_FRONT_TASK = 20,		// 角色飞升前置任务
	TASK_TYPE_DIANHUA_TASK = 21,	// 点化任务
	TASK_TYPE_STAGE_TASK_1 = 22,	//进阶任务1
	TASK_TYPE_SERIAL_FEEDER_TASK = 23,		// 连环式支线任务
	TASK_TYPE_SERIAL_FEEDER_TASK2 = 24,		// 连环式支线任务2
	TASK_TYPE_SERIAL_FEEDER_TASK_HOLY = 25,	// 圣器连环支线任务
	TASK_TYPE_TEAM_CHALLENGES = 26,			// 组队挑战任务
	TASK_TYPE_ROLE_GUILD_TASK = 27,		// 角色家族任务

	TASK_TYPE_MAX
};

enum TASK_TEAM_LIMIT	// 组队限制
{
	TASK_TEAM_UNCONFINED = 0,		// 没有限制
	TASK_TEAM_MUST_ALONE = 1,		// 必须单独完成
	TASK_TEAM_AT_LEAST_2 = 2,		// 最少 2 人组队
	TASK_TEAM_AT_LEAST_3 = 3,		// 最少 3 人组队
	TASK_TEAM_AT_LEAST_4 = 4,		// 最少 4 人组队
	TASK_TEAM_AT_LEAST_5 = 5,		// 最少 5 人组队

	TASK_TEAM_TYPE_MAX				// 组队类型限制最大值
};

enum TASK_CONDITION		// 完成条件
{
	TASK_NEED_TO_DO_NOTHING = 0,		// 什么都不做
	TASK_NEED_TALK_WITH_NPC = 1,		// 与 NPC 交谈
	TASK_NEED_KILL_MONGROUP = 2,		// 消灭怪物组中的任意怪物
	TASK_NEED_PASS_TRANSCRI = 3,		// 通关副本类型
	TASK_NEED_INTO_SCENARIO = 4,		// 进入场景ID
	TASK_NEED_LOAD_PROGRESS = 5,		// 加载进度条(类似于采集、打铁啥的需要进度条的操作)
	TASK_NEED_GIVE_PROGROUP = 6,		// 交予道具组
	TASK_NEED_USES_PROGROUP = 7,		// 使用道具组
	TASK_NEED_BUY_PROPERTY = 8,			// 购买道具
	TASK_NEED_PLAY_CUTSCENE = 9,		// 播放剧情动画
	TASK_NEED_KILL_MONSTERS = 10,		// 消灭单种怪物
	TASK_NEED_GIVE_PROPERTY = 11,		// 提交单种道具
	TASK_NEED_USES_PROPERTY = 12,		// 使用单种道具
	TASK_NEED_KILL_IN_ORDER = 13,		// 按顺序击杀怪物
	TASK_NEED_HAVE_PROPERTY = 14,		// 拥有道具
	TASK_NEED_BEAT_MONGROUP = 15,		// 战胜某个关卡 / 击败某组怪物
	TASK_NEED_MEET_REQUIRES = 16,		// 满足需求
	TASK_NEED_TRAP_CREATURE = 17,		// 捕捉生物
	TASK_NEED_GAIN_TASKPROP = 18,		// 杀怪收集任务道具
	TASK_NEED_ENTER_FB = 19,			// 进入副本类型
	TASK_NEED_ESCORT = 20,				// 运镖
	TASK_NEED_COMMIT_PROPERTY = 21,		// 提交物品（提交任务时触发）
	TASK_NEED_PASS_DESIGNATED_MEDAL_FB = 22,	// 通关指定勋章副本 p1:指定副本seq
	TASK_NEED_HAVE_PROPERTY_LIST = 23,	// 拥有道具组(仅仅允许任务道具)
	TASK_NEED_MEET_ANECDOTE_REQUIRES = 24, // 满足奇闻NPC的需求 p1:奇闻类型
	TASK_NEED_PASS_CRYSTAL_FB_SEQ = 25,	// 通关指定水晶副本 p1:指定水晶副本seq p2:指定次数
	TASK_NEED_ACCEPT_ESCORT = 26,		// 接镖
	TASK_NEED_COURAGE_CHALLENGE = 27,	// 通关锢魔之塔
	TASK_NEED_CREATE_ROLE_DAYS = 28,	// 创角天数 
	TASK_NEED_MATERIAL = 29,			// 玉虚境 p1:chapter_id(从1开始) p2:level(从1开始)
	TASK_NEED_MI_YI = 30,				// 远征魔冢 p1:远征哪一层 p2:怪物数
	TASK_NEED_WAIT_UNTIL_NEXT_DAY = 31,	// 等待直到第二天 
	TASK_NEED_PARTNER_LEVEL_ACHIVE = 32,// 伙伴达到指定等级 p1 partner_id p2 level
	TASK_NEED_VILLAGE_LEVEL_ACHIVE = 33,// 指定村庄声望达到指定等级 p1 village_id p2 level
	TASK_NEED_COLLECTION_TU_JIAN_ACTIVE_NUM = 34,	// 收集图鉴激活个数 param2 个数
	TASK_NEED_ONE_FA_BAO_UP_LEVEL = 35,		// 升级任 1 法宝多少级  param2 级数
	TASK_NEED_OPEN_YIN_YANG_EYE = 36,		// 开关阴阳眼 1:开 0:关
	TASK_NEED_PLAY_GAME = 37,				// 玩游戏 p1:游戏类型 p2(胜利次数)
	TASK_NEED_PASS_WANLING_FB = 38,		// 通关万灵画本 p1:画本ID   p2:次数
	TASK_NEED_CAREER_ADVANCE = 39,		// 职业晋升  p1:大阶  p2:小阶
	TASK_NEED_COMMIT_MONEY = 40,			// 提交货币 param1 货币类型 param2 数量
	TASK_NEED_DREAM_NOTES_CHALLENGE = 41,	// 通关梦渊笔录 param1:层数
	TASK_NEED_PARTNER_STRENGTHEN = 42,		 // 指定伙伴强化 P1:伙伴ID P2:强化次数
	TASK_NEED_PARTNER_BREAK = 43,			 // 指定伙伴晋升 P1:伙伴ID P2:晋升次数
	TASK_NEED_ROLE_BEAT_MONGROUP = 44,		 // 玩家个人怪   p1 怪物id 
	TASK_NEED_COMMIT_PET = 45,		 //提交宠物 p1 是宠物id

	TASK_NEED_MAX
};

enum TASK_GAME_PLAY_TYPE//TASK_NEED_PLAY_GAME P1 枚举
{
	TASK_GAME_PLAY_TYPE_0 = 0,//元素克制
	TASK_GAME_PLAY_TYPE_1 = 1,//破坏祭坛 p2破坏次数
	TASK_GAME_PLAY_TYPE_2 = 2,//修补阵法 p2修复次数
	TASK_GAME_PLAY_TYPE_3 = 3,//修行 p2修行次数
	TASK_GAME_PLAY_TYPE_MAX,
};

enum TASK_MEET_REQUIRES_TYPE	// 16 号枚举的 参数1
{
	TASK_MEET_PROF_PROMOTE = 0,		// 职业进阶 - 参数2 进阶等级
	TASK_MEET_HAVE_PARTNER = 1,		// 拥有伙伴 - 参数2 伙伴ID
	TASK_MEET_LEARN_SKILLS = 2,		// 学习技能
	TASK_MEET_JOIN_BATTLES = 3,		// 参与战斗 - 参数2 怪物(组)ID
	TASK_MEET_OPEN_A_CHEST = 4,		// 打开宝箱 - 参数2 宝箱ID
	TASK_MEET_OWN_CREATURE = 5,		// 拥有宠物 - 参数2 宠物ID
	TASK_MEET_PLAY_ONCE_COLOSSEUM = 6,	// 参加一次古神宝库
	TASK_MEET_TRANSACTION_TASK = 7,		//丝绸之路
	TASK_MEET_MEDAL_TASK_PLAY_VIDEO = 8, // 勋章任务打开密笺
	TASK_MEET_PET_RELEASE = 9,		// 宠物放生
	TASK_MEET_PET_COMPOSE = 10,		// 宠物合成
	TASK_MEET_DREAM_NOTES_CHALLENGE = 11,		// 通关梦渊笔录 param2:目标层数
	TASK_MEET_BOUNTY_TASK = 12,					// 赏金任务 param2:完成次数
	TASK_MEET_RESEARCH_TASK = 13,				// 三界除魔 param2：完成次数
	TASK_MEET_PET_MONOPOLY_CATCH_PET = 14,	// 灵宠岛获得X只宠物 param2:只数
	TASK_MEET_LUN_HUI_WANG_CHUAN = 15,		// 轮回忘川通过第几层 param2:层数
	TASK_MEET_GOD_PRINT_WISH = 16,	// 祈神次数 param 2 次数
	TASK_MEET_SKILL_LEVEL = 17,		// 主角技能等级  param2:等级
	TASK_MEET_JOIN_GUILD = 18,		// 加入一个家族
	TASK_MEET_TREASURE_TASK = 19,	// 九州秘宝 param2 次数
	TASK_MEET_BOUNTY_LAST_TASK = 20,// 完成N轮赏金任务 param2 次数
	TASK_MEET_TREASURE_LAST_TASK = 21,	// 完成N轮九州秘宝 param2 次数
	TASK_MEET_BOUNTY_TASK2 = 22,	// 完成2*N次赏金任务 param2 次数(N要小于一天的任务量/2)
	TASK_MEET_WANLING_FB_TIMES = 23,// 今日完成N次万灵画本  param2 次数		
	TASK_MEET_KILL_WILD_BOSS = 24,	// 击败野外首领 param2 次数
	TASK_MEET_REALM_BREAK_FIGHT = 25, // 心灵之境通关 P2:第几关 

	TASK_MEET_TYPE_MAX
};

enum TASK_GIVE_UP_TYPE
{
	GIVEUP_TYPE_CAPTAIN_CAN = 0,	// 当身份为『队长』时，能不能放弃任务
	GIVEUP_TYPE_CAPTAIN_ALL = 1,	// 当身份为『队长』时，放弃任务是否波及全队
	GIVEUP_TYPE_MEMBERS_CAN = 2,	// 当身份为『队员』时，能不能放弃任务（默认不会波及全队）
	GIVEUP_TYPE_ONESELF_CAN = 3,	// 当身份为『个人』时，能不能放弃任务，适用于没有组队的情况

	GIVEUP_TYPE_MAX
};

enum TASK_REWARD_ALLOT_TYPE		// 任务奖励分配类型	AT -> According to，根据
{
	ALLOT_TYPE_COMMON = 0,		// 只要完成任务，奖励一概固定
	ALLOT_TYPE_ATPROF = 1,		// 根据职业不同，奖励也有所不同
	ALLOT_TYPE_ATLEVE = 2,		// 根据等级不同，奖励也有所不同
	ALLOT_TYPE_PET_CHOICE = 3,	// 根据历史选择的宠物奖励seq

	ALLOT_TYPE_MAX
};

enum TASK_STATE_TYPE
{
	TASK_STATE_UNDERWAY = 0,	// 进行中
	TASK_STATE_FINISHED = 1,	// 可提交

	TASK_STATE_MAX
};

enum SPECIAL_ENUM
{
	TASK_IS_SHARE_NONE = 0,			// 任务是否全队同时接受 - 否
	TASK_IS_SHARE_TRUE = 1,			// 任务是否全队同时接受 - 是

	TASK_GIVEUP_TYPE_NONE = 0,		// 放弃类型 - 逻辑否
	TASK_GIVEUP_TYPE_TRUE = 1,		// 放弃类型 - 逻辑是

	TASK_REMOVE_REASON_OTHERS = 0,	// 任务移除原因 - 其它原因
	TASK_REMOVE_REASON_FINISH = 1,	// 任务移除原因 - 完成任务

	TASK_STATE_HAVE_NO_CONDUCTED = 0,	// 任务状态 - 未进行
	TASK_STATE_ALREADY_COMPLETED = 1,	// 任务状态 - 已完成
	TASK_STATE_STILLS_IN_PROCESS = 2,	// 任务状态 - 进行中

	TASK_LIST_TYPE_ACCEPTED = 0,		// 任务列表 - 已接受
	TASK_LIST_TYPE_ACCEPTABLE = 1,		// 任务列表 - 可接受
};

enum TASK_CHECK_OPERA_TYPE
{
	TASK_CHECK_OPERA_TYPE_CLEAR = 0,		//清空该类型任务， param 任务类型
	TASK_CHECK_OPERA_TYPE_HAS_TASK = 1,		//查找该类型任务， param 任务类型

	TASK_CHECK_OPERA_TYPE_MAX,
};

struct ItemContainer	// 道具组 - 任务状态 容器
{
	ItemContainer(unsigned short p_item_id = 0, unsigned short p_com_num = 0)
	{
		item_id = p_item_id;
		com_num = p_com_num;
	}

	unsigned short item_id;		// ushort 道具ID			任务ID
	unsigned short com_num;		// ushort 提交的数量			任务状态

	bool operator < (const ItemContainer & cobj) const
	{
		if (this->item_id < cobj.item_id) return true;
		else if (this->item_id == cobj.item_id && this->com_num < cobj.com_num) return true;
		
		return false;
	}
};

struct TaskInfoPro		// 任务信息 - 协议格式
{
	TaskInfoPro() { this->Reset(); }

	void Reset()
	{
		task_id = 0;
		state_type = 0;
		prograss = 0;
		expire_time = 0;
	}

	unsigned short task_id;			// 任务ID
	char state_type;				// 状态类型 \see TASK_STATE_TYPE
	unsigned char prograss;			// 进程

	unsigned int expire_time;		// 任务到期时间
};

struct TaskInfo		// 任务信息 - 存库格式
{
	TaskInfo(unsigned short new_task_id = 0, unsigned char cur_versions = 0, unsigned int p_expire_time = 0) { this->Reset(new_task_id, cur_versions, p_expire_time); }

	void Reset(unsigned short new_task_id = 0, unsigned char cur_versions = 0, unsigned int p_expire_time = 0)
	{
		task_id = new_task_id;
		prog_num = 0;
		versions = cur_versions;
		expire_time = p_expire_time;
	}

	unsigned short task_id;			// 任务ID
	unsigned char prog_num;			// 进度数值(例如杀怪的数量、获取的道具数量等)
	unsigned char versions;			// 任务版本

	unsigned int expire_time;		// 任务到期的时间戳 timestamp
};

enum PARTNER_SOAR_TASK_STATUS
{
	PARTNER_SOAR_TASK_STATUS_BEGIN = 0,
	PARTNER_SOAR_TASK_STATUS_ACCEPT = 1,	//伙伴飞升任务接取中
	PARTNER_SOAR_TASK_STATUS_FINISH = 2,	//伙伴飞升任务完成

	PARTNER_SOAR_TASK_STATUS_MAX
};

static const int PARTNER_UPGRADE_TASK_PRESTIGE_VILLAGE_ID = 2;	//宛村
static const int PARTNER_UPGRADE_TASK_ITEM_ID = 26722;			//鹿尧碎片

struct TaskParam
{
	TaskParam() { this->Reset(); }

	void Reset()
	{
		task_count = 0;
		
		for (int t_cur = 0; t_cur < MAX_ACCEPTED_TASK_NUM; ++t_cur)
		{
			task_info_list[t_cur].Reset();
		}

		today_adv_tri_times = 0;
		adv_task_id = 0;
		adv_task_timestamp = 0;

		is_cur_accept_partner_soar_task = 0;
		partner_soar_task_status = PARTNER_SOAR_TASK_STATUS_BEGIN;
		cur_partner_soar_task_id = 0;

		is_partner_upgrade_task_buy_flag = 0;
		is_old_user_partner_upgrade_task_buy_flag = 0;
		reserve_sh = 0;
	}

	const TaskInfo * GetTaskByID(unsigned short task_id)
	{
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (task_info_list[t_cur].task_id == task_id)
			{
				return & task_info_list[t_cur];
			}
		}

		return NULL;
	}

	bool InsertTask(const TaskInfo & p_task_info)
	{
		if (task_count >= MAX_ACCEPTED_TASK_NUM) return false;
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (p_task_info.task_id == task_info_list[t_cur].task_id) return false;
		}

		task_info_list[task_count++] = p_task_info;
		return true;
	}

	void RemoveTask(const unsigned short task_id)		// 用最后一个任务覆盖这个任务
	{
		if (task_count <= 0 || task_count > MAX_ACCEPTED_TASK_NUM) return;
		for (int t_cur = 0; t_cur < task_count; ++t_cur)
		{
			if (task_info_list[t_cur].task_id != task_id) continue;
			
			task_info_list[t_cur] = task_info_list[task_count - 1];
			--task_count;
			return;
		}
	}

	void RemoveTaskByIdx(int index)
	{
		if (index < 0 || index >= task_count || index >= MAX_ACCEPTED_TASK_NUM || task_count <= 0 || task_count > MAX_ACCEPTED_TASK_NUM) return;

		task_info_list[index] = task_info_list[task_count - 1];
		--task_count;
	}

	int task_count;
	TaskInfo task_info_list[MAX_ACCEPTED_TASK_NUM];

	unsigned short today_adv_tri_times;		// 今日奇遇任务触发的次数
	unsigned short adv_task_id;				// 当前标记的奇遇任务ID
	unsigned int adv_task_timestamp;		// 奇遇任务超时时间戳

	char is_cur_accept_partner_soar_task;		// 是否是当天接取的伙伴飞升任务 1:当天 0:初始状态 2:不是当天
	char partner_soar_task_status;				// 伙伴飞升任务状态
	unsigned short cur_partner_soar_task_id;	// 当前接取的飞升任务ID

	bool is_partner_upgrade_task_buy_flag;				//特殊伙伴晋升标识(购买过指定宛村声望商店,指定道具ID)
	char is_old_user_partner_upgrade_task_buy_flag;		//老玩家根据以前数据判断晋升标识
	short reserve_sh;
};
typedef char TaskParamHex[sizeof(TaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskParamHex) < 6400);

struct TaskTraceParam		// 任务追踪设置
{
	TaskTraceParam() { this->Reset(); }

	void Reset()
	{
		list_len = 0;
		memset(trace_list, 0, sizeof(trace_list));
	}

	int list_len;
	unsigned int trace_list[TASK_TRACE_MAX_NUM];
};
typedef char TaskTraceParamHex[sizeof(TaskTraceParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskTraceParamHex) < 3072);

struct TaskChosedInfo
{
	TaskChosedInfo() {}
	unsigned short index;
	unsigned short param;
};

struct TaskChosedParam		// 任务选择记录
{
	TaskChosedParam() { this->Reset(); }

	void Reset()
	{
		chosed_count = 0;
		memset(chosed_list, 0, sizeof(chosed_list));
	}

	int chosed_count;
	TaskChosedInfo chosed_list[MAX_TASK_CHOSED_RECORD_NUM];
};
typedef char TaskChosedParamHex[sizeof(TaskChosedParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskChosedParamHex) < 4500);


enum NEW_TASK_ACTION
{
	NEW_TASK_ACTION_TALK_TO_NPC = 0,	// 与NPC对话 param1 npc_seq
	NEW_TASK_ACTION_COMMIT_ITEM = 1,	// 提交物品 param1:item_id  param2:num  param3:task_id
	NEW_TASK_ACTION_ANECDOTE_SUCC = 2,	// 奇闻挑战成功 param1 anecdote_type
};

static const int MAX_SPECIAL_ITEM_NUM = 20;			//特殊副本道具背包最多存储上限20
static const int MIN_SUSPICION = 50;				//最低指认嫌疑度
static const int MAX_NPC_NUM_WXL = 9;				//NPC个数

struct TaskWenXiangLouFbParam
{
	TaskWenXiangLouFbParam() { this->Reset(); }

	void Reset()
	{
		role_enter_posi = 0;
		has_find_identity = 0;
		has_finish_fb = 0;
		lyjz_gahter_flag = 0;

		wenxianglou_has_enter = 0;
		reserver_ch = 0;
		yinyuanyixianqian_flag = 0;
		load_status = 0;

		memset(npc_sucspicion_list, 0, sizeof(npc_sucspicion_list));
		memset(npc_talk_list, 0, sizeof(npc_talk_list));
		memset(npc_identity_list, 0, sizeof(npc_identity_list));
		memset(npc_check_list, 0, sizeof(npc_check_list));

		reward_count = 0;
		memset(reward_list, 0, sizeof(reward_list));
	}

	char role_enter_posi;
	char has_find_identity;//是否指认正确
	char has_finish_fb;		//是否完成副本

	char lyjz_gahter_flag;//灵药救主采集标记

	char wenxianglou_has_enter;
	char reserver_ch;
	char yinyuanyixianqian_flag;
	//---------------------------------温香楼-------------------------------------
	char load_status;				// 进行阶段

	char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc嫌疑值
	char npc_talk_list[MAX_NPC_NUM_WXL];			// npc对话记录
	char npc_identity_list[MAX_NPC_NUM_WXL];		// npc指认记录
	char npc_check_list[MAX_NPC_NUM_WXL];			// npc查证记录（常驻）

	//临时道具背包                                                                                                                                                                       
	int reward_count;
	int reward_list[MAX_SPECIAL_ITEM_NUM];//虚拟副本道具包 -- 不允许超过20个
	 //---------------------------------温香楼-------------------------------------
};
typedef char TaskWenXiangLouFbParamHex[sizeof(TaskWenXiangLouFbParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TaskWenXiangLouFbParamHex) < 450);

#pragma pack(pop)
#endif