#ifndef __SCENE_H__
#define __SCENE_H__

#include "scene/aoi/zonematrix.h"
#include "obj/objmanager.h"
#include "obj/singlemanager.h"
#include "inetworkmodule.h"
#include "transportlist.h"
#include "config/sceneconfig.h"
#include "servercommon/maildef.hpp"
#include "servercommon/mapunlockdef.h"
#include <set>

static const int MAX_SCENE_NAME_LENGTH = 32;

class SceneManager;
class Map;
class RoleInitParam;
class RoleOtherInitParam;
class Role;
class RoleModuleManager;
class ItemListParam;
class SkillListParam;
class FriendListParam;
class BlacklistsParam;
class MailListParam;
class RoleFirstKillParamList;
class AchievementListParam;
class SpecialLogic;
class NPCMonster;
class RandActivityRoleParamList;
class RoleCommonSaveDataParam;
class RoleCourageChallengeLevelParamList;
class GodPrintItemListParam;
class NewCourageChallengeLevelParamList;

class Scene
{
public:
	enum SCENE_TYPE
	{
		SCENE_TYPE_COMMON_SCENE = 0,							// ��ͨ�����������٣�
		SCENE_TYPE_GUAJI = 1,									// �һ�����
		SCENE_TYPE_COURAGE_CHALLENGE = 2,						// ��������ս �����˸�����
		SCENE_TYPE_MIYI = 3,									// ��ҽ����ı ����������Ӿ��ɣ�
		SCENE_TYPE_CRYSTAL_FB = 4,								// ˮ������
		SCENE_TYPE_TEAM_FIGHT = 5,								// С�Ӿ���
		SCENE_TYPE_MAZE_RACER = 6,								// �����Թ�
		SCENE_TYPE_GATHER_FB = 7,								// �ɼ���
		SCENE_TYPE_MAZE_RACER_WAITING_HALL = 8,					// �����Թ��ȴ�����
		SCENE_TYPE_BRAVE_GROUND = 9,							// �´��ع�
		SCENE_TYPE_GUILD_FIGHT = 10,							// ������Ҷ�
		SCENE_TYPE_CHIEF_ELECTION = 11,							// ��ϯ��ѡ
		SCENE_TYPE_CHIEF_PEAK_BATTLE = 12,						// ��ϯ�۷�ս
		SCENE_TYPE_GUILD_HONOR_BATTLE = 13,						// ������ҫս
		SCENE_TYPE_GUILD_ANSWER = 14,							// �������
		SCENE_TYPE_CHALLENGE_FB = 15,							// ��ս����
		SCENE_TYPE_ADVANCE_FB = 16,								// ���׸���
		SCENE_TYPE_PLATFORM_BATTLE = 17,						// ��̨ս
		SCENE_TYPE_GUILD_STATION = 18,							// ����פ��
		SCENE_TYPE_GUIDE_FB = 19,								// ָ������
		SCENE_TYPE_MEDAL_FB = 20,								// ѫ�¸���
		SCENE_TYPE_FANTASY_FB = 21,								// ����þ�
		SCENE_TYPE_ANIMAL_RACE = 22,							// ���޾���
		SCENE_TYPE_ONLY_FIGHT = 23,								// ��һ�а�(���˰������Ҷ�)
		SCENE_TYPE_SHI_TU = 24,									// ʦͽ��������
		SCENE_TYPE_CHIEF_ELECTION_SEA = 25,						// ��ϯ��ѡ��ѡ
		SCENE_TYPE_SHAN_HAI_BOSS = 26,							// ɽ��������boss
		SCENE_TYPE_DU_CHUANG_ZEI_YING = 27,						// ������Ӫ
		SCENE_TYPE_WEN_XIANG_LOU = 28,							// ����¥
		SCENE_TYPE_CATCH_FOX = 29,								// ץ�ú���
		SCENE_TYPE_STAR_KING = 30,								// ˾���Ǿ�
		SCENE_TYPE_DREAM_NOTES_CHALLENGE = 31,					// ��Ԩ��¼
		SCENE_TYPE_FORTUNE_MAZE = 32,							// �����Թ�
		SCENE_TYPE_MI_JING_QI_WEN = 33,							// �ؾ�����
		SCENE_TYPE_MATERIAL_FB = 34,							// ���龳(���ϸ���)
		SCENE_TYPE_RELIC_LOOTING = 35,							// �ż��ᱦ
		SCENE_TYPE_SOUL_IN_ZHEN = 36,							// ����֮��
		SCENE_TYPE_PET_GOD_FB = 37,								// ��ӡ֮��
		SCENE_TYPE_DU_JIE = 38,									// �ɽ�
		SCENE_TYPE_MI_JING_QI_WEN_2 = 39,						// �ؾ�����2 (��������)
		SCENE_TYPE_FISHING_GROUND = 40,							// ���㳡

