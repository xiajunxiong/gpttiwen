#include "scenemanager.hpp"
#include "config/scenemanagerconfig.h"
#include "gamelog.h"
#include "map.h"
#include "servercommon/userprotocal/msgheader.h"
#include "engineadapter.h"
#include "obj/character/role.h"
#include "scene/scene.h"
#include "servercommon/serverconfig/serverconfigpool.h"
#include "rmibackobjdef.h"
#include "other/rolemodulemanager.hpp"
#include "internalcomm.h"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "other/pet/pet.hpp"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "battle/battle_manager_local.hpp"
#include "servercommon/struct/skilllistparam.hpp"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/roleotherinitparam.hpp"
#include "servercommon/struct/friendparam.h"
#include "config/reloadconfigmanager.hpp"
#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activityimpl/activityworldboss2.hpp"
#include "global/activity/activityimpl/activityworldboss3.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/courseofgrowthmanager/courseofgrowthmanager.hpp"
#include "global/team/team.hpp"
#include "servercommon/errornum.h"
#include "scene/speciallogicimpl/speciallogicbraveground.hpp"
#include "scene/speciallogicimpl/speciallogicguildhonorbattle.hpp"
#include "world.h"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "global/cross/crossmanager.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/cross/crossuserregister.hpp"
#include "global/cross/crossusershadow.hpp"
#include "obj/character/robot.h"
#include "other/robot/robotmanager.hpp"
#include "scene/delayloginmanager.hpp"
#include "other/rolemail/rolemail.hpp"
#include "battle/battle_video_config.hpp"
#include "config/logicconfigmanager.hpp"
#include "battle/battle_video_manager.hpp"
#include "servercommon/userprotocal/msgbattle.h"
#include "global/team/teammanager.hpp"
#include "global/guild/guild.hpp"
#include "global/guild/guildmanager.hpp"
#include "chat/chatmanager.h"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/platformbattle/platformbattle.h"
#include "global/couragechallengerank/couragechallengerank.hpp"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "servercommon/struct/rolefirstkillparam.hpp"
#include "global/system/system.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/gatherdreammanager/gatherdreamrecordmanager.hpp"
#include "other/monitor/systemmonitor.hpp"
#include "global/mail/usermail.hpp"
#include "global/mail/mailmanager.hpp"
#include "other/huashen/huashenmanager.hpp"
#include "chat/crosschatgroupmanager.h"
#include "other/huanjie/rolehuanjie.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"
#include "servercommon/string/gameworldstr.h"
#include "config/activityconfig/chiefelection/activitychiefelectionconfig.hpp"
#include "global/activity/activityimpl/activitychiefelection.hpp"
#include "global/usercache/usercache.hpp"
#include "other/rolecross/rolecross.hpp"
#include "protocol/msgother.h"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityconsumecarousel.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/wildbossmanager/wildbossmanager.h"
#include "global/activity/activityimpl/activityworldboss.hpp"
#include "global/activity/activityimpl/activityworldboss2.hpp"
#include "protocol/msgplatformbattle.h"
#include "protocol/msgworldboss.h"
#include "protocol/msgworldboss2.h"
#include "protocol/msgworldboss3.hpp"
#include "config/randactivityconfig/impl/randactivityquanmindiscountconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivityimpl/raquanmindiscountmanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityquanmindiscount.hpp"
#include "global/activity/activityimpl/activitybraveground.hpp"
#include "global/server_first_kill/serverfirstkill.hpp"
#include "global/cross/originbusinessregister.hpp"
#include "other/route/mailroute.hpp"
#include "global/randactivity/randactivityimpl/randactivityshenshoujianglin.hpp"
#include "protocol/randactivity/msgrashenshoujianglin.hpp"
#include "servercommon/rolecommonsavedef.h"
#include "protocol/msgcolosseum.h"
#include "global/colosseum/colosseummanager.hpp"
#include "global/shanhaibossmanager/shanhaibossmanager.hpp"
#include "protocol/msgrole.h"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "global/cloudarena/cloudarena.hpp"
#include "global/cloudarena/cloudarenauser.hpp"
#include "global/marketmanager/marketmanager.hpp"
#include "protocol/msgcloudarena.h"
#include "other/salary/salary.hpp"
#include "other/hongbao/hongbaomanager.hpp"
#include "other/bigdipper/bigdippermanager.hpp"
#include "other/formation/formation.hpp"
#include "global/givegift/givegift.h"
#include "global/shituseeking/shituseekingmanager.h"
#include "global/activity/activityimpl/activityhundredghost.hpp"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "global/tiandidaohen/tiandidaohen.h"
#include "friend/friendintimacy.hpp"
#include "other/event/eventhandler.hpp"
#include "global/jieyi/jieyizumanager.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "other/endlesstower/endlesstowerconfig.hpp"
#include "global/colosseum/gladiator.hpp"
#include "global/cowreportgoodnews/cowreportgoodnews.hpp"
#include "global/randactivity/randactivityimpl/randactivityhorcruxtiancheng.hpp"
#include "global/trademarket/trademarketmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityjixinggaozhao.hpp"
#include "global/tigerbringsblessing/tigerbringsblessing.hpp"
#include "global/datasynccheck/datasynccheck.hpp"
#include "other/fallengod/fallengodconfig.hpp"
#include "other/rolelifeskill/rolenewlifeskill.hpp"
#include "qingyuan/weddingmanger.hpp"
#include "global/worldarena/worldarena.hpp"
#include "global/worldteamarenamanager/worldteamarenarankmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "other/cycleactivity/qingyuanduiduipeng/qingyuanduiduipengmanager.hpp"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "other/cycleactivity/lanternriddie/lanternriddie.hpp"
#include "global/guild/guildmember.hpp"
#include "other/formation/pethelperformation.hpp"
#include "global/bigcross/utachampionrecordmanager.hpp"
#include "global/bigcross/utaguessshadow.hpp"
#include "global/universe_team_arena/UTAShadow.hpp"
#include "global/bigcross/bigcrossmanager.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"
#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "other/hongmengtianjie/hongmengtianjiemanager.hpp"
#include "timer/delayreloadconfigtimer.hpp"
#include "global/kuafuyingxionglumanager/kuafuyingxionglumanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityservercompetition.hpp"
#include "global/randactivity/randactivityimpl/randactivitysaltysweetbattle.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "global/team/teamrecruit.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"
#include "config/randactivityconfig/impl/randactivityshenshoujianglinconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycharismatictanabataconfig.hpp"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"
#include "other/queryhandler/queryhandler.hpp"
#include "global/offlinebattledata/offlinebattledata.hpp"
#include "global/team/teammatch.hpp"

bool SceneManager::Init(const SceneManagerConfig& scm)
{
	m_message_handler.Init(this);

	for (ConfigMapList::const_iterator iter = scm.configmanager.maps.begin(); iter != scm.configmanager.maps.end(); ++iter)
	{
		if (m_map_list.find(iter->map.id) != m_map_list.end())
		{
			gamelog::g_log_scenemanager.printf(LL_WARNING, "SceneManager On INIT, create map error, map id(%d) is exist!", iter->map.id);
			return false;
		}

		Map *map = new Map(iter->map);
		m_map_list[iter->map.id] = map;

		gamelog::g_log_scenemanager.printf(LL_INFO, "Init map[%d].", iter->map.id);
	}

	for (ConfigSceneList::const_iterator iter = scm.configmanager.scenes.begin(); iter != scm.configmanager.scenes.end(); ++iter)
	{
		m_scene_id_to_type_map[iter->scene.id] = iter->scene_type;

		if (Scene::IsStaticScene(iter->scene_type))
		{
			Scene *scene = 0;
			bool ret = this->CreateScene(iter->scene, COMMON_SCENE_KEY, &scene, iter->scene_type, 0, false);
			if (!ret)
			{
				gamelog::g_log_scenemanager.printf(LL_WARNING, "Create scene[%d, %s]  fail.", iter->scene.id, iter->scene.name.c_str());
				return false;
			}
			gamelog::g_log_scenemanager.printf(LL_INFO, "Init scene[%d, %s].", iter->scene.id, iter->scene.name.c_str());
		}
		else
		{
			FBConfig fbconfig;
			fbconfig.config = iter->scene;
			fbconfig.scene_type = iter->scene_type;
			m_fb_config_list[iter->scene.id] = fbconfig;
		}
	}

	m_check_timeout = ServerConfigPool::Instance().common_config.timeout.is_check_timeout;
	m_check_timeout_interval = ServerConfigPool::Instance().common_config.timeout.check_timeout_interval_ms;
	m_heartbeat_dead_time = ServerConfigPool::Instance().common_config.timeout.gameworld_heartbeat_dead_time_ms;

	gamelog::g_log_scenemanager.printf(LL_INFO, "SceneManager::Init check_timeout[%s], check_interval_ms[%lu], heartbeat_dead_ms[%lu]",
		m_check_timeout ? "true" : "false", m_check_timeout_interval, m_heartbeat_dead_time);

	//TODO test
	//m_scene_match_manager.Init(5, 0, new NormalMatchLogicImpl(&m_scene_match_manager));

	return true;
}

void SceneManager::OnRecv(const GSNetID &netid, IP userip, const char *data, int length)
{
	if (length < (int)sizeof(Protocol::MessageHeader))
	{
		EngineAdapter::Instance().NetDisconnect(netid, INVALID_USER_ID, "null", -1, "MsgHeaderSizeError");
		return;
	}

	NetSceneMap::iterator i = m_net_to_rolelocal.find(netid);
	if (i != m_net_to_rolelocal.end())
	{
		i->second.last_active_time = m_game_time;

		SceneList::Iterator iter = m_scene_list.Find(i->second.scene_index);
		if (iter != m_scene_list.End())
		{
			Scene *scene = *iter;
			Role *role = GetRoleHelper(scene, i->second.obj_id, "SceneManager::OnRecv");
			if (NULL == role)
			{
				gamelog::g_log_scenemanager.printf(LL_ERROR, "[SceneManager::OnRecv Error][scene_index:%d obj_id:%d] [gateway:%d netid:%d]", (int)i->second.scene_index, (int)i->second.obj_id,
					(int)netid.gateway_netid, (int)netid.netid);

				m_net_to_rolelocal.erase(netid);    // netid列表有，但是role不存在了，导致这个netid一直挂住，玩家进不去游戏 modify by xiaolong

				return;
			}

			role->OnRecvFromClient(m_game_time, userip);

			m_message_handler.HandleMessageFromClient(netid, scene, role, i->second.obj_id, data, length);
		}
		else
		{
			gamelog::g_log_scenemanager.printf(LL_DEBUG, "SceneId:%d NOT found.", i->second.scene_index);
		}
	}
	else
	{
		m_message_handler.HandleMessageFromClientNoLogin(netid, userip, data, length);
	}
}

bool SceneManager::OnDisconnect(const GSNetID &netid, SceneIndex *p_scene_index /*= NULL*/, ObjID *p_obj_id /*= NULL*/)
{
	bool ret = false;

//#ifdef _DEBUG
//	printf("SceneManager::OnDisconnect netid[%d,%d]\n", netid.gateway_netid, netid.netid);
//#endif

	NetSceneMap::iterator i = m_net_to_rolelocal.find(netid);
	if (i != m_net_to_rolelocal.end())
	{
		if (i->second.scene_index != SceneIndex(-1))
		{
			SceneList::Iterator iter = m_scene_list.Find(i->second.scene_index);
			if (iter != m_scene_list.End())
			{
				if (NULL != p_scene_index) *p_scene_index = i->second.scene_index;
				if (NULL != p_obj_id) *p_obj_id = i->second.obj_id;

				this->Logout(*iter, i->second.obj_id);

				ret = true;
			}
		}
		else
		{
			gamelog::g_log_scenemanager.printf(LL_ERROR, "[SceneManager::OnDisconnect Error][scene_index:%d obj_id:%d]", (int)i->second.scene_index, (int)i->second.obj_id);
		}

		m_net_to_rolelocal.erase(netid);   // 无论怎样都要干掉netid 之前是在else分支里面 by xiaolong
	}

	return ret;
}

void SceneManager::OnGateWayDisconnect(NetID gateway_netid)
{
	gamelog::g_log_scenemanager.printf(LL_DEBUG, "SceneManager::OnGateWayDisconnect netid[%d]", gateway_netid);

	this->LogoutAllRole(gateway_netid);
}

int SceneManager::GetSceneList(int *scene_list, int max_num)
{
	int count = 0;

	for (SceneIDToIndexMap::iterator i = m_sceneidmap.begin(); count < max_num && i != m_sceneidmap.end(); ++i)
	{
		scene_list[count++] = i->first.scene_id;
	}

	for (FBConfigList::iterator i = m_fb_config_list.begin(); count < max_num && i != m_fb_config_list.end(); ++i)
	{
		scene_list[count++] = i->first; // 将副本也注册到loginserver
	}

	return count;
}

Scene * SceneManager::GetSceneByIndex(SceneIndex scene_index)
{
	if (m_scene_list.Exist(scene_index))
	{
		return m_scene_list[scene_index];
	}

	return NULL;
}

Scene * SceneManager::GetSceneById(int id, int key)
{
	SceneIDToIndexMap::iterator it = m_sceneidmap.find(SceneIDKey(id, key));
	if (it != m_sceneidmap.end())
	{
		return this->GetSceneByIndex(it->second);
	}

	return NULL;
}

Role * SceneManager::GetRoleHelper(Scene *scene, ObjID obj_id, const char *funstr)
{
	if (NULL == scene || NULL == funstr)
	{
		return NULL;
	}

	Obj *role_obj = scene->GetObj(obj_id);
	if (NULL == role_obj)
	{
		gamelog::g_log_scenemanager.printf(LL_ERROR, "Scene: [%d]%s %s obj_id:%d is not exist in scene.", scene->GetSceneID(), scene->GetSceneName(), funstr, obj_id);
		return NULL;
	}

	if (role_obj->GetObjType() != Obj::OBJ_TYPE_ROLE)
	{
		gamelog::g_log_scenemanager.printf(LL_ERROR, "Scene: [%d]%s %s obj_id:%d is not a role.", scene->GetSceneID(), scene->GetSceneName(), funstr, obj_id);
		return NULL;
	}
	Role* role = (Role*)role_obj;
	gamelog::g_log_scenemanager.printf(LL_DEBUG, "Scene: [%d]%s %s found, obj_id[%d] role[%d, %s] .", scene->GetSceneID(), scene->GetSceneName(), funstr, obj_id, role->GetUID(), role->GetName());
	return role;
}

int SceneManager::GetOnlineRoleNum()
{
	return static_cast<int>(m_net_to_rolelocal.size());
}

int SceneManager::GetMaxRoleNum()
{
	return GLOBALCONFIG->GetServerMaxRoleNum();
}

Role * SceneManager::GetRole(const UserID &user_id)
{
	if (INVALID_USER_ID == user_id) return NULL;

	UIDSceneMap::iterator iter = m_uid_to_rolelocal.find(user_id);
	if (iter != m_uid_to_rolelocal.end())
	{
		SceneList::Iterator scene_iter = m_scene_list.Find(iter->second.scene_index);
		if (scene_iter != m_scene_list.End())
		{
			Role *role = GetRoleHelper((*scene_iter), iter->second.obj_id, "SceneManager::GetRole");
			if (NULL != role && role->GetUserId() == user_id)
			{
				if (NULL == role->GetRoleModuleManager())
				{
#ifdef _DEBUG
					assert(0); // 如果role是合法的但rolemodulemanager不合法，则视为role不合法
#endif
					return NULL;
				}
				return role;
			}
		}
	}

	return NULL;
}

Role * SceneManager::GetRole(SceneIndex scene_index, ObjID obj_id)
{
	SceneList::Iterator iter = m_scene_list.Find(scene_index);
	if (iter != m_scene_list.End())
	{
		Obj *obj = (*iter)->GetObj(obj_id);
		if (NULL != obj && Obj::OBJ_TYPE_ROLE == obj->GetObjType())
		{
			return (Role*)obj;
		}
	}

	return NULL;
}

void SceneManager::RegisterRoleLocal(const GSNetID &netid, SceneIndex sceneid, ObjID obj_id)
{
	Scene *scene = GetSceneByIndex(sceneid);
	if (NULL == scene) return;

	Obj *role_obj = scene->GetObj(obj_id);
	if (NULL == role_obj) return;

	if (role_obj->GetObjType() != Obj::OBJ_TYPE_ROLE) return;

	Role *role = (Role*)role_obj;

	UserID user_id = role->GetUserId();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 此处是为了debug加的日志，如果出现该日志，则表明又有Role影子的情况
	UIDSceneMap::iterator uid_role_item;
	if ((uid_role_item = m_uid_to_rolelocal.find(user_id)) != m_uid_to_rolelocal.end())
	{
		// 难道又出现Role影子的情况？！！
		gamelog::g_log_serious_error.printf(LL_ERROR, "[SceneManager::EnterScene Error][UserID(%d, %d) is already exist in SceneIndex:%d ObjID:%d]",
			user_id.db_index, user_id.role_id, (int)uid_role_item->second.scene_index, (int)uid_role_item->second.obj_id);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	RoleLocal rl(sceneid, obj_id, m_game_time);
	m_uid_to_rolelocal[user_id] = rl;
	m_net_to_rolelocal[netid] = rl;

	gamelog::g_log_debug.printf(LL_DEBUG, "SceneManager::RegisterRoleLocal role[%d,%s] netid[%d,%d] scene_id[%u] obj_id[%u] scene_index[%u]",
		role->GetUID(), role->GetName(), netid.gateway_netid, netid.netid, sceneid, obj_id, rl.scene_index);

	// 	gglobalprotocal::GGlobalRoleEnterScene res;  // 同步到globalserver
	// 	res.db_index = user_id.db_index;
	// 	res.role_id = user_id.role_id;
	// 	res.scene_id = scene->GetSceneID();
	// 	res.scene_key = scene->GetSceneKey();
	// 	res.obj_id = role->GetId();
	// 	res.scene_index = scene->GetIndex();
	// 	InternalComm::Instance().NetSend(InternalComm::Instance().m_golbal_server_id, (const char*)&res, sizeof(gglobalprotocal::GGlobalRoleEnterScene));
}

void SceneManager::UnregisterRoleLocal(const GSNetID &netid, const UserID &user_id)
{
	m_net_to_rolelocal.erase(netid);
	m_uid_to_rolelocal.erase(user_id);

	gamelog::g_log_debug.printf(LL_DEBUG, "SceneManager::UnregisterRoleLocal uid[%d] netid[%d,%d]",
		UidToInt(user_id), netid.gateway_netid, netid.netid);
}

bool SceneManager::Logout(Scene *scene, ObjID obj_id)
{
	Obj *role_roj = scene->GetObj(obj_id);
	if (NULL == role_roj || role_roj->GetObjType() != Obj::OBJ_TYPE_ROLE)
	{
		return false;
	}

	Role *role = (Role*)role_roj;
	UserID user_id = role->GetUserId();

	role->OnLogout(); // 要把状态处理完再下线

	scene->RoleLeaveScene(obj_id, true);

	role->Save(true); // 存档	

//#ifdef _DEBUG
//	printf("SceneManager::Logout role[%d,%s]\n", role->GetUID(), role->GetName());
//#endif

	DelayLoginManager::GetInstance().OnUserLogout(user_id);

	return true;
}

void SceneManager::Logout(const UserID &user_id)
{
	UIDSceneMap::iterator i = m_uid_to_rolelocal.find(user_id);
	if (i != m_uid_to_rolelocal.end())
	{
		if (i->second.scene_index != SceneIndex(-1))
		{
			SceneList::Iterator iter = m_scene_list.Find(i->second.scene_index);
			SceneList::Iterator end_iter = m_scene_list.End();
			if (iter != end_iter) // 找到直接退出
			{
				this->Logout(*iter, i->second.obj_id);
			}
			else // 找不到容错 不然 m_uid_to_rolelocal会保存user_id的映射 下次再也进不了游戏 
			{
				Role *role = this->GetRole(user_id);
				if (NULL == role) // 再找一次 找不到拉倒 直接干
				{
					gamelog::g_log_scenemanager.printf(LL_ERROR,
						"[SceneManager::Logout(UserID:%d,%d) Error][scene_index:%d obj_id:%d] delete uid_local",
						user_id.db_index, user_id.role_id, (int)i->second.scene_index, (int)i->second.obj_id);

//#ifdef _DEBUG
//					printf("[SceneManager::Logout(UserID:%d,%d) Error][scene_index:%d obj_id:%d] delete uid_local\n",
//						user_id.db_index, user_id.role_id, (int)i->second.scene_index, (int)i->second.obj_id);
//#endif

					m_uid_to_rolelocal.erase(user_id);
				}
				else // 竟然还有role 严重了 
				{
					gamelog::g_log_serious_error.printf(LL_ERROR,
						"[SceneManager::Logout(UserID:%d,%d) Error][scene_index:%d obj_id:%d] exist",
						user_id.db_index, user_id.role_id, (int)i->second.scene_index, (int)i->second.obj_id);
				
//#ifdef _DEBUG
//					printf("[SceneManager::Logout(UserID:%d,%d) Error][scene_index:%d obj_id:%d] exist\n",
//						user_id.db_index, user_id.role_id, (int)i->second.scene_index, (int)i->second.obj_id);
//#endif
				}
			}
		}
		else
		{
//#ifdef _DEBUG
//			printf("[SceneManager::Logout(UserID) Error][scene_index:%d obj_id:%d]\n", (int)i->second.scene_index, (int)i->second.obj_id);
//
//#endif

			gamelog::g_log_scenemanager.printf(LL_ERROR, "[SceneManager::Logout(UserID) Error][scene_index:%d obj_id:%d]", (int)i->second.scene_index, (int)i->second.obj_id);
			m_uid_to_rolelocal.erase(i);
		}
	}
}

// 存在未知原因导致玩家无法踢出，进程无法正常关闭
void SceneManager::LogoutAllRole(NetID gateway_netid, FilterFunc func, void *param_p)
{
	struct DisItem
	{
		RoleLocal rl;
		GSNetID netid;
	};

	int not_filter_num = 0;
	while (true)
	{
		static DisItem gsnetid_list[512];
		int dis_num = 0;

		for (NetSceneMap::iterator i = m_net_to_rolelocal.begin(); dis_num < 512 && i != m_net_to_rolelocal.end(); ++i)
		{
			if ((NetID)-1 == gateway_netid || i->first.gateway_netid == (unsigned short)(gateway_netid))
			{
				DisItem di;
				di.rl = i->second;
				di.netid = i->first;
				gsnetid_list[dis_num++] = di;
			}
		}

		if (not_filter_num >= dis_num)
		{
			break;
		}

		// 统计有几个人不需要被踢
		int tmp_num = 0;
		for (int i = 0; i < dis_num; ++i)
		{
			if (gsnetid_list[i].rl.scene_index != SceneIndex(-1))
			{
				SceneList::Iterator iter = m_scene_list.Find(gsnetid_list[i].rl.scene_index);
				if (iter != m_scene_list.End())
				{
					if (NULL != func)
					{
						if (!func(*iter, gsnetid_list[i].rl.obj_id, param_p))
						{
							++tmp_num;
							continue;
						}
					}

					if (!this->Logout(*iter, gsnetid_list[i].rl.obj_id)) // 这里有可能出现logout失败导致m_net_to_rolelocal没删除干净的情况 xiaolong
																	 // 提完之后还得再循环一次检查踢干净了没
					{
						// 当返回false时表示obj_id已非玩家的实例，此时需要把netid从m_net_to_rolelocal删掉，否则会死循环 by chenyulin
						// 即无法通过gsnetid_list[i].rl找到Role实例（原因未知）
						m_net_to_rolelocal.erase(gsnetid_list[i].netid);
				}
					continue; 
			}
			}

			m_net_to_rolelocal.erase(gsnetid_list[i].netid);
		}

		// 如果不需要踢出的玩家的数量不同，则重新赋值
		if (tmp_num != not_filter_num)
		{
			not_filter_num = tmp_num;
		}
	}
}

void SceneManager::KickAllRole(NetID gateway_netid, FilterFunc func, void *param_p)
{
	struct DisItem
	{
		RoleLocal rl;
		GSNetID netid;
	};

	int not_filter_num = 0;
	while (true)
	{
		static DisItem gsnetid_list[512];
		int dis_num = 0;

		for (NetSceneMap::iterator i = m_net_to_rolelocal.begin(); dis_num < 512 && i != m_net_to_rolelocal.end(); ++i)
		{
			if ((NetID)-1 == gateway_netid || i->first.gateway_netid == (unsigned short)(gateway_netid))
			{
				DisItem di;
				di.rl = i->second;
				di.netid = i->first;
				gsnetid_list[dis_num++] = di;
			}
		}

		if (not_filter_num >= dis_num)
		{
			break;
		}

		// 统计有几个人不需要被踢
		int tmp_num = 0;
		for (int i = 0; i < dis_num; ++i)
		{
			if (gsnetid_list[i].rl.scene_index != SceneIndex(-1))
			{
				SceneList::Iterator iter = m_scene_list.Find(gsnetid_list[i].rl.scene_index);
				if (iter != m_scene_list.End())
				{
					if (NULL != func)
					{
						if (!func(*iter, gsnetid_list[i].rl.obj_id, param_p))
						{
							++tmp_num;
							continue;
						}
					}

					if (NULL != *iter)
					{
						Role *role = (Role *)((*iter)->GetObj(gsnetid_list[i].rl.obj_id));
						this->Logout(*iter, gsnetid_list[i].rl.obj_id);
						EngineAdapter::Instance().NetDisconnect(gsnetid_list[i].netid, role->GetUserId(), role->GetName(), (*iter)->GetSceneID(), __FUNCTION__);
					}

					continue;
				}
			}

			m_net_to_rolelocal.erase(gsnetid_list[i].netid);
		}

		// 如果不需要踢出的玩家的数量不同，则重新赋值
		if (tmp_num != not_filter_num)
		{
			not_filter_num = tmp_num;
		}
	}
}

bool SceneManager::CheckIsDelayDeleteScene(int scene_id)
{
	FBConfigList::iterator it = m_fb_config_list.find(scene_id);
	if (it != m_fb_config_list.end())
	{
		return Scene::IsDelayDeleteScene(it->second.scene_type, scene_id);
	}
	return false;
}

bool SceneManager::CheckIsCommonFBScene(int scene_id)
{
	FBConfigList::iterator it = m_fb_config_list.find(scene_id);
	if (it != m_fb_config_list.end())
	{
		int scene_type = it->second.scene_type;
		if (Scene::SCENE_TYPE_GUILD_ANSWER == scene_type ||
			Scene::SCENE_TYPE_GUILD_FIGHT == scene_type ||
			Scene::SCENE_TYPE_MAZE_RACER_WAITING_HALL == scene_type ||
			Scene::SCENE_TYPE_MAZE_RACER == scene_type ||
			Scene::SCENE_TYPE_TEAM_FIGHT == scene_type ||
			Scene::SCENE_TYPE_PLATFORM_BATTLE == scene_type ||
			Scene::SCENE_TYPE_ANIMAL_RACE == scene_type ||
			Scene::SCENE_TYPE_ONLY_FIGHT == scene_type || 
			Scene::SCENE_TYPE_GATHER_FB == scene_type || 
			Scene::SCENE_TYPE_FISHING_GROUND == scene_type ||
			Scene::SCENE_TYPE_WORLD_TEAM_ARENA == scene_type || 
			Scene::SCENE_TYPE_QING_YUAN_SHENG_HUI == scene_type ||
			Scene::SCENE_TYPE_UTA == scene_type) 
		{
			return true;
		}
		UNSTD_STATIC_CHECK(55 == Scene::SCENE_TYPE_COUNT);
	}
	return false;
}

bool SceneManager::CheckIsChiefElectionSeaScene(int scene_id)
{
	return LOGIC_CONFIG->GetActivityChiefElectionConfig()->GetOhterCfg().IsSeaElectionSceneid(scene_id);
}

bool SceneManager::GetChiefElectionScene(int role_id, int now_scene_id, int * scene_id, int * scene_key)
{
	UserCacheNode * user_cache = UserCacheManager::Instance().GetUserNode(role_id);
	ActivityChiefElection * activity = (ActivityChiefElection *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_CHIEF_ELECTION);
	if (user_cache == NULL || activity == NULL || !activity->IsActivityOpen())
	{
		return false;
	}

	return activity->GetScene(role_id, user_cache->GetBaseProfession(), now_scene_id, scene_id, scene_key);
}

bool SceneManager::IsFB(int scene_id)
{
	FBConfigList::iterator it = m_fb_config_list.find(scene_id);
	if (it != m_fb_config_list.end())
	{
		return true;
	}
	return false;
}

bool SceneManager::CreateFB(int fb_scene_id, int fb_scene_key, Scene **scene, bool check_key /*= true*/, unsigned int scene_timeout /*= 0*/)
{
	if (check_key && 0 == fb_scene_key) return false; // FB 不允许使用0，规范化以减少错误发生的可能

	FBConfigList::iterator it = m_fb_config_list.find(fb_scene_id);
	if (it == m_fb_config_list.end()) return false;

	bool timeout_kick = it->second.config.is_timeout_kick_user; // 计时场景时间到是否踢人
	if (0 == scene_timeout)	// 如果非0则以外面传入为准 如果fb需要倒计时 则此处将scene_timeout赋值
	{
		scene_timeout = it->second.config.scene_timeout;
	}

	Scene *fb_scene = 0;
	bool ret = CreateScene(it->second.config, fb_scene_key, &fb_scene, it->second.scene_type, scene_timeout, timeout_kick);
	if (!ret) return false;

	*scene = fb_scene;

	return true;
}

bool SceneManager::DestroyFB(SceneIndex scene_index)
{
	Scene *scene = GetSceneByIndex(scene_index);
	if (NULL == scene)
	{
		return false;
	}

	if (0 != scene->RoleNum())
	{
		return false;
	}

	m_destroy_scene_list.push_back(scene_index);
	m_sceneidmap.erase(SceneIDKey(scene->GetSceneID(), scene->GetSceneKey()));

	return true;
}

bool SceneManager::GoTo(Role * role, int target_scene_id, int target_scene_key)
{
	Posi  target_pos;
	target_pos = this->GetTownPos(target_scene_id);

	return this->GoTo(role, target_scene_id, target_scene_key, target_pos);
}

bool SceneManager::GoTo(Role * role, int target_scene_id, int target_scene_key, Posi  target_pos, bool is_set_target_pos)
{
	if (NULL == role) return false;

	Scene *scene = role->GetScene();
	if (NULL == scene) return false;

	if (is_set_target_pos)
	{
		target_pos = this->GetTownPos(target_scene_id);
	}

	bool return_ret = true;

	// 在同一个场景内传送，重设坐标就可以
	if (scene->GetSceneID() == target_scene_id && scene->GetSceneKey() == target_scene_key)
	{
		return role->ResetPos(target_pos.x, target_pos.y);
	}

	return return_ret && this->ChangeScene(scene, role->GetId(), target_scene_id, target_scene_key, target_pos);
}

bool SceneManager::GoTo(Robot *robot, int target_scene_id, int target_scene_key, const Posi &target_pos)
{
	if (NULL == robot) return false;

	Scene *scene = robot->GetScene();
	if (NULL == scene) return false;

	// 在同一个场景内传送，重设坐标就可以
	if (scene->GetSceneID() == target_scene_id && scene->GetSceneKey() == target_scene_key)
	{
		return robot->ResetPos(target_pos.x, target_pos.y);
	}

	return ROBOTMANAGER.RobotChangeScene(robot, target_scene_id, target_scene_key, target_pos, __FUNCTION__);
}

bool SceneManager::TeamGoTo(Team* team, int target_scene_id, int target_scene_key, const Posi &target_pos)
{
	if (NULL == team) return false;

	Role* leader_role = team->GetLeaderRole();
	if (NULL == leader_role) return false;

	Scene* old_scene = leader_role->GetScene();
	if (NULL == old_scene) return false;

	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int role_num = 0;
	role_list[role_num++] = leader_role;

	Robot* robot_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	int robot_num = 0;

	int member_num = team->GetMemberCount();
	for (int i = 0; i < member_num; ++i)
	{
		TeamMember* member = team->GetMemberInfo(i);
		if (NULL == member || member->Invalid()) continue;
		if (member->uid == leader_role->GetUID()) continue;

		if (!member->IsRobot())
		{
			if (!member->is_follow || !member->is_online) continue;

			Role* role = team->GetMemberRoleByIndex(i);
			if (NULL == role) continue;
			
			role_list[role_num++] = role;
		}
		else
		{
			Robot* robot = team->GetMemberRobotByIndex(i);
			if (NULL == robot) continue;

			robot_list[robot_num++] = robot;
		}
	}

	for (int i = 0; i < role_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Role* role = role_list[i];
		this->GoTo(role, target_scene_id, target_scene_key, target_pos);
	}

	for (int i = 0; i < robot_num && i < MAX_TEAM_MEMBER_NUM; ++i)
	{
		Robot* robot = robot_list[i];
		this->GoTo(robot, target_scene_id, target_scene_key, target_pos);
	}

	Scene* new_scene = this->GetSceneById(target_scene_id, target_scene_key);
	if (NULL != new_scene)
	{
		if (new_scene->IsInStaticScene() && !old_scene->IsInStaticScene())
		{
			TeamManager::Instance().OnTeamSwitchToStaticScene(team);
		}
		else if (!new_scene->IsInStaticScene() && old_scene->IsInStaticScene())
		{
			TeamManager::Instance().OnTeamSwitchToSpecialScene(team);
		}
	}

	return true;
}

Posi SceneManager::GetTownPos(int scene_id)
{
	FBConfigList::iterator it = m_fb_config_list.find(scene_id);
	if (it != m_fb_config_list.end())
	{
		return Posi(it->second.config.townpoint.scenex, it->second.config.townpoint.sceney);
	}

	return Posi(0,0);
}

bool SceneManager::FlyByShoe(Role *role, int target_scene_id, int x, int y)
{
	if (NULL == role 
		|| !role->GetRoleStatusManager()->CanFlyByShoe(target_scene_id) 
		|| !role->GetRoleModuleManager()->CanEnterScene(target_scene_id)) return false;

	Scene * scene = this->GetSceneById(target_scene_id, COMMON_SCENE_KEY);
	if (NULL == scene || !scene->CanEnter(role)) return false;

	if (Scene::SCENE_TYPE_ONLY_FIGHT == role->GetScene()->GetSceneType())	//以一敌百不能用这种方式离开
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_FLY_SHOE);
		return false;
	}

	if (Scene::SCENE_TYPE_HONGMENG_TIANJIE == role->GetScene()->GetSceneType())	//不能用这种方式离开副本
	{
		role->NoticeNum(errornum::EN_GOTO_COMMON_SCENE_NOT);
		return false;
	}

	Team* team = role->GetMyTeam();
	if (NULL != team)
	{
		TeamMember * team_member =team->GetMemberInfo(role);
		if(team_member== NULL)return false;

		if (!team->IsLeader(role))
		{
			 if(team_member->is_follow)return false;

			 Posi target_pos(x, y);
			 this->GoTo(role, target_scene_id, COMMON_SCENE_KEY, target_pos);
			 return true;
		}

		bool can_team_enter = true;		// 每个队员都要飘报错信息，所以检测到不能进入的队员时不能直接返回；
		for (int i = 0; i < MAX_TEAM_MEMBER_NUM; ++i)
		{
			TeamMember *member = team->GetMemberInfo(i);
			if (NULL == member || !member->is_follow || member->uid == role->GetUID()) continue;

			if (!member->IsRobot())
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL == member_role) continue;

				if (!scene->CanEnter(member_role)
					|| !member_role->GetRoleModuleManager()->CanEnterScene(target_scene_id))
				{
					can_team_enter = false;
					continue;
				}
			}
		}

		if (!can_team_enter)
		{
			role->NoticeNum(errornum::EN_MEMBER_CAN_NOT_ENTER_SCENE);
			return false;
		}

		Posi target_pos(x, y);
		this->TeamGoTo(team, target_scene_id, COMMON_SCENE_KEY, target_pos);
	}
	else
	{
		Posi target_pos(x, y);
		this->GoTo(role, target_scene_id, COMMON_SCENE_KEY, target_pos);
	}
	

	return true;
}

