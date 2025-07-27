#ifndef __BATTLE_SCRIPT_DEF_HPP__
#define __BATTLE_SCRIPT_DEF_HPP__

#include "servercommon/struct/battle/battle_def.hpp"

////////////////////////////////////////////////////// BATTLE_MOVE_SCRIPT ��Ϊ�ű�//////////////////////////////////////////////
enum MOVE_SCRIPT_CHECK_POINT
{
	MOVE_SCRIPT_CHECK_POINT_MIN = 0,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_START = MOVE_SCRIPT_CHECK_POINT_MIN,
	MOVE_SCRIPT_CHECK_POINT_ON_CHARACTER_DIE = 1,
	MOVE_SCRIPT_CHECK_POINT_ON_ONE_SIDE_ALL_DEAD = 2,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_END = 3,
	MOVE_SCRIPT_CHECK_POINT_ON_ROUND_PREPARE = 4, // �غ�׼���׶�
	//MOVE_SCRIPT_CHECK_POINT_ON_CHARACTER_START_MOVE = 5, // ��ɫ��ʼ�ж�ʱ
	MOVE_SCRIPT_CHECK_POINT_MAX,
};

enum SCRIPT_EXCUTE_TYPE
{
	SCRIPT_EXCUTE_TYPE_MIN = 0,
	SCRIPT_EXCUTE_TYPE_NORMAL = SCRIPT_EXCUTE_TYPE_MIN, // 0 �����߼�
	SCRIPT_EXCUTE_TYPE_DISABLE_ON_CONDITION_FALSE,		// 1 ������������ʱͣ�ýű�
	SCRIPT_EXCUTE_TYPE_COUNTER_SCRIPT_EXCUTE,			// 2 ������������ʱ���÷��ű��¼�
	SCRIPT_EXCUTE_TYPE_MAX,
};

enum MOVE_SCRIPT_COMPARE_TYPE
{
	MOVE_SCRIPT_COMPARE_TYPE_GREATER,
	MOVE_SCRIPT_COMPARE_TYPE_LESS,
	MOVE_SCRIPT_COMPARE_TYPE_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_NOT_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_GTEATER_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_LESS_EQUAL,
	MOVE_SCRIPT_COMPARE_TYPE_DEVIDED_BY,     // ����
	MOVE_SCRIPT_COMPARE_TYPE_NOT_DEVIDED_BY, // ������ 
	MOVE_SCRIPT_COMPARE_TYPE_MAX,
};

