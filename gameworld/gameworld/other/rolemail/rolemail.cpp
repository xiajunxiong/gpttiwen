#include "rolemail.hpp"
#include "other/rolemodulemanager.hpp"
#include "global/mail/mailitem.hpp"
#include "gamelog.h"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "protocol/msgscmail.hpp"
#include "world.h"
#include "servercommon/errornum.h"
#include "servercommon/servercommon.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "servercommon/internalprotocal/ggprotocal.h"
#include "item/knapsack.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

RoleMail::RoleMail() : m_mgr(NULL), m_is_change(false), m_last_get_list_time(0u), m_last_one_key_fetch_time(0u)
{
	memset(m_mail_list, 0, sizeof(m_mail_list));
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		m_empty_slot_queue.push(i);
	}
}

RoleMail::~RoleMail()
{
	this->Release();
}

void RoleMail::Init(RoleModuleManager* mgr, const MailListParam& mail_list)
{
	if (NULL == mgr) return;

	m_mgr = mgr;
	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	for (int i = 0; i < mail_list.count; ++i)
	{
		int mail_index = mail_list.mail_list[i].mail_param.mail_index;
		if (NULL != m_mail_list[mail_index] && !m_mail_list[mail_index]->Invalid())
		{
			gamelog::g_log_rolemail.printf(LL_ERROR, "RoleMail::Init add mail fail, duplicate mail! role[%d,%s] mail_index[%d] serial_num[%s], mail_param[%s]",
				m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
				mail_list.mail_list[i].mail_param.mail_index,
				mail_list.mail_list[i].mail_param.mail_serial,
				MailParamLog(&mail_list.mail_list[i].mail_param));

			continue;
		}
		if (mail_list.mail_list[i].mail_param.status.recv_time + MAX_MAIL_RESERVER_TIME < now_second)
		{
			m_dirty_mark[mail_index] = true;
			m_is_change = true;
			m_old_state[mail_index] = true;
			continue;
		}

		if (NULL == m_mail_list[mail_index])
		{
			m_mail_list[mail_index] = new MailItem();
			m_mail_list[mail_index]->mail_param.mail_index = mail_index;
		}

		MailParam& mail_param = m_mail_list[mail_index]->mail_param;
		memcpy(&mail_param, &mail_list.mail_list[i].mail_param, sizeof(mail_param));

		this->CheckMailItemDataVersion(mail_param);
		
		mail_param.contentparam.contenttxt[sizeof(MailContentTxt) - 1] = 0;
		mail_param.contentparam.subject[sizeof(MailSubject) - 1] = 0;
		mail_param.contentparam.contenttxt_length = strlen(mail_param.contentparam.contenttxt);
		mail_param.contentparam.subject_length = strlen(mail_param.contentparam.subject);

		m_old_state[mail_index] = true;
	
		gamelog::g_log_rolemail.printf(LL_INFO, "RoleMail::Init add mail succ! role[%d,%s] mail_index[%d] serial_num[%s]",
			m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
			mail_list.mail_list[i].mail_param.mail_index,
			mail_list.mail_list[i].mail_param.mail_serial);
	}

	this->CheckUnReadMail();
}