bool SceneManager::CheckSceneIsExist(int scene_id)
{
	return m_scene_id_to_type_map.end() != m_scene_id_to_type_map.find(scene_id);
}

int SceneManager::GetSceneTypeBySceneID(int scene_id)
{
	std::map<int, int>::iterator type_it = m_scene_id_to_type_map.find(scene_id);
	if (type_it == m_scene_id_to_type_map.end())
	{
		return Scene::SCENE_TYPE_COMMON_SCENE; // 找不到就当作普通场景，一般不可能找不到
	}

	return type_it->second;
}

void SceneManager::RoleTimeOut(Scene *scene, Role *role, const char *reason, int param1 /*= 0*/)
{
	gamelog::g_log_scenemanager.buff_printf("[Scene::CheckRoleTimeOut Error][name:%s UserID(%d, %d) SceneIndex:%d ObjID:%d param1:%d reason %s]",
		role->GetName(), role->GetUserId().db_index, role->GetUserId().role_id, (int)scene->GetIndex(), (int)role->GetId(), param1, reason);

	UIDSceneMap::iterator uid_i = m_uid_to_rolelocal.find(role->GetUserId());
	if (uid_i != m_uid_to_rolelocal.end())
	{
		gamelog::g_log_scenemanager.buff_printf("[m_uid_to_rolelocal SceneIndex:%d ObjID:%d]", (int)uid_i->second.scene_index, (int)uid_i->second.obj_id);
	}
	else
	{
		gamelog::g_log_scenemanager.buff_printf("[m_uid_to_rolelocal can NOT find]");
	}

	NetSceneMap::iterator net_i = m_net_to_rolelocal.find(role->GetNetId());
	if (net_i != m_net_to_rolelocal.end())
	{
		gamelog::g_log_scenemanager.buff_printf("[m_net_to_rolelocal  SceneIndex:%d ObjID:%d]", (int)net_i->second.scene_index, (int)net_i->second.obj_id);
	}
	else
	{
		gamelog::g_log_scenemanager.buff_printf("[m_net_to_rolelocal can NOT find]");
	}
	gamelog::g_log_scenemanager.commit_buff(LL_ERROR);

	this->Logout(scene, role->GetId());

	EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), scene->GetSceneID(), reason);
}

void SceneManager::Update(unsigned long intervals)
{
	m_game_time = EngineAdapter::Instance().GetGameTime();
	time_t now = EngineAdapter::Instance().Time();
	unsigned int now_dayid = EngineAdapter::Instance().DayID();

	this->DestroyScene();

	if (m_check_timeout && (m_game_time - m_last_check_timeout > m_check_timeout_interval))
	{
		this->EraseTimeOut();
		m_last_check_timeout = m_game_time;
	}

	//暂时不需要检查加速
	//if (now - m_last_chk_speedup_time >= 30)
	//{
	//	m_last_chk_speedup_time = now;
	//	this->EraseSpeedup();
	//}

	for (SceneList::Iterator i = m_scene_list.Beg(); i != m_scene_list.End(); ++i)
	{
		(*i)->Update(intervals, now, now_dayid);
	}

	this->DestroyScene(); // 这里不清理会导致 场景无效还是能进入场景 那么角色就直接被干掉 

	//m_scene_match_manager.Update(intervals);
}

void SceneManager::OnRecvMsgFromBattle(NetID netid, const char* data, int length)
{
	battlegameprotocol::MessageHeader *header = (battlegameprotocol::MessageHeader*)data;

	int msg_type = header->msg_type;
	if (msg_type < 0 || msg_type >= ARRAY_ITEM_COUNT(m_battle_game_handler) || NULL == m_battle_game_handler[msg_type])
	{
		return;
	}

	(this->*(m_battle_game_handler[msg_type]))(data, length);
}

void SceneManager::OnRecvMsgFromCross(const char *msg, int length)
{
	crossgameprotocal::MessageHeader *header = (crossgameprotocal::MessageHeader*)msg;

	int msg_type_index = header->msg_type - crossgameprotocal::MT_CROSS_GAME_BEGIN;
	if (msg_type_index < 0 || msg_type_index >= (int)sizeof(m_on_cross_handler) / (int)sizeof(m_on_cross_handler[0]) ||
		NULL == m_on_cross_handler[msg_type_index])
	{
		return;
	}

	(this->*(m_on_cross_handler[msg_type_index]))((const char *)header, length);
}

void SceneManager::OnActivityStatusChange(ActivityStatus * as)
{
}

void SceneManager::KickOutAllBraveGround()
{
	for (SceneList::Iterator i = m_scene_list.Beg(); i != m_scene_list.End(); ++i)
	{
		Scene * scene = *i;
		if (NULL != scene && scene->GetSceneType() == Scene::SCENE_TYPE_BRAVE_GROUND)
		{
			SpecialLogicBraveGround * special_logic = (SpecialLogicBraveGround *)scene->GetSpecialLogic();
			if (special_logic != NULL)
			{
				special_logic->OnActivityBraveGroundClose();
			}
		}
	}
}

bool SceneManager::SendToUser(const UserID &user_id, const void *data, int length)
{
	Role *role = this->GetRole(user_id);
	if (NULL == role)
	{
		return false;
	}

	bool ret = EngineAdapter::Instance().NetSend(role->GetNetId(), data, length);
	return ret;
}

bool SceneManager::NoticeToUser(const UserID &user_id, int notice_num)
{
	Role *role = this->GetRole(user_id);
	if (NULL == role)
	{
		return false;
	}

	role->NoticeNum(notice_num);
	return true;
}

bool UniqueServerFilter(Role* role, void* param)
{
	if (NULL == role || NULL == param) return false;

	const UniqueServerID* mother_usid = World::GetInstWorld()->GetMotherServerId(role->GetUniqueServerID());
	if (NULL == mother_usid) return false;

	UniqueServerID* usid = (UniqueServerID*)param;
	return *usid == *mother_usid;
}

bool SceneManager::SystemMsgFilter(const char *msg, int length, SysMsgType msg_type, int color /* = 0 */, int display_pos /* = 0 */, 
	int min_level /* = 0 */, int max_level /* = 0*/, RoleFilterFunc func /* = NULL */, void *param_p /* = NULL */)
{
	if (NULL == func)
	{
		return this->SystemMsgAll(msg, length, msg_type, color, display_pos, min_level, max_level);
	}

	Protocol::SCSystemMsg sm;
	int sendlen = 0;
	if (!this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		return false;
	}

	static const int SEND_NUM = 512;
	static GSNetID gsnetid_list[SEND_NUM];
	int num = 0;

	for (NetSceneMap::iterator i = m_net_to_rolelocal.begin(); i != m_net_to_rolelocal.end(); ++i)
	{
		SceneIndex si = i->second.scene_index;
		ObjID obj_id = i->second.obj_id;
		Role* role = this->GetRole(si, obj_id);
		if (func(role, param_p))
		{
			gsnetid_list[num++] = i->first;

			if (num >= SEND_NUM)
			{
				EngineAdapter::Instance().NetMulSend(gsnetid_list, num, (const char*)&sm, sendlen);
				num = 0;
			}
		}
	}

	if (num > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetid_list, num, (const char*)&sm, sendlen);
	}
	
	return true;
}

bool SceneManager::SystemMsgAll(const char *msg, int length, SysMsgType msg_type, int color /*= 0*/, int display_pos /*= 0*/, 
	int min_level /*= 0*/, int max_level /* = 0*/, const char *spid /*= 0*/, int spid_id_length /*= 0*/)
{
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level, spid, spid_id_length))
	{
		return World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
	}

	return false;
}

bool SceneManager::SystemMsgAllGame(const char * msg, int length, SysMsgType msg_type, int color, int display_pos, int min_level, int max_level)
{
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		return World::GetInstWorld()->SendToALLGameAllGateway((const char *)&sm, sendlen);
	}

	return false;
}

bool SceneManager::SystemMsgProf(int prof, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, int min_level, int max_level)
{
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		ChatManager::Instance().SendProfChannel(prof, (const char *)&sm, sendlen);
		return true;
	}

	return false;
}

bool SceneManager::SystemMsgPerson(UserID uid, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, int min_level, int max_level)
{
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		return SendToUser(uid, (const char*)&sm, sendlen);
	}

	return false;
}

bool SceneManager::SystemMsgScene(Scene* scene, const char* msg, int length, SysMsgType msg_type, int color /*= 0*/, int display_pos /*= 0*/, 
	int min_level /*= 0*/, int max_level /* = 0*/)
{
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (!this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		return false;
	}

	for (int i = 0; i < scene->RoleNum(); ++i)
	{
		Role* role = scene->GetRoleByIndex(i);
		if (NULL == role) continue;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &sm, sendlen);
	}

	return true;
}

bool SceneManager::SystemMsgPerson2(Role * role, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, int min_level, int max_level)
{
	if (role == NULL)
	{
		return false;
	}

	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		return EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&sm, sendlen);
	}

	return false;
}

bool SceneManager::SystemMsgTeamMember(Team * team, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, 
	int min_level, int max_level, bool notice_leader)
{
	if (NULL == team)
	{
		return false;
	}
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		team->SendToMember(&sm, sendlen, notice_leader);
	}

	return false;
}

bool SceneManager::SystemMsgPersonAndTeamMember(Role * role, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, 
	int min_level, int max_level, bool notice_leader)
{
	//有队伍则都发,没队伍则发自己
	if (NULL == role)
	{
		return false;
	}
	static Protocol::SCSystemMsg sm;

	int sendlen = 0;

	if (this->CreateSystemMsg(&sm, &sendlen, msg, length, msg_type, color, display_pos, min_level, max_level))
	{
		Team * team = role->GetMyTeam();
		if (NULL != team)
		{
			team->SendToMember(&sm, sendlen, notice_leader);
		}
		else
		{
			return EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&sm, sendlen);
		}
	}

	return false;
}

bool SceneManager::CreateSystemMsg(Protocol::SCSystemMsg * sm, int * sendlen, const char * msg, int length, SysMsgType msg_type, int color, int display_pos, 
	int min_level, int max_level, const char *spid, int spid_id_length)
{
	if (length < 0 || length > PROTOCAL_MAX_SYSTEM_MSG_LENGTH || NULL == sm || NULL == sendlen) return false;

	sm->send_time = (unsigned int)EngineAdapter::Instance().Time();
	sm->msg_type = msg_type.GetPosBinary();
	sm->reserve_sh = 0;
	sm->min_level = (short)min_level;
	sm->max_level = (short)max_level;
	sm->msg_length = (short)length;
	memcpy(sm->msg, msg, length);
	sm->color = 0;
	sm->display_pos = 0;
	memset(sm->spid_id, 0, sizeof(sm->spid_id));
	memcpy(sm->spid_id, spid, spid_id_length);

	*sendlen = sizeof(Protocol::SCSystemMsg) - PROTOCAL_MAX_SYSTEM_MSG_LENGTH + length;

	return true;
}

void SceneManager::OnReloadConfig(int config_type, int param1, int param2)
{
	if (RELOAD_CONFIG_TYPE_ACTIVITY == config_type)
	{
		DelayReloadConfigTimer* timer = new DelayReloadConfigTimer(config_type, param1, param2);
		EngineAdapter::Instance().CreateTimerSecond(60, timer);

		return;
	}

	ReloadConfigManager::Instance().ReloadConfig(config_type, param1, param2);
}

SpecialLogicBraveGround * SceneManager::GetSpecialLogicBraveGround(int scene_id, int fb_key, bool is_create)
{
	Scene * scene = this->GetSceneById(scene_id, fb_key);
	if (NULL == scene)
	{
		if (is_create)
		{
			this->CreateFB(scene_id, fb_key, &scene);
		}
	}

	if (NULL != scene)
	{
		SpecialLogic * special_logic = scene->GetSpecialLogic();
		if (Scene::SCENE_TYPE_BRAVE_GROUND == special_logic->GetSceneType())
		{
			return (SpecialLogicBraveGround*)special_logic;
		}
	}

	return NULL;
}

SpecialLogicChiefElection * SceneManager::GetSpecialLogicChiefElection(int scene_id, int fb_key, bool is_create)
{
	Scene * scene = this->GetSceneById(scene_id, fb_key);
	if (NULL == scene)
	{
		if (is_create)
		{
			this->CreateFB(scene_id, fb_key, &scene);
		}
	}

	if (NULL != scene)
	{
		SpecialLogic * special_logic = scene->GetSpecialLogic();
		if (Scene::SCENE_TYPE_CHIEF_ELECTION == special_logic->GetSceneType())
		{
			return (SpecialLogicChiefElection*)special_logic;
		}
	}

	return NULL;
}

SpecialLogicChiefPeakBattle * SceneManager::GetSpecialLogicChiefPeakBattle(int scene_id, int fb_key, bool is_create)
{
	Scene * scene = this->GetSceneById(scene_id, fb_key);
	if (NULL == scene)
	{
		if (is_create)
		{
			this->CreateFB(scene_id, fb_key, &scene);
		}
	}

	if (NULL != scene)
	{
		SpecialLogic * special_logic = scene->GetSpecialLogic();
		if (Scene::SCENE_TYPE_CHIEF_PEAK_BATTLE == special_logic->GetSceneType())
		{
			return (SpecialLogicChiefPeakBattle*)special_logic;
		}
	}

	return NULL;
}

SpecialLogicGuildHonorBattle * SceneManager::GetSpecialLogicGuildHonorBattle(int scene_id, int fb_key, bool is_create)
{
	Scene * scene = this->GetSceneById(scene_id, fb_key);
	if (NULL == scene)
	{
		if (is_create)
		{
			this->CreateFB(scene_id, fb_key, &scene);
		}
	}

	if (NULL != scene)
	{
		SpecialLogic * special_logic = scene->GetSpecialLogic();
		if (Scene::SCENE_TYPE_GUILD_HONOR_BATTLE == special_logic->GetSceneType())
		{
			return (SpecialLogicGuildHonorBattle*)special_logic;
		}
	}

	return NULL;
}

bool SceneManager::ChangeScene(Scene *scene, ObjID obj_id, int target_scene_id, int target_scene_key, const Posi &target_pos)
{
	Obj *obj = scene->GetObj(obj_id);
	if (NULL == obj || obj->GetObjType() != Obj::OBJ_TYPE_ROLE)
	{
		return false;
	}

	Role *role = (Role*)obj;

	Scene * target_scene = this->GetSceneById(target_scene_id, target_scene_key);
	if (NULL != target_scene) // scene 在本地
	{
		return this->ChangeLocalSceneHelper(scene, role, target_scene, target_pos);
	}
	else
	{
		Scene *fb_scene = NULL;
		bool ret = this->CreateFB(target_scene_id, target_scene_key, &fb_scene);
		if (ret)
		{
			return this->ChangeLocalSceneHelper(scene, role, fb_scene, target_pos);
		}
	}

	return false;
}

bool SceneManager::ChangeLocalSceneHelper(Scene *old_scene, Role *role, Scene *target_scene, const Posi &target_pos)
{
	old_scene->RoleLeaveScene(role->GetId());  // 离开当前场景（这句要放在保存之前，因为该函数内可能会引起角色数据变化

	int target_scene_id = target_scene->GetSceneID();
	Posi target_pos_tmp = target_pos;
	role->Save(false, &target_scene_id, &target_pos_tmp);

	static RoleInitParam p;
	role->GetRoleInitParam(&p);

	if (Scene::IsStaticScene(role->GetScene()->GetSceneType()))	// 只有普通场景才会保存到last_scene中
	{
		p.last_scene_id = p.scene_id;
		p.last_scene_x = p.scene_x;
		p.last_scene_y = p.scene_y;
	}

	p.scene_id = target_scene->GetSceneID();
	p.scene_x = target_pos.x;
	p.scene_y = target_pos.y;

	RoleModuleManager* role_module_manager = role->GetRoleModuleManager();
	role->SetRoleModuleManager(NULL); // 原有的module_mgr需要置空，相当于移交了控制权

	PlatName pname;
	role->GetPlatName(pname);

	// 下面这些param，都只是用于RoleEnterScene传参时占位用的
	static RoleOtherInitParam other_p;
	static ItemListParam item_list_param; item_list_param.count = 0;
	static SkillListParam skill_list_param; skill_list_param.count = 0;
	static MailListParam maillist_param; maillist_param.count = 0;
	static RoleFirstKillParamList first_kill_list; first_kill_list.count = 0;
	static RandActivityRoleParamList rand_activity_role_data_list; rand_activity_role_data_list.count = 0;
	static RoleCommonSaveDataParam role_common_save_list; role_common_save_list.count = 0;
	static RoleCourageChallengeLevelParamList role_courage_challenge_list; role_courage_challenge_list.count = 0;
	static GodPrintItemListParam god_print_item_list; god_print_item_list.count = 0;
	static NewCourageChallengeLevelParamList new_courage_challenge_list; new_courage_challenge_list.count = 0;
	
	Role *new_role = NULL;
	target_scene->RoleEnterScene(role->GetPlatSpid(), pname, role->GetUserId(), role->GetNetId(),
								 role_module_manager,
								 p,
								 other_p,
								 item_list_param,
								 skill_list_param,
								 maillist_param,
								 first_kill_list,
								 rand_activity_role_data_list,
								 role_common_save_list,
								 role_courage_challenge_list,
								 god_print_item_list,
								 new_courage_challenge_list,
								 false,
								 &new_role);

	if (NULL != new_role)
	{
		// 用于追踪玩家去向 追查Role的m_role_module_mgr为空的问题
		gamelog::g_log_role_unique_check.printf(LL_INFO, "SceneManager::ChangeLocalSceneHelper role[%d,%s] old_scene[%d,%s]->new_scene[%d,%s] old_unique_id[%lld]->new_unique_id[%lld]",
			new_role->GetUID(), new_role->GetName(), old_scene->GetSceneID(), old_scene->GetSceneName(), target_scene->GetSceneID(), target_scene->GetSceneName(), role->GetRoleCheckUniqueId(), new_role->GetRoleCheckUniqueId());
	}

	return true;
}

void RMIRoleSaveBackObjImplLocal::RoleSaveRet(int ret)
{
	if (ret != 0) // 处理失败情况
	{
		Error(log_user_id, GAME_ERROR_EXECUTE, ret);
	}
}

bool SceneManager::CreateScene(const ConfigScene &scene_config, int scene_key, Scene **scene_create, int scene_type, unsigned int scene_timeout, bool timeout_kick)
{
	MapList::iterator scene_map = m_map_list.find(scene_config.mapid);
	if (scene_map == m_map_list.end())
	{
		gamelog::g_log_scenemanager.printf(LL_WARNING, "SceneManager On INIT, create scene[id:%d] error, map id(%d) is NOT exist!", scene_config.id, scene_config.mapid);
		return false;
	}

	Scene *scene = new Scene(this);

	SceneIndex sceneid = (SceneIndex)m_scene_list.Insert(scene);
	bool ret = scene->Init(sceneid, scene_config, scene_map->second, scene_type, scene_key, scene_timeout, timeout_kick);
	if (!ret)
	{
		m_scene_list.Erase(sceneid);
		delete scene;
		return false;
	}

	m_sceneidmap[SceneIDKey(scene_config.id, scene_key)] = scene->GetIndex();

	*scene_create = scene;

	return true;
}

void SceneManager::EraseTimeOut()
{
	struct TmpTimeoutRecd
	{
		TmpTimeoutRecd() : scene(NULL), obj_id(0) {}

		Scene *scene;
		ObjID obj_id;
	};

	TmpTimeoutRecd timeout_recorder[32];
	int timeout_count = 0;

	GSNetID disconnect_netid[32];
	Role *disconnect_role[32] = { NULL };
	int disconn_count = 0;

	for (NetSceneMap::iterator i = m_net_to_rolelocal.begin(); timeout_count < 32 && disconn_count < 32 && i != m_net_to_rolelocal.end(); ++i)
	{
		// 这里之前有严重的bug game_time有可能小于last_active_time 因为对时导致game_time变小 一减就变成负的 unsigned 就跪了
		// 那么用加法不就不会跪了么 -- by chenyulin
		if (m_game_time > i->second.last_active_time + m_heartbeat_dead_time)
		{
			UserID user_id;

			Role *role = GetRole(i->second.scene_index, i->second.obj_id);
			if (NULL != role)
			{
				user_id = role->GetUserId();
			}
			//printf("EraseTimeOut user_id[%d, %d] obj_id:%d.", user_id.db_index, user_id.role_id, i->second.obj_id);
			gamelog::g_log_scenemanager.printf(LL_MAINTANCE, "EraseTimeOut user_id[%d, %d] obj_id:%d.", user_id.db_index, user_id.role_id, i->second.obj_id);

			GSNetID netid = i->first;
			if (i->second.scene_index != SceneIndex(-1))
			{
				SceneList::Iterator iter = m_scene_list.Find(i->second.scene_index);
				if (iter != m_scene_list.End())
				{
					timeout_recorder[timeout_count].scene = *iter;
					timeout_recorder[timeout_count].obj_id = i->second.obj_id;
					++timeout_count;
				}
			}

			disconnect_netid[disconn_count] = netid;
			disconnect_role[disconn_count] = role;
			++disconn_count;
		}
	}

	for (int i = 0; i < timeout_count; ++i)
	{
		this->Logout(timeout_recorder[i].scene, timeout_recorder[i].obj_id);
	}

	unsigned int kick_out_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	for (int i = 0; i < disconn_count; ++i)
	{
		Role *role = disconnect_role[i];
		if (NULL != role)
		{
			static char reason_str[128] = { 0 };

			SNPRINTF(reason_str, sizeof(reason_str), "EraseTimeOut_%d_%u", role->GetLastMsgFromClientTimestamp(), kick_out_time);

			EngineAdapter::Instance().NetDisconnect(disconnect_netid[i], role->GetUserId(), role->GetName(), NULL != role->GetScene() ? role->GetScene()->GetSceneID() : -1, reason_str);
		}
		else
		{
			EngineAdapter::Instance().NetDisconnect(disconnect_netid[i], INVALID_USER_ID, "null", -1, "EraseTimeOut");
		}
	}

	for (SceneList::Iterator i = m_scene_list.Beg(); i != m_scene_list.End(); ++i)
	{
		Scene *scene = *i;
		scene->CheckRoleTimeOut(m_game_time);
	}
}

