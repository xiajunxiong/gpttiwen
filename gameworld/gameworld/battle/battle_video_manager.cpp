#include "battle_video_manager.hpp"
#include "engineadapter.h"
#include "config/logicconfigmanager.hpp"
#include "battle/battle_video_config.hpp"
#include "servercommon/struct/structcommon.h"
#include "global/rmibackobjdef.h"
#include "gamelog.h"
#include "internalcomm.h"
#include "serverlogic.h"
#include <iostream>
#include <fstream>
#include "servercommon/userprotocal/msgbattle.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "global/team/team.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

BattleVideoMgr::BattleVideoMgr() : m_data_state(GLOBAL_SYSTEM_DATA_STATE_INVALID),
	m_last_save_time(0u)
{

}

BattleVideoMgr::~BattleVideoMgr()
{
	this->Release();
}

BattleVideoMgr& BattleVideoMgr::Instance()
{
	static BattleVideoMgr mgr;
	return mgr;
}

void BattleVideoMgr::Update(unsigned long interval, time_t now_second)
{
	if (!this->IsLoadFinish())
	{
		return;
	}

	unsigned int now = static_cast<unsigned int>(now_second);
	if (m_last_save_time > 0 && now > m_last_save_time + 60)
	{
		this->Save();
		m_last_save_time = now;
	}
}

