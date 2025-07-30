#ifndef __MAP_UNLOCK_HPP__
#define __MAP_UNLOCK_HPP__

#include "gamedef.h"
#include "servercommon/mapunlockdef.h"

#include <set>
#include <vector>

class RoleModuleManager;
class MapUnlock				// 场景解锁
{
public:
	MapUnlock();
	~MapUnlock();

	//void* operator new(size_t c);
	//void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager, const MapUnlockParam & map_unlock_data);
	void GetOtherInitParam(MapUnlockParam * map_unlock_data);

	void OnRoleLogin();									// 玩家登陆时的操作
	void OnRoleLevelUp();
	bool CheckMapIsUnlock(int scene_id, int *param = NULL);			// 检测场景是否解锁

	void OnEnterScene(int scene_id);				// 记录玩家进入场景，检测弹窗
	bool IsRoleHaveBeenEntered(int scene_id);		// 检测玩家是否曾进入过某个场景

	void OnRolePassEctype(int ectype_type);			// 记录玩家通关副本
	bool IsRoleHaveBeenPassed(int ectype_type);		// 检测玩家是否通关过某个副本

	void ReqestRemoveActiveIrregularID(int remove_id);

	void GmChangeCheckSwitch(int switch_state);		// 改变检查标记的状态

private:
	RoleModuleManager * m_role_module_mgr;
	MapUnlockParam		m_map_unlock_param;

	bool gm_check_switch;		// 场景合法性检查标记；如果为 false 表示不检测场景解锁
	std::set<int> unlock_irregular_id_set;	// 当前解锁的大地图ID
	std::set<int> active_irregular_id_set;	// 需要播放动画的标记

	bool CheckUnlockByCondition(int unlock_type, int unlock_param);
	void SendSceneActiveID(int active_id);			// 用于首次进入场景时提示弹窗
	void SendIrregularIdList();
	void SendFBTypeFirstEnter();
	void CheckAllSceneUnlock();
};

#endif