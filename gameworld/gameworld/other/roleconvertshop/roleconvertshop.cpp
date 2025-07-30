#include "roleconvertshop.hpp"
#include "protocol/msgother.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "other/shop/shopconfig.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "gamelog.h"
#include "item/itempool.h"
#include "item/expense/norexitem.hpp"
#include "servercommon/surfacedef.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/surface/surfaceconfig.hpp"
#include "other/surface/surface.hpp"
#include "global/colosseum/gladiator.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "other/prestige/prestigeconfig.hpp"
#include "other/prestige/prestige.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/flyupsystem/flyupsystem.hpp"

RoleConvertShop::RoleConvertShop() : m_mgr(NULL)
{

}

RoleConvertShop::~RoleConvertShop()
{

}

void RoleConvertShop::Init(RoleModuleManager* mgr, const RoleConvertShopParam& param)
{
	m_mgr = mgr;
	m_param = param;

	for (int type = 0; type < CONVERT_TYPE_MAX; ++type)
	{
		this->ClearNonExistItemRecord(type);
	}
}

void RoleConvertShop::GetInitParam(RoleConvertShopParam* out_param)
{
	if (NULL != out_param) *out_param = m_param;
}

void RoleConvertShop::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	this->ResetConvertTimes(CONVERT_LIMIT_TYPE_DAILY);
}

void RoleConvertShop::OnWeekChange()
{
	this->ResetConvertTimes(CONVERT_LIMIT_TYPE_WEEKLY);
}

void RoleConvertShop::OnMonthChange()
{
	this->ResetConvertTimes(CONVERT_LIMIT_TYPE_MONTHLY);
}

void RoleConvertShop::OnHuanJieChange()
{
	this->ResetConvertTimes(CONVERT_LIMIT_TYPE_HUAN_JIE);
}

void RoleConvertShop::OnCloudArenaSeasonChange()
{
	this->ResetConvertTimes(CONVERT_LIMIT_TYPE_CLOUD_ARENA_SEASON);
}

void RoleConvertShop::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void RoleConvertShop::OnResetModuleDataWeek()
{
	
}

void RoleConvertShop::OnResetModuleDataMonth()
{
	
}

