#include "rapaimaihangmanager.hpp"
#include "engineadapter.h"
#include "internalcomm.h"
#include "gamelog.h"

#include "obj/character/role.h"

#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivitypaimaihang.hpp"

#include "global/rmibackobjdef.h"

#include "protocol/randactivity/msgrapaimaihang.hpp"

RAPaiMaiHangManager & RAPaiMaiHangManager::Instance()
{
	static RAPaiMaiHangManager ra_pai_mai_hang_manager;
	return ra_pai_mai_hang_manager;
}

void RAPaiMaiHangManager::OnServerStart()
{
	this->LoadRAPaiMaiHangData(0);
}

void RAPaiMaiHangManager::OnServerStop()
{
	this->Save();
}

bool RAPaiMaiHangManager::LoadRAPaiMaiHangData(long long id_from)
{
	RMIInitRAPaiMaiHangBackObjectImpl * impl = new RMIInitRAPaiMaiHangBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitRAPaiMaiHangDataAsyn(id_from, impl);
}

void RAPaiMaiHangManager::Update(time_t now_second)
{
	if (now_second >= m_next_save_db_timestamp && this->IsDirty())
	{
		this->Save();
		m_next_save_db_timestamp = now_second + MAX_NEXT_SAVE_DB_TIMESTAMP;
	}

}

void RAPaiMaiHangManager::Init(const RARoleParMaiHangDBParam & param)
{
	for (std::map<int, RARoleMaiHangDBItem>::const_iterator it = param.data_list.begin(); it != param.data_list.end(); ++it)
	{
		if (it->first <= 0 || it->first != it->second.uid)
		{
			gamelog::g_log_ra_pai_mai_hang.printf(LL_CRITICAL, "[%s] uid[%d] != it->first[%d]", __FUNCTION__, it->second.uid, it->first);
			continue;
		}
		m_param.data_list[it->first] = it->second;
		m_dirty_mark[it->first] = false;
		m_old_state[it->first] = true;
	}

	m_load_flag = true;

	//初始化后保存一份数据,防止之后删除时没有数据可用
	this->Save(&m_param);
}

void RAPaiMaiHangManager::GetInitParam(RARoleParMaiHangDBParam * param)
{
	if (!m_load_flag || !m_change_flag)
	{
		return;
	}

	*param = m_param;

	std::set<int> delete_set;
	for (std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.begin(); it != m_param.data_list.end(); it++)
	{
		if (m_dirty_mark[it->first])
		{
			if (!it->second.IsValid())
			{
				if (m_old_state[it->first]) continue;

				param->data_list[it->first].change_state = structcommon::CS_DELETE;
				delete_set.insert(it->first);

			}
			else
			{
				param->data_list[it->first].change_state = m_old_state[it->first] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
			}
		}
	}

	for (std::set<int>::iterator it = delete_set.begin(); it != delete_set.end(); it++)
	{
		m_param.data_list.erase(*it);
	}
}

void RAPaiMaiHangManager::ClearDirtyMark(RARoleParMaiHangDBParam * param)
{
	if (!m_change_flag) return;

	for (std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.begin(); it != m_param.data_list.end(); it++)
	{
		m_old_state[it->first] = true;
		m_dirty_mark[it->first] = false;
		it->second.change_state = structcommon::CS_NONE;
		param->data_list[it->first].change_state = structcommon::CS_NONE;
	}
	m_change_flag = false;
}

