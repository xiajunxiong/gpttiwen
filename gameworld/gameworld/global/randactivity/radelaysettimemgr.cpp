#include "radelaysettimemgr.hpp"
#include "randactivitymanager.hpp"
#include <set>
#include "gamelog.h"
#include "global/commonsave/commonsavemanager.hpp"

RADelaySetTimeMgr::RADelaySetTimeMgr(RandActivityManager* ra_activity_mgr) : m_ra_activity_mgr(ra_activity_mgr)
{
	CommonSaveManager::Instance().RegisterLoadSuccCallback(this);
}

RADelaySetTimeMgr::~RADelaySetTimeMgr()
{

}

void RADelaySetTimeMgr::CheckDelaySetTime(unsigned int now_second)
{
	this->CheckRADelaySettimeMap(now_second);
	this->CheckRADelayForceSettimeMap(now_second);
	this->CheckRAOpenCfgMap(now_second);
}

bool RADelaySetTimeMgr::AddDelaySetTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type, unsigned int delay_to_time, const char* caller, bool is_init)
{
	long long key = ConvertParamToLongLong(rand_activity_type, begin_time); // 用活动+开始时间做key，一般不会冲突，也不允许冲突
	RASetTimeItemIt it = m_ra_delay_settime_map.find(key);
	if (it == m_ra_delay_settime_map.end())
	{
		RASetTimeItem &item = m_ra_delay_settime_map[key];
		item.rand_activty_type = rand_activity_type;
		item.begin_time = begin_time;
		item.end_time = end_time;
		item.open_type = open_type;
		item.delay_to_time = delay_to_time;
		item.is_force = false;

		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddDelaySetTime activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]", 
			rand_activity_type, begin_time, end_time, open_type, delay_to_time, caller ? caller : "NULL");
	
		if (!is_init)
		{
			this->AddToCommonSave(item);
		}
		
		return true;
	}
	else
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "RADelaySetTimeMgr::AddDelaySetTime_FAILED_1 activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]",
			rand_activity_type, begin_time, end_time, open_type, delay_to_time, caller ? caller : "NULL");

		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddDelaySetTime_FAILED_2 already have activity delay at same begin time, activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]",
			it->second.rand_activty_type, it->second.begin_time, it->second.end_time, 
			it->second.open_type, it->second.delay_to_time, caller ? caller : "NULL");
	
		return false;
	}
}

bool RADelaySetTimeMgr::AddDelayForceSetTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type, unsigned int delay_to_time, const char* caller, bool is_init)
{
	long long key = ConvertParamToLongLong(rand_activity_type, begin_time); // 用活动+开始时间做key，一般不会冲突，也不允许冲突
	RASetTimeItemIt it = m_ra_delay_force_settime_map.find(key);
	if (it == m_ra_delay_force_settime_map.end())
	{
		RASetTimeItem &item = m_ra_delay_force_settime_map[key];
		item.rand_activty_type = rand_activity_type;
		item.begin_time = begin_time;
		item.end_time = end_time;
		item.open_type = open_type;
		item.delay_to_time = delay_to_time;
		item.is_force = true;

		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddDelayForceSetTime activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]",
			rand_activity_type, begin_time, end_time, open_type, delay_to_time, caller ? caller : "NULL");

		if (!is_init)
		{
			this->AddToCommonSave(item);
		}
		
		return true;
	}
	else
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "RADelaySetTimeMgr::AddDelayForceSetTime_FAILED_1 activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]",
			rand_activity_type, begin_time, end_time, open_type, delay_to_time, caller ? caller : "NULL");

		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddDelayForceSetTime_FAILED_2 already have activity delay at same begin time, activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] delay_to_time[%u] caller[%s]",
			it->second.rand_activty_type, it->second.begin_time, it->second.end_time,
			it->second.open_type, it->second.delay_to_time, caller ? caller : "NULL");

		return false;
	}
}

