#ifndef __ROLE_JOIN_ACTIVITY_REWARD_HPP__
#define __ROLE_JOIN_ACTIVITY_REWARD_HPP__

#include "servercommon/rolejoinactivityrewardparam.hpp"
#include "protocol/msgrolejoinactivityreward.hpp"

class RoleModuleManager;
class Role;



class RoleJoinActivityReward
{
public:
	RoleJoinActivityReward();
	~RoleJoinActivityReward();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleJoinActivityRewardParam & param);
	void GetInitParam(RoleJoinActivityRewardParam * param);
	
	void OnLogin();
	void OnDayChange();

	void OnClientReq(Protocol::CSRoleJoinActivityRewardReq * _req);

	void RecodeJoinInfo(int _activity_type);

	static void AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item);
	
private:
	void GetJoinReward(int _activity_type);
	void SendInfo();
	void RewardSettlementByMail();

	bool IsCanOneClick(int activite_type);
	void OneKeyFinish(int bit_type);	// bit_type: 玩家选择的完成类型(bitset)

	RoleModuleManager * m_role_module_mgr;
	RoleJoinActivityRewardParam m_param;
};



#endif