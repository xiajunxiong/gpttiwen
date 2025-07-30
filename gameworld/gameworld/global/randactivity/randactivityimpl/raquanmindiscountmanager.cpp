#include "raquanmindiscountmanager.hpp"

#include "randactivityquanmindiscount.hpp"
#include "world.h"
#include "global/rmibackobjdef.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"
#include "internalcomm.h"
#include "serverlogic.h"
#include "global/worldstatus/worldstatus.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityquanmindiscountconfig.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "engineadapter.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

RAQuanMinDiscountManager & RAQuanMinDiscountManager::Instance()
{
	static RAQuanMinDiscountManager ins;
	return ins;
}

RAQuanMinDiscountManager::RAQuanMinDiscountManager()
{
	m_is_load_finish = false;
	m_is_change = false;
	m_last_save_time = 0;
	m_status = structcommon::CS_NONE;

	for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		m_info[i].Reset(); 
	}
	for (int i = 0; i < QM_DISCOUNT_MAX_UID; ++i)
	{
		m_uid_list[i].Reset();
	}
}

RAQuanMinDiscountManager::~RAQuanMinDiscountManager()
{
}

void RMIInitQuanMinDiscountBackObjectImpl::InitQuanMinDiscountDataRet(int ret, const QuanMinDiscountParam &param_list)
{
	if (0 != ret)
	{
		printf("QuanMinDiscountManager::Init Failed ret[%d]\n", ret);
	}

	RAQuanMinDiscountManager::Instance().Init(ret, param_list);
}

void RMISaveQuanMinDiscountBackObjectImpl::SaveQuanMinDiscountDataRet(int ret)
{
	if (0 != ret)
	{
		printf("RMISaveQuanMinDiscountBackObjectImpl SaveQuanMinDiscountRet Failed ret[%d]\n", ret);
		gamelog::g_log_debug.printf(LL_INFO, "SaveQuanMinDiscountRet ret[%d]", ret);
	}
}

void RAQuanMinDiscountManager::Init(int ret, const QuanMinDiscountParam & param_list)
{
	//if (CrossConfig::Instance().IsHiddenServer()) 不能屏蔽这里，否则IsLoadFinish返回False，隐藏服就开不起来了
	//{
	//	return; // 逻辑统一在原服
	//}

	for (int i = 0; i < param_list.count && i < QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT; ++i)
	{
		if (0 == param_list.data_list[i].uid)
		{
			continue;
		}

		for (int k = 0; k < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++k)
		{
			if (0 < param_list.data_list[i].buy_count[k])
			{
				for (int index = 0; index < QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM; ++index)
				{
					if (0 != m_info[k].buy_list[index].uid)
					{
						continue;
					}

					m_info[k].buy_list[index].uid = param_list.data_list[i].uid;
					m_info[k].buy_list[index].buy_count = param_list.data_list[i].buy_count[k];
					break;
				}
			}
		}
	}

	if (param_list.count >= QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT)
	{
		this->LoadData(param_list.next_id);
	}
	else
	{
		gamelog::g_log_world.printf(LL_INFO, "%s finish load data at %u %lu", __FUNCTION__,
			(unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());
		m_is_load_finish = true;
		m_last_save_time = EngineAdapter::Instance().Time();
		this->SetInfoCountAndTime();
	}
}

void RAQuanMinDiscountManager::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return; // 逻辑统一在原服
	}

	if (!m_is_change) return;

	std::map<int, std::map<int, int> > uid_map;		//第一个key-uid  第二个key-商品seq value-购买次数
	for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		for (int index = 0; index < QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM; ++index)
		{	
			if (0 == m_info[i].buy_list[index].uid)break;

			std::map<int, std::map<int, int> >::iterator it = uid_map.find(m_info[i].buy_list[index].uid);
			if (it == uid_map.end())
			{
				uid_map[m_info[i].buy_list[index].uid][i] = m_info[i].buy_list[index].buy_count;
			}
			else
			{
				it->second[i] = m_info[i].buy_list[index].buy_count;
			}
		}
	}

	int inc_index = 0;
	for (std::map<int, std::map<int, int> >::const_iterator vit = uid_map.begin(); vit != uid_map.end(); ++vit)
	{
		m_uid_list[inc_index].uid = vit->first;

		for (std::map<int, int>::const_iterator zit = vit->second.begin(); zit != vit->second.end(); ++zit)
		{
			m_uid_list[inc_index].buy_count[zit->first] = zit->second;
		}

		inc_index++;
	}

	static QuanMinDiscountParam param;

	int count = (int)uid_map.size();
	int for_count = count / QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT;
	int for_count_index = count % QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT;
	if (0 != for_count_index)
	{
		for_count++;
	}

	for (int i = 0; i < QM_DISCOUNT_MAX_UID / QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT && i < for_count; ++i)
	{
		if (i == for_count - 1)
		{
			//最后一轮存储 不一定满 QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT
			param.count = for_count_index;
		}
		else
		{
			param.count = QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT;
		}

		for (int k = 0; k < QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT; k++)
		{
			param.data_list[k].change_state = m_status;
			param.data_list[k].uid = m_uid_list[i * QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT + k].uid;
			param.data_list[k].buy_flag = 0;

			memcpy(param.data_list[k].buy_count, m_uid_list[i * QUAN_MIN_DISCOUNT_PARAM_MAX_COUNT + k].buy_count, sizeof(param.data_list[k].buy_count));
		}

		RMISaveQuanMinDiscountBackObjectImpl * impl = new RMISaveQuanMinDiscountBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		if (i == for_count - 1)
		{
			if (ac.SaveQuanMinDiscountDataAsyn(param, impl))
			{
				m_is_change = false;
			}
			else
			{
				gamelog::g_log_debug.printf(LL_WARNING, "QuanMinDiscountManager::Save Call RMI Method Failed");
			}
		}
		else
		{
			if (!ac.SaveQuanMinDiscountDataAsyn(param, impl))
			{
				gamelog::g_log_debug.printf(LL_WARNING, "QuanMinDiscountManager::Save Call RMI Method Failed");
			}
		}
	}
}

