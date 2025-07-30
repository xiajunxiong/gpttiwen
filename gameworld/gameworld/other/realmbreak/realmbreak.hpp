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
	void OnUseItem(ItemID item_id, short num);			// ʹ��������
	void OnBreak();										// ͻ��	
	void OnStartTuna();									// ����
	void OnBattleFinish(bool is_win, int seq);			// ս������
	void OnFetchExp();									// ��ȡ��ɫ����
	void GmSetInfo(int op_type, int param1);			
	int GetCurLevel() { return m_param.cur_seq; }
	bool IsPassFightLevel(int fight_seq);
	int GetCurFightSeq() { return m_param.cur_fight_seq; };

	void OnSoulFight(int seq);							// ����֮��ս��

	void SendAllInfo();
	void SendItemInfo();

private:
	void GaiMingReissue();

	void FillItemVec(std::vector<ItemConfigData> & item_list);
	bool DepositItem(int item_num, RealmBreakItem * item_list);

	RoleModuleManager * m_module_mgr;

	RealmBreakParam m_param;
	unsigned int last_tuna_timestamp;				//�ϴ�����ʱ���

	unsigned int last_add_break_exp_timestamp;		//�ϴ�ʱ������������ֵʱ���
	unsigned int last_add_player_exp_timestamp;		//�ϴ�ʱ�������Ӿ���ʱ���
};






#endif