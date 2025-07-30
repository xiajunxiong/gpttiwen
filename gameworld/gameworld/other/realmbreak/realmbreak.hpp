#ifndef __REALM_BREAK_HPP__
#define __REALM_BRAEK_HPP__

#include "servercommon/realmbreakdef.hpp"

class RoleModuleManager;
class RealmBreak
{
public:
	RealmBreak();
	~RealmBreak();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *module_mgr, const RealmBreakParam & param);
	void GetInitParam(RealmBreakParam * param);
	void Update(time_t now_second);

	void OnLogin(long long last_save_time);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnRoleLevelUp();
	void OnUseItem(ItemID item_id, short num);			// 使用灵力丹
	void OnBreak();										// 突破	
	void OnStartTuna();									// 吐纳
	void OnBattleFinish(bool is_win, int seq);			// 战斗结束
	void OnFetchExp();									// 领取角色经验
	void GmSetInfo(int op_type, int param1);			
	int GetCurLevel() { return m_param.cur_seq; }
	bool IsPassFightLevel(int fight_seq);
	int GetCurFightSeq() { return m_param.cur_fight_seq; };

	void OnSoulFight(int seq);							// 心灵之境战斗

	void SendAllInfo();
	void SendItemInfo();

private:
	void GaiMingReissue();

	void FillItemVec(std::vector<ItemConfigData> & item_list);
	bool DepositItem(int item_num, RealmBreakItem * item_list);

	RoleModuleManager * m_module_mgr;

	RealmBreakParam m_param;
	unsigned int last_tuna_timestamp;				//上次吐纳时间戳

	unsigned int last_add_break_exp_timestamp;		//上次时间间隔增加灵力值时间戳
	unsigned int last_add_player_exp_timestamp;		//上次时间间隔增加经验时间戳
};






#endif