void RAPaiMaiHangManager::OnAddRecord(int role_id, int type, int seq, int activity_open_day, int param1, int param2, int param3)
{
	if(type < 0 || type >= RA_PAI_HANG_BANG_RECORD_TYPE_MAX || role_id <= 0) return;
	
	RAPaiMaiHangRecordParam record_info;
	record_info.type = type;
	record_info.seq = seq;
	record_info.activity_open_day = activity_open_day;
	record_info.param1 = param1;
	record_info.param2 = param2;
	record_info.param3 = param3;
	record_info.record_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.find(role_id);
	if (m_param.data_list.end() == it)
	{
		m_param.data_list[role_id].uid = role_id;
		m_old_state[role_id] = false;
	}
	else
	{
		m_old_state[role_id] = m_old_state[role_id] ? true : false;
		m_param.data_list[role_id].uid = role_id;
	}

	m_change_flag = true;
	m_dirty_mark[role_id] = true;
	RARoleMaiHangDBItem & db_item = m_param.data_list[role_id];
	if (db_item.record_count >= ARRAY_LENGTH(db_item.record_list))
	{
		db_item.record_list[0] = record_info;
		std::sort(db_item.record_list, db_item.record_list + ARRAY_LENGTH(db_item.record_list));
	}
	else
	{
		db_item.record_list[db_item.record_count++] = record_info;
	}
}

void RAPaiMaiHangManager::OnClearRecordByRole(int role_id)
{
	if(role_id <= 0 || m_param.data_list.empty()) return;

	std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.find(role_id);
	if(it == m_param.data_list.end()) return;

	it->second.uid = 0;
	m_change_flag = true;
	m_dirty_mark[role_id] = true;
	m_old_state[role_id] = false;
}

void RAPaiMaiHangManager::OnClearAllRecord()
{
	if(m_param.data_list.empty()) return;
	 
	for (std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.begin(); it != m_param.data_list.end(); ++it)
	{
		it->second.uid = 0;
		m_dirty_mark[it->first] = true;
		m_old_state[it->first] = false;
	}
	m_change_flag = true;
}

void RAPaiMaiHangManager::OnSendRecordToRole(Role * role)
{
	if(NULL == role) return;

	Protocol::SCRAPaiMaiHangUserRecordInfo info;
	info.record_count = 0;
	std::map<int, RARoleMaiHangDBItem>::iterator it = m_param.data_list.find(role->GetUID());
	if (it != m_param.data_list.end())
	{
		for (int i = 0; i < it->second.record_count && i < ARRAY_LENGTH(it->second.record_list) && i < ARRAY_LENGTH(info.record_list); i++)
		{
			if(it->second.record_list[i].activity_open_day <= 0) continue;

			info.record_list[info.record_count++] = it->second.record_list[i];
		}
	}

	int length = sizeof(info) - (ARRAY_LENGTH(info.record_list) - info.record_count) * sizeof(info.record_list[0]);
	role->GetRoleModuleManager()->NetSend(&info, length);
}

RAPaiMaiHangManager::RAPaiMaiHangManager() : m_change_flag(false), m_load_flag(false)
{
	m_param.data_list.clear();
	m_dirty_mark.clear();
	m_old_state.clear();
}

RAPaiMaiHangManager::~RAPaiMaiHangManager()
{
}

void RAPaiMaiHangManager::Save(const RARoleParMaiHangDBParam * param)
{
	if (!this->IsLoadFinish()) return;

	static RARoleParMaiHangDBParam ra_pai_mai_hang_param;
	if (NULL != param)
	{
		ra_pai_mai_hang_param = *param;
	}
	this->GetInitParam(&ra_pai_mai_hang_param);

	RMISaveRAPaiMaiHangBackObjectImpl *backobj = new RMISaveRAPaiMaiHangBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveRAPaiMaiHangDataAsyn(ra_pai_mai_hang_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&ra_pai_mai_hang_param);
	}
}

void RAPaiMaiHangManager::LoadRAPaiMaiHangSucc()
{
	if (this->IsLoadFinish())
	{
		m_next_save_db_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + MAX_NEXT_SAVE_DB_TIMESTAMP;
	}
}

void RMIInitRAPaiMaiHangBackObjectImpl::InitRAPaiMaiHangDataRet(int ret, const RARoleParMaiHangDBParam &list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		RAPaiMaiHangManager & ra_pai_mai_hang_manager = RAPaiMaiHangManager::Instance();
		if (NULL == &ra_pai_mai_hang_manager) return;

		ra_pai_mai_hang_manager.Init(list_param);
		ra_pai_mai_hang_manager.LoadRAPaiMaiHangSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}
