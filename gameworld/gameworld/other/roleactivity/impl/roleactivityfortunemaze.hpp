#ifndef __ROLE_ACTIVITY_FORTUNE_MAZE_HPP__
#define __ROLE_ACTIVITY_FORTUNE_MAZE_HPP__

#include "other/roleactivity/roleactivity.hpp"
#include "servercommon/roleactivity/rafortunemazeparam.hpp"

class RoleActivityFortuneMaze : public RoleActivity
{
public:
	RoleActivityFortuneMaze(int type);
	virtual ~RoleActivityFortuneMaze();

	void* operator new(size_t c);
	void operator delete(void* m);

	virtual void Init(RoleModuleManager *mgr, const RoleActivityRoleData &p);
	virtual void GetInitParam(RoleActivityRoleData *p);
	virtual void Update(time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	virtual void OnLoginSendInfo();
	virtual void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	virtual void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor);

	void OnPetLearnSkill();
	void JoinTimeLimitActivity();
	void CostCoin(int coin);
	void CatchPet();
	void PassCrystal();
	void ExchangeShop();
	void GetPrestige(int add_prestige);
	void TreasureHunt(int times);
	void KillWildBoos();
	void StrengThenPet();
	void ExpeditionModron();
	void UseCrystalPower(int use_power);
	void OnCheckRoleLoginDelay(int old_dayid, int now_dayid);
	void BuyStone(int num);

	void OnRoleEnterSpecialFb();
	void OnRoleOpenTreasureChest();
	void SetRoleOldSceneId(int old_scene_id);
	//----------------------Ãÿ ‚ÕÊ∑®£®∆¥Õº£©
	void SetMapMarkIndex(int map_index);
	void RandPinTuIndex(int index);
	void MoivePinTu(int map_index, int moive_index_1, int moive_index2);

	void GMAddTimes(int times);
	void ItemAddTimes(int times);
private:
	void CheckPinTuFinish(int map_index);
	void SendInfo();
	void SendMapInfo();
	
	RAFortuneMazeParam m_param;
};


#endif
