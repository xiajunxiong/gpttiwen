#include "rankmanager.hpp"
#include "servercommon/rankdef.hpp"

#include "global/rmibackobjdef.h"
#include "engineadapter.h"
#include "internalcomm.h"

#include "obj/character/role.h"
#include "serverlogic.h"
#include "world.h"
#include "servercommon/string/globalstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

RankManager::RankManager()
	: m_rank_data_next_save_time(0), m_rank_shapshot_timestamp(0)
{
	m_person_rank.SetRankManager(this);
	m_team_rank.SetRankManager(this);
	m_guild_rank.SetRankManager(this);
	m_pet_rank.SetRankManager(this);
	m_partner_rank.SetRankManager(this);
}

RankManager::~RankManager()
{

}

RankManager & RankManager::Instance()
{
	static RankManager umm;
	return umm;
}

void RankManager::OnServerStart()
{
	{
		for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
		{
			RMIInitPersonRankBackObjectImpl *backobj = new RMIInitPersonRankBackObjectImpl();
			backobj->rank_type = i;

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitPersonRankAsyn(i, backobj);
		}
	}

	{
		RMIInitTeamRankBackObjectImpl *backobj = new RMIInitTeamRankBackObjectImpl();

		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		ac.InitTeamRankAsyn(backobj);
	}

	{
		for (int i = 0; i < PET_RANK_TYPE_MAX; ++i)
		{
			RMIInitPetRankBackObjectImpl *backobj = new RMIInitPetRankBackObjectImpl();
			backobj->rank_type = i;

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitPetRankAsyn(i, backobj);
		}
	}

	{
		for (int i = 0; i < PARTNER_RANK_TYPE_MAX; ++i)
		{
			RMIInitPartnerRankBackObjectImpl *backobj = new RMIInitPartnerRankBackObjectImpl();
			backobj->rank_type = i;

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
			ac.InitPartnerRankAsyn(i, backobj);
		}
	}
}

void RankManager::OnServerStop()
{
	this->Save();
}

void RankManager::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsLoadFinish()) return;

	if (m_rank_shapshot_timestamp > 0 && now_second >= m_rank_shapshot_timestamp)
	{
		this->SetRankShapshotTimestamp();
	}

	if (m_rank_data_next_save_time > 0 && now_second > m_rank_data_next_save_time)
	{
		this->Save();
		m_rank_data_next_save_time = EngineAdapter::Instance().Time() + RANK_DATA_SAVE_TO_DB_TIME;
	}

	m_person_rank.Update(now_second);
	m_team_rank.Update(now_second);
	m_guild_rank.Update(now_second);
	m_pet_rank.Update(now_second);
	m_partner_rank.Update(now_second);
	m_pet_top_rank_shadow.Update(static_cast<unsigned int>(now_second));
	m_partner_top_rank_shadow.Update(static_cast<unsigned int>(now_second));
}

void RankManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
}

void RankManager::OnWeekChange()
{
	m_guild_rank.OnWeekChange();
}

void RankManager::OnUserLogin(Role *user)
{
	m_person_rank.OnUserLogin(user);
	m_partner_rank.OnUserLogin(user);
}

