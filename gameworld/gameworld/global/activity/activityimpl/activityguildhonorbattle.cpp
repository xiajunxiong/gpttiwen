#include<algorithm>

#include "world.h"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "global/usercache/usercache.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/guild/guildmanager.hpp"
#include "global/guild/guild.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "item/money.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "battle/battle_manager_local.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/cross/crossmanager.h"
#include "servercommon/errornum.h"

#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"
#include "config/activityconfig/guildhonorbattle/activityguildhonorbattleconfig.hpp"
#include "scene/speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "protocol/msgguildhonorbattle.h"
#include "activityguildhonorbattle.hpp"

ActivityGuildHonorBattle::ActivityGuildHonorBattle(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_GUILD_HONOR_BATTLE)
{

}

ActivityGuildHonorBattle::~ActivityGuildHonorBattle()
{

}

void ActivityGuildHonorBattle::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_OPEN:
			{
				GuildHonorBattleManager::Instance().OnGuildHonorBattleBegin();
			}
			break;

		case ACTIVITY_STATUS_CLOSE:
			{
				this->OnActivityClose();
				GuildHonorBattleManager::Instance().OnGuildHonorBattleClose();
			}
			break;
	}
}

void ActivityGuildHonorBattle::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Activity::Update(interval, now_second);
}

void ActivityGuildHonorBattle::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);
}

void ActivityGuildHonorBattle::GoToBattleScene(Role * role)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
			return;
		}

		role->GetRoleModuleManager()->GetRoleActivityManager()->JoinActivityInCross(ACTIVITY_TYPE_GUILD_HONOR_BATTLE, 0, true);
	}
	else
	{
		GuildHonorBattleManager::Instance().GoToBattleScene(role);
	}
}

void ActivityGuildHonorBattle::GmOperate(int type)
{

}





void ActivityGuildHonorBattle::OnActivityClose()
{
	BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_GUILD_HONOR_BATTLE);
}
