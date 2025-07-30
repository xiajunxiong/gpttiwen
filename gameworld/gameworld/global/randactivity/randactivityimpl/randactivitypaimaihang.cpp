#include "randactivitypaimaihang.hpp"
#include "gamelog.h"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitypaimaihangconfig.hpp"

#include "global/topicmanager/topicmanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/rapaimaihangmanager/rapaimaihangmanager.hpp"

#include "protocol/randactivity/msgrapaimaihang.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitypaimaihang.hpp"

RandActivityPaiMaiHang::RandActivityPaiMaiHang(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_PAI_MAI_HANG)
{
}

RandActivityPaiMaiHang::~RandActivityPaiMaiHang()
{
}

void RandActivityPaiMaiHang::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy(&m_param, ra_data->data, sizeof(m_param));
}

void RandActivityPaiMaiHang::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivityPaiMaiHang::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);
	this->SendNoticeInfo(user);
}

void RandActivityPaiMaiHang::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if(m_rand_activity_status != ACTIVITY_STATUS_OPEN) return;

	//这里只检测活动进行中
	this->OnCheckTime((unsigned int)now_second, false);
}

void RandActivityPaiMaiHang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivity::OnDayChange(old_dayid, now_dayid);
	if(old_dayid == now_dayid) return;

	this->OnReCalcTime();
	this->SendNoticeInfo();
}

void RandActivityPaiMaiHang::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_OPEN:
		{
			this->OnReCalcTime();
			this->SendNoticeInfo();
		}
		break;
		case ACTIVITY_STATUS_CLOSE:	//直接对活动当天竞拍进行结算
		{
			this->OnCheckTime(0, true);
			RAPaiMaiHangManager::Instance().OnClearAllRecord();
		}
		break;
	default:
		break;
	}
}

bool RandActivityPaiMaiHang::IsBidTime(RAPaiMaiHangSingleShopItemInfo shop_item_info)
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_param.pai_mai_start_timestamp <= now_second && now_second <=  shop_item_info.end_timestamp)
	{
		return true;
	}

	return false;
}

RAPaiMaiHangSingleShopItemInfo * RandActivityPaiMaiHang::GetShopItemInfo(int seq)
{
	if(seq < 0 || seq >= ARRAY_LENGTH(m_param.shop_item_list)) return NULL;

	return &m_param.shop_item_list[seq];
}

void RandActivityPaiMaiHang::OnSetEndTimestamp(RAPaiMaiHangSingleShopItemInfo & shop_item_info, int delay_time)
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	int delay_time_second = delay_time * SECOND_PER_MIN;
	if (now_second <= shop_item_info.end_timestamp && now_second >= shop_item_info.end_timestamp - delay_time_second)
	{
		shop_item_info.end_timestamp = GetMin(shop_item_info.end_timestamp + delay_time_second, m_param.pai_mai_last_end_timestamp);
	}
}

void RandActivityPaiMaiHang::SendNoticeInfo(Role * user)
{
	Protocol::SCRAPaiMaiHangNoticeInfo info;
	info.pai_mai_start_timestamp = m_param.pai_mai_start_timestamp;
	info.pai_mai_end_timestamp = m_param.pai_mai_end_timestamp;
	info.pai_mai_last_end_timestamp = m_param.pai_mai_last_end_timestamp;
	for (int i = 0; i < ARRAY_LENGTH(info.shop_item_list) && i < ARRAY_LENGTH(m_param.shop_item_list); ++i)
	{
		info.shop_item_list[i] = m_param.shop_item_list[i];
	}

	if (NULL != user)
	{
		user->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_RA_PAI_MAI_HANG, (const char *)&info, sizeof(info));
	}
}

void RandActivityPaiMaiHang::SendNoticeSingleInfo(int seq, Role * user)
{
	if(seq < 0 || seq >= ARRAY_LENGTH(m_param.shop_item_list)) return;

	Protocol::SCRAPaiMaiHangSingleNoticeInfo info;
	info.seq = seq;
	info.shop_item_info = m_param.shop_item_list[seq];

	if (NULL != user)
	{
		user->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_RA_PAI_MAI_HANG, (const char *)&info, sizeof(info));
	}
}

