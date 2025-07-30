#ifndef __ROLE_ACTIVITY_TU_JIAN_TREASURE_HPP__
#define __ROLE_ACTIVITY_TU_JIAN_TREASURE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/ratujiantreasureparam.hpp"

class RoleActivityTuJianTreasure : public RoleActivity
{
public:
	RoleActivityTuJianTreasure(int type);
	virtual ~RoleActivityTuJianTreasure();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);

	virtual void OnLogin();
	virtual void OnLoginSendInfo();
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);
	virtual void OnRoleLevelUp(int old_level, int to_level);

	bool IsRoleTuJianTreasureOpen() { return m_param.ra_tu_jian_treasure_end_timestamp > 0 ? true : false; }
	void OnTuJianTreasureCheckReward(int type, int param1, std::vector<ItemConfigData> item_list);		//类型1、2设置次数先经过此函数判断
	void OnTujianTreasureBuyCard(ItemID card_id, int trade_way);
	void CheckTuJianTreasureOpen(int role_level);

	void OnSetLoginDay();
public:
	void SendRATuJianTreasureInfo();
	void OnRATuJianTreasureFetchDay(int seq);
	void OnRATuJianTreasureFetch(int seq);
	bool SetTuJianTreasureCount(int type, int param1, int param2, int count = 1);
	void OnRATuJianTreasureSetData();


	RATuJianTreasureParam m_param;
};

#endif	//__ROLE_ACTIVITY_TU_JIAN_TREASURE_HPP__
