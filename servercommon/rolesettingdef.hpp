#ifndef __ROLE_SETTING_DEF_HPP__
#define __ROLE_SETTING_DEF_HPP__
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

struct RoleSetSystemParam
{
	enum SYSTEM_SET_INFO
	{
		SYSTEM_SET_INFO_RESET = 0,					// ��ԭĬ��
		SYSTEM_SET_INFO_TOTAL_VOLUME = 1,			// ������					Ĭ��100
		SYSTEM_SET_INFO_MUSIC_VOLUME = 2,			// ����						Ĭ��100
		SYSTEM_SET_INFO_SOUND_EFFECTS = 3,			// ��Ч						Ĭ��100
		SYSTEM_SET_INFO_PICTURE_EFFECTS = 4,		// ����						Ĭ�ϵ�
		SYSTEM_SET_INFO_NUMBER_OF_SCREENS = 5,		// ͬ������					Ĭ��50
		SYSTEM_SET_INFO_SHOW_OTHER_PLAYER = 6,		// �Ƿ���ʾ�������			Ĭ����
		SYSTEM_SET_INFO_SHOW_PLAYER_NAME = 7,		// �Ƿ���ʾ�������			Ĭ����
		SYSTEM_SET_INFO_SHOW_OWN_SIDE_HP = 8,		// �Ƿ���ʾ����HP			Ĭ����
		SYSTEM_SET_INFO_SHOW_SYSTEM_MSG = 9,		// �Ƿ���ʾϵͳ������Ϣ		Ĭ����
		SYSTEM_SET_INFO_REFUSE_TEAM_REQUEST = 10,	// �Ƿ�ܾ��������			Ĭ�Ϸ�
		SYSTEM_SET_INFO_REFUSE_STRANGER_MSG = 11,	// �Ƿ�ܾ�İ������Ϣ		Ĭ�Ϸ�
		SYSTEM_SET_INFO_AUTO_RESEARCH_TASK = 12,	// �Ƿ��Զ�����
		SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_1 = 13,	// �Ƿ���ʾȺ��1��Ϣ		Ĭ����
		SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_2 = 14,	// �Ƿ���ʾȺ��2��Ϣ		Ĭ����
		SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_3 = 15,	// �Ƿ���ʾȺ��3��Ϣ		Ĭ����
		SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_4 = 16,	// �Ƿ���ʾȺ��4��Ϣ		Ĭ����
		SYSTEM_SET_INFO_NOTICE_CHAT_GROUP_5 = 17,	// �Ƿ���ʾȺ��5��Ϣ		Ĭ����
		SYSTEM_SET_INFO_JOYSTICK = 18,				// �Ƿ�ʹ��ҡ��				Ĭ����
		SYSTEM_SET_INFO_POWER_SAVE = 19,			// �Ƿ�����ʡ��ģʽ			Ĭ�ϲ�����
		SYSTEM_SET_INFO_SHIELDING_FLOWERS = 20,		// �����ͻ���Ч				Ĭ�ϲ�����
		SYSTEM_SET_INFO_GUILD_ANSWER_SHOW_ROLE_NUM = 21,		// ���������ʾ����		Ĭ��100
		SYSTEM_SET_INFO_BATTLE_CONTROL_MODE = 22,	// ѡ��ս������ģʽ

		SYSTEM_SET_INFO_MAX,
	};

	enum VOLUME_PERSENT
	{
		VOLUME_PERSENT_CLOSE = -101,
		VOLUME_PERSENT_MIN = 0,
		VOLUME_PERSENT_MAX = 101,
	};

	enum PICTURE_QUALITY
	{
		PICTURE_QUALITY_DEFAULT = -1,
		PICTURE_QUALITY_LOW = 0,	
		PICTURE_QUALITY_MIDDLE = 1,
		PICTURE_QUALITY_HIGH = 2,
	};

	enum NUMBER_OF_SCREENS
	{
		NUMBER_OF_SCREENS_MIN = 0,
		NUMBER_OF_SCREENS_MAX = 100,
	};

