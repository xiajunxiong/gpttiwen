#include "autoupshelve.hpp"
#include "autoupshelveinfo.hpp"
#include "engineadapter.h"
#include "gamelog.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"

AutoUpshelve::AutoUpshelve() : m_init_finish(false), m_is_change(false), m_next_save_time(0u)
{

}

AutoUpshelve::~AutoUpshelve()
{
	this->Release();
}

void AutoUpshelve::Update(time_t now)
{
	if (!m_init_finish) return;

	unsigned int now_second = static_cast<unsigned int>(now);
	AutoUpshelveInfoMap::iterator it = m_auto_info_map.begin();
	for (; it != m_auto_info_map.end(); ++it)
	{
		AutoUpshelveInfo* info = it->second;
		info->Update(now_second);
	}

	if (now_second > m_next_save_time)
	{
		this->SaveDB();
		m_next_save_time = now_second + 60;
	}
}

void AutoUpshelve::SaveDB()
{
	static AutoUpshelveDataList list;
	list.count = 0;
	this->GetChangeSaleItemList(&list);

	if (list.count <= 0) return;

#ifdef _DEBUG
	for (int i = 0; i < list.count; ++i)
	{
		if (list.data_list[i].change_state == structcommon::CS_DELETE)
		{
			gamelog::g_log_world.printf(LL_INFO, "AutoUpshelve::SaveDB DELETE unique_id[%lld]", list.data_list[i].unique_id);
		}
	}
#endif

	RMISaveSystemAutoUpshelveBackObjectImpl *impl = new RMISaveSystemAutoUpshelveBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	bool ret = ac.SaveSystemAutoUpshelveDataAsync(list, impl);
	if (ret)
	{
		this->ClearDirtyFlag(list);
	}
}

void RMISaveSystemAutoUpshelveBackObjectImpl::SaveRet(int ret)
{
	gamelog::g_log_trade_market.printf(LL_INFO, "RMISaveSystemAutoUpshelveBackObjectImpl::SaveRet ret[%d]", ret);
}

void AutoUpshelve::OnInitFinish()
{
	m_init_finish = true;
	m_next_save_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 60;
}

bool AutoUpshelve::AddAutoUpshelveInfo(const AutoUpshelveParam& param, bool is_init)
{
	AutoUpshelveInfo* info = this->GetAutoUpshelveInfo(param.unique_id);
	if (NULL != info)
	{
		if (is_init)
		{
			gamelog::g_log_trade_market.printf(LL_INFO, "AutoUpshelve::AddAutoUpshelveInfo FAILED reason[Already Exist] unique_id[%lld] item_id[%d]", param.unique_id,param.item_id);
			return false;
		}
		else
		{
			info->Init(param, is_init);
		}
	}
	else
	{
		AutoUpshelveInfo* new_info = new AutoUpshelveInfo();
		new_info->Init(param, is_init);

		m_auto_info_map.insert(AutoUpshelveInfoMap::value_type(new_info->GetUniqueID(), new_info));
	}

	return true;
}

bool AutoUpshelve::RemoveAutoUpshelveInfo(long long unique_id)
{
	AutoUpshelveInfo* info = this->GetAutoUpshelveInfo(unique_id);
	if (NULL == info) return false;

	info->SetInvalidFlag(true);
	info->SetDirty(true);
	this->SetIsChange(true);
	return true;
}

void AutoUpshelve::OnItemSold(long long unique_id)
{
	AutoUpshelveInfo* info = this->GetAutoUpshelveInfo(unique_id);
	if (NULL == info) return;

	info->OnSold();
}

void AutoUpshelve::GetChangeSaleItemList(ARG_OUT AutoUpshelveDataList* data)
{
	if (!m_is_change)
	{
		data->count = 0;
		return;
	}

	int count = 0;
	AutoUpshelveInfoMap::iterator it = m_auto_info_map.begin();
	for (; it != m_auto_info_map.end() && count < ARRAY_LENGTH(data->data_list); ++it)
	{
		AutoUpshelveInfo& info = *it->second;
		if (!info.IsDirty()) continue;

		if (info.IsValid())
		{
			//update or insert
			data->data_list[count].change_state = (info.GetOldState() ? structcommon::CS_UPDATE : structcommon::CS_INSERT);
			data->data_list[count].unique_id = info.GetUniqueID();
			info.GetInitParam(&data->data_list[count].param);
		}
		else
		{
			//delete
			if (!info.GetOldState()) continue;

			data->data_list[count].change_state = structcommon::CS_DELETE;
			data->data_list[count].unique_id = it->first;
		}

		++count;
	}

	data->count = count;

	return;
}

void AutoUpshelve::ClearDirtyFlag(const AutoUpshelveDataList& data)
{
	for (int i = 0; i < data.count; ++i)
	{
		long long unique_id = data.data_list[i].unique_id;

		AutoUpshelveInfoMap::iterator it = m_auto_info_map.find(unique_id);
		if (it == m_auto_info_map.end()) continue;

		AutoUpshelveInfo* info = it->second;

		if (info->IsValid())
		{
			info->SetDirty(false);
			info->SetOldState(true);
		}
		else
		{
			this->EraseAutoUpshelveInfo(info->GetUniqueID());
		}
	}
}

void AutoUpshelve::EraseAutoUpshelveInfo(long long unique_id)
{
#ifdef _DEBUG
	gamelog::g_log_world.printf(LL_INFO, "AutoUpshelve::EraseAutoUpshelveInfo unique_id[%lld]", unique_id);
#endif

	AutoUpshelveInfo* info = NULL;

	AutoUpshelveInfoMap::iterator it = m_auto_info_map.find(unique_id);
	if (it != m_auto_info_map.end())
	{
		info = it->second;
		m_auto_info_map.erase(it);
	}

	if (NULL != info)
	{
		delete info;
	}
}

AutoUpshelveInfo* AutoUpshelve::GetAutoUpshelveInfo(long long unique_id)
{
	AutoUpshelveInfoMap::iterator it = m_auto_info_map.find(unique_id);
	if (it != m_auto_info_map.end())
	{
		return it->second;
	}

	return NULL;
}

void AutoUpshelve::Release()
{
	AutoUpshelveInfoMap::iterator it = m_auto_info_map.begin();
	for (; it != m_auto_info_map.end(); ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}
	m_auto_info_map.clear();
}

