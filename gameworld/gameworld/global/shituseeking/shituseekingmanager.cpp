#include "shituseekingmanager.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "obj/character/role.h"
#include "world.h"

#include "other/mentor/shitu.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/mentor/shituconfig.hpp"

#include "engineadapter.h"
#include "protocol/msgshitu.h"
#include "config/logicconfigmanager.hpp"
#include "gameworld/internalcomm.h"

#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/usercache/usercache.hpp"
#include "global/rmibackobjdef.h"

ShiTuSeekingManager & ShiTuSeekingManager::Instance()
{
	static ShiTuSeekingManager instance;
	return instance;
}

void ShiTuSeekingManager::OnServerStart()
{	
	this->LoadShiTuSeekingData(0);
}

void ShiTuSeekingManager::OnServerStop()
{
	this->Save();
}

bool ShiTuSeekingManager::LoadShiTuSeekingData(long long id_from)
{
	RMIInitShiTuSeekingDataBackObjectImpl *impl = new RMIInitShiTuSeekingDataBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitShiTuSeekingDataAsyn(id_from, impl);
}

void ShiTuSeekingManager::OnUserLogin(const UserID & user_id)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
	if (NULL != role)
	{
		this->SendPreferencesInfo(role);
	}
}

void ShiTuSeekingManager::Update(time_t now_second)
{
	if(!this->IsLoadFinish()) return;

	if (m_shi_tu_seeking_next_save_time > 0 && now_second > m_shi_tu_seeking_next_save_time)
	{
		this->CheckTimeOut();
		this->Save();
		m_shi_tu_seeking_next_save_time = EngineAdapter::Instance().Time() + SHI_TU_SEEKING_SAVE_TO_DB_TIME;
	}
}

void ShiTuSeekingManager::Init(const ShiTuSeekingParam & param)
{	
	//m_param.apprentice_count = param.apprentice_count;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (param.apprentice_list[i].data.Invalid()) continue;

		if( param.apprentice_list[i].data.seeking_type != SHI_TU_SEEKING_TYPE_APPRENTICE || param.apprentice_list[i].data.timestamp <= 0 || 
			param.apprentice_list[i].data.end_timestamp <= 0)
		{
			
			continue;
		}
		m_param.apprentice_list[i] = param.apprentice_list[i];
		m_apprentice_old_state[i] = true;
	}
//	m_param.master_count = param.master_count;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (param.master_list[i].data.Invalid()) continue;

		 if(param.master_list[i].data.seeking_type != SHI_TU_SEEKING_TYPE_MASTER ||param.master_list[i].data.timestamp <= 0 || 
			param.master_list[i].data.end_timestamp <= 0)
		{

			continue;
		}
		m_param.master_list[i] = param.master_list[i];
		m_master_old_state[i] = true;
	}
	m_load_flag = true;
	
	//初始化后保存一份数据,防止之后删除的时候没有数据可用
	this->Save(&m_param);
}

void ShiTuSeekingManager::GetInitParam(ShiTuSeekingParam * param)
{
	if(NULL == param) return;

	if(!m_change_flag) return;

	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (m_apprentice_dirty_mark[i])
		{
			if (m_param.apprentice_list[i].data.Invalid())
			{
				if(m_apprentice_old_state[i]) continue;

				param->apprentice_list[i].change_state = structcommon::CS_DELETE;
				param->apprentice_list[i].index = i;

			}	
			else
			{
				param->apprentice_list[i].change_state = m_apprentice_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->apprentice_list[i].index = i;
			
				param->apprentice_list[i].data = m_param.apprentice_list[i].data;
			}
		}
	}

	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (m_master_dirty_mark[i])
		{
			if (m_param.master_list[i].data.Invalid())
			{
				if (m_master_old_state[i]) continue;

				param->master_list[i].change_state = structcommon::CS_DELETE;
				param->master_list[i].index = i;
			}
			else
			{
				param->master_list[i].change_state = m_master_old_state[i] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				param->master_list[i].index = i;

				param->master_list[i].data = m_param.master_list[i].data;
			}
		}
	}

}

void ShiTuSeekingManager::ClearDirtyMark(ShiTuSeekingParam * param)
{
	if(!m_change_flag) return;

	memset(m_apprentice_dirty_mark, 0, sizeof(m_apprentice_dirty_mark));
	memset(m_master_dirty_mark, 0, sizeof(m_master_dirty_mark));
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{	
		m_apprentice_old_state[i] = true;
		param->apprentice_list[i].change_state = structcommon::CS_NONE;
		m_param.apprentice_list[i].change_state = structcommon::CS_NONE;
	}
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		m_master_old_state[i] = true;
		param->master_list[i].change_state = structcommon::CS_NONE;
		m_param.master_list[i].change_state = structcommon::CS_NONE;
	}
	m_change_flag = false;
}

