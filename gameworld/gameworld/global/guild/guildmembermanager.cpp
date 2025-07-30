#include "global/usercache/usercache.hpp"
#include "gamelog.h"
#include "guildmembermanager.hpp"
#include "guildmember.hpp"
#include "world.h"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/struct/structcommon.h"
#include "servercommon/errornum.h"
#include "servercommon/string/globalstr.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgchatmsg.h"

#include "engineadapter.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/route/mailroute.hpp"
#include "other/title/title.hpp"
#include "other/title/titleconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/money.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "chat/chatmanager.h"
#include "config/otherconfig/guildconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "other/roleguildinborn/roleguildinborn.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "monster/monster_group_pool.hpp"
#include "monster/monsterpool.hpp"

GuildMemberManager::GuildMemberManager()
{
	m_guild = NULL;
	m_patriarch_user_id = INVALID_USER_ID;
	m_member_count = 0;
	m_patriarch_level = 0;
	memset(m_member_list, 0, sizeof(m_member_list));

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_post_count, 0, sizeof(m_post_count));
	memset(m_patriarch_name, 0, sizeof(m_patriarch_name));

	m_change_flag = false;
}

GuildMemberManager::~GuildMemberManager()
{
	for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; i++)
	{
		if (NULL != m_member_list[i])
		{
			delete m_member_list[i];
		}
	}
	memset(m_member_list, NULL, sizeof(m_member_list));
}

void GuildMemberManager::Init(const GuildMemberParam &member_param, bool is_sync_from_origin /* = false */)
{
	int count = member_param.count;
	if (member_param.count > GUILD_MAX_MEMBER_COUNT)
	{
		count = GUILD_MAX_MEMBER_COUNT;
	}

	if(CrossConfig::Instance().IsHiddenServer())		//加个判断更稳，游戏服按理就init一次
	{
		for (int i = 0; i < ARRAY_LENGTH(m_member_list); ++i)
		{
			if (NULL != m_member_list[i])
			{
				delete m_member_list[i];
				m_member_list[i] = NULL;
			}
		}

		//m_patriarch_user_id = INVALID_USER_ID;
		//m_patriarch_level = 0;
		//memset(m_patriarch_name, 0, sizeof(m_patriarch_name));

		m_member_count = 0;
		memset(m_post_count, 0, sizeof(m_post_count));
		m_userid_index_map.clear();

		//隐藏服不需要这个3个东西，不保存成员数据
		memset(m_dirty_mark, 0, sizeof(m_dirty_mark));		
		memset(m_old_state, 0, sizeof(m_old_state));
		m_change_flag = false;
	}

	unsigned int partriarch_join_time = 0u;
	for (int i = 0; i < count; ++i)
	{
		int index = (int)member_param.member_list[i].index;
		if (index < 0 || index >= GUILD_MAX_MEMBER_COUNT) continue;
		if (NULL != m_member_list[index]) continue;

		GuildMember * member_item = new GuildMember(this);
		member_item->Init(member_param.member_list[i]);

		if (0 == member_param.member_list[i].level)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(member_param.member_list[i].uid);
			if (NULL != node)
			{
				member_item->SetLevel(node->level);
				member_item->SetName(node->role_name);
				member_item->SetAppearance(node->appearance);
			}
		}

		if (GUILD_POST_PATRIARCH == member_item->GetPost())
		{
			if (0u == partriarch_join_time)
			{
				partriarch_join_time = (unsigned int)member_item->GetJoinTime();
				m_patriarch_user_id = IntToUid(member_item->GetUid());
				F_STRNCPY(m_patriarch_name, member_item->GetName(), sizeof(GameName));
			}
			else if (member_item->GetJoinTime() < partriarch_join_time)
			{
				partriarch_join_time = (unsigned int)member_item->GetJoinTime();
				m_patriarch_user_id = IntToUid(member_item->GetUid());
				F_STRNCPY(m_patriarch_name, member_item->GetName(), sizeof(GameName));
			
				gamelog::g_log_serious_error.printf(LL_WARNING, "GuildMemberManager::Init More Than One Partiarch, guild_id[%d]", m_guild->GetGuildID());
			}
			else
			{
				gamelog::g_log_serious_error.printf(LL_WARNING, "GuildMemberManager::Init More Than One Partiarch, guild_id[%d]", m_guild->GetGuildID());
			}
		}

		m_member_list[index] = member_item;
		m_old_state[index] = true;
		m_member_count++;

		int post = member_item->GetPost();
		if (post >= 0 && post < GUILD_POST_MAX)
		{
			m_post_count[post]++;
		}
		m_userid_index_map[IntToUid(member_item->GetUid())] = index;
	}

	if (0 == partriarch_join_time)
	{
		gamelog::g_log_serious_error.printf(LL_WARNING, "GuildMemberManager::Init Partriarch Not Found, guild_id[%d]", m_guild->GetGuildID());
	
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			int max_total_contribute = -1;
			int index = -1;
			for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; ++i)
			{
				// 找一个贡献最高的人
				if (NULL == m_member_list[i]) continue;

				GuildMember * member = m_member_list[i];
				if (member->GetTotalGongXian() > max_total_contribute)
				{
					max_total_contribute = member->GetTotalGongXian();
					index = i;
				}
			}

			if (index >= 0 && index < GUILD_MAX_MEMBER_COUNT && NULL != m_member_list[index])
			{
				m_member_list[index]->SetPost(GUILD_POST_PATRIARCH);
				m_guild->AddEventhandler(m_member_list[index]->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, GUILD_POST_PATRIARCH);
			
				gamelog::g_log_guild.printf(LL_INFO, "GuildMemberManager::Init System Appoint Partriarch role[%d,%s]", 
					m_member_list[index]->GetUid(), m_member_list[index]->GetName());
			}
		}	
	}
}

