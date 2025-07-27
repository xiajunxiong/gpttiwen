#ifndef __DU_JIE_DEF__
#define __DU_JIE_DEF__

#include "servercommon/servercommon.h"

////// 渡劫

//最大关卡数
static const int DU_JIE_MAX_FB_NUM = 9;

//前置任务类型 (创号就开始记录)
enum DuJieTaskType
{
	DU_JIE_TASK_TYPE_DREAM_NOTES_CHALLENGE = 0, //通关梦渊X层	p_1:层数
	DU_JIE_TASK_TYPE_COURAGE_CHALLENGE = 1,		//通关X级锢魔之塔	p_1:等级段
	DU_JIE_TASK_TYPE_MONSTER_GROUP = 2,			//单人战胜怪物组X	p_1:怪物组
	DU_JIE_TASK_TYPE_LUN_HUI_WANG_CHUAN = 3,	//通关轮回忘川X关	p_1:关卡seq
	DU_JIE_TASK_TYPE_SHAN_HAI_BOSS = 4,			//连续P1天战胜 P2难度山海精怪	p_1:天数  p_2:难度(写死只记录一种难度)
	DU_JIE_TASK_TYPE_PRESTIGE = 5,				//任意P1个村庄声望达到P2以上	p_1:层数  p_2:声望等级
	DU_JIE_TASK_TYPE_TOTAL_CAPABILITY = 6,		//综合战力达到X		p_1:战力
	DU_JIE_TASK_TYPE_GET_CHIVALROUS = 7,		// 累计获得侠义值 param1:数量
	DU_JIE_TASK_TYPE_COURSE_LEVEL = 8,			// 成就-历程等级 param1:等级
	DU_JIE_TASK_TYPE_COUESE_OF_GROWTH = 9,		// 成就-目标阶段 param1:阶段 param2:星级
	DU_JIE_TASK_TYPE_ROLE_CAPABILITY = 10,		// 角色战力 param1:战力数值

	DU_JIE_TASK_TYPE_MAX,
};

//任务参数
enum DuJieTaskParam
{
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_DAYS,		//P2难度山海精怪连续通关天数
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_TODAY_PASS,	//P2难度山海精怪今日通关通关标记
	DU_JIE_TASK_PARAM_SHAN_HAI_BOSS_MAX_DAYS,	//P2难度山海精怪最大连续通关天数
	DU_JIE_TASK_PARAM_LUN_HUI_WANG_CHUAN,		//轮回忘川历史最高关卡

	DU_JIE_TASK_PARAM_MAX,
};

#pragma pack(push, 4)
struct DuJieParam
{
	DuJieParam()
	{
		this->Reset();
	}
	void Reset()
	{
		hold_ll = 0;
		max_pass_fb_seq = -1;
		next_front_gate_pass = 0;
		memset(task_param_list_, 0, sizeof(task_param_list_));
		memset(hold_list_, 0, sizeof(hold_list_));
		memset(task_param_list_, 0, sizeof(task_param_list_));

	}

	long long hold_ll;
	int max_pass_fb_seq;			//最高关卡
	int next_front_gate_pass;	//前置关卡通关(最高关卡下一关的)
	
	int task_param_list_[DU_JIE_TASK_PARAM_MAX];
	int hold_list_[10];
	//改需求 改成各关卡公用  用第0个
	int day_assist_times_[DU_JIE_MAX_FB_NUM];	//今日渡劫助战次数[FB_index]
};

typedef char DuJieParamHex[sizeof(DuJieParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DuJieParamHex) < 256);


#pragma pack(pop)

#endif