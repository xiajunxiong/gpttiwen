#include "global/activity/activitymanager.hpp"
#include "servercommon/logdef.h"
#include "gamelog.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "global/usercache/usercache.hpp"

#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "bbl_impl_huan_jie_zhan_chang_tian_ti.hpp"

BBLImplHuanJieZhanChang::BBLImplHuanJieZhanChang()
{
}

BBLImplHuanJieZhanChang::~BBLImplHuanJieZhanChang()
{
}

void BBLImplHuanJieZhanChang::OnBattleFinish(battlegameprotocol::BattleGameFightResultAck * ack)
{
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	int fight_seq = ack->business_data.param1;
	int attacker_role_id = ack->business_data.param2;
	int defender_role_id = ack->business_data.param3;

	std::map<int, ActivityHuanJieZhanChangShangHaiInfo> shang_hai_info;
	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameFightResultAck::RoleInfo  & curr = ack->attacker_list.role_list[i];
		ActivityHuanJieZhanChangShangHaiInfo temp;
		temp.shanghai = curr.role_statistic.total_damage_to_enemy;
		temp.zhi_liao = curr.role_statistic.total_heal_other;
		temp.chengshou_shanghai = curr.role_statistic.total_injure_from_enemy;
		shang_hai_info[curr.uid] = temp;
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameFightResultAck::RoleInfo & curr = ack->defender_list.role_list[i];
		ActivityHuanJieZhanChangShangHaiInfo temp;
		temp.shanghai = curr.role_statistic.total_damage_to_enemy;
		temp.zhi_liao = curr.role_statistic.total_heal_other;
		temp.chengshou_shanghai = curr.role_statistic.total_injure_from_enemy;
		shang_hai_info[curr.uid] = temp;
	}

	long long unique_key = 0;
	int fight_round = ack->statistic_data.round_num;
	UniqueIDGenerator::Instance()->GenUniqueID(&unique_key);

	switch (ack->battle_result)
	{
		case RESULT_TYPE_ATTACKER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->attacker_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_WIN, ack->statistic_data.round_num, fight_seq, true, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 1, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}	

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, role_id, 1, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->defender_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL, ack->statistic_data.round_num, fight_seq, false, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 2, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, role_id, 2, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}
			}
			break;
		case RESULT_TYPE_DEFENDER_WIN:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->attacker_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL, ack->statistic_data.round_num, fight_seq, true, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 2, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, role_id, 2, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->defender_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_WIN, ack->statistic_data.round_num, fight_seq, false, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 1, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, role_id, 1, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}
			}
			break;

		default:
			{
				for (int i = 0; i < ack->attacker_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->attacker_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW, ack->statistic_data.round_num, fight_seq,true, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 0, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, 0, 0, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}

				for (int i = 0; i < ack->defender_list.role_count; ++i)
				{
					const battlegameprotocol::BattleGameFightResultAck::RoleInfo & role_info = ack->defender_list.role_list[i];
					int role_id = role_info.uid;
					int role_level = role_info.level;
					activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW, ack->statistic_data.round_num, fight_seq,false, shang_hai_info);

					UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(role_id);
					if (user_node)
					{
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_ROLE, role_id, user_node->role_name, NULL, user_node->profession, 0, NULL, NULL, fight_round, unique_key, role_level, 0, 0);
					}

					for (int pet_num = 0; pet_num < role_info.pet_num && pet_num < ARRAY_LENGTH(role_info.pet_id_list); ++pet_num)
					{
						int pet_id = role_info.pet_id_list[pet_num];
						int pet_level = role_info.pet_level_list[pet_num];
						LOG_QUICK12(gamelog::g_log_quick, LOG_TYPE_HUAN_JIE_PET, pet_id, NULL, NULL, 0, 0, NULL, NULL, fight_round, unique_key, pet_level, 0, 0);
					}
				}
			}
			break;
	}

	activity->RemoveFightItemInfo(fight_seq);
}

void BBLImplHuanJieZhanChang::OnBattleRunAway(battlegameprotocol::BattleGameRoleRunAway * ack)
{
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	std::map<int, ActivityHuanJieZhanChangShangHaiInfo> shang_hai_info;

	int fight_seq = ack->business_data.param1;
	activity->SetFightResult(ack->uid, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_FAIL, 0, fight_seq, false, shang_hai_info);
}

void BBLImplHuanJieZhanChang::OnBattleForceFinish(battlegameprotocol::BattleGameForceFinishFight * ack)
{
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	std::map<int, ActivityHuanJieZhanChangShangHaiInfo> shang_hai_info;
	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameForceFinishFight::RoleInfo & curr = ack->attacker_list.role_list[i];
		ActivityHuanJieZhanChangShangHaiInfo temp;
		temp.shanghai = curr.role_statistic.total_damage_to_enemy;
		temp.zhi_liao = curr.role_statistic.total_heal_other;
		temp.chengshou_shanghai = curr.role_statistic.total_injure_from_enemy;
		shang_hai_info[curr.uid] = temp;
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		battlegameprotocol::BattleGameForceFinishFight::RoleInfo & curr = ack->defender_list.role_list[i];
		ActivityHuanJieZhanChangShangHaiInfo temp;
		temp.shanghai = curr.role_statistic.total_damage_to_enemy;
		temp.zhi_liao = curr.role_statistic.total_heal_other;
		temp.chengshou_shanghai = curr.role_statistic.total_injure_from_enemy;
		shang_hai_info[curr.uid] = temp;
	}

	int fight_seq = ack->business_data.param1;

	for (int i = 0; i < ack->attacker_list.role_count; ++i)
	{
		int role_id = ack->attacker_list.role_list[i].uid;
		activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW, ack->statistic_data.round_num, fight_seq, true, shang_hai_info);
	}

	for (int i = 0; i < ack->defender_list.role_count; ++i)
	{
		int role_id = ack->defender_list.role_list[i].uid;
		activity->SetFightResult(role_id, ACTIVITY_HUAN_JIE_ZHAN_CHANG_RESULT_DRAW, ack->statistic_data.round_num, fight_seq, false, shang_hai_info);
	}

	activity->RemoveFightItemInfo(fight_seq);
}
