#ifndef __ROLE_CHALLENGE_FB_HPP__
#define __ROLE_CHALLENGE_FB_HPP__

#include "servercommon/rolechallengefbdef.hpp"

class RoleModuleManager;
class RoleChallengeFb
{
public:
	RoleChallengeFb();
	~RoleChallengeFb();

	void InitParam(RoleModuleManager * mgr, const ChallengeFbParam & param);
	void GetChallengeFbParam(ChallengeFbParam * param);

	void OnWeekChange();
	void OnRoleLogin();
	void OnPassChallengeFb(int fb_id, int chapter);

	int GetHighestPass(int fb_id);

	void* operator new(size_t c);
	void operator delete(void* m);

private:
	RoleModuleManager * m_mgr;
	ChallengeFbParam m_param;

	void SendRoleChallengeFbInfo();
};

#endif