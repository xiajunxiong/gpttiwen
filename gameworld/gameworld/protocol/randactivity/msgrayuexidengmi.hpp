#ifndef __MSG_RA_YUE_XI_DENG_MI_HPP__
#define __MSG_RA_YUE_XI_DENG_MI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	//////////////////////////////////////// ��Ϧ���� ///////////////////////////////////////////////////
	static const int RA_YXDM_MAX_USER_ANSWER_LENGTH = 16;	// ��Ϧ���մ������

	class CSRAYueXiDengMiRequest	//!<5320 ����-��Ϧ���� ����
	{
	public:
		enum RA_YUE_XI_DENG_MI_REQ_TYPE
		{
			RA_YUE_XI_DENG_MI_REQ_TYPE_INFO = 0,		// ������Ϣ
			RA_YUE_XI_DENG_MI_REQ_TYPE_START = 1,		// ����ʼ����
			RA_YUE_XI_DENG_MI_REQ_TYPE_ANSWER = 2,		// ���� user_answer:��Ҵ�
		};
	public:
		CSRAYueXiDengMiRequest();
		MessageHeader header;

		int op_type;
		char user_answer[RA_YXDM_MAX_USER_ANSWER_LENGTH];
	};

	class SCRAYueXiDengMiInfo		//!<5321 ����-��Ϧ���� ���Ϣ�·�
	{
	public:
		SCRAYueXiDengMiInfo();
		MessageHeader header;

		int cur_question_id;	// ��ǰ��Ŀid
		int today_answer_count;	// �����Ѵ�������
	};

	class SCRAYueXiDengMiAnswerResult		//!<5323 ����-��Ϧ���� �������·�
	{
	public:
		SCRAYueXiDengMiAnswerResult();
		MessageHeader header;

		int is_true;		// �ش��Ƿ���ȷ, 0-����, 1-��ȷ
	};


	//////////////////////////////////////// ��Ϧ�۳� ///////////////////////////////////////////////////
	//!<3000 �����������
	enum RA_YUE_XI_LEI_CHONG_REQ_TYPE					//��Ӧ 3000Э��� opera_type
	{
		RA_YUE_XI_LEI_CHONG_REQ_TYPE_INFO = 0,				// ������Ϣ
		RA_YUE_XI_LEI_CHONG_REQ_TYPE_RECEIVE_REWARD = 1,	// ������ȡ����, param1-seq
	};

	class SCRAYueXiLeiChongInfo							//<! 5322 ����-��Ϧ�۳� ���Ϣ�·�
	{
	public:
		SCRAYueXiLeiChongInfo();
		MessageHeader header;

		int m_total_chongzhi;		// ��ڼ��ܳ�ֵ���
		BitMap<MAX_RA_YUE_XI_LEI_CHONG_BITMAP> m_receive_reward_flag;		// ������ȡflag
	};



}

#pragma pack(pop)

#endif  
