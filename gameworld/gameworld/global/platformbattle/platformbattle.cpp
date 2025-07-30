#include <string>

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "world.h"
#include "scene/scenemanager.hpp"
#include "internalcomm.h"
#include "global/rmibackobjdef.h"
#include "global/worldstatus/worldstatus.hpp"
#include "battle/battle_manager_local.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/partner/partnerequipmentconfig.hpp"
#include "other/train/trainconfig.hpp"
#include "global/usercache/usercache.hpp"
#include "item/itempool.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "other/robot/robotconfig.hpp"
#include "servercommon/errornum.h"
#include "servercommon/serverconfig/nameconfig.h"
#include "skill/skillpool.hpp"
#include "servercommon/serverconfig/battleconfig/battle_special_effect_pool.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"
#include "other/train/train.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/serverconfig/appearanceconfig.hpp"

#include "global/commonsave/commonsavemanager.hpp"
#include "global/platformbattle/platformbattleconfig.hpp"
#include "protocol/msgplatformbattle.h"
#include "platformbattle.h"
#include "servercommon/protobuf/platform_battle_role_data.pb.h"
#include "gamelog.h"
#include "servercommon/protobufmanager.hpp"

PlatformBattle & PlatformBattle::Instance()
{
	static PlatformBattle instance;
	return instance;
}

void PlatformBattle::OnServerStart()
{
	this->LoadPlatformBattleData(0);
}

void PlatformBattle::OnServerStop()
{
	this->Save(true);
}

bool PlatformBattle::LoadPlatformBattleData(long long inc_id_from)
{
	if (m_load_flag)
	{
		return false;
	}

	RMIInitPlatformBattleBackObjectImpl * impl = new RMIInitPlatformBattleBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitPlatformBattleAsyn(inc_id_from, impl);
}

void PlatformBattle::LoadPlatformBattleDataSucc()
{
	m_load_flag = true;

	this->CheckAllPlatform();
}

bool PlatformBattle::IsLoadFinish()
{
	return m_load_flag;
}

void PlatformBattle::OnGameServerConnect(std::set<int> server_id)
{
	if (!server_id.empty())
	{
		this->SyncRoleInfoHiddenToGameworld(*server_id.begin());
	}
}

void PlatformBattle::OnDayChange()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	for (int platform_number = 1; platform_number <= PLATFORM_BATTLE_PLATFORM_NUM; ++platform_number)
	{
		int top1_index = 0;
		PlatformBattlePlatformInfo & platform_info = m_platform_info[platform_number];
		const PlatformBattleRoleData * rank_item_role_data_ptr = platform_info.GetPlatformBattleRoleData(top1_index);
		
		if (rank_item_role_data_ptr == NULL || rank_item_role_data_ptr->IsRobot())
		{
			continue;
		}

		int day = (int)(now - rank_item_role_data_ptr->top1_time + SECOND_PER_DAY - 1) / SECOND_PER_DAY;
		this->OnTop1DayChange(rank_item_role_data_ptr->role_id, platform_number, day);
	}
}

void PlatformBattle::OnUserLogin(Role * role)
{
	if(NULL == role) return;

	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role->GetUID());
	if (role_info_iter != m_role_info.end())
	{
		PlatformBattleRoleInfo & role_info = role_info_iter->second;

		std::vector<int> need_remove;

		for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator iter = role_info.platform_play_data.begin(); iter != role_info.platform_play_data.end(); ++iter)
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info = iter->second;

			std::set<unsigned long long> pet_list;
			for (int i = 0; i < (int)role_battle_info.battling_pet.size(); ++i)
			{
				pet_list.insert(role_battle_info.battling_pet[i].first);
			}

			//如果没有就需要删除
			if (!role->GetRoleModuleManager()->GetPet()->HasPet(pet_list))
			{
				need_remove.push_back(iter->first);
			}

			for (int i = 0; i < (int)role_battle_info.battling_pet.size(); ++i)
			{
				role->GetRoleModuleManager()->GetPet()->SetPetPlatformBattleState(role_battle_info.battling_pet[i].first, true);
			}

			for (int i = 0; i < (int)role_battle_info.battling_partner.size(); ++i)
			{
				role->GetRoleModuleManager()->GetPartner()->SetPartnerPlatformBattleState(role_battle_info.battling_partner[i].first, true);
			}
		}

		//玩家没有拥有擂台上的宠物，就下掉这个擂台
		for (int i = 0; i < (int)need_remove.size(); ++i)
		{
			gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | remove role_id:%d platform_num:%d", __FUNCTION__, __LINE__, role->GetUID(), need_remove[i]);
			this->RemovePlatformPlayInfo(role, need_remove[i], false, __FUNCTION__);
		}
	}

	this->SendRoleInfo(role);
	
	//以下要再隐藏服才执行
	role->GetRoleModuleManager()->GetPet()->SyncPetDataPlatformBattle();
	this->SynRoleInfo(role);
	this->SyncPartnerData(role);
	this->SendRecordNotice(role->GetUID());
	this->SendAchievementInfo(role->GetUID());
}

void PlatformBattle::OnUserLogout(Role * role)
{
	//离线
}

void PlatformBattle::OnUserLoginOriginGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->SendRecordNotice(role_id);
	this->SendAchievementInfo(role_id, true);
}

void PlatformBattle::OnDelPet(Role * role, unsigned long long pet_uuid)
{
	int role_id = role->GetUID();
	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role_id);
	if (role_info_iter != m_role_info.end())
	{
		PlatformBattleRoleInfo & role_info = role_info_iter->second;

		std::vector<int> need_remove;
		for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator iter = role_info.platform_play_data.begin(); iter != role_info.platform_play_data.end(); ++iter)
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info = iter->second;

			std::set<unsigned long long> pet_list;
			for (int i = 0; i < (int)role_battle_info.battling_pet.size(); ++i)
			{
				if (role_battle_info.battling_pet[i].first == pet_uuid)
				{
					need_remove.push_back(iter->first);
					break;
				}
			}
		}

		for (int i = 0; i < (int)need_remove.size(); ++i)
		{
			gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | remove role_id:%d platform_num:%d del_pet_uuid:%lld", __FUNCTION__, __LINE__, role_id, need_remove[i], pet_uuid);
			this->RemovePlatformPlayInfo(role, need_remove[i], false, __FUNCTION__);
		}
	}
}

void PlatformBattle::InitPlatformInfo(int platform_num, int rank, const PlatformBattleRoleData & role_data)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int index = rank - 1;

	PlatformBattlePlatformInfo & platform_info = m_platform_info[platform_num];

	if (index >= (int)platform_info.GetRankItemSize() && (int)platform_info.GetRankItemSize() <= PLATFORM_BATTLE_RANK_ITEM_MAX)
	{
		PlatformBattleRankItemInfoNode rank_node;
		platform_info.rank_list.resize(rank, rank_node);
	}

	if (index < 0 || index >= (int)platform_info.GetRankItemSize())
	{
		return;
	}

	PlatformBattleRankItemInfoNode rank_node;
	rank_node.rank_item = role_data;
	platform_info.rank_list[index] = rank_node;
}

void PlatformBattle::ChallengePlatform(Role * role, int platform_number, int challenge_rank, MsgPlatformBattlePlayItem(&arg_pet_index)[PLATFORM_BATTLE_PET_PARTNER_MAX], MsgPlatformBattlePlayItem(&arg_partner_id)[PLATFORM_BATTLE_PET_PARTNER_MAX])
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::set<unsigned long long> pet_unique_ids;
	std::set<int> partner_ids;
	int ret_num = this->CheckArg(role, arg_pet_index, arg_partner_id, pet_unique_ids, partner_ids);
	if (ret_num != 0)
	{
		gamelog::g_log_platform_battle.printf(LL_INFO, "%s line:%d | role:[%s %d]  check_arg_ret:[%d]", __FUNCTION__, __LINE__, role->GetName(), role->GetUID(), ret_num);
		return;
	}

	ret_num = this->CanJoinPlatformBattle(role->GetUID(), platform_number, challenge_rank, pet_unique_ids, partner_ids);
	if (ret_num != 0)
	{
		gamelog::g_log_platform_battle.printf(LL_INFO, "%s line:%d | role:[%s %d]  can_join_platform_battle_ret:[%d]", __FUNCTION__, __LINE__, role->GetName(), role->GetUID(), ret_num);
		return;
	}

	int max_allowable_num = LOGIC_CONFIG->GetPlatformBattleConfig()->GetMaxAllowableNum(platform_number);
	if ((int)pet_unique_ids.size() > max_allowable_num || (int)partner_ids.size() > max_allowable_num || (pet_unique_ids.size() == 0 && partner_ids.size() == 0))
	{
		return;
	}

	std::vector<PetBattleData> ack_pet_battle_data;			//调用战斗函数所需要用到的数据
	std::vector<PartnerBattleData> ack_partner_battle_data;		//调用战斗函数所需要用到的数据
	std::vector<int> ack_pet_pos_info;						//调用战斗函数所需要用到的数据
	std::vector<int> ack_partner_pos_info;					//调用战斗函数所需要用到的数据

	PlatformBattleRoleData role_play_data;		//玩家出战信息，战斗完成后。用作上擂数据存库
	role_play_data.role_id = role->GetUID();
	role_play_data.role_pet_train_skill_level = role->GetRoleModuleManager()->GetTrain()->GetRolePetTrainSkillLevel();
	role_play_data.role_level = role->GetLevel();
	role_play_data.avatar_type = role->GetAvatarType();
	role_play_data.headshort_id = role->GetHeadshotID();
	role_play_data.prof = role->GetProfession();
	role->GetName(role_play_data.role_name);
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		PetBattleData pet_temp_data;
		if (role->GetRoleModuleManager()->GetPet()->GetPlatformBattleCharacterData(arg_pet_index[i].pet_index, &pet_temp_data, role_play_data.battling_pet[i]))
		{
			pet_temp_data.pet_exclusive_skill_disable_flag = arg_pet_index[i].pet_exclusive_skill_disable_flag;
			pet_temp_data.lian_yao_exclusive_skill_disable_flag = arg_pet_index[i].lian_yao_exclusive_skill_disable_flag;
			ack_pet_battle_data.push_back(pet_temp_data);
			ack_pet_pos_info.push_back(arg_pet_index[i].pet_battle_pos);
			role_play_data.battling_pet[i].pos = arg_pet_index[i].pet_battle_pos;
		}

		PartnerBattleData partner_temp_data;
		if (role->GetRoleModuleManager()->GetPartner()->GetPlatformBattleCharacterData(arg_partner_id[i].partner_id, &partner_temp_data, role_play_data.battling_partner[i], partner_ids))
		{
			ack_partner_battle_data.push_back(partner_temp_data);
			ack_partner_pos_info.push_back(arg_partner_id[i].partner_battle_pos);
			role_play_data.battling_partner[i].pos = arg_partner_id[i].partner_battle_pos;
		}
	}

	int def_uid = 0;																		//调用战斗函数所需要用到的数据
	std::vector<PetBattleData> def_pet_battle_data;					//调用战斗函数所需要用到的数据
	std::vector<PartnerBattleData> def_partner_battle_data;				//调用战斗函数所需要用到的数据
	std::vector<int> def_pet_pos_info;						//调用战斗函数所需要用到的数据
	std::vector<int> def_partner_pos_info;					//调用战斗函数所需要用到的数据

	if (!this->GetBattleData(platform_number, challenge_rank, def_uid, def_pet_battle_data, def_pet_pos_info, def_partner_battle_data, def_partner_pos_info))
	{
		return;
	}

	if ((ack_pet_battle_data.size() == 0 && ack_partner_battle_data.size() == 0) || (def_pet_battle_data.size() == 0 && def_partner_battle_data.size() == 0))
	{
		return;
	}

	BattleData_Business business_data; business_data.Reset();
	business_data.param1 = platform_number;
	business_data.param2 = role->GetUID();
	business_data.param3 = def_uid;
	bool ret = business_data.Push(&role_play_data, sizeof(role_play_data));		//记录挑战者这场战斗用了什么伙伴或宠物，打赢时候记录到擂台排行榜中
	if (!ret)
	{
		return;
	}
	WorldStatus::Instance().LogFunctionStats(role, "PlatformBattle");

	if (!BattleManagerLocal::GetInstance().StartPlatformBattleReq(role, ack_pet_battle_data.size(), ack_pet_battle_data.empty() ? NULL : &ack_pet_battle_data[0], ack_pet_pos_info.empty() ? NULL : &ack_pet_pos_info[0],
								      ack_partner_battle_data.size(), ack_partner_battle_data.empty() ? NULL : &ack_partner_battle_data[0], ack_partner_pos_info.empty() ? NULL : &ack_partner_pos_info[0], IntToUid(def_uid),
								      def_pet_battle_data.size(), def_pet_battle_data.empty() ? NULL : &def_pet_battle_data[0], def_pet_pos_info.empty() ? NULL : &def_pet_pos_info[0], def_partner_battle_data.size(),
								      def_partner_battle_data.empty() ? NULL : &def_partner_battle_data[0], def_partner_pos_info.empty() ? NULL : &def_partner_pos_info[0], business_data))
	{
		return;
	}
	else
	{
		role->LogActive(LOG_ACTIVE_TYPE_PLATFORM_BATTLE);
	}
}

