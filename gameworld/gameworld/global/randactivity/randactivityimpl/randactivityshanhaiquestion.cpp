#include "randactivityshanhaiquestion.hpp"
#include "global/usercache/usercache.hpp"
#include "world.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/activitydef.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "obj/character/role.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaiquestionconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"



RandActivityShanHaiQuestion::RandActivityShanHaiQuestion(RandActivityManager *activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SHANHAI_QUESTION)
{
	start_time = 0;
	end_time = 0;
	is_open = false;
}

RandActivityShanHaiQuestion::~RandActivityShanHaiQuestion()
{

}

void RandActivityShanHaiQuestion::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (start_time >= end_time || start_time < GetZeroTime(now_second))	//隔天
	{
		this->SetActivityTime();
	}

	if (start_time >= end_time) return;

	if (this->GetStatus() == ACTIVITY_STATUS_OPEN &&start_time <= now_second && end_time > now_second && is_open==false)
	{
		is_open = true;

		this->SendActivityStatusAllInfo();
	}
	else if (this->GetStatus() == ACTIVITY_STATUS_OPEN && (start_time > now_second || end_time <= now_second) && is_open==true)
	{
		is_open = false;

		this->SendActivityStatusAllInfo();
	}	

}

void RandActivityShanHaiQuestion::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		this->SetActivityTime();
	}
	if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		is_open = false;
	}
}

/**
* 输入格式：1100
* 输出时间戳
*/
time_t RandActivityShanHaiQuestion::ForHourMinuteGetTimestamp(int hour_minute)
{
	int t_hour = hour_minute / 100;
	int t_minute = hour_minute % 100;

	if (0 > t_hour || t_hour >= 24) return 0;
	if (0 > t_minute || t_minute >= 60) return 0;

	time_t now_time = EngineAdapter::Instance().Time();
	int next_day_zeor_time = EngineAdapter::Instance().NextDayInterval(0, 0, 0);

	int cut_time = ((24 - t_hour) * 3600) - (t_minute * 60);

	time_t r_timestamp = now_time + next_day_zeor_time - cut_time;

	return r_timestamp;
}

void RandActivityShanHaiQuestion::SendActivityStatusAllInfo()
{
	Protocol::SCRAShanHaiQuestionStatusInfo msg;

	int open_type = is_open ? 1 : 0;
	msg.is_can_answer = open_type;

	World::GetInstWorld()->SendToAllGateway((const char*)&msg, sizeof(msg));
}

void RandActivityShanHaiQuestion::SetActivityTime()
{
	const RandActivityShanHaiQuestionConfig * config = static_cast<const RandActivityShanHaiQuestionConfig *>			//获取配置
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION));
	if (NULL == config)
	{
		return;
	}

	const QuestionOtherCfg & other_cfg = config->GetQuertionOtherCfg();

	if (other_cfg.start_times >= other_cfg.end_times) return;

	start_time = this->ForHourMinuteGetTimestamp(other_cfg.start_times);
	end_time = this->ForHourMinuteGetTimestamp(other_cfg.end_times);
}
