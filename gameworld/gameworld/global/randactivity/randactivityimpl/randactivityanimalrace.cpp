#include "randactivityanimalrace.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"

#include "servercommon/titledef.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/string/gameworldstr.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityanimalraceconfig.hpp"

#include "other/title/title.hpp"
#include "other/escort/escort.hpp"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/animalrace/animalracemanager.hpp"

#include "global/team/team.hpp"
#include "global/guild/guild.hpp"
#include "protocol/randactivity/msgraanimalrace.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "gameworld/obj/character/role.h"

RandActivityAnimalRace::RandActivityAnimalRace(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_ANIMAL_RACE), is_time(false)
{
	for(int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
		activity_scene_apppearance->Reset();
}

RandActivityAnimalRace::~RandActivityAnimalRace()
{
}

void RandActivityAnimalRace::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (ACTIVITY_STATUS_OPEN == to_status)
	{
		AnimalRaceManager::Instance().ClearAllAnimalRaceRecord();
		this->ResetApp();
		this->OnRefreshGameInfo(EngineAdapter::Instance().Time());
	}
	else if (ACTIVITY_STATUS_CLOSE == to_status)
	{
		const RandActivityAnimalRaceConfig * config = static_cast<const RandActivityAnimalRaceConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
		if (NULL == config)
		{
			return;
		}
		const RAOtherCfg & other_cfg = config->GetOtherCfg();
		SpecialLogicAnimalRace * animal_scene = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicAnimalRace(other_cfg.scene_id, COMMON_FB_KEY);
		if (NULL == animal_scene) return;

		this->ResetApp();
		animal_scene->OnActivityClose();
	}
}

void RandActivityAnimalRace::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);

	if (m_param.bet_end_timestamp != 0 && m_param.bet_end_timestamp <= now_second)
	{
		m_param.is_bet = false;
		m_param.bet_end_timestamp = 0;
	}

	if (m_param.next_refresh_game_info_stamp != 0 && m_param.next_refresh_game_info_stamp <= now_second)
	{
		m_param.next_refresh_game_info_stamp = 0;
		this->OnRefreshGameInfo(now_second);
	}
	else if (this->GetStatus() == ACTIVITY_STATUS_OPEN && m_param.next_refresh_game_info_stamp == 0)
	{
		this->OnRefreshGameInfo(now_second);
	}
}

void RandActivityAnimalRace::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy(&m_param, ra_data->data, sizeof(m_param));

}

void RandActivityAnimalRace::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;

	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));

	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivityAnimalRace::OnUserLogout(Role * user)
{
	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		this->SceneRoleNumLess(user->GetUID());
	}
}

void RandActivityAnimalRace::AddBetCount(int add_num, int animal_race_number, int uid)
{
	if (!m_param.is_bet) return;

	m_param.total_bet_count += add_num;
	m_param.m_type_count[animal_race_number - 1] += add_num;
	for (int i = 0; i < MAX_ANIMAL_RACE_NUM; i++)
	{
		if (m_param.m_type_count[i] > 0)
		{
			m_param.m_type_response_rate[i] = static_cast<int>(m_param.total_bet_count * 1.0 / m_param.m_type_count[i] * 100);
		}
		else
		{
			m_param.m_type_response_rate[i] = static_cast<int>(m_param.total_bet_count * 100);
		}
	}

	AnimalRaceEntry param;
	this->GetSaveDBData(param);
	AnimalRaceManager::Instance().OnRoleAddBetUpdateRecord(param);


	/*std::map<int, std::vector<role_bet_info> >::iterator it = m_info_list.find(animal_race_number);
	if (it == m_info_list.end())
	{
		role_bet_info info;
		info.role_uid = uid;
		info.bet_count = add_num;
		m_info_list[animal_race_number].push_back(info);
	}
	else
	{
		bool is_found = false;
		for (std::vector<role_bet_info>::iterator info_it = it->second.begin(); info_it != it->second.end(); info_it++)
		{
			if (info_it->role_uid == uid)
			{
				is_found = true;
				info_it->bet_count += add_num;
				break;
			}
		}
		if (!is_found)
		{
			role_bet_info info;
			info.role_uid = uid;
			info.bet_count = add_num;
			m_info_list[animal_race_number].push_back(info);
		}
	}*/
	
	return;
}

