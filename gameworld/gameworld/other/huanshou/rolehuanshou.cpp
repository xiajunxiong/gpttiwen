#include "rolehuanshou.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/huanshouconfig.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"

#include "protocol/msghuanshou.hpp"

#include "item/knapsack.h"
#include "obj/character/role.h"
#include "skill/skillpool.hpp"
#include "item/huanshouskill/huanshouskillitem.hpp"

#include "global/worldstatus/worldstatus.hpp"

#include "other/rolemodulemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "other/pet/pet.hpp"
#include "other/funopen/funopen.hpp"


RoleHuanShou::RoleHuanShou() : m_module_mgr(NULL), m_knapsack_is_change(false)
{
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_grid_list, 0, sizeof(m_grid_list));
}

RoleHuanShou::~RoleHuanShou()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL != m_grid_list[i])
		{
			this->ClearBagGrid(i);
		}
	}
}

void RoleHuanShou::Init(RoleModuleManager * module_mgr, int bag_list_num, const ItemListParam::ItemListData * bag_data_list[], const RoleHuanShouParam & param)
{
	m_module_mgr = module_mgr;
	m_param = param;

	for (int i = 0; i < bag_list_num && i < ARRAY_ITEM_COUNT(m_grid_list); i++)
	{
		if (NULL == bag_data_list[i]) continue;

		if (bag_data_list[i]->index < HUAN_SHOU_GRID_INDEX_BEGIN || bag_data_list[i]->index >= HUAN_SHOU_GRID_INDEX_END)
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[%s bag] role[%s %d] invalid index,  index[%d]", __FUNCTION__,
				m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRole()->GetUID(), bag_data_list[i]->index);
			continue;
		}
		int real_index = bag_data_list[i]->index - HUAN_SHOU_GRID_INDEX_BEGIN;
		if (NULL != m_grid_list[real_index])
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[%s bag Duplicate] role[%s %d] index[%d] real_index[%d] old_item_id[%d] new_item_id[%d] old_has_param[%d]",
				__FUNCTION__, m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRole()->GetUID(), bag_data_list[i]->index, real_index,
				(int)m_grid_list[real_index]->item_id, (int)bag_data_list[i]->item_wrapper.item_id,
				(int)(NULL != m_grid_list[real_index]->param));
			continue;
		}
		const ItemBase *itembase = ITEMPOOL->GetItem(bag_data_list[i]->item_wrapper.item_id);
		if (NULL == itembase || bag_data_list[i]->item_wrapper.num <= 0 || bag_data_list[i]->item_wrapper.num > itembase->GetPileLimit())
		{
			gamelog::g_log_serious_error.printf(LL_CRITICAL, "[%s bag Error][[%s %d] index:%d  real_index[%d] item_id:%d num:%d]", __FUNCTION__,
				m_module_mgr->GetRole()->GetName(), m_module_mgr->GetRole()->GetUID(), bag_data_list[i]->index, real_index,
				(int)bag_data_list[i]->item_wrapper.item_id, (int)bag_data_list[i]->item_wrapper.num);
			continue;
		}
	
		m_grid_list[real_index] = new ItemGridData();
		if (!m_grid_list[real_index]->Set(bag_data_list[i]->item_wrapper))
		{
			m_grid_list[real_index]->Clear();
			continue;
		}
		m_old_state[real_index] = true;
	}

}

void RoleHuanShou::GetInitParam(RoleHuanShouParam * param)
{
	*param = m_param;
}

void RoleHuanShou::GetChangeBagList(int * count, ItemListParam::ItemListData * save_list)
{
	if (!m_knapsack_is_change)
	{
		*count = 0;
		return;
	}
	int tmp_count = 0;
	for (int i = 0; tmp_count < *count && i < ARRAY_ITEM_COUNT(m_grid_list); i++)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
			{
				if (!m_old_state[i]) continue;

				save_list[tmp_count].change_state_item = structcommon::CS_DELETE;

				save_list[tmp_count].index = HUAN_SHOU_GRID_INDEX_BEGIN + i;
				save_list[tmp_count].item_wrapper.Reset();
				++tmp_count;

				gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s CS_DELETE role[%d,%s] item from index[%d]", __FUNCTION__,
					m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), i);

			}
			else
			{
				if (m_old_state[i])
				{
					save_list[tmp_count].change_state_item = structcommon::CS_UPDATE;

					gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s CS_UPDATE role[%d,%s] item_id[%d] num[%d] from  index[%d]", __FUNCTION__,
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, m_grid_list[i]->num, i);
				}
				else
				{
					save_list[tmp_count].change_state_item = structcommon::CS_INSERT;

					gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s CS_INSERT role[%d,%s] item_id[%d] num[%d] from index[%d]", __FUNCTION__,
						m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), m_grid_list[i]->item_id, m_grid_list[i]->num, i);
				}

				save_list[tmp_count].index = HUAN_SHOU_GRID_INDEX_BEGIN + i;
				m_grid_list[i]->WrapTo(&save_list[tmp_count].item_wrapper);
				++tmp_count;

			}
		}
	}
	*count = tmp_count;
}

void RoleHuanShou::Update(unsigned int now_second)
{
	this->OnCheckLianDan((unsigned int)EngineAdapter::Instance().Time());
}

void RoleHuanShou::OnRoleLogin()
{
	this->OnCheckLianDan((unsigned int)EngineAdapter::Instance().Time());
}

