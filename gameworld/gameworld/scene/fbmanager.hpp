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
		FB_TYPE_COURAGE_CHALLENGE = 1,		//!< 1 勇气的挑战
		FB_TYPE_MIYI = 2,					//!< 2 密医的阴谋
		FB_TYPE_CRYSTAL_FB = 3,				//!< 3 水晶副本
		FB_TYPE_MEDAL = 4,					//!< 4 勋章副本
		FB_TYPE_GATHER_FB = 5,				//!< 5 采集副本
		FB_TYPE_MAZE_RACER = 6,				//!< 6 迷宫竞速(等待大厅)
		FB_TYPE_BRAVE_GROUND = 7,			//!< 7 勇闯地宫
		FB_TYPE_GUILD_FIGHT = 8,			//!< 8 家族大乱斗
		FB_TYPE_CHALLENGE_FB = 9,			//!< 9 挑战副本
		FB_TYPE_GUILD_ANSWER = 10,			//!< 10 家族答题
		FB_TYPE_ADVANCE_FB = 11,			//!< 11 进阶副本
		FB_TYPE_GUILD_STATION = 12,			//!< 12 家族驻地
		FB_TYPE_PLATFORM_BATTLE = 13,		//!< 13 擂台战
		FB_TYPE_GUIDE_FB = 14,				//!< 14 指引副本
		FB_TYPE_FANTASY_FB = 15,			//!< 15 四象幻境
		FB_TYPE_ANIMAL_RACE = 16,			//!< 16 神兽径赛
		FB_TYPE_ONLY_FIGHT = 17,			//!< 17 以一敌百(个人版家族大乱斗)
		FB_TYPE_SHI_TU_FB = 18,				//!< 18 师徒副本
		FB_TYPE_SHAN_HAI_BOSS = 19,			//!< 19 山海降妖大boss
		FB_TYPE_DU_CHUANG_ZEI_YING = 20,	//!< 20 独闯贼营
		FB_TYPE_WEN_XIANG_LOU = 21,			//!< 21 温香楼
		FB_TYPE_CATCH_FOX = 22,				//!< 22 抓拿狐妖
		FB_TYPE_STAR_KING = 23,				//!< 23 司命星君
		FB_TYPE_DREAM_NOTES_CHALLENGE = 24,	//!< 24 梦渊笔录
		FB_TYPE_FORTUNE_MAZE = 25,			//!< 25 幸运迷宫
		FB_TYPE_MI_JING_QI_WEN = 26,		//!< 26 秘境迷宫
		FB_TYPE_MATERIAL_SCENE = 27,		//!< 27 玉虚境(材料副本)
		FB_TYPE_RELIC_LOOTING = 28,			//!< 28 遗迹夺宝
		FB_TYPE_MAIN_TASK_CATCH_FOX = 29,	//!< 29 主线任务抓拿狐妖
		FB_TYPE_SOUL_IN_ZHEN = 30,			//!< 30 阵中之魂
		FB_TYPE_PET_GOD = 31,				//!< 31 神印之地 P1:fb_seq
		FB_TYPE_DU_JIE = 32,				//!< 32 渡劫 P1:fb_level
		FB_TYPE_MI_JING_QI_WEN_2 = 33,		//!< 33 秘境迷宫2 ，万象奇闻
		FB_TYPE_FISHING_GROUND = 34,		//!< 34 钓鱼场
		FB_TYPE_WAN_LING_FB = 35,			//!< 35 万灵话本
		FB_TYPE_WEDDING_FB = 36,			//!< 36 婚宴
		FB_TYPE_WORLD_TEAM_ARENA = 37,		//!< 37 英雄会武
		FB_TYPE_SHI_TU_CHUAN_GONG = 38,		//!< 38 师徒传功
		FB_TYPE_QING_YUAN_SHENG_HUI = 39,		//!< 39 情缘盛会
		FB_TYPE_QINGYUAN_DUIDUIPENG = 40,	//!< 40 情缘对对碰
		FB_TYPE_SAN_XIAN_GUI_DONG = 41,		//!< 41 三仙归洞
		FB_TYPE_TU_ZI_KUAI_PAO = 42,		//!< 42 兔子快跑
		FB_TYPE_TAO_QUAN_NA_JI = 43,		//!< 43 套圈纳吉
		FB_TYPE_UTA = 44,					//!< 44 天下第一比武
		FB_TYPE_HONG_MENG_TIAN_JIE = 45,	//!< 45 鸿蒙天界
		FB_TYPE_GOTO_SCHOOL = 46,			//!< 46 我要上学
		FB_TYPE_NEW_COURAGE_CHALLENGE = 47,	//!< 47 弑神之塔

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