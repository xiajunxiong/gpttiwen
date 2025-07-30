#ifndef _GL_RMIBACKOBJDEF_H_
#define _GL_RMIBACKOBJDEF_H_

#include "rmiclient/rmiglobal.hpp"
#include "servercommon/darmi/rmisystemclient.h"
#include "servercommon/commondata.hpp"

class WorldStatus;

class RMIInitWorldStatusBackObjectImpl : public RMIInitWorldStatusBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitWorldStatusRet(int ret, const WorldStatusParam &worldstatus_param);
};

class RMISaveWorldStatusBackObjectImpl : public RMISaveWorldStatusBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveWorldStatusRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveWorldStateBackObjectImpl SaveWorldStateRet ret:%d \n", ret);
		}
	}
};

class RMIHeartbeatCheckBackImpl : public RMIHeartbeatCheckBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void HeartbeatCheckRet();
};

class RMIInitOfflineRegisterBackObjectImpl : public RMIInitOfflineRegisterBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);


protected:
	virtual void InitOfflineRegisterRet(int ret, const OfflineRegisterParam &offline_reg_param);
};

class RMISaveOfflineRegisterBackObjectImpl : public RMISaveOfflineRegisterBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveOfflineRegisterRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveOfflineRegisterBackObjectImpl SaveOfflineRegisterRet ret:%d \n", ret);
		}
	}
};

//--------------------------------------------------- baby -----------------------------------------------------

class RMIInitBabyDataBackObjectImpl : public RMIInitBabyDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);


protected:
	virtual void InitBabyDataRet(int ret, const AllBabyDataParam & all_baby_data_param);
};

class RMISaveBabyDataBackObjectImpl : public RMISaveBabyDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveBabyDataRet(int ret)
	{
		if (0 != ret)
		{
			printf( "RMISaveBabyDataBackObjectImpl::SaveBabyDataRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- guild -----------------------------------------------------

class RMICreateGuildBackObjectImpl : public RMICreateGuildBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

	GuildBaseParam guild_base;

	UserID user_id;
	GameName name;

protected:
	virtual void CreateGuildRet(int ret, GuildID guild_id);
};


class RMIDismissGuildBackObjectImpl : public RMIDismissGuildBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	int guild_id;

protected:
	virtual void DismissGuildRet(int ret);
};

class RMIInitGuildBackObjectImpl : public RMIInitGuildBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitGuildRet(int ret, const AllGuildParam &all_guild_list, const AllGuildGathersDreamParam &guild_dream_list);
};

class RMISaveGuildBackObjectImpl : public RMISaveGuildBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveGuildRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveGuildBackObjectImpl SaveGuildRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- mail -----------------------------------------------------

class RMISaveMailBackObjectImpl : public RMISaveMailBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveMailsRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveMailBackObjectImpl SaveMailsRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitMailsBackObjectImpl : public RMIInitMailsBackObject
{
public:
	UserID user_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitMailsRet(int ret, const MailListParam &mail_param);
};

class RMIGetSystemMailsBackObjectImpl : public RMIGetSystemMailBackObject
{
public:
	UserID user_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void GetSystemMailsRet(int ret, const UserSystemMailListParam &mail_param);
};

class RMIRemoveSystemMailsBackObjectImpl : public RMIRemoveSystemMailBackObject
{
public:
	UserID user_id;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void RemoveSystemMailsRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMIRemoveSystemMailsBackObjectImpl RemoveSystemMailsRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitActivityDataBackImpl : public RMIInitActivityDataBackObject
{
public:
	RMIInitActivityDataBackImpl()
	{
	}

	void * operator new(size_t c);
	void   operator delete(void *m);

protected:
	virtual void InitActivityDataRet(int ret, const ActivityDataListParam& list_param);
};

class RMISaveActivityDataBackImpl : public RMISaveActivityDataBackObject
{
public:
	RMISaveActivityDataBackImpl()
	{
	}

	void * operator new(size_t c);
	void   operator delete(void *m);

protected:
	virtual void SaveActivityDataRet(int ret);
};

class RMIGetPublicNoticeBackObjectImpl : public RMIGetPublicNoticeBackObject
{
public:

protected:
	virtual void GetPublicNoticeRet(int ret, const PublicNoticeList &notice_list);
};

class RMIInitTradeMarketDataBackObjectImpl : public RMIInitTradeMarketDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int result, const TradeMarketData& data);
};

