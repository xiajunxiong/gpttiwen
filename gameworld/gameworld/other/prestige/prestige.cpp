#include "prestige.hpp"
#include "config/logicconfigmanager.hpp"
#include "prestigeconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "protocol/msgprestige.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "engineadapter.h"
#include "item/itempool.h"
#include "equipment/equipment.h"
#include "other/event/eventhandler.hpp"
#include "other/title/title.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "world.h"
#include "equipment/jewelry.h"
#include "servercommon/logdef.h"
#include "gamelog.h"
#include "task/taskmanager.h"
#include "config/otherconfig/professionconfig.hpp"

#include "other/advancecareer/advancecareer.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"

Prestige::Prestige()
{
}

Prestige::~Prestige()
{
}

void Prestige::Init(RoleModuleManager * role_module_manager, const NewPrestigeParam & param)
{
	m_role_module_mgr = role_module_manager;

	m_param = param;
	
	this->CheckParamData("CheckPoint_111");

	this->CheckBuyCountVersion();
}

void Prestige::GetOtherInitParam(NewPrestigeParam * param)
{
	*param = m_param;
}

void Prestige::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.today_add_value = 0;
	m_param.today_donate_count = 0;
	this->SendDonateInfo();
}

void Prestige::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	this->ResetBuyCount(PRESTIGE_SHOP_LIMIT_TYPE_DAY);

	{
		for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
		{
			m_param.prestige_village_list[i].use_limit = 0;
		}
	}

	this->SendAllInfo();
}

void Prestige::OnWeekChange()
{
	this->ResetBuyCount(PRESTIGE_SHOP_LIMIT_TYPE_WEEK);
	this->SendAllInfo();
}

void Prestige::OnCheckPartnerUpgradeTask()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.new_buy_count); ++i)
	{
		if (m_param.new_buy_count[i] <= 0)
		{
			continue;
		}

		const PrestigeShopCfg *cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetShopByIndexCfg(i);
		if (NULL == cfg || PARTNER_UPGRADE_TASK_PRESTIGE_VILLAGE_ID != cfg->village_id || PARTNER_UPGRADE_TASK_ITEM_ID != cfg->item_id)
		{
			continue;
		}

		m_role_module_mgr->GetTaskManager()->OnSetPartnerUpgradeTaskBuyFlag();
	}
}

bool Prestige::AddPrestige(int village_id, int add, const char* reason /* = NULL */, short * use_num /* = NULL */, int item_id /* = 0 */)
{
	if (village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return false;
	PrestigeVillageNode& village = m_param.prestige_village_list[village_id];

	int shengwangka_up_limit = 0;
	if (NULL != use_num && *use_num > 0)
	{
		const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role_module_mgr->GetRole()->GetProfession(), m_role_module_mgr->GetAdvanceCareer()->GetSubLevel());
		if (NULL == cfg)
		{
			return false;
		}
		shengwangka_up_limit = cfg->shengwangka_up_limit;
		if (village.use_limit + *use_num >= cfg->shengwangka_up_limit && item_id != -1)
		{
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_prestige_use_limit, item_id, cfg->shengwangka_up_limit);
			if (length > 0)
			{
				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
				}
			}
			if (village.use_limit >= cfg->shengwangka_up_limit)
			{
				return false;
			}
			*use_num = (cfg->shengwangka_up_limit - village.use_limit) >= *use_num ? *use_num : (cfg->shengwangka_up_limit - village.use_limit);
		}
		add *= *use_num;
	}

	unsigned char temp_level = village.level;
	int temp_prestige = village.prestige;
	int old_prestige = village.prestige;
	int old_level = village.level;

	const PrestigeUpGradeCfg *cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetUpGradeCfg(village_id, temp_level + 1);
	if (NULL == cfg)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::prestige_over, village_id);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*) &sm, sendlen);
			}
		}
		return false;
	}

	if (temp_prestige > MAX_INT - add) temp_prestige = MAX_INT;
	else temp_prestige += add;

	bool is_up_grade = false;
	while (NULL != cfg && temp_prestige >= cfg->need_prestige)
	{
		is_up_grade = true;
		bool has_title = m_role_module_mgr->GetTitle()->HasTitle(cfg->title_id);
		if (!has_title)
		{
			m_role_module_mgr->GetTitle()->AddTitle(cfg->title_id, __FUNCTION__);
		}
		temp_level++;
		temp_prestige -= cfg->need_prestige;

		cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetUpGradeCfg(village_id, temp_level + 1);
	}
	village.level = temp_level;
	village.prestige = temp_prestige;
	if (NULL != use_num && shengwangka_up_limit > 0 && *use_num > 0)
	{
		village.use_limit += *use_num;
	}

	Protocol::SCPrestigeChangeNotice msg_add_notice;
	msg_add_notice.village_id = village_id;
	msg_add_notice.param = add;
	m_role_module_mgr->NetSend((const char*)&msg_add_notice, sizeof(msg_add_notice));

	// 如果关系升级了
	if (is_up_grade)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_prestige_uplevel_notie,  village_id, temp_level);
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
	}

	EventHandler::Instance().OnAddPrestige(m_role_module_mgr->GetRole(), village_id, add, old_level, village.level);
	this->SendSingleVillageInfo(village_id);

	ROLE_LOG_QUICK10(LOG_TYPE_ROLE_PRESTIGE, m_role_module_mgr->GetRole(), village_id, add, reason, NULL, village.prestige, village.level, old_prestige, old_level, 0);

	return true;
}

