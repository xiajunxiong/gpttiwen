#ifndef __BATTLE_HALO_DEF_HPP__
#define __BATTLE_HALO_DEF_HPP__

enum HALO_RANGE_PRIORITY				//!< ע�����ö��ͬʱ��������������Χ������Ҫע�����ȼ�
{
	HALO_RANGE_PRIORITY_SELF = 0,				//!< ����
	HALO_RANGE_PRIORITY_VERTICAL = 1,			//!< ����
	HALO_RANGE_PRIORITY_STRONG_POINT = 2,		//!< ǿ����
	HALO_RANGE_PRIORITY_HORIZONTAL = 3,			//!< ����
	HALO_RANGE_PRIORITY_ALLY = 4,				//!< ����(�������ǣ�
	HALO_RANGE_PRIORITY_SIDE = 5,				//!< ����ȫ��

	HALO_RANGE_PRIORITY_NUM,
};

enum HALO_TARGET_SIDE
{
	HALO_TARGET_SIDE_ALLY = 0,
	HALO_TARGET_SIDE_ENEMY = 1,
};

struct HaloEffectParam
{
	HaloEffectParam() : param1(0), param2(0), param3(0), param4(0) { }

	int param1;
	int param2;
	int param3;
	int param4;
};

enum HALO_EFFECT_TYPE
{
	HALO_EFFECT_TYPE_INVALID = 0,
	HALO_EFFECT_TYPE_ADD_PHYSIC_INJURE_REDUCE_RATIO = 1,	//!< ���������˺�����X%  P1������ǧ�ֱ�
	HALO_EFFECT_TYPE_ADD_HEAL_EFFECT_RATIO = 2,				//!< ��������Ч��X%      P1������ǧ�ֱ�
	HALO_EFFECT_TYPE_ADD_DAMAGE_RATIO = 3,					//!< �����˺�X%			P1������ǧ�ֱ�	P2:����X%Ѫ����Ч(ǧ�ֱ�)(0�����Ӳ���2)
	HALO_EFFECT_TYPE_ADD_ANTI_CHAOS_RATIO = 4,				//!< ���ӻ��ҿ���X%      P1������ǧ�ֱ�
	HALO_EFFECT_TYPE_LOW_SPEED_RATIO = 5,					//!< �����ٶ�X%			P1������ǧ�ֱ�
	HALO_EFFECT_TYPE_ADD_DAMAGE_RATIO_BY_TYPE = 6,			//!< �����˺�X%			P1������ǧ�ֱ�	P2:\see BATTLE_DAMAGE_EFFECT_TYPE_MAX
	HALO_EFFECT_TYPE_ADD_CONTROL_HIT = 7,					//!< ���ӿ�������X%		P1������ǧ�ֱ�

	HALO_EFFECT_TYPE_MAX
};

#endif