bool RoleMail::AddMail(const MailParam &new_param, bool send_to_client)
{
	bool ret = true;

	MailItem *mailitem = this->GetFreeMailItemSlot(new_param.status.kind);  // 邮箱已满
	if (NULL != mailitem)
	{
		int index = mailitem->mail_param.mail_index;
		if (index >= 0 && index < MAX_MAIL_NUM)
		{
			memcpy(&mailitem->mail_param, &new_param, sizeof(mailitem->mail_param));
			mailitem->mail_param.mail_index = index;

			MailParam& mail_param = mailitem->mail_param;
			mail_param.contentparam.contenttxt[sizeof(MailContentTxt) - 1] = 0;
			mail_param.contentparam.subject[sizeof(MailSubject) - 1] = 0;
			mail_param.contentparam.contenttxt_length = strlen(mail_param.contentparam.contenttxt);
			mail_param.contentparam.subject_length = strlen(mail_param.contentparam.subject);

			// 兼容后台接口
			if (mailitem->mail_param.contentparam.reserver_coin_bind > 0)
			{
				mailitem->mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += mailitem->mail_param.contentparam.reserver_coin_bind;
			}
			if (mailitem->mail_param.contentparam.reserver_gold > 0)
			{
				mailitem->mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += mailitem->mail_param.contentparam.reserver_gold;
			}

			if (strcmp(mailitem->mail_param.mail_serial, "") == 0)
			{
				EngineAdapter::Instance().GenerateSerialNum(LocalConfig::Instance().GetMergeServerId(), m_mgr->GetRole()->GetUID(), mailitem->mail_param.mail_serial);
			}

			this->MailItemCheckMedal(mailitem);

			this->CheckMailItemDataVersion(mail_param);

			m_dirty_mark[index] = true;
			m_is_change = true;

			if (send_to_client)
			{
				this->SendMailAck(mailitem, true);
			}

			this->LogMailContent(mail_param, __FUNCTION__);

			ret = true;
		}

		if (!mailitem->mail_param.contentparam.IsInvalid())
		{
			gamelog::g_log_rolemail.printf(LL_INFO, "mail->add mail[ret:%s, receiver[uid:%d], mail_param[%s]]", ret ? "succ" : "full",
				m_mgr->GetRole()->GetUID(), MailParamLog(&mailitem->mail_param));
			
		}
	}
	else
	{
		MailItem mail_item;
		mail_item.mail_param = new_param;

		// 兼容后台接口
		if (mail_item.mail_param.contentparam.reserver_coin_bind > 0)
		{
			mail_item.mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += mail_item.mail_param.contentparam.reserver_coin_bind;
		}
		if (mail_item.mail_param.contentparam.reserver_gold > 0)
		{
			mail_item.mail_param.contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += mail_item.mail_param.contentparam.reserver_gold;
		}

		this->MailItemCheckMedal(mailitem);

		this->CheckMailItemDataVersion(mail_item.mail_param);

		if (send_to_client)
		{
			this->SendMailAck(&mail_item, false);
		}

		ret = false;

		if (!mail_item.mail_param.contentparam.IsInvalid())
		{
			gamelog::g_log_rolemail.printf(LL_INFO, "mail->add mail[ret:%s, receiver[uid:%d], mail_param[%s]]", ret ? "succ" : "full",
				m_mgr->GetRole()->GetUID(), MailParamLog(&mail_item.mail_param));
		}
	}

	return ret;
}

bool RoleMail::RemoveMail(int mail_index)
{
	static Protocol::SCMailDeleteAck mda;
	mda.mail_index = mail_index;

	bool result = RemoveMailHelper(mail_index);

	if (!result)
	{
		mda.ret = -1;
	}
	else
	{
		mda.ret = 0;
	}
	m_mgr->NetSend(&mda, sizeof(mda));

	return result;
}

void RoleMail::Update(time_t now_second)
{
	unsigned int now = static_cast<unsigned int>(now_second);
	bool is_delete = false;
	for (int i = 0; i < MAX_MAIL_NUM; i++)
	{
		if(NULL != m_mail_list[i] && 0 == m_mail_list[i]->mail_param.status.is_lock && !m_mail_list[i]->Invalid())
		{
			if (m_mail_list[i]->mail_param.status.recv_time + MAX_MAIL_RESERVER_TIME < now_second)
			{
				delete m_mail_list[i];
				m_mail_list[i] = NULL;

				m_empty_slot_queue.push(i);

				m_dirty_mark[i] = true;
				m_is_change = true;
				m_old_state[i] = true;
				is_delete = true;
			}
		}
	}
	if(is_delete)
	{
		this->FetchMailList(true);
	}
}

void RoleMail::GetChangeMailList(MailListParam *p)
{
	if (!m_is_change)
	{
		p->count = 0;
		return;
	}

	for (int i = 0; i < MAX_MAIL_NUM && p->count < MAX_MAIL_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_mail_list[i] || m_mail_list[i]->Invalid())
			{
				if (m_old_state[i])
				{
					p->mail_list[p->count].mail_param.mail_index = i;
					p->mail_list[p->count].change_state = structcommon::CS_DELETE;

					gamelog::g_log_rolemail.printf(LL_WARNING, "GetChangeMailList [DELETE], role[%d,%s] mail_index[%d]",
						m_mgr->GetUid(), m_mgr->GetRoleName(), i);
				}
				else
				{
					gamelog::g_log_rolemail.printf(LL_WARNING, "GetChangeMailList [Dirty But No OldState], role[%d,%s] mail_index[%d]",
						m_mgr->GetUid(), m_mgr->GetRoleName(), i);
					continue;
				}
			}
			else
			{
				if (m_old_state[i])
				{
					p->mail_list[p->count].mail_param = m_mail_list[i]->mail_param;
					p->mail_list[p->count].change_state = structcommon::CS_UPDATE;

					gamelog::g_log_rolemail.printf(LL_WARNING, "GetChangeMailList [UPDATE], role[%d,%s] mail_index[%d] mailparam[%s]",
						m_mgr->GetUid(), m_mgr->GetRoleName(), i, MailParamLog(&m_mail_list[i]->mail_param));
				}		
				else
				{
					p->mail_list[p->count].mail_param = m_mail_list[i]->mail_param;
					p->mail_list[p->count].change_state = structcommon::CS_INSERT;

					gamelog::g_log_rolemail.printf(LL_WARNING, "GetChangeMailList [INSERT], role[%d,%s] mail_index[%d] mailparam[%s]",
						m_mgr->GetUid(), m_mgr->GetRoleName(), i, MailParamLog(&m_mail_list[i]->mail_param));
				}
			}

			p->count += 1;
		}
	}
}