void PlatformBattle::RemovePlatformPlayInfo(Role * role, int platform_number, bool is_client_req, const char * reason)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenPlatformBattleReq msg;
		msg.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_REMOVE;
		msg.param1 = role->GetUID();
		msg.param2 = platform_number;
		msg.param3 = is_client_req ? 1 : 0;
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, sizeof(msg));

		return;
	}

	this->RemovePlatformPlayInfoByRoleId(role->GetUID(), platform_number, reason);

	this->SendRoleInfo(role);
	PlatformBattle::Instance().SendPlatformInfo(role, platform_number);
}

void PlatformBattle::RemovePlatformPlayInfoByRoleId(int role_id, int platform_number, const char * reason)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role_id);
	if (role_info_iter == m_role_info.end())
	{
		return;
	}
	PlatformBattleRoleInfo & role_info = role_info_iter->second;  //这玩家占领擂台信息

	std::map<int, PlatformBattleRoleBattleInfoItem>::iterator role_play_info_iter = role_info.platform_play_data.find(platform_number);
	if (role_play_info_iter == role_info.platform_play_data.end())
	{
		return;
	}

	gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | role_id:%d platform_num:%d rank:%d reason:%s", __FUNCTION__, __LINE__,
					      role_id, platform_number, role_play_info_iter->second.rank, reason ? reason : "");

	PlatformBattlePlatformInfo & rank_info = m_platform_info[platform_number];
	rank_info.RemoveRankNode(role_play_info_iter->second.rank);//这里之后role_play_info_iter 不可用了
}

void PlatformBattle::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (now_second >= m_next_save_time)
	{
		this->Save();
		m_next_save_time = now_second + 60;
	}

	{
		unsigned int & platform_battle_settle_accounts_time = WorldStatus::Instance().GetCommonData().platform_battle_settle_accounts_time;

		if (platform_battle_settle_accounts_time == 0)
		{
			platform_battle_settle_accounts_time = (unsigned int)(now_second + EngineAdapter::Instance().NextWeekInterval(5, 13, 0, 0));
		}

		if (now_second >= platform_battle_settle_accounts_time)
		{
			platform_battle_settle_accounts_time = (unsigned int)(now_second + EngineAdapter::Instance().NextWeekInterval(5, 13, 0, 0));
			this->OnTimeOutSettleAccounts();
		}
	}
}

void FillPlatformBattleRecordRoleInfo(CommonSavePlatformBattleRecordRoleInfo & role_info, const PlatformBattleRoleData * role_data)
{
	if (role_data == NULL)
	{
		return;
	}

	role_info.role_id = role_data->role_id;
	role_info.role_level = role_data->role_level;
	memcpy(role_info.role_name, role_data->role_name, sizeof(role_info.role_name));
	role_info.avatar_type = role_data->avatar_type;
	role_info.prof = role_data->prof;
	role_info.headshot_id = role_data->headshort_id;
}

void PlatformBattle::OnBattleFinish(int platform_number, int attacker, int defender, BattleData_Business  business_data, bool is_win)
{
	PlatformBattlePlatformInfo & rank_info = m_platform_info[platform_number];

	PlatformBattleRoleData insert_item;
	if (!business_data.Pop(&insert_item, sizeof(insert_item)))
	{
		return;
	}

	std::set<unsigned long long> pet_indexs;
	std::set<int> partner_ids;
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		if (insert_item.battling_pet[i].item_unique_id != 0)
		{
			pet_indexs.insert(insert_item.battling_pet[i].item_unique_id);
		}

		if (insert_item.battling_partner[i].partner_id != 0)
		{
			partner_ids.insert(insert_item.battling_partner[i].partner_id);
		}
	}

	int defender_index = -1;
	int attacker_index = -1;
	for (int rank_index = 0; rank_index < (int)rank_info.GetRankItemSize(); ++rank_index)
	{
		if (defender_index == -1 && rank_info.rank_list[rank_index].rank_item.role_id == defender)
		{
			defender_index = rank_index;
		}

		if (attacker_index == -1 && rank_info.rank_list[rank_index].rank_item.role_id == attacker)
		{
			attacker_index = rank_index;
		}
	}


	bool is_win_high_capacity = false;
	PlatformBattleRoleData  temp_defender_role_data = rank_info.GetPlatformBattleRoleDataTemp(defender_index);
	if (insert_item.GetAllCapacity() < temp_defender_role_data.GetAllCapacity())
	{
		is_win_high_capacity = true;
	}
	
	CommonSavePlatformBattleRecordRoleInfo role_info_1; role_info_1.Reset();
	CommonSavePlatformBattleRecordRoleInfo role_info_2; role_info_2.Reset();
	FillPlatformBattleRecordRoleInfo(role_info_1, &insert_item);
	FillPlatformBattleRecordRoleInfo(role_info_2, &temp_defender_role_data);

	CommonSavePlatformBattleRoleRecord attacker_record_data;	
	attacker_record_data.role_info_1 = role_info_1;
	attacker_record_data.role_info_2 = role_info_2;
	attacker_record_data.platform_num = platform_number;
	attacker_record_data.time = (unsigned int)EngineAdapter::Instance().Time();
	attacker_record_data.is_win = is_win;
	attacker_record_data.last_rank = attacker_index + 1;
	attacker_record_data.curr_rank = attacker_record_data.last_rank;

	CommonSavePlatformBattleRoleRecord defender_record_data;
	defender_record_data.role_info_1 = role_info_1;
	defender_record_data.role_info_2 = role_info_2;
	defender_record_data.platform_num = platform_number;
	defender_record_data.time = (unsigned int)EngineAdapter::Instance().Time();
	defender_record_data.is_win = !is_win;
	defender_record_data.last_rank = defender_index + 1;
	defender_record_data.curr_rank = defender_record_data.last_rank;

	// 若攻方没有排名
	if (attacker_index == -1)
	{
		if (defender_index != -1 && is_win)
		{
			// 攻方获取守方的名次， 守方及其后所有人下降一位
			rank_info.Add(defender_index + 1, insert_item);

			attacker_record_data.curr_rank = defender_index + 1 > PLATFORM_BATTLE_RANK_ITEM_MAX ? 0 : defender_index + 1;
			defender_record_data.curr_rank = defender_index + 2 > PLATFORM_BATTLE_RANK_ITEM_MAX ? 0 : defender_index + 2;
		}
		else
		{
			// 两人都没找到，无论输赢，小于50个就插入
			if ((int)rank_info.GetRankItemSize() < PLATFORM_BATTLE_RANK_ITEM_MAX)
			{
				int insert_rank = (int)rank_info.GetRankItemSize() + 1;
				rank_info.Add(insert_rank, insert_item);

				attacker_record_data.curr_rank = insert_rank;
			}
		}
	}
	// 若攻方有排名
	else
	{
		if (defender_index == -1 || defender_index > attacker_index || !is_win) // 若守方没有排名,或排名比攻方低, 或没赢
		{
			// 不需要操作
		}
		else
		{
			// 守方排名比攻方前，则替换双方的排名
			rank_info.SwapRank(attacker_index + 1, defender_index + 1, &insert_item);

			attacker_record_data.curr_rank = defender_record_data.last_rank;
			defender_record_data.curr_rank = attacker_record_data.last_rank;
		}
	}

	rank_info.RemoveRedundant();

	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(attacker);
	if (record_data != NULL && record_data->is_first_times != 0)
	{
		record_data->is_first_times = 0;
		this->SendRecordNotice(attacker);
	}

	if (defender_index != -1)
	{
		this->AddRecord(attacker,&attacker_record_data);
		this->AddRecord(defender, &defender_record_data);
	}

	if (is_win)
	{
		this->OnAckSucc(attacker, platform_number);
		if (is_win_high_capacity)
		{
			this->OnWinHighCapacity(attacker, platform_number);
		}
	}
	else
	{
		this->OnDefSucc(defender, platform_number);
	}
		
	gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | platform_num:%d ,attacker:%d rank:%d to %d , defender:%d rank:%d to %d", __FUNCTION__, __LINE__, platform_number, 
					      attacker, attacker_record_data.last_rank, attacker_record_data.curr_rank,
					      defender, defender_record_data.last_rank, defender_record_data.curr_rank);
}

