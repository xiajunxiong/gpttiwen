#ifndef __ROLE_ACTIVITY_MANAGER_HPP__
#define __ROLE_ACTIVITY_MANAGER_HPP__

#include "servercommon/roleactivitydef.hpp"
#include "servercommon/activitydef.hpp"
#include "protocol/msgrandactivity.hpp"
#include "protocol/msgactivity.hpp"

class Role;
class RoleModuleManager;
class RoleActivity;

class RoleActivityManager
{
public:
	void* operator new(size_t c);
	void operator delete(void* m);

	RoleActivityManager();
	~RoleActivityManager();

	void SetRole(RoleModuleManager *mgr);

	void Init(RoleModuleManager *mgr, const RoleActivityCommonParam &p, const RandActivityRoleParamList & data_list);
	void GetInitParam(RoleActivityCommonParam *p, RandActivityRoleParamList * data_list, bool is_logout);

	void OnLogin();
	void Update(time_t now_second);
	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnMonthChange();
	void OnRoleLevelUp(int old_level, int to_level);
	void OnAddChongZhi(long long chongzhi);
	void OnNewBuyGiftAddChongZhi(long long chongzhi);
	void OnConsumeGold(long long consume_gold);
	long long GetDayChongZhiNum() { return m_common_param.chongzhi_num; }

	void CheckInitRandActivity(int rand_act_type, bool is_force = false);

	void OnActivityEnterReq(Protocol::CSActivityEnterReq * aer);
	void OnActivityEnterReq2(int activity_type, int param1 = 0);

	void OnRandActivityOpReq(Protocol::CSRandActivityOperaReq* req);
	void OnChongzhiOperateReq(Protocol::CSChongzhiOperateReq* req);
	void OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq* req);

	//���⽱��
	void OnRandActivityBonusOpReq(Protocol::CSRandActivityBonusOperaReq* req);
	void OnFetchBonus(Protocol::CSRandActivityBonusOperaReq * req);
	void OnCheckBonusConfig(int version_id);
	void SetBonusFlag(int seq, bool is_set);
	void SendBonusInfo();

	bool JoinActivityInCross(int activity_type, int limit_level, bool is_limit_guild = false);

	void OnSuccsGather(int gather_id, int seq_id, const Posi & gather_pos); 

	//���ܿ���ֵ
	int OnBuyGiftItemChongZhi(int chongzhi_num, int ARG_OUT * price);

	// ��ֵ���
	void CheckChongzhiExtraRewardByNum(int chongzhi_num);
	void CheckChongzhiExtraRewardByFlag(int chongzhi_flag, int spid);
	void OnSendNewChongZhiReloadNotice();

	void OnRAOpenOnlineSend(int activity_type);
	void OnRAClose(int activity_type);

	void SendAllInfo();

	RoleActivityCommonParam & GetCommonParam() { return m_common_param; }
	void ClearDirtyMark();
	RoleActivity * GetRoleActivity(int rand_activity_type);

	// �����Ƿֲ���ȥ��
	void OnGiveRoleActivityItem(int type, int boss_id = 0 , std::vector<ItemConfigData>* drop_list = NULL);
	// ---------------------------------���ֻ-------------------------------
	int OnFetchScoreReward(int seq);
	void CalcRoleCap();
	// ---------------------------------�¶�Ͷ��-------------------------------
	int OnMonthInvestmentOp(int type, int seq);
	void OnMonthInvestmentDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnCmdBuyMonthInvestmentCheck();
	int OnCmdBuyMonthInvestment(int chongzhi_gold, long long unique_id);
	// ----------------------------��ʱ�����--------------------------------
	void OnActivitySignUpReq(int seq);
	void OnPutOutSignUpReward(int activity_id);
	// 	�׳�
	void GetFirstChongzhiReward(int first_recharge_type);
	void OnFetchExclusiveReward(int type);
	void OnParticipateLimitActivity(int activity_type);
	void OnFirstBuyItemCheck(int first_recharge_cmd_type, int cfg_ver);
	int OnFirstCmdBuyItem(int chong_zhi_gold, long long unique_id, int first_recharge_cmd_type);
	void OnFirstChongzhiDayChange();
	void SendFristChongzhiReward();
	int OnFirstCmdBuyGiftItem(int first_recharge_cmd_type, ARG_OUT int * price);


	//�¿�,�ܿ�
	int OnMonthBuyGiftItem(int param, int type, ARG_OUT int * price);
	void OnBuyMonthShopItem(int seq);
	void OnFetchMonthDayReward(int card_type);
	bool OnBuyMonthCheck(int req_type, int param1, int cfg_ver, long long ARG_OUT & unique_key);
	int OnBuyCmdMonthCard(int chong_zhi_gold, long long unique_id);
	void OnMonthCardDayChange();
	void OnMonthCardWeekChange();
	void OnSendMonthCardInfo();
	bool HoldMonthCard();

	void SendChongZhiInfo();
private:
	void FetchFanliReward(int reward_seq);

	void SendScoreRewardInfo();

	int OnRAMonthInvestmentFetch(int seq);
	void SendRAMonthInvestmentInfo();
	int OnRABuyMonthInvestment();
	void SendActivitySignUpInfo();
	
	// � -----------------------
	// ��һ�а�
	bool OnActivityEnterOnlyFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross);
	// С�Ӿ���
	bool OnActivityEnterTeamFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross);
	// ������Ҷ�
	bool OnActivityEnterGuildFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross);
	// �������
	bool OnActivityEnterGuildAnswer(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross);
	// Ӣ�ۻ���
	bool OnActivityEnterWorldTeamArena(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross);

	void CheckRACompensation();
private:

	////////////////////////////////////////////
	// ��
	RoleModuleManager * m_role_module_mgr;
	RoleActivityCommonParam m_common_param;

	RoleActivity * m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MAX];

};

#endif // __ROLE_ACTIVITY_MANAGER_HPP__