void BattleVideoMgr::Save()
{
	static BattleVideoListParam save_list;
	save_list.count = 0;

	VideoMap::iterator it = m_video_map.begin();
	for (; it != m_video_map.end() && save_list.count < BattleVideoListParam::MAX_VIDEO_NUM; ++it)
	{
		BattleVideoInfo& info = it->second;
		if (info.isdirty)
		{
			if (info.IsValid())
			{
				if (!info.oldstate)
				{
					//insert
					save_list.video_list[save_list.count].change_state = structcommon::CS_INSERT;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
				else
				{
					//update
					save_list.video_list[save_list.count].change_state = structcommon::CS_UPDATE;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
			}
			else
			{
				if (info.oldstate)
				{
					//delete
					save_list.video_list[save_list.count].change_state = structcommon::CS_DELETE;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
			}

			info.isdirty = false;
			info.oldstate = true;
		}
	}

	FirstKillVideoMap::iterator it1 = m_first_kill_video_map.begin();
	for (; it1 != m_first_kill_video_map.end() && save_list.count < BattleVideoListParam::MAX_VIDEO_NUM; ++it1)
	{
		BattleVideoInfo& info = it1->second;
		if (info.isdirty)
		{
			if (info.IsValid())
			{
				if (!info.oldstate)
				{
					//insert
					save_list.video_list[save_list.count].change_state = structcommon::CS_INSERT;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
				else
				{
					//update
					save_list.video_list[save_list.count].change_state = structcommon::CS_UPDATE;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
			}
			else
			{
				if (info.oldstate)
				{
					//delete
					save_list.video_list[save_list.count].change_state = structcommon::CS_DELETE;
					save_list.video_list[save_list.count].param = info.param;
					save_list.count += 1;
				}
			}

			info.isdirty = false;
			info.oldstate = true;
		}
	}

	if (save_list.count > 0)
	{
		RMISaveBattleVideoDataBackObjectImpl * impl = new RMISaveBattleVideoDataBackObjectImpl();
		RMIGlobalClient ac;
		ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
		if (!ac.SaveBattleVideoDataAsyn(save_list, impl))
		{
			gamelog::g_log_debug.printf(LL_WARNING, "BattleVideoMgr::Save Call RMI Method Failed");
		}
	}
}

bool BattleVideoMgr::OnServerStart()
{
	return this->LoadVideo(0LL);
}

void BattleVideoMgr::OnServerStop()
{
	this->Save();
}

bool BattleVideoMgr::NeedChangeVideo(int monster_group_id, int average_score)
{
	BattleVideoInfo* vi = this->GetVideoInfo(monster_group_id, false);
	if (NULL == vi)
	{
		return true;
	}
	else
	{
		if (average_score == 0 || average_score <= vi->param.average_score)
		{
			return false;
		}
	}
	return false;
}

bool BattleVideoMgr::TryRecordVideo(short battle_mode, int monster_group_id, int average_score, long long video_file_id, bool is_server_first_kill)
{
	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
	if (NULL == cfg)
	{
		return false;
	}

	if (cfg->battle_mode != battle_mode) return false;

	BattleVideoInfo* vi = this->GetVideoInfo(monster_group_id, is_server_first_kill);
	if (NULL == vi)
	{
		if (!is_server_first_kill && battle_mode == BATTLE_MODE_HONGMENG_TIANJIE && average_score != 0)
		{
			// 鸿蒙天界 不是全服击杀记录的话 则是通关记录，需要首次通关后 统一上传三个BOSS 的记录
			return false;
		}

		static BattleVideoInfo tmp_vi;
		tmp_vi.Reset();
		
		tmp_vi.isdirty = true;
		tmp_vi.oldstate = false;

		tmp_vi.param.is_server_first_skill = is_server_first_kill ? 1 : 0;
		tmp_vi.param.average_score = average_score;
		tmp_vi.param.battle_mode = battle_mode;
		tmp_vi.param.monster_group_id = monster_group_id;
		tmp_vi.param.record_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		tmp_vi.param.version = cfg->version;
		tmp_vi.param.video_file_id = video_file_id;

		if (this->ReadVideoFile(tmp_vi.param.video_file_id, &tmp_vi.video_buffer, &tmp_vi.video_len))
		{
			if (!is_server_first_kill)
			{
				m_video_map.insert(VideoMap::value_type(monster_group_id, tmp_vi));
			}
			else
			{
				m_first_kill_video_map.insert(FirstKillVideoMap::value_type(video_file_id, tmp_vi));
			}

			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [INSERT SUCC] Insert monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				tmp_vi.param.monster_group_id, is_server_first_kill ? "true" : "false", tmp_vi.param.battle_mode, tmp_vi.param.average_score,
				tmp_vi.param.record_timestamp, tmp_vi.param.version, tmp_vi.param.video_file_id);
		}	
		else
		{
			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [INSERT FAIL, file not exist or data not compatible] Insert monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				tmp_vi.param.monster_group_id, is_server_first_kill ? "true" : "false", tmp_vi.param.battle_mode, tmp_vi.param.average_score,
				tmp_vi.param.record_timestamp, tmp_vi.param.version, tmp_vi.param.video_file_id);
		
			return false;
		}
	}
	else
	{
		if (average_score == 0 || average_score >= vi->param.average_score)
		{
			return false;
		}

		if (is_server_first_kill && vi->IsValid()) // 首杀不更新录像
		{
			return false;
		}

		if (battle_mode == BATTLE_MODE_ENDLESS_TOWER)
		{
			return false;//无尽之塔只有首杀
		}

		char* new_video_buffer = NULL;
		int new_video_len = 0;
		if (this->ReadVideoFile(video_file_id, &new_video_buffer, &new_video_len))
		{
			if (vi->video_buffer != NULL)
			{
				delete[] vi->video_buffer;
			}
			vi->video_buffer = new_video_buffer;
			vi->video_len = new_video_len;

			vi->isdirty = true;
			vi->param.average_score = average_score;
			vi->param.record_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			vi->param.video_file_id = video_file_id;

			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [REPLACE SUCC] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				vi->param.monster_group_id, is_server_first_kill ? "true" : "false", vi->param.battle_mode, vi->param.average_score,
				vi->param.record_timestamp, vi->param.version, vi->param.video_file_id);
		}
		else
		{
			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [REPLACE FAIL, file not exist or data not compatible] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				vi->param.monster_group_id, is_server_first_kill ? "true" : "false", vi->param.battle_mode, vi->param.average_score,
				vi->param.record_timestamp, vi->param.version, vi->param.video_file_id);
		
			return false;
		}
		
	}

	return true;
}

bool BattleVideoMgr::GameHiddenSynVideo(const BattleVideoParam & info_param, const int & video_length, char * video_buffer)
{
	if(video_length < 0) return false;
	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(info_param.monster_group_id);
	if(NULL == cfg) return false;

	std::stringstream ss;
	ss << EngineAdapter::Instance().GetLogDir() << "/fightdata/" << info_param.video_file_id;

	bool is_write_succ = false;
#ifdef WIN32
	std::string folder = EngineAdapter::Instance().GetLogDir() + "/fightdata/";
	CreateDirectory(folder);
	is_write_succ = WriteFile(ss.str().c_str(), video_buffer, video_length, false, true);
#else
	is_write_succ = EngineAdapter::Instance().AsynFileWrite(ss.str().c_str(), video_buffer, video_length, false);
#endif
	
	if (is_write_succ)
	{
		//默认覆盖
		BattleVideoInfo* vi = this->GetVideoInfo(info_param.monster_group_id, (0 != info_param.is_server_first_skill));
		if (NULL == vi)
		{
			static BattleVideoInfo tmp_vi;
			tmp_vi.Reset();

			tmp_vi.isdirty = true;
			tmp_vi.oldstate = false;

			char *buffer = new char[video_length + 1];
			memcpy(buffer, video_buffer, video_length);
			buffer[video_length] = 0;

			tmp_vi.video_buffer = buffer;
			tmp_vi.video_len = video_length;

			tmp_vi.param = info_param;
		
			if (0 != tmp_vi.param.is_server_first_skill)
			{
				m_first_kill_video_map.insert(FirstKillVideoMap::value_type(tmp_vi.param.video_file_id, tmp_vi));
			}
			else
			{
				m_video_map.insert(VideoMap::value_type(tmp_vi.param.monster_group_id, tmp_vi));
			}

			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [INSERT SUCC] Insert monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				tmp_vi.param.monster_group_id, 0 != info_param.is_server_first_skill ? "true" : "false", tmp_vi.param.battle_mode, tmp_vi.param.average_score,
				tmp_vi.param.record_timestamp, tmp_vi.param.version, tmp_vi.param.video_file_id);
		}
		else
		{
			if (vi->video_buffer != NULL)
			{
				delete[] vi->video_buffer;
			}
			char *buffer = new char[video_length + 1];
			memcpy(buffer, video_buffer, video_length);
			buffer[video_length] = 0;

			vi->video_buffer = buffer;
			vi->video_len = video_length;

			vi->isdirty = true;
			vi->param = info_param;

			gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::TryRecordVideo [REPLACE SUCC] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
				vi->param.monster_group_id, 0 != info_param.is_server_first_skill ? "true" : "false", vi->param.battle_mode, vi->param.average_score,
				vi->param.record_timestamp, vi->param.version, vi->param.video_file_id);
		}
	}
	return is_write_succ;
}

