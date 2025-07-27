#include "matchengine.hpp"
#include "matchmanager.hpp"
#include "imatchlogic.hpp"
#include "engineadapter.h"

MatchEngine::MatchEngine() : m_mgr(NULL), m_has_init(false), m_team_id_generator(0),
  m_team_member_num(0), m_business_type(MATCH_BUSINESS_NONE),
  m_unfull_team_match_interval_ms(0u), m_full_team_match_interval_ms(0u),
  m_match_unfull_pool_countdown_ms(0u), m_match_full_pool_countdown_ms(0u),
  m_match_logic_ptr(NULL)
{

}

MatchEngine::~MatchEngine()
{
	if (NULL != m_match_logic_ptr)
	{
		delete m_match_logic_ptr;
	}

	for (size_t i = 0; i < m_unfull_team_pool.size(); ++i)
	{
		if (m_unfull_team_pool[i] != NULL)
		{
			m_unfull_team_pool[i]->Destroy();
			m_unfull_team_pool[i] = NULL;
		}
	}

	for (size_t i = 0; i < m_full_team_pool.size(); ++i)
	{
		if (m_full_team_pool[i] != NULL)
		{
			m_full_team_pool[i]->Destroy();
			m_full_team_pool[i] = NULL;
		}
	}
}

void MatchEngine::Init(MatchManager* mgr, const MatchEngineInitParam& param)
{
	if (m_has_init)
	{
		return;
	}

	m_has_init = true;

	m_mgr = mgr;
	m_team_member_num = param.team_member_num;
	m_business_type = param.business_type;
	m_match_logic_ptr = param.logic_ptr;
	m_unfull_team_match_interval_ms = param.unfull_team_match_interval_ms;
	m_full_team_match_interval_ms = param.full_team_match_interval_ms;

	m_match_unfull_pool_countdown_ms = m_unfull_team_match_interval_ms;
	m_match_full_pool_countdown_ms = m_full_team_match_interval_ms;
}

void MatchEngine::Reset()
{
	m_team_id_generator = 0;

	for (size_t i = 0; i < m_unfull_team_pool.size(); ++i)
	{
		MatchTeam* mt = m_unfull_team_pool[i];
		if (NULL == mt) continue;

		m_unfull_team_pool[i]->Destroy();
		m_unfull_team_pool[i] = NULL;
	}
	UnfullTeamPool().swap(m_unfull_team_pool);

	for (size_t i = 0; i < m_full_team_pool.size(); ++i)
	{
		MatchTeam* mt = m_full_team_pool[i];
		if (NULL == mt) continue;

		m_full_team_pool[i]->Destroy();
		m_full_team_pool[i] = NULL;
	}
	FullTeamPool().swap(m_full_team_pool);

	UserTeamHash().swap(m_user_team_hash);
}

void MatchEngine::Update(unsigned long interval_ms)
{
	if (!m_has_init) return;

	this->DoClearUnfullTeam();
	this->DoClearFullTeam();

	if (m_match_unfull_pool_countdown_ms < interval_ms)
	{
		this->DoTeamMatchTeam();
		this->DoClearUnfullTeam();

		m_match_unfull_pool_countdown_ms =
			interval_ms - m_match_unfull_pool_countdown_ms + m_unfull_team_match_interval_ms;
	}
	else
	{
		m_match_unfull_pool_countdown_ms -= interval_ms;
	}

	if (m_match_full_pool_countdown_ms < interval_ms)
	{
		this->DoTeamMatchEnemy();
		this->DoClearFullTeam();

		m_match_full_pool_countdown_ms =
			interval_ms - m_match_full_pool_countdown_ms + m_full_team_match_interval_ms;
	}
	else
	{
		m_match_full_pool_countdown_ms -= interval_ms;
	}
	//printf("countdown=%lu %lu\n", m_match_unfull_pool_countdown_ms, m_match_full_pool_countdown_ms);
}

bool MatchEngine::OnUserStartMatchReq(int user_num, int logic_type, MatchUser** user_list)
{
	if (!m_has_init) return false;

	if (user_num > m_team_member_num || user_num <= 0) return false;

	for (int i = 0; i < user_num; ++i)
	{
		if (this->IsUserMatching(user_list[i]->uid)) return false;
	}

	MatchTeam::MatchUserList mu_list;
	for (int i = 0; i < user_num; ++i)
	{
		mu_list.push_back(user_list[i]);
	}

	MatchTeam* mt = this->CreateMatchTeam(user_num, logic_type, mu_list);

	if (user_num == m_team_member_num)
	{
		m_full_team_pool.push_back(mt);
	}
	else
	{
		m_unfull_team_pool.push_back(mt);
	}

	m_mgr->OnTeamStartMatch(mt);

	return true;
}