void ShiTuSeekingManager::OnUserChangeSeeking(Role *role, int seeking_type, ShiTuSeekingNotice seeking_notice)
{
	if (NULL == role || seeking_notice == NULL)
	{
		return;
	}
	const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();
	bool is_find = false;
	if (seeking_type == SHI_TU_SEEKING_TYPE_APPRENTICE)  //徒弟
	{
		if (role->GetRoleModuleManager()->GetShiTu()->IsApprenticeFull())
		{
			return;
		}
		//查找玩家是否已经发布宣言了
		int seq = this->GetApprenticeSeekingIndexByUid(role->GetUID());
		if (seq != -1)
		{
			this->GetApprenticeSeekingData(seq, role->GetUID(), seeking_type, seeking_notice);
			//此处不做改动状态,防止新增状态被改动
			m_apprentice_dirty_mark[seq] = true;
			is_find = true;
		}

		if (!is_find)
		{
			//查找是否有空的位置
			bool is_succ = false;
			for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
			{
				if(!m_param.apprentice_list[i].data.Invalid()) continue;

				this->GetApprenticeSeekingData(i, role->GetUID(), seeking_type, seeking_notice);
				//如果状态为true(空),则此时转为false,为新增,如果此时状态为false(删除),则此时转为true,为update
				if (m_apprentice_old_state[i])
				{
					m_apprentice_old_state[i] = false;
				}
				else
				{
					m_apprentice_old_state[i] = true;
				}
				m_apprentice_dirty_mark[i] = true;
				is_succ = true;
				break;
			}
			if (!is_succ)
			{	
				//查找其中最早发布的宣言进行替换
				int index = 0;
				for (int i = 1; i < SHI_TU_SEEKING_MAX_COUNT; i++)
				{
					//运行到这的时候,理论上不会出现空的情况
					if (m_param.apprentice_list[i].data.Invalid() || m_param.apprentice_list[index].data.Invalid()) continue;

					if (m_param.apprentice_list[i].data.timestamp < m_param.apprentice_list[index].data.timestamp)
					{
						index = i;
					}
				}
				this->GetApprenticeSeekingData(index, role->GetUID(), seeking_type, seeking_notice);
				m_apprentice_old_state[index] = true;
				m_apprentice_dirty_mark[index] = true;
			}
		}
	}
	else if (seeking_type == SHI_TU_SEEKING_TYPE_MASTER)				//师傅
	{
		if (role->GetLevel() < other_cfg.graduation_level || role->GetRoleModuleManager()->GetShiTu()->IsMasterFull())
		{
			return;
		}
		//查找玩家是否已经发布宣言了
		int seq  = this->GetMasterSeekingIndexByUid(role->GetUID());
		if (seq != -1)
		{
			this->GetMasterSeekingData(seq, role->GetUID(), seeking_type, seeking_notice);
			//此处不做改动状态,防止新增状态被改动
			m_master_dirty_mark[seq] = true;
			is_find = true;
		}

		if (!is_find)
		{
			//查找是否有空的位置
			bool is_succ = false;
			for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
			{
				if (!m_param.master_list[i].data.Invalid()) continue;

				this->GetMasterSeekingData(i, role->GetUID(), seeking_type, seeking_notice);
				//如果状态为true(空),则此时转为false,为新增,如果此时状态为false(删除),则此时转为true,为update
				if (m_master_old_state[i])
				{
					m_master_old_state[i] = false;
				}
				else
				{
					m_master_old_state[i] = true;
				}
				m_master_dirty_mark[i] = true;
				is_succ = true;
				break;
			}
			if (!is_succ)
			{
				//查找其中最早发布的宣言进行替换
				int index = 0;
				for (int i = 1; i < SHI_TU_SEEKING_MAX_COUNT; i++)
				{
					//运行到这的时候,理论上不会出现空的情况
					if (m_param.master_list[i].data.Invalid() || m_param.master_list[index].data.Invalid()) continue;

					if (m_param.master_list[i].data.timestamp < m_param.master_list[index].data.timestamp)
					{
						index = i;
					}
				}
				this->GetMasterSeekingData(index, role->GetUID(), seeking_type, seeking_notice);
				m_master_old_state[index] = true;
				m_master_dirty_mark[index] = true;
			}
		}
	}

	m_change_flag = true;
	role->NoticeNum(noticenum::NT_SHI_TU_SEEKING_SUCC);
	this->SendShiTuSeekingNoticeList(seeking_type, role);
}

