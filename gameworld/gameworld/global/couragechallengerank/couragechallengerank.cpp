#include "couragechallengerank.hpp"
#include "engineadapter.h"
#include "gameworld/internalcomm.h"

#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/struct/structcommon.h"
#include "obj/character/role.h"
#include "global/rmibackobjdef.h"
#include "global/usercache/usercache.hpp"
#include "gamelog.h"
#include "protocol/msgcouragechallenge.h"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"

CourageChallengeRankManager & CourageChallengeRankManager::Instance()
{
	static CourageChallengeRankManager courage_challenge_rank_manager;
	return courage_challenge_rank_manager; 
}

void CourageChallengeRankManager::Update(unsigned int now_second)
{
	if (now_second >= m_next_save_db_timestamp && this->IsDirty())
	{
		this->UpdataRankList();
		this->Save();
		m_next_save_db_timestamp = now_second + COURAGE_CHALLENGE_SAVE_DB_TIME;
	}
}

void CourageChallengeRankManager::OnServerStart()
{
	this->LoadCourageChallengeRankData(0);
}

void CourageChallengeRankManager::OnServerStop()
{
	this->Save();
}

bool CourageChallengeRankManager::LoadCourageChallengeRankData(long long id_from)
{
	RMIInitCourageChallengeRankBackObjectImpl * impl = new RMIInitCourageChallengeRankBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitCourageChallengeRankAsyn(id_from, impl);
}

void CourageChallengeRankManager::Init(const CourageChallengeRankParam & param)
{
	for (std::map<int, CourageChallengeRankItem>::const_iterator it = param.data_list.begin(); it != param.data_list.end(); it++)
	{
		if (it->first <= 0 || it->first != it->second.uid)
		{
			gamelog::g_Log_courage_challenge_rank.printf(LL_CRITICAL, "[%s] uid[%d] != it->first[%d]", __FUNCTION__, it->second.uid, it->first);
			continue;
		}
		m_rank_param.data_list[it->first] = it->second;
		m_dirty_mark[it->first] = false;
		m_old_state[it->first] = true;
	}
	
	//此时可能由于合服而出现多个相同排行数据
	bool is_same_data = false;
	int max_pos = 0;
	for (std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.begin(); it != m_rank_param.data_list.end(); it++)
	{
		if (it->second.pos <= 0 || it->second.pos > MAX_COURAGE_CHALLENGE_RANK_NUM) continue;

		if (m_rank_list[it->second.pos - 1].IsValid())
		{
			is_same_data = true;
			break;
		}

		m_rank_list[it->second.pos - 1] = it->second;
//		m_user_index_map[it->first] = it->second.pos - 1;
		max_pos = max_pos < it->second.pos ? it->second.pos : max_pos;
	}
	//检测排序是否存在空位
	if (!is_same_data)
	{
		for (int i = 0; i < max_pos && i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
		{
			if (!m_rank_list[i].IsValid())
			{
				is_same_data = true;
				break;
			}
		}
	}

	//出现相同排行数据则需要重新排序
	if (is_same_data)
	{
		this->UpdataRankList();
		m_change_flag = true;
	}

	m_load_flag = true;

	//初始化后保存一份数据,防止之后删除时没有数据可用
	this->Save(&m_rank_param);
}

void CourageChallengeRankManager::GetInitParam(CourageChallengeRankParam * param)
{
	if (!m_load_flag || !this->IsDirty())
	{
		return;
	}

	*param = m_rank_param;

	std::set<int> delete_set;
	for (std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.begin(); it != m_rank_param.data_list.end(); it++)
	{
		if (m_dirty_mark[it->first])
		{
			if (!it->second.IsValid())
			{
				if(m_old_state[it->first]) continue;
				
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
		m_rank_param.data_list.erase(*it);
	}

}

void CourageChallengeRankManager::ClearDirtyMark(CourageChallengeRankParam * param)
{
	if(!m_change_flag) return;

	for (std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.begin(); it != m_rank_param.data_list.end(); it++)
	{
		m_old_state[it->first] = true;
		m_dirty_mark[it->first] = false;
		it->second.change_state = structcommon::CS_NONE;
		param->data_list[it->first].change_state = structcommon::CS_NONE;
	}
	m_change_flag = false;
}

void CourageChallengeRankManager::RoleInfoChange(SynCourageChallengeRankInfo * role_info)
{
	if(NULL == role_info || role_info->uid <= 0) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		static crossgameprotocal::HiddenGameCourageChallengeRankRoleChange hgccrrc;
		hgccrrc.role_info = *role_info;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgccrrc.role_info.uid).db_index, (const char*)&hgccrrc, sizeof(hgccrrc));
		return;
	}

	std::map<int, CourageChallengeRankItem>::iterator item_it = m_rank_param.data_list.find(role_info->uid);
	if (item_it == m_rank_param.data_list.end())
	{
		m_rank_param.data_list[role_info->uid].SetData(*role_info);
		m_dirty_mark[role_info->uid] = true;
		m_old_state[role_info->uid] = false;
	}
	else
	{
		item_it->second.SetData(*role_info);
		m_dirty_mark[role_info->uid] = true;
		m_old_state[role_info->uid] = m_old_state[role_info->uid] ? true : false;
	}

	m_change_flag = true;

	//策划说没有打过的玩家不要上本服排行榜
	/*if (role_info->star <= 0)
	{
		m_change_flag = true;
		return;
	}*/

	/*int pos = 0;
	UidIndexMapIt it = m_user_index_map.find(role_info->uid);
	if (it != m_user_index_map.end())
	{
		if (it->second < 0 || it->second >= MAX_COURAGE_CHALLENGE_RANK_NUM)
		{
			this->UpdataRankList();	//映射错乱，重新排序，此时迭代器已失效
		}
		else
		{
			m_rank_list[it->second] = m_rank_param.data_list[role_info->uid];
			pos = it->second + 1;
		}
	}
	int conform_pos = this->GetConformRoleRankPos(*role_info, pos);
	if (conform_pos > 0 && conform_pos <= MAX_COURAGE_CHALLENGE_RANK_NUM)
	{
		this->UpdataRankList(role_info, conform_pos);
	}*/
}

