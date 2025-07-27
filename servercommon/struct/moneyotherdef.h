#ifndef __MONEY_OTHER_DEF_H__
#define __MONEY_OTHER_DEF_H__

#include "servercommon/servercommon.h"

const static int MAX_MONEY_TYPE_RESERVED = 20;

enum MoneyType
{
	MONEY_TYPE_ALL								= -2,		// Э����
	MONEY_TYPE_CONVERT_ITEM						= -1,		// ���߶һ�ʱʹ��
	MONEY_TYPE_GOLD								= 0,		// Ԫ��
	MONEY_TYPE_IMMORTAL_COIN					= 1,		// �ɱ�(������)
	MONEY_TYPE_COIN_BIND						= 2,		// ͭ��
	MONEY_TYPE_GUILD_CONTRIBUTION				= 3,		// ���ṱ��
	MONEY_TYPE_CHIVALROUS						= 4,		// ����ֵ
	MONEY_TYPE_COLOSSEUM						= 5,		// ����������
	MONEY_TYPE_FACE_SCORE						= 6,		// ��ֵ��
	MONEY_TYPE_SURFACE_COIN						= 7,		// ��װ��
	MONEY_TYPE_PLATFORM_BATTLE_CREDITS			= 8,		// ��̨ս����
	MONEY_TYPE_RESERVE							= 9,		// ����
	MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS		= 10,		// �ý�ս������
	MONEY_TYPE_LUCK_COIN						= 11,		// ����ֵ
	MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN			= 12,		// �ý�Ӳ��
	MONEY_TYPE_YUAN_SUI_ZHI_LING				= 13,		// Ԫ��֮��
	MONEY_TYPE_DUI_ZHANG_GONG_ZI				= 14,		// �ӳ�����
	MONEY_TYPE_CLOUD_ARENA_COIN					= 15,		// �ƶ˾�����
	MONEY_TYPE_SILVER_COIN						= 16,		// ����(����) 
	MONEY_TYPE_FEN_CHEN							= 17,		// �۳� (����ֽ�����)
	MONEY_TYPE_GUESS_COIN						= 18,		// ���±�
	MONEY_TYPE_CAN_HUN							= 19,		// �л꣬Ԫ����������Դ
	MONEY_TYPE_MAX
};

UNSTD_STATIC_CHECK(MONEY_TYPE_MAX <= MAX_MONEY_TYPE_RESERVED);

struct MoneyOtherParam
{
	MoneyOtherParam() { this->Reset(); }

	void Reset()
	{
		memset(other_currency, 0, sizeof(other_currency));
	}

	long long other_currency[MAX_MONEY_TYPE_RESERVED];
};

typedef char MoneyOtherParamHex[sizeof(MoneyOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MoneyOtherParamHex) < 512);



#endif	//__MONEY_OTHER_DEF_H__