bool Prestige::CheckPrestige(int village_id, int level)
{
	if (village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return false;
	PrestigeVillageNode& village = m_param.prestige_village_list[village_id];

	return village.level >= level;
}

int Prestige::GetVillageLevel(int village_id)
{
	if (village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return false;
	PrestigeVillageNode& village = m_param.prestige_village_list[village_id];

	return village.level;
}

int Prestige::GetMaxVillageLevel()
{
	int max_village_level = 0;
	for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
	{
		if (max_village_level < m_param.prestige_village_list[i].level)
		{
			max_village_level = m_param.prestige_village_list[i].level;
		}
	}

	return max_village_level;
}

void Prestige::OnShoping(int index, int num)
{
	this->CheckBuyCountVersion(); // 购买前检查声望商店重置配置版本号

	if (num <= 0) return;
	if (index < 0 || index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM) return;
	const PrestigeShopCfg *cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetShopByIndexCfg(index);
	if (NULL == cfg || cfg->index < 0 || cfg->index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM)return;
	if (cfg->village_id < 0 || cfg->village_id >= PRESTIGE_VILLAGE_MAX_NUM) return;
	PrestigeVillageNode& village = m_param.prestige_village_list[cfg->village_id];
	//////////////// 判断 ////////////////
	if (village.level < cfg->relation)return;
	bool can_buy = false;
	switch (cfg->require_type)
	{
	case PRESTIGE_SHOP_REQUIRE_TYPE_NO:
		can_buy = true;
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_1:
		if (cfg->require_value <= m_role_module_mgr->GetRole()->GetLevel())
		{
			can_buy = true;
		}
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_2:
		can_buy = false;
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_3:
		can_buy = false;
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_4:
		can_buy = false;
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_5:
		can_buy = false;
		break;
	case PRESTIGE_SHOP_REQUIRE_TYPE_6:
		can_buy = false;
		break;
	}

	if (!can_buy)
	{
		return;
	}

	int new_buy_count = m_param.new_buy_count[cfg->index] + num;
	if (cfg->limit_type != PRESTIGE_SHOP_LIMIT_TYPE_BEGIN && new_buy_count > cfg->limit_times)
	{
		return;
	}

	switch (cfg->money_type)
	{
		case CURRENCY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(cfg->price * num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
		case CURRENCY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->ImmortalCoinMoreThan(cfg->price * num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case CURRENCY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(cfg->price * num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		default:
		return;
	}

	ItemConfigData item_configdata(cfg->item_id, 0 != cfg->is_bind, num);
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(1, &item_configdata, false))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}

	//////////////// 消耗 ////////////////
	switch (cfg->money_type)
	{
		case CURRENCY_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(cfg->price * num, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
		}
		break;
		case CURRENCY_TYPE_IMMORTAL_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->UseImmortalCoin(cfg->price * num, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		case CURRENCY_TYPE_COIN_BIND:
		{
			if (!m_role_module_mgr->GetMoney()->UseCoinBind(cfg->price * num, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
		}
		break;
		default:
		return;
	}

	// 次数
	if (cfg->limit_type != PRESTIGE_SHOP_LIMIT_TYPE_BEGIN)
	{
		if (new_buy_count > MAX_UINT8)
		{
			m_param.new_buy_count[cfg->index] = MAX_UINT8;
		}
		else
		{
			m_param.new_buy_count[cfg->index] = new_buy_count;
		}
	}

	gamelog::g_log_prestige.printf(LL_INFO, "Prestige::OnShoping role[%d,%s] index[%d] new_buy_count[%d]",
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), cfg->index, m_param.new_buy_count[cfg->index]);

	//////////////// 给予 ////////////////
	
	if (!m_role_module_mgr->GetKnapsack()->PutList(1, &item_configdata, PUT_REASON_PRESTIGE_BUY))
	{
		m_role_module_mgr->GetKnapsack()->SendErrorNotice();
		return;
	}
	//////////////////////////////////////

	this->SendSingleShopInfo(index);
	EventHandler::Instance().OnPrestigeShopping(m_role_module_mgr->GetRole(), cfg->village_id, cfg->item_id);
}

int Prestige::GetVillageCountByLevelLower(int level)
{
	int ret = 0;
	for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
	{
		if (this->CheckPrestige(i, level))
		{
			ret++;
		}
	}

	return ret;
}

int Prestige::GetMaxPrestigeVillageId()
{
	int max_prestige = 0;
	int village_id = 0;
	for (int i = 1; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
	{
		if (m_param.prestige_village_list[i].prestige > max_prestige && 0 != TaskPool::Instance()->RandPrestigeTaskGroup(i))
		{
			const PrestigeUpGradeCfg * up_grade_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetMaxLevelCfg(i);
			if (NULL == up_grade_cfg) continue;

			if (m_param.prestige_village_list[i].level >= up_grade_cfg->relation)
			{
				continue;
			}

			max_prestige = m_param.prestige_village_list[i].prestige;
			village_id = i;
		}
	}

	if (village_id <= 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM)
	{
		village_id = 2;
	}

	return village_id;
}

void Prestige::OnDonate(int village_id, int column, int index)
{
	static ItemGridData item_data;
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(column, index, &item_data)) return;

	const PrestigeVillageCfg * village_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetVillageCfg(village_id);
	if (NULL == village_cfg) return;

	const ItemBase * item_base = ITEMPOOL->GetItem(item_data.item_id);
	if (NULL == item_base) return;

	const CareerAdvanceCfg* cfg = LOGIC_CONFIG->GetProfessionConfig()->GetCareerAdvanceCfg(m_role_module_mgr->GetRole()->GetProfession(), m_role_module_mgr->GetAdvanceCareer()->GetSubLevel());
	if (NULL == cfg)
	{
		return;
	}
	if (m_param.today_donate_count >= cfg->juanzeng_up_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RESTIGE_ERR_NOT_ENOUGH);
		return;
	}

	const PrestigeDonateCfg *donate_cfg = NULL;
	if (item_base->GetItemType() == I_TYPE_EQUIPMENT)
	{
		Equipment * equipment_base = (Equipment *)item_base;
		if (equipment_base->GetEquipLevel() < village_cfg->donate_min_level || equipment_base->GetEquipLevel() > village_cfg->donate_max_level)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RESTIGE_ERR_ITEM_LEVEL);
			return;
		}
		//检查是否镶嵌了宝石
		NetValueItemParam*  equipment_data = item_data.param;
		if (equipment_data->equip_param.HasStone())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_HAS_STONE_ERR);
			return;
		}

		if (equipment_data->equip_param.HasZhuLing())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_DONATE_ERR_HAS_ZHU_LING_ERR);
			return;
		}

		donate_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetDonateCfg(village_id, equipment_base->GetEquipColor(), equipment_base->GetEquipLevel());
	}
	else if(item_base->GetItemType() == I_TYPE_JEWELRY)
	{
		Jewelry * jewelry_base = (Jewelry *)item_base;
		if (jewelry_base->GetJewelryLevel() < village_cfg->donate_min_level || jewelry_base->GetJewelryLevel() > village_cfg->donate_max_level)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_RESTIGE_ERR_ITEM_LEVEL);
			return;
		}

		//检查是否镶嵌了宝石
		NetValueItemParam*  jewelry_data = item_data.param;
		if (jewelry_data->jewelry_param.HasStone())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_EQUIPMENT_HAS_STONE_ERR);
			return;
		}

		if (jewelry_data->jewelry_param.HasZhuLing())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_DONATE_ERR_HAS_ZHU_LING_ERR);
			return;
		}

		donate_cfg = LOGIC_CONFIG->GetPrestigeConfig()->GetDonateCfg(village_id, jewelry_base->GetEquipColor(), jewelry_base->GetJewelryLevel());
	}
	else return;

	if (NULL == donate_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RESTIGE_ERR_ITEM_QUALITY);
		return;
	}
	//////////////////////消耗物品/////////////////////
	if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))return;
	
	//////////////////////获得声望/////////////////////
	m_param.today_add_value += donate_cfg->reward_prestige;
	m_param.today_donate_count++;
	this->AddPrestige(village_id, donate_cfg->reward_prestige, __FUNCTION__);

	this->SendDonateInfo();
	Protocol::SCPrestigeDonateNotice notice;
	notice.today_donate_count = m_param.today_donate_count;
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &notice, sizeof(notice));

	RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
		(m_role_module_mgr->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
	if (NULL != god_token)
	{
		god_token->OnRAGodTokenAddCount(RA_GOD_TOKEN_DAY_TYPE_PRESTIGE);
	}
}

