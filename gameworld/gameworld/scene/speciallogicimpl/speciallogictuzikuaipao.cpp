#include "engineadapter.h"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "config/logicconfigmanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "gamecommon.h"
#include "gamelog.h"
#include "global/team/team.hpp"

#include "obj/otherobj/tuzikuaipaotuzi.hpp"
#include "other/cycleactivity/tuzikuaipao/tuzikuaipaoconfig.hpp"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "speciallogictuzikuaipao.hpp"

SpecialLogicTuZiKuaiPao::SpecialLogicTuZiKuaiPao(Scene * scene) : SpecialLogic(scene)
{
	m_is_finsih = 0;
	m_is_create_tu_zi = 0;
	m_phase = 1;
	m_next_phase_time = 0;
	ArrayFillZero(m_result);

	m_tu_zi_kuai_pao_phase_time[TU_ZI_KUAI_PAO_PHASE_TYPE_SET1] = TU_ZI_KUAI_PAO_SET_DURATION;
	m_tu_zi_kuai_pao_phase_time[TU_ZI_KUAI_PAO_PHASE_TYPE_SET2] = TU_ZI_KUAI_PAO_SET_DURATION;
	m_tu_zi_kuai_pao_phase_time[TU_ZI_KUAI_PAO_PHASE_TYPE_READY] = 10;
	m_tu_zi_kuai_pao_phase_time[TU_ZI_KUAI_PAO_PHASE_TYPE_START] = 9999;
} 

SpecialLogicTuZiKuaiPao::~SpecialLogicTuZiKuaiPao()
{
}

void SpecialLogicTuZiKuaiPao::Update(unsigned long interval, time_t now_second)
{
	if (m_is_finsih)
	{
		return;
	}

	this->UpdateSetInfo(interval,now_second);
	this->UpdateMatch(interval, now_second);
}

void SpecialLogicTuZiKuaiPao::UpdateSetInfo(unsigned long interval, time_t now_second)
{
	if (m_phase > TU_ZI_KUAI_PAO_PHASE_TYPE_SET2)
	{
		return;
	}

	if (now_second < m_next_phase_time)
	{
		for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
		{
			const SpecialLogicTuZiKuaiPaoRoleInfo & curr = iter->second;
			if (!curr.IsSet(m_phase, true))
			{
				return;
			}
		}
	}

	for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		SpecialLogicTuZiKuaiPaoRoleInfo & curr = iter->second;
		curr.SetDefault(m_phase);
	}
	
	m_phase += 1;
	m_next_phase_time = now_second + m_tu_zi_kuai_pao_phase_time[m_phase];
	this->SendSceneInfo();
}

void SpecialLogicTuZiKuaiPao::UpdateMatch(unsigned long interval, time_t now_second)
{
	if (m_phase < TU_ZI_KUAI_PAO_PHASE_TYPE_READY)
	{
		return;
	}

	this->CreateTuZi();

	if (m_phase < TU_ZI_KUAI_PAO_PHASE_TYPE_START)
	{
		if (now_second >= m_next_phase_time)
		{
			m_phase += 1;
			m_next_phase_time = now_second + m_tu_zi_kuai_pao_phase_time[m_phase];
			this->SendSceneInfo();
		}

		return;
	}

	this->Checkfinish();
}

void SpecialLogicTuZiKuaiPao::OnRoleEnterScene(Role * role)
{
	int role_id = role->GetUID();

	const SpecialLogicTuZiKuaiPaoRoleInfo * info_ptr = MapValuesConstPtr(m_role_info, role_id);
	if (!info_ptr && (int)m_role_info.size() >= TU_ZI_KUAI_PAO_ROLE_NUM)
	{
		this->DelayKickOutRole(role);
		return;
	}

	SpecialLogicTuZiKuaiPaoRoleInfo & info = m_role_info[role_id];
	info.role_id = role_id;
	info.is_leader = role->IsLeader();

	this->InitSceneInfo();
	this->SendSceneInfo(role);
	this->SendRoleInfo(role);

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d role_id:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), role_id);
}

