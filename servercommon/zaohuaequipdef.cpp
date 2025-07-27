#include "zaohuaequipdef.hpp"

ZaoHuaLongZhuCiTiaoInfo * ZaoHuaLongZhuSlotParam::LongZhuGetFreeCiTiao()
{
	if (wear_ci_tiao.ci_tiao_id == 0)
	{
		return &wear_ci_tiao;
	}

	for (int i = 0; i < ARRAY_LENGTH(ci_tiao_list); ++i)
	{
		ZaoHuaLongZhuCiTiaoInfo & curr = ci_tiao_list[i];
		if (curr.ci_tiao_id == 0)
		{
			return &curr;
		}
	}

	return NULL;
}

ZaoHuaLongZhuCiTiaoInfo ZaoHuaLongZhuSlotParam::ClearCiTiao(int ci_tiao_index)
{
	ZaoHuaLongZhuCiTiaoInfo ret;

	for (int i = ci_tiao_index; i < ARRAY_LENGTH(ci_tiao_list); ++i)
	{
		if (i == ci_tiao_index)
		{
			ret = ci_tiao_list[i];
			ci_tiao_list[i].Reset();
		}

		if (i > ci_tiao_index)
		{
			ci_tiao_list[i - 1] = ci_tiao_list[i];
			ci_tiao_list[i].Reset();
		}

	}

	return ret;
}

void ZaoHuaLongZhuSlotParam::ClearAllCiTiao()
{
	ArrayCallReset(ci_tiao_list);
}
