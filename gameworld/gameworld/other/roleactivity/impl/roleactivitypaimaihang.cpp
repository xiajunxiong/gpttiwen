#include "roleactivitypaimaihang.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitypaimaihangconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/rapaimaihangmanager/rapaimaihangmanager.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitypaimaihang.hpp"

#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "protocol/randactivity/msgrapaimaihang.hpp"

#include "obj/character/role.h"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"

RoleActivityPaiMaiHang::RoleActivityPaiMaiHang(int type)
	:RoleActivity(type)
{
}

RoleActivityPaiMaiHang::~RoleActivityPaiMaiHang()
{
}

void RoleActivityPaiMaiHang::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RARolePaiMaiHangParam));
}

void RoleActivityPaiMaiHang::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RARolePaiMaiHangParam));
}

void RoleActivityPaiMaiHang::OnLogin()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if(NULL == activity) return;

	for (int i = 0; i < MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM && i < ARRAY_LENGTH(m_param.shop_item_list); ++i)
	{
		RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(i);
		if (NULL == shop_item_info || shop_item_info->uid != m_role_module_mgr->GetUid())
		{
			m_param.shop_item_list[i].RetReset();
		}
	}
	this->SetRoleDataChange();
}

void RoleActivityPaiMaiHang::OnLoginSendInfo()
{
	this->SendAllInfo();
}

void RoleActivityPaiMaiHang::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < ARRAY_LENGTH(m_param.shop_item_list); ++i)
	{
		m_param.shop_item_list[i].DayReset();
	}
	this->SendAllInfo();
	this->SetRoleDataChange();
}

void RoleActivityPaiMaiHang::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		RAPaiMaiHangManager::Instance().OnClearRecordByRole(m_role_module_mgr->GetUid());

		this->SetRoleDataChange();
	}
}

void RoleActivityPaiMaiHang::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if(NULL == raor) return;
	
	switch (raor->opera_type)
	{
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_ALL_INFO:
		{
			this->SendAllInfo();
			RAPaiMaiHangManager::Instance().OnSendRecordToRole(m_role_module_mgr->GetRole());
		}
		break;
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_BID:
		{
			this->OnBidShopItem(raor->param_1, raor->param_2);
		}	
		break;
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_AUTO_BID:
		{
			this->OnAutoBidShopItem(raor->param_1, raor->param_2, raor->param_3);
		}
		break;
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_CANCEL_AUTO:
		{
			this->OnCancelAutoBid(raor->param_1);
		}
		break;
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_RETRIEVE:
		{
			this->OnRetrieveGold(raor->param_1, raor->param_2, raor->param_3);
		}
		break;
		case Protocol::RA_PAI_MAI_HANG_REQ_TYPE_SET_SHOW_FALG:
		{
			this->OnSetShowFlag(raor->param_1, raor->param_2);
		}
		break;
		case RA_PAI_MAI_HANG_REQ_TYPE_RECORD_INFO:
		{
			RAPaiMaiHangManager::Instance().OnSendRecordToRole(m_role_module_mgr->GetRole());
		}
		break;
		case RA_PAI_MAI_HANG_REQ_TYPE_CHANGE_ADD_PRICE:
		{
			this->OnChangeAddPrice(raor->param_1, raor->param_2);
		}
		break;
	default:
		break;
	}
}

void RoleActivityPaiMaiHang::OnSetLastBidPrice(int seq, int last_bid_price)
{
	if(seq < 0 || seq >= ARRAY_LENGTH(m_param.shop_item_list)) return;

	m_param.shop_item_list[seq].last_bid_gold = last_bid_price;
	this->SendSingleInfo(seq);
	this->SetRoleDataChange();
}

void RoleActivityPaiMaiHang::OnRetReset(int seq)
{
	if (seq < 0 || seq >= ARRAY_LENGTH(m_param.shop_item_list)) return;

	m_param.shop_item_list[seq].RetReset();
	this->SendSingleInfo(seq);
	this->SetRoleDataChange();
}

