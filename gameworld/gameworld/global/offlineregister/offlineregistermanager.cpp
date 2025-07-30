#include "offlineregistermanager.hpp"
#include "offlineregister.hpp"

#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "servercommon/struct/structcommon.h"
#include "gameworld/gameworld/gamelog.h"
#include "other/rolemodulemanager.hpp"
#include "other/marriage/marriage.hpp"
#include "other/mentor/shitu.hpp"
#include "other/title/title.hpp"
#include "other/event/eventhandler.hpp"
#include "item/money.h"
#include "globalconfig/globalconfig.h"
#include "config/activityconfig/chongzhiconfig.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "config/logicconfigmanager.hpp"
#include "gameworld/protocol/msgguild.hpp"
#include "gameworld/global/guild/guildmanager.hpp"
#include "item/knapsack.h"
#include "protocol/msgcolosseum.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitysports.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "chat/chatmanager.h"
#include "servercommon/struct/system/cmdresult.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "chat/crosschatgroupmanager.h"
#include "other/trade_credit/trade_credit.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/impl/roleactivitypaimaihang.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/duel/duel.hpp"
#include "servercommon/dueldef.h"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "friend/friendmanager.h"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "other/duel/duelconfig.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanleichong.hpp"
#include "servercommon/noticenum.h"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"

OfflineRegisterManager::OfflineRegisterManager()
	: m_data_state(OFFLINE_REGISTER_STATE_INVALID), m_last_save_time(0), m_last_remove_time(0)
{
	memset(&m_change_param, 0, sizeof(m_change_param));	
}

OfflineRegisterManager::~OfflineRegisterManager()
{
	for (OfflineRegisterMapIt it = m_offline_register_map.begin(), end = m_offline_register_map.end(); it != end; ++ it)
	{
		delete it->second;
	}

	m_offline_register_map.clear();
}

void OfflineRegisterManager::OnServerStart()
{
	this->LoadOfflineRegister(0);	
}

void OfflineRegisterManager::OnServerStop()
{
	this->CheckSaveOfflineRegister(EngineAdapter::Instance().Time() + MAX_SAVE_OFFLINE_REGISTER_INTERVAL * 2); 
}

void OfflineRegisterManager::Update(unsigned long interval, time_t now_second)
{
	if (OFFLINE_REGISTER_STATE_LOAD_FINISH != m_data_state) return;

	{
		if (now_second >= m_last_remove_time + MAX_REMOVE_OFFLINE_REGISTER_INTERVAL)
		{
			for (OfflineRegisterMapIt it = m_offline_register_map.begin(); it != m_offline_register_map.end();)
			{
				if (it->second->CanRemove(static_cast<unsigned int>(now_second)))
				{
					this->OnOfflineRegisterChange(it->second, structcommon::CS_DELETE);

					delete it->second;
					m_offline_register_map.erase(it++);
				}
				else 
				{
					++ it;
				}
			}

			m_last_remove_time = now_second;
		}
	}
	
	{
		if (now_second >= m_last_save_time + MAX_SAVE_OFFLINE_REGISTER_INTERVAL)
		{
			this->CheckSaveOfflineRegister(now_second);
			m_last_save_time = now_second;
		}
	}
}


