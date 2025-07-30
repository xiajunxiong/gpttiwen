#include "rolehongmengtianjie.hpp"

#include "other/hongmengtianjie/hongmengtianjieconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/maildef.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/event/eventhandler.hpp"
#include "engineadapter.h"
#include "protocol/msghongmengtianjie.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"
#include "config/otherconfig/knapsack_config.hpp"

RoleHongMengTianJie::RoleHongMengTianJie()
{
}

RoleHongMengTianJie::~RoleHongMengTianJie()
{
}

void RoleHongMengTianJie::Init(RoleModuleManager * mgr, const RoleHongMengTianJieParam & param)
{
	m_mgr = mgr;
	m_param = param;
}

void RoleHongMengTianJie::GetInitParam(RoleHongMengTianJieParam * param)
{
	*param = m_param;
}

void RoleHongMengTianJie::Update(unsigned int now_second)
{
	bool is_send = false;
	if (now_second >= m_param.next_help_refresh_time)
	{
		m_param.help_times += LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_times;
		if (m_param.help_times > LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_reward_limit)
			m_param.help_times = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_reward_limit;

		if(0 == m_param.next_help_refresh_time) m_param.help_times = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_reward_limit;

		unsigned int next_refresh_time = 0;
		for (int i = 0; i < (int)LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_reward_repeat.size(); i++)
		{
			int refresh_day = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().help_reward_repeat[i];
			unsigned int temp_time = EngineAdapter::Instance().NextWeekInterval(refresh_day, 0, 0, 0);

			if (next_refresh_time == 0 || temp_time < next_refresh_time)
			{
				next_refresh_time = temp_time;
			}
		}

		m_param.next_help_refresh_time = now_second + next_refresh_time;
		is_send = true;
	}
	if (now_second >= m_param.next_reward_refresh_time)
	{
		for (int i = 0; i < HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM; i++)
		{
			m_param.reward_times[i] += LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().reward_times[i];
			if (m_param.reward_times[i] > LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().boss_reward_limit)
				m_param.reward_times[i] = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().boss_reward_limit;

			if (0 == m_param.next_reward_refresh_time) m_param.reward_times[i] = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().boss_reward_limit;
		}

		unsigned int next_refresh_time = 0;
		for (int i = 0; i < (int)LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().boss_reward_repeat.size(); i++)
		{
			int refresh_day = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetOtherCfg().boss_reward_repeat[i];
			unsigned int temp_time = EngineAdapter::Instance().NextWeekInterval(refresh_day, 0, 0, 0);

			if (next_refresh_time == 0 || temp_time < next_refresh_time)
			{
				next_refresh_time = temp_time;
			}
		}

		m_param.next_reward_refresh_time = now_second + next_refresh_time;
		is_send = true;
	}

	if (is_send) this->SendInfo();
}

void RoleHongMengTianJie::OnWeekChange()
{
	memset(m_param.limit_buy_times, 0, sizeof(m_param.limit_buy_times));

	this->SendInfo();
}

void RoleHongMengTianJie::SendInfo()
{
	Protocol::SCHongMengTianJieRoleInfo info;
	info.help_times = m_param.help_times;
	memcpy(info.reward_times, m_param.reward_times, sizeof(info.reward_times));
	memcpy(info.limit_buy_times, m_param.limit_buy_times, sizeof(info.limit_buy_times));
	info.fb_first_pass_mark = m_param.fb_first_pass_mark;

	m_mgr->NetSend(&info, sizeof(info));
}

