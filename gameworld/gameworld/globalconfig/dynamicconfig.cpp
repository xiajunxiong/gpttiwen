#include "dynamicconfig.hpp"

#include "servercommon/configcommon.h"

#include "engineadapter.h"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/struct/global/worldteamarenaparam.hpp"

DynamicConfig::DynamicConfig()
{

}

DynamicConfig::~DynamicConfig()
{

}

bool DynamicConfig::Init(const std::string &path, std::string *err)
{
	UNSTD_STATIC_CHECK(MAX_ACTIVITY_OPEN_WEEKDAY > 0);

	char errinfo[1024] = {0}; 
	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path))
	{
		*err = path + " Load fail: " +document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}

	{
		ActivityCfg temp_activitycfg_list[ACTIVITY_TYPE_MAX];

		TiXmlElement *activitylistElement = RootElement->FirstChildElement("activitylist");
		if (!activitylistElement)
		{
			*err = path + ": xml node error in [config->activitylist]";
			return false;
		}

		TiXmlElement *activityElement = activitylistElement->FirstChildElement("activity");

		while (activityElement != NULL)
		{
			int type = 0;
			if (!GetSubNodeValue(activityElement, "type", type) || type <= ACTIVITY_TYPE_INVALID || type >= ACTIVITY_TYPE_MAX)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->type:%d].", path.c_str(), type);
				*err = errinfo;
				return false;
			}
			
			ActivityCfg *activity_cfg = &temp_activitycfg_list[type];

			if (!activity_cfg->Invalid())
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->type:%d repeat].", path.c_str(), type);
				*err = errinfo;
				return false;
			}
			activity_cfg->type = type;

			int is_open = 0;
			if (!GetSubNodeValue(activityElement, "is_open", is_open) || is_open < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->is_open:%d].", path.c_str(), is_open);
				*err = errinfo;
				return false;
			}
			activity_cfg->is_open = (0 != is_open);
			int weekday_count = 0;
			int activity_time_count = 0;
			{
				TiXmlElement *weekdaylistElement = activityElement->FirstChildElement("weekdaylist");
				if (!weekdaylistElement)
				{
					*err = path + ": xml node error in [config->activitylist->activity->weekdaylist]";
					return false;
				}

				TiXmlElement *weekdayElement = weekdaylistElement->FirstChildElement("weekday");
				if (!weekdayElement)
				{
					*err = path + ": xml node error in [config->activitylist->activity->weekdaylist->weekday]";
					return false;
				}

				while (weekdayElement)
				{
					int week_day = 0;

					if (weekday_count >= MAX_ACTIVITY_OPEN_WEEKDAY)
					{
						*err = path + ": xml node error in [config->activitylist->activity->weekdaylist->weekday] more than limit";
						return false;
					}

					if (!GetNodeValue(weekdayElement, week_day) || week_day <= 0 || week_day > MAX_ACTIVITY_OPEN_WEEKDAY)
					{
						*err = path + ": xml node error in [config->activitylist->activity->weekdaylist->weekday]";
						return false;
					}

					week_day = week_day % MAX_ACTIVITY_OPEN_WEEKDAY;

					if (week_day < 0 || week_day >= ARRAY_ITEM_COUNT(activity_cfg->open_weekday_list)) // 为了解除
					{
						*err = path + ": xml node error in [config->activitylist->activity->weekdaylist->weekday value error]";
						return false;
					}

					activity_cfg->open_weekday_list[week_day] = 1;
							
					++ weekday_count;
					weekdayElement = weekdayElement->NextSiblingElement();
				}
			}

			{
				TiXmlElement *activitytimeElement = activityElement->FirstChildElement("activity_time");
				if (!activitytimeElement)
				{
					*err = path + ": xml node error in [config->activitylist->activity->activity_time]";
					return false;
				}

				while (activitytimeElement)
				{
					if (activity_time_count >= MAX_ACTIVIYT_TIME_CFG_NUM)
					{
						*err = path + ": xml node error in [config->activitylist->activity->activity_time] more than limit";
						return false;
					}

					{
						TiXmlElement *standytimeElement = activitytimeElement->FirstChildElement("standytime");
						if (!standytimeElement)
						{
							*err = path + ": xml node error in [config->activitylist->activity->standytime not exist]";
							return false;
						}
						StatusTimeCfg *status_timecfg = &activity_cfg->statustime_list[activity_time_count].statustime[ACTIVITY_STATUS_STANDY];
						status_timecfg->status = ACTIVITY_STATUS_STANDY;

						if (!GetSubNodeValue(standytimeElement, "hour", status_timecfg->start_hour) || status_timecfg->start_hour < 0 || status_timecfg->start_hour > 23)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->standytime->hour:%d].", path.c_str(), status_timecfg->start_hour);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(standytimeElement, "min", status_timecfg->start_min) || status_timecfg->start_min < 0 || status_timecfg->start_min > 59)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->standytime->min:%d].", path.c_str(), status_timecfg->start_min);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(standytimeElement, "sec", status_timecfg->start_sec) || status_timecfg->start_sec < 0 || status_timecfg->start_sec > 59)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->standytime->sec:%d].", path.c_str(), status_timecfg->start_sec);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(standytimeElement, "dur_s", status_timecfg->dur_s) || status_timecfg->dur_s < 0)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->standytime->dur_s:%d].", path.c_str(), status_timecfg->dur_s);
							*err = errinfo;
							return false;
						}
					}

					{
						TiXmlElement *opentimeElement = activitytimeElement->FirstChildElement("opentime");
						if (!opentimeElement)
						{
							*err = path + ": xml node error in [config->activitylist->activity->opentime not exist]";
							return false;
						}
						StatusTimeCfg *status_timecfg = &activity_cfg->statustime_list[activity_time_count].statustime[ACTIVITY_STATUS_OPEN];
						status_timecfg->status = ACTIVITY_STATUS_OPEN;

						if (!GetSubNodeValue(opentimeElement, "hour", status_timecfg->start_hour) || status_timecfg->start_hour < 0 || status_timecfg->start_hour > 23)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->opentime->hour:%d].", path.c_str(), status_timecfg->start_hour);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(opentimeElement, "min", status_timecfg->start_min) || status_timecfg->start_min < 0 || status_timecfg->start_min > 59)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->opentime->min:%d].", path.c_str(), status_timecfg->start_min);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(opentimeElement, "sec", status_timecfg->start_sec) || status_timecfg->start_sec < 0 || status_timecfg->start_sec > 59)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->opentime->sec:%d].", path.c_str(), status_timecfg->start_sec);
							*err = errinfo;
							return false;
						}
						if (!GetSubNodeValue(opentimeElement, "dur_s", status_timecfg->dur_s) || status_timecfg->dur_s < 0)
						{
							SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->opentime->dur_s:%d].", path.c_str(), status_timecfg->dur_s);
							*err = errinfo;
							return false;
						}
					}

					++ activity_time_count;
					activitytimeElement = activitytimeElement->NextSiblingElement();
				}	
				activity_cfg->statustime_list_count = activity_time_count;
			}
			if (ACTIVITY_TYPE_WORLD_TEAM_ARENA == activity_cfg->type)
			{
				if (1 != weekday_count)
				{
					SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in [config->activitylist->activity->type:%d][weekdaylist->weekday] count != 1", 
						path.c_str(), type);
					return false;
				}
				if (1 != activity_time_count)
				{
					SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->activitylist->activity->type:%d] [activity_time] count != 1", 
						path.c_str(), type);
					return false;
				}
				int wday_index = ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM % MAX_ACTIVITY_OPEN_WEEKDAY;
				if (1 != activity_cfg->open_weekday_list[wday_index])
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in  [config->activitylist->activity->type:%d] [weekday] != %d",
						path.c_str(), type, ACTIVITY_WORLD_TEAM_ARENA_WEEK_DAY_NUM);
					return false;
				}
				StatusTimeCfg & standy_status_time = activity_cfg->statustime_list[0].statustime[ACTIVITY_STATUS_STANDY];
				if (standy_status_time.Invalid() || ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR != standy_status_time.start_hour ||
					ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN != standy_status_time.start_min || ACTIVITY_WORLD_TEAM_ARENA_STANDY_DUR_S != standy_status_time.dur_s)
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->activitylist->activity->type:%d] [standytime] [dur_s <= 0 or != %d] [hour != %d] [min != %d]",
						path.c_str(), type, ACTIVITY_WORLD_TEAM_ARENA_STANDY_DUR_S, ACTIVITY_WORLD_TEAM_ARENA_STANDY_HOUR, ACTIVITY_WORLD_TEAM_ARENA_STANDY_MIN);
					return false;
				}
				StatusTimeCfg & open_status_time = activity_cfg->statustime_list[0].statustime[ACTIVITY_STATUS_OPEN];
				if (open_status_time.Invalid() || ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR != open_status_time.start_hour ||
					ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN != open_status_time.start_min)
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in [config->activitylist->activity->type:%d] [opentime] [dur_s <= 0] [hour != %d] [min != %d]",
						path.c_str(), type, ACTIVITY_WORLD_TEAM_ARENA_OPEN_HOUR, ACTIVITY_WORLD_TEAM_ARENA_OPEN_MIN);
					return false;
				}
			}

			activityElement = activityElement->NextSiblingElement();
		}

		memcpy(&m_activitycfg_list, temp_activitycfg_list, sizeof(m_activitycfg_list));
	}

	{
		SpecialActivityCfg temp_special_activitycfg_list[SPECIAL_ACTIVITY_TYPE_MAX];

		TiXmlElement *SpecialActivityListElement = RootElement->FirstChildElement("special_activity_list");
		if (!SpecialActivityListElement)
		{
			*err = path + ": xml node error in [config->special_activity_list]";
			return false;
		}

		TiXmlElement *SpecialActivityElement = SpecialActivityListElement->FirstChildElement("special_activity");
// 		if (!SpecialActivityElement)
// 		{
// 			*err = path + ": xml node error in [config->special_activity_list->special_activity]";
// 			return false;
// 		}

		while (SpecialActivityElement)
		{
			int type = 0;
			if (!GetSubNodeValue(SpecialActivityElement, "type", type) || type <= SPECIAL_ACTIVITY_TYPE_INVALID || type >= SPECIAL_ACTIVITY_TYPE_RESVERED)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->type:%d].", path.c_str(), type);
				*err = errinfo;
				return false;
			}

			type = type - SPECIAL_ACTIVITY_TYPE_INVALID;
			if (type <= 0 || type >= SPECIAL_ACTIVITY_TYPE_MAX)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->type:%d].", path.c_str(), type);
				*err = errinfo;
				return false;
			}

			SpecialActivityCfg *special_activity_cfg = &temp_special_activitycfg_list[type];

			if (!special_activity_cfg->Invalid())
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->type:%d repeat].", path.c_str(), type);
				*err = errinfo;
				return false;
			}
			special_activity_cfg->type = type + SPECIAL_ACTIVITY_TYPE_INVALID;

			int is_open = 0;
			if (!GetSubNodeValue(SpecialActivityElement, "is_open", is_open) || is_open < 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->is_open:%d].", path.c_str(), is_open);
				*err = errinfo;
				return false;
			}
			special_activity_cfg->is_open = (0 != is_open);
			
			TiXmlElement *BeginTimeElement = SpecialActivityElement->FirstChildElement("begin_time");
			if (!BeginTimeElement)
			{
				*err = path + ": xml node error in [config->special_activity_list->special_activity->begin_time]";
				return false;
			}
			
			time_t begin_time = 0;
			if (!ConvertTimeStringToUnixTime(BeginTimeElement->FirstChild()->Value(), &begin_time))
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->begin_time].", path.c_str());
				*err = errinfo;
				return false;
			}
			special_activity_cfg->begin_time = static_cast<unsigned int>(begin_time);

			TiXmlElement *EndTimeElement = SpecialActivityElement->FirstChildElement("end_time");
			if (!EndTimeElement)
			{
				*err = path + ": xml node error in [config->special_activity_list->special_activity->end_time]";
				return false;
			}

			time_t end_time = 0;
			if (!ConvertTimeStringToUnixTime(EndTimeElement->FirstChild()->Value(), &end_time))
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity->end_time].", path.c_str());
				*err = errinfo;
				return false;
			}
			special_activity_cfg->end_time = static_cast<unsigned int>(end_time);

			if (special_activity_cfg->end_time <= special_activity_cfg->begin_time)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->special_activity_list->special_activity] end_time less than begin_time.", path.c_str());
				*err = errinfo;
				return false;
			}

			SpecialActivityElement = SpecialActivityElement->NextSiblingElement();
		}

		memcpy(&m_special_activitycfg_list, temp_special_activitycfg_list, sizeof(m_special_activitycfg_list));
	}

	{
		TiXmlElement *RandActivityListElement = RootElement->FirstChildElement("rand_activity_list");
		if (!RandActivityListElement)
		{
			*err = path + ": xml node error in [config->rand_activity_list]";
			return false;
		}

		TiXmlElement *RandActivityElement = RandActivityListElement->FirstChildElement("rand_activity");
// 		if (!RandActivityElement)
// 		{
// 			*err = path + ": xml node error in [config->rand_activity_list->rand_activity]";
// 			return false;
// 		}

		while (RandActivityElement)
		{
			int type = 0;
			if (!GetSubNodeValue(RandActivityElement, "type", type) || type < RAND_ACTIVITY_TYPE_BEGIN || type >= RAND_ACTIVITY_TYPE_END)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->rand_activity_list->special_activity->type:%d].", path.c_str(), type);
				*err = errinfo;
				return false;
			}

			type = type - RAND_ACTIVITY_TYPE_BEGIN;
			if (type < 0 || type >= RAND_ACTIVITY_TYPE_MAX)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->rand_activity_list->special_activity->type:%d].", path.c_str(), type);
				*err = errinfo;
				return false;
			}

			RandActivityCfg &cfg = m_rand_activitycfg_list[type];

			if (!cfg.Invalid())
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->rand_activity_list->special_activity->type:%d repeat].", path.c_str(), type);
				*err = errinfo;
				return false;
			}
			cfg.type = type + RAND_ACTIVITY_TYPE_BEGIN;

			if (!GetSubNodeValue(RandActivityElement, "is_open", cfg.is_open))
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->rand_activity_list->special_activity->is_open].", path.c_str());
				*err = errinfo;
				return false;
			}

			if (!GetSubNodeValue(RandActivityElement, "duration_day", cfg.duration_day) || cfg.duration_day <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo),  "%s: xml node error in  [config->rand_activity_list->special_activity->duration_day:%d].", path.c_str(), cfg.duration_day);
				*err = errinfo;
				return false;
			}

			RandActivityElement = RandActivityElement->NextSiblingElement();
		}
	}

	return true;
}