void PlatformBattle::SendRoleInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCPlatformBattleRoleInfo protocol;
	protocol.count = 0;

	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role->GetUID());
	if (role_info_iter != m_role_info.end())
	{
		PlatformBattleRoleInfo & role_info = role_info_iter->second;
		for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator iter = role_info.platform_play_data.begin(); iter != role_info.platform_play_data.end() && protocol.count < PLATFORM_BATTLE_MAX_JOIN_NUM; ++iter)
		{
			if (iter->second.IsInvaild())
			{
				continue;
			}

			protocol.role_info[protocol.count] = iter->second;
			protocol.count += 1;
		}
	}

	unsigned int send_length = sizeof(protocol) - (PLATFORM_BATTLE_MAX_JOIN_NUM - protocol.count) * sizeof(protocol.role_info[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
}

void PlatformBattle::SendPlatformInfo(Role * role, int platform_number, int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCPlatformBattleRankInfo protocol;
	protocol.platform_number = (short)platform_number;
	protocol.count = 0;

	std::map<int, PlatformBattlePlatformInfo>::iterator iter = m_platform_info.find(platform_number);

	if (iter != m_platform_info.end())
	{
		 PlatformBattlePlatformInfo & platform_info = iter->second;
		 for (int rank_index = 0; rank_index < (int)platform_info.GetRankItemSize() && protocol.count < ARRAY_LENGTH(protocol.rank_item); ++rank_index)
		{
			const PlatformBattleRoleData & role_data = platform_info.GetPlatformBattleRoleDataTemp(rank_index);

			Protocol::SCPlatformBattleRankInfo::PlatformBattleRankItem & msg_item = protocol.rank_item[protocol.count];
			msg_item.role_id = role_data.role_id;
			msg_item.role_level = role_data.role_level;
			msg_item.top_level = UserCacheManager::Instance().GetUserTopLevel(role_data.role_id);
			msg_item.avatar_type = role_data.avatar_type;
			msg_item.headshot_id = role_data.headshort_id;
			F_STRNCPY(msg_item.role_name, role_data.role_name, sizeof(msg_item.role_name));

			for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
			{
				Protocol::MsgPlatformBattleRankItemPetInfo & pet_info = msg_item.battling_pet[i];
				const PlatformBattlePet & pet_data = role_data.battling_pet[i];

				pet_info.pos = role_data.battling_pet[i].pos;
				pet_info.pet_id = pet_data.pet_id;
				pet_info.pet_capability = pet_data.pet_capability;
				pet_info.pet_level = pet_data.pet_level;
				pet_info.pet_strengthen_level = pet_data.pet_strengthen_level;
				pet_info.pet_skin_id = pet_data.pet_battle_data.skin_id;
				memcpy(pet_info.pet_name, pet_data.pet_name, sizeof(pet_info.pet_name));

				Protocol::MsgPlatformBattleRankItemPartnerInfo & partner_info = msg_item.battling_partner[i];
				partner_info.pos = role_data.battling_partner[i].pos;
				partner_info.partner_id = role_data.battling_partner[i].partner_id;
				partner_info.partner_capability = role_data.battling_partner[i].capability;
				partner_info.partner_skin_id = role_data.battling_partner[i].partner_battle_data.skin_id;
				partner_info.level = role_data.battling_partner[i].partner_battle_data.battle_data.level;
				partner_info.str_level = role_data.battling_partner[i].partner_battle_data.str_lv;
			}

			protocol.count += 1;
		}	
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.rank_item) - protocol.count) * sizeof(protocol.rank_item[0]);


	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
	}
	else if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, send_length);
	}
}

void PlatformBattle::SendPlatformFirstInfo(Role * role, const std::vector<unsigned char> & platform_number_list)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCPlatformBattleRankFirstInfo protocol;
	protocol.count = 0;

	for (int i = 0; i < (int)platform_number_list.size() && protocol.count < ARRAY_LENGTH(protocol.first_item); ++i)
	{
		Protocol::SCPlatformBattleRankFirstInfo::PlatformBattleRankItem & msg_item = protocol.first_item[protocol.count];
		int platform_number = platform_number_list[i];
		std::map<int, PlatformBattlePlatformInfo>::iterator iter = m_platform_info.find(platform_number);
		if (iter != m_platform_info.end())
		{
			msg_item.platform_number = platform_number;

			const PlatformBattlePlatformInfo & platform_info = iter->second;
			const PlatformBattleRoleData  role_data = platform_info.GetPlatformBattleRoleDataTemp(0);

			msg_item.role_id = role_data.role_id;
			msg_item.role_level = role_data.role_level;
			msg_item.top_level = UserCacheManager::Instance().GetUserTopLevel(role_data.role_id);
			msg_item.avatar_type = role_data.avatar_type;
			msg_item.headshot_id = role_data.headshort_id;
			F_STRNCPY(msg_item.role_name, role_data.role_name, sizeof(msg_item.role_name));
			for (int z = 0; z < PLATFORM_BATTLE_PET_PARTNER_MAX; ++z)
			{
				Protocol::MsgPlatformBattleRankItemPetInfo & pet_info = msg_item.battling_pet[z];
				const PlatformBattlePet & pet_data = role_data.battling_pet[z];

				pet_info.pos = role_data.battling_pet[z].pos;
				pet_info.pet_id = pet_data.pet_id;
				pet_info.pet_capability = pet_data.pet_capability;
				pet_info.pet_level = pet_data.pet_level;
				pet_info.pet_strengthen_level = pet_data.pet_strengthen_level;
				pet_info.pet_skin_id = pet_data.pet_battle_data.skin_id;
				memcpy(pet_info.pet_name, pet_data.pet_name, sizeof(pet_info.pet_name));

				Protocol::MsgPlatformBattleRankItemPartnerInfo & partner_info = msg_item.battling_partner[z];
				const PlatformBattlePartner & partner_data = role_data.battling_partner[z];
				partner_info.pos = partner_data.pos;
				partner_info.partner_id = partner_data.partner_id;
				partner_info.partner_capability = partner_data.capability;
				partner_info.partner_skin_id = partner_data.partner_battle_data.skin_id;
				partner_info.level = partner_data.partner_battle_data.battle_data.level;
				partner_info.str_level = role_data.battling_partner[i].partner_battle_data.str_lv;
			}
			protocol.count += 1;
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.first_item) - protocol.count) * sizeof(protocol.first_item[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
}

void PlatformBattle::SetChangeFlag(bool flag)
{
	m_change_flag = flag;
}

void PlatformBattle::GmReq(Role * role,int type, int param)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (type == 0)
		{
			this->SettleAccounts();
		}

		if (type == 110)
		{
			this->ResetPlatformBattle();
		}

		if (type == 111)
		{
			this->SettleAccounts();
			this->ResetPlatformBattle();
		}

		if (type == 112)
		{
			this->OnTimeOutSettleAccounts();
		}


		if (type == 113)
		{
			this->ResetAchievement();
		}
	}

	if (type == 1)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			crossgameprotocal::GameHiddenPlatformBattleReq msg;
			msg.type = crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO;
			msg.param1 = LocalConfig::Instance().GetMergeServerId();
			InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, sizeof(msg));
		}
		else
		{
			this->SyncRoleInfoHiddenToGameworld();
		}
	}

	if (type == 2)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			return;
		}

		int server_id = param != 0 ? param : 93;
		UserID user_id;
		user_id.db_index = server_id;
		user_id.role_id = 1000;
		int role_id = UidToInt(user_id);
		int platform_num = 1;

		crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo protocol;
		protocol.type = PLATFORM_BATTLE_SYNC_ROLE_INFO_UPDATE;
		protocol.all_role_info.clear();

		MsgPlatformBattleRoleInfoItemList send_item;
		send_item.role_id = role_id;

		std::map<int, PlatformBattleRoleInfo>::iterator iter = m_role_info.find(role_id);
		MapAddOrUpdate(send_item.role_info_items, platform_num, PlatformBattleRoleBattleInfoItem(platform_num));
		protocol.all_role_info.push_back(send_item);

		if (!protocol.all_role_info.empty())
		{
			InternalComm::Instance().SendSerializeMsgToGameThroughCross(&protocol, server_id);
		}
	}

	if (type == 3)
	{
		this->RemoveRoleInfoMapItemByRoleId(1, 1);
	}
}


PlatformBattleRoleInfo * PlatformBattle::GetPlatformBattleRoleInfo(int role_id)
{
	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role_id);
	if (role_info_iter != m_role_info.end())
	{
		PlatformBattleRoleInfo & role_info = role_info_iter->second;
		return &role_info;
	}

	return NULL;
}

CommonSavePlatformBattleReceordData * PlatformBattle::GetRoleRecordData(int role_id, bool is_auto_create,bool set_data_change)
{
	CommonSavePlatformBattleReceordData * save_data = m_record_save_data->GetTypeData<CommonSavePlatformBattleReceordData>(role_id, is_auto_create);
	if (save_data != NULL)
	{
		if (save_data->role_id == 0)
		{
			save_data->role_id = role_id;
			set_data_change = true;
		}

		if (set_data_change)
		{
			m_record_save_data->SetDataChange(role_id);
		}
	
		return save_data;
	}

	return NULL;
}

void PlatformBattle::AddRecord(int role_id, const CommonSavePlatformBattleRoleRecord * record)
{
	if (record == NULL || role_id <= 0)
	{
		return;
	}

	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id);
	if (record_data == NULL)
	{
		return;
	}

	int index = record_data->record_index % ARRAY_LENGTH(record_data->records);
	record_data->record_index = index + 1;
	record_data->records[index] = *record;

	if (record->last_rank != 0 && (record->curr_rank == 0 || record->curr_rank > record->last_rank))
	{
		const PlatformBattlePlatformCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattlePlatformCfg(record->platform_num);
		if (cfg != NULL)
		{
			record_data->lose_record_flag |= (1 << cfg->position);
		}

		this->SendRecordNotice(role_id);
	}
}

void PlatformBattle::SendRecord(Role * role, int role_id)
{
	if (role != NULL)
	{
		role_id = role->GetUID();
	}

	Protocol::SCPlatformBattleRecord protocol;
	protocol.record_num = 0;

	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id, false, false);
	if (record_data != NULL)
	{
		for (int i = 0; i < ARRAY_LENGTH(record_data->records) && protocol.record_num < ARRAY_LENGTH(protocol.records); ++i)
		{
			if (record_data->records[i].time == 0)
			{
				continue;
			}

			protocol.records[protocol.record_num++] = record_data->records[i];
		}
	}

	unsigned int send_length = sizeof(protocol) - (ARRAY_LENGTH(protocol.records) - protocol.record_num) * sizeof(protocol.records[0]);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_length);
	}
}

void PlatformBattle::SendLastRankInfo(Role * role, int role_id)
{
	if (role != NULL)
	{
		role_id = role->GetUID();
	}

	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id, false, false);
	if (record_data == NULL)
	{
		return;
	}

	Protocol::SCPlatformBattleLastRankInfo protocol;
	SameArrayTypeCopy(protocol.info, record_data->last_rank_info);
	unsigned int send_length = sizeof(protocol);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, send_length);
	}
}

void PlatformBattle::SendRecordNotice(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCPlatformBattleRecordNotice protocol;
	protocol.is_first_times = 1;
	protocol.lose_record_flag = 0;

	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id, false, false);
	if (record_data != NULL)
	{
		protocol.is_first_times = record_data->is_first_times;
		protocol.lose_record_flag = record_data->lose_record_flag;
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
	}
}

void PlatformBattle::SetClientNotice(int role_id, int type, int param)
{
	CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id, false);
	if (record_data != NULL)
	{
		if (type == Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE)
		{
			record_data->is_first_times = 0;
		}
		
		if(type== Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE_2)
		{
			record_data->lose_record_flag &= ~(1 << param);
		}

		this->SendRecordNotice(role_id);
	}
}

void PlatformBattle::ClearLastRankRecord()
{
	std::vector<RetCommonSaveData> all_data = m_record_save_data->GetAllData();
	for (int i = 0; i < (int)all_data.size(); ++i)
	{
		RetCommonSaveData & get_data = all_data[i];
		CommonSavePlatformBattleReceordData & record_data_item = get_data.data.platform_battle_record_data;
		record_data_item.is_first_times = 1;
		ArrayCallReset(record_data_item.last_rank_info);
		m_record_save_data->SetData(get_data.key, get_data.data);
	}
}

