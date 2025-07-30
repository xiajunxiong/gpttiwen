#include "gatherdreamrecordmanager.hpp"
#include "engineadapter.h"
#include "internalcomm.h"
#include "gamelog.h"

#include "global/rmibackobjdef.h"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "item/itempool.h"
#include "obj/character/role.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msgother.h"
#include "other/rolecross/rolecross.hpp"
#include "other/rolemodulemanager.hpp"

GatherDreamRecordManager & GatherDreamRecordManager::Instance()
{
	static GatherDreamRecordManager gather_dream_manager;
	return gather_dream_manager;
}

void GatherDreamRecordManager::OnServerStart()
{
	this->LoadGatherDreamData(0);
}

void GatherDreamRecordManager::OnServerStop()
{
	this->Save();
}

bool GatherDreamRecordManager::LoadGatherDreamData(long long id_from)
{
	RMIInitGatherDreamRecordBackObjectImpl *impl = new RMIInitGatherDreamRecordBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitGatherDreamRecordDataAsyn(id_from, impl);
}

void GatherDreamRecordManager::Update(time_t now_second)
{
	if(!this->IsLoadFinish()) return;

	if (m_gather_dream_record_next_save_time > 0 && now_second >= m_gather_dream_record_next_save_time)
	{	
		this->Save();
		m_gather_dream_record_next_save_time = EngineAdapter::Instance().Time() + GATHER_DREAM_RECORD_SAVE_TO_DB_TIME;
	}
}

void GatherDreamRecordManager::Init(const GatherDreamRecordParam & param)
{
	UNSTD_STATIC_CHECK(GATHER_DREAM_INFO_TYPE_MAX == 2);
	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		const RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? param.get_data_list : param.gift_data_list;
		RecordMap & m_info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
		for (RecordMap::const_iterator it = info_list.begin(); it != info_list.end(); it++)
		{
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				if (it->second[k].index < 0)
				{
					gamelog::g_log_gather_dream_record.printf(LL_CRITICAL, "[%s INVALID] role[%d, %d]", __FUNCTION__, it->first, it->second[k].index);
					continue;
				}
				if (NULL == ItemPool::Instance()->GetItem(it->second[k].data_info.item_id))
				{
					gamelog::g_log_gather_dream_record.printf(LL_CRITICAL, "[%s INVALID] role[%d, %d] item[%d] ", __FUNCTION__, it->first, it->second[k].index, it->second[k].data_info.item_id);
					continue;
				}
				m_info_list[it->first].push_back(it->second[k]);
				m_dirty_mark[GatherDreamRecordKey(it->first, it->second[k].index)] = false;
				m_old_state[GatherDreamRecordKey(it->first, it->second[k].index)] = true;
			}
		}
	}
	m_load_flag = true;

	//初始化后保存一份数据,防止之后删除的时候没有数据可用
	this->Save(&m_param);
}

void GatherDreamRecordManager::GetInitParam(GatherDreamRecordParam * param)
{
	if (!this->IsLoadFinish() || !m_change_flag)
	{
		return;
	}
	*param = m_param;
	std::map<GatherDreamRecordKey, int> delete_map;
	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? param->get_data_list : param->gift_data_list;
		for (RecodMapIt it = info_list.begin(); it != info_list.end(); it++)
		{
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				GatherDreamRecordKey key(it->first, it->second[k].index);
				if (m_dirty_mark[key])
				{
					if (!it->second[k].data_info.IsValid())
					{
						if(m_old_state[key]) continue;

						it->second[k].change_state = structcommon::CS_DELETE;
						delete_map[key] = i;
					}
					else
					{
						it->second[k].change_state = m_old_state[key] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
					}
				}
			}
		}
	}
	for (std::map<GatherDreamRecordKey, int>::iterator it = delete_map.begin(); it != delete_map.end(); it++)
	{
		RecordMap & info_list = (it->second == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
		RecodMapIt info_list_it = info_list.find(it->first.role_id);
		if (info_list_it != info_list.end())
		{
			for (RecodIndexMapIt info_it = info_list_it->second.begin(); info_it != info_list_it->second.end(); info_it++)
			{
				if (it->first.index == info_it->index)
				{
					info_list_it->second.erase(info_it);
					break;
				}
			}
		}
	}

}

void GatherDreamRecordManager::ClearDirtyMark(GatherDreamRecordParam * param)
{
	if (!m_change_flag) return;

	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? param->get_data_list : param->gift_data_list;
		for (RecodMapIt it = info_list.begin(); it != info_list.end(); it++)
		{
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				GatherDreamRecordKey key(it->first, it->second[k].index);
				m_old_state[key] = true;
				m_dirty_mark[key] = false;
				it->second[k].change_state = structcommon::CS_NONE;
			}
		}
	}
}

void GatherDreamRecordManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;
	
	unsigned int now_to_next_day_zero_time = static_cast<unsigned int>(EngineAdapter::Instance().NextDayInterval(0, 0, 0));
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unsigned int today_zero_time = now_time - (SECOND_PER_DAY - now_to_next_day_zero_time);
	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
		for (RecodMapIt it = info_list.begin(); it != info_list.end(); it++)
		{
			for (int k = 0; k < (int)it->second.size(); k++)
			{
				//超过七天的记录需要删除 
				if (it->second[k].data_info.timestamp + SECOND_PER_DAY * 6 < today_zero_time)
				{
					GatherDreamRecordKey key(it->first, it->second[k].index);
					m_old_state[key] = false;
					m_dirty_mark[key] = true;
					it->second[k].data_info.Reset();
					m_change_flag = true;
				}
			}
		}
	}
}

void GatherDreamRecordManager::AddRecord(GatherDreamRecordInfo & get_record, GatherDreamRecordInfo & gift_record, GuildGatherDreamParam & param)
{
	static GatherDreamRecordParam::GatherDreamRecordSaveEntry get_record_save; get_record_save.Reset();
	static GatherDreamRecordParam::GatherDreamRecordSaveEntry gift_record_save; gift_record_save.Reset();
	get_record_save.data_info = get_record;
	gift_record_save.data_info = gift_record;
	get_record_save.index = param.record_index++;
	//七天记录会删除，所以不用担心index重复
	if (param.record_index >= MAX_INT)
	{
		param.record_index = 0;
	}
	gift_record_save.index = param.record_index++;
	if (param.record_index >= MAX_INT)
	{
		param.record_index = 0;
	}

	//gift_record.role_id为受赠人
	m_param.get_data_list[gift_record.role_id].push_back(get_record_save);
	m_param.gift_data_list[get_record.role_id].push_back(gift_record_save);

	GatherDreamRecordKey get_key(gift_record.role_id, get_record_save.index);
	GatherDreamRecordKey gift_key(get_record.role_id, gift_record_save.index);

	//退出家族后此时是否已删除数据库数据
	if (m_old_state.end() != m_old_state.find(get_key))
	{
		m_old_state[get_key] = m_old_state[get_key] ? false : true;
	}
	else
	{
		m_old_state[get_key] = false;
	}
	if (m_old_state.end() != m_old_state.find(gift_key))
	{
		m_old_state[gift_key] = m_old_state[gift_key] ? false : true;
	}
	else
	{
		m_old_state[gift_key] = false;
	}

	m_dirty_mark[get_key] = true;
	m_dirty_mark[gift_key] = true;
	m_change_flag = true;
}

void GatherDreamRecordManager::UpdateDataInfo(SynGatherDreamRecord info, int uid, int guild_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameGuildGatherDreamRoleChange hgggdrc;
		hgggdrc.role_info = info;
		hgggdrc.role_uid = uid;
		hgggdrc.guild_id = guild_id;
		
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index, (const char *)&hgggdrc, sizeof(hgggdrc));
		return;
	}

	std::set<int> m_update_set;
	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
		RecodMapIt it = info_list.find(uid);
		if (it != info_list.end())
		{
			for (RecodIndexMapIt index_it = it->second.begin(); index_it != it->second.end(); index_it++)
			{
				if (index_it->data_info.IsValid() && m_update_set.end() == m_update_set.find(index_it->data_info.role_id))
				{
					//由玩家自身记录,查询与之相关玩家信息来进行同步
					RecordMap & data_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.gift_data_list : m_param.get_data_list;	//玩家的赠送则对应玩家为受赠，反之亦然
					RecodMapIt data_it = data_list.find(index_it->data_info.role_id);
					if (data_it != data_list.end())
					{
						for (RecodIndexMapIt data_index_it = data_it->second.begin(); data_index_it != data_it->second.end(); data_index_it++)
						{
							if (data_index_it->data_info.role_id == it->first)
							{
								data_index_it->data_info.SetData(info);
								m_dirty_mark[GatherDreamRecordKey(it->first, index_it->index)] = true;
								m_old_state[GatherDreamRecordKey(it->first, index_it->index)] = m_old_state[GatherDreamRecordKey(it->first, index_it->index)] ? true : false;
							}
						}
						m_update_set.insert(index_it->data_info.role_id);
					}

				}
			}
			m_change_flag = true;
		}
		m_update_set.clear();
	}
	Guild * guild = GuildManager::Instance().GetGuild(guild_id);
	if (NULL != guild)
	{
		guild->OnUpdateRoleInfo(info, uid);
	}
}

void GatherDreamRecordManager::LeaveGuild(int uid)
{
	//std::set<int> m_update_set;
	for (int i = 0; i < GATHER_DREAM_INFO_TYPE_MAX; i++)
	{
		RecordMap & info_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
		RecodMapIt it = info_list.find(uid);
		if (it != info_list.end())
		{
			for (RecodIndexMapIt index_it = it->second.begin(); index_it != it->second.end(); index_it++)
			{
				//保留其他玩家与之相关的记录
				/*if (index_it->data_info.IsValid() && m_update_set.end() == m_update_set.find(index_it->data_info.role_id))
				{
					//由玩家自身记录,查询与之相关玩家信息来进行修改
					RecordMap & data_list = (i == GATHER_DREAM_INFO_TYPE_GET) ? m_param.gift_data_list : m_param.get_data_list;
					RecodMapIt data_it = data_list.find(index_it->data_info.role_id);
					if (data_it != data_list.end())
					{
						for (RecodIndexMapIt data_index_it = data_it->second.begin(); data_index_it != data_it->second.end(); data_index_it++)
						{
							if (data_index_it->data_info.role_id == it->first)
							{
								m_dirty_mark[GatherDreamRecordKey(data_it->first, data_index_it->index)] = true;
								m_old_state[GatherDreamRecordKey(data_it->first, data_index_it->index)] = false;
								data_index_it->data_info.Reset();
							}
						}
					}
					m_update_set.insert(index_it->data_info.role_id);
				}*/
				m_dirty_mark[GatherDreamRecordKey(it->first, index_it->index)] = true;
				m_old_state[GatherDreamRecordKey(it->first, index_it->index)] = false;
				index_it->data_info.Reset();
			}
			m_change_flag = true;
		}
		//m_update_set.clear();
	}
}

