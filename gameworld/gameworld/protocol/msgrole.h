#ifndef MSGROLE_H
#define MSGROLE_H

#include "gamedef.h"
#include "servercommon/struct/roleinitparam.h"
#include "servercommon/struct/rolequeryparam.hpp"
#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/commondata.hpp"
#include "servercommon/bitmapfunction.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "servercommon/attributedef.hpp"
#include "servercommon/rolesettingdef.hpp"
#include "servercommon/facescoredef.h"
#include "servercommon/partnerdef.h"
#include "item/itembase.h"
#include "equipment/jewelry.h"
#include "equipment/equipment.h"
#include "protocol/msgpet.h"
#include "other/pet/petconfig.hpp"
#include "servercommon/petdef.h"
#include "servercommon/jieyidef.h"
#include "servercommon/smartmountssystemdef.hpp"
#include "other/shengqi/holyequipmanager.hpp"
#include "msgpet.h"

#pragma pack(push, 4)

namespace Protocol
{
	/*
		请求角色信息
	*/
	class SCRoleInfoAck			//!< 1400
	{
	public:
		SCRoleInfoAck();		
		MessageHeader		header;
		
		int role_id;			//!< 角色id
		GameName name;			//!< 角色名 GameName = char[32]
		short level;			//!< 等级
		short profession;		//!< 职业
		int top_level;			//!< 巅峰等级
		int title_id;			//!< 称号
		int facescroe_id;		//!< 颜值系统 | 头衔seq
		long long cur_exp;		//!< 当前经验值

		GuildName guild_name;	//!< 家族名 GuildName = char[32]
		int move_speed;

		Appearance appearance;	//!< 外观

		unsigned int create_time;	//!< 创角时间
		int kill_value;			//!< 杀戮值
		char authority_type;	//!< 权限
		char reserve_ch;
		short reserve_sh;
	};

	class SCRoleSpeedChangeNotice			//!< 1408 obj移速改变通知
	{
	public:
		SCRoleSpeedChangeNotice();
		MessageHeader		header;

		int obj_id;
		int move_speed;
	};

	class SCRoleStatusChangeNotice
	{
	public:
		SCRoleStatusChangeNotice();			//!< 1409 - obj 状态改变通知
		MessageHeader header;

		ObjID obj_id;				//!< 要更新的玩家的 Obj ID
		short cur_status;			//!< 当前状态
		short cur_move_status;		//!< 当前移动状态
		short reserve_sh;
	};

	class SCRoleAutoTalkInfo		//!< 1410 角色自动对话信息下发
	{
	public:
		SCRoleAutoTalkInfo();
		MessageHeader header;

		enum ROLE_AUTO_TALK_NOTICE_TYPE
		{	
			ROLE_AUTO_TALK_NOTICE_TYPE_INIT = 0,	//初始化,刚登陆的时候,不需要给玩家发对话
			ROLE_AUTO_TALK_NOTICE_TYPE_SEND = 1,	//切换对话ID,给玩家发送对话

			ROLE_AUTO_TALK_NOTICE_TYPE_MAX
		};

		int notice_type;
		int talk_groups_id;
		int talk_id;
	};

	class SCRoleAuthorityInfoChange		//!< 1411 角色权限信息改变下发
	{
	public:
		SCRoleAuthorityInfoChange();
		MessageHeader header;

		char authority_type;		// 权限
		char reserve_ch;
		short reserve_sh;
	};

	class SCRoleHistoryMaxCapabilityChange		//!< 1412 角色历史评分变化下发
	{
	public:
		SCRoleHistoryMaxCapabilityChange();
		MessageHeader header;

		int max_role_capability;			// 角色历史最高战力	
	};

	class CSAllInfoReq						//!< 1450 角色所有模块信息请求
	{
	public:
		CSAllInfoReq();
		MessageHeader	header;
	};

	class SCMoney									//!< 1401 角色货币信息
	{
	public:

		struct MoneyInfo
		{
			short reserve_sh;						//!< 保留值
			short money_type;						//!< 货币类型 \see MONEY_TYPE_MAX
			Int64 money_num;						//!< 货币数量
		};