void Prestige::SendAllInfo()
{
	Protocol::SCPrestigeAllShopInfo shop_info;
	memcpy(shop_info.buy_count, m_param.new_buy_count, sizeof(shop_info.buy_count));
	m_role_module_mgr->NetSend((const char*)&shop_info, sizeof(shop_info));

	Protocol::SCPrestigeAllVillageInfo village_info;
	village_info.count = 0;
	for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
	{
		village_info.prestige_village_list[i] = m_param.prestige_village_list[i];
		village_info.count++;
	}
	m_role_module_mgr->NetSend((const char*)&village_info, sizeof(village_info));

	this->SendDonateInfo();
}

void Prestige::SendSingleVillageInfo(int village_id)
{
	if (village_id < 0 || village_id >= PRESTIGE_VILLAGE_MAX_NUM) return;
	Protocol::SCPrestigeSingleVillageInfo village_info;
	village_info.index = village_id;
	village_info.prestige_village = m_param.prestige_village_list[village_id];
	m_role_module_mgr->NetSend((const char*)&village_info, sizeof(village_info));
}

void Prestige::SendSingleShopInfo(int index)
{
	if (index < 0 || index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM) return;
	Protocol::SCPrestigeSingleShopInfo shop_info;
	shop_info.index = index;
	shop_info.buy_count = m_param.new_buy_count[index];
	m_role_module_mgr->NetSend((const char*)&shop_info, sizeof(shop_info));
}