void RandActivityPaiMaiHang::OnReCalcTime()
{
	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		return;
	}
	const RAPaiMaiHangOtherCfg & other_cfg = config->GetOtherCfg();
	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_param.Reset();
	m_param.pai_mai_start_timestamp = now_second + EngineAdapter::Instance().NextDayInterval(other_cfg.auction_time / 100, other_cfg.auction_time % 100, 0);
	m_param.pai_mai_end_timestamp = now_second + EngineAdapter::Instance().NextDayInterval(other_cfg.end_time / 100, other_cfg.end_time % 100, 0);
	if (m_param.pai_mai_end_timestamp < m_param.pai_mai_start_timestamp)
	{
		m_param.pai_mai_end_timestamp += SECOND_PER_DAY;
	}
	m_param.pai_mai_last_end_timestamp = now_second + EngineAdapter::Instance().NextDayInterval(other_cfg.last_end_time / 100, other_cfg.last_end_time % 100, 0);
	if (m_param.pai_mai_last_end_timestamp < m_param.pai_mai_end_timestamp)
	{
		m_param.pai_mai_last_end_timestamp += SECOND_PER_DAY;
	}
	m_param.activity_open_day = config->GetActOpenDay(m_rand_activity_manager, m_rand_activity_type);
	for (int i = 0; i < ARRAY_LENGTH(m_param.shop_item_list); i++)
	{
		const RAPaiMaiHangShopCfg * shop_cfg = config->GetShopCfg(m_rand_activity_manager, i, m_param.activity_open_day);
		if (NULL != shop_cfg)
		{
			m_param.shop_item_list[i].bid_price = shop_cfg->base_price;
			m_param.shop_item_list[i].add_price = config->GetOtherCfg().pre_price;
		}
		m_param.shop_item_list[i].end_timestamp = m_param.pai_mai_end_timestamp;
	}
}

void RandActivityPaiMaiHang::OnCheckTime(unsigned int now_second, bool is_close)
{
	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		return;
	}
	for (int i = 0; i < ARRAY_LENGTH(m_param.shop_item_list) && i < MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM; ++i)
	{
		if(1 == m_param.shop_item_list[i].is_big_end_flag) continue;
		if(!is_close &&  now_second < m_param.shop_item_list[i].end_timestamp) continue;

		RAPaiMaiHangSingleShopItemInfo & item_info = m_param.shop_item_list[i];
		item_info.is_big_end_flag = 1;

		if (item_info.uid <= 0)
		{
			gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] Auction without", __FUNCTION__, m_param.activity_open_day, i);
			continue;
		}

		const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(m_rand_activity_manager, i, m_param.activity_open_day);
		if(NULL == shop_item_cfg) 
		{
			gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] uid[%d] bid_price[%d] GetConfig Failed", __FUNCTION__, m_param.activity_open_day, i , item_info.uid, item_info.bid_price);
			continue;
		}

		gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] uid[%d] bid_price[%d] ready", __FUNCTION__, m_param.activity_open_day, i, item_info.uid, item_info.bid_price);

		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, shop_item_cfg->item);
		
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_pai_mai_hang_bid_subject);
		int length = 0;
		if (m_param.shop_item_list[i].remain_deposit_gold > 0)
		{
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = m_param.shop_item_list[i].remain_deposit_gold;
			length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_pai_mai_hang_bid_succ_content1, shop_item_cfg->item.item_id, m_param.shop_item_list[i].remain_deposit_gold);
		}
		else
		{
			length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_pai_mai_hang_bid_succ_content, shop_item_cfg->item.item_id);
		}
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(item_info.uid, MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] uid[%d] bid_price[%d] succ", __FUNCTION__, m_param.activity_open_day, i, item_info.uid, item_info.bid_price);
		}
		this->SendNoticeSingleInfo(i);
	}
}