enum MOVE_SCRIPT_CHECK_COND_TYPE	// �ҷ������﷽    �з�����ҷ� 
{
	MOVE_SCRIPT_COND_TYPE_ROUND_NUM,			// �Ƚ���ʱ�Ļغ���  compare_num���غ���
	MOVE_SCRIPT_COND_TYPE_MONSTER_ID,			// �Ƚ��ҷ�ĳ��λ�Ĺ���ID  compare_num: ����ID   param1:���
	MOVE_SCRIPT_COND_TYPE_MONSTER_HP_PERCENT,	// �Ƚ��ҷ�ĳ��λ(��ָ������)��Ѫ���ٷֱ�  compare_num: �ٷֱ�  param1:���  param2:monster_id(��ѡ)
	MOVE_SCRIPT_COND_TYPE_ENEMY_SPECIAL_STATE,	// �Ƚϵз�ĳ��λ�Ƿ�����ĳ״̬  compare_num:1��0���ǻ��)   param1:���  param2��״ֵ̬
	MOVE_SCRIPT_COND_TYPE_MONSTER_NUM,			// �Ƚ��ҷ�������  compare_num:������  param1:ָ������ID����ѡ��
	MOVE_SCRIPT_COND_TYPE_MONSTER_SPECIAL_STATE,// �Ƚ��ҷ�ĳ��λ�Ƿ�����ĳ״̬  compare_num:1��0 (�ǻ��)   param1:���  param2: ״ֵ̬
	MOVE_SCRIPT_COND_TYPE_HP_PERCENT,			// �Ƚ�ĳ��λ��Ѫ���ٷֱ�  compare_num: �ٷֱ�  param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_COND_TYPE_ENEMY_NUM,			// �Ƚϵз���λ��  compare_num: ��λ��  param1:�Ƿ�������  param2:�Ƿ������
	MOVE_SCRIPT_COND_TYPE_RAND_NUM,				// �Ƚ�һ�������  compare_num: �����  param1:���������X (0, X]
	MOVE_SCRIPT_COND_TYPE_MP_PERCENT,			// �Ƚ�ĳ��λ�������ٷֱ�  compare_num: �ٷֱ�  param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARA_IMPRISON_STATE,	// �Ƚ�ĳ��λ�Ƿ������ڽ���״̬ compare_num:1��0  param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARA_ABNORMAL_STATE, // �Ƚ�ĳ��λ�Ƿ��������쳣״̬ compare_num:1��0  param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_COND_TYPE_CHARACTER_NUM_ON_ROW,	// �Ƚ�ĳ�ŵĵ�λ�� compare_num: ��λ��  param1:side[0:��ҷ�,1:���﷽] param2:[0:����,1:ǰ��] 
	MOVE_SCRIPT_COND_TYPE_SIDE_HAS_IMPRISON_STATE,	// �Ƚ�ĳ���Ƿ���ڴ��ڽ���״̬�ĵ�λ compare_num:��1  param1:side[0:��ҷ�,1:���﷽] 
	MOVE_SCRIPT_COND_TYPE_SIDE_HAS_ABNORMAL_STATE,	// �Ƚ�ĳ���Ƿ���ڴ����쳣״̬�ĵ�λ compare_num:��1  param1:side[0:��ҷ�,1:���﷽] 
	MOVE_SCRIPT_COND_TYPE_CHARACTER_ELEMENT_10,		// �Ƚ���ҷ�ĳ��λ�Ƿ�10ˮ��10���10���10��  compare_num:��1  param1:���[0,10)  param2:Ԫ������[1ˮ 2�� 3�� 4��]
	MOVE_SCRIPT_COND_TYPE_MONSTER_DIE_ORDER,		// �Ƚϵ�X�������Ĺ��﷽��� compare_num:���[0,10) param1����X�������ģ�����X��ȡֵ��Χ[0,10)
	MOVE_SCRIPT_COND_TYPE_MONSTER_HP,		// �Ƚ��ҷ�����Ѫ����ֵ compare_num:Ѫ�� param1:���[0,10)  param2:monster_id����ѡ��  ps:�˽ű�ֻ���ҹ��﷽   
	MOVE_SCRIPT_COND_TYPE_IN_SHIELD,		// �Ƚϵ�λ�Ƿ������������£�ӵ������Ϊ37��buff) compare_num:��1  param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_COND_TYPE_PET_NUM,			// �Ƚ���ҷ����ϳ�������  compare_num:������
	MOVE_SCRIPT_COND_TYPE_MAX,
};

