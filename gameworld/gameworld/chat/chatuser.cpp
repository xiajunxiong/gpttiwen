#include <stdlib.h>
#include <memory.h>

#include "common/platform/system.h"
#include "chatuser.h"

#include "engineadapter.h"

#include "servercommon/servercommon.h"
#include "servercommon/errornum.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/guilddef.hpp"
#include "obj/character/role.h"

#include "config/sharedconfig/sharedconfig.h"
#include "chat/chatmanager.h"
#include "globalconfig/globalconfig.h"
#include "servercommon/string/gameworldstr.h"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "chat/chatconfig.h"
#include "global/worldstatus/worldstatus.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"

ChatUser::ChatUser()
	: m_chat_manager(NULL), m_uid(0), m_netid(INVALID_GSNETID), m_world_msg_last_length(0), m_world_msg_same_length_accumulate_count(0),
	m_world_msg_first_same_length_timestamp(0), m_ignore_world_talk(false)
{
}

ChatUser::ChatUser(ChatManager *chat_manager, int uid, GSNetID netid)
	: m_chat_manager(chat_manager), m_uid(uid), m_netid(netid), m_world_msg_last_length(0), m_world_msg_same_length_accumulate_count(0), 
	m_world_msg_first_same_length_timestamp(0), m_ignore_world_talk(false)
{
	memset(next_active_list, 0, sizeof(next_active_list));
}

ChatUser::~ChatUser()
{

}

int ChatUser::GetUID() const
{
	return m_uid;
}

bool ChatUser::IgnoreWorldTalk()
{
	if (GLOBALCONFIG->GetWorldTalkIsLimit())
	{
		return m_ignore_world_talk;
	}

	return false;
}

bool ChatUser::IsForbidChannel(Role *role)
{
	if (NULL == role)
	{
		return true;
	}

	return role->IsForbidTalk();
}

bool ChatUser::IsShowMyTalk(Role * role)
{
	if (NULL == role)
	{
		return false;
	}
	
	bool is_show = false;
	switch (role->GetForbidTalkType())
	{
		case FORBID_TALK_TYPE_SILENCE:
		{
			is_show = true;
		}
		break;
		default:
		break;
	}

	return is_show;
}

bool ChatUser::ChannelCanChat(Role *role, int channel_type)
{
	if (NULL == role)
	{
		return false;
	}

	if (channel_type < chatdef::CHANNEL_TYPE_WORLD || channel_type >= chatdef::CHANNEL_TYPE_MAX)
	{
		return false;
	}

	int channel_open_level = this->GetChannelOpenLevel(channel_type);

	if (role->GetLevel() < channel_open_level)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chat_open_level, channel_open_level, channel_type);
		if (length > 0)
		{
			static Protocol::SCSystemMsg sm;
			int sendlen = 0;

			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}

		return false;
	}


	if (channel_type != chatdef::CHANNEL_TYPE_TEAM)
	{
		unsigned int now_sec = (unsigned int)(EngineAdapter::Instance().Time());

		if (now_sec < next_active_list[channel_type])
		{
			role->NoticeNum(errornum::EN_CHAT_FREQUENTLY);
			return false;
		}

		int cooldown = this->GetCooldown(channel_type);
		next_active_list[channel_type] = now_sec + cooldown;
	}


	return true;
}

int ChatUser::GetChannelOpenLevel(int channel_type)
{
	int channel_open_level = GLOBALCONFIG->GetTalkLevel();

	switch (channel_type)
	{
		case chatdef::CHANNEL_TYPE_GUILD:
			channel_open_level = GLOBALCONFIG->GetTalkLevelXianMeng();
			break;

		default:
			break;
	}

	return channel_open_level;
}

void ChatUser::OnSingleChat(Protocol::CSSingleChat *sc_req)
{
}

void ChatUser::OnGuildChat(Protocol::CSChannelChat *cc_req)
{
}

void ChatUser::OnWorldChat(Protocol::CSChannelChat *cc_req)
{
	if (NULL == cc_req) return;

	if (GLOBALCONFIG->GetWorldTalkIsLimit() && !m_ignore_world_talk)
	{
		if (cc_req->msg_length >= GLOBALCONFIG->GetWorldTalkEffectMsgMinLen())
		{
			time_t now = EngineAdapter::Instance().Time();

			if (now >= m_world_msg_first_same_length_timestamp + GLOBALCONFIG->GetWorldTalkAccumulateCountResetInterval() || cc_req->msg_length != m_world_msg_last_length)
			{
				m_world_msg_first_same_length_timestamp = now;
				m_world_msg_last_length = cc_req->msg_length;
				m_world_msg_same_length_accumulate_count = 1;

			}
			else
			{
				++m_world_msg_same_length_accumulate_count;

				int torelate_count = GLOBALCONFIG->GetWorldTalkTorelateSameMaxCount();
				if (m_world_msg_same_length_accumulate_count >= torelate_count)
				{
					m_ignore_world_talk = true;
				}
			}
		}
	}
}

bool ChatUser::CanSingleChatLimit(Role *role, int to_uid)
{
	if (NULL == role)
	{
		return false;
	}

	if (role->GetLevel() >= GLOBALCONFIG->GetSingleChatNumLevel())
	{
		return true;
	}

	unsigned int nowtime = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int last_time = role->GetRoleModuleManager()->GetSingleChatNumTime();

	if (nowtime - last_time < GLOBALCONFIG->GetSingleChatNumTime() * SECOND_PER_HOUR)
	{
		if (role->GetRoleModuleManager()->GetSingleChatNumNum() >= GLOBALCONFIG->GetSingleChatNumNum())
		{
			if (role->GetRoleModuleManager()->IsInSingleChatNumList(to_uid))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		role->GetRoleModuleManager()->ResetSingleChatNumData();
		role->GetRoleModuleManager()->SetSingleChatNumTime(nowtime);
	}

	role->GetRoleModuleManager()->AddSingleChatNumUser(to_uid);
	return true;
}

bool ChatUser::operator==(const ChatUser & other) const
{
	if (this->GetUID() == other.GetUID())
	{
		return true;
	}

	return false;
}

int ChatUser::GetCooldown(int channal_type)
{
	int cd_s = 1;	//CHANNEL_CD_TIME_WORLD;

	switch (channal_type)
	{
	case chatdef::CHANNEL_TYPE_WORLD:
		cd_s = LOGIC_CONFIG->GetChatConfig()->GetChatChanelLimitLevel(chatdef::CHANNEL_TYPE_WORLD).interval_time;
		if (WorldStatus::Instance().IsAnswerTime())cd_s = cd_s / 2;//世界答题时间，间隔缩短一半
		break;

	case chatdef::CHANNEL_TYPE_SCENE:
		cd_s = LOGIC_CONFIG->GetChatConfig()->GetChatChanelLimitLevel(chatdef::CHANNEL_TYPE_SCENE).interval_time;
		break;

	case chatdef::CHANNEL_TYPE_TEAM:
		cd_s = LOGIC_CONFIG->GetChatConfig()->GetChatChanelLimitLevel(chatdef::CHANNEL_TYPE_TEAM).interval_time;
		break;

	case chatdef::CHANNEL_TYPE_GUILD:
		cd_s = LOGIC_CONFIG->GetChatConfig()->GetChatChanelLimitLevel(chatdef::CHANNEL_TYPE_GUILD).interval_time;
		break;

	case chatdef::CHANNEL_TYPE_PROF:
		cd_s = LOGIC_CONFIG->GetChatConfig()->GetChatChanelLimitLevel(chatdef::CHANNEL_TYPE_PROF).interval_time;
		break;
	}

	return cd_s;
}

