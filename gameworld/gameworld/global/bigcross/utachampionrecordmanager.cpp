#include "utachampionrecordmanager.hpp"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "world.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "global/bigcross/bigcrossmanager.hpp"
#include "global/guild/guild.hpp"
#include "servercommon/errornum.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/cross/crossmanager.h"

// 测试命令添加头文件
#include "other/title/title.hpp"
#include "servercommon/facescoredef.h"
#include "other/facescore/facescore.hpp"
#include "global/team/team.hpp"

///////////////////

UTAChampionRecordManager & UTAChampionRecordManager::Instance()
{
	static UTAChampionRecordManager st_mg;
	return st_mg;
}

void UTAChampionRecordManager::OnGetUTAChampionRecord(Role * _role , Protocol::CSGetUTAChampionRecord * _get_utacr)
{
	if (NULL == _role || NULL == _get_utacr)
		return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!BigCrossManager::Instance().IsBigCrossServerConnected())
		{
			_role->NoticeNum(errornum::EN_BIGCROSS_SERVER_LOST);
			return;
		}

		this->OnGetUTAChampionRecordByHidden(_role->GetUID(), _get_utacr->begin_index , _get_utacr->end_index);
	}
	else
	{
		this->OnGetUTAChampionRecordByGameWorld(_role->GetUID(), _get_utacr->begin_index, _get_utacr->end_index);
	}
}

void UTAChampionRecordManager::SendUTAChampionRecordToRole(const char * _data, int _length)
{
	bigchgameprotocol::BigCHGameGetUTAChampionRecordAck * bg_ack = (bigchgameprotocol::BigCHGameGetUTAChampionRecordAck *)_data;
	int uid = bg_ack->uid;
	int length = bg_ack->length;
	int total_length = sizeof(bigchgameprotocol::BigCHGameGetUTAChampionRecordAck) + length;

	if (_length != total_length)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "%s ERROR !!! msg_length :%d != total_length :%d",
			__FUNCTION__, _length, total_length);
		return;
	}

	Protocol::SCGetUTAChampionRecordAck * ack = (Protocol::SCGetUTAChampionRecordAck *)(_data + sizeof(bigchgameprotocol::BigCHGameGetUTAChampionRecordAck));

	// 收到消息的一定是隐藏服
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId() ,(const char*)ack, length);
	}
	else
	{
		// 发回主服
		InternalComm::Instance().SendToGameUserThroughCross(uid,(const char*)ack, length);
	}
}

void UTAChampionRecordManager::GameHiddenGetUTAChampionRecord(crossgameprotocal::GameHiddenGetUTAChampionRecord * _get_utacr)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == _get_utacr)
		return;

	if (!BigCrossManager::Instance().IsBigCrossServerConnected())
	{
		World::GetInstWorld()->NoticeNum(_get_utacr->uid, errornum::EN_BIGCROSS_SERVER_LOST);
		return;
	}

	this->OnGetUTAChampionRecordByHidden(_get_utacr->uid, _get_utacr->begin_index, _get_utacr->end_index);
}

void UTAChampionRecordManager::GmAddUTAChampionRecord(Role * _role, int _index)
{
	// 用于测试 , 直接将输入命令的玩家的信息和队伍信息作为冠军信息存入
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == _role)
		return;

	bigchgameprotocol::HGameBigCAddUTAChampionRecordGmTest gm_test;
	gm_test.index = _index;

	Team * team = _role->GetMyTeam();
	if (NULL == team)
	{
		gm_test.count = 1;
		this->BuildUTAChampionRecordUserInfo(_role, gm_test.user_info[0]);
	}
	else
	{
		// 测试用 , 只添加在线的
		int team_count = team->GetMemberCount();
		int count = 0;
		for (int i = 0; i < team_count; ++i)
		{
			TeamMember * team_member = team->GetMemberInfo(i);
			if (NULL != team_member)
			{
				Role * team_role = World::GetInstWorld()->GetSceneManager()->GetRole(team_member->uid);
				if (NULL != team_role)
				{
					this->BuildUTAChampionRecordUserInfo(team_role, gm_test.user_info[count++]);
				}
			}
		}
		gm_test.count = count;
	}

	InternalComm::Instance().SendToBigCrossServer((const char*)&gm_test, sizeof(gm_test));
}