class RMISaveTradeMarketDataBackObjectImpl : public RMISaveTradeMarketDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret);
};

class RMISaveUserTradeRecordBackObjectImpl : public RMISaveUserTradeRecordBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret);
};

class RMIInitUserTradeRecordBackObjectImpl : public RMIInitUserTradeRecordBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int ret, const UserTradeRecordData& data);
};

class RMISaveSystemTradeRecordBackObjectImpl : public RMISaveSystemTradeRecordBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret);
};

class RMIInitSystemTradeRecordBackObjectImpl : public RMIInitSystemTradeRecordBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int ret, const SystemTradeRecord& data);
};

class RMIInitSystemRecycleRuleBackObjectImpl : public RMIInitSystemRecycleRuleBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int ret, const SystemRecycleRuleList& data);
};

class RMISaveSystemRecycleRuleBackObjectImpl : public RMISaveSystemRecycleRuleBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret);
};

class RMIInitSystemAutoUpshelveBackObjectImpl : public RMIInitSystemAutoUpshelveBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int ret, const AutoUpshelveDataList& data);
};

class RMISaveSystemAutoUpshelveBackObjectImpl : public RMISaveSystemAutoUpshelveBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret);
};

class RMIInitPersonRankBackObjectImpl : public RMIInitPersonRankBackObject
{
public:
	int rank_type;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitPersonRankRet(int ret, const PersonRankParam &person_rank_param);
};

class RMISavePersonRankBackObjectImpl : public RMISavePersonRankBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SavePersonRankRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISavePersonRankBackObjectImpl SavePersonRankRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitTeamRankBackObjectImpl : public RMIInitTeamRankBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitTeamRankRet(int ret, const TeamRankParam &team_rank_param);
};

class RMISaveTeamRankBackObjectImpl : public RMISaveTeamRankBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveTeamRankRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveTeamRankBackObjectImpl SaveTeamRankRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitPetRankBackObjectImpl : public RMIInitPetRankBackObject
{
public:
	int rank_type;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitPetRankRet(int ret, const PetRankParam &pet_rank_param);
};

class RMISavePetRankBackObjectImpl : public RMISavePetRankBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SavePetRankRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISavePetRankBackObjectImpl SavePetRankRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitPartnerRankBackObjectImpl : public RMIInitPartnerRankBackObject
{
public:
	int rank_type;

	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitPartnerRankRet(int ret, const PartnerRankParam &partner_rank_param);
};

class RMISavePartnerRankBackObjectImpl : public RMISavePartnerRankBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SavePartnerRankRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISavePartnerRankBackObjectImpl SavePartnerRankRet Failed ret:%d\n", ret);
		}
	}
};


class ColosseumManager;
class RMIInitChallengeUserObjectImpl : public RMIInitChallengeUserBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitChallengeUserRet(int ret, const ChallengeUserListParam &challenge_user_param);
};

class RMISaveChallengeUserObjectImpl : public RMISaveChallengeUserBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveChallengeUserRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveChallengeUserObjectImpl SaveChallengeUserRet ret:%d \n", ret);
		}
	}
};

//--------------------------------------------------- avtivityroledata -----------------------------------------------------
class RMIInitActivityRoleDataBackObjectImpl : public RMIInitActivityRoleDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);


protected:
	virtual void InitActivityRoleDataRet(int ret, const ActivityRoleDataParam & all_baby_data_param);
};

class RMISaveActivityRoleDataBackObjectImpl : public RMISaveActivityRoleDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveActivityRoleDataRet(int ret);
};

//--------------------------------------------------- commonsavedata -----------------------------------------------------
class RMIInitCommonDataBackObjectImpl : public RMIInitCommonDataBackObject
{
public:
	void * operator new(size_t c);
	void	operator delete(void * m);


protected:
	virtual void InitDataRet(int ret, const CommonSaveDataParam & data_param);
};

class RMISaveCommonDataBackObjectImpl : public RMISaveCommonDataBackObject
{
public:
	void * operator new(size_t c);
	void	operator delete(void * m);

protected:
	virtual void SaveDataRet(int ret);
};


//--------------------------------------------------- 擂台战 -----------------------------------------------------

class RMIInitPlatformBattleBackObjectImpl : public RMIInitPlatformBattleBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);


