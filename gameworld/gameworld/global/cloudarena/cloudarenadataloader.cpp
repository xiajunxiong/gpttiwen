#include "cloudarenadataloader.hpp"
#include "servercommon/servercommon.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "cloudarena.hpp"

CloudArenaDataLoader::CloudArenaDataLoader() : m_data_status(GLOBAL_SYSTEM_DATA_STATE_INVALID),
	m_user_data_status(GLOBAL_SYSTEM_DATA_STATE_INVALID)
{

}

CloudArenaDataLoader::~CloudArenaDataLoader()
{

}

void CloudArenaDataLoader::SetDataStatus(int status)
{
	m_data_status = status;
	this->OnLoadStatusChange();
}

void CloudArenaDataLoader::SetUserDataStatus(int status)
{
	m_user_data_status = status;
	this->OnLoadStatusChange();
}

bool CloudArenaDataLoader::IsAllLoadFinish() const
{
	return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_data_status
		&& GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_user_data_status;
}

bool CloudArenaDataLoader::LoadData()
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_user_data_status) return false;

	m_data_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;

	RMIInitCloudArenaDataBackObjectImpl* impl = new RMIInitCloudArenaDataBackObjectImpl();
	
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitCloudArenaDataAsyn(impl);
}

bool CloudArenaDataLoader::LoadUserData(long long offset)
{
	if (GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_user_data_status) return false;

	m_user_data_status = GLOBAL_SYSTEM_DATA_STATE_LOADING;

	RMIInitCloudArenaUserDataBackObjectImpl* impl = new RMIInitCloudArenaUserDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	return ac.InitCloudArenaUserDataAsyn(offset, impl);
}

void CloudArenaDataLoader::OnLoadStatusChange()
{
	if (this->IsAllLoadFinish())
	{
		CloudArena::Instance().OnLoadAllSucc();
	}
}

