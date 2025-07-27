#ifndef __REALM_BREAK_DEF_HPP__
#define __REALM_BREAK_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_REALM_BREAK_ITEM_NUM = 50;

struct RealmBreakItem
{
	RealmBreakItem() { this->Reset(); }

	void Reset()
	{
		item_id = 0;
		is_bind = 0;
		num = 0;
	}

	ItemID item_id;
	short is_bind;
	int num;
};

struct RealmBreakParam
{
	RealmBreakParam() { this->Reset(); }

	void Reset()
	{
		is_old_user_flag = 0;
		is_change_ver_reset_flag = 0;
		cur_seq = 0;
		tuna_num = 0;
		break_exp = 0;
		cumulation_user_exp_count = 0;
		user_cumulation_pratice_time = 0;
		cumulation_user_coin_count = 0;
		last_rand_item_timestamp = 0;
		item_count = 0;
		cur_fight_seq = 0;
		for (int i = 0; i < MAX_REALM_BREAK_ITEM_NUM; ++i)
		{
			cumulation_item_list[i].Reset();
		}
		memset(reserve_ll, 0, sizeof(reserve_ll));
		last_get_fix_item_timestamp = 0;
		is_reissue = 0;
		memset(reserve_ch, 0, sizeof(reserve_ch));
	}
	void FetchReset()
	{
		cumulation_user_exp_count = 0;
		cumulation_user_coin_count = 0;
		user_cumulation_pratice_time = 0;
		for (int i = 0; i < item_count && i < MAX_REALM_BREAK_ITEM_NUM; ++i)
		{
			cumulation_item_list[i].Reset();
		}
		item_count = 0;
	}

	char is_old_user_flag;							//����ҵ�һ�ε�½���� 1:��Ҫ����
	char is_change_ver_reset_flag;					//����汾������ñ�ʶ 0:δ���� 1:������
	short cur_seq;									//��ǰ����(��Ӧ����seq) ��ҳ�ʼΪ1��	
	int tuna_num;									//��ǰ�׶���ʹ�����ɴ��� �޸�Ϊÿ������(���ײ�����)
	unsigned int break_exp;							//��ǰ����ֵ
	unsigned int cumulation_user_exp_count;			//����ۻ��Ĵ���ȡ����ֵ
	unsigned int user_cumulation_pratice_time;		//����ۻ�������ʱ��
	unsigned int cumulation_user_coin_count;		//����ۻ��Ĵ���ȡͭ��
	unsigned int last_rand_item_timestamp;			//����ϴ��������ʱ���
	short item_count;
	short cur_fight_seq;
	RealmBreakItem cumulation_item_list[MAX_REALM_BREAK_ITEM_NUM];	//����ۻ��Ĵ���ȡ�����б�
	long long reserve_ll[3];

	unsigned int last_get_fix_item_timestamp;		//����ϴλ�ù̶����߽���ʱ���

	char is_reissue;		// �Ƿ񲹷���������
	char reserve_ch[3];
};

typedef char RealmBreakParamHex[sizeof(RealmBreakParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RealmBreakParamHex) < 1024);


#pragma pack(pop)

#endif