#ifndef __MSG_DREAM_NOTES_CHALLENGE_HPP__
#define __MSG_DREAM_NOTES_CHALLENGE_HPP__

#pragma pack(push) 
#pragma pack(4)

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

namespace Protocol
{
	class CSDreamNotesChallengeOp					//!< 3570 ��Ԩ��¼����
	{
	public:
		CSDreamNotesChallengeOp();
		MessageHeader header;

		enum DREAM_NOTES_CHALLENGE_OP_TPYE
		{
			DREAM_NOTES_CHALLENGE_OP_TPYE_START_CHALLENGE = 0,			// ��ʼ��ս	param1 Ŀ��ؿ�
			DREAM_NOTES_CHALLENGE_OP_TPYE_FAST_PASS = 1,				// ɨ��		param1 Ŀ��ؿ�
			DREAM_NOTES_CHALLENGE_OP_TPYE_GO_ON_CHALLENGE = 2,			// ������ս  param1 Ŀ��ؿ�
			DREAM_NOTES_CHALLENGE_OP_TYPE_BUY_TIMES = 3,				// �������	param1 ������� 
		};

		short op_type;
		short param1;
	};


	enum DREAM_NOTES_CHALLENGE_OBJ_TYPE
	{
		DREAM_NOTES_CHALLENGE_OBJ_TYPE_BOSS = 0,
		DREAM_NOTES_CHALLENGE_OBJ_TYPE_NPC = 1,
	};

	struct SpecialLogicDreamNotesChallengeObjInfo
	{
		SpecialLogicDreamNotesChallengeObjInfo() : npc_id(0), is_need_talk(0), obj_type(0), obj_id(0), pos_x(0), pos_y(0)
		{}

		int npc_id;
		char is_need_talk;		//0:����Ҫ�Ի�  1����Ҫ�Ի�
		char obj_type;
		ObjID obj_id;
		short pos_x;
		short pos_y;
	};

	class SCDreamNotesChallengeInfo					//!< 3571 ��Ԩ��¼��ɫ��Ϣ
	{
	public:
		SCDreamNotesChallengeInfo();
		MessageHeader header;

		short pass_level;					// ��ǰͨ�ع�����߹ؿ�
		short remaining_times;				// ����ʣ�����
		short buy_times;					// ����ɨ���������
		short use_item_add_times;			// ʹ�õ������ӵĴ���
	};

	struct SCDreamNotesChallengeSceneInfo			//!< 3572 ��Ԩ��¼������Ϣ
	{
	public:
		SCDreamNotesChallengeSceneInfo();
		MessageHeader header;

		static const int MAX_SCENE_OBJ_NUM = 4;

		short level;			// ��ǰ�����ؿ�
		char kill_boss_flag;	// 0:δ��ɱ  1���ѻ�ɱ
		char count;
		SpecialLogicDreamNotesChallengeObjInfo obj_list[MAX_SCENE_OBJ_NUM];
	};


	class SCDreamNotesChallengeNotice			// 3573 ��Ԩ��¼֪ͨ
	{
	public:
		SCDreamNotesChallengeNotice();
		MessageHeader header;

		int npc_id;
		int is_auto_navigate;			//�Ƿ��Զ�Ѱ·  0:�� 1:�Զ�Ѱ·
	};
}

#pragma pack(pop)

#endif