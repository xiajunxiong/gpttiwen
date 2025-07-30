#include "scene.h"
#include "gamelog.h"
#include "scenemanager.hpp"
#include "engineadapter.h"
#include "map.h"
#include "globalconfig/globalconfig.h"
#include "rmibackobjdef.h"
#include "internalcomm.h"
#include "obj/obj.h"
#include "obj/character/role.h"
#include "scene/asynreqregister/asynreqregister.hpp"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/errornum.h"
#include "scene/speciallogic.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/sundryconfig.hpp"
#include "config/otherconfig/sceneminemonsterconfig.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolemodulemanager.hpp"
#include "monster/monsterpool.hpp"
#include "protocol/msgscene.h"
#include "other/route/mailroute.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "other/pet/pet.hpp"
#include "other/partner/partner.hpp"
#include "other/rolecross/rolecross.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "other/buffmanager/buffmanager.hpp"
#include "world.h"

Posi GetPosFromInt(int pos_int)
{
	return Posi(pos_int % 10000, pos_int / 10000);
}

Scene::Scene(SceneManager *scene_manager) : m_index(0), m_scene_id(0), m_scene_key(0), m_scene_type(0), m_scene_manager(scene_manager),
	m_map(NULL), m_is_delay_delete_scene(false), m_delay_delete_scene_time(0u), m_scene_create_time(0u), m_scene_timeout(0), 
	m_is_timeout_kick_role(false), m_scene_level_min(0), m_scene_level_max(0), m_special_logic(NULL), m_is_sync_gather(true) //!< \todo 改回false 并读配置
{
	memset(m_scene_name, 0, sizeof(m_scene_name));
}

void Scene::UpdateObj(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	for (ObjManager::ObjIterator i = m_obj_manager.FirstObj(); i != m_obj_manager.LastObj(); ++i)
	{
		(*i)->Update(interval, now_second, now_dayid);
	}
}

void Scene::RemoveObj()
{
	if (m_remove_obj_list.size() != 0)
	{
		for (int i = 0; i < (int)m_remove_obj_list.size(); ++i)
		{
			if (!m_obj_manager.Exist(m_remove_obj_list[i]))
			{
				continue;
			}

			Obj *obj = m_obj_manager.GetObj(m_remove_obj_list[i]);
			if (NULL == obj) continue;

			m_special_logic->OnRemoveObj(obj);
		
			m_obj_manager.Remove(m_remove_obj_list[i]);

			switch (obj->GetObjType())
			{
			case Obj::OBJ_TYPE_INVALID:
				gamelog::g_log_scene.printf(LL_WARNING, "Scene:%s Remove Invalid Type Obj", m_scene_name);
				break;
			case Obj::OBJ_TYPE_ROLE:
				m_role_manager.Remove(obj);
				break;
			case Obj::OBJ_TYPE_NPC_MONSTER:
				m_npc_monster_manager.Remove(obj);
				break;
			case Obj::OBJ_TYPE_NPC_GATHERS:
				m_npc_gathers_manager.Remove(obj);
				break;
			case Obj::OBJ_TYPE_NPC_CHOSED:
				m_npc_chosed_manager.Remove(obj);
				break;
			case Obj::OBJ_TYPE_ROBOT:
				m_robot_manager.Remove(obj);
				break;
			default:
				break;
			}

			if (Obj::IsRandPos(obj->GetObjType()))
			{
				this->RemoveAvoidPos(obj->GetPos());
			}

			delete obj;

			UNSTD_STATIC_CHECK(12 == Obj::OBJ_TYPE_COUNT);
		}

		m_remove_obj_list.clear();

		if (!IsStaticScene(this->GetSceneType()))
		{
			if (0 == m_role_manager.Size() && m_special_logic->CanDestroy() && !m_is_delay_delete_scene)
			{
				m_scene_manager->DestroyFB(m_index);
			}
		}
	}
}

Scene::~Scene()
{
	this->Release();
	if (NULL != m_special_logic)
	{
		delete m_special_logic;
	}
}

