#ifndef __ROLE_ACTIVITY_HPP__
#define __ROLE_ACTIVITY_HPP__

#include "servercommon/roleactivitydef.hpp"
#include "servercommon/activitydef.hpp"
#include "protocol/msgrandactivity.hpp"
#include "protocol/msgactivity.hpp"
#include "obj/character/npc_gather.h"


class Role;
class RoleModuleManager;

struct RoleActivityRoleDataNode
{
	RoleActivityRoleDataNode()
	{
		is_dirty = false;
		is_new_create = false;
		is_invalid = false;
		memset(&data, 0, sizeof(data));
	}

	bool is_dirty;
	bool is_new_create;		
	bool is_invalid;
	RoleActivityRoleData data;
};

class RoleActivity
{
public:
	RoleActivity(int type);
	virtual ~RoleActivity();

	void SetRole(RoleModuleManager * mgr) { m_role_module_mgr = mgr; }
	RoleModuleManager *  GetRole() { return m_role_module_mgr; }
	int GetRoleActivityType() { return m_role_activity_type; }

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p) {}
	virtual void GetInitParam(RoleActivityRoleData *p) {}
	virtual void ClearDirtyMark();

	virtual void OnLogin() {}				
	virtual void OnLoginSendInfo() {}			
	virtual void OnLoginCheck() {}			//登录时调用 (与活动是否开启无关, 也不会调用 CheckInitRandActivity)

	virtual void Update(time_t now_second) {}
	virtual void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid){}
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid) {}
	virtual void OnWeekChange() {}
	virtual void OnMonthChange() {}
	virtual void OnRoleLevelUp(int old_level, int to_level) {}

	virtual int  OnActivityEnterReq(Protocol::CSActivityEnterReq *aer) { return  0; }
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false) {}
	virtual void OnRAOpenOnlineSend() {}
	virtual void OnRAClose() {}

	virtual void OnCheckInitBonus();
	virtual bool IsFinishBonusCond(int type = 0) { return false; }

	void OnSendResultNotice(short result_type, const std::vector<ItemConfigData> * item_list = NULL, const std::vector<RAMoneyInfo> * money_list = NULL, int param1 = 0, int param2 = 0, int exp = 0) const;

	virtual void OnConsumeGold(long long consume_gold) {}
	virtual void OnAddChongZhi(long long chongzhi) {}

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor) {}
	virtual void OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req) {}

	virtual void OnSuccsGather(int gather_id, int seq_id, const Posi & gather_pos) {}

	virtual bool IsDataChange();
	virtual void SetRoleDataChange() { m_is_dirty = true; }

	virtual bool IsNewCreate() { return m_is_new_create; }
	virtual void SetNewCreate(bool flag) { m_is_new_create = flag; }

	virtual bool IsInvalid() { return m_is_invalid; }
	virtual void SetInvalid(bool flag) { m_is_invalid = flag; }

	void OnParticipateRA(const char* func);
private:
	RoleActivity();

protected:
	RoleModuleManager * m_role_module_mgr;
	int m_role_activity_type;

	bool m_is_dirty;
	bool m_is_new_create;							//Êý¾Ý¿âÊÇ·ñÓÐ¶ÔÓ¦¼ÇÂ¼
	bool m_is_invalid;

};

#endif // __ROLE_ACTIVITY_HPP__


