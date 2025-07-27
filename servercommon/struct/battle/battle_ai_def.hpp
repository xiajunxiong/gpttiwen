#ifndef __BATTLE_AI_DEF_HPP__
#define __BATTLE_AI_DEF_HPP__

#include "servercommon/servercommon.h"

////////////////////////////////////////////////////// BATTLE_AI ///////////////////////////////////////////////////
enum BATTLE_AI_TYPE
{
	BATTLE_AI_TYPE_INVALID = 0,
	BATTLE_AI_TYPE_TEST = 1,
	BATTLE_AI_TYPE_GOBLIN_PET = 2,					// ���񱩵ĸ粼�ֳ���
	BATTLE_AI_TYPE_RESURRECT_WITCH = 3,				// �Ḵ����ҵ�Ů��
	BATTLE_AI_TYPE_TARGET_LOWEST_HP_PERCENT = 4,	// ֻ��Ѫ�����ٵ�Ŀ��
	BATTLE_AI_TYPE_PARTNER = 5,						// ���
	BATTLE_AI_TYPE_AUTO_CHARACTER = 6,				// �Զ��Ľ�ɫ�����ǡ��������Ӱ�ӡ�����Ӱ�ӣ�
	BATTLE_AI_TYPE_NPC_HELPER = 7,					// ��սNPC
	BATTLE_AI_TYPE_JIANG_ZI_YA = 8,					// ���������AI
	BATTLE_AI_TYPE_TARGET_HIGH_MP_PERCENT = 9,		// ֻ��������ߵ�Ŀ��(����˳��:�ȳ���,û�г������������,Ĭ�ϵз�) 
	BATTLE_AI_TYPE_DA_SI_MING = 10,					// ��˾�����AI
	BATTLE_AI_TYPE_MAX,
};

static const int BATTLE_AI_MOVE_MAX_NUM = 20;

struct BattleAIMove
{
	BattleAIMove() : priority(0), move_type(0), move_cd(0), move_cd_in_config(0), move_param1(0), move_param2(0), move_param3(0), no_reduce_cd_flag(0), is_attack_move(false) {}

	bool IsValid() { return 0 != move_type; }

	short priority;
	short move_type;
	short move_cd;
	short move_cd_in_config;

	int move_param1;
	int move_param2;
	int move_param3;

	short no_reduce_cd_flag;   // ֵΪ1ʱ�� ���غϲ���CD

	bool is_attack_move;	// �Ƿ�һ��������Ϊ
};

enum ASSIGN_TARGET_TYPE // ָ��Ŀ������
{
	ASSIGN_TARGET_TYPE_INVALID = 0,
	ASSIGN_TARGET_TYPE_ALL = 1,					// ȫ�����һ��Ŀ��
	ASSIGN_TARGET_TYPE_ALL_ENEMY = 2,			// ȫ��������һ��Ŀ��
	ASSIGN_TARGET_TYPE_ONE_ENEMY = 3,			// ָ������һ��Ŀ��
	ASSIGN_TARGET_TYPE_ALL_FRIEND = 4,			// ȫ���Ѿ����һ��Ŀ��
	ASSIGN_TARGET_TYPE_ONE_FRIEND = 5,			// ָ���Ѿ�һ��Ŀ��
	ASSIGN_TARGET_TYPE_ALL_FRIEND_BUT_SELF = 6, // ȫ���Ѿ����һ��Ŀ�굫�Լ�����
	ASSIGN_TARGET_TYPE_SELF = 7,				// ָ���Լ�ΪĿ��
	ASSIGN_TARGET_TYPE_FRIEND_MONSTER = 8,		// ָ���Ѿ�����
	ASSIGN_TARGET_TYPE_NOT_SAME_ID_WITH_SELF = 9,//���һ�����Լ�ID��һ����Ŀ��
	ASSIGN_TARGET_TYPE_ENEMY_IN_CHAOS = 10,		// ���һ�����ڻ���״̬�ĵ���
	ASSIGN_TARGET_TYPE_MAX,
};

enum BATTLE_STRATEGY
{
	BATTLE_STRATEGY_INVALID = -1,		//!< -1 ��
	BATTLE_STRATEGY_OTHER = 0,			//!< 0 ����
	BATTLE_STRATEGY_FOCUS_FIRE = 1,		//!< 1 ����
	BATTLE_STRATEGY_HEAL = 2,			//!< 2 ����
	BATTLE_STRATEGY_REALIVE = 3,		//!< 3 ����
	BATTLE_STRATEGY_CONTROL = 4,		//!< 4 ����
	BATTLE_STRATEGY_BREAK_DEFENCE = 5,	//!< 5 �Ʒ�
	BATTLE_STRATEGY_BURN_MANA = 6,		//!< 6 ����
	BATTLE_STRATEGY_DEFENSE = 7,		//!< 7 ����
	BATTLE_STRATEGY_PURIFY = 8,			//!< 8 ����
	BATTLE_STRATEGY_MAX = 9,
};

extern int STRATEGY_PRIORITY_ARRAY[BATTLE_STRATEGY_MAX];

#endif