void ShiTuSeekingManager::SetPreferences(Role * role, char(&arg)[SHI_TU_PREFERENCES_NUM])
{
	char(&set)[SHI_TU_PREFERENCES_NUM] = role->GetRoleModuleManager()->GetCommonData().shi_tu_preferences;
	for (int i = 0; i < ARRAY_LENGTH(set) && i < ARRAY_LENGTH(arg); ++i)
	{
		set[i] = arg[i];
	}

	this->SendPreferencesInfo(role);
}
void ShiTuSeekingManager::SendPreferencesInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCShiTuPreferencesInfo  protocol;
	char(&set)[SHI_TU_PREFERENCES_NUM] = role->GetRoleModuleManager()->GetCommonData().shi_tu_preferences;
	for (int i = 0; i < ARRAY_LENGTH(set) && i < ARRAY_LENGTH(protocol.preferences); ++i)
	{
		protocol.preferences[i] = set[i];
	}



	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
}


void ShiTuSeekingManager::SendShiTuSeekingNoticeList(int seeking_type, Role * role, int role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameShiTuSeekReq req;
		req.type = crossgameprotocal::HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE_INFO;
		req.role_id = role_id;
		req.p1 = seeking_type;

		InternalComm::Instance().SendToGameThroughCrossByRoleId(&req, sizeof(req), role_id);

		return;
	}

	if (role == NULL)
	{
		return;
	}

	Protocol::SCShiTuSeekingInfo protocol;
	protocol.count = 0;
	protocol.seeking_type = seeking_type;
	memset(protocol.self_seeking, 0, sizeof(protocol.self_seeking));
	
	int index = this->GetApprenticeSeekingIndexByUid(role->GetUID());
	if (index != -1)
	{
		F_STRNCPY(protocol.self_seeking, m_param.apprentice_list[index].data.seeking_notice, sizeof(protocol.self_seeking));
	}
	index = this->GetMasterSeekingIndexByUid(role->GetUID());
	if (index != -1)
	{
		F_STRNCPY(protocol.self_seeking, m_param.master_list[index].data.seeking_notice, sizeof(protocol.self_seeking));
	}

	int count = 0;
	if (seeking_type == SHI_TU_SEEKING_TYPE_MASTER)		//如果是师傅，就要发徒弟宣言列表
	{
		for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
		{
			if(m_param.apprentice_list[i].data.Invalid()) continue;

			ShiTuSeeking & node = m_param.apprentice_list[i].data;
			UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(node.uid);
			if (NULL == user_cache_node)
			{
				continue;
			}

			bool is_online = UserCacheManager::Instance().IsUserOnline(node.uid);
			protocol.seeking_info[count].user_id = user_cache_node->uid;
			protocol.seeking_info[count].sex = user_cache_node->GetSex();
			protocol.seeking_info[count].prof = user_cache_node->profession;
			protocol.seeking_info[count].is_online = is_online;
			protocol.seeking_info[count].level = user_cache_node->level;
			protocol.seeking_info[count].timestamp = node.timestamp;
			protocol.seeking_info[count].avatar_type = user_cache_node->avatar_type;
			protocol.seeking_info[count].headshot_id = user_cache_node->appearance.headshot_id;

			F_STRNCPY(protocol.seeking_info[count].gamename, user_cache_node->role_name, sizeof(protocol.seeking_info[count].gamename));
			F_STRNCPY(protocol.seeking_info[count].seeking_notice, node.seeking_notice, sizeof(protocol.seeking_info[count].seeking_notice));

			memset(protocol.seeking_info[count].guild_name, 0, sizeof(protocol.seeking_info[count].guild_name));

			count += 1;
		}
		protocol.count = count;
	}
	else  	if (seeking_type == SHI_TU_SEEKING_TYPE_APPRENTICE)		//如果是徒弟，就要发师傅宣言列表
	{
		for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
		{
			if (m_param.master_list[i].data.Invalid()) continue;

			ShiTuSeeking & node = m_param.master_list[i].data;
			UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(node.uid);
			if (NULL == user_cache_node)
			{
				continue;
			}

			bool is_online = UserCacheManager::Instance().IsUserOnline(node.uid);
			protocol.seeking_info[count].user_id = user_cache_node->uid;
			protocol.seeking_info[count].sex = user_cache_node->GetSex();
			protocol.seeking_info[count].prof = user_cache_node->profession;
			protocol.seeking_info[count].is_online = is_online;
			protocol.seeking_info[count].level = user_cache_node->level;
			protocol.seeking_info[count].timestamp = node.timestamp;
			protocol.seeking_info[count].avatar_type = user_cache_node->avatar_type;
			protocol.seeking_info[count].headshot_id = user_cache_node->appearance.headshot_id;

			F_STRNCPY(protocol.seeking_info[count].gamename, user_cache_node->role_name, sizeof(protocol.seeking_info[count].gamename));
			F_STRNCPY(protocol.seeking_info[count].seeking_notice, node.seeking_notice, sizeof(protocol.seeking_info[count].seeking_notice));

			memset(protocol.seeking_info[count].guild_name, 0, sizeof(protocol.seeking_info[count].guild_name));

			count += 1;
		}
		protocol.count = count;
	}

	unsigned int length = sizeof(protocol) - (SHI_TU_SEEKING_MAX_COUNT - protocol.count) * sizeof(protocol.seeking_info[0]);
	
	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, length);
	}

	if (role_id)
	{
		InternalComm::Instance().SendToHiddenUserThroughCross(role_id, &protocol, length);
	}

}

