#ifndef __RAND_ACTIVITY_HORCRUX_TIANCHENG_HPP__
#define __RAND_ACTIVITY_HORCRUX_TIANCHENG_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"

class RandActivityHorcruxTianCheng : public RandActivity
{
public:
	RandActivityHorcruxTianCheng(RandActivityManager *activity_manager);
	~RandActivityHorcruxTianCheng();

	virtual void OnUserLogin(Role * user);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	void GetPoolReward(int get_percent, unsigned int ARG_OUT & item_num);
	void SendInfo(Role * role = NULL);
	void GameHiddenSendInfo(int role_id);

	void Draw(Role * role);
	int DrawFromHidden(int role_id, long long req_id, int check_floor, const BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM>* bit_map);

	void AddDrawTime(int draw_times);

private:
	RandActivityHorcruxTianChengParam m_param;
};

#endif