void RoleMail::ClearDirtyMark()
{
	if (!m_is_change) return;

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (m_dirty_mark[i])
		{
			if (NULL == m_mail_list[i] || m_mail_list[i]->Invalid())
			{
				m_old_state[i] = false;
			}
			else
			{
				m_old_state[i] = true;
			}
		}
	}

	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	m_is_change = false;
}

int RoleMail::ClearGodBeastAdventItem(ItemID item_id)
{
	int count = 0;

	for (int i = 0; i < MAX_MAIL_NUM; i++)
	{
		if (m_mail_list[i] == NULL || m_mail_list[i]->Invalid()) continue;
		if (!m_mail_list[i]->HasAttachment()) continue;
		if (m_mail_list[i]->HasFetched()) continue;

		bool is_clear = false;

		for (int index = 0; index < MAX_ATTACHMENT_ITEM_NUM; index++)
		{
			if (m_mail_list[i]->mail_param.contentparam.item_list[index].item_id == item_id)
			{
				count += m_mail_list[i]->mail_param.contentparam.item_list[index].num;

				m_mail_list[i]->mail_param.contentparam.item_list[index].Reset();
				is_clear = true;

				for (int j = index; j < MAX_ATTACHMENT_ITEM_NUM - 1 && !m_mail_list[i]->mail_param.contentparam.item_list[j + 1].Invalid(); j++)
				{
					m_mail_list[i]->mail_param.contentparam.item_list[j] = m_mail_list[i]->mail_param.contentparam.item_list[j + 1];
					m_mail_list[i]->mail_param.contentparam.item_list[j + 1].Reset();
				}
				index--;	// 后面的数据往前移了,继续检查这个下标
			}
		}

		if (is_clear)
		{
			strcat(m_mail_list[i]->mail_param.contentparam.contenttxt, gamestring::g_randactivity_god_beast_advent_receive_content);
		}
	}

	return count;
}

bool RoleMail::FetchMailList(bool force /*= false*/)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!force && m_last_get_list_time + MIN_GET_MAIL_LIST_INTERVAL >= now) return false;
	
	m_last_get_list_time = now;

	static Protocol::SCMailListAck mla;
	mla.is_init = 1;
	mla.count = 0;

	int count = 0;
	for (int i = 0; i < MAX_MAIL_NUM && count < MAX_MAIL_NUM; ++i)
	{
		if (NULL == m_mail_list[i] || m_mail_list[i]->Invalid()) continue;

		mla.mails[count].mail_index = m_mail_list[i]->mail_param.mail_index;
		mla.mails[count].has_attachment = m_mail_list[i]->HasAttachment() ? 1 : 0;
		mla.mails[count].mail_status = m_mail_list[i]->mail_param.status;

		mla.mails[count].subject_length = m_mail_list[i]->mail_param.contentparam.subject_length;
		F_STRNCPY(mla.mails[count].subject, m_mail_list[i]->mail_param.contentparam.subject, sizeof(mla.mails[count].subject));

#ifdef _DEBUG
		assert(mla.mails[count].subject_length == strlen(mla.mails[count].subject));
#else
		if (0 == mla.mails[count].subject_length)
		{
			mla.mails[count].subject_length = strlen(mla.mails[count].subject);
		}
#endif


		++count;
		if (count >= MAX_MAIL_SEND_LIST_SIZE)
		{
			mla.count = count;
			EngineAdapter::Instance().NetSendSerializeMsg(m_mgr->GetRole()->GetNetId(), &mla);

			count = 0;
			mla.is_init = 0;
		}
	}

	if (count >= 0)
	{
		mla.count = count;
		EngineAdapter::Instance().NetSendSerializeMsg(m_mgr->GetRole()->GetNetId(), &mla);
	}
	

	return true;
}

