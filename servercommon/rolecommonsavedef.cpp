#include "servercommon/struct/structcommon.h"
#include "rolecommonsavedef.h"

void RoleCommonSaveData::Reset(int save_type)
{
	switch (save_type)
	{
		case ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE:
		case ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU:
		case ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG:
		case ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG :		//定制宝箱
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO:		//专属礼包
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE:		//限时宝盒
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI:		//限时宝袋
		case ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL:
		case ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY:
		case ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		case ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD:
		case ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH:
		case ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING:
		case ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE:
		case ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_HE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT:
		case ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT:
		case ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI:
		case ROLE_COMMON_SAVE_DATA_TYPE_YIN_HU_BAI_SUI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA:
		case ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN:
		case ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT:
		case ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU:
		case ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX:
		case ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE:
		case ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4:
		case ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU:
		case ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI:
		case ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE:
		case ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN:
			{
				cmd_buy_item_data.Reset();
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO:
			{
				xun_bao_data.Reset();
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2:
			{
				pet_train_data2.Reset();
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN:
			{
				partner_train_data.Reset();
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM:
			{
				role_give_gift_data.Reset();
			}
			break;
		default:
			{
				memset(this, 0, sizeof(*this));
			}
			break;
	}

	UNSTD_STATIC_CHECK(ROLE_COMMON_SAVE_DATA_TYPE_MAX == 69);
}

void RoleCommonSaveGameItem::Reset()
{
	item_id = 0; 
	is_bind = false; 
	num = 0;
}

void RoleCommonSaveCmdBuyItem::Reset()
{
	param1 = 0;
	param2 = 0;
	param3 = 0;
	param4 = 0;
	cfg_vesion = 0;
	need_gold = 0;
	rewards_num = 0;
	create_time = 0;
	ra_begin_timestamp = 0;
	unique_id = 0;
	param5 = 0;

	for (int i = 0; i < ARRAY_LENGTH(reserved_ch); ++i)
	{
		reserved_ch[i] = 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(rewards_item); ++i)
	{
		rewards_item[i].Reset();
	}
}

unsigned int GetRoleCommonSaveDataLengthByType(int save_type)
{
	unsigned int length = 0;
	switch (save_type)
	{
		case ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE:
		case ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU:
		case ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG:
		case ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG:		//定制宝箱
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO:		//专属礼包
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE:		//限时宝盒
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI:		//限时宝袋
		case ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL:
		case ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY:
		case ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE:
		case ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD:
		case ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH:
		case ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING:
		case ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE:
		case ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_HE:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT:
		case ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT:
		case ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI:
		case ROLE_COMMON_SAVE_DATA_TYPE_YIN_HU_BAI_SUI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA:
		case ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN:
		case ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT:
		case ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO:
		case ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU:
		case ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN:
		case ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX:
		case ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE:
		case ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4:
		case ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP:
		case ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3:
		case ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG:
		case ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU:
		case ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI:
		case ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE:
		case ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI:
		case ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN:
			{
				length = sizeof(RoleCommonSaveCmdBuyItem);
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO:
			{
				length = sizeof(RoleCommonSaveXunBao);
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2:
			{
				length = sizeof(RoleCommonSavePetTrainData2);
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN:
			{
				length = sizeof(RoleCommonSavePartnerTrainData);
			}
			break;
		case ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM:
			{
				length = sizeof(RoleCommonSaveRoleGiveGiftData);
			}
			break;
		default:
			{
				length = 0;
			}
			break;
	}
	UNSTD_STATIC_CHECK(ROLE_COMMON_SAVE_DATA_TYPE_MAX == 69);

	if (length > sizeof(RoleCommonSaveData))
	{
		length = 0;
	}

	return length;
}

bool RoleCommonSaveDataParam::Serialize(TLVSerializer & outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count && i < ARRAY_LENGTH(data_list); ++i)
	{
		const DBRoleCommonSaveDataItem & rank_item = data_list[i];

		ret = outstream.Push(rank_item.change_state) && outstream.Push(rank_item.save_data_type) && outstream.Push(rank_item.param1)
			&& outstream.Push(rank_item.param2);
		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetRoleCommonSaveDataLengthByType(rank_item.save_data_type);
		if (length > 0)
		{
			TLVSerializer role_data;
			role_data.Reset((void *)&rank_item.data, sizeof(rank_item.data));
			role_data.MoveCurPos(length);

			ret = outstream.Push(role_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

bool RoleCommonSaveDataParam::Unserialize(TLVUnserializer & instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count && i < ARRAY_LENGTH(data_list); ++i)
	{
		DBRoleCommonSaveDataItem & item = data_list[i];
		ret = instream.Pop(&item.change_state) && instream.Pop(&item.save_data_type) && instream.Pop(&item.param1)
			&& instream.Pop(&item.param2);

		if (!ret)
		{
			return false;
		}

		if (item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetRoleCommonSaveDataLengthByType(item.save_data_type);
		if (length > 0)
		{
			TLVUnserializer role_data;
			ret = instream.Pop(&role_data);

			if (role_data.Size() <= sizeof(item.data))
			{
				memcpy(&item.data, role_data.Ptr(), role_data.Size());
			}

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

void RoleCommonSaveXunBao::Reset()
{
	draw_type = 0;
	xi_you_floor_curr_times = 0;
	free_time = 0;
}

void RoleCommonSaveRoleGiveGiftData::Reset()
{
	give_times = 0;
}
