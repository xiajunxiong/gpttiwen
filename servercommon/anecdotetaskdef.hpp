#ifndef __ANECDOTE_TASK_DEF_HPP__
#define __ANECDOTE_TASK_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/bitmapfunction.hpp"

#pragma pack(push, 4)

static const int MAX_ANECDOTE_TASK_TYPE = 50;
static const int ANECDOTE_BIG_EVENT_REWARD_VERSION = 1;

enum ANECDOTE_TASK_TYPE
{
	ANECDOTE_TASK_TYPE_INVALID = 0,
	ANECDOTE_TASK_TYPE_THIEF_CAMP_RESCUE = 1,	//!< 1 独闯贼营
	ANECDOTE_TASK_TYPE_PUZZLE = 2,				//!< 2 九宫拼图
	ANECDOTE_TASK_TYPE_SUDOKU = 3,				//!< 3 洛书奇门
	ANECDOTE_TASK_TYPE_BROTHEL_KILLER_EVENT = 4,//!< 4 温香楼杀人事件
	ANECDOTE_TASK_TYPE_MARRIAGE_HELPER = 5,		//!< 5 姻缘一线牵
	ANECDOTE_TASK_TYPE_MAGIC_PILL = 6,			//!< 6 灵药救主
	ANECDOTE_TASK_TYPE_GUARDIAN_CAMP_SPY = 7,	//!< 7 铁卫营奸细
	ANECDOTE_TASK_TYPE_EYE_SIGHT_TRAINING = 8,	//!< 8 眼力训练
	ANECDOTE_TASK_TYPE_GUARDIAN_CAMP_LEADER = 9,//!< 9 铁卫营奸细后续
	ANECDOTE_TASK_TYPE_CATCH_FOX = 10,			//!< 10 抓拿狐妖
	ANECDOTE_TASK_TYPE_STAR_KING = 11,			//!< 11 司命星君
	ANECDOTE_TASK_TYPE_FINISH_KING = 12,		//!< 12 鱼王的眼泪
	ANECDOTE_TASK_TYPE_TIANXIAJUEDUI = 13,		//!< 13 天下绝对
	ANECDOTE_TASK_TYPE_DUANSHIFEI = 14,			//!< 14 断是非
	ANECDOTE_TASK_TYPE_YINYUANYIXIANQIAN = 15,	//!< 15 姻缘一线牵
	ANECDOTE_TASK_TYPE_ANTIQUE_APPRECIATION = 16,//!< 16 古董鉴赏
	ANECDOTE_TASK_TYPE_TRAVELING_POET = 17,		//!< 17 游历诗人
	ANECDOTE_TASK_TYPE_MYSETRIAL_TREASURE = 18,	//!< 18 神秘宝藏
	ANECDOTE_TASK_TYPE_TREASURE_BOX_UNLOCK = 19,//!< 19 宝箱解密
	ANECDOTE_TASK_TYPE_FIND_FOOD = 20,			//!< 20 寻味美食
	ANECDOTE_TASK_TYPE_REAL_FAKE_PAINTING = 21, //!< 21 真假画作
	ANECDOTE_TASK_TYPE_3X3_DIGITAL_KLOTSKI = 22,//!< 22 3X3数字华容道
	ANECDOTE_TASK_TYPE_POEMS_AND_SONGS = 23,	//!< 23 诗词歌赋
	ANECDOTE_TASK_TYPE_QUESTION_OF_TONGBAO = 24,//!< 24 彤宝的疑问
	ANECDOTE_TASK_TYPE_SOLVE_PUZZLE = 25,		//!< 25 机关巧解
	ANECDOTE_TASK_TYPE_MEMORY_MASTER = 26,		//!< 26 记忆大师
	ANECDOTE_TASK_TYPE_SOUL_IN_ZHEN = 27,		//!< 27 阵中之魂
	ANECDOTE_TASK_TYPE_XIANSHI_SHOUTU = 28,		//!< 28 仙师收徒
	ANECDOTE_TASK_TYPE_QIANNIANTAOHUA = 29,		//!< 29 千年桃花开
	ANECDOTE_TASK_TYPE_QUMODAOREN = 30,			//!< 30 驱魔道人
	ANECDOTE_TASK_TYPE_SHAMOGUAILONG = 31,		//!< 31 沙漠怪龙
	ANECDOTE_TASK_TYPE_SHIZHEZHIHUN = 32,		//!< 32 诗者之魂
	ANECDOTE_TASK_TYPE_DIWANGHUAXIANG = 33,		//!< 33 帝王画像
	ANECDOTE_TASK_TYPE_HAIYUSHIZHE = 34,		//!< 34 海鱼使者
	ANECDOTE_TASK_TYPE_NEW_MUSIC_GAME = 35,		//!< 35 新音游
	ANECDOTE_TASK_TYPE_NEW_LANTERN_FESTIVAL = 36, //!< 36 新灯会答题
	ANECDOTE_TASK_TYPE_37 = 37, // 37 游历诗人（沙漠）
	ANECDOTE_TASK_TYPE_38 = 38, // 38 宝箱解密（沙漠）
	ANECDOTE_TASK_TYPE_39 = 39, // 39 宝箱解密（海底）
	ANECDOTE_TASK_TYPE_40 = 40, // 40 天下绝对（二）
	ANECDOTE_TASK_TYPE_41 = 41, // 41 诗词歌赋（二）
	ANECDOTE_TASK_TYPE_42 = 42, // 42 天下绝对（三） 