const CourageChallengeRankItem * CourageChallengeRankManager::GetRankItem(int role_id)
{
	std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.find(role_id);
	if (it == m_rank_param.data_list.end())
	{
		return NULL;
	}
	return &it->second;
}

void CourageChallengeRankManager::OnGetRankUidList(std::vector<int>& rank_uid_list)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	for (int i = 0; i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
	{
		if (!m_rank_list[i].IsValid()) break;

		rank_uid_list.push_back(m_rank_list[i].uid);
	}
}

void CourageChallengeRankManager::GameHiddenRankList(crossgameprotocal::HiddenGameCourageChallengeRankReq * req)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	Protocol::SCCourageChallengeRankListInfo info;
	info.count = 0;
	switch (req->rank_type)
	{
		case COURAGE_CHALLENGE_RANK_TYPE_SERVER:
		{
			for (int i = 0; i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
			{
				if(!m_rank_list[i].IsValid()) break;

				info.rank_list[info.count] = m_rank_list[i];
				this->OnGetInfo(info.rank_list[info.count]);
				info.count++;
			}
		}
		break;
		case COURAGE_CHALLENGE_RANK_TYPE_GUILD:
		case COURAGE_CHALLENGE_RANK_TYPE_FRIEND:
		{
			if (req->count < 0 || req->count > MAX_COURAGE_CHALLENGE_RANK_NUM) return;
			for (int i = 0; i < req->count; i++)
			{
				std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.find(req->role_uid_list[i]);
				if(it == m_rank_param.data_list.end()) continue;

				info.rank_list[info.count] = it->second;
				this->OnGetInfo(info.rank_list[info.count]);
				info.count++;
			}
		}
		break;
		default:
		return;
	}
	
	info.rank_type = req->rank_type;

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&info, req->m_uuid);
}