bool RoleHuanShou::Put(const ItemDataWrapper & item_wrapper, int put_reason, int * out_index)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item_wrapper.item_id);
	if (NULL == item_base || item_base->GetItemType() != I_TYPE_HUAN_SHOU_SKILL) return false;

	ItemDataWrapper tmp_item_wrapper(item_wrapper);
	if (0 == tmp_item_wrapper.item_unique_id)
	{
		UniqueIDGenerator::Instance()->GenUniqueID(&tmp_item_wrapper.item_unique_id);
	}

	if (PUT_REASON_MARKET == put_reason)	//市场购买的道具不可与其他来源道具堆叠
	{
		tmp_item_wrapper.is_market_buy_flag = 1;
	}

	int empty_grid_index = -1;
	static int real_put_index_list[ITEM_COLUMN_GRID_NUM_MAX] = { 0 };
	int real_put_index_count = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
		{
			if (-1 == empty_grid_index)
			{
				empty_grid_index = i;
			}
		}
		else if (m_grid_list[i]->CanBeMerge(true)) // 下面Merge里会去对比invalid_time
		{
			int rest_num = 0;
			int merged_gold_price = 0;
			int merge_result = m_grid_list[i]->Merge(tmp_item_wrapper, &rest_num, &merged_gold_price);
			if (ITEM_MERGE_COMPLETE == merge_result)
			{
				real_put_index_list[real_put_index_count] = i; // 已经放完 不需要检查count了
				++real_put_index_count;

				tmp_item_wrapper.num = 0;
				tmp_item_wrapper.gold_price = 0;
				break;
			}
			else if (ITEM_MERGE_NOT_COMPLETE == merge_result)
			{
				real_put_index_list[real_put_index_count] = i;
				++real_put_index_count;

				tmp_item_wrapper.num = rest_num;
				tmp_item_wrapper.gold_price -= merged_gold_price;
			}

			if (real_put_index_count >= MAX_HUAN_SHOU_GRID_NUM) return false;
		}
	}

	if (tmp_item_wrapper.num > 0) // 还有放剩的
	{
		if (this->IsInvalidBagGrid(empty_grid_index))
		{
			m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_BAG_NOT_SPACE);
			return false;
		}

		if (NULL == m_grid_list[empty_grid_index])
		{
			m_grid_list[empty_grid_index] = new ItemGridData();
		}
		m_grid_list[empty_grid_index]->Set(tmp_item_wrapper);

		if (real_put_index_count >= MAX_HUAN_SHOU_GRID_NUM) return false;

		real_put_index_list[real_put_index_count] = empty_grid_index;
		++real_put_index_count;
	}

	
	if (NULL != out_index)
	{
		*out_index = real_put_index_list[0];	//只传第一个
	}
	for (int i = 0; i < real_put_index_count; i++)
	{
		this->SetBagGridDrity(real_put_index_list[i], Knapsack::ITEM_CHANGE_NOTICE_REASON_PUT, true, put_reason);
	}

	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s role[%d, %s] item[id:%d num:%d name:%s] put_index[%d] put_reason[%d]", __FUNCTION__,
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_wrapper.item_id, item_wrapper.num, item_base->GetItemName(),
		real_put_index_list[0], put_reason);
	if (item_base->IsRecord())
	{
		char log_quick_str[64] = { 0 };
		SNPRINTF(log_quick_str, sizeof(log_quick_str), "item_put_%d", (int)put_reason);
		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_PUT, m_module_mgr->GetRole(), item_wrapper.item_id, item_wrapper.num, log_quick_str, NULL);
	}

	return true;
}

bool RoleHuanShou::ConsumeItem(ItemID item_id, int num, const char * reason)
{
	if (num <= 0) return false;

	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	static int index_list[ARRAY_ITEM_COUNT(m_grid_list)] = { 0 };
	int index_num = 0;
	int cur_own_count = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
		{
			continue;
		}

		if (m_grid_list[i]->item_id == item_id)
		{
			cur_own_count += m_grid_list[i]->num;
			index_list[index_num++] = i;

			if (cur_own_count >= num) break;
		}
	}
	//所需数量不足
	if (cur_own_count < num)
	{
		return false;
	}

	const char *item_param_string = NULL;
	int need_consume_num = num;
	for (int i = 0; i < index_num && i < ARRAY_ITEM_COUNT(index_list); ++i)
	{
		int index = index_list[i];
		if (NULL == m_grid_list[index])
		{
			continue;
		}

		ItemGridData* item_grid_data = m_grid_list[index];
		int cur_consume_num = (need_consume_num <= item_grid_data->num) ? need_consume_num : item_grid_data->num;
		need_consume_num -= cur_consume_num;

		if (NULL != item_grid_data->param)
		{
			item_param_string = item_grid_data->param->GetParamLog(item_base->GetItemType());
		}

		item_grid_data->num -= cur_consume_num;
		if (item_grid_data->num <= 0)
		{
#ifdef TRACK_ITEM_FLAG
			gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]", __FUNCTION__,
				m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_grid_data->item_id, item_grid_data->item_unique_id);
#endif
			this->ClearBagGrid(index);
			item_grid_data = NULL;
		}
		this->SetBagGridDrity(index, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME, true);
	}
	EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, num);

	if (item_base->IsRecord())
	{
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "[%s Succ][user[%s %d] item_name:%s item_id:%d consume_num:%d  reason:%s]", __FUNCTION__,
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), item_base->GetItemName(), (int)item_id, num, NULL == reason ? "" : reason);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), item_id, num, reason, item_param_string);
	}

	return true;
}

bool RoleHuanShou::ConsumeItemByIndex(int index, int num, const char * reason)
{
	if (this->IsInvalidBagGrid(index) || num <= 0) return false;
	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return false;

	ItemGridData* item_grid_data = m_grid_list[index];
	if (item_grid_data->num < num)
	{
		return false;
	}

	ItemID item_id = item_grid_data->item_id;
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base)
	{
		return false;
	}
	const char *item_param_string = NULL;
	if (NULL != item_grid_data->param)
	{
		item_param_string = item_grid_data->param->GetParamLog(item_base->GetItemType());
	}
	item_grid_data->num -= num;
	int remain_num = item_grid_data->num;
	bool is_bind = item_grid_data->is_bind;
	if (item_grid_data->num <= 0)
	{
#ifdef TRACK_ITEM_FLAG
		gamelog::g_log_item_track.printf(LL_INFO, "TRACK_ITEM_FLAG %s CLEAR_GRID role[%d,%s] item[%d] unique_id[%lld]", __FUNCTION__,
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), item_grid_data->item_id, item_grid_data->item_unique_id);
#endif
		this->ClearBagGrid(index);
		item_grid_data = NULL;
	}
	this->SetBagGridDrity(index, Knapsack::ITEM_CHANGE_NOTICE_REASON_CONSUME, true);
	EventHandler::Instance().OnConsumeItem(m_module_mgr->GetRole(), item_id, num);

	if (item_base->IsRecord())
	{
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "[%s Succ][user[%s %d] item_id:%d item_name:%s(%d) consume_num:%d remain_num:%d reason:%s]", __FUNCTION__,
			m_module_mgr->GetRole()->GetName(), UidToInt(m_module_mgr->GetRole()->GetUserId()), (int)item_id, item_base->GetItemName(), (int)is_bind, (int)num, (int)remain_num, NULL == reason ? "" : reason);

		ROLE_LOG_QUICK6(LOG_TYPE_ITEM_CONSUME, m_module_mgr->GetRole(), item_id, num, reason, item_param_string);
	}

	return true;
}

const ItemGridData * RoleHuanShou::GetItemGridData2ByBag(int index) const
{
	if (this->IsInvalidBagGrid(index)) return NULL;
	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return NULL;

	const ItemBase *item_base = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (NULL == item_base) return false;

	if (m_grid_list[index]->num <= 0 || m_grid_list[index]->num > item_base->GetPileLimit()) return NULL;

	return m_grid_list[index];
}

