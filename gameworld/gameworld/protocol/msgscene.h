#ifndef MSGSCENE_H
#define MSGSCENE_H

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"
#include "servercommon/taskdef.h"
#include "servercommon/reliclootingdef.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/weddingdef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	/*
	进入退出场景
	*/

	class SCEnterScene					//!< 进入场景 1100
	{
	public:
		SCEnterScene();
		MessageHeader	header;

		int				sceneid;		//!< 场景ID
		ObjID			obj_id;			//!< 对象在场景中的id
		short			is_first;		//!< 是否第一次进入此场景
		short			pos_x;			//!< 对象在场景中的位置X
		short			pos_y;			//!< 对象在场景中的位置Y
		int				scene_key;		//!< 场景key
	};

	class SCLeaveScene					//!< 离开场景通知 1101 \note 暂未使用
	{
	public:
		SCLeaveScene();
		MessageHeader	header;
		int				sceneid;		//!< 离开的场景ID
	};

	class SCVisibleObjEnterRole			//!< 其他玩家进入视野 1102
	{
	public:
		SCVisibleObjEnterRole();
		MessageHeader	header;

		int				role_id;		//!< 玩家ID
		GameName		role_name;		//!< 玩家名

		ObjID			obj_id;			//!< 玩家对象ID
		short			level;			//!< 玩家等级
		int				top_level;		//!< 巅峰等级
		int				kill_value;		//!< 杀戮值 

		Appearance		appearance;		//!< 外观

		int				move_speed;		//!< 速度
		short			pos_x;			//!< 在场景中的位置X
		short			pos_y;			//!< 在场景中的位置Y
		Scalar			distance;		//!< 方向距离 (即在当前方向上要走的距离）
		Dir				dir;			//!< 方向 [-PI,-PI]
		int				scene_id;		//!< 标识这是哪个场景的obj

		char			behavior_state;	//!< 行为状态 \see RoleStatusManager::ROLE_STATUS
		char			is_leader;		//!< 是否队长[0:否 1:是]
		char			is_in_team;		//!< 是否在队内[0:否 1:是]
		char			move_state;		//!< 保留字-> 移动状态[0-127],参照 RoleStatusManager::ROLE_MOVE_STATUS
		int				team_id;		//!< 队伍ID
		char			team_is_red;	//!< 是否红名队伍
		char			guild_post;		//!< 保留字-> 家族职位, 无家族: -1
		short			sh_reserve;		//!< 保留字

		enum ATTACH_TYPE
		{
			ATTACH_TYPE_ESCORT = 0,		//!< 镖车的seq
			ATTACH_TYPE_GUILD = 1,		//!< 家族的家族名、族徽
			ATTACH_TYPE_TITLE = 2,		//!< 头顶称号
			ATTACH_TYPE_FACESCORE = 3,	//!< 颜值头衔资源ID
			ATTACH_TYPE_FOLLOW_PET = 4,	//!< 宠物跟随
			ATTACH_TYPE_JIE_YI_ZU_NAME = 5,	//!< 结义组名字
			ATTACH_TYPE_MAX_NUM
		};
		UNSTD_STATIC_CHECK(ATTACH_TYPE_MAX_NUM < 31);

		const static int ATTACH_DATA_LENGTH = 2048; // attach数据总长度加起来一定不能超过这个长度

		int				attach_mask;
		char			attach_data[ATTACH_DATA_LENGTH];
	};

	class SCUpdateFollowList	//!< 1117 更新跟随列表 (更换队长时下发）
	{
	public:
		SCUpdateFollowList();
		MessageHeader header;

		int follow_team_id;
		ObjID follow_objid_list[MAX_TEAM_MEMBER_NUM];
		short reserve_sh;

		UNSTD_STATIC_CHECK(MAX_TEAM_MEMBER_NUM % 2 != 0);
	};

	class SCAddFollow			//!< 1118 增加跟随者 （新入队或归队时下发）
	{
	public:
		SCAddFollow();
		MessageHeader header;

		int	follow_team_id;		//!< 跟随的队伍ID
		ObjID add_objid;		//!< 跟随objid列表
		short reserve_sh;		//!< 保留字
	};

	class SCRemoveFollow		//!< 1119 删除跟随者 （离队或暂离时下发）
	{
	public:
		SCRemoveFollow();
		MessageHeader header;

		int follow_team_id;		//!< 跟随的队伍ID
		ObjID remove_objid;		//!< 删除的跟随者objid
		short is_remove_all;	//!< 当值为1，解散整个跟随队伍
	};

	

	class SCChangeAppearanceRole			//!< 外观改变 1112
	{
	public:
		SCChangeAppearanceRole();
		MessageHeader	header;

		int obj_id;
		Appearance appearance;				//!< 外观
	};

	class SCVisibleObjEnterNPCMonster	//!< 1109 NPC怪物进入视野
	{
	public:
		SCVisibleObjEnterNPCMonster();
		MessageHeader header;

		int npc_id;						//!< npc_id
		int client_show_id;				//!< 客户端显示模型id
		int monster_group_id;			//!< 怪物组ID
		int battle_mode;				//!< 战斗模式  \see BATTLE_MODE
		int npc_seq;					//!< npc_seq   \note 用于标识服务端的NPC

		short obj_id;					//!< 对象ID
		short reserve_sh;				//!< 保留字

		int				speed;			//!< 速度
		short			pos_x;			//!< 在场景中的位置X
		short			pos_y;			//!< 在场景中的位置Y
		Scalar			distance;		//!< 方向距离 (即在当前方向上要走的距离）
		Dir				dir;			//!< 方向 [-PI,-PI]
		int				scene_id;		//!< 标识这是哪个场景的obj
	};

	class SCVisibleObjEnterNPCGathers	//!< 1110 NPC采集物进入视野
	{
	public:
		SCVisibleObjEnterNPCGathers();
		MessageHeader header;

		int npc_id;					//!< 用于显示外观
		int npc_seq;				//!< 用于标识服务器的专有参数
		short posi_x;				//!< 在场景中的位置X
		short posi_y;				//!< 在场景中的位置y

		int scene_id;				//!< 场景ID

		ObjID obj_id;				//!< 对象ID
		short gather_times;			//!< 采集次数
	};


	class SCVisibleObjEnterSpecialTransport	//!< 1113 特别逻辑传送门进入视野
	{
	public:
		SCVisibleObjEnterSpecialTransport();
		MessageHeader header;

		int scene_id;				//!< 场景ID
		ObjID obj_id;				//!< 对象ID
		short posi_x;				//!< 在场景中的位置X
		short posi_y;				//!< 在场景中的位置y
		short reserved;				//!< 保留字
		
		int param1;
		int param2;
	};

	class SCVisibleObjEnterTreasureChest	//!< 1114 宝箱进入视野
	{
	public:
		SCVisibleObjEnterTreasureChest();
		MessageHeader header;

		int scene_id;				//!< 场景ID
		ObjID obj_id;				//!< 对象ID
		short posi_x;				//!< 在场景中的位置X
		short posi_y;				//!< 在场景中的位置y

		short is_open;				//!< 是否打开了
		int treasure_chest_id;		//!< 宝箱id
	};

	class SCVisibleObjEnterNPCFuncNpc		//!< 1115 功能性NPC进入视野
	{
	public:
		enum FUN_NPC_TYPE
		{
			FUN_NPC_TYPE_NORMAL = 0,//!< 默认
			FUN_NPC_TYPE_CHOSED = 1,//!< 交互npc
		};
		SCVisibleObjEnterNPCFuncNpc();
		MessageHeader header;

		int scene_id;				//!< 场景ID
		ObjID obj_id;				//!< 对象ID
		short fun_type;				//!< 功能类型
		short posi_x;				//!< 在场景中的位置X
		short posi_y;				//!< 在场景中的位置y

		int npc_id;					//!< 用于显示外观

		int info_type;
		int param1;
		int param2;
		int param3;
		Dir dir;					//!< 方向 [-PI,-PI]
	};

	class SCVisibleObjEnterXunYouCar	//!< 1128 巡游花车进入视野
	{
	public:
		SCVisibleObjEnterXunYouCar();
		MessageHeader header;

		ObjID			obj_id;			//!< 对象ID
		short			is_stop;		//!< 处于等待状态

		int				car_type;		//!< 巡游花车类型
		
		int				speed;			//!< 速度
		short			pos_x;			//!< 在场景中的位置X
		short			pos_y;			//!< 在场景中的位置Y
		Scalar			distance;		//!< 方向距离 (即在当前方向上要走的距离）
		Dir				dir;			//!< 方向 [-PI,-PI]
		int				scene_id;		//!< 标识这是哪个场景的obj

		//主办者信息
		struct HostInfo
		{
			HostInfo()
			{
				uid = 0;

				memset(role_name, 0, sizeof(role_name));
				avatar_type = 0;

				obj_id = 0;
				hole_sh = 0;
			}
			int uid;
			GameName role_name;
			int avatar_type;		//!< 角色模型[1,6]

			ObjID obj_id;			//!< 对象ID
			short hole_sh;
		};

		HostInfo host_info_[WeddingFB::HOST_NUM];	//主办者信息[2]
	};

	class SCVisibleObjEnterPeakTokenObj	//!< 1129 天下至尊令 雕像 进入视野
	{
	public:
		SCVisibleObjEnterPeakTokenObj();
		MessageHeader header;

		ObjID			obj_id;			//!< 对象ID
		short			reserved;		//!< 保留字

		int				speed;			//!< 速度
		short			pos_x;			//!< 在场景中的位置X
		short			pos_y;			//!< 在场景中的位置Y
		Dir				dir;			//!< 方向 [-PI,-PI]
		int				scene_id;		//!< 标识这是哪个场景的obj

		int				creat_uid;		//!< 创建者uid
		GameName		creat_name;		//!< 创建者名字
		Appearance		creat_appearance;	//!< 创建者外观

		long long		act_id;			//!< 活动id
		unsigned int	act_begin_time;	//!< 活动开始时间戳
		unsigned int	act_end_time;	//!< 活动结束时间戳

		int	role_fetch_time;			//!< 角色领奖次数
	};

	class SCVisibleObjEnterTuZiKuaiPaoTuzi	//!< 1138 兔子快跑兔子
	{
	public:
		SCVisibleObjEnterTuZiKuaiPaoTuzi();
		MessageHeader header;

		ObjID			obj_id;			//!< 对象ID
		short			reserved;		//!< 保留字

		int				speed;			//!< 速度
		short			pos_x;			//!< 在场景中的位置X
		short			pos_y;			//!< 在场景中的位置Y
		Scalar			distance;		//!< 方向距离 (即在当前方向上要走的距离）
		Dir				dir;			//!< 方向 [-PI,-PI]
		int				scene_id;		//!< 标识这是哪个场景的obj
		int tu_zi_type;
		int role_id;
		unsigned int time;			//发送时间
		int track_type;
		int track_point_index;
	};

	class SCVisibleObjLeave			//!< 对象离开视野 1103
	{
	public:
		SCVisibleObjLeave();
		MessageHeader	header;		
		ObjID			obj_id;		//!< 对象在场景中的ID
		unsigned short	reserved;	//!< 保留字
	};

	/*
	移动
	*/
	class CSObjMove					//!< 请求移动 1150
	{
	public:
		CSObjMove();
		MessageHeader	header;
		short			pos_x;			//!< 玩家角色在客户端中的当前位置X
		short			pos_y;			//!< 玩家角色在客户端中的当前位置Y
		short			target_pos_x;	//!< 目标位置X
		short			target_pos_y;	//!< 目标位置Y
		int				cur_scene_id;	//!< 客户端当前所在场景ID
	};

	class SCObjMove					//!< 移动返回 1104	\note 会对一个范围内的每个玩家下发，客户端需要判断好是哪个对象在移动
	{
	public:
		SCObjMove();
		MessageHeader	header;
		
		ObjID			obj_id;		//!< 对象ID
		short			reserve_sh;	//!< 保留字
		short			pos_x;		//!< 对象当前的位置X
		short			pos_y;		//!< 对象当前的位置Y
		Dir				dir;		//!< 方向[-PI,PI]
		Scalar			distance;	//!< 方向距离
		
	};

	class SCResetPost				//!< 强设主角位置 1105
	{
	public:
		enum
		{
			RESET_TYPE_DEFAULT,		//!< 默认原因 0
			RESET_TYPE_SLOW,		
			RESET_TYPE_JUMP,
		};

		SCResetPost();
		MessageHeader	header;
		short			x;			//!< 强设到当前场景位置X
		short			y;			//!< 强设到当前场景位置Y
		int				scene_id;	//!< 当前场景ID
	};

	class CSFlyByShoe				//!< 使用飞天神靴请求 1151
	{
	public:
		CSFlyByShoe();
		MessageHeader	header;

		int				scene_id;	//!< 传送至场景ID
		short			pos_x;		//!< 传送至位置X
		short			pos_y;		//!< 传送至位置Y
	};

	class SCChangeSceneNotice		//!< 切换场景通知  1106
	{
	public:
		SCChangeSceneNotice();
		MessageHeader	header;

		int				change_scene_reason;
		int				target_scene_id;
	};

	class CSEnterFB			//!< 1107 请求进入副本
	{
	public:
		CSEnterFB();
		MessageHeader header;

		short fb_type;		//!< 副本类型  \see FB_TYPE
		short reserve;		//!< 保留字
		int param_1;		//!< 参数1
		int param_2;		//!< 参数2
	};

	class CSLeaveFB			//!< 1108 请求离开副本
	{
	public:
		CSLeaveFB();		
		MessageHeader header;
	};

	class SCSceneObjList		//!< 1116 场景中obj位置列表
	{
	public:
		SCSceneObjList();
		MessageHeader header;

		struct ObjInfo
		{
			short pos_x;		//!< 位置x
			short pos_y;		//!< 位置y
			ObjID obj_id;		//!< obj_id
			short obj_type;		//!< obj类型 \see Obj::_ObjType
		};

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_NORMAL = 0,	//!< 0 正常下发（通常在进入场景时）
			NOTIFY_REASON_ADD = 1,		//!< 1 新增obj时下发
			NOTIFY_REASON_DELETE = 2,	//!< 2 删除obj时下发
		};

		static const int MAX_OBJ_NUM = 256;

		short notify_reason;	//!< 0:normal   1:add   2:remove
		short obj_num;			//!< obj数量

		ObjInfo obj_list[MAX_OBJ_NUM];	//!< obj位置列表
	};

	class SCJoinTeamNotifyArea	//!< 1120 加入队伍，通知附近的人
	{
	public:
		SCJoinTeamNotifyArea();
		MessageHeader header;

		ObjID obj_id;			//!< 加入队伍玩家的obj
		short is_red;			//!< 是否红名队伍
		short team_id;			//!< 队伍ID
	};

	class SCLeaveTeamNotifyArea	//!< 1121 离开队伍  通知附近的人
	{
	public:
		SCLeaveTeamNotifyArea();
		MessageHeader header;

		ObjID obj_id;			//!< 离开队伍玩家的obj
		short reserve_sh;		//!< 保留字
	};

	class CSRolePatrol			//!< 2017 开始巡逻/停止巡逻
	{
	public:
		CSRolePatrol();
		MessageHeader header;

		int is_start_or_stop;	//!< 0:停止巡逻  1:开始巡逻
	};

	class CSRoleTaskPathFinding	//!< 2019 玩家任务寻路通知
	{
	public:
		CSRoleTaskPathFinding();
		MessageHeader header;

		int task_id;			//!< 寻路任务ID
		short is_stop;			//!< 是否停止  [0:开始寻路  1:停止寻路]
		short reserve;			//!< 保留字
	};

	class SCRoleSceneEffect						//!< 1122人物buff效果，得到特效
	{
	public:
		enum ROLE_SCENE_EFFECT_TYPE
		{
			ROLE_SCENE_EFFECT_TYPE_0 = 0,//!< 回血
			ROLE_SCENE_EFFECT_TYPE_1 = 1,//!< 回蓝
			ROLE_SCENE_EFFECT_TYPE_2 = 2,//!< 回血回蓝
			ROLE_SCENE_EFFECT_TYPE_3 = 3,//!< 属性buff
			ROLE_SCENE_EFFECT_TYPE_4 = 4,//!< 效果buff
		};
		SCRoleSceneEffect();
		MessageHeader		header;
		ObjID buffer_obj;
		short resever_sh;
		int effect_type;
	};

	class SCSpecialAppearanceChangeNotice	//!< 1123  特殊形象改变通知  
	{
	public:
		SCSpecialAppearanceChangeNotice();
		MessageHeader		header;

		ObjID obj_id;						//!< 改变的objid
		short special_appearance_type;		//!< 特殊外观类型
		int special_appearance_param;		//!< 特殊外观参数
	};

	class CSRoleTalkToNpc						//!< 1124 与NPC对话通用
	{
	public:
		CSRoleTalkToNpc();
		MessageHeader		header;
	};

	class CSEscapeFromCurrentPosition			//!< 1152 客户端脱离卡死
	{
	public:
		CSEscapeFromCurrentPosition();
		MessageHeader		header;
	};

	class CSAutoEscapeFromCurrentPosition		//!< 1153 客户端自动脱离卡死
	{
	public:
		CSAutoEscapeFromCurrentPosition();
		MessageHeader header;
	};

	class CSGetCurPos	//!< 1154 机器人客户端查询自己的位置
	{
	public:
		CSGetCurPos();
		MessageHeader header;
	};

	class SCOjbSpeedChangeNotice			//!< 1155 obj移速改变通知
	{
	public:
		SCOjbSpeedChangeNotice();
		MessageHeader		header;

		int obj_id;
		int move_speed;
	};
	
	class SCGetCurPos
	{
	public:
		SCGetCurPos();
		MessageHeader header;

		int scene_id;
		short pos_x;
		short pos_y;
	};

	class CSWenXiangLouReq
	{
	public:
		CSWenXiangLouReq();		//3374
		MessageHeader header;

		enum REQ_TYPE
		{
			REQ_TYPE_TALK = 0,//交谈				param1:npc_seq
			REQ_TYPE_FIND_ITEM = 1,//查找道具		param1:item_seq
			REQ_TYPE_IDENTITY = 2,//指认			param1:npc_seq
			REQ_TYPE_FIGHT = 3,	//发起结算战斗		无参数
			REQ_TYPE_NPC_INFO = 4,//请求NPC信息
			REQ_TYPE_SCENE_INFO = 5,//请求场景信息
		};

		int req_type;
		int param1;
	};

	class SCWenXiangLouNpcInfo
	{
	public:
		SCWenXiangLouNpcInfo();		//3375
		MessageHeader header;

		char load_status;								// 进行阶段
		char reserve_ch;
		short reserve_sh;
		char npc_sucspicion_list[MAX_NPC_NUM_WXL];		// npc嫌疑值
		char npc_talk_list[MAX_NPC_NUM_WXL];			// npc对话记录
		char npc_identity_list[MAX_NPC_NUM_WXL];		// npc指认记录
		char npc_check_list[MAX_NPC_NUM_WXL];			// npc查证记录（常驻）
	};

	class SCWenXiangLouReqSceneInfo
	{
	public:
		SCWenXiangLouReqSceneInfo();
		MessageHeader header;

		char load_status;		// 进行阶段
		char role_enter_posi;	//是否触发进入特定地点
		char has_find_identity;	//是否指认正确
		char has_finish_fb;		//是否完成副本

		char wenxianglou_has_enter;
		char reserve_ch;
		short reserve_sh;
		//临时道具背包                                                                                                                                                                       
		int reward_count;
		int reward_list[MAX_SPECIAL_ITEM_NUM];//虚拟副本道具包 -- 不允许超过20个
	};

	class SCRAMiJingQiWenShopInfo
	{
	public:
		SCRAMiJingQiWenShopInfo();		//3612 商店信息
		struct ItemInfo
		{
			ItemInfo() :item_id(0), num(0), is_bind(0), price_type(PRICE_TYPE_0), price(0), buy_times(0), discount(0) {}
			enum PRICE_TYPE
			{
				PRICE_TYPE_0 = 0,//元宝
				PRICE_TYPE_2 = 2,//铜钱
			};
			ItemID item_id;
			short num;
			int is_bind;
			int price_type;
			int price;
			int buy_times;
			int m_buy_times;
			int buy_item_idx;//购买发送的下标
			int discount;
		};
		const static int MAX_SHOP_ITEM_NUM = 15;
		MessageHeader header;
		int shop_num;
		ItemInfo shop_list[MAX_SHOP_ITEM_NUM];
	};

	struct MiJingQuestion
	{
		MiJingQuestion() : question_id(0), is_ture(0), answer_index(0) {}
		int question_id;//题目
		int is_ture;	// 是否正确   0：未答题，1：正确，2：错误
		int answer_index;//答案标记 - 展示队长答题的选项
	};

	class SCRAMiJingQiWenQuestionInfo
	{
	public:
		SCRAMiJingQiWenQuestionInfo();		//3614 题目下发
		
		MessageHeader header;

		MiJingQuestion question_info;
	};

	class CSRAMiJingQiWenQuestionReq
	{
	public:
		CSRAMiJingQiWenQuestionReq();		//3615 答题

		MessageHeader header;

		int answer_id;
		int npc_obj_id;
	};

	class SCRAFortuneMazeBoxNum
	{
	public:
		SCRAFortuneMazeBoxNum();		//3516

		MessageHeader header;

		int box_num;
	};

	class CSRAMiJingQiWenBuyItemReq
	{
	public:
		CSRAMiJingQiWenBuyItemReq();		//3619 答题

		MessageHeader header;

		int item_idx;
		int npc_obj_id;
	};

	class SCCloseWindosRet
	{
	public:
		SCCloseWindosRet();		//3620 窗口控制返回

		MessageHeader header;

		int reason;
	};

	class CSCloseWindosReq
	{
	public:
		CSCloseWindosReq();		//3621 关闭窗口

		MessageHeader header;

		int reason;
	};

	class SCRelicLootingTampInfo
	{
	public:
		SCRelicLootingTampInfo();		//!< 3622 游戏状态
		enum RELIC_LOOTING_TYPE
		{
			RELIC_LOOTING_TYPE_ROLE_LOGIN_STATUS = 0,	//!< 等待其他人加载状态
			RELIC_LOOTING_TYPE_READY_START = 1,			//!< 准备开始
			RELIC_LOOTING_TYPE_BEGAIN = 2,				//!< 游戏正式开始
		};

		MessageHeader header;

		int reason_type;
		unsigned int tamp;//!< 此时间戳结束点
	};

	class SCRelicLootingFbSkillInfo
	{
	public:
		SCRelicLootingFbSkillInfo();		//!< 3623  技能信息
		MessageHeader header;
		int stop_reason;
		PlayerSkillList m_skill;
	};

	class CSRelicLootingMatchReq				//!< 3624  遗迹夺宝 请求
	{
	public:
		CSRelicLootingMatchReq();
		MessageHeader header;

		enum TYPE
		{
			TYPE_CANCEL = 0,			//!<取消匹配
			TYPE_MATCH = 1,				//!<请求匹配
			TYPE_RANK = 2,				//!<查询排行
			TYPE_REWARD = 3,			//!<成就奖励 param seq
			TYPE_REWARD_INFO = 4,		//!<成就下发
			TYPE_SET_SHOW = 5,			//!<设置引导完毕
		};

		int is_match;
		int param;
	};

	class SCRelicLootingTeamInfoReq				//!< 3625  遗迹夺宝阵营信息
	{
	public:
		SCRelicLootingTeamInfoReq();
		MessageHeader header;
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//最大阵营人数
		enum TEAM_TYPE
		{
			TEAM_TYPE_DEFENDER = 0,			//!<守卫
			TEAM_TYPE_ATTACKER = 1,			//!<盗宝
		};
		struct TeamInfo
		{
			TeamInfo() : team_type(0), uid(0), gongxian(0), stop_reason(0), on_line(0) {}
			int team_type;
			int uid;
			int gongxian;
			int stop_reason;
			int on_line;
			GameName play_name;
		};

		TeamInfo defend_team_list[MAX_TEAM_INFO_MEMBER_NUM];
		TeamInfo attack_team_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class CSRelicLootingPlayerReq				//!< 3626  遗迹夺宝玩家操作
	{
	public:
		CSRelicLootingPlayerReq();
		MessageHeader header;
		enum MSG_TYPE
		{
			MSG_TYPE_GIVE_UP_SKILL = 1,			//!< 丢弃物品技能
			MSG_TYPE_GET_SKILL = 2,				//!< 获取物品技能 param1 对应的物品objid
			MSG_TYPE_USE_SKILL = 3,				//!< 使用技能
			MSG_TYPE_TS = 4,					//!< 接受传送 param1邀请传送的玩家uid
			MSG_TYPE_MARK = 5,					//!< 标记地图 param1 标记类型，param2 pos_x param3 pos_y
			MSG_TYPE_GIVE_UP_SKILL_STATUS = 6,	//!< 中断技能
		};

		int type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

	class SCRelicLootingObjInfo				//!< 3627  遗迹夺宝地图信息
	{
	public:
		SCRelicLootingObjInfo();
		MessageHeader header;
		const static int MAX_OBJ_NUM = 32;//最大obj数量
		enum OBJ_TYPE
		{
			OBJ_TYPE_BOX = 0,			//!<宝箱
			OBJ_TYPE_SKILL = 1,			//!<技能	param 技能id
			OBJ_TYPE_TRAP = 2,			//!<陷阱
			OBJ_TYPE_DOLL = 3,			//!<傀儡娃娃
			OBJ_TYPE_ROBOT = 4,			//!<稻草人
		};
		enum OBJ_STATUS
		{
			OBJ_STATUS_NORMAL = 0,		//!< 正常
			OBJ_STATUS_NO = 1,			//!< 被交互过了
		};
		struct Objinfo
		{
			Objinfo() : obj_type(0), status(0), param(0), param2(0), obj_pos(0, 0) {}
			int obj_type;
			int status;
			int param;
			int param2;
			Posi obj_pos;
		};
		int obj_count;
		Objinfo obj_list[MAX_OBJ_NUM];
	};

	class SCRelicLootingMemberPosInfo				//!< 3628  遗迹夺宝队员位置
	{
	public:
		SCRelicLootingMemberPosInfo();
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//最大阵营人数
		MessageHeader header;
		struct MemberPos
		{
			MemberPos() {}
			int uid;
			Posi pos;
		};
		int member_num;
		MemberPos member_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class SCRelicLootingMemberTransmitInfo				//!< 3629 遗迹夺宝询问传送
	{
	public:
		SCRelicLootingMemberTransmitInfo();
		MessageHeader header;
		int uid;
		unsigned int end_tamp;
		GameName player_name;
	};

	class SCRelicLootingMarkInfo				//!< 3631 遗迹夺宝标记下发
	{
	public:
		SCRelicLootingMarkInfo();
		MessageHeader header;
		int marker_uid;
		int mark_type;
		Posi mark_pos;
	};

	class SCRelicLootingFBFinishInfo				//!< 3632 遗迹夺宝完成下发
	{
	public:
		SCRelicLootingFBFinishInfo();
		MessageHeader header;
		const static int MAX_TEAM_INFO_MEMBER_NUM = 3;//最大阵营人数
		enum TEAM_TYPE
		{
			TEAM_TYPE_DEFENDER = 0,			//!<守卫
			TEAM_TYPE_ATTACKER = 1,			//!<盗宝
		};
		enum RELIC_LOOTING_WIN_TYPE
		{
			RELIC_LOOTING_WIN_TYPE_DRAW = 0,//超时
			RELIC_LOOTING_WIN_TYPE_WIN_A = 1,//盗宝胜
			RELIC_LOOTING_WIN_TYPE_WIN_D = 2,//守宝胜
		};
		struct TeamInfo
		{
			TeamInfo() : team_type(0), uid(0), gongxian(0), add_score(0), on_line(0)
			{
				memset(play_name, 0, sizeof(GameName));
			}
			int team_type;
			int uid;
			int gongxian;
			int add_score;
			int on_line;
			GameName play_name;
		};
		int win_type;
		TeamInfo defend_team_list[MAX_TEAM_INFO_MEMBER_NUM];
		TeamInfo attack_team_list[MAX_TEAM_INFO_MEMBER_NUM];
	};

	class SCRelicLootingRankInfo				//!< 3633 遗迹夺宝排行榜
	{
	public:
		SCRelicLootingRankInfo();
		MessageHeader header;
		const static int MAX_RANK_NUM = 100;//排行最大下发
		
		int m_pos;//我的排名
		int rank_num;
		RelicLootRankData rank_list[MAX_RANK_NUM];
	};

	class SCRelicLootingAchievementInfo				//!< 3634 遗迹夺宝成就奖励信息
	{
	public:
		SCRelicLootingAchievementInfo();
		MessageHeader header;

		int param[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX];
		int reward_status[ActivityRelicLootingRoleData::RELIC_LOOTING_PARAM_TYPE_MAX];//是否已经领取[0-1]
		int show_flag;
	};

	class SCRelicLootingEffectInfo				//!< 3635 遗迹夺宝玩家状态下发
	{
	public:
		SCRelicLootingEffectInfo();
		MessageHeader header;

		int uid;
		int special_effect_type;
	};


	class SCRAMiJingQiWenShopInfo2
	{
	public:
		SCRAMiJingQiWenShopInfo2();		//4171 商店信息
		struct ItemInfo
		{
			ItemInfo() :item_id(0), num(0), is_bind(0), price_type(PRICE_TYPE_0), price(0), buy_times(0), discount(0) {}
			enum PRICE_TYPE
			{
				PRICE_TYPE_0 = 0,//元宝
				PRICE_TYPE_2 = 2,//铜钱
			};
			ItemID item_id;
			short num;
			int is_bind;
			int price_type;
			int price;
			int buy_times;
			int m_buy_times;
			int buy_item_idx;//购买发送的下标
			int discount;
		};
		const static int MAX_SHOP_ITEM_NUM = 15;
		MessageHeader header;
		int shop_num;
		ItemInfo shop_list[MAX_SHOP_ITEM_NUM];
	};

	struct MiJingQuestion2
	{
		MiJingQuestion2() : question_id(0), is_ture(0), answer_index(0) {}
		int question_id;//题目
		int is_ture;	// 是否正确   0：未答题，1：正确，2：错误
		int answer_index;//答案标记 - 展示队长答题的选项
	};

	class SCRAMiJingQiWenQuestionInfo2
	{
	public:
		SCRAMiJingQiWenQuestionInfo2();		//4172 题目下发

		MessageHeader header;

		MiJingQuestion2 question_info;
	};

	class CSRAMiJingQiWenQuestionReq2
	{
	public:
		CSRAMiJingQiWenQuestionReq2();		//4173 答题

		MessageHeader header;

		int answer_id;
		int npc_obj_id;
	};

	class CSRAMiJingQiWenBuyItemReq2
	{
	public:
		CSRAMiJingQiWenBuyItemReq2();		//4174 答题

		MessageHeader header;

		int item_idx;
		int npc_obj_id;
	};
}

#pragma pack(pop)

#endif

