#ifndef __DA_GLOBAL_H__
#define __DA_GLOBAL_H__

#include "damodule.h"

namespace dataaccessrmi
{
	namespace global
	{
		static const char * CREATE_GUILD = "CreateGuild";
		static const char * DESTROY_GUILD = "DestroyGuild";
		static const char * INIT_GUILD = "InitGuild";
		static const char * SAVE_GUILD = "SaveGuild";

		static const char *INIT_MAILS = "InitMails";
		static const char *SAVE_MAILS = "SaveMails";
		static const char *GET_SYSTEM_MAILS = "GetSystemMails";
		static const char *REMOVE_SYSTEM_MAILS = "RemoveSystemMails";

		static const char *INIT_PERSON_RANK = "InitPersonRank";
		static const char *SAVE_PERSON_RANK = "SavePersonRank";

		static const char *INIT_TEAM_RANK = "InitTeamRank";
		static const char *SAVE_TEAM_RANK = "SaveTeamRank";

		static const char *INIT_WORLDSTATUS = "InitWorldStatus";
		static const char *SAVE_WORLDSTATUS = "SaveWorldStatus";
	
		static const char *INIT_OFFLINE_REGISTER = "InitOfflineRegister";
		static const char *SAVE_OFFLINE_REGISTER = "SaveOfflineRegister";

		static const char *INIT_CHALLENGE_USER = "InitChallengeUser";
		static const char *SAVE_CHALLENGE_USER = "SaveChallengeUser";

		static const char *HEARTBEAT_CHECK = "HeartbeatCheck";

		static const char *INIT_BABY_DATA = "InitBabyData";
		static const char *SAVE_BABY_DATA = "SaveBabyData";

		static const char* INIT_ACTIVITY_DATA = "InitActivityData";
		static const char* SAVE_ACTIVITY_DATA = "SaveActivityData";

		static const char* INIT_TRADE_MARKET_DATA = "InitTradeMarketData";
		static const char* SAVE_TRADE_MARKET_DATA = "SaveTradeMarketData";

		static const char* INIT_TRADE_MARKET_USER_RECORD = "InitTradeMarketUserRecord";
		static const char* SAVE_TRADE_MARKET_USER_RECORD = "SaveTradeMarketUserRecord";

		static const char* INIT_TRADE_MARKET_SYSTEM_RECORD = "InitTradeMarketSystemRecord";
		static const char* SAVE_TRADE_MARKET_SYSTEM_RECORD = "SaveTradeMarketSystemRecord";

		static const char* INIT_TRADE_MARKET_SYSTEM_RECYCLE_RULE = "InitTradeMarketSystemRecycleRule";
		static const char* SAVE_TRADE_MARKET_SYSTEM_RECYCLE_RULE = "SaveTradeMarketSystemRecycleRule";

		static const char* INIT_SYSTEM_AUTO_UPSHELVE_DATA = "InitSystemAutoUpshelveData";
		static const char* SAVE_SYSTEM_AUTO_UPSHELVE_DATA = "SaveSystemAutoUpshelveData";

		static const char *INIT_ACTIVITY_ROLE_DATA= "InitActivityRoleData";
		static const char *SAVE_ACTIVITY_ROLE_DATA = "SaveActivityRoleData";

		static const char * INIT_COMMON_DATA = "InitCommonData";
		static const char * SAVE_COMMON_DATA = "SaveCommonData";

		static const char *INIT_PLATFORM_BATTLE_DATA = "InitPlatformBattleData";
		static const char *SAVE_PLATFORM_BATTLE_DATA = "SavePlatformBattleData";

		static const char *INIT_SERVER_FIRST_KILL_DATA = "InitServerFirstKillData";
		static const char *SAVE_SERVER_FIRST_KILL_DATA = "SaveServerFirstKillData";

		static const char *INIT_ANIMAL_RACE_DATA = "InitAnimalRaceData";
		static const char *SAVE_ANIMAL_RACE_DATA = "SaveAnimalRaceData";

		static const char *INIT_ORIGIN_USERCACHE_DATA = "InitOriginUserCacheData";
		
		static const char *INIT_CHAT_GROUP_DATA = "InitChatGroupData";
		static const char *SAVE_CHAT_GROUP_DATA = "SaveChatGroupData";

		static const char *INIT_CROSS_CHAT_GROUP_DATA = "InitCrossChatGroupData";
		static const char *SAVE_CROSS_CHAT_GROUP_DATA = "SaveCrossChatGroupData";

		static const char *SAVE_PET_RANK = "SavePetRank";
		static const char *INIT_PET_RANK = "InitPetRank";

		static const char *SAVE_PARTNER_RANK = "SavePartnerRank";
		static const char *INIT_PARTNER_RANK = "InitPartnerRank";
	
