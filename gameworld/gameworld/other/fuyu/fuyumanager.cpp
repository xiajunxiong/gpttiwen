#include "fuyumanager.hpp"
#include "other/partner/partner.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gamelog.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/errornum.h"
#include "item/fuyu/fuyuitem.h"
#include "other/fuyu/fuyuconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/partner/partnerconfig.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/funopen/funopen.hpp"
#include "other/attributelist.hpp"

FuYuManager::FuYuManager():m_role_module_mgr(NULL), m_fuyu_wear_is_change(false)
{
	for (int i = 0; i < ARRAY_LENGTH(m_fuyu_old_state); ++i)
	{
		m_fuyu_old_state[i] = false;
	}
}

FuYuManager::~FuYuManager()
{
}

// bool FuYuManager::IsFlyUp(int partner_id)
// {
// 	return m_role_module_mgr->GetPartner()->GetPartnerSoarLevel(partner_id) != 0 ? true : false;
// }

void FuYuManager::WearInfoChange(int wear_index, int reason, bool is_notice)
{
	if (wear_index < 0 || wear_index >= ItemNamespace::MAX_FU_YU_GRID_NUM) return;

	m_fuyu_wear_is_change = true;
	m_fuyu_old_state[wear_index] = true;
	if (is_notice) this->SendSingleInfo(wear_index, reason);
}

void FuYuManager::WearInfoChange(PartnerWear partner_wear, int reason, bool is_notice)
{
	int wear_index = this->SwitchTo(partner_wear);
	if (wear_index < 0 || wear_index >= ItemNamespace::MAX_FU_YU_GRID_NUM) return;

	m_fuyu_wear_is_change = true;
	m_fuyu_old_state[wear_index] = true;
	if (is_notice) this->SendSingleInfo(partner_wear, reason);
}

void FuYuManager::GmPutFuYu(ItemID item_id, int sp_group)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return;
	const FuYuItem * fuyu_item = (const FuYuItem *)item_base;

	if (I_TYPE_FU_YU != fuyu_item->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	ItemDataWrapper compound_equip; compound_equip.Reset();
	compound_equip.item_id = item_id;
	compound_equip.num = 1;
	compound_equip.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&compound_equip.item_unique_id);
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		fuyu_item->RandomNetValueSpecial(m_role_module_mgr->GetRole(), &p_nvip, put_reason, sp_group);
		p_nvip.GetDataByItemType(fuyu_item->GetItemType(), compound_equip.param_data);
		compound_equip.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_FU_SHI, m_role_module_mgr->GetRole(), compound_equip.item_id, 0, __FUNCTION__, p_nvip.GetParamLog(fuyu_item->GetItemType()));
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutItemDataWrapper(compound_equip))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetKnapsack()->Put(compound_equip, PUT_REASON_GM);
}

int FuYuManager::SwitchTo(PartnerWear partner_wear)
{
	return (partner_wear.first - 1) * WEAR_FU_YU_WEAR_NUM + partner_wear.second;
}

FuYuManager::PartnerWear FuYuManager::SwitchTo(int index)
{
	int partner_id = (index / WEAR_FU_YU_WEAR_NUM) + 1;
	int wear_index = index % WEAR_FU_YU_WEAR_NUM;
	return std::make_pair(partner_id, wear_index);
}

ItemGridData * FuYuManager::GetFuYuWearNode(int wear_idx)
{
	if (wear_idx < 0 || wear_idx >= ItemNamespace::MAX_FU_YU_GRID_NUM) return NULL;

	ItemGridData* grid = &m_wear_fuyu_grid[wear_idx];
	if (NULL == grid || NULL == grid->param || grid->Invalid()) return NULL;

	return grid;
}

ItemGridData * FuYuManager::GetFuYuWearNode(PartnerWear partner_wear)
{
	if (m_role_module_mgr->GetPartner()->IsPartnerInvalid(partner_wear.first) || partner_wear.second < 0 || partner_wear.second >= WEAR_FU_YU_WEAR_NUM) return NULL;
	int wear_idx = this->SwitchTo(partner_wear);

	ItemGridData* grid = &m_wear_fuyu_grid[wear_idx];
	if (NULL == grid || NULL == grid->param || grid->Invalid()) return NULL;

	return grid;
}

void FuYuManager::UpdateTimeOut()
{
	std::vector<SPTimeOut> temp_timeout_vec;
	// 符玉背包
	for (int index = 0; index < ARRAY_LENGTH(m_wear_fuyu_grid); index++)
	{
		ItemGridData* grid_data = this->GetFuYuWearNode(index);
		if (NULL == grid_data) continue;

		if (grid_data->param->fu_yu_param.expired_timestamp > 0)
		{
			if (EngineAdapter::Instance().Time() >= (time_t)grid_data->param->fu_yu_param.expired_timestamp)
			{	// 如果已经超时直接清掉,不用记录了
				grid_data->param->fu_yu_param.sp_id = 0;
				grid_data->param->fu_yu_param.expired_timestamp = 0;
				continue;
			}

			temp_timeout_vec.push_back(SPTimeOut(grid_data->param->fu_yu_param.expired_timestamp, grid_data->item_id, grid_data->item_unique_id));
		}
	}

	// 背包物品栏
	for (int index = 0; index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; index++)
	{
		ItemGridData * grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index);
		if (NULL == grid_data) continue;

		const ItemBase* item_base = ITEMPOOL->GetItem(grid_data->item_id);
		if (NULL == item_base) continue;	
		if(item_base->GetItemSubType() != ItemNamespace::ITEM_SUB_TYPE_FU_YU || item_base->GetItemType() != I_TYPE_FU_YU) continue;

		if (grid_data->param->fu_yu_param.expired_timestamp > 0)
		{
			if (EngineAdapter::Instance().Time() >= (time_t)grid_data->param->fu_yu_param.expired_timestamp)
			{	// 如果已经超时直接清掉,不用记录了
				grid_data->param->fu_yu_param.sp_id = 0;
				grid_data->param->fu_yu_param.expired_timestamp = 0;
				m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
				continue;
			}

			temp_timeout_vec.push_back(SPTimeOut(grid_data->param->fu_yu_param.expired_timestamp, grid_data->item_id, grid_data->item_unique_id));
		}
	}

	if (!temp_timeout_vec.empty())
	{
		std::sort(temp_timeout_vec.begin(), temp_timeout_vec.end());
		for (size_t i = 0; i < temp_timeout_vec.size(); i++)
		{
			m_sp_timeout.push(temp_timeout_vec[i]);
		}
	}
}

