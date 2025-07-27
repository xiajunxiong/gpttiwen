#ifndef __BATTLE_SPECIAL_EFFECT_DEF_HPP__
#define __BATTLE_SPECIAL_EFFECT_DEF_HPP__

////////////////////////////////////////// ��Ч /////////////////////////////////////////////
static const int BATTLE_SPECIAL_EFFECT_TYPE_NUM = 1000; // ��Ӧ��Ч��.xlsx ��Ч�б�sheet��sp_type�ֶ�


enum SPECIAL_EFFECT_DAMAGE_CONTROL
{
	SPE_DAMAGE_INVALID = 0,
	SPE_DAMAGE_ELEMENT_MAGIC_UP = 1,			//!< 1. ��Ԫ�ط����˺�up	p1 ��������(���壬ǿ������ǿ��)  p2 ��߰ٷֱ�
	SPE_DAMAGE_SKILL_UP_1 = 2,					//!< 2. �����˺�up(������id)    p1 ����id(0:������)  p2 ���ǧ�ֱ� P3:�Ƿ���Ҫ�����쳣״̬(0:�� 1:��)(����+�쳣)
	SPE_DAMAGE_SKILL_UP_2 = 3,					//!< 3. �����˺�up(����������)  p1 �������� p2 ���ǧ�ֱ�
	SPE_DAMAGE_PHYSICAL_SKILL_IGNORE_DEF = 4,	//!< 4. �����ܺ����������  p1 ���԰ٷֱ� P2:����ID(0:���ʾ���м��ܶ���)
	SPE_DAMAGE_MAGIC_SKILL_IGNORE_DEF = 5,		//!< 5. �������ܺ��Է�������  p1 ���԰ٷֱ� P2:0(ħ��) 1(����(����)) 2(�ظ�)
	SPE_DAMAGE_RACE_UP = 6,						//!< 6. �������˺�����  p1 ����   p2 �����ٷֱ�
	SPE_DAMAGE_RACE_DOWN = 7,					//!< 7. �ܵ������˺����� p1 ����  p2 ���ٰٷֱ�
	SPE_DAMAGE_ELEMENT_MAGIC_DOWN = 8,			//!< 8. �ܵ�Ԫ�ط����˺����� p1 Ԫ��  p2 ���ٰٷֱ�
	SPE_DAMAGE_DOWN = 9,						//!< 9. �����˺�����  p1 �˺����� \see BATTLE_INJURE_TYPE  p2 �˺�����ٷֱ�
	SPE_DAMAGE_COUNTER_ATTACK_UP = 10,			//!< 10.�����˺����� p1 ��������(0:��Ȼ���� 1:���ܷ��� 2:����0��1�� p2 ��߰ٷֱ� 
	SPE_DAMAGE_MAGIC_FLOAT = 11,				//!< 11.���������˺����� p1 �����ٷֱȵ�λ  p2 �����ٷֱȸ�λ
	SPE_DAMAGE_FIRE_MAGIC = 12,					//!< 12.��Ԫ�ط����˺�UP  p1:��߰ٷֱ�
	SPE_DAMAGE_WATER_MAGIC = 13,				//!< 13.ˮԪ�ط����˺�UP  p1:��߰ٷֱ�
	SPE_DAMAGE_WIND_MAGIC = 14,					//!< 14.��Ԫ�ط����˺�UP  p1:��߰ٷֱ�
	SPE_DAMAGE_EARTH_MAGIC = 15,				//!< 15.��Ԫ�ط����˺�UP  p1:��߰ٷֱ�
	SPE_DAMAGE_SKILL_ATTACH_ELEMENT = 16,		//!< 16.���ܸ���Ԫ������  p1:����Ԫ��[1:ˮ 2:�� 3:�� 4:�� 5:����]  p2:��߰ٷֱ�
	SPE_DAMAGE_IMMUNE = 17,						//!< 17.�����˺� p1 �����˺�����  \see BATTLE_INJURE_TYPE
	SPE_DAMAGE_DOWN_PHYSIC = 18,				//!< 18.�����˺����� p1:�˺�����ٷֱ�
	SPE_DAMAGE_DOWN_MAGIC = 19,					//!< 19.�����˺����� p1:�˺�����ٷֱ�
	SPE_DAMAGE_DOWN_IN_PATRIFY_STATE = 20,		//!< 20.ʯ��״̬���˺����� p1 �˺����� \see BATTLE_INJURE_TYPE  p2 �˺�����ٷֱ�
	SPE_DAMAGE_SKILL_CRITICAL_INC = 21,			//!< 21.���ܱ���ʱ,X%�������� p1:��߰ٷֱ� p2:����ID (0:������) P3:X(�ٷֱ�, 0:��ʾ����)
	SPE_DAMAGE_DOWN_BY_LEVEL = 22,				//!< 22.�����˺����ȼ����� p1:X    �����˺����ٵȼ�*X����ֵ��Ⱥ��Ч��ֻ��33%)
	SPE_DAMAGE_UP_BY_LEVEL = 23,				//!< 23.�����˺����ȼ����� p1:X    �����˺����ӵȼ�*X����ֵ��Ⱥ��Ч��ֻ��33%)
	SPE_DAMAGE_UP_BY_ENEMY_HP_PERCENT = 24,		//!< 24.�з���������X%ʱ���˺�����Y%  p1:X  p2:Y
	SPE_DAMAGE_UP_TO_REAL_MINE_MONSTER = 25,	//!< 25.������Ұ�ⰵ���˺��ӳ� p1:�ӳɱ�����ʮ��ֱȣ�����ר�ã�
	SPE_DAMAGE_DOWN_FROM_MONSTER = 26,			//!< 26.�ܵ������˺����� p1:���������ʮ��ֱȣ�����ר�ã�
	SPE_DAMAGE_UP_FIXED = 27,					//!< 27.������ħ���˺��˺���������̶�ֵ P1:�˺�ֵ  
	SPE_DAMAGE_PHYSICAL_UP_FIXED = 28,			//!< 28.�����˺���������̶�ֵ P1:�˺�ֵ
	SPE_DAMAGE_MAGIC_UP_FIXED = 29,				//!< 29.ħ���˺���������̶�ֵ P1:�˺�ֵ
	SPE_DAMAGE_PHYSICAL_IGNORE_DEF_IFXED = 30,	//!< 30.����Ŀ������̶�ֵ P1:���ӷ���ֵ
	SPE_DAMAGE_INJURE_DOWN_FIXED = 31,			//!< 31.�˺�������ٹ̶�ֵ P1:�����˺����(���ٺ�����-1) 
	SPE_DAMAGE_LOWER_HP_HIGHER = 32,			//!< 32.Ѫ������X%ʱ,�˺�����Y% p1:X P2:Y P3:(0:�����˺� 1��ħ���˺� 2��������ħ���˺�)
	SPE_DAMAGE_UP_BY_TARGET_BUFF = 33,			//!< 33.��Ŀ������˺�ʱ,��X%�������(P1)�˺�,�Դ�������BUFFĿ���������Y% P1:����˺��ٷֱ� P2:(0:�����˺� 1��ħ���˺� 2��������ħ���˺�)  P3:X P4:Y (��߸���X+Y)
	SPE_DAMAGE_UP_BY_TAGET_SPC_STATUS = 34,		//!< 34.�Դ��ڿ���״̬��Ŀ������X%�˺�  P1:��߰ٷֱ�(X) P2:(0:�����˺� 1��ħ���˺� 2��������ħ���˺�)
	SPE_DAMAGE_UP_BY_RATE = 35,					//!< 35.��������˺� P1:���ʰٷֱ�  P2:�˺���߰ٷֱ�
	SPE_DAMAGE_DOWN_BY_HP_DROP_1 = 36,			//!< 36.Ѫ��ÿ����X%, �˺��ͽ���Y%   P1:X (0, 1000)  P2:Y (0, 1000)
	SPE_DAMAGE_UP_PERCENT = 37,					//!< 37.�����˺����������� p1:����ǧ�ֱ� P2:�����Ƿ���Ҫ�����쳣״̬(0:�� 1:��)(����+�쳣)
	SPE_DAMAGE_INC_FINALY_DAMAGE_ADD_BUFF = 38,	//!< 38.X%������������˺�Y%,����Ŀ�����BUFF P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�) P3:buff_id P4:buff_level
	SPE_DAMAGE_UP_AGAINST_STEALTH = 39,			//!< 39.������Ŀ���˺����X%  p1:��߰ٷֱ�
	SPE_DAMAGE_SKILL_UP_BY_RATE = 40,			//!< 40.�����˺��м���up(������id)    p1 ����id  p2 ���ǧ�ֱ�  p3 ����ǧ�ֱ�
	SPE_DAMAGE_UP_ON_HIGH_HP_PERCENT = 41,		//!< 41.Ѫ������X%ʱ���˺�����Y%  p1:X(�ٷֱ�) p2:Y(�ٷֱ�)
	SPE_DAMAGE_UP_ON_STEALTH_OR_ACTION_UNDER_STEALTH = 42, //!< 42.��������״̬�����������ж���ʼʱ��������״̬���������˺�����ǧ�ֱ� P1: �˺�����ǧ�ֱ� 
	SPE_DAMAGE_UP_BY_JOB = 43,					//!< 43.����ְҵ�����˺�  p1:ְҵID  p2:����ǧ�ֱ�   ְҵIDȡֵ[ȭʦ100 ����200 ��ͷ300 ����400 ����500 ��ʦ600 ҽʦ700 ��ʦ800]
	SPE_DAMAGE_INC_BY_HP_DROP_PERCENT = 44,		//!< 44.Ѫ��ÿ����X%�����������˺�Y%  p1:X(ǧ�ֱȣ� p2:Y(ǧ�ֱȣ�
	SPE_DAMAGE_DOWN_BY_JOB = 45,				//!< 45.����ְҵ�����˺� P1:ְҵID P2:����ǧ�ֱ� ְҵIDȡֵ[ȭʦ100 ����200 ��ͷ300 ����400 ����500 ��ʦ600 ҽʦ700 ��ʦ800]
	SPE_DAMAGE_UP_BY_BOTH_SIDES_HP = 46,		//!< 46.�Է���ǰ������������,���˺�����X% P1:X(ǧ�ֱ�) P2:���غ���(0:���ܻغ�����)  ��PVP
	SPE_DAMAGE_INC_UP = 47,						//!< 47.%X������߶Եз��˺� P1:(BATTLE_DAMAGE_EFFECT_TYPE_MAX) P2:����ǧ�ֱ� P3:X(�ٷֱ�,0:��ʾ����)
	SPE_DAMAGE_INC_UP_BY_SKILL_EFFECT_TYPE = 48,//!< 48.����65����Ч�����Ͳ���3ϵ�� P1:����ǧ�ֱ�
	SPE_DAMAGE_INC_UP_BY_TARGET_NUM = 49,		//!< 49.���ܷ�̯��λԽ��,�˺����Խ�� P1:X(ǧ�ֱ�) P2:����˺�����Y%(ǧ�ֱ�) P3:BATTLE_DAMAGE_EFFECT_TYPE_MAX ��ʽ:X*�ֵ���λ����
	SPE_DAMAGE_INC_UP_BY_BOSS = 50,				//!< 50.��߶�BOSS��λ���˺�X% P1:X(ǧ�ֱ�) (��PVE)
	SPE_DAMAGE_REAL_DAMAGE_UP_BY_TYPE = 51,		//!< 51.���ĳ�������˺�ʱ,�����ʵ�˺� P1.BATTLE_DAMAGE_EFFECT_TYPE_MAX P2:����˺�ֵ P3:Ⱥ����ߵ��˺�ֵ
	SPE_DAMAGE_CENTER_TARGET_UP = 52,			//!< 52.����Ŀ�굥λ�˺���� P1:X(ǧ�ֱ�)
	SPE_DAMAGE_INC_UP_BY_ATTR_TYPE = 53,		//!< 53.���Ա�Ŀ���/�����X%�˺� P1:X(ǧ�ֱ�) P2:(0:�� 1:��) P3:��������(\see BATTLE_ATTR_MAX)
	SPE_DAMAGE_SPECIAL_REAL_DAMAGE_UP = 54,		//!< 54.�����ʵ�˺� P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�) P3:Z(ǧ�ֱ�) ��ʽ:����*X + �ظ�*Y + ����*Z
	SPE_DAMAGE_DOWN_SKILL_CRITICAL = 55,		//!< 55.�ܵ�����ʱ,X%���ʼ��� p1:����(ǧ�ֱ�)  P2:X(ǧ�ֱ�, 0:��ʾ����)
	SPE_DAMAGE_REAL_DAMAGE_UP_BY_LEVEL = 56,	//!< 56.�����ʵ�˺� P1:X(ǧ�ֱ�) ��ʽ:�ȼ�*X
	SPE_DAMAGE_REAL_DAMAGE_DOWN_BY_LEVEL = 57,	//!< 57.������ʵ�˺� P1:X(ǧ�ֱ�) ��ʽ:�ȼ�*X
	SPE_DAMAGE_INC_UP_AND_DEC_UP = 58,			//!< 58.���X�˺�,����Y�˺����� P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�) P3:��λ����(\see BATTLE_CHARACTER_TYPE_MAX)  ���(����)��ʽ:�ҷ����ϸõ�λ���ʹ������*X(Y)
	SPE_DAMAGE_REAL_DAMAGE_UP_HUANSHOU_LEVEL = 59,	//!< 59.���ĳ�������˺�ʱ,�����ʵ�˺� P1.BATTLE_DAMAGE_EFFECT_TYPE_MAX P2:X(ǧ�ֱ�) ��߹�ʽ:���޵ȼ�*X
	SPE_DAMAGE_SKILL_IGNORE_DEF_HUANSHOU_LEVEL = 60,//!< 60.���ܺ������/����  P1:X(ǧ�ֱ�) P2:����ID(0:���ʾ���м��ܶ���) P3.BATTLE_DAMAGE_EFFECT_TYPE_MAX ���ӹ�ʽ:���޵ȼ�*X
	SPE_DAMAGE_INC_UP_WITH_WEAR_ELEMENT = 61,	//!< 61.���ָ��Ԫ��ˮ��ʱ,�˺����X P1:\see ELEMENT_TYPE_MAX P2:X(ǧ�ֱ�)
	SPE_DAMAGE_INC_UP_BY_TARGET_ROLE = 62,		//!< 62.��߶Խ�ɫĿ����˺� P1:X(��ֱ�)  ��߹�ʽ:X * ˫�����޵ȼ���  (����PVP)
	SPE_DAMAGE_DOWN_BY_TARGET_ROLE = 63,		//!< 63.����Ŀ����Լ���ɵ��˺� P1:X(��ֱ�) ���͹�ʽ:X * ˫�����޵ȼ���  (����PVP)