bool RoleHuanShou::GetItemGridWrapDataByBag(int index, ItemDataWrapper * out_wrapper) const
{
	if (this->IsInvalidBagGrid(index) || NULL == out_wrapper) return false;
	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid()) return false;

	const ItemBase *item_base = ITEMPOOL->GetItem(m_grid_list[index]->item_id);
	if (NULL == item_base) return false;

	if (m_grid_list[index]->num <= 0 || m_grid_list[index]->num > item_base->GetPileLimit()) return false;

	return m_grid_list[index]->WrapTo(out_wrapper);
}

int RoleHuanShou::GetSpaceGridIdx() const
{
	int valid_grid_idx = -1;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
		{
			valid_grid_idx = i;
			break;
		}
	}

	return valid_grid_idx;
}

bool RoleHuanShou::IsHasItemCount(int item_id, int num) const
{
	if(num <= 0) return false;

	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (NULL == item_base) return false;

	int cur_own_count = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
		{
			continue;
		}

		if (m_grid_list[i]->item_id == item_id)
		{
			cur_own_count += m_grid_list[i]->num;

			if (cur_own_count >= num) break;
		}
	}
	//所需数量不足
	if (cur_own_count < num)
	{
		return false;
	}

	return true;
}

bool RoleHuanShou::HasBagNum(int grid_num) const
{
	int last_num = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL != m_grid_list[i] && !m_grid_list[i]->Invalid())
		{
			continue;
		}
		last_num += 1;
	}

	return last_num >= grid_num;
}

void RoleHuanShou::SetBagGridDrity(int grid_index, int reason_type, bool to_notice_client, int put_reason)
{
	if (this->IsInvalidBagGrid(grid_index))
	{
		return;
	}

	m_dirty_mark[grid_index] = true;
	m_knapsack_is_change = true;
	if (to_notice_client)
	{
		this->OnSendBagSingleInfo(grid_index, reason_type, put_reason);
	}
}

void RoleHuanShou::ClearBagGrid(int index)
{
	// index的合法性由外部保证
	if (NULL == m_grid_list[index]) return;

	m_grid_list[index]->Clear();
	delete m_grid_list[index];
	m_grid_list[index] = NULL;
}

void RoleHuanShou::ClearBagDirtyMark()
{
	if (!m_knapsack_is_change) return;

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid())
			{
				m_old_state[i] = false;
			}
			else
			{
				m_old_state[i] = true;
			}
		}
	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	m_knapsack_is_change = false;
}

