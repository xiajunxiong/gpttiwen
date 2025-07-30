#include "world.h"
#include "other/pet/pet.hpp"
#include "item/money.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "config/otherconfig/guildconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "engineadapter.h"
#include "internalcomm.h"
#include "guildmanager.hpp"
#include "guildmembermanager.hpp"
#include "guild.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "global/rmibackobjdef.h"
#include "gamelog.h"
#include "serverlogic.h"
#include "guildmember.hpp"
#include "item/knapsack.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "chat/chatmanager.h"
#include "friend/friendmanager.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "global/activity/activitymanager.hpp"
#include "other/facescore/facescore.hpp"
#include "servercommon/namefilter.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "global/gatherdreammanager/gatherdreamrecordmanager.hpp"
#include "servercommon/gatherdreamdef.hpp"
#include "other/pet/petconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/funopen/funopen.hpp"

#include "global/rank/rankmanager.hpp"
#include "other/title/title.hpp"

static char SYNC_GUILD_BUFF[1024 * 1024];

bool GuildManager::IsInvildName(GuildName name)
{
	for (GuildMapConstIt it = m_guild_map.begin(); it != m_guild_map.end(); ++it)
	{
		if (0 == strncmp(it->second->GetName(), name, sizeof(GuildName)))
		{
			return true;
		}
	}
	return false;
}

void GuildManager::SendGuildList(Role* role)
{
	time_t  now_time = EngineAdapter::Instance().Time();
	time_t &next_send_time = m_send_limit_map[role->GetUID()];
	if (next_send_time > now_time) return;
	next_send_time = now_time + 10;
	int user_origin_plat_type = role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType();
	int user_origin_server_id = role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId();
	bool is_cross = false;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		is_cross = true;
	}

	static Protocol::SCSendGuildList info;
	info.count = 0;
	for (GuildMapConstIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		if (info.count >= Protocol::MAX_GUILD_COUNT) break;

		Guild * guild = it->second;
		if (NULL == guild) continue;

		if (is_cross)
		{
			if(guild->GetPlatType() != user_origin_plat_type || guild->GetServerId() != user_origin_server_id) continue;
		}

		GuildMemberManager & mem_mgr_ref = guild->GetMemberManagerRef();
		Protocol::GuildNodeInfo& node = info.guild_list[info.count++];
		node.guild_id = guild->GetGuildID();
		node.patriarch_id = mem_mgr_ref.GetPatriarchUid();
		node.level = guild->GetGuildLevel();
		node.member_count = mem_mgr_ref.GetMemberCount();
		guild->GetGuildName(node.guild_name);
		mem_mgr_ref.GetPatriarchName(node.patriarch_name);
		F_STRNCPY(node.guild_banner, guild->GetBanner(), sizeof(node.guild_banner));
		node.seven_day_active = guild->GetSevenDayTotalActive();
		node.seven_day_active_last_change_times = guild->GetActiveLastChangeTimes();

		node.extra_member_limit = 0;	//厢房额外人数上限
		const GuildBaseConstructCfg * gc_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_GUEST_ROOM, guild->GetBuildLevel(GUILD_BUILD_TYPE_GUEST_ROOM));
		if(NULL != gc_cfg)	node.extra_member_limit = gc_cfg->paramet_1;
	}

	int len = sizeof(info) - ((Protocol::MAX_GUILD_COUNT - info.count) * sizeof(Protocol::GuildNodeInfo));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, len);
}

void GuildManager::SendGuildNotice(Role * role, GuildID guild_id)
{
	GuildMapConstIt it = m_guild_map.find(guild_id);
	if (it == m_guild_map.end()) return;
	Guild* guild = it->second;
	if (NULL == guild) return;

	static Protocol::SCSendGuildNotic info;
	info.guild_id = guild_id;
	guild->GetGuildNotice(info.notice);
	
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void GuildManager::SendPetTrainingInfo(Role * role)
{
	if (NULL == role) return;
	static Protocol::SCSendGuildTrainingInfo info;
	memcpy(info.guild_training_level, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level, sizeof(info.guild_training_level));
	memcpy(info.training_pet_index, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_pet_list, sizeof(info.training_pet_index));
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
}

void GuildManager::EraseApplyGuild(int uid, int guild_id)
{
	std::map<int, std::set<int> >::iterator find_it = m_user_apply_guild.find(uid);
	if (m_user_apply_guild.end() != find_it) find_it->second.erase(guild_id);
}

void GuildManager::DeleteApplyRecord(int uid)
{
	m_user_apply_guild.erase(uid);
}

void GuildManager::GmGuildManagerHandler(int op_type, int guild_id)
{
	enum GuildMgrHandlerType
	{
		GUILD_MGR_DAY_CHANGE_NOW = 0,		// 立刻执行 OnDayChange；
		GUILD_MGR_SET_PROTECTION = 1,		// 刷新家族保护时间为 0；
		GUILD_MGR_GET_PROTECTION = 2,		// 获取指定家族的保护时间；
	};

	switch (op_type)
	{
	case GUILD_MGR_DAY_CHANGE_NOW:
		this->OnDayChange(1, 2);
		break;
	case GUILD_MGR_SET_PROTECTION:
		{
			GuildMapIt it = m_guild_map.find(guild_id);
			if (m_guild_map.end() != it && NULL != it->second) it->second->GmGuildHandler(0, 0);
		}
		break;
	case GUILD_MGR_GET_PROTECTION:
		{
			GuildMapIt it = m_guild_map.find(guild_id);
			if (m_guild_map.end() != it && NULL != it->second) it->second->GmGuildHandler(1, 0);
		}
		break;
	}
}

void GuildManager::OnSyncGuildChangeFromGame(int guild_id, int plat_type, int op_type, const AllGuildParam::GuildParam& param)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	if (op_type == crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_ADD)
	{
		this->AddGuild(param, false);
	}
	else if (op_type == crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_UPDATE)
	{
		Guild* guild = this->GetGuild(guild_id);
		if (NULL != guild)
		{
			guild->Init(param, NULL, true);
		}
	}
	else if (op_type == crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_REMOVE)
	{
		this->RemoveGuild(guild_id);
	}
}

void GuildManager::RemoveCreatingGuildRole(int role_id)
{
	m_creating_guild_role.erase(role_id);
}

void GuildManager::SendGuildBossChapterRankList(Role * role)
{
	if(NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		static crossgameprotocal::HiddenGameBossChapterRankReq hgbcrr;
		hgbcrr.uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index, (const char*)&hgbcrr, sizeof(hgbcrr));
		return;
	}


	static Protocol::SCGuildBossChapterGuildRankInfo info;
	info.count = 0;
	
	if (!m_guild_boss_chapter_rank_list.empty())
	{
		for (int i = 0; i < (int)m_guild_boss_chapter_rank_list.size() && info.count < ARRAY_LENGTH(info.rank_list); i++)
		{
			info.rank_list[info.count++] = m_guild_boss_chapter_rank_list[i];
		}
	}
	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);
}

void GuildManager::HiddenGameBossChapterRankReq(long long uuid)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static Protocol::SCGuildBossChapterGuildRankInfo info;
	info.count = 0;

	if (!m_guild_boss_chapter_rank_list.empty())
	{
		for (int i = 0; i < (int)m_guild_boss_chapter_rank_list.size() && info.count < ARRAY_LENGTH(info.rank_list); i++)
		{
			info.rank_list[info.count++] = m_guild_boss_chapter_rank_list[i];
		}
	}
	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&info, uuid);
}

void GuildManager::UpdateGuildBossChapterRank(GuildBossChapterRank * info)
{
	if(NULL == info) return;
	
	for (int i = 0; i < (int)m_guild_boss_chapter_rank_list.size(); i++)
	{
		if (m_guild_boss_chapter_rank_list[i].guild_id == info->guild_id)
		{
			m_guild_boss_chapter_rank_list[i] = *info;
			break;
		}
	}
	std::sort(m_guild_boss_chapter_rank_list.begin(), m_guild_boss_chapter_rank_list.end());
}

void GuildManager::GmDismissGuild(GuildID guild_id)
{
	this->DismissGuild(guild_id);
}


void GuildManager::GmGuildActiveSettlement()
{
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild * guild = it->second;
		if (NULL == guild) continue;

		// 家族周活跃个人排行榜结算
		guild->GmGuildActiveSettlement();
	}
}


void GuildManager::OnGameQueryHiddenIfSyncGuildSucceed(int plat_type, int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	if (this->IsServerSyncGuildSucc(plat_type, server_id))
	{
		gamelog::g_log_guild.printf(LL_INFO, "OnGameQueryHiddenIfSyncGuildSucceed_1 plat_type[%d] server_id[%d]", plat_type, server_id);

		crossgameprotocal::HiddenGameSyncGuildSuccNotice notice;
		InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&notice, sizeof(notice));
	}
	else
	{
		gamelog::g_log_guild.printf(LL_INFO, "OnGameQueryHiddenIfSyncGuildSucceed_2 plat_type[%d] server_id[%d]", plat_type, server_id);

		this->RequestSyncAllGuild(server_id, false);
	}
}

void GuildManager::OnHiddenNoticeGameSyncGuildSucc()
{
	m_check_sync_succ_timestamp = 0u;

	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::OnHiddenNoticeGameSyncGuildSucc next_check_timestamp[%u]", m_check_sync_succ_timestamp);
}

void GuildManager::OnDisconnectFromCrossServer()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->RemoveAllServerToSyncSuccRecorder();
	}
}

void GuildManager::OnOriginDisconnectFromCrossServer(int plat_type, int server_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->RemoveServerToSyncSuccRecorder(plat_type, server_id);
	}
}