bool RoleMail::FetchMailDetail(int mail_index)
{
	if (mail_index < 0 || mail_index >= MAX_MAIL_NUM || NULL == m_mail_list[mail_index] || m_mail_list[mail_index]->Invalid())
	{
		return false;
	}

	Protocol::SCMailDetailAck mda;
	mda.mail_index = static_cast<short>(mail_index);
	mda.mail_reason = static_cast<short>(m_mail_list[mail_index]->mail_param.status.reason);

	memcpy(mda.subject, m_mail_list[mail_index]->mail_param.contentparam.subject, sizeof(MailSubject));
	memcpy(mda.contenttxt, m_mail_list[mail_index]->mail_param.contentparam.contenttxt, sizeof(MailContentTxt));
	mda.item_count = 0;
	int count = 0;
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		MailContentParam& mcp = m_mail_list[mail_index]->mail_param.contentparam;
		if (0 == mcp.item_list[i].item_id) continue;

		const ItemBase* item = ITEMPOOL->GetItem(mcp.item_list[i].item_id);
		if (NULL == item) continue;

		mda.item_list[count].item_id = mcp.item_list[i].item_id;
		mda.item_list[count].column = 0;
		mda.item_list[count].index = 0;
		mda.item_list[count].num = mcp.item_list[i].num;
		mda.item_list[count].is_bind = (char)mcp.item_list[i].is_bind;
		mda.item_list[count].invalid_time = mcp.item_list[i].invalid_time;
		mda.item_list[count].item_unique_id = mcp.item_list[i].item_unique_id;
		mda.item_list[count].is_market_buy_flag = mcp.item_list[i].is_market_buy_flag;
		mda.item_list[count].reserve_ch = 0;
		mda.item_list[count].has_param = 0;
		if (NULL != mcp.item_list[i].param_data && mcp.item_list[i].has_param > 0)
		{
			mda.item_list[count].has_param = 1;
			int param_length = NetValueItemParam::GetDataLengthByItemType(item->GetItemType());
			param_length = GetMin(ARRAY_ITEM_COUNT(mda.item_list[count].param_data), param_length);

			mda.item_list[count].param_length = param_length;
			memcpy(mda.item_list[count].param_data, mcp.item_list[i].param_data, param_length);
		}
		++count;
	}
	mda.item_count = count;
	mda.virtual_item_list = m_mail_list[mail_index]->mail_param.contentparam.virtual_item_list;

	EngineAdapter::Instance().NetSendSerializeMsg(m_mgr->GetRole()->GetNetId(), &mda);
	
	if (!m_mail_list[mail_index]->mail_param.status.is_read) // 这里不再通知客户端设置成已读 客户端自己设置成已读 
	{
		m_mail_list[mail_index]->mail_param.status.is_read = 1;
		m_dirty_mark[mail_index] = true;
		m_is_change = true;
	}

	return true;
}