void RoleHuanShou::OnSendBagAllInfo()
{
	static Protocol::SCHuanShouBagInfoAck info;
	info.reserve_sh = 0;
	info.item_count = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_grid_list); ++i)
	{
		if (NULL == m_grid_list[i] || m_grid_list[i]->Invalid()) continue;

		const ItemGridData * item_grid_data = m_grid_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(item_grid_data->item_id);
		if (NULL == item_base) continue;

		InfoType2 & ptc_con = info.item_list[info.item_count++];
		ptc_con.item_id = item_grid_data->item_id;
		ptc_con.is_bind = (char)item_grid_data->is_bind;
		ptc_con.index = i;
		ptc_con.is_market_buy_flag = item_grid_data->is_market_buy_flag;
		ptc_con.reserve_ch = 0;
		ptc_con.num = item_grid_data->num;
		ptc_con.has_param = false;
		ptc_con.invalid_time = item_grid_data->invalid_time;
		ptc_con.item_unique_id = item_grid_data->item_unique_id;

		if (NULL != item_grid_data->param) // 只发送有参数的
		{
			ptc_con.has_param = true;
			ptc_con.param_length = item_grid_data->param->GetDataByItemType(item_base->GetItemType(), ptc_con.param_data);
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleHuanShou::OnSendBagSingleInfo(int index, short reason_type, short put_reason)
{
	if (this->IsInvalidBagGrid(index)) return;

	static Protocol::SCHuanShouBagSingleInfo info;
	info.reason = reason_type;
	info.put_reason = put_reason;
	info.index = static_cast<short>(index);
	if (NULL == m_grid_list[index] || m_grid_list[index]->Invalid())
	{
		info.item_id = 0;
		info.num = 0;
		info.is_bind = 0;
		info.has_param = 0;
		info.invalid_time = 0;
		info.item_unique_id = 0;
		info.is_market_buy_flag = 0;
		info.reserve_ch = 0;
	}
	else
	{
		info.item_id = m_grid_list[index]->item_id;
		info.num = m_grid_list[index]->num;
		info.is_bind = m_grid_list[index]->is_bind ? 1 : 0;
		info.has_param = (NULL != m_grid_list[index]->param);
		info.invalid_time = m_grid_list[index]->invalid_time;
		info.item_unique_id = m_grid_list[index]->item_unique_id;
		info.is_market_buy_flag = m_grid_list[index]->is_market_buy_flag;
		info.reserve_ch = 0;

		const ItemBase* item_base = ITEMPOOL->GetItem(info.item_id);
		if (NULL == item_base)
		{
			return;
		}
		info.param_length = m_grid_list[index]->param->GetDataByItemType(item_base->GetItemType(), info.param_data);
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

const AttributeList & RoleHuanShou::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attr_list.Reset();
		
		for(int i = 0; i < ARRAY_ITEM_COUNT(m_param.huan_shou_list); ++i)
		{
			//不附身基础属性也要加
			const RoleHuanShouData & huanshou_data = m_param.huan_shou_list[i];
			const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(i);
			if (NULL == base_cfg)
			{
				continue;
			}
			const HuanShouAttrGroupLevelCfg * level_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetAttrGroupLevelCfgByLevel(base_cfg->attr_group_id, huanshou_data.str_level);
			if (NULL == level_cfg)
			{
				continue;
			}

			for (int i = 0; i < level_cfg->attr_count && i < ARRAY_ITEM_COUNT(level_cfg->attr_list); ++i)
			{
				m_attr_list.AddAttrCfg(level_cfg->attr_list[i]);
			}

			//附身才加成以下
			if (m_param.fushen_id != i || !m_param.huan_shou_list[i].IsValid())
			{
				continue;
			}

			std::vector<SkillPro> skill_list;
			if (level_cfg->skill_id > 0)
			{
				skill_list.push_back(SkillPro(level_cfg->skill_id, level_cfg->skill_level));
			}

			int max_point_num = GetMin(ARRAY_LENGTH(huanshou_data.point_level_list), LOGIC_CONFIG->GetHuanShouConfig()->GetPointCount());
			for (int i = 0; i < max_point_num; ++i)
			{
				if (huanshou_data.point_level_list[i] <= 0) continue;

				const HuanShouPotentialPointLevelCfg * point_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetPointLevelCfg(base_cfg->potential_group_id, i,
					huanshou_data.point_level_list[i]);
				if (NULL == point_cfg)
				{
					continue;
				}

				if (HUAN_SHOU_POINT_TIAN_FU_TYPE_ATTR == point_cfg->tianfu_type)
				{
					AttrItemCfg attr_info(point_cfg->param1, point_cfg->param2);
					m_attr_list.AddAttrCfg(attr_info);
				}
				else if (HUAN_SHOU_POINT_TIAN_FU_TYPE_PASSIVE_SKILL == point_cfg->tianfu_type)
				{
					skill_list.push_back(SkillPro(point_cfg->param1, point_cfg->param2));
				}
			}

			this->GetPassiveSkillGridList(huanshou_data, skill_list, base_cfg->skill_lattice);

			for (int i = 0; i < (int)skill_list.size(); ++i)
			{
				const GamePassiveSkillCfg * paasive_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_list[i].skill_id);
				if (NULL == paasive_cfg)
				{
					continue;
				}
				int spid = paasive_cfg->sp_id;
				if (paasive_cfg->is_level_spid)
				{
					if (skill_list[i].skill_level >= 1 && skill_list[i].skill_level < ARRAY_LENGTH(paasive_cfg->level_spid_list))
					{
						spid = paasive_cfg->level_spid_list[skill_list[i].skill_level];
					}
				}
				GetSpecialEffectAttr(spid, m_attr_list, base_add, m_module_mgr->GetRole()->GetLevel(), huanshou_data.str_level);
			}
		}
	}
	base_add.Add(m_attr_list.m_attrs);
	return m_attr_list;
}

void RoleHuanShou::OnOpenHuanShouEgg(int bag_column, int bag_index)
{
	const ItemGridData * item_data = m_module_mgr->GetKnapsack()->GetItemGridData(bag_column, bag_index);
	if (NULL == item_data)
	{
		return;
	}

	int egg_item_id = item_data->item_id;
	const HuanShouEggOpenCfg * egg_open_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetHuanShouEggOpenCfg(egg_item_id);
	if (NULL == egg_open_cfg || egg_open_cfg->egg_id < 0 || egg_open_cfg->egg_id >= ARRAY_LENGTH(m_param.egg_cur_open_times_list))
	{
		return;
	}

	int & cur_open_egg_times = m_param.egg_cur_open_times_list[egg_open_cfg->egg_id];
	const HuanShouEggCfg * egg_cfg = NULL;
	if(egg_open_cfg->rare_guarantee > 0 && (cur_open_egg_times % egg_open_cfg->rare_guarantee) + 1 == egg_open_cfg->rare_guarantee)
	{
		//满足保底
		egg_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetRandRareHuanShouEggCfgByEgg(egg_open_cfg->egg_id);
	}
	else
	{
		egg_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetRandHuanShouEggCfgByEgg(egg_open_cfg->egg_id);
	}

	if (NULL == egg_cfg || this->IsInvalidId(egg_cfg->huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(egg_cfg->huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndex(bag_column, bag_index, 1, __FUNCTION__))
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[egg_cfg->huanshou_id];
	if (huan_shou_info.IsValid())
	{
		bool is_put_succ = false;
		ItemConfigData convert_item(base_cfg->convert_item_id, true, 1);
		if (m_module_mgr->GetKnapsack()->CheckForPut(base_cfg->convert_item_id, 1, false))
		{
			if (m_module_mgr->GetKnapsack()->Put(convert_item, PUT_REASON_HUAN_SHOU_ACTIVE_CONVERT))
			{
				is_put_succ = true;
			}
		}

		if (!is_put_succ)
		{
			m_module_mgr->GetKnapsack()->SendMail(&convert_item, 1);
		}

		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_ACTIVE_CONVERT, egg_cfg->huanshou_id);
	}
	else
	{
		huan_shou_info.str_level = 1;
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_ACTIVE, egg_cfg->huanshou_id);
		EventHandler::Instance().OnGetNewHuanShou(m_module_mgr->GetRole(), egg_cfg->huanshou_id);
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d] active", __FUNCTION__, m_module_mgr->GetUid(), 
			m_module_mgr->GetRoleName(), egg_cfg->huanshou_id);
	}

	cur_open_egg_times += 1;
	this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_RARE_COUNT, egg_open_cfg->egg_id, cur_open_egg_times);
	EventHandler::Instance().OnOpenHuanShouEgg(m_module_mgr->GetRole());
	if (egg_cfg->is_broadcast)
	{
		WorldStatus::Instance().AddHuanShouAnnInfo(m_module_mgr->GetRole(), egg_item_id, egg_cfg->huanshou_id);

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_huan_shou_open_egg_rare_broadcast_content, m_module_mgr->GetUid(), 
			m_module_mgr->GetRoleName(), egg_open_cfg->egg_id, base_cfg->quality, egg_cfg->huanshou_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE));
		}
	}
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] open huanshou_id[%d] egg_id[%d] times[%d]", __FUNCTION__, m_module_mgr->GetUid(),
		m_module_mgr->GetRoleName(), egg_cfg->huanshou_id, egg_open_cfg->egg_id, cur_open_egg_times);
}

bool RoleHuanShou::OnOpenHuanShouEgg(int huanshou_id)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return false;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg) 
	{
		return false;
	}
	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (huan_shou_info.IsValid())
	{
		bool is_put_succ = false;
		ItemConfigData convert_item(base_cfg->convert_item_id, true, 1);
		if (m_module_mgr->GetKnapsack()->CheckForPut(base_cfg->convert_item_id, 1, false))
		{
			if (m_module_mgr->GetKnapsack()->Put(convert_item, PUT_REASON_HUAN_SHOU_ACTIVE_CONVERT))
			{
				is_put_succ = true;
			}
		}

		if (!is_put_succ)
		{
			m_module_mgr->GetKnapsack()->SendMail(&convert_item, 1);
		}

		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_ACTIVE_CONVERT, huanshou_id);
	}
	else
	{
		huan_shou_info.str_level = 1;
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_ACTIVE, huanshou_id);
		EventHandler::Instance().OnGetNewHuanShou(m_module_mgr->GetRole(), huanshou_id);
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d] active", __FUNCTION__, m_module_mgr->GetUid(),
			m_module_mgr->GetRoleName(), huanshou_id);
	}

	return true;
}

