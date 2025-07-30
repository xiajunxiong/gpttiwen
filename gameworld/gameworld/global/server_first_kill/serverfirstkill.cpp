#include "serverfirstkill.hpp"
#include "internalcomm.h"
#include "common/platform/system.h"
#include "servercommon/errornum.h"
#include "servercommon/maildef.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "servercommon/userprotocal/msgbattle.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/team/team.hpp"
#include "global/cross/crossmanager.h"
#include "global/usercache/usercache.hpp"

#include "gameworld/gamelog.h"
#include "gameworld/world.h"
#include "gameworld/engineadapter.h"
#include "gameworld/item/knapsack.h"
#include "gameworld/scene/scenemanager.hpp"
#include "gameworld/monster/monsterpool.hpp"
#include "gameworld/monster/monster_group_pool.hpp"
#include "gameworld/config/logicconfigmanager.hpp"
#include "gameworld/obj/character/robot.h"
#include "global/rmibackobjdef.h"
#include "other/route/mailroute.hpp"
#include "other/robot/robotmanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "battle/battle_video_manager.hpp"

ServerFirstKill::ServerFirstKill() :m_first_kill_next_save_time(0), change_flag(false), m_load_flag(false)
{
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	memset(m_old_state, 0, sizeof(m_old_state));
}

ServerFirstKill::~ServerFirstKill()
{
	for (FirstKillInfoMap::iterator delay_it = m_info_delay_map.begin(); delay_it != m_info_delay_map.end(); delay_it++)
	{
		for (int i = 0; i < (int)delay_it->second.size(); i++)
		{
			if (NULL != delay_it->second[i].video_buffer)
			{
				delete[] delay_it->second[i].video_buffer;
				delay_it->second[i].video_buffer = NULL;
			}
		}
	}
	m_info_delay_map.clear();
}

void ServerFirstKill::Save(const ServerFirstKillParam *param)
{
	if(!this->IsLoadFinish()) return;

	static ServerFirstKillParam server_first_kill_param;
	if (param != NULL)
	{
		server_first_kill_param = *param;
	}
	this->GetInitParam(&server_first_kill_param);

	RMISaveServerFirstKillBackObjectImpl *backobj = new RMISaveServerFirstKillBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());

	bool ret = ac.SaveServerFirstKillAsyn(server_first_kill_param, backobj);
	if (ret)
	{
		this->ClearDirtyMark(&server_first_kill_param);
	}

}

ServerFirstKill & ServerFirstKill::Instance()
{
	static ServerFirstKill server_first_kill_mgr;
	return server_first_kill_mgr;
}

void ServerFirstKill::OnServerStart()
{
	this->LoadServerFirstKillData(0);
}

void ServerFirstKill::OnServerStop()
{
	this->Save();
}

bool ServerFirstKill::LoadServerFirstKillData(long long id_from)
{
	RMIInitServerFirstKillBackObjectImpl *impl = new RMIInitServerFirstKillBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitServerFirstKillAsyn(id_from, impl);
}

void ServerFirstKill::Update(time_t now_second)
{
	if(!this->IsLoadFinish()) return;
	
	if (m_first_kill_next_save_time > 0 && now_second > m_first_kill_next_save_time)
	{
		this->Save();
		m_first_kill_next_save_time = EngineAdapter::Instance().Time() + SERVER_FIRST_KILL_SAVE_TO_DB_TIME;
	}
	//本服保存记录不发送奖励
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	this->CheckInfoDelay(static_cast<unsigned int>(now_second));
}