bool RAQuanMinDiscountManager::OnServerStart()
{
	return this->LoadData(0);
}

void RAQuanMinDiscountManager::OnServerStop()
{
	this->Save();
}

void RAQuanMinDiscountManager::Update(unsigned long interval, time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return; // 逻辑统一在原服
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	//暗箱操作 每过一小时 自动增加购买人数
	RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);
	if (NULL == activity)return;

	if (ACTIVITY_STATUS_OPEN != activity->GetStatus())return;

	if (structcommon::CS_NONE == m_status)
	{
		m_status = structcommon::CS_UPDATE;
	}

	//活动开启 3600s 保存一次数据
	if (now_second - m_last_save_time >= 3600)
	{
		this->Save();
		m_last_save_time = now_second;
	}

	for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, i);
		if (NULL == cfg)continue;

		if (0 == m_info[i].next_add_timestamp && 0 != cfg->hour)
		{
			m_info[i].next_add_timestamp = (unsigned int)now_second + cfg->hour * 3600;
			activity->SetDirty();
		}
		else
		{
			if (now_second >= m_info[i].next_add_timestamp && 0 != m_info[i].next_add_timestamp)
			{
				m_info[i].buy_count += cfg->support_number;
				m_info[i].next_add_timestamp = (unsigned int)now_second + cfg->hour * 3600;
				this->CalcDiscount(i);
				m_is_change = true;
				activity->SetDirty();
			}
		}
	}
}

void RAQuanMinDiscountManager::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return; // 逻辑统一在原服
	}

	if (to_status == ACTIVITY_STATUS_OPEN)
	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

		const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
		if (NULL != config)
		{
			m_status = structcommon::CS_UPDATE;
			m_last_save_time = EngineAdapter::Instance().Time();
			for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
			{
				const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, i);
				if (NULL != cfg)
				{
					m_info[i].next_add_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + cfg->hour * 3600;
				}
			}
		}
	}

	if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		m_is_change = true;
		m_status = structcommon::CS_DELETE;
		this->Save();

		m_last_save_time = 0;
		for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
		{
			m_info[i].Reset();
		}	
	}
}

void RAQuanMinDiscountManager::OnBuyCommodity(int seq, int uid, bool is_max_discount)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM) return;

	m_info[seq].buy_count++;
	m_is_change = true;

	{
		RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
		if (NULL != ramgr)
		{
			RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);
			if (NULL != activity)
			{
				RandActivityQuanMinDiscount * act = (RandActivityQuanMinDiscount*)activity;
				if (NULL != act)
				{
					act->SetDirty();
				}
			}
		}
	}

	if (is_max_discount) return;

	int real_uid = 0;
	if (this->CheckHasUid(seq, uid))
	{
		real_uid = uid;
	}

	for (int i = 0; i < QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM; ++i)
	{
		if (real_uid == m_info[seq].buy_list[i].uid)
		{
			m_info[seq].buy_list[i].uid = uid;
			m_info[seq].buy_list[i].buy_count++;
			break;
		}
	}
	this->CalcDiscount(seq);
}

int RAQuanMinDiscountManager::GetBuyCount(int seq)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM)return -1;

	return m_info[seq].buy_count;
}