void GuildMemberManager::GetChangeMemberList(GuildMemberParam *member_param)
{
	if (!m_change_flag)
	{
		member_param->count = 0;
		return;
	}

	int count = 0;
	for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; ++i)
	{
		if (!m_dirty_mark[i]) continue;
		
		GuildMemberParam::Member & mem_data = member_param->member_list[count++];
		if (NULL == m_member_list[i])
		{
			if (!m_old_state[i]) continue;

			mem_data.change_state = structcommon::CS_DELETE;
			mem_data.index = i;
		}
		else
		{
			mem_data.index = i;
			mem_data.change_state = m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
			m_member_list[i]->GetAttr(&mem_data);

			if (mem_data.change_state == structcommon::CS_INSERT)
			{
				gamelog::g_log_guild.printf(LL_INFO, "%s %d | guild[%d %s] member_index:%d role_id[%d]", __FUNCTION__, __LINE__, m_guild->GetGuildID(), m_guild->GetGuildName(),
					mem_data.index, mem_data.uid);
			}
		}
	}

	member_param->count = count;
}

void GuildMemberManager::GetMemberList(GuildMemberParam *member_param)
{
	int count = 0;
	for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; ++i)
	{
		GuildMemberParam::Member & mem_data = member_param->member_list[count];
		if (NULL != m_member_list[i])
		{
			m_member_list[i]->GetAttr(&mem_data);
			++count;
		}	
	}

	member_param->count = count;
}

void GuildMemberManager::ClearDirtyMark()
{
	if (!m_change_flag) return;

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	for (int i = 0; i < GUILD_MAX_MEMBER_COUNT; ++i)
	{
		m_old_state[i] = (NULL != m_member_list[i]);
	}

	m_change_flag = false;
}

void GuildMemberManager::Dismiss()
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		if (it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) continue;

		GuildMember * member = this->GetMemberByIndex(it->second);
		if (NULL == member) continue;
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->GetUid()));
		if (NULL != role)
		{
			role->SetGuildID(0);
			role->SetGuildPostion(-1);	
			role->GetRoleModuleManager()->GetTitle()->CalGuildMemberTitle(TITLE_NOTICE_TYPE_DEFAULT);	// 重算称号

			Protocol::SCQuitGuild info;
			info.reason = Protocol::SCQuitGuild::QUIT;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
		}
	}
}

void GuildMemberManager::Update(time_t now_sec)
{
	for (UserIDApplyMapIt it = m_apply_map.begin(); it != m_apply_map.end();)
	{
		if (now_sec >= it->second.timeout_timestamp)	// 如果超时了，就从申请表里删除
		{
			GuildManager::Instance().EraseApplyGuild(UidToInt(it->first), m_guild->GetGuildID());
			m_apply_map.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void GuildMemberManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	for (UserIDApplyMapIt it = m_apply_map.begin(); it != m_apply_map.end(); it++)
	{
		it->second.rank_fetch_flag = 0;
	}

	{
		UserIDIndexMapIt it_begin = m_userid_index_map.begin();
		UserIDIndexMapIt it_end = m_userid_index_map.end();

		for (; it_begin != it_end; ++it_begin)
		{
			GuildMember * member = this->GetMemberByIndex(it_begin->second);
			if (NULL == member)
				continue;

			member->OnDayChange(old_dayid, now_dayid);

			this->SetDirtyMark(it_begin->second);
		}
	}

	this->SendGuildMemberInfo(NULL);
}

void GuildMemberManager::OnWeekChange()
{
	for (UserIDApplyMapIt it = m_apply_map.begin(); it != m_apply_map.end(); it++)
	{
		it->second.baodi_reward_flag = 0;
	}

	int member_count = this->GetMemberCount();
	for (int i = 0; i < member_count; ++i)
	{
		GuildMember * member = this->GetMemberByIndex(i);
		if (NULL == member)
			continue;

		member->OnWeekChange();

		this->SetDirtyMark(i);
	}
}

void GuildMemberManager::OnGuildLevelUp(int new_guild_level)
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetRoleGuildInborn()->OnSetGuildLevel(new_guild_level);
		}
	}
}

