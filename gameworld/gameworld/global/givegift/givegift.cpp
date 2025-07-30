#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "engineadapter.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "world.h"
#include "internalcomm.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/rolegivegift/rolegivegiftconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "gamelog.h"

#include "global/commonsave/commonsave.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "protocol/msggift.h"
#include "givegift.h"

GiveGift & GiveGift::Instance()
{
	static GiveGift r;
	return r;
}

void GiveGift::OnDayChange()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->ReturnItem();
}

void GiveGift::OnUserLogin(Role * role)
{	
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SendGiftInfo(role->GetUID());
}

void GiveGift::OnUserLoginOriginGame(int role_id)
{
	this->SendGiftInfo(role_id);
}

bool GiveGift::AddGiveGift(const CommonSaveGiveGiftData & data)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenGiveGiftAddGift add;
		add.gift_data = data;

		return InternalComm::Instance().SendToHiddenThroughCross((const char *)&add, sizeof(add));
	}

	CommonSave * cs = this->GetCommonSaveGiveGift();
	if (cs == NULL)
	{
		return false;
	}

	CommonSaveKey key(data.to_role_id, data.uuid);
	CommonSaveGiveGiftData * save_data = cs->GetTypeData<CommonSaveGiveGiftData>(key, true);
	if (save_data == NULL)
	{
		return false;
	}

	CommonSaveKey key2(data.from_role_id, data.uuid);
	CommonSaveGiveGiftData * save_data2 = cs->GetTypeData<CommonSaveGiveGiftData>(key2, true);
	if (save_data2 == NULL)
	{
		cs->RemoveData(key);
		return false;
	}

	*save_data = data;
	*save_data2 = data;
	this->SendGiftInfoUpdate(data.to_role_id, data);
	this->SendGiftInfoUpdate(data.from_role_id, data);

	return true;
}

int GiveGift::FetchItem(int role_id, const long long & give_gift_id_ll,bool call_from_game)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenGiveGiftReq fetch_req;
		fetch_req.req_type = crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_FETCH;
		fetch_req.role_id = role_id;
		fetch_req.give_gift_id_ll = give_gift_id_ll;

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&fetch_req, sizeof(fetch_req));

		return -__LINE__;
	}


	CommonSaveGiveGiftData * data = this->GetCommonSaveGiveGiftDataLL(role_id, give_gift_id_ll);
	if (data == NULL)
	{
		return -__LINE__;
	}

	CommonSaveGiveGiftData * data2 = this->GetCommonSaveGiveGiftDataLL(data->from_role_id, give_gift_id_ll);
	if (data2 == NULL)
	{
		return -__LINE__;
	}

	if (data->fetch_time || data->to_role_id != role_id)
	{
		return -__LINE__;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (call_from_game)
	{
		data->fetch_time = now;
		data2->fetch_time = now;

		crossgameprotocal::HiddenGameGiveGiftRetInfo info;
		info.role_id = role_id;
		info.give_gift_id_ll = give_gift_id_ll;
		info.gift_data = *data;
		InternalComm::Instance().SendToGameThroughCrossByRoleId((const char *)&info, sizeof(info), role_id);

		gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d | role_id:%d send HiddenGameGiveGiftRetInfo %s", __FUNCTION__, __LINE__, role_id, ::CommonSaveGiveGiftDataToString(*data).c_str());

		return -__LINE__;
	}


	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role == NULL)
	{
		return -__LINE__;
	}

	ItemDataWrapper wrap;
	wrap.item_id = data->item.item_id;
	wrap.num = data->item.num;
	wrap.invalid_time = data->item.invaild_time;
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(wrap, PUT_REASON_FETCH_GIVE_GIFT))
	{
		return -__LINE__;
	}

	data->fetch_time = now;
	data2->fetch_time = now;
	this->SendGiftInfoUpdate(role_id, *data);
	this->SendGiftInfoUpdate(data->from_role_id, *data2);
	EventHandler::Instance().OnFetchFriendGift(role);
	gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d |  role_id:%d fetch %s", __FUNCTION__, __LINE__, role_id, ::CommonSaveGiveGiftDataToString(*data).c_str());
	
	return 0;
}

void GiveGift::SendGiftInfo(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSave * cs = this->GetCommonSaveGiveGift();
	if (cs == NULL)
	{
		return;
	}

	CommonSaveKey key1(role_id, INT_MIN, INT_MIN);
	CommonSaveKey key2(role_id, INT_MAX, INT_MAX);

	static Protocol::SCGetGiftInfo protocol;
	protocol.type = Protocol::SC_GET_GIFT_INFO_TYPE_DEFAULT;
	protocol.count = 0;

	std::vector<RetCommonSaveData> ret_data = cs->GetRangeData(key1, key2);
	for (int i = 0; i < (int)ret_data.size() && protocol.count < ARRAY_LENGTH(protocol.gift_info); ++i)
	{
		protocol.gift_info[protocol.count] = ret_data[i].data.give_gift_data;
		protocol.count += 1;
	}

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.gift_info) - protocol.count) * sizeof(protocol.gift_info[0]);
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_len);
	}
}

