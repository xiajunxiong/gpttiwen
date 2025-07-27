#ifndef __RA_RAND_ACTIVITY_ROLE_PARAM_HPP__
#define __RA_RAND_ACTIVITY_ROLE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


#pragma pack(push, 4)
//////////////////////////////////////////// 2084 随机活动 - 财神聚宝  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_CAI_SHEN_JU_BAO_MAX_ITEM = 20;
const int static RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM >= RA_CAI_SHEN_JU_BAO_MAX_ITEM);
struct RACaiShenJuBaoParam
{
	RACaiShenJuBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_cai_shen_ju_bao_begin_timestamp = 0;
		
		for (int i = 0; i < RA_CAI_SHEN_JU_BAO_MAX_ITEM; ++i)
		{
			ra_cai_shen_ju_bao_buy_day_times[i] = 0;
			ra_cai_shen_ju_bao_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_cai_shen_ju_bao_begin_timestamp;
	unsigned short ra_cai_shen_ju_bao_buy_day_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_cai_shen_ju_bao_buy_times[RA_CAI_SHEN_JU_BAO_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_CAI_SHEN_JU_BAO_BUY_MAX_ITEM> buy_flag;										//购买标识 0:未购买 1:已购买
};

UNSTD_STATIC_CHECK(sizeof(RACaiShenJuBaoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2085 随机活动 - 定制宝箱  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_MAX_ITEM = 20;
const int static RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM >= RA_DING_ZHI_BAO_XIANG_MAX_ITEM);
struct RADingZhiBaoXiangParam
{
	RADingZhiBaoXiangParam() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_DING_ZHI_BAO_XIANG_BUY_MAX_ITEM> buy_flag;										//购买标识 0:未购买 1:已购买
	int has_free_reward;																		//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiangParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2086 随机活动 - 专属礼包  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM = 5;
const int static RA_ZHUAN_SHU_LI_BAO_CHONG_ZHI_RECORD_NUM= 5;
const int static RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_OLD_NUM = 8;		
const int static RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM = 5;

struct ZhuanShuLiBaoAciveItem
{
	ZhuanShuLiBaoAciveItem()
	{
		this->Reset();
	}

	void Reset()
	{
		is_active = 0;
		type = 0;
		buy_times = 0;
		end_time = 0;
	}

	short is_active;				//非0就是激活了
	short type;				//配置表type			
	int buy_times;				//购买次数
	unsigned int end_time;		//结束时间
};

//专属礼包-每日礼包(废弃)
struct ZhuanShuLiBaoDayOldItem
{
	ZhuanShuLiBaoDayOldItem() { this->Reset(); }

	bool IsInVaild()
	{
		return phase <= 0 || type < 0;
	}
	void Reset()
	{
		phase = 0;
		type = 0;
		buy_times = 0;
		reserve_sh = 0;
	}

	short phase;		//阶段
	short type;			//索引
	short buy_times;
	short reserve_sh;
};

//专属礼包-每日礼包
struct ZhuanShuLiBaoDayItem
{
	ZhuanShuLiBaoDayItem() { this->Reset(); }
	ZhuanShuLiBaoDayItem(const ZhuanShuLiBaoDayOldItem & day_item)
	{
		phase = day_item.phase;
		type = day_item.type;
		buy_times = day_item.buy_times;
		reserve_sh = day_item.reserve_sh;
	}

	bool IsInVaild()
	{
		return phase <= 0 || type < 0;
	}

	void Reset()
	{
		phase = 0;
		type = 0;
		buy_times = 0;
		reserve_sh = 0;
		end_timestamp = 0;
	}

	short phase;		//阶段
	short type;			//索引
	short buy_times;
	short reserve_sh;
	unsigned int end_timestamp;		//礼包结束时间戳
};

struct RAZhuanShuLiBaoParam
{
	RAZhuanShuLiBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		active_role_level = 0;
		for (int i = 0; i < ARRAY_LENGTH(active_item); ++i)
		{
			active_item[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(reserve_active_item); ++i)
		{
			reserve_active_item[i].Reset();
		}
		login_day = 0;
		last_phase = 0;
		check_init_next_zero_timestamp = 0;
		for (int i = 0; i < ARRAY_LENGTH(old_day_item); i++)
		{
			old_day_item[i].Reset();
		}
		for (int i = 0; i < ARRAY_LENGTH(new_day_item); i++)
		{
			new_day_item[i].Reset();
		}
		reserve_ch = 0;
		is_data_move_flag = 0;
		reserve_sh = 0;
	}

	unsigned int ra_begin_timestamp;
	int active_role_level;
	ZhuanShuLiBaoAciveItem active_item[RA_ZHUAN_SHU_LI_BAO_MAX_EXIST_ITEM];
	ZhuanShuLiBaoAciveItem reserve_active_item[15];		//保留,防止拓展
	int login_day;
	int last_phase;
	unsigned int check_init_next_zero_timestamp;		//用来防治未登录玩家初始化与daychange,导致天数直接为2
	ZhuanShuLiBaoDayOldItem old_day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_OLD_NUM];		//每日礼包
	ZhuanShuLiBaoDayItem new_day_item[RA_ZHUAN_SHU_LI_BAO_DAY_MAX_ITEM_NUM];
	char reserve_ch;
	char is_data_move_flag;				//每日任务旧数据迁移 0:未迁移 
	short reserve_sh;
			
};

UNSTD_STATIC_CHECK(sizeof(RAZhuanShuLiBaoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2087 随机活动 - 经验飙升 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RAJingYanBiaoShengParam
{
	RAJingYanBiaoShengParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		day_buy_times = 0;
		buy_times = 0;
	}

	unsigned int ra_begin_timestamp;
	int day_buy_times;
	int buy_times;
};

UNSTD_STATIC_CHECK(sizeof(RAJingYanBiaoShengParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2088 随机活动 - 限时宝盒  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_HE_MAX_ITEM);
struct RAXianShiBaoHeParam
{
	RAXianShiBaoHeParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_XIAN_SHI_BAO_HE_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHeParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2089 随机活动 - 限时宝袋  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_DAI_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_DAI_MAX_ITEM);
struct RAXianShiBaoDaiParam
{
	RAXianShiBaoDaiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		can_buy_seq = 0;
		has_free_reward = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_DAI_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	int can_buy_seq;							//购买过的礼包seq，只有购买过前一个，才能买后面
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_XIAN_SHI_BAO_DAI_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoDaiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2103 随机活动 - 神兽降临  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM = 20;

struct RAShenShouJiangLinParam
{
	RAShenShouJiangLinParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		for (int i = 0; i < RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_SHEN_SHOU_JIANG_SHOU_MAX_ITEM];				//每种物品限购次数
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAShenShouJiangLinParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2107 随机活动 - 春日登峰 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM = 16;
const int static RA_CHUN_RI_DENG_FENG_MAX_LEVEL = 6;

struct RAChunRiDengFengParam
{
	RAChunRiDengFengParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		layer = 0;
		explore_total_times = 0;
		explore_times = 0;
		get_item_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int layer;
	unsigned int explore_total_times;		//探索总次数							
	unsigned short explore_times;		//没到达第七层得次数，满足配置表得次数直接到第七层
	unsigned short get_item_flag;		//第7层道具只可获得一次，不可重复获得，记录第七层获取什么道具	
};


UNSTD_STATIC_CHECK(RA_CHUN_RI_DENG_FENG_GET_ITEM_NUM <= (int)(sizeof( ((RAChunRiDengFengParam *)0)->get_item_flag) * 8))

UNSTD_STATIC_CHECK(sizeof(RAChunRiDengFengParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2109 随机活动 - 组合贺礼 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_ZU_HE_HE_LI_MAX_ITEM = 60;

struct RAZuHeHeLiBuyTimes
{
	RAZuHeHeLiBuyTimes() { this->Reset(); }
	void Reset()
	{
		ra_day_buy_times = 0;
		ra_buy_times = 0;
	}

	unsigned short ra_day_buy_times;			//每种物品每天限购次数
	unsigned short ra_buy_times;				//每种物品限购次数
};

struct RAZuHeHeLiParam
{
	RAZuHeHeLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < RA_ZU_HE_HE_LI_MAX_ITEM; ++i)
		{
			buy_times[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	RAZuHeHeLiBuyTimes buy_times[RA_ZU_HE_HE_LI_MAX_ITEM];
};

UNSTD_STATIC_CHECK(sizeof(RAZuHeHeLiParam) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2112 随机活动 - 令牌天赐 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_MAX_ITEM = 32;

struct RALingPaiTianCiParam
{
	RALingPaiTianCiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2113 随机活动 - 时装商店 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_FASHION_SHOP_MAX_ITEM = 64;

struct RAFashionShopParam
{
	RAFashionShopParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < ARRAY_LENGTH(buy_times); ++i)
		{
			buy_times[i] = 0;
		}
	}

	unsigned int ra_begin_timestamp;
	unsigned int buy_times[RA_FASHION_SHOP_MAX_ITEM];
};

UNSTD_STATIC_CHECK(sizeof(RAFashionShopParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2114 随机活动 - 1元购 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

struct RAOneYuanGouParam
{
	RAOneYuanGouParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		buy_times = 0;
	}

	unsigned int ra_begin_timestamp;
	int buy_times;
};

UNSTD_STATIC_CHECK(sizeof(RAOneYuanGouParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2115 随机活动 - 灵玉基金  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_LING_YU_JI_JIN_MAX_ITEM = 20;			//不能超过32，RALingYuJinJiParam::fetch_flag
const int static RA_LING_YU_JI_JIN_MAX_PHASE = 8;			//不能超过32，RALingYuJinJiParam::active_flag

struct RALingYuJinJiParam
{
	RALingYuJinJiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		active_flag = 0;
		for (int i = 0; i < RA_LING_YU_JI_JIN_MAX_PHASE; ++i)
		{
			fetch_flag[i] = 0;
		}
	}

	unsigned int ra_begin_timestamp;
	int active_flag;				//激活哪个阶段的标志
	int fetch_flag[RA_LING_YU_JI_JIN_MAX_PHASE];		//每个阶段领取奖励标志
};

UNSTD_STATIC_CHECK(sizeof(RALingYuJinJiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2116 随机活动 - 心法无量(令牌天赐2) //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_2_MAX_ITEM = 32;

struct RALingPaiTianCi2Param
{
	RALingPaiTianCi2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCi2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2130 随机活动 - 赋世魂器(令牌天赐3) //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_LING_PAI_TIAN_CI_3_MAX_ITEM = 32;

struct RALingPaiTianCi3Param
{
	RALingPaiTianCi3Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		chong_zhi_num = 0;
		fetch_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned int chong_zhi_num;
	unsigned int fetch_flag;
};

UNSTD_STATIC_CHECK(sizeof(RALingPaiTianCi3Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2128 随机活动 - 珍品商铺  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_ZHEN_PIN_SHANG_PU_MAX_ITEM = 100;

struct RAZhenPinShangPuBuyTimes
{
	RAZhenPinShangPuBuyTimes() { this->Reset(); }
	void Reset()
	{
		type =0;
		buy_times = 0;
	}

	short type;  //配置表的type
	short buy_times;  //配置表的type
};

struct RAZhenPinShangPuParam
{
	RAZhenPinShangPuParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;

		for (int i = 0; i < ARRAY_LENGTH(ra_day_buy_times); ++i)
		{
			ra_day_buy_times[i].Reset();
		}

		for (int i = 0; i < ARRAY_LENGTH(ra_buy_times); ++i)
		{
			ra_buy_times[i].Reset();
		}
	}

	unsigned int ra_begin_timestamp;
	RAZhenPinShangPuBuyTimes ra_day_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];			//每种物品每天限购次数
	RAZhenPinShangPuBuyTimes ra_buy_times[RA_ZHEN_PIN_SHANG_PU_MAX_ITEM];				//每种物品限购次数
};

UNSTD_STATIC_CHECK(sizeof(RAZhenPinShangPuParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2161 随机活动 - 定制宝箱2  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM = 20;
const int static RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM >= RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM);
struct RADingZhiBaoXiang2Param
{
	RADingZhiBaoXiang2Param() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_2_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_DING_ZHI_BAO_XIANG_2_BUY_MAX_ITEM> buy_flag;										//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};
UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiang2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2162 随机活动 - 限时宝盒2 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_2_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_HE_2_MAX_ITEM);
struct RAXianShiBaoHe2Param
{
	RAXianShiBaoHe2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		has_free_reward = 0;

		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_2_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_2_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_2_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_XIAN_SHI_BAO_HE_2_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe2Param) <= sizeof(RoleActivityRoleData))
//////////////////////////////////////////// 2163 随机活动 - 限时宝袋2 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM = 20;
const int static RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM = 32;
UNSTD_STATIC_CHECK(RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM >= RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM);
struct RAXianShiBaoDai2Param
{
	RAXianShiBaoDai2Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		can_buy_seq = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_DAI_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}
		buy_flag.Reset();
	}

	unsigned int ra_begin_timestamp;
	int can_buy_seq;							//购买过的礼包seq，只有购买过前一个，才能买后面
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_DAI_2_MAX_ITEM];				//每种物品限购次数
	BitMap<RA_XIAN_SHI_BAO_DAI_2_BUY_MAX_ITEM> buy_flag;						//购买标识 0:未购买 1:已购买
	int has_free_reward;													//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoDai2Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2182 随机活动 - 熔炼有礼 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
namespace RARongLianYouLi
{
	static const int RONG_LIAN_YOU_LI_MAX_TASK_NUM = 12;		// 最大任务数量
	UNSTD_STATIC_CHECK(RONG_LIAN_YOU_LI_MAX_TASK_NUM % 2 == 0)

	static const int RONG_LIAN_YOU_LI_MAX_BITMAP = 32;

};

struct RARongLianYouLiParam
{
	RARongLianYouLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		total_task_finish = 0;
		reserved_sh = 0;
		ra_activity_end_stamp = 0;

		has_task_stage_reward.Reset();

		memset(task_score, 0, sizeof(task_score));
		memset(reward_receive_num, 0, sizeof(reward_receive_num));
	}

	unsigned int ra_begin_timestamp;
	unsigned int ra_activity_end_stamp;		// 活动结束时间戳，判断活动结束是否发玩家未领取奖励

	// 累计任务礼盒
	short total_task_finish;			// 总完成任务数量
	short reserved_sh;					// 预留
	BitMap<RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_BITMAP> has_task_stage_reward;	// 任务阶段奖励是否已领取

	// 列表任务
	short task_score[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];			// 任务进度[0-11]
	short reward_receive_num[RARongLianYouLi::RONG_LIAN_YOU_LI_MAX_TASK_NUM];	// 奖励已领取数量[0-11]
};
UNSTD_STATIC_CHECK(sizeof(RARongLianYouLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2191 随机活动 - 锦衣天香  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RAJinYiTianXiangParam
{
	RAJinYiTianXiangParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		get_reward_flag = 0;
		mark = 0;
		has_draw_value = 0;
	}

	unsigned int ra_begin_timestamp;
	int draw_times;
	unsigned int get_reward_flag;		
	int mark;
	int has_draw_value;
};
UNSTD_STATIC_CHECK(sizeof(RAJinYiTianXiangParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2195 随机活动 - 元气烟火  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
struct RAYuanQiYanHuoParam
{
	RAYuanQiYanHuoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		is_buy = 0;
		is_fetch_free_reward = 0;
		get_reward_flag = 0;
	}

	unsigned int ra_begin_timestamp;
	int is_buy;
	int is_fetch_free_reward;
	unsigned int get_reward_flag;
};
UNSTD_STATIC_CHECK(sizeof(RAYuanQiYanHuoParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2200 随机活动 - 端阳自选  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

const static int DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM = 32;
struct RADuanYangZiXuanParam
{
	RADuanYangZiXuanParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		is_buy_reward = 0;
		fetch_reward_flag = 0;
		ArrayFillZero(select_reward_seq);
	}

	unsigned int ra_begin_timestamp;
	int is_buy_reward;
	unsigned int fetch_reward_flag;
	unsigned char select_reward_seq[DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM];  //只是记录给客户端看选了哪个礼包
};

UNSTD_STATIC_CHECK((int)(sizeof(((RADuanYangZiXuanParam *)0)->fetch_reward_flag) * 8) <= DUAN_YANG_ZI_XUAN_BUY_ITEM_MAX_NUM)
UNSTD_STATIC_CHECK(sizeof(RADuanYangZiXuanParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2207 随机活动 - 端阳福礼  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const static int DUAN_YANG_FU_LI_BUY_ITEM_MAX_NUM = 20;

struct RADuanYangFuLiItem
{
	RADuanYangFuLiItem() { this->Reset(); }
	void Reset()
	{
		is_buy = 0;
		fetch_reward_times = 0;
	}

	short is_buy;		//是否购买了
	short fetch_reward_times;  //领取奖励次数
};

struct RADuanYangFuLiParam
{
	RADuanYangFuLiParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayCallReset(item_list);
	}

	unsigned int ra_begin_timestamp;
	RADuanYangFuLiItem item_list[DUAN_YANG_FU_LI_BUY_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RADuanYangFuLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2208 随机活动 - 金龙聚宝 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const static int JIN_LONG_JU_BAO_SELECT_NUM_SELECT = 4;		//自选奖励数量
const static int JIN_LONG_JU_BAO_SELECT_NUM_RAND = 8;		//非自选奖励数量
const static int JIN_LONG_JU_BAO_MAX_BUY_ITEM = 20;		//
const static int JIN_LONG_JU_BAO_RECORD_NUM = 20;		//
		
struct RAJinLongJuBaoItem
{
	RAJinLongJuBaoItem() { this->Reset(); }
	void Reset()
	{
		is_get = 0;
		is_select = 0;
		reserved_sh = 0;
		reward_group_id = 0;
		reward_seq = 0;
	}

	unsigned char is_get;
	unsigned char is_select;
	unsigned short reserved_sh;
	int reward_group_id;
	int reward_seq;
};

struct RAJinLongJuBaoRecord
{
	RAJinLongJuBaoRecord() { this->Reset(); }
	void Reset()
	{
		time = 0;
		times = 0;		
		item_id = 0;
		num = 0;
	}

	unsigned int time;
	int times;		//用来排序
	unsigned short item_id;
	unsigned short num;
};

struct RAJinLongJuBaoParam
{
	RAJinLongJuBaoParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		draw_times = 0;
		times_reward_fetch_flag = 0;
		ArrayCallReset(item_list_select);
		ArrayCallReset(item_list_rand);
		record_index = 0;
		ArrayCallReset(record_list);
		ArrayCallReset(bao_di_info_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];		//20 每种物品每天限购次数
	unsigned short ra_buy_times[JIN_LONG_JU_BAO_MAX_BUY_ITEM];			//20 每种物品限购次数
	int draw_times;						//   抽奖次数
	unsigned int times_reward_fetch_flag;					//   轮数奖励领取标志
	RAJinLongJuBaoItem item_list_select [JIN_LONG_JU_BAO_SELECT_NUM_SELECT] ;	//4 自选奖励列表
	RAJinLongJuBaoItem item_list_rand[JIN_LONG_JU_BAO_SELECT_NUM_RAND];		//8 非自选奖励列表
	int record_index;
	RAJinLongJuBaoRecord record_list[JIN_LONG_JU_BAO_RECORD_NUM];
	BaoDiInfoItem bao_di_info_list[BAO_DI_INFO_ITEM_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RADuanYangFuLiParam) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2210 随机活动 - 神兽恩泽 //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////

const static int SHEN_SHOU_EN_ZE_MAX_BUY_ITEM = 20;		//
const static int SHEN_SHOU_EN_ZE_RECORD_MAX_NUM = 30;		//

struct RAShenShouEnZeRecord
{
	RAShenShouEnZeRecord() { this->Reset(); }
	void Reset()
	{
		time = 0;
		times = 0;
		item_id = 0;
		num = 0;
	}

	unsigned int time;
	int times;		//用来排序
	unsigned short item_id;
	unsigned short num;
};

struct RAShenShouEnZeParam
{
	RAShenShouEnZeParam() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		ArrayFillZero(ra_day_buy_times);
		ArrayFillZero(ra_buy_times);
		draw_times = 0;
		is_select = 0;
		select_reward_seq = 0;
		record_index = 0;
		ArrayCallReset(record_list);
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];		//20 每种物品每天限购次数
	unsigned short ra_buy_times[SHEN_SHOU_EN_ZE_MAX_BUY_ITEM];			//20 每种物品限购次数
	int draw_times;						//   抽奖次数
	int is_select;					//是否选择了物品
	int select_reward_seq;				//选择物品的seq
	int record_index;
	RAShenShouEnZeRecord record_list[SHEN_SHOU_EN_ZE_RECORD_MAX_NUM];
};

UNSTD_STATIC_CHECK(sizeof(RAShenShouEnZeParam) <= sizeof(RoleActivityRoleData))



//////////////////////////////////////////// 2214 随机活动 - 限时宝盒3  (四季花牌)//////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_3_MAX_ITEM = 20;

struct RAXianShiBaoHe3Param
{
	RAXianShiBaoHe3Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;	
		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_3_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_3_MAX_ITEM];				//每种物品限购次数
	int has_free_reward;							//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe3Param) <= sizeof(RoleActivityRoleData))


//////////////////////////////////////////// 2215 随机活动 - 限时宝盒4  (四季宝盒)//////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_XIAN_SHI_BAO_HE_4_MAX_ITEM = 20;

struct RAXianShiBaoHe4Param
{
	RAXianShiBaoHe4Param() { this->Reset(); }
	void Reset()
	{
		ra_begin_timestamp = 0;
		for (int i = 0; i < RA_XIAN_SHI_BAO_HE_4_MAX_ITEM; ++i)
		{
			ra_day_buy_times[i] = 0;
			ra_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_begin_timestamp;
	unsigned short ra_day_buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];			//每种物品每天限购次数
	unsigned short ra_buy_times[RA_XIAN_SHI_BAO_HE_4_MAX_ITEM];				//每种物品限购次数
	int has_free_reward;							//免费宝箱，0-未领取，1-已领取
};

UNSTD_STATIC_CHECK(sizeof(RAXianShiBaoHe4Param) <= sizeof(RoleActivityRoleData))

//////////////////////////////////////////// 2216 随机活动 - 定制宝箱3 四季宝箱  //////////////////////////////////////////
////////////////////////////////////////////  服务器数据  //////////////////////////////////////////////////////
//empty

////////////////////////////////////////////  个人数据  //////////////////////////////////////////////////////
const int static RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM = 20;
struct RADingZhiBaoXiang3Param
{
	RADingZhiBaoXiang3Param() { this->Reset(); }
	void Reset()
	{
		ra_ding_zhi_bao_xiang_begin_timestamp = 0;
	
		for (int i = 0; i < RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM; ++i)
		{
			ra_ding_zhi_bao_xiang_day_buy_times[i] = 0;
			ra_ding_zhi_bao_xiang_buy_times[i] = 0;
		}

		has_free_reward = 0;
	}

	unsigned int ra_ding_zhi_bao_xiang_begin_timestamp;
	unsigned short ra_ding_zhi_bao_xiang_day_buy_times[RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM];		//每种物品每天限购次数
	unsigned short ra_ding_zhi_bao_xiang_buy_times[RA_DING_ZHI_BAO_XIANG_3_MAX_ITEM];		//每种物品限购次数
	int has_free_reward;								//免费宝箱，0-未领取，1-已领取
};
UNSTD_STATIC_CHECK(sizeof(RADingZhiBaoXiang2Param) <= sizeof(RoleActivityRoleData))

//看的眼花，新加的放独立文件里

#pragma pack(pop)

#endif	
