#include "global/activity/activitymanager.hpp"

#include "global/activity/activityimpl/activitychiefpeakbattle.hpp"
#include "bbl_impl_chief_peak_battle.hpp"


BBLImplChiefPeakBattle::BBLImplChiefPeakBattle()
{
}

BBLImplChiefPeakBattle::~BBLImplChiefPeakBattle()
{
}

void BBLImplChiefPeakBattle::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity == NULL)
	{
		return;
	}

	switch (ack->battle_result)
	{
		case RESULT_TYPE_ATTACKER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					activity->SetFightResult(role_id, true);
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					int role_id = ack->defender_list.role_list[i].uid;
					activity->SetFightResult(role_id, false);
				}
			}
			break;
		case RESULT_TYPE_DEFENDER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					activity->SetFightResult(role_id, false);
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					int role_id = ack->defender_list.role_list[i].uid;
					activity->SetFightResult(role_id, true);
				}
			}
			break;

		default:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					activity->SetFightResult(role_id, false);
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					int role_id = ack->defender_list.role_list[i].uid;
					activity->SetFightResult(role_id, false);
				}
			}
			break;
	}
}

void BBLImplChiefPeakBattle::OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity == NULL)
	{
		return;
	}

	activity->SetFightResult(ack->uid, false);
}

void BBLImplChiefPeakBattle::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	ActivityChiefPeakBattle * activity = (ActivityChiefPeakBattle *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_PEAK_BATTLE);
	if (activity == NULL)
	{
		return;
	}

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		int role_id = ack->attacker_list.role_list[i].uid;
		activity->SetFightResult(role_id, false);
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		int role_id = ack->defender_list.role_list[i].uid;
		activity->SetFightResult(role_id, false);
	}
}