bool MatchEngine::OnUserStopMatchReq(int user_num, int* uid_list)
{
	if (!m_has_init) return false;

	for (int i = 0; i < user_num; ++i)
	{
		int uid = uid_list[i];
		UserTeamHash::iterator it = m_user_team_hash.find(uid);
		if (it == m_user_team_hash.end()) continue;

		long long team_id = it->second;

		MatchTeam* mt = this->GetTeamByTeamID(team_id);
		if (NULL != mt && mt->IsValid())
		{
			this->DoRemoveUserOnTeam(mt, uid);
		}
	}

	return true;
}

MatchTeam* MatchEngine::CreateMatchTeam(int user_num, int logic_type, const MatchTeam::MatchUserList& mu_list)
{
	MatchTeam* mt = new MatchTeam();
	mt->team_id = this->GenerateTeamID();
	mt->logic_type = logic_type;
	mt->user_list = mu_list;
	mt->is_valid = true;
	mt->start_match_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	for (size_t i = 0; i < mt->user_list.size(); ++i)
	{
		int uid = mt->user_list[i]->uid;
		m_user_team_hash[uid] = mt->team_id;
	}

	this->RecalcMatchTeamLogic(mt);

	return mt;
}

long long MatchEngine::GenerateTeamID()
{
	return ++m_team_id_generator;
}

void MatchEngine::RecalcMatchTeamLogic(MatchTeam* mt)
{
	m_match_logic_ptr->RecalcMatchTeamLogic(mt);
}

void MatchEngine::DoTeamMatchTeam()
{
	//printf("~~~~~~~~~~DoTeamMatchTeam start~~~~~~~~~~~\n");

	for (size_t i = 0; i < m_unfull_team_pool.size(); ++i)
	{
		MatchTeam* mtA = m_unfull_team_pool[i];
		if (NULL == mtA || !mtA->IsValid()) continue;

		MatchTeam* full_match_team = NULL;
		std::vector<MatchTeam*> unfull_match_candidate_list;
		for (size_t k = i + 1; k < m_unfull_team_pool.size(); ++k)
		{
			MatchTeam* mtB = m_unfull_team_pool[k];
			if (NULL == mtB || !mtB->IsValid()) continue;

			if (static_cast<int>(mtA->user_list.size() + mtB->user_list.size()) > m_team_member_num) continue;

			if (m_match_logic_ptr->IsTeamMatchTeam(mtA, mtB))
			{
				if ((int)mtA->user_list.size() + (int)mtB->user_list.size() == m_team_member_num)
				{
					full_match_team = mtB;
					break;
				}
				else
				{
					unfull_match_candidate_list.push_back(mtB);
				}
			}
		}

		// 找到一个可以满人的队伍来合并
		if (full_match_team != NULL)
		{
			m_mgr->OnTeamMatchTeam(mtA, full_match_team);
			this->DoTeamCombine(mtA, full_match_team);
			m_full_team_pool.push_back(mtA);
			m_unfull_team_pool[i] = NULL; // 移交控制权给team_pool
		}
		// 找到至少一个可以合并的队伍但不满人
		else if (unfull_match_candidate_list.size() > 0)
		{
			MatchTeam* mtCombind = unfull_match_candidate_list[rand() % unfull_match_candidate_list.size()];
			m_mgr->OnTeamMatchTeam(mtA, mtCombind);

			this->DoTeamCombine(mtA, mtCombind);
		}
	}

	//printf("~~~~~~~~~~DoTeamMatchTeam end~~~~~~~~~~~~~\n");
}

void MatchEngine::DoClearUnfullTeam()
{
	UnfullTeamPool tmp_pool;
	tmp_pool.reserve(m_unfull_team_pool.size());
	tmp_pool.swap(m_unfull_team_pool);

	for (size_t i = 0; i < tmp_pool.size(); ++i)
	{
		MatchTeam* mt = tmp_pool[i];
		if (NULL == mt) continue;

		if (mt->IsValid())
		{
			m_unfull_team_pool.push_back(mt);
		}
		else
		{
			this->DoClearTeamUserHash(mt);
			mt->Destroy();
			tmp_pool[i] = NULL;
		}
	}
}

