#ifndef __RA_SHEN_CI_HUA_FU_PARAM_HPP__
#define __RA_SHEN_CI_HUA_FU_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#pragma pack(push, 4)
//////////////////////////////////////////// 2217 ���� - ��ͻ��� //////////////////////////////////////////
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////
const static int SHEN_CI_HUA_FU_SELECT_NUM_SELECT = 3;		//��ѡ��������
const static int SHEN_CI_HUA_FU_SELECT_NUM_RAND = 6;		//����ѡ��������
const static int SHEN_CI_HUA_FU_MAX_BUY_ITEM = 20;		//
const static int SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM = 26;		//

struct RAShenCiHuaFuItem
{
	RAShenCiHuaFuItem() { this->Reset(); }
	void Reset()
	{
		is_get = 0;
		is_select = 0;
		reserved_sh = 0;
		reward_group_id = 0;
		reward_seq = 0;
	}

	unsigned char is_get;
	unsigned char is_select;
	unsigned short reserved_sh;
	int reward_group_id;
	int reward_seq;
};


struct RAShenCiHuaFuParam
{
	RAShenCiHuaFuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		ArrayFillZero(ra_day_exchange_times);
		ArrayFillZero(ra_exchange_times);
		ArrayCallReset(item_list_select);
		ArrayCallReset(item_list_rand);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];		//20 ÿ����Ʒÿ���޹�����
	unsigned short ra_buy_times[SHEN_CI_HUA_FU_MAX_BUY_ITEM];			//20 ÿ����Ʒ�޹�����
	unsigned short ra_day_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 ÿ����Ʒÿ�� �һ�����
	unsigned short ra_exchange_times[SHEN_CI_HUA_FU_MAX_EXCHANGE_SHOP_ITEM];	//26 ÿ����Ʒ    �һ�����
	RAShenCiHuaFuItem item_list_select[SHEN_CI_HUA_FU_SELECT_NUM_SELECT];		//3 ��ѡ�����б�
	RAShenCiHuaFuItem item_list_rand[SHEN_CI_HUA_FU_SELECT_NUM_RAND];		//6 ����ѡ�����б�
};

UNSTD_STATIC_CHECK(sizeof(RAShenCiHuaFuParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	