	SPE_DAMAGE_MAX,

};

enum SPECIAL_EFFECT_ATTR_CHANGE
{
	SPE_ATTR_INVALID = 0,
	SPE_ATTR_ADD_ATTR = 1,						//!< 1. ������      p1 ��������  p2 ���Ա仯ֵ
	SPE_ATTR_ADD_SKILL_LEVEL = 2,				//!< 2. �Ӽ��ܵȼ�  p1 ����id  p2 ���ܵȼ��仯ֵ
	SPE_ATTR_ADD_ATTR_BY_LEVEL = 3,				//!< 3. ������ȼ�������  p1 ��������  p2 �ȼ�ϵ��(�ٷֱȣ�P3: ���ӵ�����ֵ  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_PET_LEVEL = 4,			//!< 4. ������ȼ�������  p1 ��������  p2 �ȼ�ϵ��(�ٷֱȣ�P3: ���ӵ�����ֵ  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_ANOTHER_ATTR = 5,		//!< 5. ĳ����ֵ������һ����ֵ  p1 ������������  p2 ������������  p3 ϵ��(�ٷֱȣ�     p1���Լӵ�ֵ=p2���Ե�ֵ/p3ϵ�� 
	SPE_ATTR_ADD_ATTR_BY_PARTNER_LEVEL = 6,		//!< 6. �����ȼ������� p1 ��������  p2 �ȼ�ϵ��(�ٷֱȣ�P3: ���ӵ�����ֵ  (P2 * level + P3)
	SPE_ATTR_ADD_ATTR_BY_HUANSHOU_LEVEL = 7,	//!< 7. �����޵ȼ������� p1 ��������  p2 �ȼ�ϵ��(�ٷֱȣ�P3: ���ӵ�����ֵ  (P2 * level + P3)

	SPE_ATTR_MAX,
};

enum SPECIAL_EFFECT_TYPE
{
	SPECIAL_EFFECT_TYPE_INVALID = 0,
	SPECIAL_EFFECT_TYPE_TIMING = 1,				//!< 1.���ڴ���ʱ������Ч
	SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL = 2,		//!< 2.�����˺�����Ч
	SPECIAL_EFFECT_TYPE_ADD_ATTR = 3,			//!< 3.�����Ե���Ч
	SPECIAL_EFFECT_TYPE_SKILL_HIT = 4,			//!< 4.�������е���Ч(������)
	SPECIAL_EFFECT_TYPE_OTHER = 5,				//!< 5.��������Ч
	SPECIAL_EFFECT_TYPE_HALO = 6,				//!< 6.�⻷��Ч
	SPECIAL_EFFECT_TYPE_MAX,
};