enum MOVE_SCRIPT_EVENT
{
	MOVE_SCRIPT_EVENT_INVALID = 0,
	MOVE_SCRIPT_EVENT_SUMMON = 1,						// �ٻ�  param1:�ٻ�seq  param2:���[0,10)��-1��ָ�������ΪԴ��-1��ʾû��Դ��
	MOVE_SCRIPT_EVENT_SET_MONSTER_MOVE_PRIORITY = 2,	// ���ù�����Ϊ���ȶ�  param1:���[0,10)  param2:monster_id(��ѡ)  param3:��Ϊ���  param4:�µ����ȶ�
	MOVE_SCRIPT_EVENT_SIDE_HEAL = 3,					// ĳһ��ȫ���Ѫ  param1:side[0,1]  param2:0Ϊ�ٷֱȣ�1Ϊ��ֵ  param3:�ظ���ֵ
	MOVE_SCRIPT_EVENT_SIDE_DAMAGE = 4,					// ĳһ��ȫ���Ѫ  param1:side[0,1]  param2:0Ϊ�ٷֱȣ�1Ϊ��ֵ  param3:�ظ���ֵ
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_MOVE = 5,		// ���ù�����һ�ε���Ϊ  param1:���[0,10)  param2:monster_id(��ѡ) param3:��Ϊ����  param4:��Ϊ����p1,  param5:��Ϊ����p2   ��PS��p1,p2�ĺ����������Ϊ��p1 p2һ����
	MOVE_SCRIPT_EVENT_ADD_BUFF_TO_CHARACTER = 6,		// ��buff��ĳ����ɫ  param1:side[0,1] param2:���[0,10) param3:buff_id  param4:buff_level
	MOVE_SCRIPT_EVENT_SET_MONSTER_ASSIGN_MOVE = 7,		// ���ù���ָ����Ϊ��Ŀ�� param1:���[0,10) param2:monster_id(��ѡ) param3:ָ����Ϊ���[0, move_num) param4:ָ��Ŀ������  param5:ָ��Ŀ��
	MOVE_SCRIPT_EVENT_REINFORCE = 8,					// ��Ԯ  param1:�µĹ�����ID
	MOVE_SCRIPT_EVENT_KILL_ALL_MONSTER = 9,				// ɱ�����й���
	MOVE_SCRIPT_EVENT_MONSTER_COMBINE_ATTACK = 10,		// ����ϻ�(�ù��Ｏ�й���ĳ����㣩 param1:side[0,1] param2:���[0,10) param3:monster_id (Ϊ0����ȫ��������ָ�����
	MOVE_SCRIPT_EVENT_CANCEL_MONSTER_ASSIGN_MOVE = 11,	// ȡ������ָ����Ϊ param1:���[0,10) param2:monster_id(��ѡ) 
	MOVE_SCRIPT_EVENT_LOWEST_HP_SUFFER_DAMAGE = 12,		// Ѫ����͵ĵ�λ�ܵ��˺� param1:side[0,1] param2:Ѫ������[0:�ٷֱ� 1:��ֵ] param3:�ܵ��˺��� param4:�˺�������[0:��������ٷֱ� 1:��ǰ�����ٷֱ� 2:��ֵ]
	MOVE_SCRIPT_EVENT_FIND_TARGET_SUFFER_DAMAGE = 13,	// ʹĿ���ܵ��˺� param1:Ŀ������seq  param2:�ܵ��˺���  param3:�˺�������[0:��������ٷֱ� 1:��ǰ�����ٷֱ� 2:��ֵ] 
	MOVE_SCRIPT_EVENT_SWITCH_POSITION = 14,				// ʹĿ��ǰ��λ�õ��� param1:side[0,1] param2:���[0,10)
	MOVE_SCRIPT_EVENT_FIND_TARGET_ADD_BUFF = 15,		// ��buff��Ŀ�� param1:Ŀ������seq  param2:buff_id  param3:buff_level param4:���ʰٷֱ�[0,100]  ��0��100���Ǳ��У�Ϊ�˼��ݾɵĽű����ã�
	MOVE_SCRIPT_EVENT_FIND_TARGET_COMBINE_ATTACK = 16,	// �������Ŀ�겢�ϻ� param1:Ŀ������seq  param2:monster_id ��Ϊ0��ȫ����ﶼ��ϻ�������ָ��������кϻ���
	MOVE_SCRIPT_EVENT_DISABLE_SCRIPT = 17,				// �ǰ�ű�ʧЧ param1:����
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_KILL = 18,		// ϵͳ����Ŀ�겢����  param1:Ŀ������seq  param2:�Ƿ����(1:�� 0:��)
	MOVE_SCRIPT_EVENT_ENABLE_OR_DISABLE_SCRIPT = 19,	// ��ű���Ч����Ч param1:�ű�ID  param2:[0:��Ч 1:��Ч]
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_HP = 20,		// ����Ŀ�겢�޸�Ѫ�� param1:Ŀ������seq param2:�޸ĵ�Ѫ����������Ѫ��������Ѫ��
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_MP = 21,		// ����Ŀ�겢�޸����� param1:Ŀ������seq param2:�޸ĵ���������������������������
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_ATTACK = 22,	// ����Ŀ�겢�ͷŹ�����Ϊ���չ����ܣ� param1:Ŀ������seq  
	MOVE_SCRIPT_EVENT_FIND_TARGET_ADD_HP_BY_ATK = 23,	// ����Ŀ�겢��Ѫ��Ѫ��Ϊ��������ֵ param1:Ŀ������seq 
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_BIANSHEN = 24,	// ����Ŀ�겢����Ŀ������ǹ���) param1:Ŀ������seq   param2������ɹ���ID
	MOVE_SCRIPT_EVENT_SUMMON_BY_ROLE_NUM = 25,			// ������������ٻ�  param1:�ٻ�seq  param2:Դ���[0,10)��-1��ָ�������ΪԴ��-1��ʾû��Դ��param3:�����ٻ�����
	MOVE_SCRIPT_EVENT_SET_NPC_HELPER_NEXT_MOVE = 26,	// ������սNPC��һ�ε���Ϊ  param1:���[0,10)  param2:��սnpc_id(��ѡ) param3:��Ϊ����  param4:��Ϊ����p1,  param5:��Ϊ����p2  
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_SECOND_MOVE = 27,// ���ù�����һ�εĶ�����Ϊ param1:���[0,10)  param2:monster_id(��ѡ) param3:��Ϊ����  param4:��Ϊ����p1,  param5:��Ϊ����p2   ��PS��p1,p2�ĺ����������Ϊ��p1 p2һ����
	MOVE_SCRIPT_EVENT_SUMMON_NPC_HELPER = 28,			// �ٻ���սNPC param1:�ٻ�����սNPC_ID param2:���[0,10)
	MOVE_SCRIPT_EVENT_CLIENT_SPECIAL_NEED = 29,			// �ͻ����������� param1:����(1:�Ի��� 2:�о�ͷ 3:ָ��)  param2:��Ӧ����(��Ϊ�Ի�������Ի�ID����Ϊ�о�ͷ�����ͷ��ЧID����Ϊָ��������ָ��ID��
	MOVE_SCRIPT_EVENT_CLIENT_CUSTOM_EVENT = 30,			// �ͻ����Զ���ű����� param1 param2 param3 param4 param5 param6 ��ԭ�����ظ��ͻ��ˣ�����������ʲô�·��ľ���ʲô
	MOVE_SCRIPT_EVENT_SIDE_HEAL_BY_FAKE_SKILL = 31,		// ĳһ��ȫ���Ѫ(�ͻ��˱���Ϊĳ����ɫ�ͷż��ܣ�  param1:side[0,1]  param2:0Ϊ�ٷֱȣ�1Ϊ��ֵ  param3:�ظ���ֵ param4:�ͷż����߸��[0, 10) param5:�ͷż���ID param6:�ͷż��ܵȼ�
	MOVE_SCRIPT_EVENT_SIDE_DAMAGE_BY_FAKE_SKILL = 32,	// ĳһ��ȫ���Ѫ(�ͻ��˱���Ϊĳ�������ܻ��п�Ѫ��param1:side[0,1]  param2:0Ϊ�ٷֱȣ�1Ϊ��ֵ  param3:��Ѫ��ֵ param4:�ͷż����߸��[0, 20) �� -1 (-1��û���ͷ��ߣ� param5:�ͷż���ID param6:�ͷż��ܵȼ� 
	MOVE_SCRIPT_EVENT_SET_MONSTER_NEXT_SECOND_MOVE_2 = 33, // ���ù�����һ�εĶ�����Ϊ������Ϊ������ѡ�� param1:���[0,10)  param2:monster_id(��ѡ)  param3:��Ϊ��� param4:ָ��Ŀ������  param5:ָ��Ŀ��
	MOVE_SCRIPT_EVENT_FIND_TARGET_MODIFY_HP_PERCENT = 34,  // ����Ŀ�겢���ٷֱ��޸�Ѫ�� param1:Ŀ������seq  param2:�޸ĵ�Ѫ���ٷֱȣ�������Ѫ��������Ѫ��
	MOVE_SCRIPT_EVENT_SUMMON_HEART_DEVIL = 35,			// ǰ���ٻ���ħ������ param1 �ٻ���ħseq  param2 �����buff_id  param3 �����buff_lv
	MOVE_SCRIPT_EVENT_FIND_TARGET_SUFFER_DAMAGE_FAKE_SKILL = 36,	// ʹĿ���ܵ��˺�(����Ϊ�ٵļ���) param1:Ŀ������seq  param2:�ܵ��˺���  param3:�˺�������[0:��������ٷֱ� 1:��ǰ�����ٷֱ� 2:��ֵ]  param4:�ͷż��ܸ��[0,20)��-1������   param5:�����ͷż���ID  param6:�����ͷż��ܵȼ�
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_ADD_SPECIAL_EFFECT = 37,	// ���ҹ���Ŀ�겢��Ŀ�������Ч param1: Ŀ������seq param2:��ЧID 
	MOVE_SCRIPT_EVENT_FIND_TARGET_AND_BIANSHEN_2 = 38,	// ����Ŀ�겢����,������ǰѪ����Ŀ������ǹ���) param1:Ŀ������seq   param2������ɹ���ID
	MOVE_SCRIPT_EVENT_FIND_TARGET_MONSTER_REMOVE_SPECIAL_EFFECT = 39, // ���ҹ���Ŀ�겢�Ƴ�Ŀ��������Ч param1: Ŀ������seq param2:��ЧID 