void ServerFirstKill::Init(const ServerFirstKillParam & param)
{
	memset(m_old_state, true, sizeof(m_old_state));
	for (int i = 0; i < FIRST_KILL_TYPE_MAX; i++)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{
			if (param.data_list[i][k].first_kill_info.kill_flag)
			{
				if (param.data_list[i][k].first_kill_info.type <= 0 || param.data_list[i][k].first_kill_info.type > FIRST_KILL_TYPE_MAX)
				{
					gamelog::g_log_first_kill.printf(LL_CRITICAL, "[ServerFirstKill::Init INVALID ] [type:%d]", param.data_list[i][k].first_kill_info.type);
					continue;
				}
				if (param.data_list[i][k].first_kill_info.monster_group_id < 0 )
				{
					gamelog::g_log_first_kill.printf(LL_CRITICAL, "[ServerFirstKill::Init INVALID ] [monster_group_id:%d]", param.data_list[i][k].first_kill_info.monster_group_id);
					continue;
				}
				if (param.data_list[i][k].first_kill_info.uid[0] <= 0)
				{
					gamelog::g_log_first_kill.printf(LL_CRITICAL, "[ServerFirstKill::Init INVALID ] [uid:%d]", param.data_list[i][k].first_kill_info.uid[0]);
					continue;
				}
				this->CheckConfigIsChange(i, k, param);
			}
		}
	}
	m_load_flag = true;

	//初始化后保存一份数据,防止之后删除的时候没有数据可用
	this->Save(&server_param);
}

void ServerFirstKill::GetInitParam(ServerFirstKillParam * param)
{
	if(NULL == param) return;

	if (!change_flag) return;
	
	for (int i = 0; i < FIRST_KILL_TYPE_MAX; i++)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{
			if (m_dirty_mark[i][k])
			{
				if (server_param.data_list[i][k].first_kill_info.Invalid())
				{
					if (m_old_state[i][k]) continue;

					param->data_list[i][k].change_state = structcommon::CS_DELETE;
					param->data_list[i][k].id = (i + 1) * FIRST_KILL_TYPE_BASE_NUM + k;
					param->data_list[i][k].first_kill_info.server_id = server_param.data_list[i][k].first_kill_info.server_id;
					param->data_list[i][k].first_kill_info.battle_video_record_file_id = server_param.data_list[i][k].first_kill_info.battle_video_record_file_id;
				}
				else
				{
					param->data_list[i][k].change_state = m_old_state[i][k] ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
					param->data_list[i][k].id = (i + 1) * FIRST_KILL_TYPE_BASE_NUM + k;

					param->data_list[i][k].first_kill_info = server_param.data_list[i][k].first_kill_info;
				}
			}
		}
	}
}

void ServerFirstKill::ClearDirtyMark(ServerFirstKillParam * param)
{
	if(!change_flag) return;
	
	memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
	for (int i = 0; i < FIRST_KILL_TYPE_MAX; i++)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{
			m_old_state[i][k] = true;
			server_param.data_list[i][k].change_state = structcommon::CS_NONE;
			param->data_list[i][k].change_state = structcommon::CS_NONE;
		}
	}

	change_flag = false;
}

void RMIInitServerFirstKillBackObjectImpl::InitServerFirstKillRet(int ret, const ServerFirstKillParam & server_first_kill_param)
{
	if(ret < 0)
	{
		printf("RMIInitServerFirstKillBackObjectImpl::InitServerFirstKillRet Failed \n"); 
	}
	else
	{ 
		ServerFirstKill &server_first_kill = ServerFirstKill::Instance();
		if (NULL == &server_first_kill) return;

		server_first_kill.Init(server_first_kill_param);
		server_first_kill.LoadServerFirstKillSucc();
		printf("%s:%d\n", __FUNCTION__, ret);
	}
}

void ServerFirstKill::OnUserResetName(const UserID & user_id, GameName name)
{
	//采用UserCache
	/*int uid = UidToInt(user_id);

	for (int index = 0; index < FIRST_KILL_TYPE_MAX; ++index)
	{
		for(int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		if (!server_param.data_list[index][k].first_kill_info.Invalid())
		{
			for (int i = 0; i < MEMBER_NAME_MAX_NUM; ++i)
			{
				if (server_param.data_list[index][k].first_kill_info.uid[i] == uid)
				{
					F_STRNCPY(server_param.data_list[index][k].first_kill_info.member_name[i], name, sizeof(GameName));
					m_dirty_mark[index][k] = true;
					m_old_state[index][k] = true;
					change_flag = true;
					break;
				}
			}
		}
	}*/
}

