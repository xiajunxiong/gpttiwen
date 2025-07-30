#include "rmibackmanager.hpp"
#include "engineadapter.h"
#include "servercommon/darmi/daglobal.hpp"
#include "global/rmibackobjdef.h"
#include "gamelog.h"

#include "internalcomm.h"
#include "serverlogic.h"

RMIBackManager & RMIBackManager::Instance()
{
	static RMIBackManager rmibackm;
	return rmibackm;
}

void RMIBackManager::Update(unsigned long interval, time_t now_second)
{
	if (now_second > m_next_check_timeout)
	{
		this->CheckOutTime((unsigned int)now_second);
		m_next_check_timeout = (unsigned int)now_second + 1;
	}
}

void RMIBackManager::AddCallInfo(int module, int method,
								const TLVSerializer & in_stream, unsigned long rmi_back_id, 
								unsigned int timeout, int try_times)
{
	if (!this->IsValidModuleType(module))
	{
		return;
	}
	if (!this->IsValidMethodType(method))
	{
		return;
	}

	int in_stream_size = in_stream.Size();
	if (0 > in_stream_size)
	{
		return;
	}

	if (this->IsValidRMIBackObj(rmi_back_id))
	{
		return;
	}

	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int info_out_time = now_time + timeout;
	if (now_time >= info_out_time)
	{//
		info_out_time += now_time + RMI_CALL_INFO_DEF_OUT_TIMT;
	}

	//保证在至少在updata的下一帧检测
	m_call_time_out_map.
		insert( RMICallInfoTimeMap::value_type(info_out_time, rmi_back_id) );

	RMICallInfo& call_info = m_call_info_map[rmi_back_id];
	call_info.message_id = rmi_back_id;
	
	call_info.time_out_ln = timeout;
	call_info.try_times = try_times;

	call_info.module = module;
	call_info.method = method;

	call_info.tlv_buff.clear();
	call_info.tlv_buff.resize(in_stream_size);

	if (call_info.tlv_buff.size() > 0)
	{
		memcpy(&call_info.tlv_buff[0], in_stream.Ptr(), call_info.tlv_buff.size());
	}
}

bool RMIBackManager::IsValidModuleType(int rmi_call_type)
{
	UNSTD_STATIC_CHECK(RMI_CALL_MODULE_TYPE_MAX == 2);
	if (rmi_call_type > RMI_CALL_MODULE_TYPE_INVALID
		&& rmi_call_type < RMI_CALL_MODULE_TYPE_MAX)
	{
		return true;
	}
	return false;
}

bool RMIBackManager::IsValidMethodType(int rmi_call_type)
{
	UNSTD_STATIC_CHECK(RMI_CALL_METHOD_TYPE_MAX == 33);
	if (rmi_call_type > RMI_CALL_METHOD_TYPE_INVALID
		&& rmi_call_type < RMI_CALL_METHOD_TYPE_MAX)
	{
		return true;
	}
	return false;
}

bool RMIBackManager::IsValidRMIBackObj(unsigned long message_id) const
{
	RMICallInfoMap::const_iterator c_it = m_call_info_map.find(message_id);
	if (c_it == m_call_info_map.end())
	{
		return false;
	}

	return true;
}

void RMIBackManager::OnRMICallBackSucc(unsigned long message_id)
{
	RMICallInfoMap::iterator it = m_call_info_map.find(message_id);
	if (it == m_call_info_map.end())
	{
		return;
	}
	
	m_call_info_map.erase(it);
}

RMIBackManager::RMIBackManager()
{
	m_next_check_timeout = 0;
}

RMIBackManager::~RMIBackManager()
{
}

void RMIBackManager::CheckOutTime(unsigned int now_second, bool clear_all)
{
	int for_index = 0;
	const int for_max = (int)m_call_time_out_map.size();

	//取等号是为了检测
	while (!m_call_time_out_map.empty() && for_index <= for_max)
	{
		for_index++;
		unsigned int rmi_back_id = 0;
		{
			RMICallInfoTimeMap::iterator timeout_it = m_call_time_out_map.begin();
			if (timeout_it->first > now_second && !clear_all)
			{
				break;
			}
			rmi_back_id = timeout_it->second;

			//删除超时检测 迭代器失效
			m_call_time_out_map.erase(timeout_it);
		}

		{
			//注:操作会使迭代器失效
			this->TryCallAgain(rmi_back_id);
		}
	}
	//绝对有问题(在循环中插入了失效时间小于当前时间的项目)  估计会死循环
}

