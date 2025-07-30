#ifndef __MSG_RA_HUN_QI_FAN_LI_HPP__
#define __MSG_RA_HUN_QI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//////////////////////////////////////// �������� ///////////////////////////////////////////////////
	//!<3000 �����������
	enum RA_HUN_QI_FAN_LI_REQ_TYPE						//��Ӧ 3000Э��� opera_type
	{
		RA_HUN_QI_FAN_LI_REQ_TYPE_INFO = 0,					// ��ȡ���Ϣ
		RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY = 1,				// �齱
		RA_HUN_QI_FAN_LI_REQ_TYPE_LOTTERY_COUNT_REWARD = 2,	// ��ȡ�齱��������, param1-seq
	};

	class SCRAHunQiFanLiInfo							//<! 5220	 ����-����������Ϣ
	{
	public:
		SCRAHunQiFanLiInfo();
		MessageHeader header;

		uint32_t m_last_lottery_count;	// ���ʣ��齱����
		int m_total_lottery_count;		// ����ܳ齱����
		int m_today_recharge;			// ���ճ�ֵ���
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_recharge_reward_flag;		// ��ֵ������ȡflag
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_lottery_reward_flag;		// �齱����������ȡflag
		BitMap<MAX_RA_HUN_QI_FAN_LI_BITMIP> m_cur_rotary_table_state;	// ��ǰ����״̬, �ѳ鵽���Ľ�����Ϊ1,ȫ�����ˢ��
	};


	//////////////////////////////////////// ����ҫ�� ///////////////////////////////////////////////////
	//!<3000 �����������
	enum RA_TIAN_YI_YAO_SHI_REQ_TYPE						//��Ӧ 3000Э��� opera_type
	{
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_INFO = 0,			// ��ȡ���Ϣ
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_LOTTERY = 1,		// �齱
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_TEN_LOTTERY = 2,	// ʮ����
		RA_TIAN_YI_YAO_SHI_REQ_TYPE_BUY = 3,			// ����齱������� p1-seq
	};

	class SCRATianYiYaoShiInfo				//<! 5221	 ����-����ҫ����Ϣ
	{
	public:
		SCRATianYiYaoShiInfo();
		MessageHeader header;

		uint32_t cur_log_tail;		// ��ǰ��־β��

		uint16_t m_buy_count[MAX_RA_TIAN_YI_YAO_SHI_BUY_TYPE];		// �����������
		RATianYiYaoShiLOG m_lottery_log[MAX_RA_TIAN_YI_YAO_SHI_LOG];	// �齱��־
	};



}

#pragma pack(pop)

#endif  
