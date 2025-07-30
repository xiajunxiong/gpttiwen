#ifndef __SCENE_MANAGER_HPP__
#define __SCENE_MANAGER_HPP__

#include <map>
#include <vector>
#include <set>

#include "common/raobjlist.h"
#include "scene.h"
#include "messagehandler.hpp"
#include "inetworkmodule.h"
#include "serverlogic.h"

#include "servercommon/serverdef.h"
#include "config/sceneconfig.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/userprotocal/msgsystem.h"
#include "speciallogic.hpp"
#include "speciallogicimpl/speciallogiccrystalfb.hpp"
#include "speciallogicimpl/speciallogicmazeracer.hpp"
#include "speciallogicimpl/speciallogicmazeracerwaitinghall.hpp"
#include "speciallogicimpl/speciallogicguildfight.hpp"
#include "speciallogicimpl/speciallogicguildanswer.hpp"
#include "speciallogicimpl/speciallogicchallengefb.hpp"
#include "speciallogicimpl/speciallogicguildstation.hpp"
#include "scene/speciallogicimpl/speciallogicguidefb.hpp"
#include "scene/speciallogicimpl/speciallogicmedal.hpp"
#include "scene/speciallogicimpl/speciallogicfantasyfb.hpp"
#include "scene/speciallogicimpl/speciallogiconlyfight.hpp"
#include "scene/speciallogicimpl/speciallogicfshitufb.hpp"
#include "scene/speciallogicimpl/speciallogicchiefelectionsea.hpp"
#include "scene/speciallogicimpl/speciallogicanimalrace.hpp"
#include "scene/speciallogicimpl/speciallogicshanhaiboss.hpp"
#include "scene/speciallogicimpl/speciallogicduchuangzeiying.hpp"
#include "scene/speciallogicimpl/speciallogicwenxianglou.hpp"
#include "scene/speciallogicimpl/speciallogiccatchfox.hpp"
#include "scene/speciallogicimpl/speciallogicstarking.hpp"
#include "scene/speciallogicimpl/speciallogicdreamnoteschallenge.hpp"
#include "scene/speciallogicimpl/speciallogicmaterial.hpp"
#include "scene/speciallogicimpl/speciallogicfishingground.hpp"
#include "other/petgodfb/speciallogicpetgodfb.hpp"
#include "other/dujie/speciallogicfbdujie.hpp"
#include "other/wanlingfb/speciallogicwanlingfb.hpp"
#include "scene/speciallogicimpl/speciallogicworldteamarena.hpp"
#include "scene/speciallogicimpl/speciallogicshituchuangong.hpp"
#include "scene/speciallogicimpl/speciallogicqingyuanshenghui.hpp"
#include "scene/speciallogicimpl/speciallogicqingyuanduiduipeng.hpp"

//#include "scene/scenematchmanager/scenematchmanager.hpp"

#define GET_SPECIAL_LOGIC(scene_type, class_name)\
	class_name * Get##class_name(int scene_id, int fb_key, bool is_create = true)\
	{\
		Scene * scene = this->GetSceneById(scene_id, fb_key);\
		if (NULL == scene && is_create) { this->CreateFB(scene_id, fb_key, &scene); }\
		if (NULL != scene)\
		{\
			SpecialLogic * special_logic = scene->GetSpecialLogic();\
			if (NULL != special_logic && scene_type == special_logic->GetSceneType()) { return (class_name*)special_logic; }\
		}\
		return NULL;\
	}

