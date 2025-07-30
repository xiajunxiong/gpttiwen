#ifndef __MSG_SCHOOL_EXERCISE_H__
#define __MSG_SCHOOL_EXERCISE_H__

#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push)
#pragma pack(4)

namespace Protocol
{
	class SCSchoolExerciseInfo		//!< 3590 У��������Ϣ
	{
	public:
		SCSchoolExerciseInfo();
		MessageHeader header;

		long long clear_flag;		//!< ͨ�ر�� ���Զ�������ʽ��ʾ����XλΪ1��seqΪX�Ĺؿ���ͨ�أ�
		int has_open_ui_ever;		//!< �Ƿ�������UI  0:��  1:��
	};

	class CSSchoolExerciseChallenge	//!< 3591 У��������ս�ؿ�
	{
	public:
		CSSchoolExerciseChallenge();
		MessageHeader header;

		int seq;					//!< ��ս�Ĺؿ���� 
	};

	class SCSchoolExerciseChallengeSucc //!< 3592 ��ս�ɹ�֪ͨ
	{
	public:
		SCSchoolExerciseChallengeSucc();
		MessageHeader header;

		int seq;					//!< ��ս�Ĺؿ���� 
		int has_reward;				//!< �Ƿ��н��� 0:�� 1:��
	};

	class CSSchoolExerciseFirstOpenUI //!< 3593 �״δ�UIʱ����
	{
	public:
		CSSchoolExerciseFirstOpenUI();
		MessageHeader header;
	};
}

#pragma pack(pop)

#endif