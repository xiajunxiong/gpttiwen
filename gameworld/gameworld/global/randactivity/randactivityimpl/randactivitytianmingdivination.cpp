#include "randactivitytianmingdivination.hpp"
#include "obj/character/role.h"
#include "gameworld/protocol/randactivity/msgratianmingdivination.hpp"
#include "other/rolemodulemanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "gameworld/internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/localconfig.hpp"


RandActivityTianMingDivination::RandActivityTianMingDivination(RandActivityManager *rand_activity_manager) :
RandActivity(rand_activity_manager, RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION)
{

}

RandActivityTianMingDivination::~RandActivityTianMingDivination()
{

}

void RandActivityTianMingDivination::OnItemAdd(TianMingDivinationInfo * info)
{
	if (NULL == info)return;
	rate_info_vec.push_back(*info);
}

void RandActivityTianMingDivination::OnRoleGetRateItem(Role * role,const ItemConfigData * item)
{
	if (NULL == role)return;
	TianMingDivinationInfo node;
	node.uid = role->GetUID();
	F_STRNCPY(node.role_name, role->GetName(), sizeof(GameName));
	node.rate_item_id = item->item_id;
	node.item_num = item->num;
	rate_info_vec.push_back(node);
	this->SendTianMingPoolInfo(role);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		static crossgameprotocal::CrossGameSyncRATianMing msg;
		msg.info = node;
		msg.info_from_server_id = role->GetUniqueServerID();
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), role->GetUserId().db_index, (const char*)&msg, sizeof(msg));
	}
	else
	{
		static crossgameprotocal::GameCrossSyncRATianMing msg;
		msg.info = node;
		msg.info_from_server_id = role->GetUniqueServerID();
		InternalComm::Instance().SendToHiddenThroughCross((const char*)&msg, sizeof(msg));
	}
}

void RandActivityTianMingDivination::SendTianMingPoolInfo(Role * role)
{
	Protocol::SCTianMingRewardPoolInfo msg;
	msg.pool_num = 0;
	for (std::vector<TianMingDivinationInfo>::reverse_iterator it = rate_info_vec.rbegin(); it != rate_info_vec.rend() &&
		msg.pool_num < Protocol::SCTianMingRewardPoolInfo::MAX_POOL_NUM;it++)
	{
		msg.pool_info[msg.pool_num] = *it;
		msg.pool_num += 1;
	}
	role->GetRoleModuleManager()->NetSend((const char*)&msg, sizeof(msg) - (Protocol::SCTianMingRewardPoolInfo::MAX_POOL_NUM - msg.pool_num) * sizeof(TianMingDivinationInfo));
}