bool RADelaySetTimeMgr::AddOpenCfg(const RandActivityOpenDetail& cfg, const char* call_func)
{
	long long key = ConvertParamToLongLong(cfg.activity_type, cfg.begin_timestamp); // 用活动+开始时间做key，一般不会冲突，也不允许冲突
	RAOpenCfgMap::iterator it = m_ra_open_cfg_map.find(key);
	if (it == m_ra_open_cfg_map.end())
	{
		RandActivityOpenDetail temp_cfg = cfg;
		temp_cfg.begin_timestamp += 3; // 加3秒以免活动未结束导致新活动开不起来
		m_ra_open_cfg_map[key] = temp_cfg;
		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddOpenCfg activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] seven_day_open[%s] FROM[%s]",
			temp_cfg.activity_type, temp_cfg.begin_timestamp, temp_cfg.end_timestamp, temp_cfg.open_type, temp_cfg.sevenday_open ? "True" : "False", call_func);

		return true;
	}
	else
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "RADelaySetTimeMgr::AddOpenCfg Duplicate activity_type[%d] begin_time[%u] end_time[%u] open_type[%d] seven_day_open[%s] FROM[%s]",
			cfg.activity_type, cfg.begin_timestamp, cfg.end_timestamp, cfg.open_type, cfg.sevenday_open ? "True" : "False", call_func);

		return false;
	}
}

void RADelaySetTimeMgr::Callback()
{
	this->OnCommonSaveLoadSucc();
}

void RADelaySetTimeMgr::OnCommonSaveLoadSucc()
{
	CommonSave* common_save = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA);
	assert(NULL != common_save);
	
	if (NULL == common_save)
	{
		gamelog::g_log_serious_error.printf(LL_ERROR, "RADelaySetTimeMgr::OnCommonSaveLoadSucc GetCommonSave Failed!");
		return;
	}

	RetCommonSaveData get_data;
	common_save->BeginDataForeach();
	while (common_save->GetNextData(get_data))
	{
		CommonSaveRADelaySetTimeData& data = get_data.data.ra_delay_set_time_data;
		if (0 == data.is_force)
		{
			this->AddDelaySetTime(data.rand_activty_type, data.begin_time, data.end_time, data.open_type, data.delay_to_time, __FUNCTION__, true);
		}
		else
		{
			this->AddDelayForceSetTime(data.rand_activty_type, data.begin_time, data.end_time, data.open_type, data.delay_to_time, __FUNCTION__, true);
		}
	}
}

void RADelaySetTimeMgr::RemoveFromCommonSave(const RASetTimeItem& item)
{
	CommonSave* common_save = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA);
	if (NULL == common_save) return;

	common_save->RemoveData(this->GetCommonSaveKey(item));
}

void RADelaySetTimeMgr::AddToCommonSave(const RASetTimeItem& item)
{
	CommonSave* common_save = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA);
	if (NULL == common_save) return;

	CommonSaveKey key = this->GetCommonSaveKey(item);
	CommonSaveData data;
	this->AssembleCommonSaveData(item, data);
	common_save->SetData(key, data);
}

CommonSaveKey RADelaySetTimeMgr::GetCommonSaveKey(const RASetTimeItem& item)
{
	return CommonSaveKey(item.rand_activty_type, static_cast<long long>(item.begin_time));
}

void RADelaySetTimeMgr::AssembleCommonSaveData(const RASetTimeItem& item, ARG_OUT CommonSaveData& data)
{
	data.ra_delay_set_time_data.Reset();
	data.ra_delay_set_time_data.rand_activty_type = item.rand_activty_type;
	data.ra_delay_set_time_data.begin_time = item.begin_time;
	data.ra_delay_set_time_data.end_time = item.end_time;
	data.ra_delay_set_time_data.open_type = item.open_type;
	data.ra_delay_set_time_data.delay_to_time = item.delay_to_time;
	data.ra_delay_set_time_data.is_force = item.is_force;
}

void RADelaySetTimeMgr::CheckRADelaySettimeMap(unsigned int now_second)
{
	RASetTimeItemMap aleady_set_time_keys; // 执行过的任务，用于删除
	for (RASetTimeItemIt item_it = m_ra_delay_settime_map.begin(); item_it != m_ra_delay_settime_map.end(); ++item_it)
	{
		if (now_second >= item_it->second.delay_to_time)
		{
			aleady_set_time_keys.insert(RASetTimeItemMap::value_type(item_it->first, item_it->second));
			if (now_second >= item_it->second.end_time)
			{
				// 时间已过，没有必要再设置活动了，写个日志吧
				gamelog::g_log_rand_activity.printf(LL_WARNING, "CheckRADelaySettimeMap nowtime >= endtime, activity[%d] begin_time[%u] end_time[%u]",
					item_it->second.rand_activty_type, item_it->second.begin_time, item_it->second.end_time);
				continue;
			}
			m_ra_activity_mgr->OnSetActivityTime(item_it->second.rand_activty_type, item_it->second.begin_time, item_it->second.end_time, 0, item_it->second.open_type);
		}
	}
	for (RASetTimeItemIt key_it = aleady_set_time_keys.begin(); key_it != aleady_set_time_keys.end(); ++key_it)
	{
		this->RemoveFromCommonSave(key_it->second);
		m_ra_delay_settime_map.erase(key_it->first);
	}
}