void PlatformBattle::ClearRecord()
{
	m_record_save_data->ClearAllData();
	m_record_save_data->Save();
}

void PlatformBattle::SyncPartnerData(Role * role)
{
	if(NULL == role) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role->GetUID());
	if (role_info_iter == m_role_info.end())
	{
		return;
	}
	PlatformBattleRoleInfo & role_info = role_info_iter->second;  //这玩家占领擂台信息
	for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator role_play_info_iter = role_info.platform_play_data.begin(); role_play_info_iter != role_info.platform_play_data.end(); ++role_play_info_iter)
	{
		const PlatformBattleRoleBattleInfoItem & role_play_info_item = role_play_info_iter->second;
		PlatformBattlePlatformInfo & platform_info = m_platform_info[role_play_info_iter->first];

		int rank_index = role_play_info_item.rank - 1;

		PlatformBattleRoleData * rank_item_role_data_ptr = platform_info.GetPlatformBattleRoleData(rank_index);
		if (!rank_item_role_data_ptr)
		{
			continue;
		}

		PlatformBattleRoleData & rank_item_role_data = *rank_item_role_data_ptr;
		if (rank_item_role_data.role_id != role->GetUID())
		{
			continue;
		}

		platform_info.SetPlatformBattleRankItemInfoChange(rank_index);

		std::set<int> partner_list;
		for (int i = 0; i < (int)role_play_info_item.battling_partner.size(); ++i)
		{
			partner_list.insert(role_play_info_item.battling_partner[i].first);
		}

		for (int i = 0; i < ARRAY_LENGTH(rank_item_role_data.battling_partner); ++i)
		{
			rank_item_role_data.battling_partner[i].Reset();
		}

		for (int i = 0; i < (int)role_play_info_item.battling_partner.size(); ++i)
		{
			int partner_id = role_play_info_item.battling_partner[i].first;
			int pos = role_play_info_item.battling_partner[i].second;
			if (partner_id != 0 && role->GetRoleModuleManager()->GetPartner()->GetPlatformBattleCharacterData(partner_id, NULL, rank_item_role_data.battling_partner[i], partner_list))
			{
				rank_item_role_data.battling_partner[i].pos = pos;
			}
		}
	}
}

void PlatformBattle::SyncPetData(int role_id, const PlatformBattlePet & platform_battle_pet_data)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role_id);
	if (role_info_iter == m_role_info.end())
	{
		return;
	}
	PlatformBattleRoleInfo & role_info = role_info_iter->second;  //这玩家占领了哪些擂台信息

	int platform_num = 0;
	int rank = 0;
	for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator role_play_info_iter = role_info.platform_play_data.begin(); role_play_info_iter != role_info.platform_play_data.end(); ++role_play_info_iter)
	{
		const PlatformBattleRoleBattleInfoItem & role_play_info_item = role_play_info_iter->second;
		for (int i = 0; i < (int)role_play_info_item.battling_pet.size(); ++i)
		{
			if (role_play_info_item.battling_pet[i].first == platform_battle_pet_data.item_unique_id)
			{
				platform_num = role_play_info_item.platform_number;
				rank = role_play_info_item.rank;
				break;
			}
		}

		if (platform_num != 0)
		{
			break;
		}
	}

	PlatformBattlePlatformInfo & platform_info = m_platform_info[platform_num];
	int rank_index = rank - 1;
	PlatformBattleRoleData * rank_item_role_data_ptr = platform_info.GetPlatformBattleRoleData(rank_index);
	if (!rank_item_role_data_ptr)
	{
		return;
	}

	PlatformBattleRoleData & rank_item_role_data = *rank_item_role_data_ptr;   //擂台排行榜上，这项出战信息
	if (rank_item_role_data.role_id != role_id)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(rank_item_role_data.battling_pet); ++i)
	{
		if (rank_item_role_data.battling_pet[i].item_unique_id == platform_battle_pet_data.item_unique_id)
		{
			PlatformBattlePet old_data = rank_item_role_data.battling_pet[i];
			rank_item_role_data.battling_pet[i] = platform_battle_pet_data;

			rank_item_role_data.battling_pet[i].pos = old_data.pos;
			rank_item_role_data.battling_pet[i].pet_battle_data.pet_exclusive_skill_disable_flag = old_data.pet_battle_data.pet_exclusive_skill_disable_flag;			
			if (rank_item_role_data.battling_pet[i].pet_battle_data.lianyao_pet_id == old_data.pet_battle_data.lianyao_pet_id)
			{
				rank_item_role_data.battling_pet[i].pet_battle_data.lian_yao_exclusive_skill_disable_flag = old_data.pet_battle_data.lian_yao_exclusive_skill_disable_flag;
			}

			platform_info.SetPlatformBattleRankItemInfoChange(rank_index);

			break;
		}
	}	
}

void PlatformBattle::SynRoleInfo(Role * role)
{
	if(NULL == role) return;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.find(role->GetUID());
	if (role_info_iter == m_role_info.end())
	{
		return;
	}
	PlatformBattleRoleInfo & role_info = role_info_iter->second;  //这玩家占领擂台信息
	for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator role_play_info_iter = role_info.platform_play_data.begin(); role_play_info_iter != role_info.platform_play_data.end(); ++role_play_info_iter)
	{
		const PlatformBattleRoleBattleInfoItem & role_play_info_item = role_play_info_iter->second;
		PlatformBattlePlatformInfo & platform_info = m_platform_info[role_play_info_iter->first];

		int rank_index = role_play_info_item.rank - 1;
		if (rank_index < 0 || rank_index >= (int)platform_info.GetRankItemSize())
		{
			continue;
		}

		PlatformBattleRoleData * rank_item_role_data_ptr = platform_info.GetPlatformBattleRoleData(rank_index);
		if (!rank_item_role_data_ptr)
		{
			continue;
		}

		PlatformBattleRoleData & rank_item_role_data = *rank_item_role_data_ptr;   //擂台排行榜上，这项出战信息
		if (rank_item_role_data.role_id != role->GetUID())
		{
			continue;
		}

		rank_item_role_data.role_pet_train_skill_level = role->GetRoleModuleManager()->GetTrain()->GetRolePetTrainSkillLevel();
		rank_item_role_data.role_level = role->GetLevel();
		rank_item_role_data.avatar_type = role->GetAvatarType();
		rank_item_role_data.headshort_id = role->GetHeadshotID();
		rank_item_role_data.prof = role->GetProfession();
		role->GetName(rank_item_role_data.role_name);

		platform_info.SetPlatformBattleRankItemInfoChange(rank_index);
	}
}

void PlatformBattle::SetRoleInfo(int type, const std::vector<MsgPlatformBattleRoleInfoItemList> & vec)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (type == PLATFORM_BATTLE_SYNC_ROLE_INFO_DEFAULT)
	{
		this->ClearRoleInfo();
	}
	else if (type == PLATFORM_BATTLE_SYNC_ROLE_INFO_UPDATE)
	{
		for (int index = 0; index < (int)vec.size(); ++index)
		{
			const MsgPlatformBattleRoleInfoItemList & curr = vec[index];
			int role_id = curr.role_id;
			
			for (std::map<int, PlatformBattleRoleBattleInfoItem >::const_iterator iter = curr.role_info_items.begin(); iter != curr.role_info_items.end(); ++iter)
			{
				const PlatformBattleRoleBattleInfoItem & curr_item = iter->second;
				if (curr_item.rank == 0)
				{
					this->RemoveRoleInfoMapItemByRoleId(curr_item.platform_number, role_id);
				}
				else
				{
					MapAddOrUpdate(m_role_info[role_id].platform_play_data, curr_item.platform_number, curr_item);
				}
			}

			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(curr.role_id);
			if (role != NULL)
			{
				this->SendRoleInfo(role);
			}
		}
	}
}

void PlatformBattle::SyncRoleInfoHiddenToGameworld(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo protocol;
	protocol.type = PLATFORM_BATTLE_SYNC_ROLE_INFO_DEFAULT;
	protocol.all_role_info.clear();
	protocol.all_role_info.reserve(m_role_info.size());
	InternalComm::Instance().SendSerializeMsgToGameThroughCross(&protocol, server_id);

	protocol.type = PLATFORM_BATTLE_SYNC_ROLE_INFO_UPDATE;
	for (std::map<int, PlatformBattleRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		MsgPlatformBattleRoleInfoItemList item_list;
		item_list.role_id = iter->first;
		item_list.role_info_items = iter->second.platform_play_data;
		protocol.all_role_info.push_back(item_list);
		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&protocol, server_id);
	}
}

void PlatformBattle::ChangeRoleInfoHiddenToGameworld2(int role_id, const std::vector<int> & platform_num_vec)
{
	for (int i = 0; i < (int)platform_num_vec.size(); ++i)
	{
		this->ChangeRoleInfoHiddenToGameworld(role_id, platform_num_vec[i]);
	}
}

void PlatformBattle::ChangeRoleInfoHiddenToGameworld(int role_id, int platform_num)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int server_id = UserID(role_id).db_index;

	static crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo protocol;
	protocol.type = PLATFORM_BATTLE_SYNC_ROLE_INFO_UPDATE;
	protocol.all_role_info.clear();

	MsgPlatformBattleRoleInfoItemList send_item;
	send_item.role_id = role_id;
	std::map<int, PlatformBattleRoleInfo>::iterator iter = m_role_info.find(role_id);
	if (iter != m_role_info.end())			
	{
		const PlatformBattleRoleInfo & role_info = iter->second;

		std::map<int, PlatformBattleRoleBattleInfoItem> ::const_iterator battle_info_iter = role_info.platform_play_data.find(platform_num);
		battle_info_iter != role_info.platform_play_data.end() ?  //找到了玩家信息，但找不到这个擂台信息就是是删除
			MapAddOrUpdate(send_item.role_info_items, platform_num, battle_info_iter->second) :
			MapAddOrUpdate(send_item.role_info_items, platform_num, PlatformBattleRoleBattleInfoItem(platform_num));
	}
	else  //玩家信息找不到就发送删除
	{
		MapAddOrUpdate(send_item.role_info_items, platform_num, PlatformBattleRoleBattleInfoItem(platform_num));
	}

	protocol.all_role_info.push_back(send_item);
	if (!protocol.all_role_info.empty())
	{
		InternalComm::Instance().SendSerializeMsgToGameThroughCross(&protocol, server_id);
	}
}

int PlatformBattle::FetchAchievementInfoReward(Role * role, int type, int seq)
{
	int role_id = role->GetUID();

	const PlatformBattleAchievementItemItemCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementItemItemCfg(type, seq);
	if (cfg == NULL)
	{
		return __LINE__;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id, false);
	if (achievement_data == NULL)
	{
		return __LINE__;
	}

	CommonSavePlatformBattleAchievementItem * achievement_item_data =achievement_data->GetCommonSavePlatformBattleAchievementItem(type);
	if (achievement_item_data == NULL)
	{
		return __LINE__;
	}
	int process_num = achievement_item_data->num;
	if (IsPlatformBattleRankAchievementType(type))
	{
		if (process_num <= 0 || process_num > cfg->param)
		{
			return __LINE__;
		}
	}
	else
	{
		if (process_num < cfg->param)
		{
			return __LINE__;
		}
	}

	if (seq < 0 || seq >= (int)(sizeof(achievement_item_data->fetch_flag) * 8))
	{
		return __LINE__;
	}

	if (achievement_item_data->fetch_flag & 1 << seq)
	{
		return __LINE__;
	}

	if (!cfg->rewards.empty())
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPutList((short)cfg->rewards.size(), &cfg->rewards[0]))
		{
			role->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return __LINE__;
		}
	}

	achievement_item_data->fetch_flag |= 1 << seq;
	if (!cfg->rewards.empty())
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->rewards.size(), &cfg->rewards[0], PUT_REASON_PLATFORM_ACHIEVEMENT_REWARD);
	}
	
	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->jifen, cfg->jifen_type, __FUNCTION__);

	this->SendAchievementInfo(role_id);
	
	return 0;
}

