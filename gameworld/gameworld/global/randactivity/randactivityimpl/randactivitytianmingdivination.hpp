#ifndef __RAND_ACTIVITY_TIAN_MING_DIVINATION_HPP_
#define __RAND_ACTIVITY_TIAN_MING_DIVINATION_HPP_

#include "global/randactivity/randactivity.hpp"
#include "servercommon/activitydef.hpp"

class RandActivityTianMingDivination : public RandActivity
{
public:
	RandActivityTianMingDivination(RandActivityManager *rand_activity_manager);
	virtual ~RandActivityTianMingDivination();

	void OnItemAdd(TianMingDivinationInfo * info);

	void OnRoleGetRateItem(Role *role,const ItemConfigData *item);
	void SendTianMingPoolInfo(Role *role);
private:
	std::vector<TianMingDivinationInfo> rate_info_vec;//珍稀物品活动列表
};

#endif 