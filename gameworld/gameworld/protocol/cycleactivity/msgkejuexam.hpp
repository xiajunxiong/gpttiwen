#ifndef __MSG_KE_JU_EXAM_HPP__
#define __MSG_KE_JU_EXAM_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"

#pragma pack(push,4) 

namespace Protocol
{
	class CSKeJuExamCSReq		//!<5135 �ƾٿ��� �ͻ�������
	{
	public:
		CSKeJuExamCSReq();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_SEND_INFO = 0,		// ���������Ϣ
			OP_TYPE_CREATE_NPC = 1,		// ����NPCλ��
			OP_TYPE_START_ANSWERING = 2,// ��ʼ����
			OP_TYPE_ANSWER = 3,			// ����  p1:ѡ��Ĵ�
			OP_TYPE_USED_CARD = 4,		// ʹ��������߿�
			OP_TYPE_PRAISE = 5,			// ��ҵ���,  p1:�������˵�uid
			OP_TYPE_LAST_BATTLE = 6,	// ����ս��	
			OP_TYPE_APPLY_FRIEND = 7,	// ��������, p1-��������uid
			OP_TYPE_SEEK_HELP = 8,		// ����������
			OP_TYPE_SUGGEST = 9,		// �Ǵ����߽���, p1-����Ĵ�
		};

		int op_type;
		int param_1;
	};

	class SCKeJuExamNPCInfo		//!<5136 �ƾٿ��� NPC����Ŀ����
	{
	public:
		SCKeJuExamNPCInfo();
		MessageHeader header;

		int npc_seq;
		int question_id;
	};

	class SCKeJuExamTimeOutInfo		//!<5137 �ƾٿ��� ���ⳬʱʱ���
	{
	public:
		SCKeJuExamTimeOutInfo();
		MessageHeader header;

		int answerer_role_id;
		unsigned int answer_timeout_timestamp;
	};

	class SCKeJuExamAnswerResultInfo		//!<5138 �ƾٿ��� ��������Ϣ
	{
	public:
		SCKeJuExamAnswerResultInfo();
		MessageHeader header;

		int	answer;			// ��, 0--��ʱδ����
		int result;			// �Ƿ���ȷ,0-����
	};

	class SCKeJuExamBaseInfo				//!<5139 �ƾٿ��Ի�����Ϣ ��¼ʱ�·�
	{
	public:
		SCKeJuExamBaseInfo();
		MessageHeader header;

		char start_task_flag;			//�����Ƿ��������
		char is_receive_last_reward;	//�Ƿ�����ȡ���ս���
		short used_question_count;		//�Ѵ�������
		int cur_satisfaction;			//��ǰ�����
		int cur_card;					//��Ƭ
	};


	struct KeJuExamPraise
	{
		KeJuExamPraise() { this->Reset(); }

		void Reset()
		{
			uid = 0;
			cur_card = 0;
			word_id = 0;
		}

		int uid;			// ���uid
		int cur_card;		// ��ǰ��Ƭ,0-û��
		int word_id;		// ����id
	};

	class SCKeJuExamPraiseInfo		//!<5140 �ƾٿ��� ���޺͵�����Ϣ�·�
	{
	public:
		SCKeJuExamPraiseInfo();
		MessageHeader header;

		KeJuExamPraise user_info[MAX_TEAM_MEMBER_NUM];
	};


	class SCKeJuExamNotifyInfo				//!<5141 �ƾٿ��� Ư��
	{
	public:
		SCKeJuExamNotifyInfo();
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_PRAISE = 1,	//����
			OP_TYPE_FRIEND = 2,	//��������
			OP_TYPE_USECARD = 3,//ʹ�õ��߿�
		};

		int op_type;
		GameName name;
		int param;		// type=3ʱ��ʾ���߿�id
	};

	class SCKeJuExamUseCardInfo			//!<5142 �ƾٿ��� ʹ�õ���
	{
	public:
		SCKeJuExamUseCardInfo();
		MessageHeader header;

		int card_type;
		int uid;
		int param;		//param, ���⿨-����Ŀid,�Ŵ�-�����id
	};


	struct KeJuExamLastResult
	{
		KeJuExamLastResult()
		{}

		void Reset()
		{
			uid = 0;
			cur_satisfaction = 0;
			true_question_num = 0;
			answered_question_num = 0;
			praise_times = 0;
		}

		int uid;
		int cur_satisfaction;			// �����
		short true_question_num;		// �ش���ȷ��Ŀ���� 
		short answered_question_num;	// �ѻش���Ŀ����
		int praise_times;				// ����������
	};

	class SCKeJuExamLastResultInfo			//!<5143 �ƾٿ��� ���ս���
	{
	public:
		SCKeJuExamLastResultInfo();
		MessageHeader header;

		KeJuExamLastResult last_result[MAX_TEAM_MEMBER_NUM];
	};

};


#pragma pack(pop) 

#endif