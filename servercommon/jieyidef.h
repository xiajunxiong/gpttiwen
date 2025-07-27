#ifndef __JIE_YI_ZU_DEF_DEF_H__
#define __JIE_YI_ZU_DEF_DEF_H__

const static int JIE_YI_ZU_MAX_ROLE_ITEM_NUM = 5;
const static int JIE_YI_ZU_NAME_LENGTH = 12;
const static int JIE_YI_ZU_MEMBER_NAME_LENGTH = 8; 
const static int JIE_YI_ZU_TEXT_LENGTH = 128;
const static int JIE_YI_ZU_LEAVE_WAIT_TIME = 3600;
const static int JIE_YI_ZU_KICK_LOGOUT_TIME = 2 * SECOND_PER_DAY;
const static int JIE_YI_ZU_XUAN_YAN_TEXT_LENGTH = 300;
const static int JIE_YI_XUAN_YAN_ITEM_MAX_NUM = 50;
const static int JIE_YI_MOD_VER = 1;

struct JieYiZuRoleBaseInfo
{
	JieYiZuRoleBaseInfo()
	{
		uid = 0;
		memset(name, 0, sizeof(name));
		role_level = 0;
		prof = 0;
		avatar_type = 0;
		headshot_id = 0;
	}

	int uid;
	GameName name;
	int role_level;
	int prof;
	short		avatar_type;
	short		headshot_id;
};

struct JieYiZuCreateRole
{
	JieYiZuCreateRole()
	{
		is_confirm = 0;
		is_zu_zhang = 0;
	}

	JieYiZuRoleBaseInfo role_base_info; 
	short is_confirm;
	short is_zu_zhang;
};

typedef  char JieYIZuMemberName[JIE_YI_ZU_MEMBER_NAME_LENGTH];
typedef  char JieYIZuText[JIE_YI_ZU_TEXT_LENGTH];
typedef  char JieYIZuXuanYanText[JIE_YI_ZU_XUAN_YAN_TEXT_LENGTH];

//---------------------以下是存库结构，注意对齐--------------------------------------
#pragma pack(push, 4)
struct JieYIZuName
{
	void Reset()
	{
		memset(str1, 0, sizeof(str1));
		memset(str2, 0, sizeof(str2));
		member_num = 0;
	}

	char str1[8];
	char str2[4];
	int member_num;
};
#pragma pack(pop)

#endif
