#ifndef __SKILL_LIST_PARAM_HPP__
#define __SKILL_LIST_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

static const int MAX_SKILL_NUM = 256;
static const int MAX_LIFE_SKILL_NUM = 50; 

enum LIFE_SKILL_TYPE
{
	LIFE_SKILL_TYPE_INVALID = -1,					//!< -1 ���Ϸ������������
	LIFE_SKILL_TYPE_MIN = 0,
	LIFE_SKILL_TYPE_DIG = LIFE_SKILL_TYPE_MIN,		//!< 0 �ھ�		p1:���ʰٷֱ�   p2:����   p3:���ʰٷֱ�  p4:����
	LIFE_SKILL_TYPE_LUMBERING,						//!< 1 ��ľ
	LIFE_SKILL_TYPE_PICK_FLOWER,					//!< 2 �ɻ�
	LIFE_SKILL_TYPE_LIGHT_ARMOUR,					//!< 3 ���		p1:���Ϸ�������	p3:����ӳɰٷֱȣ��ͣ�  p4:����ӳɰٷֱȣ��ߣ�
	LIFE_SKILL_TYPE_HEAVY_ARMOUR,					//!< 4 �ؼ�
	LIFE_SKILL_TYPE_QUILTED_ARMOUR,					//!< 5 ����
	LIFE_SKILL_TYPE_KNUCKLES,						//!< 6 ȭ��
	LIFE_SKILL_TYPE_SWORD,							//!< 7 ��
	LIFE_SKILL_TYPE_AXE,							//!< 8 ��ͷ
	LIFE_SKILL_TYPE_SPEAR,							//!< 9 ǹ
	LIFE_SKILL_TYPE_BOW,							//!< 10 ��
	LIFE_SKILL_TYPE_MAGICIAN_WAND,					//!< 11 ����
	LIFE_SKILL_TYPE_MISSIONARY_WAND,				//!< 12 ������
	LIFE_SKILL_TYPE_WIZARD_WAND,					//!< 13 ��ʦ��
	LIFE_SKILL_TYPE_SHIELD,							//!< 14 ����
	LIFE_SKILL_TYPE_HELMET,							//!< 15 �ؼ�ͷ��
	LIFE_SKILL_TYPE_LIGHT_CAP,						//!< 16 ���ͷ��
	LIFE_SKILL_TYPE_CLOTH_HOODS,					//!< 17 ����ͷ��
	LIFE_SKILL_TYPE_HEAVY_BOOTS,					//!< 18 �ؼ�սѥ
	LIFE_SKILL_TYPE_LIGHT_SHOES,					//!< 19 ���Ь��
	LIFE_SKILL_TYPE_CLOTH_SHOES,					//!< 20 ����Ь��

	LIFE_SKILL_TYPE_MAX,
};
UNSTD_STATIC_CHECK(LIFE_SKILL_TYPE_MAX <= MAX_LIFE_SKILL_NUM);

enum LIFE_SKILL_CLASS
{
	LIFE_SKILL_CLASS_INVALID = 0,

	LIFE_SKILL_CLASS_GATHER = 1,			//!< �ɼ���
	LIFE_SKILL_CLASS_CRAFTS = 2,			//!< ������

	LIFE_SKILL_CLASS_MAX
};

class SkillListParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct __changeskill
	{
		char index;
		char change_state;

		UInt16 skill_id;
		short skill_level;
		int skill_exp;
	};	

	int count;
	__changeskill skill_list[MAX_SKILL_NUM];
};

#pragma pack(push, 4)

struct LifeSkillInfo
{
	LifeSkillInfo() : skill_level(0), reserve_sh(0), skill_exp(0) {}

	short skill_level;
	short reserve_sh;
	int skill_exp;
};

struct SkillOtherParam
{
public:
	SkillOtherParam() { this->Reset(); }

	void Reset()
	{
		reserve_ll = reserve_ll2 = 0;
		memset(life_skill_list, 0, sizeof(life_skill_list));
		skill_uplevel_consume_coin_return_flag = 0;
		common_skill_reset_flag = 0;
		common_use_skill_num = 0;
		memset(common_use_skill_idx_list, 0, sizeof(common_use_skill_idx_list));
	}

	long long reserve_ll;
	long long reserve_ll2;

	LifeSkillInfo life_skill_list[MAX_LIFE_SKILL_NUM];

	char skill_uplevel_consume_coin_return_flag;
	char common_skill_reset_flag;
	short common_use_skill_num;
	short common_use_skill_idx_list[MAX_SKILL_NUM];
};
typedef char SkillOtherParamHex[sizeof(SkillOtherParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(SkillOtherParamHex) < 2048);

#pragma pack(pop)
#endif