	MOVE_SCRIPT_EVENT_MAX,
};

enum MOVE_SCRIPT_TARGET //!< �ű�Ŀ������ \see�����side_type --> [0:������1:�ط���2:˫��]��
{
	MOVE_SCRIPT_TARGET_INVALID = 0,
	MOVE_SCRIPT_TARGET_ALL_CHARACTER = 1,						//!< ���н�ɫ
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER = 2,				//!< ���һ����ɫ
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_STATE = 3,				//!< ���д���ĳ״̬�Ľ�ɫ param1: ״̬�� \see BATTLE_SPECIAL_STATE
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_ABNORMAL_DEBUFF = 4,	//!< ���д����쳣״̬�µĽ�ɫ 
	MOVE_SCRIPT_TARGET_ALL_CHARACTER_IN_IMPRISON_DEBUFF = 5,	//!< ���д��ڽ���״̬�µĽ�ɫ
	MOVE_SCRIPT_TARGET_ONE_CHARACTER = 6,						//!< ����ָ��һ����ɫ param1:side[0:���� 1:�ط�] param2:grid_idx[0,10) 
	MOVE_SCRIPT_TARGET_ONE_ROW = 7,								//!< ָ��һ�Ž�ɫ param1:side[0:��ҷ� 1:���﷽] param2:row[0:���� 1:ǰ��]
	MOVE_SCRIPT_TARGET_ONE_SIDE = 8,							//!< ָ��һ�߽�ɫ param1:side[0:��ҷ� 1:���﷽]
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_ABNORMAL_DEBUFF = 9,	//!< ���һ�������쳣״̬�µĽ�ɫ
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_IMPRISON_DEBUFF = 10,//!< ���һ�����ڽ���״̬�µĽ�ɫ
	MOVE_SCRIPT_TARGET_RANDOM_ONE_CHARACTER_IN_ONE_ROW = 11,	//!< ���ĳ��һ����ɫ param1:side[0:��ҷ� 1:���﷽] param2:row[0:���� 1:ǰ��]
	MOVE_SCRIPT_TARGET_LOWEAST_DEFENSE_CHARACTER = 12,			//!< ������ͷ����Ľ�ɫ param1:side[0:��ҷ� 1:���﷽]
	MOVE_SCRIPT_TARGET_LOWEAST_DAMAGE_LAST_ROUND_CHARACTER = 13,//!< �����ϻغ��������˺��Ľ�ɫ param1:side[0:��ҷ� 1:���﷽]