void SceneManager::DestroyScene()
{
	if (m_destroy_scene_list.size() != 0)
	{
		for (int i = 0; i < (int)m_destroy_scene_list.size(); ++i)
		{
			Scene *scene = GetSceneByIndex(m_destroy_scene_list[i]);
			if (NULL == scene) continue;

			delete scene;
			m_scene_list.Erase(m_destroy_scene_list[i]);
		}

		m_destroy_scene_list.clear();
	}
}

void SceneManager::InitBattleGameHandler()
{
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_battle_game_handler) >= battlegameprotocol::MT_BATTLE_GAME_MAX);
	memset(m_battle_game_handler, 0, sizeof(m_battle_game_handler));
	std::set<std::string> function_add_set;

	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_START_FIGHT_ACK, SceneManager::OnBattleStartAck, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_FIGHT_RESULT_NOTICE, SceneManager::OnBattleResultAck, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_RUN_AWAY, SceneManager::OnBattleRoleRunAway, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_SET_AUTO, SceneManager::OnBattleRoleSetAuto, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_SET_AUTO_MOVE, SceneManager::OnBattleRoleSetAutoMove, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ONLY_SET_PET_AUTO_MOVE, SceneManager::OnBattleRoleOnlySyncPetAutoMove, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_USE_ITEM, SceneManager::OnBattleRoleUseItem, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_SEAL_PET, SceneManager::OnBattleRoleSealPet, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_INST_KILL_MONSTER, SceneManager::OnBattleRoleInstKillMonster, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_BATTLE_INSTANCE_RELEASED, SceneManager::OnBattleInstanceReleaseNotice, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_FORCE_FINISH_FIGHT, SceneManager::OnBattleForceFinish, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_HP_SYNC, SceneManager::OnBattleSyncWorldBossHp, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_2_HP_SYNC, SceneManager::OnBattleSyncWorldBossHp2, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_3_HP_SYNC, SceneManager::OnBattleSyncWorldBossHp3, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_REALLY_IN_BATTLE_RESP, SceneManager::OnBattleCheckRoleReallyInBattle, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_USER_MSG_RESP, SceneManager::OnBattleSendMsgThougthGame, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_USER_MSG_RESP_BY_UID, SceneManager::OnBattleSendMsgToRole, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_GUILD_HONOR_FIGHT_REQUEST_BACKUP, SceneManager::OnBattleGuildHonorFightRequestBackUp, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_GUILD_HONOR_ROLE_DIE_LIST, SceneManager::OnBattleGuildHonorFightRoleDieList, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_GUILD_HONOR_ROLE_STATE_CHANGE_NOTICE, SceneManager::OnBattleGameGuildHonorRoleStateChangeNotice, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_GUILD_HONOR_SYNC_ROUND_TOTAL_DAMAGE, SceneManager::OnBattleGameGuildHonorSyncRoundTotalDamage, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE, SceneManager::OnBattleRoleReinforce, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE_END, SceneManager::OnBattleRoleReinforceEnd, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_ROLE_SYNC_OB_STATUS, SceneManager::OnBattleRoleSyncOBStatus, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_CHECK_BATTLE_START_FAILED_ACK, SceneManager::OnBattleCheckFightStartFailed, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_START_FIGHT_FAILED, SceneManager::OnBattleStartFailed, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_GAME_BATTLE_SPECIAL_RESULT_NOTICE, SceneManager::OnSpecialBattleResultAck, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_FORCE_FINISH_BATTLE_MODE_RESP, SceneManager::OnBattleForceFinishBattleByModeResp, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_MODIFY_ANIMATION_SPEED_RESP, SceneManager::OnBattleModifyAnimationSpeedAck, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_SYNC_ROLE_STAT, SceneManager::OnBattleSyncWorldBossRoleStat, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_2_SYNC_ROLE_STAT, SceneManager::OnBattleSyncWorldBoss2RoleStat, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_WORLD_BOSS_3_SYNC_ROLE_STAT, SceneManager::OnBattleSyncWorldBoss3RoleStat, function_add_set);
	BATTLE_MESSAGE_HANDLER_REGISTER(MT_BATTLE_GAME_PERFORM_CHANGE_CARD_SKILL, SceneManager::OnBattlePerformChangeCardSkill, function_add_set);
}

SceneManager::SceneManager() : m_game_time(0), m_last_check_timeout(0), m_check_timeout_interval(10000), m_check_timeout(true),
	m_heartbeat_dead_time(100000)
{
	this->InitBattleGameHandler();
	this->InitCrossHandler();
}

void SceneManager::OnBattleStartAck(const char* data, int length)
{
	battlegameprotocol::BattleGameStartFightAck* msg = (battlegameprotocol::BattleGameStartFightAck*)data;
	
	static battlegameprotocol::BattleGameStartFightAck real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	BattleManagerLocal::GetInstance().OnFightStart(&real_msg);
}

void SceneManager::OnBattleResultAck(const char* data, int length)
{
	battlegameprotocol::BattleGameFightResultAck* msg = (battlegameprotocol::BattleGameFightResultAck*)data;
	static battlegameprotocol::BattleGameFightResultAck real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	BattleManagerLocal::GetInstance().OnFightFinish(&real_msg);
	EventHandler::Instance().OnFinishFight(&real_msg);
}

void SceneManager::OnBattleRoleRunAway(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleRunAway* msg = (battlegameprotocol::BattleGameRoleRunAway*)data;
	static battlegameprotocol::BattleGameRoleRunAway real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	BattleManagerLocal::GetInstance().OnRoleRunAway(&real_msg);
}

void SceneManager::OnBattleRoleSetAuto(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleSetAuto* msg = (battlegameprotocol::BattleGameRoleSetAuto*)data;

	BattleManagerLocal::GetInstance().OnRoleSetAuto(msg->role_id, msg->is_set_auto);
}

void SceneManager::OnBattleRoleSetAutoMove(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleSetAutoMove* msg = (battlegameprotocol::BattleGameRoleSetAutoMove*)data;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role)
	{
		return;
	}

	Pet* pet = role->GetRoleModuleManager()->GetPet();
	assert(NULL != pet);

	// 更新宠物的自动行为
	for (int i = 0; i < msg->pet_num && i < ROLE_BATTLE_PET_NUM; ++i)
	{
		int pet_index = -1;
		const PetParam* pet_param = pet->GetPetParam(msg->pet_id_list[i], msg->pet_unique_id_list[i], &pet_index);
		if (NULL == pet_param) continue;
		
		pet->ModifyAutoMove(pet_index, msg->pet_auto_move_type_list[i], msg->pet_auto_skill_id_list[i],
			msg->pet_auto_skill_level_list[i]);
	}

	// 更新玩家一动自动行为  
	if (BATTLE_MOVE_TYPE_AI_SKILL == msg->first_move_type || BATTLE_MOVE_TYPE_PERFORM_SKILL == msg->first_move_type) // 屏蔽同步普攻
	{
		role->SetBattleAutoMove(true, msg->first_move_type, msg->first_skill_id, msg->first_skill_level, true, false);

		role->GetRoleModuleManager()->GetPetHelperFormation()->SynBattleAutoSkill(true, msg->first_move_type, msg->first_skill_id, msg->first_skill_level, 0, 0);
		role->GetRoleModuleManager()->GetRoleFormation()->SynBattleAutoSkill(true, msg->first_move_type, msg->first_skill_id, msg->first_skill_level, 0, 0);
	}

	// 更新玩家二动自动行为
	if (0 == msg->second_move_pet_id)
	{
		role->SetBattleAutoMove(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, true, true);
	}
	else
	{			
		if (BATTLE_MOVE_TYPE_AI_SKILL == msg->second_move_type || BATTLE_MOVE_TYPE_PERFORM_SKILL == msg->second_move_type)
		{
			role->SetBattleAutoMove(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, true, true, true);

			role->GetRoleModuleManager()->GetPetHelperFormation()->SynBattleAutoSkill(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, msg->second_move_pet_id, msg->second_move_pet_unique_id);
			role->GetRoleModuleManager()->GetRoleFormation()->SynBattleAutoSkill(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, msg->second_move_pet_id, msg->second_move_pet_unique_id);
		}
		else if (BATTLE_MOVE_TYPE_NORMAL_ATTACK == msg->second_move_type && LOGIC_CONFIG->GetPetConfig()->IsNormalAttackPet(msg->second_move_pet_id))
		{
			role->SetBattleAutoMove(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, true, true, true);

			role->GetRoleModuleManager()->GetPetHelperFormation()->SynBattleAutoSkill(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, msg->second_move_pet_id, msg->second_move_pet_unique_id);
			role->GetRoleModuleManager()->GetRoleFormation()->SynBattleAutoSkill(false, msg->second_move_type, msg->second_skill_id, msg->second_skill_level, msg->second_move_pet_id, msg->second_move_pet_unique_id);
		}
	}
}

void SceneManager::OnBattleRoleOnlySyncPetAutoMove(const char* data, int length)
{
	battlegameprotocol::BattleGameOnlySyncPetAutoMove* msg = (battlegameprotocol::BattleGameOnlySyncPetAutoMove*)data;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role)
	{
		return;
	}

	for (int i = 0; i < msg->pet_num && i < ROLE_BATTLE_PET_NUM; ++i)
	{
		bool is_main_fight_pet = (0 == i);
		role->GetRoleModuleManager()->GetPet()->ModifySpecifyPetAutoMove(msg->pet_id_list[i],
			msg->pet_unique_id_list[i], msg->pet_auto_move_type_list[i], 
			msg->pet_auto_skill_id_list[i], msg->pet_auto_skill_level_list[i], is_main_fight_pet);
	}
}

void SceneManager::OnBattleRoleUseItem(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleUseItem* msg = (battlegameprotocol::BattleGameRoleUseItem*)data;

	BattleManagerLocal::GetInstance().OnRoleUseItem(msg->role_id, msg->item_column, msg->item_index, msg->item_id);
}

void SceneManager::OnBattleRoleSealPet(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleSealPet* msg = (battlegameprotocol::BattleGameRoleSealPet*)data;

	BattleManagerLocal::GetInstance().OnRoleSealPet(msg->role_id, msg->monster_id, msg->monster_group_id, msg->pet_id, msg->seal_card_item_id, msg->is_succ > 0, msg->is_sure_give_pet_on_succ, msg->is_auto_seal_pet > 0, msg->battle_mode, msg->is_bind_pet > 0);
}

void SceneManager::OnBattleRoleInstKillMonster(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleInstantKillMonster* msg = (battlegameprotocol::BattleGameRoleInstantKillMonster*)data;

	BattleManagerLocal::GetInstance().OnRoleInstKillMonster(msg->role_id, msg->monster_id, msg->instant_kill_item_id, msg->is_succ > 0);
}

void SceneManager::OnBattleInstanceReleaseNotice(const char* data, int length)
{
	battlegameprotocol::BattleGameInstanceReleaseNotice* msg = (battlegameprotocol::BattleGameInstanceReleaseNotice*)data;

	BattleManagerLocal::GetInstance().OnBattleInstanceReleaseNotice(msg->battle_id);
}

void SceneManager::OnBattleForceFinish(const char* data, int length)
{
	battlegameprotocol::BattleGameForceFinishFight* msg = (battlegameprotocol::BattleGameForceFinishFight*)data;
	static battlegameprotocol::BattleGameForceFinishFight real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	BattleManagerLocal::GetInstance().OnBattleForceFinish(&real_msg);
}

void SceneManager::OnBattleSyncWorldBossHp(const char* data, int length)
{
	battlegameprotocol::BattleGameSyncWorldBossHp* msg = (battlegameprotocol::BattleGameSyncWorldBossHp*)data;

	ActivityWorldBoss* act = static_cast<ActivityWorldBoss*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;
	
	act->SyncBossCurHp(msg->cur_hp, msg->round_modify_hp);
}

void SceneManager::OnBattleSyncWorldBossHp2(const char * data, int length)
{
	battlegameprotocol::BattleGameSyncWorldBoss2Hp* msg = (battlegameprotocol::BattleGameSyncWorldBoss2Hp*)data;

	ActivityWorldBoss2* act = static_cast<ActivityWorldBoss2*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;

	act->SyncBossCurHp(msg->cur_hp, msg->round_modify_hp);
}

void SceneManager::OnBattleSyncWorldBossHp3(const char * data, int length)
{
	battlegameprotocol::BattleGameSyncWorldBossHp* msg = (battlegameprotocol::BattleGameSyncWorldBossHp*)data;

	ActivityWorldBoss3* act = static_cast<ActivityWorldBoss3*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;

	act->SyncBossCurHp(msg->cur_hp, msg->round_modify_hp);
}

void SceneManager::OnBattleCheckRoleReallyInBattle(const char* data, int length)
{
	battlegameprotocol::BattleGameCheckRoleReallyInBattleResp* msg = (battlegameprotocol::BattleGameCheckRoleReallyInBattleResp*)data;

	BattleManagerLocal::GetInstance().OnCheckRoleReallyInBattle(msg->uid, msg->is_in_battle == 1, msg->battle_id, msg->battle_server_idx);
}

void SceneManager::OnBattleSendMsgThougthGame(const char* data, int length)
{
	battlegameprotocol::BattleGameUserMsgResp* msg = (battlegameprotocol::BattleGameUserMsgResp*)data;

	if (length - sizeof(battlegameprotocol::BattleGameUserMsgResp) > 0)
	{
		static GSNetID netid;
		netid.gateway_netid = World::GetInstWorld()->GetGateWayNetIdByIndex(msg->bg_netid.gateway_index);
		if ((NetID)-1 == netid.gateway_netid) return;
		
		netid.netid = msg->bg_netid.net_id;
		EngineAdapter::Instance().NetSend(netid, data + sizeof(battlegameprotocol::BattleGameUserMsgResp), 
			length - sizeof(battlegameprotocol::BattleGameUserMsgResp));
	}
}

void SceneManager::OnBattleSendMsgToRole(const char* data, int length)
{
	battlegameprotocol::BattleGameUserMsgRespByUid* msg = (battlegameprotocol::BattleGameUserMsgRespByUid*)data;

	int header_size = sizeof(battlegameprotocol::BattleGameUserMsgRespByUid);
	if (length - header_size > 0)
	{
		if (RobotManager::IsRobot(msg->uid))
		{
			return;
		}
		Role* role = this->GetRole(IntToUid(msg->uid));
		if (NULL != role)
		{
			EngineAdapter::Instance().NetSend(role->GetNetId(), data + header_size, length - header_size);
		}
		else
		{
			gamelog::g_log_battle.printf(LL_WARNING, "OnBattleSendMsgToRole role_not_online uid[%d]", msg->uid);
		}
	}
	else
	{
#ifdef _DEBUG
		assert(0);
#endif
	}
}

void SceneManager::OnBattleGuildHonorFightRequestBackUp(const char* data, int length)
{
	battlegameprotocol::BattleGameGuildHonorFightRequestBackup* req = (battlegameprotocol::BattleGameGuildHonorFightRequestBackup*)data;

	BattleManagerLocal::GetInstance().OnGuildHonorFightRequestBackup(req);
}

void SceneManager::OnBattleGuildHonorFightRoleDieList(const char* data, int length)
{
	battlegameprotocol::BattleGameGuildHonorRoleDieNotice* req = (battlegameprotocol::BattleGameGuildHonorRoleDieNotice*)data;

	BattleManagerLocal::GetInstance().OnGuildHonorFightRoleDieNotice(req);
}

void SceneManager::OnBattleGameGuildHonorRoleStateChangeNotice(const char* data, int length)
{
	battlegameprotocol::BattleGameGuildHonorRoleStateChangeNotice* req = (battlegameprotocol::BattleGameGuildHonorRoleStateChangeNotice*)data;

	BattleManagerLocal::GetInstance().OnGuildHonorFightRoleStateChangeNotice(req);
}

void SceneManager::OnBattleGameGuildHonorSyncRoundTotalDamage(const char* data, int length)
{
	battlegameprotocol::BattleGameGuildHonorSyncTotalDamage* req = (battlegameprotocol::BattleGameGuildHonorSyncTotalDamage*)data;

	BattleManagerLocal::GetInstance().OnGuildHonorFightSyncTotalDamage(req);
}

void SceneManager::OnBattleRoleReinforce(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleReinforceBattle* req = (battlegameprotocol::BattleGameRoleReinforceBattle*)data;

	BattleManagerLocal::GetInstance().OnRoleReinforceBattle(req->uid, req->battle_id);
}

void SceneManager::OnBattleRoleReinforceEnd(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleReinforceBattleEnd* req = (battlegameprotocol::BattleGameRoleReinforceBattleEnd*)data;

	BattleManagerLocal::GetInstance().OnRoleReinforceBattleEnd(req->uid, req->battle_id);
}

void SceneManager::OnBattleRoleSyncOBStatus(const char* data, int length)
{
	battlegameprotocol::BattleGameRoleSyncOBStatus* req = (battlegameprotocol::BattleGameRoleSyncOBStatus*)data;

	BattleManagerLocal::GetInstance().OnRoleSyncOBStatus(req);
}

void SceneManager::OnBattleStartFailed(const char* data, int length)
{
	battlegameprotocol::BattleGameStartFightFailed* req = (battlegameprotocol::BattleGameStartFightFailed*)data;

	BattleManagerLocal::GetInstance().OnFightStartFailed(req);
}

void SceneManager::OnBattleCheckFightStartFailed(const char* data, int length)
{
	battlegameprotocol::BattleGameCheckBattleHasFailAck* req = (battlegameprotocol::BattleGameCheckBattleHasFailAck*)data;
	
	BattleManagerLocal::GetInstance().OnCheckFightStartHasFailed(req);
}

void SceneManager::OnBattleForceFinishBattleByModeResp(const char* data, int length)
{
	battlegameprotocol::BattleGameForceFinishBattleModeResp* resp = (battlegameprotocol::BattleGameForceFinishBattleModeResp*)data;

	BattleManagerLocal::GetInstance().OnForceFinishBattleModeResp(resp);
}

void SceneManager::OnSpecialBattleResultAck(const char * data, int length)
{
	battlegameprotocol::BattleGameSpecialFightResultAck* msg = (battlegameprotocol::BattleGameSpecialFightResultAck*)data;
	static battlegameprotocol::BattleGameSpecialFightResultAck real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	BattleManagerLocal::GetInstance().OnFightSpecialFinish(&real_msg);
}

void SceneManager::OnBattleModifyAnimationSpeedAck(const char* data, int length)
{
	battlegameprotocol::BattleGameModifyAnimationSpeedResp* msg = (battlegameprotocol::BattleGameModifyAnimationSpeedResp*)data;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->uid));
	if (NULL != role)
	{
		if (BATTLE_MODE_SINGLE_ARENA == msg->battle_mode)
		{
			role->GetRoleModuleManager()->GetCommonData().single_arena_animation_speed = static_cast<char>(msg->speed_type);
		}
		else if (0 == msg->is_team)
		{
			role->GetRoleModuleManager()->GetCommonData().animation_speed = static_cast<char>(msg->speed_type);
		}
		else
		{
			role->GetRoleModuleManager()->GetCommonData().team_animation_speed = static_cast<char>(msg->speed_type);
		}

		Protocol::SCBattleAnimationSpeedModify resp;
		resp.speed_type = msg->speed_type;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &resp, sizeof(resp));
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(msg->teammate_uid_list); ++i)
	{
		int uid = msg->teammate_uid_list[i];
		if (uid <= 0) continue;

		Protocol::SCBattleAnimationSpeedModify resp;
		resp.speed_type = msg->speed_type;

		World::GetInstWorld()->GetSceneManager()->SendToUser(IntToUid(uid), &resp, sizeof(resp));
	}
}

void SceneManager::OnBattleSyncWorldBossRoleStat(const char* data, int length)
{
	battlegameprotocol::BattleGameWorldBossSyncRoleStat* msg = (battlegameprotocol::BattleGameWorldBossSyncRoleStat*)data;
	ActivityWorldBoss* act = static_cast<ActivityWorldBoss*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;

	for (int i = 0; i < msg->role_count && i < ARRAY_LENGTH(msg->delta_stat_list); ++i)
	{
		act->AddJoinRecord(msg->delta_stat_list[i].role_id, msg->delta_stat_list[i].delta_damage, msg->delta_stat_list[i].delta_heal);
	}
}

void SceneManager::OnBattleSyncWorldBoss2RoleStat(const char* data, int length)
{
	battlegameprotocol::BattleGameWorldBoss2SyncRoleStat* msg = (battlegameprotocol::BattleGameWorldBoss2SyncRoleStat*)data;
	ActivityWorldBoss2* act = static_cast<ActivityWorldBoss2*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;

	for (int i = 0; i < msg->role_count && i < ARRAY_LENGTH(msg->delta_stat_list); ++i)
	{
		act->AddJoinRecord(msg->delta_stat_list[i].role_id, msg->delta_stat_list[i].delta_damage, msg->delta_stat_list[i].delta_heal);
	}
}

void SceneManager::OnBattleSyncWorldBoss3RoleStat(const char * data, int length)
{
	battlegameprotocol::BattleGameWorldBoss3SyncRoleStat* msg = (battlegameprotocol::BattleGameWorldBoss3SyncRoleStat*)data;
	ActivityWorldBoss3* act = static_cast<ActivityWorldBoss3*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3));
	if (NULL == act) return;
	if (ACTIVITY_STATUS_OPEN != act->GetActivityStatus()) return;

	for (int i = 0; i < msg->role_count && i < ARRAY_LENGTH(msg->delta_stat_list); ++i)
	{
		act->AddJoinRecord(msg->delta_stat_list[i].role_id, msg->delta_stat_list[i].delta_damage, msg->delta_stat_list[i].delta_heal);
	}
}

void SceneManager::OnBattlePerformChangeCardSkill(const char * data, int length)
{
	battlegameprotocol::BattleGamePerformChangeCardSkill* msg = (battlegameprotocol::BattleGamePerformChangeCardSkill*)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		OfflineRegisterManager::Instance().OnSetLifeSkillChangeCard(msg->role_id, msg->change_card_seq);
	}
	else
	{
		role->GetRoleModuleManager()->GetRoleNewLifeSkill()->OnRoleBattlePerformSkill(msg->change_card_seq);
	}
	
}

void SceneManager::OnGameSendMailTransferToAnotherServer(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
#ifdef _DEBUG
		assert(0);
#endif
		return;
	}

	crossgameprotocal::GameHiddenSendMailTransferToAnotherServer* msg = (crossgameprotocal::GameHiddenSendMailTransferToAnotherServer *)data;

	crossgameprotocal::HiddenGameSendMailTransfer hgsmt;
	hgsmt.origin_plat_type = msg->origin_plat_type;
	hgsmt.origin_server_id = msg->origin_server_id;
	hgsmt.recieve_uid = msg->recieve_uid;
	memcpy(&hgsmt.mail_param, &msg->mail_param, sizeof(MailParam));

	InternalComm::Instance().SendToGameThroughCross(hgsmt.origin_plat_type, hgsmt.origin_server_id, (char *)&hgsmt, sizeof(hgsmt));
}

void SceneManager::OnGameQueryHiddenIfGuildSyncSucceed(const char* data, int length)
{
	crossgameprotocal::GameHiddenQueryIfSyncGuildSucceed* msg = (crossgameprotocal::GameHiddenQueryIfSyncGuildSucceed*)data;

	GuildManager::Instance().OnGameQueryHiddenIfSyncGuildSucceed(msg->plat_type, msg->server_id);
}

void SceneManager::OnHiddenNoticeGameSyncGuildSucc(const char* data, int length)
{
	GuildManager::Instance().OnHiddenNoticeGameSyncGuildSucc();
}

void SceneManager::OnGameHiddenSyncSpecialInfoReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == data)
	{
		return;
	}

	crossgameprotocal::GameHiddenSyncSpecialInfoReq * req = (crossgameprotocal::GameHiddenSyncSpecialInfoReq *)data;
	
	crossgameprotocal::HiddenGameSyncSpecialInfoRet hgssir;
	hgssir.cross_open_server_timestamp = SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime();
	hgssir.hidden_server_idx = CrossConfig::Instance().GetHiddenServerIDX();

	InternalComm::Instance().SendToGameThroughCross2(&hgssir, sizeof(hgssir), req->server_id);
}

void SceneManager::OnHiddenGameSyncSpecialInfoRet(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	crossgameprotocal::HiddenGameSyncSpecialInfoRet * ret = (crossgameprotocal::HiddenGameSyncSpecialInfoRet *)data;

	CrossManager::Instance().OnCrossOpenTamp(ret->cross_open_server_timestamp);
	CrossManager::Instance().OnSetHiidenServerIDX(ret->hidden_server_idx);
}

void SceneManager::OnCrossGameDataSycnCheckAck(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::CrossGameDataSyncCheckAck* msg = (crossgameprotocal::CrossGameDataSyncCheckAck*)data;
	DataSyncCheck::Instance().RemoveCheck(msg->type);
}

void SceneManager::OnGameHiddenSyncConsumeCarouselNewRecord(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenConsumeCarouselAddNewRecord* msg = (crossgameprotocal::GameHiddenConsumeCarouselAddNewRecord*)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(UniqueServerID(msg->plat_type, msg->server_id));
	if (NULL == ramgr) return;

	RandActivityConsumeCarousel* act = (RandActivityConsumeCarousel*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);
	if (NULL == act) return;

	RAShowInfoParam::RAShowInfoDataParam p;
	p.item_id = msg->item_id;
	p.num = msg->item_num;
	F_STRNCPY(p.role_name, msg->role_name, sizeof(p.role_name));

	act->OnAddShowListInfo(&p, true);
}

void SceneManager::OnHiddenGameSyncConsumeCarouselNewRecord(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameConsumeCarouselAddNewRecord* msg = (crossgameprotocal::HiddenGameConsumeCarouselAddNewRecord*)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityConsumeCarousel* act = (RandActivityConsumeCarousel*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);
	if (NULL == act) return;

	RAShowInfoParam::RAShowInfoDataParam p;
	p.item_id = msg->item_id;
	p.num = msg->item_num;
	F_STRNCPY(p.role_name, msg->role_name, sizeof(p.role_name));

	act->OnAddShowListInfo(&p, true);
}

void SceneManager::OnGameRoleLoginSendCrossChatGroupSimpleInfoReq(const char *data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenRoleLoginSendCrossChatGroupSimpleInfo* info = (crossgameprotocal::GameHiddenRoleLoginSendCrossChatGroupSimpleInfo*)data;

	CrossChatGroupManager::Instance().OnRoleLoginGameSendChatGroupSimpleInfo(info->uid, info->plat_type, info->server_id, ARRAY_LENGTH(info->cross_chat_group_list), info->cross_chat_group_list);
}