void PlatformBattle::ResetAchievement()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_role_achievement_save_data->ClearAllData();
	m_role_achievement_save_data->Save();
}

void PlatformBattle::OnRankChange(int role_id, int platform_number, int rank, int old_rank)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (rank != 0 && rank < old_rank)
	{
		this->OnRankUp(role_id, platform_number, rank);
	}

	if (rank == 1)
	{
		this->OnTop1(role_id, platform_number);
	}
}

void PlatformBattle::OnRankUp(int role_id, int platform_number, int rank)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_ANY_TOP_X ,PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_X,PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_X,
		PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_X,PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_X,PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_X,
	};

	PlatformBattleRoleInfo * role_info = this->GetPlatformBattleRoleInfo(role_id);
	if (role_info == NULL)
	{
		return;
	}

	bool need_send = false;
	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		need_send = achievement_data->SetTypeRankProcess(type, rank);
	}

	if (need_send)
		this->SendAchievementInfo(role_id);
}

void PlatformBattle::OnTop1(int role_id, int platform_number)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_BA_ZHU_TOP_1_NUM,PLATFORM_BATTLE_ACHIEVEMENT_QING_LONG_TOP_1_NUM,	PLATFORM_BATTLE_ACHIEVEMENT_BAI_HU_TOP_1_NUM,
		PLATFORM_BATTLE_ACHIEVEMENT_ZHU_QUE_TOP_1_NUM,PLATFORM_BATTLE_ACHIEVEMENT_XUAN_WU_TOP_1_NUM, };

	PlatformBattleRoleInfo * role_info = this->GetPlatformBattleRoleInfo(role_id);
	if (role_info == NULL)
	{
		return;
	}

	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		int count = 0;
		for (std::map<int, PlatformBattleRoleBattleInfoItem>::const_iterator iter = role_info->platform_play_data.begin(); iter != role_info->platform_play_data.end(); ++iter)
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info_item = iter->second;

			if (!achievement_cfg->IsInPlatformList(role_battle_info_item.platform_number) || role_battle_info_item.rank != 1)
			{
				continue;
			}

			count += 1;
		}

		achievement_data->SetTypeProcess(type, count);
	}	

	this->SendAchievementInfo(role_id);
}

void PlatformBattle::OnAckSucc(int role_id, int platform_number)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_ANY_ACK_TIMES };
	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		achievement_data->AddTypeProcess(type);
	}

	this->SendAchievementInfo(role_id);
}

void PlatformBattle::OnDefSucc(int role_id, int platform_number)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_ANY_DEF_TIMS };

	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		achievement_data->AddTypeProcess(type);
	}

	this->SendAchievementInfo(role_id);
}

void PlatformBattle::OnPartnerAndPetNumChange(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	PlatformBattleRoleInfo * role_info = this->GetPlatformBattleRoleInfo(role_id);
	if (role_info == NULL)
	{
		return;
	}

	const PlatformBattleAchievementCfg * partner_num_achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(PLATFORM_BATTLE_ACHIEVEMENT_PARTNER_NUM);
	const PlatformBattleAchievementCfg * pet_num_achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(PLATFORM_BATTLE_ACHIEVEMENT_PET_NUM);

	int partner_num = 0;
	int pet_num = 0;
	for (std::map<int, PlatformBattleRoleBattleInfoItem>::const_iterator iter = role_info->platform_play_data.begin(); iter != role_info->platform_play_data.end(); ++iter)
	{
		const PlatformBattleRoleBattleInfoItem & role_battle_info_item = iter->second;

		if (partner_num_achievement_cfg && partner_num_achievement_cfg->IsInPlatformList(role_battle_info_item.platform_number))
		{
			partner_num += (int)role_battle_info_item.battling_partner.size();
		}

		if (pet_num_achievement_cfg && pet_num_achievement_cfg->IsInPlatformList(role_battle_info_item.platform_number))
		{
			pet_num += (int)role_battle_info_item.battling_pet.size();
		}
	}

	achievement_data->SetTypeProcess(PLATFORM_BATTLE_ACHIEVEMENT_PARTNER_NUM,partner_num);
	achievement_data->SetTypeProcess(PLATFORM_BATTLE_ACHIEVEMENT_PET_NUM, pet_num);
}

void PlatformBattle::OnTop1DayChange(int role_id, int platform_number, int day_count)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	bool need_send = false;

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_TOP_1_DAY };
	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		need_send = achievement_data->SetTypeProcess(type, day_count);
	}

	if(need_send)
		this->SendAchievementInfo(role_id);
}

void PlatformBattle::OnWinHighCapacity(int role_id, int platform_number)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id);
	if (achievement_data == NULL)
	{
		return;
	}

	int type_list[] = { PLATFORM_BATTLE_ACHIEVEMENT_CAPACITY_TIMES };
	for (int index = 0; index < ARRAY_LENGTH(type_list); ++index)
	{
		int type = type_list[index];
		const PlatformBattleAchievementCfg * achievement_cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattleAchievementCfg(type);
		if (achievement_cfg == NULL)
		{
			continue;
		}

		if (!achievement_cfg->IsInPlatformList(platform_number))
		{
			continue;
		}

		achievement_data->AddTypeProcess(type);
	}

	this->SendAchievementInfo(role_id);
}

void PlatformBattle::SendAchievementInfo(int role_id, bool is_hidden_to_game)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCPlatformBattleAchievementInfo protocol;
	for (int i = 0; i < PLATFORM_BATTLE_ACHIEVEMENT_MAX; ++i)
	{
		protocol.achievement_info[i].Reset();
	}

	CommonSavePlatformBattleAchievementData * achievement_data = this->GetCommonSavePlatformBattleAchievementData(role_id, false, false);
	if (achievement_data != NULL)
	{
		for (int i = 0; i < PLATFORM_BATTLE_ACHIEVEMENT_MAX; ++i)
		{
			protocol.achievement_info[i] = achievement_data->achievement_info[i];
		}
	}

	if (!is_hidden_to_game)			
	{
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role != NULL)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
		}
	}
	else
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
	}
}

PlatformBattle::PlatformBattle()
{
	m_load_flag = false;
	m_next_save_time = 0;
	m_change_flag = false;
	m_record_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_RECORD);
	m_role_achievement_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_ACHIEVEMENT_DATA);
	m_other_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_PLATFORM_BATTLE_OTHER_DATA);
	assert(m_record_save_data != NULL);
	assert(m_role_achievement_save_data != NULL);
	assert(m_other_save_data != NULL);
}

PlatformBattle::~PlatformBattle()
{
}

void PlatformBattle::Save(bool is_stop)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (is_stop == false && m_change_flag == false)
	{
		return;
	}

	for (std::map<int, PlatformBattlePlatformInfo>::iterator iter = m_platform_info.begin(); iter != m_platform_info.end(); ++iter)
	{
		PlatformBattleParam platform_battle_param;
		platform_battle_param.count = 0;

		PlatformBattlePlatformInfo & rank_info = iter->second;

		int insert_begin_index = rank_info.exist_num;
		for (int i = 0; i < (int)rank_info.GetRankItemSize() && platform_battle_param.count < PLATFORM_BATTLE_RANK_ITEM_MAX; ++i)
		{
			const PlatformBattleRankItemInfoNode * save_node_ptr = rank_info.GetPlatformBattleRankItemInfoNode(i);
			if (save_node_ptr == NULL)
			{
				continue;
			}

			const PlatformBattleRankItemInfoNode & save_node = *save_node_ptr;
			PlatformBattleParam::PlatformBattleSaveEntry & save_entry = platform_battle_param.data_list[platform_battle_param.count];
			save_entry.rank_item.platform_number = iter->first;
			save_entry.rank_item.platform_rank = i + 1;

			if (save_node.is_dirty)
			{
				save_entry.change_state = i < insert_begin_index ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
				PB_PlatformBattleRoleData * pb_data = (PB_PlatformBattleRoleData *)ProtobufMgr::Instance()->GetProtoMessage(PROTO_PLATFORM_ROLE_DATA);
				if (NULL == pb_data)
				{
					continue;
				}
				if (!save_node.rank_item.GetPBData(pb_data))
				{
#ifdef _DEBUG
					assert(0);
#endif
					continue;
				}

				if (!pb_data->SerializeToArray(save_entry.rank_item.role_data_buffer, sizeof(save_entry.rank_item.role_data_buffer)))
				{
#ifdef _DEBUG
					assert(0);
#endif
					continue;
				}
				save_entry.rank_item.role_data_buffer_length = pb_data->GetCachedSize();

				platform_battle_param.count += 1;
			}
		}

		for (int i = (int)rank_info.GetRankItemSize(); i < rank_info.exist_num && platform_battle_param.count < PLATFORM_BATTLE_RANK_ITEM_MAX; ++i)
		{
			int rank = i + 1;
			PlatformBattleParam::PlatformBattleSaveEntry & save_entry = platform_battle_param.data_list[platform_battle_param.count];
			save_entry.rank_item.platform_number = iter->first;
			save_entry.rank_item.platform_rank = rank;
			save_entry.change_state = structcommon::CS_DELETE;
			platform_battle_param.count += 1;
		}

		if (platform_battle_param.count > 0)
		{
			RMISavePlatformBattleBackObjectImpl * impl = new RMISavePlatformBattleBackObjectImpl();
			RMIGlobalClient ac;
			ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

			if (ac.SavePlatformBattleAsyn(platform_battle_param, impl))
			{
				this->ClearDirtyMark(iter->first);
			}
		}
	}

	m_change_flag = false;
}

void PlatformBattle::ClearDirtyMark(int platform_num)
{
	std::map<int, PlatformBattlePlatformInfo>::iterator iter = m_platform_info.find(platform_num);

	if (iter == m_platform_info.end())
	{
		return;
	}

	PlatformBattlePlatformInfo & platform_info = iter->second;
	for (std::vector<PlatformBattleRankItemInfoNode>::iterator platform_rank_iter = platform_info.rank_list.begin(); platform_rank_iter != platform_info.rank_list.end(); ++platform_rank_iter)
	{
		platform_rank_iter->is_dirty = false;
	}
	platform_info.exist_num = (int)platform_info.GetRankItemSize();
}