bool RoleMail::FetchAttachment(int mail_index, int item_index, int is_last)
{
	Role* role = m_mgr->GetRole();

	static Protocol::SCFetchAttachmentAck fmaa;
	fmaa.mail_index = mail_index;
	fmaa.item_index = item_index;

	if (mail_index < 0 || mail_index >= MAX_MAIL_NUM ||
		NULL == m_mail_list[mail_index] ||
		m_mail_list[mail_index]->Invalid() ||
		!m_mail_list[mail_index]->HasAttachment() ||
		0 != m_mail_list[mail_index]->mail_param.status.is_fetch)
	{
		fmaa.ret = -1;
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&fmaa, sizeof(fmaa));

		gamelog::g_log_rolemail.printf(LL_INFO, "mail -> fetch attachment[ ret:fail, uid:%d, mail_index:%d item_index:%d]", role->GetUID(), mail_index, item_index);

		return false;
	}
	else
	{
		if (!m_mail_list[mail_index]->OperTimeIsValid())  // 不允许频繁对一封邮件进行提取操作 提取流程是异步的 (客户端连续发多次提取邮件的包 就有可能提取多分邮件) 
		{
			gamelog::g_log_rolemail.printf(LL_INFO, "mail->fetch attachment[ ret:fail, oper time is invalid uid:%d, mail_index:%d item_index:%d]", role->GetUID(), mail_index, item_index);
			return false;
		}

		static ggprotocal::MailAttachment attachment;
		attachment.sender_uid = m_mail_list[mail_index]->mail_param.status.sender_uid;
		attachment.mail_index = mail_index;
		attachment.item_index = item_index;
		attachment.is_last = is_last;
		F_STRNCPY(attachment.subject, m_mail_list[mail_index]->mail_param.contentparam.subject, sizeof(attachment.subject));
		F_STRNCPY(attachment.contenttxt, m_mail_list[mail_index]->mail_param.contentparam.contenttxt, sizeof(attachment.contenttxt));
		F_STRNCPY(attachment.serial_num, m_mail_list[mail_index]->mail_param.mail_serial, sizeof(attachment.serial_num));
		attachment.item_count = 0;

		if (item_index >= 0)
		{
			if (item_index >= MAX_ATTACHMENT_ITEM_NUM || m_mail_list[mail_index]->mail_param.contentparam.item_list[item_index].Invalid())
			{
				fmaa.ret = -1;
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&fmaa, sizeof(fmaa));

				gamelog::g_log_rolemail.printf(LL_INFO, "mail->fetch attachment[ret:fail, uid:%d, mail_index:%d item_index:%d]", role->GetUID(), mail_index, item_index);

				return false;
			}

			attachment.item_count = 1;
			attachment.item_list[0] = m_mail_list[mail_index]->mail_param.contentparam.item_list[item_index];
		}
		else
		{
			if (MAIL_FETCH_ALL == item_index)
			{
				attachment.item_count = 0;
				for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; i++)
				{
					if (!m_mail_list[mail_index]->mail_param.contentparam.item_list[i].Invalid())
					{
						m_mail_list[mail_index]->mail_param.contentparam.item_status.item_status[i] = ATTACHMENT_ITEM_STATUS_GET;
						attachment.item_list[attachment.item_count++] = m_mail_list[mail_index]->mail_param.contentparam.item_list[i];
					}
				}
			}

			if (MAIL_FETCH_ALL == item_index || MAIL_FETCH_VIRTUAL == item_index)
			{
				for (int i = 0; i < MAX_VIRTUAL_ITEM_NUM; ++i)
				{
					if (m_mail_list[mail_index]->mail_param.contentparam.virtual_item_list.virtual_item[i] != 0)
					{
						m_mail_list[mail_index]->mail_param.contentparam.item_status.item_status[i + MAX_ATTACHMENT_ITEM_NUM] = ATTACHMENT_ITEM_STATUS_GET;
					}
				}
				attachment.virtual_item_list = m_mail_list[mail_index]->mail_param.contentparam.virtual_item_list;
				memcpy(attachment.item_status, m_mail_list[mail_index]->mail_param.contentparam.item_status.item_status, sizeof(attachment.item_status));
			}
		}

		if (attachment.item_count > 0 && !role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList(attachment.item_count, attachment.item_list))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}

		MailRoute::Instance().OnMailAttachment(role, &attachment);

		return true;
	}
}

bool RoleMail::FetchAttachmentAffirm(int mail_index, int result, int item_index, int is_last)
{
	if (0 != result) return false;
	if (mail_index < 0 || mail_index >= MAX_MAIL_NUM || NULL == m_mail_list[mail_index] ||
		m_mail_list[mail_index]->Invalid() || !m_mail_list[mail_index]->HasAttachment())
	{
		return false;
	}

	MailParam &mail_param = m_mail_list[mail_index]->mail_param;

	gamelog::g_log_rolemail.printf(LL_INFO, "mail->fetch attachment affirm[ret:succ, recver[uid:%d], item_index[%d], mail_param[%s]",
		m_mgr->GetRole()->GetUID(), item_index, MailParamLog(&mail_param));

	m_dirty_mark[mail_index] = true;
	m_is_change = true;

	m_mail_list[mail_index]->mail_param.status.is_read = 1;
	m_mail_list[mail_index]->mail_param.status.is_fetch = 1;

	static Protocol::SCFetchAttachmentAck fmaa;
	fmaa.mail_index = mail_index;
	fmaa.item_index = item_index;
	fmaa.ret = 0;
	EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&fmaa, sizeof(fmaa));

	return true;
}

