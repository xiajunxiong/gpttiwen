#include "datasynccheck.hpp"

#include "engineadapter.h"
#include "gamelog.h"

#include "global/activity/activitymanager.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/usercache/usercache.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "friend/friendintimacy.hpp"
#include "friend/friendmanager.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"

DataSyncCheck & DataSyncCheck::Instance()
{
	static DataSyncCheck st_dyc;
	return st_dyc;
}

void DataSyncCheck::Update(unsigned long _interval, time_t _now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	std::map<unsigned int, std::set<int> >::iterator it_begin = m_data_sync_check_map.begin();
	while (it_begin != m_data_sync_check_map.end())
	{
		if (_now_second < it_begin->first)
			break;

		std::set<int>::iterator sub_it_beign = it_begin->second.begin();
		while (sub_it_beign != it_begin->second.end())
		{
			int _type = *sub_it_beign;
			it_begin->second.erase(*sub_it_beign);

			if (m_info_arr[_type].count > 0)
			{
				m_info_arr[_type].count = 0;// 之前还没收到回应的就不算了
				this->ReSyncData(_type);
			}

			sub_it_beign = it_begin->second.begin();
		}

		if (0 == it_begin->second.size())
		{
			m_data_sync_check_map.erase(it_begin);
		}

		it_begin = m_data_sync_check_map.begin();
	}
}

void DataSyncCheck::AddCheck(int _type)
{
	if (_type < 0 || _type >= crossgameprotocal::DATA_SYNC_CHECK_TYPE_MAX)
		return;

	int old_count = m_info_arr[_type].count;

	// 使再同步时间错开
	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
	unsigned int next_check_time = now_time + DATA_SYNC_CHECK_INTERVAL + _type;
	if (0 == m_info_arr[_type].count)
	{
		++m_info_arr[_type].count;
		m_info_arr[_type].time = next_check_time;

		std::set<int> temp;
		temp.insert(_type);
		m_data_sync_check_map.insert(std::make_pair(next_check_time, temp));
	}
	else
	{
		++m_info_arr[_type].count;
	}

	gamelog::g_log_debug.printf(LL_INFO, "DataSyncCheck::AddCheck  type:%d  times:%u  old_count:%d  new_count:%d  is_hidden:%d",
		_type, next_check_time, old_count, m_info_arr[_type].count , CrossConfig::Instance().IsHiddenServer());
}

void DataSyncCheck::RemoveCheck(int _type)
{
	if (_type < 0 || _type >= crossgameprotocal::DATA_SYNC_CHECK_TYPE_MAX)
		return;

	int old_count = m_info_arr[_type].count;
	if (m_info_arr[_type].count <= 0)
	{
		// 回执消息太慢了?
		gamelog::g_log_debug.printf(LL_INFO, "DataSyncCheck::RemoveCheck[error]  type:%d  old_count:%d  new_count:%d ",
			_type, old_count, m_info_arr[_type].count);
		return;
	}

	--m_info_arr[_type].count;

	if (0 == m_info_arr[_type].count)
	{
		unsigned int time = m_info_arr[_type].time;

		m_data_sync_check_map[time].erase(_type);
		if (0 == m_data_sync_check_map[time].size())
		{
			m_data_sync_check_map.erase(time);
		}

		gamelog::g_log_debug.printf(LL_INFO, "DataSyncCheck::RemoveCheck[Finish]  type:%d ",
			_type);
	}

	gamelog::g_log_debug.printf(LL_INFO, "DataSyncCheck::RemoveCheck  type:%d  old_count:%d  new_count:%d",
		_type, old_count, m_info_arr[_type].count);
}

void DataSyncCheck::ReSyncData(int _type)
{
	if (_type < 0 || _type >= crossgameprotocal::DATA_SYNC_CHECK_TYPE_MAX)
		return;

	switch (_type)
	{
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_CROSS_USER_REGISTER:
		{
			// 原逻辑里被屏蔽了 所以这里暂时先不加了
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_GUILD:
		{
			// 原逻辑里有对应的处理逻辑, 所以这里也暂时不加
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_ACTIVITY:
		{
			ActivityManager::Instance().OnConnectCrossServerSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_WORLD_STATUS:
		{
			WorldStatus::Instance().OnConnectCrossServerSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_USER_CACHE:
		{
			UserCacheManager::Instance().OnConnectCrossServerSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_CHAT:
		{
			// 原逻辑里被屏蔽了 所以这里暂时先不加了
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_RANK:
		{
			RankManager::Instance().OnConnectCrossSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_ORDER:
		{
			// 原逻辑里没有数据向跨服同步 , 只是打开了一个标记
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_MARKET:
		{
			MarketManager::Instance().OnConnectCrossSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_HONG_BAO:
		{
			HongBaoManager::Instance().OnGameworldAccept();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY:
		{
			FriendIntimacyManager::Instance().OnConnectHiddenSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_FRIEND:
		{
			FriendManager::Instance().OnConnectHiddenSucc();
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_RAND_ACTIVITY:
		{
			RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
			if (NULL != ramgr)
			{
				ramgr->SendActivityDataToCross();
			}
		}
		break;
	case crossgameprotocal::DATA_SYNC_CHECK_TYPE_KUA_FU_YING_XIONG_LU:
		{
			KuaFuYingXiongLuManager::Instance().OnConnectCrossSucc();
		}
		break;
	}

	gamelog::g_log_debug.printf(LL_INFO, "DataSyncCheck::ReSyncData  type:%d ",
		_type);
}

DataSyncCheck::DataSyncCheck()
{

}

DataSyncCheck::~DataSyncCheck()
{

}