void PlatformBattle::CheckAllPlatform()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int platform_number = 1; platform_number <= PLATFORM_BATTLE_PLATFORM_NUM; ++platform_number)
	{
		PlatformBattlePlatformInfo & platform_info = m_platform_info[platform_number];
		platform_info.platform_num = platform_number;

		for (int robot_data_index = 0; robot_data_index < PLATFORM_BATTLE_RANK_ITEM_MAX; ++robot_data_index)
		{
			PlatformBattleRobotInfo robot_info_item;
			robot_info_item.robot_role_avatar_type = RandomNum(AVATAR_TYPE_INVALID + 1, AVATAR_TYPE_MAX);
			robot_info_item.robot_role_prof = AppearanceConfig::Instance().RandProfessionByAvatarType(robot_info_item.robot_role_avatar_type, 1);
			ROBOTCONFIG->GetRandomName(SEX_ARRAY[robot_info_item.robot_role_avatar_type], robot_info_item.robot_role_name, false);
			ROBOTCONFIG->GetPlatformBattleRotbotCfg(platform_number, robot_data_index, &robot_info_item);
			platform_info.robot_infos.push_back(robot_info_item);
		}

		platform_info.exist_num = platform_info.GetRankItemSize();

		int robot_count = 0;
		for (int rank_index = 0; rank_index < platform_info.GetRankItemSize(); ++rank_index)
		{
			int rank = rank_index + 1;
			PlatformBattleRoleData * rank_item_role_data_ptr = platform_info.GetPlatformBattleRoleData(rank_index);
			if (!rank_item_role_data_ptr)
			{
				continue;
			}

			PlatformBattleRoleData & rank_item_role_data = *rank_item_role_data_ptr;   
			if (rank_item_role_data.role_id <= 0)			
			{
				robot_count += 1;
				rank_item_role_data.role_id = -platform_number * 10000 - robot_count;
				rank_item_role_data.param1 = robot_count;
			}

			if (rank == 1 && rank_item_role_data.top1_time == 0)
			{
				rank_item_role_data.top1_time = (unsigned int)EngineAdapter::Instance().Time();
			}

			this->UpdateRoleInfoMapItem(platform_number, rank, rank_item_role_data);
		}

		platform_info.AddRobot();
	}
}

void PlatformBattle::RemoveRoleInfoMapItemByRoleId(int platform_num, int role_id)
{
	PlatformBattleRoleData role_info_data;
	role_info_data.role_id = role_id;
	this->UpdateRoleInfoMapItem(platform_num, 0, role_info_data);
}

void PlatformBattle::RemoveRoleInfoMapItem(int platform_num, const PlatformBattleRoleData & rank_item)
{
	this->UpdateRoleInfoMapItem(platform_num, 0, rank_item);
}

void PlatformBattle::UpdateRoleInfoMapItem(int platform_num, int rank, const PlatformBattleRoleData & rank_item)
{
	if (rank_item.role_id <= 0)
	{
		return;  //小于0是机器人
	}

	int role_id = rank_item.role_id;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);

	PlatformBattleRoleInfo & role_info = m_role_info[role_id];
	std::map<int, PlatformBattleRoleBattleInfoItem>::iterator  role_battle_info_item_iter = role_info.platform_play_data.find(platform_num);

	int old_rank = INT_MAX;
	bool pet_and_partner_num_change = true;

	PlatformBattleRoleBattleInfoItem update_item(platform_num, rank);
	update_item.SetPetAndPartnerData(rank_item);

	if (role_battle_info_item_iter != role_info.platform_play_data.end())
	{
		PlatformBattleRoleBattleInfoItem & old_item = role_battle_info_item_iter->second;
		old_rank = old_item.rank;

		for (int i = 0; i < (int)old_item.battling_pet.size(); ++i)
		{
			if (role != NULL)
			{
				role->GetRoleModuleManager()->GetPet()->SetPetPlatformBattleState(old_item.battling_pet[i].first, false);
			}			
		}

		for (int i = 0; i < (int)old_item.battling_partner.size(); ++i)
		{
			if (role != NULL)
			{
				role->GetRoleModuleManager()->GetPartner()->SetPartnerPlatformBattleState(old_item.battling_partner[i].first, false);
			}
		}

		pet_and_partner_num_change = old_item.battling_pet.size() < update_item.battling_pet.size() || old_item.battling_partner.size() < update_item.battling_partner.size();		
	}

	if (rank <= 0 && role_battle_info_item_iter != role_info.platform_play_data.end())
	{
		role_info.platform_play_data.erase(role_battle_info_item_iter);
	}
	else
	{
		for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
		{
			if (role != NULL)
			{
				role->GetRoleModuleManager()->GetPet()->SetPetPlatformBattleState(rank_item.battling_pet[i].item_unique_id, true);
				role->GetRoleModuleManager()->GetPartner()->SetPartnerPlatformBattleState(rank_item.battling_partner[i].partner_id, true);
			}
		}
		
		MapAddOrUpdate(role_info.platform_play_data, platform_num, update_item);
	}

	this->OnRankChange(role_id, platform_num, rank, old_rank);
	if (pet_and_partner_num_change) this->OnPartnerAndPetNumChange(role_id);


	if (role != NULL)
	{
		this->SendRoleInfo(role);
	}

	this->ChangeRoleInfoHiddenToGameworld(rank_item.role_id, platform_num);
}

bool PlatformBattle::GetBattleData(int platform_num, int challenge_rank, int ARG_OUT & def_uid, std::vector<PetBattleData>ARG_OUT & pet_battle_data, std::vector<int>ARG_OUT & pet_pos, std::vector<PartnerBattleData>ARG_OUT & partner_battle_data, std::vector<int>ARG_OUT & partner_pos)
{
	const PlatformBattlePlatformInfo & platform_info = m_platform_info[platform_num];

	int challenge_rank_index = challenge_rank - 1;
	PlatformBattleRoleData  role_data = platform_info.GetPlatformBattleRoleDataTemp(challenge_rank_index);
	def_uid = role_data.role_id;

	if (def_uid == 0)
	{
		return false;
	}
	
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		if (role_data.battling_pet[i].IsValid())
		{
			const PlatformBattlePet & pet = role_data.battling_pet[i];
			pet_battle_data.push_back(pet.pet_battle_data);
			pet_pos.push_back(pet.pos);
		}

		if (role_data.battling_partner[i].IsValid())
		{
			const PlatformBattlePartner & partner = role_data.battling_partner[i];
			partner_battle_data.push_back(partner.partner_battle_data);
			partner_pos.push_back(partner.pos);
		}
	}

	return true;
}

static bool PlatformBattleRewardPriority(const PlatformBattleRoleBattleInfoItem & role_info_item1, const PlatformBattleRoleBattleInfoItem & role_info_item2)
{
	const PlatformBattlePlatformCfg * cfg1=LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattlePlatformCfg(role_info_item1.platform_number);
	const PlatformBattlePlatformCfg * cfg2 = LOGIC_CONFIG->GetPlatformBattleConfig()->GetPlatformBattlePlatformCfg(role_info_item2.platform_number);

	if (cfg1 && cfg2)
	{
		if (cfg1->platform_level < cfg2->platform_level)
			return true;
		if (cfg1->platform_level > cfg2->platform_level)
			return false;
	}

	if (role_info_item1.rank < role_info_item2.rank)
		return true;

	if (role_info_item1.rank > role_info_item2.rank)
		return false;

	return false;
}

void PlatformBattle::OnTimeOutSettleAccounts()
{
	this->ClearLastRankRecord();
	this->SettleAccounts();
	this->ResetPlatformBattle();
	this->ResetAchievement();

	SCPlatformBattleSettleAccountNotice notice;
	World::GetInstWorld()->SendToALLGameAllGateway(&notice, sizeof(notice));
}

void PlatformBattle::SettleAccounts()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();

	for (std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = m_role_info.begin(); role_info_iter != m_role_info.end(); ++role_info_iter)
	{
		int role_id = role_info_iter->first;

		gamelog::g_log_platform_battle.printf(LL_INFO, " PlatformBattle::SettleAccounts role_id:%d", role_id);

		UserCacheNode * usercache = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL == usercache)
		{
			gamelog::g_log_platform_battle.printf(LL_INFO, " PlatformBattle::SettleAccounts role_id:%d usercache_is_null", role_id);
			continue;
		}

		if (now - usercache->last_online_timestamp > SECOND_PER_WEEK)
		{
			gamelog::g_log_platform_battle.printf(LL_INFO, " PlatformBattle::SettleAccounts role_id:%d  is_week_logout", role_id);
			continue;
		}

		std::vector<PlatformBattleRoleBattleInfoItem> role_platform_reward_priority_vec;		//超过3个擂台，只给3个擂台奖励
		const PlatformBattleRoleInfo & role_info = role_info_iter->second;
		for (std::map<int, PlatformBattleRoleBattleInfoItem>::const_iterator iter = role_info.platform_play_data.begin(); iter != role_info.platform_play_data.end(); ++iter)
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info_item = iter->second;

			if (role_battle_info_item.platform_number == 0 || role_battle_info_item.rank == 0)
			{
				continue;
			}

			gamelog::g_log_platform_battle.printf(LL_INFO, " PlatformBattle::SettleAccounts role_id:%d platform_number:%d rank:%d", role_id, role_battle_info_item.platform_number, role_battle_info_item.rank);

			role_platform_reward_priority_vec.push_back(role_battle_info_item);
		}

		if (role_platform_reward_priority_vec.empty())
		{
			continue;
		}

		//发奖励
		std::sort(role_platform_reward_priority_vec.begin(), role_platform_reward_priority_vec.end(), PlatformBattleRewardPriority);

		CommonSavePlatformBattleReceordData * record_data = this->GetRoleRecordData(role_id);

		static MailContentParam contentparam; contentparam.Reset();
		int reward_count = 0;
		std::string reward_content2;
		for (int i = 0; i < (int)role_platform_reward_priority_vec.size() && i < PLATFORM_BATTLE_MAX_RANK_REWARD; ++i)
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info_item = role_platform_reward_priority_vec[i];

			if (record_data != NULL && i < ARRAY_LENGTH(record_data->last_rank_info))
			{
				record_data->last_rank_info[i].platform_num = role_battle_info_item.platform_number;
				record_data->last_rank_info[i].rank = role_battle_info_item.rank;
			}

			const PlatformBattleRewardCfg::RewardItemCfg * cfg = LOGIC_CONFIG->GetPlatformBattleConfig()->GetRankReward(role_battle_info_item.platform_number, role_battle_info_item.rank);
			if (cfg == NULL)
			{
				continue;
			}

			// 发邮件			
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += cfg->coin;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS] += cfg->credits;

			const std::vector<ItemConfigData> & rewards = cfg->reward_item;
			for (int k = 0; k < (int)rewards.size() && reward_count < MAX_ATTACHMENT_ITEM_NUM; ++k)
			{
				const ItemConfigData & reward = rewards[k];

				int same_index = -1;
				for (int check_same_index = 0; check_same_index < reward_count; ++check_same_index)
				{
					ItemDataWrapper & mail_item = contentparam.item_list[check_same_index];
					if (mail_item.item_id == reward.item_id)
					{
						same_index = check_same_index;
						break;
					}
				}


				if (same_index != -1)
				{
					ItemDataWrapper & mail_item = contentparam.item_list[same_index];
					mail_item.num += reward.num;
				}
				else
				{
					const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
					if (NULL == item_base) continue;

					ItemDataWrapper & mail_item = contentparam.item_list[reward_count++];
					mail_item.item_id = reward.item_id;
					mail_item.num = reward.num;
					mail_item.is_bind = reward.is_bind ? 1 : 0;
					mail_item.invalid_time = item_base->CalInvalidTime();
				}
			}

			char temp[1024] = { 0 };
			int len = SNPRINTF(temp, sizeof(temp), gamestring::g_platform_battle_reward_content2, role_battle_info_item.platform_number, role_battle_info_item.rank);
			if (len > 0)
			{
				reward_content2 += temp;
			}
			
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_platform_battle_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_platform_battle_reward_content, reward_content2.c_str());
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(role_info_iter->first, MAIL_REASON_PLATFORM_BATTLE, contentparam);
		}
	}
}