void RoleMail::OneKeyFetchAttachment()
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (now < m_last_one_key_fetch_time + MAIL_ONEKEY_FETCH_TIME_INTERVAL)
	{
		m_mgr->NoticeNum(errornum::EN_ONEKEY_FETCH_CD_LIMIT);
		return;
	}
	m_last_one_key_fetch_time = now;

	for (int mail_index = 0; mail_index < MAX_MAIL_NUM; ++mail_index)
	{
		if (NULL == m_mail_list[mail_index] || m_mail_list[mail_index]->Invalid())
		{
			continue;
		}

		if (m_mail_list[mail_index]->HasAttachment())
		{
			this->FetchAttachment(mail_index, MAIL_FETCH_ALL, 0);
		}
		else if (0 == m_mail_list[mail_index]->mail_param.status.is_read)
		{
			this->FetchMailDetail(mail_index);
		}
	}
	this->FetchMailList(true);
}

bool RoleMail::CleanMail()
{
	bool is_first_notice = true;
	for (int index = 0; index < MAX_MAIL_NUM; index++)
	{
		if (NULL == m_mail_list[index] ||
			m_mail_list[index]->Invalid() ||
			0 != m_mail_list[index]->mail_param.status.is_lock ||
			(m_mail_list[index]->HasAttachment() && 0 == m_mail_list[index]->mail_param.status.is_fetch) ||
			0 == m_mail_list[index]->mail_param.status.is_read)
		{
			if (is_first_notice && NULL != m_mail_list[index] && !m_mail_list[index]->Invalid() && m_mail_list[index]->HasAttachment() && 0 == m_mail_list[index]->mail_param.status.is_fetch)
			{
				m_mgr->NoticeNum(errornum::EN_ITEM_NOT_FETCH_ATTACHMENT);
				is_first_notice = false;
			}

			continue;
		}

		delete m_mail_list[index];
		m_mail_list[index] = NULL;

		m_empty_slot_queue.push(index);

		m_dirty_mark[index] = true;
		m_is_change = true;
	}

	return this->FetchMailList(true);
}

void RoleMail::SendMailAck(const MailItem *mail_item, bool is_succ)
{
	if (NULL == mail_item) return;

	if (is_succ)
	{
		static Protocol::SCRecvNewMail new_mail;
		new_mail.mail_brief.mail_index = mail_item->mail_param.mail_index;
		new_mail.mail_brief.has_attachment = mail_item->HasAttachment() ? 1 : 0;
		new_mail.mail_brief.subject_length = mail_item->mail_param.contentparam.subject_length;
		
#ifdef _DEBUG
		assert(new_mail.mail_brief.subject_length == strlen(mail_item->mail_param.contentparam.subject));
#else
		if (0 == new_mail.mail_brief.subject_length)
		{
			new_mail.mail_brief.subject_length = strlen(mail_item->mail_param.contentparam.subject);
		}
#endif
		
		new_mail.mail_brief.mail_status = mail_item->mail_param.status;
		F_STRNCPY(new_mail.mail_brief.subject, mail_item->mail_param.contentparam.subject, sizeof(new_mail.mail_brief.subject));
		
		EngineAdapter::Instance().NetSendSerializeMsg(m_mgr->GetRole()->GetNetId(), &new_mail);
	}
	else
	{
		if (MAIL_TYPE_PERSONAL == mail_item->mail_param.status.kind)
		{
			// 这里 不能 循环发邮件
			gamelog::g_log_rolemail.printf(LL_INFO, "UserMail::SendMailAck receiver[%d] full sender[%d]",
				UidToInt(m_mgr->GetRole()->GetUID()), mail_item->mail_param.status.sender_uid);

			/*static MailContentParam contentparam; contentparam.Reset();
			contentparam = mail_item->mail_param.contentparam;
			m_usermail_manager->MailToUser(MAIL_TYPE_SYSTEM, IntToUid(mail_item->mail_param.status.sender_uid), SYSTEM_MAIL_REASON_MAIL_RECEIVER_FULL, contentparam); */
		}
	}

	if (MAIL_TYPE_PERSONAL != mail_item->mail_param.status.kind) return; // 系统邮件不通知发件人  

	Protocol::SCMailSendAck send_ack;
	send_ack.ret = is_succ ? 0 : -1;
	World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(mail_item->mail_param.status.sender_uid), (const char*)&send_ack, sizeof(send_ack));
}

void RoleMail::Release()
{
	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (NULL != m_mail_list[i])
		{
			delete m_mail_list[i];
			m_mail_list[i] = NULL;
		}
	}
	memset(m_old_state, 0, sizeof(m_old_state));
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
}