Guild * GuildManager::AddGuild(const AllGuildParam::GuildParam &guild_param, bool is_create, bool replace_on_repeat /* = false */, const GuildGatherDreamParam * guild_dream_param /* = NULL*/)
{
	bool is_in_cross = CrossConfig::Instance().IsHiddenServer();
	GuildMapIt it = m_guild_map.find(guild_param.base_param.guild_id);
	if (it != m_guild_map.end())
	{
		if (replace_on_repeat && is_in_cross)
		{
			delete it->second;
			it->second = NULL;
		}
		else
		{
			printf("GuildManager::AddGuild[%d] Failed! Repeated\n", guild_param.base_param.guild_id);
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildManager::AddGuild GUILD REPEAT] [guild_id:%d]", (int)guild_param.base_param.guild_id);
			return NULL;
		}
	}

	Guild *guild = new Guild(this);
	guild->Init(guild_param, guild_dream_param);

	m_guild_map[guild_param.base_param.guild_id] = guild;
	if (!is_in_cross)
	{
		GuildBossChapterRank guild_boss_chapter_rank_info;
		if (guild->GetGuildBossChapterInfo(&guild_boss_chapter_rank_info))
		{
			m_guild_boss_chapter_rank_list.push_back(guild_boss_chapter_rank_info);//此时不要进行排序,如果是初始化则由外部排序,如果是创建默认排最后
		}
	}

	return guild;
}

GuildManager::GuildManager() : m_check_sync_succ_timestamp(0u)
{
	m_load_flag = false;

}

GuildManager::~GuildManager()
{
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		delete it->second;
	}

	m_guild_map.clear();
}

GuildManager & GuildManager::Instance()
{
	static GuildManager gm;
	return gm;
}

void RMIInitGuildBackObjectImpl::InitGuildRet(int ret, const AllGuildParam &guild_list, const AllGuildGathersDreamParam &guild_dream_list)
{
	GuildManager::Instance().Init(ret, guild_list, guild_dream_list);
}

void GuildManager::Init(int ret, const AllGuildParam & guild_list, const AllGuildGathersDreamParam &guild_dream_list)
{
	if (0 != ret)
	{
		printf("GuildManager::Init Failed \n");
		ServerLogic::GetInstServerLogic()->StopServer("GuildManager::Init");
	}
	else
	{
		GuildID next_guild_id_from = 0;

		for (int i = 0; i < guild_list.count && i < MAX_ONCE_GUILD_COUNT; ++i)
		{
			const AllGuildParam::GuildParam & guild = guild_list.guild_list[i];
			const GuildGatherDreamParam & guild_dream = guild_dream_list.guild_gather_dream_list[i];
			if (guild.member_param.count > 0)
			{
				this->AddGuild(guild, false, false, &guild_dream);
				
				if (0 == guild.base_param.guild_name[0])
				{
					printf("Empty Guild Name, Guild ID: %d\n", guild.base_param.guild_id);
					gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildManager::Init INVALID GUILD NAME] [guild_id:%d]", (int)guild.base_param.guild_id);
				}

				if (0 != guild.base_param.guild_banner[0])
				{
					int hash = GetStringHash(sizeof(GuildBanner), guild.base_param.guild_banner);
					m_guild_banner_hash_set.insert(hash);
				}
			}
			else
			{
				printf("GuildManager::Init : AddGuild Failed Guild ID: %d, GuildName[%s]\n", guild.base_param.guild_id, guild.base_param.guild_name);
				gamelog::g_log_serious_error.printf(LL_CRITICAL, "[GuildManager::Init INVALID MEMBER COUNT] [guild_id:%d] member_cnt[%d] guild_name[%s]", (int)guild.base_param.guild_id, guild.member_param.count, guild.base_param.guild_name);
			}

			if (guild.base_param.guild_id > next_guild_id_from)
			{
				next_guild_id_from = guild.base_param.guild_id;
			}
		}

		if (guild_list.count >= MAX_ONCE_GUILD_COUNT)
		{
			RMIInitGuildBackObjectImpl *impl = new RMIInitGuildBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitGuildAsyn(next_guild_id_from, impl);
		}
		else
		{
			printf("Init Guild Succ, Guild Count: %d\n", static_cast<int>(m_guild_map.size()));

			gamelog::g_log_world.printf(LL_INFO, "GuildManager::Init finish load data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());

			m_load_flag = true;
			if (!CrossConfig::Instance().IsHiddenServer() && !m_guild_boss_chapter_rank_list.empty())
			{
				//初始化时已经由AddGuild接口添加了所有家族，此时只需排序即可
				std::sort(m_guild_boss_chapter_rank_list.begin(), m_guild_boss_chapter_rank_list.end());
			}
			// 家族加载完毕后才能加载家族排行榜
			RankManager::Instance().GetGuildRank()->InitGuildRank();
		}
	}
}

void GuildManager::Update(unsigned long interval, time_t now_second)
{
	if (!m_load_flag) return;

	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild *guild = it->second;
		guild->Update(interval, now_second);
	}

	if (m_check_sync_succ_timestamp > 0u && m_check_sync_succ_timestamp < static_cast<unsigned int>(now_second))
	{
		m_check_sync_succ_timestamp = static_cast<unsigned int>(now_second) + 30;
		this->QueryHiddenIfSyncGuildSucceed();
	}
}

void GuildManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	std::vector<GuildID> dismiss_vector;
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild *guild = it->second;
		if (NULL != guild && !guild->OnDayChange(old_dayid, now_dayid))
		{
			// 发送邮件通知
			static MailContentParam mail;
			int lensub = SNPRINTF(mail.subject, sizeof(mail.subject), gamestring::g_guild_dismiss_subject);
			int length = SNPRINTF(mail.contenttxt, sizeof(mail.contenttxt), gamestring::g_guild_dismiss_content, guild->GetName());
			if (lensub > 0 && length > 0) guild->GetMemberManagerRef().SendMailToAllMember(MAIL_REASON_DEFAULT, mail);

			// 塞进解散表里
			dismiss_vector.push_back(it->first);
		}
	}

	// 开始解散
	for (std::vector<GuildID>::const_iterator it = dismiss_vector.begin(); it != dismiss_vector.end(); ++it)
	{
		this->DismissGuild(*it);
	}
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		this->RefreshGuildBossChapterRank();
	}
}

void GuildManager::OnWeekChange()
{
	std::vector<GuildID> dismiss_vector;
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild * guild = it->second;
		if (NULL == guild) continue;
		guild->OnWeekChange();
	}
}

void GuildManager::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild* guild = it->second;
		if (NULL == guild) continue;

		guild->OnResetData(old_dayid, now_dayid);
	}
}

void GuildManager::OnConnectCrossServerSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->RequestSyncAllGuild(0, true);
	}
	else
	{
		m_check_sync_succ_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 30; // 30秒后向隐藏服确认是否已同步
	
		gamelog::g_log_guild.printf(LL_INFO, "GuildManager::OnConnectCrossServerSucc next query time[%u]", m_check_sync_succ_timestamp);
	}
}

void GuildManager::SyncAllGuildToCross()
{
	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::SyncAllGuildToCross begin");
	
	bool is_first = true;

	static AllGuildParam param;
	param.count = 0;

	GuildMapIt it = m_guild_map.begin();
	GuildMapIt end = m_guild_map.end();
	for (; it != end; ++it)
	{
		GuildName name = { 0 };
		int guild_id = it->second->GetGuildID();
		it->second->GetGuildName(name);

		gamelog::g_log_guild.printf(LL_INFO, "SyncAllGuildToCross add guild_id:%d guile_name:%s", guild_id, name);

		it->second->GetInitParam(&param.guild_list[param.count++], false);

		if (param.count >= MAX_ONCE_GUILD_COUNT)
		{		
			crossgameprotocal::GameHiddenResponseGuildList msg;
			msg.plat_type = LocalConfig::Instance().GetPlatType();
			msg.server_id = LocalConfig::Instance().GetMergeServerId();
			msg.is_last = 0;
			msg.is_first = is_first ? 1 : 0;
			is_first = false;
			memcpy(SYNC_GUILD_BUFF, &msg, sizeof(msg));

			TLVSerializer serializer;
			serializer.Reset(SYNC_GUILD_BUFF + sizeof(msg), sizeof(SYNC_GUILD_BUFF) - sizeof(msg));
			bool ret = param.Serialize(serializer);

			gamelog::g_log_guild.printf(LL_INFO, "SyncAllGuildToCross_1 ret:%d  msg_length[%d]", (int)ret, (int)(sizeof(msg) + serializer.Size()));

			if (!ret)
			{
#ifdef _DEBUG
				assert(0);
#endif
				return;
			}

			InternalComm::Instance().SendToHiddenThroughCross(SYNC_GUILD_BUFF, sizeof(msg) + serializer.Size());

			param.count = 0;
		}
	}

	crossgameprotocal::GameHiddenResponseGuildList msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.server_id = LocalConfig::Instance().GetMergeServerId();
	msg.is_last = 1;
	msg.is_first = is_first ? 1 : 0;
	is_first = false;
	memcpy(SYNC_GUILD_BUFF, &msg, sizeof(msg));

	TLVSerializer serializer;
	serializer.Reset(SYNC_GUILD_BUFF + sizeof(msg), sizeof(SYNC_GUILD_BUFF) - sizeof(msg));
	bool ret = param.Serialize(serializer);
	gamelog::g_log_guild.printf(LL_INFO, "SyncAllGuildToCross_2 ret:%d msg_length[%d]", (int)ret, (int)(sizeof(msg) + serializer.Size()));

	if (!ret)
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	InternalComm::Instance().SendToHiddenThroughCross(SYNC_GUILD_BUFF, sizeof(msg) + serializer.Size());

	param.count = 0;

	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::SyncAllGuildToCross end");
}

void GuildManager::RequestSyncAllGuild(int server_id, bool is_all_server)
{
	crossgameprotocal::HiddenGameRequestGuildList msg;

	int plat_type = LocalConfig::Instance().GetPlatType();
	if (is_all_server)
	{
		InternalComm::Instance().SendToGameThroughCross(plat_type, 0, (const char*)&msg, sizeof(msg));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, sizeof(msg));
	}
}

