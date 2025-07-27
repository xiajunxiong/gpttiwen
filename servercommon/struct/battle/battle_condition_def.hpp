#ifndef __BATTLE_CONDITION_DEF_HPP__
#define __BATTLE_CONDITION_DEF_HPP__

enum BATTLE_CONDITION_TYPE
{
	BATTLE_COND_WIN = 0,					// ͨ�� 
	BATTLE_COND_WIN_ROUND_BELOW_X,			// X�غϽ���ս�� param1 �غ���
	BATTLE_COND_DEAD_CHARACTER_BELOW_X,		// ������ɫС��X�� param1 Ϊ0ʱ��ÿ���غ϶�Ҫ�ж� Ϊ1ʱ��ս������ʱ�ж�  param2����������
	BATTLE_COND_CHARACTER_HP_PERCENT,		// ս������ʱ�����ٷֱȴ���X% param1 Ϊ0ʱ��ȫ�� Ϊ1ʱ��������  param2:����ֵ�ٷֱ�
	BATTLE_COND_KILL_MONSTER,				// X�غ��ڻ�ɱ���� param1 �غ���  param2 ���  param3 ����ID
	BATTLE_COND_WIN_WITH_PARTNER,			// Я�����ͨ�� param1 ���ID
	BATTLE_COND_WIN_WITH_PARTNER_AND_PET,	// Я����������ͨ�� param1 ���ID param2 ����ID
	BATTLE_COND_WIN_WITH_PET_QUALITY,		// Я��X��YƷ�ʳ���ͨ��  param1 ������� param2 Ʒ��
	BATTLE_COND_WIN_WITH_PARTNER_PROF,		// Я��X��Yְҵ���ͨ��  param1 ������ param2 ְҵ
	BATTLE_COND_WIN_WITH_PET_LEVEL,			// Я������X��Yǿ���ȼ�����ͨ��  param1 ������� param2 ǿ���ȼ�
	BATTLE_COND_WIN_WITH_PARTNER_QUALITY,	// Я��X��YƷ�ʻ��ͨ��	 param1 �������  param2 Ʒ��
	BATTLE_COND_WIN_WITH_PARTNER_SEX,		// Я��X��Y�Ա���ͨ��  param1 �������  param2 �Ա�
	BATTLE_COND_WIN_WITH_PETID_STR_LEVEL,	// Я��+Xǿ���ȼ���Y����IDͨ��  param1 ����id  param2ǿ���ȼ�
	BATTLE_COND_WIN_WITH_NUM_LEVEL_QUA,		// Я��X�� + Yǿ���ȼ���zƷ�ʳ��� param1 �������  param2ǿ���ȼ�  param3 Ʒ��

	BATTLE_COND_TYPE_NUM
};

struct BattleCond
{
	BattleCond() : cond_type(-1), param1(0), param2(0), param3(0) {}

	int cond_type;
	int param1;
	int param2;
	int param3;
};

/*
struct CondData_WinWithPartner
{

};

union BattleCondData
{

};*/

#endif