void ServerFirstKill::ClearFirstKillItem(int type_index, int index)
{
	if(!this->IsValidIndex(index) || type_index < 0 || type_index >= FIRST_KILL_TYPE_MAX) return;

	server_param.data_list[type_index][index].Reset(false);
	m_dirty_mark[type_index][index] = true;
	m_old_state[type_index][index] = false;
	change_flag = true;
}

bool ServerFirstKill::SendFirstKillInfo(Role * role, int type, int level_group)
{
	if (NULL == role)
	{
		return false;
	}
	if (type <= 0 || type > FIRST_KILL_TYPE_MAX || level_group <= 0)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}

	/*if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameFirstKillInfoReq hgfkir;
		hgfkir.uuid = role->GetUUID();
		hgfkir.type = type;
		hgfkir.level_group = level_group;

		InternalComm::Instance().SendToGameThroughCross(role->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), role->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(),
			(const char *)&hgfkir, sizeof(hgfkir));
		return true;
	}*/
	
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenFirstKillInfoReq ghfkir;
		ghfkir.uid = role->GetUID();
		ghfkir.type = type;
		ghfkir.level_group = level_group;

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghfkir, sizeof(ghfkir));
		return true;
	}

	Protocol::SCServerFirstKillInfo info;
	info.count = 0;

	for (int i = 0; i < FIRST_KILL_BOSS_TYPE_MAX_NUM; i++)
	{
		if (server_param.data_list[type - 1][i].first_kill_info.level_group == level_group && server_param.data_list[type - 1][i].first_kill_info.type == type)
		{
			this->GetSendInfo(info, type - 1, i);
			if (info.count >= TYPE_LEVEL_GROUP_SEQ_MAX_NUM)
			{
				break;
			}
		}
	}

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);
	return true;
}

void ServerFirstKill::SendFirstKillInfoToHidden(long long uuid, int type, int level_group)
{
	if (type <= 0 || type > FIRST_KILL_TYPE_MAX || level_group <= 0)
	{
		return;
	}
	Protocol::SCServerFirstKillInfo info;
	info.count = 0;

	for (int i = 0; i < FIRST_KILL_BOSS_TYPE_MAX_NUM; i++)
	{
		if (server_param.data_list[type - 1][i].first_kill_info.level_group == level_group && server_param.data_list[type - 1][i].first_kill_info.type == type)
		{
			this->GetSendInfo(info, type - 1, i);
			if (info.count >= TYPE_LEVEL_GROUP_SEQ_MAX_NUM)
			{
				break;
			}
		} 
	}

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&info, uuid);
}

void ServerFirstKill::SendFirstKillInfoToGame(int uid, int type, int level_group)
{
	if (type <= 0 || type > FIRST_KILL_TYPE_MAX || level_group <= 0)
	{
		return;
	}

	Protocol::SCServerFirstKillInfo info;
	info.count = 0;

	for (int i = 0; i < FIRST_KILL_BOSS_TYPE_MAX_NUM; i++)
	{
		if (server_param.data_list[type - 1][i].first_kill_info.level_group == level_group && server_param.data_list[type - 1][i].first_kill_info.type == type)
		{
			this->GetSendInfo(info, type - 1, i);
			if (info.count >= TYPE_LEVEL_GROUP_SEQ_MAX_NUM)
			{
				break;
			}
		}
	}

	InternalComm::Instance().SendSerializeMsgToGameUserThroughCross(uid, &info);
}