bool GuildMemberManager::AddApplyFor(Role * role, bool is_notice)
{
	if (NULL == role) return false;
	if (m_apply_map.find(role->GetUserId()) != m_apply_map.end())
	{
		role->NoticeNum(errornum::EN_GUILD_APPLY_REPEAT);
		return true;
	}

	GuildApplyUserInfo& user_info = m_apply_map[role->GetUserId()];
	user_info.uid = role->GetUID();
	user_info.avatar_timestamp = role->GetAvatarTimestamp();
	user_info.level = role->GetLevel();
	user_info.top_level = role->GetTopLevel();
	user_info.profession = role->GetProfession();
	role->GetName(user_info.name);
	user_info.timeout_timestamp = EngineAdapter::Instance().Time() + 12 * SECOND_PER_HOUR;		// 12 小时后超时
	user_info.avatar_type = role->GetAvatarType();
	user_info.baodi_reward_flag = role->GetRoleModuleManager()->GetCommonData().guild_boss_baodi_reward_flag_OBSOLETE;
	user_info.rank_fetch_flag = role->GetRoleModuleManager()->GetCommonData().is_fetch_boss_chapter_damage_rank_reward;

	if (is_notice)role->NoticeNum(errornum::EN_GUILD_APPLY_SUCC);
	this->SendApplyListToLegalPost();
	return true;
}

bool GuildMemberManager::ApplyJoinAck(const UserID & user_id, int result)
{
	UserIDApplyMapIt it = m_apply_map.find(user_id);
	if (it == m_apply_map.end()) return false;

	if (0 == result)// 拒绝
	{
		GuildManager::Instance().EraseApplyGuild(UidToInt(user_id), m_guild->GetGuildID());
	}
	else// 同意
	{
		UserIDApplyMapIt it = m_apply_map.find(user_id);
		if (m_apply_map.end() == it) return false;

		if (EngineAdapter::Instance().Time() > it->second.timeout_timestamp)
		{
			return false;
		}

		if (NULL == this->AddMember(user_id, it->second.name, GUILD_POST_MEMBER, it->second.baodi_reward_flag, it->second.rank_fetch_flag))
		{
			return false;
		}

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		if (NULL != role)
		{
			m_guild->SendGuildBaseInfo(role);
		}

		GuildManager::Instance().DeleteApplyRecord(UidToInt(user_id));
	}
	m_apply_map.erase(user_id);
	return true;
}

void GuildMemberManager::SendApplyList(Role * role)
{
	if (NULL == role) return;
	static Protocol::SCSendGuildApplyList info;
	info.count = 0;

	for (UserIDApplyMapIt it = m_apply_map.begin(), end = m_apply_map.end(); it != end && info.count < Protocol::APPLY_COUNT_MAX; ++it)
	{
		info.apply_list[info.count++] = it->second;
	}

	int len = sizeof(info) - (Protocol::APPLY_COUNT_MAX - info.count) * sizeof(GuildApplyUserInfo);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, len);
}

GuildMember * GuildMemberManager::AddMember(UserID user_id, GameName name, int post, int baodi_fetch_flag, char rank_fetch_flag)
{
	// 判断是否存在该玩家
	if (NULL != this->GetMember(user_id)) return NULL;
	// 判断人数是否达到上限
	if (m_member_count >= m_guild->GetMaxMemberCount()) return NULL;

	UserCacheNode* node = UserCacheManager::Instance().GetUserNode(user_id);
	if (NULL == node) return NULL;

	// 找到第一个空位
	int i = 0;
	for (; i < ARRAY_ITEM_COUNT(m_member_list); ++i)
	{
		if (NULL == m_member_list[i])
		{
			break;
		}
	}
	if (i >= ARRAY_ITEM_COUNT(m_member_list))
	{
		// 没有空位
		return NULL;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);

	// 成员参数初始化赋值
	time_t now = EngineAdapter::Instance().Time();
	static GuildMemberParam::Member member_param; member_param.Reset();
	member_param.index = i;
	member_param.uid = UidToInt(user_id);
	member_param.level = node->level;
	F_STRNCPY(member_param.name, name, sizeof(GameName));
	member_param.appearance = node->appearance;
	member_param.post = post;
	member_param.gongxian = 0;
	member_param.total_gongxian = 0;
	member_param.join_time = now;
	member_param.last_login_time = now;
	member_param.m_profession = node->profession;

	if (NULL != role)
	{
		member_param.member_data.guild_boss_baodi_reward_flag = role->GetRoleModuleManager()->GetCommonData().guild_boss_baodi_reward_flag_OBSOLETE;
		member_param.member_data.is_fetch_boss_chapter_damage_rank_reward = role->GetRoleModuleManager()->GetCommonData().is_fetch_boss_chapter_damage_rank_reward;
		member_param.member_data.guild_day_active_reawrd_flag = role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag;
	}
	else
	{
		member_param.member_data.guild_boss_baodi_reward_flag = baodi_fetch_flag;
		member_param.member_data.is_fetch_boss_chapter_damage_rank_reward = rank_fetch_flag;
	}

	// 新建成员类
	GuildMember * member_item = new GuildMember(this);
	member_item->Init(member_param);
	m_member_list[i] = member_item;
	this->SetPostCount(post, 1);
	m_member_count++;
	if (GUILD_POST_PATRIARCH == post)
	{
		m_patriarch_user_id = user_id;
		F_STRNCPY(m_patriarch_name, member_param.name, sizeof(GameName));
		m_patriarch_level = member_param.level;
	}

	member_item->SetOnline(UserStatusNode::STATUS_TYPE_OFFLINE);
	if (NULL != role)
	{
		member_item->SetOnline(UserStatusNode::STATUS_TYPE_ONLINE);
		member_item->SysContribution((int)role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION));
		role->SetGuildID(m_guild->GetGuildID(), false);
		role->SetGuildPostion(member_item->GetPost());

		EventHandler::Instance().OnJoinGuild(role, m_guild->GetGuildID());
		m_guild->SendGuildWeekTaskInfo(role);

		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_join_guild, m_guild->GetName());
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role->GetUserId(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
		}		
	}

	gamelog::g_log_guild.printf(LL_INFO, "guild[%d,%s]  addmember role[%d] index:%d", m_guild->GetGuildID(), m_guild->GetGuildName(), UidToInt(user_id), i);

	this->SetDirtyMark(i);
	m_userid_index_map[user_id] = i;

	this->OnAddMember(member_param, i);
	m_guild->AddEventhandler(name, now, GUILD_EVENTHANDLER_ADD_MEMBER, 0);
	this->SendGuildMemberInfo(NULL);

	m_apply_map.erase(UidToInt(user_id));
	GuildManager::Instance().DeleteApplyRecord(UidToInt(user_id));

	if (NULL != role)
	{
		m_guild->SendGuildActiveInfo(role);
		role->SendGuildActiveInfo();
	}

	return member_item;
}