bool Scene::Init(SceneIndex scene_index, const ConfigScene &configscene, Map *map, int scene_type, int scene_key, unsigned int scene_timeout, bool timeout_kick)
{
	assert(NULL != map);

	m_map = map;

	STRNCPY(m_scene_name, configscene.name.c_str(), sizeof(m_scene_name));

	m_scene_id = configscene.id;

	m_scene_type = scene_type;
	m_scene_key = scene_key;

	m_scene_create_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (!Scene::IsStaticScene(scene_type))
	{
		m_scene_timeout = scene_timeout;
		if (0 != scene_timeout) m_is_timeout_kick_role = timeout_kick;
	}

	m_special_logic = SpecialLogic::CreateSpecialLogic(this, scene_type);

	m_index = scene_index;

	m_role_manager.SetScene(this);
	m_role_manager.Init();

	m_npc_monster_manager.SetScene(this);
	m_npc_monster_manager.Init();

	m_npc_gathers_manager.SetScene(this);
	m_npc_gathers_manager.Init();

	m_robot_manager.SetScene(this);
	m_robot_manager.Init();

	m_npc_chosed_manager.SetScene(this);
	m_npc_chosed_manager.Init();

	Axis ru_x, ru_y;
	m_map->GetArea(&ru_x, &ru_y);

	// zone matrix 单元区域
	static const int ZONE_UNIT_X = (int)GLOBALCONFIG->GetKeyConfig_ZoneMatrixX();		// 地图单元的划分距离
	static const int ZONE_UNIT_Y = (int)GLOBALCONFIG->GetKeyConfig_ZoneMatrixY();
	ZoneMatrix::CreateZoneMatrix(&m_zone_matrix, ru_x, ru_y, ZONE_UNIT_X, ZONE_UNIT_Y, &m_obj_manager);

	m_transport_list.Init(configscene.doors);
	m_town_point.scene_id = configscene.townpoint.sceneid;
	m_town_point.pos.Init(configscene.townpoint.scenex, configscene.townpoint.sceney);

	m_mine_monster.Init(configscene);

	m_scene_level_min = configscene.scene_level_min;
	m_scene_level_max = configscene.scene_level_max;

	{
		ConfigSceneDoorList::const_iterator it_door = configscene.doors.begin();
		for (; it_door != configscene.doors.end(); ++it_door)
		{
			Posi pos(it_door->x, it_door->y);
			this->AddAvoidPos(pos);
		}
		Posi townpoint_pos(configscene.townpoint.scenex, configscene.townpoint.sceney);
		this->AddAvoidPos(townpoint_pos);
	
		const std::vector<Posi>* npc_list = NpcPool::Instance()->GetSceneClientNpcList(m_scene_id);
		if (NULL != npc_list)
		{
			std::vector<Posi>::const_iterator npc_it = npc_list->begin();
			for (; npc_it != npc_list->end(); ++npc_it)
			{
				this->AddAvoidPos(*npc_it);
			}
		}
	}

	gamelog::g_log_scene.printf(LL_INFO, "[%d:%d] %s start up.", m_scene_id, m_scene_key, m_scene_name);

	return true;
}

bool Scene::ReInit(const ConfigScene &configscene, Map *map)
{
	assert(NULL != map);

	m_map = map;

	if (configscene.id != m_scene_id) return false;

	m_transport_list.Init(configscene.doors);

	m_mine_monster.Init(configscene);

	m_town_point.scene_id = configscene.townpoint.sceneid;
	m_town_point.pos.Init(configscene.townpoint.scenex, configscene.townpoint.sceney);

	return true;
}

void Scene::Release()
{
	m_index = 0;
	m_zone_matrix.DestriyZoneMatrix();

	for (ObjManager::ObjIterator i = m_obj_manager.FirstObj(); i != m_obj_manager.LastObj(); ++i)
	{
		delete (*i);
	}
	m_obj_manager.Clear();
	m_role_manager.Release();
	m_npc_monster_manager.Release();

	m_remove_obj_list.clear();

	gamelog::g_log_scene.printf(LL_INFO, "[%d:%d] %s release.", m_scene_id, m_scene_key, m_scene_name);
}

void Scene::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	this->RemoveObj();
	this->UpdateObj(interval, now_second, now_dayid);
	//TODO 刷怪逻辑 m_scene_monster_logic.Update(interval);
	this->RemoveObj();

	if (m_is_timeout_kick_role && (m_scene_create_time + m_scene_timeout < now_second))
	{
		if (!m_special_logic->OnTimeout())
		{
			int role_num = m_role_manager.Size();  // fb 时间到 直接刷到 last_scene 

			for (int i = 0; i < role_num; ++i)
			{
				Role *role = this->GetRoleByIndex(i);
				if (NULL == role) continue;

				Posi last_scene_pos;
				int last_scene_id = role->GetLastScene(&last_scene_pos);

				gamelog::g_log_debug.printf(LL_DEBUG, "Scene::Update m_is_timeout_kick_role: %s[%d %d]", role->GetName(), role->GetUserId().db_index, role->GetUserId().role_id);

				m_scene_manager->GoTo(role, last_scene_id, 0, last_scene_pos);
			}
		}
	}

	m_special_logic->Update(interval, now_second);

	if (0 == m_role_manager.Size() && m_special_logic->CanDestroy() && m_is_delay_delete_scene && now_second >= m_delay_delete_scene_time)
	{
		m_is_delay_delete_scene = false; m_scene_manager->DestroyFB(m_index);
	}
}

Obj * Scene::GetObj(ObjID id)
{
	if (m_obj_manager.Exist(id))
	{
		Obj *obj = m_obj_manager.GetObj(id);

		if (!obj->IsValid()) return NULL;

		return obj;
	}

	return NULL;
}

void Scene::AddObj(Obj *obj)
{
	obj->SetScene(this);

	switch (obj->GetObjType())
	{
	case Obj::OBJ_TYPE_INVALID:
		gamelog::g_log_scene.printf(LL_WARNING, "Scene:%s Add Invalid Type Obj", m_scene_name);
		break;

	case Obj::OBJ_TYPE_ROLE:
		m_role_manager.Add(obj);
		break;

	case Obj::OBJ_TYPE_NPC_MONSTER:
		m_npc_monster_manager.Add(obj);
		break;

	case Obj::OBJ_TYPE_NPC_GATHERS:
		m_npc_gathers_manager.Add(obj);
		break;

	case Obj::OBJ_TYPE_ROBOT:
		m_robot_manager.Add(obj);
		break;

	case Obj::OBJ_TYPE_NPC_CHOSED:
		m_npc_chosed_manager.Add(obj);
		break;

	default:
		break;
	}

	if (Obj::IsRandPos(obj->GetObjType()))
	{
		this->AddAvoidPos(obj->GetPos());
	}

	m_obj_manager.Add(obj);
	m_special_logic->OnAddObj(obj);
	obj->OnEnterScene();

	if (Obj::OBJ_TYPE_NPC_GATHERS == obj->GetObjType() && this->IsSyncGather())
	{
		this->SyncObjInfo(obj, Protocol::SCSceneObjList::NOTIFY_REASON_ADD);
	}
}