int ServerFirstKill::OnGiveFirstKillReward(battlegameprotocol::BattleGameFightResultAck::RoleList * role_list, int round_num, int monster_group_id, int battle_mode, int record_score, long long record_file_id)
{
	//本服打,跨服没开,则不产生记录
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			return -333;
		}
	}
	const ServerFirstKillCfg * node_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(monster_group_id);
	if(NULL == node_cfg) return -1;

	const MonsterGroupCfg* monster_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
	if(NULL == monster_group_cfg) return -4;

	int type_index = node_cfg->type - 1;
	int type_seq = node_cfg->type_seq;
	if (!this->IsValidIndex(type_seq) || type_index < 0 || type_index >= FIRST_KILL_TYPE_MAX)
	{
		return -5;
	}
	//因合服问题,所以本服与跨服都有首杀记录,以跨服记录为准下发(跨服:跨服+本服,本服:本服)  //如果本服都有记录,代表跨服肯定有
	if (!server_param.data_list[type_index][type_seq].first_kill_info.Invalid())
	{
		return -6;
	}

	ServerFirstKillParam::ServerFirstKillSaveEntry data_item;
	data_item.id = node_cfg->type * FIRST_KILL_TYPE_BASE_NUM + node_cfg->type_seq;
	data_item.first_kill_info.server_id = LocalConfig::Instance().GetMergeServerId();
	data_item.first_kill_info.type = node_cfg->type;
	data_item.first_kill_info.level_group = node_cfg->level_group;
	data_item.first_kill_info.seq = node_cfg->seq;
	data_item.first_kill_info.monster_group_id = node_cfg->monster_group_id;
	data_item.first_kill_info.battle_video_record_file_id = record_file_id;
	
	for (int i = 0; i < role_list->role_count && i < MEMBER_NAME_MAX_NUM; i++)
	{
		int role_id = role_list->role_list[i].uid;
		if (RobotManager::Instance().IsRobot(role_id))
		{
			Robot * robot = RobotManager::Instance().GetRobot(role_id);
			if(NULL == robot) continue;

			robot->GetName(data_item.first_kill_info.member_name[i]);
			data_item.first_kill_info.uid[i] = role_id;
		}
		else
		{
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
			if (NULL == role) continue;

			role->GetName(data_item.first_kill_info.member_name[i]);
			data_item.first_kill_info.uid[i] = role_id;
		}	
	}

	data_item.first_kill_info.member_count = role_list->role_count;
	data_item.first_kill_info.kill_timestamp = static_cast<long long>(EngineAdapter::Instance().Time());
	data_item.first_kill_info.kill_game_timestamp = static_cast<long long>(PITime());
	data_item.first_kill_info.round_num = round_num;
	data_item.first_kill_info.kill_flag = 1;
	data_item.first_kill_info.reward_data.coin = node_cfg->coin;
	data_item.first_kill_info.reward_data.data.item_id = node_cfg->reward_item.item_id;
	data_item.first_kill_info.reward_data.data.is_bind = node_cfg->reward_item.is_bind;
	data_item.first_kill_info.reward_data.data.item_num = node_cfg->reward_item.num;

	//添加录像映射,保存记录
	if (!BattleVideoMgr::Instance().TryRecordVideo(battle_mode, monster_group_id, record_score, record_file_id, true))
	{
		return -222;
	}

	//目前记录同步一份到跨服
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		//保存一份本服自身的首杀情况
		m_dirty_mark[type_index][type_seq] = true;
		m_old_state[type_index][type_seq] = false;
		server_param.data_list[type_index][type_seq] = data_item;
		change_flag = true;

		//收集录像数据, 同步过去时需要连同录像一起
		const BattleVideoInfo* info = BattleVideoMgr::Instance().GetVideoInfoConst(monster_group_id, true);
		if (NULL == info || !info->IsValid() || NULL == info->video_buffer || info->video_len < 0)
		{
			return -777;
		}
		crossgameprotocal::GameHiddenFirstKillReq ghfkr;
		ghfkr.info = data_item;
		ghfkr.info_param = info->param;
		ghfkr.video_length = info->video_len;
		if (info->video_len >= static_cast<int>(sizeof(ghfkr.video_buffer)))
		{
			return -666;
		}
		memcpy(ghfkr.video_buffer, info->video_buffer, info->video_len);
		
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&ghfkr);
		return 0;
	}
	else
	{
		FirstKillDelayInfo delay_info;
		delay_info.info = data_item;
		delay_info.delay_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 3;

		//跨服产生的记录录像已经写入,不需要重复写
		/*delay_info.video_length = info->video_len;
		delay_info.video_buffer = new char[info->video_len];	//记得释放内存
		memcpy(delay_info.video_buffer, info->video_buffer, info->video_len);*/
		m_info_delay_map[monster_group_id].push_back(delay_info);
	}

	return 0;
}

