#include "mapunlock.hpp"
#include "mapunlockconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "protocol/msgmapunlock.h"
#include "servercommon/errornum.h"
#include "task/taskrecorder.h"
#include "gameworld/scene/scene.h"
#include "scene/fbmanager.hpp"
#include "other/bigdipper/bigdipper.hpp"
#include "other/flyupsystem/flyupsystem.hpp"

MapUnlock::MapUnlock() : gm_check_switch(true)
{
	
}

MapUnlock::~MapUnlock()
{
}

void MapUnlock::Init(RoleModuleManager * role_module_manager, const MapUnlockParam & map_unlock_data)
{
	m_role_module_mgr = role_module_manager;
	m_map_unlock_param = map_unlock_data;
}

void MapUnlock::GetOtherInitParam(MapUnlockParam * map_unlock_data)
{
	* map_unlock_data = m_map_unlock_param;
}

void MapUnlock::OnEnterScene(int scene_id)
{
	if (scene_id < 0 || scene_id >= BIT_COUNT_OF_LONG_64 * SCENE_ENTER_RECORD_LIST_NUM_MAX) return;

	if (scene_id == m_role_module_mgr->GetRole()->GetScene()->GetSceneID())
	{
		int ectype_type = m_role_module_mgr->GetRole()->GetScene()->GetSceneType();
		if (ectype_type < 0 || ectype_type >= BIT_COUNT_OF_LONG_64 * ECTYPE_PASS_RECORD_LIST_NUM_MAX) return;

		int list_seq_fb = ectype_type / BIT_COUNT_OF_LONG_64;
		int posi_idx_fb = ectype_type % BIT_COUNT_OF_LONG_64;
		m_map_unlock_param.ectype_enter_flag[list_seq_fb] |= (1LL << posi_idx_fb);
	}

	int list_seq = scene_id / BIT_COUNT_OF_LONG_64;
	int posi_idx = scene_id % BIT_COUNT_OF_LONG_64;
	if (0 == (m_map_unlock_param.scene_enter_flag[list_seq] & (1LL << posi_idx)))
	{
		m_map_unlock_param.scene_enter_flag[list_seq] |= (1LL << posi_idx);
		this->SendSceneActiveID(LOGIC_CONFIG->GetMapUnlockConfig()->GetActiveID(scene_id));
	}

	int irregular_id = LOGIC_CONFIG->GetMapUnlockConfig()->GetIrregularID(scene_id);
	if (unlock_irregular_id_set.end() == unlock_irregular_id_set.find(irregular_id))
	{
		unlock_irregular_id_set.insert(irregular_id);
		active_irregular_id_set.insert(irregular_id);
		this->SendIrregularIdList();
	}
}

bool MapUnlock::IsRoleHaveBeenEntered(int scene_id)
{
	if (scene_id < 0 || scene_id >= BIT_COUNT_OF_LONG_64 * SCENE_ENTER_RECORD_LIST_NUM_MAX) return false;

	int list_seq = scene_id / BIT_COUNT_OF_LONG_64;
	int posi_idx = scene_id % BIT_COUNT_OF_LONG_64;
	return (m_map_unlock_param.scene_enter_flag[list_seq] & (1LL << posi_idx)) > 0;
}

void MapUnlock::OnRolePassEctype(int ectype_type)
{
	if (ectype_type < 0 || ectype_type >= BIT_COUNT_OF_LONG_64 * ECTYPE_PASS_RECORD_LIST_NUM_MAX) return;

	int list_seq = ectype_type / BIT_COUNT_OF_LONG_64;
	int posi_idx = ectype_type % BIT_COUNT_OF_LONG_64;
	m_map_unlock_param.ectype_pass_flag[list_seq] |= (1LL << posi_idx);
}