bool Scene::DeleteObj(ObjID obj_id)
{
	if (!m_obj_manager.Exist(obj_id))
	{
		return false;
	}

	Obj *obj = m_obj_manager.GetObj(obj_id);
	if (NULL != obj && obj->IsValid())
	{
		obj->OnLeaveScene();
		obj->SetInvalid();

		m_remove_obj_list.push_back(obj_id); // 延迟删除操作 让Obj可以调用DeleteObj将自己移除

		if (this->IsSyncGather() && Obj::OBJ_TYPE_NPC_GATHERS == obj->GetObjType())
		{
			this->SyncObjInfo(obj, Protocol::SCSceneObjList::NOTIFY_REASON_DELETE);
		}
	}
	else
	{
		gamelog::g_log_debug.printf(LL_WARNING, "[Scene::DeleteObj Warring][sceneindex(%d) obj_id(%d) delete repeat]", m_index, obj_id);
	}

	return true;
}

Role * Scene::GetRoleByUID(int uid)
{
	for (int j = 0; j < (int)this->RoleNum(); ++j)
	{
		Role *temp_role = this->GetRoleByIndex(j);
		if (NULL != temp_role && Obj::OBJ_TYPE_ROLE == temp_role->GetObjType() && uid == temp_role->GetUID())
		{
			return temp_role;
		}
	}

	return NULL;
}

Role * Scene::GetRoleByIndex(int index)
{
	Role *role = (Role*)m_role_manager.GetObjByIndex(index);
	if (NULL != role && role->IsValid())
	{
		return role;
	}

	return NULL;
}

Role * Scene::GetRoleByObjID(ObjID obj_id)
{
	Obj *obj = this->GetObj(obj_id);
	if (NULL != obj && Obj::OBJ_TYPE_ROLE == obj->GetObjType())
	{
		return (Role*)obj;
	}

	return NULL;
}

NPCMonster * Scene::GetMonsterByIndex(int index)
{
	NPCMonster* npc = (NPCMonster*)m_npc_monster_manager.GetObjByIndex(index);
	if (NULL != npc && npc->IsValid())
	{
		return npc;
	}

	return NULL;
}

NPCMonster* Scene::GetMonsterByObjID(ObjID obj_id)
{
	Obj *obj = this->GetObj(obj_id);
	if (NULL != obj && Obj::OBJ_TYPE_NPC_MONSTER == obj->GetObjType())
	{
		return (NPCMonster*)obj;
	}

	return NULL;
}

int Scene::RoleNum()
{
	return m_role_manager.Size();
}

int Scene::NPCMonsterNum()
{
	return m_npc_monster_manager.Size();
}

bool Scene::SendToRole(const char * msg, int length, RoleFilterFunc role_filter, void * func_param)
{
	static const int MAX_ROLE_NUM_IN_SCENE = 8196;
	static GSNetID send_netid[MAX_ROLE_NUM_IN_SCENE];

	int role_num = GetMin((int)m_role_manager.Size(), MAX_ROLE_NUM_IN_SCENE);

	int send_num = 0;
	if (NULL == role_filter)
	{
		for (int i = 0; i < role_num; ++i)
		{
			Role *role = this->GetRoleByIndex(i);
			if (NULL != role)
			{
				send_netid[send_num++] = role->GetNetId();
			}
		}
	}
	else
	{
		for (int i = 0; i < role_num; ++i)
		{
			Role *role = this->GetRoleByIndex(i);
			if (NULL != role)
			{
				if (role_filter(role, func_param))
				{
					send_netid[send_num++] = role->GetNetId();
				}
			}
		}
	}

	return EngineAdapter::Instance().NetMulSend(send_netid, send_num, msg, length);
}

bool Scene::SendToCurrSceneRoleByRoleId(int role_id, const void * msg, int length)
{
	Role * role = this->GetRoleByUID(role_id);
	if (role != NULL)
	{
		return EngineAdapter::Instance().NetSend(role->GetNetId(), msg, length);
	}

	return false;
}

void Scene::MailToRole(int reason, const MailContentParam & contentparam, int mail_kind, RoleFilterFunc role_filter, void * func_param)
{
	UserID send_list[MAX_GAMEWORLD_SEND_MAIL_USER];
	int send_num = 0, role_num = GetMin((int)m_role_manager.Size(), MAX_GAMEWORLD_SEND_MAIL_USER);
	if (NULL == role_filter)
	{
		for (int i = 0; i < role_num; ++i)
		{
			Role *role = this->GetRoleByIndex(i);
			if (NULL != role)
			{
				send_list[send_num++] = role->GetUserId();
			}
		}
	}
	else
	{
		for (int i = 0; i < role_num; ++i)
		{
			Role *role = this->GetRoleByIndex(i);
			if (NULL != role)
			{
				if (role_filter(role, func_param))
				{
					send_list[send_num++] = role->GetUserId();
				}
			}
		}
	}

	MailRoute::MailToMultUser(send_list, send_num, reason, contentparam, mail_kind);
}