void ShiTuSeekingManager::RemoveSeeking(Role * role, bool is_notice)
{
	int uid = role->GetUID();
	int index = this->GetApprenticeSeekingIndexByUid(uid);
	if (index != -1)
	{
		this->ClearShiTuSeekingItem(SHI_TU_SEEKING_TYPE_APPRENTICE, index);
	}
	index = this->GetMasterSeekingIndexByUid(uid);
	if (index != -1)
	{
		this->ClearShiTuSeekingItem(SHI_TU_SEEKING_TYPE_MASTER, index);
	}

	if (is_notice)
	{
		const ShiTuOtherCfg & other_cfg = LOGIC_CONFIG->GetShiTuConfig()->GetOtherCfg();
		this->SendShiTuSeekingNoticeList(role->GetLevel() >= other_cfg.graduation_level ? SHI_TU_SEEKING_TYPE_APPRENTICE : SHI_TU_SEEKING_TYPE_MASTER, role);
	}
}

ShiTuSeekingManager::ShiTuSeekingManager(): m_change_flag(false), m_load_flag(false), m_shi_tu_seeking_next_save_time(0)
{
	memset(m_apprentice_dirty_mark, 0, sizeof(m_apprentice_dirty_mark));
	memset(m_master_dirty_mark, 0, sizeof(m_master_dirty_mark));
	memset(m_apprentice_old_state, true, sizeof(m_apprentice_old_state));
	memset(m_master_old_state, true, sizeof(m_master_old_state));
}

ShiTuSeekingManager::~ShiTuSeekingManager()
{
}


void ShiTuSeekingManager::ClearShiTuSeekingItem(int seeking_type, int index)
{
	if (index < 0 || index >= SHI_TU_SEEKING_MAX_COUNT) return;

	if (seeking_type == SHI_TU_SEEKING_TYPE_APPRENTICE)
	{
		if (m_param.apprentice_list[index].data.Invalid()) return;

		m_param.apprentice_list[index].data.Reset();
		m_apprentice_old_state[index] = false;
		m_apprentice_dirty_mark[index] = true;
		m_change_flag = true;
	}
	else if (seeking_type == SHI_TU_SEEKING_TYPE_MASTER)
	{
		if (m_param.master_list[index].data.Invalid()) return;

		m_param.master_list[index].data.Reset();
		m_master_old_state[index] = false;
		m_master_dirty_mark[index] = true;
		m_change_flag = true;
	}
}

void ShiTuSeekingManager::CheckTimeOut()
{
	time_t now_second = EngineAdapter::Instance().Time();
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if(m_param.apprentice_list[i].data.Invalid()) continue;

		if (now_second > m_param.apprentice_list[i].data.end_timestamp)
		{
			this->ClearShiTuSeekingItem(m_param.apprentice_list[i].data.seeking_type, i);
		}
	}

	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (m_param.master_list[i].data.Invalid()) continue;

		if (now_second > m_param.master_list[i].data.end_timestamp)
		{
			this->ClearShiTuSeekingItem(m_param.master_list[i].data.seeking_type, i);
		}
	}
}

void ShiTuSeekingManager::Save(const ShiTuSeekingParam * param)
{
	if (!this->IsLoadFinish()) return;

	static ShiTuSeekingParam shi_tu_seeking_param;
	if (param != NULL)
	{
		shi_tu_seeking_param = *param;
	}
	this->GetInitParam(&shi_tu_seeking_param);

	RMISaveShiTuSeekingDataBackObjectImpl *backobj = new RMISaveShiTuSeekingDataBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveShiTuSeekingDataAsyn(shi_tu_seeking_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&shi_tu_seeking_param);
	}

}