void GuildManager::InitGuildBySync(int plat_type, int server_id, const AllGuildParam &guild_list, bool is_first, bool is_last)
{
	for (int i = 0; i < guild_list.count; ++i)
	{
		gamelog::g_log_guild.printf(LL_INFO, "InitGuildBySync guild_id:%d guild_name:%s", guild_list.guild_list[i].base_param.guild_id, guild_list.guild_list[i].base_param.guild_name);
		this->AddGuild(guild_list.guild_list[i], false, true);		
	}
	
	if (is_last)
	{
		this->AddServerToSyncSuccRecorder(plat_type, server_id);

		crossgameprotocal::HiddenGameSyncGuildSuccNotice msg;
		InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&msg, sizeof(msg));
	}
	
}

bool GuildManager::OnServerStart()
{
	RMIInitGuildBackObjectImpl *impl = new RMIInitGuildBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitGuildAsyn(0, impl);
}

void GuildManager::OnServerStop()
{
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		it->second->Save();
	}
}

bool GuildRecommendSortFunction(const Guild * g1, const Guild * g2)
{
	if (!g1 && !g2)
		return false;

	if (g1 && !g2)
		return true;
	
	if (!g1 && g2)
		return false;

	return g1->GetSevenDayTotalActive() > g2->GetSevenDayTotalActive();
}

void GuildManager::OnRoleLogin(Role * role)
{
	if (NULL == role) return;

	Guild * guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild)
	{
		guild = this->GetGuild(role->GetUserId());
		if (NULL == guild || NULL == guild->GetMemberManager()->GetMember(role->GetUserId()))
		{
			role->SetGuildID(0);
			role->SetGuildPostion(-1);
			this->SendMyApplyList(role);
			guild = NULL;
		}
	}
	else if (NULL == guild->GetMemberManager()->GetMember(role->GetUserId()))
	{
		role->SetGuildID(0);
		role->SetGuildPostion(-1);
		this->SendMyApplyList(role);
		guild = NULL;
	}

	if (!guild)
	{
		return;
	}

	role->SetGuildID(guild->GetGuildID(), true);
	guild->OnRoleLogin(role);
	//默认上线设置一次,防止离线时家族升级
	role->GetRoleModuleManager()->GetRoleGuildInborn()->OnSetGuildLevel(guild->GetGuildLevel());
}

void GuildManager::OnRoleLogout(Role * role)
{
	if (NULL == role) return;

	Guild * guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return;

	guild->OnRoleLogout(role);
	role->SetGuildID(guild->GetGuildID());
}

void GuildManager::OnRoleLevelUp(Role* role, int level)
{
	if (NULL == role) return;

	Guild * guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return;

	guild->OnRoleLevelUp(role, level);
}

void GuildManager::GuildRecommend(Role * role)
{
	Guild * guild = this->GetGuild(role->GetGuildID());

	time_t now = EngineAdapter::Instance().Time();
	CommonDataParam & common_data = role->GetRoleModuleManager()->GetCommonData();
	if (now >= common_data.next_can_guild_recommend_time && FunOpen::IsFunOpen(role, FUN_TYPE_GUILD) && !CrossConfig::Instance().IsHiddenServer())
	{
		/*
		 ——推荐最高活跃那个家族给玩家
		——不向玩家发送自身所在家族的邀请
		——如果玩家加入一个新家族未满3个自然日，不进行邀请
		——对玩家已经发送邀请后，在七个自然日内，不再对该玩家进行邀请
		——如果玩家拒绝了家族邀请，那么十四个自然日内，不再对该玩家进行邀请
		*/

		std::vector<Guild *> guild_list;
		for (GuildMapIt it = m_guild_map.begin(); it != m_guild_map.end(); ++it)
		{
			Guild * curr_guild = it->second;
			if (NULL == curr_guild) continue;

			guild_list.push_back(curr_guild);
		}

		std::sort(guild_list.begin(), guild_list.end(), GuildRecommendSortFunction);

		for (int i = 0; i < (int)guild_list.size(); ++i)
		{
			Guild * curr_guild = guild_list[i];
			if (!curr_guild)
			{
				continue;
			}

			if (curr_guild->GetEmptyMemberNum() <= 0)
			{
				continue;
			}

			if (guild)
			{
				GuildMember * guild_member = guild->GetMemberManagerRef().GetMember(role->GetUID());
				if (guild_member && now < guild_member->GetJoinTime() + 3 * SECOND_PER_DAY)
				{
					break;
				}

				if (curr_guild->GetGuildID() == guild->GetGuildID())
				{
					break;
				}

				if (curr_guild->GetSevenDayTotalActive() <= guild->GetSevenDayTotalActive())
				{
					break;
				}
			}

			common_data.next_can_guild_recommend_time = (unsigned int)(now + SECOND_PER_WEEK);
			Protocol::SCGuildRecommend guild_recommend;
			guild_recommend.guild_id = curr_guild->GetGuildID();
			curr_guild->GetName2(guild_recommend.guild_name);
			curr_guild->GetPatriarchName(guild_recommend.role_name);
			EngineAdapter::Instance().NetSend(role->GetNetId(), &guild_recommend, sizeof(guild_recommend));
			break;
		}
	}
}

void RMICreateGuildBackObjectImpl::__exception(int error)
{
	RMICreateGuildBackObject::__exception(error);

	GuildManager::Instance().RemoveCreatingGuildRole(UidToInt(user_id));
}

void RMICreateGuildBackObjectImpl::__timeout()
{
	RMICreateGuildBackObject::__timeout();

	GuildManager::Instance().RemoveCreatingGuildRole(UidToInt(user_id));
}

void RMICreateGuildBackObjectImpl::CreateGuildRet(int ret, GuildID guild_id)
{
	GuildManager::Instance().RemoveCreatingGuildRole(UidToInt(user_id));

	if (0 == ret && guild_id > 0) // 1：DB创建成功
	{
		guild_base.guild_id = guild_id;
		static AllGuildParam::GuildParam guild_param;
		memcpy(&guild_param.base_param, &guild_base, sizeof(guild_param.base_param));

		Guild *guild = GuildManager::Instance().AddGuild(guild_param, true);
		if (NULL != guild)
		{
			// 把族长塞到成员列表里
			GuildMember * member = guild->GetMemberManager()->AddMember(user_id, name, GUILD_POST_PATRIARCH);
			if (NULL == member)
			{
				printf("%s Error -1 \n", __FUNCTION__);
			}

			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
			if (NULL != role)
			{
				Protocol::SCCreateRet ret;
				ret.param = Protocol::SCCreateRet::CREATE;
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&ret, sizeof(ret));
			}

			if (!CrossConfig::Instance().IsHiddenServer())
			{
				crossgameprotocal::GameHiddenSyncGuildChange msg;
				msg.guild_id = guild->GetGuildID();
				msg.plat_type = LocalConfig::Instance().GetPlatType();
				msg.op_type = crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_ADD;
				guild->GetInitParam(&msg.param, false);
				InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
			}

			if (NULL != role)
			{
				role->Save(); // 立刻保存玩家数据，否则游戏服挂掉可导致CommonData中的guild_id未保存
			}
		}
	}
}

int GuildManager::CreateGuild(Role * role, Protocol::CSCreateGuild* req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	if (NULL != this->GetGuild(role->GetGuildID()))
	{
		role->NoticeNum(errornum::EN_ALREADY_IN_GUILD);
		return -1;
	}

	if (m_creating_guild_role.find(role->GetUID()) != m_creating_guild_role.end())
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return -111;
	}

	// 判断玩家等级
	if (role->GetLevel() < CREATE_GUILD_LEVEL_LIMTI)
	{
		role->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return -2;
	}

	if (strncmp(req->name, "", sizeof(req->name)) == 0)
	{
		role->NoticeNum(errornum::EN_GUILD_NAME_EMPTY);
		return -10;
	}

	// 判断家族名是否存在
	if (this->IsInvildName(req->name))
	{
		role->NoticeNum(errornum::EN_GUILD_INVILD_NAME);
		return -11;
	}

	if (strncmp(req->banner, "", sizeof(req->banner)) == 0)
	{
		role->NoticeNum(errornum::EN_GUILD_BANNER_EMPTY);
		return -12;
	}

	if (this->IsBannerUsed(req->banner))
	{
		role->NoticeNum(errornum::EN_GUILD_BANNER_IS_USED);
		return -14;
	}

	if (NameFilter::Instance().HasSensitiveWord(sizeof(req->banner), req->banner))
	{
		role->NoticeNum(errornum::EN_GUILD_BANNER_SENSITIVE_WORDS);
		return -13;
	}

	if (InternalComm::Instance().GetDBHandler().GetGlobalDBSession().IsNeedReconnect())
	{
		role->NoticeNum(errornum::EN_DB_SESSION_LOST);
		return -15;
	}

	bool is_discount = false;
	{
		// 判断货币
		int need_money_type = 0, need_money_num = 0;
		if (role->GetRoleModuleManager()->GetRoleActivityManager()->GetCommonParam().history_chongzhi_num > 0 &&
			role->GetRoleModuleManager()->GetCommonData().guild_create_consume_coin_bind == 0)	// 判断是否可以用铜币创建
		{
			need_money_type = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->first_charge_create_cost.first;
			need_money_num = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->first_charge_create_cost.second;
			is_discount = true;
		}
		else
		{
			need_money_type = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->create_cost_type;
			need_money_num = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->create_cost;
		}

		if (!role->GetRoleModuleManager()->GetMoney()->MoneyTypeMoreThan(need_money_type, need_money_num))
		{
			role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
			return -3;
		}

		// 消耗货币
		if (!role->GetRoleModuleManager()->GetMoney()->UseMoneyType(need_money_type, need_money_num, __FUNCTION__))
		{
			role->NoticeNum(errornum::EN_ACTIVITY_SILK_ROADS_MONEY_NOT_ENOUGH);
			return -4;
		}
	}

	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	// 创建申请
	{
		static GuildBaseParam guild_param;
		guild_param.Reset();

		guild_param.create_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		F_STRNCPY(guild_param.guild_name, req->name, sizeof(GuildName));
		F_STRNCPY(guild_param.guild_notice, req->notice, sizeof(GuildNotice));
		F_STRNCPY(guild_param.guild_banner, req->banner, sizeof(GuildBanner));

		guild_param.guild_level = 1;
		guild_param.build_flag = LOGIC_CONFIG->GetGuildConfig()->GetGuildBuildInit();
		guild_param.guild_exp = other_cfg.initial_funding;
		guild_param.origin_plat_type = LocalConfig::Instance().GetPlatType();
		guild_param.origin_server_id = LocalConfig::Instance().GetMergeServerId();

		RMICreateGuildBackObjectImpl *cgbo = new RMICreateGuildBackObjectImpl();
		cgbo->guild_base = guild_param;
		cgbo->user_id = role->GetUserId();
		role->GetName(cgbo->name);
		RMIGlobalClient gc;
		gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		bool ret = gc.CreateGuildAsyn(role->GetUserId().db_index, cgbo->guild_base, cgbo);
		if (!ret)
		{
			printf("%s Error %d \n", __FUNCTION__, ret);
			gamelog::g_log_guild.printf(LL_INFO, "GuildManager::CreateGuild rmi async send failed! role[%d,%s]", role->GetUID(), role->GetName());
			return -100;
		}
		else
		{
			int hash = GetStringHash(sizeof(GuildBanner), req->banner);
			GuildManager().Instance().m_guild_banner_hash_set.insert(hash);

			role->Save(); // 立刻保存玩家数据 若创建家族后游戏服立刻挂掉，则玩家免费创建了家族

			m_creating_guild_role.insert(role->GetUID());
		}
	}

	if (is_discount)
	{
		role->GetRoleModuleManager()->GetCommonData().guild_create_consume_coin_bind = 1;
		role->GetRoleModuleManager()->GetRoleActivityManager()->SendFristChongzhiReward();
	}

	return 0;
}

