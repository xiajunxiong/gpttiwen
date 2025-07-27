#include "protobufmanager.hpp"
#include "google/protobuf/stubs/common.h"
#include "servercommon/protobuf/role_battle_data.pb.h"
#include "servercommon/protobuf/platform_battle_role_data.pb.h"
#include "servercommon/protobuf/cloud_arena_fight_record_list.pb.h"
#include "servercommon/protobuf/role_pet_partner_data.pb.h"
#include "servercommon/protobuf/role_mirageterritory_data.pb.h"
#include "servercommon/protobuf/worldarena_base_data.pb.h"
#include "servercommon/protobuf/worldarena_user_data.pb.h"
#include "servercommon/protobuf/worldarena_schedule_data.pb.h"
#include "servercommon/protobuf/utachampionrecord_data.pb.h"
#include "servercommon/protobuf/universe_team_arena_team_data.pb.h"
#include "servercommon/protobuf/uta_guess_data.pb.h"
#include "servercommon/protobuf/uta_last_season_main_over_view.pb.h"

ProtobufMgr* gProtoBufMgr = NULL;
ProtobufMgr* ProtobufMgr::Instance()
{
	if (NULL == gProtoBufMgr)
	{
		gProtoBufMgr = new ProtobufMgr();
	}

	return gProtoBufMgr;
}

google::protobuf::Message* ProtobufMgr::GetProtoMessage(int type)
{
	if (type < 0 || type >= PROTO_TYPE_MAX)
	{
		return NULL;
	}

	if (m_used_times[type] > 100)
	{
#ifdef _DEBUG
		// 检测可能出现的因使用不当导致Message实例变得臃肿的情况
		int spaceused = m_proto_list[type]->SpaceUsed();
		if (spaceused > 500000)
		{
			assert(0);
		}
#endif

		// 当被调用过很多次以后，可能会积聚了大量内存（因使用不当），这里保险起见删掉之前的Message实例，释放掉之前已用的内存	
		google::protobuf::Message* new_message = m_proto_list[type]->New();
		delete m_proto_list[type];
		m_proto_list[type] = new_message;
		
		m_used_times[type] = 0;
	}

	google::protobuf::Message* message = m_proto_list[type];
	message->Clear();

	++m_used_times[type];

	return message;
}

ProtobufMgr::ProtobufMgr()
{
	m_proto_list[PROTO_ROLE_BATTLE_DATA] = new PB_RoleBattleData();
	m_proto_list[PROTO_PLATFORM_ROLE_DATA] = new PB_PlatformBattleRoleData();
	m_proto_list[PROTO_BATTLE_CHARACTER_DATA] = new PB_BattleCharacterData();
	m_proto_list[PROTO_CLOUD_ARENA_FIGHT_RECORD_LIST_DATA] = new PB_CloudArenaFightRecordList();
	m_proto_list[PROTO_ROLE_PET_INFO_DATA] = new PB_RolePetInfoData();
	m_proto_list[PROTO_ROLE_PARTNER_INFO_DATA] = new PB_RolePartnerInfoData();
	m_proto_list[PROTO_ROLE_MIRAGETERRITORY_INFO_DATA] = new PB_RoleMirageTerritoryInfoData_MirageTerritoryAllInfo();
	m_proto_list[PROTO_WORLD_ARENA_BASE_DATA] = new PB_WorldArenaBaseData();
	m_proto_list[PROTO_WORLD_ARENA_USER_DATA] = new PB_WorldArenaUserData();
	m_proto_list[PROTO_WORLD_ARENA_SCHEDULE_DATA] = new PB_WorldArenaScheduleData();
	m_proto_list[PROTO_WORLD_ARENA_USER_SIGNUP_DATA] = new PB_WorldArenaUserSignupData();
	m_proto_list[PROTO_WORLD_ARENA_USER_SCHEDULE_DATA] = new PB_WorldArenaUserScheduleData();
	m_proto_list[PROTO_WORLD_ARENA_USER_SCORE_DATA] = new PB_WorldArenaUserScoreData();
	m_proto_list[PROTO_UTA_CHAMPION_RECORD_DATA] = new PB_UTAChampionRecordData();
	m_proto_list[PROTO_UTA_CHAMPION_USER_TEAM_INFO_DATA] = new PB_UTAChampionRecordUserTeamInfoData();
	m_proto_list[PROTO_UTA_TEAM_DATA] = new PB_UTATeamData();
	m_proto_list[PROTO_UTA_TEAM_MEMBER_DATA] = new PB_UTATeamMemberData();
	m_proto_list[PROTO_UTA_GUESS_USER_INFO_DATA] = new PB_UTAGuessUserInfoData();
	m_proto_list[PROTO_UTA_GUESS_TEAM_INFO_DATA] = new PB_UTAGuessTeamInfoData();
	m_proto_list[PROTO_UTA_LAST_SEASON_MAIN_OVERVIEW_DATA] = new PB_UTAMainOverviewScheduleList();

	UNSTD_STATIC_CHECK(PROTO_TYPE_MAX == 20);

	memset(m_used_times, 0, sizeof(m_used_times));
}

ProtobufMgr::~ProtobufMgr()
{
	for (int i = 0; i < PROTO_TYPE_MAX; ++i)
	{
		if (NULL != m_proto_list[i])
		{
			delete m_proto_list[i];
			m_proto_list[i] = NULL;
		}
	}

	google::protobuf::ShutdownProtobufLibrary();
}

