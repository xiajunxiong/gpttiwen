#ifndef __ROLE_ACTIVITY_MONPOLY_GIFT_HPP__
#define __ROLE_ACTIVITY_MONPOLY_GIFT_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/racowparam.hpp"

class RoleActivityMonpolyGift : public RoleActivity
{
public:
	RoleActivityMonpolyGift(int type);
	virtual ~RoleActivityMonpolyGift();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnBattleFinish(bool is_win,int battle_mode);
public:
	void SendInfo(int old_point = 0, int rand_point = 0);
	void SendQuestionInfo();
	void UseDice();
	void ShowDiceEnd();
	void SpecialPlay();
	void Answer(int answer_id);
	void GetRandGift();
	void ExchangeItem(int item_seq);
	void OnNewNowMap();

	RAMonpolyGiftParam m_param;
};

#endif	//__ROLE_ACTIVITY_LUCKY_BAG_HPP__