enum SPECIAL_OTHER_EFFECT
{
	SPE_OTHER_INVALID = 0,
	SPE_OTHER_AVOID_BEING_ABSORB_HP = 1,	//!< 1.��ֹ����Ѫ
	SPE_OTHER_ADD_HP_UP = 2,				//!< 2.�ܵ�����Ч������ p1 ���Ӱٷֱ�
	SPE_OTHER_IMMUNE_BOUNCE_DAMAGE = 3,		//!< 3.���߷����˺� p1 �˺�����(0:���� 1:ħ��)
	SPE_OTHER_POISON_PROB_UP = 4,			//!< 4.�ж�������� p1 ����ǧ�ֱ�
	SPE_OTHER_PETRIFY_PROB_UP = 5,			//!< 5.ʯ��������� p1 ����ǧ�ֱ�
	SPE_OTHER_CHAOS_PROB_UP = 6,			//!< 6.���Ҹ������ p1 ����ǧ�ֱ�
	SPE_OTHER_SLEEP_PROB_UP = 7,			//!< 7.��˯������� p1 ����ǧ�ֱ�
	SPE_OTHER_DRUNK_PROB_UP = 8,			//!< 8.���������� p1 ����ǧ�ֱ�
	SPE_OTHER_GUARD_DEFENCE_UP = 9,			//!< 9.���������ӳ� p1 ���Ӱٷֱ�
	SPE_OTHER_SHIELD_DAMAGE_DOWN = 10,		//!< 10.ʥ�����˼��� p1 ���ٰٷֱ�
	SPE_OTHER_PERFORM_UPGRADED_SKILL = 11,	//!< 11.�ͷ���ǿ�漼�� p1:���ʰٷֱ�  p2:�滻ǰ����ID  p3:�滻����ID 
	SPE_OTHER_SKILL_CONSUME_LESS_MP = 12,	//!< 12.�ͷż������Ľ��� p1:���Ͱٷֱ�
	SPE_OTHER_LOWER_HP_HIGHER_DEF = 13,		//!< 13.ѪԽ�ٷ�Խ�ߣ�ֻ�ڼ����˺�ʱ��Ч�� ���ȴ���ʽ�У�
	SPE_OTHER_LOWER_HP_HIGHER_MAGIC_DEF = 14,//!< 14.ѪԽ��ħ��Խ�ߣ�ֻ�ڼ����˺�ʱ��Ч�����ȴ���ʽ�У�
	SPE_OTHER_LOWER_HP_HIGHER_DAMAGE_DEC_PHYSIC = 15,	//!< 15.ѪԽ���������Խ�� p1 ����ϵ���ٷֱ�  ��ʽ�����˰ٷֱ� =��1-Ѫ���ٷֱȣ�*����ϵ��
	SPE_OTHER_LOWER_HP_HIGHER_DAMAGE_DEC_MAGIC = 16,	//!< 16.ѪԽ��ħ������Խ�� p1 ����ϵ���ٷֱ�  ��ʽ�����˰ٷֱ� =��1-Ѫ���ٷֱȣ�*����ϵ��
	SPE_OTHER_IMMUNE_PATRIFY = 17,			//!< 17.����ʯ��
	SPE_OTHER_IMMUNE_CHAOS = 18,			//!< 18.���߻���
	SPE_OTHER_USE_ITEM_ADD_HP_UP = 19,		//!< 19.�Ժ�ҩ�ظ�HPЧ������  p1 ���Ӱٷֱ�
	SPE_OTHER_USE_ITEM_ADD_MP_UP = 20,		//!< 20.����ҩ�ظ�MPЧ������  p1 ���Ӱٷֱ�
	SPE_OTHER_CATCH_BABY_PROB_UP = 21,		//!< 21.ץ������������ p1:���Ӽ���ǧ�ֱ�
	SPE_OTHER_SKILL_HEAL_UP = 22,			//!< 22.��������Ч������ p1:�������ưٷֱ�  p2:ָ������ID��Ϊ0��ָ�����ܣ�
	SPE_OTHER_SKILL_HEAL_CRITICAL = 23,		//!< 23.�������ƿɴ������� p1:�������ưٷֱ� p2:��������ǧ�ֱ�
	SPE_OTHER_FEAR_PROB_UP = 24,			//!< 24.��ս������� p1 ����ǧ�ֱ�
	SPE_OTHER_ADD_SKILL_SUCC_RATE = 25,		//!< 25.���Ӽ��ܳɹ��� p1 ���Ӱٷֱ�  p2 ����ID
	SPE_OTHER_ANTI_ABNORMAL_IMPRISON = 26,	//!< 26.�����쳣������״̬����ǧ�ֱ�  p1 ����ǧ�ֱ�
	SPE_OTHER_SKILL_SPLASH_FACTOR_UP = 27,	//!< 27.���ܽ����˺�ϵ������ p1 ����ǧ�ֱ�
	SPE_OTHER_SACRIFICE_FACTOR_DOWN = 28,	//!< 28.��������Ѫ��ϵ������ p1 ����ǧ�ֱ�
	SPE_OTHER_COMBO_SAME_TARGET_NOT_DEC = 29, //!< 29.�������ܴ����ͬĿ��˥����������ǧ�ֱ� p1������ID p2������˥������ǧ�ֱ� 
	SPE_OTHER_COMBO_HIT_TIMES_INC = 30,		//!< 30.�������ܴ���������� p1 ����ID p2 ���Ӵ��� p3:���ʣ�ǧ�ֱȣ�(Ϊ0��ض�������
	SPE_OTHER_SKILL_ADD_MAX_TARGET_NUM = 31,//!< 31.�������Ŀ�������� p1 ����ID  p2 ���Ӵ���  p3:���ʣ�ǧ�ֱȣ�(Ϊ0��ض�������
	SPE_OTHER_SKILL_NO_COST_RAGE_POINT = 32,//!< 32.�ͷż����м��ʲ�����ŭ�� p1 ���ʰٷֱ�
	SPE_OTHER_FIX_DAMAGE_TO_OTHER = 33,		//!< 33.�Ե��˹̶��˺� p1 �̶��˺�ֵ
	SPE_OTHER_FIX_INJURE_FROM_OTHER = 34,	//!< 34.���ܵ��˵��˺�Ϊ�̶�ֵ p1 �̶��˺�ֵ
	SPE_OTHER_SKILL_NO_RESTRAIN_BY_ELEMENT = 35, //!< 35.���ܲ���Ԫ��ˮ������ p1 ����ID��Ϊ0���޼��ܣ�
	SPE_OTHER_MAGIC_CRITICAL_OBSOLETE = 36,			//!< 36.�ͷŷ�������ʱ���и��ʱ��� p1 �������ʰٷֱ� p2 ��������ϵ���ٷֱ� P3:0(ħ��) 1(����(����)) 2(�ظ�) --����
	SPE_OTHER_MAGIC_CRITICAL_SINGLE_SKILL_OBSOLETE = 37, //!< 37. ���巨���и��ʱ��� p1 �������ʰٷֱ� p2 ��������ϵ���ٷֱ�   -- ����
	SPE_OTHER_SUSTAIN_HEAL_UP = 38,			//!< 38.����������ѪBUFF�Ļ�ѪЧ�� p1 �����ٷֱ�  p2 ���Լ���ID��ָ�������ĸ����ܣ���0��ָ����
	SPE_OTHER_SEVERE_INJURE_PROB_UP = 39,	//!< 39.���˸������ p1 ����ǧ�ֱ�
	SPE_OTHER_IMMUNE_TAUNT = 40,			//!< 40.���߳���
	SPE_OTHER_USE_OTHER_SKILL_EFFECT = 41,	//!< 41.ʹ���������ܵļ���Ч�� p1:���ʰٷֱ�  p2:�滻ǰ����ID  p3:�滻����ID �����ǰѼ��ܵ�Ч���滻�������ͷŵ������Ǹ����ܣ� 
	
