#ifndef __TRANSACTION_TASK_HPP__
#define __TRANSACTION_TASK_HPP__

#include "servercommon/transactiontaskdef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/struct/moneyotherdef.h"
#include "gameworld/protocol/msgother.h"

class RoleModuleManager;
class TransactionTask
{
public:
	TransactionTask();
	~TransactionTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * module_mgr, const TransactionTaskParam & param);
	void GetInitParam(TransactionTaskParam * param);

	void OnRoleLogin();
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();
	void TransactionTaskOp(Protocol::CSTransactionTaskReq * req);

	void RoleLevelUp(int cur_level);
	void CheckVersion();
	void ReferItem(int index, int refer_num,bool _is_onekey_finish = false);				// 提交物品
	void GetFinallyReward(bool _is_onekey_finish = false);								// 领取总奖励
	int GetTodayFinish();									
	void OneKeyFinishTransactionTask();									// 一键完成
	bool OnTransactionTaskFindReward(int consume_type, int count);
	int GetFindRewardCount() { return m_param.find_count; }	

	void SendTaskInfo();

private:
	bool CheckAllTaskFinish();
	int GetLastReferNum();
	void RefreshTask(int level);
	void RemoveYestodayTask();

	RoleModuleManager * m_module_mgr;
	TransactionTaskParam m_param;

};






#endif
