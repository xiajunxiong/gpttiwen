#ifndef __RAND_ACTIVITY_SERVER_COMPETITION_HPP__
#define __RAND_ACTIVITY_SERVER_COMPETITION_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"

class Role;
class RandActivityServerCompetition : public RandActivity
{
public:
	RandActivityServerCompetition(RandActivityManager *activity_manager);
	~RandActivityServerCompetition();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void SyncActivityDataFromCross(CrossRandActivityDataParam &param);		

	bool IsInTime(int rank_type);
	void OnSendAllInfo(Role * role = NULL);

	int GetRankTypeParam(int user_uid, int type);

	void SendAllRankInfoToCross(int role_id);

	void GmTestGiveReward(Role* role);
private:
	void OnActivityOpen();
	void OnCheckTime(unsigned int now_second, bool is_close);
	void OnActivityRankEnd(int rank_type, bool is_close);

	RAServerCompetitionParam m_param;
	bool is_update_flag;
};


#endif
