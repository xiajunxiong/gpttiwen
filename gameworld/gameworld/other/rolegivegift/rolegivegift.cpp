#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "global/cross/crossmanager.h"
#include "engineadapter.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"
#include "internalcomm.h"

#include "other/event/eventhandler.hpp"
#include "other/rolecommonsave/rolecommonsave.hpp"
#include "global/givegift/givegift.h"
#include "protocol/msggift.h"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "other/rolegivegift/rolegivegiftconfig.hpp"
#include "rolegivegift.hpp"

#include "friend/friendmanager.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "servercommon/string/gameworldstr.h"
#include "chat/chatmanager.h"
#include "config/otherconfig/friendgiftconfig.hpp"
#include "friend/friendintimacy.hpp"
#include "protocol/msgfriend.h"
#include "global/usercache/usercache.hpp"

#include "other/shop/shopconfig.h"
#include "protocol/msgitem.h"

RoleGiveGift::RoleGiveGift()
{
	m_module_mgr = NULL;
}

RoleGiveGift::~RoleGiveGift()
{
}

void RoleGiveGift::Init(RoleModuleManager * mgr)
{
	m_module_mgr = mgr;
}

void RoleGiveGift::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM);
	if (rcs == NULL)
	{
		return;
	}
	rcs->ClearAllData();
	this->SendRoleInfo();
}

int RoleGiveGift::GiveItem(int to_role_id, int item_id, int num, char(&str)[COMMON_SAVE_GIVE_GIFT_STR_NUM])
{
	if (to_role_id == 0)
	{
		return -__LINE__;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		m_module_mgr->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return -__LINE__;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(item_id);
	if (item_base == NULL)
	{
		return -__LINE__;
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return -__LINE__;
	}

	const RoleGiveGiftCfg * give_cfg = LOGIC_CONFIG->GetRoleGiveGiftConfig()->GetRoleGiveGiftCfg(item_id);
	if (give_cfg == NULL)
	{
		return -__LINE__;
	}

	const RoleGiveTimesCfg * times_cfg = LOGIC_CONFIG->GetRoleGiveGiftConfig()->GetRoleGiveTimesCfg(give_cfg->times_group_id);
	if (times_cfg == NULL)
	{
		return -__LINE__;
	}

	RoleCommonSaveRoleGiveGiftData * role_give_data = this->GetRoleCommonSaveRoleGiveGift(to_role_id, give_cfg->times_group_id);
	if (role_give_data == NULL)
	{
		return -__LINE__;
	}

	if (role_give_data->give_times + num > times_cfg->give_times)
	{
		return -__LINE__;
	}

	if (!m_module_mgr->GetKnapsack()->HasItem(item_id, num))
	{
		return -__LINE__;
	}

	CommonSaveGiveGiftData data;
	data.Reset();

	data.uuid = unique_key;
	data.from_role_id = m_module_mgr->GetUid();
	data.to_role_id = to_role_id;
	data.time = (unsigned int)EngineAdapter::Instance().Time();
	SNPRINTF(data.str, ARRAY_LENGTH(data.str), "%s", str);
	data.item.item_id = item_id;
	data.item.num = num;
	data.item.invaild_time = item_base->CalInvalidTime();
	data.time_group_id = times_cfg->times_group_id;

	bool ret = GiveGift::Instance().AddGiveGift(data);
	if (!ret)
	{
		return -__LINE__;
	}

	role_give_data->give_times += num;
	m_module_mgr->GetKnapsack()->ConsumeItem(item_id, num, __FUNCTION__);
	gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d | %s %s", __FUNCTION__, __LINE__, ::CommonSaveGiveGiftDataToString(data).c_str(), ret ? "SUCC" : "FAIL");

	this->SendRoleInfo();

	// 检测和记录是不是首日聊天的增加亲密度
	FriendIntimacyManager::Instance().OnSingleChat(m_module_mgr->GetRole()->GetUID(), to_role_id, (unsigned int)EngineAdapter::Instance().Time(), __FUNCTION__);

	return 0;
}

int RoleGiveGift::GiveFlower(int _to_role_id, int _item_id, int _num, bool _is_auto_buy, char(&_str)[COMMON_SAVE_GIVE_GIFT_STR_NUM])
{
	// 1.一些基础验证
	if (!this->CanGiveFlower(_to_role_id, _item_id, _num, _is_auto_buy))
	{
		return -1;
	}

	// 2. 检查道具和消耗道具
	if (!this->ConsumeSendFlowerItem(_item_id, _num, _is_auto_buy))
	{
		return -2;
	}

	// 3. 给目标送花 , 前面消耗完了 , 送花必须送到
	this->GiveFlowerHelp(_to_role_id, _item_id, _num, _str);

	// 4. 广播送花特效
	FriendIntimacyManager::Instance().GiveFlowerNotify( m_module_mgr->GetRole()->GetUID(), _to_role_id, _item_id);

	// 5. 收到花的玩家一些相关信息变动
	this->GiveFlowerChange(_to_role_id);

	return 0;
}

void RoleGiveGift::SendRoleInfo()
{
	Protocol::SCGiftRoleInfo protocol;
	protocol.count = 0;

	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM);
	if (rcs != NULL)
	{
		RetRoleCommonSaveData get_data;
		rcs->BeginDataForeach();
		while (rcs->GetNextData(get_data) && protocol.count < ARRAY_LENGTH(protocol.give_info))
		{
			protocol.give_info[protocol.count].role_id = get_data.key.param1;
			protocol.give_info[protocol.count].type = get_data.key.param2;
			protocol.give_info[protocol.count].num = get_data.data.role_give_gift_data.give_times;
			protocol.count += 1;
		}
	}


	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.give_info) - protocol.count) * sizeof(protocol.give_info[0]);

	m_module_mgr->NetSend(&protocol, send_len);
}

