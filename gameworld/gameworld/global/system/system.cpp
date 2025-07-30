#ifdef __unix
#include "hotfix/hotfix.h"
#include <malloc.h>
#include <signal.h>
#include <unistd.h>
#endif

#include "global/rmibackobjdef.h"

#include "global/worldstatus/worldstatus.hpp"
#include "global/mail/mailmanager.hpp"
#include "globalconfig/globalconfig.h"
#include "servercommon/struct/system/cmdresult.h"

#include "gamelog.h"
#include "internalcomm.h"
#include "world.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/money.h"
#include "other/route/mailroute.hpp"

#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmember.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/cross/crossmanager.h"
#include "global/trademarket/trademarketmanager.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "other/pet/petconfig.hpp"
#include "config/logicconfigmanager.hpp"

#include "systemdef.hpp"
#include "system.hpp"
#include "item/knapsack.h"
#include "global/trademarket/systemrecyclerule.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/monitor/systemmonitor.hpp"
#include "protocol/msgother.h"
#include "other/pet/pet.hpp"
#include "rmibackobjdef.h"
#include "other/event/eventhandler.hpp"
#include "other/trade_credit/trade_credit.hpp"
#include "servercommon/performancecatch.hpp"
#include "recoverclientheartbeatchecktimer.hpp"

#define REGISTER_DBCOMMAND_HANLDER(cmd_str, cmd_handler) { \
	int cmd_hash = GetStringHash(cmd_str); \
	if (m_cmd_handler_map.find(cmd_hash) != m_cmd_handler_map.end()) { \
		printf("REGISTER_DBCOMMAND_HANLDER handler[%s] hash conflit...", cmd_str); \
		assert(0); \
	} \
	m_cmd_handler_map.insert(DBCommandHandlerMap::value_type(cmd_hash, cmd_handler)); \
}

System::System()
	: m_next_get_public_notice(0), m_next_get_cmd(0), m_next_check_timeout_log_time(0)
{
	// memset(m_normal_noticelist, 0, sizeof(m_normal_noticelist)); 该结构有string 不允许memset

	// 注册处理函数
	REGISTER_DBCOMMAND_HANLDER("Mute", &System::Mute);
	REGISTER_DBCOMMAND_HANLDER("Cmd", &System::Cmd);
	REGISTER_DBCOMMAND_HANLDER("CmdToRole", &System::CmdToRole);
	REGISTER_DBCOMMAND_HANLDER("CmdToRoleRename", &System::CmdToRoleRename);
	REGISTER_DBCOMMAND_HANLDER("CmdToRoleKickOut", &System::CmdToRoleKickOut);
	REGISTER_DBCOMMAND_HANLDER("CmdToReloadCrossConfig", &System::CmdToReloadCrossConfig);
	REGISTER_DBCOMMAND_HANLDER("CmdRoleHasNewMessage", &System::CmdRoleHasNewMessage);

	REGISTER_DBCOMMAND_HANLDER("resetnotice", &System::ResetNotice);

	REGISTER_DBCOMMAND_HANLDER("CmdToActivity", &System::CmdToActivity);
	REGISTER_DBCOMMAND_HANLDER("CmdToSetActivityTime", &System::CmdToSetActivityTime);
	REGISTER_DBCOMMAND_HANLDER("ForceSetRandActivityTime", &System::ForceSetRandActivityTime);
	REGISTER_DBCOMMAND_HANLDER("CmdToRandActivity", &System::CmdToRandActivity);
	//REGISTER_DBCOMMAND_HANLDER("ChongzhiCmd", &System::CmdChongzhi); 放到role_cmd中处理
	REGISTER_DBCOMMAND_HANLDER("newnotice", &System::CmdNewNotice);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketUpshelveNormalItem", &System::CmdTradeMarketUpshelveNormalItem);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketUpshelvePet", &System::CmdTradeMarketUpshelvePet);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketUpshelveEquip", &System::CmdTradeMarketUpshelveEquip);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketUpshelveJewel", &System::CmdTradeMarketUpshelveJewel);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketUpshleveRandomPet", &System::CmdTradeMarketUpshleveRandomPet);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketRemoveRecycleRule", &System::CmdTradeMarketRemoveRecycleRule);
	REGISTER_DBCOMMAND_HANLDER("CmdTradeMarketSetRecycleRule", &System::CmdTradeMarketSetRecycleRule);
	REGISTER_DBCOMMAND_HANLDER("CmdSetSystemAutoUpshelve", &System::CmdSetSystemAutoUpshelve);
	REGISTER_DBCOMMAND_HANLDER("CmdUnsetSystemAutoUpshelve", &System::CmdUnsetSystemAutoUpshelve);
	REGISTER_DBCOMMAND_HANLDER("CmdAddTradeCredit", &System::CmdAddTradeCredit);

	REGISTER_DBCOMMAND_HANLDER("AddDataaccessAllowIPPrefix", &System::AddDataaccessAllowIPPrefix);
	REGISTER_DBCOMMAND_HANLDER("SensitiveWords", &System::SensitiveWords);
	REGISTER_DBCOMMAND_HANLDER("Retrieve", &System::Retrieve);
	REGISTER_DBCOMMAND_HANLDER("RetrievePet", &System::CmdRetrievePet);
	REGISTER_DBCOMMAND_HANLDER("QueryRetrievePet", &System::CmdQueryRetrievePet);
	REGISTER_DBCOMMAND_HANLDER("RetrieveExp", &System::CmdRetrieveExp);
	REGISTER_DBCOMMAND_HANLDER("CmdRoute", &System::CmdRoute);
	REGISTER_DBCOMMAND_HANLDER("CmdOrderManagerSetOpen", &System::CmdOrderManagerSetOpen);
	REGISTER_DBCOMMAND_HANLDER("CmdHotfix", &System::CmdHotfix);
	REGISTER_DBCOMMAND_HANLDER("CmdBattleHotfix", &System::CmdBattleHotfix);
	REGISTER_DBCOMMAND_HANLDER("CmdChangeGuildName", &System::CmdChangeGuildName);
	REGISTER_DBCOMMAND_HANLDER("CmdChangeGuildNotice", &System::CmdChangeGuildNotice);
	REGISTER_DBCOMMAND_HANLDER("CmdChangeGuildPost", &System::CmdChangeGuildPost);
	REGISTER_DBCOMMAND_HANLDER("CmdGuildQuit", &System::CmdGuildQuit);
	REGISTER_DBCOMMAND_HANLDER("CmdNoticeClientRestart", &System::CmdNoticeClientRestart);
	REGISTER_DBCOMMAND_HANLDER("CmdCrossHotfix", &System::CmdCrossHotfix);
	REGISTER_DBCOMMAND_HANLDER("RetrieveOtherMoney", &System::RetrieveOtherMoney);
	REGISTER_DBCOMMAND_HANLDER("CmdCloseClientHeartBeatCheck", &System::CmdBlockClientHeartBeatCheck);
	REGISTER_DBCOMMAND_HANLDER("CmdToWorldTeamArena", &System::CmdToWorldTeamArena);
	REGISTER_DBCOMMAND_HANLDER("CmdToAdjWorldTeamArenaTime", &System::CmdToAdjWorldTeamArenaTime);
	REGISTER_DBCOMMAND_HANLDER("CmdToMergeCross", &System::CmdToMergeCross);
	REGISTER_DBCOMMAND_HANLDER("CmdToCloseKuaFuYingXiongLu", &System::CmdToCloseKuaFuYingXiongLu);
	REGISTER_DBCOMMAND_HANLDER("CmdToSetKuaFuYingXiongLuLockFlag", &System::CmdToSetKuaFuYingXiongLuLockFlag);
	REGISTER_DBCOMMAND_HANLDER("CmdMallocTrim", &System::CmdMallocTrim);
	REGISTER_DBCOMMAND_HANLDER("CmdKill", &System::CmdKill);
	REGISTER_DBCOMMAND_HANLDER("CmdSyncInfoToHidden", &System::CmdSyncInfoToHidden);
}

System::~System()
{

}

void System::Init()
{
	m_next_get_public_notice = m_next_get_cmd = EngineAdapter::Instance().Time();
	m_next_check_timeout_log_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_normal_notice.Init(LocalConfig::Instance().GetUniqueServerID());
}



void System::Update(unsigned long interval, time_t now_second)
{
	if (!World::GetInstWorld()->IsAllDataLoadFinish()) 
	{
		return;
	}

	if (now_second > m_next_get_public_notice)
	{
		this->StartPublicNoticeCheck();
		m_next_get_public_notice = now_second + 30;
	}

	if (now_second > m_next_get_cmd)
	{
		this->StartCmdCheck();
		m_next_get_cmd = now_second + GLOBALCONFIG->GetGetCmdTimeInterval_S();
	}

	if (now_second > m_next_check_timeout_log_time)
	{
		this->StartTimeOutLogCheck();
		m_next_check_timeout_log_time = static_cast<unsigned int>(now_second) + 10;
	}

	this->UpdateNormalNotice(now_second);
}

bool System::NormalNoticeIsEmpty(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return m_normal_notice.NormalNoticeIsEmpty();
	}
	else
	{
		NormalPublicNoticeMap::iterator it = m_hidden_notice_map.find(usid);
		if (it == m_hidden_notice_map.end())
		{
			it = m_hidden_notice_map.insert(NormalPublicNoticeMap::value_type(usid, NormalPublicNotice())).first;
			it->second.Init(usid);
		}

		return it->second.NormalNoticeIsEmpty();
	}
}