	enum SYSTEM_SET_COMMON
	{
		SYSTEM_SET_COMMON_NO = 0,
		SYSTEM_SET_COMMON_YES = 1,
	};

	enum SYSTEM_SET_BATTLE_CONTROL_MODE
	{
		SYSTEM_SET_BATTLE_CONTROL_MODE_CLASSIC = 0,		// ����
		SYSTEM_SET_BATTLE_CONTROL_MODE_SLIDE = 1,		// ����
		SYSTEM_SET_BATTLE_CONTROL_MODE_NUM,
	};

	RoleSetSystemParam()
	{
		this->Reset();
	}

	void Reset()
	{
		total_volume = VOLUME_PERSENT_MAX;
		music_volume = VOLUME_PERSENT_MAX;
		sound_effect = VOLUME_PERSENT_MAX;
		picture_quality = PICTURE_QUALITY_DEFAULT;
		number_of_screens = NUMBER_OF_SCREENS_MAX;
		is_show_other_player = SYSTEM_SET_COMMON_YES;
		is_show_player_name = SYSTEM_SET_COMMON_YES;
		is_show_own_hp = SYSTEM_SET_COMMON_YES;
		is_show_system_msg = SYSTEM_SET_COMMON_YES;
		is_refuse_team_requset = SYSTEM_SET_COMMON_NO;
		is_refuse_stranger_msg = SYSTEM_SET_COMMON_NO;
		is_default = SYSTEM_SET_COMMON_YES;
		is_auto_research_task = SYSTEM_SET_COMMON_YES;
		is_notice_chat_group_1_OBSOLETE = SYSTEM_SET_COMMON_YES;
		is_notice_chat_group_2_OBSOLETE = SYSTEM_SET_COMMON_YES;
		is_notice_chat_group_3_OBSOLETE = SYSTEM_SET_COMMON_YES;
		is_notice_chat_group_4_OBSOLETE = SYSTEM_SET_COMMON_YES;
		is_notice_chat_group_5_OBSOLETE = SYSTEM_SET_COMMON_YES;
		is_transfer_chat_group_data = 0;
		is_use_joystick = SYSTEM_SET_COMMON_YES;
		is_power_save = SYSTEM_SET_COMMON_NO;

		for (int i = 0; i < ARRAY_ITEM_COUNT(is_notice_chat_group); ++i)
		{
			is_notice_chat_group[i] = SYSTEM_SET_COMMON_YES;
		}

		is_shielding_flowers = SYSTEM_SET_COMMON_NO;
		guild_answer_show_role_num = NUMBER_OF_SCREENS_MAX;
		battle_control_mode = SYSTEM_SET_BATTLE_CONTROL_MODE_CLASSIC;
		reserve_ch = 0;
	}
	short total_volume;
	short music_volume;
	short sound_effect;
	short picture_quality;
	int number_of_screens;
	char is_show_other_player;
	char is_show_player_name;
	char is_show_own_hp;
	char is_show_system_msg;
	char is_refuse_team_requset;
	char is_refuse_stranger_msg;
	char is_default;
	char is_auto_research_task;
	char is_notice_chat_group_1_OBSOLETE;
	char is_notice_chat_group_2_OBSOLETE;
	char is_notice_chat_group_3_OBSOLETE;
	char is_notice_chat_group_4_OBSOLETE;
	char is_notice_chat_group_5_OBSOLETE;
	char is_transfer_chat_group_data;
	char is_use_joystick;
	char is_power_save;
	char is_notice_chat_group[20];
	char is_shielding_flowers;
	unsigned char guild_answer_show_role_num;
	char battle_control_mode;
	char reserve_ch;
};
UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(RoleSetSystemParam::is_notice_chat_group) == MAX_ROLE_CROSS_GROUP);
UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(RoleSetSystemParam::is_notice_chat_group) % 4 == 0);

typedef char RoleSetSystemParamHex[sizeof(RoleSetSystemParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleSetSystemParamHex) < 128);

#pragma pack(pop)

#endif // !__ROLE_SETTING_DEF_HPP__