void RoleConvertShop::BuyReq(int convert_type, int seq, int buy_num, ItemID item_id)
{
	if (buy_num <= 0) return;
	if (convert_type < 0 || convert_type >= CONVERT_TYPE_MAX) return;
	if (seq < 0 || seq >= CONVERT_TYPE_MAX_ITEM_COUNT) return;

	const ConvertShopItemCfg* cfg = LOGIC_CONFIG->GetShopConfig()->GetConvertShopItemCfg(convert_type, seq);
	if (NULL == cfg)
	{
		m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (CONVERT_TYPE_CLOUD_ARENA == convert_type)
	{
		if (cfg->is_cloud_arena_new_shop && !m_mgr->IsOpenCloudArenaNewShop())
		{
			gamelog::g_log_cloud_arena.printf(LL_WARNING, "RoleConvertShop::BuyReq CloudArenaShop Error1 \
			role[%d,%s] seq[%d] is_new_shop_item[%d] open_new_shop_time[%u]",
				m_mgr->GetUid(), m_mgr->GetRoleName(), seq,
				cfg->is_cloud_arena_new_shop,
				m_mgr->GetCommonData().cloud_arena_new_shop_timestamp);

			m_mgr->NoticeNum(errornum::EN_CLOUD_ARENA_SHOP_ERROR);
			return;
		}
		if (!cfg->is_cloud_arena_new_shop && m_mgr->IsOpenCloudArenaNewShop())
		{
			gamelog::g_log_cloud_arena.printf(LL_WARNING, "RoleConvertShop::BuyReq CloudArenaShop Error2 \
			role[%d,%s] seq[%d] is_new_shop_item[%d] open_new_shop_time[%u]",
				m_mgr->GetUid(), m_mgr->GetRoleName(), seq,
				cfg->is_cloud_arena_new_shop,
				m_mgr->GetCommonData().cloud_arena_new_shop_timestamp);
			
			m_mgr->NoticeNum(errornum::EN_CLOUD_ARENA_SHOP_ERROR);
			return;
		}
	}

	if (item_id > 0 && item_id != cfg->item.item_id)
	{
		m_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_BUY_ITEM_NOT_SAME);
		return;
	}

	if (CONVERT_REQUIRE_TYPE_ROLE_LEVEL == cfg->require_type)
	{
		if (m_mgr->GetRole()->GetLevel() < cfg->require_value)
		{
			m_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_VILLAGE_PRESTIGE == cfg->require_type)
	{
		int village_id = cfg->require_value;
		const PrestigeVillageCfg* village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
		if (NULL == village_cfg)
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		static const int PRESTIGE_REVERENCE = 3; // 声望-崇敬
		if (!m_mgr->GetPrestige()->CheckPrestige(village_id, PRESTIGE_REVERENCE))
		{
			m_mgr->NoticeNum(errornum::EN_PRESTIGE_NOT_ENOUGH);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_COLOSSEUM == cfg->require_type)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			m_mgr->NoticeNum(errornum::EN_SHOP_ITEM_NOT_CROSS_CONVERT_BUY);
			return;
		}
		
		Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(m_mgr->GetRole()->GetUserId());
		if(NULL == gladiator || gladiator->m_highest_rank >= cfg->require_value)
		{
			m_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_COLOSSEUM);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_DAN_ID == cfg->require_type)
	{
		int dan_id = m_mgr->GetCommonData().cloud_arena_season_highest_dan_id;
		if (dan_id < cfg->require_value)
		{
			m_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_DAN);
			return;
		}
	}
	else if (CONVERT_REQUIRE_TYPE_CLOUD_ARENA_SEASON_HIGHEST_RANK == cfg->require_type)
	{
		int rank = m_mgr->GetCommonData().cloud_arena_season_highest_rank;
		if (rank >= cfg->require_value) // 因为服务端rank是从0开始，配置从1开始，所以用>=
		{
			m_mgr->NoticeNum(errornum::EN_ITEM_REQUIRE_CLOUD_ARENA_RANK);
			return;
		}
	}

	//有前置购买条件
	if (-1 != cfg->last_seq)
	{
		const ConvertShopItemCfg* last_cfg = LOGIC_CONFIG->GetShopConfig()->GetConvertShopItemCfg(convert_type, cfg->last_seq);
		if (NULL == last_cfg)
		{
			m_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		const ItemBase * last_item_base = ITEMPOOL->GetItem(last_cfg->item.item_id);
		if (NULL == last_item_base)
		{
			m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		if (m_param.convert_list[convert_type].record_list[cfg->last_seq].convert_times < last_cfg->limit_buy_times)	//last_seq由配置读取检测确保正确
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_role_convert_buy_front_item_not_buy_notice, last_item_base->GetItemName());
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
				}
			}
			return;
		}
	}
	if (cfg->feisheng_count > m_mgr->GetFlyUpSystem()->GetFlyUpFlag())
	{
		m_mgr->NoticeNum(errornum::EN_SHOP_BUY_ITEM_FAILED_BY_SOAR_COUNT_NOT_ENOUGH);
		return;
	}

	if (CONVERT_LIMIT_TYPE_NONE != cfg->limit_type)
	{
		// 如果购买时，发现之前记录的物品ID与配置的不一样, 或版本号不一样，则要重置进度
		if (m_param.convert_list[convert_type].record_list[seq].item_id != cfg->item.item_id
			|| m_param.convert_list[convert_type].record_list[seq].version != cfg->version)
		{
			ConvertItemRecord& record = m_param.convert_list[convert_type].record_list[seq];
			record.convert_times = 0;
			record.item_id = cfg->item.item_id;
			record.open_day = cfg->open_day;
			record.version = cfg->version;

			gamelog::g_log_convert_shop.printf(LL_INFO, "BuyReq ResetRecord role[%d,%s] convert_type[%d] seq[%d] item_id[%d] version[%d]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), convert_type, seq, item_id, cfg->version);
		}
		
		if (m_param.convert_list[convert_type].record_list[seq].convert_times + buy_num > cfg->limit_buy_times)
		{
			m_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_BUY_TIMES_LIMIT);
			return;
		}
	}

	ItemConfigData put_item = cfg->item;
	put_item.num = buy_num * cfg->item.num;
	if (!m_mgr->GetKnapsack()->CheckForPut(put_item.item_id, put_item.num, false))
	{
		m_mgr->GetKnapsack()->SendErrorNotice();
		//m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (cfg->price_type == MONEY_TYPE_CONVERT_ITEM)
	{
		int item_count = m_mgr->GetKnapsack()->Count(cfg->conver_item_id);
		if (item_count <= 0 || item_count * buy_num < cfg->price * buy_num)
		{
			m_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH);
			return;
		}
		if (!m_mgr->GetKnapsack()->ConsumeItem(cfg->conver_item_id, cfg->price * buy_num, __FUNCTION__))
		{
			m_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH);
			return;
		}
	}
	else
	{
		int total_price = cfg->price * buy_num;
		if (total_price > 0 && !this->ConsumeResource(cfg->price_type, total_price, item_id))
		{
			m_mgr->NoticeNum(errornum::EN_CONVERT_SHOP_RESOURCE_NOT_ENOUGH);
			return;
		}
	}

	if (!m_mgr->GetKnapsack()->Put(put_item, PUT_REASON_CONVERT_SHOP))
	{
		m_mgr->GetKnapsack()->SendErrorNotice();
		//m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		gamelog::g_log_convert_shop.printf(LL_INFO, "%s faild! role[%d,%s] convert_type[%d] seq[%d] item_id[%d] buy_num[%d]", __FUNCTION__,
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), convert_type, seq, item_id, buy_num);
		return;
	}

	//printf("ConvertBuy role[%d,%s] item_id[%d] num[%d]\n", m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), put_item.item_id, put_item.num);

	gamelog::g_log_convert_shop.printf(LL_INFO, "BuyReq BuyItem role[%d,%s] convert_type[%d] seq[%d] item_id[%d] buy_num[%d]",
		m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), convert_type, seq, item_id, buy_num);

	if (CONVERT_LIMIT_TYPE_NONE != cfg->limit_type)
	{
		m_param.convert_list[convert_type].record_list[seq].item_id = cfg->item.item_id;
		m_param.convert_list[convert_type].record_list[seq].convert_times += buy_num;
		
		gamelog::g_log_convert_shop.printf(LL_INFO, "BuyReq ChangeRecord role[%d,%s] convert_type[%d] seq[%d] item_id[%d] buy_num[%d] cur_times[%d]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), convert_type, seq, item_id, buy_num, m_param.convert_list[convert_type].record_list[seq].convert_times);

		this->SendRecordChange(convert_type, seq);
	}
}