void RoleHuanShou::OnStrHuanShouLevel(int huanshou_id)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (!huan_shou_info.IsValid())
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_NOT_ACTIVE);
		return;
	}

	const HuanShouAttrGroupCfg * attr_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetAttrGroupCfg(base_cfg->attr_group_id);
	if (NULL == attr_cfg)
	{
		return;
	}

	if (huan_shou_info.str_level >= attr_cfg->max_str_level || huan_shou_info.str_level >= ARRAY_ITEM_COUNT(attr_cfg->level_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_STR_LEVEL_LIMIT);
		return;
	}

	const HuanShouAttrGroupLevelCfg & level_cfg = attr_cfg->level_list[huan_shou_info.str_level];
	if (!level_cfg.consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)level_cfg.consume_list.size() && stuff_count < ARRAY_ITEM_COUNT(stuff_list); ++i)
		{
			stuff_list[stuff_count].item_id = level_cfg.consume_list[i].item_id;
			stuff_list[stuff_count].num = level_cfg.consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count += 1;
		}

		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			if (consume_list.count > 0)
			{
				if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return;
				}
			}
		}
	}

	huan_shou_info.str_level += 1;
	this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_STR_LEVEL, huanshou_id, huan_shou_info.str_level);
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
	EventHandler::Instance().OnStrHuanShouLevel(m_module_mgr->GetRole(), huanshou_id, huan_shou_info.str_level);
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d] str_level[%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), huanshou_id, huan_shou_info.str_level);
}

void RoleHuanShou::OnLearnSkill(int huanshou_id, int bag_index, int lock_flag)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (!huan_shou_info.IsValid())
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_NOT_ACTIVE);
		return;
	}

	std::set<int> lock_skill_index;
	for (int i = 0; i < BitCount(lock_flag); ++i)
	{
		if (lock_flag & (1 << i))
		{
			lock_skill_index.insert(i);
		}
	}

	const ItemGridData * item_data = this->GetItemGridData2ByBag(bag_index);
	if (NULL == item_data)
	{
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data->item_id);
	if (NULL == item_base || I_TYPE_HUAN_SHOU_SKILL != item_base->GetItemType())
	{
		return;
	}

	const HuanShouSkillItem * skill_item = (const HuanShouSkillItem *)item_base;
	if (NULL == skill_item)
	{
		return;
	}

	const GamePassiveSkillCfg * learn_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(skill_item->GetSkillId());
	if (NULL == learn_skill_cfg)
	{
		return;
	}

	std::map<ItemID, int> consume_list;
	consume_list[item_data->item_id] += 1;
	const HuanShouOtherCfg & other_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetOtherCfg();
	int need_item_count = 0;
	int has_skill_count = huan_shou_info.GetSkillCount();
	if (!lock_skill_index.empty())
	{
		int lock_count = lock_skill_index.size();
		for (int i = 0; i < lock_count && i < ARRAY_ITEM_COUNT(other_cfg.need_skill_num_list); ++i)
		{
			if (has_skill_count < other_cfg.need_skill_num_list[i])
			{
				m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_SKILL_NOT_ENOUGH);
				return;
			}

			need_item_count += other_cfg.need_item_num_list[i];
		}
	}

	if (has_skill_count < 0)
	{
		return;
	}
	int max_skill_count_limit = GetMin(ARRAY_ITEM_COUNT(huan_shou_info.skill_list), base_cfg->skill_lattice); 

	int replace_gift_idnex = -1;
	if (LOGIC_CONFIG->GetHuanShouConfig()->IsCanAddNewSkill(has_skill_count) && has_skill_count < max_skill_count_limit)
	{
		replace_gift_idnex = has_skill_count;
	}
	else
	{
		std::vector<int> can_replace_skill_index;
		for (int i = 0; i < has_skill_count && i < max_skill_count_limit; ++i)
		{
			if(huan_shou_info.skill_list[i] <= 0) break;

			if (skill_item->GetSkillId() == huan_shou_info.skill_list[i])
			{
				m_module_mgr->NoticeNum(errornum::EN_REPEAT_PET_SKILL);
				return;
			}

			const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(huan_shou_info.skill_list[i]);
			if (NULL == passive_skill_cfg)
			{
				return;
			}

			bool is_lock = lock_skill_index.find(i) != lock_skill_index.end();
			if (learn_skill_cfg->IsElementalSkillType() && passive_skill_cfg->IsElementalSkillType())
			{
				if (learn_skill_cfg->skill_type == passive_skill_cfg->skill_type && learn_skill_cfg->skill_priority <= passive_skill_cfg->skill_priority)
				{
					m_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL);
					return;
				}

				if (is_lock)
				{
					m_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED_2);
					m_module_mgr->NoticeNum(errornum::EN_CAN_NOT_REPLACE_PET_SKILL_LOCKED);
					return;
				}

				replace_gift_idnex = i;
			}

			if (is_lock)
			{
				continue;
			}

			can_replace_skill_index.push_back(i);
		}

		if (-1 == replace_gift_idnex)
		{
			if (can_replace_skill_index.empty())
			{
				return;
			}

			int r = RandomNum((int)can_replace_skill_index.size());
			replace_gift_idnex = can_replace_skill_index[r];
		}
	}
	
	//检测道具(技能书:幻兽背包 技能锁定道具:人物背包)
	if (!this->IsHasItemCount(item_data->item_id, 1))
	{
		m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}
	if(need_item_count > 0)
	{
		if (!m_module_mgr->GetKnapsack()->HasItem(other_cfg.lock_item_id, need_item_count))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}
		if(!m_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.lock_item_id, need_item_count, __FUNCTION__))
		{
			m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
			return;
		}
	}
	//上面已判断,该处不应该会失败
	this->ConsumeItem(item_data->item_id, 1, __FUNCTION__);
	
	huan_shou_info.new_skill_gird_idx = replace_gift_idnex;
	huan_shou_info.new_skill_id = skill_item->GetSkillId();
	this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_LEARN_SKILL, huanshou_id, huan_shou_info.new_skill_gird_idx, huan_shou_info.new_skill_id);
	EventHandler::Instance().OnHuanShouLearnSkill(m_module_mgr->GetRole(), huanshou_id);
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d] str_level[%d] new_skill[index:%d id:%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), huanshou_id, huan_shou_info.str_level, huan_shou_info.new_skill_gird_idx, huan_shou_info.new_skill_id);
}

void RoleHuanShou::OnSaveSkill(int huanshou_id, bool is_save)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (!huan_shou_info.IsValid())
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_NOT_ACTIVE);
		return;
	}

	if (!is_save)
	{
		if (huan_shou_info.IsHasNewSkill())
		{
			gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s clear[idx:%d id:%d]", __FUNCTION__, huan_shou_info.new_skill_gird_idx, huan_shou_info.new_skill_id);
		}
		huan_shou_info.NewSkillReset();
		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_CLEAR_LEARN_SKILL, huanshou_id);
		return;
	}
	
	if (!huan_shou_info.IsHasNewSkill())
	{
		return;
	}

	int skill_idx = huan_shou_info.new_skill_gird_idx;
	int old_skill_id = huan_shou_info.skill_list[skill_idx];
	huan_shou_info.skill_list[skill_idx] = huan_shou_info.new_skill_id;
	huan_shou_info.NewSkillReset();
	if (m_param.IsFushen() && m_param.fushen_id == huanshou_id)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
	}
	this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_SAVE_LEARN_SKILL, huanshou_id, skill_idx, huan_shou_info.skill_list[skill_idx]);
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] idx[%d] old_skill[%d] new_skill[%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), skill_idx, old_skill_id, huan_shou_info.skill_list[skill_idx]);
}