int GuildManager::QuitGuild(Role * role, Protocol::CSQuitGuild * req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}
	if (ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_FIGHT))
	{
		role->NoticeNum(errornum::EN_GUILD_GUILD_FIGHT_OPEN_NOT_QUIT_GUILD);
		return -2;
	}

	int guild_id = req->guild_id;
	Guild *guild = this->GetGuild(guild_id);
	if (NULL == guild) return -3;

	GuildMemberManager & mem_mgr = guild->GetMemberManagerRef();

	if (mem_mgr.GetMemberCount() <= 1)
	{// 解散
		this->DismissGuild(guild_id);
	}
	else
	{// 退出
		if (mem_mgr.GetPatriarchUserID() == role->GetUserId()) return -2;

		if (!role->GetScene()->GetSpecialLogic()->CanLeaveGuild())
		{
			role->NoticeNum(errornum::EN_THIS_SCENE_FORBID_LEAVE_GUILD);
			return -4;
		}

		return mem_mgr.RemoveMember(role->GetUserId(), Protocol::SCQuitGuild::QUIT);
	}

	return 0;
}

void RMIDismissGuildBackObjectImpl::DismissGuildRet(int ret)
{
	if (0 == ret) // 1：DB解散成功
	{
		GuildManager::Instance().RemoveGuild(guild_id);
	}
}

int GuildManager::DismissGuild(GuildID guild_id)
{
	Guild *guild = this->GetGuild(guild_id);
	if (NULL == guild) return -1;

	{
		RMIDismissGuildBackObjectImpl *dgbo = new RMIDismissGuildBackObjectImpl();

		dgbo->guild_id = guild_id;

		RMIGlobalClient gc;
		gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		return gc.DismissGuildAsyn(dgbo->guild_id, dgbo);
	}
}

void GuildManager::SendMyApplyList(Role * role)
{
	if (NULL == role) return;

	Protocol::SCPlayerApplyList pal;
	pal.apply_count = 0;

	std::map<int, std::set<int> >::iterator find_it = m_user_apply_guild.find(role->GetUID());
	if (m_user_apply_guild.end() != find_it)
	{
		for (std::set<int>::iterator loop = find_it->second.begin(); loop != find_it->second.end(); ++loop)
		{
			if (pal.apply_count >= PLAYER_APPLY_COUNT_MAX) break;
			pal.guild_id_list[pal.apply_count++] = *loop;
		}
	}
	
	int length = sizeof(pal) - (PLAYER_APPLY_COUNT_MAX - pal.apply_count) * sizeof(pal.guild_id_list[0]);
	if (length > 0) role->GetRoleModuleManager()->NetSend((const void *)&pal, length);
}

bool GuildManager::IsBannerUsed(GuildBanner banner)
{
	int hash = GetStringHash(sizeof(GuildBanner), banner);
	return m_guild_banner_hash_set.find(hash) != m_guild_banner_hash_set.end();
}

void GuildManager::RefreshGuildBossChapterRank()
{
	m_guild_boss_chapter_rank_list.clear();
	for (GuildMap::iterator it = m_guild_map.begin(); it != m_guild_map.end(); it++)
	{
		if (NULL != it->second)
		{
			static GuildBossChapterRank info; info.Reset();
			it->second->GetGuildBossChapterInfo(&info);
			m_guild_boss_chapter_rank_list.push_back(info);
		}
	}
	if (!m_guild_boss_chapter_rank_list.empty())
	{
		std::sort(m_guild_boss_chapter_rank_list.begin(), m_guild_boss_chapter_rank_list.end());
	}
}

void GuildManager::QueryHiddenIfSyncGuildSucceed()
{
	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::QueryHiddenIfSyncGuildSucceed next query time[%u]", m_check_sync_succ_timestamp);

	crossgameprotocal::GameHiddenQueryIfSyncGuildSucceed msg;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.server_id = LocalConfig::Instance().GetMergeServerId();

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
}

bool GuildManager::IsServerSyncGuildSucc(int plat_type, int server_id)
{
	return m_sync_guild_succ_recorder.find(server_id) != m_sync_guild_succ_recorder.end();
}

void GuildManager::AddServerToSyncSuccRecorder(int plat_type, int server_id)
{
	m_sync_guild_succ_recorder.insert(server_id);

	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::AddServerToSyncSuccRecorder plat_type[%d] server_id[%d]", plat_type, server_id);
}

void GuildManager::RemoveAllServerToSyncSuccRecorder()
{
	m_sync_guild_succ_recorder.clear();

	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::RemoveAllServerToSyncSuccRecorder");
}

void GuildManager::RemoveServerToSyncSuccRecorder(int plat_type, int server_id)
{
	m_sync_guild_succ_recorder.erase(server_id);

	gamelog::g_log_guild.printf(LL_INFO, "GuildManager::RemoveServerToSyncSuccRecorder plat_type[%d] server_id[%d]", plat_type, server_id);
}

int GuildManager::ApplyForJoin(Role * role, GuildID guild_id)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	if (!role->GetRoleModuleManager()->CanApplyJoinGuild())
	{
		role->NoticeNum(errornum::EN_GUILD_FREQUENTLY_QUILD);
		return -2;
	}

	if (NULL != this->GetGuild(role->GetGuildID())) return -3;
	
	int uid = role->GetUID();
	std::map<int, std::set<int> >::iterator apply_it = m_user_apply_guild.find(uid);
	if (m_user_apply_guild.end() != apply_it && (int)apply_it->second.size() >= PLAYER_APPLY_COUNT_MAX)
	{
		role->NoticeNum(errornum::EN_GUILD_APPLY_LIMIT_NUM);
		return -10;
	}

	Guild *guild = this->GetGuild(guild_id);
	if (NULL == guild) return -2;

	int max_count = guild->GetMaxMemberCount();
	if (guild->GetMemberManager()->GetMemberCount() >= max_count) return -5;

	if (guild->IsNeedApproveJoin())
	{
		if (!guild->GetMemberManager()->AddApplyFor(role))
		{
			return -4;
		}

		std::map<int, std::set<int> >::iterator find_it = m_user_apply_guild.find(uid);
		if (m_user_apply_guild.end() != find_it)
		{
			find_it->second.insert(guild_id);
		}
		else
		{
			std::set<int> tmp_set; tmp_set.insert(guild_id);
			m_user_apply_guild[uid] = tmp_set;
		}

		this->SendMyApplyList(role);
	}
	else
	{
		guild->JoinGuild(role);
	}

	return 0;
}

int GuildManager::ApplyForJoinOneClick(Role * role)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	/*if (!role->GetRoleModuleManager()->CanApplyJoinGuild())
	{
		role->NoticeNum(errornum::EN_GUILD_FREQUENTLY_QUILD);
		return -2;
	}*/

	if (NULL != this->GetGuild(role->GetUserId())) return -3;

	int uid = role->GetUID(), apply_num = 0;
	std::map<int, std::set<int> >::iterator apply_it = m_user_apply_guild.find(uid);
	if (m_user_apply_guild.end() != apply_it) apply_num = apply_it->second.size();

	if (apply_num >= PLAYER_APPLY_COUNT_MAX)
	{
		role->NoticeNum(errornum::EN_GUILD_APPLY_LIMIT_NUM);
		return -10;
	}
	
	int resi_num = PLAYER_APPLY_COUNT_MAX - apply_num, count = 0;
	for (GuildMapConstIt it = m_guild_map.begin(); it != m_guild_map.end() && count < resi_num; ++it)
	{
		if (role->GetGuildID() != 0)break;//每次循环检测一下是否已经被加入家族

		Guild * guild = it->second;
		if (NULL == guild || guild->GetMemberManager()->GetMemberCount() >= guild->GetMaxMemberCount()) continue;

		if (guild->IsNeedApproveJoin())
		{
			if (!guild->GetMemberManager()->AddApplyFor(role, false)) continue;
			++count;

			int guild_id = guild->GetGuildID();
			std::map<int, std::set<int> >::iterator find_it = m_user_apply_guild.find(uid);
			if (m_user_apply_guild.end() != find_it)
			{
				find_it->second.insert(guild_id);
			}
			else
			{
				std::set<int> tmp_set; tmp_set.insert(guild_id);
				m_user_apply_guild[uid] = tmp_set;
			}
		}
		else
		{
			if (guild->JoinGuild(role))
			{
				break;
			}			
		}
	}

	this->SendMyApplyList(role);
	role->NoticeNum(errornum::EN_GUILD_APPLY_SUCC);
	return 0;
}

