#include "other/monitor/monitor.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "obj/character/role.h"

#include "internalcomm.h"
#include "engineadapter.h"

#include "item/itembase.h"
#include "item/itempool.h"

#include "servercommon/errornum.h"
#include "servercommon/servercommon.h"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/maildef.hpp"

#include "mailroute.hpp"
#include "gameworld/gamelog.h"
#include "global/mail/mailmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "globalconfig/globalconfig.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "other/rolemail/rolemail.hpp"
#include "other/trade_credit/trade_credit.hpp"

MailRoute & MailRoute::Instance()
{
	static MailRoute mail_route;
	return mail_route;
}

void MailRoute::SendMail(Role *role, Protocol::CSMailSend *mailsend)
{
}

void MailRoute::RemoveMail(Role *role, int mail_index)
{
	if (NULL == role)
	{
		return;
	}

	if (mail_index < 0 || mail_index >= MAX_MAIL_NUM) return;

	role->GetRoleModuleManager()->GetRoleMail()->RemoveMail(mail_index);
	//UserMailManager::Instance().OnDeleteMail(role, mail_index);
}

void MailRoute::GetMailList(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	role->GetRoleModuleManager()->GetRoleMail()->FetchMailList();
	//UserMailManager::Instance().OnFetchMailList(role);
}

void MailRoute::ReadMail(Role *role, int mail_index)
{
	if (NULL == role)
	{
		return;
	}

	if (mail_index < 0 || mail_index >= MAX_MAIL_NUM) return;

	role->GetRoleModuleManager()->GetRoleMail()->FetchMailDetail(mail_index);
	//UserMailManager::Instance().OnFetchMailDetail(role, mail_index);
}

void MailRoute::FetchMailAttachment(Role *role, Protocol::CSMailFetchAttachment *attachment)
{
	if (NULL == role)
	{
		return;
	}

	if (attachment->mail_index < 0 || attachment->mail_index >= MAX_MAIL_NUM) return;
	if (attachment->item_index >= MAX_ATTACHMENT_ITEM_NUM) return;
	if (attachment->item_index < 0 && (MAIL_FETCH_VIRTUAL != attachment->item_index && MAIL_FETCH_ALL != attachment->item_index)) return;

// 	if (!role->GetKnapsack()->CheckEmptyGridNoLessThan(attachment->item_num))
// 	{
// 		if (0 != attachment->is_last) role->NoticeNum(errornum::EN_HAS_NO_SPACING);
// 		return;
// 	}

	FetchAttachment fam;
	fam.db_index = role->GetUserId().db_index;
	fam.role_id = role->GetUserId().role_id;
	fam.mail_index = attachment->mail_index;
	fam.item_index = attachment->item_index;
	fam.is_last = attachment->is_last;

	gamelog::g_log_mail.printf(LL_INFO, "mail_route -> attachment request[ ret:succ, uid:%d, role_name:%s, mail_index:%d ]", 
		UidToInt(UserID((int)fam.db_index, (int)fam.role_id)), (const char*)role->GetName(), (int)fam.mail_index);

	role->GetRoleModuleManager()->GetRoleMail()->FetchAttachment(attachment->mail_index, attachment->item_index, attachment->is_last);
	//UserMailManager::Instance().OnFetchMailAttachment(role, &fam);
}

