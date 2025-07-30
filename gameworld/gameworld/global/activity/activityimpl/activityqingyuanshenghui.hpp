#ifndef __ACTIVITY_QING_YUAN_SHENG_HUI_HPP__
#define __ACTIVITY_QING_YUAN_SHENG_HUI_HPP__

#include "global/activity/activity.hpp"
#include "servercommon/commonsavedef.h"
#include <vector>


struct QingYuanShengHuiXuYuan
{
	virtual void XuYuan()
	{	
		//TODO gfp 检测格式
		int tmp = 0;

		for (int i=0;i< 1000;++i)
		{

		}
		
		
	}

	char text[1512];
};

class Role;
class CommonSave;
class ActivityQingYuanShengHui: public Activity
{
public:
	ActivityQingYuanShengHui(ActivityManager *activity_manager);
	~ActivityQingYuanShengHui();

	virtual void OnActivityStatusChange(int from_status, int to_status);
	virtual void Update(unsigned long interval, time_t now_second);
	//virtual void OnUserLogin(Role * role);	
	//virtual void OnWeekChange();

	//virtual void OnConnectCrossServerSucc();
	//virtual void OnUserLoginOriginGame(int role_id);
	//virtual void OnGameServerConnect(std::set<int> server_id);	//隐藏服 并且 是跨服活动 才会调

private:


};
//,0x570c481,0xe4ff0000
#endif