	SPE_OTHER_ADD_REALIVE_SUCC_RATE = 42,	//!< 42.���Ӹ���ĳɹ��� p1 �ɹ�������ǧ�ֱ�
	SPE_OTHER_ONLY_INJURE_PARTIAL = 43,		//!< 43.�����˺�ʱ�и���ֻ�ܵ������˺�  p1 ���ʰٷֱ�  p2 �˺��ٷֱ�
	SPE_OTHER_TRANSFORM_ENEMY_INJURE_TO_HEAL = 44,//!< 44.����ת���з��˺�Ϊ����  p1 ����ǧ�ֱ�  p2 �˺�ת��ǧ�ֱ�
	SPE_OTHER_STEAL_DEFENSE_ON_DAMAGE_ABSORB_HP = 45, //!< 45.��Ѫʱ����͵ȡ�з����� p1 ���ʰٷֱ�  p2 ͵ȡ�����ٷֱ�  p3 ͵ȡ����BUFF_ID  p4 ��͵ȡ����DEBUFF_ID 
	SPE_OTHER_SKILL_KILL_SOMEONE_EXTRA_NORMAL_ATTACK_RANDOM_TARGET = 46, //!< 46.�ͷż��ܻ�ɱ���м��ʶ�������һ�����˷���һ���չ� p1:���ʰٷֱ�
	SPE_OTHER_INJURE_SKILL_EXTRA_PERFORM = 47,	//!< 47.�˺����ܶ����ͷ�һ��  p1 ���ʰٷֱ�  p2 ��������0, ����������1  p3:����ID����0���ǲ��޼��ܣ�
	SPE_OTHER_TRIGGER_SPID_ADD_HP = 48,		//!< 48.������һ��ĳ��Чʱ���м��ʶ����Ѫ   p1:������ЧID   p2:���ʰٷֱ�   p3:����X(�ٷֱ�)   p4:����Y    ������Чp1ʱ����p2�ĸ��ʶ����Ѫ(�ȼ�*p3/1000+p4)
	SPE_OTHER_UPGRADE_SKILL_POSOBILITY_UP = 49, //!< 49.����ͷ���ǿ�漼�ܵĸ��� p1 ��߰ٷֱ� p2 ����ID p3 ��ǿ�漼��ID 
	SPE_OTHER_ADD_HP_FROM_DAMAGE = 50,		//!< 50.����˺���Ѫ p1 �˺�ת��ǧ�ֱ� P2 �˺�����(BATTLE_DAMAGE_EFFECT_TYPE_MAX)
	SPE_OTHER_BUFF_SUBSTITUTION = 51,		//!< ����ʱ���ϣ� 51.ͨ�����ܼ�BUFFʱ���滻BUFF_ID��BUFF_LV  p1:����ID  p2:���滻��buff_id  p3:�滻���µ�buff_id  p4:�滻���µ�buff_lv  
	SPE_OTHER_SPECIAL_REALIVE_BUFF_UPGRADE = 52,//!< 52.���⸴��Buffǿ�� p1 ����Ѫ������ǧ�ֱ�  
	SPE_OTHER_RESIST_IMPRISON_STATE_BOUNCE = 53,	//!< 53.�ֿ�����Ч���ɹ�,��X%���ʷ�������ԴĿ��(Ŀ�����û�����߸�Ч����ض�����,�޷��ֿ�) P1: X(�ٷֱ�)
	SPE_OTHER_PET_FATAL_INJURE_ADD_HP = 54,			//!< 54 �����ܵ������˺���,X%���Ѫ������Y%(����ʱ������)(�˴���ЧĬ����������) P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�)
	SPE_OTHER_PROB_CONTROL_IMMUNE = 55,				//!< 55 X%�������߿��� P1:X(ǧ�ֱ�) P2:�ڼ��غ�(0������)
	SPE_OTHER_ANTI_IMPRISON_BOUNCE = 56,			//!< 56 �����ֿ�����(����)״̬X%,Y%���ʻػ����� P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�) (�����ػ�����,Ŀ�����û�����߸�Ч����ض�����,�޷��ֿ�)
	SPE_OTHER_MP_LOW_LOSS_HP = 57,					//!< 57 �з���λ����ֵ����X%�������̿۳���ǰ����ֵY% P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�)
	SPE_OTHER_RAGE_POINT_CONSUME_LOW = 58,			//!< 58 ����������ŭ��ֵ���� P1:���͵ĵ���
	SPE_OTHER_ABSORB_HP_RATE_UP = 59,				//!< 59 ��Ѫ�������� p1:����ǧ�ֱ�
	SPE_OTHER_INSTANT_KILL_BEFORE_SKILL_HIT = 60,	//!< 60 ���ܿɴ�����ɱ p1:����ǧ�ֱ� p2:����ID  p3:֪ͨ�ͻ��˲����µļ���ID
	SPE_OTHER_SKILL_ADD_CRITICAL_RATE = 61,			//!< 61 �������������� p1:����ǧ�ֱ� p2:����ID
	SPE_OTHER_CHANGE_SKILL_ID = 62,					//!< 62 �µ��滻����ID  p1:�ɼ���ID_1  p2:�¼���ID_1  p3:�ɼ���ID_2  p4:�¼���ID_2
	SPE_OTHER_COUNTER_ATTACK_RATE_UP = 63,			//!< 63 ������������ p1:����ǧ�ֱ�
	SPE_OTHER_ADD_BUFF_SUCC_ADD_BUFF = 64,			//!< 64 ��BUFF�ɹ��������BUFF  p1:����BuffID  p2:׷��BuffID  p3:׷��Buff�ȼ�
	SPE_OTHER_ADD_EXTRA_PERFORM_SKILL_RATE = 65,	//!< 65 �����Զ����ͷŵļ��ܣ����Ӷ����ͷŵĸ���  p1:����ID  p2:���ӵĸ��� ǧ�ֱ�
	SPE_OTHER_SKILL_DAMAGE_NO_DECLINE_ON_NEXT_TARGET = 66, //!< 66 �˺����ݼ� p1:����ID
	SPE_OTHER_SKILL_REAL_DAMAGE_RATE_UP = 67,		//!< 67 ��߼���Ч��19��Ч��12��Ч��48����ʵ�˺�����  p1:���ݸ���Ч���İٷֱ�/ǧ�ֱ�
	SPE_OTHER_HALO_ADD_DAMAGE_RATIO_UP = 68,		//!< 68 �⻷��Ч3.�����˺�X%  P1������ǧ�ֱ�  P2:����Y%Ѫ����Ч(ǧ�ֱ�)(0�����Ӳ���2)
	SPE_OTHER_SKILL_IGNORE_DEFENSE_AND_SHIELD = 69,	//!< 69 �������ӷ�����ʥ��Ч��  P1:����ID��0���޼��ܣ�
	SPE_OTHER_DEBUFF_COUNTER = 70,					//!< 70 �����ܵ��쳣������Ч��ʱ��ʹ�Է�Ҳ�������Ч��    P1:����ǧ�ֱ�
	SPE_OTHER_SKILL_FACTOR_UP = 71,					//!< 71 ���Ӽ���ϵ���������չ���  P1:����ϵ������ǧ�ֱ�  P2:����ID ����0�����⼼�ܣ�
	SPE_OTHER_SKILL_EXTRA_PERFORM = 72,				//!< 72.�����и��ʶ����ͷ�һ��  p1 ���ʰٷֱ�  p2 ��������0, ����������1  p3:����ID����0���ǲ��޼��ܣ� P4:����ϵ������ٷֱȣ�ע���ǳ˷������Ǽ�����
	SPE_OTHER_BURN_MP_DEEP_BLOW_MP_PERCENT_UP = 73,	//!< 73.��߼���Ч��9���һ���۵ķ���ֵ P1:X�ٷֱ�(ԭ��+X)
	SPE_OTHER_SILENT_PROB_UP = 74,					//!< 74.��Ĭ������� P1:���ǧ�ֱ�
	SPE_OTHER_ADD_CRITICAL_RATE_ON_LOWER_HP = 75,	//!< 75 Ѫ������X%ʱ���������� p1:����������ǧ�ֱ� p2:Ѫ����ֵX���ٷֱȣ�
	SPE_OTHER_SHIELD_CONEFFICIENT_UP = 76,			//!< 76 ��߻���BUFF����37,46,48����ֵ���� P1:���ݸ���Ч����ϵ������(�ٷֱ�/ǧ�ֱ�)
	SPE_OTHER_MAGIC_CRITICAL_2 = 77,				//!< 77.�ͷŷ�������ʱ����ʱ���ӱ�������,��Ŀ��������buff������ʶ������� p1 ��������ǧ�ֱ�  p2 ����BUFFĿ�걩�����ʶ�������ǧ�ֱ� p3:BATTLE_DAMAGE_EFFECT_TYPE_MAX
	SPE_OTHER_ADD_BUFF_TO_TARGET_ON_EXPLODE_EFFECT = 78, //!< 78.�Ա���Ч����ʱ����Ŀ���BUFF   P1:buff_id   P2:buff_lv   P3:����ǧ�ֱ�
	SPE_OTHER_SKILL_EXTRA_PERFORM_ON_CRITICAL = 79,	//!< 79.�����ͷŴ�����ɱ���и��ʶ����ͷ�һ��  p1 ���ʰٷֱ�  p2 ��������0, ����������1  p3:����ID����0���ǲ��޼��ܣ� P4:����ϵ������ٷֱȣ�ע���ǳ˷������Ǽ�����
	SPE_OTHER_GUARD_BY_JIEYI_MEMBER = 80,			//!< 80.���廤��  p1:������Ѫ���ٷֱ���ֵ������Ѫ���ٷֱȲŻỤ����
	SPE_OTHER_ADD_HP_UP_BY_JIEYI_MEMBER = 81,		//!< 81.�ܵ����Խ����Ա������Ч������ p1:���Ӱٷֱ�
	SPE_OTHER_EXTRA_PERFORM_ANOTHER_SKILL = 82,		//!< 82.�ɹ��ͷż���P1���и���P3�ͷż���P2   P1:ԭ����ID    P2:�����ͷŵļ���ID    P3:����ǧ�ֱ�
	SPE_OTHER_SKILL_DAMAGE_REDUCE_DECLINE_ON_NEXT_TARGET = 83, //!< 83 �˺��ݼ�ϵ������ p1:����ID  p2:�˺��ݼ�ϵ������ǧ�ֱ�
	SPE_OTHER_ENEMY_DEAD_RECOVER_HP_DROP_PERCENT = 84,	//!< 84 �з�����ʱ�ָ�����X%������ʧ����ֵ  p1:X ��ǧ�ֱȣ�
	SPE_OTHER_SKILL_EXTRA_PERFORM_ON_KILL = 85,		//!< 85.�����ͷŻ�ɱ���и��ʶ����ͷ�һ��  p1 ���ʰٷֱ�  p2 ��������0, ����������1  p3:����ID����0���ǲ��޼��ܣ� P4:����ϵ������ٷֱȣ�ע���ǳ˷������Ǽ�����
	SPE_OTHER_HEAL_HP_OVERFLOW = 86,	//!< 86.�ܵ����Ʋ��������������ӿɵ�����������BUFF P1:X (�ٷֱ�) P2:buff_id P3:buff_level P4:���غ���(0:û�лغ�����) ����ֵ: X * �������ֵ / �ٷֱ�
	SPE_OTHER_ADD_BUFF_BY_ONLY_INJURE_PERCENT = 87,	//!< 87.�����ܵ��������X%�˺�,���BUFF P1:X(�ٷֱ�) P2:buff_id P3:buff_level P4:���غ���(0:û�лغ�����)
	SPE_OTHER_ADD_MORE_ATTR_BY_TRIGGER_SPE_44 = 88,	//!< 88.��ʱ��������Ч44�ӵ�����ֵ����X%�� P1:X(ǧ�ֱ�)
	SPE_OTHER_TAUNT_UPGRADE_1 = 89,					//!< 89.ǿ���������������ʺͷ���ϵ����  P1:������(�ٷֱ�)  P2:����ϵ��(�ٷֱȣ�
	SPE_OTHER_WIND_SEED_CONTROL_POSIBILITY_UP = 90,	//!< 90.����쫷����ӵĿ��Ƹ��� p1:���ӵĸ���ǧ�ֱ�
	SPE_OTHER_REAL_DAMAGE_SKILL_EFFECT_UP = 91,		//!< 91.���˼���Ч��ǿ��������Ч��12��  p1:����X  p2:����Y 
	SPE_OTHER_GUARD_BY_QING_YUAN_TARGET = 92,		//!< 92.��Ե���� ��Ѫ����X,����һ����Y����������,����ʱ���BUFF P1:X(�ٷֱ�) P2:Y(�ٷֱ�) P3:buff_id P4:buff_level
	SPE_OTHER_SACRIFICE_SELF_ADD_QINGYUAN = 93,		//!< 93.��������ǰX%Ѫ��������,����ԵĿ��ӿɵ�����������BUFF�����������˺� P1:X(�ٷֱ�) P2:Y(�ٷֱ�) P3:buff_id P4:buff_level ����ֵ:Y*���Ѫ��(��ԵĿ��)/�ٷֱ� ǰ���ɫѪ����������Ҫ����50%
	SPE_OTHER_ADD_BUFF_QING_YUAN_BY_SELF_DIE = 94,	//!< 94.��������,����ԵĿ�����BUFF  P1:buff_id P2:buff_level

