#ifndef __MSG_RA_BURNING_SUMMER_HPP__
#define __MSG_RA_BURNING_SUMMER_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raburningsummerparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	//��������
	enum RA_SUMMER_LI_LIAN_REQ_TYPE		//��Э��3000
	{
		RA_SUMMER_LI_LIAN_REQ_TYPE_CHALLENGE = 0,				// ��ս			param1:���û����activity_time
		RA_SUMMER_LI_LIAN_REQ_TYPE_FETCH_ALL_PASS_REWARD = 1,	// ��ȡȫͨ���� 
	};

	class SCRASummerLiLianAllInfo				//!< 3075 ����-����������Ϣ�·�
	{	
	public:
		SCRASummerLiLianAllInfo();
		MessageHeader header;

		short open_day;												//���������
		char is_fetch_all_pass_flag;								//�Ƿ���ȡ��ͨ������ʶ 0:δ��ȡ 1:����ȡ
		char reserve_ch;
		BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//�Ƿ���ɱ��ʶ MAX_RA_SUMMER_LI_LIAN_BOSS_NUM - 31	[1,31]
	};

	//��ʻ���
	enum RA_COLORFUL_LANTERN_REQ_TYPE	//��Э��3000
	{
		RA_COLORFUL_LANTERN_REQ_TYPE_START_GAME = 0,				// ��ʼ��Ϸ
		RA_COLORFUL_LANTERN_REQ_TYPE_CATCH_LANTERN = 1,				// ��ץ����		param1:type
		RA_COLORFUL_LANTERN_REQ_TYPE_FETCH_REWARD = 2,				// ��ȡ������	param1:task_id
		RA_COLORFUL_LANTERN_REQ_TYPE_FORCE_CLOSE = 3,				// ǿ�ƹر�
		RA_COLORFUL_LANTERN_REQ_TYPE_FLOW_AWAY = 4,					// ��������		param1:type

		RA_COLORFUL_LANTERN_REQ_TYPE_MAX
	};

	class SCRAColorfulLanternAllInfo		//!< 3076 ����-��ʻ�����Ϣ�·�
	{
	public:
		SCRAColorfulLanternAllInfo();
		MessageHeader header;

		short last_game_score_count;								//��һ����Ϸ��õĻ���
		short day_single_score_count;								//���쵥�λ������ֵ
		short activity_single_score_count;							//��ڵ��λ������ֵ
		short day_join_times;										//���ղ������
		short activity_join_times;									//��ڲ������
		short day_score_count;										//���ջ�õ��ܷ�
		int activity_score_count;									//��ڻ�õ��ܻ���
		BitMap<MAX_RA_COLORFUL_LANTERN_TASK_NUM> m_task_list_flag;	//�����б��ʶ 0:δ��� 1:�����
	};

	enum RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE
	{
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_START_GAME = 0,			//��Ϸ��ʼ
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_IN_THE_GAME = 1,			//��Ϸ������
		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_GAME_OVER = 2,				//��Ϸ�����·�����

		RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_MAX
	};

	class SCRAColorfulLanternGameInfo	//!< 3077 ����-��ʻ�����Ϸ��Ϣ�·�
	{
	public:
		SCRAColorfulLanternGameInfo();
		MessageHeader header;

		short notice_type;							//RA_COLORFUL_LANTERN_GAME_NOTICE_TYPE_MAX
		short single_score_count;					//������Ϸ�ڵĻ���
		unsigned int game_end_timestamp;			//������Ϸ����ʱ���
	};

	class SCRAColorfulLanternLightInfo	//!< 3078 ����-��ʻ��ƻ�����Ϣ�·�
	{
	public:
		SCRAColorfulLanternLightInfo();
		MessageHeader header;

		short light_list[MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM];		//��������  MAX_RA_COLORFUL_LANTERN_LIGHT_TYPE_NUM  -- 10
	};

	//�����̵�
	enum RA_SUMMER_STORE_REQ_TYPE
	{
		RA_SUMMER_STORE_REQ_TYPE_INFO = 0,									//������Ϣ�·�
		RA_SUMMER_STORE_REQ_TYPE_BUY_ITEM = 1,								//��������� param1:seq
		RA_SUMMER_STORE_REQ_TYPE_BUY_CMD_ITEM_CONFIG_CHECK = 2,				//����ֱ������ǰ��������������ü�� param:seq
		RA_SUMMER_STORE_REQ_TYPE_OPEN_ACTIVITY = 3,							//�򿪻����
	};

	class SCRASummerStoreInfo		//!< 3079 ����-�����̵���Ϣ�·�
	{
	public:
		SCRASummerStoreInfo();
		MessageHeader header;

		short is_need_recal_phase_flag;											//0:������ 1:����׶�
		short is_open_today_flag;												//�����Ƿ�򿪹����� 0:û�� 1���򿪹�
		short today_buy_times[MAX_RA_SUMMER_STORE_PHASE_SHOP_ITEM_NUM];			//ÿ���޹�
		short activity_buy_times[MAX_RA_SUMMER_STORE_SHOP_ITEM_NUM];			//��޹�
	};

	class SCRASummerStoreSingleInfo		//!< 3082 ����-�����̵굥����Ϣ�·�
	{
	public:
		SCRASummerStoreSingleInfo();
		MessageHeader header;

		short seq;				
		short index;
		short today_buy_times;
		short activity_buy_times;
	};

	//���˻�ǩ
	enum RA_LUCKY_SIGN_REQ_TYPE
	{
		RA_LUCKY_SIGN_REQ_TYPE_INFO = 0,						//������Ϣ�·�
		RA_LUCKY_SIGN_REQ_TYPE_SET_SHOW_FLAG = 1,				//�������õ�ǰ����� param1: 0.ǩ�Ľ��� 1.��ǩ����
		RA_LUCKY_SIGN_REQ_TYPE_DAY_SIGN = 2,					//����ÿ��ǩ��
	};

	class SCRALuckySignInfo			//!< 3080 ����-���˻�ǩ��Ϣ�·�
	{
	public:
		SCRALuckySignInfo();
		MessageHeader header;

		char is_base_show_flag;			//0:ǩ�Ľ��� 1:��ǩ����
		char is_fetch_reward_flag;		//0:δ��ȡ	1: ����ȡ
		short sign_id;
		int reward_type_seq;
	};

	enum RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE
	{
		RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_CHALLENGE = 0,				// ��ս			param1:���û����activity_time
		RA_SHAN_HAI_ZHENG_FENG_REQ_TYPE_FETCH_ALL_PASS_REWARD = 1,	// ��ȡȫͨ���� 
	};

	class SCRAShanHaiZhengFengAllInfo				//!< 3121 ����-ɽ��������Ϣ�·�
	{
	public:
		SCRAShanHaiZhengFengAllInfo();
		MessageHeader header;

		short open_day;												//���������
		char is_fetch_all_pass_flag;								//�Ƿ���ȡ��ͨ������ʶ 0:δ��ȡ 1:����ȡ
		char reserve_ch;
		BitMap<MAX_RA_SUMMER_LI_LIAN_BOSS_NUM> m_first_flag;		//�Ƿ���ɱ��ʶ MAX_RA_SUMMER_LI_LIAN_BOSS_NUM - 31	[1,31]
	};
}


#pragma pack(pop)

#endif // __MSG_RA_BURNING_SUMMER_HPP__
