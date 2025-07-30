#ifndef __OFFLINE_REGISTER_HPP__
#define __OFFLINE_REGISTER_HPP__

#include <stdlib.h>

#include "servercommon/serverdef.h"

#include "servercommon/struct/global/offlineregisterparam.hpp"

class OfflineRegister
{
public:
	OfflineRegister();
	OfflineRegister(const UserID &user_id);
	~OfflineRegister();

	void Init(const OfflineRegisterParam::OfflineRegisterAttr &attr);
	void GetAttr(OfflineRegisterParam::OfflineRegisterAttr *attr);

	const UserID & GetUserID() const { return m_user_id; }

	void SetLastDivorceTime(unsigned int divorce_time) { m_last_divorce_time = divorce_time; }
	unsigned int GetLastDivorceTime() { return m_last_divorce_time; }

	void SetTitleObtain(int title_id) { m_other_param.title_obtain = title_id; }
	void SetTitleDecTimes(unsigned int _dec_times) { m_other_param.title_times = _dec_times; }

	const OfflineRegisterParam::OfflineRegisterOtherParam & GetOtherParam() { return m_other_param; }

	void AddShiTuRelationBreakInfo(int master_apprentice_uid, int type);
	void ClearShiTuRelationBreakInfo();

	void AddGuildGongXian(long long add_num);
	void ClearGuildGongXian();

	bool IsTrademarketItemEmpty();
	void AddTrademarketNum(int add_num, ItemID item_id);
	void ClearTrademarketNum();

	int GetChongzhiListCount() { return m_other_param.chongzhi_count; }
	void OnChongzhiCmd(int chongzhi_num);
	int GetChongzhiNumByIndex(int index);
	void ClearChongzhiList();

	void KickOutGuildTamp(time_t now_second);
	time_t GetKickOutGuildTamp() { return m_other_param.kick_out_guild_tamp; }
	void ClearKickOutGuildTamp();

	void SetForbidTalkSeconds(int forbid_talk_seconds, int forbid_talk_type);
	int GetForbidTalkSeconds() const { return m_other_param.forbid_talk_seconds; }
	void ClearForbidTalksSeconds();

	void SetRetrieveParam(Int64 unbind_gold, Int64 bind_gold, Int64 bind_coin, Int64 spirit_jade, int vip_exp, ItemID item_id, short item_num, short retrieve_reason);
	void ClearRetrieveParam();
	void SetRetrieveOtherMoneyParam(MoneyOtherParam retrieve_other_currency, short retrieve_reason);
	void ClearRetrieveOtherMoneyParam();

	void SetColosseumNewReportFlag() { m_other_param.colosseum_new_report_flag = 1; }
	void ClearColosseumNewReportFlag() { m_other_param.colosseum_new_report_flag = 0; }
	bool HasColosseumNewReportFlag() const { return 1 == m_other_param.colosseum_new_report_flag; }

	void SetRandSportsEnd();
	bool GetRandSportsEnd() const { return m_other_param.rand_sports_end == 1; }
	void ClearRandSportsEnd() { m_other_param.rand_sports_end = 0; }

	void SetGladiatorEveryDayReward(int rank_pos);
	const int * GetGladiatorEveryDayReward(int *count);
	void ClearGladiatorEveryDayReward();

	void SetCrossChatGroupCreate(long long group_id,int invalid_index);
	bool GetCrossChatGroupCreate(long long *group_id,int *invalid_index);
	void ClearCrossChatGroupCreate();

	void SetJoinCrossChatGroup(long long group_id);
	void ClearJoinCrossChatGroup();

	void SetLimitTimeActivtyCount(int type);
	void ClearLimitTimeActivtyCount();

	void SetGuildBossChapterBaodiReward(unsigned int day_id, int baodi_flag);
	void ClearGuildBossChapterBaodiReward();
	void SetGuildBossChapterDamageRankReward(unsigned int day_id);
	void ClearGuildBossChapterRankReward();

	void SetGuildGatherDreamPublishFlag();
	void ClearGuildGatherDreamPublishFlag();

	void SetHuanJieRankScore(int rank_score, int is_finish_ding_ji);
	void ClearHuanJieRankScore();

	void SetRetrievePetInfo(int cmd_id, int pet_id, long long item_unique_id);
	void ClearRetrievePetInfo();