RoleCommonSaveRoleGiveGiftData * RoleGiveGift::GetRoleCommonSaveRoleGiveGift(int to_role_id, int times_group_id)
{
	RoleCommonSave * rcs = m_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_GIVE_ITEM);
	if (rcs == NULL)
	{
		return NULL;
	}

	if (rcs->GetSaveDataNum() > 1000)
	{
		return NULL;
	}

	RoleCommonSaveKey key(to_role_id, times_group_id);
	RoleCommonSaveRoleGiveGiftData * data = rcs->GetTypeData<RoleCommonSaveRoleGiveGiftData>(key);
	return data;
}

bool RoleGiveGift::CanGiveFlower(int _to_role_id, int _item_id, int _num, bool _is_auto_buy)
{
	Role * temp_role = m_module_mgr->GetRole();
	if (NULL == temp_role)
		return false;

	if (_to_role_id == 0)
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::CanGiveFlower[Error_1]  user[%d %s]  _to_role_id == 0",
			temp_role->GetUID(), temp_role->GetName());
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (_to_role_id == temp_role->GetUID())
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::CanGiveFlower[Error_2]  user[%d %s]  _to_role_id == temp_role->GetUID()",
			temp_role->GetUID(), temp_role->GetName());
		m_module_mgr->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_GIFT_MY);
		return false;
	}

	if(!FriendManager::Instance().IsFriend(m_module_mgr->GetRole()->GetUID() , _to_role_id))
	{
		m_module_mgr->NoticeNum(errornum::EN_FRIEND_BLESS_NOT_FRIEND);
		return false;
	}

	// 检查一下目标是否在线  不在线则不能送
	int status_type = UserCacheManager::Instance().GetUserOnlineStatus(_to_role_id);
	if (UserStatusNode::STATUS_TYPE_OFFLINE == status_type)
	{
		m_module_mgr->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		return false;
	}
	
	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		m_module_mgr->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return false;
	}

	const ItemBase * item_base = ITEMPOOL->GetItem(_item_id);
	if (item_base == NULL)
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::CanGiveFlower[Error_3]  user[%d %s]  item_base == NULL  item_id:%d",
			temp_role->GetUID(), temp_role->GetName(), _item_id);
		return false;
	}

	const FriendGiftSendFlowerCfg * flower_cfg = LOGIC_CONFIG->GetFriendGiftConfig()->GetSendFlowerCfg(_item_id);
	if (NULL == flower_cfg)
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::CanGiveFlower[Error_4]  user[%d %s]  item_id:%d  is not a flower item",
			temp_role->GetUID(), temp_role->GetName(), _item_id);
		m_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	if (!flower_cfg->is_batch && _num > 1)
	{
		m_module_mgr->NoticeNum(errornum::EN_ITEM_CANNOT_MULTI_USE);
		return false;
	}

	if (!flower_cfg->is_fast_buy &&  _is_auto_buy)
	{
		m_module_mgr->NoticeNum(errornum::EN_SHOP_ITEM_NOT_SUPPORT_FAST_BUY);
		return false;
	}

	return true;
}