#define BATTLE_MESSAGE_HANDLER_REGISTER(BATTLEMSGCODE, FUNCTION_NAME, FUNCTION_SET)																\
{																																				\
	if (NULL != m_battle_game_handler[battlegameprotocol::BATTLEMSGCODE])																		\
	{																																			\
		printf("BATTLE_MESSAGE_HANDLER_REGISTER msg_code[%d] repeat\n", battlegameprotocol::BATTLEMSGCODE);										\
		assert(0);																																\
		return;																																	\
	}																																			\
	if (FUNCTION_SET.find(#FUNCTION_NAME) != FUNCTION_SET.end())																				\
	{																																			\
		printf("BATTLE_MESSAGE_HANDLER_REGISTER msg_code[%d] function[%s] repeat\n", battlegameprotocol::BATTLEMSGCODE, #FUNCTION_NAME);		\
		assert(0);																																\
		return;																																	\
	}																																			\
	m_battle_game_handler[battlegameprotocol::BATTLEMSGCODE] = &FUNCTION_NAME;																	\
	FUNCTION_SET.insert(#FUNCTION_NAME);																										\
}

#define CROSS_MESSAGE_HANDLER_REGISTER(CROSSMSGCODE, FUNCTION_NAME, FUNCTION_SET)																\
{																																				\
	if(NULL != m_on_cross_handler[crossgameprotocal::CROSSMSGCODE - crossgameprotocal::MT_CROSS_GAME_BEGIN])									\
	{																																			\
		printf("CROSS_MESSAGE_HANDLER_REGISTER msg_code[%d] repeat\n", crossgameprotocal::CROSSMSGCODE);										\
		assert(0);																																\
		return;																																	\
	}																																			\
	if(FUNCTION_SET.find(#FUNCTION_NAME) != FUNCTION_SET.end())																					\
	{																																			\
		printf("CROSS_MESSAGE_HANDLER_REGISTER msg_code[%d] function[%s] repeat\n", crossgameprotocal::CROSSMSGCODE, #FUNCTION_NAME);			\
		assert(0);																																\
		return;																																	\
	}																																			\
	m_on_cross_handler[crossgameprotocal::CROSSMSGCODE - crossgameprotocal::MT_CROSS_GAME_BEGIN] = &FUNCTION_NAME;								\
	FUNCTION_SET.insert(#FUNCTION_NAME);																										\
}

const static int SCENE_MANAGER_RANDOM_GET_ROLE_MAX_COUNT = 16;

class SceneManagerConfig;
class Map;
class Scene;
class World;
class Role;
class Robot;
class Team;
class RMIRoleSaveBackObject;
class SpecialLogicCrystalFB;
class SpecialLogicMazeRacer;
class SpecialLogicMazeRacerWaitingHall;
class SpecialLogicBraveGround;
class SpecialLogicChiefElection;
class SpecialLogicChiefPeakBattle;
class SpecialLogicGuildHonorBattle;
class SpecialLogicGuildAnswer;
class SpecialLogicAdvanceFb;
class SpecialLogicFantasyFb;
class SpecialLogicOnlyFight;
class SpecialLogicAnimalRace;
class SpecialLogicShanHaiBoss;
class SpecialLogicWenXiangLouFb;
class SpecialLogicDuChuangZeiYing;
class SpecialLogicCatchFoxFb;
class SpecialLogicStarKingFb;
class SpecialLogicDreamNotesChallengeFb;
class SpecialLogicFortuneMaze;
class SpecialLogicMiJingQiWenFB;
class SpecialLogicRelicLootingFb;
class SpecialLogicMaterial;
class SpecialLogicSoulInZhen;
class SpecialLogicMiJingQiWenFB2;
class SpecialLogicFishingGround;
class SpecialLogicWeddingFB;
class SpecialLogicWorldTeamArena;
class SpecialLogicSanXianFb;
class SpecialLogicHongMengTianJie;

bool UniqueServerFilter(Role* role, void* param);

class SceneManager
{
public:
	struct SceneIDKey
	{
		SceneIDKey() : scene_id(0), scene_key(0) {}
		SceneIDKey(int sid, int skey) : scene_id(sid), scene_key(skey) {}

		int scene_id;
		int scene_key;
		bool operator < (const SceneIDKey &v) const
		{
			return (scene_id < v.scene_id) || (scene_id == v.scene_id && scene_key < v.scene_key);
		}
	};

	struct RoleLocal
	{
		RoleLocal() : scene_index(INVALID_OBJ_ID), obj_id(INVALID_OBJ_ID), last_active_time(0) {}
		RoleLocal(SceneIndex _s, ObjID _u, unsigned long lat) : scene_index(_s), obj_id(_u), last_active_time(lat) {}

		SceneIndex	scene_index;
		ObjID	obj_id;
		unsigned long last_active_time;
	};

	struct FBConfig
	{
		ConfigScene config;
		int scene_type;
	};

	typedef std::map<GSNetID, RoleLocal> NetSceneMap;
	typedef RAObjList<Scene*> SceneList;
	typedef std::map<int, Map*> MapList;
	typedef std::map<int, int> SceneIdTypeMap; // <id, scene_type>
	typedef std::map<SceneIDKey, SceneIndex> SceneIDToIndexMap;
	typedef std::vector<SceneIndex> DestroySceneList; // 场景的延迟删除
	typedef std::map<UserID, RoleLocal> UIDSceneMap;
	typedef std::map<int, FBConfig> FBConfigList;

	typedef void (SceneManager::*OnBattleGameMsgHandler)(const char *data, int length);
	typedef void (SceneManager::*OnCrossGameMsgHandler)(const char *data, int length);
public:
	SceneManager();
	~SceneManager();

	bool Init(const SceneManagerConfig& scm);
	bool ReInit(const SceneManagerConfig &scm);
	void Release();

	void OnRecv(const GSNetID &netid, IP userip, const char *data, int length);
	bool OnDisconnect(const GSNetID &netid, SceneIndex *p_scene_index = NULL, ObjID *p_obj_id = NULL);
	void OnGateWayDisconnect(NetID gateway_netid);

	int GetSceneList(int *scene_list, int max_num);
	Scene * GetSceneByIndex(SceneIndex scene_index);
	Scene * GetSceneById(int id, int key);

	static Role * GetRoleHelper(Scene *scene, ObjID obj_id, const char *funstr);

	int GetOnlineRoleNum();
	int GetMaxRoleNum();

	Role * GetRole(const UserID &user_id);
	Role * GetRole(SceneIndex scene_index, ObjID obj_id);

	void RegisterRoleLocal(const GSNetID &netid, SceneIndex sceneid, ObjID obj_id);
	void UnregisterRoleLocal(const GSNetID &netid, const UserID &user_id);

	bool Logout(Scene *scene, ObjID obj_id);
	void Logout(const UserID &user_id);
	typedef bool(*FilterFunc)(Scene *scene, ObjID obj_id, void *param_p);
	void LogoutAllRole(NetID gateway_netid = (NetID)-1, FilterFunc func = NULL, void *param_p = NULL);
	void KickAllRole(NetID gateway_netid = (NetID)-1, FilterFunc func = NULL, void *param_p = NULL);

	bool CheckIsDelayDeleteScene(int scene_id);
	bool CheckIsCommonFBScene(int scene_id);
	bool CheckIsChiefElectionSeaScene(int scene_id);
	bool GetChiefElectionScene(int role_id, int now_scene_id, int * scene_id, int * scene_key);
	bool IsFB(int scene_id);

	bool CreateFB(int fb_scene_id, int fb_scene_key, Scene **scene, bool check_key = true, unsigned int scene_timeout = 0);
	bool DestroyFB(SceneIndex scene_index);

	bool GoTo(Role * role, int target_scene_id, int target_scene_key);
	bool GoTo(Role * role, int target_scene_id, int target_scene_key,  Posi  target_pos, bool is_set_target_pos = false);
	bool GoTo(Robot *robot, int target_scene_id, int target_scene_key, const Posi &target_pos);
	bool TeamGoTo(Team* team, int target_scene_id, int target_scene_key, const Posi &target_pos);
	Posi GetTownPos(int scene_id);
	
	bool FlyByShoe(Role *role, int target_scene_id, int x, int y); // 使用飞鞋飞到某处

	bool CheckSceneIsExist(int scene_id);
	int  GetSceneTypeBySceneID(int scene_id);

	void RoleTimeOut(Scene *scene, Role *role, const char *reason, int param1 = 0);

	void Update(unsigned long intervals);

	void OnRecvMsgFromBattle(NetID netid, const char* data, int length);
	void OnRecvMsgFromCross(const char *msg, int length);

	void OnActivityStatusChange(ActivityStatus *as);
	void KickOutAllBraveGround();

	bool SendToUser(const UserID &user_id, const void *data, int length);
	bool NoticeToUser(const UserID &user_id, int notice_num);

	typedef bool(*RoleFilterFunc)(Role*, void *param_p);
	bool SystemMsgFilter(const char *msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, 
		int max_level = 0, RoleFilterFunc func = NULL, void *param_p = NULL);
	bool SystemMsgAll(const char *msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, 
		int max_level = 0, const char *spid = 0, int spid_id_length = 0);
	bool SystemMsgAllGame(const char * msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0);
	bool SystemMsgProf(int prof, const char * msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0);
	bool SystemMsgPerson(UserID uid, const char *msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0);
	bool SystemMsgScene(Scene* scene, const char* msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0);
	bool SystemMsgPerson2(Role * role, const char * msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0);
	bool SystemMsgTeamMember(Team * team, const char * msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0, bool notice_leader = true);
	bool SystemMsgPersonAndTeamMember(Role * role, const char * msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, int min_level = 0, int max_level = 0, bool notice_leader = true);
	static bool CreateSystemMsg(Protocol::SCSystemMsg *sm, int *sendlen, const char *msg, int length, SysMsgType msg_type, int color = 0, int display_pos = 0, 
		int min_level = 0, int max_level = 0, const char *spid = 0, int spid_id_length = 0);

	void OnReloadConfig(int config_type, int param1, int param2);

	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_CRYSTAL_FB, SpecialLogicCrystalFB);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MAZE_RACER, SpecialLogicMazeRacer);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MAZE_RACER_WAITING_HALL, SpecialLogicMazeRacerWaitingHall);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_GUILD_FIGHT, SpecialLogicGuildFight);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_GUILD_ANSWER, SpecialLogicGuildAnswer);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_CHALLENGE_FB, SpecialLogicChallengeFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_ADVANCE_FB, SpecialLogicAdvanceFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_GUILD_STATION, SpecialLogicGuildStation);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_GUIDE_FB, SpecialLogicGuideFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MEDAL_FB, SpecialLogicMedal);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_FANTASY_FB, SpecialLogicFantasyFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_ONLY_FIGHT, SpecialLogicOnlyFight);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_SHI_TU, SpecialLogicShiTuFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_CHIEF_ELECTION_SEA, SpecialLogicChiefElectionSea); 
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_ANIMAL_RACE, SpecialLogicAnimalRace);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_SHAN_HAI_BOSS, SpecialLogicShanHaiBoss);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_DU_CHUANG_ZEI_YING, SpecialLogicDuChuangZeiYing);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_WEN_XIANG_LOU, SpecialLogicWenXiangLouFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_CATCH_FOX, SpecialLogicCatchFoxFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_STAR_KING, SpecialLogicStarKingFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_DREAM_NOTES_CHALLENGE, SpecialLogicDreamNotesChallengeFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_FORTUNE_MAZE, SpecialLogicFortuneMaze);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MI_JING_QI_WEN, SpecialLogicMiJingQiWenFB);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_RELIC_LOOTING, SpecialLogicRelicLootingFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MATERIAL_FB, SpecialLogicMaterial);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_SOUL_IN_ZHEN, SpecialLogicSoulInZhen);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_PET_GOD_FB, SpecialLogicPetGodFB);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_DU_JIE, SpecialLogicFBDuJie);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_MI_JING_QI_WEN_2, SpecialLogicMiJingQiWenFB2);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_WAN_LING_FB, SpecialLogicWanLingFB);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_FISHING_GROUND, SpecialLogicFishingGround);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_WEDDING_FB, SpecialLogicWeddingFB);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_WORLD_TEAM_ARENA, SpecialLogicWorldTeamArena);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_SHI_TU_CHUAN_GONG, SpecialLogicShiTuChuanGong);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_QING_YUAN_SHENG_HUI, SpecialLogicQingYuanShengHui);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_QINGYUAN_DUIDUIPENG, SpecialLogicQingYuanDuiDuiPeng);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_EMPRTY_SCENE, SpecialLogicSanXianFb);
	GET_SPECIAL_LOGIC(Scene::SCENE_TYPE_HONGMENG_TIANJIE, SpecialLogicHongMengTianJie);


	SpecialLogicBraveGround * GetSpecialLogicBraveGround(int scene_id, int fb_key, bool is_create = true);
	SpecialLogicChiefElection * GetSpecialLogicChiefElection(int scene_id, int fb_key, bool is_create = false);
	SpecialLogicChiefPeakBattle * GetSpecialLogicChiefPeakBattle(int scene_id, int fb_key, bool is_create = false);
	SpecialLogicGuildHonorBattle * GetSpecialLogicGuildHonorBattle(int scene_id, int fb_key, bool is_create = true);

	//SceneMatchManager* GetSceneMatchManager() { return &m_scene_match_manager; }
