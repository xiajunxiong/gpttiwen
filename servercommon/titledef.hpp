#ifndef __TITLE_DEF_HPP__
#define __TITLE_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

enum TITLESOURCETYPE
{
	TITLESOURCETYPE_INVALID = 0,

	TITLESOURCETYPE_DEFAULT = 1,					// ����
	TITLESOURCETYPE_CALCULATION = 2,				// �����棬�����
	TITLESOURCETYPE_COLOSSEUM = 3,					// ���񱦿�
	TITLESOURCETYPE_BRAVE_GROUND_TOP_ROLE = 4,					// �´��ع����а�ǰ3
	TITLESOURCETYPE_HUAN_JIE_ZHAN_CHANG_TOP_ROLE = 5,					// �ý�ս�����������ƺ�
	TITLESOURCETYPE_GUILD_HONOR_BATTLE = 6,					// ��������ս
	TITLESOURCETYPE_CLOUD_ARENA = 7,				// �ƶ˾���
	TITLESOURCETYPE_QING_YUAN = 8,				// ��Ե

	TITLESOURCETYPE_MAX,
};


static const int TITLE_MAX_NUM = 200;

struct TitleData
{
	TitleData(UInt16 _title_id = 0, UInt16 _reserve_ush = 0, UInt32 _due_time = 0)
	{
		title_id = _title_id;
		reserve_ush = _reserve_ush;
		due_time = _due_time;
	}

	void Reset()
	{
		title_id = 0;
		reserve_ush = 0;
		due_time = 0;
	}

	UInt16 title_id;				// �ƺ�id
	UInt16 reserve_ush;
	UInt32 due_time;				// ����ʱ��  0 ��ʾ����ʱ
};

struct TitleParam
{
	TitleParam() { this->Reset(); }

	void Reset()
	{
		wear_index = -1;
		wear_title_id = 0;
		for (int i = 0; i < TITLE_MAX_NUM; ++i)
		{
			title_list[i].Reset();
		}
		is_first_create_role = 0;
		is_first_syn_title_to_facescore = 0;
		reserve_short = 0;
	}

	short wear_index;						// ���ŵĳƺ�index
	UInt16 wear_title_id;					// ���ŵĳƺ�id����ֹ��Щ�ƺŲ����棬��¼ʱ�����ż����Ƿ�ӵ�У�index�ͻ��
	TitleData title_list[TITLE_MAX_NUM];
	char is_first_create_role;				// �ж�����Ƿ��Ǹմ�����ɫ 0 �մ��� 1 �Ͻ�ɫ
	char is_first_syn_title_to_facescore;	// �Ƿ�ͬ���ƺŵ���ֵϵͳ 0:δͬ�� 1:��ͬ��
	short reserve_short;
};

typedef char TitleParamHex[sizeof(TitleParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TitleParamHex) < 4096);

#pragma pack(pop)
#endif // !__TITLE_DEF_HPP__