void RoleGiveGift::GiveFlowerHelp(int _to_role_id, int _item_id, int _num, char(&_str)[COMMON_SAVE_GIVE_GIFT_STR_NUM])
{
	// 1. 发送送花信息
	// 客户端要求走发给与道具的信息 , 而不是聊天
	// 这里伪造一个已经领取的送花记录给客户端 , 因为策划要求送花是不需要领取的
	unsigned int now_times = (unsigned int)EngineAdapter::Instance().Time();
	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::GiveFlowerHelp[Error_1]  user[%d %s] to_role_id:%d  item_id:%d  num:%d  ",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),_to_role_id,  _item_id, _num);
		return;
	}

	CommonSaveGiveGiftData data;
	data.Reset();

	data.uuid = unique_key;
	data.from_role_id = m_module_mgr->GetUid();
	data.to_role_id = _to_role_id;
	data.fetch_time = now_times;
	data.time = now_times;
	SNPRINTF(data.str, ARRAY_LENGTH(data.str), "%s", _str);

	data.item.item_id = _item_id;
	data.item.num = _num;
	data.item.invaild_time = 0;
	data.time_group_id = 0;

	bool ret = GiveGift::Instance().AddGiveGift(data);
	if (!ret)
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::GiveFlowerHelp[Error_2]  AddGiveGift_error user[%d %s] to_role_id:%d  item_id:%d  num:%d  ",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(),_to_role_id, _item_id , _num);
		return;
	}

	// 2. 送花成功 送花是需要加亲密度的
	const FriendGiftSendFlowerCfg * flower_cfg = LOGIC_CONFIG->GetFriendGiftConfig()->GetSendFlowerCfg(_item_id);
	if (NULL == flower_cfg)
	{
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::GiveFlowerHelp[Error_3]  AddGiveGift_error user[%d %s] to_role_id:%d  item_id:%d  num:%d  ",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), _to_role_id, _item_id, _num);
		return ;
	}
	int flower_val = flower_cfg->intimacy_add * _num;
	FriendIntimacyManager::Instance().AddIntimacy(m_module_mgr->GetRole()->GetUID(), _to_role_id, flower_val, "RoleGiveGift::GiveFlowerHelp[Flower]");
	EventHandler::Instance().OnIntimacyAddBySend(m_module_mgr->GetRole(), _to_role_id, flower_val);//豪气值

	// 3. 检测和记录是不是首日聊天的增加亲密度
	FriendIntimacyManager::Instance().OnSingleChat(m_module_mgr->GetRole()->GetUID(), _to_role_id, now_times , __FUNCTION__);
}

