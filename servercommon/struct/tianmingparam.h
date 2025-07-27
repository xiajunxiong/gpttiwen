#ifndef __TIAN_MING_PARAM_H__
#define __TIAN_MING_PARAM_H__

#include "servercommon/servercommon.h"

enum TIAN_MING_ADD_ATTR_SYS_TYPE
{
	TIAN_MING_ADD_ATTR_SYS_TYPE_INVALID  = 0 ,
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HU_JING = 1,				// ���λ���
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_HUA_DIAN = 2,				// ���λ���
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_JING_DAI = 3,				// ���ν���
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIANG_NANG = 4,				// ��������
	TIAN_MING_ADD_ATTR_SYS_TYPE_PEI_SHI_XIU_JIAN = 5,				// ���λ���
	TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIP_STRENGTHEN = 6,				// ����ȫ��װ��ǿ��
	TIAN_MING_ADD_ATTR_SYS_TYPE_GEM = 7,							// ����ȫ��ʯ
	TIAN_MING_ADD_ATTR_SYS_TYPE_EQUIPMENT = 8,						// ����ȫ��װ��
	TIAN_MING_ADD_ATTR_SYS_TYPE_MAX,
};

static const int MAX_TIAN_MING_LEVEL_COUNT = 401;					// ����ϵͳ���ȼ�400  

/////////////////////// [����] ///////////////////////

struct TianMingParam
{
	TianMingParam() { this->Reset(); }

	void Reset()
	{
		level = 0;
		exp = 0;
		reserve_ll = 0;

		memset(sys_level, 0, sizeof(sys_level));
	}

	int level;													// ����ϵͳ ��ǰ�ܵȼ�
	int exp;													// ����ϵͳ ��ǰ����

	long long reserve_ll;										// ���� , Ҳ����б����Ҫ

	int sys_level[TIAN_MING_ADD_ATTR_SYS_TYPE_MAX];				// ����ϵͳÿ������ĵ�ǰ�ȼ� , ������󷽱���չϵͳ�ӳ�����
};

typedef char TianMingParamHex[sizeof(TianMingParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(TianMingParamHex) < 128);


#endif	//__TIAN_MING_PARAM_H__