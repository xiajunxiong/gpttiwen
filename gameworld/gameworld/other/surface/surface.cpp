#include "surface.hpp"
#include "surfaceconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamelog.h"
#include "gameworld/engineadapter.h"
#include "obj/character/role.h"
#include "other/event/eventhandler.hpp"
#include "servercommon/errornum.h"
#include "other/prestige/prestigeconfig.hpp"
#include "gameworld/gameworld/item/itempool.h"
#include "other/fashioncollection/fashioncollection.hpp"

#include <set>

Surface::Surface() : m_active_count(0)
{
	memset(m_active_surface_list, 0, sizeof(m_active_surface_list));
}

Surface::~Surface()
{
}

void Surface::Init(RoleModuleManager * role_module_manager, const SurfaceParam & param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.surface_list); i++)
	{
		const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(i);
		if(NULL == cfg || m_param.surface_list[i].level <= 0 || cfg->type < 0 || cfg->type >= SURFACE_TYPE_MAX) continue;

		m_active_surface_list[cfg->type]++;
		m_active_count++;
	}
}

void Surface::GetOtherInitParam(SurfaceParam * param)
{
	*param = m_param;
}

const AttributeList & Surface::ReCalcAttr(AttributeList & base_add, bool is_recalc)
{
	if (is_recalc)
	{
		m_attrs_add.Reset();
		std::map<int, int> suit_count;
		std::set<int> id_set;
		for (int i = 0; i < SURFACE_MAX_NUM; ++i)
		{
			SurfaceParamData& data = m_param.surface_list[i];
			if (data.level <= 0) continue;

			const SurfaceCfg *surface_cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(i);
			if (NULL != surface_cfg)
			{
				if (surface_cfg->suit_id > 0)
				{
					suit_count[surface_cfg->suit_id]++;
				}
				id_set.insert(i); 
			}
		}

		for (std::map<int, int>::const_iterator it = suit_count.begin(); it != suit_count.end(); ++it)
		{
			const SurfaceSuitCfg *suit_cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceSuitCfg(it->first);
			if (NULL == suit_cfg) continue;

			if (suit_cfg->suit_num <= it->second)
			{
				for (int i = 0; i < BATTLE_ATTR_MAX; i++)
				{
					m_attrs_add.m_attrs[i] += suit_cfg->attrs[i];
				}
			}
		}

		// 时装属性迁移到时装升级系统
// 		int prof = m_role_module_mgr->GetRole()->GetAppearanceProf();
// 		for (std::set<int>::iterator set_it = id_set.begin(); set_it != id_set.end(); set_it++)
// 		{
// 			const SurfaceAttrCfg * attr_cfg= LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceAttrCfgByIdAndProf(*set_it, prof);
// 			if(NULL == attr_cfg) continue;
// 
// 			for (int i = 0; i < BATTLE_ATTR_MAX; i++)
// 			{
// 				m_attrs_add.m_attrs[i] += attr_cfg->attrs[i];
// 			}
// 		}

	}

	base_add.Add(m_attrs_add.m_attrs);
	return m_attrs_add;
}

void Surface::Update(unsigned long interval, time_t now_second)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.surface_list); ++i)
	{
		SurfaceParamData & curr = m_param.surface_list[i];

		if (curr.end_time > 0 && now_second > curr.end_time)
		{
			const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(i);
			if (NULL == cfg)
			{
				continue;
			}

			if (0 <= cfg->type && cfg->type < SURFACE_TYPE_MAX)
			{
				if (m_param.wear_surface[cfg->type] == i)
				{
					this->WearSurface(cfg->type, 0);
				}

				m_active_surface_list[cfg->type] -= 1;
				m_active_count -= 1;
			}


			curr.Reset();
			this->SendSingleInfo(i, Protocol::SCSurfaceSingleInfo::SURFACE_REASON_TYPE_END_TIME_NOTICE);
			m_role_module_mgr->GetFashionCollection()->Expired(cfg->id);
		}
	}
}