void System::AddNormalNotice(const UniqueServerID& usid, const PublicNoticeList::NoticeAttr &notice_attr)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_normal_notice.AddNormalNotice(notice_attr);
	}
	else
	{
		NormalPublicNoticeMap::iterator it = m_hidden_notice_map.find(usid);
		if (it == m_hidden_notice_map.end())
		{
			it = m_hidden_notice_map.insert(NormalPublicNoticeMap::value_type(usid, NormalPublicNotice())).first;
			it->second.Init(usid);
		}

		it->second.AddNormalNotice(notice_attr);
	}
}

void System::UpdateNormalNotice(time_t now_second)
{
	unsigned int now = static_cast<unsigned int>(now_second);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_normal_notice.UpdateNormalNotice(now);
	}
	else
	{
		NormalPublicNoticeMap::iterator it = m_hidden_notice_map.begin();
		for (; it != m_hidden_notice_map.end(); ++it)
		{
			it->second.UpdateNormalNotice(now);
		}
	}
}

void System::SyncNoticeFromGame(const UniqueServerID& usid, int count, const PublicNoticeList::NoticeAttr* notice_attr)
{

}

void System::ResetNotice(const UniqueServerID& usid)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_normal_notice.ResetNormalNotice();
		m_next_get_public_notice = EngineAdapter::Instance().Time() - 30;

		crossgameprotocal::GameHiddenCmdResetPublicNotice msg;
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	
		gamelog::g_log_debug.printf(LL_INFO, "System::ResetNotice SyncCmdToHidden");
	}
	else
	{
		NormalPublicNoticeMap::iterator it = m_hidden_notice_map.find(usid);
		if (it != m_hidden_notice_map.end())
		{
			it->second.ResetNormalNotice();

			gamelog::g_log_debug.printf(LL_INFO, "System::ResetNotice Hidden Recv Reset Cmd usid[%d,%d]", usid.plat_type, usid.server_id);
		}
	}
}

