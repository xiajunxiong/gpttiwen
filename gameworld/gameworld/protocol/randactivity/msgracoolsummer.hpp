#ifndef __MSG_RA_COOL_SUMMER_HPP__
#define __MSG_RA_COOL_SUMMER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/racoolsummerparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//�����ۿ��̳�
	enum RA_SUPER_OFF_SHOP_REQ_TYPE
	{	
		RA_SUPER_OFF_SHOP_REQ_TYPE_INFO = 0,									//������Ϣ
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_NORMAL_ITEM = 1,							//��������� param1:seq(����seq)
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_DISC_ITEM = 2,							//�������ۿ۵��� param1:seq(�·�phase_rand_disc_item_list����seq)
		RA_SUPER_OFF_SHOP_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK = 3,				//����ֱ������ǰ����˴��������ü��  param1:seq param2: 0.�ۿۻ��� 1:��������
	};

	class SCRASuperOffShopInfo				//!< 3081  ����-�����ۿ��̳���Ϣ�·�
	{
	public:
		SCRASuperOffShopInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;												//0:������ 1:����׶�
		short phase_rand_disc_item_list[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];		//ˢ�����ۿ���Ʒ�б�			MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM - 25
		short today_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM];	//��������ÿ���޹�			MAX_RA_SUPER_OFF_SHOP_PHASE_NORMAL_ITEM_NUM - 27
		short today_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM];		//�ۿۻ���ÿ���޹�			MAX_RA_SUPER_OFF_SHOP_PHASE_DISC_ITEM_NUM - 5
		short activity_normal_buy_times[MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM];		//�������ܻ�޹�			MAX_RA_SUPER_OFF_SHOP_NORMAL_ITEM_NUM - 135
		short activity_disc_buy_times[MAX_RA_SUPER_OFF_SHOP_DISC_ITEM_NUM];			//�ۿۻ��ܻ�޹�			
	};

	class SCRASuperOffShopSingleInfo		//!< 3083  ����-�����ۿ��̳ǵ�����Ϣ�·�
	{
	public:
		SCRASuperOffShopSingleInfo();
		MessageHeader header;

		enum NOTICE_TYPE
		{
			NOTICE_TYPE_NORMAL = 1,		//�������� param1:seq(ÿ��) param2:index(�)
			NOTICE_TYPE_DISC = 2,		//�ۿۻ��� param1:index(ÿ��) param2:seq(�)
		};
		int notice_type;
		short param1;
		short param2;
		short today_buy_times;
		short activity_buy_times;
	};

	//������ˬ
	enum RA_COOL_BREEZE_REQ_TYPE
	{
		RA_COOL_BREEZE_REQ_TYPE_INFO = 0,					//������Ϣ�·�
		RA_COOL_BREEZE_REQ_TYPE_COMMIT_TASK = 1,			//�����������	  param1:�����±�(index) param2:0:�̶����� 1:�������
		RA_COOL_BREEZE_REQ_TYPE_REFRESH_TASK = 2,			//����ˢ�¹̶�����  param1:�����±�(index)
	};

	class SCRACoolBreezeInfo		//!< 3085 ����-������ˬ��Ϣ�·�
	{
	public:
		SCRACoolBreezeInfo();
		MessageHeader header;

		short last_refresh_fixe_open_day;												//�ϴ�ˢ�¹̶�����ʱ��Ӧ�Ļ��������
		short last_refresh_random_open_day;												//�ϴ�ˢ���������ʱ��Ӧ���������
		unsigned int next_fixe_task_refresh_timestamp;									//�´ι̶�����ˢ��ʱ���			
		unsigned int next_random_task_refresh_timestamp;								//�´��������ˢ��ʱ���
		RACoolBreezeTaskInfo fixe_task_list[MAX_RA_COOL_BREEZE_FIXE_TASK_NUM];			//�̶������б�
		RACoolBreezeTaskInfo random_task_list[MAX_RA_COOL_BREEZE_RANDOM_TASK_NUM];		//��������б�
	};

	class SCRACoolBreezeSingleInfo		//!< 3086 ����-������ˬ������Ϣ�·�
	{
	public:
		SCRACoolBreezeSingleInfo();
		MessageHeader header;

		short index;	
		short task_group_type;				//1:�̶����� 2:�������
		RACoolBreezeTaskInfo task_info;
	};

	//�����ٱ�
	enum RA_LOTUS_MI_BAO_REQ_TYPE
	{
		RA_LOTUS_MI_BAO_REQ_TYPE_INFO = 0,						//������Ϣ�·�
		RA_LOTUS_MI_BAO_REQ_TYPE_ACTIVE_GRID = 1,				//���󼤻�ĳ������ param1:row[0,3] param2:column[0,3]
		RA_LOTUS_MI_BAO_REQ_TYPE_FETCH_ACTIVE_ALL_REWARD = 2,	//��ȡ���뽱��
	};

	class SCRALotusMiBaoInfo			//!< 3087  ����-�����ٱ���Ϣ�·�
	{
	public:
		SCRALotusMiBaoInfo();
		MessageHeader header;

		short reserve_sh;
		char reserve_ch;
		char is_fetch_all_reward_flag;								//��ȡ���뽱����ʶ  0:δ��ȡ 1:����ȡ
		//BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//�ճ�����������ʶ һ�л�һ��:(cond - 1) * 10 + (param - 1)  ����ȫ��(3)RA_LOUTS_MI_BAO_COND_NUM - 1  
		BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//ÿ�����ӵļ����ʶ	row[0, 3] * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM + column[0,3]
		
	};

	//�����ٱ�
	enum RA_SHAN_HAI_MI_BAO_REQ_TYPE
	{
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_INFO = 0,						//������Ϣ�·�
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_ACTIVE_GRID = 1,				//���󼤻�ĳ������ param1:row[0,3] param2:column[0,3]
		RA_SHAN_HAI_MI_BAO_REQ_TYPE_FETCH_ACTIVE_ALL_REWARD = 2,	//��ȡ���뽱��
	};

	class SCRAShanHaiMiBaoInfo			//!< 3122  ����ɽ���ٱ���Ϣ�·�
	{
	public:
		SCRAShanHaiMiBaoInfo();
		MessageHeader header;

		short reserve_sh;
		char reserve_ch;
		char is_fetch_all_reward_flag;								//��ȡ���뽱����ʶ  0:δ��ȡ 1:����ȡ
																	//BitMap<RA_LOUTS_MI_BAO_COND_NUM> cond_flag;					//�ճ�����������ʶ һ�л�һ��:(cond - 1) * 10 + (param - 1)  ����ȫ��(3)RA_LOUTS_MI_BAO_COND_NUM - 1  
		BitMap<MAX_RA_LOTUS_MI_BAO_GRID_NUM> grid_active_flag;		//ÿ�����ӵļ����ʶ	row[0, 3] * MAX_RA_LOTUS_MI_BAO_COLUMN_NUM + column[0,3]

	};
}


#pragma pack(pop)

#endif	//__MSG_RA_COOL_SUMMER_HPP__
