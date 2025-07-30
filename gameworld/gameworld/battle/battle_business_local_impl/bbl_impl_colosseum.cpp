#include "bbl_impl_colosseum.hpp"
#include "world.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/colosseum/colosseummanager.hpp"

BBLImplColosseum::BBLImplColosseum()
{
}

BBLImplColosseum::~BBLImplColosseum()
{
}

void BBLImplColosseum::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	BattleBusinessLocal::OnBattleForceFinish(ack);

	if (RESULT_TYPE_FORCED_FINISH != ack->result_type) return;
	// 飘字提示结算 - 只飘攻方，因为守方一定是 AI，就没必要飘了
	for (int i = 0; i < ack->attacker_list.role_count && i < SIDE_MAX_ROLE_NUM; ++i)
	{
		Role * mem = World::GetInstWorld()->GetSceneManager()->GetRole(ack->attacker_list.role_list[i].uid);
		if (NULL == mem) continue;

		//此处暂时屏蔽，因为赛季已经结束，不再做排名处理
		//ColosseumManager::Instance().SysReport(mem, ack->business_data.colosseum_target_uid, false);
		mem->NoticeNum(errornum::EN_GAME_SEASON_ALREADY_END);
	}
}