#ifndef __DATA_SYNC_CHECK_HPP__
#define __DATA_SYNC_CHECK_HPP__

#include <map>
#include "gamedef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

static const int DATA_SYNC_CHECK_INTERVAL = 15;			// 15秒检查一次

class DataSyncCheck
{
	struct DataCheckInfo
	{
		DataCheckInfo():count(0), time(0){}

		int count;				// 次数
		unsigned int time;		// 超时时间
	};

public:
	/*
		用于主服向跨服或隐藏服同步全局数据验证 , 保证对应服务器确实有收到数据
	*/

	static DataSyncCheck & Instance();
	void Update(unsigned long _interval, time_t  _now_second);
	// 调用一次加一次次数, 因为某些数据同步会发送多次数据
	void AddCheck(int _type);
	// 调用一次减一次次数, 因为某些数据同步会发送多次数据
	void RemoveCheck(int _type);
	// 重新同步某个类型的数据
	void ReSyncData(int _type);

private:
	DataSyncCheck();
	~DataSyncCheck();

	std::map<unsigned int, std::set<int> > m_data_sync_check_map;						// <时间 , type>
	DataCheckInfo m_info_arr[crossgameprotocal::DATA_SYNC_CHECK_TYPE_MAX];

};

#endif	// __DATA_SYNC_CHECK_HPP__