bool Scene::RoleLeaveScene(ObjID obj_id, bool is_logout /*= false*/)
{
	Obj *obj = this->GetObj(obj_id);
	if (NULL == obj || Obj::OBJ_TYPE_ROLE != obj->GetObjType())
	{
		gamelog::g_log_scene.printf(LL_WARNING, "Scene: [%d]%s OnLogout uid:%d is not exist in scene.", m_index, m_scene_name, obj_id);
		return false;
	}

	Role *role = (Role*)obj;

	gamelog::g_log_role_unique_check.printf(LL_INFO, "Scene::RoleLeaveScene role[%d,%s] cur_scene[%d,%s] is_logout[%s] unique_id[%lld]",
		role->GetUID(), role->GetName(), m_scene_id, m_scene_name, is_logout ? "True" : "False", role->GetRoleCheckUniqueId());

	GSNetID leaverole_netid = role->GetNetId();
	UserID leaverole_userid = role->GetUserId();

	role->GetSceneStatus()->OnRoleLeaveScene();
	m_special_logic->OnRoleLeaveScene(role, is_logout);
	role->GetRoleModuleManager()->GetBuffManager()->OnRoleLeaveScene(is_logout);

	this->DeleteObj(obj_id);

	// 执行完LeaveScene之后SceneManager的m_net_to_rolelocal和m_uid_to_rolelocal的映射就被删除了。
	// 依赖这2个映射的函数如：SceneManager::GetRole就无法正确找到Role对象 
	// 所以小龙决定把UnregisterRoleLocal放到DeleteObj后面，这样才能在obj->OnLeaveScene触发的所有逻辑保证能get到role
	// UnregisterRoleLocal意味这个role彻底消失，不能再后续再添加任何逻辑
	// role不能再使用啦------------------------------------------------------------
	m_scene_manager->UnregisterRoleLocal(leaverole_netid, leaverole_userid);

	if (is_logout)
	{
		if (m_scene_manager->CheckIsDelayDeleteScene(m_scene_id))
		{
			m_is_delay_delete_scene = true; 
			m_delay_delete_scene_time = EngineAdapter::Instance().Time() + 300;		// 保留5分钟
		}
		else
		{
			//TODO 其他需要判断的场景，看是否需要保留场景, 或参考G1 为UserID缓存一个场景ID和KEY
		}
	}

	return true;
}

bool Scene::IsDelayDeleteScene(int scene_type, int scene_id)
{
	bool ret = false;
	switch (scene_type)
	{
	case SCENE_TYPE_MIYI:
	case SCENE_TYPE_CRYSTAL_FB:
	case SCENE_TYPE_GUILD_HONOR_BATTLE:
	case SCENE_TYPE_COURAGE_CHALLENGE:
	case SCENE_TYPE_GATHER_FB:
	case SCENE_TYPE_ADVANCE_FB:
	case SCENE_TYPE_MEDAL_FB:
	case SCENE_TYPE_FANTASY_FB:
	case SCENE_TYPE_DU_CHUANG_ZEI_YING:
	case SCENE_TYPE_WEN_XIANG_LOU:
	case SCENE_TYPE_DREAM_NOTES_CHALLENGE:
	case SCENE_TYPE_MI_JING_QI_WEN:
	case SCENE_TYPE_MATERIAL_FB:
	//case SCENE_TYPE_RELIC_LOOTING:
	case SCENE_TYPE_MI_JING_QI_WEN_2:
	case SCENE_TYPE_FISHING_GROUND:
	case SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM:
	case SCENE_TYPE_WORLD_TEAM_ARENA:
	case SCENE_TYPE_HONGMENG_TIANJIE:
		ret = true;
		break;
	}

	UNSTD_STATIC_CHECK(SCENE_TYPE_COUNT == 55);
	return ret;
}

bool Scene::RoleEnterSceneAsyn(bool is_micro_pc, const GSNetID &netid, IP userip, const UserID & user_id, PlatName pname, bool is_login, int plat_spid)
{
	RMIRoleInitBackObjectImpl *ribo = new RMIRoleInitBackObjectImpl();
	ribo->scene_manager = m_scene_manager;
	ribo->scene_index = m_index;
	ribo->scene_id = m_scene_id;
	ribo->netid = netid;
	ribo->userip = userip;
	ribo->user_id = user_id;
	ribo->plat_spid = plat_spid;
	ribo->is_login = is_login;
	F_STRNCPY(ribo->pname, pname, sizeof(ribo->pname));
	ribo->is_micro_pc = is_micro_pc;

	RMIRoleClient rc;
	bool ret = false;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		rc.__bind_session(InternalComm::Instance().GetDBHandler().GetRoleDB());
		ret = rc.RoleInitAsyn(UidToInt(user_id), ribo, (const char *)pname);
	}
	else
	{
		const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(user_id);
		if (NULL == cui)
		{
			delete ribo;
			return false;
		}
		UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
		const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
		if (NULL == db_handler)
		{
			delete ribo;
			return false;
		}
		rc.__bind_session(db_handler->GetRoleDB());
		ret = rc.RoleInitAsyn(UidToInt(cui->original_user_id), ribo, (const char *)pname);
	}
	
	if (ret)
	{
		ribo->inc_key = AsynReqRegister::Instance().OnAsynReq(netid);
	}

	return ret;
}