void SpecialLogicTuZiKuaiPao::OnRoleLeaveScene(Role * role, bool is_logout)
{
	if (is_logout || this->IsFinish())
	{
		return; 
	}
	
	int win_role_id = 0;
	int role_id = role->GetUID();
	for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		if (iter->first != role_id)
		{
			win_role_id = iter->first;
			break;
		}
	}

	if (win_role_id == 0)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_result); ++i)
	{
		m_result[i] = win_role_id;
	}

	this->SendSceneInfo();
	this->Checkfinish();

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d role_id:%d win_role_id:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), role_id, win_role_id);
}

bool SpecialLogicTuZiKuaiPao::CanDestroy()
{
	return true;
}

void SpecialLogicTuZiKuaiPao::Set(Role * role, int(&tu_zi_type)[TU_ZI_KUAI_PAO_MAX_TRACK_NUM])
{
	if (!role)
	{
		return;
	}

	int role_id=role->GetUID();
	SpecialLogicTuZiKuaiPaoRoleInfo * info_ptr = MapValuesPtr(m_role_info, role_id);
	if (!info_ptr)
	{
		return;
	}

	int index = -1;
	if (m_phase == TU_ZI_KUAI_PAO_PHASE_TYPE_SET1)
	{
		index = 0;
	}

	if (m_phase == TU_ZI_KUAI_PAO_PHASE_TYPE_SET2)
	{
		index = 1;
	}

	if (index < 0 || index >= TU_ZI_KUAI_PAO_SET_TIMES)
	{
		return;
	}

	std::set<int> all_tu_zi_type = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetAllTuZiType();
	for (int i = 0; i < ARRAY_LENGTH(tu_zi_type); ++i)
	{
		int type = tu_zi_type[i];
		if (type != 0 && all_tu_zi_type.erase(type) == 0)
		{
			return;
		}
	}

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d role_id:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), role_id);
	SameArrayTypeCopy(info_ptr->set_tu_zi[index], tu_zi_type);
	this->SendSceneInfo(role);
}

void SpecialLogicTuZiKuaiPao::SetReady(Role * role)
{
	if (!role)
	{
		return;
	}

	int role_id = role->GetUID();
	SpecialLogicTuZiKuaiPaoRoleInfo * info_ptr = MapValuesPtr(m_role_info, role_id);
	if (!info_ptr)
	{
		return;
	}

	int index = -1;
	if (m_phase == TU_ZI_KUAI_PAO_PHASE_TYPE_SET1)
	{
		index = 0;
	}

	if (m_phase == TU_ZI_KUAI_PAO_PHASE_TYPE_SET2)
	{
		index = 1;
	}

	if (index < 0 || index >= TU_ZI_KUAI_PAO_SET_TIMES)
	{
		return;
	}

	info_ptr->set_tu_zi_ready[index] = 1;
	this->SendSceneInfo(role);
}