void CourageChallengeRankManager::SendRankListInfo(Role * role, int rank_type, int count, int *role_uid_list)
{
	if(NULL == role || count < 0 || count > MAX_COURAGE_CHALLENGE_RANK_NUM) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if(rank_type < 0 || rank_type >= COURAGE_CHALLENGE_RANK_TYPE_MAX) return;
		static crossgameprotocal::HiddenGameCourageChallengeRankReq hgccrr;
		hgccrr.m_uuid = role->GetRoleModuleManager()->GetRoleCross()->GetOriginUUID();
		hgccrr.rank_type = rank_type;
		hgccrr.count = count;
		memcpy(hgccrr.role_uid_list, role_uid_list, sizeof(hgccrr.role_uid_list[0]) * count);

		int length = sizeof(hgccrr) - (MAX_COURAGE_CHALLENGE_RANK_NUM - hgccrr.count) * sizeof(hgccrr.role_uid_list[0]);
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index, (const char*)&hgccrr, length);
		return;
	}


	Protocol::SCCourageChallengeRankListInfo info;
	info.count = 0;
	switch (rank_type)
	{
	case COURAGE_CHALLENGE_RANK_TYPE_SERVER:
	{
		for (int i = 0; i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
		{
			if (!m_rank_list[i].IsValid()) break;

			info.rank_list[info.count] = m_rank_list[i];
			this->OnGetInfo(info.rank_list[info.count]);
			info.count++;
		}
	}
	break;
	case COURAGE_CHALLENGE_RANK_TYPE_GUILD:
	case COURAGE_CHALLENGE_RANK_TYPE_FRIEND:
	{
		for (int i = 0; i < count && i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
		{
			std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.find(role_uid_list[i]);
			if (it == m_rank_param.data_list.end()) continue;

			info.rank_list[info.count] = it->second;
			this->OnGetInfo(info.rank_list[info.count]);
			info.count++;
		}
	}
	break;
	default:
		return;
	}

	info.rank_type = rank_type;

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);
}

CourageChallengeRankManager::CourageChallengeRankManager(): m_change_flag(false), m_load_flag(false), m_next_save_db_timestamp(0)
{
	//m_user_index_map.clear();
	m_dirty_mark.clear();
	m_old_state.clear();
}

CourageChallengeRankManager::~CourageChallengeRankManager()
{
}

void RMIInitCourageChallengeRankBackObjectImpl::InitCourageChallengeRankDataRet(int ret, const CourageChallengeRankParam & list_param)
{
	if (ret < 0)
	{
		printf("%s Failed \n", __FUNCTION__);
	}
	else
	{
		CourageChallengeRankManager & courage_challenge_rank_manager = CourageChallengeRankManager::Instance();
		if (NULL == &courage_challenge_rank_manager) return;

		courage_challenge_rank_manager.Init(list_param);
		courage_challenge_rank_manager.LoadCourageChallengeRankSucc();
		printf("%s:%d\n", __FUNCTION__,ret);
	}
	
}

bool SortRank(const CourageChallengeRankItem & a, const CourageChallengeRankItem & b)
{
	if (a.star != b.star) return a.star > b.star;

	if (a.layer_level != b.layer_level) return a.layer_level > b.layer_level;

	return a.get_timestamp < b.get_timestamp;
}

bool SortRank1(const CourageChallengeRankItem & a, const SynCourageChallengeRankInfo & b)
{
	if(a.star != b.star) return a.star > b.star;

	if(a.layer_level != b.layer_level) return a.layer_level > b.layer_level;

	return a.get_timestamp < b.get_timestamp;
}

bool SortRank2(const std::pair<int, CourageChallengeRankItem> & a, const std::pair<int, CourageChallengeRankItem> & b)
{
	if(a.first == b.first) return true;

	return SortRank(a.second, b.second);
}

