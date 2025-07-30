#ifndef __RAND_ACTIVITY_JIXING_GAOZHAO_HPP__
#define __RAND_ACTIVITY_JIXING_GAOZHAO_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/rajixinggaozhaoparam.hpp"
#include <deque>

class RandActivityJiXingGaoZhao : public RandActivity
{
public:
	RandActivityJiXingGaoZhao(RandActivityManager *activity_manager);
	~RandActivityJiXingGaoZhao();

	virtual void OnUserLogin(Role * user);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void Init(const ActivityData & arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	void GetPoolReward(int get_percent, unsigned int ARG_OUT & item_num);
	void SendInfo(Role * role = NULL);
	void GameHiddenSendInfo(int role_id);

	bool Draw(Role * role);
	int DrawFromHidden(int role_id, long long req_id, int check_floor, const BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM>* bit_map);

	void AddDrawTime(int draw_times);

	void AddDeque(const JiXingGaoZhaoRecordInfo & info);
	void SendRecordInfo(Role * role = NULL);
private:
	RandActivityJiXingGaoZhaoParam m_param;

	std::deque<JiXingGaoZhaoRecordInfo> queue_map;
};

#endif