void UTAChampionRecordManager::OnRequestUpdateUserData(int role_id)
{
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL == role) return;

	bigchgameprotocol::HGameBigCUserSyncDataForUTAChampionRecord msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.hidden_server_idx = CrossManager::Instance().GetHiddenServerIDX();
	msg.role_id = role_id;
	
	msg.info.uid = role_id;
	F_STRNCPY(msg.info.role_name, role->GetName(), sizeof(msg.info.role_name));
	msg.info.plat_id = msg.plat_type;
	msg.info.server_id = IntToUid(role_id).db_index;
	msg.info.level = role->GetLevel();
	msg.info.fly_flag = role->GetRoleModuleManager()->GetAppearance().fly_flag;
	msg.info.top_level = role->GetTopLevel();
	msg.info.title_id = role->GetRoleModuleManager()->GetTitle()->GetWearTitle();
	msg.info.facescore_id = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_TITLENAME);
	Guild * guild = role->GetGuild();
	if (NULL == guild)
	{
		memset(msg.info.guild_name, 0, sizeof(GuildName));
		memset(msg.info.guild_banner, 0, sizeof(GuildBanner));
	}
	else
	{
		F_STRNCPY(msg.info.guild_name, guild->GetName(), sizeof(GuildName));
		F_STRNCPY(msg.info.guild_banner, guild->GetBanner(), sizeof(GuildBanner));
	}

	const Appearance& appearance = role->GetRoleModuleManager()->GetAppearance();
	msg.info.prof_base = appearance.prof_base;
	msg.info.advance_times = appearance.advance_times;
	msg.info.avatar_type = appearance.avatar_type;
	msg.info.color = appearance.color;
	msg.info.weapon_id = appearance.weapon_id;
	msg.info.sub_weapon_id = appearance.sub_weapon_id;
	msg.info.headshot_id = appearance.headshot_id;
	msg.info.special_appearance_type = appearance.special_appearance_type;
	msg.info.special_appearance_param = appearance.special_appearance_param;
	memcpy(msg.info.wear_surface, appearance.wear_surface, sizeof(msg.info.wear_surface));
	UNSTD_STATIC_CHECK(sizeof(msg.info.wear_surface) == sizeof(appearance.wear_surface));

	msg.info.smart_mounts_index = appearance.smart_mounts_index;
	msg.info.is_use_advanced_flag = appearance.is_use_advanced_flag;

	InternalComm::Instance().SendToBigCrossServer((const char*)&msg, sizeof(msg));
}

UTAChampionRecordManager::UTAChampionRecordManager()
{

}

UTAChampionRecordManager::~UTAChampionRecordManager()
{

}

void UTAChampionRecordManager::OnGetUTAChampionRecordByHidden(int _uid , int _begin_index , int _end_index)
{
	if (!CrossConfig::Instance().IsHiddenServer() )
		return;

	bigchgameprotocol::HGameBigCGetUTAChampionRecord  hb_get_utacr;
	hb_get_utacr.uid = _uid;
	hb_get_utacr.unique_hidden_server_id = UniqueHiddenServerID(LocalConfig::Instance().GetPlatType(), CrossManager::Instance().GetHiddenServerIDX()) ;
	hb_get_utacr.begin_index = _begin_index;
	hb_get_utacr.end_index = _end_index;

	InternalComm::Instance().SendToBigCrossServer((const char*)&hb_get_utacr, sizeof(hb_get_utacr));
}

void UTAChampionRecordManager::OnGetUTAChampionRecordByGameWorld(int _uid, int _begin_index, int _end_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenGetUTAChampionRecord gh_get_utacr;
	gh_get_utacr.uid = _uid;
	gh_get_utacr.begin_index = _begin_index;
	gh_get_utacr.end_index = _end_index;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&gh_get_utacr, sizeof(gh_get_utacr));
}

void UTAChampionRecordManager::BuildUTAChampionRecordUserInfo(Role * _role, UTAChampionRecordUserInfo & _out_user_info)
{
	if (NULL == _role)
		return;

	_out_user_info.uid = _role->GetUID();
	_role->GetName(_out_user_info.role_name);
	_out_user_info.plat_id = _role->GetPlatSpid();
	_out_user_info.server_id = _role->GetUserId().db_index;

	_out_user_info.level = _role->GetLevel();
	_out_user_info.fly_flag = _role->GetRoleModuleManager()->GetAppearance().fly_flag;
	_out_user_info.top_level = _role->GetTopLevel();
	Guild * guild = _role->GetGuild();
	if (NULL == guild)
	{
		memset(_out_user_info.guild_name, 0, sizeof(GuildName));
		memset(_out_user_info.guild_banner, 0, sizeof(GuildBanner));
	}
	else
	{
		F_STRNCPY(_out_user_info.guild_name, guild->GetName(), sizeof(GuildName));
		F_STRNCPY(_out_user_info.guild_banner, guild->GetBanner(), sizeof(GuildBanner));
	}

	_out_user_info.title_id = _role->GetRoleModuleManager()->GetTitle()->GetWearTitle();
	_out_user_info.facescore_id = _role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_TITLENAME);

	_out_user_info.prof_base = _role->GetRoleModuleManager()->GetAppearance().prof_base;
	_out_user_info.advance_times = _role->GetRoleModuleManager()->GetAppearance().advance_times;
	_out_user_info.avatar_type = _role->GetRoleModuleManager()->GetAppearance().avatar_type;
	_out_user_info.color = _role->GetRoleModuleManager()->GetAppearance().color;
	_out_user_info.weapon_id = _role->GetRoleModuleManager()->GetAppearance().weapon_id;
	_out_user_info.sub_weapon_id = _role->GetRoleModuleManager()->GetAppearance().sub_weapon_id;

	_out_user_info.sub_weapon_id = _role->GetRoleModuleManager()->GetAppearance().headshot_id;
	_out_user_info.sub_weapon_id = _role->GetRoleModuleManager()->GetAppearance().special_appearance_type;
	_out_user_info.sub_weapon_id = _role->GetRoleModuleManager()->GetAppearance().special_appearance_param;

	UNSTD_STATIC_CHECK(sizeof(_out_user_info.wear_surface) == sizeof(_role->GetRoleModuleManager()->GetAppearance().wear_surface));
	memcpy(_out_user_info.wear_surface, _role->GetRoleModuleManager()->GetAppearance().wear_surface,
		sizeof(_out_user_info.wear_surface));
}