	void SetGuildFightFetchRewardZeroTime(unsigned int zero_timstamp);
	void ClearGuildFightFetchRewardZeroTime();

	void AddRetrieveExp(long long retrieve_exp);
	void ClearRetrieveExpInfo();

	void SetOnlyFightFetchRewardZeroTime(unsigned int zero_timstamp);
	void ClearOnlyFightFetchRewardZeroTime();

	void SetTeamFightFetchRewardZeroTime(unsigned int zero_timstamp);
	void ClearTeamFightFetchRewardZeroTime();

	void SetTradeMarketBuyItemInfo(ItemID item_id, int consume_gold);
	void ClearTradeMarketBuyItemInfo();

	void SetTradeCreditAddCredit(int credit_num);
	void ClearTradeCreditAddCredit();

	void SetCloudArenaAddArenaCoin(int add_num);
	void ClearCloudArenaCoin();

	void AddColosseumCoin(int add_num);
	void ClearColosseumCoin();

	void OnUserGetNormalCrystalFBBossReward(int seq);
	void ClearUserGetNormalCrystalFBBossRewardFlag();

	void AddHongBaoTimes(int _hongbao_type, int _times);
	int GetHongBaoTimes(int _hongbao_type);
	void ClearHongBaoTimes(int _hongbao_type);

	void AddHongBaoSilverCoin(int _val);
	int GetHongBaoSilverCoin();
	void ClearHongBaoSilverCoin();

	void OnSetRAPaiMaiHangLastBidInfo(int seq, int bid_price);
	void ClearRAPaiMaiHangLastBidInfo();

	void OnSetDuelFight(char duel_type, char is_attack, char is_win, unsigned char level_diff);
	void ClearDuelFight();

	void OnSetGuildFightRank(int guild_fight_rank);
	void ClearGuildFightRank();
	void OnSetGuildHonorRank(int guild_honor_rank);
	void ClearGuildHonorRank();

	void OnSetWinChiefElection(int prof);
	void ClearWinChiefElection();
	void OnSetJoinJieYiDay(int join_day);
	void ClearJoinJieYiDay();

	void OnReceviceFlower();
	void ClearReceviceFlower();

	void AddCowReportGoodNewsReturnGoldOfflineEvent(int _val);
	void ClearCowReportGoodNewsReturnGoldOfflineEvent();
	
	void AddFriendOfflineEvent();
	void ClearAddFriendOfflineEvent();

	void AddTigerBringsBlessingReturnGoldOfflineEvent(int _val);
	void ClearTigerBringsBlessingReturnGoldOfflineEvent();

	void SetQingYuanShiZhuangBuyMark(int buy_mark);
	void ClearQingYuanShiZhuangBuyMark();

	void SetQingYuanShiLeiChongGold(int chongzhi_gold);
	void ClearQingYuanLeiChongGold();

	void SetWorldTeamArenaInfo(int team_rank, unsigned int timestamp, int * title_id = NULL);
	void ClearWorldTeamArenaInfo();
	
	void SetCloudArenaUseTimesTimestamp(unsigned int timestamp);
	void ClearCloudArenaUseTimesTimestamp();

	void SetLifeSkillChangeCard(int fight_change_card_seq);
	void ClearLifeSkillChangeCard();

	void SetRoleChangeName(GameName new_name, bool is_cost);
	void ClearRoleChangeName();

	bool SetAuthorityType(int effect_type, int effect_param, int effect_param1);
	void ClearAuthorityType();

	bool SetHorcruxTianCheng(int times, int floor_back);
	void ClearHorcruxTianCheng();

	bool SetJiXingGaoZhao(int times, int floor_back);
	void ClearJiXingGaoZhao();

	void AddFriendIntimacy(int _add_value);
	void ClearFriendIntimacy();

	bool CanRemove(unsigned int now_second);

	void * operator new(size_t c);
	void operator delete(void *m);

private:
	UserID m_user_id;

	unsigned int m_last_divorce_time;					// 上次离婚时间

	int m_chongzhi_cmd_flag;							// 充值命令标记

	OfflineRegisterParam::OfflineRegisterOtherParam m_other_param;
};

#endif // __OFFLINE_REGISTER_HPP__