void RandActivityAnimalRace::GetTypeCountAndTypeResponse(int * type_count, int * type_response_rate)
{	
	for (int i = 0; i < MAX_ANIMAL_RACE_NUM; i++)
	{
		type_count[i] = m_param.m_type_count[i];
		type_response_rate[i] = m_param.m_type_response_rate[i];
	}
}

long long RandActivityAnimalRace::GetTimestamp(int * op_type, char *rank, int len)
{
	long long timestamp = 0;
	if (!is_time)
	{
		timestamp = m_param.next_refresh_game_info_stamp;
		*op_type = Protocol::SCRAAnimalRaceInfo::ANIMAL_RACE_INFO_TYPE_NOT_TIME;
		memcpy(rank, m_param.animation_data.rank, sizeof(char) * len);
	}
	else if (m_param.bet_end_timestamp == 0)
	{
		timestamp = m_param.next_refresh_game_info_stamp;
		*op_type = Protocol::SCRAAnimalRaceInfo::ANIMAL_RACE_INFO_TYPE_ROLE_DOING;
		memcpy(rank, m_param.animation_data.rank, sizeof(char) * len);
	}
	else
	{
		timestamp = m_param.bet_end_timestamp;
		*op_type = Protocol::SCRAAnimalRaceInfo::ANIMAL_RACE_INFO_TYPE_ROLE_READY;
		memcpy(rank, m_param.animation_data.rank, sizeof(char) * len);
	}

	return timestamp;
}