bool RoleGiveGift::ConsumeSendFlowerItem(int _item_id, int _num, bool _is_aotu_buy)
{
	ItemExtern::ItemConsumeList consume_list;
	consume_list.Reset();

	// 获取可消耗的背包材料列表
	short stuff_count = 0;
	ItemExtern::ItemConsumeConfig stuff_list[ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM];
	stuff_list[stuff_count].item_id = _item_id;
	stuff_list[stuff_count].num = _num;
	stuff_list[stuff_count].buyable = _is_aotu_buy;
	stuff_count += 1;

	// 策划有病系列 , 改了消耗类型
	int need_buy_count = 0;
	if (_is_aotu_buy)
	{
		int has_count = m_module_mgr->GetKnapsack()->Count(_item_id);
		need_buy_count = _num - has_count;
		if (need_buy_count < 0)
		{
			// 表示不需要购买
			need_buy_count = 0;
		}
	}
	else
	{
		int has_count = m_module_mgr->GetKnapsack()->Count(_item_id);
		if (_num > has_count)
		{
			Protocol::SCLackItem li;
			li.item_id = _item_id;
			li.item_count = _num - has_count;
			EngineAdapter::Instance().NetSend(m_module_mgr->GetRole()->GetNetId(), (const char*)&li, sizeof(li));
			return false;
		}
	}

	if (need_buy_count > 0)
	{
		// 需要购买
		const FastBuyContainer * buy_cfg = LOGIC_CONFIG->GetShopConfig()->GetFastBuyCfg(_item_id);
		if (NULL == buy_cfg)
		{
			return false;
		}

		// 需要购买的金额
		int price = buy_cfg->unit_price * need_buy_count;

		// 消耗货币
		switch (buy_cfg->currency_type)
		{
		case CURRENCY_TYPE_GOLD:
			if (!m_module_mgr->GetMoney()->UseGold(price, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return false;
			}
			break;
		case CURRENCY_TYPE_IMMORTAL_COIN:
			if (!m_module_mgr->GetMoney()->UseImmortalCoin(price, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
				return false;
			}
			break;
		case CURRENCY_TYPE_COIN_BIND:
			if (!m_module_mgr->GetMoney()->UseCoinBind(price, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return false;
			}
			break;

		default:
			if (!m_module_mgr->GetMoney()->UseOtherCoin(price, buy_cfg->currency_type, __FUNCTION__))
			{
				m_module_mgr->NoticeNum(errornum::EN_OTHER_MONEY_NOT_ENOUGH);
				return false;
			}
			break;
		}

		// 记录一下方便查问题
		gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::ConsumeSendFlowerItem   consume_money  user[%d %s]  need_buy_num:%d  moeny_type:%d  total_price:%d ",
			m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), need_buy_count, buy_cfg->currency_type, price);
	}

	// 消耗道具
	int real_comsume = _num - need_buy_count;
	if (!m_module_mgr->GetKnapsack()->ConsumeItem(_item_id, real_comsume, __FUNCTION__))
	{
		return false;
	}

	gamelog::g_log_give_gift.printf(LL_INFO, "RoleGiveGift::ConsumeSendFlowerItem   consume_item  user[%d %s]  item_id:%d  num:%d  need_buy_num:%d  real_comsume:%d  is_auto_buy:%d",
		m_module_mgr->GetRole()->GetUID(), m_module_mgr->GetRole()->GetName(), _item_id, _num , need_buy_count , real_comsume , _is_aotu_buy);

	return true;
}

void RoleGiveGift::GiveFlowerChange(int _to_role_id)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(_to_role_id);
	if (NULL == role)
	{
		if (CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::HiddenGameGiveFlowerChange hggfc;
			hggfc.role_id = _to_role_id;

			InternalComm::Instance().SendToGameThroughCross2((const char *)&hggfc, sizeof(hggfc), IntToUid(hggfc.role_id).db_index);
		}
		else
		{
			crossgameprotocal::GameHiddenGiveFlowerChange ghgfc;
			ghgfc.role_id = _to_role_id;

			InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghgfc, sizeof(ghgfc));
		}
	}
	else
	{
		EventHandler::Instance().OnFetchFriendGift(role);
	}
}
