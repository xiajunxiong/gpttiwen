#include "randactivitygodtoken.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/usercache/usercache.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "obj/character/role.h"
#include "global/randactivity/randactivitymanager.hpp"

RandActivityGodToken::RandActivityGodToken(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_GOD_TOKEN)
{
}

RandActivityGodToken::~RandActivityGodToken()
{
}

void RandActivityGodToken::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);

	if (to_status == ACTIVITY_STATUS_CLOSE)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
				if (NULL != god_token)
				{
					god_token->OnRAGodTokenPatchLevelReward();
					god_token->OnRAGodTokenReclaimItem();
				}
			}
		}
		else
		{
			std::vector<int> online_user_list;
			UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
			for (int i = 0; i < (int)online_user_list.size(); i++)
			{
				Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
				if (NULL == role) continue;

				RoleCross* rolecross = role->GetRoleModuleManager()->GetRoleCross();
				UniqueServerID usid = UniqueServerID(rolecross->GetOriginPlatType(), rolecross->GetOriginServerId());
				if (usid != m_rand_activity_manager->GetUniqueServerID())
				{
					continue;
				}

				RoleActivityGodToken * god_token = static_cast<RoleActivityGodToken *>
					(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_TOKEN));
				if (NULL != god_token)
				{
					god_token->OnRAGodTokenPatchLevelReward();
					god_token->OnRAGodTokenReclaimItem();
				}
			}
		}
	}
}