	MOVE_SCRIPT_TARGET_MAX
};

struct BattleCheckCondition
{
	BattleCheckCondition() : condition_type(0), compare_type(0), compare_num(0), cond_param1(0), cond_param2(0) {}

	short condition_type;
	short compare_type;
	int compare_num;

	int cond_param1;
	int cond_param2;
};

struct BattleScriptEvent
{
	BattleScriptEvent() : move_script_idx(0), script_event_type(0), param1(0), param2(0), param3(0), param4(0), param5(0), param6(0) {}

	short move_script_idx;
	short script_event_type;
	int param1;
	int param2;
	int param3;
	int param4;
	int param5;
	int param6;
};

struct BattleMoveScript // ��Ϊ�ű�
{
	BattleMoveScript() : script_id(0), is_disable(false), check_point_type(0), condition_excute_type(0), check_condition_num(0), script_event_num(0), counter_script_event_num(0) {}

	bool IsDisable() { return is_disable; }

	int script_id;
	bool is_disable;
	short check_point_type;
	short condition_excute_type;

	short check_condition_num;
	BattleCheckCondition check_condition_list[BATTLE_SCRIPT_CONDITION_NUM];

	short script_event_num;
	BattleScriptEvent script_event_list[BATTLE_SCRIPT_EVENT_NUM];

	short counter_script_event_num;
	BattleScriptEvent counter_script_event_list[BATTLE_SCRIPT_EVENT_NUM];
};

#endif