/*
int CourageChallengeRankManager::GetConformRoleRankPos(const SynCourageChallengeRankInfo &role_info, int role_pos)
{
	int pos = 0;
	if (role_pos <= 0 || role_pos > MAX_COURAGE_CHALLENGE_RANK_NUM)
	{
		role_pos = MAX_COURAGE_CHALLENGE_RANK_NUM;
	}
	else
	{
		pos = role_pos--;	
	}

	for (int i = role_pos - 1; i >= 0; i--)
	{
		if(!m_rank_list[i].IsValid() || m_rank_list[i].uid == role_info.uid)
		{
			pos = i + 1;
			continue;
		 }
		if (SortRank1(m_rank_list[i], role_info))
		{
			break;	
		}
		pos = i + 1;
	}
	
	return pos;
}

void CourageChallengeRankManager::UpdataRankList(SynCourageChallengeRankInfo* role_info, int conform_role_pos)
{
	//重新排序
	if (conform_role_pos <= 0 || conform_role_pos > MAX_COURAGE_CHALLENGE_RANK_NUM)
	{
		//重置一些相关的数据
		m_user_index_map.clear();
		memset(m_rank_list, 0, sizeof(m_rank_list));
		for (std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.begin(); it != m_rank_param.data_list.end(); it++)
		{
			if(!it->second.IsValid()) continue;

			it->second.pos = 0;
			m_dirty_mark[it->first] = true;
			m_old_state[it->first] = m_old_state[it->first] ? true : false;
		}
		//对数据进行排序
		std::vector<std::pair<int, CourageChallengeRankItem> > m_item_list;
		//m_item_list.insert(m_item_list.begin(), m_rank_param.data_list.begin(), m_rank_param.data_list.end());
		m_item_list.assign(m_rank_param.data_list.begin(), m_rank_param.data_list.end());
		std::sort(m_item_list.begin(), m_item_list.end(), SortRank2);
		for (int i = 0; i < MAX_COURAGE_CHALLENGE_RANK_NUM && i < (int)m_item_list.size(); i++)
		{
			//零星玩家不上榜
			if (!m_item_list[i].second.IsValid() || 0 == m_item_list[i].second.star) break;
			 
			m_rank_list[i] = m_item_list[i].second;
			m_rank_list[i].pos = i + 1;
			m_rank_param.data_list[m_rank_list[i].uid] = m_rank_list[i];
			m_user_index_map[m_rank_list[i].uid] = i;
		}
	}
	else //从玩家所符位置开始操作
	{
		if(role_info == NULL) return;
		CourageChallengeRankItem rank_item = m_rank_list[conform_role_pos - 1];
		CourageChallengeRankItem last_item;
		for (int i = conform_role_pos - 1; i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
		{
			if(!rank_item.IsValid()) break;
			if (i + 1 >= MAX_COURAGE_CHALLENGE_RANK_NUM )
			{	
				m_rank_param.data_list[rank_item.uid].pos = 0;
				m_user_index_map.erase(rank_item.uid);
				m_dirty_mark[rank_item.uid] = true;
				m_old_state[rank_item.uid] = m_old_state[rank_item.uid] ? true : false;
				break;
			}
			//该玩家本身就在排名之中,所以此时后面顺序为正常
			if(rank_item.uid == role_info->uid) break;	
			last_item = rank_item;
			rank_item = m_rank_list[i + 1];
			m_rank_list[i + 1] = last_item;
		}
		m_rank_list[conform_role_pos - 1].SetData(*role_info);
		//修正对应所符排行与映射
		for (int i = conform_role_pos - 1; i < MAX_COURAGE_CHALLENGE_RANK_NUM; i++)
		{
			if(!m_rank_list[i].IsValid()) break;

			m_rank_list[i].pos = i + 1;
			m_user_index_map[m_rank_list[i].uid] = i;
			m_rank_param.data_list[m_rank_list[i].uid] = m_rank_list[i];
			m_dirty_mark[m_rank_list[i].uid] = true;
			m_old_state[m_rank_list[i].uid] = m_old_state[m_rank_list[i].uid] ? true : false;
		}
	}
}*/

void CourageChallengeRankManager::UpdataRankList()
{
	memset(m_rank_list, 0, sizeof(m_rank_list));
	/*for (std::map<int, CourageChallengeRankItem>::iterator it = m_rank_param.data_list.begin(); it != m_rank_param.data_list.end(); it++)
	{
		if (!it->second.IsValid()) continue;

		it->second.pos = 0;
		m_dirty_mark[it->first] = true;
		m_old_state[it->first] = m_old_state[it->first] ? true : false;
	}*/
	//对数据进行排序
	std::vector<std::pair<int, CourageChallengeRankItem> > m_item_list;
	m_item_list.assign(m_rank_param.data_list.begin(), m_rank_param.data_list.end());
	std::sort(m_item_list.begin(), m_item_list.end(), SortRank2);
	//for (int i = 0; i < MAX_COURAGE_CHALLENGE_RANK_NUM && i < (int)m_item_list.size(); i++)
	//{
	//	//零星玩家不上榜
	//	if (!m_item_list[i].second.IsValid() || 0 == m_item_list[i].second.star) break;

	//	m_rank_list[i] = m_item_list[i].second;
	//	m_rank_list[i].pos = i + 1;
	//	m_rank_param.data_list[m_rank_list[i].uid] = m_rank_list[i];
	//}
	//减少此时存储数量
	for (int i = 0; i < (int)m_item_list.size(); i++)
	{
		if (!m_item_list[i].second.IsValid()) break;

		//零星玩家不上榜
		if (i < MAX_COURAGE_CHALLENGE_RANK_NUM && 0 != m_item_list[i].second.star)
		{
			m_rank_list[i] = m_item_list[i].second;
			if (m_rank_list[i].pos != i + 1)
			{
				m_rank_list[i].pos = i + 1;
				m_dirty_mark[m_item_list[i].first] = true;
				m_old_state[m_item_list[i].first] = m_old_state[m_item_list[i].first] ? true : false;
				m_rank_param.data_list[m_item_list[i].first] = m_rank_list[i];
			}
		}
		else
		{
			if (0 != m_item_list[i].second.pos)
			{
				m_item_list[i].second.pos = 0;
				m_dirty_mark[m_item_list[i].first] = true;
				m_old_state[m_item_list[i].first] = m_old_state[m_item_list[i].first] ? true : false;
				m_rank_param.data_list[m_item_list[i].first] = m_item_list[i].second;
			}
		}
	}
}