void SceneManager::OnHiddenGameRoleColosseumReqRouter(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameRoleColosseumReqRouter* req = (crossgameprotocal::HiddenGameRoleColosseumReqRouter*)data;

	switch (req->req_type)
	{
	case Protocol::CSColosseumReq::REQ_TYPE_FIHGT:
		{
			SCNoticeNum resp;
			resp.notice_num = errornum::EN_CROSS_NOT_ALLOW_COLOSSEUM_FIGHT;

			InternalComm::Instance().SendToHiddenUserThroughCross(req->uid, (const char*)&resp, sizeof(resp));
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_FETCH_EXP:
		{
			// nothing to do
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_GET_AWARD:
		{
			SCNoticeNum resp;
			resp.notice_num = errornum::EN_CROSS_NOT_ALLOW_COLOSSEUM_EVERY_REWARD;

			InternalComm::Instance().SendToHiddenUserThroughCross(req->uid, (const char*)&resp, sizeof(resp));
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_REFRESH_TARGET:
		{
			ColosseumManager::Instance().RoleRefreshTargetOnHidden(req->uid);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_ROLE_INFO:
		{
			ColosseumManager::Instance().SendRoleInfoToHidden(req->uid);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_TARGET_INFO:
		{
			ColosseumManager::Instance().SendTargetInfoToHidden(req->uid);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_TOP_INFO:
		{
			ColosseumManager::Instance().SendTopInfoToHidden(req->uid);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_RANK_INFO:
		{
			ColosseumManager::Instance().SendRankInfoToHidden(req->uid, req->param);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_REPORT_INFO:
		{
			ColosseumManager::Instance().SendReportInfoToHidden(req->uid);
		}
		break;

	case Protocol::CSColosseumReq::REQ_TYPE_GET_RANK_TOP_INFO:
		{
			ColosseumManager::Instance().SendRankTopSimpleInfoToHidden(req->uid);
		}
		break;
	}
}

void SceneManager::OnHiddenGameVideoSimpleInfoReq(const char* data, int length)
{
	crossgameprotocal::HiddenGameVideoSimpleInfoReq* req = (crossgameprotocal::HiddenGameVideoSimpleInfoReq*)data;

	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	long long file_id = BattleVideoMgr::Instance().GetVideoFileID(req->monster_group_id, (0 != req->is_server_first_kill));

	Protocol::SCBattleReportSimpleInfo resp;
	resp.monster_group_id = req->monster_group_id;
	resp.param1 = req->param1;
	resp.param2 = req->param2;
	resp.param3 = req->param3;
	resp.report_file_id = file_id;
	resp.is_server_first_kill = req->is_server_first_kill;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&resp, sizeof(resp), req->uuid);
}

void SceneManager::OnGameHiddenVideoSimpleInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenVideoSimpleInfoReq* req = (crossgameprotocal::GameHiddenVideoSimpleInfoReq*)data;

	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	long long file_id = BattleVideoMgr::Instance().GetVideoFileID(req->monster_group_id, (0 != req->is_server_first_kill));

	Protocol::SCBattleReportSimpleInfo resp;
	resp.monster_group_id = req->monster_group_id;
	resp.param1 = req->param1;
	resp.param2 = req->param2;
	resp.param3 = req->param3;
	resp.report_file_id = file_id;
	resp.is_server_first_kill = req->is_server_first_kill;

	InternalComm::Instance().SendToGameUserThroughCross(req->uid,(const char*)&resp, sizeof(resp));
}

void SceneManager::OnHiddenGameVideoDetailInfoReq(const char* data, int length)
{
	crossgameprotocal::HiddenGameVideoDetailInfoReq* req = (crossgameprotocal::HiddenGameVideoDetailInfoReq*)data;

	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	const BattleVideoInfo* info = BattleVideoMgr::Instance().GetVideoInfoConst(req->monster_group_id, (0 != req->is_server_first_kill));
	if (NULL == info || !info->IsValid())
	{
		return;
	}

	if (NULL == info->video_buffer)
	{
		return;
	}

	static crossgameprotocal::GameHiddenVideoDetailInfoResp resp;
	resp.type = req->type;
	resp.uuid = req->uuid;
	resp.video_length = info->video_len;
	resp.info_param = info->param;
	if (info->video_len >= sizeof(resp.video_buffer))
	{
		return;
	}
	memcpy(resp.video_buffer, info->video_buffer, info->video_len);

	InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&resp);
}

void SceneManager::OnGameHiddenVideoDetailInfoResp(const char* data, int length)
{
	static crossgameprotocal::GameHiddenVideoDetailInfoResp resp;
	int offset = 0;
	if (!resp.Deserialize(data, length, &offset))
	{
		return;
	}

	switch (resp.type)
	{
		case crossgameprotocal::VIDEO_DETAIL_INFO_REQ_TYPE_WATCH:
		{
			UserID user_id = LongLongToUniqueUserID(resp.uuid).user_id;
			Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
			if (NULL == role) return;

			Team* team = role->GetMyTeam(false);
			if (NULL == team)
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), resp.video_buffer, resp.video_length);
			}
			else
			{
				if (team->GetLeaderUID() == role->GetUID())
				{
					team->SendToFollowMember(resp.video_buffer, resp.video_length);
				}
				else
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				}
			}
		}
		break;
		case crossgameprotocal::VIDEO_DETAIL_INFO_REQ_TYPE_SYN:
		{
			BattleVideoMgr::Instance().GameHiddenSynVideo(resp.info_param, resp.video_length, resp.video_buffer);
		}
		break;
		default:
		break;
	}
}

void SceneManager::OnGameHiddenVideoDetailInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenVideoDetailInfoReq* req = (crossgameprotocal::GameHiddenVideoDetailInfoReq*)data;

	const VideoCfg* cfg = LOGIC_CONFIG->GetBattleVideoConfig()->GetVideoCfg(req->monster_group_id);
	if (NULL == cfg)
	{
		return;
	}

	const BattleVideoInfo* info = BattleVideoMgr::Instance().GetVideoInfoConst(req->monster_group_id, (0 != req->is_server_first_kill));
	if (NULL == info || !info->IsValid())
	{
		return;
	}

	if (NULL == info->video_buffer)
	{
		return;
	}

	static crossgameprotocal::HiddenGameVideoDetailInfoResp resp;
	resp.uid = req->uid;
	resp.video_length = info->video_len;
	resp.monster_group_id = req->monster_group_id;
	resp.is_server_first_kill = req->is_server_first_kill;
	if (info->video_len >= sizeof(resp.video_buffer))
	{
		return;
	}
	memcpy(resp.video_buffer, info->video_buffer, info->video_len);

	InternalComm::Instance().SendSerializeMsgToGameThroughCross(&resp, IntToUid(resp.uid).db_index);
}

void SceneManager::OnHiddenGameVideoDetailInfoResp(const char * data, int length)
{
	static crossgameprotocal::HiddenGameVideoDetailInfoResp resp;
	int offset = 0;
	if (!resp.Deserialize(data, length, &offset))
	{
		return;
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(resp.uid);
	if (NULL == role) return;

	Team* team = role->GetMyTeam(false);
	if (NULL == team)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), resp.video_buffer, resp.video_length);
	}
	else
	{
		if (team->GetLeaderUID() == role->GetUID())
		{
			team->SendToFollowMember(resp.video_buffer, resp.video_length);
		}
		else
		{
			role->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		}
	}
}

void SceneManager::OnHiddenGameGuildEventhandlerSend(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildEventhandlerSend* hgges = (crossgameprotocal::HiddenGameGuildEventhandlerSend *)data;

	Guild * guild = GuildManager::Instance().GetGuild(hgges->guild_id);
	if (NULL != guild)
	{
		guild->AddEventhandler(&hgges->eventhandler_node);
	}
}

void SceneManager::OnGameHiddenChatGroupSynHandlerSend(const char * data, int length)
{
	crossgameprotocal::GameCrossSyncCrossChatGroupInfo * msg = (crossgameprotocal::GameCrossSyncCrossChatGroupInfo *)data;

	CrossChatGroupManager::Instance().OnGameServerInLine(msg->from_usid.server_id,msg->count, msg->chat_group_list);
}

void SceneManager::OnGameHiddenChatGroupSynHandlerSendRet(const char * data, int length)
{
	crossgameprotocal::CrossGameSyncCrossChatGroupInfo * msg = (crossgameprotocal::CrossGameSyncCrossChatGroupInfo *)data;
	ChatManager::Instance().OnHidenServerBack(msg->count, msg->chat_group_list);
}

void SceneManager::OnHiddenGameChatGroupSynHandlerSend(const char * data, int length)
{
	crossgameprotocal::HiddenGameSyncCrossChatGroupInfo * msg = (crossgameprotocal::HiddenGameSyncCrossChatGroupInfo *)data;

	ChatManager::Instance().OnCrossSynGame(&msg->chat_group_list, msg->user_id);
}

void SceneManager::OnHiddenGameOnlyFightSynInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameSynOnlyFightInfo * msg = (crossgameprotocal::HiddenGameSynOnlyFightInfo *)data;

	WorldStatus::Instance().HiddenGameOnlyFightDataSyn(&msg->param);
}

void SceneManager::OnGameHiddenOnlyFightSynInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenSynOnlyFightInfoReq * msg = (crossgameprotocal::GameHiddenSynOnlyFightInfoReq *)data;

	WorldStatus::Instance().GameHiddenOnlyFightDataSynReq(msg->server_id);


	//验证用回执
	crossgameprotocal::CrossGameDataSyncCheckAck ack;
	ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_ACTIVITY;
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), msg->server_id, (const char*)&ack, sizeof(ack));
}

void SceneManager::OnGameHiddenReqPlatformBattleRoleInfo(const char * data, int length)
{
	crossgameprotocal::GameHiddenPlatformBattleReq * msg = (crossgameprotocal::GameHiddenPlatformBattleReq *)data;
	switch (msg->type)
	{
		case crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_PLATFORM_INFO:
			{
				PlatformBattle::Instance().SendPlatformInfo(NULL, msg->param3, msg->param2);
			}
			break;

		case crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO:
			{
				PlatformBattle::Instance().SyncRoleInfoHiddenToGameworld(msg->param1);
			}
			break;
		case crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_REMOVE:
			{
				PlatformBattle::Instance().RemovePlatformPlayInfoByRoleId(msg->param1, msg->param2, STRING_SPRINTF("%s is_req_client:%d", __FUNCTION__, msg->param3).c_str());
			}
			break;
		case crossgameprotocal::GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN:
			{
				switch (msg->param1)
				{
					case Protocol::PLATFORM_BATTLE_REQ_TYPE_RECORD:		
						{
							PlatformBattle::Instance().SendRecord(NULL, msg->param2);
						}
						break;
					case Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE:
					case Protocol::PLATFORM_BATTLE_REQ_TYPE_SET_NOTICE_2:
						{
							PlatformBattle::Instance().SetClientNotice(msg->param2, msg->param1, msg->param3);
						}
						break;
					case Protocol::PLATFORM_BATTLE_REQ_TYPE_LAST_RANK_INFO:
						{
							PlatformBattle::Instance().SendLastRankInfo(NULL, msg->param2);
						}
						break;
				}				
			}
			break;

	}	
}

void SceneManager::OnHiddenGameRetPlatformBattleRoleInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameRetPlatformBattleRoleInfo msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	PlatformBattle::Instance().SetRoleInfo(msg.type, msg.all_role_info);
}

void SceneManager::OnHiddenGameSyncLogQuick(const char* data, int length)
{
	crossgameprotocal::HiddenGameSyncLogQuick* msg = (crossgameprotocal::HiddenGameSyncLogQuick*)data;

	static crossgameprotocal::HiddenGameSyncLogQuick real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset))
	{
		return;
	}

	LOG_QUICK12(gamelog::g_log_quick, real_msg.n1, real_msg.n2, 
		real_msg.z1.c_str(), real_msg.z2.c_str(), real_msg.n3, 
		real_msg.n4, real_msg.z3.c_str(), real_msg.z4.c_str(), 
		real_msg.n5, real_msg.n6, real_msg.n7, real_msg.n8, real_msg.n9);
}

void SceneManager::OnHiddenGameSyncLogToOriginServer(const char* data, int length)
{
	crossgameprotocal::HiddenGameSyncLogToOriginServer* msg = (crossgameprotocal::HiddenGameSyncLogToOriginServer*)data;
	if (length != 
		sizeof(crossgameprotocal::HiddenGameSyncLogToOriginServer) 
		- sizeof(msg->buffer) 
		+ msg->length)
	{
		return;
	}

	if (msg->length <= 0 || msg->length >= crossgameprotocal::SYNC_LOG_BUFFER_SIZE)
	{
		return;
	}

	char buffer[crossgameprotocal::SYNC_LOG_BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	F_STRNCPY(buffer, msg->buffer, sizeof(buffer));

	LogAgent* log_agent = NULL;
	switch (msg->log_type)
	{
	case SYNC_LOG_TYPE_MONEY_GOLD:
		{
			log_agent = &gamelog::g_log_moneygold;
		}
		break;

	case SYNC_LOG_TYPE_MONEY_COIN:
		{
			log_agent = &gamelog::g_log_moneycoin;
		}
		break;

	case SYNC_LOG_TYPE_TASK:
		{
			log_agent = &gamelog::g_log_task;
		}
		break;

	case SYNC_LOG_TYPE_TASK_STAT:
		{
			log_agent = &gamelog::g_log_task_stats;
		}
		break;
	case SYNC_LOG_TYPE_SCORE_TASK:
		{
			log_agent = &gamelog::g_log_score_task;
		}
		break;

	default:
		{
			//nothing to do
		}
		break;
	}

	if (NULL == log_agent)
	{
		return;
	}

	log_agent->print(LL_INFO, buffer);
}

void SceneManager::OnHiddenGameGuildBossChapterRankReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameBossChapterRankReq * req = (crossgameprotocal::HiddenGameBossChapterRankReq * )data;

	GuildManager::Instance().HiddenGameBossChapterRankReq(req->uuid);
}

void SceneManager::OnGameHiddenSyncPublicNotice(const char* data, int length)
{
	crossgameprotocal::GameHiddenSyncPublicNotice notice;
	int offset = 0;
	if (!notice.Deserialize(data, length, &offset))
	{
		return;
	}

	bool normal_notice_is_empty = System::Instance().NormalNoticeIsEmpty(UniqueServerID(notice.plat_type, notice.server_id));

	for (int i = 0; i < notice.count && i < ARRAY_ITEM_COUNT(notice.notice_list); ++i)
	{
		if (notice.notice_list[i].urgent)
		{
			const char *notice_str = notice.notice_list[i].notice.c_str();
			int notice_len = static_cast<int>(notice.notice_list[i].notice.length());
			UniqueServerID usid(notice.plat_type, notice.server_id);
			this->SystemMsgFilter(notice_str, notice_len, GetMsgType(notice.notice_list[i].type), 
				notice.notice_list[i].color, 0, 0, 0, UniqueServerFilter, &usid);
		}	
		else if (normal_notice_is_empty)
		{
			System::Instance().AddNormalNotice(UniqueServerID(notice.plat_type, notice.server_id), notice.notice_list[i]);
		}
	}
}

void SceneManager::OnGameHiddenSyncOnlineInfo(const char* data, int length)
{
	crossgameprotocal::GameHiddenSyncOnlineInfo* msg = (crossgameprotocal::GameHiddenSyncOnlineInfo*)data;

	SystemMonitor::Instance().OnSyncOnlineInfoFromGame(msg->plat_type, msg->server_id, msg->online_role_num, msg->incross_role_num, msg->crossing_role_num);
}

void SceneManager::OnHiddenGameInviteJoinTeam(const char* data, int length)
{
	crossgameprotocal::HiddenGameInviteJoinTeam* msg = (crossgameprotocal::HiddenGameInviteJoinTeam*)data;

	msg->inviter_info.name[sizeof(GameName) - 1] = 0;

	TeamManager::Instance().OnInvitationReqFromHidden(msg->inviter_info, msg->team_item, msg->team_type, msg->cross_team_index, msg->team_member_num, msg->target_uid, msg->team_level_limit_low, msg->team_level_limit_high,msg->invite_type);
}

void SceneManager::OnGameHiddenInviteJoinTeamInOrigin(const char* data, int length)
{
	crossgameprotocal::GameHiddenInviteJoinTeamInOrigin* msg = (crossgameprotocal::GameHiddenInviteJoinTeamInOrigin*)data;

	msg->inviter_info.name[sizeof(GameName) - 1] = 0;

	TeamManager::Instance().OnInvitationReqFromOrigin(msg->inviter_info, msg->team_item, msg->team_type, msg->team_index, msg->team_member_num, msg->target_uid, msg->team_level_limit_low, msg->team_level_limit_high, msg->invite_type);
}

void SceneManager::OnGameHiddenReqJoinTeamInfo(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenReqJoinTeamInfo * req = (crossgameprotocal::GameHiddenReqJoinTeamInfo *)data;
	Team * user_team = TeamManager::Instance().GetTeamByTeamIndex(req->cross_team_index);
	
	crossgameprotocal::HiddenGameJoinTeamInfoRet invitation;
	invitation.is_exist = 0;
	invitation.cross_team_index = req->cross_team_index;
	invitation.target_uid = req->target_uid;

	if (user_team != NULL && user_team->IsMember(req->inviter_uid))
	{
		invitation.is_exist = 1;
		invitation.team_type = user_team->GetTeamType();
		invitation.team_member_num = user_team->GetMemberCount();
		invitation.inviter_info = user_team->GetMsgRoleBaseInfo(req->inviter_uid);		
		user_team->GetTeamItemInfo(&invitation.team_item);		
	}

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(req->target_uid).db_index, (const char *)&invitation, sizeof(invitation));
}

void SceneManager::OnHiddenGameJoinTeamInfoRet(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameJoinTeamInfoRet * msg = (crossgameprotocal::HiddenGameJoinTeamInfoRet *)data;
	Role * target_role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->target_uid);
	if (NULL == target_role)
	{
		return;
	}

	//  邀请对方	
	Protocol::SCTeamInvitationNotice notice;
	notice.notice_type = msg->is_exist ? Protocol::TEAM_INVITATION_NOTICE_TYPE_UPDATE : Protocol::TEAM_INVITATION_NOTICE_TYPE_NOT_EXIST;
	notice.is_from_cross = 1;
	notice.xia_yi_zhi = target_role->GetRoleModuleManager()->GetSalary()->CalculateChivalrousByTeamType(msg->team_type, msg->team_item);
	notice.team_index = msg->cross_team_index;
	notice.team_type = msg->team_type;
	notice.team_member_num = msg->team_member_num;
	notice.inviter_info = msg->inviter_info;
	EngineAdapter::Instance().NetSend(target_role->GetNetId(), (const char *)&notice, sizeof(notice));
}

void SceneManager::OnGameHiddenInviteJoinTeamRet(const char* data, int length)
{
	crossgameprotocal::GameHiddenInvationJoinTeamRet* msg = (crossgameprotocal::GameHiddenInvationJoinTeamRet*)data;

	gamelog::g_log_debug.printf(LL_DEBUG, "OnGameHiddenInviteJoinTeamRet inviter[%d] result[%d] notice_num[%d]",
		msg->inviter_uid, msg->result, msg->notice_num);

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->inviter_uid);
	if (NULL == role) return;

	role->NoticeNum(msg->notice_num);
}

void SceneManager::OnHiddenGameInviteJoinTeamInOriginRet(const char* data, int length)
{
	crossgameprotocal::HiddenGameInviteJoinTeamInOriginRet* msg = (crossgameprotocal::HiddenGameInviteJoinTeamInOriginRet*)data;

	gamelog::g_log_debug.printf(LL_DEBUG, "OnHiddenGameInviteJoinTeamInOriginRet inviter[%d] result[%d] notice_num[%d]",
		msg->inviter_uid, msg->result, msg->notice_num);

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->inviter_uid);
	if (NULL == role) return;

	role->NoticeNum(msg->notice_num);
}

void SceneManager::OnHiddenGameGuildGatherDreamPublishListReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamPublishListReq* msg = (crossgameprotocal::HiddenGameGuildGatherDreamPublishListReq *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->SendGatherDreamListToCross(msg->uid);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamRecordListReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamRecordListReq * msg = (crossgameprotocal::HiddenGameGuildGatherDreamRecordListReq *)data;
	GatherDreamRecordManager::Instance().SendRecordListInfoToCross(msg->uid, msg->type);
}

void SceneManager::OnHiddenGameGuildGatherDreamPublishReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamPublishReq * msg = (crossgameprotocal::HiddenGameGuildGatherDreamPublishReq *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGamePublishGatherDream(msg);
	}
}

void SceneManager::OnGameHiddenGuildGatherDreamPublishRet(const char * data, int length)
{
	crossgameprotocal::GameHiddenGuildGatherDreamPublishRet * msg = (crossgameprotocal::GameHiddenGuildGatherDreamPublishRet *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{	
		guild->OnGameHiddenPublishGatherDream(msg);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamGiftReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamGiftReq * msg = (crossgameprotocal::HiddenGameGuildGatherDreamGiftReq *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGameGiftGatherDream(msg);
	}
}

void SceneManager::OnGameHiddenGuildGatherDreamGiftRet(const char * data, int length)
{
	crossgameprotocal::GameHiddenGuildGatherDreamGiftRet * msg = (crossgameprotocal::GameHiddenGuildGatherDreamGiftRet *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->info.guild_id);
	if (NULL != guild)
	{
		guild->OnGameHiddenGiftGatherDream(msg);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamGiftAddRecord(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord * msg = (crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->info.guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGameGiftGatherDreamAddRecord(msg);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamFetchReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamFetchReq * msg = (crossgameprotocal::HiddenGameGuildGatherDreamFetchReq *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGameFetchGatherDream(msg->uid);
	}
}

void SceneManager::OnGameHiddenGuildGatherDreamFetchRet(const char * data, int length)
{
	crossgameprotocal::GameHiddenGuildGatherDreamFetchRet * msg = (crossgameprotocal::GameHiddenGuildGatherDreamFetchRet *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnGameHiddenFetchGatherDream(msg);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamFetchChange(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamFetchChange * msg = (crossgameprotocal::HiddenGameGuildGatherDreamFetchChange *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGameFetchGatherDreamChange(msg);
	}
}

void SceneManager::OnGameHiddenSystemMailSend(const char * data, int length)
{
	crossgameprotocal::GameHiddenSendSystemMail* msg = (crossgameprotocal::GameHiddenSendSystemMail*)data;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->send_to_uid);
	if (NULL != role)
	{
		if (role->GetRoleModuleManager()->GetRoleMail()->AddMail(msg->mail_param, true))
		{
			gamelog::g_log_mail.printf(LL_INFO, "OnGameHiddenSystemMailSend add rolemail succ, serial[%s] recver[%d], mail_param[%s]",
				msg->mail_param.mail_serial, msg->send_to_uid, MailParamLog(&msg->mail_param));
		}
		else
		{
			gamelog::g_log_mail.printf(LL_INFO, "OnGameHiddenSystemMailSend add rolemail fail, serial[%s] recver[%d], mail_param[%s]",
				msg->mail_param.mail_serial, msg->send_to_uid, MailParamLog(&msg->mail_param));
		}
	}
	else
	{
		crossgameprotocal::HiddenGameSendSystemMail msgmail;
		msgmail.send_to_uid = msg->send_to_uid;
		msgmail.mail_param = msg->mail_param;
		InternalComm::Instance().SendToGameThroughCross(msg->plat_type, msg->server_id, (const char*)&msgmail, sizeof(msgmail));
	}
}

void SceneManager::OnHiddenGameSystemMailSend(const char * data, int length)
{
	crossgameprotocal::HiddenGameSendSystemMail* msg = (crossgameprotocal::HiddenGameSendSystemMail*)data;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->send_to_uid));
	if (NULL != role)
	{
		if (role->GetRoleModuleManager()->GetRoleMail()->AddMail(msg->mail_param, true))
		{
			gamelog::g_log_mail.printf(LL_INFO, "OnHiddenGameSystemMailSend add rolemail succ, serial[%s] recver[%d], mail_param[%s]",
				msg->mail_param.mail_serial, msg->send_to_uid, MailParamLog(&msg->mail_param));
		}
		else
		{
			gamelog::g_log_mail.printf(LL_INFO, "OnHiddenGameSystemMailSend add rolemail fail, serial[%s] recver[%d], mail_param[%s]",
				msg->mail_param.mail_serial, msg->send_to_uid, MailParamLog(&msg->mail_param));
		}
	}
	else
	{
		UserMailManager::Instance().SyncMailFromHidden(IntToUid(msg->send_to_uid), msg->mail_param);
	}
}

void SceneManager::OnHiddenGameSyncHuanJieRoleInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameSyncHuanJieRoleInfo * msg = (crossgameprotocal::HiddenGameSyncHuanJieRoleInfo *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_uid);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetHuaShenManager()->SetRoleInfo(msg->rank_score, msg->is_finish_ding_ji);
	}
	else
	{
		OfflineRegisterManager::Instance().OnHuanJieRankScoreChange(IntToUid(msg->role_uid), msg->rank_score, msg->is_finish_ding_ji);
	}
}

void SceneManager::OnHiddenGameGuildGatherDreamRoleInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameGuildGatherDreamRoleChange * msg = (crossgameprotocal::HiddenGameGuildGatherDreamRoleChange *)data;
	GatherDreamRecordManager::Instance().UpdateDataInfo(msg->role_info, msg->role_uid, msg->guild_id);
}

void SceneManager::OnHiddenGameHuanJieSyncTianTiRecord(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameHuanJieSyncTianTiRecord * msg = (crossgameprotocal::HiddenGameHuanJieSyncTianTiRecord *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetRoleHuanJie()->AddTianTiRecord(msg->record_data);

		crossgameprotocal::GameHiddenSyncSingleRecordAck ack;
		ack.role_id = msg->role_id;
		ack.record_id = msg->record_id;

		InternalComm::Instance().SendToHiddenThroughCross((char *)&ack, sizeof(ack));
	}
}

void SceneManager::OnHiddenGameHuanJieSyncTianTiRecordAck(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenSyncSingleRecordAck * msg = (crossgameprotocal::GameHiddenSyncSingleRecordAck *)data;
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity == NULL)
	{
		return;
	}

	activity->SyncSinleRecordAck(msg->role_id, msg->record_id);
}

void SceneManager::OnHiddenGameSyncGuildChat(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameGuildChatSync* sync = (crossgameprotocal::HiddenGameGuildChatSync*)data;
	if (length != sizeof(crossgameprotocal::HiddenGameGuildChatSync) - sizeof(sync->chat_msg) + sync->chat_msg_len)
	{
		return;
	}

	if (sizeof(sync->chat_msg) <= sync->chat_msg_len)
	{
		return;
	}

	Guild* guild = GuildManager::Instance().GetGuild(sync->guild_id);
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManager()->OnChat(sync->chat_msg, sync->chat_msg_len, false); // need sync 必须填false 否则无限同步下去
}

void SceneManager::OnGameHiddenSyncGuildChat(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenGuildChatSync* sync = (crossgameprotocal::GameHiddenGuildChatSync*)data;
	if (length != sizeof(crossgameprotocal::GameHiddenGuildChatSync) - sizeof(sync->chat_msg) + sync->chat_msg_len)
	{
		return;
	}

	if (sizeof(sync->chat_msg) <= sync->chat_msg_len)
	{
		return;
	}

	Guild* guild = GuildManager::Instance().GetGuild(sync->guild_id);
	if (NULL == guild)
	{
		return;
	}

	guild->GetMemberManager()->OnChat(sync->chat_msg, sync->chat_msg_len, false); // need sync 必须填false 否则无限同步下去
}

void SceneManager::OnGameHiddenNoticeSensitiveWordsReload(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenSensitiveWordsReload* req = (crossgameprotocal::GameHiddenSensitiveWordsReload*)data;
	
	gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenNoticeSensitiveWordsReload plat_type[%d] server_id[%d]", req->plat_type, req->server_id);

	UniqueServerID target_usid(req->plat_type, req->server_id);
	
	Protocol::SCNoticeClientReloadSensitiveWords resp;

	static GSNetID gsnetid[256];
	int rolenum = 0;

	std::vector<int> online_user_list;
	UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
	for (int i = 0; i < (int)online_user_list.size(); i++)
	{
		Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
		if (NULL == role) continue;

		RoleCross* rolecross = role->GetRoleModuleManager()->GetRoleCross();
		UniqueServerID usid = UniqueServerID(rolecross->GetOriginPlatType(), rolecross->GetOriginServerId());
		if (usid != target_usid)
		{
			continue;
		}

		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenNoticeSensitiveWordsReload send to role[%d,%s]", role->GetUID(), role->GetName());

		gsnetid[rolenum++] = role->GetNetId();
		if (rolenum >= ARRAY_ITEM_COUNT(gsnetid))
		{
			EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, (const char*)&resp, sizeof(resp));
			rolenum = 0;
		}
	}

	if (rolenum > 0)
	{
		EngineAdapter::Instance().NetMulSend(gsnetid, rolenum, (const char*)&resp, sizeof(resp));
	}
}

void SceneManager::OnGameHiddenCmdKickRole(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenCmdKickRole* req = (crossgameprotocal::GameHiddenCmdKickRole*)data;
	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdKickRole plat_type[%d] server_id[%d] role[%d,%s]", req->plat_type, req->server_id, role->GetUID(), role->GetName());

		EngineAdapter::Instance().NetDisconnect(role->GetNetId(), role->GetUserId(), role->GetName(), 0, "KickOut");
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdKickRole Role Not Found, plat_type[%d] server_id[%d] role[%d]", req->plat_type, req->server_id, req->role_id);
	}
}

void SceneManager::OnGameHiddenCmdMuteRole(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenCmdMuteRole* req = (crossgameprotocal::GameHiddenCmdMuteRole*)data;
	
	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdMuteRole plat_type[%d] server_id[%d] role[%d,%s] mute_second[%d]", req->plat_type, req->server_id, role->GetUID(), role->GetName(), req->mute_second);

		role->ForbidTalk((time_t)req->mute_second, "Command", "Command", req->forbid_talk_type);
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdMuteRole Role Not Found, Send To OriginGameWorld, plat_type[%d] server_id[%d] role[%d] mute_second[%d]", req->plat_type, req->server_id, req->role_id, req->mute_second);
	
		crossgameprotocal::HiddenGameCmdMuteRole hgcmr;
		hgcmr.role_id = req->role_id;
		hgcmr.mute_second = req->mute_second;

		InternalComm::Instance().SendToGameThroughCross(req->plat_type, req->server_id, (const char*)&hgcmr, sizeof(hgcmr));
	}
}

void SceneManager::OnHiddenGameCmdMuteRole(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameCmdMuteRole* req = (crossgameprotocal::HiddenGameCmdMuteRole*)data;

	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnHiddenGameCmdMuteRole role[%d,%s] mute_second[%d]", role->GetUID(), role->GetName(), req->mute_second);

		role->ForbidTalk((time_t)req->mute_second, "Command", "Command", req->forbid_talk_type);
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnHiddenGameCmdMuteRole role_id[%d] mute_second[%d]", req->role_id, req->mute_second);

		RMIRoleMuteBackObjectImpl* impl = new RMIRoleMuteBackObjectImpl();
		impl->role_id = req->role_id;

		const rmi::Session& session = InternalComm::Instance().GetDBHandler().GetRoleDB();
		RMIRoleClient rc;
		rc.__bind_session(session);
		if (!rc.RoleMuteAsyn(req->role_id, req->mute_second, req->forbid_talk_type, impl))
		{
			OfflineRegisterManager::Instance().OnForbidTalk(IntToUid(req->role_id), req->mute_second, req->forbid_talk_type);
		}
	}
}