void ShiTuSeekingManager::LoadShiTuSeekingSucc()
{
	if (this->IsLoadFinish())
	{
		m_shi_tu_seeking_next_save_time = EngineAdapter::Instance().Time() + SHI_TU_SEEKING_SAVE_TO_DB_TIME;
	}
}

void ShiTuSeekingManager::GetApprenticeSeekingData(int index, int uid, int seeking_type, ShiTuSeekingNotice seeking_notice)
{
	if(index < 0 || index >= SHI_TU_SEEKING_MAX_COUNT) return;

	m_param.apprentice_list[index].data.uid = uid;
	m_param.apprentice_list[index].data.seeking_type = seeking_type;
	m_param.apprentice_list[index].data.timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_param.apprentice_list[index].data.end_timestamp = m_param.apprentice_list[index].data.timestamp + SHI_TU_SEEKING_BEING_TIME;
	F_STRNCPY(m_param.apprentice_list[index].data.seeking_notice, seeking_notice, sizeof(m_param.apprentice_list[index].data.seeking_notice));
}

void ShiTuSeekingManager::GetMasterSeekingData(int index, int uid, int seeking_type, ShiTuSeekingNotice seeking_notice)
{
	if (index < 0 || index >= SHI_TU_SEEKING_MAX_COUNT) return;

	m_param.master_list[index].data.uid = uid;
	m_param.master_list[index].data.seeking_type = seeking_type;
	m_param.master_list[index].data.timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_param.master_list[index].data.end_timestamp = m_param.master_list[index].data.timestamp + SHI_TU_SEEKING_BEING_TIME;
	F_STRNCPY(m_param.master_list[index].data.seeking_notice, seeking_notice, sizeof(m_param.master_list[index].data.seeking_notice));
}

int ShiTuSeekingManager::GetApprenticeSeekingIndexByUid(int uid)
{
	int ret = -1;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (m_param.apprentice_list[i].data.Invalid()) continue;

		if (uid == m_param.apprentice_list[i].data.uid)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

int ShiTuSeekingManager::GetMasterSeekingIndexByUid(int uid)
{
	int ret = -1;
	for (int i = 0; i < SHI_TU_SEEKING_MAX_COUNT; i++)
	{
		if (m_param.master_list[i].data.Invalid()) continue;

		if (uid == m_param.master_list[i].data.uid)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

void ShiTuSeekingManager::RemoveEarliestTimeApprenticeSeeking()
{
	std::map<int, ShiTuSeeking>::iterator remove = m_apprentice_seeking_list.end();
	for (std::map<int, ShiTuSeeking>::iterator iter = m_apprentice_seeking_list.begin(); iter != m_apprentice_seeking_list.end(); ++iter)
	{
		if (remove == m_apprentice_seeking_list.end() || remove->second.timestamp > iter->second.timestamp)
		{
			remove = iter;
		}
	}

	if (remove != m_apprentice_seeking_list.end())
	{
		m_apprentice_seeking_list.erase(remove);
	}
}

void ShiTuSeekingManager::RemoveEarliestTimeMasterSeeking()
{
	std::map<int, ShiTuSeeking>::iterator remove = m_master_seeking_list.end();
	for (std::map<int, ShiTuSeeking>::iterator iter = m_master_seeking_list.begin(); iter != m_master_seeking_list.end(); ++iter)
	{
		if (remove == m_master_seeking_list.end() || remove->second.timestamp > iter->second.timestamp)
		{
			remove = iter;
		}
	}

	if (remove != m_master_seeking_list.end())
	{
		m_master_seeking_list.erase(remove);
	}
}

void RMIInitShiTuSeekingDataBackObjectImpl::InitShiTuSeekingDataRet(int ret, const ShiTuSeekingParam & list_param)
{
	if (ret < 0)
	{
		printf("RMIInitShiTuSeekingDataBackObjectImpl::InitShiTuSeekingDataRet Failed \n");
	}
	else
	{
		ShiTuSeekingManager &shi_tu_seeking_mgr = ShiTuSeekingManager::Instance();
		if (NULL == &shi_tu_seeking_mgr) return;

		shi_tu_seeking_mgr.Init(list_param);
		shi_tu_seeking_mgr.LoadShiTuSeekingSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}

void RMISaveShiTuSeekingDataBackObjectImpl::SaveShiTuSeekingDataRet(int ret)
{
	if (0 != ret)
	{
		printf("RMISaveShiTuSeekingDataBackObjectImpl::SaveShiTuSeekingDataRet Failed ret:%d\n", ret);
	}
}
