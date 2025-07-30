#include "systemrecycle.hpp"
#include "item/itembase.h"
#include "item/itempool.h"
#include "systemrecyclerule.hpp"
#include "global/rmibackobjdef.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "engineadapter.h"

SystemRecycle::SystemRecycle() : m_is_change(false), m_next_save_time(0u), 
	m_next_check_lapse_time(0u)
{
	
}

SystemRecycle::~SystemRecycle()
{
	this->Release();
}

void SystemRecycle::Update(time_t now)
{
	unsigned now_second = static_cast<unsigned int>(now);
	if (m_next_save_time > 0u && now_second > m_next_save_time)
	{
		this->SaveDB();
		m_next_save_time = now_second + 10;
	}

	if (m_next_check_lapse_time > 0u && now_second > m_next_check_lapse_time)
	{
		this->CheckRuleLapse();
		m_next_check_lapse_time = now_second + 60;
	}
}

void SystemRecycle::SaveDB()
{
	static SystemRecycleRuleList list;
	list.count = 0;
	this->GetChangeSaleItemList(&list);

	if (list.count <= 0) return;

	RMISaveSystemRecycleRuleBackObjectImpl *impl = new RMISaveSystemRecycleRuleBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	bool ret = ac.SaveTradeMarketSystemRecycleRuleAsync(list, impl);
	if (ret)
	{
		this->ClearDirtyFlag(list);
	}
}

void SystemRecycle::OnInitFinish()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	
	m_next_check_lapse_time = now + 60;
	m_next_save_time = now + 10;
}

void RMISaveSystemRecycleRuleBackObjectImpl::SaveRet(int ret)
{
	gamelog::g_log_trade_market.printf(LL_INFO, "RMISaveSystemRecycleRuleBackObjectImpl::SaveRet ret[%d]", ret);
}

bool SystemRecycle::AddRecycleRule(const RecycleRuleParam& param, bool is_init)
{
	const RecycleRuleInfo* info = this->GetRuleInfo(param.rule_id);
	if (NULL != info) return false;

	RecycleRuleInfo* new_info = new RecycleRuleInfo();
	new_info->Init(param, is_init);
	if (!is_init)
	{
		m_is_change = true;
	}

	m_rule_map.insert(RuleMap::value_type(new_info->GetRuleID(), new_info));
	
	ItemRuleHash::iterator it = m_item_rule_hash.find(new_info->GetItemID());
	if (it == m_item_rule_hash.end())
	{
		it = m_item_rule_hash.insert(ItemRuleHash::value_type(new_info->GetItemID(), RuleVec())).first;
	}
	it->second.push_back(new_info);

	return true;
}

bool SystemRecycle::RemoveRecycleRule(long long rule_id)
{
	RecycleRuleInfo* info = this->GetRuleInfo(rule_id);
	if (NULL == info) return false;

	info->SetInvalidFlag(true);
	info->SetDirty(true);
	m_is_change = true;
	return true;
}

bool SystemRecycle::IsMatchRule(ItemID item_id, int price, const NetValueItemParam& item_param, ARG_OUT long long* rule_id)
{
	ItemRuleHash::iterator it = m_item_rule_hash.find(item_id);
	if (it == m_item_rule_hash.end()) return false;

	for (size_t i = 0; i < it->second.size(); ++i)
	{
		if (it->second[i]->IsMatchRule(item_id, price, item_param))
		{
			if (NULL != rule_id)
			{
				*rule_id = it->second[i]->GetRuleID();
			}
			return true;
		}
	}

	return false;
}

void SystemRecycle::OnRecycle(long long rule_id)
{
	RecycleRuleInfo* info = this->GetRuleInfo(rule_id);
	if (NULL != info)
	{
		info->OnRecycle();
		m_is_change = true;
	}
}

void SystemRecycle::GetChangeSaleItemList(ARG_OUT SystemRecycleRuleList* data)
{
	if (!m_is_change)
	{
		data->count = 0;
		return;
	}

	int count = 0;
	RuleMap::iterator it = m_rule_map.begin();
	for (; it != m_rule_map.end(); ++it)
	{
		RecycleRuleInfo& info = *it->second;
		if (!info.IsDirty()) continue;

		if (info.IsValid())
		{
			//update or insert
			data->rule_list[count].change_state = (info.GetOldState() ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			data->rule_list[count].rule_id = info.GetRuleID();
			info.GetInitParam(&data->rule_list[count].param);
		}
		else
		{
			//delete
			if (!info.GetOldState()) continue;
	
			data->rule_list[count].change_state = structcommon::CS_DELETE;
			data->rule_list[count].rule_id = it->first;
		}
		
		++count;
	}

	data->count = count;

	return;
}

void SystemRecycle::ClearDirtyFlag(const SystemRecycleRuleList& data)
{
	for (int i = 0; i < data.count; ++i)
	{
		long long rule_id = data.rule_list[i].rule_id;

		RuleMap::iterator it = m_rule_map.find(rule_id);
		if (it == m_rule_map.end()) continue;

		RecycleRuleInfo* info = it->second;

		if (info->IsValid())
		{
			info->SetDirty(false);
			info->SetOldState(true);
		}
		else
		{
			this->EraseRule(info->GetItemID(), info->GetRuleID());
		}
	}
}

void SystemRecycle::EraseRule(ItemID item_id, long long rule_id)
{
	RecycleRuleInfo* info = NULL;

	RuleMap::iterator it = m_rule_map.find(rule_id);
	if (it != m_rule_map.end())
	{
		info = it->second;
		m_rule_map.erase(it);
	}

	ItemRuleHash::iterator iter = m_item_rule_hash.find(item_id);
	if (iter != m_item_rule_hash.end())
	{
		RuleVec& vec = iter->second;
		for (RuleVec::iterator rule_it = vec.begin(); rule_it != vec.end(); ++rule_it)
		{
			RecycleRuleInfo* rri = *rule_it;
			if (rri->GetRuleID() == rule_id)
			{
				vec.erase(rule_it);
				break;
			}
		}
	}

	if (NULL != info)
	{
		delete info;
	}
}

RecycleRuleInfo* SystemRecycle::GetRuleInfo(long long rule_id)
{
	RuleMap::iterator it = m_rule_map.find(rule_id);
	if (it != m_rule_map.end())
	{
		return it->second;
	}

	return NULL;
}

void SystemRecycle::Release()
{
	RuleMap::iterator it = m_rule_map.begin();
	for (; it != m_rule_map.end(); ++it)
	{
		if (NULL != it->second)
		{
			RecycleRuleInfo* info = it->second;
			delete info;
			it->second = NULL;
		}
	}

	m_rule_map.clear();
	m_item_rule_hash.clear();
}

bool SystemRecycle::CheckRuleLapse()
{
	unsigned int now = (unsigned int)(EngineAdapter::Instance().Time());

	std::vector<long long> lapse_vec;
	RuleMap::iterator it = m_rule_map.begin();
	for (; it != m_rule_map.end(); ++it)
	{
		if (NULL != it->second && it->second->IsValid())
		{
			if (it->second->IsLapse(now))
			{
				lapse_vec.push_back(it->first);
			}
		}
	}

	for (size_t i = 0; i < lapse_vec.size(); ++i)
	{
		this->RemoveRecycleRule(lapse_vec[i]);
	}

	return true;
}