void FuYuManager::SendAllInfo()
{
	static Protocol::SCAllFuYuInfo msg;
	msg.count = 0;

	for (int i = 0; i < ItemNamespace::MAX_FU_YU_GRID_NUM; i++)
	{
		ItemGridData* grid_data = this->GetFuYuWearNode(i);
		if (NULL == grid_data) continue;

		const ItemBase* item_base = ITEMPOOL->GetItem(grid_data->item_id);
		if (NULL == item_base) continue;

		FuYuItemInfo& info = msg.info_list[msg.count++];
		PartnerWear partner_wear = this->SwitchTo(i);
		info.partner_id = partner_wear.first;
		info.wear_idx = partner_wear.second;
		info.item_id = grid_data->item_id;
		info.num = grid_data->num;
		info.is_bind = grid_data->is_bind ? 1 : 0;
		info.has_param = false;
		info.invalid_time = grid_data->invalid_time;
		info.item_unique_id = grid_data->item_unique_id;

		if (NULL != grid_data->param && NULL != item_base) // 只发送有参数的
		{
			info.has_param = true;
			info.param_length = grid_data->param->GetDataByItemType(item_base->GetItemType(), info.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &msg);
}

void FuYuManager::SendSingleInfo(int wear_index, int reason)
{
	PartnerWear partner_wear = this->SwitchTo(wear_index);

	static Protocol::SCFuYuInfo msg;
	msg.reason = reason;
	msg.info.partner_id = partner_wear.first;
	msg.info.wear_idx = partner_wear.second;
	msg.info.item_id = 0;
	msg.info.num = 0;
	msg.info.is_bind = 0;
	msg.info.has_param = 0;
	msg.info.invalid_time = 0;
	msg.info.item_unique_id = 0;
	msg.info.param_length = 0;

	ItemGridData* grid_data = this->GetFuYuWearNode(wear_index);
	if (NULL != grid_data)
	{
		const ItemBase* item_base = ITEMPOOL->GetItem(grid_data->item_id);	
		
		msg.info.item_id = grid_data->item_id;
		msg.info.num = grid_data->num;
		msg.info.is_bind = grid_data->is_bind ? 1 : 0;
		msg.info.invalid_time = grid_data->invalid_time;
		msg.info.item_unique_id = grid_data->item_unique_id;

		if (NULL != grid_data->param && NULL != item_base) // 只发送有参数的
		{
			msg.info.has_param = 1;
			msg.info.param_length = grid_data->param->GetDataByItemType(item_base->GetItemType(), msg.info.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &msg);
}

void FuYuManager::SendSingleInfo(PartnerWear partner_wear, int reason)
{
	static Protocol::SCFuYuInfo msg;
	msg.reason = reason;
	msg.info.partner_id = partner_wear.first;
	msg.info.wear_idx = partner_wear.second;
	msg.info.item_id = 0;
	msg.info.num = 0;
	msg.info.is_bind = 0;
	msg.info.has_param = 0;
	msg.info.invalid_time = 0;
	msg.info.item_unique_id = 0;
	msg.info.param_length = 0;

	ItemGridData* grid_data = this->GetFuYuWearNode(partner_wear);
	if (NULL != grid_data)
	{
		const ItemBase* item_base = ITEMPOOL->GetItem(grid_data->item_id);

		msg.info.item_id = grid_data->item_id;
		msg.info.num = grid_data->num;
		msg.info.is_bind = grid_data->is_bind ? 1 : 0;
		msg.info.invalid_time = grid_data->invalid_time;
		msg.info.item_unique_id = grid_data->item_unique_id;

		if (NULL != grid_data->param && NULL != item_base) // 只发送有参数的
		{
			msg.info.has_param = 1;
			msg.info.param_length = grid_data->param->GetDataByItemType(item_base->GetItemType(), msg.info.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_role_module_mgr->GetRole()->GetNetId(), &msg);
}

void FuYuManager::Init(RoleModuleManager * mgr, const FuYuCommonParam & param, const std::vector<const ItemListParam::ItemListData*>& fuyu_item_list)
{
	m_role_module_mgr = mgr;
	m_param = param;

	for (size_t i = 0; i < fuyu_item_list.size(); i++)
	{
		const ItemListParam::ItemListData * item_data = fuyu_item_list[i];
		int index = item_data->index - ItemNamespace::FU_YU_GRID_INDEX_BEGIN;
		if (index < 0 || index >= ItemNamespace::MAX_FU_YU_GRID_NUM)
		{
			continue;
		}

		ItemGridData & grid = m_wear_fuyu_grid[index];
		grid.item_id = item_data->item_wrapper.item_id;
		grid.num = 1;
		grid.is_bind = (0 != item_data->item_wrapper.is_bind);
		grid.item_unique_id = item_data->item_wrapper.item_unique_id;
		grid.is_market_buy_flag = item_data->item_wrapper.is_market_buy_flag;
		grid.param = ItemBase::CreateParamFromParamData(item_data->item_wrapper.param_data);

//		FuYuParam & fuyu_param = grid.param->fu_yu_param;
		
		gamelog::g_log_fu_yu.printf(LL_INFO, "%s index[%d] item_id[%d] param[%s]", __FUNCTION__, index, grid.item_id, grid.param->GetParamLog(I_TYPE_FU_YU));

		if (0 == grid.item_unique_id)
		{
			long long item_unique_id = 0;
			UniqueIDGenerator::Instance()->GenUniqueID(&item_unique_id);
			if (0 != item_unique_id)
			{
				grid.item_unique_id = item_unique_id;

#ifdef _DEBUG
				gamelog::g_log_item_track.printf(LL_INFO, "%s assign unique_id[%lld] to role[%d,%s] item[%d] num[%d] from index[%d]", __FUNCTION__,
					item_unique_id, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					grid.item_id, grid.num, index);
#endif
			}
		}

		m_fuyu_old_state[index] = true;
	}
}

void FuYuManager::GetInitParam(FuYuCommonParam * param)
{
	*param = m_param;
}

void FuYuManager::GetChangeFuYuItemList(int * count, ItemListParam::ItemListData * save_list)
{
	int tmp_count = 0;

	for (int wear_index = 0; wear_index < ARRAY_LENGTH(m_wear_fuyu_grid); ++wear_index)
	{
		const ItemGridData & curr_fuyu = m_wear_fuyu_grid[wear_index];
		if (curr_fuyu.Invalid())
		{
			if (!m_fuyu_old_state[wear_index]) continue;

			save_list[tmp_count].change_state_item = structcommon::CS_DELETE;
		}
		else
		{
			m_fuyu_old_state[wear_index] ? save_list[tmp_count].change_state_item = structcommon::CS_UPDATE :
				save_list[tmp_count].change_state_item = structcommon::CS_INSERT;
		}

		save_list[tmp_count].index = wear_index + ItemNamespace::FU_YU_GRID_INDEX_BEGIN;
		ItemDataWrapper & item_data_wrapper = save_list[tmp_count].item_wrapper;
		const ItemGridData & grid_data = m_wear_fuyu_grid[wear_index];
		grid_data.WrapTo(&item_data_wrapper);

		if (!curr_fuyu.Invalid() && NULL != curr_fuyu.param)
		{
			gamelog::g_log_medal.printf(LL_INFO, "%s index[%d] item_id[%d] param[%s]", __FUNCTION__, wear_index, curr_fuyu.item_id, curr_fuyu.param->GetParamLog(I_TYPE_FU_YU));
		}

		++tmp_count;
	}

	*count = tmp_count;
}

void FuYuManager::ClearDirtyMark()
{
	if (!m_fuyu_wear_is_change) return;

	for (int i = 0; i < ARRAY_LENGTH(m_fuyu_old_state); ++i)
	{
		m_fuyu_old_state[i] = !m_wear_fuyu_grid[i].Invalid();
	}

	m_fuyu_wear_is_change = false;
}

void FuYuManager::ReCalcPartnerAttr(int partner_id, AttrCon & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		AttributeList attr;
		attr.Reset();

		std::vector<std::pair<int, std::pair<int, int> > > temp_spid_list;		// 临时存放特效[特效组][特效id][特效等级]
		for (int wear_index = 0; wear_index < WEAR_FU_YU_WEAR_NUM; wear_index++)
		{
			ItemGridData* grid_data = this->GetFuYuWearNode(PartnerWear(partner_id, wear_index));
			if(NULL == grid_data) continue;

			FuYuParam& fuyu_param = grid_data->param->fu_yu_param;

			for (int j = 0; j < fuyu_param.attr_num; j++)
			{
				attr.m_attrs[fuyu_param.base_attr_list[j].attr_type] += fuyu_param.base_attr_list[j].attr_value;
			}

			if(fuyu_param.expired_timestamp != 0) continue;

			std::vector<int> single_fuyu_sp_list;
			if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id))
				single_fuyu_sp_list.push_back(grid_data->param->fu_yu_param.sp_id);
			if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.extra_sp_id))
				single_fuyu_sp_list.push_back(grid_data->param->fu_yu_param.extra_sp_id);

			for (size_t sp_index = 0; sp_index < single_fuyu_sp_list.size(); sp_index++)
			{
				int sp_group = LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(single_fuyu_sp_list[sp_index]);
				int sp_level = LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(single_fuyu_sp_list[sp_index]);
				const FuYuSPCfg * sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(sp_group, sp_level);
				if (NULL == sp_cfg) continue;

				if (sp_cfg->limit_type == FU_YU_SP_PROF_LIMIT)
				{
					const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
					if (NULL == partner_cfg) continue;

					if ((partner_cfg->profession * PROFESSION_BASE) != sp_cfg->param1) continue;
				}
				else if (sp_cfg->limit_type == FU_YU_SP_PARNTER_LIMIT)
				{
					if (partner_id != sp_cfg->param1) continue;
				}
				UNSTD_STATIC_CHECK(FU_YU_SP_LIMIT_TYPE_MAX == 3);

				{
					bool is_same = false;
					for (size_t j = 0; j < temp_spid_list.size(); j++)
					{
						if (sp_group == temp_spid_list[j].first)
						{
							if (sp_level <= temp_spid_list[j].second.second)
							{
								is_same = true;
								break;
							}

							// 如果等级更高, 覆盖掉低等级的
							temp_spid_list[j] = std::make_pair(sp_group, std::make_pair(single_fuyu_sp_list[sp_index], sp_level));
							is_same = true;
							break;
						}
					}

					if (!is_same) temp_spid_list.push_back(std::make_pair(sp_group, std::make_pair(single_fuyu_sp_list[sp_index], sp_level)));
				}
			}
		}

		AttributeList temp_invalid_attr;	// 符玉系统不会有根据属性加属性的特效类型, 临时建一个没用的占位变量传进去
		for (size_t index = 0; index < temp_spid_list.size(); index++)
		{
			::GetSpecialEffectAttr(temp_spid_list[index].second.first, attr, temp_invalid_attr, m_role_module_mgr->GetRole()->GetLevel());
		}

		m_partner_attr_list.erase(partner_id);
		m_partner_attr_list.insert(std::make_pair(partner_id, attr));
	}

	std::map<int, AttributeList>::const_iterator it = m_partner_attr_list.find(partner_id);
	if (it != m_partner_attr_list.end())
	{
		for (int i = 0; i < BATTLE_ATTR_MAX; i++)
		{
			base_add.attr_list[i] += it->second.m_attrs[i];
		}
	}
}

void FuYuManager::ReCalcAllPartnerAttr()
{
	m_partner_attr_list.clear();

	for (int partner_id = 1; partner_id < PARTNER_NUM_MAX; partner_id++)
	{
		if(m_role_module_mgr->GetPartner()->IsPartnerInvalid(partner_id)) continue;

		AttrCon base_add;	// 一个没用的临时变量, 用于占位
		this->ReCalcPartnerAttr(partner_id, base_add, true);
	}
}

void FuYuManager::GetPassiveSkill(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list)
{
	std::vector<std::pair<int, std::pair<int, int> > > temp_spid_list;	// 临时存放特效[特效组][特效id][特效等级]
	for (int i = 0; i < WEAR_FU_YU_WEAR_NUM; i++)
	{
		ItemGridData* grid_data = this->GetFuYuWearNode(std::make_pair(partner_id, i));
		if (NULL == grid_data) continue;

		// 时间戳大于0表示特效在保留时间,不生效
		if(grid_data->param->fu_yu_param.expired_timestamp > 0)
			continue;

		std::vector<int> single_fuyu_sp_list;
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id))
			single_fuyu_sp_list.push_back(grid_data->param->fu_yu_param.sp_id);
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.extra_sp_id))
			single_fuyu_sp_list.push_back(grid_data->param->fu_yu_param.extra_sp_id);

		for (size_t sp_index = 0; sp_index < single_fuyu_sp_list.size(); sp_index++)
		{
			int sp_group = LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(single_fuyu_sp_list[sp_index]);
			int sp_level = LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(single_fuyu_sp_list[sp_index]);
			const FuYuSPCfg * sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(sp_group, sp_level);
			if (NULL == sp_cfg) continue;

			if (sp_cfg->limit_type == FU_YU_SP_PROF_LIMIT)
			{
				const PartnerBasicCfg * partner_cfg = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerBasicCfg(partner_id);
				if (NULL == partner_cfg) continue;

				if ((partner_cfg->profession * PROFESSION_BASE) != sp_cfg->param1) continue;
			}
			else if (sp_cfg->limit_type == FU_YU_SP_PARNTER_LIMIT)
			{
				if (partner_id != sp_cfg->param1) continue;
			}
			UNSTD_STATIC_CHECK(FU_YU_SP_LIMIT_TYPE_MAX == 3);

			{
				bool is_same = false;
				for (size_t j = 0; j < temp_spid_list.size(); j++)
				{
					if (sp_group == temp_spid_list[j].first)
					{
						if (sp_level <= temp_spid_list[j].second.second)
						{
							is_same = true;
							break;
						}

						// 如果等级更高, 覆盖掉低等级的
						temp_spid_list[j] = std::make_pair(sp_group, std::make_pair(single_fuyu_sp_list[sp_index], sp_level));
						is_same = true;
						break;
					}
				}

				if (!is_same) temp_spid_list.push_back(std::make_pair(sp_group, std::make_pair(single_fuyu_sp_list[sp_index], sp_level)));
			}
		}
	}

	for (size_t i = 0; i < temp_spid_list.size(); i++)
	{
		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = temp_spid_list[i].second.first;
		}
		else
		{
#ifdef _DEBUG
			assert(0);
#else
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num[%d] >= max_other_sp_num[%d] ", __FUNCTION__, *out_other_sp_num, max_other_sp_num);
#endif
			return;
		}
	}
}

