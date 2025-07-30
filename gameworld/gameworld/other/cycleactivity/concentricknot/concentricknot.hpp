#ifndef __CONCENTRIC_KNOT_HPP__
#define __CONCENTRIC_KNOT_HPP__

#include "servercommon/cycleactivity/concentricknotparam.hpp"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

class ConcentricKnot
{
public:
	ConcentricKnot();
	~ConcentricKnot();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * mgr, const ConcentricKnotParam & param);
	void GetInitParam(ConcentricKnotParam * param);

	void OnDayChange();
	void OnLogout();

	void OnClientReq(void * msg);

	void OnAddActiveReward(int active);
	bool IsReward() { return m_param.reward_times > 0; }

	void SendInfo();
	void SetUseItem(int use_item) { m_param.use_item = use_item; }
	void SetEndGameTimes(unsigned int game_end_times) { m_param.game_end_times = game_end_times; }
private:
	void OnCanGame();
	void OnAgree(int is_agree);
	void OnStartGame(int cards_group);
	void OnOpenCard(int card_id);
	void OnEndGame(bool is_finish);

	RoleModuleManager * m_mgr;
	ConcentricKnotParam m_param;
};

#endif
