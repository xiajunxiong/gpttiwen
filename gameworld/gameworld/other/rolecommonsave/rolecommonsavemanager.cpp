#include "servercommon/struct/itemlistparam.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "gamelog.h"
#include "rolecommonsavemanager.hpp"

RoleCommonSaveManager::RoleCommonSaveManager()
{
	m_role_module_mgr = NULL;

	for (int i = 0; i < ARRAY_LENGTH(m_save_list); ++i)
	{
		m_save_list[i] = NULL;
	}

	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZU_HE_HE_LI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_FASHION_SHOP));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CUMULATIVE_RECHARGE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ONE_YUAN_GOU));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_LING_YU_JIN_JI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_JIANG_LIN));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BAG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CAI_SHEN_JU_BAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MYSTERIOUS_TRIAL));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XUN_BAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_PET_TRAIN_2));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_PARTNER_TRAIN));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZHUAN_SHU_LI_BAO_DAY));	
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUMMER_STORE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SUPER_OFF_SHOP));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZHEN_PIN_SHANG_PU));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_LUCK_BAG_BATCH));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_GONG_YING));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_HORCRUX_TIANCHENG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MET));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_MEET));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZHAOCAIJINBAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YUN_ZE_SLOT_MACHINE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CHAOZHI_FENXIANG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_COLORFUL_DOUBLE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_XIANG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_HE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHAN_HAI_BAO_DAI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_2));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_2));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_DAI_2));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_JIXING_GAOZHAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_WEEKEND_GIFT));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_CHAO_ZHI_GIFT));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YINHU_WENSHI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YIN_HU_BAI_SUI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_JINGJI_YUEKA));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_QING_YUAN_SHI_ZHUANG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YAO_XING_SHI_LIAN));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_GOD_BEAST_ADVENT));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_WAN_LING_ZHU_LI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_JIN_YI_TIAN_XIANG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_YAN_HUO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_YUAN_QI_JISHIBU));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_ZI_XUAN));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_LUCKY_BLIND_BOX));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DUAN_YANG_FU_LI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_JIN_LONG_JU_BAO));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHEN_SHOU_EN_ZE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_QI_XI_TE_HUI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_3));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIAN_SHI_BAO_HE_4));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_PET_EXCHANGE_SHOP));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DING_ZHI_BAO_XIANG_3));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_XIN_FU_LING_CHONG));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_SHEN_CI_HUA_FU));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_TIAN_YI_YAO_SHI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_NEW_COURAGE_CHALLENGE));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_DAN_BI_FAN_LI));
	this->Add(new RoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_ZHUI_YUE_SHANG_DIAN));
	
	UNSTD_STATIC_CHECK(69 == ROLE_COMMON_SAVE_DATA_TYPE_MAX);
}

RoleCommonSaveManager::~RoleCommonSaveManager()
{
	for (int i = 0; i < ARRAY_LENGTH(m_save_list); ++i)
	{
		if (NULL != m_save_list[i])
		{
			delete m_save_list[i];
			m_save_list[i] = NULL;
		}
	}
}

RoleCommonSave * RoleCommonSaveManager::GetRoleCommonSave(int save_type)
{
	if (save_type < 0 || save_type >= ARRAY_LENGTH(m_save_list))
	{
		assert(0);
		return NULL;
	}

	return m_save_list[save_type];
}

void RoleCommonSaveManager::Init(RoleModuleManager * mgr, const RoleCommonSaveDataParam & data_param)
{
	m_role_module_mgr = mgr;
	for (int i = 0; i < ARRAY_LENGTH(m_save_list); ++i)
	{
		RoleCommonSave * save = this->GetRoleCommonSave(i);
		if (save)
		{
			save->InitRoleModuleManager(mgr);
		}
	}

	for (int i = 0; i < data_param.count && i < ARRAY_LENGTH(data_param.data_list); ++i)
	{
		const RoleCommonSaveDataParam::DBRoleCommonSaveDataItem & curr = data_param.data_list[i];
		RoleCommonSave * save = this->GetRoleCommonSave(curr.save_data_type);
		if (save)
		{
			RoleCommonSaveKey key(curr.param1, curr.param2);
			save->InitData(key, curr.data);
		}
	}
}