int GuildMemberManager::RemoveMember(UserID user_id, int reason)
{
	UserIDIndexMapIt it = m_userid_index_map.find(user_id);
	if (it == m_userid_index_map.end()) return -1;

	int index = it->second;
	int uid = UidToInt(it->first);

	if (it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) return -1;
	GuildMember* member = m_member_list[it->second];
	if (NULL == member) return -2;
	if (member->GetUid() != UidToInt(user_id))
	{
		gamelog::g_log_guild.printf(LL_INFO, "GuildMemberManager::RemoveMember Err real_kick_uid[%d] fail_kick_uid[%d] reason[%d]", UidToInt(user_id), member->GetUid(), reason);
		return -3;
	}
	GameName member_name;
	F_STRNCPY(member_name, member->GetName(), sizeof(GameName));

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->GetUid()));
	if (NULL != role)
	{
		Protocol::SCQuitGuild info;
		info.reason = reason;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
		role->SetGuildID(0);
		role->SetGuildPostion(-1);
		role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_GUILD_QIFU, __FUNCTION__);//检查祈福属性																									
		role->GetRoleModuleManager()->GetTitle()->CalGuildMemberTitle(TITLE_NOTICE_TYPE_DEFAULT);	// 退出家族重算称号
	}
	else//被踢出时，玩家处于离线状态
	{
		OfflineRegisterManager::Instance().OnUserKickedOutGuild(IntToUid(member->GetUid()));
	}

	{
		// 踢人之前先清理对应的家族个人活跃排行
		m_guild->OnRemoveMemberByGuildActivePersonRank(member);
	}

	gamelog::g_log_guild.printf(LL_INFO, "%s guild[%d,%s] index:%d user[%d] reason[%d]", __FUNCTION__, m_guild->GetGuildID(), m_guild->GetGuildName(), 
		index, member->GetUid(), reason);
	this->SetPostCount(member->GetPost(), -1);
	
	delete m_member_list[index];
	m_member_list[index] = NULL;

	this->SetDirtyMark(index);
	m_member_count--;
	m_userid_index_map.erase(it);
	this->SendGuildMemberInfo(NULL);

	this->OnRemoveMember(uid, index);
	m_guild->OnRemoveMember(uid);
	m_guild->AddEventhandler(member_name, EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_DEC_MEMBER, 0);
	return 0;
}

GuildMember * GuildMemberManager::GetMember(UserID user_id, int *member_index)
{
	UserIDIndexMapIt it = m_userid_index_map.find(user_id);
	if (it == m_userid_index_map.end()) return NULL;

	if (NULL != member_index) *member_index = it->second;

	return m_member_list[it->second];
}

GuildMember* GuildMemberManager::GetMemberByIndex(int member_index)
{
	if (member_index < 0 || member_index >= GUILD_MAX_MEMBER_COUNT) return NULL;

	return m_member_list[member_index];
}

GuildMember * GuildMemberManager::GetPatriarchMember(int * member_index)
{
	return this->GetMember(m_patriarch_user_id, member_index);
}

std::vector<GuildMember*> GuildMemberManager::GetAllGuildMember()
{
	std::vector<GuildMember*> ret;
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		GuildMember * guild_member = this->GetMemberByIndex(it->second);
		if (guild_member != NULL)
		{
			ret.push_back(guild_member);
		}
	}

	return ret;
}

int GuildMemberManager::AddGongXian(const UserID& user_id, int value, const char* reason)
{
	GuildMember *member = this->GetMember(user_id);
	if (NULL == member) return -1;
	if (value <= 0)  return -2;

	int ret = member->AddGongXian(value, reason);

	return ret;
}