void Prestige::SendDonateInfo()
{
	Protocol::SCPrestigeDonateInfo info;
	info.today_add_value = m_param.today_add_value;
	info.today_donate_count = m_param.today_donate_count;
	m_role_module_mgr->NetSend((const char*)&info, sizeof(info));
}

void Prestige::CheckBuyCountVersion()
{
	int max_version = m_param.buy_count_version;

	const PrestigeConfig::BuyCountVersionVec& vec = LOGIC_CONFIG->GetPrestigeConfig()->GetBuyCountVersionVec();
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (m_param.buy_count_version >= vec[i].version) continue;
		if (vec[i].index < 0 || vec[i].index >= ARRAY_ITEM_COUNT(m_param.new_buy_count)) continue;

		gamelog::g_log_prestige.printf(LL_INFO, "CheckBuyCountVersion role[%d,%s] index[%d] version[%d] old_buy_count[%d] reset to 0",
			m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), vec[i].index, vec[i].version, m_param.new_buy_count[vec[i].index]);

		max_version = GetMax(max_version, vec[i].version);
		m_param.new_buy_count[vec[i].index] = 0;
	}

	m_param.buy_count_version = static_cast<char>(max_version);
}

void Prestige::ResetBuyCount(int limit_type)
{
	const PrestigeConfig::ShopCfgMap& limit_buy_shop_cfg_map = LOGIC_CONFIG->GetPrestigeConfig()->GetLimitBuyShopCfgMap();

	std::map<int, std::map<int, PrestigeShopCfg> >::const_iterator it = limit_buy_shop_cfg_map.find(limit_type);
	if (it == limit_buy_shop_cfg_map.end()) return;
	for (std::map<int, PrestigeShopCfg>::const_iterator shop_it = it->second.begin(); shop_it != it->second.end(); ++shop_it)
	{
		if (shop_it->second.index < 0 || shop_it->second.index >= NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM)
		{
			continue;
		}

		gamelog::g_log_prestige.printf(LL_INFO, "Prestige::ResetBuyCount role[%d,%s] limit_type[%d] index[%d] old_buy_count[%d] reset to 0",
			m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), limit_type, shop_it->second.index, m_param.new_buy_count[shop_it->second.index]);

		m_param.new_buy_count[shop_it->second.index] = 0;
	}
}

