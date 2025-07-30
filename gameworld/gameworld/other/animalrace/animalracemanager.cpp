#include "animalracemanager.hpp"
#include "gamelog.h"
#include "internalcomm.h"
#include "global/rmibackobjdef.h"
#include "gameworld/engineadapter.h"
#include "servercommon/activitydef.hpp"
#include <vector>

AnimalRaceManager & AnimalRaceManager::Instance()
{
	static AnimalRaceManager animal_race_manager;
	return animal_race_manager;
}

void AnimalRaceManager::OnServerStart()
{
	this->LoadAnimalRaceData(0);
}

void AnimalRaceManager::OnServerStop()
{
	this->Save();
}

bool AnimalRaceManager::LoadAnimalRaceData(long long id_from)
{
	RMIInitAnimalRaceBackObjectImpl *impl = new RMIInitAnimalRaceBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitAnimalRaceAsyn(id_from, impl);
}

void AnimalRaceManager::Update(time_t now_second)
{
	if(!this->ISLoadFinish()) return;

	if (m_animal_race_next_save_time > 0 && now_second > m_animal_race_next_save_time)
	{
		this->Save();
		m_animal_race_next_save_time = EngineAdapter::Instance().Time() + ANIMAL_RACE_SAVE_TO_DB_TIME;
	}
}

void AnimalRaceManager::Init(const AnimalRaceParam & param)
{
	for (int i = 0; i < param.count && i < (int)param.data_list.size(); i++)
	{
		if (param.data_list[i].animal_race_info.date_id <= 0)
		{
			gamelog::g_log_animal_race.printf(LL_CRITICAL, "[AnimalRaceManager::Init INVALID ] [date_id:%d]", param.data_list[i].animal_race_info.date_id);
			continue;
		}
		if (param.data_list[i].animal_race_info.win_numner <= 0 || param.data_list[i].animal_race_info.win_numner > MAX_ANIMAL_RACE_NUM)
		{
			gamelog::g_log_animal_race.printf(LL_CRITICAL, "[AnimalRaceManager::Init INVALID ] [win_number:%d]", param.data_list[i].animal_race_info.win_numner);
			continue;
		}
		if (param.data_list[i].animal_race_info.pirze_pool < 0)
		{
			gamelog::g_log_animal_race.printf(LL_CRITICAL, "[AnimalRaceManager::Init INVALID ] [pirze_pool:%d]", param.data_list[i].animal_race_info.pirze_pool);
			continue;
		}
		if (param.data_list[i].animal_race_info.time_stamp <= 0)
		{
			gamelog::g_log_animal_race.printf(LL_CRITICAL, "[AnimalRaceManager::Init INVALID ] [time_stamp:%lld]", param.data_list[i].animal_race_info.time_stamp);
			continue;
		}

		m_param.data_list.push_back(param.data_list[i]);
		m_param.count++;
		m_old_state[i] = true;
		m_dirty_mark[i] = false;
	}
	m_load_flag = true;

	//初始化后保存一份数据,防止之后删除的时候没有数据可用
	this->Save(&m_param);
}

void AnimalRaceManager::GetInitParam(AnimalRaceParam * param)
{
	if(NULL == param) return;
	
	*param = m_param;

	if(!m_change_flag) return;

	std::vector<int> delete_list;

	for (int i = 0; i < m_param.count && i < (int)m_param.data_list.size(); i++)
	{
		if (m_dirty_mark[i])
		{
			if (m_param.data_list[i].animal_race_info.InValid())
			{
				if(m_old_state[i]) continue;

				param->data_list[i].change_state = structcommon::CS_DELETE;
				param->data_list[i].id = i + 1;

				delete_list.push_back(i);
			}
			else
			{
				param->data_list[i].change_state = m_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->data_list[i].id = i + 1;
			}
		}
	}
	
	if (delete_list.size() >= 1)
	{	
		for (int i = 0; i < (int)delete_list.size() && delete_list[i] - i < m_param.count && delete_list[i] - i < (int)m_param.data_list.size(); i++)
		{
			m_param.data_list.erase(m_param.data_list.begin() + (delete_list[i] - i));   //注意括号不可删除,否则会造vector迭代器越界
			m_param.count--;
		}
	}
}

void AnimalRaceManager::ClearDirtyMark(AnimalRaceParam * param)
{
	if(!m_change_flag) return;

	for (int i = 0; i < m_param.count && i < (int)m_param.data_list.size(); i++)
	{
		m_old_state[i] = true;
		m_dirty_mark[i] = false;
		m_param.data_list[i].change_state = structcommon::CS_NONE;
		param->data_list[i].change_state = structcommon::CS_NONE;
	}
	m_change_flag = false;
}

bool AnimalRaceManager::GetWinNumberAndResponse(int server_id, int date_id, int * win_number, int * response)
{
	if(m_param.count <= 0 || m_param.data_list.size() <= 0) return false;

	for (int i = 0; i < m_param.count && i < (int)m_param.data_list.size(); i++)
	{
		const AnimalRaceParam::AnimalRaceSaveEntry & data_item = m_param.data_list[i];
		if (data_item.animal_race_info.server_id == server_id && data_item.animal_race_info.date_id == date_id)
		{
			*win_number = data_item.animal_race_info.win_numner;
			*response = data_item.animal_race_info.response_rate_num[data_item.animal_race_info.win_numner - 1];
			return true;
		}
	}
	return false;
}

