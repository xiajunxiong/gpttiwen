#ifndef __MSG_MARRIAGE_HPP__
#define __MSG_MARRIAGE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/marrydef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class CSMarriageReq		//!<2551
	{
	public:
		enum MARRIAGE_REQ_TYPE
		{
			MARRIAGE_REQ_TYPE_INFO,						//!< ������Ϣ
			MARRIAGE_REQ_TYPE_PROPOSE,					//!< �������	p1 ������� p2 Ŀ��uid
			MARRIAGE_REQ_TYPE_PROPOSE_REPLY,				//!< �ظ����	p1  �Ƿ����  p2 Ŀ��uid
			MARRIAGE_REQ_TYPE_DIVORCE,					//!< �������
			MARRIAGE_REQ_TYPE_DIVORCE_REPLY,				//!< �ظ����	p1 �Ƿ�����������
			MARRIAGE_REQ_TYPE_DIVORCE_FORCE,				//!< ����ǿ�����
			MARRIAGE_REQ_TYPE_REGRET_DIVORCE_FORCE,		//!< ȡ��ǿ�����
			MARRIAGE_REQ_TYPE_TASK_REQ_ROUTE,				//!< ��Ե�ճ�����ת��
			MARRIAGE_REQ_TYPE_REQ_TASK,					//!< �ճ�����ʼ  p1��1	����ƴͼ ��2	������Ϭ��3	�����ѹأ�4	����ƴͼ��5	�����ĵ�
			MARRIAGE_REQ_TYPE_AUTO_FB,					//!< ɨ��
			MARRIAGE_REQ_TYPE_ANSWSER_DUET,				//!< ˫�˻ش� p1 ��ѡ��Ĵ�
			MARRIAGE_REQ_TYPE_ANSWSER_SINGLE,				//!< ���˻ش� p1 ��ѡ��Ĵ�
			MARRIAGE_REQ_TYPE_QUERY_OTHER,				//!< ��ѯ���˽����Ϣ p1 role_id
			MARRIAGE_REQ_TYPE_CANCEL_PROPOSE,			//!< ȡ���������	p1  Ŀ��uid
			MARRIAGE_REQ_TYPE_DUET_PUZZLE,				//!< p1 �Ƿ������
			MARRIAGE_REQ_TYPE_NEXT_QUESTION_DUET,				//!< ˫��������һ��
			MARRIAGE_REQ_TYPE_NEXT_QUESTION_SINGLE,				//!< ����������һ��
		};

		CSMarriageReq();
		MessageHeader		header;

		short				req_type;
		short				param1;
		int				param2;
		int				param3;
	};

	class SCMarryReqRoute	//!<2552			�����Ϣ
	{
	public:
		SCMarryReqRoute();
		MessageHeader		header;

		int					marry_type;			//!< �������
		int					req_uid;				//!< �����uid
		GameName			req_name;				//!< ���������
	};

	class SCMarryInfo	//!<2553			��Ϣ
	{
	public:
		SCMarryInfo();
		MessageHeader		header;

		int lover_uid;								//!< ����UID
		GameName lover_name;						//!< ��������
		int avatar_type;
		unsigned int last_marry_time;					//!< ���ʱ��
		unsigned int last_divorce_time;					//!< ���ʱ��	
		unsigned int apply_divorce_time;				//!< ����ǿ�����ʱ��

		int puzzle_play_times;			//����ƴͼ��Ĵ���
		int question_play_times;			//������Ĵ���
		int max_pass_fb_wave;				//�����
		short curr_wave;			//��ǰ����
		short is_auto_fb;				//�Ƿ�ɨ��
	};

	class SCDivorceReqRoute		//!<2554			�Է��������
	{
	public:
		SCDivorceReqRoute();
		MessageHeader		header;
	};

	class SCMarryProposeNotice	//!<2555	
	{
	public:
		SCMarryProposeNotice();
		MessageHeader		header;

		enum OP_RESULT
		{
			OP_RESULT_PROPOSE_SUCC,					//!<֪ͨ����������ѷ��͵��Է�
			OP_RESULT_REFUSE,							//!<�Է��ܾ�
		};

		int result;
	};

	struct MarriageSeekingInfo
	{
		GameName			gamename;
		GuildName			guildname;
		int					user_id;
		char				reserve_ch;
		signed char				sex;
		char				prof;
		char				is_online;
		short				level;
		short				reserve_sh;
		unsigned int		timestamp;
		long long		m_avator_timestamp;
		int				avator_frame_id;
		MarriageSeekingNotice marriage_seeking_notice;
	};


	class CSMarriageSeekingOpera			//!<2556
	{
	public:
		CSMarriageSeekingOpera();
		MessageHeader header;

		short type;
		short reserve_sh;
		MarriageSeekingNotice marriage_seeking_notice;
	};

	class SCMarriageSeekingInfo						//!<2557
	{
	public:
		SCMarriageSeekingInfo();							
		MessageHeader header;

		int count;
		MarriageSeekingInfo	marriage_seeking_list[MARRIAGE_SEEKING_MAX_COUNT];
	};

	class SCMarriageTaskRoute						//!<2558  ת��
	{
	public:
		SCMarriageTaskRoute();					//��2251 req=MARRIAGE_REQ_TYPE_TASK_REQ_ROUTE���Է��ͻ��յ����
		MessageHeader header;

		int role_id;
		int param1;
		int param2;
		unsigned int param3;
	};

	class SCMarriageTaskQuestionInfo						//!<2559		��Ŀ��Ϣ
	{
	public:
		SCMarriageTaskQuestionInfo();
		MessageHeader header;

		char question_type;		//�Ƿ�˫��
		char question_num;		//�ڼ���
		short question_id;  //����id
		unsigned int end_time;//����ʱ��
	};

	class SCMarriageTaskQuestionResult						//!<2560  ���
	{
	public:
		SCMarriageTaskQuestionResult();
		MessageHeader header;

		char question_type;		//�Ƿ�˫��
		char is_right;		//�Ƿ���ȷ
		short question_id;  //����id
	};

	class SCMarriageQueryRet								//!<2561  ���
	{
	public:
		SCMarriageQueryRet();
		MessageHeader header;

		int role_id;				
		int lover_id;
		short prof;
		short sex;
	};

	class SCMarriageDuetPuzzleInfo								//!<2562  ���
	{
	public:
		SCMarriageDuetPuzzleInfo();
		MessageHeader header;

		enum INFO_TYPE
		{
			DUET_PUZZLE_START,		//��ʼ		p1 ����ʱ��
			DUET_PUZZLE_END,			//����		p1 0:û�������   1:���������
			DUET_PUZZLE_LOGOUT,		//������һ������,�ͻᷢ
			DUET_PUZZLE_CANCEL,		//ȡ��
		};


		int info_type;
		unsigned int param1;
	};

	class SCMarriageDuetTaskLogoutNotice								//!<2563 ����֪ͨ
	{
	public:
		SCMarriageDuetTaskLogoutNotice();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif // __MSG_MARRIAGE_HPP__