int GuildManager::ApplyForJoinAck(Role * role, Protocol::CSApplyJoinGuildAck * join_ack)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(join_ack->guild_id);
	if (NULL == guild) return -2;

	// 判断申请人是否有家族
	Guild *apply_guild = this->GetGuild(IntToUid(join_ack->uid));
	if (NULL != apply_guild)
	{
		role->NoticeNum(errornum::EN_GUILD_APPLICANT_HAS_GUILD);
		guild->GetMemberManager()->ApplyJoinAck(IntToUid(join_ack->uid), 0);
		return -3;
	}

	// 判断审批人是否在家族内
	GuildMember *verify_member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == verify_member)
	{
		guild->GetMemberManager()->ApplyJoinAck(IntToUid(join_ack->uid), 0);
		return -4;
	}

	// 判断审批人权限
	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(verify_member->GetPost());
	if (NULL == member_type_cfg) return -9;
	if (0 == member_type_cfg->privilege[APPROVAL])
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	// 判断家族是否有空位
	if (!guild->GetMemberManager()->IsHavePlace()) return -6;

	// 审批
	if (!guild->GetMemberManager()->ApplyJoinAck(IntToUid(join_ack->uid), join_ack->result)) return -8;


	UserCacheNode* leadernode = UserCacheManager::Instance().GetUserNode(role->GetGuild()->GetMemberManagerRef().GetPatriarchUserID());
	this->NewbieJoinGuild(leadernode,join_ack->uid, guild->GetGuildID());
	guild->SendGuildBaseInfo(World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(join_ack->uid)));
	return 0;
}

int GuildManager::ExpelOut(Role * role, Protocol::CSGuildExpelOutReq * req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	if (role->GetUID() == req->expel_uid)
	{
		role->NoticeNum(errornum::EN_GUILD_CAN_NOT_KICKOUT_SELF);
		return -11;
	}
	// 判断家族是否存在
	Guild *guild = this->GetGuild(req->guild_id);
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	UserID user_id = IntToUid(req->expel_uid);
	GuildMember *expel_member = guild->GetMemberManager()->GetMember(user_id);
	if (NULL == expel_member) return -3;

	GuildMember *verify_member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == verify_member) return -4;

	// 判断权限
	int my_post = verify_member->GetPost();
	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(my_post);
	if (NULL == member_type_cfg || 0 == member_type_cfg->privilege[KICK])
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	// 判断我的职位是否比对方高
	int his_post = expel_member->GetPost();
	if (my_post >= his_post) // post值越大，职位越低
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -7;
	}

	Role * target = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != target && !target->GetScene()->GetSpecialLogic()->CanLeaveGuild())
	{
		role->NoticeNum(errornum::EN_TARGET_SCENE_FORBID_LEAVE_GUILD);
		return -6;
	}

	return guild->GetMemberManager()->RemoveMember(user_id, Protocol::SCQuitGuild::EXPEL_OUT);
}

int GuildManager::SetGuildNotice(Role * role, Protocol::CSSetGuildNoticeReq * req)
{
	if (NULL == role) return -1;

	// 判断家族是否存在
	Guild *guild = this->GetGuild(req->guild_id);
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMember *member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == member) return -3;

	// 判断权限
	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(member->GetPost());
	if (NULL == member_type_cfg || 0 == member_type_cfg->privilege[NOTICE])
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	return guild->SetNotice(role, req->notice);
}