bool System::CmdToActivity(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;

	int activity_type = 0;
	if (!(iss >> activity_type).fail())
	{
		if (activity_type > ACTIVITY_TYPE_INVALID && activity_type < ACTIVITY_TYPE_MAX)
		{
			ActivityManager::Instance().OnForceToNextState(activity_type);
			succ = true;
			error_msg = "Succ";
		}
		else
		{
			error_msg = "Invalid Activity Type";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::CmdToSetActivityTime(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	bool succ = false;

	int activity_type = 0;
	unsigned int next_status_switch_timestamp = 0;
	if (!(iss >> activity_type >> next_status_switch_timestamp).fail())
	{
		if (activity_type > ACTIVITY_TYPE_INVALID && activity_type < ACTIVITY_TYPE_MAX)
		{
			ActivityManager::Instance().OnForceToNextState(activity_type, &next_status_switch_timestamp);
			succ = true;
			error_msg = "Succ";
		}
		else
		{
			error_msg = "Invalid Activity Type";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::ForceSetRandActivityTime(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;

	int activity_type = 0, open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;
	unsigned int begin_time = 0, end_time = 0;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

	if (!(iss >> activity_type >> begin_time >> end_time >> open_type).fail())
	{
		if (activity_type >= RAND_ACTIVITY_TYPE_BEGIN && activity_type < RAND_ACTIVITY_TYPE_END)
		{
			if (NULL != ramgr)
			{
				ramgr->OnForceSetActivityTime(activity_type, begin_time, end_time, open_type);
				succ = true;
				error_msg = "Succ";
			}
			else
			{
				succ = false;
				error_msg = "RandActivityManager is NULL";
			}
		}
		else
		{
			error_msg = "RandActivity Type Invalid";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::CmdToRandActivity(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;

	int activity_type = 0, limit_day = 0, open_type = RAND_ACTIVITY_OPEN_TYPE_NORMAL;
	unsigned int begin_time = 0, end_time = 0;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

	if (!(iss >> activity_type >> begin_time >> end_time >> limit_day).fail())
	{
		if (limit_day >= 0 && activity_type >= RAND_ACTIVITY_TYPE_BEGIN && activity_type < RAND_ACTIVITY_TYPE_END)
		{
			if (NULL != ramgr)
			{
				ramgr->OnSetActivityTime(activity_type, begin_time, end_time, limit_day, open_type);
				succ = true;
				error_msg = "Succ";
			}
			else
			{
				succ = false;
				error_msg = "RandActivityManager is NULL";
			}		
		}
		else
		{
			error_msg = "RandActivity Type Invalid or Limit Day Invalid";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::CmdChongzhi(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	/* 放到role_cmd中处理
	bool succ = false;
	int role_id = 0, chongzhi_num = 0;

	if (!(iss >> role_id >> chongzhi_num).fail())
	{
		if (chongzhi_num > 0)
		{
			if (NULL != UserCacheManager::Instance().GetUserNode(role_id))
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
				if (NULL != role)
				{
					role->GetRoleModuleManager()->GetRoleActivityManager()->CheckChongzhiExtraRewardByNum(chongzhi_num);
					//role->GetRoleCrossRandActivity()->CheckChongzhiExtraRewardByNum(chongzhi_num);
				}
				else
				{
					OfflineRegisterManager::Instance().OnChongzhiCmd(IntToUid(role_id), chongzhi_num);
				}

				succ = true;
				error_msg = "Succ";
			}
			else
			{
				error_msg = "Not find user";
			}
		}
		else
		{
			error_msg = "Chongzhi_num is error";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
	*/

	return false;
}

bool System::CmdNewNotice(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	int role_id = 0, notice_type = 0;
	if (!(iss >> notice_type >> role_id).fail())
	{
		if (notice_type > NEWNOTICE_TYPE_INVALID && notice_type < NEWNOTICE_TYPE_MAX)
		{
			this->NewNoticeCmdHelper(notice_type, role_id);
			succ = true;
			error_msg = "Succ";
		}
		else
		{
			error_msg = "Notice_type is error";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::CmdTradeMarketUpshelveNormalItem(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	// 交易行上架普通物品

	bool succ = false;
	ItemID item_id = 0;
	short item_num = 0;
	int price = 0;
	int system_buy_minute = 0;
	int system_offshelve_minute = 0;
	if (!(iss >> item_id >> item_num >> price >> system_buy_minute >> system_offshelve_minute).fail())
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			error_msg = "item not exist";
			return false;
		}
		if (item_num <= 0)
		{
			error_msg = "item num error";
			return false;
		}
		if (price <= 0)
		{
			error_msg = "price error";
			return false;
		}

		static SaleItemParam saleitem_param;
		saleitem_param.Reset();
		saleitem_param.gold_price = price;
		saleitem_param.item_wrapper.item_id = item_id;
		saleitem_param.item_wrapper.num = item_num;
		saleitem_param.item_wrapper.is_bind = 0;
		saleitem_param.item_wrapper.has_param = 0;
		saleitem_param.system_buy_minute_num = system_buy_minute;
		saleitem_param.system_offshelve_minute_num = system_offshelve_minute;

		succ = TradeMarketManager::Instance().AddItemOnSystemShelve(saleitem_param, error_msg);
	}
	else
	{
		error_msg = "param error";
	}

	return succ;
}

bool System::CmdTradeMarketUpshelvePet(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	// 交易行上架宠物

	bool succ = false;
	ItemID item_id = 0;
	int item_num = 0;
	int price = 0;
	int system_buy_minute = 0;
	int system_offshelve_minute = 0;

	char errmsgBuff[200] = { 0 };

	static NetValueItemParam param;
	param.Reset();
	
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.pet_param.passive_list) == 20);
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.pet_param.growth) == 5);
	if (!(iss >> item_id >> item_num >> price >> system_buy_minute >> system_offshelve_minute
		>> param.pet_param.str_level 
		>> param.pet_param.level
		>> param.pet_param.passive_list[0].passive_skill_id
		>> param.pet_param.passive_list[1].passive_skill_id
		>> param.pet_param.passive_list[2].passive_skill_id
		>> param.pet_param.passive_list[3].passive_skill_id
		>> param.pet_param.passive_list[4].passive_skill_id
		>> param.pet_param.passive_list[5].passive_skill_id
		>> param.pet_param.passive_list[6].passive_skill_id
		>> param.pet_param.passive_list[7].passive_skill_id
		>> param.pet_param.passive_list[8].passive_skill_id
		>> param.pet_param.passive_list[9].passive_skill_id
		>> param.pet_param.passive_list[10].passive_skill_id
		>> param.pet_param.passive_list[11].passive_skill_id
		>> param.pet_param.passive_list[12].passive_skill_id
		>> param.pet_param.passive_list[13].passive_skill_id
		>> param.pet_param.passive_list[14].passive_skill_id
		>> param.pet_param.passive_list[15].passive_skill_id
		>> param.pet_param.passive_list[16].passive_skill_id
		>> param.pet_param.passive_list[17].passive_skill_id
		>> param.pet_param.passive_list[18].passive_skill_id
		>> param.pet_param.passive_list[19].passive_skill_id
		>> param.pet_param.comprehend_gift_num
		>> param.pet_param.growth[0]
		>> param.pet_param.growth[1]
		>> param.pet_param.growth[2]
		>> param.pet_param.growth[3]
		>> param.pet_param.growth[4]
		).fail())
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			SNPRINTF(errmsgBuff, sizeof(errmsgBuff), "item not exist item_id[%d]",
				item_id);
			error_msg = errmsgBuff;
			return false;
		}
		if (item_num <= 0)
		{
			error_msg = "item num error";
			return false;
		}
		if (price <= 0)
		{
			error_msg = "price error";
			return false;
		}

		param.pet_param.pet_id = item_id; 
		param.pet_param.pet_growth_version = LOGIC_CONFIG->GetPetConfig()->GetPetCurGrowthVersion(item_id);
		if (!TradeMarketManager::Instance().CheckSystemSaleItemPet(param, error_msg))
		{
			return false;
		}

		for (int i = 0; i < item_num; ++i)
		{
			static SaleItemParam saleitem_param;
			saleitem_param.Reset();
			saleitem_param.gold_price = price;
			saleitem_param.item_wrapper.item_id = item_id;
			saleitem_param.item_wrapper.num = 1;
			saleitem_param.item_wrapper.is_bind = 0;
			saleitem_param.item_wrapper.has_param = 1;
			saleitem_param.system_buy_minute_num = system_buy_minute;
			saleitem_param.system_offshelve_minute_num = system_offshelve_minute;

			param.GetData(saleitem_param.item_wrapper.param_data);

			succ = TradeMarketManager::Instance().AddItemOnSystemShelve(saleitem_param, error_msg);
			if (!succ)
			{
				break;
			}
		}
	}
	else
	{
		error_msg = "param error";
		succ = false;
	}

	return succ;
}

bool System::CmdTradeMarketUpshleveRandomPet(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	ItemID item_id = 0;
	int item_num = 0;
	int price = 0;
	int str_level = 0;
	int system_buy_minute = 0;
	int system_offshelve_minute = 0;

	if (!(iss >> item_id >> item_num >> price >> system_buy_minute >> system_offshelve_minute >> str_level).fail())
	{
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item)
		{
			error_msg = "item not exist";
			return false;
		}
		if (item_num <= 0)
		{
			error_msg = "item num error";
			return false;
		}
		if (price <= 0)
		{
			error_msg = "price error";
			return false;
		}

		for (int i = 0; i < item_num; ++i)
		{
			static SaleItemParam saleitem_param;
			saleitem_param.Reset();
			saleitem_param.gold_price = price;
			saleitem_param.item_wrapper.item_id = item_id;
			saleitem_param.item_wrapper.num = 1;
			saleitem_param.item_wrapper.is_bind = 0;
			saleitem_param.item_wrapper.has_param = 1;
			saleitem_param.system_buy_minute_num = system_buy_minute;
			saleitem_param.system_offshelve_minute_num = system_offshelve_minute;

			static NetValueItemParam param;
			param.Reset();
			if (!LOGIC_CONFIG->GetPetConfig()->RandPetParam(item_id, str_level, false, param))
			{
				error_msg = "Rand Pet Param Failed";
				succ = false;
				break;
			}
			param.GetData(saleitem_param.item_wrapper.param_data);

			succ = TradeMarketManager::Instance().AddItemOnSystemShelve(saleitem_param, error_msg);
			if (!succ)
			{
				break;
			}
		}
	}
	else
	{
		error_msg = "param error";
		succ = false;
	}

	return succ;
}

bool System::CmdTradeMarketUpshelveEquip(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	ItemID item_id = 0;
	int item_num = 0;
	int price = 0;
	int system_buy_minute = 0;
	int system_offshelve_minute = 0;

	static NetValueItemParam param;
	param.Reset();

	// 交易行上架装备
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.equip_param.attr_list) == 6);
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.equip_param.attr_extra_inc) == 6);

	bool param_error = true;
	do 
	{
		if ((iss >> item_id >> item_num >> price >> system_buy_minute >> system_offshelve_minute >> param.equip_param.attr_num).fail())
		{
			break;
		}

		for (int i = 0; i < param.equip_param.attr_num && i < ARRAY_ITEM_COUNT(param.equip_param.attr_list); ++i)
		{
			if ((iss >> param.equip_param.attr_list[i].attr_type >> param.equip_param.attr_list[i].attr_value).fail())
			{
				break;
			}
		}
		if (iss.fail())
		{
			break;
		}

		int actual_phrase_num = 0;
		if ((iss >> param.equip_param.sp_id >> param.equip_param.inc_num >> actual_phrase_num).fail())
		{
			break;
		}

		for (int i = 0; i < actual_phrase_num && i < ARRAY_ITEM_COUNT(param.equip_param.attr_extra_inc); ++i)
		{
			if ((iss >> param.equip_param.attr_extra_inc[i].attr_type >> param.equip_param.attr_extra_inc[i].attr_value).fail())
			{
				break;
			}
			param.equip_param.attr_extra_inc[i].un_lock = 1;
		}
		if (iss.fail())
		{
			break;
		}

		param_error = false;

	} while (0);

	if (param_error)
	{
		error_msg = "param error";
		succ = false;
	}
	
	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item)
	{
		error_msg = "item not exist";
		return false;
	}
	if (item->GetItemType() != I_TYPE_JEWELRY)
	{
		error_msg = "item not jewelry";
		return false;
	}
	if (item_num <= 0)
	{
		error_msg = "item num error";
		return false;
	}
	if (price <= 0)
	{
		error_msg = "price error";
		return false;
	}

	if (!TradeMarketManager::Instance().CheckSystemSaleItemEquip((const Equipment*)item, param, error_msg))
	{
		return false;
	}

	for (int i = 0; i < item_num; ++i)
	{
		static SaleItemParam saleitem_param;
		saleitem_param.Reset();
		saleitem_param.gold_price = price;
		saleitem_param.item_wrapper.item_id = item_id;
		saleitem_param.item_wrapper.num = 1;
		saleitem_param.item_wrapper.is_bind = 0;
		saleitem_param.item_wrapper.has_param = 1;
		saleitem_param.system_buy_minute_num = system_buy_minute;
		saleitem_param.system_offshelve_minute_num = system_offshelve_minute;

		param.GetData(saleitem_param.item_wrapper.param_data);

		succ = TradeMarketManager::Instance().AddItemOnSystemShelve(saleitem_param, error_msg);
		if (!succ)
		{
			break;
		}
	}

	return succ;
}

bool System::CmdTradeMarketUpshelveJewel(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	bool succ = false;
	ItemID item_id = 0;
	int item_num = 0;
	int price = 0;
	int system_buy_minute = 0;
	int system_offshelve_minute = 0;

	static NetValueItemParam param;
	param.Reset();

	// 交易行上架装备
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.jewelry_param.attr_list) == 6);
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(param.jewelry_param.attr_extra_inc) == 6);

	bool param_error = true;
	do
	{
		if ((iss >> item_id >> item_num >> price >> system_buy_minute >> system_offshelve_minute >> param.jewelry_param.attr_num).fail())
		{
			break;
		}

		for (int i = 0; i < param.jewelry_param.attr_num && i < ARRAY_ITEM_COUNT(param.jewelry_param.attr_list); ++i)
		{
			if ((iss >> param.jewelry_param.attr_list[i].attr_type >> param.jewelry_param.attr_list[i].attr_value).fail())
			{
				break;
			}
		}
		if (iss.fail())
		{
			break;
		}

		int actual_phrase_num = 0;
		int sp_id = 0;
		if ((iss >> sp_id >> param.jewelry_param.inc_num >> actual_phrase_num).fail())
		{
			break;
		}

		for (int i = 0; i < actual_phrase_num && i < ARRAY_ITEM_COUNT(param.jewelry_param.attr_extra_inc); ++i)
		{
			if ((iss >> param.jewelry_param.attr_extra_inc[i].attr_type >> param.jewelry_param.attr_extra_inc[i].attr_value).fail())
			{
				break;
			}
			param.jewelry_param.attr_extra_inc[i].un_lock = 1;
		}
		if (iss.fail())
		{
			break;
		}

		param_error = false;

	} while (0);

	if (param_error)
	{
		error_msg = "param error";
		succ = false;
	}

	const ItemBase* item = ITEMPOOL->GetItem(item_id);
	if (NULL == item)
	{
		error_msg = "item not exist";
		return false;
	}
	if (item->GetItemType() != I_TYPE_JEWELRY)
	{
		error_msg = "item not jewelry";
		return false;
	}
	if (item_num <= 0)
	{
		error_msg = "item num error";
		return false;
	}
	if (price <= 0)
	{
		error_msg = "price error";
		return false;
	}

	if (!TradeMarketManager::Instance().CheckSystemSaleItemJewelry((const Jewelry*)item, param, error_msg))
	{
		return false;
	}

	for (int i = 0; i < item_num; ++i)
	{
		static SaleItemParam saleitem_param;
		saleitem_param.Reset();
		saleitem_param.gold_price = price;
		saleitem_param.item_wrapper.item_id = item_id;
		saleitem_param.item_wrapper.num = 1;
		saleitem_param.item_wrapper.is_bind = 0;
		saleitem_param.item_wrapper.has_param = 1;
		saleitem_param.system_buy_minute_num = system_buy_minute;
		saleitem_param.system_offshelve_minute_num = system_offshelve_minute;

		param.GetData(saleitem_param.item_wrapper.param_data);

		succ = TradeMarketManager::Instance().AddItemOnSystemShelve(saleitem_param, error_msg);
		if (!succ)
		{
			break;
		}
	}

	return succ;
}

