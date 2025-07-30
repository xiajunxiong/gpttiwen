#ifndef __RAND_ACTIVITY_SHEN_SHOU_JIANG_LIN_HPP__
#define __RAND_ACTIVITY_SHEN_SHOU_JIANG_LIN_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityShenShouJiangLin: public RandActivity
{
public:
	RandActivityShenShouJiangLin(RandActivityManager *activity_manager);
	~RandActivityShenShouJiangLin();

	virtual void OnUserLogin(Role * user);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	void GetPoolReward(int get_percent, int ARG_OUT & gold_num, int ARG_OUT & item_num);

	int Draw(Role * role);
	int DrawFromHidden(int role_id, long long req_id);

	void SendInfo(Role * role = NULL);
	void GameHiddenSendInfo(int role_id);
	int AddDrawTime(int draw_times);
private:
	RandActivityShenShouJiangLinParam m_param;
};

#endif