int DynamicConfig::GetActivtyNextStatus(int activity_type, int curr_status, int next_open_wday, StatusTimeCfgList *statuestime_list_cfg)
{
	if (NULL == statuestime_list_cfg) return ACTIVITY_STATUS_CLOSE;

	switch (curr_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			StatusTimeCfg *status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_STANDY];
			if (status_timecfg->Invalid())
			{
				status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_OPEN];
			}

			if (!status_timecfg->Invalid())
			{
				return status_timecfg->status;
			}
		}
		break;

	case ACTIVITY_STATUS_STANDY:
		{
			return ACTIVITY_STATUS_OPEN;
		}
		break;

	case ACTIVITY_STATUS_OPEN:
		{
			return ACTIVITY_STATUS_CLOSE;
		}
		break;
	}

	return ACTIVITY_STATUS_CLOSE;
}

unsigned int DynamicConfig::GetActivityNextSwitchTime(int activity_type, int curr_status, int next_open_wday, StatusTimeCfgList *statuestime_list_cfg, bool is_find)
{
	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (NULL == statuestime_list_cfg) return now + 7 * 24 * 60 * 60;

	switch (curr_status)
	{
	case ACTIVITY_STATUS_CLOSE:
		{
			StatusTimeCfg *status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_STANDY];
			if (status_timecfg->Invalid())
			{
				status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_OPEN];
			}

			const tm *local_time = EngineAdapter::Instance().LocalTime();
			if (!is_find && ACTIVITY_TYPE_INVALID < activity_type && activity_type < ACTIVITY_TYPE_MAX)
			{
				for (int i = 0; i < 7; i++)
				{
					int wday = (local_time->tm_wday + i) % 7; 
					if (m_activitycfg_list[activity_type].open_weekday_list[wday] > 0)
					{
						next_open_wday = wday;
						break;
					}
				}
			}

			int interval = EngineAdapter::Instance().NextWeekInterval(next_open_wday, status_timecfg->start_hour, status_timecfg->start_min, status_timecfg->start_sec);

			if (!is_find && (next_open_wday != local_time->tm_wday || interval < 24 * 60 * 60))		// 如果当天已过开启时间，找到的已经是下一周时间，不需要加7天
			{
				interval += 7 * 24 * 60 * 60;
			}

			return now + interval;
		}
		break;

	case ACTIVITY_STATUS_STANDY:
		{
			StatusTimeCfg *status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_STANDY];

			return now + status_timecfg->dur_s;
		}
		break;

	case ACTIVITY_STATUS_OPEN:
		{
			StatusTimeCfg *status_timecfg = &statuestime_list_cfg->statustime[ACTIVITY_STATUS_OPEN];

			return now + status_timecfg->dur_s;
		}
		break;
	}

	return now + 7 * 24 * 60 * 60;
}