bool System::CmdTradeMarketSetRecycleRule(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	static RecycleRuleParam param;
	param.Reset();

	bool succ = false;
	if (!(iss >> param.item_id >> param.rule_id >> param.begin_timestamp 
		>> param.end_timestamp >> param.recycle_num_limit
		>> param.recycle_price_limit).fail())
	{
		const ItemBase* item = ITEMPOOL->GetItem(param.item_id);
		if (NULL == item)
		{
			error_msg = "item_not_exist";
			return false;
		}

		if (item->GetItemType() >= I_TYPE_NET_VALUE_ITEM_BEGIN)
		{
			RecycleRuleDetail dd;
			switch (item->GetItemType())
			{
			case I_TYPE_PET:
				{
					if ((iss >> dd.pet.skill_num >> dd.pet.growth_total
						>> dd.pet.growth[ADD_POINT_TYPE_VITALITY]
						>> dd.pet.growth[ADD_POINT_TYPE_STRENGTH]
						>> dd.pet.growth[ADD_POINT_TYPE_POWER]
						>> dd.pet.growth[ADD_POINT_TYPE_AGILITY]
						>> dd.pet.growth[ADD_POINT_TYPE_MAGIC]).fail())
					{
						error_msg = "Pet Detail Error";
						return false;
					}
				}
				break;

			case I_TYPE_EQUIPMENT:
				{					
					if ((iss >> dd.equipment.attr_num).fail())
					{
						error_msg = "Equipment Detail attr_num Error";
						return false;
					}

					if (dd.equipment.attr_num < 0 || dd.equipment.attr_num > EquipmentParam::ATTR_NUM_MAX)
					{
						error_msg = "Equipment Detail attr_num value Error";
						return false;
					}

					for (int i = 0; i < dd.equipment.attr_num; ++i)
					{
						if ((iss >> dd.equipment.attr_list[i].attr_type >> dd.equipment.attr_list[i].attr_value).fail())
						{
							error_msg = "Equipment Detail attr_info Error";
							return false;
						}
					}
		
					if ((iss >> dd.equipment.extra_attr_num).fail())
					{
						error_msg = "Equipment Detail extra_attr_num Error";
						return false;
					}

					if (dd.equipment.extra_attr_num < 0 || dd.equipment.extra_attr_num > EquipmentParam::ATTR_NUM_MAX)
					{
						error_msg = "Equipment Detail extra_attr_num value Error";
						return false;
					}

					for (int i = 0; i < dd.equipment.extra_attr_num; ++i)
					{
						if ((iss >> dd.equipment.extra_attr_list[i].attr_type >> dd.equipment.extra_attr_list[i].attr_value).fail())
						{
							error_msg = "Equipment Detail extra_attr_info Error";
							return false;
						}
					}
				}
				break;

			default:
				{
					error_msg = "Not Support This Type Of Item Yet";
					return false;
				}
				break;
			}
			dd.GetData(param.data);
		}

		succ = TradeMarketManager::Instance().AddRecycleRule(param);
		if (succ)
		{
			error_msg = "Succ";
		}
		else
		{
			error_msg = "error";
		}
	}
	else
	{
		error_msg = "Param Error";
		return false;
	}
	
	return succ;
}

bool System::CmdTradeMarketRemoveRecycleRule(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	long long rule_id = 0;
	if (!(iss >> rule_id).fail())
	{
		succ = TradeMarketManager::Instance().RemoveRecycleRule(rule_id);
		if (succ)
		{
			error_msg = "Succ";
		}
		else
		{
			error_msg = "error";
		}
	}
	else
	{
		error_msg = "Param Error";
		return false;
	}

	return succ;
}

bool System::CmdSetSystemAutoUpshelve(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	static AutoUpshelveParam param;
	param.Reset();
	if ((iss >> param.item_id >> param.upshelve_num >> param.upshelve_price
		>> param.upshelve_interval_m >> param.auto_offshelve_time_m
		>> param.auto_upshelve_limit_num >> param.price_float_percent
		>> param.price_float_limit_percent >> param.begin_timestamp
		>> param.end_timestamp >> param.unique_id 
		>> param.stop_upshelve_world_level
		>> param.start_upshelve_world_level).fail())
	{
		error_msg = "Param Error";
		return false;
	}

	if (param.upshelve_price <= 0)
	{
		error_msg = "Upshelve Price <= 0 Error";
		return false;
	}

	if (param.stop_upshelve_world_level < 0)
	{
		error_msg = "Stop Upshelve Level Param Error";
		return false;
	}

	if (param.start_upshelve_world_level < 0)
	{
		error_msg = "Start Upshelve Level Param Error";
		return false;
	}

	const ItemBase* item = ITEMPOOL->GetItem(param.item_id);
	if (NULL == item)
	{
		error_msg = "Item Not Found";
		return false;
	}

	succ = TradeMarketManager::Instance().GetAutoUpshelve().AddAutoUpshelveInfo(param, false);
	if (!succ)
	{
		error_msg = "AddAutoUpshelveInfo return false";
	}
	else
	{
		error_msg = "Succ";
	}
	
	return succ;
}

bool System::CmdUnsetSystemAutoUpshelve(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	long long unique_id = 0;
	if ((iss >> unique_id).fail())
	{
		error_msg = "Param Error";
		return false;
	}
	succ = TradeMarketManager::Instance().GetAutoUpshelve().RemoveAutoUpshelveInfo(unique_id);
	if (!succ)
	{
		error_msg = "RemoveAutoUpshelveInfo return false";
	}
	else
	{
		error_msg = "Succ";
	}
	return succ;
}

bool System::CmdAddTradeCredit(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	int role_id = 0;
	int credit_num = 0;
	if (!(iss >> role_id >> credit_num).fail())
	{
		int user_status = UserCacheManager::Instance().GetUserOnlineStatus(role_id);
		switch (user_status)
		{
		case UserStatusNode::STATUS_TYPE_OFFLINE:
			{
				OfflineRegisterManager::Instance().OnTradeCreditAddCredit(role_id, credit_num);
				succ = true;
			}
			break;
		case UserStatusNode::STATUS_TYPE_ONLINE:
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
				if (NULL != role)
				{
					succ = role->GetRoleModuleManager()->GetTradeCredit()->AddCredit(credit_num, __FUNCTION__);
				}
			}
			break;

		case UserStatusNode::STATUS_TYPE_CROSS:
			{
				crossgameprotocal::GameHiddenAddCredit msg;
				msg.credit_num = credit_num;
				msg.role_id = role_id;
				InternalComm::Instance().SendToHiddenThroughCross((char *)&msg, sizeof(msg));
			}
			break;
		}

		if (succ)
		{
			error_msg = "Succ";
		}
		else
		{
			error_msg = "error";
		}
	}
	else
	{
		error_msg = "Param Error";
		return false;
	}

	return succ;
}

bool System::SensitiveWords(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	static Protocol::SCNoticeClientReloadSensitiveWords msg;
	World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));

	gamelog::g_log_debug.printf(LL_INFO, "System::SensitiveWords");

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenSensitiveWordsReload ghswr;
		ghswr.plat_type = LocalConfig::Instance().GetPlatType();
		ghswr.server_id = LocalConfig::Instance().GetMergeServerId();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghswr, sizeof(ghswr));
	
		gamelog::g_log_debug.printf(LL_INFO, "System::SensitiveWords Send To Hidden");
	}

	return true;
}

bool System::ResetNotice(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	this->ResetNotice(LocalConfig::Instance().GetUniqueServerID());
	error_msg = "Succ";
	return true;
}


void System::AddTimeOutLogInfo(unsigned int query_timestamp)
{
	unsigned int launch_rmi_request_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 5;
	static TimeOutLogInfo info;
	info.query_timestamp = query_timestamp;
	info.rmi_launch_time = launch_rmi_request_time;
	m_timeout_log_info_queue.push_back(info);	
}

bool System::OnCmd(int cmd_id, const std::string &creator, const std::string &cmd, ARG_OUT std::string& error_msg, long long opid)
{
	// 貌似前两个参数都没什么作用，只有最后参数cmd才是包含了要处理的命令
	// 由于cmd包含了命令字符串，个别可能还有参数，因此定义格式为空格分隔，
	// 比如 forbidtalk 1234 ，其中forbidtalk为命令，1234为参数（role_id),中间用空格隔开

	std::istringstream iss;
	iss.str(cmd);

	std::string c;
	if ((iss >> c).fail())
	{
		error_msg = "StringStream Get Param Failed";
		return false;
	}

	int cmd_hash = GetStringHash(c.length(), c.c_str());
	DBCommandHandlerMap::iterator it = m_cmd_handler_map.find(cmd_hash);
	if (it == m_cmd_handler_map.end())
	{
		gamelog::g_log_cmd.printf(LL_WARNING, "Cmd Not Found: [%d][%s] %s", cmd_id, creator.c_str(), cmd.c_str());
		error_msg = "Cmd Not Found";
		return false;
	}
	else
	{
		DBCommandHandler f = it->second;
		bool ret = (this->*f)(cmd_id, iss, error_msg);
		if (ret)
		{
			gamelog::g_log_cmd.printf(LL_INFO, "Suc [%d][%s] %s",  cmd_id, creator.c_str(), cmd.c_str());
		}
		else
		{
			gamelog::g_log_cmd.printf(LL_INFO, "Fail [%d][%s] %s, error_msg[%s]",  cmd_id, creator.c_str(), cmd.c_str(), error_msg.c_str());
		}

		if (error_msg == "")
		{
			if (ret)
			{
				error_msg = "SUCCESS";
			}
			else
			{
				error_msg = "FAILED";
			}
		}
	
		return ret;
	}

}

