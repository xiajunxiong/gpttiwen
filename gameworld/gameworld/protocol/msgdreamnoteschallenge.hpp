#ifndef __MSG_DREAM_NOTES_CHALLENGE_HPP__
#define __MSG_DREAM_NOTES_CHALLENGE_HPP__

#pragma pack(push) 
#pragma pack(4)

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/servercommon.h"

namespace Protocol
{
	class CSDreamNotesChallengeOp					//!< 3570 梦渊笔录请求
	{
	public:
		CSDreamNotesChallengeOp();
		MessageHeader header;

		enum DREAM_NOTES_CHALLENGE_OP_TPYE
		{
			DREAM_NOTES_CHALLENGE_OP_TPYE_START_CHALLENGE = 0,			// 开始挑战	param1 目标关卡
			DREAM_NOTES_CHALLENGE_OP_TPYE_FAST_PASS = 1,				// 扫荡		param1 目标关卡
			DREAM_NOTES_CHALLENGE_OP_TPYE_GO_ON_CHALLENGE = 2,			// 继续挑战  param1 目标关卡
			DREAM_NOTES_CHALLENGE_OP_TYPE_BUY_TIMES = 3,				// 购买次数	param1 购买次数 
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
		char is_need_talk;		//0:不需要对话  1：需要对话
		char obj_type;
		ObjID obj_id;
		short pos_x;
		short pos_y;
	};

	class SCDreamNotesChallengeInfo					//!< 3571 梦渊笔录角色信息
	{
	public:
		SCDreamNotesChallengeInfo();
		MessageHeader header;

		short pass_level;					// 当前通关过的最高关卡
		short remaining_times;				// 今天剩余次数
		short buy_times;					// 今天扫荡购买次数
		short use_item_add_times;			// 使用道具增加的次数
	};

	struct SCDreamNotesChallengeSceneInfo			//!< 3572 梦渊笔录场景信息
	{
	public:
		SCDreamNotesChallengeSceneInfo();
		MessageHeader header;

		static const int MAX_SCENE_OBJ_NUM = 4;

		short level;			// 当前副本关卡
		char kill_boss_flag;	// 0:未击杀  1：已击杀
		char count;
		SpecialLogicDreamNotesChallengeObjInfo obj_list[MAX_SCENE_OBJ_NUM];
	};


	class SCDreamNotesChallengeNotice			// 3573 梦渊笔录通知
	{
	public:
		SCDreamNotesChallengeNotice();
		MessageHeader header;

		int npc_id;
		int is_auto_navigate;			//是否自动寻路  0:否 1:自动寻路
	};
}

#pragma pack(pop)

#endif