int GuildManager::GetGuildInfo(Role * role, Protocol::CSGetGuildList * req)
{
	switch (req->req_type)
	{
	case Protocol::CSGetGuildList::GET_GUILD_BUILD_INFO:
		{
			Guild* guild = this->GetGuild(req->param);
			if (NULL != guild)
			{
				//guild->SendGuildBuildInfo(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_LIST:
		{
			this->SendGuildList(role);
		}
		break;

	case Protocol::CSGetGuildList::GET_GUILD_NOTICE:
		{
			this->SendGuildNotice(role, req->param);
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_BASE_INFO:
		{
			Guild* guild = NULL;
			if (0 == req->param)
			{
				guild = this->GetGuild(role->GetGuildID());
			}
			else
			{
				guild = this->GetGuild(req->param);
			}

			if (NULL != guild)
			{
				guild->SendGuildBaseInfo(role);
				guild->SendAllBuildInfo(role);
				guild->SendSpeedUpInfo(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_MEMBER_LIST:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->SendGuildMemberInfo(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_APPLY_LIST:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->GetMemberManager()->SendApplyList(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_TASK_INFO:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->SendGuildWeekTaskInfo(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_TASK_RANK_INFO:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->SendContrRankListInfo(role);
			}
		}
		break;
	case Protocol::CSGetGuildList::GET_GUILD_GATHER_DREAM_LIST:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->SendGatherDreamList(role);
			}
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_PUBLISH_GATHER_DREAM:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->OnPublishGatherDream(role, req->param, req->param1);
			}
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_GIFT_GATHER_DREAM:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->OnGiftGatherDream(role, req->param);
			}
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_FETCH_GATHER_DREAM:
		{
			Guild* guild = this->GetGuild(role->GetGuildID());
			if (NULL != guild)
			{
				guild->OnFetchGatherDream(role);
			}
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_GIFT_GATHER_DREAM_LIST:
		{
			GatherDreamRecordManager::Instance().SendRecordListInfo(role, GATHER_DREAM_INFO_TYPE_GIFT);
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_GET_GATHER_DREAM_LIST:
		{
			GatherDreamRecordManager::Instance().SendRecordListInfo(role, GATHER_DREAM_INFO_TYPE_GET);
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_ROLE_GIFT_GATHER_DREAM_LIST:
		{
			role->GetRoleModuleManager()->GetRoleGatherDream()->SendGatherDreamList();
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_ROLE_CAN_PULISH_LIST:
		{
			role->GetRoleModuleManager()->GetRoleGatherDream()->SendCanPublishList();
		}
		break;
		case Protocol::CSGetGuildList::GET_GUILD_ROLE_SET_PUBLISH_SEND:
		{
			role->GetRoleModuleManager()->GetRoleGatherDream()->SetPublishSendFlag(req->param);
		}
		break;
		case Protocol::CSGetGuildList::TAN_HE_ZU_ZHANG:
			{
				Guild * guild = this->GetGuild(role->GetGuildID());
				if (NULL != guild)
				{
					guild->TanHeZuZhang(role);
				}
			}
			break;
		case Protocol::CSGetGuildList::REFUSE_GUILD_RECOMMEND:
			{
				CommonDataParam & common_data = role->GetRoleModuleManager()->GetCommonData();
				common_data.next_can_guild_recommend_time += SECOND_PER_WEEK;
			}
			break;
		case Protocol::CSGetGuildList::REQ_GUILD_RECOMMEND:
			{
				GuildManager::Instance().GuildRecommend(role);
			}
			break;
	}
	return 0;
}

int GuildManager::GuildTrainingUp(Role * role, int type)
{
	if (NULL == role)return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	if (type <= PET_TRAINING_TYPE_MIN || type >= PET_TRAINING_TYPE_MAX) return -2;

	int &level = role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[type];

	const GuildPetTrainingCfg *cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(type, level + 1);
	if (NULL == cfg)return -3;

	// 家族等级不够 不能升级
	Guild* guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -11;
	if (guild->GetGuildLevel() < cfg->guild_level) return -12;

	Money* money = role->GetRoleModuleManager()->GetMoney();

	// 判断消耗
	if (money->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION) < cfg->gongxian_cost)
	{
		return -4;
	}
	
	switch (cfg->coin_type)
	{
	case MONEY_TYPE_GOLD:
		if (!money->GoldMoreThan(cfg->coin_cost))
		{
			return -5;
		}
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		if (!money->AllCoinMoreThan(cfg->coin_cost))
		{
			return -5;
		}
		break;
	case MONEY_TYPE_COIN_BIND:
		if (!money->CoinBindMoreThan(cfg->coin_cost))
		{
			return -5;
		}
		break;
	}

	// 消耗
	money->UseOtherCoin(cfg->gongxian_cost, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);

	switch (cfg->coin_type)
	{
	case MONEY_TYPE_GOLD:
		money->UseGold(cfg->coin_cost, __FUNCTION__);
		break;
	case MONEY_TYPE_IMMORTAL_COIN:
		money->UseAllCoin(cfg->coin_cost, __FUNCTION__);
		break;
	case MONEY_TYPE_COIN_BIND:
		money->UseCoinBind(cfg->coin_cost, __FUNCTION__);
		break;
	}

	//等级提升
	level++;
	this->SendPetTrainingInfo(role);
	return 0;
}

int GuildManager::GuildTraining(Role * role, int pet_index, int places_index)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	int max_index = 0;
	const GuildPetTrainingCfg * max_index_cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(PET_FRIENDS, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[PET_FRIENDS]);
	if (NULL != max_index_cfg)
	{
		max_index += max_index_cfg->add_param;
	}
	if (places_index < 0 || places_index >= GUILD_PET_TRAINING_PET_MAX_COUNT || places_index > max_index) return -2;
	
	GuildTrainingPetParam& training_param = role->GetRoleModuleManager()->GetGuildCommonData().guild_training_pet_list[places_index];

	// 如果没有宠物正在训练
	if (training_param.pet_index < 0)
	{
		for (int i = 0; i < GUILD_PET_TRAINING_PET_MAX_COUNT; ++i)
		{
			// 如果你要训练的宠物已经在训练中
			int training_index = role->GetRoleModuleManager()->GetGuildCommonData().guild_training_pet_list[i].pet_index;
			if (pet_index == training_index)
			{
				role->NoticeNum(errornum::EN_GUILD_PET_TRAINING_ALREADY);
				return -13;
			}
		}

		const PetParam *pet_param = role->GetRoleModuleManager()->GetPet()->GetPetParam(pet_index);
		if (NULL == pet_param)
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return -3;
		}

		const PetLevelCfg * level_cfg = LOGIC_CONFIG->GetPetConfig()->GetLevelCfg(pet_param->level);
		if (level_cfg == NULL)
		{ 
			return -301;
		}

		if (level_cfg->is_need_breach && pet_param->is_up_level_break == 0)
		{
			role->NoticeNum(errornum::EN_GUILD_PET_TRAINING_ERR_WITHOUT_BREAK_LEVEL);
			return -302;
		}

		// 如果你要训练的宠物是出战宠物 就训练失败
		if (pet_index == role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[NEW_FIGHT_INDEX])
		{
			role->NoticeNum(errornum::EN_PET_IS_FIGHT);
			return -12;
		}
		else
		{
			for (int i = NEW_STANDBY_INDEX1; i <= NEW_STANDBY_INDEX5; ++i)
			{
				// 如果你要训练的宠物是待机宠物 就把待机宠物下掉
				if (pet_index == role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[i])
				{
					role->GetRoleModuleManager()->GetCommonData().new_pet_status_index[i] = -1;
					role->GetRoleModuleManager()->GetPet()->SendPetOtherInfo();
					break;
				}
			}
		}

		int gongxian_cost = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingGongXian(pet_param->level);
		if (!role->GetRoleModuleManager()->GetMoney()->OtherCoinMoreThan(gongxian_cost, MONEY_TYPE_GUILD_CONTRIBUTION))
		{
			role->NoticeNum(errornum::EN_GUILD_GONGXIAN_NOT_ENOUGH);
			return -14;
		}

		role->GetRoleModuleManager()->GetMoney()->UseOtherCoin(gongxian_cost, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
		training_param.pet_index = pet_index;
		training_param.begin_time = (unsigned int)EngineAdapter::Instance().Time();
	}
	else
	{
		const PetParam *pet_param = role->GetRoleModuleManager()->GetPet()->GetPetParam(training_param.pet_index);
		if (NULL == pet_param)
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return -3;
		}

		unsigned int max_time = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->training_max_time;
		//计算强化增加的训练时间
		const GuildPetTrainingCfg * time_cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(TRAINING_ENDURANCE, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[TRAINING_ENDURANCE]);
		if (NULL != time_cfg)
		{
			max_time += time_cfg->add_param;
		}

		double exp_inc_rate = 1.0;
		const GuildPetTrainingCfg * exp_cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(TRAINING_EXPERIENCE, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[TRAINING_EXPERIENCE]);
		if (NULL != exp_cfg)
		{
			exp_inc_rate += exp_cfg->add_param / 100.0;
		}

		// 计算训练时间 
		unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();
		unsigned int training_time = now_time - training_param.begin_time;
		if (training_time > max_time)	// 如果大于上限
		{
			training_time = max_time;
		}

		// 每份经验有多少
		int exp_per = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingExp(pet_param->level);
		exp_per = int(exp_per * exp_inc_rate);

		int add_exp = (int)(training_time / LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->exp_time * exp_per);

		if (add_exp > 0)
		{
			role->GetRoleModuleManager()->GetPet()->AddExp(training_param.pet_index, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
		}

		static Protocol::SCGuildPetTrainingExp info;
		info.pet_index = training_param.pet_index;
		info.time = training_time;
		info.exp = add_exp;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
		training_param.Reset();
	}

	this->SendPetTrainingInfo(role);
	return 0;
}

void GuildManager::ClearAllGuildTraining(Role * role,time_t end_tamp)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}
	
	for (int i = 0; i < GUILD_PET_TRAINING_PET_MAX_COUNT;i++)
	{
		GuildTrainingPetParam& training_param = role->GetRoleModuleManager()->GetGuildCommonData().guild_training_pet_list[i];
		// 如果没有宠物正在训练
		if (training_param.pet_index < 0)
		{
			continue;
		}
		else
		{
			const PetParam *pet_param = role->GetRoleModuleManager()->GetPet()->GetPetParam(training_param.pet_index);
			if (NULL == pet_param)
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
				return;
			}

			unsigned int max_time = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->training_max_time;
			//计算强化增加的训练时间
			const GuildPetTrainingCfg * time_cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(TRAINING_ENDURANCE, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[TRAINING_ENDURANCE]);
			if (NULL != time_cfg)
			{
				max_time += time_cfg->add_param;
			}

			double exp_inc_rate = 1.0;
			const GuildPetTrainingCfg * exp_cfg = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingCfg(TRAINING_EXPERIENCE, role->GetRoleModuleManager()->GetGuildCommonData().guild_training_level[TRAINING_EXPERIENCE]);
			if (NULL != exp_cfg)
			{
				exp_inc_rate += exp_cfg->add_param / 100.0;
			}

			// 计算训练时间 
			unsigned int now_time = (unsigned int)end_tamp;
			unsigned int training_time = now_time - training_param.begin_time;
			if (training_time > max_time)	// 如果大于上限
			{
				training_time = max_time;
			}

			// 每份经验有多少
			int exp_per = LOGIC_CONFIG->GetGuildConfig()->GetPetTrainingExp(pet_param->level);
			exp_per = int(exp_per * exp_inc_rate);

			int add_exp = (int)(training_time / LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->exp_time * exp_per);

			if (add_exp > 0)
			{
				role->GetRoleModuleManager()->GetPet()->AddExp(training_param.pet_index, add_exp, __FUNCTION__, Protocol::SCSendPetSingleInfo::PET_NOTIE_TYPE_EXP);
			}

			static Protocol::SCGuildPetTrainingExp info;
			info.pet_index = training_param.pet_index;
			info.time = training_time;
			info.exp = add_exp;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
			training_param.Reset();
		}
	}

	this->SendPetTrainingInfo(role);
}

int GuildManager::OnGuildAbdicationOp(Role * role, Protocol::CSGuildAbdicationOp * req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	if (role->GetUID() == req->uid)
	{
		role->NoticeNum(errornum::EN_GUILD_CAN_NOT_APPOINT_SELF);
		return -11;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(req->guild_id);
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMember *old_patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == old_patriarch) return -3;

	GuildMember *new_patriarch = guild->GetMemberManager()->GetMember(IntToUid(req->uid));
	if (NULL == new_patriarch) return -4;

	// 判断权限
	if (GUILD_POST_PATRIARCH != old_patriarch->GetPost())
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	old_patriarch->SetPost(GUILD_POST_MEMBER);
	new_patriarch->SetPost(GUILD_POST_PATRIARCH);
	guild->AddEventhandler(new_patriarch->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, GUILD_POST_PATRIARCH);
	guild->SendGuildBaseInfo();

	guild->Save(); //!< 转让族长需要立马保存 \bug #43281

	return 0;
}

int GuildManager::OnGuildImpeachmentOp(Role * role, Protocol::CSGuildImpeachmentOp * req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(req->guild_id);
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMemberManager & mem_mgr = guild->GetMemberManagerRef();
	UserID patriarch_user_id = mem_mgr.GetPatriarchUserID();
	if (role->GetUID() == UidToInt(patriarch_user_id))
	{
		role->NoticeNum(errornum::EN_GUILD_CAN_NOT_IMPEACH_YOURSELF);
		return -20;
	}

	GuildMember *old_patriarch = mem_mgr.GetMember(mem_mgr.GetPatriarchUserID());
	if (NULL == old_patriarch) return -3;

	GuildMember *new_patriarch = mem_mgr.GetMember(role->GetUserId());
	if (NULL == new_patriarch) return -4;

	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(new_patriarch->GetPost());
	if (NULL == member_type_cfg || 0 == member_type_cfg->privilege[IMPEACH])
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -12;
	}

	UserCacheNode * patriarch_cache = UserCacheManager::Instance().GetUserNode(old_patriarch->GetUid());
	if (NULL == patriarch_cache)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return -13;
	}

	unsigned int now_time = (unsigned int)EngineAdapter::Instance().Time();

	// 族长七天未上线才可以弹劾
	if (now_time - patriarch_cache->last_online_timestamp < 7 * SECOND_PER_DAY)
	{
		role->NoticeNum(errornum::EN_GUILD_IMPEACHMENT_CANNOT);
		return -4;
	}

	old_patriarch->SetPost(GUILD_POST_MEMBER);
	new_patriarch->SetPost(GUILD_POST_PATRIARCH);
	guild->AddEventhandler(new_patriarch->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, GUILD_POST_PATRIARCH);
	guild->SendGuildBaseInfo();

	return 0;
}

int GuildManager::OnGuildAppointmentOp(Role * role, Protocol::CSGuildAppointmentOp * op)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(op->guild_id);
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return -2;
	}

	if (role->GetUID() == op->uid)
	{
		role->NoticeNum(errornum::EN_GUILD_CAN_NOT_APPOINT_SELF);
		return -99;
	}

	if (op->uid == guild->GetMemberManager()->GetPatriarchUid())
	{
		// 如果uid为族长的则为弹劾
		Protocol::CSGuildImpeachmentOp req;
		req.guild_id = op->guild_id;
		return this->OnGuildImpeachmentOp(role, &req);
	}
	else if (GUILD_POST_PATRIARCH == op->post)
	{
		// 如果职位为族长的则为让位
		Protocol::CSGuildAbdicationOp req;
		req.guild_id = op->guild_id;
		req.uid = op->uid;
		return this->OnGuildAbdicationOp(role, &req);
	}
	else
	{
		if (op->post < GUILD_POST_PATRIARCH || op->post >= GUILD_POST_MAX) return -11;

		// 判断人是否在家族内
		GuildMember *boss_member = guild->GetMemberManager()->GetMember(role->GetUserId());
		if (NULL == boss_member) return -3;

		GuildMember *subordinate_member = guild->GetMemberManager()->GetMember(IntToUid(op->uid));
		if (NULL == subordinate_member) return -4;

		if (boss_member->GetPost() >= subordinate_member->GetPost() ||
			boss_member->GetPost() >= op->post ||
			boss_member->GetPost() > GUILD_POST_VICE_PATRIARCH)
		{
			role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
			return -5;
		}

		const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(op->post);
		if (NULL == member_type_cfg) return -6;

		if(member_type_cfg->max_num >= 0)
		{
			int now_count = guild->GetMemberManager()->GetPostCount(op->post);
			if (member_type_cfg->max_num <= now_count)
			{
				role->NoticeNum(errornum::EN_GUILD_POST_REACHED_MAXINUM);
				return -7;
			}
		}

		subordinate_member->SetPost(op->post);
		guild->AddEventhandler(subordinate_member->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, op->post);
		guild->SendGuildBaseInfo(role);
	}

	return 0;
}

