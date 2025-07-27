#ifndef __RA_CONSUME_CAROUSEL_PARAM_HPP__
#define __RA_CONSUME_CAROUSEL_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2055 ���� - ����ת��  //////////////////////////////////////////

//-----------------------------����ת��------------------------------------
static const int CONSUME_CAROUSEL_MAX_NUM = 12;
static const int RA_SHOW_INFO_MAX_NUM = 10;

struct RAShowInfoParam
{
	RAShowInfoParam()
	{
		this->Reset();
	}
	void Reset()
	{
		count = 0;
		for (int i = 0; i < RA_SHOW_INFO_MAX_NUM; i++)
		{
			m_ra_show_data_list[i].Reset();
		}

	}
	struct RAShowInfoDataParam
	{
		RAShowInfoDataParam()
		{
			this->Reset();
		}
		void Reset()
		{
			item_id = 0;
			num = 0;
			memset(role_name, 0, sizeof(role_name));
		}

		unsigned short item_id;
		short num;
		GameName role_name;
	};
	int count;			//��ǰ��ȡ����ϡ��Ʒ�Ĵ��� ������10
	RAShowInfoDataParam m_ra_show_data_list[RA_SHOW_INFO_MAX_NUM];
};


#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RAConsumerCarouselParam
{
	RAConsumerCarouselParam() { this->Reset(); }

	void Reset()
	{
		ra_consume_carousel_begin_timestamp = 0;
		old_ra_consume_carousel_last_num = 0;
		old_ra_consume_carousel_consume_num = 0;
		ra_consume_carousel_last_index = -1;
		old_num_data_move_flag = 0;
		reserve_ch = 0;
		ra_today_consume_num = 0;
		ra_consume_num_in_time = 0;
		new_ra_consume_carousel_last_num = 0;
		new_ra_consume_carousel_consume_num = 0;
	}

	unsigned int ra_consume_carousel_begin_timestamp;
	short old_ra_consume_carousel_last_num;								//ʣ����� -> ����
	short old_ra_consume_carousel_consume_num;							//���Ĵ��� -> ����
	short ra_consume_carousel_last_index;								//�ϴγ�ȡ�ĵ���index
	char old_num_data_move_flag;										//�ɴ���Ǩ�Ʊ�ʶ 0:δǨ�� 1:��Ǩ��
	char reserve_ch;													//����
	unsigned int ra_today_consume_num;									//�����ۻ�����
	unsigned int ra_consume_num_in_time;								//�ʱ�����ۻ�����
	int new_ra_consume_carousel_last_num;								//ʣ�����
	int new_ra_consume_carousel_consume_num;							//���Ĵ���
};

UNSTD_STATIC_CHECK(sizeof(RAConsumerCarouselParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_CONSUME_CAROUSEL_PARAM_HPP__
