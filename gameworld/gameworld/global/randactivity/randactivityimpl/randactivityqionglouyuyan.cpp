#include "randactivityqionglouyuyan.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "gameworld/gamelog.h"
#include "gameworld/engineadapter.h"
#include "obj/character/role.h"
#include "config/randactivityconfig/impl/randactivityqionglouyuyanconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/errornum.h"

RandActivityQiongLouYuYan::RandActivityQiongLouYuYan(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN), next_send_all_tamp(0)
{
}

RandActivityQiongLouYuYan::~RandActivityQiongLouYuYan()
{
}

void RandActivityQiongLouYuYan::Init(const ActivityData & data)
{
	RandActivity::Init(data);
	const RandActivityData * ra_data = &data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	m_param = *(RAQiongLouYuYanParam *)ra_data->data;
}

void RandActivityQiongLouYuYan::GetInitParam(ActivityData & data)
{
	RandActivity::GetInitParam(data);

	RandActivityData * ra_data = &data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	*(RAQiongLouYuYanParam *)ra_data->data = m_param;
}

void RandActivityQiongLouYuYan::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (m_param.next_status_tamp <= now_second)
	{
		const RandActivityQiongLouYuYanConfig * config = static_cast<const RandActivityQiongLouYuYanConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN));
		if (config == NULL)return;
		const RAQiongLouYuYanOtherCfg & other_cfg = config->GetOtherCfg();

		if (m_param.next_status_tamp == 0)
		{
			//初始化
			m_param.now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_PREPARATION;
			int hour_p = other_cfg.start_time / 100;
			int min_p = other_cfg.start_time % 100;
			m_param.next_status_tamp = (unsigned int)now_second + EngineAdapter::Instance().NextDayInterval(hour_p, min_p, 0) + (other_cfg.phase_times - 1) * PER_DAY_SECOND;
			this->SendInfo();
		}
		else
		{
			switch (m_param.now_status)
			{
			case QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_PREPARATION:
				m_param.now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_READY;
				m_param.next_status_tamp = other_cfg.prepare_times + (unsigned int)now_second;
				this->SendInfo();
				break;
			case QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_READY:
				m_param.now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BEGAIN;
				m_param.next_status_tamp = other_cfg.banquet_times + (unsigned int)now_second;
				this->CheckAccumulate();
				this->SendInfo();
				break;
			case QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BEGAIN:
				m_param.now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BLANK;
				m_param.next_status_tamp = (unsigned int)now_second + EngineAdapter::Instance().NextDayInterval(0, 0, 0);
				this->SendInfo();
				break;
			case QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BLANK:
				{
					m_param.OnPartEnd();
					m_param.open_times = now_second;
					member_join_vec.clear();
					m_param.now_status = QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_PREPARATION;
					int hour_p = other_cfg.start_time / 100;
					int min_p = other_cfg.start_time % 100;
					m_param.next_status_tamp = (unsigned int)now_second + EngineAdapter::Instance().NextDayInterval(hour_p, min_p, 0) + (other_cfg.phase_times - 1) * PER_DAY_SECOND;
					this->SendInfo();
				}
				break;
			default:
				break;
			}
		}
	}

	if (next_send_all_tamp > 0 && next_send_all_tamp <= now_second)
	{
		this->SendInfo();
		next_send_all_tamp = 0;
	}
}

void RandActivityQiongLouYuYan::OnUserLogin(Role * user)
{
	RandActivity::OnUserLogin(user);
	if(ACTIVITY_STATUS_OPEN == this->GetStatus())
	{
		this->SendInfo(user);
	}
}

void RandActivityQiongLouYuYan::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		this->SendInfo();
	}
	else if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		m_param.Reset();
	}
}

bool RandActivityQiongLouYuYan::CanEatFood(Role * role)
{
	if (m_param.banquet_type == BANQUET_TYPE_NO || m_param.now_status != QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_BEGAIN)
	{
		role->NoticeNum(errornum::EN_QIONG_LOU_YU_YAN_NOT_YAN_HUI);
		return false;
	}

	return true;
}

bool RandActivityQiongLouYuYan::OnRoleAdd(Role * role, int add_num)
{
	if (m_param.now_status != QIONG_LOU_YU_YAN_STATUS_TYPE_NEXT_PREPARATION)
	{
		role->NoticeNum(errornum::EN_QIONG_LOU_YU_YAN_NO_PREPARATION_TIMES);
		return false;
	}
	m_param.accumulate_num += add_num;
	bool has_find = false;
	for (std::vector<UserID>::iterator it = member_join_vec.begin(); it != member_join_vec.end(); it++)
	{
		if (*it == role->GetUserId())
		{
			has_find = true;
		}
	}
	if (!has_find)
	{
		member_join_vec.push_back(role->GetUserId());
	}
	this->NextSendAllInfo();
	return true;
}

void RandActivityQiongLouYuYan::SendInfo(Role * role)
{
	Protocol::SCQiongLouYuYanInfo info;
	info.accumulate_num = m_param.accumulate_num;
	info.now_status = m_param.now_status;
	info.next_status_tamp = m_param.next_status_tamp;
	info.banquet_type = m_param.banquet_type;
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, sizeof(info));
	}
}

void RandActivityQiongLouYuYan::GmAddAccumulate(int add_num)
{
	m_param.accumulate_num += add_num;
	this->NextSendAllInfo();
}

void RandActivityQiongLouYuYan::NextSendAllInfo()
{
	time_t now_second = EngineAdapter::Instance().Time();
	if (next_send_all_tamp < now_second)
	{
		next_send_all_tamp = now_second + 2;//延迟2秒刷新一次
	}
}

void RandActivityQiongLouYuYan::CheckAccumulate()
{
	const RandActivityQiongLouYuYanConfig * config = static_cast<const RandActivityQiongLouYuYanConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN));
	if (config == NULL)return;

	int open_day = config->GetActRealOpenDay(m_rand_activity_manager, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	const RAQiongLouYuYanConfig * banquet_cfg = config->GetConfig(open_day, m_param.accumulate_num);
	if (banquet_cfg == NULL)
	{
		return;
	}

	m_param.banquet_type = banquet_cfg->banquet_type;

	if (m_param.banquet_type == BANQUET_TYPE_NO)
	{
		//未满足条件
		for (std::vector<UserID>::iterator it = member_join_vec.begin(); it != member_join_vec.end(); it++)
		{
			static MailContentParam contentparam; contentparam.Reset();
			const RAQiongLouYuYanRewardConfig * reward_cfg = config->RandRewardByGroup(banquet_cfg->reward_group);
			if (reward_cfg == NULL)continue;
			int reward_num = 0;
			for (std::vector<ItemConfigData>::const_iterator item_it = reward_cfg->m_reward_vec.begin();
				item_it != reward_cfg->m_reward_vec.end() && reward_num < MAX_ATTACHMENT_ITEM_NUM; item_it++)
			{
				contentparam.item_list[reward_num].item_id = item_it->item_id;
				contentparam.item_list[reward_num].num = item_it->num;
				contentparam.item_list[reward_num++].is_bind = item_it->is_bind ? 1 : 0;
			}
			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_qiong_lou_yu_yan_subject, RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_qiong_lou_yu_yan_content);
			if (length1 > 0 && length2 > 0)
			{
				MailRoute::MailToUser((*it), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
	this->SendInfo();
}
