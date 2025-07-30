#ifndef __RAND_ACTIVITY_SALTY_SWEET_BATTLE_HPP__
#define __RAND_ACTIVITY_SALTY_SWEET_BATTLE_HPP__

#include "global/randactivity/randactivity.hpp"
#include "protocol/msgrandactivity.hpp"
#include "servercommon/roleactivity/razongxiangparam.hpp"

class RandActivitySaltySweetBattle :public RandActivity
{
public:
	RandActivitySaltySweetBattle(RandActivityManager *activity_manager);
	~RandActivitySaltySweetBattle();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnUserLogin(Role *user);
	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void SyncActivityDataFromCross(CrossRandActivityDataParam &param);
	void OnGameDataChange(int pre_win_type, int salty_value, int sweet_value, int status_mod, int to_uid);

	void OnRoleVote(int add_type, int add_value, Role * role = NULL);
	void OnUidVote(int add_type, int add_value, int uid);
	void GetWinType(int *pre_win, int * status);
	void RASendGameInfo(Role * role = NULL, bool is_brocast = false);
	void OnDataChangeSendCross(int to_uid = 0);
	void GMset() { m_param.activity_end_tamp = (unsigned int)EngineAdapter::Instance().Time(); }
private:
	void EndGame();
	
	RASaltySweetBattleServerParam m_param;
};


#endif