void FuYuManager::Update(unsigned long interval, time_t now_second)
{
	if (!m_sp_timeout.empty())
	{
		SPTimeOut& time_out = m_sp_timeout.front();
		// 同一时间不会有太多的相同时间戳的记录, 所以不用循环
		if (time_out.timeout_timestamp <= now_second)
		{
			bool is_found = false;
			// 符玉背包
			for (int index = 0; !is_found && index < ARRAY_LENGTH(m_wear_fuyu_grid); index++)
			{
				ItemGridData* grid_data = this->GetFuYuWearNode(index);
				if (NULL == grid_data) continue;

				if (grid_data->item_id == time_out.item_id &&
					grid_data->item_unique_id == time_out.item_unique_id)
				{
					grid_data->param->fu_yu_param.sp_id = 0;
					grid_data->param->fu_yu_param.expired_timestamp = 0;
					this->WearInfoChange(index, FU_YU_SEND_REASON_SP_TIMEOUT);
					is_found = true;
					break;
				}
			}

			// 背包物品栏
			for (int index = 0; !is_found && index < ItemNamespace::ITEM_COLUMN_GRID_NUM_MAX; index++)
			{
				ItemGridData * grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index);
				if (NULL == grid_data) continue;

				if (grid_data->item_id == time_out.item_id &&
					grid_data->item_unique_id == time_out.item_unique_id)
				{
					grid_data->param->fu_yu_param.sp_id = 0;
					grid_data->param->fu_yu_param.expired_timestamp = 0;
					m_role_module_mgr->GetKnapsack()->SetItemDirty(ItemNamespace::ITEM_COLUMN_TYPE_ITEM, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
					is_found = true;
					break;
				}
			}

			// 无论是否找到,都删掉对应的信息记录
			m_sp_timeout.pop();
		}
	}

}

