#ifndef __DATA_SYNC_CHECK_HPP__
#define __DATA_SYNC_CHECK_HPP__

#include <map>
#include "gamedef.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

static const int DATA_SYNC_CHECK_INTERVAL = 15;			// 15����һ��

class DataSyncCheck
{
	struct DataCheckInfo
	{
		DataCheckInfo():count(0), time(0){}

		int count;				// ����
		unsigned int time;		// ��ʱʱ��
	};

public:
	/*
		�����������������ط�ͬ��ȫ��������֤ , ��֤��Ӧ������ȷʵ���յ�����
	*/

	static DataSyncCheck & Instance();
	void Update(unsigned long _interval, time_t  _now_second);
	// ����һ�μ�һ�δ���, ��ΪĳЩ����ͬ���ᷢ�Ͷ������
	void AddCheck(int _type);
	// ����һ�μ�һ�δ���, ��ΪĳЩ����ͬ���ᷢ�Ͷ������
	void RemoveCheck(int _type);
	// ����ͬ��ĳ�����͵�����
	void ReSyncData(int _type);

private:
	DataSyncCheck();
	~DataSyncCheck();

	std::map<unsigned int, std::set<int> > m_data_sync_check_map;						// <ʱ�� , type>
	DataCheckInfo m_info_arr[crossgameprotocal::DATA_SYNC_CHECK_TYPE_MAX];

};

#endif	// __DATA_SYNC_CHECK_HPP__