void RoleActivityPaiMaiHang::SendAllInfo()
{
	Protocol::SCRAPaiMaiHangUserInfo info;

	for (int i = 0; i < ARRAY_LENGTH(info.shop_item_list) && i < ARRAY_LENGTH(m_param.shop_item_list); ++i)
	{
		info.shop_item_list[i] = m_param.shop_item_list[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityPaiMaiHang::SendSingleInfo(int seq)
{
	if(seq < 0 || seq >= ARRAY_LENGTH(m_param.shop_item_list)) return;

	Protocol::SCRAPaiMaiHangUserSingleInfo info;
	info.seq = seq;
	info.shop_item_info = m_param.shop_item_list[seq];

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityPaiMaiHang::OnBidShopItem(int seq, int bid_price)
{
	if(seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM || bid_price <= 0) return;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (0 != shop_item_info->is_big_end_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION);
		return;
	}
	if (!activity->IsBidTime(*shop_item_info))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME);
		return;
	}
	if (shop_item_info->uid == m_role_module_mgr->GetUid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_OF_HIGH);
		return;
	}
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(bid_price))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	if (shop_item_info->bid_price + config->GetOtherCfg().pre_price > bid_price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_PRICE_CHANGE);
		activity->SendNoticeSingleInfo(seq, m_role_module_mgr->GetRole());
		return;
	}

	m_role_module_mgr->GetMoney()->UseGold(bid_price, __FUNCTION__);
	RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_BID, seq, activity->GetActivityOpenDay(), bid_price);
	RAPaiMaiHangManager::Instance().OnAddRecord(shop_item_info->uid, RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD, seq, activity->GetActivityOpenDay());

	//自动竞价
	int bid_count = shop_item_info->bid_price + shop_item_info->remain_deposit_gold;
	if (0 != shop_item_info->is_auto_bid && bid_count >= bid_price + shop_item_info->add_price)
	{	
		shop_item_info->bid_price = bid_price + shop_item_info->add_price;
		shop_item_info->remain_deposit_gold = bid_count - shop_item_info->bid_price;

		//返还
		this->OnReturnGold(m_role_module_mgr->GetRole(), bid_price, shop_item_cfg->item.item_id, activity->GetActivityOpenDay(), seq, false, m_role_module_mgr->GetUid());
		RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD, seq, activity->GetActivityOpenDay());
		RAPaiMaiHangManager::Instance().OnAddRecord(shop_item_info->uid, RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID, seq, activity->GetActivityOpenDay(), shop_item_info->bid_price,
		shop_item_info->remain_deposit_gold);
		Role * bid_user = World::GetInstWorld()->GetSceneManager()->GetRole(shop_item_info->uid);
		if (NULL != bid_user)
		{
			RoleActivityPaiMaiHang * ra_pai_mai_hang = static_cast<RoleActivityPaiMaiHang *>
				(bid_user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
			if (NULL != ra_pai_mai_hang)
			{
				ra_pai_mai_hang->OnSetLastBidPrice(seq, shop_item_info->bid_price);
			}
		}
		else
		{
			OfflineRegisterManager::Instance().OnSetRAPaiMaiHangLastBidInfo(shop_item_info->uid, seq, shop_item_info->bid_price);
		}
	}
	else
	{
		//返还
		if (shop_item_info->uid > 0)
		{
			this->OnReturnGold(shop_item_info->uid, bid_count, shop_item_cfg->item.item_id, activity->GetActivityOpenDay(), seq, shop_item_info->is_auto_bid);
		}

		shop_item_info->bid_price = bid_price;
		shop_item_info->remain_deposit_gold = 0;
		shop_item_info->add_price = config->GetOtherCfg().pre_price;
		shop_item_info->uid = m_role_module_mgr->GetUid();
		F_STRNCPY(shop_item_info->role_name, m_role_module_mgr->GetRoleName(), sizeof(shop_item_info->role_name));
		shop_item_info->is_show_user_flag = m_param.shop_item_list[seq].is_show_user_flag;
		shop_item_info->is_auto_bid = 0;
	}
	
	m_param.shop_item_list[seq].last_bid_gold = bid_price;
	activity->OnSetEndTimestamp(*shop_item_info, config->GetOtherCfg().delay_time);
	activity->SetDirty();
	activity->SendNoticeSingleInfo(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] bid[%d, %d, %d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), activity->GetActivityOpenDay(),
		seq, bid_price);
}