		static const char *INIT_BATTLE_VIDEO_DATA = "InitBattleVideoData";
		static const char *SAVE_BATTLE_VIDEO_DATA = "SaveBattleVideoData";

		static const char *INIT_SHI_TU_SEEKING_DATA = "InitShiTuSeekingData";
		static const char *SAVE_SHI_TU_SEEKING_DATA = "SaveShiTuSeekingData";

		static const char *INIT_QUAN_MIN_DISCOUNT_DATA = "InitQuanMinDiscountData";
		static const char *SAVE_QUAN_MIN_DISCOUNT_DATA = "SaveQuanMinDiscountData";

		static const char *INIT_COURAGE_CHALLENGE_RANK_DATA = "InitCourageChallengeRankData";
		static const char *SAVE_COURAGE_CHALLENGE_RANK_DATA = "SaveCourageChallengeRankData";

		static const char *INIT_GATHER_DREAM_RECORD_DATA = "InitGatherDreamRecordData";
		static const char *SAVE_GATHER_DREAM_RECORD_DATA = "SaveGatherDreamRecordData";

		static const char* INIT_CLOUD_ARENA_DATA = "InitCloudArenaData";
		static const char* SAVE_CLOUD_ARENA_DATA = "SaveCloudArenaData";

		static const char* INIT_CLOUD_ARENA_USER_DATA = "InitCloudArenaUserData";
		static const char* SAVE_CLOUD_ARENA_USER_DATA = "SaveCloudArenaUserData";

		static const char * INIT_HONG_BAO = "InitHongBao";
		static const char * SAVE_HONG_BAO = "SaveHongBao";

		static const char * INIT_RA_ROLE_PAI_MAI_HANG_DATA = "InitRARolePaiMaiHangData";
		static const char * SAVE_RA_ROLE_PAI_MAI_HANG_DATA = "SaveRARolePaiMaiHangData";

		static const char * INIT_FRIEND_INTIMACY = "InitFriendIntimacy";
		static const char * SAVE_FRIEND_INTIMACY = "SaveFriendIntimacy";
	

		static const char * INIT_COW_REPORT_GOOD_NEWS_BUY_LIST = "InitCowReportGoodNewsBuyList";
		static const char * SAVE_COW_REPORT_GOOD_NEWS_BUY_LIST = "SaveCowReportGoodNewsBuyList";
		
				
		static const char * INIT_FRIEND = "InitFriend";
		static const char * SAVE_FRIEND = "SaveFriend";

		static const char * INIT_BLACK_LIST = "InitBlackList";
		static const char * SAVE_BLACK_LIST = "SaveBlackList";


		static const char * INIT_TIGER_BRINGS_BLESSING_BUY_LIST = "InitTigerBringsBlessingBuyList";
		static const char * SAVE_TIGER_BRINGS_BLESSING_BUY_LIST = "SaveTigerBringsBlessingBuyList";

		static const char * INIT_WEDDING_LIST = "InitWeddingList";
		static const char * SAVE_WEDDING_LIST = "SaveWeddingList";

		static const char* INIT_WORLD_ARENA_BASE_DATA = "InitWorldArenaBaseData";
		static const char* SAVE_WORLD_ARENA_BASE_DATA = "SaveWorldArenaBaseData";

		static const char* INIT_WORLD_ARENA_USER_DATA = "InitWorldArenaUserData";
		static const char* SAVE_WORLD_ARENA_USER_DATA = "SaveWorldArenaUserData";

		static const char* INIT_WORLD_ARENA_SCHEDULE_DATA = "InitWorldArenaScheduleData";
		static const char* SAVE_WORLD_ARENA_SCHEDULE_DATA = "SaveWorldArenaScheduleData";

		static const char * INIT_MARKET_ITEM_DATA = "InitMarketItemData";
		static const char * SAVE_MARKET_ITEM_DATA = "SaveMarketItemData";
		
		static const char * INIT_WORLD_TEAM_ARENA_SIGN_UP_DATA = "InitWorldTeamArenaSignUpData";
		static const char * SAVE_WORLD_TEAM_ARENA_SIGN_UP_DATA = "SaveWorldTeamArenaSignUpData";

		static const char * INIT_WORLD_TEAM_ARENA_TEAM_RANK_DATA = "InitWorldTeamArenaTeamRankData";
		static const char * SAVE_WORLD_TEAM_ARENA_TEAM_RANK_DATA = "SaveWorldTeamArenaTeamRankData";
	
		static const char * INIT_OFFLINE_USER_BATTLE_DATA = "InitOfflineUserBattleData";
		static const char * SAVE_OFFLINE_USER_BATTLE_DATA = "SaveOfflineUserBattleData";
	}
}

#endif // __DA_GLOBAL_H__