bool DynamicConfig::GetActivityNextCfg(int activity_type, int *next_open_wday, StatusTimeCfgList *statuestime_list_cfg, bool *is_find)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX || NULL == next_open_wday || NULL == statuestime_list_cfg) return false;

	if (m_activitycfg_list[activity_type].statustime_list_count <= 0) return false;

	const tm *local_time = EngineAdapter::Instance().LocalTime();

	int next_wday = local_time->tm_wday;

	for (int i = 0; i < 7; i++)
	{
		int wday = (local_time->tm_wday + i) % 7; 
		if (this->IsActivityOpenDay(activity_type, wday))
		{
			next_wday = wday;
			if (NULL != is_find) *is_find = true;
			break;
		}
	}
	
	int min_interval = 60 * 60 * 24 * 7;

	for (int i = 0; i < m_activitycfg_list[activity_type].statustime_list_count; i++)
	{
		ActivityCfg *temp_cfg = &m_activitycfg_list[activity_type];
		if (!temp_cfg->is_open) continue;

		StatusTimeCfg *status_timecfg = &temp_cfg->statustime_list[i].statustime[ACTIVITY_STATUS_STANDY];
		if (status_timecfg->Invalid())
		{
			status_timecfg = &temp_cfg->statustime_list[i].statustime[ACTIVITY_STATUS_OPEN];
		}

		int temp_interval = EngineAdapter::Instance().NextWeekInterval(next_wday, status_timecfg->start_hour, status_timecfg->start_min, status_timecfg->start_sec);
		if (temp_interval < min_interval)
		{
			min_interval = temp_interval;
			memcpy(statuestime_list_cfg, &temp_cfg->statustime_list[i], sizeof(StatusTimeCfgList));
		}
	}

	if (next_wday == local_time->tm_wday && min_interval > 60 * 60 * 24)
	{
		for (int i = 1; i < 7; i++)
		{
			int wday = (local_time->tm_wday + i) % 7; 
			if (this->IsActivityOpenDay(activity_type, wday))
			{
				next_wday = wday;
				break;
			}
		}
	}

	*next_open_wday = next_wday;

	return true;
}

bool DynamicConfig::IsActivityOpenDay(int activity_type, int weekday)
{
	if (activity_type <= ACTIVITY_TYPE_INVALID || activity_type >= ACTIVITY_TYPE_MAX || weekday < 0 || weekday >= MAX_ACTIVITY_OPEN_WEEKDAY) return false;

	if (m_activitycfg_list[activity_type].open_weekday_list[weekday] > 0) return true;

	return false;
}

bool DynamicConfig::GetSpecialActivityTime(int special_activity_type, unsigned int *begin_time, unsigned int *end_time)
{
	special_activity_type = special_activity_type - SPECIAL_ACTIVITY_TYPE_INVALID;
	if (special_activity_type <= 0 || special_activity_type >= SPECIAL_ACTIVITY_TYPE_MAX) return false;

	if (NULL != begin_time) *begin_time = m_special_activitycfg_list[special_activity_type].begin_time;
	if (NULL != end_time) *end_time = m_special_activitycfg_list[special_activity_type].end_time;

	return true;
}

const RandActivityCfg * DynamicConfig::GetRandActivityCfg(int rand_activity_type)
{
	rand_activity_type -= RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX) return NULL;

	return &m_rand_activitycfg_list[rand_activity_type];
}

