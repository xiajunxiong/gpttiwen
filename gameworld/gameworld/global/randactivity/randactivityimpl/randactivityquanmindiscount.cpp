#include "randactivityquanmindiscount.hpp"
#include "world.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"
#include "raquanmindiscountmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityquanmindiscountconfig.hpp"

RandActivityQuanMinDiscount::RandActivityQuanMinDiscount(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT)
{
}

RandActivityQuanMinDiscount::~RandActivityQuanMinDiscount()
{
}

void RandActivityQuanMinDiscount::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy(&m_param, ra_data->data, sizeof(m_param));

	if (0 == m_param.is_set_data_flag)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
		if (NULL == ramgr || NULL == config) return;

		for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
		{
			const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, i);
			if (NULL == cfg) continue;

			int old_buy_count = m_param.buy_count[i];
			if (m_param.buy_count[i] >= 30000)
			{
				m_param.buy_count[i] = config->GetMinOrMaxBuyNeedNum(cfg->discount_id, true);
			}
			else if (m_param.buy_count[i] < 0)
			{
				m_param.buy_count[i] = config->GetMinOrMaxBuyNeedNum(cfg->discount_id, false) - 1;
			}
			gamelog::g_log_qm_discount.printf(LL_INFO, "%s seq[%d] old_buy_count[%d] new_buy_count[%d]", __FUNCTION__, i, old_buy_count, m_param.buy_count[i]);
		}
		m_param.is_set_data_flag = 1;
	}
}

void RandActivityQuanMinDiscount::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	this->SetData();

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}


void RandActivityQuanMinDiscount::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	RAQuanMinDiscountManager::Instance().OnSpecialActivityStatusChange(from_status, to_status);
}

void RandActivityQuanMinDiscount::SetData()
{
	for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		m_param.buy_count[i] = RAQuanMinDiscountManager::Instance().GetBuyCount(i);
		m_param.next_add_timestamp[i] = RAQuanMinDiscountManager::Instance().GetNextAddTime(i);
	}
}
