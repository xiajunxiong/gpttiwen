#include "babymanager.hpp"
#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "servercommon/struct/structcommon.h"

void RMIInitBabyDataBackObjectImpl::InitBabyDataRet(int ret, const AllBabyDataParam & all_baby_data_param)
{
	if (ret < 0)
	{
		printf("RMIInitBabyDataBackObjectImpl::InitBabyDataRet Failed ret=%d", ret);
		EngineAdapter::Instance().StopGame();
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < all_baby_data_param.count; i++)
	{
		const BabyDataParam & baby_item_data = all_baby_data_param.baby_list[i];
		if (all_baby_data_param.addition_info[i].baby_inc_id > next_id_from)
		{
			next_id_from = all_baby_data_param.addition_info[i].baby_inc_id;
		}

		BabyManager::Instance().AddBaby(baby_item_data);
	}

	if (all_baby_data_param.count > 0)
	{
		BabyManager::Instance().LoadBaby(next_id_from);
	}
	else
	{
		BabyManager::Instance().LoadBabySucc();
	}
}

BabyManager & BabyManager::Instance()
{
	static BabyManager instance;
	return instance;
}

void BabyManager::OnServerStart()
{
	this->LoadBaby(0);
}

void BabyManager::OnServerStop()
{
	this->Save();
}

bool BabyManager::LoadBaby(long long inc_id_from)
{
	if (m_load_flag)
	{
		return false;
	}

	RMIInitBabyDataBackObjectImpl *impl = new RMIInitBabyDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitBabyAsyn(inc_id_from, impl);
}

void BabyManager::LoadBabySucc()
{
	m_load_flag = true;
}

bool BabyManager::IsLoadFinish()
{	
	return m_load_flag;	
}

void BabyManager::Update(unsigned long interval, time_t now_second)
{
	if (!IsLoadFinish())
	{
		return;
	}

	if (now_second >= m_next_save_time)
	{
		m_next_save_time = static_cast<unsigned int>(now_second + BABY_SAVE_INTERVAL);		
		this->Save();
	}
}

void BabyManager::AddBaby(const BabyDataParam & baby_data)
{
	BabyDataNode baby_node;
	baby_node.is_new_create = false;
	baby_node.is_dirty = false;
	baby_node.baby_data_param = baby_data;
	m_all_baby_map[baby_data.baby_id] = baby_node;
}

long long BabyManager::AddNewBaby(int db_index, const GameName baby_name, int father_uid, const GameName father_name, int mother_uid, const GameName mother_name, bool is_duet)
{
	long long baby_uuid = this->GetNewUniqueId(db_index);

	BabyDataParam baby_data;

	baby_data.baby_id = baby_uuid;
	baby_data.level = is_duet ? 0 : 1;			//
	baby_data.is_duet = is_duet ? 1 : 0;
	baby_data.father_id = father_uid;
	baby_data.mother_id = mother_uid;

	if (baby_name != NULL)
	{
		STRNCPY(baby_data.baby_name, baby_name, sizeof(baby_data.baby_name));
	}

	if (father_name != NULL)
	{
		STRNCPY(baby_data.father_name, father_name, sizeof(baby_data.father_name));
	}

	if (mother_name != NULL)
	{
		STRNCPY(baby_data.mother_name, mother_name, sizeof(baby_data.mother_name));
	}

	BabyDataNode baby_node;
	baby_node.baby_data_param = baby_data;

	m_all_baby_map[baby_uuid] = baby_node;	

	m_is_chage = true;

	return baby_uuid;
}

void BabyManager::RemoveBaby(long long baby_uid, int role_id)
{
	std::map<long long, BabyDataNode>::iterator iter = m_all_baby_map.find(baby_uid);
	if (iter != m_all_baby_map.end())
	{
		if (iter->second.baby_data_param.level == 0)			//如果生产阶段被删除，设置baby_id为0
		{
			iter->second.baby_data_param.baby_id = 0;
		}
		else
		{
			if (iter->second.baby_data_param.father_id == role_id)
			{
				iter->second.baby_data_param.father_id = 0;
				memset(iter->second.baby_data_param.father_name, 0, sizeof(iter->second.baby_data_param.father_name));
			}
			else if (iter->second.baby_data_param.mother_id == role_id)
			{
				iter->second.baby_data_param.mother_id = 0;
				memset(iter->second.baby_data_param.mother_name, 0, sizeof(iter->second.baby_data_param.mother_name));
			}
		}

		iter->second.is_dirty = true;
		m_is_chage = true;
	}
}

BabyDataNode * BabyManager::GetBabyData(long long baby_uuid)
{
	const std::map<long long, BabyDataNode>::iterator iter = m_all_baby_map.find(baby_uuid);
	if (iter != m_all_baby_map.end() && iter->second.baby_data_param.baby_id == baby_uuid)
	{
		return &iter->second;
	}

	return NULL;
}

void BabyManager::SetBabyDataDirty(BabyDataNode * baby_node_data)
{
	if (baby_node_data == NULL)
	{
		return;
	}

	m_is_chage = true;
	baby_node_data->is_dirty = true;
}

BabyManager::BabyManager() :m_load_flag(false), m_cur_sign(0), m_next_save_time(0), m_is_chage(false)
{

}

BabyManager::~BabyManager()
{
}

void BabyManager::Save()
{
	if (m_is_chage == false)
	{
		return;
	}

	AllBabyDataParam baby_data_param;
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	for (std::map<long long, BabyDataNode>::iterator iter = m_all_baby_map.begin(); iter != m_all_baby_map.end();)
	{
		baby_data_param.count = 0;

		BabyDataNode & baby_node = iter->second;
		if (baby_node.is_dirty)
		{
			if (baby_node.IsInvalid())
			{
				if (baby_node.is_new_create)
				{
					m_all_baby_map.erase(iter++);
					continue;
				}
				
				baby_data_param.addition_info[baby_data_param.count].change_state = structcommon::CS_DELETE;
				baby_data_param.baby_list[baby_data_param.count] = baby_node.baby_data_param;
				baby_data_param.count += 1;
			}
			else
			{
				baby_data_param.addition_info[baby_data_param.count].change_state = !baby_node.is_new_create ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				baby_data_param.baby_list[baby_data_param.count] = baby_node.baby_data_param;
				baby_data_param.count += 1;
			}
		}

		if (baby_data_param.count > 0)
		{
			RMISaveBabyDataBackObjectImpl * impl = new RMISaveBabyDataBackObjectImpl();

			if (ac.SaveBabyAsyn(baby_data_param, impl))
			{
				if (baby_data_param.addition_info[baby_data_param.count].change_state == structcommon::CS_DELETE)
				{
					m_all_baby_map.erase(iter++);
					continue;
				}
				else
				{
					iter->second.is_dirty = false;
					iter->second.is_new_create = false;
				}
			}
		}

		++iter;
	}

	m_is_chage = false;
}

long long BabyManager::GetNewUniqueId(int db_index)
{
	long long nowtime = EngineAdapter::Instance().Time();

	long long unique_id = (nowtime << 32) + static_cast<int>(db_index << DB_INDEX_MARK_BIT) + m_cur_sign;

	++m_cur_sign;
	if (m_cur_sign >= (1 << DB_INDEX_MARK_BIT))
	{
		m_cur_sign = 0;
	}

	return unique_id;
}