		SCENE_TYPE_WAN_LING_FB = 41,							// ���黰��
		SCENE_TYPE_WEDDING_FB = 42,								// ����
		SCENE_TYPE_WORLD_ARENA_STANDBY_ROOM = 43,				// �۽����ݵȴ�����
		SCENE_TYPE_HUASHENG_MIJING_WAIT = 44,					// ��ʥ�ؼ� ����ˮ���ĵ�ͼ
		SCENE_TYPE_WORLD_TEAM_ARENA = 45,						// Ӣ�ۻ���
		SCENE_TYPE_QING_YUAN_SHENG_HUI = 46,					// ��Եʢ��
		SCENE_TYPE_SHI_TU_CHUAN_GONG = 47,						// ʦͽ����
		SCENE_TYPE_QINGYUAN_DUIDUIPENG = 48,					// ��Ե�Զ���
		SCENE_TYPE_EMPRTY_SCENE = 49,							// �հ׵��˸���
		SCENE_TYPE_TU_ZI_KUAI_PAO = 50,							// ���ӿ���
		SCENE_TYPE_TAO_QUAN_NA_JI = 51,							// ��Ȧ�ɼ�
		SCENE_TYPE_UTA = 52,									// ���µ�һ
		SCENE_TYPE_HONGMENG_TIANJIE = 53,						// �������
		SCENE_TYPE_GOTO_SCHOOL = 54,						// ��Ҫ��ѧ

		SCENE_TYPE_COUNT,
	};
	UNSTD_STATIC_CHECK(SCENE_TYPE_COUNT <= ECTYPE_PASS_RECORD_LIST_NUM_MAX * BIT_COUNT_OF_LONG_64);
	struct TownPoint
	{
		TownPoint() : scene_id(0), pos(0, 0) {}
		int scene_id;
		Posi pos;
	};

	struct MineMonster
	{
		MineMonster() : mine_monster_num(0), mine_monster_count_min(0), mine_monster_count_max(0),
			mine_interval_min_s(0), mine_interval_max_s(0), mine_monster_level_min(0), 
			mine_monster_level_max(0), mine_monster_drop_num(0), prestige_id(0)
		{
			memset(mine_monsters, 0, sizeof(mine_monsters));
			memset(mine_monster_drop_list, 0, sizeof(mine_monster_drop_list));
		}

		void Init(const ConfigScene& cfg);

		int mine_monster_num;									//!< ���׹���������
		int mine_monsters[MINE_MONSTERS_NUM];					//!< ���׹������б�  ��ÿ��Ԫ�ش������ID�� ����Щ����ƴ����һ��õ�һ�鰵�׹�)
		int mine_monster_count_min;								//!< һ�鰵�׹����ٹ�����
		int mine_monster_count_max;								//!< һ�鰵�׹���������
		int mine_interval_min_s;								//!< ���ϰ��׹ֵ���̼��ʱ�� 
		int mine_interval_max_s;								//!< ���ϰ��׹ֵ�����ʱ��
		int mine_monster_level_min;								//!< ���׹ֵ���͵ȼ�
		int mine_monster_level_max;								//!< ���׹ֵ���ߵȼ�
		int mine_monster_drop_num;
		int mine_monster_drop_list[MINE_MONSTERS_DROP_NUM];		//!< ���׹ֵ����б�
		int prestige_id;
	};

	typedef std::map<int, int> AvoidPointMap;

public:
	Scene(SceneManager *scene_manager);
	~Scene();

	bool Init(SceneIndex scene_index, const ConfigScene &configscene, Map *map, int scene_type, int scene_key, unsigned int scene_timeout, bool timeout_kick);
	bool ReInit(const ConfigScene &configscene, Map *map);
	void Release();
	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	ZoneMatrix* GetZoneMatrix() { return &m_zone_matrix; }
	SceneIndex GetIndex() { return m_index; }
	int GetSceneID() { return m_scene_id; }
	int GetSceneKey() { return m_scene_key; }
	int GetSceneType() { return m_scene_type; }
	const char * GetSceneName() { return m_scene_name; }
	Map * GetMap() { return m_map; }
	Transport * GetTransport(int tid) { return m_transport_list.GetTransport(tid); }
	Transport * GetOneTransport() { return m_transport_list.GetOneTransport(); }
	SpecialLogic *GetSpecialLogic() { return m_special_logic; }

	Obj * GetObj(ObjID id);
	void AddObj(Obj *obj);
	bool DeleteObj(ObjID obj_id);

	Role * GetRoleByUID(int uid);
	Role * GetRoleByIndex(int index);
	Role * GetRoleByObjID(ObjID obj_id);

	NPCMonster * GetMonsterByIndex(int index);
	NPCMonster * GetMonsterByObjID(ObjID obj_id);

	int RoleNum();
	int NPCMonsterNum();