void OfflineRegisterManager::OnDivorceReq(const UserID &target_user_id, unsigned int last_divorce_time)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetLastDivorceTime(last_divorce_time);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnTitleSendReq(const UserID & target_user_id, int title_id, unsigned int _dec_times )
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetTitleObtain(title_id);
	offline_reg->SetTitleDecTimes(_dec_times);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnUserFirstEnterScene(Role *user)
{
	if (NULL == user) return;

	UserID user_id = user->GetUserId();

	OfflineRegister *offline_register = this->GetOfflineRegister(user_id);
	if (NULL == offline_register) return;

	if (0 != offline_register->GetLastDivorceTime())
	{
		user->GetRoleModuleManager()->GetMarriage()->OnDivorce(offline_register->GetLastDivorceTime());
		offline_register->SetLastDivorceTime(0);
	}

	const OfflineRegisterParam::OfflineRegisterOtherParam & other_param = offline_register->GetOtherParam();

	{
		for (int i = 0; i < SHITU_MAX_APPRENTICE_COUNT + SHITU_MAX_MASTER_COUNT; ++i)
		{
			if (other_param.shi_tu_break_info[i].uid != 0)
			{
				gamelog::g_log_shi_tu.printf(LL_INFO, "%s %d role_id[%d %s] type:%d target_uid:%d",
					__FUNCTION__, __LINE__, user->GetUID(), user->GetName(), other_param.shi_tu_break_info[i].break_type, other_param.shi_tu_break_info[i].uid);

				user->GetRoleModuleManager()->GetShiTu()->DeleteRelation(other_param.shi_tu_break_info[i].break_type, other_param.shi_tu_break_info[i].uid);
			}
		}

		offline_register->ClearShiTuRelationBreakInfo();
	}

	if (0 != other_param.title_obtain)
	{
		user->GetRoleModuleManager()->GetTitle()->AddTitle(other_param.title_obtain, __FUNCTION__, other_param.title_times , TITLE_NOTICE_TYPE_NO_NOTICE);
		offline_register->SetTitleObtain(0);
	}

	if (0 != other_param.add_guild_gongxian)
	{
		user->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_param.add_guild_gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
		offline_register->ClearGuildGongXian();
	}

	if (0 != offline_register->GetChongzhiListCount())
	{
		int chongzhi_count = offline_register->GetChongzhiListCount();
		int chongzhi_flag = 0;

		int spid = user->GetClientShowSpid();
		if (0 == spid)
		{
			std::string plat_user_name = user->GetPlatName();
			spid = globalconfig::SpidConfig::GetSpidByStrPlatName(plat_user_name);
			user->SetClientShowSpid(spid);
		}

		for (int i = 0; i < chongzhi_count; ++i)
		{
			int chongzhi_num = offline_register->GetChongzhiNumByIndex(i);
			int seq = -1;
			if (chongzhi_num > 0)
			{
				const ChongzhiReward *reward_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetExtraRewardByNum(chongzhi_num, user->GetCurrencyType(), spid);
				if (NULL != reward_cfg)
				{
					chongzhi_flag |= (1 << reward_cfg->seq);
					seq = reward_cfg->seq;
				}

// 				// 单笔充值活动
// 				user->GetRoleActivity()->OnRADanbiChongzhiCheckChongzhi(chongzhi_num);
// 				user->GetRoleActivity()->OnRAFirstDoubleCharge(chongzhi_num);
// 				user->GetRoleActivity()->OnRADanbiChongzhiFanliPercent(chongzhi_num);
			}
			gamelog::g_log_rand_activity.printf(LL_INFO, "OfflineRegisterManager::OnUserFirstEnterScene user[%d,%s] check offline chongzhi list, chongzhi_num[%d] currency[%d] spid[%d] seq[%d]",
				user->GetUID(), user->GetName(), chongzhi_num, user->GetCurrencyType(), spid, seq);
		}
		user->GetRoleModuleManager()->GetRoleActivityManager()->CheckChongzhiExtraRewardByFlag(chongzhi_flag, spid);
		//user->GetRoleCrossRandActivity()->CheckChongzhiExtraRewardByFlag(chongzhi_flag);

		offline_register->ClearChongzhiList();
	}

	if (0 != offline_register->GetKickOutGuildTamp())
	{
		Protocol::SCQuitGuild info;
		info.reason = Protocol::SCQuitGuild::EXPEL_OUT;
		EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&info, sizeof(info));
		user->SetGuildID(0);
		user->SetGuildPostion(-1);
		GuildManager::Instance().ClearAllGuildTraining(user, offline_register->GetKickOutGuildTamp());
		offline_register->ClearKickOutGuildTamp();
	}

	if (0 != other_param.trademarket_finish || !offline_register->IsTrademarketItemEmpty())
	{
		EventHandler::Instance().OnTradeMarketOffline(user, other_param.trademarket_finish, other_param.sale_item_id);
		offline_register->ClearTrademarketNum();
	}

	if (-1 != other_param.forbid_talk_seconds)
	{
		user->ForbidTalk((time_t)other_param.forbid_talk_seconds, "Command", "Command", other_param.forbid_talk_type);
		offline_register->ClearForbidTalksSeconds();
	}

	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(other_param.other_currency) >= MAX_MONEY_TYPE_RESERVED);
	if (0 != other_param.retrieve_unbind_gold || 0 != other_param.retrieve_bind_coin || 0 != other_param.retrieve_item_id
		|| 0 != other_param.retrieve_item_num || -1 != other_param.retrieve_reason || 0 != other_param.other_currency[MONEY_TYPE_SILVER_COIN])
	{
		user->GetRoleModuleManager()->GetMoney()->RetrieveMoney(other_param.retrieve_unbind_gold, 0, other_param.retrieve_bind_coin, other_param.other_currency[MONEY_TYPE_SILVER_COIN], other_param.retrieve_reason);
		user->GetRoleModuleManager()->GetKnapsack()->RetrieveItem(other_param.retrieve_item_id, other_param.retrieve_item_num, other_param.retrieve_reason);
		offline_register->ClearRetrieveParam();
	}

	for (int i = 0; i < MONEY_TYPE_MAX && i < ARRAY_LENGTH(other_param.other_currency); ++i)
	{
		if (other_param.other_currency[i] != 0)
		{
			MoneyOtherParam retrieve_other_currency;
			UNSTD_STATIC_CHECK(sizeof(retrieve_other_currency.other_currency) <= sizeof(other_param.other_currency));
			memcpy(retrieve_other_currency.other_currency, other_param.other_currency, sizeof(retrieve_other_currency.other_currency));
			user->GetRoleModuleManager()->GetMoney()->RetrieveMoney(retrieve_other_currency, other_param.other_currency_retrieve_reason);
			offline_register->ClearRetrieveOtherMoneyParam();
			break;
		}
	}

	if (0 != other_param.colosseum_new_report_flag)
	{
		offline_register->ClearColosseumNewReportFlag();
		Protocol::SCColosseumNewReport msg;
		EngineAdapter::Instance().NetSend(user->GetNetId(), (const char*)&msg, sizeof(msg));
	}

	if (0 != other_param.rand_sports_end)
	{
		offline_register->ClearRandSportsEnd();

		RoleActivitySports * sports = static_cast<RoleActivitySports * >
			(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SPORTS));
		if (NULL != sports)
		{
			sports->OnRaSportsHistroyEnd();
		}	
	}

	if (0 != other_param.gladiator_everyday_count)
	{//先把数据覆盖到玩家上再清理
		CommonDataParam& com_data = user->GetRoleModuleManager()->GetCommonData();
		/*if(com_data.gladiator_everyday_reward_count >= OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST)
		{//原本就已经留存了3天了，直接替换
			com_data.gladiator_everyday_reward_count = other_param.gladiator_everyday_count;
			for (int i = 0; i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST && i < other_param.gladiator_everyday_count; i++)
			{
				//替换顶去最旧的，把新的排到最末尾
				for (int j = 0; j < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1; j++)
				{
					com_data.gladiator_everyday_reward_list[j] = com_data.gladiator_everyday_reward_list[j + 1];
				}
				com_data.gladiator_everyday_reward_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1] = other_param.gladiator_everyday_reward[i];
			}
		}
		else
		{
			//没存留或者存留没满
			//进行插入操作
			for (int i = 0; i < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST && i < other_param.gladiator_everyday_count;i++)
			{
				if (com_data.gladiator_everyday_reward_count >= OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST)
				{
					//替换顶去最旧的，把新的排到最末尾
					for (int j = 0; j < OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1; j++)
					{
						com_data.gladiator_everyday_reward_list[j] = com_data.gladiator_everyday_reward_list[j + 1];
					}
					com_data.gladiator_everyday_reward_list[OFFLINE_GLADIATOR_EVERYDAY_REWARD_MAX_LIST - 1] = other_param.gladiator_everyday_reward[i];
				}
				else
				{
					com_data.gladiator_everyday_reward_list[com_data.gladiator_everyday_reward_count++] = other_param.gladiator_everyday_reward[i];
				}
			}
		}*/
		com_data.gladiator_everyday_reward_count = other_param.gladiator_everyday_count;
		com_data.gladiator_everyday_reward_list[0] = other_param.gladiator_everyday_reward[0];
		offline_register->ClearGladiatorEveryDayReward();
	}

	if (0 != other_param.finish_limit_activity_count)
	{
		RoleActivityFortuneMaze * fortune_maze = static_cast<RoleActivityFortuneMaze *>
			(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_FORTUNE_MAZE));
		if (NULL != fortune_maze)
		{
			UNSTD_STATIC_CHECK(ACTIVITY_TYPE_MAX < BIT_COUNT_OF_BYTE * (int)sizeof(other_param.finish_limit_activity_count));
			for (int i = ACTIVITY_TYPE_WORLD_BOSS; i < ACTIVITY_TYPE_MAX; ++i)
			{
				if (0 != (other_param.finish_limit_activity_count & (1 << i)))
				{
					fortune_maze->JoinTimeLimitActivity();
				}
			}
		}

		offline_register->ClearLimitTimeActivtyCount();
	}

	if (0 != other_param.create_cross_group_id && 0 != other_param.invalid_index)
	{
		ChatManager::Instance().OnRoleCreateCrossGroupRet(other_param.create_cross_group_id, user->GetUID(), other_param.invalid_index);
		offline_register->ClearCrossChatGroupCreate();
	}

	if (0 != other_param.join_cross_group_id)
	{
		int empty_index = -1;
		if (user->GetRoleModuleManager()->FindEmptyCrossGroupIndex(&empty_index))
		{
			user->GetRoleModuleManager()->GetCommonData().cross_chat_group_list[empty_index] = other_param.join_cross_group_id;
			if (CrossConfig::Instance().IsHiddenServer())
			{
				CrossChatGroupManager::Instance().SendRoleOneGroupInfo(user, other_param.join_cross_group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);
			}
			else
			{
				ChatManager::Instance().SendRoleOneGroupInfo(user, other_param.join_cross_group_id, Protocol::SCRoleOneGroupInfo::REASON_ADD);
			}
		}

		offline_register->ClearJoinCrossChatGroup();
	}

	if (0 != other_param.guild_boss_baodi_reward_flag)
	{
		user->GetRoleModuleManager()->GetCommonData().guild_boss_baodi_reward_flag_OBSOLETE = other_param.guild_boss_baodi_reward_flag;
		user->GetRoleModuleManager()->GetCommonData().fetch_boss_chapter_baodi_reward_week_id = other_param.fetch_boss_chapter_baodi_reward_week_id;
		offline_register->ClearGuildBossChapterBaodiReward();
	}

	if (0 != other_param.is_fetch_guild_boss_chapter_damage_rank_reward)
	{
		user->GetRoleModuleManager()->GetCommonData().is_fetch_boss_chapter_damage_rank_reward = other_param.is_fetch_guild_boss_chapter_damage_rank_reward;
		user->GetRoleModuleManager()->GetCommonData().fetch_boss_chapter_rank_reward_day_id = other_param.fetch_boss_chapter_rank_reward_day_id;
		offline_register->ClearGuildBossChapterRankReward();
	}

	if (0 != other_param.today_role_guild_gather_dream_publish)
	{
		user->GetRoleModuleManager()->GetRoleGatherDream()->SetTodayPublishFlag(true);
		offline_register->ClearGuildGatherDreamPublishFlag();
	}

	if (0 != other_param.huan_jie_rank_score)
	{
		user->GetRoleModuleManager()->GetHuaShenManager()->SetRoleInfo(other_param.huan_jie_rank_score, other_param.is_finish_ding_ji);
		offline_register->ClearHuanJieRankScore();
	}

	if (0 != other_param.retrieve_pet_cmd_id)
	{
		std::string opresult;
		if (user->GetRoleModuleManager()->GetPet()->RetrievePet(other_param.retrieve_pet_id, other_param.retrieve_pet_unique_id))
		{
			opresult = "OfflineRegister Retrieve Pet Success";
		}
		else
		{
			opresult = "OfflineRegister Retrieve Pet FAILED!";
		}

		{
			static CmdResult result;
			result.count = 1;
			result.result_list[0].cmd_id = other_param.retrieve_pet_cmd_id;
			F_STRNCPY(result.result_list[0].opresult, opresult.c_str(), sizeof(result.result_list[0].opresult));

			RMIWriteCmdResultBackObjectImpl* impl = new RMIWriteCmdResultBackObjectImpl();
			RMISystemClient sc;
			sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
			sc.WriteCmdResult(RMISystemClient::CMD_TYPE_GLOBAL, impl, &result);
		}
		
		offline_register->ClearRetrievePetInfo();
	}

	if (0 != other_param.guild_fight_fetch_reward_zero_timestamp)
	{
		user->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT] = other_param.guild_fight_fetch_reward_zero_timestamp;
		user->GetRoleModuleManager()->GetCommonData().fetch_guild_fight_join_reward_flag = 1;
		offline_register->ClearGuildFightFetchRewardZeroTime();
	}

	if (0 != other_param.retrieve_exp)
	{
		user->GmRetrieveExp(other_param.retrieve_exp);
		offline_register->ClearRetrieveExpInfo();
	}

	if (0 != other_param.only_fight_fetch_reward_zero_timestamp)
	{
		user->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT] = other_param.only_fight_fetch_reward_zero_timestamp;
		user->GetRoleModuleManager()->GetCommonData().fetch_only_fight_join_reward_flag = 1;
		offline_register->ClearOnlyFightFetchRewardZeroTime();
	}

	if (0 != other_param.team_fight_fetch_reward_zero_timestamp)
	{
		user->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT] = other_param.team_fight_fetch_reward_zero_timestamp;
		user->GetRoleModuleManager()->GetCommonData().fetch_team_fight_join_reward_flag = 1;
		offline_register->ClearTeamFightFetchRewardZeroTime();
	}

	if (0 != other_param.trademarket_purchase_succ_consume_gold)
	{
		EventHandler::Instance().OnTradeMarketPurchase(user, other_param.trademarket_buy_item_id, other_param.trademarket_purchase_succ_consume_gold);
		offline_register->ClearTradeMarketBuyItemInfo();
	}

	if (0 != other_param.credit_num)
	{
		user->GetRoleModuleManager()->GetTradeCredit()->AddCredit(other_param.credit_num, __FUNCTION__);
		offline_register->ClearTradeCreditAddCredit();
	}

	if (0 != other_param.add_cloud_arena_coin_num)
	{
		user->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_param.add_cloud_arena_coin_num, MONEY_TYPE_CLOUD_ARENA_COIN, __FUNCTION__);
		offline_register->ClearCloudArenaCoin();
	}

	if (0 != other_param.add_colosseum_coin)
	{
		user->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_param.add_colosseum_coin, MONEY_TYPE_COLOSSEUM, __FUNCTION__);
		offline_register->ClearColosseumCoin();
	}

	if (0 != other_param.has_get_normal_crystal_fb_reward)
	{
		user->GetRoleModuleManager()->GetRoleCrystalFb()->OnRoleGetFBBossReward(other_param.normal_crystal_fb_reward_seq);
		offline_register->ClearUserGetNormalCrystalFBBossRewardFlag();
	}

	if (0 != other_param.hongbao_silver_coin)
	{
		user->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_param.hongbao_silver_coin, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
		user->GetRoleModuleManager()->OnFetchGuildHongBao();
		offline_register->ClearHongBaoSilverCoin();
	}

	for (int i = 0; i < HONGBAO_TYPE_MAX && i < MAX_HONG_BAO_TYPE_COUNT; ++i)
	{
		if (other_param.hongbao_times[i] != 0)
		{
			user->GetRoleModuleManager()->AddHongBaoTimes(i, __FUNCTION__ , other_param.hongbao_times[i]);

			offline_register->ClearHongBaoTimes(i);
		}
	}

	RoleActivityPaiMaiHang * ra_pai_mai_hang = static_cast<RoleActivityPaiMaiHang *>
		(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL != ra_pai_mai_hang)
	{
		for (int i = 0; i < ARRAY_LENGTH(other_param.ra_last_bid_price); ++i)
		{
			if (0 != other_param.ra_last_bid_price[i])
			{
				ra_pai_mai_hang->OnSetLastBidPrice(i, other_param.ra_last_bid_price[i]);
			}
		}
		offline_register->ClearRAPaiMaiHangLastBidInfo();
	}

	if (DUEL_BATTLE_TYPE_BEGIN < other_param.duel_type && other_param.duel_type < DUEL_BATTLE_TYPE_END
		&& 0 <= other_param.duel_is_attack && other_param.duel_is_attack <= 1
		&& 0 <= other_param.duel_is_win && other_param.duel_is_win <= 1 && 0 <= other_param.duel_level_diff)
	{
		user->GetRoleModuleManager()->GetDuel()->OnBattleFinish(other_param.duel_type, other_param.duel_is_attack == 1 ? true : false, other_param.duel_is_win == 1 ? true : false, other_param.duel_level_diff, 0, LOGIC_CONFIG->GetDuelConfig()->GetOtherCfg().reduce_coin);
		offline_register->ClearDuelFight();
	}
	
	if (0 != other_param.max_guild_fight_rank)
	{
		user->GetRoleModuleManager()->OnSetGuildFightInfo(other_param.max_guild_fight_rank);
		offline_register->ClearGuildFightRank();
	}

	if (0 != other_param.max_guild_honor_rank)
	{
		user->GetRoleModuleManager()->OnSetGuildHonorInfo(other_param.max_guild_honor_rank);
		offline_register->ClearGuildHonorRank();
	}

	if (0 != other_param.win_chief_election_prof)
	{
		EventHandler::Instance().OnWinChiefElection(user, other_param.win_chief_election_prof);
		offline_register->ClearWinChiefElection();
	}
	
	if (0 != other_param.join_jie_yi_day)
	{
		EventHandler::Instance().OnJieYiRoleChnage(user, other_param.join_jie_yi_day);
		offline_register->ClearJoinJieYiDay();
	}

	if (0 != other_param.recevice_flower_count)
	{
		EventHandler::Instance().OnFetchFriendGift(user, other_param.recevice_flower_count);
		offline_register->ClearReceviceFlower();
	}

	if (other_param.cow_report_good_news_return_gold > 0)
	{
		CowReportGoodNewsManager::Instance().OnCowReportGoodNewsOfflineEventByLogin(user , other_param.cow_report_good_news_return_gold);
		offline_register->ClearCowReportGoodNewsReturnGoldOfflineEvent();
	}
		
	if (other_param.has_add_friend_event)
	{
		FriendManager::Instance().OnAddFriendOfflineEventByLogin(user);
		offline_register->ClearAddFriendOfflineEvent();
	}

	if (other_param.tiger_brings_blessing_return_gold > 0)
	{
		TigerBringsBlessingManager::Instance().OnTigerBringsBlessingOfflineEventByLogin(user, other_param.tiger_brings_blessing_return_gold);
		offline_register->ClearTigerBringsBlessingReturnGoldOfflineEvent();
	}

	if (other_param.qingyuan_shizhuang_buy_mark > 0)
	{
		RoleActivityQingYuanShiZhuang * qingyuan_shizhuang = static_cast<RoleActivityQingYuanShiZhuang *>
			(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG));
		if (NULL != qingyuan_shizhuang)
		{
			qingyuan_shizhuang->OnLiHunBuFa(other_param.qingyuan_shizhuang_buy_mark);
		}
		offline_register->ClearQingYuanShiZhuangBuyMark();
	}
	if (other_param.qingyuan_leichong_gold > 0)
	{
		RoleActivityQingYuanLeiChong * role_activity = static_cast<RoleActivityQingYuanLeiChong *>
			(user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG));
		if (NULL != role_activity)
		{
			role_activity->OnLiHunBuFa(other_param.qingyuan_leichong_gold, true);
		}
		offline_register->ClearQingYuanLeiChongGold();
	}

	if (0 != other_param.world_team_arena_title_id || 0 != other_param.world_team_arena_rank)
	{
		user->GetRoleModuleManager()->OnSetWorldTeamArenaInfo(other_param.world_team_arena_rank, other_param.world_team_arena_timestamp, 
			(other_param.world_team_arena_title_id > 0) ? &other_param.world_team_arena_title_id : NULL);
		user->GetRoleModuleManager()->OnSetWorldTeamArenaTimestamp(other_param.world_team_arena_timestamp);
		offline_register->ClearWorldTeamArenaInfo();
	}

	if (0 != other_param.fight_change_card_seq)
	{
		user->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRoleBattlePerformSkill(other_param.fight_change_card_seq);
		offline_register->ClearLifeSkillChangeCard();
	}

	if (0 != other_param.new_name_flag)
	{
		GameName new_name;
		memset(new_name, 0, sizeof(new_name));
		F_STRNCPY(new_name, other_param.new_name, sizeof(GameName));

		if (other_param.new_name_cost_flag == 1)
		{
			user->OnResetNameReq(new_name);
		}
		else
		{
			user->ResetName(new_name);
		}
		offline_register->ClearRoleChangeName();
	}

	if (-1 != other_param.effect_type)
	{
		if (other_param.effect_type >= Role::AUTHORITY_TYPE_INVALID || other_param.effect_type < Role::AUTHORITY_TYPE_MAX)
		{
			char old_auth = user->GetAuthorityType();

			user->SetAuthorityType(other_param.effect_param, true);

			user->Save();

			if (Role::AUTHORITY_TYPE_GUIDER != old_auth && Role::AUTHORITY_TYPE_GUIDER == other_param.effect_param)
			{
				user->NoticeNum(noticenum::NT_NEWHAND_GUILD_SUCC);
			}

			{
				//Protocol::SCRoleAuthority ra;
				//ra.obj_id = m_obj_id;
				//ra.authority_type = m_authority_type;
				//EngineAdapter::Instance().NetSend(m_netid, (const char*)&ra, sizeof(ra));
			}

			//TODO this->OnRoleBaseInfoChange();
			user->SendRoleAuthorityInfo();
			ROLE_LOG_QUICK6(LOG_TYPE_CHANGE_AUTHORITY_TYPE, user, old_auth, other_param.effect_param, NULL, NULL);
		}

		offline_register->ClearAuthorityType();
	}

	if (0 != other_param.horcrux_tian_cheng_times)
	{
		RoleActivityHorcruxTianCheng * role_activity = (RoleActivityHorcruxTianCheng *)user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
		if (NULL != role_activity)
		{
			role_activity->OnAddRandTimes(other_param.horcrux_tian_cheng_times);
			if (other_param.floor_back != 0)
			{
				//role_activity->ClearRandTimes();
			}
		}

		offline_register->ClearHorcruxTianCheng();
	}

	if (0 != other_param.jixing_gaozhao_floor_back)
	{
		RoleActivityJiXingGaoZhao * role_activity = (RoleActivityJiXingGaoZhao *)user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
		if (NULL != role_activity)
		{
			role_activity->OnAddRandTimes(other_param.jixing_gaozhao_times);
			if (other_param.floor_back != 0)
			{
				//role_activity->ClearRandTimes();
			}
		}

		offline_register->ClearJiXingGaoZhao();
	}

	if (0 != other_param.add_value)
	{
		RoleActivityCharismaticTanabata * activity = (RoleActivityCharismaticTanabata *)user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
		if (NULL != activity)
		{
			activity->OnRoleSendFlows(other_param.add_value);
		}
		offline_register->ClearFriendIntimacy();
	}

	if (other_param.cloud_arena_use_times_timestamp > 0u)
	{
		unsigned int zero_time = GetZeroTime(other_param.cloud_arena_use_times_timestamp);
		unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		if (now >= zero_time && now - zero_time < SECOND_PER_DAY)
		{
			// 只允许当天的次数找回
			EventHandler::Instance().OnCloudArenaUseTimes(user->GetUID());

			gamelog::g_log_cloud_arena.printf(LL_INFO, "OfflineRegisterManager::OnUserFirstEnterScene_1 user[%d]", UidToInt(user_id));
		}
		else
		{
			gamelog::g_log_cloud_arena.printf(LL_INFO, "OfflineRegisterManager::OnUserFirstEnterScene_2 user[%d]", UidToInt(user_id));
		}
		offline_register->ClearCloudArenaUseTimesTimestamp();
	}

	this->OnOfflineRegisterChange(offline_register, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnUserKickedOutGuild(const UserID & target_user_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->KickOutGuildTamp(EngineAdapter::Instance().Time());

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnColosseumNewReport(const UserID& target_user_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetColosseumNewReportFlag();

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnUserKickedOutChatGroup(const UserID & target_user_id)
{
}

void OfflineRegisterManager::OnShiTuBreakRelationReq(int role_id, int type, int master_apprentice_uid)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->AddShiTuRelationBreakInfo(master_apprentice_uid, type);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnAddGuildGongXian(int role_id, int add_num, const char* reason)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->AddGuildGongXian(add_num);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

OfflineRegister * OfflineRegisterManager::GetOfflineRegister(const UserID &user_id)
{
	OfflineRegisterMapIt it = m_offline_register_map.find(user_id);
	if (it != m_offline_register_map.end())
	{
		return it->second;
	}

	return NULL;
}

OfflineRegister * OfflineRegisterManager::CreateOfflineRegister(const UserID &user_id)
{
	OfflineRegister *offline_reg = this->GetOfflineRegister(user_id);
	if (NULL == offline_reg)
	{
		offline_reg = new OfflineRegister(user_id);
		m_offline_register_map[user_id] = offline_reg;

		this->OnOfflineRegisterChange(offline_reg, structcommon::CS_INSERT);
	}

	return offline_reg;
}

bool OfflineRegisterManager::LoadOfflineRegister(long long offline_reg_id_from)
{
	if (OFFLINE_REGISTER_STATE_LOAD_FINISH == m_data_state) return false;

	m_data_state = OFFLINE_REGISTER_STATE_LOADING;

	RMIInitOfflineRegisterBackObjectImpl *impl = new RMIInitOfflineRegisterBackObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.InitOfflineRegisterAsyn(offline_reg_id_from, impl);
}

void RMIInitOfflineRegisterBackObjectImpl::InitOfflineRegisterRet(int ret, const OfflineRegisterParam &offline_reg_param)
{
	if (ret < 0 || offline_reg_param.count > OfflineRegisterParam::MAX_OFFLINE_REGISTER_COUNT) 
	{
		printf(" RMIInitOfflineRegisterBackObjectImpl::InitOfflineRegisterRet Failed ret=%d, count=%d\n", ret, offline_reg_param.count);
		EngineAdapter::Instance().StopGame();
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < offline_reg_param.count; i++)
	{
		if (offline_reg_param.offline_register_list[i].id_offline_register > next_id_from)
		{
			next_id_from = offline_reg_param.offline_register_list[i].id_offline_register;
		}

		if (offline_reg_param.offline_register_list[i].role_id <= 0) continue;

		UserID user_id = IntToUid(offline_reg_param.offline_register_list[i].role_id);

		if (!OfflineRegisterManager::Instance().AddOfflineRegister(user_id, offline_reg_param.offline_register_list[i]))
		{
			gamelog::g_log_serious_error.printf(LL_ERROR, " RMIInitOfflineRegisterBackObjectImpl::InitOfflineRegisterRet AddOfflineRegister Failed, role_id[%d]", UidToInt(user_id));
			//EngineAdapter::Instance().StopGame(); // 数据重复可能由于深夜时服务器数据备份，但数据量太大导致有些数据入库失败（因为会锁表，不允许写操作），导致出现了这样数据重复的情况（没删成功，却又再次插入了数据）
			//return;
		}
	}

	if (offline_reg_param.count > 0)	
	{
		OfflineRegisterManager::Instance().LoadOfflineRegister(next_id_from);
	}
	else
	{
		OfflineRegisterManager::Instance().LoadOfflineRegisterSucc();
	}
}

bool OfflineRegisterManager::OnOfflineRegisterChange(OfflineRegister *offline_reg, int change_state)
{
	if (NULL == offline_reg || !this->CheckSaveOfflineRegister(EngineAdapter::Instance().Time())) return false;

	if (m_change_param.count < OfflineRegisterParam::MAX_OFFLINE_REGISTER_COUNT)
	{
		OfflineRegisterParam::OfflineRegisterAttr *offline_reg_attr = &m_change_param.offline_register_list[m_change_param.count];

		offline_reg_attr->change_state = change_state;
		offline_reg->GetAttr(offline_reg_attr);

		++ m_change_param.count;

		return true; 
	}

	return false;
}

bool OfflineRegisterManager::AddOfflineRegister(const UserID &user_id, const OfflineRegisterParam::OfflineRegisterAttr &offline_reg_attr)
{
	OfflineRegisterMapIt it = m_offline_register_map.find(user_id);
	if (it != m_offline_register_map.end()) return false;
	
	OfflineRegister *offline_reg = new OfflineRegister();
	offline_reg->Init(offline_reg_attr);

	m_offline_register_map[user_id] = offline_reg;
	
	return true;
}

void OfflineRegisterManager::LoadOfflineRegisterSucc()
{
	m_data_state = OFFLINE_REGISTER_STATE_LOAD_FINISH;
}

bool OfflineRegisterManager::CheckSaveOfflineRegister(time_t now_second)
{
	if (now_second >= m_last_save_time + MAX_SAVE_OFFLINE_REGISTER_INTERVAL || m_change_param.count >= OfflineRegisterParam::MAX_OFFLINE_REGISTER_COUNT) 
	{
		if (m_change_param.count > 0)
		{
			RMISaveOfflineRegisterBackObjectImpl *impl = new RMISaveOfflineRegisterBackObjectImpl();

			RMIGlobalClient gc;
			gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB()); 

			if (!gc.SaveOfflineRegisterAsyn(m_change_param, impl)) return false;

			m_change_param.count = 0;
		}
	}

	return true; 
}

bool OfflineRegisterManager::IsLoadFinish()
{
	return (OFFLINE_REGISTER_STATE_LOAD_FINISH == m_data_state);
}

OfflineRegisterManager & OfflineRegisterManager::Instance()
{
	static OfflineRegisterManager orm;
	return orm;
}

void OfflineRegisterManager::OnChongzhiCmd(const UserID &target_user_id, int chongzhi_num)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnChongzhiCmd(chongzhi_num);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);

	gamelog::g_log_rand_activity.printf(LL_INFO, "OfflineRegisterManager::OnChongzhiCmd user[%d] chongzhi_num[%d]",
		UidToInt(target_user_id), chongzhi_num);
}