int GatherDreamRecordManager::GetHelpNum(int role_uid, int gift_role_uid)
{
	if(role_uid == gift_role_uid) return 0;

	int help_num = 0;
	RecodMapIt it = m_param.get_data_list.find(role_uid);
	if (it != m_param.get_data_list.end())
	{
		for (RecodIndexMapIt index_it = it->second.begin(); index_it != it->second.end(); index_it++)
		{
			if (gift_role_uid == index_it->data_info.role_id)
			{
				help_num++;
			}
		}
	}
	return help_num;
}

void GatherDreamRecordManager::SendRecordListInfo(Role * role, int type)
{
	if(NULL == role || type < 0 || type >= GATHER_DREAM_INFO_TYPE_MAX) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameGuildGatherDreamRecordListReq hgggdrlr;
		hgggdrlr.uid = role->GetUID();
		hgggdrlr.type = type;
		InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), (const char*)&hgggdrlr, sizeof(hgggdrlr));
		return;
	}
	RecordMap & data_list = (type == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;	
	Protocol::SCRoleGatherDreamRecordInfoList info;
	info.type = type;
	info.count = 0;
	RecodMapIt it = data_list.find(role->GetUID());
	if (it != data_list.end())
	{
		for (RecordReverseMapIt index_it = it->second.rbegin(); index_it != it->second.rend(); index_it++)
		{
			if (!index_it->data_info.IsValid()) continue;

			info.data_list[info.count++] = index_it->data_info;
			if(info.count >= MAX_GATHER_DREAM_SEND_LIST_NUM) break;
		}
	}

	int length = sizeof(info) - (MAX_GATHER_DREAM_SEND_LIST_NUM - info.count) * sizeof(info.data_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, length);
}

void GatherDreamRecordManager::SendRecordListInfoToCross(int uid, int type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	
	RecordMap & data_list = (type == GATHER_DREAM_INFO_TYPE_GET) ? m_param.get_data_list : m_param.gift_data_list;
	Protocol::SCRoleGatherDreamRecordInfoList info;
	info.type = type;
	info.count = 0;
	RecodMapIt it = data_list.find(uid);
	if (it != data_list.end())
	{
		for (RecordReverseMapIt index_it = it->second.rbegin(); index_it != it->second.rend(); index_it++)
		{
			if (!index_it->data_info.IsValid()) continue;

			info.data_list[info.count++] = index_it->data_info;
			if (info.count >= MAX_GATHER_DREAM_SEND_LIST_NUM) break;
		}
	}

	int length = sizeof(info) - (MAX_GATHER_DREAM_SEND_LIST_NUM - info.count) * sizeof(info.data_list[0]);
	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char *)&info, length);
}


GatherDreamRecordManager::GatherDreamRecordManager() : m_gather_dream_record_next_save_time(0LL), m_change_flag(false), m_load_flag(false)
{
	m_old_state.clear();
	m_dirty_mark.clear();
}

GatherDreamRecordManager::~GatherDreamRecordManager()
{
}

void GatherDreamRecordManager::Save(GatherDreamRecordParam * param)
{
	if(!this->IsLoadFinish()) return;

	static GatherDreamRecordParam gather_dream_record_param;
	if (NULL != param)
	{
		gather_dream_record_param = *param;
	}
	this->GetInitParam(&gather_dream_record_param);

	RMISaveGatherDreamRecordBackObjectImpl *backobj = new RMISaveGatherDreamRecordBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveGatherDreamRecordDataAsyn(gather_dream_record_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&gather_dream_record_param);
	}

}

void GatherDreamRecordManager::LoadGatherDreamDataSucc()
{
	if (this->IsLoadFinish())
	{
		m_gather_dream_record_next_save_time = EngineAdapter::Instance().Time() + GATHER_DREAM_RECORD_SAVE_TO_DB_TIME;
	}
}

void RMIInitGatherDreamRecordBackObjectImpl::InitGatherDreamRecordDataRet(int ret, const GatherDreamRecordParam & list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		GatherDreamRecordManager &gather_dream_record_manager = GatherDreamRecordManager::Instance();
		if (NULL == &gather_dream_record_manager) return;

		gather_dream_record_manager.Init(list_param);
		gather_dream_record_manager.LoadGatherDreamDataSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}
