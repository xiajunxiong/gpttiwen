#ifndef __OFFLINE_REGISTER_MANAGER_HPP__
#define __OFFLINE_REGISTER_MANAGER_HPP__

#include <map>
#include <vector>

#include "servercommon/serverdef.h"

#include "servercommon/servercommon.h"
#include "servercommon/struct/global/offlineregisterparam.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include <functional>

class OfflineRegister;
class Role;

static const int MAX_SAVE_OFFLINE_REGISTER_INTERVAL = 20;		// 回档时间
static const int MAX_REMOVE_OFFLINE_REGISTER_INTERVAL = 30;		// Remove时间

enum OFFLINE_REGISTER_DATA_STATE
{				
	OFFLINE_REGISTER_STATE_INVALID = 0,							// 初始状态
	OFFLINE_REGISTER_STATE_LOADING,								// 数据加载中 
	OFFLINE_REGISTER_STATE_LOAD_FINISH,							// 数据加载完成 
	OFFLINE_REGISTER_DATA_STATE_MAX
};

class OfflineRegisterManager
{
	friend class RMIInitOfflineRegisterBackObjectImpl;

public:

	static OfflineRegisterManager & Instance();

	void OnServerStart();
	void OnServerStop();

	bool IsLoadFinish();

	OfflineRegister * GetOfflineRegister(const UserID &user_id);
	void Update(unsigned long interval, time_t now_second);

	void OnDivorceReq(const UserID &target_user_id, unsigned int last_marry_time);
	void OnTitleSendReq(const UserID &target_user_id, int title_id , unsigned int _dec_times = 0);
	void OnUserFirstEnterScene(Role *user);
	void OnUserKickedOutGuild(const UserID & target_user_id);
	void OnColosseumNewReport(const UserID& target_user_id);

	void OnUserKickedOutChatGroup(const UserID & target_user_id);

	void OnShiTuBreakRelationReq(int role_id, int type, int master_apprentice_uid);
	void OnAddGuildGongXian(int role_id, int add_num, const char* reason);
	void OnChongzhiCmd(const UserID &target_user_id, int chongzhi_num);

	void OnTrademarketFinish(int uid, int add_num, ItemID item_id);    //成就交易行(售卖者成就次数)

	void OnForbidTalk(const UserID& target_user_id, int forbid_talk_seconds, int forbid_talk_type);
	void OnRetrieve(const UserID &target_user_id, Int64 unbind_gold, Int64 bind_gold, Int64 bind_coin, Int64 spirit_jade, int vip_exp, ItemID item_id, short item_num, short retrieve_reason);
	void OnRetrieveOtherMoney(const UserID &target_user_id, MoneyOtherParam retrieve_other_currency, short retrieve_reason);

	void OnRandActivityEnd(const UserID & user_id, int rand_type);

	void OnGladiatorGetEveryDayReward(const UserID & user_id, int rank_pos);

	void OnLimitTimeActivityEnd(const UserID & user_id, int type);							//封神战令 活动结束设置完成限时活动次数
	void OnGuildBossChapterBaodiReward(const UserID & user_id, unsigned int week_id, int baodi_flag);		//玩家已领取家族试炼保底奖励
	void OnGuildBossChapterDamageRankReward(const UserID & user_id, unsigned int day_id);	//玩家已领取家族试炼伤害排名奖励
	void OnGuildGatherDreamPublishFlag(const UserID & user_id);		//玩家发布集愿
	void OnHuanJieRankScoreChange(const UserID & user_id, int rank_score, int is_finish_ding_ji);	//幻界战场排位分变化
	void OnRetrievePet(const UserID& user_id, int cmd_id, int pet_id, long long item_unique_id);
	void OnGuildFightFetchJoinReward(const UserID & user_id, unsigned int guild_fight_fetch_reward_zero_timestamp);	//领取家族大乱斗参与奖励当天晚上0点时间戳
	void OnOnlyFightFetchJoinReward(const UserID & user_id, unsigned int only_fight_fetch_reward_zero_timestamp);	//领取以一敌百参与奖励当天晚上0点时间戳
	void OnTeamFightFetchJoinReward(const UserID & user_id, unsigned int team_fight_fetch_reward_zero_timestamp);	//领取竞技对抗赛参与奖励当天晚上0点时间戳