	public:
		SCMoney();
		MessageHeader		header;
		
		short is_no_show;							//!< 是否静默改变 0:否（即要提示玩家)  1:是(即不提示玩家)
		short count;								//!< money_list的数量

		MoneyInfo money_list[MONEY_TYPE_MAX];		//!< 货币列表  \see MoneyInfo  \see MONEY_TYPE_MAX
	};

	class SCRoleAttrList							//!< 1402 属性列表
	{
	public:
		SCRoleAttrList();
		MessageHeader header;

		struct AttrPair
		{
			int attr_type;							//!< 属性类型号 \see enum BATTLE_ATTR
			int attr_value;							//!< 属性的值
		};

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_NORMAL = 0,
			NOTIFY_REASON_SYNC_HPMP_IN_FB = 1,
			NOTIFY_REASON_FA_BAO_DRAW = 2,
			NOTIFY_REASON_HEART_SKILL_EQUIP_CHANGE = 3,
			NOTIFY_REASON_DIAN_HUA_SHI_CAP = 4,
		};

		short attr_count;							//!< 下发的属性条数
		short notify_reason;						//!< 下发的理由 \see NOTIFY_REASON
		int capability;								//!< 玩家战力(评分)
		AttrPair attr_list[BATTLE_ATTR_MAX];		//!< 具体数量由attr_count决定  \see BATTLE_ATTR_MAX
	};

	class SCRoleAttributePlanInfo					//!< 1403 加点方案信息
	{
	public:
		SCRoleAttributePlanInfo();
		MessageHeader header;

		struct PlanDetail
		{
			char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< 计划名  \see ATTRIBUTE_PLAN_NAME_LENGTH
			short remain_point;							//!< 剩余点数
			short add_point_list[ADD_POINT_TYPE_MAX];   //!< 加点的数值 \see ADD_POINT_TYPE_MAX
			short custom_add_point_list[ADD_POINT_TYPE_MAX];
			short reserve_sh;							//!< 保留字
		};
		UNSTD_STATIC_CHECK(ADD_POINT_TYPE_MAX == 5);

		short plan_num;								//!< 当前有多少个方案 范围[1,3]
		short cur_plan_index;						//!< 当前方案序号 范围[0, plan_num)
		PlanDetail plan_list[MAX_ATTR_PLAN_NUM];	//!< 数组长度为3  \see PlanDetail

		short reset_plan_times;						//!< 洗点次数
		short tui_jian_auto_add;							//!< 保留字
	};

	class SCRoleExpChange							//!< 1404 经验改变通知
	{
	public:
		SCRoleExpChange();
		MessageHeader header;

		long long change_exp;						//!< 改变的经验
		long long cur_exp;							//!< 改变后经验
		short change_reason;						//!< 改变的原因  \see ADD_EXP_TYPE
		short reserve_sh;							//!< 保留字
	};

	class SCRoleLevelChange							//!< 1405
	{
	public:
		SCRoleLevelChange();
		MessageHeader header;

		enum ROLE_LEVEL_CHANGE_TYPE
		{
			ROLE_LEVEL_CHANGE_TYPE_NORMAL = 0,		//普通等级
			ROLE_LEVEL_CHANGE_TYPE_TOP = 1,			//巅峰等级
		};

		int change_type;
		int level;									//!< 等级
		long long exp;								//!< 当前经验
	};

	class CSAttributePlanCreate						//!< 1451 创建新加点方案
	{
	public:
		CSAttributePlanCreate();
		MessageHeader header;

		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< 方案名，\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class CSAttributePlanChangeName					//!< 1452 方案改名
	{
	public:
		CSAttributePlanChangeName();
		MessageHeader header;

		int plan_index;								//!< 要改的方案序号[0,3)
		char plan_name[ATTRIBUTE_PLAN_NAME_LENGTH]; //!< 新方案名，\see ATTRIBUTE_PLAN_NAME_LENGTH
	};

	class CSAttributePlanSwitchPlan					//!< 1453 切换方案
	{
	public:
		CSAttributePlanSwitchPlan();
		MessageHeader header;

		int switch_to_index;						//!< 要切换的方案序号[0,3)
	};

	class CSAttributePlanAddPoint					//!< 1454 加点
	{
	public:
		CSAttributePlanAddPoint();
		MessageHeader header;

		short plan_index;							//!< 要加点的方案号
		short delta_attr_list[ADD_POINT_TYPE_MAX];	//!< 改变的点数， 可为负数  \see ADD_POINT_TYPE_MAX
	};

	class CSAttributePlanClearPoint					//!< 1455 洗点
	{
	public:
		CSAttributePlanClearPoint();
		MessageHeader header;

		int plan_index;								//!< 要洗点的方案
	};

	class SCRoleProfessionChangeNotice	//!< 1406 职业变更通知
	{
	public:
		SCRoleProfessionChangeNotice();
		MessageHeader header;

		int new_profession;			//!< 新的职业
	};

	class CSRoleCareerChangeReq		//!< 1457 角色转职
	{
	public:
		CSRoleCareerChangeReq();
		MessageHeader header;

		char	avartar_type;		//!< 虚拟人物类型
		char	color;				//!< 颜色
		short	reserve_sh;			//!< 保留字
		int		to_profession;		//!< 转职到哪个职业
	};

	class CSProfessionExtraInfoReq	//!< 1458 请求职业额外信息
	{
	public:
		CSProfessionExtraInfoReq();
		MessageHeader header;
	};

	class SCProfessionExtraInfo		//!< 1407 返回职业额外信息
	{
	public:
		SCProfessionExtraInfo();
		MessageHeader header;

		int change_prof_times;		//!< 当前已转职次数
	};

	class CSRoleSystemSetReq
	{
	public:
		CSRoleSystemSetReq();		// 1460
		MessageHeader header;
		struct system_set
		{
			short system_set_type;		// 设置类型
			short system_set_param;		// 设置值
		};
		int count;
		system_set system_set_list[RoleSetSystemParam::SYSTEM_SET_INFO_MAX];
	};

	class SCRoleSystemSetInfo
	{
	public:
		SCRoleSystemSetInfo();		// 1461
		MessageHeader header;

		int system_set_list[22];
	};

	class CSRoleSystemSetChatGroupReq	//!< 1462 客户端自定义设置-群组请求
	{
	public:
		CSRoleSystemSetChatGroupReq();
		MessageHeader header;

		long long chat_group_id;		//!< 群组ID
		int set_value;					//!< [0:免打扰 1:打扰]
	};

	class SCRoleSystemSetChatGroupInfo	//!< 1463 客户端自定义设置-群组信息
	{
	public:
		SCRoleSystemSetChatGroupInfo();	
		MessageHeader header;

		struct GroupInfo
		{
			long long chat_group_id;	//!< 群组ID
			int value;					//!< [0:免打扰 1:打扰]
		};

		int chat_group_num;				//!< 群组数量 
		GroupInfo system_set_group_list[MAX_ROLE_CROSS_GROUP]; //!< 按群组数量读取数组
	};

	class CSSetEncounterMineMonsterFlag		//!< 设置遭遇暗怪 2030
	{
	public:
		CSSetEncounterMineMonsterFlag();
		MessageHeader header;

		int flag;					//!< 是否遭遇暗怪  0:否  1：是
	};

	class SCEncounterMineMonsterFlag	//!< 遭遇暗怪返回 2005
	{
	public:
		SCEncounterMineMonsterFlag();
		MessageHeader header;

		int flag;					//!< 是否遭遇暗怪  0:否  1：是
	};

	class CSSetBattlePosition		//!< 设置主角站位 2031  
	{
	public:
		CSSetBattlePosition();
		MessageHeader header;

		int position;				//!< 主角站位  （0：后排   1：前排）
	};

	class SCSetBattlePosition		//!< 设置主角站位返回 2006
	{
	public:
		SCSetBattlePosition();
		MessageHeader header;

		int position;				//!< 主角站位  （0：后排   1：前排）
	};

	class CSSetBattleAuto			//!< 本地设置主角是否自动战斗 2032
	{
	public:
		CSSetBattleAuto();
		MessageHeader header;

		int is_auto;
	};

	class CSSetBattleAutoMove		//!< 本地设置主角自动战斗的行为 2033
	{
	public:
		CSSetBattleAutoMove();
		MessageHeader header;

		short first_move_type;		//!< 行为类型  \see BATTLE_MOVE_TYPE
		short first_skill_id;		//!< 技能ID
		short first_skill_level;	//!< 技能等级

		short second_move_type;		//!< 行为类型  \see BATTLE_MOVE_TYPE
		short second_skill_id;		//!< 技能ID
		short second_skill_level;	//!< 技能等级
	};

	class SCBattleAutoInfo			//!< 本地自动战斗设置信息回包 2007
	{
	public:
		SCBattleAutoInfo();
		MessageHeader header;

		int is_auto;				//!< 当前是否自动
	
		short first_move_type;		//!< 行为类型  \see BATTLE_MOVE_TYPE
		short first_skill_id;		//!< 技能ID
		short first_skill_level;	//!< 技能等级

		short second_move_type;		//!< 行为类型  \see BATTLE_MOVE_TYPE
		short second_skill_id;		//!< 技能ID
		short second_skill_level;	//!< 技能等级
	};

	class CSRoleResetName			//!< 1054 重置名字请求
	{
	public:
		CSRoleResetName();
		MessageHeader	header;

		GameName new_name;			//!< 新的名字 最长64个字节
	};

	class SCRoleResetName			//!< 1004 重置名字回包 (广播给当前场景附近的人）
	{
	public:
		SCRoleResetName();
		MessageHeader header;
		
		ObjID obj_id;				//!< 重置的角色的obj_id  (客户端要判断好是哪个角色在改名）
		short reserved;				//!< 保留字
		GameName new_name;			//!< 新的名字 最长64个字节
	};

	class CSQueryRoleInfo
	{
	public:
		CSQueryRoleInfo();		//!< 2010 查询玩家信息
		MessageHeader header;

		enum QUERY_ROLE_TYPE
		{
			QUERY_ROLE_TYPE_DETAILED = 0,			// 角色详细信息下发
			QUERY_ROLE_TYPE_SIMPLE = 1,				// 角色简易信息下发
		};

		int role_id;			//!< 目标玩家的UID
		int query_type;			
	};

	class SCQueryRoleInfoRet : public IMessageSerializer 				//!< 2011 查询玩家信息下发
	{
	public:
		SCQueryRoleInfoRet();	
		virtual ~SCQueryRoleInfoRet() {}
		MessageHeader header;
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;
			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(role_id, buffer, buffer_size, out_length);
			result = result && WriteStrN(role_name, sizeof(role_name), buffer, buffer_size, out_length);
			result = result && WriteStrN(guild_name, sizeof(guild_name), buffer, buffer_size, out_length);
			result = result && WriteUInt(avatar_type, buffer, buffer_size, out_length);
			result = result && WriteUInt(prof, buffer, buffer_size, out_length);
			result = result && WriteUInt(level, buffer, buffer_size, out_length);
			result = result && WriteInt(top_level, buffer, buffer_size, out_length);
			result = result && WriteInt(capability, buffer, buffer_size, out_length);
			result = result && WriteInt(rank_score, buffer, buffer_size, out_length);
			result = result && WriteShort(pet_count, buffer, buffer_size, out_length);
			result = result && WriteShort(partner_count, buffer, buffer_size, out_length);
			result = result && WriteShort(medal_count, buffer, buffer_size, out_length);
			result = result && WriteChar(is_finish_ding_ji, buffer, buffer_size, out_length);
			result = result && WriteChar(reserve_ch, buffer, buffer_size, out_length);
			result = result && WriteInt(kill_value, buffer, buffer_size, out_length);
			result = result && WriteInt(accessories_count, buffer, buffer_size, out_length);
			result = result && WriteShort(smart_mounts_count, buffer, buffer_size, out_length);
			result = result && WriteShort(holy_equip_count, buffer, buffer_size, out_length);	
			result = result && WriteInt(course_level, buffer, buffer_size, out_length);
			result = result && WriteInt(course_cur_fraction, buffer, buffer_size, out_length);
			result = result && WriteInt(face_score_level, buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&user_attribute, sizeof(user_attribute), buffer, buffer_size, out_length);
			result = result && WriteInt(qing_yuan_role_id, buffer, buffer_size, out_length);
			result = result && WriteStrN(qing_yuan_name, sizeof(qing_yuan_name), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&jie_yi_name, sizeof(jie_yi_name), buffer, buffer_size, out_length);
			
			for (int i = 0; i < ARRAY_LENGTH(jie_yi_name_list); ++i)
			{
				result = result && WriteStrN(jie_yi_name_list[i], sizeof(jie_yi_name_list[i]), buffer, buffer_size, out_length);
			}

			result = result && WriteStrN((const char *)&appearance_info, sizeof(appearance_info), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&equip_list, sizeof(equip_list), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&jewelry_list, sizeof(jewelry_list), buffer, buffer_size, out_length);
			result = result && WriteStrN((const char *)&face_equip_list, sizeof(face_equip_list), buffer, buffer_size, out_length);
			for (int i = 0; i < pet_count && i < MAX_FIGHT_AND_STAND_BY_PET_NUM; i++)
			{
				result = result && WriteStrN((const char *)&pet_list[i], sizeof(pet_list[i]), buffer, buffer_size, out_length);
				result = result && WriteStrN((const char *)&pet_attr_list[i], sizeof(QueryAttrList), buffer, buffer_size, out_length);
				result = result && WriteStrN((const char *)&pet_god_print_list[i], sizeof(GodPrintListPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < partner_count && i < PARTNER_SYNFIGHT_NUM_MAX; i++)
			{
				result = result && WriteStrN((const char *)&partner_list[i], sizeof(PartnerQueryPro), buffer, buffer_size, out_length);
			}
				
			for (int i = 0; i < medal_count && i < MAX_MEDAL_WEAR_GRID; i++)
			{
				result = result && WriteStrN((const char *)&medal_list[i], sizeof(MedalPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < accessories_count && i < ItemNamespace::MAX_ACCESSORIES_INDEX_NUM; i++)
			{
				result = result && WriteStrN((const char *)&accessories_list[i], sizeof(AccessoriesPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < smart_mounts_count && i < ARRAY_LENGTH(smart_mounts_list); i++)
			{
				result = result && WriteStrN((const char *)&smart_mounts_list[i], sizeof(SmartMountsSystemPro), buffer, buffer_size, out_length);
			}

			for (int i = 0; i < holy_equip_count && i < ARRAY_LENGTH(holy_equip_list); i++)
			{
				result = result && WriteStrN((const char *)&holy_equip_list[i], sizeof(HolyEquipPro), buffer, buffer_size, out_length);
			}

			return result;
		}
		int				role_id;
		GameName		role_name;			// 玩家名字
		GuildName		guild_name;			// 家族名字
		UInt32			avatar_type;		// 角色类型
		UInt32			prof;				// 职业
		UInt32			level;				// 人物等级
		int				top_level;			// 巅峰等级
		int				capability;			// 玩家战力
		int				rank_score;			// 幻界战场段位分
		short			pet_count;			// 此时出战+待战的宠物数量
		short			partner_count;		// 伙伴数量
		short			medal_count;		// 勋章数量
		char 			is_finish_ding_ji;	// 是否完成定级赛(幻界战场)
		char			reserve_ch;			
		int				kill_value;			// 杀戮值
		int				accessories_count;	// 配饰数量
		short			smart_mounts_count;	// 灵骑数量
		short			holy_equip_count;	// 圣器数量	
		int				course_level;		// 当前历程等级
		int				course_cur_fraction;// 当前历程点
		int				face_score_level;	// 颜值等级
		QueryAttrList	user_attribute;		// 角色属性
		int				qing_yuan_role_id;	// 情缘角色ID
		GameName		qing_yuan_name;		// 情缘名称
		JieYIZuName 	jie_yi_name;		// 结义组名称
		GameName		jie_yi_name_list[MAX_JIE_YI_ZU_MAX_ROLE_ITEM_NUM];	// 结义成员名称

		Appearance appearance_info;
		EquipmentPro equip_list[Equipment::E_INDEX_MAX];
		JewelryPro jewelry_list[Jewelry::J_TYPE_MAX];
		int face_equip_list[FACE_ADDITION_TYPE_MAX];					// 已装备的（资源ID，详情查看颜值系统配置表）	FACE_ADDITION_TYPE_MAX -- 10
		MsgPetInfo pet_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];				// 不包括跟随宠物		出战+待战				MAX_PET_FIGHT_AND_STANDBY_NUM - 5
		QueryAttrList pet_attr_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// 宠物属性
		GodPrintListPro pet_god_print_list[MAX_FIGHT_AND_STAND_BY_PET_NUM];	// 各宠物神印套装
		PartnerQueryPro partner_list[PARTNER_SYNFIGHT_NUM_MAX];		// 出战伙伴  PARTNER_SYNFIGHT_NUM_MAX -- 5
		MedalPro medal_list[MAX_MEDAL_WEAR_GRID];				// 穿戴令牌 MAX_MEDAL_WEAR_GRID -- 3
		AccessoriesPro accessories_list[ItemNamespace::MAX_ACCESSORIES_INDEX_NUM];// 穿戴配饰 MAX_ACCESSORIES_INDEX_NUM -- 5
		SmartMountsSystemPro smart_mounts_list[MAX_SMART_MOUNTS_SYSTEM_NUM];	// 激活灵骑列表 MAX_SMART_MOUNTS_SYSTEM_NUM -- 10
		HolyEquipPro holy_equip_list[MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM];			// 出战和待机圣器列表 MAX_HOLY_EQUIP_FIGHT_STANDBY_NUM -- 5
	};

	class CSChangeHeadshot				//!< 2035 改变头像
	{
	public:
		CSChangeHeadshot();			
		MessageHeader header;
		
		short headshot_id;				//!< 头像ID
		short reserve_sh;				//!< 保留字
	};

	class SCAttrListReason 			//!< 2036 属性来源列表下发	
	{
	public:
		SCAttrListReason();
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_ROLE = 0,			// 角色
			REASON_TYPE_PET = 1,			// 宠物
			REASON_TYPE_PARTNER = 2,		// 伙伴
			REASON_TYPE_HUASHEN_ROLE = 3,	// 化身角色
			REASON_TYPE_HUASHEN_PET = 4,	// 化身宠物

			REASON_TYPE_MAX,
		};

		short reason_type;
		short param1;
		AttributeList reason_list[MAX_REASON_TYPE_LIST_NUM];	//属性来源列表
	};

	class CSAttrListReasonReq			//!< 2037 请求来源属性列表下发
	{
	public:
		CSAttrListReasonReq();
		MessageHeader header;

		short reason_type;				//REASON_TYPE_MAX
		short param1;					//角色时默认0,宠物填pet_index, 伙伴填partner_id
	};

	class CSRoleInfoNoticeReq     //!< 2850
	{
	public:
		CSRoleInfoNoticeReq();
		MessageHeader header;
	};

	class SCRoleInfoNotice		//!< 2851
	{
	public:
		SCRoleInfoNotice();
		MessageHeader header;
		int is_cross;
	};

	class CSRoleTopLevelReq		//!< 2860 巅峰等级请求
	{
	public:
		CSRoleTopLevelReq();
		MessageHeader header;

		enum ROLE_TOP_LEVEL_REQ_TYPE
		{
			ROLE_TOP_LEVEL_REQ_TYPE_NOTICE = 0,				//点击弹窗
			ROLE_TOP_LEVEL_REQ_TYPE_FETCH = 1,				//领取奖励 p1 top_level
			ROLE_TOP_LEVEL_REQ_TYPE_FETCH_ACHIEVE = 2,		//领取成就奖励 p1 seq
		};

		short op_type;
		short param1;
	};

	class SCRoleTopLevelAllInfo	//!< 2861 巅峰等级所有信息下发
	{
	public:
		SCRoleTopLevelAllInfo();
		MessageHeader header;

		char is_refresh_first_notice_top_level_flag;			// 刷新后首次弹出 0:未弹  1:已弹
		char reserve_ch;
		short reserve_sh;
		int top_level;
		unsigned int next_top_level_refresh_timestamp;			// 巅峰等级重置时间戳
		BitMap<MAX_ROLE_TOP_LEVEL_DB_NUM> role_top_level_reward_fetch_flag;
		BitMap<MAX_ROLE_TOP_LEVEL_ACHIEVE_NUM> top_level_achieve_flag;	//巅峰等级成就奖励领取标识
	};

	class SCRoleTopLevelNoticce		//!< 2862 巅峰等级通知
	{
	public:
		SCRoleTopLevelNoticce();
		MessageHeader header;

		enum ROLE_TOP_LEVEL_NOTICE_TYPE
		{
			ROLE_TOP_LEVEL_NOTICE_TYPE_NOITCE = 0,			// 弹窗		P1 0:未弹 1:已弹
			ROLE_TOP_LEVEL_NOTICE_TYPE_FETCH = 1,			// 领取等级奖励  p1 0:未领取 1:已领取 P2 top_level
			ROLE_TOP_LEVEL_NOITCE_TYPE_FETCH_ACHIEVE = 2,	// 领取成就奖励 p1 0:未领取 1:已领取 p2 seq
		};

		short notice_type;
		short param1;
		int param2;
	};

	class SCRoleTopLevelAOINotice	//!< 2863 巅峰等级通知周边玩家
	{
	public:
		SCRoleTopLevelAOINotice();
		MessageHeader header;

		ObjID obj_id;
		short reserve_sh;
		int top_level;		//巅峰等级
	};

	////////////////////////////////////////////////////////////////////
	// 资源下载

	class CSFetchRecourceDownloadReward			// 3955 获取资源奖励
	{
	public:
		CSFetchRecourceDownloadReward();
		MessageHeader		header;
	};

	class SCFetchRecourceDownloadReward			// 3956 获取资源奖励变更
	{
	public:
		SCFetchRecourceDownloadReward();
		MessageHeader		header;

		int flag;								//是否领取标记
	};

	////////////////////////////////////////////////////////////////////
	// 防诈骗奖励
	class CSFangFraudRewardReq					// 3957 防诈骗答题
	{
	public:
		static const int MAX_REQ_NUM = 4;			// 多选答案上限

		CSFangFraudRewardReq();
		MessageHeader		header;

		int count;
		short list[MAX_REQ_NUM];
	};

	class SCFangFraudRewardInfo					// 3958 防诈骗奖励标记变更
	{
	public:
		SCFangFraudRewardInfo();
		MessageHeader		header;

		int flag;								//是否领取标记
	};

	class SCQueryRoleSimpleInfo					//!< 2025 人物简易信息下发
	{
	public:
		SCQueryRoleSimpleInfo();
		MessageHeader header;

		int role_id;
		GameName		role_name;			// 玩家名字
		UInt32			level;				// 人物等级
		int				top_level;			// 巅峰等级
		int				capability;			// 玩家战力
		Appearance appearance_info;			// 角色外观
	};

	////////////////////////////////////////////////////////////////////
	// 评价有礼

	class CSFetchPingJiaRewardReq			// 3968 评价有礼
	{
	public:
		CSFetchPingJiaRewardReq();
		MessageHeader		header;
	};

	class SCFetchPingJiaRewardInfo			// 3969 评价有礼信息
	{
	public:
		SCFetchPingJiaRewardInfo();
		MessageHeader		header;

		int flag;								//是否领取标记
	};
}

#pragma pack(pop)
#endif