protected:
	virtual void InitPlatformBattleRet(int ret, const PlatformBattleParam & param);
};

class RMISavePlatformBattleBackObjectImpl : public RMISavePlatformBattleBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SavePlatformBattleRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISavePlatformBattleBackObjectImpl::SavePlatformBattleRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 全服首杀 -----------------------------------------------------

class RMIInitServerFirstKillBackObjectImpl : public RMIInitServerFirstKillBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitServerFirstKillRet(int ret, const ServerFirstKillParam &server_first_kill_param);
};

class RMISaveServerFirstKillBackObjectImpl : public RMISaveSeverFirstKillBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveServerFirstKillRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveServerFirstKillBackObjectImpl SaveServerFirstKillRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 神兽径赛 ----------------------------------------------------

class RMIInitAnimalRaceBackObjectImpl : public RMIInitAnimalRaceBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitAnimalRaceRet(int ret, const AnimalRaceParam &animal_race_param);
};

class RMISaveAnimalRaceBackObjectImpl : public RMISaveAnimalRaceBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveAnimalRaceRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveAnimalRaceBackObjectImpl SaveAnimalRaceRet Failed ret:%d\n", ret);
		}
	}
};


class RMIInitOriginUserCacheBackObjectImpl : public RMIInitOriginUserCacheBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	UniqueServerID usid;
protected:
	virtual void InitOriginUserCacheRet(long long from_id, int count, UserCacheParam user_cache_node[MAX_INIT_USERCACHE_COUNT]);
};

class RMISaveChatGroupBackObjectImpl : public RMISaveChatGroupBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	long long m_last_save_group_id;

protected:
	virtual void SaveChatGroupRet(int ret);
};

class RMIInitChatGroupBackObjectImpl : public RMIInitChatGroupBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT]);
};

class RMISaveCrossChatGroupBackObjectImpl : public RMISaveCrossChatGroupBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	long long m_last_save_group_id;

protected:
	virtual void SaveChatGroupRet(int ret);
};

class RMIInitCrossChatGroupBackObjectImpl : public RMIInitCrossChatGroupBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT]);
};

class RMIInitBattleVideoDataBackObjectImpl : public RMIInitBattleVideoDataBackObject
{
public:
	void* operator new(size_t c);
	void operator delete(void *m);

protected:
	virtual void InitBattleVideoDataRet(int ret, const BattleVideoListParam& list_param);
};

class RMISaveBattleVideoDataBackObjectImpl : public RMISaveBattleVideoDataBackObject
{
public:
	void* operator new(size_t c);
	void operator delete(void *m);

protected:
	virtual void SaveBattleVideoDataRet(int ret);
};


//---------------------------------- 师徒宣言 --------------------------------------
class RMIInitShiTuSeekingDataBackObjectImpl : public RMIInitShiTuSeekingDataBackObject
{
public:
	void* operator new(size_t c);
	void operator delete(void *m);

protected:
	virtual void InitShiTuSeekingDataRet(int ret, const ShiTuSeekingParam& list_param);
};

class RMISaveShiTuSeekingDataBackObjectImpl : public RMISaveShiTuSeekingDataBackObject
{
public:
	void* operator new(size_t c);
	void operator delete(void *m);

protected:
	virtual void SaveShiTuSeekingDataRet(int ret);
};

//--------------------------------------------------- 全民团购 ----------------------------------------------------

class RMIInitQuanMinDiscountBackObjectImpl : public RMIInitQuanMinDiscountDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitQuanMinDiscountDataRet(int ret, const QuanMinDiscountParam& param_list);
};

class RMISaveQuanMinDiscountBackObjectImpl : public RMISaveQuanMinDiscountDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveQuanMinDiscountDataRet(int ret);
};

//--------------------------------------------------- 锢魔之塔排行 ----------------------------------------------------

class RMIInitCourageChallengeRankBackObjectImpl : public RMIInitCourageChallengeRankDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitCourageChallengeRankDataRet(int ret, const CourageChallengeRankParam &list_param);
};

class RMISaveCourageChallengeRankBackObjectImpl : public RMISaveCourageChallengeRankDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveCourageChallengeRankDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveCourageChallengeRankBackObjectImpl SaveCourageChallengeRankDataRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 家族集愿记录 ----------------------------------------------------