void GiveGift::SendGiftInfoUpdate(int role_id,const CommonSaveGiveGiftData & data)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	static Protocol::SCGetGiftInfo protocol;
	protocol.type = Protocol::SC_GET_GIFT_INFO_TYPE_UPDATE;
	protocol.count = 1;
	protocol.gift_info[0] = data;

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.gift_info) - protocol.count) * sizeof(protocol.gift_info[0]);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_len);
	}
}

int GiveGift::SetFetchAck(int role_id, long long give_gift_id_ll, bool is_succ)
{
	CommonSaveGiveGiftData * data = this->GetCommonSaveGiveGiftDataLL(role_id, give_gift_id_ll);
	if (data == NULL)
	{
		return -__LINE__;
	}

	int to_role_id = data->to_role_id;
	int from_role_id = data->from_role_id;

	CommonSaveGiveGiftData * data2 = this->GetCommonSaveGiveGiftDataLL(from_role_id, give_gift_id_ll);
	if (data2 == NULL)
	{
		return -__LINE__;
	}

	if (!is_succ)
	{
		data->fetch_time = 0;
		data2->fetch_time = 0;
	}
	else
	{
		this->SendGiftInfoUpdate(to_role_id, *data);
		this->SendGiftInfoUpdate(from_role_id, *data2);
	}

	return 0;
}

int GiveGift::GmFetchItem(int role_id,bool call_from_game)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenGiveGiftReq fetch_req;
		fetch_req.role_id = role_id;
		fetch_req.req_type = crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_GM_FETCH;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&fetch_req, sizeof(fetch_req));

		return 0;
	}


	CommonSave * cs = this->GetCommonSaveGiveGift();
	if (cs != NULL)
	{
		RetCommonSaveData get_data;
		cs->BeginDataForeach();
		while (cs->GetNextData(get_data))
		{
			if (get_data.data.give_gift_data.fetch_time)
			{
				continue;
			}

			return this->FetchItem(role_id, ::ConvertParamToLongLong(get_data.key.param2, get_data.key.param3),call_from_game);
		}
	}

	return 0;
}

GiveGift::GiveGift()
{

}

GiveGift::~GiveGift()
{

}

void GiveGift::ReturnItem()
{
	CommonSave * cs = this->GetCommonSaveGiveGift();
	if (cs == NULL)
	{
		return;
	}

	std::set<int> notice_role_id_list;
	RetCommonSaveData get_data;
	cs->BeginDataForeach();
	while (cs->GetNextData(get_data))
	{
		const CommonSaveGiveGiftData & gift_data = get_data.data.give_gift_data;
		const CommonSaveKey & key = get_data.key;
		if (get_data.data.give_gift_data.fetch_time || key.param1 == gift_data.from_role_id)	//领取过/赠送玩家的观看记录
		{
			cs->RemoveData(key);
			continue;
		}
		const RoleGiveTimesCfg * times_cfg = LOGIC_CONFIG->GetRoleGiveGiftConfig()->GetRoleGiveTimesCfg(gift_data.time_group_id);
		if (NULL != times_cfg && !times_cfg->is_day_return)		//获取不到配置则可能是以前的赠送或者是鲜花,默认返还
		{
			continue;
		}
		
		int ret_role_id = gift_data.from_role_id;
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.item_list[0].item_id = gift_data.item.item_id;
		contentparam.item_list[0].num = gift_data.item.num;
		contentparam.item_list[0].is_bind = 1;
		contentparam.item_list[0].invalid_time = gift_data.item.invaild_time;

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::give_gift_back_item_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::give_gift_back_item_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(ret_role_id, MAIL_REASON_DEFAULT, contentparam);
		}

		notice_role_id_list.insert(ret_role_id);
		cs->RemoveData(key);
		gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d |  ret_role_id:%d return %s", __FUNCTION__, __LINE__, ret_role_id, ::CommonSaveGiveGiftDataToString(gift_data).c_str());
	}

	for (std::set<int>::iterator iter = notice_role_id_list.begin(); iter != notice_role_id_list.end(); ++iter)
	{
		int tmp_role_id = *iter;
		this->SendGiftInfo(tmp_role_id);
	}
}

CommonSave * GiveGift::GetCommonSaveGiveGift() const
{
	CommonSave * cs = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_GIVE_GIFT);
	return cs;
}


CommonSaveGiveGiftData * GiveGift::GetCommonSaveGiveGiftData(int role_id, int give_gift_id1, int give_gift_id2, bool is_auto_create)
{
	CommonSave * cs = this->GetCommonSaveGiveGift();
	if (cs == NULL)
	{
		return NULL;
	}

	CommonSaveKey key(role_id, give_gift_id1, give_gift_id2);
	CommonSaveGiveGiftData * save_data = cs->GetTypeData<CommonSaveGiveGiftData>(key, is_auto_create);
	return save_data;
}

CommonSaveGiveGiftData * GiveGift::GetCommonSaveGiveGiftDataLL(int role_id, long long give_gift_id, bool is_auto_create)
{
	int id[2] = { 0 };
	::LongLongToInt(give_gift_id, id);

	return this->GetCommonSaveGiveGiftData(role_id, id[0], id[1], is_auto_create);
}