void ServerFirstKill::OnGameHiddenFirstKillReq(crossgameprotocal::GameHiddenFirstKillReq & data_info)
{
	const ServerFirstKillCfg * node_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(data_info.info.first_kill_info.monster_group_id);
	if (NULL == node_cfg) return;

	const MonsterGroupCfg* monster_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(data_info.info.first_kill_info.monster_group_id);
	if (NULL == monster_group_cfg) return;

	int type_index = node_cfg->type - 1;
	int type_seq = node_cfg->type_seq;
	if (!this->IsValidIndex(type_seq) || type_index < 0 || type_index >= FIRST_KILL_TYPE_MAX)
	{
		return;
	}
	if (!server_param.data_list[type_index][type_seq].first_kill_info.Invalid() || data_info.info.first_kill_info.type != node_cfg->type ||
			data_info.info.id % FIRST_KILL_TYPE_BASE_NUM != type_seq)
	{
		return;
	}

	FirstKillDelayInfo delay_info;
	delay_info.info_param = data_info.info_param;
	delay_info.info = data_info.info;
	delay_info.delay_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + 3;
	Protocol::SCBattleReport report;
	int out_offset = 0;
	if (!report.CheckDeserialize(data_info.video_buffer, data_info.video_length, &out_offset) || out_offset != data_info.video_length)//验证一下战报数据
	{
		return;
	}

	delay_info.video_length = data_info.video_length;
	delay_info.video_buffer = new char[data_info.video_length];
	memcpy(delay_info.video_buffer, data_info.video_buffer, data_info.video_length);
	m_info_delay_map[data_info.info.first_kill_info.monster_group_id].push_back(delay_info);
}

bool ServerFirstKill::IsFirstKill(int monster_group_id, bool is_judge)
{
	//本服打,跨服没开,则不产生录像记录
	if (is_judge)
	{
		if (!CrossConfig::Instance().IsHiddenServer())
		{
			if (!CrossManager::Instance().IsCrossServerConnected())
			{
				return false;
			}
		}
	}
	
	const MonsterGroupCfg* monster_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id);
	if(NULL == monster_group_cfg) return false;
	
	const ServerFirstKillCfg * node_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(monster_group_id);
	if (NULL == node_cfg) return false;

	return server_param.data_list[node_cfg->type - 1][node_cfg->type_seq].first_kill_info.Invalid();
}

long long ServerFirstKill::GetBattleVideoRecordFileIDByMonsterGroupID(int monster_group_id)
{
	const ServerFirstKillCfg * node_cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(monster_group_id);
	if (NULL == node_cfg) return 0;

	long long battle_video_record_file_id = 0;
	if (!server_param.data_list[node_cfg->type - 1][node_cfg->type_seq].first_kill_info.Invalid())
	{
		battle_video_record_file_id = server_param.data_list[node_cfg->type - 1][node_cfg->type_seq].first_kill_info.battle_video_record_file_id;
	}

	return battle_video_record_file_id;
}