unsigned int RAQuanMinDiscountManager::GetNextAddTime(int seq)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM)return -1;

	return m_info[seq].next_add_timestamp;
}

double RAQuanMinDiscountManager::GetCommodityDiscout(int seq)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM)return -1;

	return m_info[seq].cur_discount;
}

void RAQuanMinDiscountManager::SetInfoCountAndTime()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);
	if (NULL == activity)return;
	RandActivityQuanMinDiscount * act = (RandActivityQuanMinDiscount*)activity;

	for (int i = 0; i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		m_info[i].buy_count = act->GetBuyCount(i);
		m_info[i].next_add_timestamp = act->GetNextAddTime(i);

		this->CalcDiscount(i, true);
	}
	
}

bool RAQuanMinDiscountManager::CheckHasUid(int seq, int uid)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM) return false;

	if (0 == uid) return false;

	for (int i = 0; i < QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM; ++i)
	{
		if (uid == m_info[seq].buy_list[i].uid)
		{
			return true;
		}
	}
	return false;
}

void RAQuanMinDiscountManager::OnSyncInfoFromGame(int seq, double discount, int buy_count)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM) return;

	m_info[seq].cur_discount = discount;
	m_info[seq].buy_count = buy_count;
}

void RAQuanMinDiscountManager::CalcDiscount(int seq, bool isnt_init)
{
	if (0 > seq || seq >= QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM)return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, seq);
	if (NULL == cfg)return;

	double discount = config->GetDiscountCfg(cfg->discount_id, m_info[seq].buy_count);

	double original_discount = m_info[seq].cur_discount;
	if (original_discount > discount)
	{
		m_info[seq].cur_discount = discount;
		if (!isnt_init)
		{
			this->OnDiscountChange(seq, original_discount);
		}
	}
}

std::string GetDiscountStr(double discount)
{
	discount += 1e-6;
	if (discount - (int)discount < 1e-4) // 应算作整数
	{
		return STRING_SPRINTF("%.f", discount);
	}
	else
	{
		// 小数保留1位
		return STRING_SPRINTF("%.1f", discount);
	}
}

void RAQuanMinDiscountManager::OnDiscountChange(int seq, double original_discount)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	const RAQuanMinDiscountCfg * cfg = config->GetQuanMinDiscountCfg(ramgr, seq);
	if (NULL == cfg)return;

	for (int i = 0; i < QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM; ++i)
	{
		if(0 == m_info[seq].buy_list[i].uid) continue;
		if(0 == m_info[seq].buy_list[i].buy_count) continue;

		double original_price = (double)original_discount / 10.0 * cfg->price;
		double cur_price = (double)m_info[seq].cur_discount / 10.0 * cfg->price;
		double origin_total_price = original_price * m_info[seq].buy_list[i].buy_count;
		double cur_total_price = cur_price * m_info[seq].buy_list[i].buy_count;
		int ret_all_price = (int)round(origin_total_price - cur_total_price);
		if (ret_all_price < 0)
		{
			assert(0);
			continue;
		}

		//邮件返回差价
		static MailContentParam contentparam; contentparam.Reset();
		switch (cfg->money_type)
		{
		case MONEY_TYPE_GOLD:
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = ret_all_price;
			break;
		case MONEY_TYPE_COIN_BIND:
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = ret_all_price;
			break;
		default:
			break;
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_quan_min_discount_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_quan_min_discount_message, cfg->item.item_id, GetDiscountStr(m_info[seq].cur_discount).c_str(), m_info[seq].buy_list[i].buy_count);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(m_info[seq].buy_list[i].uid), MAIL_REASON_DEFAULT, contentparam);
		}

		gamelog::g_log_qm_discount.printf(LL_INFO, 
			"RAQuanMinDiscountManager::OnDiscountChange by mail, \
uid[%d] comomdity_seq[%d] original_discount[%.2f] curr_discount[%.2f] \
return_type[%d] buy_count[%d] origin_price[%.2f] cur_price[%.2f] \
origin_total_price[%.2f] cur_total_price[%.2f] all_return[%d] reason[%s]",
			m_info[seq].buy_list[i].uid, seq, original_discount, 
			m_info[seq].cur_discount, cfg->money_type, 
			m_info[seq].buy_list[i].buy_count, original_price, 
			cur_price, origin_total_price, cur_total_price, 
			ret_all_price, __FUNCTION__);
	}
}

bool RAQuanMinDiscountManager::LoadData(long long id_from)
{
	if (this->IsLoadFinish()) return false;
	m_is_load_finish = false;

	RMIInitQuanMinDiscountBackObjectImpl *impl = new RMIInitQuanMinDiscountBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitQuanMinDiscountDataAsyn(id_from, impl);
}
