#ifndef __ZAO_HUA_EQUIP_DEF_H__
#define __ZAO_HUA_EQUIP_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#pragma pack(push, 4)


struct ZaoHuaRefineItemParam
{
	ZaoHuaRefineItemParam()
	{
		this->Reset();
	}

	void Reset()
	{
		refine_level = 0;
		reserved_sh = 0;
		reserved_int = 0;
	}

	short refine_level;
	short reserved_sh;
	int reserved_int;
};

struct ZaoHuaRefineParam
{
	ZaoHuaRefineParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ArrayCallReset(refine_info_list);
	}

	ZaoHuaRefineItemParam refine_info_list[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];
};

typedef char ZaoHuaRefineParamHex[sizeof(ZaoHuaRefineParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ZaoHuaRefineParamHex) < 1024);


const static int ZAO_HUA_LONG_ZHU_SLOT_NUM = 5;  //����ÿ����λӵ�еĿ�λ
const static int ZAO_HUA_LONG_ZHU_CI_TIAO_NUM = 10;  //����ÿ����λ ӵ�еĴ�������

struct ZaoHuaLongZhuCiTiaoInfo
{
	ZaoHuaLongZhuCiTiaoInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		ci_tiao_id = 0;
		ci_tiao_level = 0;
	}

	int ci_tiao_id;		//����id
	int ci_tiao_level;		//�����ȼ�
};

struct ZaoHuaLongZhuSlotParam
{
	ZaoHuaLongZhuSlotParam()
	{
		this->Reset();
	}

	void Reset()
	{
		long_zhu_level = 0;
		bao_di_ci_shu = 0;
		wear_ci_tiao.Reset();
		ArrayCallReset(ci_tiao_list);
	}

	ZaoHuaLongZhuCiTiaoInfo * LongZhuGetFreeCiTiao();
	ZaoHuaLongZhuCiTiaoInfo ClearCiTiao(int ci_tiao_index);
	void ClearAllCiTiao();

	unsigned short long_zhu_level;			//��ߵȼ�
	unsigned short bao_di_ci_shu;			//�������״���
	ZaoHuaLongZhuCiTiaoInfo wear_ci_tiao;
	ZaoHuaLongZhuCiTiaoInfo ci_tiao_list[ZAO_HUA_LONG_ZHU_CI_TIAO_NUM];  //10��,����һ�������У���10������
};

struct ZaoHuaLongZhuPartParam			//��λ��Ϣ
{
	ZaoHuaLongZhuPartParam()
	{
		this->Reset();
	}

	void Reset()
	{

		slot_unlock_flag = 0;
		ArrayCallReset(long_zhu_slot_list);
	}

	unsigned int slot_unlock_flag;		//��0bit ��4�� ��5��λ �Ƿ���������	
	ZaoHuaLongZhuSlotParam long_zhu_slot_list[ZAO_HUA_LONG_ZHU_SLOT_NUM];   //5����λ
};

struct ZaoHuaLongZhuParam
{
	ZaoHuaLongZhuParam()
	{
		this->Reset();
	}

	void Reset()
	{
		ArrayCallReset(long_zhu_info);
	}

	ZaoHuaLongZhuPartParam long_zhu_info[ItemNamespace::MAX_ZAO_HUA_EQUIPMENT_GRID_NUM];  //8����λ
};

typedef char ZaoHuaLongZhuParamHex[sizeof(ZaoHuaLongZhuParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ZaoHuaLongZhuParamHex) < 8192);
#pragma pack(pop)

#endif
