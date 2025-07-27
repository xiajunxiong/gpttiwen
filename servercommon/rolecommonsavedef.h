#ifndef __ROLE_COMMON_SAVE_DEF_H__
#define __ROLE_COMMON_SAVE_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"

//会存库，添加要小心,不是直购别放这里，容量有限
enum ROLE_COMMON_SAVE_DATA_TYPE
{
	ROLE_COMMON_SAVE_DATA_TYPE_INVALID,
	ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI = 1,				//组合贺礼
	ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP = 2,			//时装商店
	ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE = 3,		//累计充值(神器惊世)
	ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU = 4,			//一元购
	ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI = 5,			//灵玉基金
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN = 6,		//神兽降临
	ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG = 7,				//福袋
	ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO = 8,			//财神聚宝
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG = 9,		//定制宝箱
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO = 10,		//专属礼包
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE = 11,		//限时宝盒
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI = 12,		//限时宝袋
	ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL = 13,		//神秘试炼
	ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT = 14,		//月度投资
	ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO	= 15,				//寻宝
	ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2 = 16,			//宠物修炼技能2
	ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN = 17,			//伙伴修炼技能
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY = 18,	//专属礼包-每日礼包
	ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE = 19,	//首充专属-成长助力
	ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD = 20,					//月卡/周卡/共用商城
	ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM = 21,					//赠送物品
	ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE = 22,			//夏日商城
	ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP = 23,			//超级折扣商城
	ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU = 24,			// 珍品商店
	ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH = 25,			// 福袋
	ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING = 26,			// 万灵共盈
	ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG = 27,			// 魂器天成
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET = 28,			// 山海初遇
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET = 29,			// 山海相逢
	ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO = 30,			//招财进宝
	ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE =31,		// 纷享同花礼
	ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG = 32,		// 超值纷享
	ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE = 33,		// 纷享加倍
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG = 34,			//山海宝箱
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_HE = 35,			//山海宝盒
	ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI = 36,			//山海宝袋
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2 = 37,		//定制宝箱2
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2 = 38,		//限时宝盒2
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2 = 39,		//限时宝袋2
	ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO = 40,			// 吉星高照
	ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT = 41,			// 周末礼包
	ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT = 42,			// 超值献礼
	ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI = 43,			// 寅虎问世
	ROLE_COMMON_SAVE_DATA_TYPE_YIN_HU_BAI_SUI = 44,			// 寅虎拜岁
	ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA = 45,			// 竞技月卡
	ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG = 46,	// 情缘时装
	ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN = 47,		// 妖行试炼
	ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT = 48,		// 异兽临世
	ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI = 49,		// 万灵筑礼
	ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG = 50,		// 锦衣天香
	ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO = 51,		// 元气烟火
	ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU = 52,		// 元气记事簿
	ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN = 53,		// 端阳自选
	ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX = 54,		// 幸运盲盒自选
	ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI = 55,		// 端阳福礼
	ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO = 56,		// 金龙聚宝
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE = 57,		// 神兽恩泽
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3 = 58,		// 限时宝盒3 四季花牌
	ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4 = 59,		// 限时宝盒4 四季宝盒
	ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP = 60,		// 宠物兑换商店
	ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI = 61,			// 七夕特惠
	ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3 = 62,	// 定制宝箱3 四季宝箱
	ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG = 63,		// 新服灵宠
	ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU = 64,			// 神赐华福
	ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI = 65,		// 天衣耀世
	ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE = 66,	// 弑神之塔战令
	ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN = 67,		// 追月商店
	ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI = 68,			// 单笔返利

	ROLE_COMMON_SAVE_DATA_TYPE_MAX,
};
//会存库，添加要小心,不是直购别放这里，容量有限

unsigned int GetRoleCommonSaveDataLengthByType(int save_type);

#pragma pack(push, 4)
//---------------------以下是存库结构，注意对齐--------------------------------------

const static int CMD_BUY_ITEM_MAX_NUM = 16;
const static int CMD_BUY_ITEM_WAIT_TIME = 120;
struct RoleCommonSaveGameItem  //游戏物品
{
	void Reset();

	unsigned short item_id;
	 short is_bind;
	int num;
};
//-----------------------直购功能
struct RoleCommonSaveCmdBuyItem
{
	void Reset();

	int param1;
	int param2;
	int param3;
	int param4;
	int cfg_vesion;
	int need_gold;	
	int rewards_num;
	unsigned int create_time;
	unsigned int ra_begin_timestamp;
	long long unique_id;
	int param5;
	unsigned char reserved_ch[12];
	RoleCommonSaveGameItem rewards_item[CMD_BUY_ITEM_MAX_NUM];
};
//-------------寻宝功能------------------
struct RoleCommonSaveXunBao
{
	void Reset();

	int draw_type;
	int xi_you_floor_curr_times;
	unsigned int free_time;
};

//-----------宠物修炼技能
struct RoleCommonSavePetTrainData2
{
	void Reset()
	{
		level = 0;
		reserve_ush = 0;
		exp = 0;
	}

	unsigned short		level;
	unsigned short		reserve_ush;
	int				exp;
};

//-----------伙伴修炼技能
struct RoleCommonSavePartnerTrainData
{
	void Reset()
	{
		level = 0;
		reserve_ush = 0;
		exp = 0;
	}

	unsigned short		level;
	unsigned short		reserve_ush;
	int				exp;
};

//-------------赠送------------------
struct RoleCommonSaveRoleGiveGiftData
{
	void Reset();

	int give_times;
};


union RoleCommonSaveData
{
	void Reset(int save_type);

	RoleCommonSaveCmdBuyItem  cmd_buy_item_data;
	RoleCommonSaveXunBao  xun_bao_data;
	RoleCommonSavePetTrainData2 pet_train_data2;
	RoleCommonSavePartnerTrainData partner_train_data;
	RoleCommonSaveRoleGiveGiftData role_give_gift_data;
};

typedef char RoleCommonSaveDataHex[sizeof(RoleCommonSaveData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCommonSaveDataHex) < 1024);

#ifdef _MSC_VER
static_assert(std::is_pod<RoleCommonSaveData>::value, "RoleCommonSaveData is not pod type");
#endif

class RoleCommonSaveDataParam
{
public:
	bool Serialize(TLVSerializer & outstream) const;
	bool Unserialize(TLVUnserializer & instream);

	struct DBRoleCommonSaveDataItem
	{
		DBRoleCommonSaveDataItem()
		{
			this->Reset(0);
		}

		void Reset(int save_type)
		{
			change_state = 0;
			save_data_type = ROLE_COMMON_SAVE_DATA_TYPE_INVALID;
			param1 = 0;
			param2 = 0;

			data.Reset(save_type);
		}

		char change_state;
		int save_data_type;
		int param1;
		int param2;

		RoleCommonSaveData data;
	};

	int count;
	DBRoleCommonSaveDataItem data_list[5000];
};

#pragma pack(pop)

#endif