void System::NewNoticeCmdHelper(int notice_type, int role_id)
{
	if (NEWNOTICE_TYPE_PERSON_MAIL == notice_type)
	{
		if (role_id > 0) 
		{
			UserMailManager::Instance().LoadUserSystemMailListByCmd(IntToUid(role_id));
		}
	}
	else if (NEWNOTICE_TYPE_QUANFU_MAIL == notice_type)
	{
		UserMailManager::Instance().LoadAllUserSystemMailListByCmd();

		/*
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			std::vector<int> cross_uid_list;
			cross_uid_list.reserve(SystemMonitor::Instance().GetInCrossRoleNum());
			UserCacheManager::Instance().GetUidListByStatusMap(&cross_uid_list, UserStatusNode::STATUS_TYPE_CROSS);
		
			for (size_t i = 0; i < cross_uid_list.size(); ++i)
			{
				UserMailManager::Instance().LoadUserSystemMailListByCmd(IntToUid(cross_uid_list[i]));
			}
		}
		*/
	}
	else if (NEWNOTICE_TYPE_CHONGZHI == notice_type)
	{
		int user_status = UserCacheManager::Instance().GetUserOnlineStatus(role_id);
		switch (user_status)
		{
		case UserStatusNode::STATUS_TYPE_ONLINE:
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
				if (NULL != role)
				{
					role->GetRoleModuleManager()->GetMoney()->ForceGetGold();
					role->GetRoleModuleManager()->GetRoleCmdNow();
				}
			}
			break;

		case UserStatusNode::STATUS_TYPE_CROSS:
			{
				crossgameprotocal::GameHiddenForceGetGold msg;
				msg.server_id = *LocalConfig::Instance().GetAllowServerIdSet().begin();
				msg.role_id = role_id;

				InternalComm::Instance().SendToHiddenThroughCross((char *)&msg, sizeof(msg));
			}
			break;
		}
	}
}

bool System::AddDataaccessAllowIPPrefix(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	std::string new_ip_prefix;
	if (!(iss >> new_ip_prefix).fail())
	{
		RMIAddDataaccessAllowIPPrefixBackObjImpl* impl = new RMIAddDataaccessAllowIPPrefixBackObjImpl();

		RMISystemClient sc;
		sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
		bool ret = sc.AddDataaccessAllowIPPrefix(impl, new_ip_prefix);
		if (!ret)
		{
			error_msg = "Send Add Dataaccess Allow IP Prefix Failed";
		}
		else
		{
			error_msg = "Send Add Dataaccess Allow IP Prefix SUCC";
			succ = true;
		}
	}
	else
	{
		error_msg = "Param Error";
	}
	

	return succ;
}

void System::StartPublicNoticeCheck()
{
	RMIGetPublicNoticeBackObjectImpl *gpnbo = new RMIGetPublicNoticeBackObjectImpl();

	RMISystemClient sc;
	sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
	sc.GetPublicNoticeAsyn(gpnbo);
}

void System::StartCmdCheck()
{
	RMIGetCmdBackObjectImpl *callback_obj = new RMIGetCmdBackObjectImpl();
	callback_obj->query_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	RMISystemClient sc;
	sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
	sc.GetCmd(RMISystemClient::CMD_TYPE_GLOBAL, callback_obj, callback_obj->query_timestamp);
}

void System::StartTimeOutLogCheck()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	while (!m_timeout_log_info_queue.empty())
	{
		const TimeOutLogInfo& info = m_timeout_log_info_queue.front();
		if (now >= info.rmi_launch_time)
		{
			RMIWriteCmdTimeOutLogBackObjectImpl *callback_obj = new RMIWriteCmdTimeOutLogBackObjectImpl();
			callback_obj->query_timestamp = info.query_timestamp;

			RMISystemClient sc;
			sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
			sc.WriteTimeoutLog(RMISystemClient::CMD_TYPE_GLOBAL, callback_obj, callback_obj->query_timestamp);

			m_timeout_log_info_queue.pop_front();
		}
		else
		{
			break;
		}
	}
}

bool System::Cmd(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	std::string c;
	iss >> c;
	if (c == "Reload")
	{
		int config_type = 0;
		// 这个是重读配置的参数，不是所有命令都需要的
		// 比如重读任务配置表，param1可以指定某个任务的id，不指定（为0）则表示重读所有
		int param1 = 0, param2 = 0;
		if (!(iss >> config_type >> param1 >> param2).fail())
		{
			// 这个命令原样发给gameworld处理，至于config_type等代表什么意思，由gameworld解析
			World::GetInstWorld()->GetSceneManager()->OnReloadConfig(config_type, param1, param2);
		}
		else
		{
			error_msg = "Reload read param failed";
			return false;
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return true;
}

bool System::CmdToReloadCrossConfig(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool ret = CrossManager::Instance().ReloadCrossConfig();
	if (ret)
	{
		error_msg = "Succ";
	}
	else
	{
		error_msg = "Reload cross failed";
	}
	return ret;
}

bool System::CmdRetrievePet(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	int role_id = 0;
	int pet_id = 0;
	long long item_unique_id = 0;
	if (!(iss >> role_id >> pet_id >> item_unique_id).fail())
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL != role)
		{
			succ = role->GetRoleModuleManager()->GetPet()->RetrievePet(pet_id, item_unique_id);
			if (succ)
			{
				error_msg = "SUCC";
			}
			else
			{
				error_msg = "RetrieveFailed";
			}
		}
		else
		{
			succ = false;
			
			OfflineRegisterManager::Instance().OnRetrievePet(IntToUid(role_id), cmd_id, pet_id, item_unique_id);

			error_msg = "Role Not Online, OfflineRegister Will Do The Retrieve Once Player Login";
		}
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

void RMIQueryRetrievePetBackObjectImpl::QueryRetrievePetRet(int ret, const ItemListParam &item_list_param)
{
	bool is_first = true;
	std::string opresult = "[";
	for (int i = 0; i < item_list_param.count; ++i)
	{
		if (item_list_param.item_list[i].item_wrapper.item_id != pet_id) continue;
		if (item_unique_id > 0 && item_unique_id != item_list_param.item_list[i].item_wrapper.item_unique_id) continue;

		if (!is_first)
		{
			opresult += ",";
		}
		is_first = false;

		NetValueItemParam param;
		param.SetData(item_list_param.item_list[i].item_wrapper.param_data);

		PetParam& pet_param = param.pet_param;
		char pet_info[1024] = { 0 };
		SNPRINTF(pet_info, sizeof(pet_info), "{\"id\":%d,\"uniqueid\":%lld,\"name\":\"%s\",\"lv\":%d,\"str_lv\":%d,\"growth\":[%d,%d,%d,%d,%d],\"passive\":[%d,%d,%d,%d,%d,%d,%d,%d],\"equipid\":%d,\"equip_skill\":[%d,%d]}",
			pet_id, item_list_param.item_list[i].item_wrapper.item_unique_id, 
			pet_param.name, pet_param.level, pet_param.str_level,
			pet_param.growth[0], pet_param.growth[1], pet_param.growth[2], pet_param.growth[3],
			pet_param.growth[4], pet_param.passive_list[0], pet_param.passive_list[1],
			pet_param.passive_list[2], pet_param.passive_list[3], pet_param.passive_list[4],
			pet_param.passive_list[5], pet_param.passive_list[6], pet_param.passive_list[7],
			pet_param.passive_list[8], pet_param.pet_equipment_id, 
			pet_param.equipment_skill_list[0], pet_param.equipment_skill_list[1]);
		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(pet_param.passive_list) >= 9);
		UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(pet_param.growth) == 5);

		opresult += pet_info;
	}
	opresult += "]";

	static CmdResult result;
	result.count = 1;
	result.result_list[0].cmd_id = cmd_id;
	if (opresult.length() >= sizeof(OPResult))
	{
		gamelog::g_log_cmd.printf(LL_WARNING, "RMIQueryRetrievePetBackObjectImpl::QueryRetrievePetRet [PetInfo Too Long] cmd_id[%d] role_id[%d] pet_id[%d] pet_info[%s]",
			cmd_id, uid, pet_id, opresult.c_str());
		opresult = "ErrorMsg Too Long";
	}
	F_STRNCPY(result.result_list[0].opresult, opresult.c_str(), sizeof(result.result_list[0].opresult));

	RMIWriteCmdResultBackObjectImpl* impl = new RMIWriteCmdResultBackObjectImpl();
	RMISystemClient sc;
	sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
	sc.WriteCmdResult(RMISystemClient::CMD_TYPE_GLOBAL, impl, &result);

	return;
}