void FuYuManager::OnRoleLogin()
{
	this->UpdateTimeOut();
	this->ReCalcAllPartnerAttr();
	this->SendAllInfo();
}

void FuYuManager::OnClientReq(Protocol::CSFuYuReq * req)
{
	if (NULL == req) return;

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_FU_YU, true))
	{
		return;
	}

	switch (req->req_type)
	{
	case FU_YU_REQ_TYPE_INFO:
		{
			this->SendAllInfo();
		}
		break;
	case FU_YU_REQ_TYPE_PUT_ON:
		{
			this->PutOn(std::make_pair(req->param1, req->param2), req->param3, req->param4);
		}
		break;
	case FU_YU_REQ_TYPE_TAKE_OFF:
		{
			this->TakeOff(std::make_pair(req->param1, req->param2));
		}
		break;
	case FU_YU_REQ_TYPE_DECOMPOSE:
		{
			this->Decompose(req->param1, req->param2);
		}
		break;
	case FU_YU_REQ_TYPE_UPGRADE:
		{
			this->Upgrade(std::make_pair(req->param1, req->param2), (ItemID)req->param3, (req->param4 == 0 ? false : true));
		}
		break;
	case FU_YU_REQ_TYPE_UPGRADE_2:
		{
			this->Upgrade(req->param1, req->param2, (ItemID)req->param3, (req->param4 == 0 ? false : true));
		}
		break;
	case FU_YU_REQ_TYPE_CONVERT_WEAR:
		{
			this->ConvertToWear(std::make_pair(req->param1, req->param2), req->param3, req->param4);
		}
		break;
	case FU_YU_REQ_TYPE_CONVERT_BAG:
		{
			this->ConvertToBag(std::make_pair(req->param1, req->param2), req->param3, req->param4);
		}
		break;
	case FU_YU_REQ_TYPE_SUBLIME_WEAR:
		{
			this->SublimeToWear(std::make_pair(req->param1, req->param2), std::make_pair(req->param3, req->param4), std::make_pair(req->param5, req->param6));
		}
		break;
	case FU_YU_REQ_TYPE_SUBLIME_BAG:
		{
			this->SublimeToBag(std::make_pair(req->param1, req->param2), std::make_pair(req->param3, req->param4), std::make_pair(req->param5, req->param6));
		}
		break;
	}
}

void FuYuManager::PutOn(PartnerWear partner_wear, int column, int bag_index)
{
	if (m_role_module_mgr->GetPartner()->IsPartnerInvalid(partner_wear.first) || partner_wear.second < 0 || partner_wear.second >= WEAR_FU_YU_WEAR_NUM) return;

	// 如果已穿戴,先卸下来
	ItemGridData* grid_data = this->GetFuYuWearNode(partner_wear);
	if (NULL != grid_data)
	{
		this->TakeOff(partner_wear);
	}

	ItemDataWrapper item_data; item_data.Reset();
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridWrapData(column, bag_index, &item_data))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	
	const ItemBase * item_base = ITEMPOOL->GetItem(item_data.item_id);
	if (NULL == item_base) return;
	const FuYuItem * fuyu_item = (const FuYuItem *)item_base;

	if (I_TYPE_FU_YU != fuyu_item->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	int wear_idx = this->SwitchTo(partner_wear);
	grid_data = &m_wear_fuyu_grid[wear_idx];
	if (!grid_data->Invalid()) return;

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] ConsumeItemByIndex_Failed!! column[%d] index[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), column, bag_index);
		return;
	}

	grid_data->Set(item_data);
	this->WearInfoChange(partner_wear, FU_YU_SEND_REASON_PUT_ON);
	m_role_module_mgr->GetPartner()->ReCalcPartnerAttrByFuYu(partner_wear.first, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_FU_YU, ADD_ATTR);

	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] partner_id[%d] wear_index[%d] fuyu_item[%s] TakeOff Succ.", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), partner_wear.first, partner_wear.second, grid_data->param->GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::TakeOff(PartnerWear partner_wear)
{
	ItemGridData* grid_data = this->GetFuYuWearNode(partner_wear);
	if (NULL == grid_data) return;

	ItemDataWrapper item; item.Reset();
	if (!grid_data->WrapTo(&item)) return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutItemDataWrapper(item, true, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_FU_YU_TAKE_OFF))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	std::string log_str = grid_data->param->GetParamLog(I_TYPE_FU_YU);

	grid_data->Clear();
	this->WearInfoChange(partner_wear, FU_YU_SEND_REASON_TAKE_OFF);
	m_role_module_mgr->GetPartner()->ReCalcPartnerAttrByFuYu(partner_wear.first, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_FU_YU, SUB_ATTR);

	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] partner_id[%d] wear_index[%d] fuyu_item[%s] TakeOff Succ.", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), partner_wear.first, partner_wear.second, log_str.c_str());
}

void FuYuManager::Decompose(int column, int bag_index)
{
	const ItemGridData* target_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData(column, bag_index);
	if (NULL == target_grid) return;

	const ItemBase * item_base = ITEMPOOL->GetItem(target_grid->item_id);
	if (NULL == item_base) return;

	if (I_TYPE_FU_YU != item_base->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	FuYuParam& target_fuyu_param = target_grid->param->fu_yu_param;

	const FuYuDecomposeCfg* decompose_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetDecomposeCfg(item_base->GetItemId());
	if (NULL == decompose_cfg) return;

	if (m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
	{
		int add_money = 0;
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_fuyu_param.extra_sp_id))
		{
			if (LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) == HUA_SHEN_FU_YU_EXTRA_SP_LEVEL)
			{
				add_money = decompose_cfg->huashen_fuyu_decompose;
			}
			else
			{
				add_money = decompose_cfg->hunyuan_fuyu_decompose;
			}
		}
		else
		{
			add_money = decompose_cfg->common_fuyu_decompose;
		}

		bool ret = m_role_module_mgr->GetMoney()->AddOtherCoin(add_money, MONEY_TYPE_SILVER_COIN, __FUNCTION__);

		gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] consume_item[item_id:%d {column[%d] bag_index[%d]}] add_is_succ[%s] add_lingyu[%d]", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), item_base->GetItemId(), column, bag_index, ret ? "Success" : "Failed", add_money);
	}
	else
	{
		gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] consume_item[item_id:%d {column[%d] bag_index[%d]}] ConsumeItemByIndex Failed...", __FUNCTION__,
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), item_base->GetItemId(), column, bag_index);
	}
}

