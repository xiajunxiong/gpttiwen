#ifndef __TREASRUE_TASK_HPP__
#define __TREASRUE_TASK_HPP__
#pragma once

#include "servercommon/treasuretaskdef.h"

class RoleModuleManager;
class TreasureTask
{
public:
	TreasureTask();
	~TreasureTask();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const TreasureTaskParam& param);
	void GetInitParam(TreasureTaskParam* param);

	void OnDayChange();
	void OnReq(void * msg);

	void OnAccept();
	void OnCommit();

	void SendInfo();
	void SendTotalItem();

	bool IsCanOneClick();	// 是否可以一键完成
	void OneClickFinishTask(unsigned int& exp, int& coin, int& jade, std::vector<ItemConfigData>& rewards);

private:
	void RandPool();
	void ReshrefTask();

	RoleModuleManager * m_role_module_mgr;
	TreasureTaskParam m_param;
};

#endif // !__TREASRUE_TASK_HPP__
