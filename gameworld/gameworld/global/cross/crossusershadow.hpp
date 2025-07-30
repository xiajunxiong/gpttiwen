#ifndef __CROSS_USER_SHADOW__
#define __CROSS_USER_SHADOW__

#include <set>
#include "engineadapter.h"
#include "servercommon/servercommon.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;

class CrossUserShadow
{
public:
	static CrossUserShadow & Instance();

	void OnSyncCrossUser(crossgameprotocal::CrossGameSyncCrossUser* cgscu);
	void OnUserEnteringHidden(crossgameprotocal::CrossGameUserEnteringHidden* cgueh);
	void OnDisconnectFromCrossServer();
	void OnUserAttemptLogin(int uid);
	void OnUserLogin(const UserID& user_id);
	bool IsUserInCross(int uid);
	void OnKickCrossUserAck(crossgameprotocal::CrossGameKickUserAck* msg);

	int GetInCrossUserNum() const { return m_in_cross_user_set.size(); }
	int GetEnteringCrossUserNum() const { return m_entering_cross_user_set.size(); }
private:
	CrossUserShadow();
	~CrossUserShadow();

	typedef std::set<int> InCrossUserSet;
	InCrossUserSet m_in_cross_user_set;			// 处于跨服状态的角色表

	typedef std::set<int> EnteringCrossUserSet;
	EnteringCrossUserSet m_entering_cross_user_set; // 处于正在登录跨服的角色表
};

#endif // __CROSS_USER_SHADOW__