void RoleHuanShou::OnLearnPPointLevelUp(int huanshou_id, int point_index)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (!huan_shou_info.IsValid())
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_NOT_ACTIVE);
		return;
	}

	if (point_index < 0 || point_index >= ARRAY_ITEM_COUNT(huan_shou_info.point_level_list))
	{
		return;
	}

	unsigned char & point_level = huan_shou_info.point_level_list[point_index];
	//ps.还未激活,则需判断前置,如已激活则跳过
	if (point_level <= 0)
	{
		const HuanShouPointUnlockCfg * unlock_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetPointUnlockCfg(point_index);
		if (NULL == unlock_cfg || unlock_cfg->adjoin_list.empty())
		{
			return;
		}
		
		bool is_can_learn = false;
		for (std::set<int>::const_iterator it = unlock_cfg->adjoin_list.begin(); it != unlock_cfg->adjoin_list.end(); ++it)
		{
			if (huan_shou_info.point_level_list[*it] >= unlock_cfg->need_active_level)
			{
				is_can_learn = true;
				break;
			}
		}
		if (!is_can_learn)
		{
			m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_PP_NOT_LEARN_BY_ADJOIN_POINT_LEVEL);
			return;
		}
	}

	const HuanShouPotentialPointLevelCfg * level_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetPointLevelCfg(base_cfg->potential_group_id, point_index, point_level + 1);
	if (NULL == level_cfg)
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_PP_LEVEL_LIMIT);
		return;
	}

	if (!level_cfg->consume_list.empty())
	{
		int stuff_count = 0;
		ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
		for (int i = 0; i < (int)level_cfg->consume_list.size() && stuff_count < ARRAY_ITEM_COUNT(stuff_list); ++i)
		{
			stuff_list[stuff_count].item_id = level_cfg->consume_list[i].item_id;
			stuff_list[stuff_count].num = level_cfg->consume_list[i].num;
			stuff_list[stuff_count].buyable = false;
			stuff_count += 1;
		}

		if (stuff_count > 0)
		{
			static ItemExtern::ItemConsumeList consume_list; consume_list.Reset();
			if (!ItemExtern::GetConsumeListByOrder(m_module_mgr->GetRole(), false, stuff_count, stuff_list, &consume_list, false))
			{
				m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
				return;
			}

			if (consume_list.count > 0)
			{
				if (!m_module_mgr->GetKnapsack()->ConsumeItemByIndexList(consume_list.count, consume_list.column_list, consume_list.index_list, consume_list.num_list, __FUNCTION__))
				{
					m_module_mgr->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
					return;
				}
			}
		}
	}

	point_level += 1;
	if (m_param.IsFushen() && m_param.fushen_id == huanshou_id)
	{
		m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
	}
	this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_LEARN_P_POINT, huanshou_id, point_index, point_level);

	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou[%d] point[idx:%d level:%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), huanshou_id, point_index, point_level);
}

void RoleHuanShou::OnLianDan()
{
	const HuanShouOtherCfg & other_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetOtherCfg();
	if (m_param.cur_lian_dan_count >= other_cfg.liandan_max)
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_LIAN_DAN_LIMIT);
		return;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(other_cfg.potential_item_id);
	if (NULL == item_base)
	{
		return;
	}

	if (!m_module_mgr->GetMoney()->UseMoney(MONEY_TYPE_SILVER_COIN, other_cfg.exchange_price, __FUNCTION__))
	{
		m_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
		return;
	}

	if (m_param.start_lian_dan_timestamp <= 0)
	{
		m_param.start_lian_dan_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	}
	m_param.cur_lian_dan_count += 1;
	this->SendOtherInfo();
	EventHandler::Instance().OnHuanShouLianDan(m_module_mgr->GetRole());
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] cur_count[%d] start_time[%u]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.cur_lian_dan_count, m_param.start_lian_dan_timestamp);
}

void RoleHuanShou::OnFuShen(int huanshou_id)
{
	if (this->IsInvalidId(huanshou_id))
	{
		return;
	}

	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
	if (NULL == base_cfg)
	{
		return;
	}

	RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
	if (!huan_shou_info.IsValid())
	{
		m_module_mgr->NoticeNum(errornum::EN_HUAN_SHOU_NOT_ACTIVE);
		return;
	}

	m_param.fushen_id = huanshou_id;
	this->SendOtherInfo();
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
	EventHandler::Instance().OnHuanShouFuShenChange(m_module_mgr->GetRole());
}

void RoleHuanShou::OnCancelFuShen()
{
	m_param.fushen_id = -1;
	this->SendOtherInfo();
	m_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_HUAN_SHOU, __FUNCTION__);
	EventHandler::Instance().OnHuanShouFuShenChange(m_module_mgr->GetRole());
}

void RoleHuanShou::OnSetCurUseImage(bool is_use_huanshou)
{
	m_param.is_use_huanshou = is_use_huanshou ? 1 : 0;
	this->SendOtherInfo();
	if (m_param.IsFushen())
	{
		EventHandler::Instance().OnHuanShouFuShenChange(m_module_mgr->GetRole());
	}
}

