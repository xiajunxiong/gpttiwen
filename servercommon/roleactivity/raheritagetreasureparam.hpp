#ifndef __RA_HERITAGE_TREASURE_PARAM_HPP__
#define __RA_HERITAGE_TREASURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// 2057 ���� - ���й屦  //////////////////////////////////////////

static const int RA_HERITAGE_TREASURE_MAX_PIECE = 13;  //�����Ƭ��
static const int RA_HERITAGE_TREASURE_MAX_CLUE_NUM = 3;  //��󼤻���������
static const unsigned char RA_HERITAGE_TREASURE_FETCH_CLUE_REWARD_MARD = 255; // ��ȡ��Ƭ�������

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAHeritageTreasureParam
{
	RAHeritageTreasureParam() { this->Reset(); }
	void Reset()
	{
		ra_heritage_treasure_begin_timestamp = 0;
		memset(ra_heritage_treasure_piece, 0, sizeof(ra_heritage_treasure_piece));
		memset(ra_heritage_treasure_clue, 0, sizeof(ra_heritage_treasure_clue));
		ra_heritage_treasure_is_fetch_final_reward = 0;
		ra_heritage_treasure_reserved_ch = 0;
	}

	unsigned int ra_heritage_treasure_begin_timestamp;
	unsigned char ra_heritage_treasure_piece[RA_HERITAGE_TREASURE_MAX_PIECE];
	unsigned char ra_heritage_treasure_clue[RA_HERITAGE_TREASURE_MAX_PIECE];
	unsigned char ra_heritage_treasure_is_fetch_final_reward;			//���ս�����ȡ��־
	unsigned char ra_heritage_treasure_reserved_ch;
};

UNSTD_STATIC_CHECK(sizeof(RAHeritageTreasureParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_HERITAGE_TREASURE_PARAM_HPP__
