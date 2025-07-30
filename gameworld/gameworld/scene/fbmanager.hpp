#ifndef __FB_MANAGER_HPP__
#define __FB_MANAGER_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/mapunlockdef.h"

class Role;
class FBManager
{
public:
	enum FB_TYPE
	{
		FB_TYPE_INVALID = 0,
		FB_TYPE_COURAGE_CHALLENGE = 1,		//!< 1 ��������ս
		FB_TYPE_MIYI = 2,					//!< 2 ��ҽ����ı
		FB_TYPE_CRYSTAL_FB = 3,				//!< 3 ˮ������
		FB_TYPE_MEDAL = 4,					//!< 4 ѫ�¸���
		FB_TYPE_GATHER_FB = 5,				//!< 5 �ɼ�����
		FB_TYPE_MAZE_RACER = 6,				//!< 6 �Թ�����(�ȴ�����)
		FB_TYPE_BRAVE_GROUND = 7,			//!< 7 �´��ع�
		FB_TYPE_GUILD_FIGHT = 8,			//!< 8 ������Ҷ�
		FB_TYPE_CHALLENGE_FB = 9,			//!< 9 ��ս����
		FB_TYPE_GUILD_ANSWER = 10,			//!< 10 �������
		FB_TYPE_ADVANCE_FB = 11,			//!< 11 ���׸���
		FB_TYPE_GUILD_STATION = 12,			//!< 12 ����פ��
		FB_TYPE_PLATFORM_BATTLE = 13,		//!< 13 ��̨ս
		FB_TYPE_GUIDE_FB = 14,				//!< 14 ָ������
		FB_TYPE_FANTASY_FB = 15,			//!< 15 ����þ�
		FB_TYPE_ANIMAL_RACE = 16,			//!< 16 ���޾���
		FB_TYPE_ONLY_FIGHT = 17,			//!< 17 ��һ�а�(���˰������Ҷ�)
		FB_TYPE_SHI_TU_FB = 18,				//!< 18 ʦͽ����
		FB_TYPE_SHAN_HAI_BOSS = 19,			//!< 19 ɽ��������boss
		FB_TYPE_DU_CHUANG_ZEI_YING = 20,	//!< 20 ������Ӫ
		FB_TYPE_WEN_XIANG_LOU = 21,			//!< 21 ����¥
		FB_TYPE_CATCH_FOX = 22,				//!< 22 ץ�ú���
		FB_TYPE_STAR_KING = 23,				//!< 23 ˾���Ǿ�
		FB_TYPE_DREAM_NOTES_CHALLENGE = 24,	//!< 24 ��Ԩ��¼
		FB_TYPE_FORTUNE_MAZE = 25,			//!< 25 �����Թ�
		FB_TYPE_MI_JING_QI_WEN = 26,		//!< 26 �ؾ��Թ�
		FB_TYPE_MATERIAL_SCENE = 27,		//!< 27 ���龳(���ϸ���)
		FB_TYPE_RELIC_LOOTING = 28,			//!< 28 �ż��ᱦ
		FB_TYPE_MAIN_TASK_CATCH_FOX = 29,	//!< 29 ��������ץ�ú���
		FB_TYPE_SOUL_IN_ZHEN = 30,			//!< 30 ����֮��
		FB_TYPE_PET_GOD = 31,				//!< 31 ��ӡ֮�� P1:fb_seq
		FB_TYPE_DU_JIE = 32,				//!< 32 �ɽ� P1:fb_level
		FB_TYPE_MI_JING_QI_WEN_2 = 33,		//!< 33 �ؾ��Թ�2 ����������
		FB_TYPE_FISHING_GROUND = 34,		//!< 34 ���㳡
		FB_TYPE_WAN_LING_FB = 35,			//!< 35 ���黰��
		FB_TYPE_WEDDING_FB = 36,			//!< 36 ����
		FB_TYPE_WORLD_TEAM_ARENA = 37,		//!< 37 Ӣ�ۻ���
		FB_TYPE_SHI_TU_CHUAN_GONG = 38,		//!< 38 ʦͽ����
		FB_TYPE_QING_YUAN_SHENG_HUI = 39,		//!< 39 ��Եʢ��
		FB_TYPE_QINGYUAN_DUIDUIPENG = 40,	//!< 40 ��Ե�Զ���
		FB_TYPE_SAN_XIAN_GUI_DONG = 41,		//!< 41 ���ɹ鶴
		FB_TYPE_TU_ZI_KUAI_PAO = 42,		//!< 42 ���ӿ���
		FB_TYPE_TAO_QUAN_NA_JI = 43,		//!< 43 ��Ȧ�ɼ�
		FB_TYPE_UTA = 44,					//!< 44 ���µ�һ����
		FB_TYPE_HONG_MENG_TIAN_JIE = 45,	//!< 45 �������
		FB_TYPE_GOTO_SCHOOL = 46,			//!< 46 ��Ҫ��ѧ
		FB_TYPE_NEW_COURAGE_CHALLENGE = 47,	//!< 47 ߱��֮��

		FB_TYPE_MAX,
	};
	UNSTD_STATIC_CHECK(FB_TYPE_MAX <= ECTYPE_PASS_RECORD_LIST_NUM_MAX * BIT_COUNT_OF_LONG_64);

public:
	static FBManager& Instance();
	bool EnterFB(Role *role, short fb_type, int param1, int param2, bool is_client_req = true);
	bool LeaveFB(Role *role);
	int GetSceneKey();
private:
	bool ConsumeBeforeRoleEnter(Role* role, short fb_type, int param1, int param2, int target_scene_id, int target_scene_key, bool only_check);

	bool ProceedSingleEnter(Role* role, int target_scene_id, Posi target_pos, int target_scene_key, short fb_type, int param1, int param2);
	bool ProceedTeamEnter(Role* role, int target_scene_id, Posi target_pos, int target_scene_key, short fb_type, int param1, int param2);

	bool ProceedSingleLeave(Role* role, int target_scene_id, Posi target_pos);
	bool ProceedTeamLeave(Role* role, int target_scene_id, Posi target_pos);

	void ProceedTeamEnterHiddenHongMengTianJie(Role* role, int fb_type, int fb_seq);

	FBManager();
	~FBManager();

	int m_scene_key;
};

#endif