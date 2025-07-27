#ifndef __RA_ZHUI_YUE_SHANG_DIAN_PARAM_HPP__
#define __RA_ZHUI_YUE_SHANG_DIAN_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"
#pragma pack(push, 4)
//////////////////////////////////////////// 2223 随机活动 - 追月商店 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const static int ZHUI_YUE_SHANG_DIAN_SELECT_NUM_SELECT = 6;		//自选奖励数量
const static int ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM = 20;		//购买物品数量
const static int ZHUI_YUE_SHANG_DIAN_RECORD_NUM = 50;		//

struct RAZhuiYueShangDianItem
{
	RAZhuiYueShangDianItem() { this->Reset(); }
	void Reset()
	{
		is_select = 0;
		reserved_ch = 0;
		reserved_sh = 0;
		reward_group_id = 0;
		reward_seq = 0;
	}

	unsigned char is_select;
	unsigned char reserved_ch;
	unsigned short reserved_sh;
	int reward_group_id;
	int reward_seq;
};

struct RAZhuiYueShangDianDrawRecord
{
	RAZhuiYueShangDianDrawRecord() { this->Reset(); }
	void Reset()
	{
		time = 0;
		sort_param = 0;
		reward_index = 0;
	}

	unsigned int time;
	int sort_param;		//N连抽时间会相同，第二排序字段，客户端用
	int reward_index;
};


struct RAZhuiYueShangDianParam
{
	RAZhuiYueShangDianParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		ArrayCallReset(bao_di_info_list);
		add_record_times = 0;
		ArrayCallReset(record_list);
		ArrayCallReset(item_list_select);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];		//20 每种物品每天限购次数
	unsigned short ra_buy_times[ZHUI_YUE_SHANG_DIAN_MAX_BUY_ITEM];			//20 每种物品限购次数
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
	int add_record_times;
	RAZhuiYueShangDianDrawRecord record_list[ZHUI_YUE_SHANG_DIAN_RECORD_NUM];
	RAZhuiYueShangDianItem item_list_select[ZHUI_YUE_SHANG_DIAN_SELECT_NUM_SELECT];		//3 自选奖励列表
};

UNSTD_STATIC_CHECK(sizeof(RAZhuiYueShangDianParam) <= sizeof(RoleActivityRoleData))


#pragma pack(pop)


#endif	
