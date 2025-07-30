#ifndef __ROLE_ACTIVITY_ZHAOCAIJINBAO_HPP__
#define __ROLE_ACTIVITY_ZHAOCAIJINBAO_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/razhaocaijinbaoparam.hpp"
#include "servercommon/rolecommonsavedef.h"
#include <deque>

class RoleActivityZhaoCaiJinBao : public RoleActivity
{
public:
	static const int BUY_TYPE = BUY_TIME_TYPE_ZHAOCAIJINBAO;
	static const int COMMON_SAVE_TYPE = ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO;

	RoleActivityZhaoCaiJinBao(int type);
	virtual ~RoleActivityZhaoCaiJinBao();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);
	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnAddChongZhi(long long chongzhi);

	int OnBuyItemCheck(int shop_index, long long ARG_OUT & unique_key);
	int OnCmdBuyItem(int chong_zhi_gold, long long unique_id);
	int OnBuyGiftItem(int seq, ARG_OUT int * price);

public:
	void RASendInfo();
	void RADraw(int times);
	void RARefresh(bool is_client = false);
	void RABuy(int seq);
	void RAFetchReward(int seq);
	void RASendRecordListInfo();
	BaoDiInfoItem * GetBaoDiInfoItem(int bao_di_id);
	void SetBaoDiInfoAfterDraw(int getreward_bao_di_id, std::set<int> add_times_list);

	RAZhaoCaiJinBaoParam m_param;

	int m_pool_rate;
	std::map<int, int> m_pool_map;

	std::deque<RAZhaoCaiJinBaoRecordInfo> m_record_list;
};

#endif	//__ROLE_ACTIVITY_ZHAOCAIJINBAO_HPP__