	void OnRetrieveExp(int role_id, long long exp);
	void OnTradeMarketBuySucc(int role_id, ItemID item_id, int consumed_gold);
	void OnCrossGroupCreateSucc(int role_id, long long group_id, int invalid_index);
	void OnTempGroupBecomeOfficialSucc(int role_id, long long group_id);

	void OnTradeCreditAddCredit(int role_id, int credit_num);
	void OnCloudArenaAddArenaCoin(int role_id, int add_num);
	void OnNormalCrystalFBGetBossReward(int role_id, int seq);

	void AddHongBaoTimes(const UserID & _target_user_id, int _hongbao_type, int _times);
	void AddHongBaoSilverCoin(const UserID & _target_user_id, int _val);

	void OnSetRAPaiMaiHangLastBidInfo(const UserID user_id, int seq, int bid_price);
	void OnSetDuelFight(const UserID user_id, int duel_type, int is_attack, int is_win, int level_diff);
	void OnSetGuildFightRank(const UserID & user_id, int guild_fight_rank);
	void OnSetGuildHonorRank(const UserID & user_id, int guild_honor_rank);

	void OnWinChiefElection(const UserID & user_id, int prof);
	void OnJieYiRoleChange(const UserID & user_id, int join_jie_yi_day);
	void OnReceviceFlower(const UserID & user_id);

	void AddCowReportGoodNewsReturnGoldOfflineEvent(const UserID & _target_user_id, int _val);
	void AddTigerBringsBlessingReturnGoldOfflineEvent(const UserID & _target_user_id, int _val);	// 寅虎纳福

	void OnColosseumHistoryRankReward(const UserID& user_id, int add_num);
	
	void AddFriendOfflineEvent(const UserID & _target_user_id , int _req);

	void OnQingYuanShiZhuangBuyMark(int role_id, int buy_mark);
	void OnQingYuanLeiChongGold(int role_id, int chongzhi_gold);

	void OnSetWorldTeamArenaInfo(const UserID &target_user_id, int team_rank, unsigned int timestamp, int * title_id = NULL);
	void OnSetLifeSkillChangeCard(const UserID &target_user_id, int fight_change_card_seq);
	void AddFriendIntimacy(const UserID &target_user_id, int _add_value);

	int OnCmdChangeRoleName(const UserID &user_id, GameName new_name, bool is_cost);
	void OnSetHorcruxTianCheng(const UserID &user_id, int times, int floor_back);
	void OnSetJiXingGaoZhao(const UserID &user_id, int times, int floor_back);

	bool OnCmdSetAuthorityType(int uid, int effect_type, int effect_param, int effect_param1);    //成就交易行(售卖者成就次数)
	void OnUserCloudArenaUseTimes(const UserID& user_id);
private:
	OfflineRegisterManager();
	~OfflineRegisterManager();

	OfflineRegister * CreateOfflineRegister(const UserID &user_id);

	bool LoadOfflineRegister(long long offline_reg_id_from);
	void LoadOfflineRegisterSucc();

	bool OnOfflineRegisterChange(OfflineRegister *offline_reg, int change_state);
	bool AddOfflineRegister(const UserID &user_id, const OfflineRegisterParam::OfflineRegisterAttr &offline_reg_attr);
	bool CheckSaveOfflineRegister(time_t now_second);

	OfflineRegisterParam m_change_param; 
	int m_data_state;
	time_t m_last_save_time; 
	time_t m_last_remove_time; 

	typedef std::map<UserID, OfflineRegister*> OfflineRegisterMap; 
	typedef std::map<UserID, OfflineRegister*>::iterator OfflineRegisterMapIt;
	OfflineRegisterMap m_offline_register_map;
};

#endif // __OFFLINE_REGISTER_MANAGER_HPP__

