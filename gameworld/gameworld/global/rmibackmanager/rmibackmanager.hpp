#ifndef __RMI_BACK_MANAGER_HPP__
#define __RMI_BACK_MANAGER_HPP__

#include "servercommon/servercommon.h"

/*
	RMIBack����
	key		
	callinfo: rmi������Ϣ

	RMIClient::  Call(.....)
	Call֮�����
	ON_RMI_CALL( xxxx  );

	RMIBackObject::__response()
	����� ON_RMI_BACK(...)
*/

//rmiback �Ƿ���Ч;
//��Ч��������������
//ON_RMI_BACK(return false;);
#define ON_RMI_BACK(...)\
{\
	if (!RMIBackManager::Instance().IsValidRMIBackObj(this->__get_id()))\
	{\
		printf("RMIBackManager::ON_RMI_BACK RMI Init Invalid [type: %s]\n", __FUNCTION__);\
		gamelog::g_log_rmi_back_manager.printf(LL_WARNING, "RMIBackManager::ON_RMI_BACK RMI Init Invalid [type: %s].", __FUNCTION__);\
		__VA_ARGS__;\
	}\
	else\
	{\
		RMIBackManager::Instance().OnRMICallBackSucc(this->__get_id()); \
	}\
}
//���������
//ON_RMI_CALL(GLOBAL, INIT_WORLDSTATUS, s, backobj);
#define ON_RMI_CALL(MODULS_NAME, METHOD_NAME, VTLS_VAR, RMI_BACK_OBJ, ...)\
{\
	if(NULL != RMI_BACK_OBJ)\
	{\
		RMIBackManager::Instance().AddCallInfo(RMI_CALL_MODULE_TYPE_DB_##MODULS_NAME,	\
						RMI_CALL_METHOD_TYPE_##METHOD_NAME,								\
						VTLS_VAR, RMI_BACK_OBJ->__get_id(), ## __VA_ARGS__);			\
	}\
}

//ģ��
enum RMICallModuleType
{
	RMI_CALL_MODULE_TYPE_INVALID = 0,
	RMI_CALL_MODULE_TYPE_DB_GLOBAL,

	RMI_CALL_MODULE_TYPE_MAX,
};
//��ϵͳ
enum RMICallMethodType
{
	RMI_CALL_METHOD_TYPE_INVALID = 0,
	RMI_CALL_METHOD_TYPE_INIT_WORLDSTATUS,
	RMI_CALL_METHOD_TYPE_INIT_ACTIVITY_DATA,
	RMI_CALL_METHOD_TYPE_INIT_ACTIVITY_ROLE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_OFFLINE_REGISTER,
	RMI_CALL_METHOD_TYPE_INIT_BABY_DATA,
	RMI_CALL_METHOD_TYPE_INIT_GUILD,
	RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_DATA,
	RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_USER_RECORD,
	RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_SYSTEM_RECORD,
	RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE,
	RMI_CALL_METHOD_TYPE_INIT_SYSTEM_AUTO_UPSHELVE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_CHALLENGE_USER,
	RMI_CALL_METHOD_TYPE_INIT_PLATFORM_BATTLE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_SERVER_FIRST_KILL_DATA,
	RMI_CALL_METHOD_TYPE_INIT_CHAT_GROUP_DATA,
	RMI_CALL_METHOD_TYPE_INIT_BATTLE_VIDEO_DATA,
	RMI_CALL_METHOD_TYPE_INIT_ANIMAL_RACE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_SHI_TU_SEEKING_DATA,
	RMI_CALL_METHOD_TYPE_INIT_QUAN_MIN_DISCOUNT_DATA,
	RMI_CALL_METHOD_TYPE_INIT_COURAGE_CHALLENGE_RANK_DATA,
	RMI_CALL_METHOD_TYPE_INIT_COMMON_DATA,
	RMI_CALL_METHOD_TYPE_INIT_GATHER_DREAM_RECORD_DATA,
	RMI_CALL_METHOD_TYPE_INIT_CLOUD_ARENA_DATA,
	RMI_CALL_METHOD_TYPE_INIT_CLOUD_ARENA_USER_DATA,
	RMI_CALL_METHOD_TYPE_INIT_RA_ROLE_PAI_MAI_HANG_DATA,
	RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_BASE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_USER_DATA,
	RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_SCHEDULE_DATA,
	RMI_CALL_METHOD_TYPE_INIT_MARKET_ITEM_DATA,
	RMI_CALL_METHOD_TYPE_INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA,
	RMI_CALL_METHOD_TYPE_INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA,
	RMI_CALL_METHOD_TYPE_INIT_OFFLINE_USER_BATTLE_DATA,
	
	RMI_CALL_METHOD_TYPE_MAX,
};

// switch case����
#define CASE_METHOD_TYPE(RMI_METHOD, RMI_BACK_OBJ_IMPL, ...)\
case RMI_CALL_METHOD_TYPE_##RMI_METHOD:\
	{\
		method = dataaccessrmi::global::RMI_METHOD;\
		backobj = new RMI_BACK_OBJ_IMPL();\
		__VA_ARGS__;\
	}\
	break;\


//Ĭ��ʧЧʱ��(s)
static unsigned int RMI_CALL_INFO_DEF_OUT_TIMT = 10;

struct RMICallInfo
{
	RMICallInfo()
	{
		message_id = 0;

		time_out_ln = 0;
		try_times = 0;

		module = 0;
		method = 0;

	};

	unsigned long message_id;	//����id

	unsigned int time_out_ln;		//ʧЧʱ��(s)
	int try_times;					//ϵͳ���Դ���

	//ָ��static
	int module;
	int method;
	
	std::vector<char> tlv_buff;	
};

//map<, call��Ϣ>
typedef std::map<unsigned long, RMICallInfo> RMICallInfoMap;

typedef std::multimap<unsigned int, unsigned long> RMICallInfoTimeMap;


class RMIBackManager
{
public:
	static RMIBackManager & Instance();

	void Update(unsigned long interval, time_t now_second);

	//������� 
	void AddCallInfo(int module, int method,
		const TLVSerializer &in_stream, unsigned long rmi_back_id, 
		unsigned int timeout = RMI_CALL_INFO_DEF_OUT_TIMT, int try_times = 0);

	//RMICallType ��Ч
	static bool IsValidModuleType(int module);
	static bool IsValidMethodType(int method);

	//RMIBackObj ��Ч
	bool IsValidRMIBackObj(unsigned long message_id) const;

	//rmiback ��Ӧ�ɹ�  ������ʧЧ
	void OnRMICallBackSucc(unsigned long message_id);
private:
	RMIBackManager();
	~RMIBackManager();

	//��ⳬʱ(����������ʧЧ)
	void CheckOutTime(unsigned int now_second, bool clear_all = false);

	//���³��� ֮ǰ�ķ���ʧЧ �����µ�  ������ʧЧ
	void TryCallAgain(unsigned long rmi_back_id);

	//��������
	void GlobalCall(const RMICallInfo &rmi_call_info);

	

	unsigned int m_next_check_timeout;

	//��ʱ���mmap
	RMICallInfoTimeMap m_call_time_out_map;

	//��Ϣ����
	RMICallInfoMap m_call_info_map;
};


#endif