long long BattleVideoMgr::GetVideoFileID(int monster_group_id, bool is_server_first_kill)
{
	const BattleVideoInfo* vi = this->GetVideoInfo(monster_group_id, is_server_first_kill);
	if (NULL == vi || !vi->IsValid())
	{
		return 0;
	}

	if (NULL == vi->video_buffer)
	{
		return 0;
	}

	return vi->param.video_file_id;
}

bool BattleVideoMgr::IsHasServerFirstKillVideo(long long battle_video_record_file_id)
{
	FirstKillVideoMap::iterator it = m_first_kill_video_map.find(battle_video_record_file_id);
	if (it != m_first_kill_video_map.end())
	{
		return true;
	}
	return false;
}

const BattleVideoInfo* BattleVideoMgr::GetVideoInfoConst(int monster_group_id, bool is_server_first_kill)
{
	return this->GetVideoInfo(monster_group_id, is_server_first_kill);
}

bool BattleVideoMgr::RemoveVideo(int monster_group_id, bool is_server_first_kill)
{
	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
	if (NULL == cfg)
	{
		return false;
	}

	BattleVideoInfo* vi = this->GetVideoInfo(monster_group_id, is_server_first_kill);
	if (NULL == vi)
	{
		return false;
	}
	vi->isdirty = true;
	vi->param.video_file_id = 0;

	return true;
}

