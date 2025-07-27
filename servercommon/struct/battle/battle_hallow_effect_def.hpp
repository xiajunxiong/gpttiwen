#ifndef __BATTLE_HALLOW_EFFECT_DEF_HPP__
#define __BATTLE_HALLOW_EFFECT_DEF_HPP__

enum HALLOW_EFFECT
{
	HALLOW_EFFECT_INVALID = 0,
	HALLOW_EFFECT_REAL_DAMAGE = 1,		//1 ��Ŀ�����ʥ���ȼ�*X����ʵ�˺�  param1:X 
	HALLOW_EFFECT_REAL_DAMAGE_FLOAT = 2, //2 ��Ŀ�����ʥ���ȼ�*X����ʵ�˺����˺�������Y%~Z%֮�両��)  param1:X  param2:Y  param3:Z
	HALLOW_EFFECT_REAL_DAMAGE_MP_CONSUMED = 3, //3 ��Ŀ�����ʥ���ȼ�*{p1}*�����ķ�����������ʵ�˺� ��������ȡֵ��Χ[0.0,1.0]
	HALLOW_EFFECT_REAL_DAMAGE_MP_REMAIN = 4, //4 ��Ŀ�����ʥ���ȼ�*{p1}*ʣ�෨����������ʵ�˺� ��������ȡֵ��Χ[0.0,1.0]
	HALLOW_EFFECT_REAL_DAMAGE_TWICE_1 = 5, //5 ��Ŀ�����ʥ���ȼ�*{p1}����ʵ�˺����˺�����Ŀ��Ѫ�������ڡ�{p2}%, �����{p3}%�Ķ����˺�
	HALLOW_EFFECT_REAL_DAMAGE_TWICE_2 = 6, //6 ��Ŀ�����ʥ���ȼ�*{p1}����ʵ�˺����˺�����Ŀ��Ѫ����С�ڡ�{p2}%, �����{p3}%�Ķ����˺�
	HALLOW_EFFECT_BURN_MAX_MP = 7,	//7 ����Ŀ�����������{p1}%����	
	HALLOW_EFFECT_ADD_HP = 8,	//8 �ظ�Ŀ��ʥ���ȼ�*{p1}��Ѫ��
	HALLOW_EFFECT_ADD_HP_TWICE = 9, //9 �ظ�Ŀ��ʥ���ȼ�*{p1}��Ѫ�� �����Ŀ��Ѫ����С�ڡ�{p2}%,���ٴλظ����λظ�Ѫ����{p3}%
	HALLOW_EFFECT_CLEAR_NEGATIVE_STATE = 10, //10 {p1}%����������и���״̬(����״̬���쳣״̬)
	HALLOW_EFFECT_ADD_BUFF = 11,	//11 ��Ŀ���Buff  p1:buff_id  p2:buff_lv  p3:���ʰٷֱ�
	HALLOW_EFFECT_KNOCKOFF_HALLOW = 12, //12 ����Ŀ�귨��
	HALLOW_EFFECT_REAL_DAMAGE_EQUALLY_SHARED = 13, //13 ���ʥ���ȼ�*{p1}�Ĺ̶��˺���ƽ�����������Ŀ��
	HALLOW_EFFECT_PET_RUN_AWAY = 14,	//14 ʹ������{p1}%�������� [0,100]
	HALLOW_EFFECT_REALIVE = 15,		//15 ����  ��Ѫ����ʥ���ȼ�*{p1}
	HALLOW_EFFECT_ADD_MP = 16,		//16 �ظ�Ŀ��{p1}%���� ���Ŀ�굱ǰ��������{p2}%,���Ϊ�ظ�{p3}%����
	HALLOW_EFFECT_ADD_SHIELD_BUFF = 17,	//17 �����������(buff����63)  p1:buff_id   p2:����ֵϵ��  p3:�ƶ��˺�ϵ��  p4:buff�ȼ�������ֵ=p2*ʥ���ȼ�  �˺�ֵ=p3*ʥ���ȼ���
	HALLOW_EFFECT_ADD_DAMAGE_AND_HEAL_BUFF = 18, //18 ��ó����˺��������˻�ѪBUFF(buff����67)  p1:buff_id   p2:�˺�ֵϵ��  p3:buff�ȼ� �������˺�ֵ=p2*ʥ���ȼ�)
	HALLOW_EFFECT_ADD_MAXHP_PERCENT = 19,	//19 �����Ѫ���ٷֱȵ�Ѫ  p1:��Ѫ�ٷֱ�[0,100]
	HALLOW_EFFECT_MAX
};

#endif