void FuYuManager::Upgrade(PartnerWear partner_wear, ItemID consume_id, bool is_protect)
{
	ItemGridData* grid_data = this->GetFuYuWearNode(partner_wear);
	if (NULL == grid_data) return;

	const ItemBase * old_item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == old_item_base) return;
	const FuYuItem * old_fuyu_item = (const FuYuItem *)old_item_base;

	// 获取当前符玉等级
	int cur_fuyu_level = old_fuyu_item->GetFuYuLevel();

	if (cur_fuyu_level >= MAX_FU_YU_RANK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_ALREADY_MAX_LEVEL);
		return;
	}

	const std::pair<int, int>* protect_consume = LOGIC_CONFIG->GetFuYuConfig()->GetProtectCfg(cur_fuyu_level);

	{// 道具参数判断
		if (!m_role_module_mgr->GetKnapsack()->HasItem(consume_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
			return;
		}

		if (is_protect && (NULL == protect_consume || !m_role_module_mgr->GetKnapsack()->HasItem((ItemID)protect_consume->first, protect_consume->second)))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_NOT_PROTECT_ITEM);
			return;
		}
	}

	// 消耗材料等级
	int materials_level = LOGIC_CONFIG->GetFuYuConfig()->GetLevelByMaterials(consume_id);
	if (materials_level <= 0) return;

	// 不可以用比符玉本身更高级的材料强化
	if (materials_level > cur_fuyu_level)
	{
		return;
	}

	// 判断是否成功
	bool is_upgrade_succ = LOGIC_CONFIG->GetFuYuConfig()->IsStrengthenSucc(consume_id, cur_fuyu_level);

	// 获取强化行为后的等级
	if (is_upgrade_succ)
	{
		cur_fuyu_level += 1;		
	}
	else
	{
		if (is_protect && cur_fuyu_level > 1)
		{
			cur_fuyu_level -= 1;
		}
		else
		{
			cur_fuyu_level = 1;
		}
	}

	ItemID new_item_id = LOGIC_CONFIG->GetFuYuConfig()->GetNextIdByRank(cur_fuyu_level);
	const ItemBase * new_item_base = ITEMPOOL->GetItem(new_item_id);
	if (NULL == new_item_base) return;
	const FuYuItem * new_fuyu_item = (const FuYuItem *)new_item_base;

	// 关于特效的处理
	int new_sp_id = 0;
	uint32_t timeout_timestamp = 0;
	int sp_group = LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id) ?
		LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(grid_data->param->fu_yu_param.sp_id) : LOGIC_CONFIG->GetFuYuConfig()->RandSpGroup();
	
	const FuYuSPCfg * fuyu_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(sp_group, cur_fuyu_level);
	if (NULL != fuyu_cfg)
	{
		// 理论上这里应该处理超时队列, 删除对应的时间戳, 但是可以不做处理,等待时间到了队列无论是否找到都自行删除
		new_sp_id = fuyu_cfg->sp_id;
	}
	else
	{
		// 如果获取不到配置但特效却是有效的, 说明是掉段的符玉,并且还不到能获取特效的等级,直接继承之前的特效
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id) && grid_data->param->fu_yu_param.expired_timestamp > 0)
		{
			new_sp_id = grid_data->param->fu_yu_param.sp_id;
			timeout_timestamp = grid_data->param->fu_yu_param.expired_timestamp;
		}
	}	

	// 如果强化失败,并且失去了特效,保留最低等级特效并增加时间戳事件, 如果根据sp_group还是获取不到配置,说明没有可用的特效
	if (!is_upgrade_succ && 0 >= new_sp_id)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id))
		{
			const FuYuSPCfg * low_sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetLowSpIdByGroup(LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(grid_data->param->fu_yu_param.sp_id));
			if (NULL != low_sp_cfg)
			{
				new_sp_id = low_sp_cfg->sp_id;
				timeout_timestamp = static_cast<uint32_t>(EngineAdapter::Instance().Time()) + SECOND_PER_DAY;
			}
		}
	}

	{	// 消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_id, 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (is_protect && !m_role_module_mgr->GetKnapsack()->ConsumeItem((ItemID)protect_consume->first, protect_consume->second, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	// 道具已消耗,没有回头路了
	ItemDataWrapper new_data; new_data.Reset();
	new_data.item_id = new_item_id;
	new_data.num = 1;
	new_data.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&new_data.item_unique_id);
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		new_fuyu_item->RandomNetValueSpecial(m_role_module_mgr->GetRole(), &p_nvip, put_reason, sp_group);
		p_nvip.fu_yu_param.sp_id = new_sp_id;
		p_nvip.fu_yu_param.expired_timestamp = timeout_timestamp;

		p_nvip.GetDataByItemType(new_fuyu_item->GetItemType(), new_data.param_data);
		new_data.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_FU_YU, m_role_module_mgr->GetRole(), new_data.item_id, 0, p_nvip.GetParamLog(new_fuyu_item->GetItemType()), __FUNCTION__);
	}
	
	std::string log_str = grid_data->param->GetParamLog(I_TYPE_FU_YU);

	{// 重设新净值
		grid_data->Clear();
		grid_data->Set(new_data);

		if (timeout_timestamp != 0)
		{
			m_sp_timeout.push(SPTimeOut(grid_data->param->fu_yu_param.expired_timestamp, grid_data->item_id, grid_data->item_unique_id));
		}
	}

	this->WearInfoChange(partner_wear, is_upgrade_succ ? FU_YU_SEND_REASON_UPGRADE_SUCC : FU_YU_SEND_REASON_UPGRADE_FAIL);
	m_role_module_mgr->GetPartner()->ReCalcPartnerAttrByFuYu(partner_wear.first, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_FU_YU, ADD_ATTR);

	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] partner_id[%d] wear_index[%d] old_fuyu_item[%s] new_fuyu_item[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), partner_wear.first, partner_wear.second, log_str.c_str(), grid_data->param->GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::Upgrade(int column, int bag_index, ItemID consume_id, bool is_protect)
{
	ItemGridData * grid_data = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, bag_index);
	if (NULL == grid_data) return;

	const ItemBase * old_item_base = ITEMPOOL->GetItem(grid_data->item_id);
	if (NULL == old_item_base) return;
	const FuYuItem * old_fuyu_item = (const FuYuItem *)old_item_base;

	if (old_fuyu_item->GetItemColumn() != ItemNamespace::ITEM_COLUMN_TYPE_ITEM) return;

	if (I_TYPE_FU_YU != old_item_base->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 获取当前符玉等级
	int cur_fuyu_level = old_fuyu_item->GetFuYuLevel();

	if (cur_fuyu_level >= MAX_FU_YU_RANK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_ALREADY_MAX_LEVEL);
		return;
	}

	const std::pair<int, int>* protect_consume = LOGIC_CONFIG->GetFuYuConfig()->GetProtectCfg(cur_fuyu_level);

	{// 道具参数判断
		if (!m_role_module_mgr->GetKnapsack()->HasItem(consume_id, 1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_ITEM);
			return;
		}

		if (is_protect && (NULL == protect_consume || !m_role_module_mgr->GetKnapsack()->HasItem((ItemID)protect_consume->first, protect_consume->second)))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_NOT_PROTECT_ITEM);
			return;
		}
	}

	// 消耗材料等级
	int materials_level = LOGIC_CONFIG->GetFuYuConfig()->GetLevelByMaterials(consume_id);
	if (materials_level <= 0) return;

	// 不可以用比符玉本身更高级的材料强化
	if (materials_level > cur_fuyu_level)
	{
		return;
	}

	// 判断是否成功
	bool is_upgrade_succ = LOGIC_CONFIG->GetFuYuConfig()->IsStrengthenSucc(consume_id, cur_fuyu_level);

	// 获取强化行为后的等级
	if (is_upgrade_succ)
	{
		cur_fuyu_level += 1;		
	}
	else
	{
		if (is_protect && cur_fuyu_level > 1)
		{
			cur_fuyu_level -= 1;
		}
		else
		{
			cur_fuyu_level = 1;
		}
	}

	ItemID new_item_id = LOGIC_CONFIG->GetFuYuConfig()->GetNextIdByRank(cur_fuyu_level);
	const ItemBase * new_item_base = ITEMPOOL->GetItem(new_item_id);
	if (NULL == new_item_base) return;
	const FuYuItem * new_fuyu_item = (const FuYuItem *)new_item_base;

	// 关于特效的处理
	int new_sp_id = 0;
	uint32_t timeout_timestamp = 0;
	int sp_group = LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id) ?
		LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(grid_data->param->fu_yu_param.sp_id) : LOGIC_CONFIG->GetFuYuConfig()->RandSpGroup();

	const FuYuSPCfg * fuyu_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(sp_group, cur_fuyu_level);
	if (NULL != fuyu_cfg)
	{
		// 理论上这里应该处理超时队列, 删除对应的时间戳, 但是可以不做处理,等待时间到了队列无论是否找到都自行删除
		new_sp_id = fuyu_cfg->sp_id;
	}
	else
	{
		// 如果获取不到配置但特效却是有效的, 说明是掉段的符玉,并且还不到能获取特效的等级,直接继承之前的特效
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id) && grid_data->param->fu_yu_param.expired_timestamp > 0)
		{
			new_sp_id = grid_data->param->fu_yu_param.sp_id;
			timeout_timestamp = grid_data->param->fu_yu_param.expired_timestamp;
		}
	}	

	// 如果强化失败,并且失去了特效,保留最低等级特效并增加时间戳事件, 如果根据sp_group还是获取不到配置,说明没有可用的特效
	if (!is_upgrade_succ && 0 >= new_sp_id)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(grid_data->param->fu_yu_param.sp_id))
		{
			const FuYuSPCfg * low_sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetLowSpIdByGroup(LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(grid_data->param->fu_yu_param.sp_id));
			if (NULL != low_sp_cfg)
			{
				new_sp_id = low_sp_cfg->sp_id;
				timeout_timestamp = static_cast<uint32_t>(EngineAdapter::Instance().Time()) + SECOND_PER_DAY;
			}
		}
	}

	{	// 消耗
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_id, 1, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (is_protect && !m_role_module_mgr->GetKnapsack()->ConsumeItem((ItemID)protect_consume->first, protect_consume->second, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	}

	// 道具已消耗,没有回头路了
	ItemDataWrapper new_data; new_data.Reset();
	new_data.item_id = new_item_id;
	new_data.num = 1;
	new_data.is_bind = 0;
	UniqueIDGenerator::Instance()->GenUniqueID(&new_data.item_unique_id);
	static NetValueItemParam p_nvip; p_nvip.Reset();
	int put_reason = 0;
	{
		new_fuyu_item->RandomNetValueSpecial(m_role_module_mgr->GetRole(), &p_nvip, put_reason, sp_group);
		p_nvip.fu_yu_param.sp_id = new_sp_id;
		p_nvip.fu_yu_param.expired_timestamp = timeout_timestamp;

		p_nvip.GetDataByItemType(new_fuyu_item->GetItemType(), new_data.param_data);
		new_data.has_param = 1;
		ROLE_LOG_QUICK6(LOG_TYPE_FU_YU, m_role_module_mgr->GetRole(), new_data.item_id, 0, p_nvip.GetParamLog(new_fuyu_item->GetItemType()), __FUNCTION__);
	}

	std::string log_str = grid_data->param->GetParamLog(I_TYPE_FU_YU);

	{// 消耗掉原本的道具然后发新道具
		// 前面已经根据背包格子获取到了道具信息,不用判断HasItem
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, bag_index, 1, __FUNCTION__))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] ConsumeItemByIndex[colume:%d, index:%d] Failed!! param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), column, bag_index, log_str.c_str());
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
	
		if (!m_role_module_mgr->GetKnapsack()->Put(new_data, is_upgrade_succ ? PUT_REASON_FU_YU_UPGRADE_SUCC : PUT_REASON_FU_YU_UPGRADE_FAIL))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] Put Failed!! old_param[%s] new_param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), log_str.c_str(), p_nvip.GetParamLog(I_TYPE_FU_YU));
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		if (timeout_timestamp != 0)
		{
			m_sp_timeout.push(SPTimeOut(timeout_timestamp, new_data.item_id, new_data.item_unique_id));
		}
	}

	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d, %s] column[%d] bag_index[%d] old_fuyu_item[%s] new_fuyu_item[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), column, bag_index, log_str.c_str(), p_nvip.GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::SublimeToWear(PartnerWear target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu)
{
	// 要升华的天赐符玉
	ItemGridData* target_grid = this->GetFuYuWearNode(target_fuyu);
	if (NULL == target_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 被消耗的天赐符玉
	ItemGridData * consume_tc_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_tianci_fuyu.first, consume_tianci_fuyu.second);
	if (NULL == consume_tc_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 被消耗的普通符玉
	ItemGridData * consume_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_fuyu.first, consume_fuyu.second);
	if (NULL == consume_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(target_grid->item_id) ||
		!LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(consume_tc_grid->item_id) ||
		LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(consume_grid->item_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * consume_item_base = ITEMPOOL->GetItem(consume_grid->item_id);
	if (NULL == consume_item_base)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const FuYuItem * consume_fuyu_item = (const FuYuItem *)consume_item_base;
	if (I_TYPE_FU_YU != consume_fuyu_item->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (consume_fuyu_item->GetFuYuLevel() != MAX_FU_YU_RANK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_LEVEL_NOT_ENOUGH);
		return;
	}

	FuYuParam& target_fuyu_param = target_grid->param->fu_yu_param;
	FuYuParam& consume_tc_fuyu_param = consume_tc_grid->param->fu_yu_param;
	FuYuParam& consume_fuyu_param = consume_grid->param->fu_yu_param;

	// 这里做一些额外的可能多余的判断,以防以后因为策划修改符玉系统产生的bug
#if 1
	{
		if (target_fuyu_param.sp_id == 0
			|| consume_tc_fuyu_param.sp_id == 0
			|| consume_fuyu_param.sp_id == 0
			|| consume_fuyu_param.expired_timestamp != 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}
#endif
	
	// 化神符玉不可以继续升华了
	if ((target_fuyu_param.extra_sp_id != 0) &&
		(LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) == HUA_SHEN_FU_YU_EXTRA_SP_LEVEL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 被消耗天赐符玉不可以是混元/化神
	if (consume_tc_fuyu_param.extra_sp_id != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 需消耗同名天赐符玉
	if (target_fuyu_param.sp_id != consume_tc_fuyu_param.sp_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_LEVEL_NOT_ENOUGH);
		return;
	}
	
	bool is_hunyuan = (target_fuyu_param.extra_sp_id != 0);
	if (is_hunyuan && !(LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) == HUN_YUAN_FU_YU_EXTRA_SP_LEVEL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	
	int extra_sp_id = 0;
	if (is_hunyuan)
	{
		// 混元符玉升华需要消耗的符玉特效与其额外特效一致, 即同一个特效组
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_fuyu_param.extra_sp_id) !=
			LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(consume_fuyu_param.sp_id))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_SP_UNSAME);
			return;
		}

		extra_sp_id = consume_fuyu_param.sp_id;
	}
	else
	{
		const FuYuSPCfg * sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(
			LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(consume_fuyu_param.sp_id), HUN_YUAN_FU_YU_EXTRA_SP_LEVEL);
		if (NULL == sp_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		extra_sp_id = sp_cfg->sp_id;
	}

	if (0 == extra_sp_id) 
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	{	// 道具消耗掉之后下面不可以继续访问道具的指针了
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_tianci_fuyu.first, consume_tianci_fuyu.second, 1, __FUNCTION__))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] ConsumeItemByIndex[colume:%d, index:%d] Failed!! param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_tianci_fuyu.first, consume_tianci_fuyu.second, consume_tc_grid->param->GetParamLog(I_TYPE_FU_YU));
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_fuyu.first, consume_fuyu.second, 1, __FUNCTION__))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] ConsumeItemByIndex[colume:%d, index:%d] Failed!! param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_fuyu.first, consume_fuyu.second, consume_grid->param->GetParamLog(I_TYPE_FU_YU));
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}
	}

	int old_extra_sp_id = target_fuyu_param.extra_sp_id;

	target_fuyu_param.extra_sp_id = extra_sp_id;

	this->WearInfoChange(target_fuyu, FU_YU_SEND_REASON_SUBLIME);
	m_role_module_mgr->GetPartner()->ReCalcPartnerAttrByFuYu(target_fuyu.first, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_FU_YU, ADD_ATTR);
	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d. %s] target_old_extra_sp[%d] param{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_extra_sp_id, target_grid->param->GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::SublimeToBag(FuYuGrid target_fuyu, FuYuGrid consume_tianci_fuyu, FuYuGrid consume_fuyu)
{	
	// 要升华的天赐符玉
	ItemGridData* target_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(target_fuyu.first, target_fuyu.second);
	if (NULL == target_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 被消耗的天赐符玉
	ItemGridData * consume_tc_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_tianci_fuyu.first, consume_tianci_fuyu.second);
	if (NULL == consume_tc_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 被消耗的普通符玉
	ItemGridData * consume_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(consume_fuyu.first, consume_fuyu.second);
	if (NULL == consume_grid)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(target_grid->item_id) ||
		!LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(consume_tc_grid->item_id) ||
		LOGIC_CONFIG->GetFuYuConfig()->IsBelongFuYu(consume_grid->item_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const ItemBase * consume_item_base = ITEMPOOL->GetItem(consume_grid->item_id);
	if (NULL == consume_item_base)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const FuYuItem * consume_fuyu_item = (const FuYuItem *)consume_item_base;
	if (I_TYPE_FU_YU != consume_fuyu_item->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (consume_fuyu_item->GetFuYuLevel() != MAX_FU_YU_RANK_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_LEVEL_NOT_ENOUGH);
		return;
	}

	FuYuParam& target_fuyu_param = target_grid->param->fu_yu_param;
	FuYuParam& consume_tc_fuyu_param = consume_tc_grid->param->fu_yu_param;
	FuYuParam& consume_fuyu_param = consume_grid->param->fu_yu_param;

	// 这里做一些额外的可能多余的判断,以防以后因为策划修改符玉系统产生的bug
#if 1
	{
		if (target_fuyu_param.sp_id == 0
			|| consume_tc_fuyu_param.sp_id == 0
			|| consume_fuyu_param.sp_id == 0
			|| consume_fuyu_param.expired_timestamp != 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}
#endif

	// 化神符玉不可以继续升华了
	if ((target_fuyu_param.extra_sp_id != 0) &&
		(LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) == HUA_SHEN_FU_YU_EXTRA_SP_LEVEL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 被消耗天赐符玉不可以是混元/化神
	if (consume_tc_fuyu_param.extra_sp_id != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	// 需消耗同名天赐符玉
	if (target_fuyu_param.sp_id != consume_tc_fuyu_param.sp_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_SP_UNSAME);
		return;
	}

	bool is_hunyuan = (target_fuyu_param.extra_sp_id != 0);
	if (is_hunyuan && !(LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) == HUN_YUAN_FU_YU_EXTRA_SP_LEVEL))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int extra_sp_id = 0;
	if (is_hunyuan)
	{
		// 混元符玉升华需要消耗的符玉特效与其额外特效一致, 即同一个特效组
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_fuyu_param.extra_sp_id) !=
			LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(consume_fuyu_param.sp_id))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_SP_UNSAME);
			return;
		}

		extra_sp_id = consume_fuyu_param.sp_id;
	}
	else
	{
		const FuYuSPCfg * sp_cfg = LOGIC_CONFIG->GetFuYuConfig()->GetSpByGroup(
			LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(consume_fuyu_param.sp_id), HUN_YUAN_FU_YU_EXTRA_SP_LEVEL);
		if (NULL == sp_cfg)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		extra_sp_id = sp_cfg->sp_id;
	}

	if (0 == extra_sp_id) 
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	{	// 道具消耗掉之后下面不可以继续访问道具的指针了
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_tianci_fuyu.first, consume_tianci_fuyu.second, 1, __FUNCTION__))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] ConsumeItemByIndex[colume:%d, index:%d] Failed!! param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_tianci_fuyu.first, consume_tianci_fuyu.second, consume_tc_grid->param->GetParamLog(I_TYPE_FU_YU));
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}

		if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(consume_fuyu.first, consume_fuyu.second, 1, __FUNCTION__))
		{
			gamelog::g_log_fu_yu.printf(LL_WARNING, "%s role[%d, %s] ConsumeItemByIndex[colume:%d, index:%d] Failed!! param[%s]", __FUNCTION__,
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), consume_fuyu.first, consume_fuyu.second, consume_grid->param->GetParamLog(I_TYPE_FU_YU));
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}
	}

	int old_extra_sp_id = target_fuyu_param.extra_sp_id;

	target_fuyu_param.extra_sp_id = extra_sp_id;
	m_role_module_mgr->GetKnapsack()->SetItemDirty(target_fuyu.first, target_fuyu.second, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE, PUT_REASON_FU_YU_SUBLIME);
	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d. %s] target_old_extra_sp[%d] param{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_extra_sp_id, target_grid->param->GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::ConvertToWear(PartnerWear target_fuyu, int target_sp_id1, int target_sp_id2)
{
	// 要升华的天赐符玉
	ItemGridData* target_grid = this->GetFuYuWearNode(target_fuyu);
	if (NULL == target_grid) return;

	FuYuParam& target_fuyu_param = target_grid->param->fu_yu_param;

	// 没有特效或特效无效就不给走下去了
	if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_fuyu_param.sp_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_CAN_NOT_SWITCH);
		return;
	}

	// 额外特效id是否有效, 判断是否是混元/化神符玉
	bool has_double_sp = LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_fuyu_param.extra_sp_id);

	// 特效保留阶段也不可转换
	if (target_fuyu_param.expired_timestamp != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_CAN_NOT_SWITCH);
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	// 未升华过的符玉不能同特效转换
	if (!has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_fuyu_param.sp_id) == LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_sp_id1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_SAME_SP_CAN_NOT_SWITCH);
			return;
		}
	}
	
	// 需要等级相同
	if (LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.sp_id) != LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) != LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	// 天赐符玉只能和天赐符玉转换, 普通符玉同理
	if(LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_fuyu_param.sp_id) ^/*异或*/ LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_fuyu_param.extra_sp_id) ^/*异或*/ LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(LOGIC_CONFIG->GetFuYuConfig()->GetOtherCfg().switch_consume_item, 1, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(LOGIC_CONFIG->GetFuYuConfig()->GetOtherCfg().switch_consume_item, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int old_sp_id = target_fuyu_param.sp_id;
	int old_extra_sp_id = target_fuyu_param.extra_sp_id;

	target_fuyu_param.sp_id = target_sp_id1;
	if (has_double_sp)
	{
		target_fuyu_param.extra_sp_id = target_sp_id2;
	}

	this->WearInfoChange(target_fuyu, FU_YU_SEND_REASON_SWITCH);
	m_role_module_mgr->GetPartner()->ReCalcPartnerAttrByFuYu(target_fuyu.first, true, Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_FU_YU, ADD_ATTR);
	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d. %s] target_old_sp[%d, %d] param{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_sp_id, old_extra_sp_id, target_grid->param->GetParamLog(I_TYPE_FU_YU));
}

