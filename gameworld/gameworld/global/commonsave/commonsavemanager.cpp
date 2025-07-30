#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "engineadapter.h"

#include "commonsavemanager.hpp"

CommonSaveManager & CommonSaveManager::Instance()
{
	static CommonSaveManager r;
	return r;
}

void CommonSaveManager::OnServerStart()
{
	this->LoadCommonData(0);
}

void CommonSaveManager::OnServerStop()
{
	this->SaveCommonData();
}

void CommonSaveManager::Update(unsigned long interval, time_t now_second)
{
	for (int i = 0; i < ARRAY_LENGTH(m_common_save_list); ++i)
	{
		if (NULL != m_common_save_list[i])
		{
			m_common_save_list[i]->Update(interval, now_second);
		}
	}
}

bool CommonSaveManager::IsLoadFinish() const
{
	return COMMON_SAVE_DATA_STATUS_LOAD_FINISH == m_data_state;
}

CommonSave * CommonSaveManager::GetCommonSave(int save_type)
{
	if (save_type < 0 || save_type >= ARRAY_LENGTH(m_common_save_list))
	{
		return NULL;
	}

	return m_common_save_list[save_type];
}

void CommonSaveManager::Gm(int op, int save_type, int p1, int p2, int p3)
{
	CommonSave * cs = this->GetCommonSave(save_type);
	if (cs == NULL)
	{
		return;
	}
	CommonSaveKey key(p1, p2, p3);

	if (op == 0)
	{
		std::vector<char> * data = cs->GetData(key,true);

		if (data && !data->empty())
		{
			(*data)[0] = 1;
			cs->SetDataChange(key);			
		}
	}

	if (op == 1)
	{
		cs->RemoveData(key);
	}

	if (op == 2)
	{
		cs->ClearAllData();
	}
}

void CommonSaveManager::RegisterLoadSuccCallback(ICommonSaveLoadSuccCallback* callback)
{
	m_load_succ_callback.push_back(callback); 
	// 在跨服中的原服RandAcitivityManager会错失调用callback的时机，
	// 但跨服中的原服RandActivityManager数据来自原服，所以暂时没有问题。
	// 若以后需要在跨服中的原服RandActivityManager开启活动，则需要修改这里
}

CommonSaveManager::CommonSaveManager()
{
	m_data_state = COMMON_SAVE_DATA_STATUS_INVALID;
	memset(m_common_save_list, 0, sizeof(m_common_save_list));
	
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_HUAN_JIE_TIAN_TI_RECORD));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_HUAN_JIE_SAVE_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_GUILD_FIGHT_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_GUILD_HONOR_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_OLD_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_GIVE_GIFT));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_RA_DELAY_SET_TIME_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_GUILD_HONOR_RECORD_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ROLE_XUAN_YAN));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU_XUAN_YAN));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_COW_REPORT_GOOD_NEWS_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_TIGER_BRINGS_BLESSING_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_QING_YUAN_DATA));
	this->Add(new CommonSave(COMMON_SAVE_DATA_TYPE_HONGMENG_TIANJI));

	UNSTD_STATIC_CHECK(22 == COMMON_SAVE_DATA_TYPE_MAX);
}

CommonSaveManager::~CommonSaveManager()
{
	for (int i = 0; i < ARRAY_LENGTH(m_common_save_list); ++i)
	{
		if (NULL != m_common_save_list[i])
		{
			delete m_common_save_list[i];
		}
	}

	memset(m_common_save_list, 0, sizeof(m_common_save_list));
}

void CommonSaveManager::InitCommoneData(const CommonSaveDataParam::DBCommonSaveDataItem & data_item)
{
	CommonSave * cs = this->GetCommonSave(data_item.save_data_type);
	if (NULL != cs)
	{
		CommonSaveKey key(data_item.param1, data_item.param2, data_item.param3);
		cs->InitData(key, data_item.data);
	}
}

bool CommonSaveManager::LoadCommonData(long long id_fram)
{
	if (COMMON_SAVE_DATA_STATUS_LOAD_FINISH == m_data_state) return false;

	m_data_state = COMMON_SAVE_DATA_STATUS_LOADING;

	RMIInitCommonDataBackObjectImpl * impl = new RMIInitCommonDataBackObjectImpl();

	RMIGlobalClient gc;
	gc.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return gc.InitCommonDataAsyn(id_fram, impl);
}

void CommonSaveManager::LoadCommonDataSucc()
{
	m_data_state = COMMON_SAVE_DATA_STATUS_LOAD_FINISH;

	for (size_t i = 0; i < m_load_succ_callback.size(); ++i)
	{
		ICommonSaveLoadSuccCallback* callback = m_load_succ_callback[i];
		callback->Callback();
	}
}

bool CommonSaveManager::SaveCommonData()
{
	for (int i = 0; i < ARRAY_LENGTH(m_common_save_list); ++i)
	{
		if (NULL != m_common_save_list[i])
		{
			m_common_save_list[i]->Save(true);
		}
	}

	return true;
}

void CommonSaveManager::Add(CommonSave * common_save)
{
	if (common_save == NULL)
	{		
		return;
	}

	int save_type = common_save->GetSaveType();
	if (save_type < 0 || save_type >= ARRAY_LENGTH(m_common_save_list))
	{
		printf("add common save fail,save_type:%d ", save_type);
		assert(0);
		return;
	}

	if (m_common_save_list[save_type] != NULL)
	{
		printf("add common save fail,save_type:%d ", save_type);
		assert(0);
		return;
	}

	m_common_save_list[save_type] = common_save;
}

void RMIInitCommonDataBackObjectImpl::InitDataRet(int ret, const CommonSaveDataParam & data_param)
{
	if (ret < 0)
	{
		printf("%s Failed ret=%d", __FUNCTION__, ret);
		EngineAdapter::Instance().StopGame();
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < data_param.count && i < ARRAY_LENGTH(data_param.data_list); i++)
	{
		const CommonSaveDataParam::DBCommonSaveDataItem & role_data_item = data_param.data_list[i];
		next_id_from = GetMax(next_id_from, role_data_item.inc_id);

		CommonSaveManager::Instance().InitCommoneData(role_data_item);
	}

	if (data_param.count > 0)
	{
		CommonSaveManager::Instance().LoadCommonData(next_id_from);
	}
	else
	{
		CommonSaveManager::Instance().LoadCommonDataSucc();
	}
}