#ifndef __YIN_YANG_EYE_HPP__
#define __YIN_YANG_EYE_HPP__

#include "servercommon/yinyangeyedef.h"

class RoleModuleManager;
class Role;

//阴阳眼
class YinYangEye
{
public:
	YinYangEye();
	~YinYangEye();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const YinYangEyeParam & param);
	void GetInitParam(YinYangEyeParam * param);

	void OnRoleLogin();

	void OnOpen();
	void OnFetchBox(int seq);
	void OnNpcOp(int seq);
	void OnBattleFinish(int type, int seq, bool is_win);
	void OnFetchReward(int type, int seq);					//延迟获取奖励 如战斗完还要对话才获取奖励
	int GetFinishRewardCount();								//获得已完成所需开眼的数量

	void SendInfo();

	bool IsOpen() { return m_param.open_mark ? true : false; }
private:

	RoleModuleManager * m_mgr;
	YinYangEyeParam m_param;
};

#endif