	ANECDOTE_TASK_TYPE_MAX
};

enum ANECDOTE_TASK_ACCEPT_STATUS
{
	ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTABLE = 0, //!< 0 可接
	ANECDOTE_TASK_ACCEPT_STATUS_ACCEPTED = 1,	//!< 1 已接
};

struct AnecdoteTaskInfo			//!< 系列任务信息
{
	AnecdoteTaskInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		succ_times = 0;
		fail_times = 0;
		cur_task_id = 0;
		has_claim_reward = 0;
		progress = 0;
		cur_task_accept_status = 0;
		reserve_sh = 0;
		reserve_ll = 0;
	}

	short succ_times;			//!< 成功次数
	short fail_times;			//!< 失败次数
	unsigned short cur_task_id;	//!< 当前任务ID
	short has_claim_reward;		//!< 是否已领取奖励
	int progress;				//!< 任务进度
	short cur_task_accept_status; //!< 当前任务接受状态 0:可接 1:已接
	short reserve_sh;			//!< 保留字
	long long reserve_ll;		//!< 保留字
};

struct AnecdoteTaskCommonParam
{
	AnecdoteTaskCommonParam()
	{
		this->Reset();
	}

	void Reset()
	{
		tianxiajuedui = 0;
		transfer_tianxiajuedui_flag = 0;
		reserve_ch = 0;
		reserve_sh = 0;
		reserve_int = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));
		memset(special_value, 0, sizeof(special_value));
	}

	int tianxiajuedui;
	char transfer_tianxiajuedui_flag;
	char reserve_ch;
	short reserve_sh;
	int reserve_int;
	long long reserve_ll[3];
	int special_value[MAX_ANECDOTE_TASK_TYPE];
};
UNSTD_STATIC_CHECK(ANECDOTE_TASK_TYPE_MAX <= MAX_ANECDOTE_TASK_TYPE)

typedef char AnecdoteTaskCommonParamHex[sizeof(AnecdoteTaskCommonParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AnecdoteTaskCommonParamHex) < 1024);

struct AnecdoteTaskParam
{
	AnecdoteTaskParam()
	{
		this->Reset();
	}

	void Reset()
	{
		reward_version = 0;
		has_big_event_expire_send_reward_mail = 0;
		big_event_version = 0;
		big_event_end_timestamp = 0u;
		big_event_end_timestamp = 0;
		big_event_reward_fetch_flag = 0;
		reserve_int = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));

		for (int i = 0; i < MAX_ANECDOTE_TASK_TYPE; ++i)
		{
			task_info_list[i].Reset();
		}
	}

	char reward_version;
	char has_big_event_expire_send_reward_mail;
	short big_event_version;
	unsigned int big_event_start_timestamp;
	unsigned int big_event_end_timestamp;
	unsigned int big_event_reward_fetch_flag;
	int reserve_int;
	long long reserve_ll[2];
	AnecdoteTaskInfo task_info_list[MAX_ANECDOTE_TASK_TYPE];
};

typedef char AnecdoteTaskParamHex[sizeof(AnecdoteTaskParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(AnecdoteTaskParamHex) < 10240);

static const int MAX_SPECIAL_ANECDOTE_TASK_LIST = 128;//修改需要数据迁移
static const int MAX_SPECIAL_ANECDOTE_TASK_TYPE = 100;//修改需要数据迁移

enum SPECIAL_ANECDOTE_TASK
{
	SPECIAL_ANECDOTE_TASK_CHALLENGE = 0,//挑战类任务

	SPECIAL_ANECDOTE_TASK_MAX,
};

struct SpecialTaskInfo
{
	SpecialTaskInfo() { this->Reset(); }
	void Reset() 
	{
		task_bit_map.Reset();
	}
	bool SetBit(int idx)
	{
		if (task_bit_map.IsBitSet(idx))return false;

		task_bit_map.SetBit(idx);
		return true;
	}

	bool IsBitSet(int idx)
	{
		if (idx < 0 || idx >= MAX_SPECIAL_ANECDOTE_TASK_LIST)return false;
		return task_bit_map.IsBitSet(idx);
	}

	BitMap<MAX_SPECIAL_ANECDOTE_TASK_LIST> task_bit_map;
};

struct SpecialAnecdoteParam
{
	SpecialAnecdoteParam()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int i = 0; i < MAX_SPECIAL_ANECDOTE_TASK_TYPE; i++)
		{
			task_list[i].Reset();
		}
	}

	SpecialTaskInfo task_list[MAX_SPECIAL_ANECDOTE_TASK_TYPE];
};

typedef char SpecialAnecdoteParamHex[sizeof(SpecialAnecdoteParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SpecialAnecdoteParamHex) < 4096);
#pragma pack(pop)

#endif