void RADelaySetTimeMgr::CheckRADelayForceSettimeMap(unsigned int now_second)
{
	RASetTimeItemMap aleady_set_time_keys; // 执行过的任务，用于删除
	for (RASetTimeItemIt item_it = m_ra_delay_force_settime_map.begin(); item_it != m_ra_delay_force_settime_map.end(); ++item_it)
	{
		if (now_second >= item_it->second.delay_to_time)
		{
			aleady_set_time_keys.insert(RASetTimeItemMap::value_type(item_it->first, item_it->second));
			if (now_second >= item_it->second.end_time)
			{
				// 时间已过，没有必要再设置活动了，写个日志吧
				gamelog::g_log_rand_activity.printf(LL_WARNING, "RADelaySetTimeMgr::CheckRADelayForceSettimeMap nowtime >= endtime, activity[%d] begin_time[%u] end_time[%u]",
					item_it->second.rand_activty_type, item_it->second.begin_time, item_it->second.end_time);
				continue;
			}
			m_ra_activity_mgr->OnForceSetActivityTime(item_it->second.rand_activty_type, item_it->second.begin_time, item_it->second.end_time, item_it->second.open_type);
		}
	}
	for (RASetTimeItemIt key_it = aleady_set_time_keys.begin(); key_it != aleady_set_time_keys.end(); ++key_it)
	{
		this->RemoveFromCommonSave(key_it->second);
		m_ra_delay_force_settime_map.erase(key_it->first);
	}
}

void RADelaySetTimeMgr::CheckRAOpenCfgMap(unsigned int now_second)
{
	std::set<long long> open_cfg_erase_key;
	for (RAOpenCfgMap::iterator it = m_ra_open_cfg_map.begin(); it != m_ra_open_cfg_map.end(); ++it)
	{
		if (now_second >= it->second.begin_timestamp)
		{
			open_cfg_erase_key.insert(it->first);
			if (now_second >= it->second.end_timestamp)
			{
				// 时间已过，没有必要再设置活动了，写个日志吧
				gamelog::g_log_rand_activity.printf(LL_WARNING, "RADelaySetTimeMgr::CheckRAOpenCfgMap nowtime >= endtime, activity[%d] begin_time[%u] end_time[%u]",
					it->second.activity_type, it->second.begin_timestamp, it->second.end_timestamp);
				continue;
			}

			RandActivity* activity = m_ra_activity_mgr->ForceGetRandActivity(it->second.activity_type);
			if (NULL == activity)
			{
				gamelog::g_log_rand_activity.printf(LL_WARNING, "RADelaySetTimeMgr::CheckRAOpenCfgMap unknown activity, activity[%d] begin_time[%u] end_time[%u]",
					it->second.activity_type, it->second.begin_timestamp, it->second.end_timestamp);
				continue;
			}

			std::string error_msg;
			if (!activity->CalActivityOpenTime(&it->second, error_msg))
			{
				gamelog::g_log_rand_activity.printf(LL_WARNING, "RADelaySetTimeMgr::CheckRAOpenCfgMap FAILED with error[%s], ra_server_id[%d], activity[%d] begin_time[%u] end_time[%u]",
					error_msg.c_str(), m_ra_activity_mgr->GetUniqueServerID().server_id, it->second.activity_type, it->second.begin_timestamp, it->second.end_timestamp);
				continue;
			}
		}
	}
	for (std::set<long long>::iterator key_it = open_cfg_erase_key.begin(); key_it != open_cfg_erase_key.end(); ++key_it)
	{
		m_ra_open_cfg_map.erase(*key_it);
	}
}

