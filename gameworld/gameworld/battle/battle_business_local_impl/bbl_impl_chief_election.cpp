#include "global/activity/activitymanager.hpp"
#include "servercommon/string/gameworldstr.h"

#include "gameworld/gamelog.h"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "bbl_impl_chief_election.hpp"


BBLImplChiefElection::BBLImplChiefElection()
{
}

BBLImplChiefElection::~BBLImplChiefElection()
{
}

void BBLImplChiefElection::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		return;
	}

	std::string attcker_list;
	std::string defender_list;

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		attcker_list = attcker_list + STRING_SPRINTF("%d,", ack->attacker_list.role_list[i].uid);
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		defender_list = defender_list + STRING_SPRINTF("%d,", ack->defender_list.role_list[i].uid);
	}

	int prof = ack->business_data.param1;
	int acttacker_cap = ack->business_data.param2;
	int defender_cap = ack->business_data.param3;
	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d attcker_list:[%s] acttacker_cap:%d defender_list:[%s] defender_cap:%d battle_result:%d", __FUNCTION__, __LINE__,
		prof, attcker_list.c_str(), acttacker_cap, defender_list.c_str(), defender_cap, ack->battle_result);

	switch (ack->battle_result)
	{
		case RESULT_TYPE_ATTACKER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					activity->SetFightResult(role_id, true, ack->business_data.param1);
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					int role_id = ack->defender_list.role_list[i].uid;
					activity->SetFightResult(role_id, false, ack->business_data.param1);
				}
			}
			break;
		case RESULT_TYPE_DEFENDER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					int role_id = ack->attacker_list.role_list[i].uid;
					activity->SetFightResult(role_id, false, ack->business_data.param1);
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					int role_id = ack->defender_list.role_list[i].uid;
					activity->SetFightResult(role_id, true, ack->business_data.param1);
				}
			}
			break;

		default:
			{
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip3);

				if (acttacker_cap >= defender_cap)
				{
					for (int i = 0; i < ack->attacker_list.role_count; ++i)
					{
						int role_id = ack->attacker_list.role_list[i].uid;
						activity->SetFightResult(role_id, true, ack->business_data.param1);

						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
						}
					}

					for (int i = 0; i < ack->defender_list.role_count; ++i)
					{
						int role_id = ack->defender_list.role_list[i].uid;
						activity->SetFightResult(role_id, false, ack->business_data.param1);

						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
						}
					}
				}
				else
				{
					for (int i = 0; i < ack->attacker_list.role_count; ++i)
					{
						int role_id = ack->attacker_list.role_list[i].uid;
						activity->SetFightResult(role_id, false, ack->business_data.param1);

						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
						}
					}

					for (int i = 0; i < ack->defender_list.role_count; ++i)
					{
						int role_id = ack->defender_list.role_list[i].uid;
						activity->SetFightResult(role_id, true, ack->business_data.param1);

						if (length > 0)
						{
							World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
						}
					}
				}
			}
			break;
	}
}

void BBLImplChiefElection::OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		return;
	}

	int prof = ack->business_data.param1;
	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d role_id:%d", __FUNCTION__, __LINE__, prof, ack->uid);

	activity->SetFightResult(ack->uid, false, ack->business_data.param1);
}

void BBLImplChiefElection::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	std::string attcker_list;
	std::string defender_list;

	int prof = ack->business_data.param1;
	int acttacker_cap = ack->business_data.param2;
	int defender_cap = ack->business_data.param3;

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		attcker_list = attcker_list + STRING_SPRINTF("%d,", ack->attacker_list.role_list[i].uid);
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		defender_list = defender_list + STRING_SPRINTF("%d,", ack->defender_list.role_list[i].uid);
	}

	gamelog::g_Log_chief_election.printf(LL_INFO, "%s line:%d | prof:%d attcker_list:[%s] attacker_cap:%d defender_list:[%s] defender_cap:%d result_type:%d", __FUNCTION__, __LINE__, 
					     prof, attcker_list.c_str(), acttacker_cap, defender_list.c_str(), defender_cap, ack->result_type);

	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (activity == NULL)
	{
		return;
	}

	if (ack->result_type == RESULT_TYPE_EXCEED_MAX_TIME)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_chief_election_sea_tip3);

		if (acttacker_cap >= defender_cap)
		{
			for (int i = 0; i < ack->attacker_list.role_count; ++i)
			{
				int role_id = ack->attacker_list.role_list[i].uid;
				activity->SetFightResult(role_id, true, ack->business_data.param1);
				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}

			for (int i = 0; i < ack->defender_list.role_count; ++i)
			{
				int role_id = ack->defender_list.role_list[i].uid;
				activity->SetFightResult(role_id, false, ack->business_data.param1);

				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
		else
		{
			for (int i = 0; i < ack->attacker_list.role_count; ++i)
			{
				int role_id = ack->attacker_list.role_list[i].uid;
				activity->SetFightResult(role_id, false, ack->business_data.param1);

				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}

			for (int i = 0; i < ack->defender_list.role_count; ++i)
			{
				int role_id = ack->defender_list.role_list[i].uid;
				activity->SetFightResult(role_id, true, ack->business_data.param1);

				if (length > 0)
				{
					World::GetInstWorld()->GetSceneManager()->SystemMsgPerson(role_id, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
				}
			}
		}
	}
}