class RMIInitGatherDreamRecordBackObjectImpl : public RMIInitGatherDreamRecordDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitGatherDreamRecordDataRet(int ret, const GatherDreamRecordParam &list_param);
};

class RMISaveGatherDreamRecordBackObjectImpl : public RMISaveGatherDreamRecordDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveGatherDreamRecordDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveGatherDreamRecordBackObjectImpl SaveGatherDreamRecordDataRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitCloudArenaDataBackObjectImpl : public RMIInitCloudArenaDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int ret, const CloudArenaParam& param);
};

class RMISaveCloudArenaDataBackObjectImpl : public RMISaveCloudArenaDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveCloudArenaDataBackObjectImpl SaveRet ret:%d \n", ret);
		}
	}
};

class RMIInitCloudArenaUserDataBackObjectImpl : public RMIInitCloudArenaUserDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRet(int result, const CloudArenaUserListParam& data);
};

class RMISaveCloudArenaUserDataBackObjectImpl : public RMISaveCloudArenaUserDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveCloudArenaUserDataBackObjectImpl SaveRet ret:%d \n", ret);
		}
	}
};


//--------------------------------------------------- 红包 ----------------------------------------------------

class HongBaoParam;
class RMIInitHongBaoBackObjectImpl : public RMIInitHongBaoBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitHongBaoRet(int _ret, int _hongbao_sys_type ,HongBaoParam & _hongbao_param);
};

class RMISaveHongBaoBackObjectImpl : public RMISaveHongBaoBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveHongBaoRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveHongBaoBackObjectImpl SaveHongBaoRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 好友亲密度系统 ----------------------------------------------------

class CowReportGoodNewsBuyListParam;
// 好友
class RMIInitCowReportGoodNewsBuyListBackObjectImpl : public RMIInitCowReportGoodNewsBuyListBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitCowReportGoodNewsBuyListRet(int _ret, CowReportGoodNewsBuyListParam & _param);
};

class RMISaveCowReportGoodNewsBuyListBackObjectImpl : public RMISaveCowReportGoodNewsBuyListBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveCowReportGoodNewsBuyListRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveCowReportGoodNewsBuyListBackObjectImpl SaveCowReportGoodNewsBuyListRet Failed ret:%d\n", ret);
		}
	}
};

// 寅虎纳福
class TigerBringsBlessingBuyListParam;
class RMIInitTigerBringsBlessingBuyListBackObjectImpl : public RMIInitTigerBringsBlessingBuyListBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitTigerBringsBlessingBuyListRet(int _ret, TigerBringsBlessingBuyListParam & _param);
};

class RMISaveTigerBringsBlessingBuyListBackObjectImpl : public RMISaveTigerBringsBlessingBuyListBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveTigerBringsBlessingBuyListRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveTigerBringsBlessingBuyListBackObjectImpl SaveTigerBringsBlessingBuyListRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 运营活动 - 拍卖行 ----------------------------------------------------

class RMIInitRAPaiMaiHangBackObjectImpl : public RMIInitRAPaiMaiHangDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitRAPaiMaiHangDataRet(int ret, const RARoleParMaiHangDBParam &list_param);
};

class RMISaveRAPaiMaiHangBackObjectImpl : public RMISaveRAPaiMaiHangDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveRAPaiMaiHangDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveRAPaiMaiHangBackObjectImpl SaveRAPaiMaiHangDataRet Failed ret:%d\n", ret);
		}
	}
};


//--------------------------------------------------- 好友系统 ----------------------------------------------------

class FriendListParam;
class BlacklistsParam;
// 好友
class RMIInitFriendBackObjectImpl : public RMIInitFriendBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitFriendRet(int _ret, FriendListParam & _friend_list_param);
};

class RMISaveFriendBackObjectImpl : public RMISaveFriendBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveFriendRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveFriendBackObjectImpl SaveFriendRet Failed ret:%d\n", ret);
		}
	}
};
// 黑名单
class RMIInitBlackBackObjectImpl : public RMIInitBlackBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitBlackRet(int _ret, BlacklistsParam & _black_list_param);
};

class RMISaveBlackBackObjectImpl : public RMISaveBlackBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveBlackRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveBlackBackObjectImpl SaveBlackRet Failed ret:%d\n", ret);
		}
	}
};

//--------------------------------------------------- 好友亲密度系统 ----------------------------------------------------