void Scene::RoleEnterScene(int plat_spid, PlatName pname, const UserID & uid, const GSNetID &netid,
	RoleModuleManager* role_moodule_manager,
	const RoleInitParam &param, 
	const RoleOtherInitParam &other_p, 
	const ItemListParam& item_list_param,
	const SkillListParam& skill_list_param,
	const MailListParam& maillist_param,
	const RoleFirstKillParamList& first_kill_list,
	const RandActivityRoleParamList & rand_activity_role_data_list,
	const RoleCommonSaveDataParam & role_common_save_list,
	const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
	const GodPrintItemListParam & god_print_item_list,
	const NewCourageChallengeLevelParamList & new_courage_challenge_list,
	bool is_login /*= false*/, 
	Role **p_role /*= 0*/)
{
	GameName role_name;
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		F_STRNCPY(role_name, param.role_name, sizeof(GameName));
	}
	else
	{
		if (NULL != role_moodule_manager)
		{
			RoleCross* rc = role_moodule_manager->GetRoleCross();
			SNPRINTF(role_name, sizeof(GameName), "%s_s%d", rc->GetOriginRoleName(), rc->GetOriginServerId());
		}
		else
		{
			SNPRINTF(role_name, sizeof(GameName), "%s_s%d", other_p.rolecross_param.origin_role_name, other_p.rolecross_param.origin_server_id);
		}
	}

	Role *role = new Role(netid, role_name);

	role->Init(plat_spid, pname, uid, role_moodule_manager, param, other_p, item_list_param, skill_list_param,maillist_param, first_kill_list, 
		rand_activity_role_data_list, role_common_save_list, role_courage_challenge_list, god_print_item_list, new_courage_challenge_list);

	this->AddObj(role);

	role->GetRoleModuleManager()->GetBuffManager()->OnRoleEnterScene(this->GetSceneType());
	role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_INVALID, __FUNCTION__, true);
	role->GetRoleModuleManager()->GetPet()->InitEnd();
	role->GetRoleModuleManager()->GetPartner()->InitEnd();
	role->GetRoleModuleManager()->GetHuaShenManager()->InitEnd();
	role->GetRoleModuleManager()->ReCalcSpeed(RoleModuleManager::SPEED_RECALC_REASON_INIT, __FUNCTION__, is_login);

	m_special_logic->OnRoleEnterScene(role);
	role->GetSceneStatus()->OnRoleEnterScene();

	if (NULL != p_role) *p_role = role;

	if (this->IsSyncGather())
	{
		this->SyncAllObjOfType(role, Obj::OBJ_TYPE_NPC_GATHERS);
	}

	if (is_login)
	{
		if (m_scene_manager->CheckIsDelayDeleteScene(m_scene_id))
		{
			m_is_delay_delete_scene = false; m_delay_delete_scene_time = 0;
		}
	}

	gamelog::g_log_role_unique_check.printf(LL_INFO, "Scene::RoleEnterScene role[%d,%s] cur_scene[%d,%s] is_login[%s] unique_id[%lld]",
		role->GetUID(), role->GetName(), m_scene_id, m_scene_name, is_login ? "True" : "False", role->GetRoleCheckUniqueId());
}

void RMIRoleInitBackObjectImpl::RoleInitRet(int ret, const RoleInitParam &p,
											const RoleOtherInitParam &other_p,
											const ItemListParam& item_list_param,
											const SkillListParam& skill_list_param,
											const MailListParam& maillist_param,
											const RoleFirstKillParamList& first_kill_list,
											const RandActivityRoleParamList & rand_activity_role_list,
											const RoleCommonSaveDataParam & role_common_save_list,
											const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
											const GodPrintItemListParam & god_print_item_list,
											const NewCourageChallengeLevelParamList & new_courage_challenge_list,
											long long last_save_time)
{
	if (0 != ret)
	{
		gamelog::g_log_login.printf(LL_ERROR, "[ERROR RMIRoleInitBackObjectImpl error ret:%d role_id:%d", ret, UidToInt(user_id));
		return;
	}

	if (!AsynReqRegister::Instance().OnAsynRspCheckNetidExist(netid, inc_key, UidToInt(user_id)))
	{
		return;
	}

	// 当数据库回来之后，如果已经有一个号存在了，那么不能让他进来，不然出现影子，会被刷元宝道具之类的
	if (NULL != scene_manager->GetRole(user_id))
	{
		gamelog::g_log_login.printf(LL_ERROR, "RMIRoleInitBackObjectImpl::RoleInitRet role reapet !!! [error] %d \n", UidToInt(user_id));
		return;
	}

	Scene *scene = scene_manager->GetSceneByIndex(scene_index);
	if (NULL == scene)
	{
		gamelog::g_log_login.printf(LL_ERROR, "[ERROR RMIRoleInitBackObjectImpl error scene_index:%d role_id:%d", scene_index, UidToInt(user_id));
		return;
	}

	RoleInitParam role_param = p;
	if (scene->GetSceneID() != p.scene_id)
	{
		role_param.scene_x = p.last_scene_x;
		role_param.scene_y = p.last_scene_y;
	}

	Role *role = 0;
	scene->RoleEnterScene(plat_spid, pname, user_id, netid, NULL, role_param,
						  other_p,
						  item_list_param,
						  skill_list_param,
						  maillist_param,
						  first_kill_list,
						  rand_activity_role_list,
						  role_common_save_list,
						  role_courage_challenge_list,
						  god_print_item_list,
						  new_courage_challenge_list,
						  is_login, &role);

	if (NULL != role)
	{
		gamelog::g_log_login.printf(LL_INFO, "RMIRoleInitBackObjectImpl::RoleInitRet login SUCC, user[%d,%s]", role->GetUID(), role->GetName());
	}

	if (is_login && NULL != role) 
	{
		role->SendAllInfo();
		role->OnLogin(last_save_time, userip, plat_spid);		
		World::GetInstWorld()->OnUserLogin(user_id);
		UserID original_user_id = user_id;

		RMIRoleSaveStatusBackObjectImpl *rssbo = new RMIRoleSaveStatusBackObjectImpl();
		rssbo->log_user_id = user_id;

		bool is_cross = false;
		RMIRoleClient rc;
		const rmi::Session *session = &InternalComm::Instance().GetDBHandler().GetRoleDB();
		if (CrossConfig::Instance().IsHiddenServer())
		{
			const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(user_id);
			if (NULL == cui)
			{
				delete rssbo;
				return;
			}

			UniqueServerID usid(cui->original_plat_type, cui->original_server_id);
			const DBHandler* db_handler = InternalComm::Instance().GetHiddenDBHandler(usid);
			if (NULL == db_handler)
			{
				delete rssbo;
				return;
			}

			session = &db_handler->GetRoleDB();
			original_user_id = cui->original_user_id;
			is_cross = true;
		}
		rc.__bind_session(*session);
		rc.RoleSaveStatusAsyn(UidToInt(original_user_id), false, is_cross, is_micro_pc, pname, rssbo);
	}
}

