#ifndef __MINI_GAME_HPP__
#define __MINI_GAME_HPP__

#include "servercommon/minigamedef.hpp"

class RoleModuleManager;
struct MiniGameParam;
class MiniGame
{
public:
	MiniGame();
	~MiniGame();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const MiniGameParam& param);
	void GetInitParam(MiniGameParam* out_param);

	void OnFinishNextStageReq();
	void OnFetchPhaseRewardReq(int phase_seq);
	void OnStartMiniGameBattleReq(int seq, bool has_pet, bool has_partner, bool has_skill);
	void OnSetMiniGameIdReq(int mini_game_id);

	void OnLogin();
	void SendInfo();
private:
	int GetNpcSeq(int seq);

	RoleModuleManager* m_mgr;
	MiniGameParam m_param;
};

#endif