int GuildManager::OnGuildShopBuy(Role * role, Protocol::CSGuildShopBuy * op)
{
	if (NULL == role) return -1;

	/*if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}*/

	if (op->shop_seq < 0 || op->shop_seq >= GUILD_SHOP_COUNT_MAX || op->count <= 0) return -2;

	const GuildShopCfg *cfg = LOGIC_CONFIG->GetGuildConfig()->GetShopCfg(op->shop_seq);
	if (NULL == cfg) return -3;
	// 判断--------------------------------------------------------
	unsigned short& now_buy_count = role->GetRoleModuleManager()->GetGuildCommonData().shop_buy_list[op->shop_seq];
	if (BUY_LIMIT_TYPE_UNLIMITED != cfg->buy_type && (now_buy_count + op->count) > cfg->buy_num)
	{
		role->NoticeNum(errornum::EN_SHOP_BUY_LIMIT);
		return -4;
	}
	Guild *guild = this->GetGuild(op->guild_id);
	if (NULL == guild) return -11;

	if (guild->GetGuildLevel() < cfg->level)  return -13;

	GuildMember *member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == member) return -12;
	int need_gongxian = op->count * cfg->gongxian_cost;
	int need_coin = op->count * cfg->present_cost;

	//计算家族商铺折扣
	const GuildBaseConstructCfg * g_construct_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_STORE, guild->GetBuildLevel(GUILD_BUILD_TYPE_STORE));
	if (NULL == g_construct_cfg) return -13;
	need_coin = need_coin * g_construct_cfg->paramet_1 / 100;
	need_gongxian = need_gongxian * g_construct_cfg->paramet_1 / 100;

	Money* money = role->GetRoleModuleManager()->GetMoney();
	if (money->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION) < need_gongxian)
	{
		role->NoticeNum(errornum::EN_GUILD_GONGXIAN_NOT_ENOUGH);
		return -5;
	}
	ItemConfigData item_data(cfg->item_id, true, op->count);
	role->GetRoleModuleManager()->GetKnapsack()->OnCheckNorexConvertItem(item_data);

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(item_data, false, false))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
		//role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -15;
	}

	switch (cfg->cost_type)
	{
	case BUY_COST_TYPE_GOLD:
		{
			if (!money->GoldMoreThan(need_coin))
			{
				role->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return -6;
			}
		}
		break;
	case BUY_COST_TYPE_IMMORTAL_COIN:
		{
			if (!money->ImmortalCoinMoreThan(need_coin))
			{
				role->NoticeNum(errornum::EN_BIND_GOLD_NOT_ENOUGH);
				return -6;
			}
		}
		break;
	case BUY_COST_TYPE_BIND_COIN:
		{
			if (!money->CoinBindMoreThan(need_coin))
			{
				role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return -6;
			}
		}
		break;
	default:
		return -6;
		break;
	}

	// 消耗--------------------------------------------------------
	if (BUY_LIMIT_TYPE_UNLIMITED != cfg->buy_type) now_buy_count += op->count;
	
	money->UseOtherCoin(need_gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	
	switch (cfg->cost_type)
	{
	case BUY_COST_TYPE_GOLD:
		role->GetRoleModuleManager()->GetMoney()->UseGold(need_coin, __FUNCTION__);
		break;
	case BUY_COST_TYPE_IMMORTAL_COIN:
		role->GetRoleModuleManager()->GetMoney()->UseImmortalCoin(need_coin, __FUNCTION__);
		break;
	case BUY_COST_TYPE_BIND_COIN:
		role->GetRoleModuleManager()->GetMoney()->UseCoinBind(need_coin, __FUNCTION__);
		break;
	}
	// 给予--------------------------------------------------------
	
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(item_data, PUT_REASON_GUILD_SHOP))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
		return -7;
	}
	
	role->GetRoleModuleManager()->SendGuildShopInfo(op->shop_seq);

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_GUILD_STORE);
	}

	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d %s] goods[%d,%d]",
					__FUNCTION__, role->GetUID(), role->GetName(), op->shop_seq, op->count);
	return 0;
}

void GuildManager::OnGuildInviteOther(Role * role, int target_uid)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	Guild * guild = role->GetGuild();
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return;
	}

	GuildMember* member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == member) return;
	if (member->GetPost() > GUILD_POST_ELDER)
	{
		role->NoticeNum(errornum::EN_GUILD_INVITE_NO_AUTHORITY);
		return;
	}


	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == target_role)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
		return;
	}

	if (NULL != target_role->GetGuild())
	{
		role->NoticeNum(errornum::EN_GUILD_OPPOSITE_HAVE_GUILD_YET);
		return;
	}


	time_t now = EngineAdapter::Instance().Time();
	std::map<int, time_t> & interval_limit_time_map=guild->GetIntervalLimitTimeMap();
	time_t & last_invite_time = interval_limit_time_map[target_uid];
	if (now < last_invite_time + 600) //10分钟内不会收到重复邀请
	{
		role->NoticeNum(errornum::EN_GUILD_CANT_INVITE_OTHER_FREQUENTLY);
		return;
	}
	interval_limit_time_map[target_uid] = now;

	// 判断结束，注意不能中途返回
	m_invitee_to_inviter_map[target_uid] = role->GetUID();

	Protocol::SCGuildInvitationNotice gin;
	memcpy(gin.guild_name, guild->GetName(), sizeof(GuildName));
	role->GetName(gin.inviter_name);
	EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char*)&gin, sizeof(gin));

	role->NoticeNum(noticenum::NT_INVITATION_ALREADY_SEND);
}

void GuildManager::OnGuildInviteReply(Role * role, int result)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	// 受邀者
	int invitee_uid = role->GetUID();
	UserID invitee_user_id = IntToUid(invitee_uid);

	UserCacheNode* invitee_node = UserCacheManager::Instance().GetUserNode(invitee_uid);
	if (NULL == invitee_node) return;

	// 邀请者
	int inviter_uid = m_invitee_to_inviter_map[invitee_uid]; // 此处设计有问题， 若有多人同时邀请一个人， 怎么办？
	UserID inviter_user_id = IntToUid(inviter_uid);

	UserCacheNode* inviter_node = UserCacheManager::Instance().GetUserNode(inviter_uid);
	if (NULL == inviter_node) return;

	if (0 == result)		// 如果拒绝
	{
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				gamestring::g_refuse_invite, inviter_node->GetNamePtr());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(invitee_user_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
		}
		
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				gamestring::g_refuse_invite_notice, invitee_node->GetNamePtr());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(inviter_user_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
		}

		return;
	}

	// 如果同意 ->
	if (NULL != role->GetGuild())
	{
		role->NoticeNum(errornum::EN_GUILD_YOU_HAVE_GUILD);
		return;
	}

	Guild * guild = this->GetGuild(IntToUid(inviter_uid));
	if (NULL == guild)
	{
		role->NoticeNum(errornum::EN_GUILD_OPPOSITE_IS_NOT_IN_GUILD);
		return;
	}

	if (!guild->GetMemberManager()->IsHavePlace())
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF),
				      gamestring::g_agree_invite_but_full);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
		return;
	}

	if (guild->IsNeedApproveJoin())
	{
		GuildMember * mem_info = guild->GetMemberManagerRef().GetMember(inviter_user_id);
		if (NULL == mem_info)
		{
			role->NoticeNum(errornum::EN_GUILD_OPPOSITE_IS_NOT_IN_GUILD);
			return;
		}

		switch (mem_info->GetPost())		// 如果邀请人是权限汪，直接加入
		{
			case GUILD_POST_PATRIARCH:
			case GUILD_POST_VICE_PATRIARCH:
			case GUILD_POST_ELDER:
				{
					GameName role_name = { 0 };
					role->GetName(role_name);
					if (guild->GetMemberManagerRef().AddMember(invitee_user_id, role_name) == NULL)
					{
						role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
						return;
					}
					UserCacheNode* leadernode = UserCacheManager::Instance().GetUserNode(guild->GetMemberManagerRef().GetPatriarchUserID());
					this->NewbieJoinGuild(leadernode, invitee_uid, guild->GetGuildID());
					guild->SendGuildBaseInfo(role);
				}
				break;
			default:
				this->ApplyForJoin(role, guild->GetGuildID());
				break;
		}
	}
	else
	{
		guild->JoinGuild(role);
	}
}

int GuildManager::OnLevelUp(Role * role)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMember *patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == patriarch) return -3;

	// 判断权限
	//if (GUILD_POST_PATRIARCH != patriarch->GetPost() && GUILD_POST_VICE_PATRIARCH != patriarch->GetPost()) return -5;
	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(patriarch->GetPost());
	if (NULL == member_type_cfg) return -4;

	if (member_type_cfg->level_up != 1)
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	return guild->LevelUp(role);
}

