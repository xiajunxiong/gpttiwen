#include "engineadapter.h"
#include "global/shanhaibossmanager/shanhaibossconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "world.h"
#include "internalcomm.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/speciallogicimpl/speciallogiccommon.hpp"
#include "obj/character/role.h"
#include "shanhaibossmanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/msgshanhaiboss.hpp"
#include "gameworld/engineadapter.h"
#include "servercommon/commondata.hpp"
#include "gameworld/other/rolemodulemanager.hpp"
#include "shanhaibossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "npc/npcmanager.h"
#include "gameworld/gamelog.h"
#include "global/worldstatus/worldstatus.hpp"

ShanHaiBossManager & ShanHaiBossManager::Instance()
{
	static ShanHaiBossManager shan_hai_boss_mgr;
	return shan_hai_boss_mgr;
}

void ShanHaiBossManager::Init(const ShanHaiBossParam & param)
{
	m_param = param;
	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		m_monster_info[i].scene_id = m_param.npc_info[i].scene_id;
		m_monster_info[i].npc_id = m_param.npc_info[i].npc_id;
		m_monster_info[i].npc_pos = m_param.npc_info[i].npc_pos;
		m_monster_info[i].death_num = m_param.npc_info[i].death_num;
		m_monster_info[i].npc_seq = -1;
	}
	this->CreateLastBoss();
}

void ShanHaiBossManager::GetInitParam(ShanHaiBossParam * param)
{
	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		m_param.npc_info[i].Reset();

		m_param.npc_info[i].scene_id = m_monster_info[i].scene_id;
		m_param.npc_info[i].npc_id = m_monster_info[i].npc_id;
		m_param.npc_info[i].npc_pos = m_monster_info[i].npc_pos;
		m_param.npc_info[i].death_num = (short)m_monster_info[i].death_num;
		m_param.npc_info[i].sh_reserver = 0;
	}
	*param = m_param;
}

void ShanHaiBossManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;

	this->SetNoCreateTime();
}

void ShanHaiBossManager::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	int limit_lv = LOGIC_CONFIG->GetShanHaiBossConfig()->GetInitiateCfg(SHAN_HAI_BOSS_GRADE_TYPE_EASY);
	if (limit_lv < 0) return;

	if (limit_lv > WorldStatus::Instance().GetWorldLevel())
	{
		return;
	}

	if (0 == m_no_create_monster_begin_time || 0 == m_no_create_monster_end_time)
	{
		this->SetNoCreateTime();
	}

	if (m_no_create_monster_begin_time <= (unsigned int)now_second && (unsigned int)now_second < m_no_create_monster_end_time && 0 == m_no_create_monster_mark)
	{
		m_no_create_monster_mark = 1;
		m_param.next_refresh_timestamp = m_no_create_monster_end_time;
		this->DeleteBoss();
		this->SendBossInfo(NULL, true);
	}

	int idx = 0;
	if ((unsigned int)now_second > m_param.next_refresh_timestamp)
	{
		const tm* time = EngineAdapter::Instance().LocalTime();
		if (NULL == time) return;
		int w_day = time->tm_wday;

		this->DeleteBoss();

		std::vector<ShanHaiBossMonsterCfg> monster_list;
		std::vector<int> scene_list;
		std::set<int> scene_set_list;
		std::vector<Posi> pos_list;

		LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterListCfg(w_day, monster_list, scene_list, scene_set_list, pos_list);
		if (monster_list.size() != pos_list.size() || monster_list.size() != scene_list.size()) return;
		if (static_cast<int>(scene_set_list.size()) != LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().monster_type_coune) return;

		for (int i = 0; i < (int)monster_list.size() && i < SHAN_HAI_BOSS_MAXNUM; ++i)
		{
			limit_lv = LOGIC_CONFIG->GetShanHaiBossConfig()->GetInitiateCfg(monster_list[i].grade_type);
			if (limit_lv < 0) continue;

			if (limit_lv > WorldStatus::Instance().GetWorldLevel())
			{
				continue;
			}

			ShanHaiBossMonsterCfg& node = monster_list[i];

			Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_list[i], COMMON_SCENE_KEY);
			if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
			{
				printf("ShanHaiBossManager::Get Scene Failed scene_id[%d]\n", scene_list[i]);
				continue;
			}

			scene->GetSceneName();

			SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
			if (special_common == NULL)
			{
				continue;
			}

			int npc_seq = -1;
			if (special_common->CreateShanHaiBoss(node.npc_id, &npc_seq, pos_list[i]))
			{
				m_monster_info[idx].npc_id = node.npc_id;
				m_monster_info[idx].scene_id = scene_list[i];
				m_monster_info[idx].npc_seq = npc_seq;
				m_monster_info[idx].npc_pos = pos_list[i];
				m_monster_info[idx].death_num = 0;
				idx++;
			}
		}

		//准点刷新
		unsigned int zero_time = (unsigned int)(now_second + EngineAdapter::Instance().NextDayInterval(0, 0, 0) - SECOND_PER_DAY);
		m_param.next_refresh_timestamp = zero_time;
		while (m_param.next_refresh_timestamp < (unsigned int)now_second)
		{
			m_param.next_refresh_timestamp += LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().live_time;
		}

		this->SendBossInfo(NULL, true);

		// 传闻
		char string[1024] = { 0 };
		int len = 0;
		for (std::set<int>::const_iterator it = scene_set_list.begin(); it != scene_set_list.end(); ++it)
		{
			len += SNPRINTF(string + len, sizeof(*it), "%d", *it);
			len += SNPRINTF(string + len, sizeof(","), ",");
		}
		string[len + 1 - sizeof(",")] = '\0';
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_shanhaiboss_broadcast, string, 0);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
				0, 0, 0, 0);
		}

		m_no_create_monster_mark = 0;
		gamelog::g_log_shan_hai_boss.printf(LL_INFO, "ShanHaiBossManager::Update now_time[%u] next_refres_time[%u]", (unsigned int)now_second, m_param.next_refresh_timestamp);
	}
}

