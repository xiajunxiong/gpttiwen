#ifndef __ROLE_ACT_WEEKEND_GIFT_HPP__
#define __ROLE_ACT_WEEKEND_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/raweekendgiftdef.hpp"
#include "servercommon/rolecommonsavedef.h"

class RoleACTWeekendGift : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_WEEKEND_GIFT;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT;

	RoleACTWeekendGift(int type);
	virtual ~RoleACTWeekendGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();


	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	//ֱ��
	int BuyItemCheck(int cfg_ver, int gift_index, long long ARG_OUT & unique_key);
	int BuyItem(int chong_zhi_gold, long long unique_id);

	void SendInfo();

	//�Ƴ���ʱ���
	void RemoveTimeOut();
	//�����������
	void CheckPutGift(unsigned int now_dayid);
public:
	//��ȡ����λ��
	int GetSaveIndex(const int gift_type_index) const;

	RoleRAWeekendGiftParam m_param;
};




#endif