void ServerFirstKill::OnServerLoadDataFinish()
{
	for (int i = 0; i < FIRST_KILL_TYPE_MAX; i++)
	{
		for (int k = 0; k < FIRST_KILL_BOSS_TYPE_MAX_NUM; k++)
		{
			ServerFirstKillEntry& info = server_param.data_list[i][k].first_kill_info;
			if (info.Invalid()) continue;
			if (0 == info.battle_video_record_file_id) continue;

			if (!BattleVideoMgr::Instance().IsHasServerFirstKillVideo(info.battle_video_record_file_id))
			{
				gamelog::g_log_first_kill.buff_printf("OnServerLoadDataFinish Video Error Clear First Kill Info, monster_group_id[%d] file_id[%lld]",
					info.monster_group_id, info.battle_video_record_file_id);

				for (int idx = 0; idx < info.member_count && idx < MEMBER_NAME_MAX_NUM; ++idx)
				{
					gamelog::g_log_first_kill.buff_printf("First Kill Member[%d,%s]", info.uid[idx], info.member_name[idx]);
				}
				gamelog::g_log_first_kill.commit_buff(LL_WARNING);

				this->ClearFirstKillItem(i, k);
			}
		}
	}
}

void ServerFirstKill::GetSendInfo(Protocol::SCServerFirstKillInfo & info, int type_index, int index)
{
	ServerFirstKillEntry & data_info = server_param.data_list[type_index][index].first_kill_info;
	info.data[info.count].type = data_info.type;
	info.data[info.count].level_group = data_info.level_group;
	info.data[info.count].seq = data_info.seq;
	info.data[info.count].boss_id = data_info.monster_group_id;
	info.data[info.count].member_count = data_info.member_count;
	info.data[info.count].round_num = data_info.round_num;
	info.data[info.count].kill_timestamp = data_info.kill_timestamp;
	for (int i = 0; i < info.data[info.count].member_count && i < MEMBER_NAME_MAX_NUM; i++)
	{
		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(data_info.uid[i]);
		if (NULL == node)
		{
			STRNCPY(info.data[info.count].member_name[i], data_info.member_name[i], sizeof(GameName));
		}
		else
		{
			node->GetName(info.data[info.count].member_name[i]);
		}
	}
	info.count++;
}

void ServerFirstKill::CheckConfigIsChange(int type_index, int index, const ServerFirstKillParam & param)
{
	if(!this->IsValidIndex(index) || type_index < 0 || type_index >= FIRST_KILL_TYPE_MAX)
	{
		return;
	}
	ServerFirstKillParam::ServerFirstKillSaveEntry & node = server_param.data_list[type_index][index];
	const ServerFirstKillParam::ServerFirstKillSaveEntry & data_item = param.data_list[type_index][index];
	const ServerFirstKillCfg * cfg = LOGIC_CONFIG->GetServerFirstKillConfig()->GetServerFirstKillCfg(data_item.first_kill_info.monster_group_id);
	if(NULL == cfg) 
	{
		node.first_kill_info.server_id = data_item.first_kill_info.server_id;
		node.first_kill_info.battle_video_record_file_id = data_item.first_kill_info.battle_video_record_file_id;
		this->ClearFirstKillItem(type_index, index);//不再清除录像,查找首杀录像不再用怪物组,使用录像文件ID
	//	BattleVideoMgr::Instance().RemoveVideo(data_item.first_kill_info.monster_group_id, true);
		return;
	}
	
	//1:原数据玩法与数据库存储的数据不一致 2:原数据的等级组与配置不一致 3:原数据的等级组seq与配置不一致 4:原数据的type_seq与配置不一致
	if (data_item.first_kill_info.type != cfg->type || cfg->level_group != data_item.first_kill_info.level_group || cfg->seq != data_item.first_kill_info.seq ||
		 data_item.id % FIRST_KILL_TYPE_BASE_NUM != cfg->type_seq)			
	{
		server_param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info = data_item.first_kill_info;
		server_param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info.level_group = cfg->level_group;
		server_param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info.seq = cfg->seq;
		server_param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info.type = cfg->type;
		if (0 == data_item.first_kill_info.kill_game_timestamp)
		{
			server_param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info.kill_game_timestamp = static_cast<long long>(PITime());
		}
		if (param.data_list[cfg->type - 1][cfg->type_seq].first_kill_info.kill_flag < 1)	//判断此时数据库是否有该数据存在
		{
			m_dirty_mark[cfg->type - 1][cfg->type_seq] = true;
			m_old_state[cfg->type - 1][cfg->type_seq] = false;		//插入
		}
		else
		{
			m_dirty_mark[cfg->type - 1][cfg->type_seq] = true;
			m_old_state[cfg->type - 1][cfg->type_seq] = true;		//更新
		}
		if (node.first_kill_info.type != cfg->type || node.first_kill_info.Invalid())	//数据原位置如果没有新数据替换,则删除之前数据
		{
			node.first_kill_info.server_id = data_item.first_kill_info.server_id;
			this->ClearFirstKillItem(type_index, index);
		}
		change_flag = true;
		return;
	}
	node = data_item;
	if (0 == data_item.first_kill_info.kill_game_timestamp)
	{
		node.first_kill_info.kill_game_timestamp = static_cast<long long>(PITime());
		m_dirty_mark[type_index][index] = true;
		change_flag = true;
	}
}