void ShanHaiBossManager::OnRoleLogin(Role * role)
{
	if (NULL == role) return;

	this->SendBossInfo(role);
	this->SendRoleInfo(role);
}

void ShanHaiBossManager::CreateLastBoss()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	if (static_cast<unsigned int>(EngineAdapter::Instance().Time()) >= m_param.next_refresh_timestamp) return;

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		const ShanHaiBossMonsterCfg* cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetMonsterCfg(m_monster_info[i].npc_id);
		if (NULL == cfg) continue;

		if(m_monster_info[i].death_num >= cfg->fight_times) continue;

		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(m_monster_info[i].scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			continue;
		}
		SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
		if (special_common == NULL)
		{
			continue;
		}

		special_common->CreateShanHaiBoss(m_monster_info[i].npc_id, &m_monster_info[i].npc_seq, m_monster_info[i].npc_pos);
	}
}

void ShanHaiBossManager::DeleteBoss()
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(m_monster_info[i].scene_id, COMMON_SCENE_KEY);
		if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE)
		{
			continue;
		}

		NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(m_monster_info[i].npc_seq); 
		if (NULL == npc_monster)
		{
			continue;
		}

		scene->DeleteObj(npc_monster->GetId());
	}
}

void ShanHaiBossManager::SendRoleInfo(Role * role)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	if (NULL == role) return;

	Protocol::SCShanHaiBossRoleInfo info;
	info.next_refresh_time = m_param.next_refresh_timestamp;
	info.today_times = role->GetRoleModuleManager()->GetShanHaiBossChallengeTime();

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void ShanHaiBossManager::HiddenSendRoleInfoToGameRole(int uid, int param)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCShanHaiBossRoleInfo info;
	info.next_refresh_time = m_no_create_monster_mark ? m_no_create_monster_end_time : m_param.next_refresh_timestamp;
	info.today_times = param;

	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char *)&info, sizeof(info));
}

void ShanHaiBossManager::SendASignleAllUser(int index)
{
	if (0 > index || index >= SHAN_HAI_BOSS_MAXNUM) return;
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCShanHaiBossSingleInfo info;

	info.boss_info.scene_id = m_monster_info[index].scene_id;
	info.boss_info.npc_id = m_monster_info[index].npc_id;
	info.boss_info.death_num = m_monster_info[index].death_num;

	InternalComm::Instance().SendToAllGameAllUserThroughCross((const char *)&info, sizeof(info));
}