void CourageChallengeRankManager::Save(const CourageChallengeRankParam *param)
{
	if (!this->IsLoadFinish()) return;

	static CourageChallengeRankParam courage_challenge_rank_param;
	if (NULL != param)
	{
		courage_challenge_rank_param = *param;
	}
	this->GetInitParam(&courage_challenge_rank_param);

	RMISaveCourageChallengeRankBackObjectImpl *backobj = new RMISaveCourageChallengeRankBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveCourageChallengeRankAsyn(courage_challenge_rank_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&courage_challenge_rank_param);
	}
	this->PrintfLog();
}

void CourageChallengeRankManager::LoadCourageChallengeRankSucc()
{
	if (this->IsLoadFinish())
	{
		m_next_save_db_timestamp = (unsigned int)(EngineAdapter::Instance().Time() + COURAGE_CHALLENGE_SAVE_DB_TIME);
	}
}

void CourageChallengeRankManager::OnGetInfo(SynCourageChallengeRankInfo & role_info)
{
	if (role_info.prof <= 0) 
	{
		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_info.uid);
		if (NULL != user_cache_node)
		{
			role_info.prof = user_cache_node->profession;
		}
	}
}

void CourageChallengeRankManager::PrintfLog()
{
	if(CrossConfig::Instance().IsHiddenServer()) return;

	const static int LOG_COURAGE_CHALLENGE_TOP_RANK_USER_NUM = 10;
	static CourageChallengeRankItem top_user_info_list[LOG_COURAGE_CHALLENGE_TOP_RANK_USER_NUM];
	for (int i = 0; i < LOG_COURAGE_CHALLENGE_TOP_RANK_USER_NUM; i++)
	{
		top_user_info_list[i].Reset();
		top_user_info_list[i] = m_rank_list[i];
	}
	UNSTD_STATIC_CHECK(10 == LOG_COURAGE_CHALLENGE_TOP_RANK_USER_NUM);

	gamelog::g_Log_courage_challenge_rank.printf(LL_INFO,
		"%s rank_type:%d uid_star_layer_level_list:"
		"(%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d), (%d, %d, %d) ",
		__FUNCTION__, COURAGE_CHALLENGE_RANK_TYPE_SERVER,
		top_user_info_list[0].uid, top_user_info_list[0].star, top_user_info_list[0].layer_level,
		top_user_info_list[1].uid, top_user_info_list[1].star, top_user_info_list[1].layer_level,
		top_user_info_list[2].uid, top_user_info_list[2].star, top_user_info_list[2].layer_level,
		top_user_info_list[3].uid, top_user_info_list[3].star, top_user_info_list[3].layer_level,
		top_user_info_list[4].uid, top_user_info_list[4].star, top_user_info_list[4].layer_level,
		top_user_info_list[5].uid, top_user_info_list[5].star, top_user_info_list[5].layer_level,
		top_user_info_list[6].uid, top_user_info_list[6].star, top_user_info_list[6].layer_level,
		top_user_info_list[7].uid, top_user_info_list[7].star, top_user_info_list[7].layer_level,
		top_user_info_list[8].uid, top_user_info_list[8].star, top_user_info_list[8].layer_level,
		top_user_info_list[9].uid, top_user_info_list[9].star, top_user_info_list[9].layer_level);

}