void RMIBackManager::TryCallAgain(unsigned long rmi_back_id)
{
	RMICallInfoMap::iterator it = m_call_info_map.find(rmi_back_id);
	if (it == m_call_info_map.end())
	{
		return;
	}

	RMICallInfo &rmi_call_info = it->second;
	

	if (!this->IsValidModuleType(rmi_call_info.module))
	{
		return;
	}
	if (!this->IsValidMethodType(rmi_call_info.method))
	{
		return;
	}

	UNSTD_STATIC_CHECK(RMI_CALL_MODULE_TYPE_MAX ==2)
	switch (rmi_call_info.module)
	{
	case RMI_CALL_MODULE_TYPE_DB_GLOBAL:
		{
			this->GlobalCall(rmi_call_info);
		}
		break;
	default:
		return;
	}

	//删除之前的
	m_call_info_map.erase(it);
}

void RMIBackManager::GlobalCall(const RMICallInfo &rmi_call_info)
{
	if (RMI_CALL_MODULE_TYPE_DB_GLOBAL != rmi_call_info.module)
	{
		return;
	}

	rmi::RMIBackObject *backobj = NULL;
	const char * method = NULL;


	UNSTD_STATIC_CHECK(RMI_CALL_METHOD_TYPE_MAX == 33);
	switch (rmi_call_info.method)
	{
	case RMI_CALL_METHOD_TYPE_INIT_WORLDSTATUS:
	case RMI_CALL_METHOD_TYPE_INIT_ACTIVITY_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_ACTIVITY_ROLE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_OFFLINE_REGISTER:
	case RMI_CALL_METHOD_TYPE_INIT_BABY_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_GUILD:
	case RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_USER_RECORD:
	case RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_SYSTEM_RECORD:
	case RMI_CALL_METHOD_TYPE_INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE:
	case RMI_CALL_METHOD_TYPE_INIT_SYSTEM_AUTO_UPSHELVE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_CHALLENGE_USER:
	case RMI_CALL_METHOD_TYPE_INIT_PLATFORM_BATTLE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_SERVER_FIRST_KILL_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_CHAT_GROUP_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_BATTLE_VIDEO_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_ANIMAL_RACE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_SHI_TU_SEEKING_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_QUAN_MIN_DISCOUNT_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_COURAGE_CHALLENGE_RANK_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_COMMON_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_GATHER_DREAM_RECORD_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_CLOUD_ARENA_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_CLOUD_ARENA_USER_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_RA_ROLE_PAI_MAI_HANG_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_BASE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_USER_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_WORLD_ARENA_SCHEDULE_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_MARKET_ITEM_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA:
	case RMI_CALL_METHOD_TYPE_INIT_OFFLINE_USER_BATTLE_DATA:
		{
			gamelog::g_log_rmi_back_manager.printf(LL_WARNING, "RMIBackManager::GlobalCall try_call[module: %d, method: %d, try_times: %d].",
				rmi_call_info.module, rmi_call_info.method, rmi_call_info.try_times);

			//默认尝试3次
			if (3 <= rmi_call_info.try_times)
			{
				printf("RMIBackManager::GlobalCall  try_call error [module: %d, method: %d, try_times: %d]\n",
					rmi_call_info.module, rmi_call_info.method, rmi_call_info.try_times);
				
				gamelog::g_log_rmi_back_manager.printf(LL_WARNING, "RMIBackManager::GlobalCall try_call error [module: %d, method: %d, try_times: %d].",
					rmi_call_info.module, rmi_call_info.method, rmi_call_info.try_times);


				ServerLogic::GetInstServerLogic()->StopServer(__FUNCTION__);
				return;
			}
		}
		break;
	}


	UNSTD_STATIC_CHECK(RMI_CALL_METHOD_TYPE_MAX == 33);
	switch(rmi_call_info.method)
	{
		CASE_METHOD_TYPE(INIT_WORLDSTATUS, RMIInitWorldStatusBackObjectImpl);
		CASE_METHOD_TYPE(INIT_ACTIVITY_DATA, RMIInitActivityDataBackImpl);
		CASE_METHOD_TYPE(INIT_ACTIVITY_ROLE_DATA, RMIInitActivityRoleDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_OFFLINE_REGISTER, RMIInitOfflineRegisterBackObjectImpl);
		CASE_METHOD_TYPE(INIT_BABY_DATA, RMIInitBabyDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_GUILD, RMIInitGuildBackObjectImpl);
		CASE_METHOD_TYPE(INIT_TRADE_MARKET_DATA, RMIInitTradeMarketDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_TRADE_MARKET_USER_RECORD, RMIInitUserTradeRecordBackObjectImpl);
		CASE_METHOD_TYPE(INIT_TRADE_MARKET_SYSTEM_RECORD, RMIInitSystemTradeRecordBackObjectImpl);
		CASE_METHOD_TYPE(INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE, RMIInitSystemRecycleRuleBackObjectImpl);
		CASE_METHOD_TYPE(INIT_SYSTEM_AUTO_UPSHELVE_DATA, RMIInitSystemAutoUpshelveBackObjectImpl);
		CASE_METHOD_TYPE(INIT_CHALLENGE_USER, RMIInitChallengeUserObjectImpl);
		CASE_METHOD_TYPE(INIT_PLATFORM_BATTLE_DATA, RMIInitPlatformBattleBackObjectImpl);
		CASE_METHOD_TYPE(INIT_SERVER_FIRST_KILL_DATA, RMIInitServerFirstKillBackObjectImpl);
		CASE_METHOD_TYPE(INIT_CHAT_GROUP_DATA, RMIInitChatGroupBackObjectImpl);
		CASE_METHOD_TYPE(INIT_BATTLE_VIDEO_DATA, RMIInitBattleVideoDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_ANIMAL_RACE_DATA, RMIInitAnimalRaceBackObjectImpl);
		CASE_METHOD_TYPE(INIT_SHI_TU_SEEKING_DATA, RMIInitShiTuSeekingDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_QUAN_MIN_DISCOUNT_DATA, RMIInitQuanMinDiscountBackObjectImpl);
		CASE_METHOD_TYPE(INIT_COURAGE_CHALLENGE_RANK_DATA, RMIInitCourageChallengeRankBackObjectImpl);
		CASE_METHOD_TYPE(INIT_COMMON_DATA, RMIInitCommonDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_GATHER_DREAM_RECORD_DATA, RMIInitGatherDreamRecordBackObjectImpl);
		CASE_METHOD_TYPE(INIT_CLOUD_ARENA_DATA, RMIInitCloudArenaDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_CLOUD_ARENA_USER_DATA, RMIInitCloudArenaUserDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_RA_ROLE_PAI_MAI_HANG_DATA, RMIInitRAPaiMaiHangBackObjectImpl);
		CASE_METHOD_TYPE(INIT_WORLD_ARENA_BASE_DATA, RMIInitWorldArenaBaseDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_WORLD_ARENA_USER_DATA, RMIInitWorldArenaUserDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_WORLD_ARENA_SCHEDULE_DATA, RMIInitWorldArenaScheduleDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_MARKET_ITEM_DATA, RMIInitMarketItemDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA, RMIInitCloudArenaUserDataBackObjectImpl);
		CASE_METHOD_TYPE(INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA, RMIInitRAPaiMaiHangBackObjectImpl);
		CASE_METHOD_TYPE(INIT_OFFLINE_USER_BATTLE_DATA, RMIInitOfflineUserBattleDataBackObjectImpl);
		//CASE_METHOD_TYPE(aaa, bbb, int i = 0;);
	default:
		printf("RMIBackManager::GlobalCall case method ERROR [module: %d, method: %d, try_times: %d]\n",
			rmi_call_info.module, rmi_call_info.method, rmi_call_info.try_times);
		return;
	}

	if (NULL == backobj
		|| NULL == method)
	{
		gamelog::g_log_rmi_back_manager.printf(LL_WARNING, "RMIBackManager::GlobalCall invalid try method: rmi_call_info.method[%d]", rmi_call_info.method);

		printf("RMIBackManager::GlobalCall invalid try method: rmi_call_info.method[%d]", rmi_call_info.method);
		return;
	}

	//这里面可能会使 *backobj 失效
	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	int ret = gc.OnRMIBackManagerTry(rmi_call_info, backobj, 
									dataaccessrmi::GLOBAL, method);
	backobj = NULL;

	printf("RMIBackManager::GlobalCall  try_call [module: %d, method: %d, try_times: %d, ret: %d]\n",
		rmi_call_info.module, rmi_call_info.method, rmi_call_info.try_times, ret);

	return;
}