void OfflineRegisterManager::OnTrademarketFinish(int uid, int add_num, ItemID item_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(uid));
	if (NULL == offline_reg) return;

	offline_reg->AddTrademarketNum(add_num, item_id);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnForbidTalk(const UserID & target_user_id, int forbid_talk_seconds, int forbid_talk_type)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetForbidTalkSeconds(forbid_talk_seconds, forbid_talk_type);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnRetrieve(const UserID &target_user_id, Int64 unbind_gold, Int64 bind_gold, Int64 bind_coin, Int64 spirit_jade, int vip_exp, ItemID item_id, short item_num, short retrieve_reason)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetRetrieveParam(unbind_gold, bind_gold, bind_coin, spirit_jade, vip_exp, item_id, item_num, retrieve_reason);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnRetrieveOtherMoney(const UserID & target_user_id, MoneyOtherParam retrieve_other_currency, short retrieve_reason)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetRetrieveOtherMoneyParam(retrieve_other_currency, retrieve_reason);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnRandActivityEnd(const UserID & user_id, int rand_type)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	if(rand_type == RAND_ACTIVITY_TYPE_SPORTS)offline_reg->SetRandSportsEnd();

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnGladiatorGetEveryDayReward(const UserID & user_id, int rank_pos)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetGladiatorEveryDayReward(rank_pos);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnLimitTimeActivityEnd(const UserID & user_id, int type)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetLimitTimeActivtyCount(type);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnGuildBossChapterBaodiReward(const UserID & user_id, unsigned int week_id, int baodi_flag)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetGuildBossChapterBaodiReward(week_id, baodi_flag);

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnGuildBossChapterDamageRankReward(const UserID & user_id, unsigned int day_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetGuildBossChapterDamageRankReward(day_id);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnGuildGatherDreamPublishFlag(const UserID & user_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetGuildGatherDreamPublishFlag();
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnHuanJieRankScoreChange(const UserID & user_id, int rank_score, int is_finish_ding_ji)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetHuanJieRankScore(rank_score, is_finish_ding_ji);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnRetrievePet(const UserID& user_id, int cmd_id, int pet_id, long long item_unique_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetRetrievePetInfo(cmd_id, pet_id, item_unique_id);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnGuildFightFetchJoinReward(const UserID & user_id, unsigned int guild_fight_fetch_reward_zero_timestamp)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetGuildFightFetchRewardZeroTime(guild_fight_fetch_reward_zero_timestamp);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnRetrieveExp(int role_id, long long exp)
{
	OfflineRegister * offline_reg = this->CreateOfflineRegister(role_id);
	if (NULL == offline_reg) return;

	offline_reg->AddRetrieveExp(exp);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnTradeMarketBuySucc(int role_id, ItemID item_id, int consumed_gold)
{
	OfflineRegister * offline_reg = this->CreateOfflineRegister(role_id);
	if (NULL == offline_reg) return;

	offline_reg->SetTradeMarketBuyItemInfo(item_id, consumed_gold);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnCrossGroupCreateSucc(int role_id, long long group_id, int invalid_index)
{
	OfflineRegister * offline_reg = this->CreateOfflineRegister(role_id);
	if (NULL == offline_reg) return;

	offline_reg->SetCrossChatGroupCreate(group_id, invalid_index);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnTempGroupBecomeOfficialSucc(int role_id, long long group_id)
{
	OfflineRegister * offline_reg = this->CreateOfflineRegister(role_id);
	if (NULL == offline_reg) return;

	offline_reg->SetJoinCrossChatGroup(group_id);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnTradeCreditAddCredit(int role_id, int credit_num)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->SetTradeCreditAddCredit(credit_num);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnCloudArenaAddArenaCoin(int role_id, int add_num)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->SetCloudArenaAddArenaCoin(add_num);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnNormalCrystalFBGetBossReward(int role_id, int seq)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->OnUserGetNormalCrystalFBBossReward(seq);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddHongBaoTimes(const UserID & _target_user_id, int _hongbao_type , int _times)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(_target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->AddHongBaoTimes(_hongbao_type , _times);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddHongBaoSilverCoin(const UserID & _target_user_id, int _val)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(_target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->AddHongBaoSilverCoin(_val);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetRAPaiMaiHangLastBidInfo(const UserID user_id, int seq, int bid_price)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetRAPaiMaiHangLastBidInfo(seq, bid_price);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddCowReportGoodNewsReturnGoldOfflineEvent(const UserID & _target_user_id, int _val)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(_target_user_id);
	if (NULL == offline_reg) return;

	if (0 == _val)
	{
		offline_reg->ClearCowReportGoodNewsReturnGoldOfflineEvent();
	}
	else if(_val > 0)
	{
		offline_reg->AddCowReportGoodNewsReturnGoldOfflineEvent(_val);
	}

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddTigerBringsBlessingReturnGoldOfflineEvent(const UserID & _target_user_id, int _val)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(_target_user_id);
	if (NULL == offline_reg) return;

	if (0 == _val)
	{
		offline_reg->ClearTigerBringsBlessingReturnGoldOfflineEvent();
	}
	else if (_val > 0)
	{
		offline_reg->AddTigerBringsBlessingReturnGoldOfflineEvent(_val);
	}

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetDuelFight(const UserID user_id, int duel_type, int is_attack, int is_win, int level_diff)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetDuelFight((char)duel_type, (char)is_attack, (char)is_win, (unsigned char)level_diff);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetGuildFightRank(const UserID & user_id, int guild_fight_rank)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetGuildFightRank(guild_fight_rank);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetGuildHonorRank(const UserID & user_id, int guild_honor_rank)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetGuildHonorRank(guild_honor_rank);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnWinChiefElection(const UserID & user_id, int prof)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetWinChiefElection(prof);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnJieYiRoleChange(const UserID & user_id, int join_jie_yi_day)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnSetJoinJieYiDay(join_jie_yi_day);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnReceviceFlower(const UserID & user_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->OnReceviceFlower();
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnColosseumHistoryRankReward(const UserID& user_id, int add_num)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->AddColosseumCoin(add_num);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddFriendOfflineEvent(const UserID & _target_user_id, int _req)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(_target_user_id);
	if (NULL == offline_reg) return;

	if (_req)
	{
		offline_reg->AddFriendOfflineEvent();
	}
	else
	{
		offline_reg->ClearAddFriendOfflineEvent();
	}

	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnQingYuanShiZhuangBuyMark(int role_id, int buy_mark)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->SetQingYuanShiZhuangBuyMark(buy_mark);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnQingYuanLeiChongGold(int role_id, int chongzhi_gold)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(role_id));
	if (NULL == offline_reg) return;

	offline_reg->SetQingYuanShiLeiChongGold(chongzhi_gold);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetWorldTeamArenaInfo(const UserID & target_user_id, int team_rank, unsigned int timestamp, int * title_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetWorldTeamArenaInfo(team_rank, timestamp, title_id);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnSetLifeSkillChangeCard(const UserID & target_user_id, int fight_change_card_seq)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetLifeSkillChangeCard(fight_change_card_seq);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::AddFriendIntimacy(const UserID & target_user_id, int _add_value)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(target_user_id);
	if (NULL == offline_reg) return;

	offline_reg->AddFriendIntimacy(_add_value);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

int OfflineRegisterManager::OnCmdChangeRoleName(const UserID & user_id, GameName new_name, bool is_cost)
{
	int ret = 0;
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg)
	{
		ret = -1;
		return ret;
	}

	offline_reg->SetRoleChangeName(new_name, is_cost);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
	return 0;
}

void OfflineRegisterManager::OnSetHorcruxTianCheng(const UserID & user_id, int times, int floor_back)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg)
	{
		return;
	}

	offline_reg->SetHorcruxTianCheng(times, floor_back);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
	return;
}

void OfflineRegisterManager::OnSetJiXingGaoZhao(const UserID & user_id, int times, int floor_back)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg)
	{
		return;
	}

	offline_reg->SetJiXingGaoZhao(times, floor_back);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
	return;
}

bool OfflineRegisterManager::OnCmdSetAuthorityType(int uid, int effect_type, int effect_param, int effect_param1)
{
	bool ret = false;

	OfflineRegister *offline_reg = this->CreateOfflineRegister(IntToUid(uid));
	if (NULL == offline_reg)
	{
		return false;
	}

	ret = offline_reg->SetAuthorityType(effect_type, effect_param, effect_param1);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
	return ret;
}

void OfflineRegisterManager::OnUserCloudArenaUseTimes(const UserID& user_id)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetCloudArenaUseTimesTimestamp(static_cast<unsigned int>(EngineAdapter::Instance().Time()));
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);

	gamelog::g_log_cloud_arena.printf(LL_INFO, "OfflineRegisterManager::OnUserCloudArenaUseTimes user[%d]", UidToInt(user_id));
}

void OfflineRegisterManager::OnOnlyFightFetchJoinReward(const UserID & user_id, unsigned int only_fight_fetch_reward_zero_timestamp)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetOnlyFightFetchRewardZeroTime(only_fight_fetch_reward_zero_timestamp);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

void OfflineRegisterManager::OnTeamFightFetchJoinReward(const UserID & user_id, unsigned int team_fight_fetch_reward_zero_timestamp)
{
	OfflineRegister *offline_reg = this->CreateOfflineRegister(user_id);
	if (NULL == offline_reg) return;

	offline_reg->SetTeamFightFetchRewardZeroTime(team_fight_fetch_reward_zero_timestamp);
	this->OnOfflineRegisterChange(offline_reg, structcommon::CS_UPDATE);
}

