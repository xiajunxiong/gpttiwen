#ifndef __KUA_FU_YING_XIONG_LU_MANAGER_HPP__
#define __KUA_FU_YING_XIONG_LU_MANAGER_HPP__

#include "servercommon/struct/global/kuafuyingxiongludef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;
class KuaFuYingXiongLuManager
{
public:
	static KuaFuYingXiongLuManager & Instance();

	void Update(unsigned long interval, time_t now_second);

	void Init(const KuaFuYingXiongLuActivityParam & param);
	void GetInitParam(KuaFuYingXiongLuActivityParam * param);
	void OnRoleLogin(Role * role);

	void OnReloadConfig();

	void OnConnectCrossSucc();
	void OnHiddenSynDataToGameworld(int server_id = 0);
	void OnGameRecvHiddenSynData(crossgameprotocal::HiddenGameRetSynKuaFuYingXiongLuData * msg);

	void OnHiddenRecvCrossActivityRankEnd(crossgameprotocal::CrossHiddenRetKuaFuYingXiongLuRankEnd * msg, std::vector<int> & rank_uid_list);

	void SendAllInfo(Role * role = NULL);
	
	//玩法重开
	void OnReopen();
	void OnClose();
	void OnSetLockFlag(bool is_lock);

	int GetPersonRankType(int kfyxlr_rank_type) const;
	long long GetActivityUniqueID() const { return m_param.activity_unique_id; }
	int GetActivityBeginTimestamp() const { return m_param.activity_begin_timestamp; }
	int GetActivityOpenCrossDay() const { return m_param.activity_open_cross_day; }
	bool IsOpen()const;
	bool IsRankInTime(int rank_type) const;

private:
	KuaFuYingXiongLuManager();
	~KuaFuYingXiongLuManager();

	//检测榜单结算
	void OnCheckTime(unsigned int now_second);
	//活动榜单结束
	void OnActivityRankEnd(int rank_type);
	//检测是否开启新一轮
	void OnCheckOpenNew(unsigned int now_second);		
	//获取榜单是否已全部发放奖励
	bool IsAllSendReward()const;

	KuaFuYingXiongLuActivityParam m_param;
};




#endif // __KUA_FU_YING_XIONG_LU_MANAGER_HPP__