	SPE_OTHER_ATTR_PHYSICAL_DEC_DAMAGE = 95,		//!< 95.�������Ըı䣨Ч��Ӧ��ͬ�ڸ����ԣ� P1:�ı�ǧ�ֱ�
	SPE_OTHER_ATTR_MAGIC_DEC_DAMAGE = 96,			//!< 96.�������Ըı䣨Ч��Ӧ��ͬ�ڸ����ԣ� P1:�ı�ǧ�ֱ�
	SPE_OTHER_ATTR_PHYSICAL_INC_DAMAGE = 97,		//!< 97.�������Ըı䣨Ч��Ӧ��ͬ�ڸ����ԣ� P1:�ı�ǧ�ֱ�
	SPE_OTHER_ATTR_MAGIC_INC_DAMAGE = 98,			//!< 98.�������Ըı䣨Ч��Ӧ��ͬ�ڸ����ԣ� p1:�ı�ǧ�ֱ�
	SPE_OTHER_ATTR_CRTICAL = 99,					//!< 99.��ɱ���Ըı� (Ч��Ӧ��ͬ�ڸ�����)  p1:�ı�ǧ�ֱ�
	SPE_OTHER_ATTR_COUNTER_ATTACK = 100,			//!< 100.���������Ըı�(Ч��Ӧ��ͬ�ڸ�����) p1:�ı�ǧ�ֱ�
	SPE_OTHER_HURT_ME_HP_LESS_INSTANT_KILL = 101,	//!< 101.Ŀ���ܵ��˺�ǰ,Ѫ������X%,�򴥷���ɱ P1:X(ǧ�ֱ�)    -- (��������:�޷��Զ�Ա,PVP)
	SPE_OTHER_ADD_DAMAGE_HP_SHILED_BUFF_HEAL = 102,	//!< 102.����˺���������BUFF��λ�ظ�X%ѪY%��(���) P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�)  (���޼���Ч��52����)
	SPE_OTHER_REMOVE_BUFF = 103,					//!< 103.ɾ������BUFF P1:buff_id P2: 0.�κ����ͼ��� 1.�˺��ͼ���
	SPE_OTHER_COMBO_SAME_TARGET_EXTRA_DAMAGE = 104,	//!< 104.�������ܴ����ͬĿ����ɶ���X%�������ֵ����ʵ�˺� p1������ID p2��X(�ٷֱ�) p3: BATTLE_PLAYER_TYPE_MAX
	SPE_OTHER_ADD_BUFF_BY_ME_SIDE_OTHER_GRID_DIE = 105,	//!< 105.������λ����ʱ,���������BUFF P1:buff_id P2:buff_level P3:����(�ٷֱ�)
	SPE_OTHER_DELIVER_CHAR_DIE_OR_ADD_BUFF_BY_DIE = 106,	//!< 106.��λ�������,����X��������ɸõ�λ���������,�����buff P1:X����(�ٷֱ�) P2:buff_id P3:buff_level
	SPE_OTHER_ABSORB_CONVERT_BUFF_EFFECT_UP = 107,	//!< 107.��������ת������buff������������/����Y%Ѫ������ P1:X(�ٷֱ�) P2:buff_id(0:������) P3:Y(�ٷֱ�)  ��ʽ : �������ֵ * X /100 
	SPE_OTHER_PERFORM_SKILL_BY_LAST_SKILL_KILL_BC = 108,	//!< 108.�ϸ����ܻ�ɱ��λ,���ʻ��ͷ�һ������ P1:skill_id P2:skill_level P3:����(ǧ�ֱ�)
	SPE_OTHER_ADD_HP_UP_2 = 109,					//!< 109.�ܵ�����Ч������  p1 ����ǧ�ֱȷֱ�
	SPE_OTHER_SKILL_HEAL_UP_2 = 110,				//!< 110.��������Ч������ p1:��������ǧ�ֱ�  p2:ָ������ID��Ϊ0��ָ�����ܣ�
	SPE_OTHER_SKILL_SPLASH_DAMAGE_TARGET_ADD_BUFF = 111,	//!< 111.���ܵ������˺��ĵ�λ���BUFF(���޼���Ч��18) P1:buff_id P2:buff_level P3:����(ǧ�ֱ�) P4:����ID(0:������)

	SPE_OTHER_ELEMENT_CRYSTAL_DAMAGE_ONLY = 112,	//!< 112 ָ�����鼰��������Ĺ���������ɱ������ֻ��1���˺�  P1:ˮ������[1ˮ 2�� 3�� 4��]
	SPE_OTHER_GAMBLE_STRIKE_SUCC_SPIKE = 113,		//!< 113 ��ɱ�ɹ�����תΪ��ɱ P1:X(ǧ�ֱ�) P2:(0:��boss��Ч)