bool RoleHongMengTianJie::IsBossTimes(bool is_all, int boss_index)
{
	bool is_have_times = false;

	if (is_all)
	{
		for (int i = 0; i < HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM; i++)
		{
			if (m_param.reward_times[i] > 0)
			{
				is_have_times = true;
				break;
			}
		}

		if (!is_have_times && m_param.help_times > 0) is_have_times = true;
	}
	else
	{
		if (boss_index < 0 || boss_index >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
			return false;

		if (m_param.reward_times[boss_index] > 0)
		{
			is_have_times = true;
		}
	}

	return is_have_times;
}

bool RoleHongMengTianJie::IsZhuZhuan(int boss_index)
{
	if (boss_index < 0 || boss_index >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
		return false;

	bool is_have_times = false;

	if (m_param.reward_times[boss_index] <= 0 && m_param.help_times > 0)
	{
		is_have_times = true;
	}

	return is_have_times;
}

bool RoleHongMengTianJie::IsFBPass(int fb_id)
{
	if (fb_id < 0 || fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM)
		return false;

	return m_param.fb_first_pass_mark.IsBitSet(fb_id);
}

void RoleHongMengTianJie::SendBossReward(int fb_id, int boss_index, std::vector<ItemConfigData>& item_list)
{
	if (fb_id < 0 || fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM)
		return;

	if (boss_index < 0 || boss_index >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
		return;

	const HongMengTianJieRewardCfg * reward_cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetRewardCfg(fb_id);
	if (NULL == reward_cfg)
		return;
	
	int reward_type = 0;
	bool is_send = false;
	if (m_param.reward_times[boss_index] <= 0)
	{
		if (m_param.help_times > 0)
		{
			item_list = reward_cfg->help_reward;

			m_param.help_times--;

			reward_type = 2; //代表助战奖励
			is_send = true;
		}
	}
	else
	{
		for (int i = 0; i < (int)reward_cfg->success_reward[boss_index].size(); i++)
		{
			int group_id = reward_cfg->success_reward[boss_index].at(i);
			const HongMengTianJieItemCfg * cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetGroupReward(group_id);
			if(NULL == cfg)
				continue;

			ItemConfigData item = cfg->item;	
			const ItemBase *itembase = ITEMPOOL->GetItem(item.item_id);
			if (NULL == itembase) continue;

			if (itembase->GetItemType() == I_TYPE_CONVERT_ITEM)
			{
				item.item_id = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(item.item_id, m_mgr->GetRole()->GetAppearanceProf());
			}

			item_list.push_back(item);

			if (cfg->is_broadcast)
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_hmtj_reward_notice,
					m_mgr->GetRoleUID(), m_mgr->GetRole()->GetName(), item.item_id);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
				}
			}
		}

		m_param.reward_times[boss_index]--;
		reward_type = 1;		// 代表BOSS 奖励
		is_send = true;
	}

	if (!item_list.empty())
	{
		if (m_mgr->GetKnapsack()->CheckForPutList2(item_list))
		{
			m_mgr->GetKnapsack()->PutVec(item_list, PUT_REASON_HONGMENG_TIANJIE);
		}
		else
		{
			m_mgr->GetKnapsack()->SendMail(item_list, SNED_MAIL_TYPE_DEFAULT, true);
		}

		// log
		gamelog::g_hongmeng_tianjie.printf(LL_INFO, "%s user[%d, %s], reward_type[%d] fb_id[%d], boss_index[%d]", __FUNCTION__, m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
			reward_type, fb_id, boss_index);
	}

	//if (reward_type == 0) m_mgr->NoticeNum(errornum::EN_HONGMENG_TIANJIE_NOT_REWARD_TIMES);
	if (reward_type == 1) m_mgr->GetKnapsack()->NotifyGetItemListVec(item_list, GET_REASON_HONGMENG_TIANJIE_PASS);	

	if (boss_index == HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM - 1)
	{
		bool is_first = false;
		if (!m_param.fb_first_pass_mark.IsBitSet(fb_id))
		{
			is_first = true;
			m_param.fb_first_pass_mark.SetBit(fb_id);
			is_send = true;

			static MailContentParam contentparam; contentparam.Reset();
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_hmtj_role_first_reward_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_hmtj_role_first_reward_content, fb_id);

			ItemDataWrapper item_info;
			int count = 0;
			for (int i = 0; i < (int)reward_cfg->person_reward.size() && i < MAX_ATTACHMENT_ITEM_NUM; i++)
			{
				const ItemBase * item_base = ITEMPOOL->GetItem(reward_cfg->person_reward[i].item_id);
				if (NULL == item_base) continue;

				item_info.item_id = reward_cfg->person_reward[i].item_id;
				item_info.is_bind = reward_cfg->person_reward[i].is_bind;
				item_info.num = reward_cfg->person_reward[i].num;
				contentparam.item_list[count++] = item_info;
			}

			MailRoute::MailToUser(m_mgr->GetRoleUID(), MAIL_REASON_DEFAULT, contentparam);

			// log
			gamelog::g_hongmeng_tianjie.printf(LL_INFO, "fb_first_pass user[%d, %s], fb_id[%d]", m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(),
				fb_id);
		}
		EventHandler::Instance().OnPassHongMengTianJie(m_mgr->GetRole(), fb_id, is_first);
	}

	if (is_send) this->SendInfo();
}

void RoleHongMengTianJie::OnBuyShop(int shop_id, int num)
{
	const HongMengTianJieStoreCfg * cfg = LOGIC_CONFIG->GetHongMengTianJieConfig()->GetStoreCfg(shop_id);
	if (NULL == cfg)
		return;

	if (cfg->limit_seq >= 0 && cfg->limit_seq < HongMengTianJie::MAX_LIMIT_SEQ_NUM)
	{
		if (m_param.limit_buy_times[cfg->limit_seq] + num > cfg->limit_convert_count)
		{
			m_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
			return;
		}
	}

	if (!m_mgr->GetKnapsack()->ConsumeItem(cfg->conver_item_id, cfg->price * num, __FUNCTION__))
	{
		m_mgr->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	ItemConfigData item(cfg->item_id, true, cfg->num * num);
	if (m_mgr->GetKnapsack()->CheckForPut2(item, false, false))
	{
		
		m_mgr->GetKnapsack()->Put(item, PUT_REASON_HONGMENG_TIANJIE_BUY);
	}
	else
	{
		m_mgr->GetKnapsack()->SendMail(&item, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	if (cfg->limit_seq >= 0 && cfg->limit_seq < HongMengTianJie::MAX_LIMIT_SEQ_NUM)
	{
		int new_num = m_param.limit_buy_times[cfg->limit_seq] + num;
		if (new_num > HongMengTianJie::MAX_LIMIT_ITEM_NUM) new_num = HongMengTianJie::MAX_LIMIT_ITEM_NUM;
		m_param.limit_buy_times[cfg->limit_seq] = new_num;
	}

	this->SendInfo();

	//log
	gamelog::g_hongmeng_tianjie.printf(LL_INFO, "%s user[%d, %s], shop_id[%d], shop_num[%d]", __FUNCTION__, m_mgr->GetRole()->GetUID(), m_mgr->GetRole()->GetName(), 
		shop_id, num);
}