bool System::CmdQueryRetrievePet(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	int role_id = 0;
	int pet_id = 0;
	if (!(iss >> role_id >> pet_id).fail())
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL != role)
		{
			succ = role->GetRoleModuleManager()->GetPet()->QueryPetInfo(pet_id, error_msg);
			if (!succ)
			{
				error_msg = "Pet Not Found";
			}
		}
		else
		{
			RMIQueryRetrievePetBackObjectImpl* impl = new RMIQueryRetrievePetBackObjectImpl();
			impl->cmd_id = cmd_id;
			impl->item_unique_id = 0LL;
			impl->pet_id = pet_id;
			impl->uid = role_id;
			
			const rmi::Session& session = InternalComm::Instance().GetDBHandler().GetRoleDB();
			RMIRoleClient rc;
			rc.__bind_session(session);
			if (rc.QueryRetrievePetAsyn(role_id, pet_id, 0LL, impl))
			{
				error_msg = "Role Offline, Proceed Offline Query, Wait One Minute To Refresh";
			}
			else
			{
				error_msg = "Role Offline, Proceed Offline Query FAILED! Please Try Again Later";
			}
		}
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdRetrieveExp(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	bool succ = false;
	int role_id = 0;
	long long retrieve_exp = 0;
	if (!(iss >> role_id >> retrieve_exp).fail())
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (NULL != role)
		{
			role->GmRetrieveExp(retrieve_exp);
			succ = true;
		}
		else
		{
			succ = false;

			OfflineRegisterManager::Instance().OnRetrieveExp(role_id, retrieve_exp);
			error_msg = "Role Not Online, OfflineRegister Will Do The Retrieve Once Player Login";
		}
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdRoute(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	bool succ = false;
	int role_id = 0;
	if (!(iss >> role_id).fail())
	{
		succ = true;

		Protocol::SCCmdRoute protcol;
		protcol.route_content_length = 0;

		if ((int)iss.str().size() < ARRAY_LENGTH(protcol.route_content))
		{
			protcol.route_content_length = (int)iss.str().size();
			STRNCPY(protcol.route_content, iss.str().c_str(), ARRAY_LENGTH(protcol.route_content));
		}

		int send_len = sizeof(protcol) - ARRAY_LENGTH(protcol.route_content) + protcol.route_content_length;

		if (role_id != 0)
		{
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
			if (NULL != role)
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protcol, send_len);
			}
			else
			{
				int state = UserCacheManager::Instance().GetUserOnlineStatus(role_id);
				if (state == UserStatusNode::STATUS_TYPE_CROSS)
				{
					InternalComm::Instance().SendToHiddenUserThroughCross(role_id, (const char *)&protcol, send_len);
				}
			}
		}
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdOrderManagerSetOpen(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	bool succ = false;
	int is_open = 0;
	if (!(iss >> is_open).fail())
	{
		OrderManager::Instance().SystemSetIsOpen(is_open > 0);

		succ = true;
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
#ifdef __unix
	bool succ = false;
	std::string so_file_name;
	std::string fix_symbol;
	if (!(iss >> so_file_name >> fix_symbol).fail())
	{
		PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_HOTFIX);
		succ = cmd_fix_2(HOTFIX_GAMEWORLD_PATH + so_file_name, fix_symbol, error_msg);
		long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_HOTFIX);
		gamelog::g_log_world.printf(LL_MAINTANCE, "System::CmdHotfix so_file[%s] symbol[%s] is_succ[%s] errormsg[%s] elapse[%lld]us=[%lld]ms",
			so_file_name.c_str(), fix_symbol.c_str(), succ ? "TRUE" : "FALSE", error_msg.c_str(), elapse_us, elapse_us / 1000);
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;

#else

	error_msg = "Windows Don't Support Hotfix Yet...Can You Make It Happen?";
	return false;

#endif
}

bool System::CmdBattleHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
#ifdef __unix
	bool succ = false;
	std::string so_file_name;
	std::string fix_symbol;
	if (!(iss >> so_file_name >> fix_symbol).fail())
	{
		battlegameprotocol::GameBattleHotfix req;
		F_STRNCPY(req.so_file_name, so_file_name.c_str(), sizeof(req.so_file_name));
		F_STRNCPY(req.symbol_str, fix_symbol.c_str(), sizeof(req.symbol_str));
		succ = World::GetInstWorld()->SendMsgToAllBattle((const char*)&req, sizeof(req));
		error_msg = "Transfered Hotfix Command To All Battle Server";
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;

#else

	error_msg = "Windows Don't Support Hotfix Yet...Can You Make It Happen?";
	return false;

#endif
}

bool System::CmdCrossHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
#ifdef __unix
	bool succ = false;
	std::string so_file_name;
	std::string fix_symbol;
	if (!(iss >> so_file_name >> fix_symbol).fail())
	{
		crossgameprotocal::GameCrossHotfix req;
		F_STRNCPY(req.so_file_name, so_file_name.c_str(), sizeof(req.so_file_name));
		F_STRNCPY(req.symbol_str, fix_symbol.c_str(), sizeof(req.symbol_str));
		succ = InternalComm::Instance().SendToCross((const char*)&req, sizeof(req));
		error_msg = "Transfered Hotfix Command To All Cross Server";
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;

#else

	error_msg = "Windows Don't Support Hotfix Yet...Can You Make It Happen?";
	return false;

#endif
}

bool System::CmdBlockClientHeartBeatCheck(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	// 通知全体玩家关闭心跳
	// 设置Timer，1分钟后让玩家恢复心跳

	SCClientHeartbeatCheckModify msg;
	msg.is_open = 0;

	World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));

	gamelog::g_log_world.printf(LL_INFO, "System::CmdBlockClientHeartBeatCheck close");

	RecoverClientHeartbeatCheckTimer* timer = new RecoverClientHeartbeatCheckTimer();
	EngineAdapter::Instance().CreateTimer(60000, timer); // 60秒后恢复

	return true;
}