void RoleHuanShou::OnHuanShouItemDecompose(std::set<short> decompose_index_list, int bag_column)
{
	if (decompose_index_list.empty() || bag_column < ItemNamespace::ITEM_COLUMN_TYPE_MIN || bag_column >= ItemNamespace::ITEM_COLUMN_TYPE_MAX)
	{
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	std::map<ItemID, int> return_list;
	std::map<short, int> real_consume_map;
	int decompose_count = 0;
	for (std::set<short>::const_iterator it = decompose_index_list.begin(); it != decompose_index_list.end(); ++it)
	{
		const ItemGridData * item_grid = m_module_mgr->GetKnapsack()->GetItemGridData(bag_column, *it);
		if (NULL == item_grid || item_grid->Invalid() || item_grid->num <= 0)
		{
			continue;
		}

		const std::vector<ItemConfigData> * decompose_item_list = LOGIC_CONFIG->GetHuanShouConfig()->GetDecomposeCfg(item_grid->item_id);
		if (NULL == decompose_item_list)
		{
			continue;
		}

		for (int i = 0; i < (int)decompose_item_list->size(); ++i)
		{
			return_list[(*decompose_item_list)[i].item_id] += (*decompose_item_list)[i].num * item_grid->num;
			decompose_count += item_grid->num;
		}
		real_consume_map[*it] = item_grid->num;
	}
	if (real_consume_map.empty() || return_list.empty())
	{
		return;
	}
	std::vector<ItemConfigData> ret_item_list;
	for (std::map<ItemID, int>::const_iterator it = return_list.begin(); it != return_list.end(); ++it)
	{
		ret_item_list.push_back(ItemConfigData(it->first, true, it->second));
	}

	if (!m_module_mgr->GetKnapsack()->CheckForPutList2(ret_item_list))
	{
		m_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
		return;
	}
	//列表中经过上面的判断,删除不会失败.不可在返回
	for (std::map<short, int>::const_iterator it = real_consume_map.begin(); it != real_consume_map.end(); ++it)
	{
		m_module_mgr->GetKnapsack()->ConsumeItemByIndex(bag_column, it->first, it->second, __FUNCTION__);
	}

	m_module_mgr->GetKnapsack()->PutVec(ret_item_list, PUT_REASON_HUAN_SHOU_DECOMPOSE);
	m_module_mgr->GetKnapsack()->NotifyGetItemListVec(ret_item_list, GET_REASON_HUAN_LING_DECOMPOSE);
	m_module_mgr->NoticeNum(noticenum::NT_ZAO_HUA_DISCOMPOSE_SUCC);
	EventHandler::Instance().OnHuanShouDecompose(m_module_mgr->GetRole(), decompose_count);
	gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] decompose_count[%d]", __FUNCTION__, 
		m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), decompose_count);
}

void RoleHuanShou::SendHuanShouAllInfo()
{
	Protocol::SCHuanShouAllInfo info;
	info.egg_count = LOGIC_CONFIG->GetHuanShouConfig()->GetEggCount();
	info.huanshou_count = 0;

	UNSTD_STATIC_CHECK(sizeof(info.egg_cur_open_times_list) == sizeof(m_param.egg_cur_open_times_list));
	memcpy(info.egg_cur_open_times_list, m_param.egg_cur_open_times_list, sizeof(info.egg_cur_open_times_list));
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.huan_shou_list) && info.huanshou_count < ARRAY_ITEM_COUNT(info.huanshou_list); ++i)
	{
		const RoleHuanShouData & huanshou_data = m_param.huan_shou_list[i];
		if (!huanshou_data.IsValid())
		{
			continue;
		}

		info.huanshou_list[info.huanshou_count] = huanshou_data;
		info.huanshou_list[info.huanshou_count].huanshou_id = i;
		info.huanshou_list[info.huanshou_count].point_level_count = LOGIC_CONFIG->GetHuanShouConfig()->GetPointCount();
		info.huanshou_count += 1;
	}

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleHuanShou::SendHuanShouSingleInfo(int huanshou_id)
{
	if(this->IsInvalidId(huanshou_id)) return;

	Protocol::SCHuanShouSingleInfo info;
	info.huan_shou_info = m_param.huan_shou_list[huanshou_id];
	info.huan_shou_info.huanshou_id = huanshou_id;
	info.huan_shou_info.point_level_count = LOGIC_CONFIG->GetHuanShouConfig()->GetPointCount(); 

	EngineAdapter::Instance().NetSendSerializeMsg(m_module_mgr->GetRole()->GetNetId(), &info);
}