void MailRoute::OnMailAttachment(Role *role, ggprotocal::MailAttachment *ma)
{
	if (ma->item_count < 0 || ma->item_count > MAX_ATTACHMENT_ITEM_NUM) return;
	
	if (NULL == role)
	{
		return;
	}

	FetchAttachmentAffirm faa;
	faa.db_index = role->GetUserId().db_index;
	faa.role_id = role->GetUserId().role_id;
	faa.mail_index = ma->mail_index;
	faa.item_index = ma->item_index;
	faa.is_last = ma->is_last;

	Knapsack *knapsack = role->GetRoleModuleManager()->GetKnapsack();
	if (ma->item_count > 0 && !knapsack->CheckForPutList(ma->item_count, &ma->item_list[0]))
	{
		if (0 != ma->is_last) role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);

		faa.result = -1;
		role->GetRoleModuleManager()->GetRoleMail()->FetchAttachmentAffirm(faa.mail_index, faa.result, faa.item_index, faa.is_last);
		//UserMailManager::Instance().OnFetchMailAttachmentAffirm(role, &faa);
		return;
	}
	
	this->LogMailContent(role, ma, __FUNCTION__);

	{
		for (int i = 0; i < ma->item_count && i < MAX_ATTACHMENT_ITEM_NUM; i++)
		{
			if (ma->item_list[i].item_id != ItemBase::INVALID_ITEM_ID)
			{
				World::GetInstWorld()->LOG_QUICK(role->GetUserId().db_index, gamelog::g_log_quick, 
					LOG_TYPE_MAIL_ITEM_IN, UidToInt(role->GetUserId()), role->GetName(), NULL,
					ma->item_list[i].item_id, ma->item_list[i].num, ma->subject, ma->contenttxt,
					ma->sender_uid, 0, NULL, NULL, 0);

				const ItemBase *itembase = ITEMPOOL->GetItem(ma->item_list[i].item_id);
				if (NULL != itembase)
				{
					knapsack->Put(ma->item_list[i], PUT_REASON_MAIL_RECEIVE);
				}
				else
				{
					role->NoticeNum(errornum::EN_ITEM_IS_NOT_EXIST);
				}
			}
		}

		Money *money = role->GetRoleModuleManager()->GetMoney();

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] > 0)
		{
			money->AddCoinBind(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN], "MailAttachment");
		}
		
		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_IMMORTAL_COIN] > 0)
		{
			money->AddImmortalCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_IMMORTAL_COIN], "MailAttachment");
		}
		
		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] > 0)
		{
			money->AddGold(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD], "MailAttachment");
		}
		
		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] > 0)
		{
			role->AddExp(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP], ADD_EXP_TYPE_MAIL, "MailAttachment");
		}
		
		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN], 
				MONEY_TYPE_GUILD_CONTRIBUTION, "MailAttachment");
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS], MONEY_TYPE_COLOSSEUM, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS], MONEY_TYPE_PLATFORM_BATTLE_CREDITS, __FUNCTION__);
		}
		
		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_ZHAN_CHANG_CREDITS] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_ZHAN_CHANG_CREDITS], MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_CREDITS, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_LUCK_COIN], MONEY_TYPE_LUCK_COIN, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_COIN] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_HUAN_JIE_COIN], MONEY_TYPE_HUAN_JIE_ZHAN_CHANG_COIN, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_TRADE_CREDIT] > 0)
		{
			role->GetRoleModuleManager()->GetTradeCredit()->AddCredit(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_TRADE_CREDIT], __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_YUAN_SUI_ZHI_LING] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_YUAN_SUI_ZHI_LING], MONEY_TYPE_YUAN_SUI_ZHI_LING, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] > 0)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN], MONEY_TYPE_SILVER_COIN, __FUNCTION__);
		}

		if (ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] > 0)
		{
			UserCacheNode *user_cache = UserCacheManager::Instance().GetUserNode(ma->sender_uid);
			if (NULL != user_cache)
			{
				ROLE_LOG_QUICK6(LOG_TYPE_MAIL_RECEIVE_GOLD, role, user_cache->uid, ma->virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD], user_cache->role_name, NULL);
			}
		}
	}
	

	faa.result = 0;

	UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);

	gamelog::g_log_mail.printf(LL_INFO, "mail_route attachment receive-> role[ uid:%d,name:%s ], "
							   "attachment[item_count:%d, %s, %s, %s, %s, %s, %s, %s, %s] virtual_items[%s]",
							   UidToInt(role->GetUserId()), (const char*)role->GetName(),
							   ma->item_count,
							   ItemDataWrapperLog(&ma->item_list[0], 0),
							   ItemDataWrapperLog(&ma->item_list[1], 1),
							   ItemDataWrapperLog(&ma->item_list[2], 2),
							   ItemDataWrapperLog(&ma->item_list[3], 3),
							   ItemDataWrapperLog(&ma->item_list[4], 4),
							   ItemDataWrapperLog(&ma->item_list[5], 5),
							   ItemDataWrapperLog(&ma->item_list[6], 6),
							   ItemDataWrapperLog(&ma->item_list[7], 7),
							   MailVirtualItemLog(&ma->virtual_item_list));

	role->GetRoleModuleManager()->GetRoleMail()->FetchAttachmentAffirm(faa.mail_index, faa.result, faa.item_index, faa.is_last);
	//UserMailManager::Instance().OnFetchMailAttachmentAffirm(role, &faa);

	role->GetRoleModuleManager()->GetMonitor()->AddMonitorNum(MONITOR_TYPE_FETCH_ATTACHMENT_NUM, 1);
}

void MailRoute::CleanMail(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	role->GetRoleModuleManager()->GetRoleMail()->CleanMail();
	//UserMailManager::Instance().OnCleanMail(role);
}

void MailRoute::OneKeyFetchAttachment(Role* role)
{
	if (NULL == role) return;

	role->GetRoleModuleManager()->GetRoleMail()->OneKeyFetchAttachment();
}

