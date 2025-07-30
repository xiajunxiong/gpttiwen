#ifndef __BIG_DIPPER_MANAGER_HPP__
#define __BIG_DIPPER_MANAGER_HPP__

#include "global/activity/activity.hpp"
#include <map>
#include <set>
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "obj/character/role.h"
#include "servercommon/bigdipperdef.h"

class Role;
class RoleModuleManager;
class BigDipperManager
{
public:
	BigDipperManager();
	~BigDipperManager();
	static BigDipperManager & Instance();

	void Init(const BigDipperMonsterParam & param);
	void GetInitParam(BigDipperMonsterParam *param);
	void Update(unsigned long interval, time_t now_second);

	void OnHideInfoBack(BigDipperMonsterParam *param);
	const BigDipperMonsterParam * GetBigDipperMonsterParam() { return &data; }

	void OnUserLogin(Role *role);
	void OnUserLogout(Role *role);

	void OnWeekChange();

	bool CanBattleMonsterBySn(int sn);

	void SendInfo(Role * role = NULL);
private:
	
	BigDipperMonsterParam data;
};

#endif