#ifndef __ROLE_ACTIVITY_SHEN_SHOU_JIANG_LIN_HPP__
#define __ROLE_ACTIVITY_SHEN_SHOU_JIANG_LIN_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleActivityShenShouJiangLin: public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_SHEN_SHOU_JIANG_LIN;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN;

	RoleActivityShenShouJiangLin(int type);
	virtual ~RoleActivityShenShouJiangLin();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	int OnBuyGiftItem(int seq, ARG_OUT int * price);

	int OnBuyItemCheck(int seq, long long ARG_OUT & unique_key);
	int OnBuyItem(int chong_zhi_gold, long long unique_id);

	BaoDiInfoItem * GetBaoDiInfoItem(int bao_di_id);
	void SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list);
	void GetAllBaoDiInfoItem(BaoDiInfoItem (&bao_di_info_list)[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM]);
public:
	void SendInfo();

	RAShenShouJiangLinParam m_param;
};

#endif	//__ROLE_ACTIVITY_PET_LOOT_HPP__