void BattleVideoMgr::SendVideo(Role * role, int monster_group_id, bool is_server_first_kill)
{
	if(NULL == role) return;

	const BattleVideoInfo* info = BattleVideoMgr::Instance().GetVideoInfoConst(monster_group_id, is_server_first_kill);
	if (NULL == info || !info->IsValid())
	{
		role->NoticeNum(errornum::EN_BATTLE_VIDEO_NOT_FOUND);
		return;
	}

	if (NULL == info->video_buffer)
	{
		role->NoticeNum(errornum::EN_BATTLE_VIDEO_NOT_FOUND);
		return;
	}

	Team* team = role->GetMyTeam(false);
	if (NULL == team)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), info->video_buffer, info->video_len);
	}
	else
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			team->SendToFollowMember(info->video_buffer, info->video_len);
		}
		else
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		}
	}
}

void BattleVideoMgr::GmWatchVideo(Role* role, long long video_file_id)
{
	char* video_buffer = NULL;
	int video_length = 0;

	if (this->ReadVideoFile(video_file_id, &video_buffer, &video_length))
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), video_buffer, video_length);

		delete[] video_buffer;
	}
	else
	{
		role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
	}
}

void BattleVideoMgr::SendVideoToCross(int monster_group_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	const BattleVideoInfo* info = this->GetVideoInfoConst(monster_group_id, false);
	if (NULL == info || !info->IsValid())
	{
		return;
	}

	if (NULL == info->video_buffer)
	{
		return;
	}

	static crossgameprotocal::GameHiddenVideoDetailInfoResp resp;
	resp.type = crossgameprotocal::VIDEO_DETAIL_INFO_REQ_TYPE_SYN;
	resp.uuid = 0;
	resp.video_length = info->video_len;
	resp.info_param = info->param;
	if (info->video_len >= sizeof(resp.video_buffer))
	{
		return;
	}
	memcpy(resp.video_buffer, info->video_buffer, info->video_len);

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&resp);
}

BattleVideoInfo* BattleVideoMgr::GetVideoInfo(int monster_group_id, bool is_server_first_kill)
{
	if (!is_server_first_kill)
	{
		VideoMap::iterator it = m_video_map.find(monster_group_id);
		if (it != m_video_map.end())
		{
			return &it->second;
		}
	}
	else
	{
		BattleVideoKey video_key = this->GetVideoKeyByMonsterGroupId(monster_group_id, is_server_first_kill);
		FirstKillVideoMap::iterator it = m_first_kill_video_map.find(video_key.battle_video_record_file_id);
		if (it != m_first_kill_video_map.end())
		{
			return &it->second;
		}
	}
	
	return NULL;
}

BattleVideoInfo * BattleVideoMgr::GetVideoInfo(BattleVideoKey video_key, bool is_server_first_kill)
{
	if (!is_server_first_kill)
	{
		VideoMap::iterator it = m_video_map.find(video_key.monster_group_id);
		if (it != m_video_map.end())
		{
			return &it->second;
		}
	}
	else
	{
		FirstKillVideoMap::iterator it = m_first_kill_video_map.find(video_key.battle_video_record_file_id);
		if (it != m_first_kill_video_map.end())
		{
			return &it->second;
		}
	}

	return NULL;
}

BattleVideoKey BattleVideoMgr::GetVideoKeyByMonsterGroupId(int monster_group_id, bool is_server_first_kill)
{
	BattleVideoKey video_key;
	if (is_server_first_kill)
	{
		video_key.battle_video_record_file_id = ServerFirstKill::Instance().GetBattleVideoRecordFileIDByMonsterGroupID(monster_group_id);
	}
	else
	{
		video_key.monster_group_id = monster_group_id;
	}
	return video_key;
}

