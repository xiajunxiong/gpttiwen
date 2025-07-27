#ifndef __BATTLE_HALLOW_GIFT_DEF_HPP__
#define __BATTLE_HALLOW_GIFT_DEF_HPP__

enum HALLOW_GIFT_EFFECT_TYPE
{
	HALLOW_GIFT_EFFECT_TYPE_INVALID = 0,
	HALLOW_GIFT_EFFECT_TYPE_ADD_ATTR = 1,	//!< 1.�����Ե���Ч			
	HALLOW_GIFT_EFFECT_TYPE_TIMING = 2,		//!< 2.���ڴ���ʱ������Ч
	HALLOW_GIFT_EFFECT_TYPE_OTHER = 3,		//!< 3.��������Ч
	HALLOW_GIFT_EFFECT_TYPE_MAX,
};

enum HALLOW_GIFT_TIMING
{
	HG_TIMING_INVALID = 0,
	HG_TIMING_ROUND_START = 1,				//!< 1 �غϿ�ʼʱ
	HG_TIMING_BEING_KNOCK_OFF = 2,			//!< 2 ʥ�����䱦ʱ
	HG_TIMING_HALLOW_PERFORM_SKILL = 3,		//!< 3 ʥ����������ʱ 
	HG_TIMING_MASTER_SUFFER_INJURE = 4,		//!< 4 �����ܵ��˺�ʱ
	HG_TIMING_NUM,
};

enum HALLOW_GIFT_TIMING_EFFECT
{
	HG_TIMING_EFFECT_INVALID = 0,
	HG_TIMING_EFFECT_MASTER_ADD_HP = 1,		//!< 1 ���˻�Ѫ  p1:��Ѫ�ٷֱ�
	HG_TIMING_EFFECT_MASTER_ADD_MP = 2,		//!< 2 ���˻���  p1:�����ٷֱ�
	HG_TIMING_EFFECT_MASTER_ADD_HP_MP = 3,	//!< 3 ���˻�Ѫ�һ���  p1:��Ѫ�ٷֱ�  p2:�����ٷֱ�
	HT_TIMING_EFFECT_ADD_NEXT_ROUND_ATTR_ACTIVE = 4, //!< 4 �»غ�ʥ����Ծ����� p1:�ٷֱ�
	HT_TIMING_EFFECT_ADD_CUR_ROUND_ATTR_ACTIVE = 5,  //!< 5 ���غ�ʥ����Ծ����� p1:�ٷֱ�  p2:����Ѫ���ٷֱ�����ڸ�ֵ [0,100]
	HT_TIMING_EFFECT_REALIVE_ONE_ALLY = 6,	//!< 6 ����һ���ѷ���λ p1:��Ѫ��ϵ��   ��Ѫ��=p1*ʥ���ȼ�
	HG_TIMING_EFFECT_NUM,
};

enum HALLOW_GIFT_OTHER_EFFECT
{
	HG_OTHER_EFFECT_INVALID = 0,
	HG_OTHER_EFFECT_CAN_PERFORM_SKILL_IF_MASTER_DEAD = 1, //!< 1 ���������Կ��ͷż��� p1 ���ʰٷֱ�[0,100]
	HG_OTHER_EFFECT_NUM,
};

enum HALLOW_GIFT_ADD_ATTR_EFFECT
{
	HG_ADD_ATTR_INVALID = 0,
	HG_ADD_ATTR_PERCENT = 1,					//!< 1 �ٷֱȼ�����   p1 ʥ����������  p2 ʥ�������԰ٷֱ�
	HG_ADD_ATTR_BASE_ON_MASTER_ADD_POINT = 2,	//!< 2 �������˼ӵ����Զ�������  p1 ���˼ӵ���������  p2 �ӵ�������ߵ��� p3 ʥ����������  p4 ʥ����������ǧ�ֱ�
	HG_ADD_ATTR_VALUE = 3,						//!< 3 �������Ե���   p1 ʥ����������  p2 ʥ�������Ե���
	HG_ADD_ATTR_NUM
};

struct HallowTimingParam_RoundStart
{};

struct HallowTimingParam_HallowPerformSkill
{
	int hallow_id;
	int hallow_skill_id;
	int hallow_skill_lv;
};

struct HallowTimingParam_BeingKnockOff
{};

struct HallowTimingParam_MasterSufferInjure
{
	short deliver_grid_idx;
	int injure;
};

union HallowTimingParam
{
	HallowTimingParam_RoundStart round_start;
	HallowTimingParam_HallowPerformSkill hallow_perform_skill;
	HallowTimingParam_BeingKnockOff being_knock_off;
	HallowTimingParam_MasterSufferInjure master_suffer_injure;
};

#endif