void RoleActivityPaiMaiHang::OnAutoBidShopItem(int seq, int bid_price, int add_price)
{
	if (seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM) return;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (add_price < config->GetOtherCfg().pre_price || 0 != add_price % config->GetOtherCfg().pre_price)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_pai_mai_hang_bid_set_add_price_notice, config->GetOtherCfg().pre_price, config->GetOtherCfg().pre_price);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
		return;
	}

	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (!activity->IsBidTime(*shop_item_info))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME);
		return;
	}
	if (0 != shop_item_info->is_big_end_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION);
		return;
	}
	if (!m_role_module_mgr->GetMoney()->GoldMoreThan(bid_price))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
		return;
	}
	if (shop_item_info->uid != m_role_module_mgr->GetUid() && bid_price < shop_item_info->bid_price + add_price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_NOT_ENOUGH);
		return;
	}

	m_role_module_mgr->GetMoney()->UseGold(bid_price, __FUNCTION__);
	RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_DEPOSIT_SUCC, seq, activity->GetActivityOpenDay(), bid_price);
	if (shop_item_info->uid != m_role_module_mgr->GetUid())
	{
		m_param.shop_item_list[seq].bid_price = bid_price;
		RAPaiMaiHangManager::Instance().OnAddRecord(shop_item_info->uid, RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD, seq, activity->GetActivityOpenDay());

		//此时其它玩家才触发延长,玩家自身最高价则不延长
		activity->OnSetEndTimestamp(*shop_item_info, config->GetOtherCfg().delay_time);
		//自动竞拍
		int bid_count = shop_item_info->bid_price + shop_item_info->remain_deposit_gold;
		if (0 != shop_item_info->is_auto_bid && bid_count >= bid_price + shop_item_info->add_price)
		{	
			RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID, seq, activity->GetActivityOpenDay(), bid_price,
				0);
			m_param.shop_item_list[seq].last_bid_gold = bid_price;
			m_param.shop_item_list[seq].RetReset();

			shop_item_info->bid_price = bid_price + shop_item_info->add_price;
			shop_item_info->remain_deposit_gold = bid_count - shop_item_info->bid_price;
			//返还
			this->OnReturnGold(m_role_module_mgr->GetRole(), bid_price, shop_item_cfg->item.item_id, activity->GetActivityOpenDay(), seq, true, m_role_module_mgr->GetUid());
			RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_BID_FAILD, seq, activity->GetActivityOpenDay());
			RAPaiMaiHangManager::Instance().OnAddRecord(shop_item_info->uid, RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID, seq, activity->GetActivityOpenDay(), shop_item_info->bid_price,
				shop_item_info->remain_deposit_gold);
			Role * bid_user = World::GetInstWorld()->GetSceneManager()->GetRole(shop_item_info->uid);
			if (NULL != bid_user)
			{
				RoleActivityPaiMaiHang * ra_pai_mai_hang = static_cast<RoleActivityPaiMaiHang *>
					(bid_user->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
				if (NULL != ra_pai_mai_hang)
				{
					ra_pai_mai_hang->OnSetLastBidPrice(seq, shop_item_info->bid_price);
				}
			}
			else
			{
				OfflineRegisterManager::Instance().OnSetRAPaiMaiHangLastBidInfo(shop_item_info->uid, seq, shop_item_info->bid_price);
			}
		}
		else
		{
			//返还
			if (shop_item_info->uid > 0)
			{
				this->OnReturnGold(shop_item_info->uid, bid_count, shop_item_cfg->item.item_id, activity->GetActivityOpenDay(), seq, true);
			}
			if (bid_price <= bid_count)
			{
				shop_item_info->bid_price = GetMin(bid_count, bid_price);
			}
			else if(bid_count == shop_item_info->bid_price)	//没有寄存
			{
				shop_item_info->bid_price += add_price;
			}
			else  
			{
				shop_item_info->bid_price = bid_count;
			}
			shop_item_info->remain_deposit_gold = bid_price - shop_item_info->bid_price;
			shop_item_info->add_price = add_price;
			shop_item_info->is_auto_bid = m_param.shop_item_list[seq].is_auto_bid;
			shop_item_info->is_show_user_flag = m_param.shop_item_list[seq].is_show_user_flag;
			shop_item_info->uid = m_role_module_mgr->GetUid();
			F_STRNCPY(shop_item_info->role_name, m_role_module_mgr->GetRoleName(), sizeof(shop_item_info->role_name));
			RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_AUTO_BID, seq, activity->GetActivityOpenDay(), shop_item_info->bid_price,
				shop_item_info->remain_deposit_gold);
			m_param.shop_item_list[seq].last_bid_gold = shop_item_info->bid_price;
			m_param.shop_item_list[seq].retrieve_gold = 0;
		}
	}
	else
	{
		shop_item_info->add_price = add_price;
		shop_item_info->remain_deposit_gold += bid_price;
		shop_item_info->is_auto_bid = 1;
		m_param.shop_item_list[seq].bid_price += bid_price;
	}
	m_param.shop_item_list[seq].add_price = add_price;
	m_param.shop_item_list[seq].is_auto_bid = 1;
	activity->SetDirty();
	activity->SendNoticeSingleInfo(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] bid[%d, %d, %d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), activity->GetActivityOpenDay(),
		seq, bid_price);
}