int Surface::ActivateSurface(int id)
{
	if (id < 0 || id >= SURFACE_MAX_NUM) return -1;
	const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(id);
	if (NULL == cfg) return -3;

	SurfaceParamData& data = m_param.surface_list[id];
	if (data.level >= MAX_INT16) return -2;
	if (data.level >0)
	{ 
// 		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_SURFACE_ALREADY_HAD);
		return -4;
	}
	data.level++;

	if (cfg->duration > 0)
	{
		unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
		data.end_time = now + cfg->duration;
	}
	

	if (cfg->type >= 0 && cfg->type < SURFACE_TYPE_MAX)
	{
		m_active_surface_list[cfg->type]++;
		m_active_count++;
	}
	this->SendSingleInfo(id, Protocol::SCSurfaceSingleInfo::SURFACE_REASON_TYPE_ACTIVATE_NOTICE);
	m_role_module_mgr->ReCalcAttr(AttributeList::RECALC_REASON_SURFACE, __FUNCTION__);

	m_role_module_mgr->GetFashionCollection()->OnActivation(id);
	gamelog::g_log_surface.printf(LL_INFO, "Func[%s] user[%d %s] id[%d] level[%d]",
								  __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
								  id, data.level);

	ROLE_LOG_QUICK6(LOG_TYPE_HUANHUA_ACTIVE, m_role_module_mgr->GetRole(), id, 0, __FUNCTION__, "");
	EventHandler::Instance().OnAddSurface(m_role_module_mgr->GetRole(), cfg->type, cfg->id);
	return 0;
}