int GuildMemberManager::GetPostCount(int post)
{
	if (post < GUILD_POST_PATRIARCH || post >= GUILD_POST_MAX) return 0;
	return m_post_count[post];
}

int GuildMemberManager::SetPostCount(int post, int add_value)
{
	if (post < GUILD_POST_PATRIARCH || post >= GUILD_POST_MAX)return 0;
	m_post_count[post] += add_value;
	return m_post_count[post];
}

bool GuildMemberManager::IsHavePlace()
{
	int max_count = m_guild->GetMaxMemberCount();
	return max_count > m_member_count;
}

bool GuildMemberManager::GetPatriarchName(GameName name)
{
	F_STRNCPY(name, m_patriarch_name, sizeof(GameName));
	return true;
}

void GuildMemberManager::SetPatriarchName(GameName name)
{
	F_STRNCPY(m_patriarch_name, name, sizeof(m_patriarch_name));
}

void GuildMemberManager::SendInfoToAllMember(const char * info, int len)
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), info, len);
		}
	}
}

void GuildMemberManager::SendSerializeMsgToAllMember(IMessageSerializer * info, int len)
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), info);
		}
	}
}

void GuildMemberManager::SendMailToAllMember(int reason, const MailContentParam & contentparam, int mail_kind, GuildMemberFilterFunc guild_member_filter, void * func_param)
{
	UserID send_list[MAX_GAMEWORLD_SEND_MAIL_USER]; int send_num = 0;

	if (NULL == guild_member_filter)
	{
		for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
		{
			if (send_num >= MAX_GAMEWORLD_SEND_MAIL_USER) break;
			send_list[send_num++] = it->first;
		}
	}
	else
	{
		for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
		{
			if (send_num >= MAX_GAMEWORLD_SEND_MAIL_USER) break;

			GuildMember * guild_member = this->GetMemberByIndex(it->second);
			if (guild_member == NULL)
			{
				continue;
			}
			if (guild_member_filter(guild_member, func_param))
			{				
				send_list[send_num++] = it->first;
			}			
		}
	}

	MailRoute::MailToMultUser(send_list, send_num, reason, contentparam, mail_kind);
}

void GuildMemberManager::SendTitleToAllMember(int title_id, const char * reason)
{
	const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
	if (NULL == title_cfg) return;		// 如果没有这个称号，返回

	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetTitle()->AddTitle(title_id, reason);
			continue;
		}

		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameRoleAddTitle msg;
			msg.title_id = title_id;
			msg.uid = UidToInt(it->first);
			F_STRNCPY(msg.reason, reason, sizeof(msg.reason));

			InternalComm::Instance().SendToGameThroughCross(m_guild->GetPlatType(), m_guild->GetServerId(), (const char*)&msg, sizeof(msg));
			continue;;
		}
		OfflineRegisterManager::Instance().OnTitleSendReq(it->first, title_id);
	}
}

void GuildMemberManager::SendGuildMemberInfo(Role * role)
{
	Protocol::SCGuildMemberInfo info;
	info.member_count = 0;
	for (UserIDIndexMapIt it = m_userid_index_map.begin(); it != m_userid_index_map.end(); ++it)
	{
		if (it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) continue;

		GuildMember * member = this->GetMemberByIndex(it->second);
		if (NULL == member) continue;

		member->GetInfo(&info.member_list[info.member_count++]);
	}

	int len = sizeof(info) - ((GUILD_MAX_MEMBER_COUNT - info.member_count) * sizeof(Protocol::MemberInfo));

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, len);
	}
	else
	{
		this->SendInfoToAllMember((const char*)&info, len);
	}
}

void GuildMemberManager::SendGuildMemberInfoChange(int change_role_id)
{
	GuildMember * member = this->GetMember(change_role_id);
	if (!member)
	{
		return;
	}

	Protocol::SCGuildMemberInfoChange info;
	member->GetInfo(&info.member_info);
	this->SendInfoToAllMember((const char *)&info, sizeof(info));
}

void GuildMemberManager::SendInfoToAllMemberFromHidden(const char * info, int len)
{
	this->SendInfoToAllMember(info, len);

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameGuildMemberThroughCross(m_guild->GetGuildID(), info, len);
	}
}

void GuildMemberManager::SendInfoToAllMemberInfo(const char * info, int length)
{
	this->SendInfoToAllMember(info, length);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameGuildMemberThroughCross(m_guild->GetGuildID(), info, length);
	}
	else
	{
		InternalComm::Instance().SendToHiddenGuildMemberThroughCross(m_guild->GetGuildID(), info, length);
	}
}

void GuildMemberManager::ClearAllMemberHonorBattleFlag()
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		GuildMember * member = this->GetMember(it->first);
		if (NULL != member)
		{
			member->ClearHonorBattleFlag();
			this->OnMemberInfoChange(member->GetUid());
			gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s uid[%d] ClearAllMemberHonorBattleFlag!", __FUNCTION__, member->GetUid());
		}
	}
}

void GuildMemberManager::SendSPUPInfoToAllMember(Protocol::SCGuildSpeedUpInfo msg)
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			GuildMember * member = this->GetMember(role->GetUID());
			if (NULL == member) continue;

			msg.is_cur_building_help = role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count;
			role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));		
		}
	}
}

