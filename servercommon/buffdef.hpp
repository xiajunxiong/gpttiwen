#ifndef __BUFF_DEF_H__
#define __BUFF_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int ROLE_BUFF_MAX_SAVE_NUM = 10;
static const int ROLE_BUFF_ATTR_NUM_MAX = 4;

enum ROLE_BUFF_ORIGIN_TYPE
{
	ROLE_BUFF_ORIGIN_TYPE_MIN = 0,

	ROLE_BUFF_ORIGIN_TYPE_TREVI_FOUNTAIN_BLESSING,		// ��Ը��

	ROLE_BUFF_ORIGIN_TYPE_MAX,
};

enum ROLE_BUFF_TYPE
{
	ROLE_BUFF_TYPE_MIN = 0,

	ROLE_BUFF_CHANGE_ATTR,					// �ı�����
	ROLE_BUFF_EXP_UP,						// ��þ�������
	ROLE_BUFF_CHANGE_MOVE_SPEED,			// �ƶ��ٶȸı�

	ROLE_BUFF_TYPE_MAX,
};

struct BuffParamChangeAttr
{
	int count;				// buff����
	char attr_type[ROLE_BUFF_ATTR_NUM_MAX];		// ��������
	int attr_value[ROLE_BUFF_ATTR_NUM_MAX];		// ����ֵ
};

struct BuffParamExpUp
{
	int up_per;				// ����������
};

struct BuffParamChangeMoveSpeed
{
	int change_per;			// ���ٰٷֱ�
};

union NetValueBuffParam
{
	NetValueBuffParam() // union�ɴ��й��캯���������Ա�������Զ���Ĺ��캯��
	{
		this->Reset();
	}

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	NetValueBuffParamData data;
	BuffParamChangeAttr change_attr;
	BuffParamExpUp exp_up;
	BuffParamChangeMoveSpeed change_move_speed;
};

struct BuffNode
{
	BuffNode() { this->Reset(); }
	void Reset()
	{
		begin_time = 0;
		end_time = 0;

		origin_type = 0;
		buff_type = 0;
		fb_type = 0;
		data.Reset();
	}

	unsigned int begin_time;
	unsigned int end_time;

	int origin_type;
	int buff_type;
	int fb_type;
	NetValueBuffParam data;
};

struct BuffListParam
{
	BuffListParam() { this->Reset(); }

	void Reset()
	{
		buff_count = 0;
		for (int i = 0; i < ROLE_BUFF_MAX_SAVE_NUM; i++)
		{
			buff_list[i].Reset();
		}
	}

	int buff_count;
	BuffNode buff_list[ROLE_BUFF_MAX_SAVE_NUM];
};

typedef char BuffListParamHex[sizeof(BuffListParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BuffListParamHex) < 1500);

UNSTD_STATIC_CHECK(sizeof(BuffParamChangeAttr) <= sizeof(NetValueBuffParamData));
UNSTD_STATIC_CHECK(sizeof(BuffParamExpUp) <= sizeof(NetValueBuffParamData));
UNSTD_STATIC_CHECK(sizeof(BuffParamChangeMoveSpeed) <= sizeof(NetValueBuffParamData));

#pragma pack(pop)

#endif
