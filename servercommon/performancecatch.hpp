#ifndef __PERFORMANCE_CATCH_HPP__
#define __PERFORMANCE_CATCH_HPP__

#include "servercommon/platfromhelper.hpp"

#include <map>
#include <stack>

enum PERFORMANCE_CATCH_TYPE
{
	PERFORMANCE_CATCH_TYPE_INVALID = 0,
	PERFORMANCE_CATCH_TYPE_RELOAD_CONFIG,						// 重读配置
	PERFORMANCE_TEST_SINGLE_CHAT,								// 私聊
	PERFORMANCE_TEST_CHANEL_CHAT,								// 频道聊天
	PERFORMANCE_TEST_MESSAGE_HANDLE,							// 协议处理
	PERFORMANCE_TEST_RANK_SNAPSHOT,								// 排行榜
	PERFORMANCE_TEST_BATTLE_CALCULATE_ROUND,					// 战斗回合计算
	PERFORMANCE_TEST_PROTOBUF,									// Protobuf的测试
	PERFORMANCE_TEST_LOGIC_CONFIG_RELOAD,						// 逻辑配置重读					
	PERFORMANCE_TEST_ROBOT_CONFIG_LOAD,							// 读取机器人配置
	PERFORMANCE_TEST_PARTNER_CONFIG_LOAD,						// 读取伙伴配置
	PERFORMANCE_TEST_PERSONRANK_INIT_DATA,						// 人物排行榜初始化构建数据表
	PERFORMANCE_TEST_PERSONRANK_GETINIT_DATA,					// 人物排行榜写回数据库
	PERFORMANCE_TEST_PERSONRANK_UPDATA_RANK,					// 人物排行榜刷新数据
	PERFORMANCE_TEST_PETRANK_INIT_DATA,							// 宠物排行榜初始化构建数据表
	PERFORMANCE_TEST_PETRANK_GETINIT_DATA,						// 宠物排行榜写回数据库
	PERFORMANCE_TEST_PETRANK_UPDATA_RANK,						// 宠物排行榜刷新数据
	PERFORMANCE_TEST_GUILDRANK_UPDATA_RANK,						// 公会排行榜刷新数据
	PERFORMANCE_TEST_PET_TIME_RECALC,							// 宠物来源属性计算时间
	PERFORMANCE_TEST_PARTNER_RECALC_REAWSON_ATTR_LIST_TIME,		// 伙伴来源属性计算时间
	PERFORMANCE_TEST_SKILL_CONFIG_RELOAD,						// 技能配置重读
	PERFORMANCE_TEST_MONSTER_CONFIG_RELOAD,						// 怪物配置重读
	PERFORMANCE_TEST_EFFECT_CONFIG_RELOAD,						// 战斗特效配置重读
	PERFORMANCE_TEST_BUFF_CONFIG_RELOAD,						// 战斗BUFF配置
	PERFORMANCE_TEST_BATTLE_SUMMON_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_SCRIPT_CONFIG_RELOAD,
	PERFORMANCE_TEST_MONSTER_GROUP_CONFIG_RELOAD,				// 怪物组配置重读
	PERFORMANCE_TEST_BATTLE_DEMO_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_FABAO_CONFIG_RELOAD,				// 战斗法宝配置重读
	PERFORMANCE_TEST_BATTLE_PET_GIFT_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_PARTNER_CONFIG_RELOAD,
	PERFORMANCE_TEST_BATTLE_SUNDRY_CONFIG_RELOAD,	
	PERFORMANCE_TEST_BATTLE_STORY_CONFIG_RELOAD,	
	PERFORMANCE_TEST_BATTLE_NPC_HELPER_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_CONDITION_CONFIG_RELOAD,
	PERFORMANCE_TEST_BATTLE_MONSTER_CONFIG_LOAD,
	PERFORMANCE_TEST_HOTFIX,
	PERFORMANCE_TEST_BATTLE_ONE_MONSTER_RELOAD,
	PERFORMANCE_TEST_BATTLE_SPECIAL_MONSTER_RELOAD,
	PERFORMANCE_TEST_REDIS,
	PERFORMANCE_TEST_BATTLE_HALLOW_CONFIG_RELOAD,
	PERFORMANCE_TEST_MALLOC_TRIM,
	PERFORMANCE_CATCH_TYPE_MAX,
};

class PerformanceCatch
{
public:
	static PerformanceCatch & Instance();

	void CalcPerformanceCatchBegin(int catch_type);
	void CalcPerformanceCatchEnd(int catch_type);

	void Update(unsigned long interval);

	void SimplePerformanceCatchBegin(int catch_type);
	void SimplePerformanceCatchEnd(int catch_type);

	void GetElapseUsBegin(int catch_type);
	long long GetElapseUsEnd(int catch_type);

private:
	PerformanceCatch();
	~PerformanceCatch();

	struct StackItem
	{
		StackItem() : catch_type(PERFORMANCE_CATCH_TYPE_INVALID) {}

		int catch_type;

#ifdef _WIN32
		LARGE_INTEGER nFreq;
		LARGE_INTEGER start_time;
		LARGE_INTEGER end_time;
#else 
		timeval tv_start;
		timeval tv_end;
#endif 
	};

	bool m_working;
	time_t m_output_interval;

	std::stack<StackItem> m_catch_stack;

	struct PerformanceItem
	{
		PerformanceItem() : call_count(0), total_us(0) {}
		PerformanceItem(long long _call_count, long long _total_us) : call_count(_call_count), total_us(_total_us) {}

		long long call_count;
		long long total_us;
	};

	typedef std::map<int, PerformanceItem> PerformanceItemMap;
	typedef std::map<int, PerformanceItem>::iterator PerformanceItemMapIt;
	PerformanceItemMap m_performance_map;

	StackItem m_simple_catch;
};

#endif // __PERFORMANCE_CATCH_HPP__