void SpecialLogicTuZiKuaiPao::Encourage(Role * role, int obj_id, int encourage_type)
{
	Obj * obj = m_scene->GetObj(obj_id);
	if (NULL == obj) return;

	if (obj->GetObjType() != Obj::OBJ_TYPE_TU_ZI_KAI_PAO_TU_ZI)
	{
		return;
	}

	int role_id = role->GetUID();
	TuZiKuaiPaoTuZi * tuzi = (TuZiKuaiPaoTuZi *)obj;
	if (tuzi->GetCreateRoleId() != role_id)
	{
		return;
	}

	SpecialLogicTuZiKuaiPaoRoleInfo * info_ptr = MapValuesPtr(m_role_info, role_id);
	if (!info_ptr)
	{
		return;
	}

	const TuZiKuaiPaoOtherCfg & other_cfg= LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoOtherCfg();
	const TuZiKuaiPaoEncourageCfg * cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoEncourageCfg(encourage_type);
	if (!cfg)
	{
		return;
	}

	if (cfg->has_add_speed)
	{
		if (info_ptr->use_carrot_times >= other_cfg.use_carrot_max_times)
		{
			return;
		}
	}

	if (cfg->succ_rate.empty())
	{
		return;
	}

	TuZiKuaiPaoTuZiParam & p = tuzi->GetTuZiParam();
	int index = std::min(p.encourage_fail_times, (int)cfg->succ_rate.size() - 1);

	int r = RandomNum(100);
	bool is_succ = false;
	if (r >= cfg->succ_rate[index])
	{
		p.encourage_fail_times += 1;
	}
	else
	{
		is_succ = true;
		p.encourage_fail_times = 0;
		p.encourage_succ_times += 1;
		tuzi->Encourage(cfg->has_add_speed);
	}
	
	if (cfg->has_add_speed)
	{
		info_ptr->use_carrot_times += 1;
	}	

	Protocol::SCTuZiKuaiPaoEncourageResult protocol;
	protocol.encourage_type = encourage_type;
	protocol.is_succ = is_succ;
	protocol.succ_times = p.encourage_succ_times;
	protocol.fail_times = p.encourage_fail_times;
	protocol.obj_id = obj_id;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

	this->SendRoleInfo(role);

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d role_id:%d tuzi_type:%d  encourage_type:%d is_succ:%d", __FUNCTION__, __LINE__,
		m_scene->GetSceneKey(), role_id, tuzi->GetTuZiType(), encourage_type, (int)is_succ);
}

void SpecialLogicTuZiKuaiPao::Leave(Role * role)
{
	//这里结果离开，队员取消暂离。离开场景

	if (!this->IsFinish())
	{
		return;
	}

	Team * team = role->GetMyTeam();
	if (!team)
	{
		this->DelayKickOutRole(role);
		return;
	}

	for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		TeamMember * info = team->GetMemberInfoByIndex(i);
		if (!info)
		{
			continue;
		}

		Role * tmp_role = m_scene->GetRoleByUID(info->uid);
		if (!tmp_role)
		{
			continue;
		}

		if (!info->IsFollow() && team->GetLeaderUID() != info->uid)
		{
			team->OnTemporarilyPart(tmp_role);
		}

		this->DelayKickOutRole(tmp_role);
	}
}

void SpecialLogicTuZiKuaiPao::GmOp(int type, int p1, int p2)
{
	time_t now = EngineAdapter::Instance().Time();

	if (m_phase < TU_ZI_KUAI_PAO_PHASE_TYPE_START)
	{
		m_next_phase_time = now;
	}
}

bool SpecialLogicTuZiKuaiPao::IsFinish()
{
	return m_is_finsih;
}

bool SpecialLogicTuZiKuaiPao::IsStart()
{
	return m_phase == TU_ZI_KUAI_PAO_PHASE_TYPE_START;
}

void SpecialLogicTuZiKuaiPao::SetTrackResult(int tack_id, int role_id)
{
	if (this->IsFinish())
	{
		return;
	}

	int index = tack_id;
	if (index < 0 || index >= ARRAY_LENGTH(m_result))
	{
		return;
	}

	if (m_result[index] != 0)
	{
		return;
	}

	m_result[index] = role_id;
	this->SendSceneInfo();

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d tack_id:%d role_id:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), tack_id, role_id);
}

void SpecialLogicTuZiKuaiPao::SendSceneInfo(Role * role)
{
	Protocol::SCTuZiKuaiPaoSceneInfo protocol;
	protocol.is_finsih = m_is_finsih;
	protocol.phase = m_phase;
	protocol.next_phase_time = (unsigned int)m_next_phase_time;
	SameArrayTypeCopy(protocol.result, m_result);

	int count = 0;
	for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end() && count < ARRAY_LENGTH(protocol.role_set_info); ++iter)
	{
		const SpecialLogicTuZiKuaiPaoRoleInfo & curr = iter->second;
		Protocol::TuZiKuaiPaoSetInfo & info = protocol.role_set_info[count++];

		info.role_id = curr.role_id;		
		SameArrayTypeCopy(info.set_tu_zi, curr.set_tu_zi);
		SameArrayTypeCopy(info.is_ready, curr.set_tu_zi_ready);
	}
	
	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		m_scene->SendToRole((char *)&protocol, sizeof(protocol));
	}
}

