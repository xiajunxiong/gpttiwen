#include "battle_business_local.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "other/event/eventhandler.hpp"
#include "battle/battle_business_local_impl/bbl_impl_common.hpp"
#include "battle/battle_manager_local.hpp"
#include "battle/battle_business_local_impl/bbl_impl_field_boss.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss2.hpp"
#include "battle/battle_business_local_impl/bbl_impl_world_boss3.hpp"
#include "battle/battle_business_local_impl/bbl_impl_platform_fight.hpp"
#include "battle/battle_business_local_impl/bbl_impl_colosseum.hpp"
#include "battle/battle_business_local_impl/bbl_impl_chief_election.hpp"
#include "battle/battle_business_local_impl/bbl_impl_chief_peak_battle.hpp"
#include "battle/battle_business_local_impl/bbl_impl_huan_jie_zhan_chang_tian_ti.hpp"
#include "battle/battle_business_local_impl/bbl_impl_cloud_arena.hpp"
#include "servercommon/errornum.h"

BattleBusinessLocal::BattleBusinessLocal() : m_battle_local_info(NULL)
{

}

BattleBusinessLocal::~BattleBusinessLocal()
{

}

BattleBusinessLocal* BattleBusinessLocal::CreateBBLImpl(int battle_mode)
{
	BattleBusinessLocal* impl = NULL;

	switch (battle_mode)
	{
	case BATTLE_MODE_BIG_DIPPER:
	case BATTLE_MODE_DEMO:
	case BATTLE_MODE_NORMAL_MONSTER:
	case BATTLE_MODE_DEMON_CAVE:
	case BATTLE_MODE_MINE_MONSTER:
	case BATTLE_MODE_COURAGE_CHALLENGE:
	case BATTLE_MODE_GUILD_BOSS:
	case BATTLE_MODE_JIAN_DAN_QIN_XIN:
	case BATTLE_MODE_NEW_COURAGE_CHALLENGE:
	case BATTLE_MODE_TEAM_CHALLENGE_TASK:
		{
			impl = new BBLImplCommon();
		}
		break;

	case BATTLE_MODE_FIELD_BOSS:
		{
			impl = new BBLImplFieldBoss();
		}
		break;

	case BATTLE_MODE_WORLD_BOSS:
		{
			impl = new BBLImplWorldBoss();
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_2:
		{
			impl = new BBLImplWorldBoss2();
		}
		break;
	case BATTLE_MODE_WORLD_BOSS_3:
		{
			impl = new BBLImplWorldBoss3();
		}
		break;

	case BATTLE_MODE_PLATFORM_BATTLE:
		{
			impl = new BBLImplPlatformFight();
		}
		break;

	case BATTLE_MODE_SINGLE_ARENA:
		{
			impl = new BBLImplColosseum();
		}
		break;

	case BATTLE_MODE_CHIEF_ELECTION:
		{
			impl = new BBLImplChiefElection();
		}
		break;
	case BATTLE_MODE_CHIEF_PEAK_BATTLE:
		{
			impl = new BBLImplChiefPeakBattle();
		}
		break;
	case BATTLE_MODE_HUAN_JIE_ZHAN_CHANG_TIAN_TI:
		{
			impl = new BBLImplHuanJieZhanChang();
		}
		break;
	case BATTLE_MODE_CLOUD_ARENA:
		{
			impl = new BBLImplCloudArena();
		}
		break;
	default:
		{
			impl = new BBLImplCommon();
		}
		break;
	}

	return impl;
}

void BattleBusinessLocal::OnBattlePrepare(int battle_mode, int role_count, Role** role_list, RoleBattleData** role_data_list)
{

	
}

void BattleBusinessLocal::OnBattleStart(battlegameprotocol::BattleGameStartFightAck* ack)
{

}

void BattleBusinessLocal::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck* ack)
{

}

void BattleBusinessLocal::OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack)
{

}

void BattleBusinessLocal::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight* ack)
{
}

void BattleBusinessLocal::OnBattleInstanceDeleted()
{
	
}


