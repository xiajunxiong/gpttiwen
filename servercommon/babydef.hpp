#ifndef __BABY_DEF_HPP__
#define __BABY_DEF_HPP__
#include "servercommon/servercommon.h"

#pragma pack(push, 4)

const static int MAX_BABY_NUM = 2;

enum CREATE_BABY_STAGE
{
	CREATE_BABY_STAGE_BEI_YUN,
	CREATE_BABY_STAGE_HUAI_YUN,
	CREATE_BABY_STAGE_TAI_JAIO,
	CREATE_BABY_STAGE_ATIVTE,
	CREATE_BABY_STAGE_MAX,
};

enum BABY_CREATE_TYPE
{
	BABY_CREATE_TYPE_INVAILD,
	BABY_CREATE_TYPE_SINGLE,		//����������ʽ
	BABY_CREATE_TYPE_DUET,		//˫��������ʽ
};

struct RoleBabyParam
{
	RoleBabyParam()
	{
		this->Reset();
	}

	void Reset()
	{
		last_reset_time = 0;
		single_ji_fen = 0;
		draw_times = 0;
		worship_times = 0;

		create_type = 0;
		reserved_ch = 0;
		reserved_sh = 0;

		creating_baby_id = 0;


		for (int i = 0; i < MAX_BABY_NUM; ++i)
		{
			baby_id_list[i] = 0;
		}
	}

	unsigned int last_reset_time;
	unsigned short single_ji_fen;	//�������� ����
	unsigned char draw_times;	//�������� ÿ���ǩ����
	unsigned char worship_times;   //�������� ÿ�칩�����	

	unsigned char create_type;	//���ڽ����������� 1�ǵ��ˣ�2��˫��
	unsigned char reserved_ch;
	short reserved_sh;

	long long creating_baby_id;			//����������baby_id
	long long baby_id_list[MAX_BABY_NUM];	//�Ѿ�������baby_id
};

typedef char BabyParamHex[sizeof(RoleBabyParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(BabyParamHex) < 1024);

#pragma pack(pop)

#endif // !__BABY_DEF_HPP__