void RoleHuanShou::SendOtherInfo()
{
	Protocol::SCHuanShouOtherInfo info;
	info.fushen_id = m_param.fushen_id;
	info.is_use_huanshou = m_param.is_use_huanshou;
	info.cur_lian_dan_count = m_param.cur_lian_dan_count;
	info.start_lian_dan_timestamp = m_param.start_lian_dan_timestamp;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleHuanShou::OnSendNoticeInfo(short notice_type, short param1, int param2, int param3)
{
	Protocol::SCHuanShouNoticeInfo info;
	info.notice_type = notice_type;
	info.param1 = param1;
	info.param2 = param2;
	info.param3 = param3;

	m_module_mgr->NetSend(&info, sizeof(info));
}

void RoleHuanShou::OnGetFuShenSkill(ARG_OUT BattleHuanShouSkillInfo * skill_info, ARG_OUT int * out_other_sp_list, int max_other_sp_num, short * out_other_sp_num)
{
	if (!FunOpen::IsFunOpen(m_module_mgr->GetRole(), FUN_TYPE_ROLE_HUAN_SHOU) || NULL == skill_info || NULL == out_other_sp_num || NULL == out_other_sp_list) return;

	if (!m_param.IsFushen() || this->IsInvalidId(m_param.fushen_id) || !m_param.huan_shou_list[m_param.fushen_id].IsValid())
	{
		return;
	}
	const RoleHuanShouData & huanshou_data = m_param.huan_shou_list[m_param.fushen_id];
	const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(m_param.fushen_id);
	if (NULL == base_cfg)
	{
		return;
	}
	const HuanShouAttrGroupLevelCfg * level_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetAttrGroupLevelCfgByLevel(base_cfg->attr_group_id, huanshou_data.str_level);
	if (NULL == level_cfg)
	{
		return;
	}

	skill_info->skill_id = level_cfg->skill_id;
	skill_info->skill_level = level_cfg->skill_level;
	skill_info->cd_start = level_cfg->cd_start;
	skill_info->cd_perform = level_cfg->cd_perform;

	std::vector<SkillPro> passive_skill_list;
	int max_point_num = GetMin(ARRAY_LENGTH(huanshou_data.point_level_list), LOGIC_CONFIG->GetHuanShouConfig()->GetPointCount());
	for (int i = 0; i < max_point_num; ++i)
	{
		if (huanshou_data.point_level_list[i] <= 0) continue;

		const HuanShouPotentialPointLevelCfg * point_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetPointLevelCfg(base_cfg->potential_group_id, i,
			huanshou_data.point_level_list[i]);
		if (NULL == point_cfg || HUAN_SHOU_POINT_TIAN_FU_TYPE_PASSIVE_SKILL != point_cfg->tianfu_type)
		{
			continue;
		}

		passive_skill_list.push_back(SkillPro(point_cfg->param1, point_cfg->param2));
	}

	this->GetPassiveSkillGridList(huanshou_data, passive_skill_list, base_cfg->skill_lattice);

	for (int i = 0; i < (int)passive_skill_list.size(); ++i)
	{
		const GamePassiveSkillCfg * paasive_cfg = SkillPool::Instance().GetPassiveSkillCfg(passive_skill_list[i].skill_id);
		if (NULL == paasive_cfg)
		{
			continue;
		}
		int spid = paasive_cfg->sp_id;
		if (paasive_cfg->is_level_spid)
		{
			if (passive_skill_list[i].skill_level >= 1 && passive_skill_list[i].skill_level < ARRAY_LENGTH(paasive_cfg->level_spid_list))
			{
				spid = paasive_cfg->level_spid_list[passive_skill_list[i].skill_level];
			}
		}
		const SpecialEffectCfg * spe_cfg = BattleSpecialEffectPool::GetInstance().GetSpecialEffectCfg(spid);
		if (NULL == spe_cfg) continue;

		bool is_has_battle_effect = false;
		for (int j = 0; j < spe_cfg->effect_num && j < EFFECT_DETAIL_NUM; ++j)
		{
			const SpecialEffectKey & key = spe_cfg->effect_detail_list[j];
			if (key.effect_type == SPECIAL_EFFECT_TYPE_ADD_ATTR) continue;

			is_has_battle_effect = true;
			break;
		}

		if (!is_has_battle_effect)
		{
			continue;
		}
		if (*out_other_sp_num < max_other_sp_num)
		{
			out_other_sp_list[(*out_other_sp_num)++] = spid;
		}
		else
		{
#ifdef _DEBUG
			assert(false);
#else
			gamelog::g_log_battle.printf(LL_INFO, "%s out_other_sp_num >= max_other_sp_num[%d] ", __FUNCTION__, max_other_sp_num);
#endif
			return;
		}
	}
}

int RoleHuanShou::GetFushenLevel() const
{
	if (!m_param.IsFushen() || this->IsInvalidId(m_param.fushen_id) || !m_param.huan_shou_list[m_param.fushen_id].IsValid())
	{
		return 0;
	}

	return m_param.huan_shou_list[m_param.fushen_id].str_level;
}

void RoleHuanShou::GmActiveHuanShou(int huanshou_id)
{
	if (-1 == huanshou_id)
	{
		int base_count = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCount();
		for (int i = 0; i < base_count && i < ARRAY_LENGTH(m_param.huan_shou_list); ++i)
		{
			if(m_param.huan_shou_list[i].IsValid()) continue;

			m_param.huan_shou_list[i].str_level = 1;
			EventHandler::Instance().OnGetNewHuanShou(m_module_mgr->GetRole(), i);
			gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d]", __FUNCTION__, m_module_mgr->GetUid(),
				m_module_mgr->GetRoleName(), i);
		}
		this->SendHuanShouAllInfo();
	}
	else
	{
		const HuanShouBaseCfg * base_cfg = LOGIC_CONFIG->GetHuanShouConfig()->GetBaseCfg(huanshou_id);
		if(NULL == base_cfg) return;
		RoleHuanShouData & huan_shou_info = m_param.huan_shou_list[huanshou_id];
		if (huan_shou_info.IsValid())
		{
			return;
		}

		huan_shou_info.str_level = 1;
		this->OnSendNoticeInfo(Protocol::SCHuanShouNoticeInfo::NOTICE_TYPE_ACTIVE, huanshou_id);
		EventHandler::Instance().OnGetNewHuanShou(m_module_mgr->GetRole(), huanshou_id);
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] huanshou_id[%d]", __FUNCTION__, m_module_mgr->GetUid(),
			m_module_mgr->GetRoleName(), huanshou_id);
	}
}

void RoleHuanShou::OnCheckLianDan(unsigned int now_second)
{
	int wait_time = LOGIC_CONFIG->GetHuanShouConfig()->GetOtherCfg().wait_time;
	if (m_param.start_lian_dan_timestamp > 0 && now_second >= m_param.start_lian_dan_timestamp + wait_time)
	{
		int interval = static_cast<int>(now_second - m_param.start_lian_dan_timestamp);
		int succ_count = GetMin(interval / wait_time, (int)m_param.cur_lian_dan_count);
		if (succ_count <= 0)
		{
			m_param.start_lian_dan_timestamp = 0;
			this->SendOtherInfo();
			return;
		}

		ItemConfigData liandan_item(LOGIC_CONFIG->GetHuanShouConfig()->GetOtherCfg().potential_item_id, true, succ_count);
		bool is_put_succ = false;
		if (m_module_mgr->GetKnapsack()->CheckForPut2(liandan_item, false))
		{
			if (m_module_mgr->GetKnapsack()->Put(liandan_item, PUT_REASON_HUAN_SHOU_LIAN_DAN))
			{
				is_put_succ = true;
			}
		}

		if (!is_put_succ)
		{
			m_module_mgr->GetKnapsack()->SendMail(&liandan_item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}

		m_param.cur_lian_dan_count -= succ_count;
		if (m_param.cur_lian_dan_count > 0)
		{
			m_param.start_lian_dan_timestamp += wait_time * succ_count;
		}
		else
		{
			m_param.start_lian_dan_timestamp = 0;
		}
		this->SendOtherInfo();
		gamelog::g_log_role_huan_shou.printf(LL_INFO, "%s user[%d, %s] cur_count[%d] start_time[%u]", __FUNCTION__,
			m_module_mgr->GetUid(), m_module_mgr->GetRoleName(), m_param.cur_lian_dan_count, m_param.start_lian_dan_timestamp);
	}
}

void RoleHuanShou::GetPassiveSkillGridList(const RoleHuanShouData & huanshou_data, std::vector<SkillPro>& skill_list, int skill_limit) const
{
	if(skill_limit <= 0) return;

	std::set<PetSkillPriority> skill_priority;
	for (int i = 0; i < ARRAY_LENGTH(huanshou_data.skill_list) && i < skill_limit; ++i)
	{
		if (huanshou_data.skill_list[i] <= 0) break;

		const GamePassiveSkillCfg * passive_skill_cfg = SkillPool::Instance().GetPassiveSkillCfg(huanshou_data.skill_list[i]);
		if (passive_skill_cfg == NULL)
		{
			continue;
		}
		PetSkillPriority tmp;
		tmp.skill_id = huanshou_data.skill_list[i];
		tmp.skill_level = 1;
		tmp.skill_type = passive_skill_cfg->IsElementalSkillType() ? 1 : passive_skill_cfg->skill_type;
		tmp.priority_num = passive_skill_cfg->skill_priority;
		tmp.priority_num_2 = 1;

		skill_priority.insert(tmp);
	}

	int last_sp_type = -1;
	for (std::set<PetSkillPriority>::iterator iter = skill_priority.begin(); iter != skill_priority.end(); ++iter)
	{
		if (last_sp_type == iter->skill_type)
		{
			continue;
		}

		last_sp_type = iter->skill_type;
		skill_list.push_back(SkillPro(iter->skill_id, iter->skill_level));
	}
	
}
