#ifndef __RAND_ACTIVITY_QIONG_LOU_YU_YAN_HPP__
#define __RAND_ACTIVITY_QIONG_LOU_YU_YAN_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/raqionglouyuyanparam.hpp"
#include <vector>

class RandActivityQiongLouYuYan : public RandActivity
{
public:
	RandActivityQiongLouYuYan(RandActivityManager *activity_manager);
	~RandActivityQiongLouYuYan();

	virtual void Init(const ActivityData &data);
	virtual void GetInitParam(ActivityData & data);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual bool CanSyncActivityDataToCross() { return false; }
	virtual void OnUserLogin(Role *user);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	bool CanEatFood(Role * role);
	bool OnRoleAdd(Role * role,int add_num);
	int GetAccumulate() { return m_param.accumulate_num; }
	unsigned int GetOpenTimes() { return m_param.open_times; }
	void SendInfo(Role * role = NULL);
	int GetBanquetType() { return m_param.banquet_type; }
	void GmNextTamp() { m_param.next_status_tamp = (unsigned int)EngineAdapter::Instance().Time(); }
	void GmAddAccumulate(int add_num);
private:
	void NextSendAllInfo();
	void CheckAccumulate();//¼ì²éÊÇ·ñÂú×ã¿ªÑç»á
	RAQiongLouYuYanParam m_param;

	//ÔÝ´æ
	time_t next_send_all_tamp;
	std::vector<UserID> member_join_vec;
};

#endif