void SceneManager::OnGameHiddenCmdNoticeNewMessage(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenCmdNoticeNewMessage* req = (crossgameprotocal::GameHiddenCmdNoticeNewMessage*)data;

	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdNoticeNewMessage plat_type[%d] server_id[%d] role[%d,%s] msg_count[%d]", req->plat_type, req->server_id, role->GetUID(), role->GetName(), req->msg_count);

		Protocol::SCNoticeClientNewMessageFromCustomService notice;
		notice.msg_count = req->msg_count;

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdNoticeNewMessage Role Not Found, Send To OriginGameWorld, plat_type[%d] server_id[%d] role[%d] msg_count[%d]", req->plat_type, req->server_id, req->role_id, req->msg_count);

		crossgameprotocal::HiddenGameCmdNoticeNewMessage hgcnnm;
		hgcnnm.role_id = req->role_id;
		hgcnnm.msg_count = req->msg_count;

		InternalComm::Instance().SendToGameThroughCross(req->plat_type, req->server_id, (const char*)&hgcnnm, sizeof(hgcnnm));
	}
}

void SceneManager::OnHiddenGameCmdNoticeNewMessage(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameCmdNoticeNewMessage* req = (crossgameprotocal::HiddenGameCmdNoticeNewMessage*)data;

	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnHiddenGameCmdNoticeNewMessage role[%d,%s] msg_count[%d]", role->GetUID(), role->GetName(), req->msg_count);

		Protocol::SCNoticeClientNewMessageFromCustomService notice;
		notice.msg_count = req->msg_count;

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&notice, sizeof(notice));
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnHiddenGameCmdNoticeNewMessage Role Not Online role_id[%d] msg_count[%d]", req->role_id, req->msg_count);
	}
}

void SceneManager::OnGameHiddenRATianMing(const char * data, int length)
{
	if (data == NULL)return;
	crossgameprotocal::GameCrossSyncRATianMing * msg = (crossgameprotocal::GameCrossSyncRATianMing *)data;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(msg->info_from_server_id);

	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION))
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
		if (NULL != activity)
		{
			RandActivityTianMingDivination * act_info = (RandActivityTianMingDivination *)activity;
			act_info->OnItemAdd(&msg->info);
		}
	}
}

void SceneManager::OnHiddenGameRATianMing(const char * data, int length)
{
	if (data == NULL)return;
	crossgameprotocal::GameCrossSyncRATianMing * msg = (crossgameprotocal::GameCrossSyncRATianMing *)data;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(msg->info_from_server_id);

	if (NULL != ramgr && ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION))
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
		if (NULL != activity)
		{
			RandActivityTianMingDivination * act_info = (RandActivityTianMingDivination *)activity;
			act_info->OnItemAdd(&msg->info);
		}
	}
}

void SceneManager::OnGameHiddenCmdResetPublicNotice(const char* data, int length)
{
	crossgameprotocal::GameHiddenCmdResetPublicNotice* msg = (crossgameprotocal::GameHiddenCmdResetPublicNotice*)data;

	UniqueServerID usid(msg->plat_type, msg->server_id);
	System::Instance().ResetNotice(usid);
}

void SceneManager::OnHiddenGameSyncPlatInCrossRoleNum(const char* data, int length)
{
	crossgameprotocal::HiddenGameSyncPlatInCrossRoleNum* msg = (crossgameprotocal::HiddenGameSyncPlatInCrossRoleNum*)data;

	SystemMonitor::Instance().OnSyncPlatInCrossNumFromHidden(msg->sub_plat_type, msg->incross_role_num);
}

void SceneManager::OnHiddenGameWorldBossSyncRankResultMsg(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldBossSyncRankResultMsg* msg = (crossgameprotocal::HiddenGameWorldBossSyncRankResultMsg*)data;

	if (msg->length != sizeof(Protocol::SCWorldBossResult))
	{
		return;
	}

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS);
	if (NULL != activity)
	{
		ActivityWorldBoss* awb = (ActivityWorldBoss*)activity;
		awb->HiddenGameSendRankResult(msg);
	}
}

void SceneManager::OnHiddenGameWorldBoss2SyncRankResultMsg(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldBoss2SyncRankResultMsg* msg = (crossgameprotocal::HiddenGameWorldBoss2SyncRankResultMsg*)data;

	if (msg->length != sizeof(Protocol::SCWorldBoss2Result))
	{
		return;
	}

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2);
	if (NULL != activity)
	{
		ActivityWorldBoss2* awb = (ActivityWorldBoss2*)activity;
		awb->HiddenGameSendRankResult(msg);
	}
}

void SceneManager::OnHiddenGameWorldBoss3SyncRankResultMsg(const char * data, int length)
{
	crossgameprotocal::HiddenGameWorldBoss3SyncRankResultMsg* msg = (crossgameprotocal::HiddenGameWorldBoss3SyncRankResultMsg*)data;

	if (msg->length != sizeof(Protocol::SCWorldBoss3Result))
	{
		return;
	}

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
	if (NULL != activity)
	{
		ActivityWorldBoss3* awb = (ActivityWorldBoss3*)activity;
		awb->HiddenGameSendRankResult(msg);
	}
}

void SceneManager::OnHiddenGameWorldBossSyncParticipateUidList(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldBossSyncParticipateUidList* msg = (crossgameprotocal::HiddenGameWorldBossSyncParticipateUidList*)data;

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS);
	if (NULL != activity)
	{
		ActivityWorldBoss* awb = (ActivityWorldBoss*)activity;
		awb->HiddenGameSyncUidList(msg);
	}
}

void SceneManager::OnHiddenGameWorldBoss2SyncParticipateUidList(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldBoss2SyncParticipateUidList* msg = (crossgameprotocal::HiddenGameWorldBoss2SyncParticipateUidList*)data;

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2);
	if (NULL != activity)
	{
		ActivityWorldBoss2* awb = (ActivityWorldBoss2*)activity;
		awb->HiddenGameSyncUidList(msg);
	}
}

void SceneManager::OnHiddenGameWorldBoss3SyncParticipateUidList(const char * data, int length)
{
	crossgameprotocal::HiddenGameWorldBoss3SyncParticipateUidList* msg = (crossgameprotocal::HiddenGameWorldBoss3SyncParticipateUidList*)data;

	Activity* activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
	if (NULL != activity)
	{
		ActivityWorldBoss3* awb = (ActivityWorldBoss3*)activity;
		awb->HiddenGameSyncUidList(msg);
	}
}

void SceneManager::OnHiddenGameQuanMinActivityInfoReq(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQuanMinActivityInfoReq* req = (crossgameprotocal::HiddenGameQuanMinActivityInfoReq*)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	crossgameprotocal::GameHiddenQuanMinActivityInfoResp resp;
	resp.plat_type = LocalConfig::Instance().GetPlatType();
	resp.server_id = LocalConfig::Instance().GetMergeServerId();
	resp.uid = req->uid;

	int count = config->GetMaxSeq(ramgr);

	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		resp.cur_discount[i] = RAQuanMinDiscountManager::Instance().GetCommodityDiscout(i);
		resp.buy_count[i] = RAQuanMinDiscountManager::Instance().GetBuyCount(i);
	}

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&resp, sizeof(resp));
}

void SceneManager::OnGameHiddenQuanMinActivityInfoResp(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQuanMinActivityInfoResp* resp = (crossgameprotocal::GameHiddenQuanMinActivityInfoResp*)data;

	UniqueServerID usid(resp->plat_type, resp->server_id);

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(usid);

	const RandActivityQuanMinDiscountConfig * config = static_cast<const RandActivityQuanMinDiscountConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT));
	if (NULL == config)
	{
		return;
	}

	int count = config->GetMaxSeq(ramgr);
	for (int i = 0; i < count && i < QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM; ++i)
	{
		RAQuanMinDiscountManager::Instance().OnSyncInfoFromGame(i, resp->cur_discount[i], resp->buy_count[i]);
	}

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(resp->uid));
	if (NULL != role)
	{
		RoleActivity* ra = role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);
		if (NULL != ra)
		{
			RoleActivityQuanMinDiscount* raqmd = (RoleActivityQuanMinDiscount*)ra;
			raqmd->SendRAQuanMinDiscountInfo();
		}
	}
}

void SceneManager::OnGameHiddenWildBossInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenWildBossInfoReq * msg = (crossgameprotocal::GameHiddenWildBossInfoReq *)data;
	switch (msg->req_type)
	{
	case 1:
		WildBossManager::Instance().HiddenSendInfoToGameRole(msg->uid, msg->param1);
		break;
	case 2:
		WildBossManager::Instance().HiddenSendMonsterPosiTorGameRole(msg->uid, msg->param);
	default:
		break;
	}
}

void SceneManager::OnHiddenGameSyncBraveGroundInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameSyncBraveGroundInfo * msg = (crossgameprotocal::HiddenGameSyncBraveGroundInfo *)data;

	ActivityBraveGround * activity = (ActivityBraveGround *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND);
	if (activity != NULL)
	{
		activity->UpdateTopRole(msg->top_role_id, msg->is_notice != 0);
	}	
}

void SceneManager::OnGameHiddenFirstKillReq(const char * data, int length)
{
	static crossgameprotocal::GameHiddenFirstKillReq ghfkr;
	int offset = 0;
	if (!ghfkr.Deserialize(data, length, &offset))
	{
		return;
	}

	ServerFirstKill::Instance().OnGameHiddenFirstKillReq(ghfkr);
}

void SceneManager::OnHiddenGameRoleReturnOriginServerReq(const char* data, int length)
{
	crossgameprotocal::HiddenGameRoleReturnOriginServerReq* msg = (crossgameprotocal::HiddenGameRoleReturnOriginServerReq *)data;

	if (msg->business_data.business_type != ReturnOriginData_Business::BUSINESS_TYPE_INVALID)
	{
		OriginBusinessRegister::Instance().RegisterBusiness(msg->role_id, msg->business_data);
	}

	crossgameprotocal::GameHiddenRoleReturnOriginServerAck ack;
	ack.role_id = msg->role_id;
	ack.business_data = msg->business_data;
	ack.result = 0;

	InternalComm::Instance().SendToHiddenThroughCross((const char*)&ack, sizeof(ack));
}

void SceneManager::OnGameHiddenRoleReturnOriginServerAck(const char* data, int length)
{
	crossgameprotocal::GameHiddenRoleReturnOriginServerAck* ack = (crossgameprotocal::GameHiddenRoleReturnOriginServerAck*)data;
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(ack->role_id));
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetRoleCross()->OnBackToOriginServerAck(ack);
	}
}

void SceneManager::OnGameHiddenTeammateJoinOriginServerTeam(const char* data, int length)
{
	crossgameprotocal::GameHiddenTeammateJoinOriginServerTeam* msg = (crossgameprotocal::GameHiddenTeammateJoinOriginServerTeam*)data;

	for (int i = 0; i < ARRAY_ITEM_COUNT(msg->business_data.teammate_list); ++i)
	{
		int teammate_uid = msg->business_data.teammate_list[i];
		Role* teammate = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(teammate_uid));
		if (NULL == teammate) continue;

		static ReturnOriginData_Business business_data;
		business_data.Reset();

		business_data.business_type = ReturnOriginData_Business::BUSINESS_TYPE_JOIN_ORIGIN_TEAM;
		business_data.join_team_id = msg->team_index;

		crossgameprotocal::HiddenGameRoleReturnOriginServerReq req;
		req.role_id = teammate_uid;
		req.business_data = business_data;

		InternalComm::Instance().SendToGameThroughCross(
			teammate->GetRoleModuleManager()->GetRoleCross()->GetOriginPlatType(), 
			teammate->GetRoleModuleManager()->GetRoleCross()->GetOriginServerId(), 
			(const char*)&req, sizeof(req));
	}
}

void SceneManager::OnHiddenGameSyncHuanJieZhanChangeInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameHuanJieZhanChangeSyncInfo * msg = (crossgameprotocal::HiddenGameHuanJieZhanChangeSyncInfo *)data;
	ActivityHuanJieZhanChang * activity = (ActivityHuanJieZhanChang *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUAN_JIE_ZHAN_CHANG);
	if (activity != NULL)
	{
		std::vector<int> vec(msg->role_id, msg->role_id + ARRAY_LENGTH(msg->role_id));
		activity->SetSeasonTopRole(vec, msg->is_notice != 0);
	}
}

void SceneManager::OnGameHiddenTestNetwork(const char* data, int length)
{
	crossgameprotocal::GameHiddenTestNetwork* req = (crossgameprotocal::GameHiddenTestNetwork*)data;
	if (req->is_start == 1)
	{
		unsigned int now = EngineAdapter::Instance().Time();
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenTestNetwork start at %d", now);
	}
	else if (req->is_end == 1)
	{
		unsigned int now = EngineAdapter::Instance().Time();
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenTestNetwork end at %d", now);
	}
}

void SceneManager::OnGameHiddenBraveGroundSetInfo(const char * data, int length)
{
	crossgameprotocal::GameHiddenBraveGroundSetInfo * msg = (crossgameprotocal::GameHiddenBraveGroundSetInfo *)data;
	ActivityBraveGround * activity = (ActivityBraveGround *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_BRAVE_GROUND);
	if (activity != NULL)
	{
		activity->GameHiddenSetRoleInfoReq(msg->role_id, msg->reached_level);
	}
}

void SceneManager::OnHiddenGameSpecialLogicActivityInfoSync(const char * data, int length)
{
	crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync * msg = (crossgameprotocal::HiddenGameSpecialLogicActivityInfoSync *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		switch (msg->special_logic_activity_type)
		{
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT:
		{
			OfflineRegisterManager::Instance().OnGuildFightFetchJoinReward(msg->role_id, msg->fetch_reward_zero_timestamp);
		}
		break;
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT:
		{
			OfflineRegisterManager::Instance().OnOnlyFightFetchJoinReward(msg->role_id, msg->fetch_reward_zero_timestamp);
		}
		break;
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT:
		{
			OfflineRegisterManager::Instance().OnTeamFightFetchJoinReward(msg->role_id, msg->fetch_reward_zero_timestamp);
		}
		break;
		default:
			break;
		}
	}
	else
	{
		switch (msg->special_logic_activity_type)
		{
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT:
		{
			role->GetRoleModuleManager()->GetCommonData().fetch_guild_fight_join_reward_flag = 1;
			role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_GUILD_FIGHT] = msg->fetch_reward_zero_timestamp;
		}
		break;
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT:
		{
			role->GetRoleModuleManager()->GetCommonData().fetch_only_fight_join_reward_flag = 1;
			role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_ONLY_FIGHT] = msg->fetch_reward_zero_timestamp;
		}
		break;
		case DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT:
		{
			role->GetRoleModuleManager()->GetCommonData().fetch_team_fight_join_reward_flag = 1;
			role->GetRoleModuleManager()->GetCommonData().fetch_special_logic_activity_reward_zero_timestamp[DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_TEAM_FIGHT] = msg->fetch_reward_zero_timestamp;
		}
		break;
		default:
			break;
		}
	}
}

void SceneManager::OnHiddenGameGetRankListReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGetRankListReq * req = (crossgameprotocal::HiddenGameGetRankListReq *)data;
	RankManager::Instance().OnHiddenGameGetRankList(req);
}

void SceneManager::OnHiddenGameShenShouJiangLinReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameShenShouJiangLinReq * req = (crossgameprotocal::HiddenGameShenShouJiangLinReq *)data;
	if (req->req_type == crossgameprotocal::HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_DRAW)
	{
		gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld shen_shou_jiang_lin draw from hidden", __FUNCTION__, __LINE__, req->role_id, req->req_id);
	}

	if (!LocalConfig::Instance().IsBelongServer(req->role_id))
	{
		gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld", __FUNCTION__, __LINE__, req->role_id, req->req_id);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityShenShouJiangLin * act = (RandActivityShenShouJiangLin *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	if (NULL == act) return;

	switch (req->req_type)
	{
		case crossgameprotocal::HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_INFO:
			{
				act->GameHiddenSendInfo(req->role_id);
			}
			break;
		case crossgameprotocal::HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_DRAW:
			{
				int ret = act->DrawFromHidden(req->role_id, req->req_id);
				gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld shen_shou_jiang_lin draw ret:%d", __FUNCTION__, __LINE__, req->role_id, req->req_id, ret);
			}
			break;
	}
}

void SceneManager::OnGameHiddenShenShouJiangLinDrawRet(const char * data, int length)
{
	TLVUnserializer2 unserializer;
	unserializer.Reset(data, length);
	crossgameprotocal::GameHiddenShenShouJiangLinDrawRet ret;
	if (!ret.Unserialize(unserializer))
	{
		return;
	}

	std::string log_str;
	for (int i = 0; i < (int)ret.items.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", ret.items[i].item_id, ret.items[i].num, ret.items[i].is_bind);
	}
	gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld seq:%d gold_num:%d %s", __FUNCTION__, __LINE__, ret.role_id, ret.req_id, ret.seq, ret.gold_num, log_str.c_str());

	if (ret.mail_str.empty())
	{
		ret.mail_str = STRING_SPRINTF(gamestring::g_act_full_bag_content, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	}

	if (!ret.broadcast_str.empty() && !CrossConfig::Instance().IsHiddenServer())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", ret.broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(ret.role_id);
	if (role != NULL)
	{
		RoleActivityShenShouJiangLin * role_activity = static_cast<RoleActivityShenShouJiangLin *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN));
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(role->GetUniqueServerID());

		const RandActivityShenShouJiangLinConfig * config = static_cast<const RandActivityShenShouJiangLinConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN));


		if (role_activity && ramgr && config)
		{
			std::set<int> bao_di_id_list;
			std::vector<const RAShenShouJiangLinItemCfg *> bao_di_cfg_list = config->GetRAShenShouJiangLinBaoDiCfgList(ramgr, bao_di_id_list);

			for (int bao_di_index = 0; bao_di_index < (int)bao_di_cfg_list.size(); ++bao_di_index)
			{
				const RAShenShouJiangLinItemCfg * curr = bao_di_cfg_list[bao_di_index];

				BaoDiInfoItem * bao_di_info_item = role_activity->GetBaoDiInfoItem(curr->bao_di_id);
				if (bao_di_info_item)
				{
					if (bao_di_info_item->is_get != 0)
					{
						continue;
					}

					if (bao_di_info_item->times + 1 >= curr->bao_di_times)
					{
						ret.items.clear();
						ret.bao_di_id = curr->bao_di_id;
						ret.seq = curr->seq;
						ret.items.push_back(curr->reward);						

						for (int i = 0; i < (int)ret.items.size(); ++i)
						{
							log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", ret.items[i].item_id, ret.items[i].num, ret.items[i].is_bind);
						}
						gamelog::g_log_shen_shou_jiang_lin.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld seq:%d gold_num:%d %s", __FUNCTION__, __LINE__, ret.role_id, ret.req_id, ret.seq, ret.gold_num, log_str.c_str());


						break;
					}
				}
			}

			role_activity->SetBaoDiInfoAfterDraw(ret.bao_di_id, bao_di_id_list);
		}

		role->GetRoleModuleManager()->GetMoney()->AddGold(ret.gold_num, __FUNCTION__, false, false, true);
		if (!ret.items.empty())
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporary((short)ret.items.size(), &ret.items[0]))
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)ret.items.size(), &ret.items[0], PUT_REASON_SHEN_SHOU_JIANG_LIN);
			}
			else
			{
				// 发邮件
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, ret.items);

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), "%s", ret.mail_str.c_str());
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(ret.role_id, MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}

		Protocol::SCRAShenShouJiangLinDrawResult protocol;
		protocol.seq = ret.seq;
		protocol.gold_num = ret.gold_num;
		protocol.item_count = (int)ret.items.size();
		for (int i = 0; i < ARRAY_LENGTH(protocol.item) && i < (int)ret.items.size(); ++i)
		{
			protocol.item[i].item_id = ret.items[i].item_id;
			protocol.item[i].num = ret.items[i].num;
			protocol.item[i].is_bind = ret.items[i].is_bind;
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = ret.gold_num;
		FillMailContentParam(contentparam, ret.items);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(ret.role_id, MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void SceneManager::OnHiddenGameHeartBeat(const char * data, int length)
{
	CrossManager::Instance().OnHiddenHeartBeat();
}

//void SceneManager::OnHiddenGameResetGuildFightInfo(const char * data, int length)
//{
//
//}
//
//void SceneManager::OnHiddenGameSetGuildFightInfo(const char * data, int length)
//{
//
//
//}
//
//void SceneManager::OnGameHiddenSetGuildFightInfo(const char * data, int length)
//{
//
//}
//
//void SceneManager::OnHiddenGameSetGuildFightBuffInfo(const char * data, int length)
//{
//	if (CrossConfig::Instance().IsHiddenServer())
//	{
//		return;
//	}
//
//	crossgameprotocal::HiddenGameSetGuildFightBuffInfo * msg = (crossgameprotocal::HiddenGameSetGuildFightBuffInfo *)data;
//
//	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
//	if (NULL != guild)
//	{
//		guild->SetGuildFightRankBuff(msg->rank);
//	}
//}
//
//void SceneManager::OnGameHiddenSetGuildFightBuffInfo(const char * data, int length)
//{
//	if (!CrossConfig::Instance().IsHiddenServer())
//	{
//		return;
//	}
//
//	crossgameprotocal::GameHiddenSetGuildFightBuffInfo * msg = (crossgameprotocal::GameHiddenSetGuildFightBuffInfo *)data;
//
//	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
//	if (NULL != guild)
//	{
//		guild->SyncGuildFightRankBuff(msg->rank);
//	}
//}

void SceneManager::OnGameHiddenChatGroupHandoverLeadership(const char* data, int length)
{
	crossgameprotocal::GameHiddenChatGroupLeadershipHandover* req = (crossgameprotocal::GameHiddenChatGroupLeadershipHandover*)data;

	CrossChatGroupManager::Instance().OnRoleHandOverLeadershipToOther(req->group_id, req->old_leader_uid, req->new_leader_uid);
}

void SceneManager::OnGameHiddenShanHaiBossInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenShanHaiBossInfoReq * msg = (crossgameprotocal::GameHiddenShanHaiBossInfoReq *)data;
	switch (msg->req_type)
	{
	case 0:
		ShanHaiBossManager::Instance().HiddenSendBossInfoToGameRole(msg->uid, msg->param1);
		break;
	case 1:
		ShanHaiBossManager::Instance().HiddenSendRoleInfoToGameRole(msg->uid, msg->param1);
		break;

	default:
		break;
	}
}

void SceneManager::OnGameHiddenRoleInfoNoticeReqRoute(const char * data, int length)
{
	crossgameprotocal::GameHiddenRoleInfoNoticeReqRoute * req = (crossgameprotocal::GameHiddenRoleInfoNoticeReqRoute *)data;

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int role_id = req->role_id;

	Protocol::SCRoleInfoNotice protocol;
	protocol.is_cross = 1;
	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
}

void SceneManager::OnGameHiddenRelicLootingReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenRelicLootingReq * req = (crossgameprotocal::GameHiddenRelicLootingReq *)data;

	ActivityRelicLooting * activity =  (ActivityRelicLooting *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
	if (activity != NULL && activity->IsActivityOpen())
	{
		switch (req->req_type)
		{
		case 1:
			activity->SendRankListToUID(req->uid);
			break;
		case 2:
			activity->SendRewardAchievementToUID(req->uid);
			break;
		case 3:
			activity->GetAchievementReward(req->uid, req->param);
			break;
		default:
			break;
		}
	}
}

void SceneManager::OnHiddenGameRelicLootingReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameRelicLootingReq * req = (crossgameprotocal::HiddenGameRelicLootingReq *)data;

	ActivityRelicLooting * activity = (ActivityRelicLooting *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_RELIC_LOOTING);
	if (activity != NULL && activity->IsActivityOpen())
	{
		switch (req->req_type)
		{
		case 4:
			activity->CrossBack(req->uid, req->param);
			break;
		default:
			break;
		}
	}
}

void SceneManager::OnGameHiddenSyncCloudArenaUserDataReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenSyncCloudArenaUserDataReq* req = (crossgameprotocal::GameHiddenSyncCloudArenaUserDataReq*)data;
	
	const CloudArenaUser* user = CloudArena::Instance().GetUser(req->uid);
	if (NULL == user) return;

	crossgameprotocal::HiddenGameSyncCloudArenaUserDataResp resp;
	resp.uid = req->uid;
	resp.season_timestamp = CloudArena::Instance().GetSeasonTimestamp();
	resp.last_season_rank = user->GetLastSeasonRank();
	resp.season_highest_dan_id = user->GetSeasonHighestDanId();
	resp.season_highest_rank = user->GetSeasonHighestRank();
	resp.season_highest_score = user->GetSeasonHighestScore();
	
	InternalComm::Instance().SendToGameThroughCross(req->plat_type, req->server_id, (const char*)&resp, sizeof(resp));
}

void SceneManager::OnHiddenGameSyncCloudArenaUserDataResp(const char* data, int length)
{
	crossgameprotocal::HiddenGameSyncCloudArenaUserDataResp* resp = (crossgameprotocal::HiddenGameSyncCloudArenaUserDataResp*)data;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(resp->uid));
	if (NULL != role)
	{
		role->GetRoleModuleManager()->UpdateCloudArenaData(resp->season_timestamp, resp->last_season_rank, resp->season_highest_dan_id, resp->season_highest_rank, resp->season_highest_score);
	}
}

void SceneManager::OnGameHiddenCloudArenaRealTimeInfoReq(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenCloudArenaRealTimeInfoReq* req = (crossgameprotocal::GameHiddenCloudArenaRealTimeInfoReq*)data;
	const CloudArenaUser* user = CloudArena::Instance().GetUser(req->role_id);
	if (NULL != user)
	{
		Protocol::SCCloudArenaRealTimeInfo info;
		info.cur_score = user->GetScore();
		info.cur_rank = CloudArena::Instance().GetUserRank(req->role_id);
		InternalComm::Instance().SendToGameUserThroughCross(req->role_id, &info, sizeof(info));
	}
}

void SceneManager::OnGameHiddenRoleLoginCloudArenaUserInfoReq(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenRoleLoginSyncCloudArenaUserInfo* req = (crossgameprotocal::GameHiddenRoleLoginSyncCloudArenaUserInfo*)data;
	CloudArena::Instance().OnUserLoginOriginSendUserInfo(req->uid);
}

void SceneManager::OnGameHiddenMarketBuyItemRecordSynReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq * msg = (crossgameprotocal::GameHiddenMarketBuyItemRecordSynReq *)data;
	MarketManager::Instance().OnHiddenReceGameBuyItemRecord(msg);
}

void SceneManager::OnHiddenGameMarketBuyItemRecordSynReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq * msg = (crossgameprotocal::HiddenGameMarketBuyItemRecordSynReq *)data;
	MarketManager::Instance().OnGameRecvHiddenBuyItemRecord(msg);
}

void SceneManager::OnGameHiddenMarketAllTransactionRecordSynReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenMarketAllTransactionRecordSynReq ghmatrsr;
	int offset = 0;
	if (!ghmatrsr.Deserialize(data, length, &offset))
	{
		return;
	}

	MarketManager::Instance().OnHiddenRecvGameAllRecord(ghmatrsr);

	//验证用回执
	crossgameprotocal::CrossGameDataSyncCheckAck ack;
	ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_MARKET;
	InternalComm::Instance().SendToGameThroughCross(ghmatrsr.usid.plat_type , ghmatrsr.usid.server_id ,(const char *)&ack, sizeof(ack));
}

void SceneManager::OnHiddenGameMarketAllTransactionRecordSynReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq hgmatrsr;
	int offset = 0;
	if (!hgmatrsr.Deserialize(data, length, &offset))
	{
		return;
	}
	switch (hgmatrsr.op_type)
	{
		case crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq::OP_TYPE_HIDDEN_CONNECT_CROSS:
		{
			MarketManager::Instance().OnGameHiddenAllRecordSyn();
		}
		break;
		case crossgameprotocal::HiddenGameMarketAllTransactionRecordSynReq::OP_TYPE_HIDDEN_RECV_GAME_INFO_SYN:
		{
			MarketManager::Instance().OnGameRecvHiddenAllRecord(hgmatrsr);
		}
		break;
	default:
		break;
	}
}

void SceneManager::OnGameHiddenVideoChangeReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenChangeVideoReq * msg = (crossgameprotocal::GameHiddenChangeVideoReq *)data;
	bool need_change = BattleVideoMgr::Instance().NeedChangeVideo(msg->monster_group_id, msg->video_score);
	crossgameprotocal::HiddenGameChangeVideoRet ret;
	ret.monster_group_id = msg->monster_group_id;
	ret.ret_reason = need_change ? crossgameprotocal::HiddenGameChangeVideoRet::RET_REASON_YES : crossgameprotocal::HiddenGameChangeVideoRet::RET_REASON_NO;
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg->uid).db_index, (const char*)&ret, sizeof(ret));
}