void AnimalRaceManager::OnAddAnimalRaceRecord(AnimalRaceEntry & param)
{
	if(param.InValid()) return;

	if(param.win_numner <= 0 || param.win_numner > MAX_ANIMAL_RACE_NUM) return;

	AnimalRaceParam::AnimalRaceSaveEntry data_item;
	data_item.id = m_param.data_list.size() + 1;
	data_item.animal_race_info = param;
	m_param.data_list.push_back(data_item);
	m_param.count++;

	m_dirty_mark[m_param.data_list.size() - 1] = true;
	m_old_state[m_param.data_list.size() - 1] = false;
	m_change_flag = true;
	this->Save();

	gamelog::g_log_animal_race.printf(LL_INFO, "[AnimalRaceManager::OnAddAnimalRaceRecord] [server_id:%d] [date_id:%d] [win:%d] [timestamp:%lld] ", 
		param.server_id, param.date_id, param.win_numner, param.time_stamp);
}

void AnimalRaceManager::OnRoleAddBetUpdateRecord(AnimalRaceEntry & param)
{
	if (param.InValid()) return;

	if (param.win_numner <= 0 || param.win_numner > MAX_ANIMAL_RACE_NUM) return;

	m_param.data_list[m_param.data_list.size() - 1].animal_race_info = param;

	m_dirty_mark[m_param.data_list.size() - 1] = true;
	m_old_state[m_param.data_list.size() - 1] = true;
	m_change_flag = true;

}

int AnimalRaceManager::GetNewDateId(int server_id)
{
	int date_id = 0;
	for (int i = 0; i < m_param.count && i < (int)m_param.data_list.size(); i++)
	{
		if (server_id == m_param.data_list[i].animal_race_info.server_id)
		{
			if (date_id < m_param.data_list[i].animal_race_info.date_id)
			{
				date_id = m_param.data_list[i].animal_race_info.date_id;
			}
		}
	}
	return date_id;
}

AnimalRaceEntry * AnimalRaceManager::GetCurIsNew(time_t now_second)
{
	if ((int)m_param.data_list.size() > 0 && m_param.count > 0)
	{
		if (m_param.data_list[m_param.data_list.size() - 1].animal_race_info.time_stamp > now_second)
		{
			return &m_param.data_list[m_param.data_list.size() - 1].animal_race_info;
		}
	}
	return NULL;
}

void AnimalRaceManager::ClearAnimalRaceItem(int index)
{
	if(index < 0 || index >= m_param.count || index >= (int)m_param.data_list.size()) return;
	
	m_dirty_mark[index] = true;
	m_old_state[index] = false;
	m_param.data_list[index].animal_race_info.time_stamp = 0;
	m_change_flag = true;
	this->Save();
}

void AnimalRaceManager::ClearAllAnimalRaceRecord()
{
	if(m_param.count <= 0 || (int)m_param.data_list.size() <= 0) return;

	for (int i = 0; i < m_param.count && i < (int)m_param.data_list.size(); i++)
	{
		m_dirty_mark[i] = true;
		m_old_state[i] = false;
		m_param.data_list[i].animal_race_info.time_stamp = 0;
	}
	m_change_flag = true;
	this->Save();
}

AnimalRaceManager::AnimalRaceManager():m_animal_race_next_save_time(0), m_change_flag(false), m_load_flag(false)
{
	m_dirty_mark.clear();
	m_old_state.clear();
}

AnimalRaceManager::~AnimalRaceManager()
{
}

void AnimalRaceManager::Save(const AnimalRaceParam * param)
{
	if(!this->ISLoadFinish()) return;

	static AnimalRaceParam animal_race_param;
	if (NULL != param)
	{
		animal_race_param = *param;
	}
	this->GetInitParam(&animal_race_param);

	RMISaveAnimalRaceBackObjectImpl *backobj = new RMISaveAnimalRaceBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveAnimalRaceAsyn(animal_race_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&animal_race_param);
	}

}

void AnimalRaceManager::LoadAnimalRaceSucc()
{
	if (this->ISLoadFinish())
	{
		m_animal_race_next_save_time = EngineAdapter::Instance().Time() + ANIMAL_RACE_SAVE_TO_DB_TIME;
	}
}

void RMIInitAnimalRaceBackObjectImpl::InitAnimalRaceRet(int ret, const AnimalRaceParam &animal_race_param)
{
	if (ret < 0)
	{
		printf("RMIInitAnimalRaceBackObjectImpl::InitAnimalRaceRet Failed \n");
	}
	else
	{
		AnimalRaceManager & animal_race_manager = AnimalRaceManager::Instance();
		if(NULL == &animal_race_manager) return;

		animal_race_manager.Init(animal_race_param);
		animal_race_manager.LoadAnimalRaceSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}
