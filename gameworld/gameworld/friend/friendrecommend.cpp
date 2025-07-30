#include "friend/friendmanager.h"
#include "friendrecommend.hpp"
#include "gameworld/global/usercache/usercache.hpp"
#include "gameworld/engineadapter.h"
#include "servercommon/servercommon.h"
#include "gameworld/obj/character/role.h"
#include "gameworld/world.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "protocol/msgfriend.h"
#include "friend/friendmanager.h"
#include "other/rolemodulemanager.hpp"

FriendRecommend::FriendRecommend() : m_role_module_mgr(NULL), m_is_get_recommend_list(false), m_recommend_uid_count(0), m_next_refresh_friend_timestamp(0)
{
	memset(m_recommend_uid_list, 0, sizeof(m_recommend_uid_list));
}

FriendRecommend::~FriendRecommend()
{

}

void FriendRecommend::Init(RoleModuleManager * role_module_manager)
{
	m_role_module_mgr = role_module_manager;
}

void FriendRecommend::GetInitParam(FriendRecommendParam* param)
{
	*param = m_param;
}

//发送数据
void FriendRecommend::SendFriendRecommend()
{
	if (!m_is_get_recommend_list)
	{
		m_is_get_recommend_list = true;
		this->RefreshFriendRecommendList();
	}

	static Protocol::SCFriendRecommendRoute frlr;

	for (int i = 0; i < FriendRecommendParam::RECOMMEND_LIST_MAX; ++i)
	{
		frlr.friend_recommend_list[i] = m_param.recommend_list[i];
	}

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&frlr, sizeof(frlr));
}

bool FriendRecommend::IsCanRefresh(int type_flag)
{
	if (LAST_USE_TYPE_FLAG_IS_ADD_ALL == type_flag && LAST_USE_TYPE_FLAG_IS_ADD_ALL != m_last_use_type_flag)
	{
		return true;
	}
	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (0 != m_next_refresh_friend_timestamp && m_next_refresh_friend_timestamp > now_second)
	{
		return false;
	}
	return true;
}

void FriendRecommend::SetNextRefreshTimestamp(int type_flag)
{
	if(LAST_USE_TYPE_FLAG_BEGIN > type_flag || type_flag >= LAST_USE_TYPE_FLAG_MAX) return;

	m_last_use_type_flag = type_flag;
	m_next_refresh_friend_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + MAX_REFRESH_TIME_NUM;
}

bool FriendRecommend::IsLastTimeRefreshUid(int uid)
{
	for (int i = 0; i < m_recommend_uid_count && i < FriendRecommendParam::RECOMMEND_LIST_MAX; i++)
	{
		if (uid == m_recommend_uid_list[i])
		{
			return true;
		}
	}
	return false;
}

//清空数据
void FriendRecommend::ClearData()
{
	m_param.Reset();
	int user_size = UserCacheManager::Instance().CountOfUser();
	if (user_size > 0)
	{
		m_param.all_status_offset = rand() % user_size;
	}
}

//列表添加好友
void FriendRecommend::AddAllRecommendFriend()
{	
	if (m_recommend_uid_count <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FRIEND_NOT_HAS_OTHER_FRIEND);
		return;
	}

	if (this->IsCanRefresh(LAST_USE_TYPE_FLAG_IS_ADD_ALL))
	{
		for (int i = 0; i < m_recommend_uid_count && i < FriendRecommendParam::RECOMMEND_LIST_MAX; ++i)
		{
			FriendManager::Instance().AddFriendReq(m_role_module_mgr->GetRole(), m_param.recommend_list[i].user_uid, true);
			m_param.recommend_list[i].Reset();
		}
		this->RefreshFriendRecommendList();
		this->SetNextRefreshTimestamp(LAST_USE_TYPE_FLAG_IS_ADD_ALL);
		this->SendFriendRecommend();	
		m_role_module_mgr->NoticeNum(noticenum::NT_SEND_ADD_ALL_FRIEND_REQ_SUCC);
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FRIEND_NOT_REFRESH);
	}
}

void FriendRecommend::AddOneRecommendFriend(int index)
{
	if (index < 0 || index >= FriendRecommendParam::RECOMMEND_LIST_MAX)
	{
		return;
	}

	FriendManager::Instance().AddFriendReq(m_role_module_mgr->GetRole(), m_param.recommend_list[index].user_uid, false);
	m_param.recommend_list[index].Reset();
}