bool BattleVideoMgr::LoadVideo(long long id_from)
{
	if (this->IsLoadFinish()) return false;

	m_data_state = GLOBAL_SYSTEM_DATA_STATE_LOADING;

	RMIInitBattleVideoDataBackObjectImpl * impl = new RMIInitBattleVideoDataBackObjectImpl();
	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	return ac.InitBattleVideoDataAsyn(id_from, impl);
}

void BattleVideoMgr::LoadVideoFinish()
{
	m_data_state = GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH;
	m_last_save_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
}

void BattleVideoMgr::InitVideoList(const BattleVideoListParam& video_list)
{
	long long max_id = 0LL;
	for (int i = 0; i < video_list.count && i < BattleVideoListParam::MAX_VIDEO_NUM; ++i)
	{
		max_id = GetMax(max_id, video_list.video_list[i].id);
		int monster_group_id = video_list.video_list[i].param.monster_group_id;
		int version = video_list.video_list[i].param.version;
		const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(monster_group_id);
		if (NULL != cfg)
		{
			if (cfg->version != version) // 版本不一样，舍弃掉（读出来也播放不了）
			{
				gamelog::g_log_video.printf(LL_WARNING, "InitVideoList video[%lld] monster_group[%d] cfg->version[%d] != version[%d]",
					video_list.video_list[i].param.video_file_id, video_list.video_list[i].param.monster_group_id,
					cfg->version, version);

				continue;
			}
		}
		bool is_server_first_kill = video_list.video_list[i].param.is_server_first_skill > 0;
		BattleVideoKey video_key;
		if (is_server_first_kill)
		{
			video_key.battle_video_record_file_id = video_list.video_list[i].param.video_file_id;
		}
		else
		{
			video_key.monster_group_id = monster_group_id;
		}

		BattleVideoInfo* vi = this->GetVideoInfo(video_key, is_server_first_kill);
		if (NULL == vi)
		{
			static BattleVideoInfo tmp_vi;
			tmp_vi.Reset();

			tmp_vi.isdirty = false;
			tmp_vi.oldstate = true;

			tmp_vi.param.is_server_first_skill = video_list.video_list[i].param.is_server_first_skill;
			tmp_vi.param.average_score = video_list.video_list[i].param.average_score;
			tmp_vi.param.battle_mode = video_list.video_list[i].param.battle_mode;
			tmp_vi.param.monster_group_id = video_list.video_list[i].param.monster_group_id;
			tmp_vi.param.record_timestamp = video_list.video_list[i].param.record_timestamp;
			tmp_vi.param.version = video_list.video_list[i].param.version;
			tmp_vi.param.video_file_id = video_list.video_list[i].param.video_file_id;

			if (this->ReadVideoFile(tmp_vi.param.video_file_id, &tmp_vi.video_buffer, &tmp_vi.video_len))
			{
				if (!is_server_first_kill)
				{
					m_video_map.insert(VideoMap::value_type(monster_group_id, tmp_vi));
				}
				else
				{
					m_first_kill_video_map.insert(FirstKillVideoMap::value_type(tmp_vi.param.video_file_id, tmp_vi));
				}
				
				gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::InitVideoList [INSERT SUCC] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
					tmp_vi.param.monster_group_id, is_server_first_kill ? "true" : "false", tmp_vi.param.battle_mode, tmp_vi.param.average_score,
					tmp_vi.param.record_timestamp, tmp_vi.param.version, tmp_vi.param.video_file_id);
			}
			else
			{
				gamelog::g_log_video.printf(LL_WARNING, "BattleVideoMgr::InitVideoList [INSERT FAIL, file not exist or data not compatible] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
					tmp_vi.param.monster_group_id, is_server_first_kill ? "true" : "false", tmp_vi.param.battle_mode, tmp_vi.param.average_score,
					tmp_vi.param.record_timestamp, tmp_vi.param.version, tmp_vi.param.video_file_id);
			}
			
		}
		else
		{
			const BattleVideoParam& vp = video_list.video_list[i].param;
			bool is_replace = false;
			if (is_server_first_kill) // 合服后，如果首杀数据有更早的，要替换
			{
				if (vp.record_timestamp < vi->param.record_timestamp)
				{
					is_replace = true;
				}
			}
			else
			{
				if (vp.average_score < vi->param.average_score ||
					(vp.average_score == vi->param.average_score && vp.record_timestamp < vi->param.record_timestamp))
				{
					is_replace = true;
				}
			}
			
			if (is_replace)
			{
				char* new_video_buffer = NULL;
				int new_video_len = 0;
				if (this->ReadVideoFile(vp.video_file_id, &new_video_buffer, &new_video_len))
				{
					if (NULL != vi->video_buffer)
					{
						delete[] vi->video_buffer;
					}

					if (!is_server_first_kill)
					{
						vi->video_buffer = new_video_buffer;
						vi->video_len = new_video_len;
						vi->param = vp;
					}
					else
					{
						m_first_kill_video_map.erase(video_key.battle_video_record_file_id);		//此时key会不同,所以不能直接替换值
						static BattleVideoInfo tmp_vi;
						tmp_vi.Reset();

						tmp_vi.isdirty = false;
						tmp_vi.oldstate = true;

						tmp_vi.param.is_server_first_skill = video_list.video_list[i].param.is_server_first_skill;
						tmp_vi.param.average_score = video_list.video_list[i].param.average_score;
						tmp_vi.param.battle_mode = video_list.video_list[i].param.battle_mode;
						tmp_vi.param.monster_group_id = video_list.video_list[i].param.monster_group_id;
						tmp_vi.param.record_timestamp = video_list.video_list[i].param.record_timestamp;
						tmp_vi.param.version = video_list.video_list[i].param.version;
						tmp_vi.param.video_file_id = video_list.video_list[i].param.video_file_id;
						m_first_kill_video_map.insert(FirstKillVideoMap::value_type(tmp_vi.param.video_file_id, tmp_vi));
					}

					gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::InitVideoList [REPLACE SUCC] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
						vp.monster_group_id, is_server_first_kill ? "true" : "false", vp.battle_mode, vp.average_score,
						vp.record_timestamp, vp.version, vp.video_file_id);
				}
				else
				{
					gamelog::g_log_video.printf(LL_INFO, "BattleVideoMgr::InitVideoList [REPLACE FAIL, file not exist or data not compatible] monster_group_id[%d] is_first_kill[%s] battle_mode[%d] average_score[%d] timestamp[%u] version[%d] file_id[%lld]",
						vp.monster_group_id, is_server_first_kill ? "true" : "false", vp.battle_mode, vp.average_score,
						vp.record_timestamp, vp.version, vp.video_file_id);
				}		
			}
		}
	}

	if (max_id > 0)
	{
		this->LoadVideo(max_id);
	}
	else
	{
		this->LoadVideoFinish();
	}
}