void RMIInitPersonRankBackObjectImpl::InitPersonRankRet(int ret, const PersonRankParam &person_rank_param)
{
	if (ret < 0)
	{
		printf("RMIInitPersonRankBackObjectImpl::Init Failed \n");
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitPersonRankBackObjectImpl::InitPersonRankRet_1");
	}
	else
	{
		PersonRank *person_rank = RankManager::Instance().GetPersonRank();
		if (NULL == person_rank) return;

		person_rank->Init(person_rank_param, rank_type);
		RankManager::Instance().LoadPersonRankSucc();

		gamelog::g_log_world.printf(LL_INFO, "RMIInitPersonRankBackObjectImpl::InitPersonRankRet finish load rank data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());
	}
}

void RMIInitTeamRankBackObjectImpl::InitTeamRankRet(int ret, const TeamRankParam &team_rank_param)
{
	if (ret < 0)
	{
		printf("RMIInitTeamRankBackObjectImpl::Init Failed \n");
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitTeamRankBackObjectImpl::InitTeamRankRet_1");
	}
	else
	{
		TeamRank *team_rank = RankManager::Instance().GetTeamRank();
		if (NULL == team_rank) return;

		team_rank->Init(team_rank_param);
		RankManager::Instance().LoadTeamRankSucc();
	}
}

void RMIInitPetRankBackObjectImpl::InitPetRankRet(int ret, const PetRankParam &pet_rank_param)
{
	if (ret < 0)
	{
		printf("RMIInitPetRankBackObjectImpl::Init Failed \n");
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitPetRankBackObjectImpl::InitPetRankRet_1");
	}
	else
	{
		PetRank *pet_rank = RankManager::Instance().GetPetRank();
		if (NULL == pet_rank) return;

		pet_rank->Init(pet_rank_param , rank_type);
		RankManager::Instance().LoadPetRankSucc();

		gamelog::g_log_world.printf(LL_INFO, "RMIInitPetRankBackObjectImpl::InitPetRankRet finish load rank data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());
	}
}

void RMIInitPartnerRankBackObjectImpl::InitPartnerRankRet(int ret, const PartnerRankParam &partner_rank_param)
{
	if (ret < 0)
	{
		printf("RMIInitPartnerRankBackObjectImpl::Init Failed \n");
		ServerLogic::GetInstServerLogic()->StopServer("RMIInitPartnerRankBackObjectImpl::InitPartnerRankRet_1");
	}
	else
	{
		PartnerRank *partner_rank = RankManager::Instance().GetPartnerRank();
		if (NULL == partner_rank) return;

		partner_rank->Init(partner_rank_param, rank_type);
		RankManager::Instance().LoadPartnerRankSucc();

		gamelog::g_log_world.printf(LL_INFO, "RMIInitPartnerRankBackObjectImpl::InitPartnerRankRet finish load rank data at %u %lu", (unsigned int)EngineAdapter::Instance().Time(), EngineAdapter::Instance().GetGameTime());
	}
}

void RankManager::LoadPersonRankSucc()
{
	if (this->IsLoadFinish())
	{
		this->LoadAllRankSucc();
	}
}

void RankManager::LoadTeamRankSucc()
{
	if (this->IsLoadFinish())
	{
		this->LoadAllRankSucc();
	}
}

void RankManager::LoadPetRankSucc()
{
	if (this->IsLoadFinish())
	{
		this->LoadAllRankSucc();
	}
}

void RankManager::LoadPartnerRankSucc()
{
	if (this->IsLoadFinish())
	{
		this->LoadAllRankSucc();
	}
}

void RankManager::LoadAllRankSucc()
{
	m_rank_data_next_save_time = EngineAdapter::Instance().Time() + RANK_DATA_SAVE_TO_DB_TIME;
	this->SetRankShapshotTimestamp();

	m_pet_top_rank_shadow.OnRankManagerLoadSucc();
	m_partner_top_rank_shadow.OnRankManagerLoadSucc();
}

void RankManager::SetRankShapshotTimestamp()
{
	m_rank_shapshot_timestamp = EngineAdapter::Instance().NextMinuteInterval(0) + static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void RankManager::Save()
{
	if (!this->IsLoadFinish()) return;

	{
		static PersonRankParam person_rank_param;

		for (int i = 0; i < PERSON_RANK_TYPE_MAX; ++i)
		{
			person_rank_param.count = 0;
			m_person_rank.GetChangeRankItemList(&person_rank_param, i);
		if (0 != person_rank_param.count)
		{
			RMISavePersonRankBackObjectImpl *backobj = new RMISavePersonRankBackObjectImpl();

			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

			bool ret = ac.SavePersonRankAsyn(person_rank_param, backobj);
			if (ret)
			{
					m_person_rank.ClearDirtyMark(i);
			}
		}
	}
	}

	{
		static TeamRankParam team_rank_param;

		m_team_rank.GetChangeRankItemList(&team_rank_param);
		if (0 != team_rank_param.count)
		{
			RMISaveTeamRankBackObjectImpl *backobj = new RMISaveTeamRankBackObjectImpl();
			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

			bool ret = ac.SaveTeamRankAsyn(team_rank_param, backobj);
			if (ret)
			{
				m_team_rank.ClearDirtyMark();
			}
		}
	}

	{
		static PetRankParam pet_rank_param;

		for (int i = 0; i < PET_RANK_TYPE_MAX; ++i)
		{
			pet_rank_param.count = 0;
			m_pet_rank.GetChangeRankItemList(&pet_rank_param, i);
		if (0 != pet_rank_param.count)
		{
			RMISavePetRankBackObjectImpl *backobj = new RMISavePetRankBackObjectImpl();
			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

			bool ret = ac.SavePetRankAsyn(pet_rank_param, backobj);
			if (ret)
			{
					m_pet_rank.ClearDirtyMark(i);
			}
		}
	}
	}

	{
		static PartnerRankParam partner_rank_param;

		for (int i = 0; i < PARTNER_RANK_TYPE_MAX; ++i)
		{
			partner_rank_param.count = 0;
			m_partner_rank.GetChangeRankItemList(&partner_rank_param, i);
			if (0 != partner_rank_param.count)
			{
				RMISavePartnerRankBackObjectImpl *backobj = new RMISavePartnerRankBackObjectImpl();
				RMIGlobalClient ac;
				ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

				bool ret = ac.SavePartnerRankAsyn(partner_rank_param, backobj);
				if (ret)
				{
					m_partner_rank.ClearDirtyMark(i);
				}
			}
		}
	}

}

void RankManager::OnGetPersonRankList(Role *user, Protocol::CSGetPersonRankList *rank_list_req)
{
	if (NULL == user || NULL == rank_list_req) return;

	m_person_rank.OnGetRankList(user, *rank_list_req);
}

void RankManager::OnHiddenGameGetRankList(crossgameprotocal::HiddenGameGetRankListReq * req)
{
	if(NULL == req) return;

	m_person_rank.OnHiddenGameGetRankList(req);
}

void RankManager::OnHiddenGameGetRankFriendList(crossgameprotocal::HiddenGameGetFriendRankListReq * req)
{
	if (NULL == req) return;

	m_person_rank.OnHiddenGameGetFriendRankList(req);
}

void RankManager::OnGetPetRankList(Role *role, int rank_type, int from_index, int to_index)
{
	m_pet_rank.OnGetRankList(role, rank_type, from_index, to_index);
}

void RankManager::OnGetPetRankDetailedInfo(Role * role, int target_uid, int rank_type, unsigned long long pet_unique_item_id)
{
	m_pet_rank.OnGetRankDetailedInfo(role, target_uid, rank_type, pet_unique_item_id);
}

void RankManager::OnGetPersonRankTopUser(Role *user, Protocol::CSGetPersonRankTopUser *top_user_req)
{
	if (NULL == user || NULL == top_user_req) return;

	m_person_rank.OnGetRankTopUser(user, *top_user_req);
}

void RankManager::SyncPersonBaseInfo(Role *user, int old_prof, int cur_prof)
{
	if (NULL == user) return;

	m_person_rank.OnBaseInfoChange(user, old_prof, cur_prof);
	m_pet_rank.OnRoleInfoChange(user);
}

void RankManager::OnRoleResetName(Role * _role)
{
	if (NULL == _role) return;

	m_person_rank.UserResetName(_role);
	m_pet_rank.OnRoleInfoChange(_role);
}

void RankManager::SyncRemovePet(Role *role, unsigned long long pet_unique_id)
{
	m_pet_rank.OnPetRemove(role, pet_unique_id);
}

void RankManager::SyncPersonRankInfo(Role * _role, int _rank_type)
{
	if (_rank_type < 0 || _rank_type >= PERSON_RANK_TYPE_MAX || NULL == _role)
	{
		return;
	}

	m_person_rank.SyncPersonRankInfo(_role, _rank_type);
}

void RankManager::OnTopLevelResetSyn(UserCacheNode * node)
{
	if(NULL == node) return;

	m_person_rank.OnTopLevelResetSyn(node);
}

void RankManager::OnGetPersonRankUidList(int rank_type, int count, std::vector<int> & rank_uid_list)
{
	m_person_rank.GetRankList(rank_type, count, rank_uid_list);
}

void RankManager::SyncPetRankInfo(Role * _role, int _rank_type , int _index , bool _is_force)
{
	if (_rank_type < 0 || _rank_type >= PET_RANK_TYPE_MAX || NULL == _role)
	{
		return;
	}

	m_pet_rank.SyncPetRankInfo(_role , _rank_type ,_index , _is_force);
}

void RankManager::SyncPartnerRankInfo(Role * _role, int _rank_type, int partner_id, bool _is_force)
{
	if (_rank_type < 0 || _rank_type >= PARTNER_RANK_TYPE_MAX || NULL == _role)
	{
		return;
	}

	m_partner_rank.SyncPartnerRankInfo(_role, _rank_type, partner_id, _is_force);
}

void RankManager::SendPartnerTopRankInfo(int user_uid)
{
	m_partner_top_rank_shadow.SendPartnerAllTopRankInfo(user_uid);
}

void RankManager::SendPetTopRankInfo(int user_uid)
{
	m_pet_top_rank_shadow.SendPetAllTopRankInfo(user_uid);
}

void RankManager::OnGetGuildRankList(Role *user, Protocol::CSGetGuildRankList *rank_list_req)
{
	if (NULL == user || NULL == rank_list_req) return;

	m_guild_rank.OnGetRankList(user, *rank_list_req);
}

void RankManager::SendRefreshTimestamp(Role * role)
{
	if(NULL == role) return;

	Protocol::SCRankRefreshTime info;
	info.rank_refresh_timestamp = m_rank_shapshot_timestamp;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &info, sizeof(info));
}

void RankManager::OnGetTeamRankList(Role *user, Protocol::CSGetTeamRankList *rank_list_req)
{
	if (NULL == user || NULL == rank_list_req) return;

	m_team_rank.OnGetRankList(user, *rank_list_req);
}

void RankManager::SyncTeamRankInfo(gglobalprotocal::SyncTeamRankInfo *team_rank_info)
{
	if (NULL == team_rank_info) return;

	m_team_rank.OnRankInfoChange(*team_rank_info);
}

void RankManager::SyncGuildRankInfo(Guild * _guild, int _rank_type)
{
	if (NULL == _guild) return;
	m_guild_rank.OnGuildRankInfoChange(_guild ,_rank_type);
}

void RankManager::ClearPersonRankType(int rank_type)
{
	m_person_rank.ClearRank(rank_type);
}

void RankManager::ClearGuildRankType(int rank_type)
{
	m_guild_rank.ClearRank(rank_type);
}

void RankManager::ClearTeamRankType(int rank_type)
{
	m_team_rank.ClearRank(rank_type);
}

void RankManager::ClearPetRankType(int rank_type)
{
	m_pet_rank.ClearRank(rank_type);
}

void RankManager::ClearPartnerRankType(int rank_type)
{
	m_partner_rank.ClearRank(rank_type);
}

void RankManager::OnConnectCrossSucc()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_person_rank.OnGameworldAccept();
	m_pet_rank.OnGameworldAccept();
	m_guild_rank.OnGameworldAccept();
}

void RankManager::OnHiddenGameRoleInfoChange(crossgameprotocal::HiddenGameRoleInfoChange * _msg)
{
	m_person_rank.SyncRankDataFromHidden(_msg);
}

void RankManager::OnHiddenGamePetInfoChange(crossgameprotocal::HiddenGamePetInfoChange * _msg)
{
	m_pet_rank.SyncRankDataFromHidden(_msg);
}

void RankManager::OnHiddenRecvGameSynLevelComplementInfo(crossgameprotocal::GameHiddenSynLevelComplementInfo * msg)
{
	m_person_rank.OnHiddenRecvGameSynLevelComplementInfo(msg);
}

int RankManager::GetAverageByLevelComplement(int server_id)
{
	return m_person_rank.GetAverageByLevelComplement(server_id);
}

void RankManager::LoadGuildRankSucc()
{
	if (this->IsLoadFinish())
	{
		this->LoadAllRankSucc();
	}
}

void RankManager::SyncPetTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPetTopRankInfo* msg)
{
	m_pet_top_rank_shadow.SyncPetTopRankFromOrigin(msg);
}