void ShanHaiBossManager::SendBossInfo(Role * role, bool send_all/* = false*/)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	if (!send_all)
	{
		if (NULL == role) return;
	}

	Protocol::SCShanHaiBossInfo info;
	info.next_refresh_time = m_no_create_monster_mark ? m_no_create_monster_end_time : m_param.next_refresh_timestamp;
	info.count = 0;
	memset(info.boss_info, 0, sizeof(info.boss_info));

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		if (0 < m_monster_info[i].scene_id && 0 < m_monster_info[i].npc_id)
		{
			info.boss_info[info.count].scene_id = m_monster_info[i].scene_id;
			info.boss_info[info.count].npc_id = m_monster_info[i].npc_id;
			info.boss_info[info.count].death_num = m_monster_info[i].death_num;
			info.count++;
		}
	}

	int len = sizeof(info) - sizeof(info.boss_info[0]) * (SHAN_HAI_BOSS_MAXNUM - info.count);

	if (send_all)
	{
		World::GetInstWorld()->SendToAllGateway((const char*)&info, len);
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, len);
	}
}

void ShanHaiBossManager::HiddenSendBossInfoToGameRole(int uid, int param)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCShanHaiBossInfo info;
	info.next_refresh_time = m_param.next_refresh_timestamp;
	info.count = 0;
	memset(info.boss_info, 0, sizeof(info.boss_info));

	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		if (0 < m_monster_info[i].scene_id && 0 < m_monster_info[i].npc_id)
		{
			info.boss_info[info.count].scene_id = m_monster_info[i].scene_id;
			info.boss_info[info.count].npc_id = m_monster_info[i].npc_id;
			info.boss_info[info.count].death_num = m_monster_info[i].death_num;
			info.count++;
		}
	}
	int len = sizeof(info) - sizeof(info.boss_info[0]) * (SHAN_HAI_BOSS_MAXNUM - info.count);

	InternalComm::Instance().SendToGameUserThroughCross(uid, (const char *)&info, len);
}

void ShanHaiBossManager::OnKillMonster(int npc_seq_id, int npc_id)
{
	int idx = 0;
	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		if (m_monster_info[i].npc_seq == npc_seq_id && m_monster_info[i].npc_id == npc_id)
		{
			m_monster_info[i].death_num++;
			idx = i;
			break;
		}
	}

	/*Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(m_monster_info[idx].scene_id, COMMON_SCENE_KEY);
	if (scene == NULL || scene->GetSceneType() != Scene::SCENE_TYPE_COMMON_SCENE) return;

	SpecialLogicCommon * special_common = (SpecialLogicCommon *)scene->GetSpecialLogic();
	if (special_common == NULL) return;

	NPCMonster* npc_monster = NpcManager::Instance().GetNPCMonster(m_monster_info[idx].npc_seq);
	if (NULL == npc_monster) return;

	if (npc_monster->GetScene()->GetSceneID() != scene->GetSceneID() || npc_monster->GetScene()->GetSceneKey() != scene->GetSceneKey()) return;

	m_monster_info[idx].death_num = npc_monster->GetDeathNum();*/

	this->SendASignleAllUser(idx);
}

void ShanHaiBossManager::SetNoCreateTime()
{
	const ShanHaiBossOtherCfg & other_cfg = LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg();

	m_no_create_monster_begin_time = (unsigned int)((EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0) - (24 - other_cfg.close_begin_time) * SECOND_PER_HOUR));
	m_no_create_monster_end_time = (unsigned int)((EngineAdapter::Instance().Time() + EngineAdapter::Instance().NextDayInterval(0, 0, 0) - (24 - other_cfg.close_end_time) * SECOND_PER_HOUR));

	if (m_no_create_monster_begin_time > m_no_create_monster_end_time)
	{
		unsigned int temp = m_no_create_monster_begin_time;
		m_no_create_monster_begin_time = m_no_create_monster_end_time;
		m_no_create_monster_end_time = temp;
	}
}

void ShanHaiBossManager::GmRefreshShanHai()
{
	m_param.next_refresh_timestamp -= LOGIC_CONFIG->GetShanHaiBossConfig()->GetOtherCfg().live_time;
	gamelog::g_log_shan_hai_boss.printf(LL_INFO, "ShanHaiBossManager::GmRefreshShanHai next_refres_time[%u]", m_param.next_refresh_timestamp);
}

ShanHaiBossManager::ShanHaiBossManager()
{
	for (int i = 0; i < SHAN_HAI_BOSS_MAXNUM; ++i)
	{
		m_monster_info[i].Reset();
	}
	m_no_create_monster_begin_time = 0;
	m_no_create_monster_end_time = 0;
	m_no_create_monster_mark = 0;
}

ShanHaiBossManager::~ShanHaiBossManager()
{
}