void RoleActivityPaiMaiHang::OnCancelAutoBid(int seq)
{
	if (seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM) return;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (shop_item_info->uid != m_role_module_mgr->GetUid())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_DEPOSIT_NEED_AUTO);
		return;
	}

	if (0 != shop_item_info->is_big_end_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION);
		return;
	}
	if (!activity->IsBidTime(*shop_item_info))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME);
		return;
	}

	//取消自动竞拍则立刻返还剩余寄存
	if (shop_item_info->remain_deposit_gold > 0)
	{
		RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_FETCH_DEPOSIT, seq, activity->GetActivityOpenDay(), shop_item_info->remain_deposit_gold);
		m_role_module_mgr->GetMoney()->AddGold(shop_item_info->remain_deposit_gold, __FUNCTION__);
		m_param.shop_item_list[seq].retrieve_gold += shop_item_info->remain_deposit_gold;
		m_param.shop_item_list[seq].bid_price -= shop_item_info->remain_deposit_gold;
	}
	m_param.shop_item_list[seq].is_auto_bid = 0;
	shop_item_info->is_auto_bid = 0;
	shop_item_info->remain_deposit_gold = 0;
	activity->SetDirty();
	activity->SendNoticeSingleInfo(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);

	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] open_day[%d] seq[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), activity->GetActivityOpenDay(),
		seq);
}

void RoleActivityPaiMaiHang::OnRetrieveGold(int seq, int retrieve_gold, int add_price)
{
	if (seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM || retrieve_gold <= 0) return;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (add_price < config->GetOtherCfg().pre_price || 0 != add_price % config->GetOtherCfg().pre_price)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_pai_mai_hang_bid_set_add_price_notice, add_price, add_price);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
		return;
	}

	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (shop_item_info->uid != m_role_module_mgr->GetUid())
	{
		return;
	}
	if (0 != shop_item_info->is_big_end_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION);
		return;
	}
	if (!activity->IsBidTime(*shop_item_info))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME);
		return;
	}
	if (shop_item_info->remain_deposit_gold <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_BID_GOLD_NOT_ENOUGH);
		return;
	}
	if (shop_item_info->remain_deposit_gold < retrieve_gold)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_RETRIEVE_FAILD);
		return;
	}

	int retrieve_count = retrieve_gold;
	if (shop_item_info->remain_deposit_gold - retrieve_gold < add_price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_DEPOSIT_NOT_ENOUGH);
		shop_item_info->is_auto_bid = 0;
		m_param.shop_item_list[seq].is_auto_bid = 0;
		retrieve_count = shop_item_info->remain_deposit_gold;
	}
	RAPaiMaiHangManager::Instance().OnAddRecord(m_role_module_mgr->GetUid(), RA_PAI_HANG_BANG_RECORD_TYPE_FETCH_DEPOSIT, seq, activity->GetActivityOpenDay(), retrieve_count);
	m_role_module_mgr->GetMoney()->AddGold(retrieve_count, __FUNCTION__);
	shop_item_info->add_price = add_price;
	shop_item_info->remain_deposit_gold -= retrieve_count;
	m_param.shop_item_list[seq].retrieve_gold += retrieve_count;
	m_param.shop_item_list[seq].add_price = add_price;
	m_param.shop_item_list[seq].bid_price -= retrieve_count;

	activity->SetDirty();
	activity->SendNoticeSingleInfo(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);

	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] open_day[%d] seq[%d] retrieve[%d] add_price[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		activity->GetActivityOpenDay(), seq, retrieve_gold, add_price);
}