void SceneManager::OnHiddenGameVideoChangeReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameChangeVideoRet * msg = (crossgameprotocal::HiddenGameChangeVideoRet *)data;
	switch (msg->ret_reason)
	{
	case crossgameprotocal::HiddenGameChangeVideoRet::RET_REASON_YES:
		BattleVideoMgr::Instance().SendVideoToCross(msg->monster_group_id);
		break;
	default:
		break;
	}
}

void SceneManager::OnCrossGameChivalrousRankListResp(const char* data, int length)
{
	crossgameprotocal::CrossGameChivalrousRankListResp* resp = (crossgameprotocal::CrossGameChivalrousRankListResp*)data;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(resp->uid);
	if (NULL == role) return;

	Protocol::SCChivalrousRankList ranklist;
	ranklist.rank_type = resp->rank_type;
	ranklist.count = 0;
	for (int i = 0; i < resp->count && ranklist.count < ARRAY_ITEM_COUNT(ranklist.rank_list); ++i)
	{
		ranklist.rank_list[ranklist.count++] = resp->rank_list[i];
	}

	int send_len = sizeof(ranklist) - sizeof(ranklist.rank_list) + ranklist.count * sizeof(ranklist.rank_list[0]);
	EngineAdapter::Instance().NetSend(role->GetNetId(), &ranklist, send_len);
}

void SceneManager::OnHiddenGameSyncCloudArenaSeasonInfo(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameSyncCloudArenaSeasonInfo* info = (crossgameprotocal::HiddenGameSyncCloudArenaSeasonInfo*)data;

	WorldStatus::Instance().GetCommonData().cloud_arena_cur_season_start_timestamp = info->cur_season_start_timestamp;
	WorldStatus::Instance().GetCommonData().cloud_arena_cur_season_end_timestamp = info->cur_season_end_timestamp;

	gamelog::g_log_cloud_arena.printf(LL_INFO, "SceneManager::OnHiddenGameSyncCloudArenaSeasonInfo season_start_timestamp[%u] season_end_timestamp[%u]",
		info->cur_season_start_timestamp, info->cur_season_end_timestamp);
}

void SceneManager::OnHiddenGamePersonRankFriendReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameGetFriendRankListReq * req = (crossgameprotocal::HiddenGameGetFriendRankListReq *)data;
	RankManager::Instance().OnHiddenGameGetRankFriendList(req);
}

void SceneManager::OnGameHiddenSyncHongBaoInfo(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSyncHongBaoInfo * msg = (crossgameprotocal::GameHiddenSyncHongBaoInfo *)data;
	HongBaoManager::Instance().OnSyncHongBaoInfoFromGame(msg);
}

void SceneManager::OnHiddenGameBigDipperSyncRet(const char* data, int length)
{
	crossgameprotocal::HiddenGameBigDipperSynRet * msg = (crossgameprotocal::HiddenGameBigDipperSynRet *)data;
	BigDipperManager::Instance().OnHideInfoBack(&msg->info);
}

void SceneManager::OnHiddenGameSyncHongBaoReqGive(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameSyncHongBaoReqGive * msg = (crossgameprotocal::HiddenGameSyncHongBaoReqGive *)data;
	HongBaoManager::Instance().OnHiddenGameSyncHongBaoReqGiveFromHidden(msg);
}

void SceneManager::OnGameHiddenSyncHongBaoReqGiveRet(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet * msg = (crossgameprotocal::GameHiddenSyncHongBaoReqGiveRet *)data;
	HongBaoManager::Instance().OnSyncReqGiveRetFromGame(msg);
}

void SceneManager::OnHiddenGameSyncHongBaoReqGet(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameSyncHongBaoReqGet * msg = (crossgameprotocal::HiddenGameSyncHongBaoReqGet *)data;
	HongBaoManager::Instance().OnHiddenGameSyncHongBaoReqGetFromHidden(msg);
}

void SceneManager::OnGameHiddenSyncHongBaoReqGetRet(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenSyncHongBaoReqGetRet * msg = (crossgameprotocal::GameHiddenSyncHongBaoReqGetRet *)data;
	HongBaoManager::Instance().OnSyncReqGetRetFromGame(msg);
}

void SceneManager::OnHiddenGameSyncHongBaoReqError(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameSyncHongBaoReqError * msg = (crossgameprotocal::HiddenGameSyncHongBaoReqError *)data;
	HongBaoManager::Instance().OnHiddenGameSyncHongBaoReqError(msg);
}

void SceneManager::OnGameHiddenGiveGiftReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenGiveGiftReq * req = (crossgameprotocal::GameHiddenGiveGiftReq *)data;
	
	switch (req->req_type)
	{
		case  crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_FETCH:
			{
				GiveGift::Instance().FetchItem(req->role_id, req->give_gift_id_ll, true);
			}
			break;
		case  crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_ACK:
			{
				GiveGift::Instance().SetFetchAck(req->role_id, req->give_gift_id_ll, req->p3);

				gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d |  SetFetchAck  role_id:%d give_gift_id_ll::%lld is_succ:%d ", __FUNCTION__, __LINE__, req->role_id, req->give_gift_id_ll, req->p3);
			}
			break;
		case  crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_GM_FETCH:
			{
				GiveGift::Instance().GmFetchItem(req->role_id, true);
			}
			break;
	}
}

void SceneManager::OnGameHiddenGiveGiftAddGift(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenGiveGiftAddGift * req = (crossgameprotocal::GameHiddenGiveGiftAddGift *)data;

	bool ret = GiveGift::Instance().AddGiveGift(req->gift_data);

	gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d | %s %s", __FUNCTION__, __LINE__, ::CommonSaveGiveGiftDataToString(req->gift_data).c_str(), ret ? "SUCC" : "FAIL");
}

void SceneManager::OnHiddenGameGiveGiftRetInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameGiveGiftRetInfo * info = (crossgameprotocal::HiddenGameGiveGiftRetInfo *)data;

	crossgameprotocal::GameHiddenGiveGiftReq fetch_req;
	fetch_req.req_type = crossgameprotocal::GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_ACK;
	fetch_req.role_id = info->role_id;
	fetch_req.give_gift_id_ll = info->give_gift_id_ll;
	fetch_req.p3 = 0;

	gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d |  role_id:%d HiddenGameGiveGiftRetInfo accept %s", __FUNCTION__, __LINE__, info->role_id, ::CommonSaveGiveGiftDataToString(info->gift_data).c_str());

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(info->role_id);
	if (role != NULL)
	{
		ItemDataWrapper wrap;
		wrap.item_id = info->gift_data.item.item_id;
		wrap.num = info->gift_data.item.num;
		wrap.invalid_time = info->gift_data.item.invaild_time;

		if (role->GetRoleModuleManager()->GetKnapsack()->Put(wrap, PUT_REASON_FETCH_GIVE_GIFT))
		{
			fetch_req.p3 = 1;
			EventHandler::Instance().OnFetchFriendGift(role);
			gamelog::g_log_give_gift.printf(LL_INFO, "%s line:%d |  role_id:%d HiddenGameGiveGiftRetInfo fetch succ %s", __FUNCTION__, __LINE__, info->role_id, ::CommonSaveGiveGiftDataToString(info->gift_data).c_str());
		}
	}

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&fetch_req, sizeof(fetch_req));
}

void SceneManager::OnHiddenGameShiTuSeekReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameShiTuSeekReq * req = (crossgameprotocal::HiddenGameShiTuSeekReq *)data;

	switch (req->type)
	{
		case  crossgameprotocal::HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE_INFO:
			{
				ShiTuSeekingManager::Instance().SendShiTuSeekingNoticeList(req->p1, NULL, req->role_id);
			}
			break;
	}
}

void SceneManager::OnGameHiddenHundredGhostBossInfoReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenHundredGhostBossInfoReq * msg = (crossgameprotocal::GameHiddenHundredGhostBossInfoReq *)data;
	
	ActivityHundredGhost* act = static_cast<ActivityHundredGhost*>(ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_HUNDRED_GHOST));
	if (NULL == act)
	{
		return;
	}

	if (!act->IsActivityOpen())
	{
		return;
	}

	act->HiddenSendBossInfoToGameRole(msg->uid);
}

void SceneManager::OnHiddenGameContinueRoleBattleOnCross(const char* data, int length)
{
	crossgameprotocal::HiddenGameContinueCrossBattle* msg = (crossgameprotocal::HiddenGameContinueCrossBattle*)data;
	
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(msg->role_id));
	if (NULL == role)
	{
		return;
	}

	if (role->InTeam())
	{
		return;
	}

	if (role->GetRoleStatusManager()->IsInBattleStatus())
	{
		return;
	}

	if (!role->GetScene()->IsInStaticScene())
	{
		return;
	}

	CrossData_Business business_data;
	business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_INVALID;
	role->GetRoleModuleManager()->GetRoleCross()->OnStartCrossReq(business_data);
}

void SceneManager::OnHiddenGameCowReportGoodNewsCanBuy(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy* msg = (crossgameprotocal::HiddenGameCowReportGoodNewsCheckCanBuy*)data;

	CowReportGoodNewsManager::Instance().OnCowReportGoodNewsCanBuyFromHidden(msg);
}

void SceneManager::OnGameHiddenCowReportGoodNewsCanBuyAck(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck* msg = (crossgameprotocal::GameHiddenCowReportGoodNewsCheckCanBuyAck*)data;

	CowReportGoodNewsManager::Instance().OnCowReportGoodNewsCheckCanBuyAckFromGameWorld(msg);
}

void SceneManager::OnHiddenGameCowReportGoodNewsBuy(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem* msg = (crossgameprotocal::HiddenGameCowReportGoodNewsBuyItem*)data;

	CowReportGoodNewsManager::Instance().OnCowReportGoodNewsBuyItemFromHidden(msg);
}

void SceneManager::OnGameHiddenCowReportGoodNewsBuyAck(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck* msg = (crossgameprotocal::GameHiddenCowReportGoodNewsBuyItemAck*)data;

	CowReportGoodNewsManager::Instance().OnCowReportGoodNewsBuyItemAckFromGameWorld(msg);
}

void SceneManager::OnSyncCowReportGoodNewsBuyOfflineEvent(const char* data, int length)
{
	crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent* msg = (crossgameprotocal::SyncCowReportGoodNewsBuyItemOfflineEvent*)data;

	CowReportGoodNewsManager::Instance().OnSyncCowReportGoodNewsBuyItemOfflineEvent(msg);
}

void SceneManager::OnHiddenGameCowReportGoodNewsGetInfo(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;
	crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo * msg = (crossgameprotocal::HiddenGameCowReportGoodNewsGetInfo*)data;

	CowReportGoodNewsManager::Instance().CowReportGoodNewsGetInfoFromHidden(msg);
}


void SceneManager::OnHiddenGameTigerBringsBlessingCanBuy(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameTigerBringsBlessingCheckCanBuy* msg = (crossgameprotocal::HiddenGameTigerBringsBlessingCheckCanBuy*)data;

	TigerBringsBlessingManager::Instance().OnTigerBringsBlessingCanBuyFromHidden(msg);
}

void SceneManager::OnGameHiddenTigerBringsBlessingCanBuyAck(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenTigerBringsBlessingCheckCanBuyAck* msg = (crossgameprotocal::GameHiddenTigerBringsBlessingCheckCanBuyAck*)data;

	TigerBringsBlessingManager::Instance().OnTigerBringsBlessingCheckCanBuyAckFromGameWorld(msg);
}

void SceneManager::OnHiddenGameTigerBringsBlessingBuy(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameTigerBringsBlessingBuyItem* msg = (crossgameprotocal::HiddenGameTigerBringsBlessingBuyItem*)data;

	TigerBringsBlessingManager::Instance().OnTigerBringsBlessingBuyItemFromHidden(msg);
}

void SceneManager::OnGameHiddenTigerBringsBlessingBuyAck(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenTigerBringsBlessingBuyItemAck* msg = (crossgameprotocal::GameHiddenTigerBringsBlessingBuyItemAck*)data;

	TigerBringsBlessingManager::Instance().OnTigerBringsBlessingBuyItemAckFromGameWorld(msg);
}

void SceneManager::OnSyncTigerBringsBlessingBuyOfflineEvent(const char* data, int length)
{
	crossgameprotocal::SyncTigerBringsBlessingBuyItemOfflineEvent* msg = (crossgameprotocal::SyncTigerBringsBlessingBuyItemOfflineEvent*)data;

	TigerBringsBlessingManager::Instance().OnSyncTigerBringsBlessingBuyItemOfflineEvent(msg);
}

void SceneManager::OnHiddenGameTigerBringsBlessingGetInfo(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;
	crossgameprotocal::HiddenGameTigerBringsBlessingGetInfo * msg = (crossgameprotocal::HiddenGameTigerBringsBlessingGetInfo*)data;

	TigerBringsBlessingManager::Instance().TigerBringsBlessingGetInfoFromHidden(msg);
}


void SceneManager::OnHiddenGameSyncCrossData(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	CrossManager::Instance().SyncCrossDataToHidden();
}

void SceneManager::OnGameHiddenCreatAutoRobotInfo(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	if (length < sizeof(crossgameprotocal::GameHiddenCreatAutoRobotInfo))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "OnGameHiddenCreatAutoRobotInfo length not long enough");
		return;
	}

	crossgameprotocal::GameHiddenCreatAutoRobotInfo * info 
		= (crossgameprotocal::GameHiddenCreatAutoRobotInfo *)data;

	World::GetInstWorld()->SetCreatAutoRobotServerID(
		info->mother_server_usid,
		info->can_creat_static_robot);
}

void SceneManager::OnHiddenGameActivityEndInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	if (length < sizeof(crossgameprotocal::HiddenGameActivityEndInfo))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "OnHiddenGameActivityEndInfo length not long enough");
		return;
	}
	crossgameprotocal::HiddenGameActivityEndInfo * msg = (crossgameprotocal::HiddenGameActivityEndInfo *)data;
	if(ACTIVITY_TYPE_GUILD_FIGHT != msg->activity_type && ACTIVITY_TYPE_GUILD_HONOR_BATTLE != msg->activity_type) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL != role)
	{	
		if (ACTIVITY_TYPE_GUILD_FIGHT == msg->activity_type)
		{
			role->GetRoleModuleManager()->OnSetGuildFightInfo(msg->param1);
		}
		else
		{
			role->GetRoleModuleManager()->OnSetGuildHonorInfo(msg->param1);
		}
	}
	else
	{
		if (ACTIVITY_TYPE_GUILD_FIGHT == msg->activity_type)
		{
			OfflineRegisterManager::Instance().OnSetGuildFightRank(msg->role_id, msg->param1);
		}
		else
		{
			OfflineRegisterManager::Instance().OnSetGuildHonorRank(msg->role_id, msg->param1);
		}
	}
}

void SceneManager::OnHiddenGameServerCourseInfoReq(const char * data, int length)
{
	if(CrossConfig::Instance().IsHiddenServer()) return;

	if (length < sizeof(crossgameprotocal::HiddenGameServerCourseInfoReq))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "HiddenGameServerCourseInfoReq length not long enough");
		return;
	}
	crossgameprotocal::HiddenGameServerCourseInfoReq * msg = (crossgameprotocal::HiddenGameServerCourseInfoReq *)data;
	CourseOfGrowthManager::Instance().OnSendAllInfoToHidden(msg->role_id);
}

void SceneManager::OnHiddenGameServerCourseChangeReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	if (length < sizeof(crossgameprotocal::HiddenGameServerCourseChangeReq))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "HiddenGameServerCourseChangeReq length not long enough");
		return;
	}
	crossgameprotocal::HiddenGameServerCourseChangeReq * msg = (crossgameprotocal::HiddenGameServerCourseChangeReq *)data;
	CourseOfGrowthManager::Instance().OnCourseFinishChangeFromHidden(msg->role_id, msg->seq, (0 != msg->is_add) ? true : false);
}

void SceneManager::OnHiddenGameRoleWinChiefElectionSyn(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	if (length < sizeof(crossgameprotocal::HiddenGameRoleWinChiefElectionSyn))
	{
		gamelog::g_log_debug.printf(LL_WARNING, "HiddenGameRoleWinChiefElectionSyn length not long enough");
		return;
	}
	crossgameprotocal::HiddenGameRoleWinChiefElectionSyn * msg = (crossgameprotocal::HiddenGameRoleWinChiefElectionSyn *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		OfflineRegisterManager::Instance().OnWinChiefElection(msg->role_id, msg->prof);
	}
	else
	{	
		EventHandler::Instance().OnWinChiefElection(role, msg->prof);
	}
}

void SceneManager::OnGameHiddenJieYiZuCreate(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuCreate * req = (crossgameprotocal::GameHiddenJieYiZuCreate *)data;
	std::vector<CommonSaveJieYiZuRoleInfoItem> vec;
	for (int i = 0; i < ARRAY_LENGTH(req->list) && i < req->count; ++i)
	{
		CommonSaveJieYiZuRoleInfoItem & curr = req->list[i];
		vec.push_back(curr);
	}
	
	JieYiZuManager::Instance().RealCreateJieYiZu(req->jie_yi_name, vec);
}

void SceneManager::OnHiddenGameJieYiZuSync(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameJieYiZuSync * req = (crossgameprotocal::HiddenGameJieYiZuSync *)data;
	if (req->sync_type == JIE_YI_ZU_SYNC_TYPE_START)
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  JIE_YI_ZU_SYNC_TYPE_START", __FUNCTION__, __LINE__);
		JieYiZuManager::Instance().OnSyncJieYiZuInfoFromHiddenStart();
	}
	
	if (req->sync_type == JIE_YI_ZU_SYNC_TYPE_UPDATE)
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  JIE_YI_ZU_SYNC_TYPE_UPDATE jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__,
			req->jie_yi_zu_info.jie_yi_zu_id, JieYIZuNameToString(req->jie_yi_zu_info.jie_yi_name).c_str());

		JieYiZuManager::Instance().OnSyncJieYiZuInfoFromHidden(req->jie_yi_zu_info);
	}

	if (req->sync_type == JIE_YI_ZU_SYNC_TYPE_END)
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  JIE_YI_ZU_SYNC_TYPE_END", __FUNCTION__, __LINE__);
		JieYiZuManager::Instance().OnSyncJieYiZuInfoFromHiddenEnd();
	}	
}

void SceneManager::OnHiddenGameJieYiZuDismiss(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameJieYiZuDismiss * req = (crossgameprotocal::HiddenGameJieYiZuDismiss *)data;
	JieYiZuManager::Instance().DismissJieYiZu(req->jie_yi_zu_id);
}