void SpecialLogicTuZiKuaiPao::SendRoleInfo(Role * role)
{
	if (!role)
	{
		return;
	}

	int role_id = role->GetUID();
	SpecialLogicTuZiKuaiPaoRoleInfo * info_ptr = MapValuesPtr(m_role_info, role_id);
	if (!info_ptr)
	{
		return;
	}

	Protocol::SCTuZiKuaiPaoRoleInfo protocol;
	protocol.use_carrot_times = info_ptr->use_carrot_times;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void SpecialLogicTuZiKuaiPao::InitSceneInfo()
{
	if (m_phase != 0 && m_next_phase_time != 0)
	{
		return;
	}

	m_phase = 1;
	m_next_phase_time = EngineAdapter::Instance().Time() + TU_ZI_KUAI_PAO_SET_DURATION;
}

void SpecialLogicTuZiKuaiPao::CreateTuZi()
{
	if (m_phase <= TU_ZI_KUAI_PAO_SET_TIMES)
	{
		return;
	}

	if (m_is_create_tu_zi)
	{
		return;
	}

	m_is_create_tu_zi = true;

	for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		const SpecialLogicTuZiKuaiPaoRoleInfo & curr = iter->second;
		
		const int(&final_set_info)[TU_ZI_KUAI_PAO_MAX_TRACK_NUM] = curr.set_tu_zi[TU_ZI_KUAI_PAO_SET_TIMES - 1];

		for (int track_type = 0; track_type < ARRAY_LENGTH(final_set_info); ++track_type)
		{
			int tu_zi_type = final_set_info[track_type];
			const TuZiKuaiPaoTuZiCfg * cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoTuZiCfg(tu_zi_type);
			if (!cfg)
			{
				continue;
			}

			const TuZiKuaiPaoTrackCfg * track_cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoTrackCfg(track_type);
			if (!track_cfg)
			{
				continue;
			}
			
			int track_point_group_index = curr.is_leader ? 0 : 1;
			const TuZiKuaiPaoTrackPointGroup * point_cfg = MapValuesConstPtr(track_cfg->point_cfg, track_point_group_index);
			if (!point_cfg)
			{
				continue;
			}
			
			Posi delta_pos = point_cfg->end_pos - point_cfg->start_pos;

			TuZiKuaiPaoTuZi * tuzi = new TuZiKuaiPaoTuZi();
			tuzi->SetPos(point_cfg->start_pos);
			tuzi->SetDir(gamecommon::GetMoveDirFloat(Posf((float)delta_pos.x, (float)delta_pos.y), NULL));
			tuzi->Init(curr.role_id, tu_zi_type, track_type, track_point_group_index, point_cfg->end_pos);
			tuzi->SetSpeed(cfg->speed);
			m_scene->AddObj(tuzi);


			gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d track_type:%d tu_zi_type:%d role_id:%d", __FUNCTION__, __LINE__, 
				m_scene->GetSceneKey(), track_type, tu_zi_type, curr.role_id);
		}
	}
}