void RMIRoleInitBackObjectImpl::__timeout()
{
	gamelog::g_log_login.printf(LL_INFO, "RMIRoleInitBackObjectImpl::__timeout role_id[%d] pname[%s] scene[%d,%d]", UidToInt(user_id), pname, scene_id, scene_index);
}

void RMIRoleInitBackObjectImpl::__exception(int error)
{
	printf("RMIRoleInitBackObjectImpl user[%s] error[%d]\n", pname, error);
}

void RMIRoleSaveStatusBackObjectImpl::RoleSaveStatusRet(int ret)
{
	if (0 != ret)
	{
		Error(log_user_id, GAME_ERROR_EXECUTE, ret);
	}
}

bool Scene::IsStaticScene(int scene_type)
{
	bool is_static_scene = false;
	switch (scene_type)
	{
	case SCENE_TYPE_COMMON_SCENE:
	case SCENE_TYPE_GUAJI:
	case SCENE_TYPE_HUASHENG_MIJING_WAIT:
		is_static_scene = true;
		break;
	}

	return is_static_scene;
}

bool Scene::IsInStaticScene()
{
	return IsStaticScene(m_scene_type);
}

void Scene::CheckRoleTimeOut(unsigned long now)
{
	int role_num = (int)m_role_manager.Size();

	for (int i = 0; i < role_num; ++i)
	{
		static const unsigned int hearbeat_dead_time = ServerConfigPool::Instance().common_config.timeout.gameworld_heartbeat_dead_time_ms;

		Role *role = this->GetRoleByIndex(i);
		if (NULL == role) continue;

		unsigned long last_client_msg_time = role->GetLastMsgFromClientTime();

		// 这里之前有严重的bug game_time有可能小于last_active_time 因为对时导致game_time变小 一减就变成负的 unsigned 就跪了
		if ((now > last_client_msg_time) && (now - last_client_msg_time > (unsigned long)(hearbeat_dead_time * 1.5f)))	// 暂时先这样，等确定了问题后再改
		{
			m_scene_manager->RoleTimeOut(this, role, "CheckRoleTimeout");
		}
	}
}

int Scene::GetTownPoint(Posi *pos /*= 0*/)
{
	if (NULL != pos) *pos = m_town_point.pos;

	return m_town_point.scene_id;
}

long long Scene::RandMineMonsterEncounterTimeMS()
{
	return RandomNum(m_mine_monster.mine_interval_min_s, m_mine_monster.mine_interval_max_s) * 1000;
}