void SceneManager::OnGameHiddenJieYiZuName(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuName * req = (crossgameprotocal::GameHiddenJieYiZuName *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuNameHelper(req->ji_yi_zu_name);
}

void SceneManager::OnGameHiddenJieYiZuMemberName(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuMemberName * req = (crossgameprotocal::GameHiddenJieYiZuMemberName *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuMemberName(req->role_id, req->member_name);
}

void SceneManager::OnGameHiddenJieYiZuText(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuText * req = (crossgameprotocal::GameHiddenJieYiZuText *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuText(req->role_id, req->text);
}

void SceneManager::OnGameHiddenJieYiZuLeave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuLeave * req = (crossgameprotocal::GameHiddenJieYiZuLeave *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuLeave(req->role_id, req->is_cancel);
}

void SceneManager::OnGameHiddenJieYiZuKick(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuKick * req = (crossgameprotocal::GameHiddenJieYiZuKick *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuKick(req->role_id, req->target_role_id);
}

void SceneManager::OnGameHiddenJieYiZuTanHe(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuTanHe * req = (crossgameprotocal::GameHiddenJieYiZuTanHe *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->TanHe(req->role_id);
}

void SceneManager::OnGameHiddenJieYiZuTanHeVote(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuTanHeVote * req = (crossgameprotocal::GameHiddenJieYiZuTanHeVote *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->TanHeVote(req->role_id,req->target_uid);
}

void SceneManager::OnGameHiddenJieYiZuInviteReply(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenJieYiZuInviteReply * req = (crossgameprotocal::GameHiddenJieYiZuInviteReply *)data;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByZuId(req->jie_yi_zu_id);
	if (!jie_yi_zu)
	{
		return;
	}

	jie_yi_zu->JieYiZuInviteReply(req->role_id, req->is_agree);
}

void SceneManager::OnGameHiddenJieYiRoleXuanYan(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenJieYiRoleXuanYan * req = (crossgameprotocal::GameHiddenJieYiRoleXuanYan *)data;
	JieYiZuManager::Instance().RoleXuanYan(req->role_id, req->is_delete, &req->text);
}

void SceneManager::OnGameHiddenJieYiZuXuanYan(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenJieYiZuXuanYan * req = (crossgameprotocal::GameHiddenJieYiZuXuanYan *)data;
	JieYiZuManager::Instance().JieYiZuXuanYan(req->role_id, req->text, req->is_delete);
}

void SceneManager::OnCrossHiddenCrossPersonRankEndlessRet(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::CrossHiddenEndlessRankRewardRet * req = (crossgameprotocal::CrossHiddenEndlessRankRewardRet *)data;

	for (int rank_id = 0; rank_id < req->count && rank_id < CHIVALROUS_RANK_LIST_NUM; rank_id++)
	{
		PersonRankItem *user_node = &req->rank_list[rank_id];
		const EndlessTowerRankRewardCfg * rank_reward_cfg = LOGIC_CONFIG->GetEndlessTowerConfig()->GetRankRewardCfg(rank_id + 1);
		if (NULL != rank_reward_cfg)
		{
			static MailContentParam contentparam; contentparam.Reset();
			for (int award_idx = 0; award_idx < rank_reward_cfg->reward_vec.size() && award_idx < MAX_ATTACHMENT_ITEM_NUM; ++award_idx)
			{
				const ItemConfigData & award_obj = rank_reward_cfg->reward_vec[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}	

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_endless_tower_rank_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_endless_tower_rank_reward_content, rank_id + 1);
			if (length1 <= 0 || length2 <= 0) continue;

			MailRoute::MailToUser(user_node->role_info.uid, MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void SceneManager::OnGameHiddenJieYiXuanYanList(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::GameHiddenJieYiXuanYanList * req = (crossgameprotocal::GameHiddenJieYiXuanYanList *)data;
	int role_id = req->role_id;
	if (req->type == JIE_YI_XUAN_YAN_TYPE_ROLE)
	{
		JieYiZuManager::Instance().SendRoleXuanYan(role_id);
	}

	if (req->type == JIE_YI_XUAN_YAN_TYPE_ZU)
	{
		JieYiZuManager::Instance().SendJieYiZuXuanYan(role_id);
	}
}

void SceneManager::OnHiddenGameJieYiZuSyncRoleData(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameJieYiSyncRoleData * req = (crossgameprotocal::HiddenGameJieYiSyncRoleData *)data;
	int role_id = req->role_data.role_id;
	JieYiZu * jie_yi_zu = JieYiZuManager::Instance().GetJieYiZuByRoleId(role_id);
	if (jie_yi_zu)
	{
		jie_yi_zu->SetSyncRoleData(req->role_data);
	}
}

void SceneManager::OnHiddenGameJieYiZuSyncRoleOnlineStatus(const char * data, int length)
{

}

void SceneManager::OnHiddenGameJieYiZuRoleChange(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameJieYiRoleChange * msg = (crossgameprotocal::HiddenGameJieYiRoleChange *)data;
	Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		OfflineRegisterManager::Instance().OnJieYiRoleChange(msg->role_id, msg->join_jie_yi_day);
	}
	else
	{
		EventHandler::Instance().OnJieYiRoleChnage(role, msg->join_jie_yi_day);
	}
}

void SceneManager::OnGameHiddenQingYuanCreate(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanCreate * req = (crossgameprotocal::GameHiddenQingYuanCreate *)data;
	std::vector<CommonSaveQingYuanRoleInfoItem> vec;
	for (int i = 0; i < ARRAY_LENGTH(req->list); ++i)
	{
		CommonSaveQingYuanRoleInfoItem & curr = req->list[i];
		vec.push_back(curr);

		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | create_qing_yuan role_id:%d name:%s", __FUNCTION__, __LINE__, curr.role_id, curr.role_name);
	}

	QingYuanManager::Instance().QingYuanCreate(req->item_type, vec);
}

void SceneManager::OnHiddenGameQingYuanDismiss(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameQingYuanDismiss * req = (crossgameprotocal::HiddenGameQingYuanDismiss *)data;
	QingYuanManager::Instance().DismissQingYuan(req->qing_yuan_id, __FUNCTION__);

	gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld", __FUNCTION__, __LINE__, req->qing_yuan_id);
}

void SceneManager::OnGameHiddenQingYuanLeave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanLeave * req = (crossgameprotocal::GameHiddenQingYuanLeave *)data;
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByQingYuanId(req->qing_yuan_id);
	if (qy)
	{
		ret = qy->SetLeave();
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanSingleLeave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanSingleLeave * req = (crossgameprotocal::GameHiddenQingYuanSingleLeave *)data;
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->LeaveSingleFromGame(req->role_id);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanForceLeave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanForceLeave * req = (crossgameprotocal::GameHiddenQingYuanForceLeave *)data; 
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->LeaveForceFromGame(req->role_id);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanCancelLeave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanCancelLeave * req = (crossgameprotocal::GameHiddenQingYuanCancelLeave *)data;
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->LeaveCancelFromGame(req->role_id);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanGiftGave(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanGiftGive * req = (crossgameprotocal::GameHiddenQingYuanGiftGive *)data;
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->GiveGiftFromGame(req->role_id, req->gift_type);	
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanGiftFetchReward(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanGiftFetchReward * req = (crossgameprotocal::GameHiddenQingYuanGiftFetchReward *)data;

	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->GiftFetchFromGame(req->role_id);		
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnHiddenGameQingYuanGiftFetchRewardRet(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQingYuanGiftFetchRewardRet * req = (crossgameprotocal::HiddenGameQingYuanGiftFetchRewardRet *)data;
	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->GiftFetchFromGameRet(req->role_id,req->gift_type);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanGiftFetchDayReward(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanGiftFetchDayReward * req = (crossgameprotocal::GameHiddenQingYuanGiftFetchDayReward *)data;

	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->GiveFetchDayFromGame(req->role_id);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnHiddenGameQingYuanGiftFetchDayRewardRet(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQingYuanGiftFetchDayRewardRet * req = (crossgameprotocal::HiddenGameQingYuanGiftFetchDayRewardRet *)data;

	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->GiveFetchDayFromGameRet(req->role_id, req->gift_type);;
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnGameHiddenQingYuanSetSkillFlag(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQingYuanSetSkillFlag * req = (crossgameprotocal::GameHiddenQingYuanSetSkillFlag *)data;

	int ret = 1;
	QingYuan * qy = QingYuanManager::Instance().GetQingYuanByRoleId(req->role_id);
	if (qy)
	{
		ret = qy->SetSkillNoEffectFlagFromGame(req->role_id, req->skill_flag);
	}

	if (ret != 0)
	{
		long long qing_yuan_id = qy ? qy->GetQingYuanId() : 0;
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d | qing_yuan_id:%lld role_id:%d ret:%d", __FUNCTION__, __LINE__, qing_yuan_id, req->role_id, ret);
	}
}

void SceneManager::OnHiddenGameQingYuanSync(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQingYuanSyncInfo * req = (crossgameprotocal::HiddenGameQingYuanSyncInfo *)data;
	if (req->sync_type == QING_YUAN_SYNC_TYPE_START)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  QING_YUAN_SYNC_TYPE_START", __FUNCTION__, __LINE__);
		QingYuanManager::Instance().OnSyncQingYuanInfoFromHiddenStart();
	}

	if (req->sync_type == QING_YUAN_SYNC_TYPE_UPDATE)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  QING_YUAN_SYNC_TYPE_UPDATE qing_yuan_id:%lld", __FUNCTION__, __LINE__,req->qing_yuan_info.qing_yuan_id);

		QingYuanManager::Instance().OnSyncQingYuanInfoFromHidden(req->qing_yuan_info, req->is_create, req->item_type);
	}

	if (req->sync_type == QING_YUAN_SYNC_TYPE_END)
	{
		gamelog::g_log_qing_yuan.printf(LL_INFO, "%s line:%d |  JIE_YI_ZU_SYNC_TYPE_END", __FUNCTION__, __LINE__);
		QingYuanManager::Instance().OnSyncQingYuanInfoFromHiddenEnd();
	}
}

void SceneManager::OnHiddenGameSyncRoleBattleData(const char* data, int length)
{
	crossgameprotocal::HiddenGameSyncRoleBattleData* msg = (crossgameprotocal::HiddenGameSyncRoleBattleData*)data;

	static crossgameprotocal::HiddenGameSyncRoleBattleData real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	switch (real_msg.business_type)
	{
	case crossgameprotocal::HiddenGameSyncRoleBattleData::BUSINESS_TYPE_COLOSSEUM:
		{
			Gladiator* gladiator = ColosseumManager::Instance().GetUserGladiator(IntToUid(real_msg.role_id));
			if (NULL != gladiator)
			{
				gladiator->SyncRoleBattleData(real_msg.role_battle_data);
				ColosseumManager::Instance().SendRoleInfoToHidden(real_msg.role_id);
			}
		}
		break;
	}
}

void SceneManager::OnGameHiddenGiveFlowerChange(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenGiveFlowerChange * msg = (crossgameprotocal::GameHiddenGiveFlowerChange *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		crossgameprotocal::HiddenGameGiveFlowerChange hggfc;
		hggfc.role_id = msg->role_id;

		InternalComm::Instance().SendToGameThroughCross2((const char * )&hggfc, sizeof(hggfc), IntToUid(hggfc.role_id).db_index);
	}
	else
	{
		EventHandler::Instance().OnFetchFriendGift(role);
	}
}

void SceneManager::OnHiddenGameGiveFlowerChange(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	crossgameprotocal::HiddenGameGiveFlowerChange * msg = (crossgameprotocal::HiddenGameGiveFlowerChange *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL == role)
	{
		OfflineRegisterManager::Instance().OnReceviceFlower(msg->role_id);
	}
	else
	{
		EventHandler::Instance().OnFetchFriendGift(role);
	}
}

void SceneManager::OnHiddenGameTianDiDaoHenSyncInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameTianDiDaoHenSyncInfo * info = (crossgameprotocal::HiddenGameTianDiDaoHenSyncInfo *)data;
	TianDiDaoHen::Instance().SetInfo(info->state, info->next_state_time);
}

void SceneManager::OnGameHiddenColosseumRankInfoSyn(const char * data, int length)
{
	if(!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenColosseumRankInfoSyn * msg = (crossgameprotocal::GameHiddenColosseumRankInfoSyn *)data;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->OnSetColosseumInfo(msg->colosseum_rank);
	}
}

void SceneManager::OnSyncFriendIntimacyInfo(const char* data, int length)
{
	crossgameprotocal::SyncFriendIntimacyInfo * info = (crossgameprotocal::SyncFriendIntimacyInfo*)data;

	FriendIntimacyManager::Instance().OnSyncFriendIntimacyInfo(info);
}

void SceneManager::OnGameHiddenAddIntimacyReq(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenAddIntimacyReq * req = (crossgameprotocal::GameHiddenAddIntimacyReq*)data;

	FriendIntimacyManager::Instance().OnAddIntimacyReqFromGameWorld(req);
}

void SceneManager::OnGameHiddenIntimacyRelationReq(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenIntimacyRelationReq * req = (crossgameprotocal::GameHiddenIntimacyRelationReq*)data;

	FriendIntimacyManager::Instance().OnIntimacyRelationReqFromGameWorld(req);
}

void SceneManager::OnGameHiddenIntimacySingleChatTimes(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenIntimacySingleChatTimes * chat_times = (crossgameprotocal::GameHiddenIntimacySingleChatTimes*)data;

	FriendIntimacyManager::Instance().OnSetIntimacySingleChatTimesFromGameWorld(chat_times);
}

void SceneManager::OnGameHiddenGiveFlowerNotice(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenGiveFlowerNotice * notice = (crossgameprotocal::GameHiddenGiveFlowerNotice*)data;

	FriendIntimacyManager::Instance().OnGiveFlowerNoticeFromGameWorld(notice);
}

void SceneManager::OnHiddenGameGiveFlowerNotice(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameGiveFlowerNotice * notice = (crossgameprotocal::HiddenGameGiveFlowerNotice*)data;

	FriendIntimacyManager::Instance().OnGiveFlowerNoticeFromHidden(notice);
}

void SceneManager::OnGameHiddenGiveFlowerBackKiss(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenGiveFlowerBackKiss * back_kiss = (crossgameprotocal::GameHiddenGiveFlowerBackKiss*)data;

	FriendIntimacyManager::Instance().OnGiveFlowerBackKissFromGameWorld(back_kiss);
}

void SceneManager::OnHiddenGameGiveFlowerBackKiss(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameGiveFlowerBackKiss * back_kiss = (crossgameprotocal::HiddenGameGiveFlowerBackKiss*)data;

	FriendIntimacyManager::Instance().OnGiveFlowerBackKissFromHidden(back_kiss);
}

void SceneManager::OnHiddenGameHorcruxTianChengReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameHorcruxTianChengReq * req = (crossgameprotocal::HiddenGameHorcruxTianChengReq *)data;
	if (req->req_type == crossgameprotocal::HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_DRAW)
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld horcrux_tiancheng draw from hidden", __FUNCTION__, __LINE__, req->role_id, req->req_id);
	}

	if (!LocalConfig::Instance().IsBelongServer(req->role_id))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld", __FUNCTION__, __LINE__, req->role_id, req->req_id);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityHorcruxTianCheng * act = (RandActivityHorcruxTianCheng *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	if (NULL == act) return;

	switch (req->req_type)
	{
	case crossgameprotocal::HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_INFO:
		{
			act->GameHiddenSendInfo(req->role_id);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_DRAW:
		{
			int ret = act->DrawFromHidden(req->role_id, req->req_id, req->rand_times, &req->floor_bitmap);
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld horcrux_tiancheng draw ret:%d", __FUNCTION__, __LINE__, req->role_id, req->req_id, ret);
		}
		break;
	}
}

void SceneManager::OnGameHiddenHorcruxTianChengDrawRet(const char * data, int length)
{
	TLVUnserializer2 unserializer;
	unserializer.Reset(data, length);
	crossgameprotocal::GameHiddenHorcruxTianChengDrawRet ret;
	if (!ret.Unserialize(unserializer))
	{
		return;
	}

	std::string log_str;
	for (int i = 0; i < (int)ret.items.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", ret.items[i].item_id, ret.items[i].num, ret.items[i].is_bind);
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld seq:%d money_type:%d num:%d %s", __FUNCTION__, __LINE__, ret.role_id, ret.req_id, ret.seq, ret.money_type, ret.reward_num, log_str.c_str());

	if (!ret.broadcast_str.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", ret.broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(ret.role_id);
	if (role != NULL)
	{
		if (ret.money_type == MONEY_TYPE_GOLD)
		{
			role->GetRoleModuleManager()->GetMoney()->AddGold(ret.reward_num, __FUNCTION__, false, false, true);
		}
		else if (ret.money_type > MONEY_TYPE_COIN_BIND)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(ret.reward_num, ret.money_type, __FUNCTION__, false, true);
		}

		if (!ret.items.empty())
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporary((short)ret.items.size(), &ret.items[0]))
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)ret.items.size(), &ret.items[0], PUT_REASON_RA_HORCRUX_TIANCHENG_DRAW);
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->SendMail(&ret.items[0], (int)ret.items.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
			}
		}

		Protocol::SCRAHorcruxTianChengDrawResult protocol;
		protocol.seq = ret.seq;
		protocol.num = ret.is_jackpot != 1 ? 0 : ret.reward_num;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

		{
			if (ret.money_type >= 0)
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList(0, NULL, GET_REASON_HORCRUX_TIANCHENG, ret.money_type, ret.reward_num);
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(ret.items, GET_REASON_HORCRUX_TIANCHENG, -1);
			}
		}

		RoleActivityHorcruxTianCheng * role_activity = (RoleActivityHorcruxTianCheng *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
		if (NULL != role_activity)
		{
			role_activity->OnAddRandTimes(1);
			if (ret.floor_back == 1)
			{
				//role_activity->ClearRandTimes();
			}
		}
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		if (ret.money_type >= 0) contentparam.virtual_item_list.virtual_item[ret.money_type] = ret.reward_num;
		FillMailContentParam(contentparam, ret.items);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(ret.role_id, MAIL_REASON_DEFAULT, contentparam);
		}

		OfflineRegisterManager::Instance().OnSetHorcruxTianCheng(IntToUid(ret.role_id), 1, ret.floor_back);
	}
}

void SceneManager::OnGameHiddenQueryItemRecommendPrice(const char* data, int length)
{
	crossgameprotocal::GameHiddenQueryItemRecommendPrice* msg = (crossgameprotocal::GameHiddenQueryItemRecommendPrice*)data;

	TradeMarketManager::Instance().OnOriginUserQueryItemRecommendPriceReq(msg);
}

void SceneManager::OnHiddenGameQueryTargetGladiatorBeforeFightReq(const char* data, int length)
{
	crossgameprotocal::HiddenGameQueryTargetGladiatorBeforeFightReq* msg = (crossgameprotocal::HiddenGameQueryTargetGladiatorBeforeFightReq*)data;

	ColosseumManager::Instance().OnHiddenGameQueryTargetGladiatorBeforeFight(msg->role_id, msg->target_index, msg->is_sweep);
}

void SceneManager::OnGameHiddenQueryTargetGladiatorBeforeFightResp(const char* data, int length)
{
	crossgameprotocal::GameHiddenQeuryTargetGladiatorBeforeFightResp* msg = (crossgameprotocal::GameHiddenQeuryTargetGladiatorBeforeFightResp*)data;
	
	static crossgameprotocal::GameHiddenQeuryTargetGladiatorBeforeFightResp real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	if (real_msg.is_sweep == 1)
	{
		ColosseumManager::Instance().OnGameHiddenQueryTargetGladiatorBeforeSweepResp(real_msg.role_id, real_msg.target_index, real_msg.target_uid, real_msg.user_total_capability, real_msg.target_total_capability);
		return;
	}

	ColosseumManager::Instance().OnGameHiddenQueryTargetGladiatorBeforeFightResp(real_msg.role_id, real_msg.target_index, real_msg.target_uid, real_msg.target_battle_data);
}

void SceneManager::OnHiddenGameColosseumSysReport(const char* data, int length)
{
	crossgameprotocal::HiddenGameColosseumFightSysReport* msg = (crossgameprotocal::HiddenGameColosseumFightSysReport*)data;
	ColosseumManager::Instance().SysReportFromHidden(msg->role_id, msg->target_uid, msg->is_win > 0, msg->stat, msg->is_sweep);
}

void SceneManager::OnGameHiddenColosseumSyncNewRankAfterSysReport(const char* data, int length)
{
	crossgameprotocal::GameHiddenColosseumSyncNewRankAfterSysReport* msg = (crossgameprotocal::GameHiddenColosseumSyncNewRankAfterSysReport*)data;
	ColosseumManager::Instance().SyncNewRankFromGameAfterSysReportFromHidden(msg->role_id, msg->target_uid, msg->is_win > 0, msg->is_in_pool > 0, msg->target_is_in_pool > 0, msg->old_rank, msg->new_rank, msg->is_sweep);
}

void SceneManager::OnHiddenGameJiXingGaoZhaoReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameJiXingGaoZhaoReq * req = (crossgameprotocal::HiddenGameJiXingGaoZhaoReq *)data;
	if (req->req_type == crossgameprotocal::HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_DRAW)
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld jixing_gaozhao draw from hidden", __FUNCTION__, __LINE__, req->role_id, req->req_id);
	}

	if (!LocalConfig::Instance().IsBelongServer(req->role_id))
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld", __FUNCTION__, __LINE__, req->role_id, req->req_id);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityJiXingGaoZhao * act = (RandActivityJiXingGaoZhao *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	if (NULL == act) return;

	switch (req->req_type)
	{
	case crossgameprotocal::HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_INFO:
		{
			act->GameHiddenSendInfo(req->role_id);
		}
		break;
	case crossgameprotocal::HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_DRAW:
		{
			int ret = act->DrawFromHidden(req->role_id, req->req_id, req->rand_times, &req->floor_bitmap);
			gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld jixing_gaozhao draw ret:%d", __FUNCTION__, __LINE__, req->role_id, req->req_id, ret);
		}
		break;
	}
}

void SceneManager::OnGameHiddenJiXingGaoZhaoDrawRet(const char * data, int length)
{
	TLVUnserializer2 unserializer;
	unserializer.Reset(data, length);
	crossgameprotocal::GameHiddenJiXingGaoZhaoDrawRet ret;
	if (!ret.Unserialize(unserializer))
	{
		return;
	}

	std::string log_str;
	for (int i = 0; i < (int)ret.items.size(); ++i)
	{
		log_str += STRING_SPRINTF("[item_id:%d num:%d is_bind:%d]", ret.items[i].item_id, ret.items[i].num, ret.items[i].is_bind);
	}
	gamelog::g_log_rand_activity.printf(LL_INFO, "%s %d | role_id:%d req_id:%lld seq:%d reward_type:%d num:%d %s", __FUNCTION__, __LINE__, ret.role_id, ret.req_id, ret.seq, ret.reward_type, ret.reward_num, log_str.c_str());

	if (!ret.broadcast_str.empty())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), "%s", ret.broadcast_str.c_str());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE));
		}
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(ret.role_id);
	if (role != NULL)
	{
		if (ret.reward_type == MONEY_TYPE_GOLD)
		{
			role->GetRoleModuleManager()->GetMoney()->AddGold(ret.reward_num, __FUNCTION__, false, false, true);
		}

		if (!ret.items.empty())
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPutListWithoutTemporary((short)ret.items.size(), &ret.items[0]))
			{
				role->GetRoleModuleManager()->GetKnapsack()->PutList((short)ret.items.size(), &ret.items[0], PUT_REASON_RA_JIXING_GAOZHAO_DRAW);
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->SendMail(&ret.items[0], (int)ret.items.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
			}
		}

		Protocol::SCRAJiXingGaoZhaoDrawResult protocol;
		protocol.seq = ret.seq;
		protocol.num = ret.is_jackpot != 1 ? 0 : ret.reward_num;

		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));

		{
			if (ret.reward_type > 0)
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList(0, NULL, GET_REASON_JIXING_GAOZHAO, ret.reward_num);
			}
			else
			{
				role->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemListVec(ret.items, GET_REASON_JIXING_GAOZHAO, -1);
			}
		}

		RoleActivityJiXingGaoZhao * role_activity = (RoleActivityJiXingGaoZhao *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
		if (NULL != role_activity)
		{
			role_activity->OnAddRandTimes(1);
			if (ret.floor_back == 1)
			{
				role_activity->ClearRandTimes();
			}
		}
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		if (ret.reward_type >= 0) contentparam.virtual_item_list.virtual_item[ret.reward_type] = ret.reward_num;
		FillMailContentParam(contentparam, ret.items);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_act_full_bag_title, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_act_full_bag_content, RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(ret.role_id, MAIL_REASON_DEFAULT, contentparam);
		}

		OfflineRegisterManager::Instance().OnSetJiXingGaoZhao(IntToUid(ret.role_id), 1, ret.floor_back);
	}
}

void SceneManager::OnCrossHiddenCrossPersonRankFallenGodRet(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::CrossHiddenFallenGodRankRewardRet * req = (crossgameprotocal::CrossHiddenFallenGodRankRewardRet *)data;

	int vec_num = 0;
	std::vector<RoleBaseInfo*> user_vec[MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM];
	
	int rank_value = 0;//前一个玩家的星灵SN
	int rank_param = 0;//前一个玩家的通关回合数
	for (int rank_id = 0; rank_id < req->count && rank_id < MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM; rank_id++)
	{
		if (vec_num < 0 || vec_num >= MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM)continue;
		PersonRankItem &user_node = req->rank_list[rank_id];
		if (rank_value == 0 && rank_param == 0)
		{
			rank_value = user_node.rank_value;
			rank_param = user_node.flexible_ll;
			vec_num++;
		}

		if (rank_value != user_node.rank_value || rank_param != user_node.flexible_ll)
		{
			rank_value = user_node.rank_value;
			rank_param = user_node.flexible_ll;
			vec_num++;
			if (vec_num > 0 && vec_num <= MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM)
			{
				user_vec[vec_num - 1].push_back(&user_node.role_info);
			}
		}
		else
		{
			if (vec_num > 0 && vec_num <= MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM)
			{
				user_vec[vec_num - 1].push_back(&user_node.role_info);
			}
		}
	}
	const FallenGodOtherCfg & other_cfg = LOGIC_CONFIG->GetFallenGodConfig()->GetOtherCfg();
	for (int idx = 0; idx < MAX_FALLEN_GOD_REWARD_MEMBER_LIST_NUM && idx < vec_num; idx++)
	{
		for (std::vector<RoleBaseInfo*>::iterator it = user_vec[idx].begin(); it != user_vec[idx].end(); it++)
		{
			{
				static MailContentParam contentparam; contentparam.Reset();
				contentparam.item_list[0] = ItemDataWrapper(other_cfg.title_id, 1, true);

				int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_fallen_god_rank_reward_subject);
				int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_fallen_god_rank_reward_content, idx + 1);
				if (length1 <= 0 || length2 <= 0) continue;

				MailRoute::MailToUser(IntToUid((*it)->uid), MAIL_REASON_DEFAULT, contentparam);
				gamelog::g_log_title.printf(LL_INFO, "SceneManager::OnCrossHiddenCrossPersonRankFallenGodRet user[%s,%d] idx[%d]", (*it)->user_name, (*it)->uid, idx);
			}
		}
	}
}

void SceneManager::OnGameHiddenWeddingRoleReq(const char * data, int length)
{
	crossgameprotocal::GameHiddenWeddingRoleReq * req = (crossgameprotocal::GameHiddenWeddingRoleReq *)data;

	WeddingManger::Instance().GameHiddenRoleReq(req);
}

void SceneManager::OnHiddenGameWeddingInfoSync(const char * data, int length)
{
	crossgameprotocal::HiddenGameWeddingInfoSync * req = (crossgameprotocal::HiddenGameWeddingInfoSync *)data;

	WeddingLocalManger::Instance().OnSycnWeddingInfo(req);
}

void SceneManager::OnGameHiddenQingYuanCommonData(const char * data, int length)
{
	crossgameprotocal::GameHiddenQingYuanCommonData * req = (crossgameprotocal::GameHiddenQingYuanCommonData *)data;

	QingYuanManager::Instance().UpDataQingYuanCommond(req->qingyuan_id, req->common_data);
}

void SceneManager::OnGameHiddenQingYuanLeiChongAddChongZhi(const char * data, int length)
{
	crossgameprotocal::GameHiddenQingYuanLeiChongAddChongZhi * req = (crossgameprotocal::GameHiddenQingYuanLeiChongAddChongZhi *)data;

	QingYuanManager::Instance().UpDataQingYuanLeiChongAddChongZhi(req->qingyuan_id, req->add_chongzhi);
}

void SceneManager::OnGameHiddenWorldArenaUserOperate(const char * data, int length)
{
	crossgameprotocal::GameHiddenWorldArenaOperate * req = (crossgameprotocal::GameHiddenWorldArenaOperate *)data;

	WorldArena::Instance().OnUserOperateFromOrigin(IntToUid(req->uid), req->oper_type, req->param1, req->param2);
}

void SceneManager::OnGameHiddenWorldArenaSignup(const char* data, int length)
{
	static crossgameprotocal::GameHiddenWorldArenaSignup real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	WorldArena::Instance().OnUserSignUp(IntToUid(real_msg.signup_info.uid), real_msg.signup_info, real_msg.role_battle_data);
}

void SceneManager::OnHiddenGameWorldArenaGiveWinBoxReward(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldArenaGiveWinBoxReward* msg = (crossgameprotocal::HiddenGameWorldArenaGiveWinBoxReward*)data;

	WorldArena::Instance().GiveWinBoxRewardAtOrigin(IntToUid(msg->uid), msg->sn);
}

void SceneManager::OnHiddenGameWorldArenaGiveSignupReward(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldArenaGiveSignupReward* msg = (crossgameprotocal::HiddenGameWorldArenaGiveSignupReward*)data;

	WorldArena::Instance().GiveSignupRewardAtOrigin(IntToUid(msg->uid));
}

void SceneManager::OnHiddenGameWorldArenaGiveDailyReward(const char* data, int length)
{
	crossgameprotocal::HiddenGameWorldArenaGiveDailyReward* msg = (crossgameprotocal::HiddenGameWorldArenaGiveDailyReward*)data;

	WorldArena::Instance().GiveDailyRewardAtOrigin(IntToUid(msg->uid));
}

void SceneManager::OnGameHiddenWorldArenaUpdateSignupInfo(const char* data, int length)
{
	static crossgameprotocal::GameHiddenWorldArenaUpdateSignupInfo real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset)) return;

	WorldArena::Instance().UpdateUserSignupInfo(real_msg.signup_info.uuid, real_msg.signup_info);
}

void SceneManager::OnCrossGameWorldTeamArenaQueryRankRet(const char * data, int length)
{
	crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet * msg = (crossgameprotocal::CrossGameWorldTeamArenaQueryRankRet *)data;

	if (0 == msg->send_type)
	{
		if (Protocol::SCWorldTeamArenaTeamSignUpDetailedInfo::NOTICE_TYPE_RANK_END == msg->notice_type)
		{
			WorldTeamArenaRankManager::Instance().OnGameRecvCrossQueryRankRet(msg);
		}
		else
		{
			WorldTeamArenaSignUpManager::Instance().OnGameRecvCrossQueryRankRet(msg);
		}
	}
	else
	{
		WorldTeamArenaSignUpManager::Instance().OnGameRecvCrossQueryRankRetSendSpecialInfo(msg);
	}
	
}

void SceneManager::OnGameHiddenRoleInfoChangeWorldTeamArenaSyn(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn * msg = (crossgameprotocal::GameHiddenRoleInfoChangeWorldTeamArenaSyn *)data;
	WorldTeamArenaSignUpManager::Instance().OnHiddenRecvGameSynRoleInfo(msg);
	WorldTeamArenaRankManager::Instance().OnHiddenRecvGameSynRoleInfo(msg);
}

void SceneManager::OnHiddenGameWorldTeamArenaSynRoleTitleInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameWorldTeamArenaSynRoleTitleInfo * msg = (crossgameprotocal::HiddenGameWorldTeamArenaSynRoleTitleInfo *)data;
	WorldTeamArenaRankManager::Instance().OnRoleTitleInfoChange(msg->role_id, msg->reason, msg->info.world_team_arena_rank, msg->info.title_id);
}

void SceneManager::OnHiddenGameWorldTeamArenaSynActivityInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameWorldTeamArenaSynActivityInfo * msg = (crossgameprotocal::HiddenGameWorldTeamArenaSynActivityInfo *)data;
	ActivityWorldTeamArena* activity = (ActivityWorldTeamArena*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity)
	{
		return;
	}

	activity->OnSetActivityInfo(msg->activity_info);
}

void SceneManager::OnGameHiddenWorldTeamArenaActivityInfoSynReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenWorldTeamArenaActivityInfoSynReq * msg = (crossgameprotocal::GameHiddenWorldTeamArenaActivityInfoSynReq *)data;
	ActivityWorldTeamArena* activity = (ActivityWorldTeamArena*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity)
	{
		return;
	}

	activity->OnHiddenSynActivityInfoToGame(msg->server_id);
}

void SceneManager::OnGameHiddenWorldTeamArenaFinallyRankReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq * msg = (crossgameprotocal::GameHiddenWorldTeamArenaFinallyRankReq *)data;
	WorldTeamArenaRankManager::Instance().OnHiddenRecvGameFinallyRankReq(msg);
}

void SceneManager::OnGameHiddenQingYuanDuiDuiPengUpDataRankInfo(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenQingYuanDuiDuiPengUpDataRankInfo * msg = (crossgameprotocal::GameHiddenQingYuanDuiDuiPengUpDataRankInfo *)data;
	QingYuanDuiDuiPengManager::Instance().OnRoleUpDate(msg->role_id, msg->name, msg->jifen, msg->pass_times);
}

void SceneManager::OnGameHiddenQingYuanDuiDuiPengRankReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenQingYuanDuiDuiPengRankReq * msg = (crossgameprotocal::GameHiddenQingYuanDuiDuiPengRankReq *)data;
	QingYuanDuiDuiPengManager::Instance().SendToGameRankInfo(msg->role_id);
}

void SceneManager::OnHiddenGameQingYuanDuiDuiPengRankInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameQingYuanDuiDuiPengUpRankInfo * msg = (crossgameprotocal::HiddenGameQingYuanDuiDuiPengUpRankInfo *)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->role_id);
	if (NULL != role)
	{
		Protocol::SCQingYuanDuiDuiRankInfo info;
		info.count = msg->count;
		memcpy(info.rank_list, msg->rank_list, sizeof(Protocol::SCQingYuanDuiDuiRankInfo::RoleData) * info.count);

		int len = sizeof(info) - sizeof(info.rank_list) + info.count * sizeof(Protocol::SCQingYuanDuiDuiRankInfo::RoleData);
		role->GetRoleModuleManager()->NetSend(&info, len);
	}
}

void SceneManager::OnHiddenGameSyncHiddenOpenServerTime(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameSyncHiddenOpenServerTime * msg = (crossgameprotocal::HiddenGameSyncHiddenOpenServerTime *)data;
	WorldStatus::Instance().SetSyncHiddenOpenServerTime(msg->hidden_open_server_time);
}

void SceneManager::GameHiddenGuildSpeedUpInfo(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSynGuildSpeedUpInfo * msg = (crossgameprotocal::GameHiddenSynGuildSpeedUpInfo *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnGameHiddenRecviceSpeedUp(msg);
	}
}

void SceneManager::HiddenGameGuildSpeedUpInfo(const char * data, int length)
{
	crossgameprotocal::HiddenGameSynGuildSpeedUpInfo * msg = (crossgameprotocal::HiddenGameSynGuildSpeedUpInfo *)data;
	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnHiddenGameRecviceSpeedUp(msg);
	}
}

void SceneManager::HiddenGameGuildSynBuildAllInfo(const char * data, int length)
{
	crossgameprotocal::GameHiddenSynGuildBuildAllInfo * msg = (crossgameprotocal::GameHiddenSynGuildBuildAllInfo *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnGameHiddenRecviceBuildAllInfo(msg);
	}
}

void SceneManager::OnSyncGuildBuildingNotifyInfo(const char * data, int length)
{
	crossgameprotocal::SyncGuildBuildingNotifyInfo * msg = (crossgameprotocal::SyncGuildBuildingNotifyInfo *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL != guild)
	{
		guild->OnSyncBuildingNotifyRecviceInfo(msg);
	}
}


void SceneManager::OnGameHiddenGetUTAChampionRecord(const char* data, int length)
{
	crossgameprotocal::GameHiddenGetUTAChampionRecord * get_utacr = (crossgameprotocal::GameHiddenGetUTAChampionRecord*)data;

	UTAChampionRecordManager::Instance().GameHiddenGetUTAChampionRecord(get_utacr);
}

void SceneManager::OnGameHiddenTransferMsgToBigCross(const char* data, int length)
{
	assert(CrossConfig::Instance().IsHiddenServer());

	if (length >= sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross) + sizeof(crossgameprotocal::MessageHeader))
	{
		const char* transfer_msg = data + sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross);
		int transfer_length = length - sizeof(crossgameprotocal::GameHiddenTransferMsgToBigcross);
		InternalComm::Instance().SendToBigCrossServer(transfer_msg, transfer_length);
	}
}

void SceneManager::OnHiddenGameGiveLotteryToRole(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameGiveLotteryToRole * hg_to_role = (crossgameprotocal::HiddenGameGiveLotteryToRole*)data;

	UTAGuessShadow::Instance().GiveLotteryToRoleFromHidden(hg_to_role);
}

void SceneManager::OnHiddenGameUTAGiveUserItems(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameUTAGiveItemsToUser real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset))
	{
		return;
	}

	UTAShadow::Instance().TryBestToGiveUserItems(real_msg.role_id, real_msg.put_reason_type, real_msg.itemlist, real_msg.mail_subject, real_msg.mail_content, real_msg.only_send_mail > 0);
}

void SceneManager::OnHiddenGameUTAGiveServerItems(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		assert(0);
		return;
	}

	crossgameprotocal::HiddenGameUTAGiveItemsToServer real_msg;
	int offset = 0;
	if (!real_msg.Deserialize(data, length, &offset))
	{
		return;
	}

	UTAShadow::Instance().OnUTAGiveItemsToServer(real_msg.itemlist, real_msg.mail_subject, real_msg.mail_content);
}

void SceneManager::OnHiddenGameUTASyncBaseInfo(const char* data, int length)
{
	crossgameprotocal::HiddenGameUTABaseInfoSync* msg = (crossgameprotocal::HiddenGameUTABaseInfoSync*)data;
	UTAShadow::Instance().OnSyncDataFromBigCross(msg->cur_status, msg->season_index, msg->round_idx, 
		msg->next_battle_timestamp, msg->active_reward_begin_timestamp, 
		msg->active_reward_end_timestamp, msg->season_end_timestamp);
}

void SceneManager::OnHiddenGameUTASyncSchedules(const char* data, int length)
{
	crossgameprotocal::HiddenGameUTASyncSchedules sync_msg;
	int offset = 0;
	if (!sync_msg.Deserialize(data, length, &offset))
	{
		assert(0);
		return;
	}

	bigchgameprotocol::BigCHGameSyncSchedules real_msg;
	int msg_type = real_msg.GetMessageType();
	offset = 0;
	if (!real_msg.Deserialize(data, length, &offset))
	{
		assert(0);
		return;
	}
	real_msg.header.msg_type = msg_type;
	UNSTD_STATIC_CHECK(sizeof(sync_msg) == sizeof(real_msg));
	UNSTD_STATIC_CHECK(sizeof(crossgameprotocal::MsgScheduleInfo) == sizeof(bigchgameprotocol::MsgScheduleInfo));
	
	UTAShadow::Instance().OnUTASyncSchedules(&real_msg);
}