bool RoleConvertShop::ConsumeSurfaceCoin(int money_type, int consume_num, ItemID item_id)
{
	const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
	if (item_base == NULL || item_base->GetItemType() != I_TYPE_EXPENSE)return false;

	const NorexItem * norex_item = (NorexItem *)item_base;
	int param1 = norex_item->GetParam1();
	if (param1 < 0 || param1 >= SURFACE_MAX_NUM) return false;
	const SurfaceCfg * cfg = LOGIC_CONFIG->GetSurfaceConfig()->GetSurfaceCfg(param1);
	if (NULL == cfg) return false;

	if(!m_mgr->GetKnapsack()->ConsumeItem(cfg->decompose_item, consume_num, __FUNCTION__))return false;
	if(!m_mgr->GetMoney()->UseOtherCoin(consume_num, money_type, __FUNCTION__))return false;

	return true;
}

void RoleConvertShop::SendAllInfo()
{
	this->SendRecordList();
}

void RoleConvertShop::SendRecordList()
{
	Protocol::SCConvertShopBuyRecord msg;

	msg.record_num = 0;
	for (int type = 0; type < CONVERT_TYPE_MAX; ++type)
	{
		int item_num = LOGIC_CONFIG->GetShopConfig()->GetConvertTypeItemNum(type);
		for (int seq = 0; seq < item_num; ++seq)
		{
			if (m_param.convert_list[type].record_list[seq].convert_times > 0)
			{
				msg.record_list[msg.record_num].item_id = m_param.convert_list[type].record_list[seq].item_id;
				msg.record_list[msg.record_num].buy_times = m_param.convert_list[type].record_list[seq].convert_times;
				msg.record_list[msg.record_num].convert_type = type;
				msg.record_list[msg.record_num].seq = seq;
				++msg.record_num;

				if (msg.record_num >= ARRAY_ITEM_COUNT(msg.record_list)) break;
			}		
		}

		if (msg.record_num >= ARRAY_ITEM_COUNT(msg.record_list)) break;
	}

	int send_len = sizeof(msg) - sizeof(msg.record_list) + msg.record_num * sizeof(msg.record_list[0]);
	EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&msg, send_len);
}