void PlatformBattle::ClearRoleInfo()
{
	std::map<int, PlatformBattleRoleInfo> tmp = m_role_info;
	m_role_info.clear();
	this->SyncRoleInfoHiddenToGameworld();

	for (std::map<int, PlatformBattleRoleInfo>::iterator role_info_iter = tmp.begin(); role_info_iter != tmp.end(); ++role_info_iter)
	{
		int role_id = role_info_iter->first;
		const PlatformBattleRoleInfo & role_info = role_info_iter->second;

		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (role)
		{
			role->GetRoleModuleManager()->GetPartner()->ClearPartnerPlatformBattleState();
			role->GetRoleModuleManager()->GetPet()->ClearPetPlatformBattleState();
			this->SendRoleInfo(role);
		}
	}
}

void PlatformBattle::ResetPlatformBattle()
{
	this->ClearRoleInfo();
	m_platform_info.clear();
	this->CheckAllPlatform();
	this->Save();
}

int PlatformBattle::CanJoinPlatformBattle(int role_id, int platform_number, int challenge_rank, std::set<unsigned long long> pet_unique_id, std::set<int> partner_ids)
{
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role == NULL)
	{
		return -__LINE__;
	}

	PlatformBattleRoleInfo & role_info = m_role_info[role_id];

	if (role_info.IsInPlatformBattle(platform_number, pet_unique_id, partner_ids))
	{
		role->NoticeNum(errornum::EN_PLATFORM_BATTLE_IN_OTHER_PLATFORM);
		return -__LINE__;
	}

	{
		std::map<int, PlatformBattleRoleBattleInfoItem>::iterator role_platform_info_iter = role_info.platform_play_data.find(platform_number);

		if (role_platform_info_iter != role_info.platform_play_data.end())
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info_item = role_platform_info_iter->second;
			if (role_battle_info_item.IsInvaild())
			{
				role_info.platform_play_data.erase(role_platform_info_iter);
				role_platform_info_iter = role_info.platform_play_data.end();
			}
		}

		if (role_platform_info_iter != role_info.platform_play_data.end())
		{
			const PlatformBattleRoleBattleInfoItem & role_battle_info_item = role_platform_info_iter->second;
			if (challenge_rank >= role_battle_info_item.rank)					//挑战自身已经守擂的擂台，要判断挑战排名
			{
				if (role != NULL)
				{
					role->NoticeNum(errornum::EN_PLATFORM_BATTLE_CANT_CHALLENGE_RANK);
				}
				return -__LINE__;		//挑战这个擂台名次比自己还是后，就返回
			}
		}
		else
		{
			if ((int)role_info.platform_play_data.size() >= PLATFORM_BATTLE_MAX_JOIN_NUM)							//挑战自身没有守擂的擂台，可能会增加守擂的数量。要判断
			{
				if (role != NULL)
				{
					role->NoticeNum(errornum::EN_PLATFORM_BATTLE_MAX_JOIN_NUM);
				}
				return -__LINE__;
			}
		}
	}

	return 0;
}

int  PlatformBattle::CheckArg(Role * role, const MsgPlatformBattlePlayItem(&arg_pet_play_info)[PLATFORM_BATTLE_PET_PARTNER_MAX], const MsgPlatformBattlePlayItem(&arg_partner_play_info)[PLATFORM_BATTLE_PET_PARTNER_MAX], std::set<unsigned long long>ARG_OUT & pet_unique_id, std::set<int>ARG_OUT & partner_ids)
{
	std::map<int, int> check_pos_map;

	//判断发过来的有没有重复的
	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		int pet_index = arg_pet_play_info[i].pet_index;
		if (pet_index == -1)
		{
			continue;
		}

		unsigned long long pet_uid = role->GetRoleModuleManager()->GetPet()->GetPetUniqueId(pet_index);
		if (pet_unique_id.count(pet_uid) != 0)
		{
			return -__LINE__;
		}
		pet_unique_id.insert(pet_uid);

		//检查位置
		int pet_pos = arg_pet_play_info[i].pet_battle_pos;
		if (pet_pos < 0 || pet_pos >= PLATFORM_BATTLE_PET_PARTNER_MAX * 2)
		{
			return -__LINE__;
		}

		int opposite_pos = (pet_pos >= PLATFORM_BATTLE_PET_PARTNER_MAX ? pet_pos - PLATFORM_BATTLE_PET_PARTNER_MAX : pet_pos + PLATFORM_BATTLE_PET_PARTNER_MAX);
		if (check_pos_map[pet_pos] != 0 || check_pos_map[opposite_pos] == PLATFORM_BATTLE_ARG_TYPE_PET)
		{
			return -__LINE__;
		}
		check_pos_map[pet_pos] = PLATFORM_BATTLE_ARG_TYPE_PET;
	}

	for (int i = 0; i < PLATFORM_BATTLE_PET_PARTNER_MAX; ++i)
	{
		int partner_id = arg_partner_play_info[i].partner_id;
		if (partner_id == 0)
		{
			continue;
		}

		if (partner_ids.count(partner_id) != 0)
		{
			return -__LINE__;
		}
		partner_ids.insert(partner_id);

		//检查位置
		int partner_pos = arg_partner_play_info[i].partner_battle_pos;
		if (partner_pos < 0 || partner_pos >= PLATFORM_BATTLE_PET_PARTNER_MAX * 2)
		{
			return -__LINE__;
		}

		int opposite_pos = (partner_pos >= PLATFORM_BATTLE_PET_PARTNER_MAX ? partner_pos - PLATFORM_BATTLE_PET_PARTNER_MAX : partner_pos + PLATFORM_BATTLE_PET_PARTNER_MAX);
		if (check_pos_map[partner_pos] != 0 || check_pos_map[opposite_pos] == PLATFORM_BATTLE_ARG_TYPE_PARTNER)
		{
			return -__LINE__;
		}

		check_pos_map[partner_pos] = PLATFORM_BATTLE_ARG_TYPE_PARTNER;
	}

	return 0;
}

CommonSavePlatformBattleAchievementData * PlatformBattle::GetCommonSavePlatformBattleAchievementData(int role_id, bool is_auto_create, bool set_data_change)
{
	CommonSavePlatformBattleAchievementData * save_data = m_role_achievement_save_data->GetTypeData<CommonSavePlatformBattleAchievementData>(role_id, is_auto_create);
	if (save_data != NULL && set_data_change)
	{
		m_role_achievement_save_data->SetDataChange(role_id);
	}

	return save_data;
}

CommonSavePlatformBattleOtherData * PlatformBattle::GetCommonSavePlatformBattleOtherData()
{
	CommonSavePlatformBattleOtherData * save_data = m_other_save_data->GetTypeData<CommonSavePlatformBattleOtherData>(0);
	if (save_data != NULL)
	{
		m_other_save_data->SetDataChange(0);
	}

	assert(save_data != NULL);

	return save_data;
}


void RMIInitPlatformBattleBackObjectImpl::InitPlatformBattleRet(int ret, const PlatformBattleParam & param)
{
	if (ret < 0)
	{
		printf("%s Failed ret=%d", __FUNCTION__, ret);
		EngineAdapter::Instance().StopGame();
		return;
	}

	long long next_id_from = 0;

	for (int i = 0; i < param.count; ++i)
	{
		const PlatformBattleParam::PlatformBattleSaveEntry & data_item = param.data_list[i];
		if (data_item.inc_id > next_id_from)
		{
			next_id_from = data_item.inc_id;
		}

		if (data_item.rank_item.platform_rank != 0)
		{
			PB_PlatformBattleRoleData * pb_data = (PB_PlatformBattleRoleData *)ProtobufMgr::Instance()->GetProtoMessage(PROTO_PLATFORM_ROLE_DATA);
			if (NULL == pb_data)
			{
				continue;
			}
			if (!pb_data->ParseFromArray(data_item.rank_item.role_data_buffer, data_item.rank_item.role_data_buffer_length))
			{
				gamelog::g_log_serious_error.printf(LL_ERROR, "InitPlatformBattleRet parse protobuf failed! number[%d] rank[%d]",
								    data_item.rank_item.platform_number,
								    data_item.rank_item.platform_rank);

#ifdef _DEBUG
				assert(0);
#endif
				continue;
			}

			static PlatformBattleRoleData role_data;
			role_data.Reset();
			if (!role_data.SetDataFromPB(*pb_data))
			{
				gamelog::g_log_serious_error.printf(LL_ERROR, "InitPlatformBattleRet set from pb_data failed! number[%d] rank[%d]",
								    data_item.rank_item.platform_number,
								    data_item.rank_item.platform_rank);

#ifdef _DEBUG
				assert(0);
#endif

				continue;
			}

			PlatformBattle::Instance().InitPlatformInfo(data_item.rank_item.platform_number, data_item.rank_item.platform_rank, role_data);
		}
	}

	if (param.count > 0)
	{
		PlatformBattle::Instance().LoadPlatformBattleData(next_id_from);
	}
	else
	{
		PlatformBattle::Instance().LoadPlatformBattleDataSucc();
	}
}

PlatformBattlePlatformInfo::PlatformBattlePlatformInfo()
{
	platform_num = 0;
	exist_num = 0;

	for (int i = 0; i < PLATFORM_BATTLE_RANK_ITEM_MAX; ++i)
	{
		robot_number.insert(i + 1);
	}
}

bool PlatformBattlePlatformInfo::Add(int rank, const PlatformBattleRoleData & rank_item)
{
	const int rank_index = rank - 1;
	if (rank_index < 0 || rank_index >(int)rank_list.size())
	{
		return false;
	}

	gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | platform_num:%d role_id:%d rank:%d", __FUNCTION__, __LINE__, this->platform_num, rank_item.role_id, rank);

	PlatformBattleRankItemInfoNode rank_node;
	rank_node.rank_item = rank_item;
	rank_list.insert(rank_list.begin() + rank_index, rank_node);

	for (int start_index = rank_index; start_index < (int)rank_list.size(); ++start_index)
	{
		rank_list[start_index].is_dirty = true;

		if (start_index == 0)
		{
			rank_list[start_index].rank_item.top1_time = (unsigned int)EngineAdapter::Instance().Time();
		}
		else
		{
			rank_list[start_index].rank_item.top1_time = 0;
		}


		int temp_rank = start_index + 1;
		PlatformBattle::Instance().UpdateRoleInfoMapItem(this->platform_num, temp_rank, rank_list[start_index].rank_item);

		if (start_index != rank_index)
		{
			//插入那个不用打印
			gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | platform_num:%d role_id:%d curr_rank:%d rank change +1", __FUNCTION__, __LINE__,
				this->platform_num, rank_list[start_index].rank_item.role_id, temp_rank);
		}
	}

	PlatformBattle::Instance().SetChangeFlag();

	return true;
}