void RoleActivityPaiMaiHang::OnChangeAddPrice(int seq, int add_price)
{
	if (seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (add_price < config->GetOtherCfg().pre_price || 0 != add_price % config->GetOtherCfg().pre_price)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_pai_mai_hang_bid_set_add_price_notice, add_price, add_price);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
		return;
	}
	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (shop_item_info->uid != m_role_module_mgr->GetUid())
	{
		return;
	}
	if (0 != shop_item_info->is_big_end_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_END_OF_AUCTION);
		return;
	}
	if (!activity->IsBidTime(*shop_item_info))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_PAI_MAI_HANG_NOT_BID_TIME);
		return;
	}
	
	shop_item_info->add_price = add_price;
	m_param.shop_item_list[seq].add_price = add_price;
	activity->SetDirty();
	activity->SendNoticeSingleInfo(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);

	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] open_day[%d] seq[%d] add_price[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		activity->GetActivityOpenDay(), seq, add_price);
}

void RoleActivityPaiMaiHang::OnReturnGold(int role_uid, int gold_num, int item_id, int open_day, int seq, bool is_auto_bid)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_uid);
	this->OnReturnGold(role, gold_num, item_id, open_day, seq, is_auto_bid, role_uid);
}

void RoleActivityPaiMaiHang::OnReturnGold(Role * role, int gold_num, int item_id, int open_day, int seq, bool is_auto_bid, int role_uid)
{
	bool is_need_send_mail = true;
	bool is_reset_succ = false;
	if (NULL != role)
	{
		if (!is_auto_bid)
		{
			role->GetRoleModuleManager()->GetMoney()->AddGold(gold_num, __FUNCTION__);
			gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] uid[%d] bid_price[%d] succ", __FUNCTION__, open_day, seq, role->GetUID(), gold_num);
			is_need_send_mail = false;
		}
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_pai_mai_hang_bid_failed_notice, item_id, seq, (unsigned int)EngineAdapter::Instance().Time());
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
			{
				role->GetRoleModuleManager()->NetSend((const char*)&sm, sendlen);
			}
			length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_pai_mai_hang_bid_failed_content, item_id);
			sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				role->GetRoleModuleManager()->NetSend((const char*)&sm, sendlen);
			}
		}
		RoleActivityPaiMaiHang * ra_pai_mai_hang = static_cast<RoleActivityPaiMaiHang *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
		if (NULL != ra_pai_mai_hang)
		{
			ra_pai_mai_hang->OnRetReset(seq);
			is_reset_succ = true;
		}
	}
	if (is_need_send_mail)
	{
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = gold_num;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_pai_mai_hang_bid_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_pai_mai_hang_bid_failed_content, item_id);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(role_uid, MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s open_day[%d] seq[%d] uid[%d] bid_price[%d] succ", __FUNCTION__, open_day, seq, role_uid, gold_num);
		}
	}
}

void RoleActivityPaiMaiHang::OnSetShowFlag(int seq, char is_show_user_flag)
{
	if(seq < 0 || seq >= MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityPaiMaiHangConfig * config = static_cast<const RandActivityPaiMaiHangConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_PAI_MAI_HANG));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	RandActivityPaiMaiHang * activity = (RandActivityPaiMaiHang *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	if (NULL == activity)
	{
		return;
	}
	const RAPaiMaiHangShopCfg * shop_item_cfg = config->GetShopCfg(ramgr, seq, activity->GetActivityOpenDay());
	if (NULL == shop_item_cfg)
	{
		return;
	}
	RAPaiMaiHangSingleShopItemInfo * shop_item_info = activity->GetShopItemInfo(seq);
	if (NULL == shop_item_info)
	{
		return;
	}
	if (shop_item_info->uid == m_role_module_mgr->GetUid())
	{
		shop_item_info->is_show_user_flag = is_show_user_flag;
		activity->SetDirty();
		activity->SendNoticeSingleInfo(seq);
	}

	m_param.shop_item_list[seq].is_show_user_flag = is_show_user_flag;
	this->SetRoleDataChange();
	this->SendSingleInfo(seq);
	gamelog::g_log_ra_pai_mai_hang.printf(LL_INFO, "%s user[%d, %s] open_day[%d] seq[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), activity->GetActivityOpenDay(),
		seq);
}