void FuYuManager::ConvertToBag(FuYuGrid target_fuyu, int target_sp_id1, int target_sp_id2)
{	
	// 要升华的天赐符玉
	ItemGridData* target_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(target_fuyu.first, target_fuyu.second);
	if (NULL == target_grid) return;

	const ItemBase * item_base = ITEMPOOL->GetItem(target_grid->item_id);
	if (NULL == item_base) return;
	const FuYuItem * fuyu_item = (const FuYuItem *)item_base;

	if (I_TYPE_FU_YU != fuyu_item->GetItemType())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_USE_ITEM_ERROR);
		return;
	}

	FuYuParam& target_fuyu_param = target_grid->param->fu_yu_param;

	// 没有特效或特效无效就不给走下去了
	if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_fuyu_param.sp_id))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_CAN_NOT_SWITCH);
		return;
	}

	// 额外特效id是否有效, 判断是否是混元/化神符玉
	bool has_double_sp = LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_fuyu_param.extra_sp_id);

	// 特效保留阶段也不可转换
	if (target_fuyu_param.expired_timestamp != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_CAN_NOT_SWITCH);
		return;
	}

	if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (!LOGIC_CONFIG->GetFuYuConfig()->IsvalidSpId(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	// 未升华过的符玉不能同特效转换
	if (!has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_fuyu_param.sp_id) == LOGIC_CONFIG->GetFuYuConfig()->GetSpGroupBySpId(target_sp_id1))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FU_YU_SAME_SP_CAN_NOT_SWITCH);
			return;
		}
	}

	// 需要等级相同
	if (LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.sp_id) != LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_fuyu_param.extra_sp_id) != LOGIC_CONFIG->GetFuYuConfig()->GetSpLevelById(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	// 天赐符玉只能和天赐符玉转换, 普通符玉同理
	if (LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_fuyu_param.sp_id) ^/*异或*/ LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_sp_id1))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (has_double_sp)
	{
		if (LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_fuyu_param.extra_sp_id) ^/*异或*/ LOGIC_CONFIG->GetFuYuConfig()->IsBelongSpid(target_sp_id2))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
	}

	if (!m_role_module_mgr->GetKnapsack()->HasItem(LOGIC_CONFIG->GetFuYuConfig()->GetOtherCfg().switch_consume_item, 1, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(LOGIC_CONFIG->GetFuYuConfig()->GetOtherCfg().switch_consume_item, 1, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int old_sp_id = target_fuyu_param.sp_id;
	int old_extra_sp_id = target_fuyu_param.extra_sp_id;

	target_fuyu_param.sp_id = target_sp_id1;
	if (has_double_sp)
	{
		target_fuyu_param.extra_sp_id = target_sp_id2;
	}

	m_role_module_mgr->GetKnapsack()->SetItemDirty(target_fuyu.first, target_fuyu.second, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE, PUT_REASON_FU_YU_SWITCH);
	gamelog::g_log_fu_yu.printf(LL_INFO, "%s role[%d. %s] target_old_sp[%d, %d] param{%s}", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), old_sp_id, old_extra_sp_id, target_grid->param->GetParamLog(I_TYPE_FU_YU));
}



bool Protocol::SCFuYuInfo::Serialize(char * buffer, int buffer_size, int * out_length)
{
	bool result = true;

	result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

	result = result && WriteInt(reason, buffer, buffer_size, out_length);
	result = result && WriteInt(info.partner_id, buffer, buffer_size, out_length);
	result = result && WriteInt(info.wear_idx, buffer, buffer_size, out_length);
	result = result && WriteUShort(info.item_id, buffer, buffer_size, out_length);
	result = result && WriteShort(info.num, buffer, buffer_size, out_length);
	result = result && WriteChar(info.is_bind, buffer, buffer_size, out_length);
	result = result && WriteChar(info.has_param, buffer, buffer_size, out_length);
	result = result && WriteUInt(info.invalid_time, buffer, buffer_size, out_length);
	result = result && WriteLL(info.item_unique_id, buffer, buffer_size, out_length);
	if (0 != info.has_param)
	{
		result = result && WriteShort(info.param_length, buffer, buffer_size, out_length);
		result = result && WriteStrN(info.param_data, info.param_length, buffer, buffer_size, out_length);
	}

	return result;
}

bool Protocol::SCAllFuYuInfo::Serialize(char * buffer, int buffer_size, int * out_length)
{
	bool result = true;

	result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

	result = result && WriteInt(count, buffer, buffer_size, out_length);
	for (int i = 0; i < count && i < ARRAY_LENGTH(info_list); ++i)
	{
		const FuYuItemInfo & info = info_list[i];

		result = result && WriteInt(info.partner_id, buffer, buffer_size, out_length);
		result = result && WriteInt(info.wear_idx, buffer, buffer_size, out_length);
		result = result && WriteUShort(info.item_id, buffer, buffer_size, out_length);
		result = result && WriteShort(info.num, buffer, buffer_size, out_length);
		result = result && WriteChar(info.is_bind, buffer, buffer_size, out_length);
		result = result && WriteChar(info.has_param, buffer, buffer_size, out_length);
		result = result && WriteUInt(info.invalid_time, buffer, buffer_size, out_length);
		result = result && WriteLL(info.item_unique_id, buffer, buffer_size, out_length);
		if (0 != info.has_param)
		{
			result = result && WriteShort(info.param_length, buffer, buffer_size, out_length);
			result = result && WriteStrN(info.param_data, info.param_length, buffer, buffer_size, out_length);
		}
	}

	return result;
}