void ServerFirstKill::CheckInfoDelay(unsigned int now_second)
{
	std::set<int> m_set;
	for (FirstKillInfoMap::iterator monster_it = m_info_delay_map.begin(); monster_it != m_info_delay_map.end(); monster_it++)
	{
		bool is_has_delay_end = false;
		for (int i = 0; i < (int)monster_it->second.size(); i++)
		{
			if (now_second < monster_it->second[i].delay_timestamp)
			{
				continue;
			}
			is_has_delay_end = true;
			break;
		}
		if (is_has_delay_end)
		{
			int index = 0;
			for (int i = 1; i < (int)monster_it->second.size(); i++)
			{	
				if (monster_it->second[i].info.first_kill_info.kill_timestamp < monster_it->second[index].info.first_kill_info.kill_timestamp)
				{
					index = i;
					continue;
				}
				if (monster_it->second[i].info.first_kill_info.kill_timestamp == monster_it->second[index].info.first_kill_info.kill_timestamp)
				{
					if (monster_it->second[i].info.first_kill_info.kill_game_timestamp <= monster_it->second[index].info.first_kill_info.kill_game_timestamp)
					{
						index = i;
					}
				}
			}

			this->SendFirstKillReward(monster_it->second[index]);
			m_set.insert(monster_it->first);
		}
	}
	if (!m_set.empty())
	{
		for (std::set<int>::iterator it = m_set.begin(); it != m_set.end(); it++)
		{
			FirstKillInfoMap::iterator delay_it = m_info_delay_map.find(*it);
			if(delay_it == m_info_delay_map.end()) continue;

			for (int i = 0; i < (int)delay_it->second.size(); i++)
			{
				if (NULL != delay_it->second[i].video_buffer)
				{
					delete[] delay_it->second[i].video_buffer;
					delay_it->second[i].video_buffer = NULL;
				}
			}
			m_info_delay_map.erase(*it);
		}
	}
}