//筛选推荐列表
bool FriendRecommend::RefreshFriendRecommendList(bool is_notice)
{
	int role_id = m_role_module_mgr->GetRole()->GetUID();

	memset(m_param.recommend_list, 0, sizeof(m_param.recommend_list));

	static int online_userid_list[FriendRecommendParam::RECOMMEND_LIST_MAX];
	int count = UserCacheManager::Instance().GetUidListByStatusMap(m_param.online_offset, FriendRecommendParam::RECOMMEND_LIST_MAX, online_userid_list, UserStatusNode::STATUS_TYPE_ONLINE, m_role_module_mgr->GetRole());

	memset(m_recommend_uid_list, 0, sizeof(m_recommend_uid_list));
	int recommend_num = 0;
	for (int i = 0; i < count && recommend_num < FriendRecommendParam::RECOMMEND_LIST_MAX; ++i)
	{
		int target_id = online_userid_list[i];
		if (role_id == target_id)
		{
			continue;
		}

		UserCacheNode* target_node_ptr = UserCacheManager::Instance().GetUserNode(target_id);
		if (NULL == target_node_ptr)
		{
			continue;
		}

		m_param.recommend_list[recommend_num].user_uid = target_id;
		target_node_ptr->GetName(m_param.recommend_list[recommend_num].user_name);
		m_param.recommend_list[recommend_num].user_level = target_node_ptr->level;
		m_param.recommend_list[recommend_num].reserve_ch = 0;
		m_param.recommend_list[recommend_num].user_avatar_timestamp = target_node_ptr->avatar_timestamp;
		m_param.recommend_list[recommend_num].user_avatar_type = target_node_ptr->avatar_type;
		m_param.recommend_list[recommend_num].user_prof = target_node_ptr->profession;
		m_param.recommend_list[recommend_num].user_headshot_id = target_node_ptr->appearance.headshot_id;
		m_param.recommend_list[recommend_num].reserve_sh = 0;
		m_param.recommend_list[recommend_num].top_level = target_node_ptr->top_level;
		m_recommend_uid_list[recommend_num] = target_id;

		recommend_num += 1;
	}
	m_recommend_uid_count = recommend_num;

	if (is_notice && 0 == recommend_num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FRIEND_NOT_HAS_OTHER_FRIEND);
	}

	if (m_param.all_status_offset >= UserCacheManager::Instance().CountOfUser())
	{
		m_param.all_status_offset = 0;
	}
	if (m_param.online_offset >= UserCacheManager::Instance().CountOfUser())
	{
		m_param.online_offset = 0;
	}

	return true;
}

void FriendRecommend::RefreshOneFriendRecommend(int index)
{
	if (index < 0 || index >= FriendRecommendParam::RECOMMEND_LIST_MAX)
	{
		return;
	}

	int role_id = m_role_module_mgr->GetRole()->GetUID();

	int online_userid_list =  0 ;
	UserCacheManager::Instance().GetUidListByStatusMap(m_param.online_offset, 1, &online_userid_list, UserStatusNode::STATUS_TYPE_ONLINE, m_role_module_mgr->GetRole());
	
	if (online_userid_list == role_id)
	{
		UserCacheManager::Instance().GetUidListByStatusMap(m_param.online_offset, 1, &online_userid_list, UserStatusNode::STATUS_TYPE_ONLINE, m_role_module_mgr->GetRole());
	}

	m_param.recommend_list[index].user_uid = online_userid_list;

	UserCacheNode* target_node_ptr = UserCacheManager::Instance().GetUserNode(online_userid_list);
	if (NULL == target_node_ptr)
	{
		return;
	}

	m_param.recommend_list[index].user_uid = online_userid_list;
	target_node_ptr->GetName(m_param.recommend_list[index].user_name);
	m_param.recommend_list[index].user_level = target_node_ptr->level;
	m_param.recommend_list[index].reserve_ch = 0;
	m_param.recommend_list[index].user_avatar_timestamp = target_node_ptr->avatar_timestamp;
	m_param.recommend_list[index].user_avatar_type = target_node_ptr->avatar_type;
	m_param.recommend_list[index].user_prof = target_node_ptr->profession;
	m_param.recommend_list[index].user_headshot_id = target_node_ptr->appearance.headshot_id;
	m_param.recommend_list[index].top_level = target_node_ptr->top_level;
	m_param.recommend_list[index].reserve_sh = 0;

	if (m_param.all_status_offset >= UserCacheManager::Instance().CountOfUser())
	{
		m_param.all_status_offset = 0;
	}
	if (m_param.online_offset >= UserCacheManager::Instance().CountOfUser())
	{
		m_param.online_offset = 0;
	}

	Protocol::SCFriendRecommendOne protocol;
	protocol.friend_recommend = m_param.recommend_list[index];
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