void SpecialLogicTuZiKuaiPao::Checkfinish()
{
	if (m_is_finsih)
	{
		return;
	}

	bool can_finish = false;
	std::map<int, int> role_to_win_num;
	for (int i = 0; i < ARRAY_LENGTH(m_result); ++i)
	{
		int role_id = m_result[i];
		if (role_id == 0)
		{
			continue;
		}

		role_to_win_num[role_id] += 1;
		if (role_to_win_num[role_id] > TU_ZI_KUAI_PAO_MAX_TRACK_NUM / 2)
		{
			can_finish = true;
		}
	}

	if (!can_finish)
	{
		return;
	}

	m_is_finsih = true;
	
	int win_role_id = 0;
	int win_times = 0;
	for (std::map<int, int>::iterator iter = role_to_win_num.begin(); iter != role_to_win_num.end(); ++iter)
	{
		if (win_role_id == 0 || iter->second > win_times)
		{
			win_role_id = iter->first;
			win_times = iter->second;
		}
	}

	gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d win_role_id:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), win_role_id);

	this->SendSceneInfo();
	Protocol::SCTuZiKuaiPaoFinishNotice protocol;
	protocol.win_role_id = win_role_id;
	m_scene->SendToRole((char *)&protocol, sizeof(protocol));

	for (std::map<int, SpecialLogicTuZiKuaiPaoRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		const SpecialLogicTuZiKuaiPaoRoleInfo & curr = iter->second;		
		Role * r = m_scene->GetRoleByUID(curr.role_id);
		if (r)
		{
			int reward_times = r->GetRoleModuleManager()->GetTuZiKuaiPaoRewardTimes();
			bool is_win = (curr.role_id == win_role_id);
			const TuZiKuaiPaoOtherCfg & other_cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoOtherCfg();
			if (reward_times >= other_cfg.reward_times)
			{
				continue;
			}
			r->GetRoleModuleManager()->GetKnapsack()->PutVec(is_win ? other_cfg.win_rewards : other_cfg.lose_rewards, PUT_REASON_TU_ZI_SAI_PAO);
			r->GetRoleModuleManager()->SetTuZiKuaiPaoRewardTimes(reward_times + 1);

			gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d role_id:%d is_win:%d", __FUNCTION__, __LINE__, m_scene->GetSceneKey(), curr.role_id, is_win);
		}
	}
}

SpecialLogicTuZiKuaiPaoRoleInfo::SpecialLogicTuZiKuaiPaoRoleInfo()
{
	role_id = 0;
	is_leader = false;
	use_carrot_times = 0;
	ArrayFillZero(set_tu_zi);
	ArrayFillZero(set_tu_zi_ready);
}

bool SpecialLogicTuZiKuaiPaoRoleInfo::IsSet(int times, bool cheak_ready) const
{
	int index = times - 1;
	if (index < 0 || TU_ZI_KUAI_PAO_SET_TIMES <= index)
	{
		return false;
	}

	if (cheak_ready && !set_tu_zi_ready[index])
	{
		return false;
	}

	for (int i = 0; i < ARRAY_LENGTH(set_tu_zi[index]); ++i)
	{
		if (set_tu_zi[index][i] == 0)
		{
			return false;
		}
	}

	return true;
}

void SpecialLogicTuZiKuaiPaoRoleInfo::SetDefault(int times)
{
	int last_times = times - 1;

	const int index = times - 1;

	if (index < 0 || TU_ZI_KUAI_PAO_SET_TIMES <= index)
	{
		return;
	}

	int(&phase_set)[TU_ZI_KUAI_PAO_MAX_TRACK_NUM] = set_tu_zi[index];

	bool is_all_set = true;
	bool is_all_not_set = true;
	std::set<int> all_tu_zi_type = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetAllTuZiType();
	for (int i = 0; i < ARRAY_LENGTH(phase_set); ++i)
	{
		int type = phase_set[i];
		if (type == 0)
		{
			is_all_set = false;
		}
		else
		{
			is_all_not_set = false;
		}

		
		all_tu_zi_type.erase(type);
	}

	if (is_all_set)
	{
		return;
	}

	if (is_all_not_set && this->IsSet(last_times))
	{
		const int last_index = last_times - 1;
		if (last_index < 0 || TU_ZI_KUAI_PAO_SET_TIMES <= last_index)
		{
			return;
		}

		SameArrayTypeCopy(set_tu_zi[index], set_tu_zi[last_index]);	
	}
	else
	{
		for (int i = 0; i < ARRAY_LENGTH(phase_set); ++i)
		{
			if (phase_set[i] == 0 && !all_tu_zi_type.empty())
			{
				int type = *all_tu_zi_type.begin();
				phase_set[i] = type;
				all_tu_zi_type.erase(type);
			}
		}
	}
}