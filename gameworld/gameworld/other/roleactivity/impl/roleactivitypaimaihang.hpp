#ifndef __ROLE_ACTIVITY_PAI_MAI_HANG_HPP__
#define __ROLE_ACTIVITY_PAI_MAI_HANG_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"

class RoleActivityPaiMaiHang : public RoleActivity
{
public:
	RoleActivityPaiMaiHang(int type);
	virtual ~RoleActivityPaiMaiHang();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	void OnSetLastBidPrice(int seq, int last_bid_price);
	void OnRetReset(int seq);
public:
	void SendAllInfo();
	void SendSingleInfo(int seq);
	void OnBidShopItem(int seq, int bid_price);
	void OnAutoBidShopItem(int seq, int bid_price, int add_price);
	void OnCancelAutoBid(int seq);
	void OnRetrieveGold(int seq, int retrieve_gold, int add_price);
	void OnChangeAddPrice(int seq, int add_price);

	void OnReturnGold(int role_uid, int gold_num, int item_id, int open_day, int seq, bool is_auto_bid);
	void OnReturnGold(Role * role, int gold_num, int item_id, int open_day, int seq, bool is_auto_bid, int role_uid);

	void OnSetShowFlag(int seq, char is_show_user_flag);

	RARolePaiMaiHangParam m_param;
};


#endif //__ROLE_ACTIVITY_PAI_MAI_HANG_HPP__
