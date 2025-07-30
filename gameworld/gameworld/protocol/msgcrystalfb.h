#ifndef __MSG_CRYSTAL_FB_H__
#define __MSG_CRYSTAL_FB_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/commondata.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	class SCCrystalFbInfo						//!< 水晶副本信息		2460
	{
	public:
		SCCrystalFbInfo();
		MessageHeader		header;

		struct CrystalFBNormalRecordPro		//每日普通秘境记录
		{
			CrystalFBNormalRecordPro(): fb_seq(-1), challenge_times(0), buy_times(0), reserve_sh(0)
			{}

			short fb_seq;
			short challenge_times;			//今日挑战次数
			short buy_times;				//今日购买的挑战次数
			short reserve_sh;
		};

		unsigned char xi_you_fb_play_times[CRYSTAL_FB_XI_YOU_MAX_LEVEL];	//大小 52，稀有副本玩的次数
	
		int normal_record_num;													//!< normal_fb_record_list的数组长度
		CrystalFBNormalRecordPro normal_fb_record_list[CRYSTAL_FB_NORMAL_NUM];	//!< 每日普通秘境的记录
	};

	class SCCrystalFbUseItemInfo						//!< 水晶副本场景玩家使用道具信息 2463
	{
	public:
		SCCrystalFbUseItemInfo();
		MessageHeader		header;

		struct UseItemInfo
		{
			UseItemInfo()
			{
				use_type = 0;
				use_times = 0;
			}

			int use_type;
			int use_times;
		};

		int count;
		UseItemInfo use_info[CRYSTAL_FB_XI_LIMIT_ITEM_MAX];		//大小100
	};

	enum CRYSTAL_FB_REQ_TYPE
	{
		CRYSTAL_FB_REQ_TYPE_NEXT_LAYER = 0,			//水晶进入下一层
		CRYSTAL_FB_REQ_TYPE_EXIT_SCENE = 1,			//四象幻境领取完奖励后倒计时结束或者点击确定退出副本 param1: 0:队员点击(队员出,暂离) 1:队长点击(全员出) 2:倒计时结束(全员出)
		CRYSTAL_FB_REQ_TYPE_BUY_ENERGY = 2,			//角色花费元宝购买体力	
		CRYSTAL_FB_REQ_TYPE_BUY_TIMES  = 3,			//请求购买水晶副本次数 param1:fb_seq
		CRYSTAL_FB_REQ_TYPE_GET_REWARD = 4,			//请求领取首通奖励 param1 : fb_seq
	};

	class CSCrystalFbReq						//!< 2462 水晶副本 / 四象幻境请求
	{
	public:
		CSCrystalFbReq();
		MessageHeader		header;

		int req_type;
		int param1;							
	};

	class SCCrystalFbObjInfo						//!< 2464 水晶副本各个对象信息
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量
		enum CRYSTAL_FB_OBJ_TYPE
		{
			CRYSTAL_FB_OBJ_TYPE_NOT = 0,		//!< 未知物品
			CRYSTAL_FB_OBJ_TYPE_MONSTER = 1,	//!< 怪物
			CRYSTAL_FB_OBJ_TYPE_NPC = 2,		//!< npc
			CRYSTAL_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			CRYSTAL_FB_OBJ_TYPE_GOUHUO = 4,		//!< 篝火
			CRYSTAL_FB_OBJ_TYPE_COIB = 5,		//!< 铜币
			CRYSTAL_FB_OBJ_TYPE_GATHER0 = 6,	//!< 采集物 - 木材
			CRYSTAL_FB_OBJ_TYPE_GATHER1 = 7,	//!< 采集物 - 矿石
			CRYSTAL_FB_OBJ_TYPE_GATHER2 = 8,	//!< 采集物 - 草
			CRYSTAL_FB_OBJ_TYPE_BOX = 9,		//!< 宝箱
			CRYSTAL_FB_OBJ_TYPE_TSD = 10,		//!< 传说门
		};

		struct CrystalFbObj
		{
			CrystalFbObj():obj_type(0), npc_id(0),pos_x(0),pos_y(0){}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCCrystalFbObjInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<当前层数
		int scene_id;

		int box_num;			//!<宝箱个数
		int m_box_num_max;		//!<最大宝箱个数

		int monster_num;		//!<明怪个数
		int m_monster_num_max;	//!<最大明怪个数

		int npc_num;			//!<已经交互过的NPC个数
		int m_npc_num_max;		//!<最大NPC个数

		int gouhuo_num;			//!<篝火数量
		int m_gouhuo_num_max;	//!<最大篝火数量

		int coin_num;			//!<铜币数量
		int m_coin_num_max;		//!<最大铜币数量

		int gather_num;			//!<采集物数量
		int m_gather_num_max;	//!<最大采集物数量

		int obj_count;
		CrystalFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCAdvanceFbObjInfo						//!< 2465 进阶副本各个对象信息
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量

		enum SPECIALLOGIC_OBJ_TYPE
		{
			SPECIALLOGIC_OBJ_TYPE_NO = -1,		//!< 无
			SPECIALLOGIC_OBJ_TYPE_BOSS = 0,		//!< boss
			SPECIALLOGIC_OBJ_TYPE_TRANSPROT = 1,//!< 传送门
			SPECIALLOGIC_OBJ_TYPE_MAX,
		};

		struct AdvanceFbObj
		{
			AdvanceFbObj() :obj_type(SPECIALLOGIC_OBJ_TYPE_NO), obj_id(-1), obj_pos_x(0),obj_pos_y(0) {}
			short obj_type;
			ObjID obj_id;
			short obj_pos_x;
			short obj_pos_y;
		};
		SCAdvanceFbObjInfo();
		MessageHeader		header;

		int obj_count;
		AdvanceFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCAdvanceFbTaskInfo						//!< 2466 进阶副本副本任务信息
	{
	public:
		SCAdvanceFbTaskInfo();
		MessageHeader		header;

		int task_id;		//任务id
		int progress;		//进度
	};

	class SCFantasyFbSceneInfo						//!< 四象幻境副本场景信息 2467
	{
	public:
		SCFantasyFbSceneInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<当前层数

		int kill_boss_flag;		//BOSS击杀标志  0:未击杀  1:已击杀

		int box_num;			//!<宝箱个数
		int m_box_num_max;		//!<最大宝箱个数

		int gather_num;			//!<采集物数量
		int m_gather_num_max;	//!<最大采集物数量
	};

	class SCFantasyFbObjInfo						//!< 2468 四象幻境副本各个对象信息
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量
		enum FANTASY_FB_OBJ_TYPE
		{
			FANTASY_FB_OBJ_TYPE_NOT = 0,		//!< 未知物品
			FANTASY_FB_OBJ_TYPE_MONSTER = 1,	//!< 怪物
			FANTASY_FB_OBJ_TYPE_NPC = 2,		//!< npc
			FANTASY_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			FANTASY_FB_OBJ_TYPE_GOUHUO = 4,		//!< 篝火
			FANTASY_FB_OBJ_TYPE_COIB = 5,		//!< 铜币
			FANTASY_FB_OBJ_TYPE_GATHER0 = 6,	//!< 采集物 - 木材
			FANTASY_FB_OBJ_TYPE_GATHER1 = 7,	//!< 采集物 - 矿石
			FANTASY_FB_OBJ_TYPE_GATHER2 = 8,	//!< 采集物 - 草
			FANTASY_FB_OBJ_TYPE_BOX = 9,		//!< 宝箱
			FANTASY_FB_OBJ_TYPE_TSD = 10,		//!< 传说门
		};

		struct FantasyFbObj
		{
			FantasyFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCFantasyFbObjInfo();
		MessageHeader		header;

		int obj_count;
		FantasyFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCFantasyFBRewardInfo						//2469 四象幻境副本暂存奖励信息下发
	{
	public:
		SCFantasyFBRewardInfo();
		MessageHeader header;

		static const int MAX_ITEM_DATA_NUM = 100;
		struct ItemData
		{
			ItemData():item_id(0), is_bind(0), num(0) {}

			ItemID item_id;
			short is_bind;
			int num;
		};

		enum FANTASY_FB_TYPE
		{
			FANTASY_FB_TYPE_INFO = 0,				//下发玩家今天完成信息 (道具信息不发送)
			FANTASY_FB_TYPE_REWARD = 1,				//下发玩家暂存奖励信息
		};

		int op_type;
		BitMap<LAYER_MAX_NUM> fantasy_finish_flag;			//今天是否已领取奖励    (二进制位运算 1-4层 0-31位使用1-4)							
		int reward_count;
		ItemData reward_list[MAX_ITEM_DATA_NUM];
	};
	
	class SCADVdropRewardInfo						//2471 进阶副本特殊掉落列表下发
	{
	public:
		SCADVdropRewardInfo();
		MessageHeader header;

		static const int MAX_DROP_DATA_NUM = 10;
		struct ItemData
		{
			ItemData() :item_id(0), is_bind(0), num(0) {}

			ItemID item_id;
			short is_bind;
			int num;
		};

		int reward_count;
		ItemData reward_list[MAX_DROP_DATA_NUM];
	};

	class SCCrystalFbEnergyInfo						//!< 水晶副本体力信息		2479
	{
	public:
		SCCrystalFbEnergyInfo();
		MessageHeader		header;

		int role_enter_energy;						//!< 现有体力
		short buy_times;							//!< 花费元宝购买体力次数
		short reserve_sh;
		unsigned int next_energy_reply_tamp;		//!< 下一次体力恢复时间戳 -- 0，无需恢复体力
		unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];//!< 是否已经通关过（二进制）CRYSTAL_FB_FIRST_FLAG == 5
		unsigned int frist_pass_reward[CRYSTAL_FB_FIRST_FLAG];//!< 是否领取过首通奖励(二进制)
		int new_jing_ying_play_times;//精英通关次数
		int use_xi_you_reward_play_times;//稀有通关次数
		int new_xi_you_play_times;//稀有总次数
	};

	class CSCrystalFbFastPassInfo						//!< 水晶副本扫荡		2478
	{
	public:
		CSCrystalFbFastPassInfo();
		MessageHeader		header;

		int fb_seq;
		int times;
	};

	class CSCrystalFbAllNpcInfo						//!< 水晶副本扫荡		2459
	{
	public:
		CSCrystalFbAllNpcInfo();

		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量
		enum CRYSTAL_FB_OBJ_TYPE
		{
			CRYSTAL_FB_OBJ_TYPE_NOT = 0,		//!< 未知物品
			CRYSTAL_FB_OBJ_TYPE_MONSTER = 1,	//!< 怪物
			CRYSTAL_FB_OBJ_TYPE_NPC = 2,		//!< npc
			CRYSTAL_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			CRYSTAL_FB_OBJ_TYPE_GOUHUO = 4,		//!< 篝火
			CRYSTAL_FB_OBJ_TYPE_COIB = 5,		//!< 铜币
			CRYSTAL_FB_OBJ_TYPE_GATHER0 = 6,	//!< 采集物 - 木材
			CRYSTAL_FB_OBJ_TYPE_GATHER1 = 7,	//!< 采集物 - 矿石
			CRYSTAL_FB_OBJ_TYPE_GATHER2 = 8,	//!< 采集物 - 草
			CRYSTAL_FB_OBJ_TYPE_BOX = 9,		//!< 宝箱
			CRYSTAL_FB_OBJ_TYPE_TSD = 10,		//!< 传说门
		};

		struct CrystalFbAllObjInfo
		{
			CrystalFbAllObjInfo() : objid(-1), obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int objid;
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};

		MessageHeader		header;

		int obj_count;
		CrystalFbAllObjInfo obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCMiJingQiWenFbObjInfo						//!< 2620 水晶副本各个对象信息
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量
		enum MI_JING_QI_WEN_FB_OBJ_TYPE
		{
			MI_JING_QI_WEN_FB_OBJ_TYPE_NOT = 0,		//!< 未知物品
			MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER = 1,	//!< 怪物
			MI_JING_QI_WEN_FB_OBJ_TYPE_NPC = 2,		//!< npc
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			MI_JING_QI_WEN_FB_OBJ_TYPE_GOUHUO = 4,		//!< 篝火
			MI_JING_QI_WEN_FB_OBJ_TYPE_COIB = 5,		//!< 铜币
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0 = 6,	//!< 采集物 - 木材
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1 = 7,	//!< 采集物 - 矿石
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2 = 8,	//!< 采集物 - 草
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOX = 9,		//!< 宝箱
			MI_JING_QI_WEN_FB_OBJ_TYPE_TSD = 10,		//!< 传说门
			MI_JING_QI_WEN_FB_OBJ_TYPE_QUESTION = 11,	//!< 答题
		};

		struct MiJingQiWenFbObj
		{
			MiJingQiWenFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCMiJingQiWenFbObjInfo();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<当前层数
		int scene_id;

		int box_num;			//!<宝箱个数
		int m_box_num_max;		//!<最大宝箱个数

		int monster_num;		//!<明怪个数
		int m_monster_num_max;	//!<最大明怪个数

		int npc_num;			//!<已经交互过的NPC个数
		int m_npc_num_max;		//!<最大NPC个数

		int gouhuo_num;			//!<篝火数量
		int m_gouhuo_num_max;	//!<最大篝火数量

		int coin_num;			//!<铜币数量
		int m_coin_num_max;		//!<最大铜币数量

		int gather_num;			//!<采集物数量
		int m_gather_num_max;	//!<最大采集物数量

		int obj_count;
		MiJingQiWenFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};

	class SCMiJingQiWenFbObjInfo2						//!< 4170 水晶副本各个对象信息
	{
	public:
		const static int MAX_OBJ_LIST_COUNT = 100;//!< 最大下发对象数量
		enum MI_JING_QI_WEN_FB_OBJ_TYPE
		{
			MI_JING_QI_WEN_FB_OBJ_TYPE_NOT = 0,		//!< 未知物品
			MI_JING_QI_WEN_FB_OBJ_TYPE_MONSTER = 1,	//!< 怪物
			MI_JING_QI_WEN_FB_OBJ_TYPE_NPC = 2,		//!< npc
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOSS = 3,		//!< Boss
			MI_JING_QI_WEN_FB_OBJ_TYPE_GOUHUO = 4,		//!< 篝火
			MI_JING_QI_WEN_FB_OBJ_TYPE_COIB = 5,		//!< 铜币
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER0 = 6,	//!< 采集物 - 木材
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER1 = 7,	//!< 采集物 - 矿石
			MI_JING_QI_WEN_FB_OBJ_TYPE_GATHER2 = 8,	//!< 采集物 - 草
			MI_JING_QI_WEN_FB_OBJ_TYPE_BOX = 9,		//!< 宝箱
			MI_JING_QI_WEN_FB_OBJ_TYPE_TSD = 10,		//!< 传说门
			MI_JING_QI_WEN_FB_OBJ_TYPE_QUESTION = 11,	//!< 答题
		};

		struct MiJingQiWenFbObj
		{
			MiJingQiWenFbObj() :obj_type(0), npc_id(0), pos_x(0), pos_y(0) {}
			int obj_type;
			int npc_id;
			short pos_x;
			short pos_y;
		};
		SCMiJingQiWenFbObjInfo2();
		MessageHeader		header;

		int level_seq;
		int layer;				//!<当前层数
		int scene_id;

		int box_num;			//!<宝箱个数
		int m_box_num_max;		//!<最大宝箱个数

		int monster_num;		//!<明怪个数
		int m_monster_num_max;	//!<最大明怪个数

		int npc_num;			//!<已经交互过的NPC个数
		int m_npc_num_max;		//!<最大NPC个数

		int gouhuo_num;			//!<篝火数量
		int m_gouhuo_num_max;	//!<最大篝火数量

		int coin_num;			//!<铜币数量
		int m_coin_num_max;		//!<最大铜币数量

		int gather_num;			//!<采集物数量
		int m_gather_num_max;	//!<最大采集物数量

		int obj_count;
		MiJingQiWenFbObj obj_list[MAX_OBJ_LIST_COUNT];
	};
}

#pragma pack(pop)

#endif