bool PlatformBattlePlatformInfo::SwapRank(int rank1, int rank2,const PlatformBattleRoleData * rank1_update_item)
{
	int rank1_index = rank1 - 1;
	int rank2_index = rank2 - 1;
	if (rank1_index < 0 || rank1_index >= (int)rank_list.size())
	{
		return false;
	}

	if (rank2_index < 0 || rank2_index >= (int)rank_list.size())
	{
		return false;
	}

	PlatformBattleRankItemInfoNode & rank_node1 = rank_list[rank1_index];
	if (rank1_update_item)
	{
		rank_node1.rank_item = *rank1_update_item;
	}

	PlatformBattleRankItemInfoNode & rank_node2 = rank_list[rank2_index];
	std::swap(rank_node1.rank_item, rank_node2.rank_item);
	rank_node1.is_dirty = true;
	rank_node2.is_dirty = true;

	if (rank1 == 1)
	{
		rank_node1.rank_item.top1_time = (unsigned int)EngineAdapter::Instance().Time();
	}

	if (rank2 == 1)
	{
		rank_node2.rank_item.top1_time = (unsigned int)EngineAdapter::Instance().Time();
	}

	gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | platform_num:%d role_id:%d rank:%d role_id2:%d rank2:%d",
		__FUNCTION__, __LINE__, this->platform_num, rank_node1.rank_item.role_id, rank1, rank_node2.rank_item.role_id, rank2);

	PlatformBattle::Instance().UpdateRoleInfoMapItem(this->platform_num, rank1, rank_node1.rank_item);
	PlatformBattle::Instance().UpdateRoleInfoMapItem(this->platform_num, rank2, rank_node2.rank_item);

	PlatformBattle::Instance().SetChangeFlag();

	return true;
}

void PlatformBattlePlatformInfo::RemoveRankNode(int rank)
{
	int rank_index = rank - 1;
	if (rank_index < 0 || rank_index >= (int)rank_list.size())
	{
		return;
	}

	gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d | platform_num:%d remove role_id:%d rank:%d", __FUNCTION__, __LINE__, this->platform_num, rank_list[rank_index].rank_item.role_id, rank);

	int role_id = rank_list[rank_index].rank_item.role_id;

	for (int start_index = rank_index; start_index < (int)rank_list.size() - 1; ++start_index)
	{
		rank_list[start_index] = rank_list[start_index + 1];
		rank_list[start_index].is_dirty = true;

		if (start_index == 0)
		{
			rank_list[start_index].rank_item.top1_time = (unsigned int)EngineAdapter::Instance().Time();
		}
		else
		{
			rank_list[start_index].rank_item.top1_time = 0;
		}

		gamelog::g_log_platform_battle.printf(LL_INFO, "%s %d |  platform_num:%d role_id:%d curr_rank_index:%d rank change -1", __FUNCTION__, __LINE__, this->platform_num, rank_list[start_index].rank_item.role_id, start_index + 1);

		int temp_rank = start_index + 1;
		PlatformBattle::Instance().UpdateRoleInfoMapItem(this->platform_num, temp_rank, rank_list[start_index].rank_item);
	}

	rank_list.pop_back();

	PlatformBattle::Instance().RemoveRoleInfoMapItemByRoleId(this->platform_num, role_id);
	PlatformBattle::Instance().SetChangeFlag();

	this->AddRobot();
}

void PlatformBattlePlatformInfo::RemoveRedundant(int num)
{
	while (this->GetRankItemSize() > num)
	{
		this->RemoveRankNode(this->GetRankItemSize());
	}
}

int PlatformBattlePlatformInfo::GetRankItemSize() const
{
	return (int)rank_list.size();
}

PlatformBattleRoleData * PlatformBattlePlatformInfo::GetPlatformBattleRoleData(int rank_index)
{
	if (rank_index < 0 || rank_index >= (int)rank_list.size())
	{
		return NULL;
	}

	PlatformBattleRoleData & item = rank_list[rank_index].rank_item;

	return &item;
}

PlatformBattleRankItemInfoNode * PlatformBattlePlatformInfo::GetPlatformBattleRankItemInfoNode(int rank_index)
{
	if (rank_index < 0 || rank_index >= (int)rank_list.size())
	{
		return NULL;
	}

	return &rank_list[rank_index];
}

PlatformBattleRoleData PlatformBattlePlatformInfo::GetPlatformBattleRoleDataTemp(int rank_index) const
{
	PlatformBattleRoleData ret;

	if (rank_index < 0 || rank_index >= (int)rank_list.size())
	{
		return ret;
	}

	const PlatformBattleRoleData & item = rank_list[rank_index].rank_item;
	if (!item.IsRobot())
	{
		return item;
	}

	int robot_data_index = item.GetRobotDataIndex();
	if (robot_data_index < 0 || robot_data_index >= (int)robot_infos.size())
	{
		return ret;
	}
	
	const PlatformBattleRobotInfo & robot_info_item = robot_infos[robot_data_index];


	ret.role_id = item.role_id;
	ret.role_level = robot_info_item.robot_role_level;
	ret.avatar_type = robot_info_item.robot_role_avatar_type;
	memcpy(ret.role_name, robot_info_item.robot_role_name, sizeof(ret.role_name));

	for (int i = 0; i < (int)robot_info_item.pet_robot_data.size() && i < ARRAY_LENGTH(ret.battling_pet); ++i)
	{
		PlatformBattlePet & pet_info = ret.battling_pet[i];

		pet_info.pet_id = robot_info_item.pet_robot_data[i].pet_data.battle_data.character_id;
		pet_info.pos = robot_info_item.pet_robot_data[i].pos;
		pet_info.pet_capability = robot_info_item.pet_robot_data[i].pet_capability;
		pet_info.pet_level = robot_info_item.pet_robot_data[i].pet_data.battle_data.level;
		pet_info.pet_strengthen_level = 0;
		memcpy(pet_info.pet_name, robot_info_item.pet_robot_data[i].pet_data.battle_data.name, sizeof(pet_info.pet_name));
		pet_info.pet_battle_data = robot_info_item.pet_robot_data[i].pet_data;
	}

	for (int i = 0; i < (int)robot_info_item.partner_robot_data.size() && i < ARRAY_LENGTH(ret.battling_partner); ++i)
	{
		PlatformBattlePartner & partner_info = ret.battling_partner[i];
		const PlatformBattleRobotPartnerItem & robot_partner_item = robot_info_item.partner_robot_data[i];
		partner_info.pos = robot_partner_item.pos;
		partner_info.level = robot_partner_item.partner_data.battle_data.level;
		partner_info.partner_id = robot_partner_item.partner_data.battle_data.character_id;
		partner_info.capability = robot_partner_item.partner_capability;
		partner_info.partner_battle_data= robot_partner_item.partner_data;  
	}

	return ret;
}

void PlatformBattlePlatformInfo::SetPlatformBattleRankItemInfoChange(int rank_index)
{
	PlatformBattleRankItemInfoNode * node=this->GetPlatformBattleRankItemInfoNode(rank_index);
	if (node)
	{
		node->is_dirty = true;
		PlatformBattle::Instance().SetChangeFlag();
	}	
}

void PlatformBattlePlatformInfo::AddRobot()
{
	if (this->GetRankItemSize() >= PLATFORM_BATTLE_RANK_ITEM_MAX)
	{
		return;
	}

	std::set<int> temp = robot_number;
	for (int rank_index = 0; rank_index < this->GetRankItemSize(); ++rank_index)
	{
		PlatformBattleRoleData & rank_item_role_data = rank_list[rank_index].rank_item;

		if (rank_item_role_data.IsRobot())
		{
			temp.erase(rank_item_role_data.param1);
		}
	}

	for (int i = this->GetRankItemSize(); i < PLATFORM_BATTLE_RANK_ITEM_MAX && !temp.empty(); ++i)
	{
		int param1 = *temp.begin();
		temp.erase(param1);

		PlatformBattleRoleData insert_item;
		insert_item.role_id = -platform_num * 1000 - param1;
		insert_item.param1 = param1;
		this->Add(this->GetRankItemSize() + 1, insert_item);
	}
}

bool PlatformBattleRankItemInfoNode::IsRobot() const
{
	return rank_item.role_id < 0;
}

PlatformBattleRoleInfo::PlatformBattleRoleInfo()
{
}

PlatformBattleRoleInfo::PlatformBattleRoleInfo(const MsgPlatformBattleRoleInfoItemList & role_info_list)
{
	this->platform_play_data = role_info_list.role_info_items;
}

bool PlatformBattleRoleInfo::IsInPlatformBattle(int exclude_platform_number, std::set<unsigned long long> pet_unique_ids, std::set<int> partner_ids)
{
	for (std::map<int, PlatformBattleRoleBattleInfoItem>::iterator iter = platform_play_data.begin(); iter != platform_play_data.end(); ++iter)
	{
		if (iter->first == exclude_platform_number)
		{
			continue;
		}

		const PlatformBattleRoleBattleInfoItem & role_play_info = iter->second;

		for (int i = 0; i < (int)role_play_info.battling_pet.size(); ++i)
		{
			if (pet_unique_ids.find(role_play_info.battling_pet[i].first) != pet_unique_ids.end())
			{
				return true;
			}
		}

		for (int i = 0; i < (int)role_play_info.battling_partner.size(); ++i)
		{
			if (partner_ids.find(role_play_info.battling_partner[i].first) != partner_ids.end())
			{
				return true;
			}
		}
	}

	return false;
}

int crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo::GetMessageType() const
{
	return header.msg_type;
}

bool crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo::Serialize(char * buffer, int buffer_size, int * out_length)
{
	if (buffer_size <= 0)
	{
		return false;
	}

	bool result = true;
	result = result && WriteStrN((char *)&header, sizeof(header), buffer, buffer_size, out_length);
	if (!result)
	{
		return false;
	}

	buffer += sizeof(header);
	buffer_size -= sizeof(header);

	TLVSerializer serializer;
	serializer.Reset(buffer, buffer_size);
	unsigned int count = all_role_info.size();
	result = result && serializer.Push(type);
	result = result && serializer.Push(count);

	for (int i = 0; i < (int)all_role_info.size(); ++i)
	{
		const MsgPlatformBattleRoleInfoItemList & curr = all_role_info[i];
		result = result && curr.Serialize(serializer);
	}

	*out_length += serializer.Size();

	return result;
}

bool crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo::Deserialize(const char * data, int data_length, int * out_offset)
{
	if (data_length <= 0)
	{
		return false;
	}

	bool result = true;
	result = result && ReadStrN((char *)&header, sizeof(header), sizeof(header), data, data_length, out_offset);
	if (!result)
	{
		return false;
	}

	data += sizeof(header);
	data_length -= sizeof(header);

	TLVUnserializer uns;
	uns.Reset(data, data_length);


	unsigned int count = 0;

	result = result && uns.Pop(&type);
	result = result && uns.Pop(&count);

	for (int i = 0; i < (int)count; ++i)
	{
		MsgPlatformBattleRoleInfoItemList curr;
		result = result && curr.Unserialize(uns);
		all_role_info.push_back(curr);
	}

	*out_offset = data_length;

	return result;
}
