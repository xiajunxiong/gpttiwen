#ifndef __PERFORMANCE_CATCH_HPP__
#define __PERFORMANCE_CATCH_HPP__

#include "servercommon/platfromhelper.hpp"

#include <map>
#include <stack>

enum PERFORMANCE_CATCH_TYPE
{
	PERFORMANCE_CATCH_TYPE_INVALID = 0,
	PERFORMANCE_CATCH_TYPE_RELOAD_CONFIG,						// �ض�����
	PERFORMANCE_TEST_SINGLE_CHAT,								// ˽��
	PERFORMANCE_TEST_CHANEL_CHAT,								// Ƶ������
	PERFORMANCE_TEST_MESSAGE_HANDLE,							// Э�鴦��
	PERFORMANCE_TEST_RANK_SNAPSHOT,								// ���а�
	PERFORMANCE_TEST_BATTLE_CALCULATE_ROUND,					// ս���غϼ���
	PERFORMANCE_TEST_PROTOBUF,									// Protobuf�Ĳ���
	PERFORMANCE_TEST_LOGIC_CONFIG_RELOAD,						// �߼������ض�					
	PERFORMANCE_TEST_ROBOT_CONFIG_LOAD,							// ��ȡ����������
	PERFORMANCE_TEST_PARTNER_CONFIG_LOAD,						// ��ȡ�������
	PERFORMANCE_TEST_PERSONRANK_INIT_DATA,						// �������а��ʼ���������ݱ�
	PERFORMANCE_TEST_PERSONRANK_GETINIT_DATA,					// �������а�д�����ݿ�
	PERFORMANCE_TEST_PERSONRANK_UPDATA_RANK,					// �������а�ˢ������
	PERFORMANCE_TEST_PETRANK_INIT_DATA,							// �������а��ʼ���������ݱ�
	PERFORMANCE_TEST_PETRANK_GETINIT_DATA,						// �������а�д�����ݿ�
	PERFORMANCE_TEST_PETRANK_UPDATA_RANK,						// �������а�ˢ������
	PERFORMANCE_TEST_GUILDRANK_UPDATA_RANK,						// �������а�ˢ������
	PERFORMANCE_TEST_PET_TIME_RECALC,							// ������Դ���Լ���ʱ��
	PERFORMANCE_TEST_PARTNER_RECALC_REAWSON_ATTR_LIST_TIME,		// �����Դ���Լ���ʱ��
	PERFORMANCE_TEST_SKILL_CONFIG_RELOAD,						// ���������ض�
	PERFORMANCE_TEST_MONSTER_CONFIG_RELOAD,						// ���������ض�
	PERFORMANCE_TEST_EFFECT_CONFIG_RELOAD,						// ս����Ч�����ض�
	PERFORMANCE_TEST_BUFF_CONFIG_RELOAD,						// ս��BUFF����
	PERFORMANCE_TEST_BATTLE_SUMMON_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_SCRIPT_CONFIG_RELOAD,
	PERFORMANCE_TEST_MONSTER_GROUP_CONFIG_RELOAD,				// �����������ض�
	PERFORMANCE_TEST_BATTLE_DEMO_CONFIG_RELOAD,		
	PERFORMANCE_TEST_BATTLE_FABAO_CONFIG_RELOAD,				// ս�����������ض�
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