int Scene::RandMineMonster(Role* role, int max_list_size, int* out_monster_id_list, short* out_monster_level_list, int header_monster_id /* = 0 */)
{
	if (m_mine_monster.mine_monster_num <= 0) return 0;

	static int idx_helper[MINE_MONSTERS_NUM]; // 标记怪物在out_list中的哪个位置
	memset(idx_helper, -1, sizeof(idx_helper));

	int monster_num = RandomNum(m_mine_monster.mine_monster_count_min, m_mine_monster.mine_monster_count_max);
	monster_num = GetMin(monster_num, max_list_size);

	std::vector<int> monster_kind_vec;
	for (int i = 0; i < m_mine_monster.mine_monster_num; ++i)
	{
		int monster_id = m_mine_monster.mine_monsters[i];
		monster_kind_vec.push_back(monster_id);
		if (role->GetRoleModuleManager()->IsNeedMonster(monster_id)) // 如果需要这只怪物，就让这只怪物的出现概率提高
		{
			// 加两只，几率要更大
			monster_kind_vec.push_back(monster_id);
			monster_kind_vec.push_back(monster_id);
			monster_kind_vec.push_back(monster_id);
		}
	}
	std::random_shuffle(monster_kind_vec.begin(), monster_kind_vec.end());

	static const int DIFFERENT_MONSTER_NUM = 2;
	int rand_monster_id_list[DIFFERENT_MONSTER_NUM] = { 0 };
	for (int i = 0; i < DIFFERENT_MONSTER_NUM; ++i)
	{
		int size = monster_kind_vec.size();
		rand_monster_id_list[i] = monster_kind_vec[rand() % size];
	}

	bool has_lv1_monster = false;
	int lv1_monster_id = 0;
	for (int i = 0; i < monster_num; ++i)
	{
		int monster_id = rand_monster_id_list[RandomNum(DIFFERENT_MONSTER_NUM)];

		out_monster_id_list[i] = monster_id;
		out_monster_level_list[i] = RandomNum(m_mine_monster.mine_monster_level_min, m_mine_monster.mine_monster_level_max);

		if (!has_lv1_monster)
		{
			const MonsterCfg* monster_cfg = MonsterPool::GetInstance().GetMonsterCfg(monster_id);
			if (NULL != monster_cfg && monster_cfg->IsRefreshLevelOneMonster())
			{
				out_monster_level_list[i] = 1;
				
				has_lv1_monster = true;
				lv1_monster_id = out_monster_id_list[i];
			}
		}
	}

	if (header_monster_id > 0 && monster_num > 0)
	{
		out_monster_id_list[0] = header_monster_id;
	}

	if (has_lv1_monster)
	{
		ROLE_LOG_QUICK6(LOG_TYPE_ROLE_LV1_MINE_MONSTER, role, 1, lv1_monster_id, NULL, NULL);
	}

	return monster_num;
}

int Scene::RandHeadMineMonster()
{
	if (!this->HasMineMonster()) return 0;

	int monster_idx = RandomNum(m_mine_monster.mine_monster_num);
	return m_mine_monster.mine_monsters[monster_idx];
}

void Scene::SyncObjInfo(Obj* obj, short notify_reason /* = 0 */)
{
	if (NULL == obj) return;

	static Protocol::SCSceneObjList msg;
	msg.notify_reason = notify_reason;
	msg.obj_num = 1;
	msg.obj_list[0].pos_x = obj->GetPos().x;
	msg.obj_list[0].pos_y = obj->GetPos().y;
	msg.obj_list[0].obj_id = obj->GetId();
	msg.obj_list[0].obj_type = obj->GetObjType();

	int send_len = sizeof(msg) - (Protocol::SCSceneObjList::MAX_OBJ_NUM - msg.obj_num) * sizeof(msg.obj_list[0]);
	this->SendToRole((const char*)&msg, send_len);
}

void Scene::SyncAllObjOfType(Role* role, short obj_type)
{
	if (NULL == role) return;

	// 同步客户端场景中某一类型物体的的坐标等（暂时只支持采集物与怪物）

	static Protocol::SCSceneObjList msg;
	msg.notify_reason = Protocol::SCSceneObjList::NOTIFY_REASON_NORMAL;
	msg.obj_num = 0;

	if (Obj::OBJ_TYPE_NPC_GATHERS == obj_type)
	{
		int gather_num = m_npc_gathers_manager.Size();
		for (int i = 0; i < gather_num && msg.obj_num < Protocol::SCSceneObjList::MAX_OBJ_NUM; ++i)
		{
			Obj* obj = m_npc_gathers_manager.GetObjByIndex(i);
			if (NULL != obj && obj->IsValid())
			{
				msg.obj_list[msg.obj_num].pos_x = obj->GetPos().x;
				msg.obj_list[msg.obj_num].pos_y = obj->GetPos().y;
				msg.obj_list[msg.obj_num].obj_id = obj->GetId();
				msg.obj_list[msg.obj_num].obj_type = obj->GetObjType();
				++msg.obj_num;
			}
		}
	}
	else if (Obj::OBJ_TYPE_NPC_MONSTER == obj_type)
	{
		int monster_num = m_npc_monster_manager.Size();
		for (int i = 0; i < monster_num && msg.obj_num < Protocol::SCSceneObjList::MAX_OBJ_NUM; ++i)
		{
			Obj* obj = m_npc_monster_manager.GetObjByIndex(i);
			if (NULL != obj && obj->IsValid())
			{
				msg.obj_list[msg.obj_num].pos_x = obj->GetPos().x;
				msg.obj_list[msg.obj_num].pos_y = obj->GetPos().y;
				msg.obj_list[msg.obj_num].obj_id = obj->GetId();
				msg.obj_list[msg.obj_num].obj_type = obj->GetObjType();
				++msg.obj_num;
			}
		}
	}
	else if (Obj::OBJ_TYPE_NPC_CHOSED == obj_type)
	{
		int monster_num = m_npc_chosed_manager.Size();
		for (int i = 0; i < monster_num && msg.obj_num < Protocol::SCSceneObjList::MAX_OBJ_NUM; ++i)
		{
			Obj* obj = m_npc_chosed_manager.GetObjByIndex(i);
			if (NULL != obj && obj->IsValid())
			{
				msg.obj_list[msg.obj_num].pos_x = obj->GetPos().x;
				msg.obj_list[msg.obj_num].pos_y = obj->GetPos().y;
				msg.obj_list[msg.obj_num].obj_id = obj->GetId();
				msg.obj_list[msg.obj_num].obj_type = obj->GetObjType();
				++msg.obj_num;
			}
		}
	}
	else
	{
		return; 
	}

	int send_len = sizeof(msg) - (Protocol::SCSceneObjList::MAX_OBJ_NUM - msg.obj_num) * sizeof(msg.obj_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&msg, send_len);
}

