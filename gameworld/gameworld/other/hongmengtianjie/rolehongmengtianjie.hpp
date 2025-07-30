#ifndef __ROLE_HONGMENG_TIANJIE_HPP__
#define __ROLE_HONGMENG_TIANJIE_HPP__

#include "servercommon/hongmengtianjiedef.hpp"

class RoleModuleManager;
class Role;

class RoleHongMengTianJie
{
public:
	RoleHongMengTianJie();
	~RoleHongMengTianJie();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const RoleHongMengTianJieParam & param);
	void GetInitParam(RoleHongMengTianJieParam * param);

	void Update(unsigned int now_second);
	void OnWeekChange();

	void SendInfo();
	bool IsBossTimes(bool is_all, int boss_index = 0);
	bool IsZhuZhuan(int boss_index);
	bool IsFBPass(int fb_id);
	void SendBossReward(int fb_id, int boss_index, std::vector<ItemConfigData> & item_list);

	void OnBuyShop(int shop_id, int num);

private:

	RoleModuleManager * m_mgr;
	RoleHongMengTianJieParam m_param;
};

#endif