private:
	bool ChangeScene(Scene *scene, ObjID obj_id, int target_scene_id, int target_scene_key, const Posi &target_pos);
	bool ChangeLocalSceneHelper(Scene *old_scene, Role *role, Scene *target_scene, const Posi &target_pos);
	bool CreateScene(const ConfigScene &scene_config, int scene_key, Scene **scene_create, int scene_type, unsigned int scene_timeout,
		bool timeout_kick);

	void EraseTimeOut();

	void DestroyScene();
	
	SceneManager(const SceneManager&);
	SceneManager& operator= (const SceneManager&);

	unsigned long m_game_time;

	MessageHandler m_message_handler;

	SceneList m_scene_list;
	MapList m_map_list;

	SceneIdTypeMap m_scene_id_to_type_map;				// 场景ID对应场景类型（方便查询）

	SceneIDToIndexMap m_sceneidmap;							// 用于映射游戏配置中的sceneid到实际运行时在m_scene_list中的下标
	DestroySceneList m_destroy_scene_list;

	NetSceneMap m_net_to_rolelocal;						// 用于查找nei_id对应的玩家所在的场景，以及获取到Role实例
	UIDSceneMap m_uid_to_rolelocal;						// 用于查找uid对应的玩家实例

	FBConfigList m_fb_config_list;

	unsigned long m_last_check_timeout;						// 上次检查超时的时间
	unsigned long m_check_timeout_interval;					// 检查timeout的时间间隔
	bool m_check_timeout;									// 是否检查超时
	unsigned long m_heartbeat_dead_time;					// 多长时间后没有心跳则认为该连接死亡 果断地断开


	//SceneMatchManager m_scene_match_manager;

	//////////////////////////////////////////////// 战斗服相关 ///////////////////////////////////////////////
	OnBattleGameMsgHandler m_battle_game_handler[1024];
	void InitBattleGameHandler();

	void OnBattleStartAck(const char* data, int length);
	void OnBattleResultAck(const char* data, int length);
	void OnBattleRoleRunAway(const char* data, int length);
	void OnBattleRoleSetAuto(const char* data, int length);
	void OnBattleRoleSetAutoMove(const char* data, int length);
	void OnBattleRoleOnlySyncPetAutoMove(const char* data, int length);
	void OnBattleRoleUseItem(const char* data, int length);
	void OnBattleRoleSealPet(const char* data, int length);
	void OnBattleRoleInstKillMonster(const char* data, int length);
	void OnBattleInstanceReleaseNotice(const char* data, int length);
	void OnBattleForceFinish(const char* data, int length);
	void OnBattleSyncWorldBossHp(const char* data, int length);
	void OnBattleSyncWorldBossHp2(const char* data, int length);
	void OnBattleSyncWorldBossHp3(const char* data, int length);
	void OnBattleCheckRoleReallyInBattle(const char* data, int length);
	void OnBattleSendMsgThougthGame(const char* data, int length);
	void OnBattleSendMsgToRole(const char* data, int length);
	void OnBattleGuildHonorFightRequestBackUp(const char* data, int length);
	void OnBattleGuildHonorFightRoleDieList(const char* data, int length);
	void OnBattleGameGuildHonorRoleStateChangeNotice(const char* data, int length);
	void OnBattleGameGuildHonorSyncRoundTotalDamage(const char* data, int length);
	void OnBattleRoleReinforce(const char* data, int length);
	void OnBattleRoleReinforceEnd(const char* data, int length);
	void OnBattleRoleSyncOBStatus(const char* data, int length);
	void OnBattleStartFailed(const char* data, int length);
	void OnBattleCheckFightStartFailed(const char* data, int length);
	void OnBattleForceFinishBattleByModeResp(const char* data, int length);
	void OnSpecialBattleResultAck(const char* data, int length);
	void OnBattleModifyAnimationSpeedAck(const char* data, int length);
	void OnBattleSyncWorldBossRoleStat(const char* data, int length);
	void OnBattleSyncWorldBoss2RoleStat(const char* data, int length);
	void OnBattleSyncWorldBoss3RoleStat(const char* data, int length);
	void OnBattlePerformChangeCardSkill(const char* data, int length);

	///////////////////////////////////////////////// 跨服相关 //////////////////////////////////////////////////
	OnCrossGameMsgHandler m_on_cross_handler[1024];
	void InitCrossHandler();

	void OnCrossHello(const char* data, int length);
	void OnCrossRegisterGameServer(const char* data, int length);
	void OnCrossRegisterGameServerRet(const char* data, int length);
	void OnDisconnectOriginalServer(const char* data, int length);
	void OnCrossStartCrossReq(const char* data, int length);
	void OnCrossStartCrossAck(const char* data, int length);
	void OnKickCrossUser(const char* data, int length);
	void OnKickCrossUserAck(const char* data, int length);
	void OnSyncCrossUser(const char* data, int length);
	void OnUserEnteringHidden(const char* data, int length);
	void OnUserAllowedEnterHidden(const char* data, int length);
	void OnCrossAllowedTeammateJoinTeam(const char* data, int length);
	void OnUserCrossTeamMatchSuccJoinTeam(const char* data, int length);
	void OnHiddenRecvGameMessage(const char* data, int length);
	void OnGameRecvHiddenMessage(const char* data, int length);
	void OnUserStartCrossMatch(const char* data, int length);
	void OnUserStopCrossMatch(const char* data, int length);
	void OnHiddenSyncCrossTeamInfoToGame(const char* data, int length);
	void OnOriginUserGetTradeMarketShelveInfo(const char* data, int length);
	void OnOriginUserSearchTradeMarketByItemID(const char* data, int length);
	void OnOriginUserSearchTradeMarketByType(const char* data, int length);
	void OnOriginUserSearchTradeMarketByBigType(const char* data, int length);
	void OnOriginUserSearchTradeMarketByTypeWithLevel(const char* data, int length);
	void OnOriginUserSearchTradeMarketMedalByColor(const char* data, int length);
	void OnOriginUserSearchTradeMarketMedalByEffectTypeAndColor(const char* data, int length);
	void OnOriginUserGetTradeMarketTransactionRecord(const char* data, int length);
	void OnOriginUserGetTradeMarketRoleTradeRecord(const char* data, int length);
	void OnOriginUserGetTradeMarketRoleFocusList(const char* data, int length);
	void OnOriginUserGetTradeMarketFocusItemInfo(const char* data, int length);
	void OnOriginUserRemoveTradeMarketFocusItem(const char* data, int length);
	void OnOriginUserAddTradeMarketFocusItem(const char* data, int length);
	void OnOriginUserGetTradeMarketMerchandiseNum(const char* data, int length);
	void OnOriginUserGetSellingList(const char* data, int length);
	void OnOriginUserSearchTradeMarketPetForQuickBuy(const char* data, int length);
	void OnOriginUserSearchAdvertise(const char* data, int length);
	void OnOriginUserTradeMarketAdvertiseReq(const char* data, int length);
	void OnOriginUserTradeMarketAdvertiseResp(const char* data, int length);
	void OnOriginUserTradeMarketAdvertiseConfirmBoardcast(const char* data, int length);

	void OnOriginUserTradeMarketRequestBuy(const char* data, int length);
	void OnOriginUserTradeMarketRequestBuyResp(const char* data, int length);
	void OnOriginUserTradeMarketRequestBuyConfirm(const char* data, int length);
	void OnOriginUserTradeMarketRequestBuyConfirmResp(const char* data, int length);
	void OnOriginUserTradeMarketRequestUpshelve(const char* data, int length);
	void OnOriginUserTradeMarketRequestUpshelveResp(const char* data, int length);
	void OnOriginUserTradeMarketOffshelveReq(const char* data, int length);
	void OnOriginUserTradeMarketOffshelveResp(const char* data, int length);
	void OnOriginUserTradeMarketRedoUpshelveReq(const char* data, int length);
	void OnOriginUserTradeMarketRedoUpshelveResp(const char* data, int length);
	void OnOriginUserTradeMarketCashOutReq(const char* data, int length);
	void OnOriginUserTradeMarketCashOutResp(const char* data, int length);
	void OnOriginUserTradeMarketRequestBuyConfirmErrorResp(const char* data, int length);

	void OnUserApplyJoinCrossTeam(const char* data, int length);
	void OnHiddenSendMailTransfer(const char* data, int length);
	void OnGameSendMailTransferToAnotherServer(const char* data, int length);
	void OnGameToGameRoleChatTransfer(const char* data, int length);
	void OnHiddenRequestGuildList(const char* data, int length);
	void OnGameSyncGuildListToHidden(const char* data, int length);
	void OnGameSyncGuildMemberChangeToHidden(const char* data, int length);
	void OnHiddenSyncGuildMemberChangeToHidden(const char * data, int length);
	void OnGameSyncGuildChangeToHidden(const char* data, int length);
	void OnGameQueryHiddenIfGuildSyncSucceed(const char* data, int length);
	void OnHiddenNoticeGameSyncGuildSucc(const char* data, int length);
	void OnGameHiddenSyncSpecialInfoReq(const char* data, int length);
	void OnHiddenGameSyncSpecialInfoRet(const char* data, int length);

	// 好友
	void OnSyncGameHiddenGetFriendInfo(const char* data, int length);
	void OnHiddenGameFriendListToRole(const char* data, int length);
	void OnHiddenGameFrinedAskListChange(const char* data, int length);
	void OnGameHiddenAddFriendReq(const char* data, int length);
	void OnGameHiddenAddFriendReplyReq(const char* data, int length);
	void OnGameHiddenRemoveFriendReq(const char* data, int length);
	void OnGameHiddenAddBlackReq(const char* data, int length);
	void OnHiddenGameBlackList(const char* data, int length);
	void OnHiddenGameBlackListChange(const char* data, int length);
	void OnGameHiddenRemoveBlackReq(const char* data, int length);
	void OnGameHiddenFriendGroupReq(const char* data, int length);
	void OnHiddenGameAskListToRole(const char* data, int length);
	void OnGameHiddenRecentlyReq(const char* data, int length);
	void OnHiddenGameRecentlyInfo(const char* data, int length);
	void OnHiddenGameFriendChangeToRole(const char* data, int length);
	void OnGameHiddenRoleLoginLogoutFriendsNotify(const char* data, int length);

	void OnSyncFriendInfo(const char* data, int length);
	void OnSyncAddFriendOfflienEvent(const char* data, int length);

	// 黑名单
	void OnSyncBlackInfo(const char* data, int length);

	void OnHiddenToGameAddTitleToGuildMember(const char* data, int length);
	void OnGameHiddenReqSyncWorldStatus(const char* data, int length);
	void OnGameHiddenSyncWorldStatus(const char* data, int length);
	void OnHiddenGameActivityForceToNextState(const char* data, int length);
	void OnGameHiddenReqSyncActivityStatus(const char* data, int length);
	void OnHiddenGameSyncActivityStatus(const char* data, int length);
	void OnHiddenGameSyncHuanJieActivityStatus(const char * data, int length);
	
	void OnGameHiddenChiefElectionReq(const char* data, int length);
	void OnGameHiddenChiefPeakBattleReq(const char* data, int length);
	void OnGameHiddenHuanJieZhanChangReq(const char * data, int length);
	void OnGameHiddenHuanJieZhanChangReq2(const char * data, int length);
	void OnGameHiddenBraveGroundReq(const char * data, int length);
	void OnGameHiddenUserCacheSync(const char* data, int length);
	void OnHiddenGameUserCacheSync(const char* data, int length);
	void OnHiddenGameChangeGuildInfo(const char* data, int length);
	void OnGameHiddenGuildHonorBattleReqRoute(const char * data, int length);
	void OnHiddenGameGuildHonorBattleMatchRecordSyncInfo(const char * data, int length);
	void OnGameSyncActivityInfoFromCross(const char* data, int length);
	void OnHiddenSyncActivityInfoFromCross(const char* data, int length);
	void OnGameHiddenSyncConsumeCarouselNewRecord(const char* data, int length);
	void OnHiddenGameSyncConsumeCarouselNewRecord(const char* data, int length);

	void OnHiddenGameChatItemInfoReq(const char * data, int length);
	void OnGameHiddenChatItemInfoReq(const char * data, int length);	
	void OnGameHiddenChatItemInfoSyn(const char * data, int length);
	void OnGameHiddenForceGetGold(const char * data, int length);

	void OnHiddenToGameAddTitleToRole(const char* data, int length);
	void OnGameHiddenOnlineStatusSync(const char* data, int length);
	void OnHiddenGameOtherServerRoleOnlineStatusSync(const char* data, int length);
	void OnCrossUserMsg(const char *data, int length);
	void OnCrossGameHiddenNotConnected(const char *data, int length);
	void OnGameHiddenQueryReq(const char *data, int length);
	void OnCrossSendAllUser(const char *data, int length);
	void OnHiddenGameNoticeLog(const char *data, int length);
	void OnGameHiddenSendChatGroup(const char *data, int length);
	void OnHiddenGameSendChatGroup(const char *data, int length);
	void OnGameHiddenChatGroupOpreta(const char *data, int length);
	void OnHiddenGameChatGroupOpreta(const char *data, int length);
	void OnGameRoleLoginSendCrossChatGroupSimpleInfoReq(const char *data, int length);
	void OnHiddenGameRoleColosseumReqRouter(const char* data, int length);
	
	void OnGameHiddenTempGroupBecomeOfficialReq(const char *data, int length);
	void OnHiddenGameTempGroupBecomeOfficialResp(const char* data, int length);
	void OnGameHiddenMazerAcerReq(const char * data, int length);
	void OnGameHiddenCommonInfoNoticeHidden(const char * data, int length);
	void OnHiddenGameFirstKillInfoReq(const char * data, int length);
	void OnGameHiddenFirstKillInfoReq(const char * data, int length);
	void OnHiddenGameVideoSimpleInfoReq(const char* data, int length);
	void OnGameHiddenVideoSimpleInfoReq(const char* data, int length);
	void OnHiddenGameVideoDetailInfoReq(const char* data, int length);
	void OnGameHiddenVideoDetailInfoResp(const char* data, int length);
	void OnGameHiddenVideoDetailInfoReq(const char* data, int length);
	void OnHiddenGameVideoDetailInfoResp(const char* data, int length);
	void OnHiddenGameGuildEventhandlerSend(const char* data, int length);
	void OnGameHiddenChatGroupSynHandlerSend(const char* data, int length);
	void OnGameHiddenChatGroupSynHandlerSendRet(const char* data, int length);
	void OnHiddenGameChatGroupSynHandlerSend(const char* data, int length);
	void OnHiddenGameOnlyFightSynInfo(const char* data, int length);
	void OnGameHiddenOnlyFightSynInfoReq(const char* data, int length);
	void OnGameHiddenReqPlatformBattleRoleInfo(const char * data, int length);
	void OnHiddenGameRetPlatformBattleRoleInfo(const char * data, int length);
	void OnHiddenGameCourageChallengeRankReq(const char * data, int length);
	void OnHiddenGameCourageChallengRankRoleChange(const char * data, int length);
	void OnHiddenGameSyncLogQuick(const char* data, int length);
	void OnHiddenGameSyncLogToOriginServer(const char* data, int length);
	void OnHiddenGameGuildBossChapterRankReq(const char* data, int length);
	void OnGameHiddenSyncPublicNotice(const char* data, int length);
	void OnGameHiddenSyncOnlineInfo(const char* data, int length);
	void OnHiddenGameInviteJoinTeam(const char* data, int length);
	void OnGameHiddenInviteJoinTeamInOrigin(const char* data, int length);
	void OnGameHiddenReqJoinTeamInfo(const char * data, int length);
	void OnHiddenGameJoinTeamInfoRet(const char * data, int length);
	void OnGameHiddenInviteJoinTeamRet(const char* data, int length);
	void OnHiddenGameInviteJoinTeamInOriginRet(const char* data, int length);
	void OnHiddenGameGuildGatherDreamPublishListReq(const char* data, int length);
	void OnHiddenGameGuildGatherDreamRecordListReq(const char* data, int length);
	void OnHiddenGameGuildGatherDreamPublishReq(const char* data, int length);
	void OnGameHiddenGuildGatherDreamPublishRet(const char* data, int length);
	void OnHiddenGameGuildGatherDreamGiftReq(const char* data, int length);
	void OnGameHiddenGuildGatherDreamGiftRet(const char* data, int length);
	void OnHiddenGameGuildGatherDreamGiftAddRecord(const char* data, int length);
	void OnHiddenGameGuildGatherDreamFetchReq(const char * data, int length);
	void OnGameHiddenGuildGatherDreamFetchRet(const char * data, int length);
	void OnHiddenGameGuildGatherDreamFetchChange(const char * data, int length);
	void OnGameHiddenSystemMailSend(const char * data, int length);
	void OnHiddenGameSystemMailSend(const char * data, int length);
	void OnHiddenGameSyncHuanJieRoleInfo(const char * data, int length);
	void OnHiddenGameGuildGatherDreamRoleInfo(const char * data, int length);
	void OnHiddenGameHuanJieSyncTianTiRecord(const char * data, int length);
	void OnHiddenGameHuanJieSyncTianTiRecordAck(const char * data, int length);
	void OnHiddenGameSyncGuildChat(const char * data, int length);
	void OnGameHiddenSyncGuildChat(const char * data, int length);
	void OnGameHiddenNoticeSensitiveWordsReload(const char * data, int length);
	void OnGameHiddenCmdKickRole(const char * data, int length);
	void OnGameHiddenCmdMuteRole(const char* data, int length);
	void OnHiddenGameCmdMuteRole(const char* data, int length);
	void OnGameHiddenCmdNoticeNewMessage(const char* data, int length);
	void OnHiddenGameCmdNoticeNewMessage(const char* data, int length);
	void OnGameHiddenRATianMing(const char *data, int length);
	void OnHiddenGameRATianMing(const char *data, int length);
	void OnHiddenGameRoleInfoChange(const char* data, int length);
	void OnHiddenGamePetInfoChange(const char* data, int length);
	void OnCrossGameRoleActiveChange(const char* data, int length);
	void OnGameHiddenGuildActiveInfoChange(const char* data, int length);
	void OnCrossGameSyncGuildRankRewardRankInfo(const char* data, int length);
	void OnGmCrossGameGuildActiveTest(const char* data, int length);
	void OnHiddenGameRoleActiveGiftInfo(const char* data, int length);
	void OnHiddenGameRoleQiFuChange(const char* data, int length);
	void OnGameHiddenRoleQiFuChange(const char* data, int length);

	void OnGameHiddenSynLevelComplement(const char* data, int length);

	void OnHiddenGameWorldBossInfo(const char* data, int length);
	void OnHiddenGameWorldBoss2Info(const char* data, int length);
	void OnHiddenGameWorldBoss3Info(const char* data, int length);

	void OnGameHiddenCmdResetPublicNotice(const char* data, int length);
	void OnHiddenGameSyncPlatInCrossRoleNum(const char* data, int length);
	void OnHiddenGameWorldBossSyncRankResultMsg(const char* data, int length);
	void OnHiddenGameWorldBoss2SyncRankResultMsg(const char* data, int length);
	void OnHiddenGameWorldBoss3SyncRankResultMsg(const char* data, int length);
	void OnHiddenGameWorldBossSyncParticipateUidList(const char* data, int length);
	void OnHiddenGameWorldBoss2SyncParticipateUidList(const char* data, int length);
	void OnHiddenGameWorldBoss3SyncParticipateUidList(const char* data, int length);

	void OnHiddenGameQuanMinActivityInfoReq(const char* data, int length);
	void OnGameHiddenQuanMinActivityInfoResp(const char* data, int length);
	void OnGameHiddenWildBossInfoReq(const char* data, int length);
	void OnHiddenGameSyncBraveGroundInfo(const char * data, int length);
	void OnGameHiddenFirstKillReq(const char * data, int length);
	void OnHiddenGameRoleReturnOriginServerReq(const char* data, int length);
	void OnGameHiddenRoleReturnOriginServerAck(const char* data, int length);
	void OnGameHiddenTeammateJoinOriginServerTeam(const char* data, int length);
	void OnHiddenGameSyncHuanJieZhanChangeInfo(const char * data, int length);
	void OnGameHiddenTestNetwork(const char* data, int length);
	void OnGameHiddenBraveGroundSetInfo(const char * data, int length);
	void OnHiddenGameSpecialLogicActivityInfoSync(const char * data, int length);
	void OnHiddenGameGetRankListReq(const char * data, int length);
	void OnGameHiddenTradeMarketHistoryTopPriceReq(const char* data, int length);

	void OnHiddenGameShenShouJiangLinReq(const char * data, int length);
	void OnGameHiddenShenShouJiangLinDrawRet(const char * data, int length);
	void OnHiddenGameHeartBeat(const char * data, int length);
	//void OnHiddenGameResetGuildFightInfo(const char * data, int length);
	//void OnHiddenGameSetGuildFightInfo(const char * data, int length);
	//void OnGameHiddenSetGuildFightInfo(const char * data, int length);
	//void OnHiddenGameSetGuildFightBuffInfo(const char * data, int length);
	//void OnGameHiddenSetGuildFightBuffInfo(const char * data, int length);

	void OnGameHiddenDeleteMutePlayerChatRecord(const char* data, int length);

	void OnGameHiddenTradeMarketSaleItemEvenhandler(const char* data, int length);
	void OnHiddenGameTradeMarketSaleItemEvenhandler(const char* data, int length);

	void OnGameHiddenAddCredit(const char * data, int length);
	void OnGameHiddenChatGroupHandoverLeadership(const char* data, int length);

	void OnGameHiddenShanHaiBossInfoReq(const char* data, int length);

	void OnGameHiddenRoleInfoNoticeReqRoute(const char * data, int length);

	void OnGameHiddenRelicLootingReq(const char * data, int length);
	void OnHiddenGameRelicLootingReq(const char * data, int length);
	void OnGameHiddenSyncCloudArenaUserDataReq(const char * data, int length);
	void OnHiddenGameSyncCloudArenaUserDataResp(const char* data, int length);
	void OnGameHiddenCloudArenaRealTimeInfoReq(const char* data, int length);
	void OnGameHiddenRoleLoginCloudArenaUserInfoReq(const char* data, int length);

	void OnGameHiddenMarketBuyItemRecordSynReq(const char* data, int length);
	void OnHiddenGameMarketBuyItemRecordSynReq(const char* data, int length);
	void OnGameHiddenMarketAllTransactionRecordSynReq(const char* data, int length);
	void OnHiddenGameMarketAllTransactionRecordSynReq(const char* data, int length);

	void OnGameHiddenVideoChangeReq(const char* data, int length);
	void OnHiddenGameVideoChangeReq(const char* data, int length);
	void OnCrossGameChivalrousRankListResp(const char* data, int length);
	void OnHiddenGameSyncCloudArenaSeasonInfo(const char* data, int length);

	void OnHiddenGamePersonRankFriendReq(const char* data, int length);
	
	void OnGameHiddenSyncHongBaoInfo(const char* data, int length);

	void OnHiddenGameBigDipperSyncRet(const char * data, int length);

	void OnHiddenGameSyncHongBaoReqGive(const char* data, int length);
	void OnGameHiddenSyncHongBaoReqGiveRet(const char* data, int length);
	void OnHiddenGameSyncHongBaoReqGet(const char* data, int length);
	void OnGameHiddenSyncHongBaoReqGetRet(const char* data, int length);
	void OnHiddenGameSyncHongBaoReqError(const char* data, int length);

	void OnGameHiddenGiveGiftReq(const char * data, int length);
	void OnGameHiddenGiveGiftAddGift(const char * data, int length);
	void OnHiddenGameGiveGiftRetInfo(const char * data, int length);

	void OnHiddenGameShiTuSeekReq(const char * data, int length);

	void OnGameHiddenHundredGhostBossInfoReq(const char* data, int length);
	void OnHiddenGameContinueRoleBattleOnCross(const char* data, int length);

	void OnHiddenGameCowReportGoodNewsCanBuy(const char* data, int length);
	void OnHiddenGameCowReportGoodNewsBuy(const char* data, int length);
	void OnGameHiddenCowReportGoodNewsBuyAck(const char* data, int length);
	void OnGameHiddenCowReportGoodNewsCanBuyAck(const char* data, int length);
	void OnSyncCowReportGoodNewsBuyOfflineEvent(const char* data, int length);
	void OnHiddenGameCowReportGoodNewsGetInfo(const char* data, int length);

	void OnHiddenGameTigerBringsBlessingCanBuy(const char* data, int length);
	void OnHiddenGameTigerBringsBlessingBuy(const char* data, int length);
	void OnGameHiddenTigerBringsBlessingBuyAck(const char* data, int length);
	void OnGameHiddenTigerBringsBlessingCanBuyAck(const char* data, int length);
	void OnSyncTigerBringsBlessingBuyOfflineEvent(const char* data, int length);
	void OnHiddenGameTigerBringsBlessingGetInfo(const char* data, int length);

	void OnHiddenGameSyncCrossData(const char* data, int length);

	void OnGameHiddenCreatAutoRobotInfo(const char* data, int length);
	void OnHiddenGameTianDiDaoHenSyncInfo(const char * data, int length);

	void OnGameHiddenColosseumRankInfoSyn(const char* data, int length);

	void OnSyncFriendIntimacyInfo(const char* data, int length);
	void OnGameHiddenAddIntimacyReq(const char* data, int length);
	void OnGameHiddenIntimacyRelationReq(const char* data, int length);
	void OnGameHiddenIntimacySingleChatTimes(const char* data, int length);
	void OnGameHiddenGiveFlowerNotice(const char* data, int length);
	void OnHiddenGameGiveFlowerNotice(const char* data, int length);
	void OnGameHiddenGiveFlowerBackKiss(const char* data, int length);
	void OnHiddenGameGiveFlowerBackKiss(const char* data, int length);

	void OnHiddenGameActivityEndInfo(const char* data, int length);
	void OnHiddenGameServerCourseInfoReq(const char* data, int length);
	void OnHiddenGameServerCourseChangeReq(const char* data, int length);
	void OnHiddenGameRoleWinChiefElectionSyn(const char* data, int length);

	void OnGameHiddenJieYiZuCreate(const char * data, int length);
	void OnHiddenGameJieYiZuSync(const char * data, int length);	
	void OnHiddenGameJieYiZuDismiss(const char * data, int length);
	void OnGameHiddenJieYiZuName(const char * data, int length);
	void OnGameHiddenJieYiZuMemberName(const char * data, int length);
	void OnGameHiddenJieYiZuText(const char * data, int length);
	void OnGameHiddenJieYiZuLeave(const char * data, int length);
	void OnGameHiddenJieYiZuKick(const char * data, int length);
	void OnGameHiddenJieYiZuTanHe(const char * data, int length);
	void OnGameHiddenJieYiZuTanHeVote(const char * data, int length);
	void OnGameHiddenJieYiZuInviteReply(const char * data, int length);
	void OnGameHiddenJieYiRoleXuanYan(const char * data, int length);
	void OnGameHiddenJieYiZuXuanYan(const char * data, int length);		
	void OnGameHiddenJieYiXuanYanList(const char * data, int length);
	void OnHiddenGameJieYiZuSyncRoleData(const char * data, int length);
	void OnHiddenGameJieYiZuSyncRoleOnlineStatus(const char * data, int length);
	void OnHiddenGameJieYiZuRoleChange(const char * data, int length);
	
	void OnGameHiddenQingYuanCreate(const char * data, int length);
	void OnHiddenGameQingYuanDismiss(const char * data, int length);
	void OnGameHiddenQingYuanLeave(const char * data, int length);
	void OnGameHiddenQingYuanSingleLeave(const char * data, int length);
	void OnGameHiddenQingYuanForceLeave(const char * data, int length);
	void OnGameHiddenQingYuanCancelLeave(const char * data, int length);
	void OnHiddenGameQingYuanSync(const char * data, int length);
	void OnGameHiddenQingYuanGiftGave(const char * data, int length);
	void OnGameHiddenQingYuanGiftFetchReward(const char * data, int length);
	void OnHiddenGameQingYuanGiftFetchRewardRet(const char * data, int length);
	void OnGameHiddenQingYuanGiftFetchDayReward(const char * data, int length);
	void OnHiddenGameQingYuanGiftFetchDayRewardRet(const char * data, int length);
	void OnGameHiddenQingYuanSetSkillFlag(const char * data, int length);

	void OnCrossHiddenCrossPersonRankEndlessRet(const char * data, int length);
	void OnHiddenGameSyncRoleBattleData(const char* data, int length);
	void OnGameHiddenGiveFlowerChange(const char * data, int length);
	void OnHiddenGameGiveFlowerChange(const char* data, int length);
	void OnHiddenGameHorcruxTianChengReq(const char * data, int length);
	void OnGameHiddenHorcruxTianChengDrawRet(const char * data, int length);
	void OnGameHiddenQueryItemRecommendPrice(const char* data, int length);
	void OnHiddenGameQueryTargetGladiatorBeforeFightReq(const char* data, int length);
	void OnGameHiddenQueryTargetGladiatorBeforeFightResp(const char* data, int length);
	void OnHiddenGameColosseumSysReport(const char* data, int length);
	void OnGameHiddenColosseumSyncNewRankAfterSysReport(const char* data, int length);
	void OnHiddenGameJiXingGaoZhaoReq(const char * data, int length);
	void OnGameHiddenJiXingGaoZhaoDrawRet(const char * data, int length);
	void OnCrossHiddenCrossPersonRankFallenGodRet(const char * data, int length);


	// 全局数据同步验证
	void OnCrossGameDataSycnCheckAck(const char * data, int length);

	//情缘-婚宴
	void OnGameHiddenWeddingRoleReq(const char * data, int length);
	void OnHiddenGameWeddingInfoSync(const char * data, int length);
	
	void OnGameHiddenQingYuanCommonData(const char * data, int length);
	void OnGameHiddenQingYuanLeiChongAddChongZhi(const char * data, int length);
	void OnGameHiddenWorldArenaUserOperate(const char * data, int length);
	void OnGameHiddenWorldArenaSignup(const char* data, int length);
	void OnHiddenGameWorldArenaGiveWinBoxReward(const char* data, int length);
	void OnHiddenGameWorldArenaGiveSignupReward(const char* data, int length);
	void OnHiddenGameWorldArenaGiveDailyReward(const char* data, int length);
	void OnGameHiddenWorldArenaUpdateSignupInfo(const char* data, int length);
	
	void OnCrossGameWorldTeamArenaQueryRankRet(const char * data, int length);
	void OnGameHiddenRoleInfoChangeWorldTeamArenaSyn(const char * data, int length);
	void OnHiddenGameWorldTeamArenaSynRoleTitleInfo(const char * data, int length);
	void OnHiddenGameWorldTeamArenaSynActivityInfo(const char * data, int length);
	void OnGameHiddenWorldTeamArenaActivityInfoSynReq(const char * data, int length);
	void OnGameHiddenWorldTeamArenaFinallyRankReq(const char * data, int length);

	void OnGameHiddenQingYuanDuiDuiPengUpDataRankInfo(const char* data, int length);
	void OnGameHiddenQingYuanDuiDuiPengRankReq(const char* data, int length);
	void OnHiddenGameQingYuanDuiDuiPengRankInfo(const char * data, int length);

	void OnHiddenGameSyncHiddenOpenServerTime(const char * data, int length);

	void GameHiddenGuildSpeedUpInfo(const char * data, int length);
	void HiddenGameGuildSpeedUpInfo(const char * data, int length);
	void HiddenGameGuildSynBuildAllInfo(const char * data, int length);
	void OnSyncGuildBuildingNotifyInfo(const char * data, int length);

	void OnGameHiddenGetUTAChampionRecord(const char* data, int length);
	void OnGameHiddenTransferMsgToBigCross(const char* data, int length);
	
	void OnHiddenGameSyncRoleReqRet(const char* data, int length);

	void OnHiddenGameGiveLotteryToRole(const char* data, int length);
	void OnHiddenGameUTAGiveUserItems(const char* data, int length);
	void OnHiddenGameUTAGiveServerItems(const char* data, int length);
	void OnHiddenGameUTASyncBaseInfo(const char* data, int length);
	void OnHiddenGameUTASyncSchedules(const char* data, int length);
	void OnGameHiddenUserRequestBigcrossVideo(const char* data, int length);
	void OnHiddenGameTransferBigCrossMsgToOrigin(const char* data, int length);
	void OnGameHiddenUserLoginInOrigin(const char* data, int length);

	void OnGameHiddenQueryWorldTeamArenaOtherInfo(const char* data, int length);
	// 异兽临世
	void OnGameHiddenGodBeastAdventBuyReq(const char * data, int length);
	void OnHiddenGameGodBeastAdventBuyInfo(const char * data, int length);
	void OnHiddenGameGodBeastAdventNumInfo(const char * data, int length);
	void OnGameHiddenGodBeastAdventReceiveTradeMarketReq(const char * data, int length);
	
	void OnCrossHiddenUTAQueryTeamMemberRankResp(const char * data, int length);
	void OnGameHiddenHongMengTianJieReq(const char * data, int length);
	void OnHiddenGameServerCompetitionRankReq(const char * data, int length);
	void OnGameHiddenServerCompetitionRankResp(const char * data, int length);

	void OnGameHiddenSaltySweetChangeDataResp(const char * data, int length);
	void OnHiddenGameSaltySweetVoteReq(const char * data, int length);
	void OnHiddenGameSyncTeamRoleRecruit(const char * data, int length);
	void OnGameHiddenSyncPetTopRankInfo(const char * data, int length);
	void OnGameHiddenSyncPartnerTopRankInfo(const char* data, int length);
	void OnCrossHiddenMeiLiRankResp(const char * data, int length);

	void OnHiddenGameAddIntimacyValue(const char * data, int length);
	void OnGameHiddenAddIntimacyValue(const char * data, int length);
	void OnHiddenGameQueryOriginServerRAStatus(const char* data, int length);
	void OnGameHiddenSendOriginServerRAStatus(const char* data, int length);

	void OnHiddenGameCleanQuery(const char* data, int length);
	void OnGameHiddenCleanQuery(const char* data, int length);

	void OnGameHiddenCmdToRole(const char* data, int length);
	void OnHiddenGameCmdToRole(const char* data, int length);
	void OnGameHiddenQueryOfflineBattleDataForRobot(const char * data, int length);
	void OnHiddenGameQueryOfflineBattleDataForRobot(const char* data, int length);
	void OnGameHiddenNotifyOfflineBattleDataForRobotResult(const char* data, int length);
	void OnGameHiddenRealRoleRobotLeaveTeamNotice(const char* data, int length);
	void OnGameHiddenSyncOfflineBattleData(const char* data, int length);
	void OnHiddenGameQueryBattleDataForOfflineBattleDataSystem(const char* data, int length);
	void OnGameHiddenReqSynKuaFuYingXiongLuData(const char * data, int length);
	void OnHiddenGameRetSynKuaFuYingXiongLuData(const char * data, int length);
	void OnCrossHiddenRetKuaFuYingXiongLuRankEnd(const char * data, int length);
	void OnHiddenGameAddSingleChatRecord(const char* data, int length);
	void OnHiddenGameUserQueryChatRecord(const char* data, int length);
	void OnGameHiddenHuanShouAnnInfoReq(const char * data, int length);
	void OnGameHiddenHuanShouAnnInfoSync(const char * data, int length);
	void OnHiddenGameHuanShouAnnInfoSync(const char * data, int length);
};

#endif