Posi Scene::GetARandPosi()
{
	static const int LOOP_TIMES = 300;
	static const int AVOID_DISTANCE = 20;
	Posi pos(0, 0);
	bool is_too_close = false;
	Posi too_close_posi(0, 0);
	for (int i = 0; i < LOOP_TIMES; ++i)
	{
		pos = m_map->GetARandWalkablePoint();
		is_too_close = false;
		for (AvoidPointMap::iterator it = m_rand_posi_avoid_list.begin(); it != m_rand_posi_avoid_list.end(); ++it)
		{
			Posi avoid_pos = GetPosFromInt(it->first);
			if (!(avoid_pos - pos).IsLongerThan(AVOID_DISTANCE))
			{
				is_too_close = true;
				if (i == LOOP_TIMES - 1)
				{
					too_close_posi = avoid_pos;
				}
				break;
			}
		}

		if (!is_too_close)
		{
			break;
		}

		if (i > 0 && 0 == i % 50)
		{
			// 尝试了50次，还没有找到可用的点，则尝试重新随机整个地图的点位
			m_map->FiltrateAndSaveWalkablePointSet();
		}
	}

	if (is_too_close)
	{
		//Posi temp_pos = gamecommon::GetDisRandPos(too_close_posi, AVOID_DISTANCE);

		printf("rand pos still too close! scene[%d,%s] pos[%d,%d] too_close_pos[%d,%d]\n", 
			m_scene_id, m_scene_name, pos.x, pos.y, too_close_posi.x, too_close_posi.y);

		//pos = temp_pos;
	}
	
	return pos;
}

int GetPosInt(const Posi& pos)
{
	return pos.x + pos.y * 10000;
}

void Scene::AddAvoidPos(const Posi& pos)
{
	int pos_int = GetPosInt(pos);
	if (m_rand_posi_avoid_list.find(pos_int) == m_rand_posi_avoid_list.end())
	{
		m_rand_posi_avoid_list.insert(AvoidPointMap::value_type(pos_int, 0));
	}
}

void Scene::RemoveAvoidPos(const Posi& pos)
{
	int pos_int = GetPosInt(pos);
	AvoidPointMap::iterator it = m_rand_posi_avoid_list.find(pos_int);
	if (m_rand_posi_avoid_list.end() != it)
	{
		m_rand_posi_avoid_list.erase(it);
	}
}

void Scene::GmPrintfWalkableList()
{
	printf("\nOnPrintfWalkableList -> SceneID: %d", this->GetSceneID());
	m_map->GmPrintfMapWalkableList();
}

bool Scene::CanEnter(Role* role, bool is_notice /* = true */)
{
	bool can_enter = role->GetRoleModuleManager()->CanEnterScene(m_scene_id, is_notice);
	if (!can_enter)
	{
		return false;
	}
	if (m_scene_level_min > role->GetLevel())
	{
		if (is_notice)
		{
			role->NoticeNum(errornum::EN_SCENE_UNLOCK_YET);
		}
		return false;
	}
	if (0 != m_scene_level_max && m_scene_level_max < role->GetLevel())
	{
		if (is_notice)
		{
			role->NoticeNum(errornum::EN_SCENE_EXCEED_MAX_LEVEL_LIMIT);
		}
		return false;
	}
	return true;
}

void Scene::MineMonster::Init(const ConfigScene& cfg)
{
	const SceneMineMonsterCfg *  scene_mine_monster_cfg = SceneMineMonsterConfig::Instance().GetSceneMineMonsterCfg(cfg.id);
	if (scene_mine_monster_cfg != NULL)
	{
		mine_interval_max_s = scene_mine_monster_cfg->mine_interval_max_s;
		mine_interval_min_s = scene_mine_monster_cfg->mine_interval_min_s;
		mine_monster_count_max = scene_mine_monster_cfg->mine_monster_count_max;
		mine_monster_count_min = scene_mine_monster_cfg->mine_monster_count_min;
		mine_monster_level_min = scene_mine_monster_cfg->mine_monster_level_min;
		mine_monster_level_max = scene_mine_monster_cfg->mine_monster_level_max;

		int monster_size = (int)scene_mine_monster_cfg->mine_monsters.size();
		mine_monster_num = monster_size < ARRAY_LENGTH(mine_monsters) ? monster_size : ARRAY_LENGTH(mine_monsters);
		for (int i = 0; i < (int)scene_mine_monster_cfg->mine_monsters.size() && i < ARRAY_LENGTH(mine_monsters); ++i)
		{
			mine_monsters[i] = scene_mine_monster_cfg->mine_monsters[i];
		}

		mine_monster_drop_num = 0;
		for (size_t i = 0; i < scene_mine_monster_cfg->drop_list.size() && i < MINE_MONSTERS_DROP_NUM; ++i)
		{
			mine_monster_drop_list[i] = scene_mine_monster_cfg->drop_list[i];
			mine_monster_drop_num += 1;
		}

		prestige_id = scene_mine_monster_cfg->prestige_id;
	}
}