void RankManager::SyncPartnerTopRankFromOrigin(crossgameprotocal::GameHiddenSyncPartnerTopRankInfo* msg)
{
	m_partner_top_rank_shadow.SyncPartnerTopRankFromOrigin(msg);
}

bool RankManager::IsLoadFinish()
{
	return m_person_rank.IsRankLoadFinish() &&
		m_team_rank.IsRankLoadFinish() &&
		m_pet_rank.IsRankLoadFinish() &&
		m_guild_rank.IsRankLoadFinish() &&
		m_partner_rank.IsRankLoadFinish();
}

//void RankManager::HonourRankReward()
//{
//	int reward_count = GLOBAL_CONFIG->GetRankConfig()->GetHonourRankRewardCount();
//	if (reward_count > RankConfig::MAX_HONOUR_RANK_REWARD_COUNT) return;
//
//	UserID user_id_list[RankConfig::MAX_HONOUR_RANK_REWARD_COUNT];
//
//	int count = m_person_rank.GetRankTopUser(PERSON_RANK_TYPE_HONOUR, reward_count, user_id_list);
//
//	for (int i = 0; i < count && i < reward_count; ++i)
//	{
//		ItemConfigData *reward_item = GLOBAL_CONFIG->GetRankConfig()->GetHonourRankRewardItem(i);
//		if (NULL == reward_item) continue;
//
//		static MailContentParam contentparam; contentparam.Reset();
//
//		contentparam.item_list[0].item_id = reward_item->item_id;
//		contentparam.item_list[0].is_bind = reward_item->is_bind ? 1 : 0;
//		contentparam.item_list[0].num = reward_item->num;
//
//		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_honour_rank_reward_subject);
//		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_honour_rank_reward_content, i + 1, reward_item->item_id);
//		if (length1 > 0 && length2 > 0)
//		{
//			GlobalServer::GetInstGlobalServer()->GetUserMailManager()->MailToUser(MAIL_TYPE_SYSTEM, user_id_list[i], MAIL_REASON_DEFAULT, contentparam);
//		}
//	}
//}
//
//void RankManager::Rank1v1Reward()
//{
//	static const int MAX_REWARD_COUNT = 100;
//
//	UserID user_id_list[MAX_REWARD_COUNT];
//	int count = m_person_rank.GetRankTopUser(PERSON_RANK_TYPE_1V1, MAX_REWARD_COUNT, user_id_list);
//
//	for (int i = 0; i < count && i < MAX_REWARD_COUNT; ++i)
//	{
//		if (0 == i)
//		{
//			const PersonRankItem *temp_rank_item = this->GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_1V1, user_id_list[0], 0);
//			if (NULL == temp_rank_item || temp_rank_item->rank_value <= 1000) 
//			{
//				break;
//			}
//		}
//
//		int rank = i + 1;
//
//		int reward_honor = GLOBAL_CONFIG->GetRankConfig()->Get1v1RewardHonor(rank);
//		if (reward_honor <= 0) 
//		{
//			break;
//		}
//
//		static MailContentParam contentparam; contentparam.Reset();
//
//		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BATTLEFIELDHONOR] = reward_honor;
//
//		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_1v1_rank_reward_subject);
//		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_1v1_rank_reward_content, rank, reward_honor);
//		if (length1 > 0 && length2 > 0)
//		{
//			GlobalServer::GetInstGlobalServer()->GetUserMailManager()->MailToUser(MAIL_TYPE_SYSTEM, user_id_list[i], MAIL_REASON_DEFAULT, contentparam);
//		}
//	}
//}