int Surface::WearSurface(int type, int id)
{
	if (id < 0 || id >= SURFACE_MAX_NUM) return -1;
	if (type < 0 || type >= SURFACE_TYPE_MAX) return -2;

	if (0 == id)
	{
		m_param.wear_surface[type] = 0;
	}
	else
	{
		SurfaceParamData data = m_param.surface_list[id];
		if (data.level <= 0) return -5;

		const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(id);
		if (NULL == cfg) return -3;
		if (cfg->type != type) return -4;

		m_param.wear_surface[type] = id;
	}

	EventHandler::Instance().OnRoleWearOrTakeOffSurface(m_role_module_mgr->GetRole());
	this->SendWearInfo();
	return 0;
}

 bool Surface::DecomposeSurface(short column, short index,int num)
{
	ItemGridData item_grid;
	if(!m_role_module_mgr->GetKnapsack()->GetItemGridData(column,index,&item_grid))
	{	
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_LOAD_ERROR);
		return false;
	}
	const ItemBase *item_base = ITEMPOOL->GetItem(item_grid.item_id);
	if(item_base == NULL || item_base->GetItemType() != I_TYPE_EXPENSE) return false;

	const NorexItem * norex_item = (NorexItem *)item_base;
	int param1 = norex_item->GetParam1();
	if (param1 < 0 || param1 >= SURFACE_MAX_NUM) return false;
	const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(param1);
	if (NULL == cfg) return false;

	ItemConfigData item;
	item.item_id = cfg->decompose_item;
	item.is_bind = true;
	item.num = cfg->decompose_num * num;
	if(!m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id,(short)item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	if (!m_role_module_mgr->GetKnapsack()->DecomposeSurface(column, index, num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SURFACE_DECOMPOSE_ERROR);
		return false;
	}
	if(!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_DECOMPOSE_SURFACE))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	//以前分解配的是时装碎片,现在改为幻化碎片,时装币无法对应,先屏蔽
	//m_role_module_mgr->GetMoney()->AddOtherCoin((Int64)item.num, MONEY_TYPE_SURFACE_COIN,__FUNCTION__);

	return true;
}

 bool Surface::AutoDecomposeActivatedSur(const NorexItem * norex_item, int num, bool is_noitce)
 {
	 if (NULL == norex_item || num <= 0) return false;

	 int param1 = norex_item->GetParam1();
	 if (param1 < 0 || param1 >= SURFACE_MAX_NUM) return false;
	 const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(param1);
	 if (NULL == cfg || cfg->id < 0 || cfg->id >= SURFACE_MAX_NUM) return false;

	 if (this->ActivateSurface(cfg->id) == 0)
	 {
		 num--;
	 }

	 if (num > 0)
	 {
		 ItemConfigData item;
		 item.item_id = cfg->decompose_item;
		 item.is_bind = true;
		 item.num = cfg->decompose_num * num;

		 bool is_true = false;
		 if (m_role_module_mgr->GetKnapsack()->CheckForPut(item.item_id, (short)item.num))
		 {
			 if (m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_DECOMPOSE_SURFACE, NULL, NULL, 0, 0, is_noitce))
			 {
				 is_true = true;
			 }
		 }

		 if (is_noitce)
		 {
 			 m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_CONVERT_ALREADY_OWNED);
		 }
		 if (!is_true)
		 {
			 m_role_module_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
		 }
	 }
	 return true;
 }

 bool Surface::IsActiveSurface(int id)
 {
	 if (id < 0 || id >= SURFACE_MAX_NUM) return false;

	 if (m_param.surface_list[id].level <= 0) return false;

	 return true;
 }

 void Surface::GetWearSurfaceList(short wear_surface[SURFACE_TYPE_MAX])
 {
	memcpy(wear_surface, m_param.wear_surface, sizeof(short) * SURFACE_TYPE_MAX);
 }

 int Surface::ExchangegItem(int seq)
 {
	 const SurfaceExchangeCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceExchangeCfg(seq);
	 if (cfg == NULL)
	 {
		 return -__LINE__;
	 }

	 if (cfg->dealline_time != 0 && EngineAdapter::Instance().Time() > cfg->dealline_time)
	 {
		 m_role_module_mgr->NoticeNum(errornum::EN_EXHANGE_ITEM_TIME_HAS_PASSED);
		 return -__LINE__;
	 }

	 if (!m_role_module_mgr->GetKnapsack()->CheckForPut2(cfg->reward))
	 {
		 return -__LINE__;
	 }

	 if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->consume_item_id, cfg->consume_num, __FUNCTION__, 0, true))
	 {
		 return -__LINE__;
	 }

	 m_role_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_SURFACE_SHOP);

	 return 0;
 }

 int Surface::GetActiveCount(int surface_type)
 {
	if (surface_type < SURFACE_TYPE_BODY || surface_type >= SURFACE_TYPE_MAX)
	{
		return m_active_count;
	}
	 return m_active_surface_list[surface_type];
 }

void Surface::SendAllInfo()
{
	Protocol::SCSurfaceAllInfo info;
	info.surface_count = 0;
	for (int i = 0; i < SURFACE_MAX_NUM; ++i)
	{
		if (m_param.surface_list[i].level <= 0) continue;
		info.surface_list[info.surface_count].id = i;
		info.surface_list[info.surface_count].level = m_param.surface_list[i].level;
		info.surface_list[info.surface_count].end_time = m_param.surface_list[i].end_time;
		info.surface_count++;
	}	
	memcpy(info.wear_surface, m_param.wear_surface, sizeof(info.wear_surface));
	int len = sizeof(info) - ((SURFACE_MAX_NUM - info.surface_count) * sizeof(Protocol::SurfaceData));
	m_role_module_mgr->NetSend((const char*)&info, len);
}

void Surface::SendSingleInfo(int seq, int reason)
{
	if (seq < 0 || seq >= SURFACE_MAX_NUM) return  ;

	Protocol::SCSurfaceSingleInfo info;
	info.reason = reason;
	info.surface.id = seq;
	info.surface.level = m_param.surface_list[seq].level;
	info.surface.end_time = m_param.surface_list[seq].end_time;

	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Surface::SendWearInfo()
{
	Protocol::SCSurfaceWearInfo info;
	memcpy(info.wear_surface, m_param.wear_surface, sizeof(info.wear_surface));

	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}
