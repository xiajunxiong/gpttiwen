#ifndef __MSG_QING_YUAN_SHENG_HUI_HPP__
#define __MSG_QING_YUAN_SHENG_HUI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commonsavedef.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	enum QING_YUAN_SHENG_HUI_REQ_TYPE
	{
		QING_YUAN_SHENG_HUI_REQ_TYPE_YU_YUE,  //ԤԼ
		QING_YUAN_SHENG_HUI_REQ_TYPE_ENTER,  //���볡��
		QING_YUAN_SHENG_HUI_REQ_TYPE_GATHER,	//�ɼ�
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_OPEN,	//�򿪴������	����һ������˫���յ�4654��4655
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_ANSWER,	//���� p1 ѡ��
		QING_YUAN_SHENG_HUI_REQ_TYPE_QUESTION_CLOSE,	//�رմ������	����һ������˫���յ�4654
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_OPEN,	//��ƥ�����
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI_CLOSE,	//�ر�ƥ�����
		QING_YUAN_SHENG_HUI_REQ_TYPE_PI_PEI,		//��ʼƥ�� P1 �Ƿ�ƥ��
		QING_YUAN_SHENG_HUI_REQ_TYPE_QI_YUAN,	//��Ը
		QING_YUAN_SHENG_HUI_REQ_TYPE_BATTLE_REWARD,	//��ȡս������ p1 ����
		QING_YUAN_SHENG_HUI_REQ_TYPE_INFO,	// ������Ϣ ,4658�·�		
	};

	class CSQingYuanShengHuiReq			//!< 4650
	{
	public:
		CSQingYuanShengHuiReq();
		MessageHeader header;

		int req_type;
		int p1;
		int p2;
	};

	class SCQingYuanShengHuiSceneInfo	//!< 4651
	{
	public:
		SCQingYuanShengHuiSceneInfo();
		MessageHeader header;

		int phase;		//��ǰ�׶�
		int phase_start;
		int phase_end;
		int role_num;
	};

	class SCQingYuanShengHuiRoleInfo			//!< 4652
	{
	public:
		SCQingYuanShengHuiRoleInfo();
		MessageHeader header;

		unsigned int can_gather_next_time;		//�´��ܲɼ�ʱ��
		short battle_times;		//ս������
		short is_get_qi_yuan_rewards;		//�Ƿ��Ѿ���Ը��
		int battle_reward_flag;		//��ȡս��������־
	};

	class SCQingYuanShengHuiQuestionResult			//!< 4653 ������
	{
	public:
		SCQingYuanShengHuiQuestionResult();
		MessageHeader header;

		int is_same;				//˫��ѡ���Ƿ���ͬ
		int question_id;
	};

	class SCQingYuanShengHuiQuestionRoute			//!< 4654
	{
	public:
		SCQingYuanShengHuiQuestionRoute();
		MessageHeader header;

		int is_open;
	};

	class SCQingYuanShengHuiQuestionInfo		//!< 4655	��Ŀ��Ϣ	
	{
	public:
		SCQingYuanShengHuiQuestionInfo();
		MessageHeader header;
			
		int question_id;			//��Ŀid	
		unsigned int end_time;		//��ǰ��Ŀʱ��
		int quest_num;
	};


	class SCQingYuanShengHuiOpponentInfo 		//!< 4656 ƥ�䵽������Ϣ		
	{
	public:
		SCQingYuanShengHuiOpponentInfo();
		MessageHeader header;

		unsigned int start_battle_time;		//��ʼս��ʱ��,ƥ�䵽�˺��ÿ���ʱ���
		MsgRoleBaseInfo role_info[2];
	};


	class SCQingYuanShengHuiMatchOpRoute			//!< 4657
	{
	public:
		SCQingYuanShengHuiMatchOpRoute();
		MessageHeader header;

		short is_open;
		short is_match;
		unsigned int begin_time;
	};

	class SCQingYuanShengHuiInfo			//!< 4658
	{
	public:
		SCQingYuanShengHuiInfo();
		MessageHeader header;
		
		int is_yu_yue;		//�Ƿ��Ѿ�ԤԼ��
		int role_num;
	};

	class SCQingYuanShengHuiQiYuanNotice 		//!< 4659
	{
	public:
		SCQingYuanShengHuiQiYuanNotice();
		MessageHeader header;

		int x;
		int y;
		unsigned int begin_time;
	};



	class CSQingYuanShengHuiReq2			//!< 2520
	{
	public:
		CSQingYuanShengHuiReq2();
		MessageHeader header;

		int length;
		char text[2048];		//��Ը����
	};
}

#pragma pack(pop)

#endif