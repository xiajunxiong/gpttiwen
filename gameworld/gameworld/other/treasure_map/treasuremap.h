#ifndef __TREASURE_MAP_HPP__
#define __TREASURE_MAP_HPP__

#include "servercommon/treasuremapdef.hpp"

class Role;
class RoleModuleManager;
class NorexItem;
class TreasureMap
{
public:
	TreasureMap();
	~TreasureMap();

	void* operator new(size_t c);
	void operator delete(void* m);

	void OnRoleEnterScene();
	void Init(RoleModuleManager * mgr, const TreasureMapParam & param);													//获得初始化Param
	void GetInitParam(TreasureMapParam * param);
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnFindDataReset();

	int GetYesterdayUndoneTimes() { return m_param.find_back_times; }
	bool RewardFindBack(int find_type, int count);
	void OnActiveStarTreasureMap(short column, short index);
	void OnResetTreasureMap(short column, short index);

	void OnRoleUpLevel(int from_level, int to_level);

	void ReFresh(bool is_force = true,bool _is_onekeyfinish = false);	
	void Clear();
	void SendInfo(bool is_new = false);

	bool CanStart();
	void OnFightWin(bool _is_one_key_finish = false);
	int GetMonsterId();
	int GetTodayFinish() { return m_param.get_reward_times; }   //今天获得宝图的次数

	void OneKeyFinishTreasureMap();			// 一键完成

	void OneClickUsedTreasureMap(unsigned int num);
	void AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item);

	//使用没有净值的宝图
	bool UseTreasureMap(int map_type, int use_num, ARG_OUT int* actual_use_num);
	void SendNotice(int param1, int use_item_id, int is_notice) const;

private:
	RoleModuleManager * m_role_module_mgr;
	TreasureMapParam m_param;
};

#endif