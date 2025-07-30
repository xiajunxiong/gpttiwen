#ifndef __MSG_COURSE_OF_GROWTH_H__
#define __MSG_COURSE_OF_GROWTH_H__

#include "gamedef.h"
#include "servercommon/courseofgrowthdef.h"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push, 4)

namespace Protocol
{
	class CSCOGOperatorRequest
	{
	public:
		CSCOGOperatorRequest();			//!< 1780 - �ɳ�֮· ��������
		MessageHeader header;

		enum COGOperatorType
		{
			COG_OP_RECEIVES_AWARD = 0,		// ��ȡ������
			COG_OP_FIGHT_THE_BOSS = 1,		// ������BOSS
			COG_OP_GET_BOSS_AWARD = 2,		// ��ȡBOSS����
			COG_OP_FETCH_LEVEL_REWARD = 3,	// ��ȡ���̵ȼ�����
			COG_OP_SERVER_COURSE_INFO = 4,	// �����ȡ����ȫ��������
		};
		
		UInt16 op_type;			//!< ����	|  0  |  1  |  2  |   3   |
		UInt16 param;			//!< ����	| seq |  /  |  /  | level |
	};

	class SCCOGAllInformations
	{
	public:
		SCCOGAllInformations();			//!< 1781 - �ɳ�֮· ������Ϣ
		MessageHeader header;

		UInt16 cur_prog;				//!< ��ǰ�׼�
		UInt16 cur_star;				//!< ��ǰ�Ǽ�
		UInt16 cur_reward_prog;			//!< ��ǰ�����׼�
		UInt16 cur_reward_star;			//!< ��ǰ�����Ǽ�

		UInt32 task_prog_list[GROWTH_TASK_NUM_PER_STAR];	//!< �������
		BitMap<GROWTH_TASK_NUM_PER_STAR> award_fetch_flag;	//!< ������ȡ״̬���(�������б�)
	};

	class SCCOGSingleTaskProgs
	{
	public:
		SCCOGSingleTaskProgs();			//!< 1782 - �ɳ�֮· ��������֪ͨ
		MessageHeader header;

		int seq;				//!< �α� - [0, 8]
		UInt32 prog_num;		//!< ������ֵ
	};

	class SCCOGRewardFetchFlag
	{
	public:
		SCCOGRewardFetchFlag();			//!< 1783 - �ɳ�֮· ������ȡ֪ͨ
		MessageHeader header;

		UInt16 seq;				//!< �α� - [0, 8]
		UInt16 flag;			//!< ��ȡ״̬
	};

	class SCCOGStarAwardStatus
	{
	public:
		SCCOGStarAwardStatus();			//!< 1784 - �ɳ�֮· ��ǰ�Ǽ������׶�
		MessageHeader header;

		UInt16 cur_reward_prog;			//!< ��ǰ�Ǽ������׶�
		UInt16 cur_reward_star;			//!< ��ǰ�Ǽ������Ǽ�
	};

	class SCCOGSingleSeqNotify
	{
	public:
		SCCOGSingleSeqNotify();			//!< 1785 - ��ɵ�������ʾ
		MessageHeader header;

		int notify_seq;				//!< ��ǰ�׶ε�������α� [0, 8]
	};

	class SCRoleCourseAllInfo		//!< 1786 �ɳ�֮· ����������Ϣ�·�
	{
	public:
		SCRoleCourseAllInfo();
		MessageHeader header;

		int course_level;						//���̵ȼ�
		int course_cur_fraction;				//���̵�
		BitMap<MAX_COURSE_ENTRY_LIMIT_NUM> finish_record_flag;					// ������Ŀ��ɼ�¼
		BitMap<MAX_COURSE_LEVEL_NUM> level_reward_fetch_flag;					// ���̵ȼ�������ȡ��¼

		int count;						
		UInt32 course_prog_list[MAX_COURSE_ENTRY_LIMIT_NUM];					// ������Ŀ����
	};

	class SCRoleCourseSingleInfo		//!< 1787  �ɳ�֮· ���̵�����Ϣ�·�
	{
	public:
		SCRoleCourseSingleInfo();
		MessageHeader header;
		
		int seq;				
		int course_prog;
	};

	class SCRoleCourseNoticeInfo			//!< 1788 �ɳ�֮· ������Ϣ֪ͨ�·�
	{
	public:
		SCRoleCourseNoticeInfo();
		MessageHeader header;

		enum ROLE_COURSE_NOTICE_TYPE
		{
			ROLE_COURSE_NOTICE_TYPE_FINISH_ENTRY = 0,		// ֪ͨ��Ŀ��ɱ�ʶ  param1 - seq  param2 - 0:δ��� 1:�����
			ROLE_COURSE_NOTICE_TYPE_LEVEL_FECHT = 1,		// ֪ͨ�ȼ���ȡ��ʶ  param1 - level param2 - 0:δ��ȡ 1:����ȡ

			ROLE_COURSE_NOTICE_TYPE_MAX
		};

		int notice_type;
		int course_level;						//���̵ȼ�
		int course_cur_fraction;				//���̵�
		int param1;
		int param2;
	};

	class SCServerCourseAllInfo			//!< 1789	�ɳ�֮· ���������Ϣ�·�	
	{
	public:
		SCServerCourseAllInfo();
		MessageHeader header;

		int server_role_count;											//�÷��������
		int count;		
		int course_finish_count_list[MAX_COURSE_ENTRY_LIMIT_NUM];		//ÿһ���������
	};

	class SCServerCourseSingleInfo			//!< 2027	�ɳ�֮· ������ɵ�����Ϣ�·�
	{
	public:
		SCServerCourseSingleInfo();
		MessageHeader header;

		int server_role_count;											//�÷��������
		int seq;														
		int course_finish_count;										
	};
}

#pragma pack(pop)
#endif