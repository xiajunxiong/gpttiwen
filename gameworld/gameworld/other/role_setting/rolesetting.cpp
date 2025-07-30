#include "rolesetting.hpp"
#include "engineadapter.h"
#include "world.h"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/noticenum.h"
#include "gameworld/protocol/msgrole.h"
#include "other/event/eventhandler.hpp"

RoleSetting::RoleSetting() :m_role_module_mgr(NULL)
{
}

RoleSetting::~RoleSetting()
{
}

void RoleSetting::Init(RoleModuleManager * role_module_manager, const RoleSetSystemParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;

	this->CheckChatGroupSettingTransfer();
}

void RoleSetting::GetOtherInitParam(RoleSetSystemParam * param)
{
	*param = m_param;
}

void RoleSetting::OnRoleLogin()
{
	this->SendSystemSetting();
	this->SendSystemSettingChatGroup();
}

bool RoleSetting::SetRoleSetting(short set_system_type, short param)
{
	if (set_system_type < RoleSetSystemParam::SYSTEM_SET_INFO_TOTAL_VOLUME || set_system_type > RoleSetSystemParam::SYSTEM_SET_INFO_MAX)
	{
		return false;
	}
	bool is_custom = true;
	switch (set_system_type)
	{
	case RoleSetSystemParam::SYSTEM_SET_INFO_RESET:
		this->ReserSystemSet();
		is_custom = false;
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_TOTAL_VOLUME:
		this->SetTotalVolume(param);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_MUSIC_VOLUME:
		this->SetMusicVolume(param);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SOUND_EFFECTS:
		this->SetSoundEffects(param);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_PICTURE_EFFECTS:
		this->SetPictureQuality(param);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NUMBER_OF_SCREENS:
		this->SetNumberOfScreens(param);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SHOW_OTHER_PLAYER:
		this->ShowOtherPlayer(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SHOW_PLAYER_NAME:
		this->ShowPlayerName(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SHOW_OWN_SIDE_HP:
		this->ShowOwnSideHp(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SHOW_SYSTEM_MSG:
		this->ShowSystemMSG(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_REFUSE_TEAM_REQUEST:
		this->SetRefuseTeamRequest(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_REFUSE_STRANGER_MSG:
		this->SetRefuseStrangerMSG(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_AUTO_RESEARCH_TASK:
		this->SetAutoResearchTask(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_1:
		this->SetNoticeChatGroup(set_system_type,param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_2:
		this->SetNoticeChatGroup(set_system_type, param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_3:
		this->SetNoticeChatGroup(set_system_type, param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_4:
		this->SetNoticeChatGroup(set_system_type, param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_5:
		this->SetNoticeChatGroup(set_system_type, param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_JOYSTICK:
		this->SetUseJoyStick(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_POWER_SAVE:
		this->SetPowerSave(param == 1);
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_SHIELDING_FLOWERS:
		{
			this->SetShieldingFlowers(param == 1);
		}
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_GUILD_ANSWER_SHOW_ROLE_NUM:
		{
			m_param.guild_answer_show_role_num = param;
		}
		break;
	case RoleSetSystemParam::SYSTEM_SET_INFO_BATTLE_CONTROL_MODE:
		{
			this->SetBattleControlMode(param);
		}
		break;
	default:
		break;
	}
	if (is_custom == true && m_param.is_default != RoleSetSystemParam::SYSTEM_SET_COMMON_NO)
	{
		m_param.is_default = RoleSetSystemParam::SYSTEM_SET_COMMON_NO;
	}
	return true;
}

bool RoleSetting::SetRoleChatGroupSetting(long long group_id, int value)
{
	const CommonDataParam& common_data = m_role_module_mgr->GetCommonData();
	for (int i = 0; i < ARRAY_ITEM_COUNT(common_data.cross_chat_group_list); ++i)
	{
		if (group_id == common_data.cross_chat_group_list[i])
		{
			m_param.is_notice_chat_group[i] = static_cast<char>(value);
		}
	}
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(common_data.cross_chat_group_list) == ARRAY_ITEM_COUNT(m_param.is_notice_chat_group));

	this->SendSystemSettingChatGroup();

	return true;
}

void RoleSetting::SetTotalVolume(short volume)
{
	if (volume < RoleSetSystemParam::VOLUME_PERSENT_CLOSE || volume > RoleSetSystemParam::VOLUME_PERSENT_MAX)
	{
		return;
	}
	m_param.total_volume = volume;
}

void RoleSetting::SetMusicVolume(short volume)
{
	if (volume < RoleSetSystemParam::VOLUME_PERSENT_CLOSE || volume > RoleSetSystemParam::VOLUME_PERSENT_MAX)
	{
		return;
	}
	m_param.music_volume = volume;
}

void RoleSetting::SetSoundEffects(short volume)
{
	if (volume < RoleSetSystemParam::VOLUME_PERSENT_CLOSE || volume > RoleSetSystemParam::VOLUME_PERSENT_MAX)
	{
		return;
	}
	m_param.sound_effect = volume;
}

void RoleSetting::SetPictureQuality(short effect_level)
{
	if (effect_level < RoleSetSystemParam::PICTURE_QUALITY_LOW || effect_level > RoleSetSystemParam::PICTURE_QUALITY_HIGH)
	{
		return;
	}
	m_param.picture_quality = effect_level;
}

void RoleSetting::SetNumberOfScreens(short player_num)
{
	if (player_num < RoleSetSystemParam::NUMBER_OF_SCREENS_MIN || player_num > RoleSetSystemParam::NUMBER_OF_SCREENS_MAX)
	{
		return;
	}
	m_param.number_of_screens = player_num;

	EventHandler::Instance().OnRoleSettingNumsOfSceenChange(m_role_module_mgr->GetRole(), player_num);
}

void RoleSetting::ShowOtherPlayer(bool is_show)
{
	m_param.is_show_other_player = is_show ? 1 : 0;
}

void RoleSetting::ShowPlayerName(bool is_show)
{
	m_param.is_show_player_name = is_show ? 1 : 0;
}

void RoleSetting::ShowOwnSideHp(bool is_show)
{
	m_param.is_show_own_hp = is_show ? 1 : 0;
}

void RoleSetting::ShowSystemMSG(bool is_show)
{
	m_param.is_show_system_msg = is_show ? 1 : 0;
}

void RoleSetting::SetRefuseTeamRequest(bool is_refuse)
{
	m_param.is_refuse_team_requset = is_refuse ? 1 : 0;
}

void RoleSetting::SetRefuseStrangerMSG(bool is_refuse)
{
	m_param.is_refuse_stranger_msg = is_refuse ? 1 : 0;
}

void RoleSetting::SetAutoResearchTask(bool is_auto)
{
	m_param.is_auto_research_task = is_auto;
}

void RoleSetting::SetNoticeChatGroup(int group_index, bool is_notice)
{
	if (RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_1 < group_index || group_index >= RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_1 + MAX_ROLE_CROSS_GROUP)return;
	
	int index = group_index - RoleSetSystemParam::SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_1;
	if (index < 0 || index >= ARRAY_ITEM_COUNT(m_param.is_notice_chat_group))
	{
		return;
	}

	m_param.is_notice_chat_group[index] = is_notice ? RoleSetSystemParam::SYSTEM_SET_COMMON_YES : RoleSetSystemParam::SYSTEM_SET_COMMON_NO;

	this->SendSystemSetting();
}

void RoleSetting::SetUseJoyStick(bool is_use)
{
	m_param.is_use_joystick = is_use ? 1 : 0;
}

void RoleSetting::SetPowerSave(bool is_save)
{
	m_param.is_power_save = is_save ? 1 : 0;
}

void RoleSetting::SendSystemSetting()
{
	Protocol::SCRoleSystemSetInfo protocol;
	
	protocol.system_set_list[0] = m_param.total_volume;
	protocol.system_set_list[1] = m_param.music_volume;
	protocol.system_set_list[2] = m_param.sound_effect;
	protocol.system_set_list[3] = m_param.picture_quality;
	protocol.system_set_list[4] = m_param.number_of_screens;
	protocol.system_set_list[5] = m_param.is_show_other_player;
	protocol.system_set_list[6] = m_param.is_show_player_name;
	protocol.system_set_list[7] = m_param.is_show_own_hp;
	protocol.system_set_list[8] = m_param.is_show_system_msg;
	protocol.system_set_list[9] = m_param.is_refuse_team_requset;
	protocol.system_set_list[10] = m_param.is_refuse_stranger_msg;
	protocol.system_set_list[11] = m_param.is_auto_research_task;
	protocol.system_set_list[12] = m_param.is_notice_chat_group_1_OBSOLETE;
	protocol.system_set_list[13] = m_param.is_notice_chat_group_2_OBSOLETE;
	protocol.system_set_list[14] = m_param.is_notice_chat_group_3_OBSOLETE;
	protocol.system_set_list[15] = m_param.is_notice_chat_group_4_OBSOLETE;
	protocol.system_set_list[16] = m_param.is_notice_chat_group_5_OBSOLETE;
	protocol.system_set_list[17] = m_param.is_use_joystick;
	protocol.system_set_list[18] = m_param.is_power_save;
	protocol.system_set_list[19] = m_param.is_shielding_flowers;
	protocol.system_set_list[20] = m_param.guild_answer_show_role_num;
	protocol.system_set_list[21] = m_param.battle_control_mode;
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(protocol.system_set_list) > 21);
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&protocol, sizeof(protocol));
}


void RoleSetting::SendSystemSettingChatGroup()
{
	const CommonDataParam& common_data = m_role_module_mgr->GetCommonData();

	Protocol::SCRoleSystemSetChatGroupInfo info;
	info.chat_group_num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(common_data.cross_chat_group_list) && i < ARRAY_ITEM_COUNT(m_param.is_notice_chat_group) && info.chat_group_num < ARRAY_ITEM_COUNT(info.system_set_group_list); ++i)
	{
		if (common_data.cross_chat_group_list[i] > 0)
		{
			info.system_set_group_list[info.chat_group_num].chat_group_id = common_data.cross_chat_group_list[i];
			info.system_set_group_list[info.chat_group_num].value = m_param.is_notice_chat_group[i];
			info.chat_group_num += 1;
		}
	}
	
	int send_len = sizeof(info) - sizeof(info.system_set_group_list) + info.chat_group_num * sizeof(info.system_set_group_list[0]);
	m_role_module_mgr->NetSend(&info, send_len);
}

void RoleSetting::CheckChatGroupSettingTransfer()
{
	if (0 == m_param.is_transfer_chat_group_data)
	{
		m_param.is_transfer_chat_group_data = 1;

		m_param.is_notice_chat_group[0] = m_param.is_notice_chat_group_1_OBSOLETE;
		m_param.is_notice_chat_group[1] = m_param.is_notice_chat_group_2_OBSOLETE;
		m_param.is_notice_chat_group[2] = m_param.is_notice_chat_group_3_OBSOLETE;
		m_param.is_notice_chat_group[3] = m_param.is_notice_chat_group_4_OBSOLETE;
		m_param.is_notice_chat_group[4] = m_param.is_notice_chat_group_5_OBSOLETE;

		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_param.is_notice_chat_group) >= 5);
	}
}

void RoleSetting::SetShieldingFlowers(bool _is_shielding)
{
	m_param.is_shielding_flowers = _is_shielding ? 1 : 0;
}

void RoleSetting::SetBattleControlMode(int mode)
{
	if (mode < 0
		|| mode >= RoleSetSystemParam::SYSTEM_SET_BATTLE_CONTROL_MODE_NUM)
	{
		return;
	}

	m_param.battle_control_mode = mode;
}

RoleSetSystemParam & RoleSetting::GetSetSystemParam()
{
	return m_param;
}