void GuildMemberManager::GameToHiddenSyncGuildMemberData(int role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	GuildMember * member = this->GetMember(role_id);
	if (!member)
	{
		return;
	}

	GuildMemberParam::Member param;
	member->GetAttr(&param);

	// 同步到跨服
	crossgameprotocal::GameHiddenSyncGuildMemberChange msg;
	msg.guild_id = m_guild->GetGuildID();
	msg.op_type = crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_UPDATE_2;
	msg.plat_type = LocalConfig::Instance().GetPlatType();
	msg.member = param;
	msg.param1 = member->GetOnLinetate();

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, sizeof(msg));
}

void GuildMemberManager::SetDirtyMark(int index)
{
	if (index < 0 || index >= GUILD_MAX_MEMBER_COUNT) return;
	m_change_flag = true;
	m_dirty_mark[index] = true;
	m_guild->SetDirty(true);
}

void GuildMemberManager::OnChat(const char* buff, int total_len, bool need_sync /* = false */)
{
	if (NULL == buff || total_len <= 0) return;

	int net_num = 0;
	static GSNetID gsnetidlist[GUILD_MAX_MEMBER_COUNT + 1];

	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		if (it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) continue;

		GuildMember * member = this->GetMemberByIndex(it->second);
		if (NULL == member) continue;


		Role *tmp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->GetUid()));
		if (NULL != tmp_role)
		{
			gsnetidlist[net_num++] = tmp_role->GetNetId();
		}
	}

	if (net_num > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetidlist, net_num, buff, total_len);
	}

	if (need_sync)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			// 同步到原服
			crossgameprotocal::HiddenGameGuildChatSync sync;
			UNSTD_STATIC_CHECK(sizeof(sync.chat_msg) > sizeof(Protocol::SCChannelChat));
			
			if (total_len < static_cast<int>(sizeof(sync.chat_msg)))
			{
				memset(sync.chat_msg, 0, sizeof(sync.chat_msg));
				sync.guild_id = this->GetGuild()->GetGuildID();
				sync.chat_msg_len = total_len;
				memcpy(sync.chat_msg, buff, total_len);
			}

			int sync_len = sizeof(sync) - sizeof(sync.chat_msg) + sync.chat_msg_len;
			int plat_type = LocalConfig::Instance().GetPlatType();
			int server_id = IntToUid(this->GetGuild()->GetGuildID()).db_index;
			InternalComm::Instance().SendToGameThroughCross(plat_type, server_id, (const char*)&sync, sync_len);
		}
		else
		{
			// 同步到跨服
			crossgameprotocal::GameHiddenGuildChatSync sync;
			UNSTD_STATIC_CHECK(sizeof(sync.chat_msg) > sizeof(Protocol::SCChannelChat));

			if (total_len < static_cast<int>(sizeof(sync.chat_msg)))
			{
				memset(sync.chat_msg, 0, sizeof(sync.chat_msg));
				sync.guild_id = this->GetGuild()->GetGuildID();
				sync.chat_msg_len = total_len;
				memcpy(sync.chat_msg, buff, total_len);
			}

			int sync_len = sizeof(sync) - sizeof(sync.chat_msg) + sync.chat_msg_len;
			InternalComm::Instance().SendToHiddenThroughCross((const char*)&sync, sync_len);
		}
	}
}

bool GuildMemberManager::IsGuildMember(int role_id)
{
	return GetMember(role_id) != NULL;
}

void GuildMemberManager::GmGuildMemberOrder(int op_type)
{
	enum GuildMemberMgrOrderType
	{
		MEM_MGR_PRINT_ALL_MEMBER_ID = 0,
		MEM_MGR_SAVE_MEMBERLIST_NOW = 1,
	};

	switch (op_type)
	{
	case MEM_MGR_PRINT_ALL_MEMBER_ID:
		printf("\nPrintf All Guild Member's RoleId --> GuildID: %d | MemNum: %d", m_guild->GetGuildID(), m_member_count);
		for (int mem_idx = 0; mem_idx < m_member_count; ++mem_idx)
		{
			GuildMember * member = m_member_list[mem_idx];
			if (NULL != member) printf("\n    Index: %d | RoleID: %d", mem_idx, member->GetUid());
		}
		printf("\nEnd.\n");
		break;
	case MEM_MGR_SAVE_MEMBERLIST_NOW:
		{
			m_guild->Save();
		}
		break;
	}
}

void GuildMemberManager::UpdateGuildHonorTitleId(int rank)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetTitle()->CalGuildHonorTitle2();
			role->GetRoleModuleManager()->OnSetGuildHonorInfo(rank);
		}
		else
		{
			if (rank > 0)
			{
				crossgameprotocal::HiddenGameActivityEndInfo info;
				info.role_id = UidToInt(it->first);
				info.activity_type = ACTIVITY_TYPE_GUILD_HONOR_BATTLE;
				info.param1 = rank;

				InternalComm::Instance().SendToGameThroughCross2((const void *)&info, sizeof(info), it->first.db_index);
			}
		}
	}
}

