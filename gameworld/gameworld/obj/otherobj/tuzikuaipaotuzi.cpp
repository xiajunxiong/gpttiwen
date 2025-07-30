#include "scene/scene.h"
#include "config/logicconfigmanager.hpp"
#include "engineadapter.h"
#include "protocol/cycleactivity/msgtuzikuaipao.hpp"
#include "gamelog.h"

#include "other/cycleactivity/tuzikuaipao/tuzikuaipaoconfig.hpp"
#include "scene/speciallogicimpl/speciallogictuzikuaipao.hpp"
#include "tuzikuaipaotuzi.hpp"

TuZiKuaiPaoTuZi::TuZiKuaiPaoTuZi() : Character(OBJ_TYPE_TU_ZI_KAI_PAO_TU_ZI)
{
	m_tu_zi_type = 0;
	m_role_id = 0;
	m_track_type = 0;

	m_run_time = 0;
	m_rest_time = 0;

	m_next_send_pos_info_time = 0;
}

TuZiKuaiPaoTuZi::~TuZiKuaiPaoTuZi()
{
}

void TuZiKuaiPaoTuZi::Init(int role_id, int tu_zi_type, int track_type,int track_point_group_index, Posi end_posi)
{
	m_role_id = role_id;
	m_tu_zi_type = tu_zi_type;
	m_track_type = track_type;
	m_track_point_group_index = track_point_group_index;
	m_end_pos = end_posi;

}

void TuZiKuaiPaoTuZi::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	Character::Update(interval, now_second, now_dayid);

	SpecialLogic * logic = m_scene->GetSpecialLogic();
	if (!logic || logic->GetSceneType() != Scene::SCENE_TYPE_TU_ZI_KUAI_PAO)
	{
		return;
	}
	SpecialLogicTuZiKuaiPao * tu_zi_logic = (SpecialLogicTuZiKuaiPao *)logic;
	if (!tu_zi_logic->IsStart())
	{
		return;
	}


	Posi target_pos = m_end_pos;
	Posi now_pos = this->GetPos();
	Posi deltaP = target_pos - now_pos;

	if (!deltaP.IsLongerThan(2))
	{
		tu_zi_logic->SetTrackResult(m_track_type, m_role_id);
		return;
	}

	const TuZiKuaiPaoTuZiCfg * cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoTuZiCfg(m_tu_zi_type);
	if (!cfg)
	{
		return;
	}
	//const TuZiKuaiPaoOtherCfg & other_cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoOtherCfg();

	if (now_second>= m_next_send_pos_info_time)
	{
		m_next_send_pos_info_time = now_second + 1;

		Protocol::SCTuZiKuaiPaoPosInfo p;
		p.obj_id = this->GetId();
		p.x = now_pos.x;
		p.y = now_pos.y;

		m_scene->GetZoneMatrix()->NotifyAreaMsg(m_obser_handle, (void *)&p, sizeof(p));
	}

	if (m_param.add_speed_time !=0 && now_second >= m_param.add_speed_time)
	{
		m_param.add_speed_time = 0;
		this->SetSpeed(cfg->speed);
	}

	switch (this->GetActionStatus())
	{
		//未移动 / 抵达
		case Character::ACTION_IDLE:
			{
				if (now_second < m_rest_time)
				{
					return;
				}
				
				if (m_rest_time != 0) //这个不等于0，说明是休息后跑动，通知客户端
				{

					Protocol::SCTuZiKuaiPaoTuZiRun protocol;
					protocol.role_id = m_role_id;
					protocol.obj_id = this->GetId();
					protocol.track_type = m_track_type;

					m_scene->SendToRole((char *)&protocol, sizeof(protocol));
				}

				int r = RandomNum(cfg->rand_run_min_sec, cfg->rand_run_max_sec);
				m_run_time = std::max(now_second + r, m_param.add_speed_time);
				this->SetMoveLogic(m_end_pos);

				gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d run track_type:%d tu_zi_type:%d role_id:%d r:%d time:%ud x:%d y:%d", __FUNCTION__, __LINE__,
					m_scene->GetSceneKey(), m_track_type, m_tu_zi_type, m_role_id, r, (unsigned int)m_run_time, now_pos.x, now_pos.y);
			}
			break;
			//移动中
		case Character::ACTION_RUN:
			{
				if (now_second < m_run_time)
				{
					return;
				}

				if (m_run_time != 0) //这个不等于0，说明是休息后跑动，通知客户端
				{
					Protocol::SCTuZiKuaiPaoTuZiRest protocol;
					protocol.role_id = m_role_id;
					protocol.obj_id = this->GetId();
					protocol.track_type = m_track_type;
					m_scene->SendToRole((char *)&protocol, sizeof(protocol));
				}

				m_rest_time = now_second + cfg->rest_sec;
				this->ActionStop();

				gamelog::g_tu_zi_kuai_pao.printf(LL_INFO, "%s  line:%d | key:%d rest track_type:%d tu_zi_type:%d role_id:%d  rest_time:%ud x:%d y:%d", __FUNCTION__, __LINE__,
					m_scene->GetSceneKey(), m_track_type, m_tu_zi_type, m_role_id, (unsigned int)m_rest_time, now_pos.x, now_pos.y);
			}
			break;
		default:
			return;
	}
}

int TuZiKuaiPaoTuZi::GetTuZiType()
{
	return m_tu_zi_type;
}

int TuZiKuaiPaoTuZi::GetTrackType()
{
	return m_track_type;
}

int TuZiKuaiPaoTuZi::GetTrackPointGroupIndex()
{
	return m_track_point_group_index;
}

int TuZiKuaiPaoTuZi::GetCreateRoleId()
{
	return m_role_id;
}

void TuZiKuaiPaoTuZi::Encourage(bool is_add_speed)
{
	time_t now = EngineAdapter::Instance().Time();
	m_rest_time = now;

	if (is_add_speed)
	{
		const TuZiKuaiPaoOtherCfg & other_cfg = LOGIC_CONFIG->GetTuZiKuaiPaoConfig()->GetTuZiKuaiPaoOtherCfg();
	
		if (now < m_param.add_speed_time)
		{
			m_param.add_speed_time += other_cfg.add_speed_sec;
		}
		else
		{
			m_param.add_speed_time = now + other_cfg.add_speed_sec;
			this->SetSpeed(this->GetSpeed() + other_cfg.carrot_add_speed);
		}
		
		m_run_time = std::max(m_run_time, m_param.add_speed_time);	
	}	
}

TuZiKuaiPaoTuZiParam & TuZiKuaiPaoTuZi::GetTuZiParam()
{
	return m_param;
}