void MatchEngine::DoTeamMatchEnemy()
{
	//printf("~~~~~~~~~~DoTeamMatchEnemy start~~~~~~~~~~~\n");
	for (size_t i = 0; i < m_full_team_pool.size(); ++i)
	{
		MatchTeam* mtA = m_full_team_pool[i];
		if (NULL == mtA || !mtA->IsValid()) continue;

		for (size_t k = i + 1; k < m_full_team_pool.size(); ++k)
		{
			MatchTeam* mtB = m_full_team_pool[k];
			if (NULL == mtB || !mtB->IsValid()) continue;

			if (mtA->team_id == mtB->team_id) continue; // 出现了相同队伍匹配到一起了，暂时先改这里试试

			if (m_match_logic_ptr->IsTeamMatchEnemy(mtA, mtB))
			{
				m_mgr->OnTeamMatchEnemy(mtA, mtB);
				mtA->is_valid = false;
				mtB->is_valid = false;

				break;
			}
		}
	}
	//printf("~~~~~~~~~~DoTeamMatchEnemy end~~~~~~~~~~~~~\n");
}

void MatchEngine::DoClearFullTeam()
{
	FullTeamPool tmp_pool;
	tmp_pool.reserve(m_full_team_pool.size());
	tmp_pool.swap(m_full_team_pool);

	for (size_t i = 0; i < tmp_pool.size(); ++i)
	{
		MatchTeam* mt = tmp_pool[i];
		if (NULL == mt) continue;

		if (mt->IsValid())
		{
			if ((int)mt->user_list.size() != m_team_member_num)
			{
				m_unfull_team_pool.push_back(mt);
			}
			else
			{
				m_full_team_pool.push_back(mt);
			}
		}
		else
		{
			this->DoClearTeamUserHash(mt);
			mt->Destroy();
			tmp_pool[i] = NULL;
		}
	}
}

void MatchEngine::DoTeamCombine(MatchTeam* mtA, MatchTeam* mtB)
{
	for (size_t i = 0; i < mtB->user_list.size(); ++i)
	{
		mtA->user_list.push_back(mtB->user_list[i]);
		int uid = mtB->user_list[i]->uid;
		m_user_team_hash[uid] = mtA->team_id;
	}
	mtB->user_list.clear();
	mtB->is_valid = false;
}

void MatchEngine::DoClearTeamUserHash(MatchTeam* mt, int uid /*= 0*/)
{
	for (size_t k = 0; k < mt->user_list.size(); ++k)
	{
		if (0 == uid || mt->user_list[k]->uid == uid)
		{
			m_user_team_hash.erase(mt->user_list[k]->uid);
		}
	}
}

void MatchEngine::DoRemoveUserOnTeam(MatchTeam* mt, int uid)
{
	for (size_t k = 0; k < mt->user_list.size(); ++k)
	{
		if (mt->user_list[k]->uid == uid)
		{
			m_user_team_hash.erase(uid);
			m_mgr->OnUserLeaveTeam(mt, uid);

			std::swap(mt->user_list[k], mt->user_list[mt->user_list.size() - 1]);
			mt->user_list[mt->user_list.size() - 1]->Destroy();
			mt->user_list.pop_back();

			if (0 == mt->user_list.size())
			{
				mt->is_valid = false;
			}
			else
			{
				this->RecalcMatchTeamLogic(mt);
			}

			break;
		}
	}
}

MatchTeam* MatchEngine::GetTeamByTeamID(long long team_id)
{
	for (size_t i = 0; i < m_unfull_team_pool.size(); ++i)
	{
		if (team_id == m_unfull_team_pool[i]->team_id)
		{
			return m_unfull_team_pool[i];
		}
	}

	for (size_t i = 0; i < m_full_team_pool.size(); ++i)
	{
		if (team_id == m_full_team_pool[i]->team_id)
		{
			return m_full_team_pool[i];
		}
	}

	return NULL;
}

bool MatchEngine::IsUserMatching(int uid)
{
	return m_user_team_hash.find(uid) != m_user_team_hash.end();
}