void GuildMemberManager::UpdateGuildHonorTitleIdFromHidden()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetTitle()->CalGuildHonorTitle2();
		}
	}
}

void GuildMemberManager::UpdateGuildFightTitle()
{
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(it->first);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetTitle()->CalGuildFightTitle();
		}
	}
}

void GuildMemberManager::AddGuildBossTotalDamage(int role_id, long long add_value, int boss_index)
{
	GuildMember * guild_member = this->GetMember(role_id);
	if (guild_member != NULL)
	{
		GuildMemberData &  member_data = guild_member->GetMemberDataAndSetDirty();
		member_data.guild_boss_total_damage += add_value;
		member_data.guild_boss_join_flag |= (1 << boss_index);
	}
}

void GuildMemberManager::UpdateGuildBossHighestDamage(int role_id, long long total_damage, ARG_OUT bool* need_update_rank)
{
	GuildMember * guild_member = this->GetMember(role_id);
	if (guild_member != NULL)
	{
		GuildMemberData &  member_data = guild_member->MutableMemberData();
		if (member_data.guild_boss_today_highest_damage < total_damage)
		{
			member_data.guild_boss_today_highest_damage = total_damage;
			this->SetDirtyMark(guild_member->GetIndex());

			if (NULL != need_update_rank)
			{
				*need_update_rank = true;
				return;
			}
		}
	}

	if (NULL != need_update_rank)
	{
		*need_update_rank = false;
	}
}

std::vector<GuildBossRankItem> GuildMemberManager::GetMemberGuildBossItem()
{
	std::vector<GuildBossRankItem> ret;
	ret.reserve(m_userid_index_map.size());
	
	for (UserIDIndexMapIt it = m_userid_index_map.begin(), end = m_userid_index_map.end(); it != end; ++it)
	{
		GuildMember * member = this->GetMemberByIndex(it->second);
		if (NULL == member) continue;

		if (member->GetMemberData().guild_boss_today_highest_damage > 0)
		{
			GuildBossRankItem item;

			item.role_id = UidToInt(it->first);
			item.guild_boss_total_damage = member->GetMemberData().guild_boss_today_highest_damage;

			ret.push_back(item);
		}
	}

	return ret;
}

void GuildMemberManager::OnGameSyncMemberChange(int op_type, int param1, const GuildMemberParam::Member & member_param)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (op_type == crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_ADD)
	{
		UserID user_id = IntToUid(member_param.uid);
		if (NULL != this->GetMember(user_id)) return;
		// 判断人数是否达到上限
		if (m_member_count >= m_guild->GetMaxMemberCount()) return;
		if (NULL != this->GetMemberByIndex(member_param.index)) return;
		
		// 新建成员类
		GuildMember * member_item = new GuildMember(this);
		member_item->Init(member_param);
		m_member_list[member_param.index] = member_item;
		this->SetPostCount(member_param.post, 1);
		m_member_count++;
		if (GUILD_POST_PATRIARCH == member_param.post)
		{
			m_patriarch_user_id = user_id;
			F_STRNCPY(m_patriarch_name, member_param.name, sizeof(GameName));
			m_patriarch_level = member_param.level;
		}

		member_item->SetOnline(UserStatusNode::STATUS_TYPE_OFFLINE);
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		if (NULL != role)
		{
			member_item->SetOnline(UserStatusNode::STATUS_TYPE_ONLINE);
			member_item->SysContribution((int)role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION));
			role->SetGuildID(m_guild->GetGuildID());
			role->SetGuildPostion(member_item->GetPost());
			EventHandler::Instance().OnJoinGuild(role, m_guild->GetGuildID());
		}

		this->SetDirtyMark(member_param.index);
		m_userid_index_map[user_id] = member_param.index;
		this->SendGuildMemberInfo(NULL);
		m_guild->SendGuildBaseInfo(role);
		m_guild->AddEventhandler(member_param.name, member_param.join_time, GUILD_EVENTHANDLER_ADD_MEMBER, 0);
	}
	else if (op_type == crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_UPDATE)
	{
		UserID user_id = IntToUid(member_param.uid);
		GuildMember* member = this->GetMember(user_id);
		if (NULL != member)
		{
			member->SetOnline(param1);
			member->Init(member_param);
			m_guild->SetAllRankRefresh(member_param.uid);
		}
	}
	else if (op_type == crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_UPDATE_2)
	{
		UserID user_id = IntToUid(member_param.uid);
		GuildMember * member = this->GetMember(user_id);
		if (NULL != member)
		{
			member->SetOnline(param1);
			member->Init(member_param);
			m_guild->SetAllRankRefresh(member_param.uid);
			this->SendGuildMemberInfoChange(member_param.uid);
		}
	}
	else if (op_type == crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_REMOVE)
	{
		UserID user_id = IntToUid(member_param.uid);
		UserIDIndexMapIt it = m_userid_index_map.find(user_id);
		if (it == m_userid_index_map.end()) return;

		GuildMember* member = this->GetMemberByIndex(it->second);
		if (NULL == member || member->GetUid() != member_param.uid) return;

		GameName member_name;
		F_STRNCPY(member_name, member->GetName(), sizeof(GameName));

		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(member->GetUid()));
		if (NULL != role)
		{
			Protocol::SCQuitGuild info;
			info.reason = Protocol::SCQuitGuild::EXPEL_OUT; // 人在跨服， 只能是被踢的
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
			role->SetGuildID(0);
		}
		m_guild->OnRemoveMember(member->GetUid());
		delete m_member_list[it->second];
		m_member_list[it->second] = NULL;

		this->SetDirtyMark(it->second);
		m_member_count--;
		m_userid_index_map.erase(it);
		this->SendGuildMemberInfo(NULL);
		m_guild->AddEventhandler(member_name, EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_DEC_MEMBER, 0);
	}
}

