/******************************************************************************************
FileName: rmiglobal.hpp
Author: dreamwang
Description: 线级 数据库 （比如军团 阵营） 回调相关定义
Version: 1.0
History:
<author>    <time>					<version >    <desc>
dreamwang   2011/08/08/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __RMI_GLOBAL_HPP__
#define __RMI_GLOBAL_HPP__

#include "rmi/rmibase.h"
#include "servercommon/struct/global/rankparam.hpp"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/struct/global/offlineregisterparam.hpp"
#include "servercommon/struct/global/babydataparam.hpp"
#include "servercommon/struct/global/maillistparam.hpp"
#include "servercommon/guilddef.hpp"
#include "servercommon/struct/global/activitydatalistparam.hpp"
#include "servercommon/struct/global/trademarketparam.hpp"
#include "servercommon/colosseumdef.hpp"
#include "servercommon/struct/global/rankparam.hpp"
#include "servercommon/struct/friendparam.h"
#include "servercommon/struct/global/platformbattleparam.hpp"
#include "servercommon/struct/global/serverfirstkilldef.h"
#include "servercommon/struct/global/animalracedef.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/chatserverdef.h"
#include "servercommon/struct/battle/battle_video_def.hpp"
#include "servercommon/shitudef.h"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include "servercommon/struct/global/couragechallengerankdef.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/cloudarenadef.hpp"
#include "servercommon/hongbaodef.hpp"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"
#include "servercommon/struct/friendintimacyparam.h"
#include "servercommon/struct/friendparam.h"
#include "servercommon/struct/tigerbringsblessingparam.hpp"
#include "servercommon/struct/global/marketmanagerparamhpp.h"

#include "servercommon/weddingdef.hpp"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

#include "global/rmibackmanager/rmibackmanager.hpp"


struct WorldArenaBaseParam;
class WorldArenaUserListParamForSave;
class WorldArenaScheduleListParamForSave;
class WorldArenaUserListParamForInit;
class WorldArenaScheduleListParamForInit;
class OfflineUserBattleDataListForInit;
class OfflineUserBattleDataListForSave;

class RMIGlobalClient : public rmi::RMIProxyObject
{
	friend class RMIInitWantBuyBackObjectImpl;

public:
	RMIGlobalClient() {}
	virtual ~RMIGlobalClient() {}

	int OnRMIBackManagerTry(const RMICallInfo &rmi_call_info, rmi::RMIBackObject *backobj,
							const char* module, const char* method);

	bool InitWorldStatusAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveWorldStatusAsyn(const WorldStatusParam &worldstatus_param, rmi::RMIBackObject *backobj);

	bool HeartbeatCheckAsyn(rmi::RMIBackObject *backobj);

	bool InitOfflineRegisterAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveOfflineRegisterAsyn(const OfflineRegisterParam &offline_reg_param, rmi::RMIBackObject *backobj);

	bool InitBabyAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveBabyAsyn(const AllBabyDataParam & all_baby_data_param, rmi::RMIBackObject *backobj);

	bool CreateGuildAsyn(int create_db_index, const GuildBaseParam &guild_base, rmi::RMIBackObject *backobj);
	bool DismissGuildAsyn(GuildID guild_id, rmi::RMIBackObject *backobj);
	bool InitGuildAsyn(GuildID guild_id_from, rmi::RMIBackObject *backobj);
	bool SaveGuildAsyn(GuildID guild_id, AllGuildParam &all_guild_param, AllGuildGathersDreamParam &guild_dream_list, rmi::RMIBackObject *backobj);

	bool InitMailsAsyn(int uid, rmi::RMIBackObject *backobj);
	bool SaveMailsAsyn(int uid, const MailListParam &mail, rmi::RMIBackObject *backobj);
	bool GetSystemMailsAsyn(long long id_from, int uid, rmi::RMIBackObject *backobj);
	bool RemoveSystemMailsAsyn(int uid, const UserSystemMailListParam &mail_param, rmi::RMIBackObject *backobj);

	bool InitActivityDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveActivityDataAsyn(const ActivityDataListParam& list_param, rmi::RMIBackObject* backobj);

	bool InitTradeMarketDataAsync(long long id_from, bool is_system_shelve, rmi::RMIBackObject* backobj);
	bool SaveTradeMarketDataAsync(const TradeMarketData& data, rmi::RMIBackObject* backobj);

	bool InitTradeMarketUserRecordAsync(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveTradeMarketUserRecordAsync(const UserTradeRecordData& data, rmi::RMIBackObject* backobj);

	bool InitTradeMarketSystemRecordAsync(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveTradeMarketSystemRecordAsync(const SystemTradeRecord& data, rmi::RMIBackObject* backobj);

	bool InitTradeMarketSystemRecycleRuleAsync(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveTradeMarketSystemRecycleRuleAsync(const SystemRecycleRuleList& data, rmi::RMIBackObject* backobj);

	bool InitSystemAutoUpshelveDataAsync(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveSystemAutoUpshelveDataAsync(const AutoUpshelveDataList& data_list, rmi::RMIBackObject* backobj);

	bool InitPersonRankAsyn(int _rank_type , rmi::RMIBackObject *backobj);
	bool SavePersonRankAsyn(const PersonRankParam &person_rank_param, rmi::RMIBackObject *backobj);

	bool InitTeamRankAsyn(rmi::RMIBackObject *backobj);
	bool SaveTeamRankAsyn(const TeamRankParam &team_rank_param, rmi::RMIBackObject *backobj);

	bool InitChallengeUserAsyn(long long next_id_from, rmi::RMIBackObject *backobj);
	bool SaveChallengeUserAsyn(const ChallengeUserListParam &challenge_user_param, rmi::RMIBackObject *backobj);

	bool InitActivityRoleDataAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveActivityRoleDataAsyn(const ActivityRoleDataParam & activity_role_data_param, rmi::RMIBackObject *backobj);

	bool InitCommonDataAsyn(long long id_from, rmi::RMIBackObject * backobj);
	bool SaveCommonDataAsyn(const CommonSaveDataParam & data_param, rmi::RMIBackObject * backobj);

	bool InitPlatformBattleAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SavePlatformBattleAsyn(const PlatformBattleParam & activity_role_data_param, rmi::RMIBackObject *backobj);

	bool InitServerFirstKillAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveServerFirstKillAsyn(const ServerFirstKillParam & server_first_kill_param, rmi::RMIBackObject *backobj);

	bool InitAnimalRaceAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveAnimalRaceAsyn(const AnimalRaceParam & animal_race_param, rmi::RMIBackObject *backobj);

	bool InitOriginUserCacheAsyn(long long id_from, rmi::RMIBackObject *backobj);
	
	bool InitChatGroupAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveChatGroupAsyn(const ChatGroupParam & chat_group_param, rmi::RMIBackObject *backobj);

	bool InitCrossChatGroupAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveCrossChatGroupAsyn(const ChatGroupParam & chat_group_param, rmi::RMIBackObject *backobj);

	bool InitPetRankAsyn(int _rank_type, rmi::RMIBackObject *backobj);
	bool SavePetRankAsyn(const PetRankParam &person_rank_param, rmi::RMIBackObject *backobj);

	bool InitPartnerRankAsyn(int _rank_type, rmi::RMIBackObject *backobj);
	bool SavePartnerRankAsyn(const PartnerRankParam &partner_rank_param, rmi::RMIBackObject *backobj);

	bool InitBattleVideoDataAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveBattleVideoDataAsyn(const BattleVideoListParam& list_param, rmi::RMIBackObject *backobj);

	bool InitShiTuSeekingDataAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveShiTuSeekingDataAsyn(const ShiTuSeekingParam& list_param, rmi::RMIBackObject *backobj);

	bool InitQuanMinDiscountDataAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveQuanMinDiscountDataAsyn(const QuanMinDiscountParam& list_param, rmi::RMIBackObject *backobj);

	bool InitCourageChallengeRankAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveCourageChallengeRankAsyn(const CourageChallengeRankParam & courage_challenge_rank_param, rmi::RMIBackObject *backobj);

	bool InitGatherDreamRecordDataAsyn(long long id_from, rmi::RMIBackObject *backobj);
	bool SaveGatherDreamRecordDataAsyn(const GatherDreamRecordParam & gather_dream_record_param, rmi::RMIBackObject *backobj);

	bool InitCloudArenaDataAsyn(rmi::RMIBackObject* backobj);
	bool SaveCloudArenaDataAsyn(const CloudArenaParam& param, rmi::RMIBackObject *backobj);

	bool InitCloudArenaUserDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveCloudArenaUserDataAsyn(const CloudArenaUserListParam& param, rmi::RMIBackObject* backobj);

	bool InitHongBaoDataAsyn(int _hong_bao_id_from, int _hong_bao_sys_type, rmi::RMIBackObject* backobj);
	bool SaveHongBaoDataAsyn( const HongBaoParam& param, rmi::RMIBackObject *backobj);

	bool InitCowReportGoodNewsBuyListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj);
	bool SaveCowReportGoodNewsBuyListDataAsyn(const CowReportGoodNewsBuyListParam & param, rmi::RMIBackObject *backobj);

	bool InitTigerBringsBlessingBuyListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj);
	bool SaveTigerBringsBlessingBuyListDataAsyn(const TigerBringsBlessingBuyListParam & param, rmi::RMIBackObject *backobj);

	bool InitRAPaiMaiHangDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveRAPaiMaiHangDataAsyn(const RARoleParMaiHangDBParam& param, rmi::RMIBackObject *backobj);

	bool InitFriendIntimacyDataAsyn(long long _id_from, rmi::RMIBackObject* backobj);
	bool SaveFriendIntimacyDataAsyn(const FriendIntimacyListParam & param, rmi::RMIBackObject *backobj);

	bool InitFriendDataAsyn(long long _id_from,  rmi::RMIBackObject* backobj);
	bool SaveFriendDataAsyn(const FriendListParam & param, rmi::RMIBackObject *backobj);

	bool InitBlackListDataAsyn(long long _id_from, rmi::RMIBackObject* backobj);
	bool SaveBlackListDataAsyn(const BlacklistsParam & param, rmi::RMIBackObject *backobj);

	bool InitWeddingDataAsyn(int _id_from, rmi::RMIBackObject* backobj);
	bool SaveWeddingDataAsyn(const WeddingInfoDBList & param, rmi::RMIBackObject *backobj);

	bool InitWorldArenaBaseDataAsyn(rmi::RMIBackObject* backobj);
	bool SaveWorldArenaBaseDataAsyn(const WorldArenaBaseParam& param, rmi::RMIBackObject *backobj);

	bool InitWorldArenaUserDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveWorldArenaUserDataAsyn(const WorldArenaUserListParamForSave& param, rmi::RMIBackObject* backobj);

	bool InitWorldArenaScheduleDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveWorldArenaScheduleDataAsyn(const WorldArenaScheduleListParamForSave& param, rmi::RMIBackObject* backobj);

	bool InitMarketItemDataAsyn(long long _id_from, rmi::RMIBackObject* backobj);
	bool SaveMarketItemDataAsyn(const MarketManagerParam & param, rmi::RMIBackObject *backobj);
	
	bool InitWorldTeamArenaSignUpDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveWorldTeamArenaSignUpDataAsyn(const WorldTeamArenaSignUpParam& param, rmi::RMIBackObject *backobj);

	bool InitWorldTeamArenaTeamRankDataAsyn(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveWorldTeamArenaTeamRankDataAsyn(const WorldTeamArenaTeamRankParam& param, rmi::RMIBackObject *backobj);

	bool InitOfflineUserBattleDataAsync(long long id_from, rmi::RMIBackObject* backobj);
	bool SaveOfflineUserBattleDataAsync(const OfflineUserBattleDataListForSave& param, rmi::RMIBackObject *backobj);
};


class RMIInitWorldStatusBackObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldStatusBackObject() {}
	virtual ~RMIInitWorldStatusBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitWorldStatusRet(int ret, const WorldStatusParam &status_param);
	void InitError();
};

class RMISaveWorldStatusBackObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldStatusBackObject() {}
	virtual ~RMISaveWorldStatusBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveWorldStatusRet(int ret);
};

class RMIHeartbeatCheckBackObject : public rmi::RMIBackObject
{
public:
	RMIHeartbeatCheckBackObject() {}
	virtual ~RMIHeartbeatCheckBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void HeartbeatCheckRet();
};

class RMIInitOfflineRegisterBackObject : public rmi::RMIBackObject
{
public:
	RMIInitOfflineRegisterBackObject()
	{
	}
	virtual ~RMIInitOfflineRegisterBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitOfflineRegisterRet(int ret, const OfflineRegisterParam &offline_reg_param);
	void InitError();
};

class RMISaveOfflineRegisterBackObject : public rmi::RMIBackObject
{
public:
	RMISaveOfflineRegisterBackObject()
	{
	}
	virtual ~RMISaveOfflineRegisterBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveOfflineRegisterRet(int ret);
};

//---------------------------------- baby data -------------------------
class RMIInitBabyDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitBabyDataBackObject()
	{
	}
	virtual ~RMIInitBabyDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitBabyDataRet(int ret, const AllBabyDataParam & all_baby_param);
};

class RMISaveBabyDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveBabyDataBackObject()
	{
	}
	virtual ~RMISaveBabyDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveBabyDataRet(int ret);
};

//---------------------------------- guild data -------------------------

class RMICreateGuildBackObject : public rmi::RMIBackObject
{
public:
	RMICreateGuildBackObject() {};
	virtual ~RMICreateGuildBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void CreateGuildRet(int ret, GuildID guild_id);
};

class RMIDismissGuildBackObject : public rmi::RMIBackObject
{
public:
	RMIDismissGuildBackObject() {};
	virtual ~RMIDismissGuildBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void DismissGuildRet(int ret);
};

class RMIInitGuildBackObject : public rmi::RMIBackObject
{
public:
	RMIInitGuildBackObject() {};
	virtual ~RMIInitGuildBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitGuildRet(int ret, const AllGuildParam &all_guild_list, const AllGuildGathersDreamParam &guild_dream_list);
};

class RMISaveGuildBackObject : public rmi::RMIBackObject
{
public:
	RMISaveGuildBackObject() {}
	virtual ~RMISaveGuildBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveGuildRet(int ret);
};


//---------------------------------- mail data -------------------------

class RMISaveMailBackObject : public rmi::RMIBackObject
{
public:
	RMISaveMailBackObject() {};
	virtual ~RMISaveMailBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void SaveMailsRet(int ret);
};

class RMIInitMailsBackObject : public rmi::RMIBackObject
{
public:
	RMIInitMailsBackObject() {};
	virtual ~RMIInitMailsBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void InitMailsRet(int ret, const MailListParam &mail_param);
};

class RMIGetSystemMailBackObject : public rmi::RMIBackObject
{
public:
	RMIGetSystemMailBackObject() {};
	virtual ~RMIGetSystemMailBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void GetSystemMailsRet(int ret, const UserSystemMailListParam &mail_param);
};

class RMIRemoveSystemMailBackObject : public rmi::RMIBackObject
{
public:
	RMIRemoveSystemMailBackObject() {};
	virtual ~RMIRemoveSystemMailBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error) {};
	virtual void __timeout() {};
	virtual void __free();

protected:
	virtual void RemoveSystemMailsRet(int ret);
};

class RMIInitActivityDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitActivityDataBackObject()
	{
	}
	virtual ~RMIInitActivityDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitActivityDataRet(int ret, const ActivityDataListParam& list_param);
};

class RMISaveActivityDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveActivityDataBackObject()
	{
	}
	virtual ~RMISaveActivityDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveActivityDataRet(int ret);
};


class RMIInitPersonRankBackObject : public rmi::RMIBackObject
{
public:
	RMIInitPersonRankBackObject(){};
	virtual ~RMIInitPersonRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitPersonRankRet(int ret, const PersonRankParam &person_rank_param);
};

class RMISavePersonRankBackObject : public rmi::RMIBackObject
{
public:
	RMISavePersonRankBackObject() {};
	virtual ~RMISavePersonRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SavePersonRankRet(int ret);
};

class RMIInitTeamRankBackObject : public rmi::RMIBackObject
{
public:
	RMIInitTeamRankBackObject() {};
	virtual ~RMIInitTeamRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitTeamRankRet(int ret, const TeamRankParam &team_rank_param);
};

class RMISaveTeamRankBackObject : public rmi::RMIBackObject
{
public:
	RMISaveTeamRankBackObject() {};
	virtual ~RMISaveTeamRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveTeamRankRet(int ret);
};

class RMIInitPetRankBackObject : public rmi::RMIBackObject
{
public:
	RMIInitPetRankBackObject(){};
	virtual ~RMIInitPetRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitPetRankRet(int ret, const PetRankParam &person_rank_param);
};

class RMISavePetRankBackObject : public rmi::RMIBackObject
{
public:
	RMISavePetRankBackObject() {};
	virtual ~RMISavePetRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SavePetRankRet(int ret);
};

class RMIInitPartnerRankBackObject : public rmi::RMIBackObject
{
public:
	RMIInitPartnerRankBackObject() {};
	virtual ~RMIInitPartnerRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitPartnerRankRet(int ret, const PartnerRankParam &person_rank_param);
};

class RMISavePartnerRankBackObject : public rmi::RMIBackObject
{
public:
	RMISavePartnerRankBackObject() {};
	virtual ~RMISavePartnerRankBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SavePartnerRankRet(int ret);
};


class RMIInitChallengeUserBackObject : public rmi::RMIBackObject
{
public:
	RMIInitChallengeUserBackObject() {}
	virtual ~RMIInitChallengeUserBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitChallengeUserRet(int ret, const ChallengeUserListParam &challenge_user_param);
	void InitError();
};

class RMISaveChallengeUserBackObject : public rmi::RMIBackObject
{
public:
	RMISaveChallengeUserBackObject() {}
	virtual ~RMISaveChallengeUserBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveChallengeUserRet(int ret);
};

class RMIInitActivityRoleDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitActivityRoleDataBackObject() {};
	virtual ~RMIInitActivityRoleDataBackObject() {};

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitActivityRoleDataRet(int ret, const ActivityRoleDataParam &role_data_param);
};

class RMISaveActivityRoleDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveActivityRoleDataRet(int ret);
};
//-------------commonsavedata-------------
class RMIInitCommonDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitCommonDataBackObject() {};
	virtual ~RMIInitCommonDataBackObject() {};

	virtual bool __response(TLVUnserializer & in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitDataRet(int ret, const CommonSaveDataParam & data_param);
};

class RMISaveCommonDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer & in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveDataRet(int ret);
};

//---------------------------------- 擂台站 -------------------------
class RMIInitPlatformBattleBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitPlatformBattleRet(int ret, const PlatformBattleParam & param);
};

class RMISavePlatformBattleBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SavePlatformBattleRet(int ret);
};

//---------------------------------- 全服首杀 -----------------------------

class RMIInitServerFirstKillBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitServerFirstKillRet(int ret, const ServerFirstKillParam &param);
};

class RMISaveSeverFirstKillBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveServerFirstKillRet(int ret);
};

//---------------------------------- 神兽径赛 -----------------------------

class RMIInitAnimalRaceBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitAnimalRaceRet(int ret, const AnimalRaceParam &param);
};

class RMISaveAnimalRaceBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveAnimalRaceRet(int ret);
};


class RMIInitOriginUserCacheBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitOriginUserCacheRet(long long from_id, int count, UserCacheParam user_cache_node[MAX_INIT_USERCACHE_COUNT]);
};

class RMISaveChatGroupBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveChatGroupRet(int ret);
};

class RMIInitChatGroupBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();
protected:
	virtual void InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT]);
};

class RMISaveCrossChatGroupBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveChatGroupRet(int ret);
};

class RMIInitCrossChatGroupBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();
protected:
	virtual void InitChatGroupRet(long long last_id, int count, ChatGroupInfo param_list[ONCE_SEND_CHAT_GROUP_COUNT]);
};

class RMIInitBattleVideoDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitBattleVideoDataBackObject()
	{
	}
	virtual ~RMIInitBattleVideoDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitBattleVideoDataRet(int ret, const BattleVideoListParam& list_param);
};

class RMISaveBattleVideoDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveBattleVideoDataBackObject()
	{
	}
	virtual ~RMISaveBattleVideoDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveBattleVideoDataRet(int ret);
};




//---------------------------------- 师徒宣言 -----------------------------

class RMIInitShiTuSeekingDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitShiTuSeekingDataBackObject()
	{
	}
	virtual ~RMIInitShiTuSeekingDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitShiTuSeekingDataRet(int ret, const ShiTuSeekingParam& list_param);
};

class RMISaveShiTuSeekingDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveShiTuSeekingDataBackObject()
	{
	}
	virtual ~RMISaveShiTuSeekingDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveShiTuSeekingDataRet(int ret);
};


//---------------------------------- 全民团购 -----------------------------

class RMIInitQuanMinDiscountDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitQuanMinDiscountDataBackObject()
	{
	}
	virtual ~RMIInitQuanMinDiscountDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitQuanMinDiscountDataRet(int ret, const QuanMinDiscountParam& list_param);
};

class RMISaveQuanMinDiscountDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveQuanMinDiscountDataBackObject()
	{
	}
	virtual ~RMISaveQuanMinDiscountDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &out_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveQuanMinDiscountDataRet(int ret);
};


//---------------------------------- 锢魔之塔排行 -----------------------------

class RMIInitCourageChallengeRankDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitCourageChallengeRankDataBackObject()
	{
	}
	virtual ~RMIInitCourageChallengeRankDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitCourageChallengeRankDataRet(int ret, const CourageChallengeRankParam& list_param);
};

class RMISaveCourageChallengeRankDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveCourageChallengeRankDataBackObject()
	{
	}
	virtual ~RMISaveCourageChallengeRankDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveCourageChallengeRankDataRet(int ret);
};


//---------------------------------- 家族集愿记录 -----------------------------

class RMIInitGatherDreamRecordDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitGatherDreamRecordDataBackObject()
	{
	}
	virtual ~RMIInitGatherDreamRecordDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitGatherDreamRecordDataRet(int ret, const GatherDreamRecordParam& list_param);
};

class RMISaveGatherDreamRecordDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveGatherDreamRecordDataBackObject()
	{
	}
	virtual ~RMISaveGatherDreamRecordDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveGatherDreamRecordDataRet(int ret);
};

class RMIInitTradeMarketDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const TradeMarketData& data);
};

class RMISaveTradeMarketDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMISaveUserTradeRecordBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitUserTradeRecordBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int ret, const UserTradeRecordData& data);
};

class RMISaveSystemTradeRecordBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitSystemTradeRecordBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int ret, const SystemTradeRecord& data);
};

class RMISaveSystemRecycleRuleBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitSystemRecycleRuleBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int ret, const SystemRecycleRuleList& list);
};

class RMISaveSystemAutoUpshelveBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitSystemAutoUpshelveBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int ret, const AutoUpshelveDataList& list);
};

class RMIInitCloudArenaDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const CloudArenaParam& data);
};

class RMISaveCloudArenaDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitCloudArenaUserDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const CloudArenaUserListParam& data);
};

class RMISaveCloudArenaUserDataBackObject : public rmi::RMIBackObject
{
public:
	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

// 红包
class RMIInitHongBaoBackObject : public rmi::RMIBackObject
{
public:
	RMIInitHongBaoBackObject() {}
	virtual ~RMIInitHongBaoBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitHongBaoRet(int _ret, int _hongbao_sys_type,  HongBaoParam & _hongbao_param);
	void InitError();
};

class RMISaveHongBaoBackObject : public rmi::RMIBackObject
{
public:
	RMISaveHongBaoBackObject() {}
	virtual ~RMISaveHongBaoBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveHongBaoRet(int ret);
};


// 福牛报喜
class RMIInitCowReportGoodNewsBuyListBackObject : public rmi::RMIBackObject
{
public:
	RMIInitCowReportGoodNewsBuyListBackObject() {}
	virtual ~RMIInitCowReportGoodNewsBuyListBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitCowReportGoodNewsBuyListRet(int _ret, CowReportGoodNewsBuyListParam & _param);
	void InitError();
};

class RMISaveCowReportGoodNewsBuyListBackObject : public rmi::RMIBackObject
{
public:
	RMISaveCowReportGoodNewsBuyListBackObject() {}
	virtual ~RMISaveCowReportGoodNewsBuyListBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveCowReportGoodNewsBuyListRet(int ret);
};

// 寅虎纳福
class RMIInitTigerBringsBlessingBuyListBackObject : public rmi::RMIBackObject
{
public:
	RMIInitTigerBringsBlessingBuyListBackObject() {}
	virtual ~RMIInitTigerBringsBlessingBuyListBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitTigerBringsBlessingBuyListRet(int _ret, TigerBringsBlessingBuyListParam & _param);
	void InitError();
};

class RMISaveTigerBringsBlessingBuyListBackObject : public rmi::RMIBackObject
{
public:
	RMISaveTigerBringsBlessingBuyListBackObject() {}
	virtual ~RMISaveTigerBringsBlessingBuyListBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveTigerBringsBlessingBuyListRet(int ret);
};


//---------------------------------- 运营活动 - 拍卖行 --------------------------------

class RMIInitRAPaiMaiHangDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitRAPaiMaiHangDataBackObject()
	{
	}
	virtual ~RMIInitRAPaiMaiHangDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRAPaiMaiHangDataRet(int ret, const RARoleParMaiHangDBParam& list_param);
};

class RMISaveRAPaiMaiHangDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveRAPaiMaiHangDataBackObject()
	{
	}
	virtual ~RMISaveRAPaiMaiHangDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRAPaiMaiHangDataRet(int ret);
};


// 红包
class RMIInitFriendIntimacyBackObject : public rmi::RMIBackObject
{
public:
	RMIInitFriendIntimacyBackObject() {}
	virtual ~RMIInitFriendIntimacyBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitFriendIntimacyRet(int _ret,  FriendIntimacyListParam & _param);
	void InitError();
};

class RMISaveFriendIntimacyBackObject : public rmi::RMIBackObject
{
public:
	RMISaveFriendIntimacyBackObject() {}
	virtual ~RMISaveFriendIntimacyBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveFriendIntimacyRet(int ret);
};
// 好友系统
class RMIInitFriendBackObject : public rmi::RMIBackObject
{
public:
	RMIInitFriendBackObject() {}
	virtual ~RMIInitFriendBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitFriendRet(int _ret, FriendListParam & _friend_list_param);
	void InitError();
};

class RMISaveFriendBackObject : public rmi::RMIBackObject
{
public:
	RMISaveFriendBackObject() {}
	virtual ~RMISaveFriendBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveFriendRet(int ret);
};

// 好友黑名单
class RMIInitBlackBackObject : public rmi::RMIBackObject
{
public:
	RMIInitBlackBackObject() {}
	virtual ~RMIInitBlackBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitBlackRet(int _ret, BlacklistsParam & _black_list_param);
	void InitError();
};

class RMISaveBlackBackObject : public rmi::RMIBackObject
{
public:
	RMISaveBlackBackObject() {}
	virtual ~RMISaveBlackBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveBlackRet(int ret);
};


//////婚宴
class RMIInitWeddingBackObject : public rmi::RMIBackObject
{
public:
	RMIInitWeddingBackObject() {}
	virtual ~RMIInitWeddingBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitWeddingRet(int _ret, WeddingInfoDBList & _param);
	void InitError();
};

class RMISaveWeddingBackObject : public rmi::RMIBackObject
{
public:
	RMISaveWeddingBackObject() {}
	virtual ~RMISaveWeddingBackObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveWeddingRet(int ret);
};

class RMIInitWorldArenaBaseDataObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldArenaBaseDataObject() {}
	virtual ~RMIInitWorldArenaBaseDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const WorldArenaBaseParam& data);
};

class RMISaveWorldArenaBaseDataObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldArenaBaseDataObject() {}
	virtual ~RMISaveWorldArenaBaseDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitWorldArenaUserDataObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldArenaUserDataObject() {}
	virtual ~RMIInitWorldArenaUserDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const WorldArenaUserListParamForInit& data);
};

class RMISaveWorldArenaUserDataObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldArenaUserDataObject() {}
	virtual ~RMISaveWorldArenaUserDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

class RMIInitWorldArenaScheduleDataObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldArenaScheduleDataObject() {}
	virtual ~RMIInitWorldArenaScheduleDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const WorldArenaScheduleListParamForInit& data);
};

class RMISaveWorldArenaScheduleDataObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldArenaScheduleDataObject() {}
	virtual ~RMISaveWorldArenaScheduleDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};
//---------------------------------- 市场 --------------------------------
class RMIInitMarketItemDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitMarketItemDataBackObject()
	{
	}
	virtual ~RMIInitMarketItemDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitMarketItemDataRet(int ret, const MarketManagerParam& list_param);
};

class RMISaveMarketItemDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveMarketItemDataBackObject()
	{
	}
	virtual ~RMISaveMarketItemDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveMarketItemDataRet(int ret);
};

//---------------------------------- 英雄会武 --------------------------------
//报名战队
class RMIInitWorldTeamArenaSignUpDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldTeamArenaSignUpDataBackObject()
	{
	}
	virtual ~RMIInitWorldTeamArenaSignUpDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitWorldTeamArenaSignUpDataRet(int ret, const WorldTeamArenaSignUpParam& list_param);
};

class RMISaveWorldTeamArenaSignUpDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldTeamArenaSignUpDataBackObject()
	{
	}
	virtual ~RMISaveWorldTeamArenaSignUpDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveWorldTeamArenaSignUpDataRet(int ret);
};

//战队排名
class RMIInitWorldTeamArenaTeamRankDataBackObject : public rmi::RMIBackObject
{
public:
	RMIInitWorldTeamArenaTeamRankDataBackObject()
	{
	}
	virtual ~RMIInitWorldTeamArenaTeamRankDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitWorldTeamArenaTeamRankDataRet(int ret, const WorldTeamArenaTeamRankParam& list_param);
};

class RMISaveWorldTeamArenaTeamRankDataBackObject : public rmi::RMIBackObject
{
public:
	RMISaveWorldTeamArenaTeamRankDataBackObject()
	{
	}
	virtual ~RMISaveWorldTeamArenaTeamRankDataBackObject()
	{
	}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveWorldTeamArenaTeamRankDataRet(int ret);
};

class RMIInitOfflineUserBattleDataObject : public rmi::RMIBackObject
{
public:
	RMIInitOfflineUserBattleDataObject() {}
	virtual ~RMIInitOfflineUserBattleDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void InitRet(int result, const OfflineUserBattleDataListForInit& data);
};

class RMISaveOfflineUserBattleDataObject : public rmi::RMIBackObject
{
public:
	RMISaveOfflineUserBattleDataObject() {}
	virtual ~RMISaveOfflineUserBattleDataObject() {}

	virtual bool __response(TLVUnserializer &in_param);
	virtual void __exception(int error);
	virtual void __timeout();
	virtual void __free();

protected:
	virtual void SaveRet(int ret);
};

#endif  // __RMI_GLOBAL_HPP__


