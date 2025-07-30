#ifndef __MAP_EXPLORATION_HPP__
#define __MAP_EXPLORATION_HPP__

#include "servercommon/mapexplorationdef.hpp"
#include <set>
#include "mapexplorationconfig.hpp"

class Role;
class RoleModuleManager;

class MapExploration
{
public:
	MapExploration();
	~MapExploration();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager* GetRoleModuleManager() { return m_role_module_mgr; }
	void Init(RoleModuleManager * role_module_manager, const MapExplorationParam & param);
	void GetOtherInitParam(MapExplorationParam *param);
	void OnRoleLogin();
	void ExplorationMap(int seq);
	void SendInfo();
private:
	RoleModuleManager * m_role_module_mgr;

	MapExplorationParam m_param;
};


#endif // !__FABAO_HPP__