bool MapUnlock::IsRoleHaveBeenPassed(int ectype_type)
{
	if (ectype_type < 0 || ectype_type >= BIT_COUNT_OF_LONG_64 * ECTYPE_PASS_RECORD_LIST_NUM_MAX) return false;

	int list_seq = ectype_type / BIT_COUNT_OF_LONG_64;
	int posi_idx = ectype_type % BIT_COUNT_OF_LONG_64;
	return (m_map_unlock_param.ectype_pass_flag[list_seq] & (1LL << posi_idx)) > 0;
}

void MapUnlock::ReqestRemoveActiveIrregularID(int remove_id)
{
	active_irregular_id_set.erase(remove_id);
}

void MapUnlock::GmChangeCheckSwitch(int switch_state)
{
	gm_check_switch = (0 != switch_state);
}

void MapUnlock::OnRoleLogin()
{
	this->CheckAllSceneUnlock();
	this->SendIrregularIdList();
	this->SendFBTypeFirstEnter();
}

bool MapUnlock::CheckMapIsUnlock(int scene_id, int * param)
{
	if (!gm_check_switch) return true;

	std::vector<UnlockSceneInfo> tmp_info_list;
	LOGIC_CONFIG->GetMapUnlockConfig()->GetUnlockInfoBySceneID(scene_id, tmp_info_list);
	if (tmp_info_list.empty()) return true;		// 没有配置，说明不锁定，默认可进入

	for (std::vector<UnlockSceneInfo>::iterator loop = tmp_info_list.begin(); loop != tmp_info_list.end(); ++loop)
	{
		const UnlockCondition * con_cfg = LOGIC_CONFIG->GetMapUnlockConfig()->GetUnlockCondition(loop->condition_id);
		if (NULL == con_cfg) continue;
		
		if (NULL != param && con_cfg->unlock_type == MAP_UNLOCK_TYPE_LEVEL)
		{
			*param = con_cfg->unlock_param;
		}
		if(!this->CheckUnlockByCondition(con_cfg->unlock_type, con_cfg->unlock_param)) continue;
		
		return true;	// 只要有一个满足条件，就认为解锁
	}
	
	return false;
}

bool MapUnlock::CheckUnlockByCondition(int unlock_type, int unlock_param)
{
	UNSTD_STATIC_CHECK(6 == MAP_UNLOCK_TYPE_MAX);
	switch (unlock_type)
	{
	case MAP_UNLOCK_TYPE_TASK:
		if (!m_role_module_mgr->GetTaskRecorder()->HaveFinished(unlock_param)) return false;
		break;
	case MAP_UNLOCK_TYPE_LEVEL:
		if (m_role_module_mgr->GetRole()->GetLevel() < unlock_param) return false;
		break;
	case MAP_UNLOCK_TYPE_PASS_FB:
		{
			int fb_list_seq = unlock_param / BIT_COUNT_OF_LONG_64;
			int fb_posi_idx = unlock_param % BIT_COUNT_OF_LONG_64;
			if (0 == (m_map_unlock_param.ectype_pass_flag[fb_list_seq] & (1LL << fb_posi_idx))) return false;
		}
		break;
	case MAP_UNLOCK_TYPE_BIG_DIPPER:
		{
			if (!m_role_module_mgr->GetBigDipper()->HasPassSN(unlock_param))return false;
		}
		break;
	case MAP_UNLOCK_TYPE_FEISHEN:
		{
			if (m_role_module_mgr->GetFlyUpSystem()->GetFlyUpFlag() < unlock_param)
			{
				return false;
			}
		}
		break;
	default:
		return false;
	}

	return true;
}

void MapUnlock::SendSceneActiveID(int active_id)
{
	if (active_id <= 0) return;

	static Protocol::SCMapUnlockFristNotice mufn;
	mufn.active_id = active_id;
	m_role_module_mgr->NetSend((const char *)&mufn, sizeof(mufn));
}