	SPE_OTHER_HALO_STILL_ACTIVE_ON_DIE = 114,		//!< 114.�⻷����Ҳ������ʧ�������ɻ���ʧ�� 
	SPE_OTHER_LOCK_HP_AND_BUFF_ON_DEADLY_INJURE = 115,	//!< 115.�ܵ������˺�ʱ��1��Ѫ,����Buff   P1 buff_id  P2 buff_lv
	SPE_OTHER_BOUNCE_DAMAGE_ON_DEADLY_INJURE = 116,	//!< 116. �ܵ������˺�ʱ�м��ʷ���ȫ���˺�(�������������Ѫ��) P1 ���ʰٷֱ�
	SPE_OTHER_SKILL_INSTANT_PERFORM = 117,			//!< 117. �����м���˲�� p1:����ID  p2:���ʰٷֱ� 
	SPE_OTHER_HEAL_SKILL_EXTRA_PERFORM = 118,		//!< 118.��Ѫ���ܶ����ͷ�һ��  p1 ���ʰٷֱ�  p2:����ID����0���ǲ��޼��ܣ�
	SPE_OTHER_IMMUNE_ALL_BUFF = 119,				//!< 119.��������͸���BUFF
	SPE_OTHER_RUN_AWAY_ON_ROUND_END = 120,			//!< 120.��ָ���غϽ���ʱ���� p1:�غ���
	SPE_OTHER_FATAL_INJURE_ADD_HP = 121,			//!< 121 �ܵ������˺���,X%���Ѫ������Y%(����ʱ������) P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�)
	SPE_OTHER_WITHSTAND_INSTANT_KILL = 122,			//!< 122 ���ʵֵ�ĳЩ��ɱЧ�� P1:����(ǧ�ֱ�) (Ŀǰ���޼���Ч��7,45,ս��������Ч60,113)
	SPE_OTHER_ADD_HP_UP_WITH_HUANSHOU_LEVEL = 123,	//!< 123 ���ڿ���״̬ʱ,�����ܵ�������Ч�� P1:X(ǧ�ֱ�) ���ӹ�ʽ:���޵ȼ�*X
	SPE_OTHER_ADD_DEF_WITH_HUANSHOU_LEVEL = 124,	//!< 124 ���ڿ���״̬ʱ,���ӷ���/���� P1:X(ǧ�ֱ�) P2:BATTLE_DAMAGE_EFFECT_TYPE_MAX ���ӹ�ʽ:���޵ȼ�*X
	SPE_OTHER_ADD_BUFF_BY_CONSUME_RAGE_POINTS = 125,//!< 125 ����Ԫ��,���BUFF P1:buff_id P2:buff_level (���ĵ�Ԫ����ֵ��ͬbuff����)

	SPE_OTHER_MAX,
};

enum SPECIAL_EFFECT_TIMING //!< ����ʱ��
{
	SPE_TIMING_INVALID = 0,
	SPE_TIMING_ON_BATTLE_START = 1,			//!< 1  ս����ʼʱ
	SPE_TIMING_BEFORE_PERFORM_SKILL = 2,	//!< 2  �����ͷŻ򹥻�ǰ
	SPE_TIMING_BEFORE_SKILL_HIT_OTHER = 3,	//!< 3  ��������ǰ
	SPE_TIMING_BEFORE_SKILL_HIT_ME = 4,		//!< 4  ����������ǰ
	SPE_TIMING_BEFORE_HURT_OTHER = 5,		//!< 5  ����˺�ǰ
	SPE_TIMING_BEFORE_HURT_ME = 6,			//!< 6  �ܵ��˺�ǰ (����)
	SPE_TIMING_ON_HP_CHANGE = 7,			//!< 7  HP�仯ʱ (����)
	SPE_TIMING_ON_MP_CHANGE = 8,			//!< 8  MP�仯ʱ (����)
	SPE_TIMING_ON_DIE = 9,					//!< 9  ��������
	SPE_TIMING_AFTER_HURT_OTHER = 10,		//!< 10 ����˺���
	SPE_TIMING_AFTER_HURT_ME = 11,			//!< 11 �ܵ��˺���
	SPE_TIMING_AFTER_SKILL_HIT_OTHER = 12,	//!< 12 �������к�
	SPE_TIMING_AFTER_SKILL_HIT_ME = 13,		//!< 13 ���������к�
	SPE_TIMING_AFTER_PERFORM_SKILL = 14,	//!< 14 �����ͷŻ򹥻��� (����)
	SPE_TIMING_CRITICAL_HIT_OTHER = 15,		//!< 15 ��ɱ�����
	SPE_TIMING_CRITICAL_HIT_ME = 16,		//!< 16 �ܵ�������
	SPE_TIMING_ON_DODGE = 17,				//!< 17 ����ʱ
	SPE_TIMING_ON_COUNTER_ATTACK = 18,		//!< 18 ����ʱ
	SPE_TIMING_ON_SACRIFICE_EFFECT = 19,	//!< 19 ����Ч������ʱ (����)
	SPE_TIMING_ON_GUARD_OTHER = 20,			//!< 20 ����������ʱ
	SPE_TIMING_ADD_HP_TO_OTHER = 21,		//!< 21 �ظ�������ɫѪ��ʱ
	SPE_TIMING_ADD_HP_TO_ME = 22,			//!< 22 �ܵ���Ѫʱ
	SPE_TIMING_ACTION_START = 23,			//!< 23 �ж���ʼʱ
	SPE_TIMING_ACTION_END = 24,				//!< 24 �ж�����ʱ
	SPE_TIMING_DEFENSE_SUCC = 25,			//!< 25 �ɹ�����ʱ
	SPE_TIMING_SUFFER_DEADLY_INJURE = 26,	//!< 26 �ܵ������˺�ǰ
	SPE_TIMING_ROUND_END = 27,				//!< 27 �غϽ���ʱ
	SPE_TIMING_TAUNT_SUCC = 28,				//!< 28 ����ɹ���
	SPE_TIMING_ON_KILL_SOMEONE = 29,		//!< 29 ��ɱʱ
	SPE_TIMING_AFTER_SUFFER_DEADLY_INJURE = 30, //!< 30 ����ǰ���� ��������9������������һ��˲�䣩(������Щ��Ҫ�ȸ�����紥����������Ч��
	SPE_TIMING_ON_SUMMON = 31,				//!< 31 ���ٻ�����ս��ʱ
	SPE_TIMING_ON_RESIST_DEBUFF = 32,		//!< 32 �ֿ�����ʱ
	SPE_TIMING_ON_ROUND_PREPARE = 33,		//!< 33 321׼��ǰ
	SPE_TIMING_ON_GAMBLE_STRIKE_FAILED = 34,//!< 34 ��ɱʧ��ʱ
	SPE_TIMING_ON_GAMBLE_STRIKE_SUCC = 35,	//!< 35 ��ɱ�ɹ�ʱ
	SPE_TIMING_ON_PURIFY_OTHER_SUCC = 36,	//!< 36 �����ɹ�������˲���״̬ʱ����ָʹ�ü��ܡ�������ҩƷ�ľ���ʱ��������������Ч��
	SPE_TIMING_ON_ACTIVE_SUMMON_PET = 37,	//!< 37 �����ٻ��������ս��ʱ
	SPE_TIMING_ON_ACTIVE_STEALTH_BREAK = 38,//!< 38 �����Ƴ�����ʱ
	SPE_TIMING_ON_GUARD_OTHER_END = 39,		//!< 39 ���������˽�����
	SPE_TIMING_ON_ROUND_END_BEFORE_BUFF = 40,//!< 40 �غϽ���ǰ����(��buff�غϽ�����һ��˲��)

	SPE_TIMING_MAX,
};