void Prestige::ShowLog(const PrestigeParam& param, const char* source)
{
	gamelog::g_log_prestige.buff_printf("ShowLog OldPrestigeParam source[%s] role[%d,%s] village_info[", 
		source, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());

	for (int i = 0; i < ARRAY_ITEM_COUNT(param.prestige_village_list); ++i)
	{
		if (i > 0)
		{
			gamelog::g_log_prestige.buff_printf(",");
		}
		const PrestigeVillageNode& village = param.prestige_village_list[i];
		gamelog::g_log_prestige.buff_printf("{prestige:%d, level:%d, use_limit:%d}", 
			village.prestige, village.level, village.use_limit);
	}

	gamelog::g_log_prestige.buff_printf("] today_donate_count[%d] today_add_value[%d] transfer_mark[%d] buy_count_version[%d] reserve_sh[%d]",
		param.today_donate_count, param.today_add_value, param.transfer_mark, param.buy_count_version, param.reserver_sh);

	gamelog::g_log_prestige.buff_print("new_buy_count[");
	for (int i = 0; i < ARRAY_ITEM_COUNT(param.new_buy_count); ++i)
	{
		if (i > 0)
		{
			gamelog::g_log_prestige.buff_printf(",");
		}
		gamelog::g_log_prestige.buff_printf("%d", param.new_buy_count[i]);
	}
	gamelog::g_log_prestige.buff_printf("]");
	gamelog::g_log_prestige.commit_buff(LL_INFO);
}

void Prestige::ShowLog(const NewPrestigeParam& param, const char* source)
{
	gamelog::g_log_prestige.buff_printf("ShowLog NewPrestigeParam source[%s] role[%d,%s] village_info[",
		source, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());

	for (int i = 0; i < ARRAY_ITEM_COUNT(param.prestige_village_list); ++i)
	{
		if (i > 0)
		{
			gamelog::g_log_prestige.buff_printf(",");
		}
		const PrestigeVillageNode& village = param.prestige_village_list[i];
		gamelog::g_log_prestige.buff_printf("{prestige:%d, level:%d, use_limit:%d}",
			village.prestige, village.level, village.use_limit);
	}

	gamelog::g_log_prestige.buff_printf("] today_donate_count[%d] today_add_value[%d] transfer_mark[%d] buy_count_version[%d] reserve_sh[%d] reserve_ll0[%lld] reserve_ll1[%lld] reserve_ll2[%lld]",
		param.today_donate_count, param.today_add_value, param.transfer_mark, param.buy_count_version, param.reserver_sh, param.reserve_ll[0], param.reserve_ll[1], param.reserve_ll[2]);
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.reserve_ll) == 3);

	gamelog::g_log_prestige.buff_print("new_buy_count[");
	for (int i = 0; i < ARRAY_ITEM_COUNT(param.new_buy_count); ++i)
	{
		if (i > 0)
		{
			gamelog::g_log_prestige.buff_printf(",");
		}
		gamelog::g_log_prestige.buff_printf("%d", param.new_buy_count[i]);
	}
	gamelog::g_log_prestige.buff_printf("]");
	gamelog::g_log_prestige.commit_buff(LL_INFO);
}

void Prestige::CheckParamData(const char* source)
{
	PrestigeParam old_param;
	memcpy(&old_param, &m_param, sizeof(old_param));
	UNSTD_STATIC_CHECK(sizeof(NewPrestigeParam) >= sizeof(PrestigeParam));

	this->ShowLog(old_param, source);
	this->ShowLog(m_param, source);
}
	