void GuildMemberManager::OnHiddenSyncMemberChange(int op_type, int param1, const GuildMemberParam::Member & member_param)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (op_type == crossgameprotocal::HiddenGameSyncGuildMemberChange::OP_TYPE_UPDATE)
	{
		UserID user_id = IntToUid(member_param.uid);
		GuildMember * member = this->GetMember(user_id);
		if (NULL != member)
		{
			member->SetOnline(param1);
			member->UpdateMemberInfoFromHidden(member_param);
			m_guild->SetAllRankRefresh(member_param.uid);
		}
	}
}

void GuildMemberManager::SendApplyListToLegalPost()
{
	int net_num = 0;
	static GSNetID net_id_list[GUILD_MAX_MEMBER_COUNT];

	for (int mem_idx = 0; mem_idx < GUILD_MAX_MEMBER_COUNT; ++mem_idx)
	{
		GuildMember * guild_mem = this->GetMemberByIndex(mem_idx);
		if (NULL == guild_mem) continue;

		switch (guild_mem->GetPost())
		{
		case GUILD_POST_PATRIARCH:
		case GUILD_POST_VICE_PATRIARCH:
		case GUILD_POST_ELDER:
			{
				Role * mem_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(guild_mem->GetUid()));
				if (NULL != mem_role) net_id_list[net_num++] = mem_role->GetNetId();
			}
			break;
		}
	}

	static Protocol::SCSendGuildApplyList info;
	info.count = 0;

	for (UserIDApplyMapIt it = m_apply_map.begin(), end = m_apply_map.end(); it != end && info.count < Protocol::APPLY_COUNT_MAX; ++it)
	{
		info.apply_list[info.count++] = it->second;
	}

	int len = sizeof(info) - (Protocol::APPLY_COUNT_MAX - info.count) * sizeof(GuildApplyUserInfo);
	EngineAdapter::Instance().NetMulSend(net_id_list, net_num, (const char*)&info, len);
}

void GuildMemberManager::OnAddMember(const GuildMemberParam::Member& member, int index)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 同步到跨服
		crossgameprotocal::GameHiddenSyncGuildMemberChange msg;
		msg.guild_id = m_guild->GetGuildID();
		msg.op_type = crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_ADD;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.member = member;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void GuildMemberManager::OnRemoveMember(int uid, int index)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 同步到跨服
		crossgameprotocal::GameHiddenSyncGuildMemberChange msg;
		msg.guild_id = m_guild->GetGuildID();
		msg.op_type = crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_REMOVE;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		
		static GuildMemberParam::Member member_param;
		member_param.index = index;
		member_param.uid = uid;
		msg.member = member_param;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void GuildMemberManager::OnMemberInfoChange(const GuildMemberParam::Member& member, int online_state)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		// 同步到跨服
		crossgameprotocal::GameHiddenSyncGuildMemberChange msg;
		msg.guild_id = m_guild->GetGuildID();
		msg.op_type = crossgameprotocal::GameHiddenSyncGuildMemberChange::OP_TYPE_UPDATE;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.member = member;
		msg.param1 = online_state;

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
	else
	{
		// 同步到跨服
		crossgameprotocal::HiddenGameSyncGuildMemberChange msg;
		msg.guild_id = m_guild->GetGuildID();
		msg.op_type = crossgameprotocal::HiddenGameSyncGuildMemberChange::OP_TYPE_UPDATE;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.param1 = online_state;
		msg.member = member;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}
}

void GuildMemberManager::OnMemberInfoChange(const UserID& user_id)
{
	int index = -1;
	GuildMember* member = this->GetMember(user_id, &index);
	if (NULL == member) return;

	GuildMemberParam::Member param;
	member->GetAttr(&param);
	this->OnMemberInfoChange(param, member->GetOnLinetate());
}

bool FilterZuZhang(GuildMember * guild_member, void * func_param)
{
	if (guild_member == NULL)
	{
		return false;
	}

	if (guild_member->GetPost() == GUILD_POST_PATRIARCH)
	{
		return false;
	}

	return true;
}

bool FilterJoinHonor(GuildMember * guild_member, void * func_param)
{
	if (NULL == guild_member || NULL == func_param) return false;

	bool is_join = *(bool *)func_param;

	const GuildMemberData& common_data = guild_member->GetMemberData();

	if (!is_join)
	{
		return common_data.join_honor_battle_flag == 0 ? true : false;
	}
	return common_data.join_honor_battle_flag == 1 ? true : false;
}