bool BattleVideoMgr::ReadVideoFile(long long file_id, ARG_OUT char** video_buffer, ARG_OUT int* video_len)
{
	std::stringstream ss;
	ss << EngineAdapter::Instance().GetLogDir() << "/fightdata/" << file_id;
	std::ifstream ifs;
	ifs.open(ss.str().c_str(), std::ios::binary);
	if (!ifs.is_open())
	{
		return false;
	}
	ifs.seekg(0, std::ios::end);                      // 设置指针到文件流尾部
	std::streampos ps = ifs.tellg();                  // 指针距离文件头部的距离，即为文件流大小
	ifs.seekg(0, std::ios::beg);					  // 指针重回文件流头部
	int len = ps;
	char* buffer = new char[len];
	ifs.read(buffer, len);
	Protocol::SCBattleReport report;
	int out_offset = 0;
	if (!report.CheckDeserialize(buffer, len, &out_offset) || out_offset != len)
	{
		delete[] buffer;
		ifs.close();
		return false;
	}
	else if (!this->CheckBattleReportEventVersion(file_id, report))
	{
		delete[] buffer;
		ifs.close();
		return false;
	}
	else
	{
		*video_buffer = buffer;
		*video_len = len;
	}

	ifs.close();
	return true;
}

void BattleVideoMgr::Release()
{
	VideoMap::iterator it = m_video_map.begin();
	for (; it != m_video_map.end(); ++it)
	{
		if (NULL != it->second.video_buffer)
		{
			delete[] it->second.video_buffer;
			it->second.video_buffer = NULL;
		}
	}
	m_video_map.clear();

	FirstKillVideoMap::iterator first_kill_it = m_first_kill_video_map.begin();
	for (; first_kill_it != m_first_kill_video_map.end(); ++first_kill_it)
	{
		if (NULL != first_kill_it->second.video_buffer)
		{
			delete[] first_kill_it->second.video_buffer;
			first_kill_it->second.video_buffer = NULL;
		}
	}
	m_first_kill_video_map.clear();
}