int GuildManager::GuildRename(Role * role, GuildName new_name)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMember *old_patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == old_patriarch) return -3;

	GuildMember *new_patriarch = guild->GetMemberManager()->GetMember(role->GetUID());
	if (NULL == new_patriarch) return -4;

	// 判断权限
	if (GUILD_POST_PATRIARCH != old_patriarch->GetPost())
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	// 判断家族名是否存在
	if (this->IsInvildName(new_name))
	{
		role->NoticeNum(errornum::EN_GUILD_INVILD_NAME);
		return -6;
	}

	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(other_cfg.guild_rename_need_itemid, 1, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return -7;
	}

	guild->GuildRename(new_name);
	guild->SendGuildBaseInfo();
	role->NoticeNum(noticenum::NT_GUILD_CHANGE_NAME_SUCC);
	return 0;
}

void GuildManager::NewbieJoinGuild(UserCacheNode * leadernode, int memberuid, GuildID guild_id)
{
	if(NULL == leadernode) return;

	Guild *guild =  this->GetGuild(guild_id);
	if (NULL == guild)
	{
		return;
	}

	GuildName guild_name;
	if(!guild->GetGuildName(guild_name)) return;

	Protocol::SCSingleChat single_chat;
	single_chat.content_type = 0;
	single_chat.from_uid = leadernode->uid;
	single_chat.to_uid = memberuid;
	single_chat.msg_length = SNPRINTF(single_chat.content, sizeof(single_chat.content), gamestring::g_chairman_to_newbie_message, guild_name);
	single_chat.authority_type = 0;
	leadernode->GetName(single_chat.username);
	single_chat.avatar_type = leadernode->avatar_type;
	single_chat.headshot_id = leadernode->appearance.headshot_id;
	single_chat.fly_flag = leadernode->appearance.fly_flag;
	single_chat.prof = leadernode->GetBaseProfession();
	single_chat.level = (short)leadernode->level;
	single_chat.top_level = leadernode->top_level;
	single_chat.avatar_timestamp = leadernode->avatar_timestamp;
	single_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	Role * to_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(memberuid));
	if (to_role != NULL)
	{
		if (FriendManager::Instance().IsBlack(to_role->GetUID(), leadernode->uid))
		{
			//role->NoticeNum(errornum::EN_YOU_ARE_IN_BLACKLISTS);
			return;
		}

		Protocol::SCChannelChat channel_chat;
		Role* leaderrole = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(leadernode->uid));
		if (leaderrole != NULL)
		{
			channel_chat.face_chatframe = single_chat.face_chatframe = leaderrole->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
			channel_chat.face_signature = single_chat.face_signature = leaderrole->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
			FriendManager::Instance().AddRecentlyChat(leaderrole, memberuid);
			FriendManager::Instance().AddRecentlyChat(to_role, leadernode->uid);
			EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&single_chat, sizeof(single_chat));
		}
		else
		{
			FriendManager::Instance().AddRecentlyChat(to_role, leadernode->uid);
			EngineAdapter::Instance().NetSend(to_role->GetNetId(), (const char*)&single_chat, sizeof(single_chat));
		}
		channel_chat.channel_type = chatdef::CHANNEL_TYPE_GUILD;
		channel_chat.msg_length = SNPRINTF(channel_chat.content, sizeof(channel_chat.content), gamestring::g_chairman_to_newbie_message, guild_name);
		channel_chat.content_type = 0;
		channel_chat.from_uid = leadernode->uid;
		leadernode->GetName(channel_chat.username);
		channel_chat.avatar_type = leadernode->avatar_type;
		channel_chat.prof = static_cast<short>(leadernode->GetProfession());
		channel_chat.headshot_id = leadernode->appearance.headshot_id;
		channel_chat.level = (short)leadernode->level;
		channel_chat.avatar_timestamp = leadernode->avatar_timestamp;
		channel_chat.top_level = leadernode->top_level;
		channel_chat.fly_flag = leadernode->appearance.fly_flag;
		channel_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		static GSNetID gsnetidlist[GUILD_MAX_MEMBER_COUNT + 1];
		gsnetidlist[0] = to_role->GetNetId();
		EngineAdapter::Instance().NetMulSend(gsnetidlist, 1, (const char*)&channel_chat, sizeof(channel_chat));
	}
	else 
	{
		gamelog::g_log_debug.printf(LL_DEBUG, "OnSignleChat Can NOT find to_user:%d.", leadernode->uid);
		Role* leaderrole = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(leadernode->uid));
		// 离线的话发到私聊板，上线就能看到
		Protocol::SCSingleChatUserNotExist scune;
		scune.to_uid = memberuid;
		if(leaderrole!= NULL)
		{ 
			single_chat.face_chatframe = leaderrole->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
			single_chat.face_signature = leaderrole->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
			EngineAdapter::Instance().NetSend(leaderrole->GetNetId(), (const char *)&scune, sizeof(Protocol::SCSingleChatUserNotExist));
			FriendManager::Instance().AddRecentlyChat(leaderrole, memberuid);
			ChatManager::Instance().AddSingleChat(memberuid, (const char *)&single_chat, sizeof(single_chat));			
		}
		else
		{
			ChatManager::Instance().AddSingleChat(memberuid, (const char *)&single_chat, sizeof(single_chat));
		}

		ChatManager::Instance().RecordUserSingleChat(memberuid, single_chat.from_uid, (const char *)&single_chat, sizeof(single_chat));
	}
}

int GuildManager::GuildSetBanner(Role* role, GuildBanner new_banner)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -1;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -2;

	// 判断人是否在家族内
	GuildMember *old_patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == old_patriarch) return -3;

	// 判断权限
	if (GUILD_POST_PATRIARCH != old_patriarch->GetPost())
	{
		role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		return -5;
	}

	if (guild->HasSetBannerToday())
	{
		role->NoticeNum(errornum::EN_GUILD_HAS_SET_BANNER_TODAY);
		return -6;
	}

	if (this->IsBannerUsed(new_banner))
	{
		role->NoticeNum(errornum::EN_GUILD_BANNER_IS_USED);
		return -8;
	}

	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	if (other_cfg.change_badge_coin > 0 && !role->GetRoleModuleManager()->GetMoney()->UseCoinBind(other_cfg.change_badge_coin, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return -7;
	}

	{
		// 删掉之前的banner hash
		int hash = GetStringHash(sizeof(GuildBanner), guild->GetBanner());
		m_guild_banner_hash_set.erase(hash);
	}

	guild->GuildSetBanner(new_banner);
	guild->SendGuildBaseInfo();
	role->NoticeNum(noticenum::NT_GUILD_CHANGE_BANNER_SUCC);

	{
		// 加上新的banner hash
		int hash = GetStringHash(sizeof(GuildBanner), guild->GetBanner());
		m_guild_banner_hash_set.insert(hash);
	}

	return 0;
}

int GuildManager::BuildingGuildBuild(Role * role, Protocol::CSGuildConstructReq * req)
{
	if (NULL == role) return -1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -2;
	}

	// 判断家族是否存在
	Guild *guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -3;

	// 判断人是否在家族内
	GuildMember *patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == patriarch) return -4;

	// 判断权限
	const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(patriarch->GetPost());
	if (NULL == member_type_cfg) return -5;

	if (member_type_cfg->build_limit != 1)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_NOT_BUILDING_PERMISSIONS);
		return -6;
	}

	guild->BuildingBuild(role, req->build_type, req->op_type);

	return 0;
}

int GuildManager::SpeedUpBuilding(Role * role)
{
	if (NULL == role) return -1;

	// 判断家族是否存在
	Guild *guild = this->GetGuild(role->GetGuildID());
	if (NULL == guild) return -3;

	// 判断人是否在家族内
	GuildMember *patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == patriarch) return -4;

	// 判断加速是否开启
	if (!guild->IsCanSpeedUp())
	{
		return -5;
	}

	GuildMember *member = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == member) return -6;

	if (CrossConfig::Instance().IsHiddenServer())	// 如果是跨服传回原服处理
	{
		return guild->OnHiddenGameSynSpeedUp(role);
	}

	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (other_cfg == NULL) return -7;

	if (!member->JoinSpeedUp(role))
	{
		return -8;
	}

	if (guild->SpeedUpBuilding(role, other_cfg->speed_up))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_SPEED_UP_FINISH);
	}
	role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_SPEED_UP_SUCCESS);
	
	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_cfg->get_contribution, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);

	gamelog::g_log_guild.printf(LL_INFO, "%s role[%d, %s] add_gongxian[%d] join building speed up", __FUNCTION__, role->GetUID(), role->GetName(), other_cfg->get_contribution);

	guild->SendSpeedUpInfo(role);
	guild->OnGameHiddenSynBuildAllInfo();
	return 0;
}

Guild * GuildManager::GetGuild(GuildID guild_id)
{
	GuildMapConstIt it = m_guild_map.find(guild_id);
	if (it == m_guild_map.end()) return NULL;
	return it->second;
}

Guild* GuildManager::GetGuild(UserID user_id)
{
	for (GuildMapIt it = m_guild_map.begin(), end = m_guild_map.end(); it != end; ++it)
	{
		Guild *guild = it->second;
		if (NULL != guild && NULL != guild->GetMemberManager()->GetMember(user_id))
		{
			return guild;
		}
	}

	return NULL;
}

void GuildManager::RemoveGuild(GuildID guild_id)
{
	GuildMapIt it = m_guild_map.find(guild_id);
	if (it == m_guild_map.end()) return;

	Guild *guild = it->second;

	guild->GetMemberManager()->Dismiss();

	{
		int hash = GetStringHash(sizeof(GuildBanner), guild->GetBanner());
		m_guild_banner_hash_set.erase(hash);
	}

	delete it->second;
	m_guild_map.erase(it);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenSyncGuildChange msg;
		msg.guild_id = guild_id;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.op_type = crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_REMOVE;
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);

		for (std::vector<GuildBossChapterRank>::iterator it = m_guild_boss_chapter_rank_list.begin(); it != m_guild_boss_chapter_rank_list.end();)
		{
			if (it->guild_id == guild_id)
			{
				m_guild_boss_chapter_rank_list.erase(it);
				break;
			}
			else
			{
				it++;
			}
		}
	}

	RankManager::Instance().GetGuildRank()->RemoveGuild(guild_id);
}