void ServerFirstKill::SendFirstKillReward(FirstKillDelayInfo & data_item)
{
	const MonsterGroupCfg* monster_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(data_item.info.first_kill_info.monster_group_id);
	if (NULL == monster_group_cfg) return;

	//该记录如果不是跨服产生,则需要写入录像
	if (data_item.info.first_kill_info.server_id != LocalConfig::Instance().GetMergeServerId())
	{
		if (!BattleVideoMgr::Instance().GameHiddenSynVideo(data_item.info_param, data_item.video_length, data_item.video_buffer))
		{
			return;
		}
	}

	char string[400] = { 0 };
	int len = 0;
	for (int i = 0; i < data_item.info.first_kill_info.member_count && i < MEMBER_NAME_MAX_NUM; i++)
	{
		int role_id = data_item.info.first_kill_info.uid[i];
		if (!RobotManager::Instance().IsRobot(role_id))
		{
			static MailContentParam contentparam; contentparam.Reset();
			ItemDataWrapper item_info;
			item_info.item_id = data_item.info.first_kill_info.reward_data.data.item_id;
			item_info.is_bind = data_item.info.first_kill_info.reward_data.data.is_bind;
			item_info.num = data_item.info.first_kill_info.reward_data.data.item_num;
			contentparam.item_list[0] = item_info;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = data_item.info.first_kill_info.reward_data.coin;

			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_server_first_kill_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_server_first_kill_content, MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name);
			MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_first_kill.printf(LL_INFO, "ServerFirstKill::GiveFirstKillReward by mail, attacker_list[%d] role[%d,%s] first_id[%d] item_id[%d] num[%d] coin[%d] reason[%s]",
				data_item.info.first_kill_info.member_count, role_id, data_item.info.first_kill_info.member_name[i], data_item.info.id, item_info.item_id, item_info.num, 
				data_item.info.first_kill_info.reward_data.coin, __FUNCTION__);
		}
		len += SNPRINTF(string + len, sizeof(GameName), "%s", data_item.info.first_kill_info.member_name[i]);
		if (i + 1 < data_item.info.first_kill_info.member_count)
		{
			SNPRINTF(string + len, sizeof(","), ",");
			len += 1;
		}
		else
		{
			break;
		}
	}

	int length = 0;
	switch (data_item.info.first_kill_info.type)
	{
	case TYPE_MIYI_BOSS:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_miyi, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_CHALLENGE:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_challenge, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_WILD_BOSS:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_boss, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_SHAN_HAI_BOSS:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_shan_hai, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_BIG_DIPPER:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_big_dipper, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_FALLEN_GOD:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_fallen_god, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	case TYPE_HONGMENG_TIANJIE:
		length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_server_first_kill_hmtj_fb, string,
			MonsterPool::GetInstance().GetMonsterCfg(monster_group_cfg->head_monster_id)->m_name, data_item.info.first_kill_info.round_num);
		break;
	default:
		break;
	}

	if (length > 0)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		SysMsgType msg_type = GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT);	// 跑马灯和系统栏都要显示
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, msg_type))
		{
			InternalComm::Instance().SendToAllGameAllUserThroughCross((const char*)&sm, sendlen);
		}
	}
	//此处type_index与type_seq已经通过外部检查过了
	int type_index = data_item.info.first_kill_info.type - 1;
	int type_seq = data_item.info.id % FIRST_KILL_TYPE_BASE_NUM;
	m_dirty_mark[type_index][type_seq] = true;
	m_old_state[type_index][type_seq] = false;
	server_param.data_list[type_index][type_seq] = data_item.info;
	change_flag = true;

	/*//同步原服录像
	crossgameprotocal::HiddenGameVideoDetailInfoReq hgvdir;
	hgvdir.type = crossgameprotocal::VIDEO_DETAIL_INFO_REQ_TYPE_SYN;
	hgvdir.monster_group_id = info.first_kill_info.monster_group_id;
	hgvdir.is_server_first_kill = 1;
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), data_item.info.first_kill_info.server_id, (const char *)& hgvdir, sizeof(hgvdir));*/
}

void ServerFirstKill::LoadServerFirstKillSucc()
{
	if (this->IsLoadFinish())
	{
		m_first_kill_next_save_time = EngineAdapter::Instance().Time() + SERVER_FIRST_KILL_SAVE_TO_DB_TIME;
	}
}