bool BattleVideoMgr::CheckBattleReportEventVersion(long long file_id, const Protocol::SCBattleReport& report)
{
	if (report.sc_battleinfo_version != Protocol::SCBattleInfo::VERSION)
	{
		gamelog::g_log_video.printf(LL_WARNING, "CheckBattleReportEventVersion file_id[%lld] report_SCBattleInfo_Version[%d] cur_SCBattleInfo_Version[%d]",
			file_id, report.sc_battleinfo_version, Protocol::SCBattleInfo::VERSION);
		return false;
	}

	if (report.sc_battleroundresult_version != Protocol::SCBattleRoundResult::VERSION)
	{
		gamelog::g_log_video.printf(LL_WARNING, "CheckBattleReportEventVersion file_id[%lld] report_SCBattleRoundResult_Version[%d] cur_SCBattleRoundResult_Version[%d]",
			file_id, report.sc_battleroundresult_version, Protocol::SCBattleRoundResult::VERSION);
		return false;
	}

	BattleEventVersionInit();
	for (size_t i = 0; i < report.event_version_list.size(); ++i)
	{
		short event_type = report.event_version_list[i].event_type;
		UInt16 event_version = report.event_version_list[i].event_version;
		if (event_type < 0 || event_type >= ARRAY_ITEM_COUNT(BATTLE_EVENT_VERSION))
		{
			gamelog::g_log_video.printf(LL_WARNING, "CheckBattleReportEventVersion EVENT_TYPE_ERROR file_id[%lld] event_type[%d]",
				file_id, static_cast<int>(event_type));
			return false;
		}
		if (event_version != BATTLE_EVENT_VERSION[event_type])
		{
			gamelog::g_log_video.printf(LL_WARNING, "CheckBattleReportEventVersion file_id[%lld] event_type[%d] report_Version[%d] cur_Version[%d]",
				file_id, static_cast<int>(event_type), static_cast<int>(event_version), static_cast<int>(BATTLE_EVENT_VERSION[event_type]));
			return false;
		}
	}
	
	return true;
}

void RMIInitBattleVideoDataBackObjectImpl::InitBattleVideoDataRet(int ret, const BattleVideoListParam& list_param)
{
	if (0 != ret)
	{
		gamelog::g_log_world.printf(LL_ERROR, "BattleVideoManager Init Failed! InitBattleVideoDataRet ret[%d]\n", ret);
		ServerLogic::GetInstServerLogic()->StopServer(__FUNCTION__);
		return;
	}

	BattleVideoMgr::Instance().InitVideoList(list_param);
}

void RMISaveBattleVideoDataBackObjectImpl::SaveBattleVideoDataRet(int ret)
{
	if (ret != 0)
	{
		gamelog::g_log_world.printf(LL_MAINTANCE, "SaveBattleVideoDataRet ret[%d]", ret);
	}
}