class FriendIntimacyListParam;
// 好友
class RMIInitFriendIntimacyBackObjectImpl : public RMIInitFriendIntimacyBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitFriendIntimacyRet(int _ret, FriendIntimacyListParam & _param);
};

class RMISaveFriendIntimacyBackObjectImpl : public RMISaveFriendIntimacyBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveFriendIntimacyRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveFriendIntimacyBackObjectImpl SaveFriendIntimacyRet Failed ret:%d\n", ret);
		}
	}
};

//////婚宴
class RMIInitWeddingBackObjectImpl : public RMIInitWeddingBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitWeddingRet(int _ret, WeddingInfoDBList & _param);
};

class RMISaveWeddingBackObjectImpl : public RMISaveWeddingBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveWeddingRet(int ret)
	{
		if (0 != ret)
		{
			printf("RMISaveWeddingBackObjectImpl SaveWeddingRet Failed ret:%d\n", ret);
		}
	}
};

class RMIInitWorldArenaBaseDataBackObjectImpl : public RMIInitWorldArenaBaseDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void InitRet(int ret, const WorldArenaBaseParam& param);
};

class RMISaveWorldArenaBaseDataBackObjectImpl : public RMISaveWorldArenaBaseDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitWorldArenaUserDataBackObjectImpl : public RMIInitWorldArenaUserDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void InitRet(int ret, const WorldArenaUserListParamForInit& param);
};

class RMISaveWorldArenaUserDataBackObjectImpl : public RMISaveWorldArenaUserDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

	void SetDeleteList(const std::vector<long long>& delete_list) { m_delete_list = delete_list; }

protected:
	virtual void SaveRet(int ret);

	std::vector<long long> m_delete_list;
};

class RMIInitWorldArenaScheduleDataBackObjectImpl : public RMIInitWorldArenaScheduleDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void InitRet(int ret, const WorldArenaScheduleListParamForInit& param);
};

class RMISaveWorldArenaScheduleDataBackObjectImpl : public RMISaveWorldArenaScheduleDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

	void SetDeleteList(const std::vector<long long>& delete_list) { m_delete_list = delete_list; }

protected:
	virtual void SaveRet(int ret);

	std::vector<long long> m_delete_list;
};

//---------------------------------- 市场 --------------------------------
class RMIInitMarketItemDataBackObjectImpl : public RMIInitMarketItemDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitMarketItemDataRet(int ret, const MarketManagerParam& list_param);
};

class RMISaveMarketItemDataBackObjectImpl : public RMISaveMarketItemDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveMarketItemDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("%s Failed ret:%d\n", __FUNCTION__, ret);
		}
	}
};

//---------------------------------- 英雄会武 --------------------------------
//报名战队
class RMIInitWorldTeamArenaSignUpDataBackObjectImpl : public RMIInitWorldTeamArenaSignUpDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitWorldTeamArenaSignUpDataRet(int ret, const WorldTeamArenaSignUpParam& list_param);
};

class RMISaveWorldTeamArenaSignUpDataBackObjectImpl : public RMISaveWorldTeamArenaSignUpDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveWorldTeamArenaSignUpDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("%s Failed ret:%d\n", __FUNCTION__, ret);
		}
	}
};

//战队排名
class RMIInitWorldTeamArenaTeamRankDataBackObjectImpl : public RMIInitWorldTeamArenaTeamRankDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void InitWorldTeamArenaTeamRankDataRet(int ret, const WorldTeamArenaTeamRankParam& list_param);
};

class RMISaveWorldTeamArenaTeamRankDataBackObjectImpl : public RMISaveWorldTeamArenaTeamRankDataBackObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

protected:
	virtual void SaveWorldTeamArenaTeamRankDataRet(int ret)
	{
		if (0 != ret)
		{
			printf("%s Failed ret:%d\n", __FUNCTION__, ret);
		}
	}
};

class RMIInitOfflineUserBattleDataBackObjectImpl : public RMIInitOfflineUserBattleDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void InitRet(int ret, const OfflineUserBattleDataListForInit& param);
};

class RMISaveOfflineUserBattleDataBackObjectImpl : public RMISaveOfflineUserBattleDataObject
{
public:
	void *	operator new(size_t c);
	void	operator delete(void *m);

	virtual void __exception(int error);
	virtual void __timeout();

protected:
	virtual void SaveRet(int ret);
};

#endif  // _GL_RMIBACKOBJDEF_H_







