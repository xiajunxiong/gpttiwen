#include "rolegatherdream.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "global/guild/guild.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "protocol/msgother.h"
#include "other/collection/collectionconfig.hpp"
#include "other/collection/collection.hpp"

RoleGatherDream::RoleGatherDream()
{
}

RoleGatherDream::~RoleGatherDream()
{
}

void RoleGatherDream::Init(RoleModuleManager * role_module_manager, const GatherDreamParam & param)
{
	m_module_mgr = role_module_manager;
	m_param = param;
}

void RoleGatherDream::GetInitParam(GatherDreamParam * param)
{
	*param = m_param;
}

void RoleGatherDream::OnRoleLogin()
{
	const std::vector<CollectionActivationCfg>* cfg = LOGIC_CONFIG->GetCollectionConfig()->GetCollectionByType(CollectionConfig::COLLECTION_TYPE_PET);
	if (NULL != cfg)
	{
		for (int i = 0; i < (int)cfg->size(); i++)
		{
			if (m_module_mgr->GetCollection()->IsActiveCollection((*cfg)[i].seq))
			{
				m_pet_id_map[((*cfg)[i].id)] = true;
			}
		}
	}
	this->SendCanPublishList();
}

void RoleGatherDream::OndayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	m_param.today_is_publish_gather_dream_flag = 0;
	memset(m_param.gift_role_uid_list, 0, sizeof(m_param.gift_role_uid_list));
	m_param.count = 0;
	m_gift_role_uid_map.clear();

	//下发协议
	this->SendGatherDreamList();
}

void RoleGatherDream::OnAddGiftUid(int uid)
{
	if(m_module_mgr->GetUid() == uid || m_param.count < 0 || m_param.count >= MAX_GATHER_DREAM_GIFT_NUM) return;

	m_gift_role_uid_map[uid] = m_param.count;
	m_param.gift_role_uid_list[m_param.count++] = uid;
	this->SendGatherDreamList();
}

void RoleGatherDream::OnGetPet(int pet_id)
{
	std::map<int, bool>::iterator it = m_pet_id_map.find(pet_id);
	if (it == m_pet_id_map.end())
	{
		m_pet_id_map[pet_id] = true;
		this->SendCanPublishList();
	}
}

bool RoleGatherDream::IsHaveGiftNum()
{
	int max_count = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->pray_give_maxnum >= MAX_GATHER_DREAM_GIFT_NUM ? MAX_GATHER_DREAM_GIFT_NUM : LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg()->pray_give_maxnum;

	return m_param.count >= max_count ? false : true;
}

bool RoleGatherDream::IsCanTodayGiftUser(int uid)
{
	std::map<int, int>::iterator it = m_gift_role_uid_map.find(uid);
	if(it != m_gift_role_uid_map.end()) return false;

	return true;
}

void RoleGatherDream::SetPublishSendFlag(int is_send)
{
	m_param.gather_dream_publish_is_send_flag = (is_send == 0) ? 0 : 1;
	this->SendCanPublishList();
}

void RoleGatherDream::SetTodayPublishFlag(bool is_publish)
{
	m_param.today_is_publish_gather_dream_flag = is_publish ? 1 : 0;
	this->SendGatherDreamList();
}

void RoleGatherDream::SendGatherDreamList()
{
	Protocol::SCRoleGiftGatherDreamList info;
	info.today_is_publish_gather_dream_flag = m_param.today_is_publish_gather_dream_flag;
	info.count = m_param.count;
	memcpy(info.gift_uild_list, m_param.gift_role_uid_list, sizeof(info.gift_uild_list));

	int length = sizeof(info) - (MAX_GATHER_DREAM_GIFT_NUM - info.count) * sizeof(info.gift_uild_list[0]);
	m_module_mgr->NetSend(&info, length);
}

void RoleGatherDream::SendCanPublishList()
{
	Protocol::SCRoleCanPublishGahterDreamItemList info;
	info.publish_is_send_flag = m_param.gather_dream_publish_is_send_flag;
	info.count = 0;
	for (std::map<int, bool>::iterator it = m_pet_id_map.begin(); it != m_pet_id_map.end(); it++)
	{
		info.pet_id_list[info.count++] = it->first;
		if(info.count >= PET_COUNT_MAX)	break;
	}

	int length = sizeof(info) - (PET_COUNT_MAX - info.count) * sizeof(info.pet_id_list[0]);
	m_module_mgr->NetSend(&info, length);
}