enum SPE_TRIGGER_EFFECT
{
	SPE_TRIGGER_EFFECT_INVALID = 0,
	SPE_TRIGGER_EFFECT_ADD_HP = 1,			//!< 1. ��Ѫ  P1 Ŀ��(0:����  1:Ŀ��) P2 ��Ѫ��ֵ���������0����ΪԼ���˲��ṩֱ�ӿ�Ѫ����Ч�� p3 �Ƿ�ǧ�ֱȼ�Ѫ(0:�� 1:�ǣ�p4:����(ǧ�ֱ�,0��1000������ض��ɹ�)
	SPE_TRIGGER_EFFECT_MODIFY_MP = 2,		//!< 2. �ı�����  P1 Ŀ��(0:����  1:Ŀ��) P2 �ı���ֵ����Ϊ����  P3 �Ƿ�ٷֱ�(0:�� 1:��)  
	SPE_TRIGGER_EFFECT_ADD_BUFF = 3,		//!< 3. ��buff  P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 buff_id  P3 buff�ȼ�  P4 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���
	SPE_TRIGGER_EFFECT_ABSORB_HP_FROM_DAMAGE = 4,		//!< 4. ����˺���Ѫ  P1 ��Ѫǧ�ֱ�   P2 �˺�����(0:����  1:ħ��  2:�����ħ��)
	SPE_TRIGGER_EFFECT_REALIVE = 5,			//!< 5. ����  P1 Ŀ��(0:����  1:Ŀ��)   P2 ��Ѫ��ֵ���������0����ΪԼ���˲��ṩֱ�ӿ�Ѫ����Ч�� p3 �Ƿ�ٷֱȼ�Ѫ(0:�� 1:�ǣ� P4 ���ʣ��ٷֱȣ�
	SPE_TRIGGER_EFFECT_ADD_HP_BY_LEVEL = 6, //!< 6. ������ȼ���Ѫ  P1 Ŀ��(0:����  1:Ŀ��) P2 ��Ѫ��ֵ����ǧ�ֱȷֱ�  P3 һ���̶�ֵ  �ܼ�Ѫ������ P2 / 1000 * level + P3
	SPE_TRIGGER_EFFECT_MODIFY_MP_BY_LEVEL = 7, //!< 7. ������ȼ��ı�����  P1 Ŀ��(0:����  1:Ŀ��) P2 ������ֵ�����ٷֱ�   �������ı�ֵ���� P2 / 100 * level
	SPE_TRIGGER_EFFECT_CLEAR_NEGATIVE_STATE = 8, //!< 8. ���������״̬��ֻ���쳣�ͽ���״̬�� P1�����ʰٷֱ�
	SPE_TRIGGER_EFFECT_ADD_HP_BY_ATK = 9,	//!< 9. ����������Ѫ P1 Ŀ�꣨0:����  1:Ŀ�꣩ P2:��Ѫ��ֵ�����ٷֱ�  P3 һ���̶�ֵ   �ܼ�Ѫ������ P2 / 100 * atk + P3
	SPE_TRIGGER_EFFECT_FRIEND_ADD_HP = 10,	//!< 10. ���Ѿ���Ѫ  P1 ��Ѫ��ֵ  P2 �Ƿ�ǧ�ֱȼ�Ѫ(0:�� 1:�ǣ�
	SPE_TRIGGER_EFFECT_EXPLODE = 11,					//!< 11. �Եз����е�λ�����ʵ�˺� P1 �˺�ϵ���ٷֱ�  �˺�=������*P1/100  P2:����(�ٷֱ� 0��100������ض��ɹ�)
	SPE_TRIGGER_EFFECT_REALIVE_ON_ALLY_ALIVE = 12, //!< 12. ������ʱ������ѷ�����������λ������Ѫ����
	SPE_TRIGGER_EFFECT_REALIVE_2 = 13,		//!< 13. �����Ѫ����BUFF P1 ��Ѫ�ٷֱ�  P2 BUFF_ID  P3 BUFF�ȼ�  P4 ���ʣ��ٷֱȣ�
	SPE_TRIGGER_EFFECT_ADD_ATK_BY_RECOVER_ATTR = 14, //!< 14. �������һ�����ݻظ�ֵ����ļӹ�BUFF  P1 BUFF_ID   P2 ת���ٷֱ�   �ӹ���ֵ=�ظ�ֵ*ת���ٷֱ�
	SPE_TRIGGER_EFFECT_FRIEND_ADD_MP = 15,				//!< 15. ���Ѿ�����  P1 ������ֵ  P2 �Ƿ�ǧ�ֱȼ���(0:�� 1:�ǣ�
	SPE_TRIGGER_EFFECT_CLEAR_FRIEND_NEGATIVE = 16,		//!< 16. ���������ɸ��Ѿ��Ŀ���  P1 ���ʰٷֱ�  P2 ���X����� 
	SPE_TRIGGER_EFFECT_PERFORM_FIRST_SKILL = 17,		//!< 17. �����ͷŵ�һ������  P1 ���ʰٷֱ�
	SPE_TRIGGER_EFFECT_PERFORM_SKILL = 18,				//!< 18. �����ͷ�һ�����ܣ������ɫû��������ܣ�Ŀ������� P1 ���ʰٷֱ� P2 ����ID P3 ���ܵȼ�
	SPE_TRIGGER_EFFECT_REMOVE_BUFF = 19,				//!< 19. ���һ��BUFF  P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 buff_id
	
	SPE_TRIGGER_EFFECT_DEC_HP_ADD_BUFF = 20,			//!< 20. �ϸ��غ��ܵ�Ѫ������X%�˺�ʱ,��Y%�������BUFF P1 buff_id P2 buff�ȼ� P3 X P4 Y  ����Ϊ�ٷֱ�
	SPE_TRIGGER_EFFECT_NOT_USE_INJURE_SKILL = 21,		//!< 21. �ϸ��غ�δʩ���˺��༼��(�������չ�),��BUFF P1 buff_id P2 buff�ȼ� 
	SPE_TRIGGER_EFFECT_DEBUFF_BY_HIGHEST_ATTR_1 = 22,	//!< 22. �з����������񡢻ظ�������ߵĽ�ɫ����DEBUFF  P1 buff_id P2 buff�ȼ�
	SPE_TRIGGER_EFFECT_ADD_SELF_SIDE_BUFF = 23,			//!< 23. ���ҷ����side_buff P1:side_buff_id
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_ATTR = 24,			//!< 24. ��������ֵ�����ҷ���ɫ���BUFF P1:buff_id P2:buff�ȼ� P3:�������� P4:0:��� 1:���
	SPE_TRIGGER_EFFECT_ADD_TWO_BUFF = 25,				//!< 25. ���2��BUFF P1.BUFF_1 P2.BUFF_2 P3.BUFF�ȼ� P4.����(�ٷֱ�) (Ŀ��Ϊ����)
	SPE_TRIGGER_EFFECT_HEART_SKILL_INVAILD = 26,		//!< 26. �ķ�����ʧЧ P1:ǧ�ֱ�(ʧЧ����) P2:���ö���(0:����  1:Ŀ��) (��ǰ���޵з�Ŀ��)
	SPE_TRIGGER_EFFECT_ADD_BUF_AND_SET_HP = 27,			//!< 27. ���������BUFF������Ѫ�� P1:buff_id P2:buff_level P3:Ѫ��ֵ(<=0��Ĭ������1��Ѫ)
	SPE_TRIGGER_EFFECT_REALIVE_BEFORE_BUFF_CLEAR = 28,	//!< 28. ����buffǰ���ֻ�����ʱ��30ʹ�ã� P1 Ŀ��(0:����  1:Ŀ��)   P2 ��Ѫ��ֵ���������0����ΪԼ���˲��ṩֱ�ӿ�Ѫ����Ч�� p3 �Ƿ�ٷֱȼ�Ѫ(0:�� 1:�ǣ� P4 ���ʣ��ٷֱȣ�
	SPE_TRIGGER_EFFECT_CLEAR_TARGET_NEGATIVE_STATE = 29,//!< 29. ���� P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���
	SPE_TRIGGER_EFFECT_LOW_DAMAGE_ADD_BUFF = 30,		//!< 30. �ϸ��غ��ܵ����˺�С�ڻ����X,����������BUFF P1:buff_id P2:buff_level P3:X(�˺�ֵ)
	SPE_TRIGGER_EFFECT_ADD_SP_BY_HAS_SP = 31,			//!< 31. �ж��Ƿ��в����е���Ч,û�������һ����� P1:sp_id_1 P2:sp_id_2
	SPE_TRIGGER_EFFECT_CHANGE_SP_BY_OTHER_SP = 32,		//!< 32. ��Чѭ�� P1:sp_id_1 P2:sp_id_2 P3:��������(ÿ����������ɾ�����������һ��)
	SPE_TRIGGER_EFFECT_OUR_ADD_BUFF = 33,				//!< 33. ���ҷ�X����λY�������BUFF P1:buff_id P2:buff_level P3:X P4:Y����(ǧ�ֱ�) (��������)
	SPE_TRIGGER_EFFECT_MODIFY_RAGE_POINT = 34,			//!< 34. �޸�ŭ��ֵ P1:�ı��ֵ����Ϊ���� P2:����ǧ�ֱ�
	SPE_TRIGGER_EFFECT_MAXHP_PERCENT_DAMAGE = 35,		//!< 35. ��Ŀ���������Ѫ���޵�X%�˺� P1:���Ѫ��ǧ�ֱ�  P2:��Ҫ�����쳣�����״̬��(1:�� 0:��)
	SPE_TRIGGER_EFFECT_SIDE_ADD_BUFF = 36,				//!< 36. ��һ����BUFF  P1:[0=���� 1=�з�]  P2 buff_id  P3 buff�ȼ�  P4 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���
	SPE_TRIGGER_EFFECT_REAL_DAMAGE_TO_ALL_ENEMY = 37,	//!< 37. �Եз�ȫ���������  P1:ϵ��ǧ�ֱ�   �˺�=��ɫ�ȼ�*ϵ��
	SPE_TRIGGER_EFFECT_ADD_BUFF_TO_ALL_ENEMY = 38,		//!< 38. �Եз�ȫ���BUFF  P1:Buff_id  P2:Buff�ȼ�   P3:����ǧ�ֱ�
	SPE_TRIGGER_EFFECT_ADD_BUFF_TO_RANDOM_ENEMEY = 39,	//!< 39. �Եз����һ����λ��BUFF  P1:Buff_id  P2:Buff�ȼ�   P3:����ǧ�ֱ�
	SPE_TRIGGER_EFFECT_SUMMON_MONSTER = 40,				//!< 40. �ٻ�����  P1:�ٻ�seq
	SPE_TRIGGER_EFFECT_REALIVE_BY_JIEYI_MEMBER = 41,	//!< 41. ����ʱ�ɽ����Ա���и���  p1:������Ѫ���ٷֱ�   p2:������븴���Ѫ���ٷֱ�  p3:����Ѫ���ٷֱ�

	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_LEVEL = 42,		//!< 42. ��Ŀ��ĵȼ�*ϵ������Ŀ�������BUFF  P1:�ȼ�ϵ��ǧ�ֱ�  P2:��������   P3:buff_id  P4:buff�ȼ� ��BUFF����ʹ������29������ֵ�ı����Ե�BUFF��
	SPE_TRIGGER_EFFECT_ADJACENT_GRID_DAMAGE_BY_SKILL_DAMAGE = 43, //!< 43. ��Χһ��ĵз���λ�ܵ����μ����˺���ǧ�ֱ��˺� P1:ǧ�ֱ�
	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_LAST_ROUND_INJURE = 44, //!< 44. �����ϻغ��ܵ��˺�ֵ�������BUFF  P1:�˺�ֵת��ϵ��ǧ�ֱ� ���������ֵ=������ֵ*P1/1000) P2:��������   P3:buff_id  P4:buff�ȼ� ��BUFF����ʹ������29������ֵ�ı����Ե�BUFF��
	SPE_TRIGGER_EFFECT_ADD_BUFF_IF_CUR_HP_PERCENT_LOWER_THAN = 45, //!< 45. ������ֵ����X%ʱ����Y%���ʻ��BUFF  P1:X(ǧ�ֱ�)  P2:Y(ǧ�ֱ�)  P3:BUFF_ID  P4:BUFF�ȼ�
	SPE_TRIGGER_EFFECT_ADD_CAN_MERGE_HP_SHIELD_BUFF = 46,	//!< 46.�����������X%Ѫ����ӿɵ�����������BUFF P1:X(�ٷֱ�) P2:Y(�ٷֱ�) P3:buff_id P4:buff_level ����ֵ : Y * ���Ѫ�� / �ٷֱ�
	SPE_TRIGGER_EFFECT_BURN_AND_ABSORB_TARGET_MP = 47,		//!< 47.ȼ��Ŀ�����������������ȼ�յĲ���  P1:ȼ��ǧ�ֱ�  P2:����ǧ�ֱ�
	SPE_TRIGGER_EFFECT_ADD_BUFF_CERTAINLY = 48,				//!< 48.���BUFF(�������,��������) P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 buff_id  P3 buff�ȼ�

	SPE_TRIGGER_EFFECT_CAN_DIE_GRID_ADD_BUFF = 49,			//!< 49.��buff(���Ը�������λ���)  P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 buff_id  P3 buff�ȼ�  P4 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���
	SPE_TRIGGER_EFFECT_REMOVE_BUFF_QINGYUAN_TARGET = 50,	//!< 50.X���������ԵĿ���������Y������״̬ P1:X(�ٷֱ�) P2:Y
	SPE_TRIGGER_EFFECT_ADD_BUFF_AND_ADD_HP = 51,			//!< 51.��BUFF���ظ�����Ѫ�� P1:X���ʴ���(�ٷֱ�) P2:buff_id P3:buff_level P4:Y(�ٷֱ�) �ظ�ֵ:Y * ���Ѫ��/100
	SPE_TRIGGER_EFFECT_MP_LESS_RECOVER_MP = 52,				//!< 52.��������X%,��ظ�Y%�����ֵ  P1:X(ǧ�ֱ�)  P2:Y(ǧ�ֱ�)

	SPE_TRIGGER_EFFECT_RECORD_BURN_MP_ADD_MP_NUM = 53,		//!< 53.��Ч54��ȼ��Ŀ���������ǰ����Ч  P1:ȼ��ǧ�ֱ�  P2:����ǧ�ֱ�
	SPE_TRIGGER_EFFECT_BURN_MP_ADD_MP_BASE_ON_RECORD = 54,	//!< 54.ȼ��Ŀ������������Ч53��¼����ֵ��
	SPE_TRIGGER_EFFECT_REVIVE_OR_HEAL = 55,					//!< 55.�������һ��������λ,�ظ�X%��Y%����,��û��������λ������ظ�һ����λZ%Ѫ����F%���� P1:X P2:Y P3:Z P4:F (���ǰٷֱ�,�������Ѫ��Ϊ����)(ͬ��Ӫ,���������)
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_ADJACENT_GRID = 56,		//!< 56.��������Χһ��λ���BUFF P1:buff_id  P2 buff�ȼ� P3 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���(��������)
	SPE_TRIGGER_EFFECT_ADD_CAN_MERGE_HP_SHIELD_BUFF_1 = 57,	//!< 57.��������ӿɵ�����������BUFF P1 buff_id  P2 buff�ȼ�  P3 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���P4 ϵ��   ���ܹ�ʽ:����ȼ� *��P4
	SPE_TRIGGER_EFFECT_PERFORM_SKILL_1 = 58,				//!< 58.�����ͷ�һ�����ܣ������ɫû��������ܣ��������ȼ���������λ,��������λ����(������)�� P1 ���ʰٷֱ� P2 ����ID P3 ���ܵȼ�
	SPE_TRIGGER_EFFECT_ADD_BUFF_BY_TARGET_ENEMY_SIDE = 59,	//!< 59.��Ϊ�з�������,���buff  P1 ���ö���(0:����  1:Ŀ��  2:����λ��ȡ��) P2 buff_id  P3 buff�ȼ�  P4 ���ʣ�ǧ�ֱ�, 0��1000������ض��ɹ���
	SPE_TRIGGER_EFFECT_POSIBLE_BURN_MANA = 60,				//!< 60.����ȼ������  P1 ���ʰٷֱ�   P2 ȼ����ֵ   
	SPE_TRIGGER_EFFECT_EXPLODE_2 = 61,						//!< 61.����ʱ�Եз����е�λ�����ʵ�˺�  P1 �̶�����
	SPE_TRIGGER_EFFECT_ADD_SHIELD_BY_MAXHP = 62,			//!< 62.�ӿɵ�����������BUFF����ֵ���������ֵ��أ� P1 buff_id  P2 buff�ȼ�  P3 �������ֵϵ���ٷֱ�   ����ֵ=�������ֵ*P3/100
	SPE_TRIGGER_EFFECT_PERFORM_SKILL_2 = 63,				//!< 63.�����ͷ�һ�����ܣ������ɫû��������ܣ��������ȼ����쳣״̬��λ,���쳣״̬��λ�����һ�����ѣ� P1 ���ʰٷֱ� P2 ����ID P3 ���ܵȼ�
	SPE_TRIGGER_EFFECT_CLEAR_NEGATIVE_STATE_1 = 64,			//!< 64.���X��������״̬��ֻ���쳣�ͽ���״̬�� P1�����ʰٷֱ� P2:X
	SPE_TRIGGER_EFFECT_LAST_ROUND_NOT_CAUSE_DAMAGE = 65,	//!< 65.�ϸ��غ����û������˺�,���BUFF P1 buff_id P2 buff�ȼ� P3 ����(ǧ�ֱ�)
	SPE_TRIGGER_EFFECT_ADD_ATTR_BUFF_BY_ROLE_ATTR = 66,		//!< 66.�������BUFF,����ֵ���ڽ�ɫ P1:buff_id P2:buff_level P3:attr_type P4:��������(ǧ�ֱ�)(����ֵ:��ɫ����*P4/1000)��BUFF����ʹ������29������ֵ�ı����Ե�BUFF��
	SPE_TRIGGER_EFFECT_ADD_BUFF_ENEMY_ALL_BC_FAKE_SKILL = 67,//!< 67.���з�ȫ��X�����ͷżټ���,Y�������BUFF(����Ϊ�ټ���) P1:X(ǧ�ֱ�) P2:Y(ǧ�ֱ�) P3:buff_id P4:skill_id


	SPE_TRIGGER_EFFECT_MAX,
};

enum SPE_EFFECT_COND						//!<  ��Ч���������� 
{
	SPE_SKILL_COND_INVALID = 0,
	SPE_SKILL_COND_SKILL_ID,				//!<  1. ������ID
	SPE_SKILL_COND_SKILL_TYPE,				//!<  2. ����������
	SPE_SKILL_COND_SKILL_MAGIC_TYPE,		//!<  3. �����ܵķ�������
	SPE_SKILL_COND_NORMAL_ATTACK_CRITICAL,	//!<  4. �չ��ұ���
	SPE_SKILL_COND_SKILL_CRITICAL,			//!<  5. ������ID���Ҵ�������  p1 ����ID
	SPE_SKILL_COND_KILL_SOMEONE,			//!<  6. ������ID������ɻ�ɱ  p1 ����ID
	SPE_SKILL_COND_MAGIC_OR_PHYSIC_SKILL,	//!<  7. �������Ƿ����������� p1 ��1���Ƿ�������0�������� 
	SPE_SKILL_COND_IS_HEAL_SKILL,			//!<  8. �жϼ����Ƿ����Ƽ���
	SPE_SKILL_COND_ON_STEALTH_OR_ACTION_UNDER_STEALTH_TRIGGER_CRITICAL, //!< 9. ��������״̬�£������������ж���ʼʱ��������״̬�£������� ��������ʱ
	SPE_SKILL_COND_MAX,
};


#endif