void RandActivityAnimalRace::GetSceneRoleApp(role_info * appearance, int * cur_count, Role * role)
{
	if(NULL == role) return;

	bool is_found = false;
	int role_uid = role->GetUID();
	for (int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
	{
		if(activity_scene_apppearance[i].Invalid()) 
		{
			continue;
		}
		if (activity_scene_apppearance[i].role_id == role_uid)
		{
			is_found = true;
		}
		appearance[(*cur_count)++] = activity_scene_apppearance[i];
	}
	if (!is_found && *cur_count < MAX_ANIMAL_RACE_SC_NUM)
	{
		this->SetRoleInfo(role, &appearance[*cur_count]);
		(*cur_count)++;
	}
}

void RandActivityAnimalRace::GetNeedInfo(int * server_id, int * date_id, time_t * next_refresh_timestamp)
{
	*server_id = m_param.server_id;
	*date_id = m_param.date_id;
	*next_refresh_timestamp = m_param.next_refresh_game_info_stamp;
}

void RandActivityAnimalRace::DistributeResults()
{
	/*if(m_param.win_animal_race_number <= 0 || m_param.win_animal_race_number > MAX_ANIMAL_RACE_NUM || m_param.total_bet_count <= 0)	return;

	std::map<int, std::vector<role_bet_info> >::iterator it = m_info_list.find(m_param.win_animal_race_number);
	if(it == m_info_list.end()) return;

	const RAAnimalRaceCfg * cfg = LOGIC_CONFIG->GetAnimalRaceConfig()->GetAnimalRaceCfg();
	if (cfg == NULL)
	{
		return;
	}

	for (std::vector<role_bet_info>::iterator info_it = it->second.begin(); info_it != it->second.end(); info_it++)
	{
		static MailContentParam contentparam;
		ItemDataWrapper item_info;
		item_info.item_id = cfg->item_consume_id;
		item_info.is_bind = true;
		item_info.num = info_it->bet_count * m_param.m_type_response_rate[m_param.win_animal_race_number - 1];

		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_animal_race_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_animal_race_content, m_param.win_animal_race_number, m_param.m_type_response_rate[m_param.win_animal_race_number - 1] * 100);
		MailRoute::MailToUser(info_it->role_uid, MAIL_REASON_DEFAULT, contentparam);
	}
	*/
}

void RandActivityAnimalRace::SceneRoleNumAdd(Role * role)
{	
	if(NULL == role) return;

	for (int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
	{
		if (!activity_scene_apppearance[i].Invalid())
		{
			continue;
		}

		this->SetRoleInfo(role, &activity_scene_apppearance[i]);
		break;
	}
}

void RandActivityAnimalRace::SceneRoleNumLess(int uid)
{
	if (this->GetStatus() == ACTIVITY_STATUS_OPEN)
	{
		for (int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
		{
			if (activity_scene_apppearance[i].Invalid())
			{
				continue;
			}
			if (activity_scene_apppearance[i].role_id == uid)
			{
				activity_scene_apppearance[i].Reset();
				break;
			}
		}
	}
}

int RandActivityAnimalRace::GetInvaildIndex()
{
	for (int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
	{
		if (activity_scene_apppearance[i].Invalid())
		{
			return i;
		}
	}

	return MAX_ANIMAL_RACE_SCENE_NUM;
}

void RandActivityAnimalRace::GetAnimationData(AnimalRaceData * data)
{
	*data = m_param.animation_data;
}

void RandActivityAnimalRace::GmForceStatus()
{
	if (!is_time)
	{
		m_param.next_refresh_game_info_stamp = 1;
	}
	else if (m_param.bet_end_timestamp == 0)
	{
		m_param.next_refresh_game_info_stamp = 1;
	}
	else
	{
		m_param.bet_end_timestamp = 1;
	}

}

void RandActivityAnimalRace::OnRefreshGameInfo(time_t now_second)
{
	if(this->GetStatus() != ACTIVITY_STATUS_OPEN) return;

	m_param.RefreshReset();
	is_time = false;

	m_param.server_id = LocalConfig::Instance().GetMergeServerId();
	m_param.is_bet = true;
	AnimalRaceEntry * info = AnimalRaceManager::Instance().GetCurIsNew(now_second);
	if (NULL != info)
	{
		m_param.date_id = info->date_id;
		m_param.win_animal_race_number = info->win_numner;
		m_param.total_bet_count = info->pirze_pool;
		memcpy(m_param.m_type_count, info->animal_race_num, sizeof(m_param.m_type_count));
		memcpy(m_param.m_type_response_rate, info->response_rate_num, sizeof(m_param.m_type_response_rate));
		m_param.animation_data = info->animation_data;
		if (!this->GetRefreshTimestamp(now_second))
		{
			return;
		}
		if (m_param.server_id != info->server_id)
		{
			AnimalRaceEntry update_info;
			this->GetSaveDBData(update_info);
			AnimalRaceManager::Instance().OnRoleAddBetUpdateRecord(update_info);
		}
		return;
	}
	if (!this->GetRefreshTimestamp(now_second))
	{
		return;
	}
	

	int date_id = AnimalRaceManager::Instance().GetNewDateId(m_param.server_id);
	if (date_id <= 0)
	{
		m_param.date_id = 1;
	}
	else
	{
		m_param.date_id = date_id + 1;
	}

	this->OnRefreshAnimationData();

	AnimalRaceEntry param;
	this->GetSaveDBData(param);
	AnimalRaceManager::Instance().OnAddAnimalRaceRecord(param);
}

bool RandActivityAnimalRace::GetRefreshTimestamp(time_t now_second)
{
	const RandActivityAnimalRaceConfig * config = static_cast<const RandActivityAnimalRaceConfig * >
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
	if (NULL == config) return false;

	const RATimeCfg & time_cfg = config->GetTimeCfg();

	int morning_start = EngineAdapter::Instance().NextDayInterval(time_cfg.start_time1, 0, 0);
	int morning_end = EngineAdapter::Instance().NextDayInterval(time_cfg.end_time1, 0, 0);
	int evening_start = EngineAdapter::Instance().NextDayInterval(time_cfg.start_time2, 0, 0);
	int evening_end = EngineAdapter::Instance().NextDayInterval(time_cfg.end_time2, 0, 0);
	int min_morning = GetMin(morning_start, morning_end);
	int min_evening = GetMin(evening_start, evening_end);
	if (min_morning < min_evening)
	{
		if (morning_start < morning_end)
		{
			m_param.next_refresh_game_info_stamp = now_second + EngineAdapter::Instance().NextDayInterval(12, 0, 0);
			m_param.is_bet = false;
			is_time = false;
			return false;
		}
		else
		{
			this->SetRefreshTimestamp(now_second);
		}
	}
	else
	{
		if (evening_start < evening_end)
		{
			m_param.next_refresh_game_info_stamp = now_second + EngineAdapter::Instance().NextDayInterval(19, 0, 0);
			m_param.is_bet = false;
			is_time = false;
			return false;
		}
		else
		{
			this->SetRefreshTimestamp(now_second);
		}
	}
	return true;
}

void RandActivityAnimalRace::SetRefreshTimestamp(time_t now_second)
{
	int time_1 = EngineAdapter::Instance().NextHourInterval(0, 0);
	int time_2 = EngineAdapter::Instance().NextHourInterval(15, 0);
	int time_3 = EngineAdapter::Instance().NextHourInterval(30, 0);
	int time_4 = EngineAdapter::Instance().NextHourInterval(45, 0);
	int min_time = GetMin(time_1, time_2);
	min_time = GetMin(min_time, time_3);
	min_time = GetMin(min_time, time_4);
	m_param.next_refresh_game_info_stamp = now_second + min_time;
	m_param.bet_end_timestamp = m_param.next_refresh_game_info_stamp - SECOND_PER_MIN * 3;
	is_time = true;
}

void RandActivityAnimalRace::SetRoleInfo(Role * role, role_info *user_info)
{
	user_info->role_id = role->GetUID();
	F_STRNCPY(user_info->role_name, role->GetName(), sizeof(GameName));

	user_info->obj_id = role->GetId();
	user_info->level = role->GetLevel();

	user_info->behavior_state = role->GetRoleStatusManager()->GetRoleStatus();
	user_info->avatar_type = role->GetAvatarType();
	user_info->appearance = role->GetRoleModuleManager()->GetAppearance();
	user_info->vip_level = role->GetVipLevel();
	user_info->profession = role->GetProfession();

	user_info->move_speed = role->GetSpeedRate();
	user_info->pos_x = 0;		//坐标,转向客户端那边算
	user_info->pos_y = 0;
	user_info->distance = role->GetDirDistance();
	user_info->dir = 0;
	user_info->scene_id = role->GetScene()->GetSceneID();
	user_info->escort_id = role->GetRoleModuleManager()->GetEscort()->GetEscortSeq();
	const TitleData *getweartitleid = role->GetRoleModuleManager()->GetTitle()->GetWearTitleInfo();
	if (getweartitleid == NULL)
	{
		user_info->title_id = 0;
	}
	else
	{
		user_info->title_id = getweartitleid->title_id;
	}

	memset(user_info->guildname, 0, sizeof(GuildName));
	Guild *guild = GuildManager::Instance().GetGuild(role->GetGuildID());
	if (NULL != guild)
	{
		memcpy(user_info->guildname, guild->GetName(), sizeof(GuildName));
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			user_info->is_leader = 1;
		}
		else
		{
			user_info->is_leader = 0;
		}
		user_info->is_in_team = 1;
	}
	else
	{
		user_info->is_leader = 0;
		user_info->is_in_team = 0;
	}
}

void RandActivityAnimalRace::ResetApp()
{	
	is_time = false;
	m_param.Reset();
	for (int i = 0; i < MAX_ANIMAL_RACE_SCENE_NUM; i++)
		activity_scene_apppearance->Reset();

}

bool SortArriveInfo(const arrive_info & a,const arrive_info & b)
{
	return a.times < b.times;
}

void RandActivityAnimalRace::OnRefreshAnimationData()
{
	int moving_distance_count[MAX_ANIMAL_RACE_NUM] = {0};	//计算距离总值
	int speed_interval = 10;	//为了确保每次都有神兽可以到达终点,在此区间为上限
	int rank = 0;
	bool first_flag[MAX_ANIMAL_RACE_NUM] = {false};		//通过这个判断此时玩家是否是第一次抵达 

	std::vector<arrive_info> arrive_info_list;

	int animal_race_1_speed = RandomNum(ANIMAL_RACE_AVERAGE_SPEED_NUM, ANIMAL_RACE_AVERAGE_SPEED_NUM + speed_interval);
	int animal_race_2_speed = RandomNum(ANIMAL_RACE_AVERAGE_SPEED_NUM, ANIMAL_RACE_AVERAGE_SPEED_NUM + speed_interval);
	int animal_race_3_speed = RandomNum(ANIMAL_RACE_AVERAGE_SPEED_NUM, ANIMAL_RACE_AVERAGE_SPEED_NUM + speed_interval);
	int animal_race_4_speed = RandomNum(ANIMAL_RACE_AVERAGE_SPEED_NUM, ANIMAL_RACE_AVERAGE_SPEED_NUM + speed_interval);
	//1.获取每一秒神兽移动的距离
	for (int i = 0; i < MAX_ANIMAL_RACE_SECOND_NUM; i++)
	{
		m_param.animation_data.moving_distance_second[0][i] = animal_race_1_speed;
		m_param.animation_data.moving_distance_second[1][i] = animal_race_2_speed;
		m_param.animation_data.moving_distance_second[2][i] = animal_race_3_speed;
		m_param.animation_data.moving_distance_second[3][i] = animal_race_4_speed;
	}
	//2.通过循环计算此时神兽在第几秒就抵达了终点 如果神兽在同一秒中都抵达了,那么则需要计算在上一秒时,神兽距离终点的距离与下一秒的速度,通过速度与距离得出抵达终点所需时间,由此来判断哪个神兽最先抵达
	for (int i = 0; i < MAX_ANIMAL_RACE_SECOND_NUM; i++)
	{
		moving_distance_count[0] += m_param.animation_data.moving_distance_second[0][i];
		moving_distance_count[1] += m_param.animation_data.moving_distance_second[1][i];
		moving_distance_count[2] += m_param.animation_data.moving_distance_second[2][i];
		moving_distance_count[3] += m_param.animation_data.moving_distance_second[3][i];
		for (int j = 0; j < MAX_ANIMAL_RACE_NUM; j++)
		{
			if (moving_distance_count[j] >= MAX_ANIMAL_RACE_DISTANCE_NUM && !first_flag[j])
			{
				arrive_info info;
				info.times = i + (MAX_ANIMAL_RACE_DISTANCE_NUM - (moving_distance_count[j] - m_param.animation_data.moving_distance_second[j][i])) * 1.0 / m_param.animation_data.moving_distance_second[j][i];
				info.number = j;
				first_flag[j] = true;
				arrive_info_list.push_back(info);
			}
		}

	}
	std::sort(arrive_info_list.begin(),arrive_info_list.end(), SortArriveInfo);
	if ((int)arrive_info_list.size() >= 2)
	{
		if (arrive_info_list[0].times == arrive_info_list[1].times)
		{
			m_param.animation_data.moving_distance_second[arrive_info_list[0].number][0] += speed_interval;
		}
	}
	for (int i = 0; i < (int)arrive_info_list.size() && i < MAX_ANIMAL_RACE_NUM; i++)
	{
		m_param.animation_data.rank[arrive_info_list[i].number] = ++rank;
	}
	m_param.win_animal_race_number = arrive_info_list[0].number + 1;
}

void RandActivityAnimalRace::GetSaveDBData(AnimalRaceEntry & param)
{
	param.server_id = m_param.server_id;
	param.date_id = m_param.date_id;
	param.win_numner = m_param.win_animal_race_number;
	param.pirze_pool = m_param.total_bet_count;
	memcpy(param.animal_race_num, m_param.m_type_count, sizeof(param.animal_race_num));
	memcpy(param.response_rate_num, m_param.m_type_response_rate, sizeof(param.response_rate_num));
	param.time_stamp = m_param.next_refresh_game_info_stamp;
	param.animation_data = m_param.animation_data;
}