void RoleConvertShop::SendRecordChange(int convert_type, int seq)
{
	if (convert_type < 0 || convert_type >= CONVERT_TYPE_MAX) return;
	if (seq < 0 || seq >= CONVERT_TYPE_MAX_ITEM_COUNT) return;

	ConvertItemRecord& record = m_param.convert_list[convert_type].record_list[seq];
	Protocol::SCConvertShopAddRecord msg;
	msg.info.item_id = record.item_id;
	msg.info.buy_times = record.convert_times;
	msg.info.convert_type = convert_type;
	msg.info.seq = seq;

	EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&msg, sizeof(msg));
}

void RoleConvertShop::ClearNonExistItemRecord(int convert_type)
{
	int item_num = LOGIC_CONFIG->GetShopConfig()->GetConvertTypeItemNum(convert_type);
	for (int seq = 0; seq < item_num; ++seq)
	{
		ConvertItemRecord& record = m_param.convert_list[convert_type].record_list[seq];
		const ConvertShopItemCfg* cfg = LOGIC_CONFIG->GetShopConfig()->GetConvertShopItemCfg(convert_type, seq);
		if (NULL != cfg && (cfg->item.item_id != record.item_id || record.version != cfg->version))
		{
			record.convert_times = 0;
			record.item_id = cfg->item.item_id;
			record.open_day = cfg->open_day;
			record.version = cfg->version;

			gamelog::g_log_convert_shop.printf(LL_INFO, "ClearNonExistItemRecord role[%d,%s] convert_type[%d] seq[%d] item_id[%d] version[%d]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), convert_type, seq, record.item_id, record.version);
		}
	}
}

bool RoleConvertShop::ConsumeResource(int money_type, int consume_num, ItemID item_id)
{
	bool ret = false;
	switch (money_type)
	{
	case MONEY_TYPE_GOLD:
		{
			ret = m_mgr->GetMoney()->UseGold(consume_num, __FUNCTION__);
		}
		break;

	case MONEY_TYPE_COIN_BIND:
		{
			ret = m_mgr->GetMoney()->UseCoinBind(consume_num, __FUNCTION__);
		}
		break;

	case MONEY_TYPE_IMMORTAL_COIN:
		{
			ret = m_mgr->GetMoney()->UseImmortalCoin(consume_num, __FUNCTION__);
		}
		break;
	case MONEY_TYPE_SURFACE_COIN:
		{
			ret = ConsumeSurfaceCoin(money_type,consume_num,item_id);
		}
		break;
	default:
		{
			ret = m_mgr->GetMoney()->UseOtherCoin(consume_num, money_type, __FUNCTION__);
		}
		break;
	}

	return ret;
}

void RoleConvertShop::ResetConvertTimes(int limit_type)
{
	bool has_change = false;
	for (int type = 0; type < CONVERT_TYPE_MAX; ++type)
	{
		int item_num = LOGIC_CONFIG->GetShopConfig()->GetConvertTypeItemNum(type);
		for (int seq = 0; seq < item_num; ++seq)
		{
			ConvertItemRecord& record = m_param.convert_list[type].record_list[seq];
			const ConvertShopItemCfg* cfg = LOGIC_CONFIG->GetShopConfig()->GetConvertShopItemCfg(type, seq);
			if (NULL != cfg && limit_type == cfg->limit_type)
			{
				record.convert_times = 0;
				has_change = true;

				gamelog::g_log_convert_shop.printf(LL_INFO, "ResetConvertTimes role[%d,%s] limit_type[%d] convert_type[%d] seq[%d] item_id[%d]",
					m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), limit_type, type, seq, record.item_id);
			}
		}
	}

	if (has_change)
	{
		this->SendRecordList();
	}
}