void SceneManager::OnGameHiddenUserRequestBigcrossVideo(const char* data, int length)
{
	crossgameprotocal::GameHiddenUserRequestWatchBigCrossVideo* msg = (crossgameprotocal::GameHiddenUserRequestWatchBigCrossVideo*)data;
	UTAShadow::Instance().OnUserWatchVideoReq(IntToUid(msg->role_id), msg->video_id);
}

void SceneManager::OnHiddenGameTransferBigCrossMsgToOrigin(const char* data, int length)
{
	crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin* msg = (crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin*)data;

	const char* real_data = data + sizeof(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin);
	int real_length = length - sizeof(crossgameprotocal::HiddenGameTransferBigcrossMsgToOrigin);
	BigCrossManager::Instance().OnRecvMsg((NetID)(-1), real_data, real_length);		
}

void SceneManager::OnGameHiddenUserLoginInOrigin(const char* data, int length)
{
	crossgameprotocal::GameHiddenUserLoginInOrigin* msg = (crossgameprotocal::GameHiddenUserLoginInOrigin*)data;

	UTAShadow::Instance().OnUserLoginOrigin(IntToUid(msg->role_id));
}

void SceneManager::OnGameHiddenQueryWorldTeamArenaOtherInfo(const char * data, int length)
{
	crossgameprotocal::GameHiddenQueryWorldTeamArenaOtherInfo* msg = (crossgameprotocal::GameHiddenQueryWorldTeamArenaOtherInfo*)data;

	WorldTeamArenaSignUpManager::Instance().SendOtherInfoToGame(msg->role_id);
}

void SceneManager::OnGameHiddenGodBeastAdventBuyReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenGodBeastAdventReq * msg = (crossgameprotocal::GameHiddenGodBeastAdventReq *)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(UniqueServerID(msg->plat_type, msg->server_id));
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent* activity = (RandActivityGodBeastAdvent*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
	if (NULL == activity) return;

	int ret = activity->BuyServerLimitItem(msg->uid);

	if (ret < 0)
	{
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user_id[%d] buy failed, ret[%d]", __FUNCTION__, msg->uid, ret);
	}

	{	// 验证完把返回值传回原服处理
		crossgameprotocal::HiddenGameGodBeastAdventInfo info;
		info.uid = msg->uid;
		info.ret = ret;
		info.seq = msg->seq;
		InternalComm::Instance().SendToGameThroughCross2((const char *)&info, sizeof(info), IntToUid(msg->uid).db_index);
	}
}

void SceneManager::OnHiddenGameGodBeastAdventBuyInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameGodBeastAdventInfo * msg = (crossgameprotocal::HiddenGameGodBeastAdventInfo *)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	if (NULL != role)
	{
		RoleActivityGodBeastAdvent * role_activity = static_cast<RoleActivityGodBeastAdvent *>
			(role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT));
		if (NULL != role_activity)
		{
			role_activity->OnHiddenGameBuyRecviceReq(msg);
		}
	}

	gamelog::g_log_rand_activity.printf(LL_INFO, "%s ret[%d] seq[%d]", __FUNCTION__, msg->ret, msg->seq);
}

void SceneManager::OnHiddenGameGodBeastAdventNumInfo(const char * data, int length)
{
	if (NULL == data) return;

	crossgameprotocal::HiddenGameGodBeastAdventNumInfo * msg = (crossgameprotocal::HiddenGameGodBeastAdventNumInfo *)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent* activity = (RandActivityGodBeastAdvent*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
	if (NULL == activity) return;

	activity->SynHiddenGameReceiveNumInfo(msg);
}

void SceneManager::OnGameHiddenGodBeastAdventReceiveTradeMarketReq(const char * data, int length)
{
	if (NULL == data) return;

	crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq * msg = (crossgameprotocal::GameHiddenGodBeastAdventReceiveTradeMarketReq *)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityGodBeastAdvent* activity = (RandActivityGodBeastAdvent*)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
	if (NULL == activity) return;

	activity->ReceiveTradeMarket(msg);
}

void SceneManager::OnCrossHiddenUTAQueryTeamMemberRankResp(const char * data, int length)
{
	crossgameprotocal::CrossHiddenUTAQueryTeamMemberRankResp* msg = (crossgameprotocal::CrossHiddenUTAQueryTeamMemberRankResp*)data;
	
	bigchgameprotocol::HGameBigCUTATeamMemberRankResp resp;
	for (int i = 0; i < UTA_TEAM_MEMBER_NUM; ++i)
	{
		resp.rank_capability[i] = msg->rank_capability[i];
		resp.rank_level[i] = msg->rank_level[i];
		resp.rank_prof[i] = msg->rank_prof[i];
	}

	resp.unique_team_id = msg->unique_team_id;
	InternalComm::Instance().SendToBigCrossServer((const char*)&resp, sizeof(resp));
}

void SceneManager::OnGameHiddenHongMengTianJieReq(const char * data, int length)
{
	WorldHongMengTianJieManager::Instance().OnReq(data);
}

void SceneManager::OnHiddenGameServerCompetitionRankReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameServerCompetitionRankReq * req = (crossgameprotocal::HiddenGameServerCompetitionRankReq *)data;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	if (NULL == activity) return;

	activity->SendAllRankInfoToCross(req->role_uid);
}

void SceneManager::OnGameHiddenServerCompetitionRankResp(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenServerCompetitionRankResp * resp = (crossgameprotocal::GameHiddenServerCompetitionRankResp *)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(resp->role_uid));
	if (NULL == role) return;

	Protocol::SCRAServerCompetitionAllRankInfo msg;
	msg = resp->msg;

	role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
}

void SceneManager::OnGameHiddenSaltySweetChangeDataResp(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSaltySweetBattleChangeReq * resp = (crossgameprotocal::GameHiddenSaltySweetBattleChangeReq *)data;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(resp->unique_server_id);
	if (NULL == ramgr) return;

	RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
	activity->OnGameDataChange(resp->pre_win_type, resp->salty_value, resp->sweet_value, resp->status_mod, resp->to_uid);
}

void SceneManager::OnHiddenGameSaltySweetVoteReq(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;
	crossgameprotocal::HiddenGameSaltySweetBattleVoteReq * req = (crossgameprotocal::HiddenGameSaltySweetBattleVoteReq *)data;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;
	RandActivitySaltySweetBattle * activity = (RandActivitySaltySweetBattle *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
	activity->OnUidVote(req->add_type, req->add_value, req->role_uid);
}

void SceneManager::OnHiddenGameSyncTeamRoleRecruit(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameTeamRoleRecruitSync* msg = (crossgameprotocal::HiddenGameTeamRoleRecruitSync*)data;
	TeamRecruit::Instance().OnTeamRecruit(msg->team_id, msg->team_type,
		msg->leader_name, msg->limit_lv_low, msg->limit_lv_high,
		msg->cur_member_num, msg->max_member_num, true);
}

void SceneManager::OnGameHiddenSyncPetTopRankInfo(const char * data, int length)
{
	crossgameprotocal::GameHiddenSyncPetTopRankInfo real_msg;
	int offset = 0;
	if (real_msg.Deserialize(data, length, &offset))
	{
		RankManager::Instance().SyncPetTopRankFromOrigin(&real_msg);
	}
	else
	{
		assert(0);
	}
}

void SceneManager::OnGameHiddenSyncPartnerTopRankInfo(const char* data, int length)
{
	crossgameprotocal::GameHiddenSyncPartnerTopRankInfo real_msg;
	int offset = 0;
	if (real_msg.Deserialize(data, length, &offset))
	{
		RankManager::Instance().SyncPartnerTopRankFromOrigin(&real_msg);
	}
	else
	{
		assert(0);
	}
}

void SceneManager::OnCrossHiddenMeiLiRankResp(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::CrossHiddenMeiLiRankRewardRet * req = (crossgameprotocal::CrossHiddenMeiLiRankRewardRet *)data;
	
	const RandActivityCharismaticTanabataConfig * config = static_cast<const RandActivityCharismaticTanabataConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA));
	if (NULL == config)return;
	for (int rank_id = 0; rank_id < req->count && rank_id < CHIVALROUS_RANK_LIST_NUM; rank_id++)
	{
		PersonRankItem *user_node = &req->rank_list[rank_id];
		UniqueServerID us_id;
		us_id.plat_type = LocalConfig::Instance().GetPlatType();
		us_id.server_id = IntToUid(user_node->role_info.uid).db_index;
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(us_id);
		if (NULL == ramgr)return;
		int act_real_open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
		const CharismaticTanabataRewardConfig * rank_reward_cfg = config->GetRewardCfg(act_real_open_day, rank_id + 1);
		if (NULL != rank_reward_cfg)
		{
			static MailContentParam contentparam; contentparam.Reset();
			for (int award_idx = 0; award_idx < rank_reward_cfg->reward_vec.size() && award_idx < MAX_ATTACHMENT_ITEM_NUM; ++award_idx)
			{
				const ItemConfigData & award_obj = rank_reward_cfg->reward_vec[award_idx];
				contentparam.item_list[award_idx] = ItemDataWrapper(award_obj.item_id, award_obj.num, award_obj.is_bind ? 1 : 0);
			}

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_meili_qixi_rank_reward_subject);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_meili_qixi_rank_reward_content, rank_id + 1);
			if (length1 <= 0 || length2 <= 0) continue;

			MailRoute::MailToUser(user_node->role_info.uid, MAIL_REASON_DEFAULT, contentparam);
		}
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%s,%d] rank_idx[%d] value[%lld]", __FUNCTION__, user_node->role_info.user_name, user_node->role_info.uid,
			rank_id + 1, user_node->rank_value);
	}
}

void SceneManager::OnHiddenGameAddIntimacyValue(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::HiddenGameFriendInitmacy * req = (crossgameprotocal::HiddenGameFriendInitmacy *)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(req->to_uid));
	if (NULL != role)
	{
		RoleActivityCharismaticTanabata * activity = (RoleActivityCharismaticTanabata *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
		if (NULL != activity)
		{
			activity->OnRoleSendFlows(req->add_val);
		}
	}
	else
	{
		//玩家所在原服找不到，记录进离线offline
		OfflineRegisterManager::Instance().AddFriendIntimacy(IntToUid(req->to_uid), req->add_val);
	}
}

void SceneManager::OnGameHiddenAddIntimacyValue(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;
	crossgameprotocal::GameHiddenFriendInitmacy * req = (crossgameprotocal::GameHiddenFriendInitmacy *)data;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(req->to_uid));
	if (NULL != role)
	{//玩家在跨服
		RoleActivityCharismaticTanabata * activity = (RoleActivityCharismaticTanabata *)role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
		if (NULL != activity)
		{
			activity->OnRoleSendFlows(req->add_val);
		}
	}
	else
	{
		//来源原服的，再发回原服，并且记录进离线
		crossgameprotocal::HiddenGameFriendInitmacy req_back;
		req_back.to_uid = req->to_uid;
		req_back.add_val = req->add_val;
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(req->to_uid).db_index, (const char*)&req_back, sizeof(req_back));
	}
}

void SceneManager::OnHiddenGameQueryOriginServerRAStatus(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	crossgameprotocal::HiddenGameQueryOriginServerRAStatus* msg = (crossgameprotocal::HiddenGameQueryOriginServerRAStatus*)data;
	RandActivity* act = ramgr->ForceGetRandActivity(msg->query_activity_type);
	if (NULL == act) return;

	act->NotifyRandActivityStatusToHiddenUser(msg->query_uid);
}

void SceneManager::OnGameHiddenSendOriginServerRAStatus(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSendOriginServerRAStatus* msg = (crossgameprotocal::GameHiddenSendOriginServerRAStatus*)data;

	UniqueServerID usid(msg->send_to_plat_type, msg->send_to_server_id);
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(usid);
	if (NULL == ramgr) return;

	RandActivity* act = ramgr->ForceGetRandActivity(msg->activity_type);
	if (NULL == act) return;

	Protocol::SCActivityStatus as;
	as.activity_open_type = msg->open_type;
	as.activity_type = msg->activity_type;
	as.is_broadcast = 1;
	as.next_status_switch_time = msg->next_status_switch_time;
	as.status = static_cast<char>(msg->status);
	as.param_1 = msg->param_1;
	as.param_2 = msg->param_2;
	act->SendRAStatusToSpecificServerUsers(&as);
}

void SceneManager::OnHiddenGameCleanQuery(const char * data, int length)
{
	crossgameprotocal::HiddenGameClearQueryInfo* msg = (crossgameprotocal::HiddenGameClearQueryInfo*)data;
	QueryHandler::Instance().ClearUserCache(UidToInt(msg->clear_target_uid.user_id));
}

void SceneManager::OnGameHiddenCleanQuery(const char * data, int length)
{
	crossgameprotocal::GameHiddenClearQueryInfo* msg = (crossgameprotocal::GameHiddenClearQueryInfo*)data;
	QueryHandler::Instance().ClearUserCache(UidToInt(msg->clear_target_uid.user_id));
}

void SceneManager::OnGameHiddenCmdToRole(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenCmdToRole* req = (crossgameprotocal::GameHiddenCmdToRole*)data;

	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdToRole plat_type[%d] server_id[%d] role[%d,%s] effect_type:%d, effect_param:%d, effect_param1:%d]", 
			req->plat_type, req->server_id, role->GetUID(), role->GetName(), req->effect_type, req->effect_param, req->effect_param1);

		bool ret = System::Instance().OnCmdToRole(role, req->effect_type, req->effect_param, req->effect_param1);
		gamelog::g_log_cmd.printf(LL_INFO, "[CmdToRole %s! User[%d, %s], effect_type:%d, effect_param:%d, effect_param1:%d]",
			(ret ? "Succ" : "Fail"), role->GetUID(), role->GetName(), req->effect_type, req->effect_param, req->effect_param1);
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdToRole Role Not Found, Send To OriginGameWorld, plat_type[%d] server_id[%d] role[%d] effect_type:%d, effect_param:%d, effect_param1:%d]", req->plat_type, req->server_id, req->role_id, 
			req->effect_type, req->effect_param, req->effect_param1);

		crossgameprotocal::HiddenGameCmdToRole hgcmr;
		hgcmr.role_id = req->role_id;
		hgcmr.effect_type = req->effect_type;
		hgcmr.effect_param = req->effect_param;
		hgcmr.effect_param1 = req->effect_param1;

		InternalComm::Instance().SendToGameThroughCross(req->plat_type, req->server_id, (const char*)&hgcmr, sizeof(hgcmr));
	}
}

void SceneManager::OnHiddenGameCmdToRole(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameCmdToRole* req = (crossgameprotocal::HiddenGameCmdToRole*)data;

	Role* role = this->GetRole(IntToUid(req->role_id));
	if (NULL != role)
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnHiddenGameCmdToRole role[%d,%s] effect_type:%d, effect_param:%d, effect_param1:%d]",
			role->GetUID(), role->GetName(), req->effect_type, req->effect_param, req->effect_param1);

		bool ret = System::Instance().OnCmdToRole(role, req->effect_type, req->effect_param, req->effect_param1);
		gamelog::g_log_cmd.printf(LL_INFO, "[CmdToRole %s! User[%d, %s], effect_type:%d, effect_param:%d, effect_param1:%d]",
			(ret ? "Succ" : "Fail"), role->GetUID(), role->GetName(), req->effect_type, req->effect_param, req->effect_param1);
	}
	else
	{
		gamelog::g_log_debug.printf(LL_INFO, "OnGameHiddenCmdToRole Role Not Found, Send To OriginGameWorld, role[%d] effect_type:%d, effect_param:%d, effect_param1:%d]", req->role_id,
			req->effect_type, req->effect_param, req->effect_param1);

		if(!System::Instance().OnCmdToRole(req->role_id, req->effect_type, req->effect_param, req->effect_param1))
		{
			bool ret = System::Instance().OnCmdToOfflineRegister(req->role_id, req->effect_type, req->effect_param, req->effect_param1);
			gamelog::g_log_cmd.printf(LL_INFO, "[OnCmdToOfflineRegister %s! User[%d], effect_type:%d, effect_param:%d, effect_param1:%d]",
				(ret ? "Succ" : "Fail"), req->role_id, req->effect_type, req->effect_param, req->effect_param1);
		}
	}

}

void SceneManager::OnGameHiddenQueryOfflineBattleDataForRobot(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot* msg =
		(crossgameprotocal::GameHiddenQueryOfflineBattleDataForRobot*)data;

	OfflineBattleDataMgr::Instance().OnQueryOfflineBattleDataForRobot(msg, true);
}

void SceneManager::OnHiddenGameQueryOfflineBattleDataForRobot(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQueryOfflineBattleDataForRobot msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	TeamMatch::Instance().OnOfflineBattleDataForRobotResp(&msg);
}

void SceneManager::OnGameHiddenNotifyOfflineBattleDataForRobotResult(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult* msg =
		(crossgameprotocal::GameHiddenNotifyOfflineBattleDataForRobotResult*)data;

	OfflineBattleDataMgr::Instance().OnNotifyOfflineBattleDataForRobotResult(msg);
}

void SceneManager::OnGameHiddenRealRoleRobotLeaveTeamNotice(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenRealRoleRobotLeaveTeamNotice* msg =
		(crossgameprotocal::GameHiddenRealRoleRobotLeaveTeamNotice*)data;

	OfflineBattleDataMgr::Instance().OnRealRoleRobotLeaveTeamNotice(msg);
}

void SceneManager::OnGameHiddenSyncOfflineBattleData(const char* data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSyncOfflineBattleData msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	OfflineBattleDataMgr::Instance().OnSyncOfflineBattleData(&msg);
}

void SceneManager::OnHiddenGameQueryBattleDataForOfflineBattleDataSystem(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQueryBattleDataForOfflineBattleDataSystem msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	OfflineBattleDataMgr::Instance().OnQueryBattleDataForHiddenOfflineBattleDataSystem(&msg);
}

void SceneManager::OnHiddenGameAddSingleChatRecord(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameAddSingleChatRecord msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	ChatManager::Instance().OnHiddenGameAddSingleChatRecord(&msg);
}

void SceneManager::OnHiddenGameUserQueryChatRecord(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameQueryUserChatRecord msg;
	int offset = 0;
	if (!msg.Deserialize(data, length, &offset))
	{
		return;
	}

	ChatManager::Instance().OnHiddenGameQueryUserChatRecord(&msg);
}

void SceneManager::OnGameHiddenHuanShouAnnInfoReq(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::GameHiddenHuanShouAnnInfoReq * req = (crossgameprotocal::GameHiddenHuanShouAnnInfoReq *)data;
	WorldStatus::Instance().OnHiddenToGameHuanShouAnnInfo(req->role_uid);
}

void SceneManager::OnGameHiddenHuanShouAnnInfoSync(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::GameHiddenHuanShouAnnInfoSync * req = (crossgameprotocal::GameHiddenHuanShouAnnInfoSync *)data;
	WorldStatus::Instance().AddHuanShouAnnInfo(req->ann_info);
}

void SceneManager::OnHiddenGameHuanShouAnnInfoSync(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::HiddenGameHuanShouAnnInfoSync * req = (crossgameprotocal::HiddenGameHuanShouAnnInfoSync *)data;
	WorldStatus::Instance().AddHuanShouAnnInfo(req->ann_info);
}

void SceneManager::OnHiddenGameCourageChallengeRankReq(const char * data, int length)
{
	crossgameprotocal::HiddenGameCourageChallengeRankReq * req = (crossgameprotocal::HiddenGameCourageChallengeRankReq *)data;

	CourageChallengeRankManager::Instance().GameHiddenRankList(req);
}

void SceneManager::OnHiddenGameCourageChallengRankRoleChange(const char * data, int length)
{
	crossgameprotocal::HiddenGameCourageChallengeRankRoleChange *msg = (crossgameprotocal::HiddenGameCourageChallengeRankRoleChange *)data;

	CourageChallengeRankManager::Instance().RoleInfoChange(&msg->role_info);
}

void SceneManager::OnHiddenGameRoleInfoChange(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer() )
	{
		return;
	}

	crossgameprotocal::HiddenGameRoleInfoChange *msg = (crossgameprotocal::HiddenGameRoleInfoChange *)data;

	RankManager::Instance().OnHiddenGameRoleInfoChange(msg);
}

void SceneManager::OnHiddenGamePetInfoChange(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGamePetInfoChange *msg = (crossgameprotocal::HiddenGamePetInfoChange *)data;

	RankManager::Instance().OnHiddenGamePetInfoChange(msg);
}

void SceneManager::OnCrossGameRoleActiveChange(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::HiddenGameRoleActiveChange *msg = (crossgameprotocal::HiddenGameRoleActiveChange *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	guild->AddActiveVal(msg->role_id, msg->add_val);
}

void SceneManager::OnGameHiddenGuildActiveInfoChange(const char* data, int length)
{
	crossgameprotocal::GameHiddenGuildActiveInfoChange *msg = (crossgameprotocal::GameHiddenGuildActiveInfoChange *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	guild->SyncActiveInfoByGameWorld(msg);
}

void SceneManager::OnHiddenGameRoleActiveGiftInfo(const char* data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameRoleActiveGiftInfo*msg = (crossgameprotocal::HiddenGameRoleActiveGiftInfo *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	guild->OnHiddenGameRoleActiveGiftInfo(msg);
}

void SceneManager::OnHiddenGameRoleQiFuChange(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::HiddenGameQiFuChange * msg = (crossgameprotocal::HiddenGameQiFuChange *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	guild->FromHiddenQifu(msg->role_id, msg->add_val);
}

void SceneManager::OnGameHiddenRoleQiFuChange(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
		return;

	crossgameprotocal::GameHiddenGuildQiFuValueChange * msg = (crossgameprotocal::GameHiddenGuildQiFuValueChange *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	guild->FromGameWorld(msg->role_id, msg->value);
}

void SceneManager::OnGameHiddenSynLevelComplement(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::GameHiddenSynLevelComplementInfo * msg = (crossgameprotocal::GameHiddenSynLevelComplementInfo *)data;
	RankManager::Instance().OnHiddenRecvGameSynLevelComplementInfo(msg);
}

void SceneManager::OnCrossGameSyncGuildRankRewardRankInfo(const char* data, int length)
{
	crossgameprotocal::CrossGameSyncGuildRankRewardRankInfo *msg = (crossgameprotocal::CrossGameSyncGuildRankRewardRankInfo *)data;

	Guild * guild = GuildManager::Instance().GetGuild(msg->guild_id);
	if (NULL == guild) return;

	switch (msg->rank_type)
	{
	case GUILD_RANK_TYPE_WEEK_ACTIVE:
		{
			guild->GetCrossGuildActiveRankReward(msg->rank_index , msg->rank_val);
		}
	default:
		break;
	}
}

void  SceneManager::OnGmCrossGameGuildActiveTest(const char* data, int length)
{
	GuildManager::Instance().GmGuildActiveSettlement();
	RankManager::Instance().GetGuildRank()->ClearRank(GUILD_RANK_TYPE_WEEK_ACTIVE);
}

void SceneManager::OnHiddenGameWorldBossInfo(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::CrossGameSyncWorldBossInfo *msg = (crossgameprotocal::CrossGameSyncWorldBossInfo *)data;

	Activity* world_boss_activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS);
	if (world_boss_activity != NULL)
	{
		ActivityWorldBoss * world_boss = (ActivityWorldBoss *)world_boss_activity;
		world_boss->SetInfo(msg);
		world_boss->SendBossInfo();
		if (msg->is_send_boss_pos > 0)
		{
			world_boss->SendBossPosInfo();
		}
	}
}

void SceneManager::OnHiddenGameWorldBoss2Info(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::CrossGameSyncWorldBoss2Info *msg = (crossgameprotocal::CrossGameSyncWorldBoss2Info *)data;

	Activity* world_boss_activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_2);
	if (world_boss_activity != NULL)
	{
		ActivityWorldBoss2 * world_boss = (ActivityWorldBoss2 *)world_boss_activity;
		world_boss->SetInfo(msg);
		world_boss->SendBossInfo();
		if (msg->is_send_boss_pos > 0)
		{
			world_boss->SendBossPosInfo();
		}
	}
}

void SceneManager::OnHiddenGameWorldBoss3Info(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	crossgameprotocal::CrossGameSyncWorldBoss3Info *msg = (crossgameprotocal::CrossGameSyncWorldBoss3Info *)data;

	Activity* world_boss_activity = ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_BOSS_3);
	if (world_boss_activity != NULL)
	{
		ActivityWorldBoss3 * world_boss = (ActivityWorldBoss3 *)world_boss_activity;
		world_boss->SetInfo(msg);
		world_boss->SendBossInfo();
		if (msg->is_send_boss_pos > 0)
		{
			world_boss->SendBossPosInfo();
		}
	}
}

void SceneManager::OnGameHiddenReqSynKuaFuYingXiongLuData(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::GameHiddenReqSynKuaFuYingXiongLuData * req = (crossgameprotocal::GameHiddenReqSynKuaFuYingXiongLuData *)data;
	KuaFuYingXiongLuManager::Instance().OnHiddenSynDataToGameworld(req->server_id);

	//验证用回执
	crossgameprotocal::CrossGameDataSyncCheckAck ack;
	ack.type = crossgameprotocal::DATA_SYNC_CHECK_TYPE_KUA_FU_YING_XIONG_LU;
	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), req->server_id, (const char*)&ack, sizeof(ack));
}

void SceneManager::OnHiddenGameRetSynKuaFuYingXiongLuData(const char * data, int length)
{
	if (CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::HiddenGameRetSynKuaFuYingXiongLuData * req = (crossgameprotocal::HiddenGameRetSynKuaFuYingXiongLuData *)data;
	KuaFuYingXiongLuManager::Instance().OnGameRecvHiddenSynData(req);
}

void SceneManager::OnCrossHiddenRetKuaFuYingXiongLuRankEnd(const char * data, int length)
{
	if (!CrossConfig::Instance().IsHiddenServer())return;

	crossgameprotocal::CrossHiddenRetKuaFuYingXiongLuRankEnd * req = (crossgameprotocal::CrossHiddenRetKuaFuYingXiongLuRankEnd *)data;
	int offset = static_cast<int>(UNSTD_OFFSETOFF(crossgameprotocal::CrossHiddenRetKuaFuYingXiongLuRankEnd, rank_uid_list));
	if (length < offset)
	{
		return;
	}

	int count = (length - offset) / sizeof(req->rank_uid_list[0]);
	if (count <= 0)
	{
		return;
	}

	if (count > ARRAY_LENGTH(req->rank_uid_list))
	{
		return;
	}

	std::vector<int> rank_uid_list;
	for (int i = 0; i < count; ++i)
	{
		rank_uid_list.push_back(req->rank_uid_list[i]);
	}

	KuaFuYingXiongLuManager::Instance().OnHiddenRecvCrossActivityRankEnd(req, rank_uid_list);
}

SceneManager::~SceneManager()
{

}

bool SceneManager::ReInit(const SceneManagerConfig &scm)
{	
	// 重读map
	for (ConfigMapList::const_iterator iter = scm.configmanager.maps.begin(); iter != scm.configmanager.maps.end(); ++iter)
	{
		MapList::iterator it_map = m_map_list.find(iter->map.id);
		if (it_map != m_map_list.end())
		{
			delete it_map->second;
			m_map_list.erase(it_map);
		}

		Map *map = new Map(iter->map);
		m_map_list[iter->map.id] = map;
	}

	// 重读scene， 并ReInit Scene
	m_fb_config_list.clear();
	for (ConfigSceneList::const_iterator it = scm.configmanager.scenes.begin(); it != scm.configmanager.scenes.end(); ++it)
	{
		MapList::iterator it_map = m_map_list.find(it->scene.mapid);
		if (it_map == m_map_list.end())
		{
			continue;
		}

		if (Scene::SCENE_TYPE_COMMON_SCENE != it->scene_type)
		{
			FBConfig fbconfig;
			fbconfig.config = it->scene;
			fbconfig.scene_type = it->scene_type;
			m_fb_config_list[it->scene.id] = fbconfig;
		}

		for (SceneList::Iterator it_scene = m_scene_list.Beg(); it_scene != m_scene_list.End(); ++it_scene)
		{
			Scene *scene = *it_scene;
			if (it->scene.id == scene->GetSceneID())
			{
				scene->ReInit(it->scene, it_map->second);
			}
		}
	}

	return true;
}

void SceneManager::Release()
{
	for (SceneList::Iterator i = m_scene_list.Beg(); i != m_scene_list.End(); ++i)
	{
		delete (*i);
	}
	m_scene_list.Clear();

	for (MapList::iterator i = m_map_list.begin(); i != m_map_list.end(); ++i)
	{
		delete i->second;
	}
	m_map_list.clear();

	m_fb_config_list.clear();
	m_sceneidmap.clear();
	m_net_to_rolelocal.clear();
	m_uid_to_rolelocal.clear();
	m_message_handler.Release();
}