/**
* \brief 返回一个空的邮件位置
* \note 若找不到空的邮件位置，则返回一个最没有价值的邮件的位置
*/
MailItem * RoleMail::GetFreeMailItemSlot(int kind)
{
	if (kind <= MAIL_TYPE_INVALID || kind >= MAIL_TYPE_MAX) return NULL;

	while (!m_empty_slot_queue.empty())
	{
		int index = m_empty_slot_queue.front();
		m_empty_slot_queue.pop();

		if (NULL == m_mail_list[index])
		{
			m_mail_list[index] = new MailItem();
			m_mail_list[index]->mail_param.mail_index = index;
			return m_mail_list[index];
		}
		else if (m_mail_list[index]->Invalid())
		{
			m_mail_list[index]->mail_param.mail_index = index;
			return m_mail_list[index];
		}
	}


	// 没有找到空位，则用最笨的方法再找一次
	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (NULL == m_mail_list[i])
		{
			m_mail_list[i] = new MailItem();
			m_mail_list[i]->mail_param.mail_index = i;
			return m_mail_list[i];
		}
	}

	// 实在找不出空位，则要把最没有价值的邮件作为替代品
	// CmpPriority 这个函数要求邮件必须满 

	MailItem *mail_tmp = m_mail_list[0];
	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		mail_tmp = this->CmpPriority(mail_tmp, m_mail_list[i]);
	}

	return mail_tmp;
}

MailItem * RoleMail::CmpPriority(MailItem *mail1, MailItem *mail2)
{
	if (NULL == mail1 || NULL == mail2) return NULL;

	if (mail1->mail_param.status.is_lock == mail2->mail_param.status.is_lock)
	{
		if (mail1->HasAttachment() == mail2->HasAttachment())
		{
			if (mail1->mail_param.status.is_read == mail2->mail_param.status.is_read)
			{
				if (mail1->mail_param.status.kind == mail2->mail_param.status.kind)
				{
					if (mail1->mail_param.status.recv_time > mail2->mail_param.status.recv_time) return mail2;
					return mail1;
				}
				else
				{
					if (mail1->mail_param.status.kind == MAIL_TYPE_PERSONAL && mail2->mail_param.status.kind != MAIL_TYPE_PERSONAL) return mail1;
					return mail2;
				}
			}
			else
			{
				if (mail1->mail_param.status.is_read && !mail2->mail_param.status.is_read) return mail1;
				return mail2;
			}
		}
		else
		{
			if (mail1->HasAttachment() && !mail2->HasAttachment()) return mail2;
			return mail1;
		}
	}
	else
	{
		if (mail1->mail_param.status.is_lock && !mail2->mail_param.status.is_lock) return mail2;
		return mail1;
	}
}

void RoleMail::CheckMailItemDataVersion(MailParam& mp)
{
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		if (mp.contentparam.item_list[i].Invalid()) continue;

		if (0 == mp.contentparam.item_list[i].has_param) continue;

		const ItemBase* item = ITEMPOOL->GetItem(mp.contentparam.item_list[i].item_id);
		if (NULL != item && I_TYPE_PET == item->GetItemType())
		{
			// 检查宠物数据版本变更
			NetValueItemParam temp_param;
			temp_param.SetData(mp.contentparam.item_list[i].param_data);
			if (temp_param.pet_param.CheckGrowthVersion(mp.contentparam.item_list[i].item_unique_id, __FUNCTION__))
			{
				temp_param.GetData(mp.contentparam.item_list[i].param_data);
			}

			if (temp_param.pet_param.CheckModifyVersion())
			{
				temp_param.GetData(mp.contentparam.item_list[i].param_data);
			}
		}
		else if (NULL != item && I_TYPE_MEDAL == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(mp.contentparam.item_list[i].param_data);
			if (temp_param.medal_param.CheckModifyVersion())
			{
				temp_param.GetData(mp.contentparam.item_list[i].param_data);
			}
		}
		else if (NULL != item && I_TYPE_EQUIPMENT == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(mp.contentparam.item_list[i].param_data);
			if (temp_param.equip_param.CheckModifyVersion())
			{
				temp_param.GetData(mp.contentparam.item_list[i].param_data);
			}
		}
		else if (NULL != item && I_TYPE_JEWELRY == item->GetItemType())
		{
			NetValueItemParam temp_param;
			temp_param.SetData(mp.contentparam.item_list[i].param_data);
			if (temp_param.jewelry_param.CheckModifyVersion())
			{
				temp_param.GetData(mp.contentparam.item_list[i].param_data);
			}
		}
	}
}