bool System::CmdToWorldTeamArena(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		error_msg = "not in hidden server";
		return false;
	}
	bool succ = false;
	int cur_season = 0;
	unsigned int sign_up_end_timestamp = 0;
	if (!(iss >> cur_season >> sign_up_end_timestamp).fail())
	{
		succ = true;
		error_msg = "SUCC";
		WorldTeamArenaRankManager::Instance().CmdRankToSignUpList(cur_season, sign_up_end_timestamp);
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdToAdjWorldTeamArenaTime(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		error_msg = "not in hidden server";
		return false;
	}
	bool succ = false;
	unsigned int sign_up_start_timestamp = 0, sign_up_end_timestamp = 0, next_status_switch_timestamp = 0;
	if (!(iss >> sign_up_start_timestamp >> sign_up_end_timestamp >> next_status_switch_timestamp).fail())
	{
		ActivityWorldTeamArena* activity = (ActivityWorldTeamArena*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
		if (NULL == activity)
		{
			succ = false;
			error_msg = "not find activity";
		}
		else
		{
			succ = true;
			error_msg = "SUCC";
			activity->CmdToAdjTime(sign_up_start_timestamp, sign_up_end_timestamp, next_status_switch_timestamp);
		}
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdToMergeCross(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		error_msg = "not in hidden server";
		return false;
	}
	bool succ = false;
	KuaFuYingXiongLuManager::Instance().OnReopen();
	succ = true;
	error_msg = "SUCC";

	return succ;
}

bool System::CmdToCloseKuaFuYingXiongLu(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		error_msg = "not in hidden server";
		return false;
	}
	bool succ = false;
	KuaFuYingXiongLuManager::Instance().OnClose();
	succ = true;
	error_msg = "SUCC";

	return succ;
}

bool System::CmdToSetKuaFuYingXiongLuLockFlag(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		error_msg = "not in hidden server";
		return false;
	}
	bool succ = false;
	int lock_flag = 0;
	if (!(iss >> lock_flag).fail())
	{
		succ = true;
		error_msg = "SUCC";
		KuaFuYingXiongLuManager::Instance().OnSetLockFlag((0 != lock_flag));
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;
}

bool System::CmdMallocTrim(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
#ifdef __unix
	{
		PerformanceCatch::Instance().GetElapseUsBegin(PERFORMANCE_TEST_MALLOC_TRIM);
		succ = true;
		error_msg = "SUCC";
		malloc_trim(0);
		long long elapse_us = PerformanceCatch::Instance().GetElapseUsEnd(PERFORMANCE_TEST_MALLOC_TRIM);
		gamelog::g_log_world.printf(LL_MAINTANCE, "System::CmdMallocTrim elapse[%lld]us=[%lld]ms", elapse_us, elapse_us / 1000);
	}
#else
	{
		succ = false;
		error_msg = "Only Work On Linux";
	}
#endif
	return succ;
}

bool System::CmdKill(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
#ifdef _MSC_VER
	assert(0);
#else
	kill(getpid(), SIGABRT);
#endif

	return true;
}

bool System::CmdSyncInfoToHidden(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	bool succ = false;
	int type = -1;
	if (!(iss >> type ).fail())
	{
		succ = true;
		CrossManager::Instance().CMDSyncInfoToHidden(type);
		error_msg = "SUCC";
	}
	else
	{
		succ = false;
		error_msg = "Param Error";
	}

	return succ;

}

bool System::CmdChangeGuildName(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	GuildID guild_id = 0;
	std::string name;

	bool is_succ = false;
	if (!(iss >> guild_id >> name).fail())
	{
		Guild *guild = GuildManager::Instance().GetGuild(guild_id);
		if (NULL != guild)
		{
			if (name.length() >= sizeof(GuildName))
			{
				error_msg = "Name Too Long";
			}
			else
			{
				guild->GmSetName(name.c_str());
				is_succ = true;
				error_msg = "Succ";
			}
		}
		else
		{
			error_msg = "Role Not Found";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::CmdChangeGuildNotice(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	GuildID guild_id = 0;
	std::string notice;

	bool is_succ = false;
	if (!(iss >> guild_id >> notice).fail())
	{
		Guild *guild = GuildManager::Instance().GetGuild(guild_id);
		if (NULL != guild)
		{
			if (notice.length() >= sizeof(GuildNotice))
			{
				error_msg = "Name Too Long";
			}
			else
			{
				guild->GmSetNotice(notice.c_str());
				is_succ = true;
				error_msg = "Succ";
			}
		}
		else
		{
			error_msg = "Role Not Found";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::CmdChangeGuildPost(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	int post = 0;

	bool is_succ = false;
	if (!(iss >> role_id >> post).fail())
	{
		if (post < GUILD_POST_PATRIARCH || post >= GUILD_POST_MAX)
		{
			error_msg = "post Error";
			return is_succ;
		}

		Guild *guild = GuildManager::Instance().GetGuild(IntToUid(role_id));
		if (NULL != guild)
		{
			if (GUILD_POST_PATRIARCH == post)
			{
				GuildMember * old_patriarch	= guild->GetMemberManager()->GetMember(guild->GetMemberManager()->GetPatriarchUserID());
				GuildMember * new_patriarch = guild->GetMemberManager()->GetMember(IntToUid(role_id));

				if (NULL != new_patriarch)
				{
					if (NULL != old_patriarch)		// 找不到就认为当前没有族长
					{
						old_patriarch->SetPost(GUILD_POST_MEMBER);
					}

					new_patriarch->SetPost(GUILD_POST_PATRIARCH);
					guild->AddEventhandler(new_patriarch->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, GUILD_POST_PATRIARCH);

					guild->Save(); //!< 转让族长需要立马保存 \bug #43281
					is_succ = true;
					error_msg = "Succ";
				}
				else
				{
					is_succ = false;
					error_msg = "Role Not Found";
				}
			}
			else
			{
				GuildMember * new_patriarch = guild->GetMemberManager()->GetMember(IntToUid(role_id));
				if (NULL != new_patriarch)
				{
					new_patriarch->SetPost(post);
					guild->AddEventhandler(new_patriarch->GetName(), EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_APPOINTMENT, post);
					is_succ = true;
					error_msg = "Succ";
				}
				else
				{
					is_succ = false;
					error_msg = "New Patriarch Not Found";
				}
			}
		}
		else
		{
			error_msg = "Guild Not Found";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::CmdGuildQuit(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;

	bool is_succ = false;
	if (!(iss >> role_id ).fail())
	{
		Guild *guild = GuildManager::Instance().GetGuild(IntToUid(role_id));
		if (NULL != guild)
		{
			GuildMemberManager & mem_mgr = guild->GetMemberManagerRef();

			if (mem_mgr.GetMemberCount() <= 1)
			{// 解散
				GuildManager::Instance().GmDismissGuild(guild->GetGuildID());
			}
			else
			{// 退出
				mem_mgr.RemoveMember(IntToUid(role_id), Protocol::SCQuitGuild::QUIT);
			}
			is_succ =  true;
			error_msg = "Succ";
		}
		else
		{
			error_msg = "Guild Not Found";
		}
	}
	return is_succ;
}

bool System::CmdNoticeClientRestart(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	std::string version_msg;
	if (!(iss >> version_msg).fail())
	{
		Protocol::SCNoticeClientRestart notice;
		memset(notice.version_msg, 0, sizeof(notice.version_msg));
		F_STRNCPY(notice.version_msg, version_msg.c_str(), sizeof(notice.version_msg));
		World::GetInstWorld()->SendToAllGateway((const char*)&notice, sizeof(notice));

		error_msg = "Succ";
	}
	else
	{
		error_msg = "Version Msg Read Failed";
	}
	
	return true;
}

bool System::CmdToRole(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	int effect_type = 0, param = 0, param1;

	bool is_succ = false;
	if (!(iss >> role_id >> effect_type >> param >> param1).fail())
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			bool ret = this->OnCmdToRole(role, effect_type, param, param1);

			gamelog::g_log_cmd.printf(LL_INFO, "[CmdToRole %s! User[%d, %s], effect_type:%d, effect_param:%d, effect_param1:%d]",
				(ret ? "Succ" : "Fail"), role_id, role->GetName(), effect_type, param, param1);
			
			is_succ = ret;
			if (is_succ)
			{
				error_msg = "Succ";
			}
			else
			{
				error_msg = "Fail";
			}
		}
		else if (UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(IntToUid(role_id)))
		{
			crossgameprotocal::GameHiddenCmdToRole ghcmr;
			ghcmr.plat_type = LocalConfig::Instance().GetPlatType();
			ghcmr.server_id = LocalConfig::Instance().GetMergeServerId();
			ghcmr.role_id = role_id;
			ghcmr.effect_type = effect_type;
			ghcmr.effect_param = param;
			ghcmr.effect_param1 = param1;

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghcmr, sizeof(ghcmr));

			is_succ = true;
			error_msg = "Succ";
		}
		else
		{	
			bool ret = this->OnCmdToRole(role_id, effect_type, param, param1);
			if (!ret)
			{
				ret = this->OnCmdToOfflineRegister(role_id, effect_type, param, param1);
			}
			is_succ = ret;
			if (is_succ)
			{
				error_msg = "Succ";
			}
			else
			{
				error_msg = "Fail";
			}
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::OnCmdToRole(Role * role, int effect_type, int effect_param, int effect_param1)
{
	bool ret = false;
	switch (effect_type)
	{
		case systemcmd::EFFECT_AUTHORITY:
			{
				ret = role->SetAuthorityType(effect_param, true);
			}
			break;

	
	}

	return ret;
}

bool System::OnCmdToOfflineRegister(int uid, int effect_type, int effect_param, int effect_param1)
{
	bool ret = false;
	switch (effect_type)
	{
		case systemcmd::EFFECT_AUTHORITY:
		{
			if (effect_param < Role::AUTHORITY_TYPE_INVALID || effect_param >= Role::AUTHORITY_TYPE_MAX)
			{
				break;
			}
			ret = OfflineRegisterManager::Instance().OnCmdSetAuthorityType(uid, effect_type, effect_param, effect_param1);
		}
		break;
	}

	return ret;
}

bool System::OnCmdToRole(int role_id, int effect_type, int effect_param, int effect_param1)
{
	bool ret = false;
	switch (effect_type)
	{
		case systemcmd::EFFECT_AUTHORITY:
		{
			if (effect_param < Role::AUTHORITY_TYPE_INVALID || effect_param >= Role::AUTHORITY_TYPE_MAX)
			{
				break;
			}
			RMIRoleCmdSetAuthorityBackObjectImpl* impl = new RMIRoleCmdSetAuthorityBackObjectImpl();
			impl->role_id = role_id;

			const rmi::Session& session = InternalComm::Instance().GetDBHandler().GetRoleDB();
			RMIRoleClient rc;
			rc.__bind_session(session);
			ret = rc.RoleCmdToRoleSetAuthorityAsyn(role_id, effect_param, impl);
		}
		break;
	}

	return false;
}

bool System::Mute(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	unsigned int forbid_time = 0;
	int forbid_talk_type = 0;

	bool succ = false;

	if (!(iss >> role_id >> forbid_time).fail())
	{
		if ((iss >> forbid_talk_type).fail() || forbid_talk_type < 0 || forbid_talk_type >= FORBID_TALK_TYPE_MAX)
		{
			forbid_talk_type = FORBID_TALK_TYPE_DEFAULT;
		}

		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			role->ForbidTalk((time_t)forbid_time, "Command", "Command", forbid_talk_type);
		}
		else if (UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(IntToUid(role_id)))
		{
			crossgameprotocal::GameHiddenCmdMuteRole ghcmr;
			ghcmr.plat_type = LocalConfig::Instance().GetPlatType();
			ghcmr.server_id = LocalConfig::Instance().GetMergeServerId();
			ghcmr.role_id = role_id;
			ghcmr.mute_second = forbid_time;
			ghcmr.forbid_talk_type = forbid_talk_type;

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghcmr, sizeof(ghcmr));
		}
		else
		{
			
			RMIRoleMuteBackObjectImpl* impl = new RMIRoleMuteBackObjectImpl();
			impl->role_id = role_id;

			const rmi::Session& session = InternalComm::Instance().GetDBHandler().GetRoleDB();
			RMIRoleClient rc;
			rc.__bind_session(session);
			if (!rc.RoleMuteAsyn(role_id, forbid_time, forbid_talk_type, impl))
			{
				OfflineRegisterManager::Instance().OnForbidTalk(IntToUid(role_id), forbid_time, forbid_talk_type);
			}
		}
	
		succ = true;
		error_msg = "Succ";

		EventHandler::Instance().OnMutePlayer(role_id, forbid_time);
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::CmdToRoleRename(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	std::string name;

	bool is_succ = false;
	int is_use_item = false;
	if (!(iss >> role_id >> name >> is_use_item).fail())
	{
		GameName new_name; memset(new_name, 0, sizeof(new_name));

		STRNCPY(new_name, name.c_str(), sizeof(new_name));
		new_name[sizeof(new_name) - 1] = 0;
		if (name.length() >= sizeof(GameName))
		{
			error_msg = "Name Too Long";
		}
		else
		{
			Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
			if (NULL != role)
			{
				if (is_use_item == 1)
				{
					role->OnResetNameReq(new_name);
				}
				else
				{
					role->ResetName(new_name);
				}

				is_succ = true;
				error_msg = "Succ";
			}
			else
			{
				int ret = OfflineRegisterManager::Instance().OnCmdChangeRoleName(IntToUid(role_id), new_name,is_use_item == 1);
				if (ret != 0)
				{
					error_msg = "Role Not Found";
				}
				else
				{
					is_succ = true;
					error_msg = "Succ";
				}
			}
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::CmdToRoleKickOut(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	bool is_succ = false;
	if (!(iss >> role_id).fail())
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), 0, "KickOut");
			is_succ = true;
			error_msg = "Succ";
		}
		else if (UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(IntToUid(role_id)))
		{
			crossgameprotocal::GameHiddenCmdKickRole ghckr;
			ghckr.plat_type = LocalConfig::Instance().GetPlatType();
			ghckr.server_id = LocalConfig::Instance().GetMergeServerId();
			ghckr.role_id = role_id;

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghckr, sizeof(ghckr));
		}
		else
		{
			error_msg = "Role Not Found";
		}
	}
	else
	{
		error_msg = "read param failed";
	}

	return is_succ;
}

bool System::Retrieve(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	Int64 unbind_gold = 0;
	Int64 bind_gold = 0;
	Int64 bind_coin = 0;
	Int64 spirit_jade = 0;
	int vip_exp = 0;
	ItemID item_id = 0;
	short item_num = 0;
	short retrieve_reason = 0;
	bool succ = false;
	

	if (!(iss >> role_id >> unbind_gold >> bind_gold >> bind_coin >> spirit_jade >> vip_exp >> item_id >> item_num >> retrieve_reason).fail())
	{
		//调用方法通知
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetMoney()->RetrieveMoney(unbind_gold, bind_gold, bind_coin, spirit_jade, retrieve_reason);
			/*role->GetRoleModuleManager()->GetVip()->RetrieveVipExp(vip_exp, retrieve_reason);*/
			role->GetRoleModuleManager()->GetKnapsack()->RetrieveItem(item_id, item_num, retrieve_reason);
		}
		else
		{
			UserID user_id = IntToUid(role_id);
			OfflineRegisterManager::Instance().OnRetrieve(user_id, unbind_gold, bind_gold, bind_coin, spirit_jade, vip_exp, item_id, item_num, retrieve_reason);
		}

		succ = true;
		error_msg = "Succ";
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}


bool System::CmdRoleHasNewMessage(int cmd_id, std::istringstream& iss, std::string& error_msg)
{
	int role_id = 0;
	int msg_count = 0;
	bool succ = false;

	if (!(iss >> role_id >> msg_count).fail())
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != role)
		{
			Protocol::SCNoticeClientNewMessageFromCustomService notice;
			notice.msg_count = msg_count;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
		}
		else if (UserStatusNode::STATUS_TYPE_CROSS == UserCacheManager::Instance().GetUserOnlineStatus(IntToUid(role_id)))
		{
			crossgameprotocal::GameHiddenCmdNoticeNewMessage ghcnnm;
			ghcnnm.plat_type = LocalConfig::Instance().GetPlatType();
			ghcnnm.server_id = LocalConfig::Instance().GetMergeServerId();
			ghcnnm.role_id = role_id;
			ghcnnm.msg_count = msg_count;

			InternalComm::Instance().SendToHiddenThroughCross((const char*)&ghcnnm, sizeof(ghcnnm));
		}
		else
		{
			error_msg = "Role Not Online";
		}

		succ = true;
		error_msg = "Succ";
	}
	else
	{
		error_msg = "read param failed";
	}

	return succ;
}

bool System::RetrieveOtherMoney(int cmd_id, std::istringstream & iss, std::string & error_msg)
{
	int role_id = 0;
	short retrieve_reason = 0;
	Int64 receive_iss[MONEY_TYPE_MAX + 2];
	memset(receive_iss, 0, sizeof(receive_iss));
	MoneyOtherParam retrieve_other_currency;
	retrieve_other_currency.Reset();
	bool succ = false;

	int n = 0;
	while (n < MONEY_TYPE_MAX + 2 && !(iss >> receive_iss[n]).fail())
	{
		n++;
	}

	if(n != MONEY_TYPE_MAX + 2)
	{
		error_msg = "read param failed";
		return succ;
	}

	for (int i = 0; i < MONEY_TYPE_MAX + 2; ++i)		// 分别获取参数
	{
		if (i == 0)
		{
			role_id = receive_iss[i];
			continue;
		}
		else if (i == MONEY_TYPE_MAX + 1)
		{
			retrieve_reason = receive_iss[i];
			break;
		}
		retrieve_other_currency.other_currency[i - 1] = receive_iss[i];
	}

	//调用方法通知
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetMoney()->RetrieveMoney(retrieve_other_currency, retrieve_reason);
	}
	else
	{
		UserID user_id = IntToUid(role_id);
		OfflineRegisterManager::Instance().OnRetrieveOtherMoney(user_id, retrieve_other_currency, retrieve_reason);
	}

	succ = true;
	error_msg = "Succ";
	
	return succ;
}

System & System::Instance()
{
	static System sys;
	return sys;
}


void RMIGetPublicNoticeBackObjectImpl::GetPublicNoticeRet(int ret, const PublicNoticeList &notice_list)
{
	if (ret != 0) return;
	if (notice_list.count <= 0 || notice_list.count > PublicNoticeList::MAX_PUBLICNOTICE_NUM) return;

	bool normal_notice_is_empty = System::Instance().NormalNoticeIsEmpty(LocalConfig::Instance().GetUniqueServerID());

	crossgameprotocal::GameHiddenSyncPublicNotice sync_notice;
	sync_notice.count = 0;
	sync_notice.plat_type = LocalConfig::Instance().GetPlatType();
	sync_notice.server_id = LocalConfig::Instance().GetMergeServerId();

	for (int i = 0; i < notice_list.count; ++i)
	{
		if (notice_list.notice_list[i].urgent)
		{
			const char *notice = notice_list.notice_list[i].notice.c_str();
			int length = static_cast<int>(notice_list.notice_list[i].notice.size());
			const char *spid_id = notice_list.notice_list[i].spid_id.c_str();
			int spid_id_length = static_cast<int>(notice_list.notice_list[i].spid_id.size());
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(notice, length, GetMsgType(notice_list.notice_list[i].type), 
				notice_list.notice_list[i].color, 0, 0, 0, spid_id, spid_id_length);
		}
		else if (normal_notice_is_empty)
		{
			System::Instance().AddNormalNotice(LocalConfig::Instance().GetUniqueServerID(), notice_list.notice_list[i]);
		}

		if (!CrossConfig::Instance().IsHiddenServer())
		{
			sync_notice.notice_list[sync_notice.count++] = notice_list.notice_list[i];
		}
	}

	if (!CrossConfig::Instance().IsHiddenServer() && sync_notice.count > 0)
	{
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&sync_notice);
	}
}

void RMIWriteCmdTimeOutLogBackObjectImpl::WriteLogRet(int ret)
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdTimeOutLogBackObjectImpl::WriteLogRet ret:%d timestamp:%u", ret, query_timestamp);
}

void RMIWriteCmdTimeOutLogBackObjectImpl::__exception(int error)
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdTimeOutLogBackObjectImpl::__exception error:%d timestamp:%u", error, query_timestamp);
}

void RMIWriteCmdTimeOutLogBackObjectImpl::__timeout()
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdTimeOutLogBackObjectImpl::__timeout timestamp:%u", query_timestamp);
}

void RMIWriteCmdResultBackObjectImpl::WriteResultRet(int ret)
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::WriteResultRet ret:%d", ret);
}