void RoleCommonSaveManager::GetInitParam(RoleCommonSaveDataParam * data_param, bool is_logout)
{
	data_param->count = 0;

	for (int i = 0; i < ARRAY_LENGTH(m_save_list); ++i)
	{
		RoleCommonSave * save = this->GetRoleCommonSave(i);
		if (save)
		{
			int old_count = data_param->count;
			save->GetChangeDataList(*data_param);
			int add_num = data_param->count - old_count;

			if (add_num > 0)
			{
				gamelog::g_log_role_common_save.printf(LL_INFO, "%s line:%d | role[%d %s] save_type:%d change_num:%d", __FUNCTION__, __LINE__,
								       m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), i, add_num);
			}
		}
	}
}

void RoleCommonSaveManager::ClearDirtyMark(const RoleCommonSaveDataParam & data_param)
{
	for (int i = 0; i < data_param.count && i < ARRAY_LENGTH(data_param.data_list); ++i)
	{
		const RoleCommonSaveDataParam::DBRoleCommonSaveDataItem & curr = data_param.data_list[i];
		RoleCommonSave * save = this->GetRoleCommonSave(curr.save_data_type);
		if (save)
		{
			save->ClearDirtyMark(curr);
		}
	}
}

void RoleCommonSaveManager::Gm(int op, int save_type, int p1, int p2)
{
	RoleCommonSave * cs = this->GetRoleCommonSave(save_type);
	if (cs == NULL)
	{
		return;
	}
	RoleCommonSaveKey key(p1, p2);

	if (op == 0)
	{
		RoleCommonSaveCmdBuyItem * data = cs->GetTypeData<RoleCommonSaveCmdBuyItem>(key, true);
		cs->SetDataChange(key);

		static int a = 1;		
		data->param1 = a;
		a += 1;
	}

	if (op == 1)
	{
		cs->RemoveData(key);
	}

	if (op == 2)
	{
		cs->ClearAllData();
	}
}

void RoleCommonSaveManager::Add(RoleCommonSave * role_common_save)
{
	if (role_common_save == NULL)
	{
		return;
	}

	int save_type = role_common_save->GetSaveType();
	if (save_type < 0 || save_type >= ARRAY_LENGTH(m_save_list))
	{
		printf("add common save fail,save_type:%d ", save_type);
		return;
	}

	if (m_save_list[save_type] != NULL)
	{
		printf("add common save fail,save_type:%d ", save_type);
		return;
	}

	m_save_list[save_type] = role_common_save;
}

std::string GetString(const RoleCommonSaveCmdBuyItem & item)
{
	std::string ret = STRING_SPRINTF("RoleCommonSaveCmdBuyItem[unique_id:%lld  cfg_vesion:%d  need_gold:%d create_time:%ud ra_begin_timestamp:%ud  p1:%d p2:%d p3:%d p4:%d p5:%d rewards_num:%d]  ",
					 item.unique_id, item.cfg_vesion, item.need_gold, item.create_time, item.ra_begin_timestamp, item.param1, item.param2, item.param3, item.param4, item.param5, item.rewards_num);

	for (int i = 0; i < item.rewards_num && i < ARRAY_LENGTH(item.rewards_item); ++i)
	{
		ret += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", item.rewards_item[i].item_id, item.rewards_item[i].num, item.rewards_item[i].is_bind);
	}

	return ret;
}


std::vector<ItemConfigData> BuyItemToItemConfigDataVec(int num, const RoleCommonSaveGameItem * item)
{
	std::vector<ItemConfigData> ret;
	if (item)
	{
		for (int i = 0; i < num && i < CMD_BUY_ITEM_MAX_NUM; ++i)
		{
			ItemConfigData t(item[i].item_id, item[i].is_bind, item[i].num);
			ret.push_back(t);
		}
	}
	return ret;
}

void ItemConfigDataVecToBuyItem(const std::vector<ItemConfigData> & item_config_data, RoleCommonSaveGameItem ARG_OUT * item, int ARG_OUT * num)
{
	if (item == NULL || num == NULL)
	{
		return;
	}
	
	*num = 0;
	for (int i = 0; i < (int)item_config_data.size() && i < CMD_BUY_ITEM_MAX_NUM; ++i)
	{
		const ItemConfigData & curr = item_config_data[i];
		item[i].item_id = curr.item_id;
		item[i].num = curr.num;
		item[i].is_bind = curr.is_bind;
		*num += 1;
	}
}