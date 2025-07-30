#ifndef __MSG_LANTERN_RIDDIE_HPP__
#define __MSG_LANTERN_RIDDIE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"
#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"

#pragma pack(push,4) 

namespace Protocol
{

	class LanternRiddieCSReq		//!<4670 ɽ���ƻ� �ͻ�������
	{
	public:
		LanternRiddieCSReq();
		MessageHeader header;

		int op_type;
		int param_1;
	};


	class SALanternRiddieNPCInfo		//!<4671 ɽ���ƻ� NPC����Ŀ����
	{
	public:
		SALanternRiddieNPCInfo();
		MessageHeader header;

		int npc_seq;
		int question_id;
	};

	class SALanternRiddieTimeOutInfo		//!<4672 ɽ���ƻ� ���ⳬʱʱ���
	{
	public:
		SALanternRiddieTimeOutInfo();
		MessageHeader header;

		unsigned int answer_timeout_timestamp;
	};

	class SALanternRiddieAnswerResultInfo		//!<4673 ɽ���ƻ� ��������Ϣ
	{
	public:
		SALanternRiddieAnswerResultInfo();
		MessageHeader header;

		int answerer_id;					// ������id,   0--��ʱ��������		 ///����ĳЩ���ɿ�����ԭ��,�����޸���������		answerer_id = �����ߴ�
		int	answerer_answer;				// �����ߴ�, 0--��ʱ��������		 ///����ĺ���,���ǵ�Ч��ʵ�������෴��,����	answerer_answer = ������id
		int cur_satisfaction;				// ��ǰ�����
		int result;		//�Ƿ���ȷ,0-����
	};

	class SALanternRiddieBaseInfo				//!<4674 ɽ���ƻ� ��¼ʱ�·�
	{
	public:
		SALanternRiddieBaseInfo();
		MessageHeader header;

		char start_task_flag;			//�����Ƿ��������
		char is_receive_last_reward;	//�Ƿ�����ȡ���ս���
		short used_question_count;		//�Ѵ�������
		int cur_satisfaction;			//��ǰ�����
		int cur_card;					//��Ƭ
	};

	struct LanternRiddiePraise
	{
		LanternRiddiePraise() { this->Reset(); }

		void Reset()
		{
			uid = 0;
			is_praise = 0;
			cur_card = 0;
			word_id = 0;
		}

		int uid;			// ���uid
		int is_praise;		// �Ƿ�����
		int cur_card;		// ��ǰ��Ƭ,0-û��
		int word_id;		// ����id
	};

	class SCLanternRiddiePraiseInfo		//!<4675 ɽ���ƻ� ���޺͵�����Ϣ�·�
	{
	public:
		SCLanternRiddiePraiseInfo();
		MessageHeader header;

		LanternRiddiePraise user_info[MAX_TEAM_MEMBER_NUM];
	};

	class SCLanternRiddieNotifyInfo				//!<4676 ɽ���ƻ� Ư��
	{
	public:
		SCLanternRiddieNotifyInfo();
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

	class SCLanternRiddieUseCardInfo			//!<4677 ɽ���ƻ� ʹ�õ���
	{
	public:
		SCLanternRiddieUseCardInfo();
		MessageHeader header;

		int card_type;
		int uid;
		int param;		//param, ���⿨-����Ŀid,�Ŵ�-�����id
	};

	struct LanternRiddieLastResult
	{
		LanternRiddieLastResult()
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

	class SCLanternRiddieLastResultInfo			//!<4678 ɽ���ƻ� ���ս���
	{
	public:
		SCLanternRiddieLastResultInfo();
		MessageHeader header;

		LanternRiddieLastResult last_result[MAX_TEAM_MEMBER_NUM];
	};

}

#pragma pack(pop)

#endif