void RMIWriteCmdResultBackObjectImpl::__exception(int error)
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::__exception error:%d", error);
}

void RMIWriteCmdResultBackObjectImpl::__timeout()
{
	gamelog::g_log_cmd.printf(LL_INFO, "RMIWriteCmdResultBackObjectImpl::__timeout");
}

void RMIGetCmdBackObjectImpl::GetCmdRet(int ret, const CmdList &cmd_list)
{
	if (0 != ret) return;

	static CmdResult result;
	result.count = 0;

	for (int i = 0; i < cmd_list.count && i < CmdList::MAX_CMD_NUM; ++i)
	{
#ifdef _DEBUG
		//printf("GetCmdRet cmd_id[%d] cmd[%s]\n", cmd_list.cmd_list[i].cmd_id,
		//	cmd_list.cmd_list[i].cmd.c_str());
#endif
		std::string error_msg;
		System::Instance().OnCmd(cmd_list.cmd_list[i].cmd_id, 
			cmd_list.cmd_list[i].creator, cmd_list.cmd_list[i].cmd,
			error_msg, cmd_list.cmd_list[i].opid);

		result.result_list[result.count].cmd_id = cmd_list.cmd_list[i].cmd_id;
		if (error_msg.length() >= sizeof(OPResult))
		{
			gamelog::g_log_cmd.printf(LL_WARNING, "RMIGetCmdBackObjectImpl::GetCmdRet [ErrorMsg Too Long] cmd_id[%d] cmd[%s] error_msg[%s]",
				cmd_list.cmd_list[i].cmd_id, cmd_list.cmd_list[i].cmd.c_str(), error_msg.c_str());
			error_msg = "ErrorMsg Too Long";
		}
		
		F_STRNCPY(result.result_list[result.count].opresult, error_msg.c_str(), sizeof(OPResult));
		result.count += 1;

		if (result.count >= CmdResult::MAX_RESULT_NUM)
		{
			RMIWriteCmdResultBackObjectImpl *callback_obj = new RMIWriteCmdResultBackObjectImpl();

			RMISystemClient sc;
			sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
			sc.WriteCmdResult(RMISystemClient::CMD_TYPE_GLOBAL, callback_obj, &result);
		
			result.count = 0;
		}
	}

	if (result.count > 0)
	{
		RMIWriteCmdResultBackObjectImpl *callback_obj = new RMIWriteCmdResultBackObjectImpl();

		RMISystemClient sc;
		sc.__bind_session(InternalComm::Instance().GetDBHandler().GetSystemDB());
		sc.WriteCmdResult(RMISystemClient::CMD_TYPE_GLOBAL, callback_obj, &result);
	}
}

void RMIGetCmdBackObjectImpl::__exception(int error)
{
	gamelog::g_log_debug.printf(LL_INFO, "RMIGetCmdBackObjectImpl::__exception error=%d", error);
}

void RMIGetCmdBackObjectImpl::__timeout()
{
	gamelog::g_log_debug.printf(LL_INFO, "RMIGetCmdBackObjectImpl::__timeout");

	System::Instance().AddTimeOutLogInfo(query_timestamp);
}