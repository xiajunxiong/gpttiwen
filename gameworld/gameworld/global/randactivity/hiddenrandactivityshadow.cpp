#include "hiddenrandactivityshadow.hpp"
#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/localconfig.hpp"
#include "world.h"
#include "servercommon/serverconfig/crossconfig.hpp"

HiddenRandActivityShadow* HiddenRandActivityShadow::Instance()
{
	static HiddenRandActivityShadow ins;
	return &ins;
}

RandActivityManager* HiddenRandActivityShadow::GetRAManager(const UniqueServerID& usid)
{
	UniqueServerID temp_usid = usid;

	const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(usid);
	if (NULL != mother_usid)
	{
		temp_usid = *mother_usid;
	}

	RAManagerMap::iterator it = m_ramgr_map.find(temp_usid);
	if (it != m_ramgr_map.end())
	{
		return it->second;
	}
	
	return NULL;
}

void HiddenRandActivityShadow::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	RAManagerMap::iterator it = m_ramgr_map.begin();
	for (; it != m_ramgr_map.end(); ++it)
	{
		RandActivityManager* ramgr = it->second;
		if (NULL != ramgr)
		{
			ramgr->Update(interval, now_second);
		}
	}
}

void HiddenRandActivityShadow::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	RAManagerMap::iterator it = m_ramgr_map.begin();
	for (; it != m_ramgr_map.end(); ++it)
	{
		RandActivityManager* ramgr = it->second;
		if (NULL != ramgr)
		{
			ramgr->OnDayChange(old_dayid, now_dayid);
		}
	}
}

void HiddenRandActivityShadow::OnWeekChange()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	RAManagerMap::iterator it = m_ramgr_map.begin();
	for (; it != m_ramgr_map.end(); ++it)
	{
		RandActivityManager* ramgr = it->second;
		if (NULL != ramgr)
		{
			ramgr->OnWeekChange();
		}
	}
}

void HiddenRandActivityShadow::OnMonthChange()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	RAManagerMap::iterator it = m_ramgr_map.begin();
	for (; it != m_ramgr_map.end(); ++it)
	{
		RandActivityManager* ramgr = it->second;
		if (NULL != ramgr)
		{
			ramgr->OnMonthChange();
		}
	}
}

void HiddenRandActivityShadow::OnSyncActivityInfoFromCross(int plat_type, int server_id, int count, CrossRandActivityDataParam * param_list)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	UniqueServerID usid(plat_type, server_id);
	RandActivityManager* ramgr = this->GetRAManager(usid);
	if (NULL == ramgr)
	{
		ramgr = this->CreateRandActivityManager();
		if (NULL == ramgr) return;

		ramgr->SetUniqueServerID(usid);
		ramgr = m_ramgr_map.insert(RAManagerMap::value_type(usid, ramgr)).first->second;
	}

	ramgr->OnSyncActivityInfoFromCross(count, param_list);
}

RandActivityManager* HiddenRandActivityShadow::CreateRandActivityManager()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return NULL;

	return new RandActivityManager();
}

HiddenRandActivityShadow::HiddenRandActivityShadow()
{
	//RandActivityManager* local_ramgr = this->CreateRandActivityManager();
	//if (NULL != local_ramgr)
	//{
	//	UniqueServerID usid = LocalConfig::Instance().GetUniqueServerID();
	//	local_ramgr->SetUniqueServerID(usid);
	//	m_ramgr_map.insert(RAManagerMap::value_type(usid, local_ramgr));
	//}
}

HiddenRandActivityShadow::~HiddenRandActivityShadow()
{
	RAManagerMap::iterator it = m_ramgr_map.begin();
	for (; it != m_ramgr_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
}