bool RoleMail::RemoveMailHelper(int mail_index)
{
	if (
		mail_index < 0 ||
		mail_index >= MAX_MAIL_NUM ||
		NULL == m_mail_list[mail_index] ||
		m_mail_list[mail_index]->Invalid() ||
		0 != m_mail_list[mail_index]->mail_param.status.is_lock
		)
	{
		return false;
	}

	if (m_mail_list[mail_index]->HasAttachment() && 0 == m_mail_list[mail_index]->mail_param.status.is_fetch)
	{
		m_mgr->NoticeNum(errornum::EN_ITEM_NOT_FETCH_ATTACHMENT);
		return false;
	}

	gamelog::g_log_rolemail.printf(LL_INFO, "mail->remove mail[ret:succ, receiver[uid:%d], mail_param[%s]]",
		m_mgr->GetRole()->GetUID(), MailParamSimpleLog(&m_mail_list[mail_index]->mail_param));

	delete m_mail_list[mail_index];
	m_mail_list[mail_index] = NULL;

	m_is_change = true;
	m_dirty_mark[mail_index] = true;

	m_empty_slot_queue.push(mail_index);

	return true;
}

void RoleMail::CheckUnReadMail()
{
	/* 客户端暂不需要
	int unread_num = 0;
	int chongzhi_mail_num = 0;

	for (int i = 0; i < MAX_MAIL_NUM; ++i)
	{
		if (NULL == m_mail_list[i] || m_mail_list[i]->Invalid()) continue;

		if (0 == m_mail_list[i]->mail_param.status.is_read)
		{
			++unread_num;
		}

		if (MAIL_TYPE_CHONGZHI == m_mail_list[i]->mail_param.status.kind)
		{
			if (m_mail_list[i]->HasAttachment() || 0 == m_mail_list[i]->mail_param.status.is_read)
			{
				++chongzhi_mail_num;
			}
		}
	}

	if (unread_num > 0 || chongzhi_mail_num > 0)
	{
		
		Protocol::SCHasUnReadMail hurm;
		hurm.unread_num = unread_num;
		hurm.chongzhi_mail_num = chongzhi_mail_num;
		EngineAdapter::Instance().NetSend(m_mgr->GetRole()->GetNetId(), (const char*)&hurm, sizeof(hurm));
		
	}
	*/
}

void RoleMail::MailItemCheckMedal(ARG_OUT MailItem *mailitem)
{
	if (NULL == mailitem) return;

	for (int i = 0; i < ARRAY_ITEM_COUNT(mailitem->mail_param.contentparam.item_list); ++i)
	{
		if (mailitem->mail_param.contentparam.item_list[i].Invalid()) continue;

		ItemID item_id = mailitem->mail_param.contentparam.item_list[i].item_id;
		const ItemBase* item = ITEMPOOL->GetItem(item_id);
		if (NULL == item) continue;
		if (I_TYPE_MEDAL != item->GetItemType()) continue;

		NetValueItemParam tmp_item_param;
		tmp_item_param.SetData(mailitem->mail_param.contentparam.item_list[i].param_data);
		if (!item->HasRandomNetValue(&tmp_item_param))
		{
			item->RandomNetValueOnPutItem(NULL, &tmp_item_param, 0);
			item->SetRandomNetValueFlag(&tmp_item_param);
			tmp_item_param.GetData(mailitem->mail_param.contentparam.item_list[i].param_data);
		}
		mailitem->mail_param.contentparam.item_list[i].has_param = 1;
	}
}

void RoleMail::LogMailContent(const MailParam& param, const char* caller)
{
	gamelog::g_log_mail.buff_printf("RoleMail::LogMailContent caller[%s] user[%d,%s] serial_num[%s] attachments[",
		caller, m_mgr->GetUid(), m_mgr->GetRoleName(), param.mail_serial);
	for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
	{
		const MailContentParam& p = param.contentparam;
		if (p.item_list[i].Invalid()) continue;

		const ItemDataWrapper& item = p.item_list[i];
		gamelog::g_log_mail.buff_printf("{item[%d,%d,%d] param[%s]},",
			item.item_id, item.num, item.is_bind,
			NetValueItemParam::GetParamLog(item.item_id, item.param_data));
	}

	gamelog::g_log_mail.buff_printf("]");
	gamelog::g_log_mail.commit_buff(LL_INFO);
}