	typedef bool(*RoleFilterFunc)(Role *role, void *func_param);
	bool SendToRole(const char *msg, int length, RoleFilterFunc role_filter = 0, void *func_param = 0);
	bool SendToCurrSceneRoleByRoleId(int role_id, const void * msg, int length);
	void MailToRole(int reason, const MailContentParam &contentparam, int mail_kind = MAIL_TYPE_SYSTEM, RoleFilterFunc role_filter = 0, void *func_param = 0);

	bool RoleLeaveScene(ObjID obj_id, bool is_logout = false);
	static bool IsDelayDeleteScene(int scene_type, int scene_id);

	bool RoleEnterSceneAsyn(bool is_micro_pc, const GSNetID &netid, IP userip, const UserID & user_id, PlatName pname, bool is_login, int plat_spid);
	void RoleEnterScene(int plat_spid,
		PlatName pname,
		const UserID & uid,
		const GSNetID &netid,
		RoleModuleManager* role_moodule_manager,
		const RoleInitParam &param,
		const RoleOtherInitParam &other_p,
		const ItemListParam& item_list_param,
		const SkillListParam& skill_list_param,
		const MailListParam& maillist_param,
		const RoleFirstKillParamList& first_kill_list,
		const RandActivityRoleParamList & rand_activity_role_data_list ,
		const RoleCommonSaveDataParam & role_common_save_list,
		const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
		const GodPrintItemListParam & god_print_item_list,
		const NewCourageChallengeLevelParamList & new_courage_challenge_list,
		bool is_login = false,
		Role **p_role = 0);

	static bool IsStaticScene(int scene_type);
	bool IsInStaticScene();

	void CheckRoleTimeOut(unsigned long now);

	int GetTownPoint(Posi *pos = 0);

	bool HasMineMonster() { return m_mine_monster.mine_monster_num > 0; }
	long long RandMineMonsterEncounterTimeMS();
	int RandMineMonster(Role* role, int max_list_size, int* out_monster_id_list, short* out_monster_level_list, int header_monster_id = 0);
	int RandHeadMineMonster(); // ���һ������id������ͷͷ
	const MineMonster& GetMineMonsterCfg() const { return m_mine_monster; }
	bool IsSyncGather() const { return m_is_sync_gather; }
	void SyncObjInfo(Obj* obj, short notify_reason = 0);
	void SyncAllObjOfType(Role* role, short obj_type);

	Posi GetARandPosi();		// ��ȡһ������ĵ�����
	void AddAvoidPos(const Posi& pos);
	void RemoveAvoidPos(const Posi& pos);
	void GmPrintfWalkableList();

	int GetScenenMinLevel() { return m_scene_level_min; }
	int GetScenenMaxLevel() { return m_scene_level_max; }

	bool CanEnter(Role* role, bool is_notice = true);

	void *operator new(size_t c);
	void operator delete(void *m);

private:
	//��ֹ�������ƣ��벻Ҫ���л����������������� ������������� ��Ϊ������ʱ��delete��ĳЩ��Ա������
	Scene(const Scene&);
	Scene& operator=(const Scene &);

	void UpdateObj(unsigned long interval, time_t now_second, unsigned int now_dayid);
	void RemoveObj();

	SceneIndex m_index;										// scenemanager�е��±���Ϊ��ʱid
	int	m_scene_id;											// ������Ϸ��ʶ�𳡾���id
	int m_scene_key;
	int m_scene_type;

	SceneManager *m_scene_manager;

	char m_scene_name[MAX_SCENE_NAME_LENGTH];

	Map *m_map;
	ZoneMatrix m_zone_matrix;

	ObjManager m_obj_manager;
	SingleManager m_role_manager;
	SingleManager m_npc_monster_manager;
	SingleManager m_npc_gathers_manager;
	SingleManager m_npc_chosed_manager;
	SingleManager m_robot_manager;

	typedef std::vector<ObjID>	RemoveObjList;				// �ӳ�ɾ����������Obj���Ե���DeleteObj���Լ��Ƴ�
	RemoveObjList m_remove_obj_list;

	TransportList m_transport_list;

	bool			m_is_delay_delete_scene;				// �Ƿ��ӳ�ɾ��
	time_t			m_delay_delete_scene_time;				// �ӳ�ɾ������ʱ��
	unsigned int    m_scene_create_time;					// ��������ʱ��
	unsigned int	m_scene_timeout;						// ������ʱʱ��
	bool			m_is_timeout_kick_role;

	TownPoint		m_town_point;
	MineMonster		m_mine_monster;							// ����

	int m_scene_level_min;									//!< ���볡���ȼ�����
	int m_scene_level_max;									//!< ���볡���ȼ�����

	SpecialLogic*	m_special_logic;						// ���ⳡ���߼�

	bool m_is_sync_gather;									//!< �Ƿ���ͻ���ͬ������������вɼ���

	AvoidPointMap m_rand_posi_avoid_list;				//!< �����λ��Ҫ�ܿ�����б��еĵ�
};

#endif