#include "marriageseekingmanager.h"

#include "global/offlineregister/offlineregistermanager.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/noticenum.h"
#include "other/marriage/marriage.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "global/usercache/usercache.hpp"
#include "protocol/msgmarry.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/marriage/marriageconfig.hpp"
#include "item/money.h"

MarriageSeekingManager::MarriageSeekingManager() 
{
}

MarriageSeekingManager::~MarriageSeekingManager()
{

}

MarriageSeekingManager & MarriageSeekingManager::Instance()
{
	static MarriageSeekingManager _instance; 
	return _instance;
}

void MarriageSeekingList::InsertSeeking(Role *role, MarriageSeekingNotice seeking_notice)
{
	if (role == NULL)
	{
		return;
	}

	MarriageSeeking seeking ;
	seeking.uid = role->GetUID();
	F_STRNCPY(seeking.seeking_notice, seeking_notice, sizeof(seeking.seeking_notice));
	seeking.timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_seeking_list[role->GetUID()] = seeking;

	if (static_cast<int>(m_seeking_list.size()) > MARRIAGE_SEEKING_MAX_COUNT)
	{
		unsigned int min_timestamp = MAX_UINT32;
		int role_id = 0;
		for (std::map<int, MarriageSeeking>::iterator iter = m_seeking_list.begin(); iter != m_seeking_list.end(); ++iter)
		{
			if (iter->second.timestamp < min_timestamp)
			{
				min_timestamp = iter->second.timestamp;
				role_id = iter->first;
			}
		}

		m_seeking_list.erase(role_id);
	}
}

void MarriageSeekingList::RemoveSeeking(int uid)
{
	m_seeking_list.erase(uid);
}

void MarriageSeekingManager::MarriageSeekingOperateType(Role *role, int type, MarriageSeekingNotice marriage_seeking_notice)
{
	if (NULL == role)
	{
		return;
	}

	switch (type)
	{
	case MARRIAGE_SEEKING_TYPE_INFO:
		{
			this->SendMarriageSeekingNoticeList(role);
		}
		break;

	case MARRIAGE_SEEKING_TYPE_INSERT:
		{
			this->InsertMarriageSeekingNotice(role,marriage_seeking_notice);
		}
		break;
	}
}

void MarriageSeekingManager::InsertMarriageSeekingNotice(Role *role, MarriageSeekingNotice seeking_notice)
{	
	if (NULL == role)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetMarriage()->IsMarried())  // 结婚人的不能再征婚
	{
		role->NoticeNum(errornum::EN_MARRIAGE_SEEKING_MARRIED);
		return;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(LOGIC_CONFIG->GetMarriageConfig()->GetOtherCfg().marriage_seek_need_coin, __FUNCTION__))
	{
		return;
	}

	MarriageSeekingList *seeklist = &m_girls_seeking_list;
	
	int sex = role->GetSex();
	if (MALE == sex)
	{
		seeklist = &m_boys_seeking_list;
	}

	seeklist->InsertSeeking(role, seeking_notice);
	
	role->NoticeNum(errornum::EN_MARRIAGE_SEEKING_SUCCESS);


	this->SendMarriageSeekingNoticeList(role);
}

void MarriageSeekingManager::SendMarriageSeekingNoticeList(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	if (role->GetRoleModuleManager()->GetMarriage()->IsMarried()) // 结婚人的不能再征婚
	{
		return;
	}

	MarriageSeekingList *seeklist = &m_girls_seeking_list;
	if (FEMALE == role->GetSex())
	{
		seeklist = &m_boys_seeking_list;
	}
	
	static Protocol::SCMarriageSeekingInfo msi;

	int count = 0;

	for (int i = 0; i < (int)seeklist->m_seeking_list.size() && count < MARRIAGE_SEEKING_MAX_COUNT; ++ i)
	{
		if (0 == seeklist->m_seeking_list[i].uid) continue;

		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(seeklist->m_seeking_list[i].uid);
		if (NULL == user_cache_node) continue;

		bool is_online = UserCacheManager::Instance().IsUserOnline(seeklist->m_seeking_list[i].uid);
		
		msi.marriage_seeking_list[count].user_id = user_cache_node->uid;
		msi.marriage_seeking_list[count].sex = user_cache_node->GetSex();
		msi.marriage_seeking_list[count].prof = user_cache_node->profession;
		msi.marriage_seeking_list[count].is_online = is_online;
		msi.marriage_seeking_list[count].level = user_cache_node->level;
		msi.marriage_seeking_list[count].timestamp = seeklist->m_seeking_list[i].timestamp;
		msi.marriage_seeking_list[count].m_avator_timestamp = 0;
		msi.marriage_seeking_list[count].avator_frame_id = 0;

		F_STRNCPY(msi.marriage_seeking_list[count].gamename, user_cache_node->role_name, sizeof(msi.marriage_seeking_list[0].gamename));
		memset(msi.marriage_seeking_list[count].guildname, 0, sizeof(msi.marriage_seeking_list[count].guildname));
		F_STRNCPY(msi.marriage_seeking_list[count].marriage_seeking_notice, seeklist->m_seeking_list[i].seeking_notice, sizeof(msi.marriage_seeking_list[0].marriage_seeking_notice));

		++count;
	}

	msi.count = count;

	int sendlen = sizeof(Protocol::SCMarriageSeekingInfo) - (MARRIAGE_SEEKING_MAX_COUNT - count) * sizeof(Protocol::MarriageSeekingInfo);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (char *)&msi, sendlen);
}

void MarriageSeekingManager::OnChangeSex(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	MarriageSeekingList *handle_list = NULL;
	if (FEMALE == role->GetSex())
	{
		handle_list = &m_boys_seeking_list;
	}
	else
	{
		handle_list = &m_girls_seeking_list;
	}

	if (NULL != handle_list)
	{
		handle_list->RemoveSeeking(role->GetUID());
	}
}

void MarriageSeekingManager::OnMarry(Role *role)
{
	if (NULL == role)
	{
		return;
	}

	MarriageSeekingList *handle_list = NULL;
	if (FEMALE == role->GetSex())
	{
		handle_list = &m_girls_seeking_list;
	}
	else
	{
		handle_list = &m_boys_seeking_list;
	}

	if (NULL != handle_list)
	{
		handle_list->RemoveSeeking(role->GetUID());
	}
}