void MapUnlock::SendIrregularIdList()
{
	Protocol::SCMapUnlockAllListInfo muali;
	
	memset(muali.irregular_flag_list, 0, sizeof(muali.irregular_flag_list));
	memset(muali.new_unlock_flag_list, 0, sizeof(muali.new_unlock_flag_list));
	for (std::set<int>::iterator ir_it = unlock_irregular_id_set.begin(); ir_it != unlock_irregular_id_set.end(); ++ir_it)
	{
		int irregular_id = *ir_it;
		if (irregular_id < 0 || irregular_id >= BIT_COUNT_OF_LONG_64 * IRREGULAR_ID_LIST_NUM_MAX) continue;

		int list_seq = irregular_id / BIT_COUNT_OF_LONG_64;
		int posi_idx = irregular_id % BIT_COUNT_OF_LONG_64;
		muali.irregular_flag_list[list_seq] |= (1LL << posi_idx);
	}
	
	for (std::set<int>::iterator notice_it = active_irregular_id_set.begin(); notice_it != active_irregular_id_set.end(); ++notice_it)
	{
		int notice_id = *notice_it;
		if (notice_id < 0 || notice_id >= BIT_COUNT_OF_LONG_64 * IRREGULAR_ID_LIST_NUM_MAX) continue;

		int notice_list_seq = notice_id / BIT_COUNT_OF_LONG_64;
		int notice_posi_idx = notice_id % BIT_COUNT_OF_LONG_64;
		muali.new_unlock_flag_list[notice_list_seq] |= (1LL << notice_posi_idx);
	}

	m_role_module_mgr->NetSend((const char *)&muali, sizeof(muali));
}

void MapUnlock::SendFBTypeFirstEnter()
{
	Protocol::SCMapUnlockFirstEnterFbType msg;
	msg.count = 0;
	memset(msg.fb_type_enter_flag, 0, sizeof(msg.fb_type_enter_flag));
	for (int i = 0; i < BIT_COUNT_OF_LONG_64 * ECTYPE_PASS_RECORD_LIST_NUM_MAX && i < Scene::SCENE_TYPE_COUNT; i++)
	{
		int notice_list_seq = i / BIT_COUNT_OF_LONG_64;
		msg.count = notice_list_seq + 1;
		int notice_posi_idx = i % BIT_COUNT_OF_LONG_64;
		if((m_map_unlock_param.ectype_enter_flag[notice_list_seq] & (1LL << notice_posi_idx)) > 0)
		{
			msg.fb_type_enter_flag[notice_list_seq] |= (1LL << notice_posi_idx);
		}
	}
	m_role_module_mgr->NetSend((const char *)&msg, (sizeof(msg) - (ECTYPE_PASS_RECORD_LIST_NUM_MAX - msg.count) * sizeof(msg.fb_type_enter_flag[0])));
}

void MapUnlock::OnRoleLevelUp()
{
	this->CheckAllSceneUnlock();
	this->SendIrregularIdList();
	this->SendFBTypeFirstEnter();
}

void MapUnlock::CheckAllSceneUnlock()
{
	int max_ir_id = LOGIC_CONFIG->GetMapUnlockConfig()->GetMaxIrregularID();
	for (int ir_idx = IRREGULAR_ID_BEGIN; ir_idx <= max_ir_id; ++ir_idx)
	{
		std::set<int> tmp_condition_id_set;
		LOGIC_CONFIG->GetMapUnlockConfig()->GetConditionIDListByIrregularID(ir_idx, tmp_condition_id_set);
		if (tmp_condition_id_set.empty()) continue;

		for (std::set<int>::iterator loop = tmp_condition_id_set.begin(); loop != tmp_condition_id_set.end(); ++loop)
		{
			const UnlockCondition * con_cfg = LOGIC_CONFIG->GetMapUnlockConfig()->GetUnlockCondition(*loop);
			if (NULL == con_cfg || !this->CheckUnlockByCondition(con_cfg->unlock_type, con_cfg->unlock_param)) continue;

			unlock_irregular_id_set.insert(ir_idx);
			break;		// 如果这个 irregular_id 解锁了，跳过相通 irregular_id 的其它检测；
		}
	}
}