void MailRoute::MailToUser(const UserID &user_id, int reason, const MailContentParam &contentparam, int mail_kind)
{
	if (mail_kind <= MAIL_TYPE_INVALID || mail_kind >= MAIL_TYPE_MAX) return;

	UserID user_list[1];
	user_list[0] = user_id;
	MailToMultUser(user_list, 1, reason, contentparam, mail_kind);
}

void MailRoute::MailToUser(const UserID &user_id, MailSubject subject, MailContentTxt contenttxt, int mail_kind)
{
	if (mail_kind <= MAIL_TYPE_INVALID || mail_kind >= MAIL_TYPE_MAX) return;

	UserID user_list[1];
	user_list[0] = user_id;
	MailToMultUser(user_list, 1, subject, contenttxt, mail_kind);
}

void MailRoute::MailToMultUser(UserID *user_list, int user_num , MailSubject subject, MailContentTxt contenttxt, int mail_kind)
{
	if (mail_kind <= MAIL_TYPE_INVALID || mail_kind >= MAIL_TYPE_MAX) return;

	static MailContentParam contentparam; contentparam.Reset();
	F_STRNCPY(contentparam.subject, subject, sizeof(contentparam.subject));
	F_STRNCPY(contentparam.contenttxt, contenttxt, sizeof(contentparam.contenttxt));
	MailToMultUser(user_list, user_num, 0, contentparam, mail_kind);
}

void MailRoute::MailToMultUser(UserID *user_list, int user_num , int reason, const MailContentParam &contentparam, int mail_kind)
{
	if (NULL == user_list || user_num <= 0 || user_num > MAX_GAMEWORLD_SEND_MAIL_USER) return;
	if (mail_kind <= MAIL_TYPE_INVALID || mail_kind >= MAIL_TYPE_MAX) return;

	UserMailManager::Instance().MailToMultUser(mail_kind, user_list, user_num, reason, contentparam);
}

void MailRoute::LogMailContent(Role* role, const ggprotocal::MailAttachment* ma, const char* caller)
{
	gamelog::g_log_mail.buff_printf("MailRoute::LogMailContent caller[%s] user[%d,%s] serial_num[%s] attachments[",
		caller, role->GetUID(), role->GetName(), ma->serial_num);
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		if (ma->item_list[i].Invalid()) continue;

		const ItemDataWrapper& item = ma->item_list[i];
		gamelog::g_log_mail.buff_printf("{item[%d,%d,%d] param[%s]},",
			item.item_id, item.num, item.is_bind,
			NetValueItemParam::GetParamLog(item.item_id, item.param_data));
	}

	gamelog::g_log_mail.buff_printf("]");
	gamelog::g_log_mail.commit_buff(LL_INFO);
}

enum FILL_MAIL_CONTENT_PARAM_RESULT
{
	FILL_MAIL_CONTENT_PARAM_RESULT_INVALID_ITEM = 1,
	FILL_MAIL_CONTENT_PARAM_RESULT_TOO_MORE_ITEM = 1 << 1,
};

int FillMailContentParam(MailContentParam & contentparam, const  ItemConfigData & item)
{
	std::vector<ItemConfigData> list;
	list.push_back(item);

	return FillMailContentParam(contentparam, list);
}

int FillMailContentParam(MailContentParam & contentparam, const std::vector<ItemConfigData> & item_list)
{
	int ret = 0;

	if (item_list.size() > ARRAY_LENGTH(contentparam.item_list))
	{
		ret |= FILL_MAIL_CONTENT_PARAM_RESULT_TOO_MORE_ITEM;
	}

	int item_count = 0;
	for (int i = 0; i < (int)item_list.size() && item_count < ARRAY_LENGTH(contentparam.item_list); ++i)
	{
		const ItemConfigData & reward = item_list[i];
		const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
		if (item_base == NULL)
		{
			ret |= FILL_MAIL_CONTENT_PARAM_RESULT_INVALID_ITEM;
			continue;
		}

		contentparam.item_list[item_count].Reset();
		contentparam.item_list[item_count].item_id = reward.item_id;
		contentparam.item_list[item_count].num = reward.num;
		contentparam.item_list[item_count].is_bind = (reward.is_bind) ? 1 : 0;
		contentparam.item_list[item_count].invalid_time = item_base->CalInvalidTime();
		item_count += 1;
	}

	return ret;
}

bool UserCacheLevelFilter(const UserCacheNode &user_node, void **func_param)
{
	if (NULL == func_param) return false;

	int min_level_limit = *((int*)func_param);

	return user_node.level >= min_level_limit; 
}
