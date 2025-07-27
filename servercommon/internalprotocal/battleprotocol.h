#ifndef __BATTLE_PROTOCOL_H__
#define __BATTLE_PROTOCOL_H__

#include "servercommon/serverdef.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/msgserialize.hpp"

#pragma pack(push)
#pragma pack(4)

namespace battleloginprotocol
{
	enum _MsgType
	{
		MT_INVALID,
		MT_REGISTER_TO_LOGIN,		// battle向login注册自己 (然后login把battle信息同步给gateway让其连接battle)
	
		MT_STORY_BATTLE_INFO,		// battle向login发送剧情战斗的整体数据
		MT_STORY_ROUND_RESULT,		// battle向login发送回合结果
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID), reserve_sh(0) {}
		MessageHeader(int type) : msg_type(type) {}

		short msg_type;
		short reserve_sh;
	};

	struct BL_RegisterToLogin
	{
		BL_RegisterToLogin() : header(MT_REGISTER_TO_LOGIN) {}
		MessageHeader header;

		int index;
		char ip[16];
		unsigned short port_for_gateway;
		unsigned short port_for_gameworld;
	};

	struct BL_SyncStoryBattleInfoMsg
	{
		BL_SyncStoryBattleInfoMsg() : header(MT_STORY_BATTLE_INFO) {}
		MessageHeader header;

		// 以下是Protocol::SCBattleInfo的数据
	};

	struct BL_SyncStoryRoundResultMsg
	{
		BL_SyncStoryRoundResultMsg() : header(MT_STORY_ROUND_RESULT) {}
		MessageHeader header;

		int round;

		// 以下是Protocol::SCBattleRoundResult的数据
	};
}

/*
namespace battlegateprotocol
{
	enum _MsgType
	{
		MT_GB_BEGIN = 0,
		MT_REGISTER_TO_BATTLE,		// gateway向battle注册自己
		MT_NET_RECV_USER_MSG,		// gateway收到用户请求并转发到battle

		MT_BG_BEGIN = 20,
		MT_NET_SEND_USER_MSG,		// battle向用户发回包，通过gateway转发
	};

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_GB_BEGIN), user_netid(0) {}
		MessageHeader(int type) : msg_type(type) {}

		short msg_type;
		unsigned short user_netid;
	};

	struct GB_RegisterToBattle
	{
		GB_RegisterToBattle() : header(MT_REGISTER_TO_BATTLE) {}
		MessageHeader header;

		int gate_index;
	};

	struct GB_NetRecvUserMsg
	{
		GB_NetRecvUserMsg() : header(MT_NET_RECV_USER_MSG) {}
		MessageHeader header;
		// 该地址以下为消息实体
	};

	struct BG_NetSendUserMsg
	{
		BG_NetSendUserMsg() : header(MT_NET_SEND_USER_MSG) {}
		MessageHeader header;
		// 该地址以下为消息实体
	};
}
*/

namespace battlegameprotocol
{
	// GAME_BATTLE  -> 游戏服发到战斗服
	// BATTLE_GAME  -> 战斗服发到游戏服
	enum _MsgType
	{
		MT_INVALID,

		MT_BATTLE_GAME_PROTOCOL_BEGIN = MT_INVALID,

		// 游戏服为玩家转发发向战斗服的请求
		MT_GAME_BATTLE_USER_MSG_REQ,		

		// 战斗服返回， 经由游戏服转发给玩家（应尽量直接由战斗服下发，只有当战斗服找不到对应的gateway时才借由游戏服下发）
		MT_BATTLE_GAME_USER_MSG_RESP,

		// 战斗服返回， 经由游戏服转发给玩家(直接通过UID查找Role)
		MT_BATTLE_GAME_USER_MSG_RESP_BY_UID,

		// 战斗开始请求（单人PVE)
		MT_GAME_BATTLE_START_SINGLE_PVE_REQ,

		// 战斗开始返回确认
		MT_BATTLE_GAME_START_FIGHT_ACK, 

		// 战斗结果通知
		MT_BATTLE_GAME_FIGHT_RESULT_NOTICE,	

		// 战斗DEMO开始请求
		MT_GAME_BATTLE_START_DEMO_REQ,

		// 看动画结束
		MT_GAME_BATTLE_ANIMATION_FINISH_REQ,

		// 玩家逃跑
		MT_BATTLE_GAME_ROLE_RUN_AWAY,

		// 强制结束战斗
		MT_GAME_BATTLE_FORCE_FINISH_FIGHT,

		// 强制结束战斗返回
		MT_BATTLE_GAME_FORCE_FINISH_FIGHT,

		// 断线重连战斗
		MT_GAME_BATTLE_ROLE_LOGIN, 

		// 玩家断线通知战斗服
		MT_GAME_BATTLE_ROLE_LOGOUT,

		// 玩家战斗操作
		MT_GAME_BATTLE_ROLE_OPERATE,

		// 玩家设置自动
		MT_BATTLE_GAME_ROLE_SET_AUTO,

		// 玩家设置自动行为
		MT_BATTLE_GAME_ROLE_SET_AUTO_MOVE,

		// 玩家输入倒计时结束
		MT_GAME_BATTLE_ROLE_FINISH_COUNTDOWN,

		// 玩家战斗中使用回血物品
		MT_GAME_BATTLE_ROLE_USE_HEAL_ITEM,

		// 玩家战斗中使用回蓝物品
		MT_GAME_BATTLE_ROLE_USE_MP_ITEM,

		// 玩家战斗中使用物品，同步回游戏服做消耗
		MT_BATTLE_GAME_ROLE_USE_ITEM,

		// 玩家战斗中使用装备
		MT_GAME_BATTLE_ROLE_USE_EQUIPMENT,

		// 玩家战斗中封印宠物
		MT_GAME_BATTLE_ROLE_SEAL_PET,

		// 玩家战斗中封印宠物，同步回游戏服做消耗
		MT_BATTLE_GAME_ROLE_SEAL_PET,

		// 玩家战斗中使用瞬杀怪物的道具
		MT_GAME_BATTLE_ROLE_USE_INST_KILL_MONSTER_ITEM,

		// 玩家战斗中使用瞬杀怪物的道具，同步回游戏服做消耗
		MT_BATTLE_GAME_ROLE_INST_KILL_MONSTER,

		// 开始组队战斗请求
		MT_GAME_BATTLE_START_TEAM_PVE_REQ,

		// 服务器重读配置
		MT_GAME_BATTLE_RELOAD_CONFIG_REQ,

		// 通知游戏服Battle实例已被释放
		MT_BATTLE_GAME_BATTLE_INSTANCE_RELEASED,

		// 世界BOSS血量通知
		MT_BATTLE_GAME_WORLD_BOSS_HP_SYNC,
		// 世界BOSS2 (巨龙boss)血量通知
		MT_BATTLE_GAME_WORLD_BOSS_2_HP_SYNC,
		// 世界BOSS3 (天神降临)血量通知
		MT_BATTLE_GAME_WORLD_BOSS_3_HP_SYNC,

		// 单人PVP
		MT_GAME_BATTLE_START_SINGLE_PVP_REQ,

		// 检查角色是否真的正在战斗
		MT_GAME_BATTLE_ROLE_REALLY_IN_BATTLE_REQ,

		// 返回角色是否真的在战斗
		MT_BATTLE_GAME_ROLE_REALLY_IN_BATTLE_RESP,

		// 开始多人PVP请求
		MT_GAME_BATTLE_MULTI_PVP_REQ,

		// 开始单人打影子请求
		MT_GAME_BATTLE_START_FIGHT_SINGLE_SHADOW_REQ,

		// 开始多人打影子请求
		MT_GAME_BATTLE_START_FIGHT_MULTI_SHADOW_REQ,

		// 开始观战
		MT_GAME_BATTLE_START_OB_REQ,

		// 开始观战（以某方为视角）
		MT_GAME_BATTLE_START_OB_SIDE_REQ,

		// 家族荣誉战 向game请求后补人员
		MT_BATTLE_GAME_GUILD_HONOR_FIGHT_REQUEST_BACKUP,

		// 家族荣誉战 向battle派遣后补人员
		MT_GAME_BATTLE_GUILD_HONOR_FIGHT_SEND_BACKUP,

		// 家族荣誉战 通知游戏服某几个玩家已挂掉
		MT_BATTLE_GAME_GUILD_HONOR_ROLE_DIE_LIST,

		// 擂台战开始战斗
		MT_GAME_BATTLE_START_PLATFORM_FIGHT_REQ,

		// 设置战术指挥
		MT_GAME_BATTLE_SET_STRATEGY_REQ,

		// 清除战术指挥
		MT_GAME_BATTLE_CLEAR_STRATEGY_REQ,

		// 离开观战（包括击飞观战和普通的观战）
		MT_GAME_BATTLE_LEAVE_OB_REQ,

		// 强制结束指定模式的战斗
		MT_GAME_BATTLE_FORCE_FINISH_BATTLE_MODE_REQ,

		// 强制结束指定模式的战斗返回
		MT_BATTLE_GAME_FORCE_FINISH_BATTLE_MODE_RESP,

		// 战斗服通知游戏服，玩家从观战变为战斗（增援逻辑）
		MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE,

		// 战斗服通知游戏服，玩家从战斗变为观战（增援逻辑）
		MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE_END,

		// 玩家暂停战斗
		MT_GAME_BATTLE_PAUSE_BATTLE_REQ,

		// 玩家战斗中发弹幕
		MT_GAME_BATTLE_ROLE_CHAT_REQ,

		// 玩家在战斗后离队
		MT_GAME_BATTLE_ROLE_LEAVE_TEAM_AFTER_BATTLE,

		// 玩家完成战斗准备
		MT_GAME_BATTLE_ROLE_FINISH_PREPARE,

		// 玩家OB状态同步
		MT_BATTLE_GAME_ROLE_SYNC_OB_STATUS,

		// 战斗服通知游戏服 战斗开始失败
		MT_BATTLE_GAME_START_FIGHT_FAILED,

		// 游戏服向战斗服查询战斗是否开始失败
		MT_GAME_BATTLE_CHECK_BATTLE_START_FAILED,

		// 战斗服向游戏服回复战斗是否开始失败
		MT_BATTLE_GAME_CHECK_BATTLE_START_FAILED_ACK,

		// 开始特殊战斗玩法
		MT_GAME_BATTLE_START_FIGHT_SPECIAL_REQ,

		// 特殊战斗玩法操作
		MT_GAME_BATTLE_START_FIGHT_SPECIAL_OPERATE_REQ,

		// 特殊玩法返回
		MT_GAME_BATTLE_SPECIAL_RESULT_NOTICE,

		// 战斗放弃
		MT_GAME_BATTLE_BATTLE_GIVEUP,

		// 玩家重进战斗
		MT_GAME_BATTLE_ROLE_REJOIN_BATTLE_SCENE,

		// 强制结束观战通知（游戏服通知战斗服清除一些观战的信息，但不需要回包，因为游戏服自行已经处理了）
		MT_GAME_BATTLE_FORCE_STOP_OB_NOTICE,

		// 玩家撤销一动
		MT_GAME_BATTLE_ROLE_UNDO_REQ,

		// 玩家调整观看战斗速度
		MT_GAME_BATTLE_MODIFY_ANIMATION_SPEED_REQ,

		// 玩家调整战斗速度返回
		MT_BATTLE_GAME_MODIFY_ANIMATION_SPEED_RESP,

		// 家族荣誉战通知增员开始战斗或战斗完成
		MT_BATTLE_GAME_GUILD_HONOR_ROLE_STATE_CHANGE_NOTICE,

		// 家族荣誉战每回合同步一次总伤害
		MT_BATTLE_GAME_GUILD_HONOR_SYNC_ROUND_TOTAL_DAMAGE,

		// 通知战斗服读取热更新代码
		MT_GAME_BATTLE_HOTFIX,

		// 战斗服同步世界BOSS玩家回合数据到游戏服
		MT_BATTLE_GAME_WORLD_BOSS_SYNC_ROLE_STAT,
		MT_BATTLE_GAME_WORLD_BOSS_2_SYNC_ROLE_STAT,
		MT_BATTLE_GAME_WORLD_BOSS_3_SYNC_ROLE_STAT,

		//世界BOSS游戏服向战斗服设置boss 血量
		MT_GAME_BATTLE_WORLD_BOSS_CHANGE_HP,
		MT_GAME_BATTLE_WORLD_BOSS_2_CHANGE_HP,
		MT_GAME_BATTLE_WORLD_BOSS_3_CHANGE_HP,

		//设置自定义指令
		MT_GAME_BATTLE_SET_CUSTOM_INSTRUCTION,

		// 只同步宠物的自动指令
		MT_BATTLE_GAME_ONLY_SET_PET_AUTO_MOVE,

		// 跳过战斗
		MT_GAME_BATTLE_SKIP_BATTLE_REQ,

		// 速战
		MT_GAME_BATTLE_FAST_FORWARD_REQ,

		// 伙伴PVE
		MT_GAME_BATTLE_START_PARTNER_PVE_REQ,

		// 释放变身卡技能,同步游戏服消耗幻化点
		MT_BATTLE_GAME_PERFORM_CHANGE_CARD_SKILL,

		// UTA每回合同步玩家的伤害、承伤等数据
		MT_BATTLE_GAME_UTA_SYNC_ROLE_STAT,

		// UTA每回合开始同步OB回合初数据
		MT_BATTLE_GAME_UTA_SYNC_ROUND_START_DATA,

		// UTA每回合战斗数据同步用于OB
		MT_BATTLE_GAME_UTA_SYNC_ROUND_RESULT_DATA,

		// 战斗投降
		MT_GAME_BATTLE_BATTLE_SURRENDER,

		MT_BATTLE_GAME_MAX,
	};

	static const int BATTLE_GAME_MSG_NUM_MAX = 1024;

	struct MessageHeader
	{
		explicit MessageHeader() : msg_type(MT_INVALID) {}
		explicit MessageHeader(int _msg_type) : msg_type(_msg_type) {}

		int msg_type;
	};

	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len);

	struct GameBattleUserMsgReq
	{
		GameBattleUserMsgReq() : header(MT_GAME_BATTLE_USER_MSG_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;

		// 紧接着Protocol的内容
	};

	struct BattleGameUserMsgResp
	{
		BattleGameUserMsgResp() : header(MT_BATTLE_GAME_USER_MSG_RESP) {}
		MessageHeader header;

		BGNetID bg_netid;

		// 紧接着Protocol的内容
	};

	struct BattleGameUserMsgRespByUid
	{
		BattleGameUserMsgRespByUid() : header(MT_BATTLE_GAME_USER_MSG_RESP_BY_UID) {}
		MessageHeader header;

		int uid;

		// 紧接着Protocol的内容
	};

	struct BattleGameSpecialBusinessData	// 用于特殊业务数据  外挂到Battle发给Game的协议中，实现特殊业务的数据传输
	{
		short battle_mode;
		short data_type;	//!< \enum BATTLE_BUSINESS_DATA_TYPE
		int data_length;
		char data[BATTLE_BUSINESS_DATA_BUFFER_LEN];
	};

	struct GameBattleStartSinglePVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSinglePVEReq() : header(MT_GAME_BATTLE_START_SINGLE_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID bg_netid;
		int uid;

		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int monster_group_id;	// 怪物组ID
		int battle_id;	// 本地战斗ID标识

		BattleData_Business business_data; // 用于原样返回的业务数据

		RoleBattleData role_battle_data;
	};

	struct GameBattleStartSinglePVPReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSinglePVPReq() : header(MT_GAME_BATTLE_START_SINGLE_PVP_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID attacker_bgnetid;
		int attacker_uid;

		BGNetID defender_bgnetid;
		int defender_uid;

		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int battle_id;	// 战斗ID标识

		BattleData_Business business_data; // 用于原样返回的业务数据

		RoleBattleData attacker_battle_data;
		RoleBattleData defender_battle_data;
	};

	struct GameBattleStartTeamPVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartTeamPVEReq() : header(MT_GAME_BATTLE_START_TEAM_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		int team_id;
		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int monster_group_id;	// 怪物组ID
		int battle_id;	// 本地战斗ID标识

		BattleData_Business business_data; // 用于原样返回的业务数据

		short leader_idx;
		short role_num;
		MemberData member_battle_data[SIDE_MAX_ROLE_NUM];

		int pos_id_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartMultiPVPReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartMultiPVPReq() : header(MT_GAME_BATTLE_MULTI_PVP_REQ){}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		struct ObserverData
		{
			BGNetID bg_netid;
			short side;
			int uid;
		};

		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int battle_id;	// 战斗ID标识
		BattleData_Business business_data; // 用于原样返回的业务数据

		short attacker_num;
		short defender_num;
		MemberData attacker_data_list[SIDE_MAX_ROLE_NUM];
		MemberData defender_data_list[SIDE_MAX_ROLE_NUM];

		int observer_num;
		ObserverData observer[BATTLE_OBSERVER_MAX_NUM];

		int attacker_pos_id_list[SIDE_MAX_ROLE_NUM];
		int defender_pos_id_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartSingleFightShadowReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSingleFightShadowReq() : header(MT_GAME_BATTLE_START_FIGHT_SINGLE_SHADOW_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		BGNetID attacker_bgnetid;
		int attacker_uid;

		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int battle_id;	// 本地战斗ID标识

		BattleData_Business business_data; // 用于原样返回的业务数据

		RoleBattleData attacker_battle_data;
		RoleBattleData defender_battle_data;
	};

	struct GameBattleStartMultiFightShadowReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartMultiFightShadowReq() : header(MT_GAME_BATTLE_START_FIGHT_MULTI_SHADOW_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct AttackerInfo
		{
			BGNetID bgnetid;
			int uid;
		};

		short battle_mode;			// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int battle_id;				// 本地战斗ID标识

		BattleData_Business business_data; // 用于原样返回的业务数据

		short attacker_num;
		short defender_num;

		AttackerInfo attacker_list[SIDE_MAX_ROLE_NUM];
		RoleBattleData attacker_data_list[SIDE_MAX_ROLE_NUM];
		RoleBattleData defender_data_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartSpecialReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartSpecialReq() : header(MT_GAME_BATTLE_START_FIGHT_SPECIAL_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			GameName			role_name;
			short				m_avatar_type;									// 虚拟人物类型
			short				reserve_sh;
			long long			m_avatar_timestamp;								// 玩家自上传头像
		};

		short battle_mode;		// 战斗模式标识 
		int battle_id;			// 战斗ID标识

		int attacker_count;
		MemberData attacker_data[SIDE_MAX_ROLE_NUM];
		int defender_count;
		MemberData defender_data[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleStartPlatformFightReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartPlatformFightReq() : header(MT_GAME_BATTLE_START_PLATFORM_FIGHT_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int attacker_uid;
		BGNetID bg_netid;
		int battle_id;

		int animation_speed_mode;

		int atk_pet_num;
		PetBattleData atk_pet_list[SIDE_MAX_ROLE_NUM];
		int atk_pet_pos_list[SIDE_MAX_ROLE_NUM];

		int atk_partner_num;
		PartnerBattleData atk_partner_list[SIDE_MAX_ROLE_NUM];
		int atk_partner_pos_list[SIDE_MAX_ROLE_NUM];

		int defender_uid;
		int def_pet_num;
		PetBattleData def_pet_list[SIDE_MAX_ROLE_NUM];
		int def_pet_pos_list[SIDE_MAX_ROLE_NUM];

		int def_partner_num;
		PartnerBattleData def_partner_list[SIDE_MAX_ROLE_NUM];
		int def_partner_pos_list[SIDE_MAX_ROLE_NUM];

		BattleData_Business business_data;
	};

	struct GameBattleStartPartnerPVEReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleStartPartnerPVEReq() : header(MT_GAME_BATTLE_START_PARTNER_PVE_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int attacker_uid;
		BGNetID bg_netid;
		int battle_id;

		short battle_mode;		// 战斗模式标识 
		short animation_speed_mode; // 动画速度
		int monster_group_id;	// 怪物组ID

		int atk_partner_num;
		PartnerBattleData atk_partner_list[SIDE_MAX_ROLE_NUM];
		int atk_partner_pos_list[SIDE_MAX_ROLE_NUM];

		BattleData_Business business_data;
	};

	struct BattleGameStartFightAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameStartFightAck() : header(MT_BATTLE_GAME_START_FIGHT_ACK) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			short grid_idx;
			short reserve_sh;
		};

		int battle_id;
		int battle_mode;
		short is_pvp;
		short is_team_up;
		short animation_speed_type;
		short reserve_sh;
		int role_count;
		RoleInfo role_list[BATTLE_MAX_ROLE_NUM];

		BattleGameSpecialBusinessData special_business_data;	// 必须放在协议最后
	};

	struct BattleGameFightResultAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameFightResultAck() : header(MT_BATTLE_GAME_FIGHT_RESULT_NOTICE) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			int level;
			char is_flyaway;
			char is_runaway;
			char is_leader;
			char pet_num;
			int pet_id_list[ROLE_BATTLE_PET_NUM];
			int pet_level_list[ROLE_BATTLE_PET_NUM];
			long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole pet_stat_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole role_statistic;
			BattleData_BusinessRole role_business;
		};

		struct RoleList
		{
			int role_count;
			RoleInfo role_list[SIDE_MAX_ROLE_NUM];
		};

		int battle_id;
		int battle_mode;
		int monster_group_id;
		int battle_result;							//!< \see RESULT_TYPE

		RoleList attacker_list;
		RoleList defender_list;

		BattleData_Business business_data;			// 单人与多人的业务数据结构可能不同， 后面看需求，可以传char[]
		BattleData_Statistic statistic_data;		// 统计战斗数据

		BattleGameSpecialBusinessData special_business_data; // 必须放在协议最后  
	};

	struct GameBattleDemoStartReq
	{
		GameBattleDemoStartReq() : header(MT_GAME_BATTLE_START_DEMO_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int battle_id;

		int demo_seq;	// 若为0，则使用Demo配置，否则使用NpcDemo配置
		int battle_mode;
		short avatar_type;

		// 玩家自动设置
		short is_auto;
		short auto_first_move_type;
		short auto_first_skill_id;
		short auto_first_skill_level;
		short auto_second_move_type;
		short auto_second_skill_id;
		short auto_second_skill_level;

		BattleData_Business business_data;
	};

	struct GameBattlePrepareFinishReq
	{
		GameBattlePrepareFinishReq() : header(MT_GAME_BATTLE_ROLE_FINISH_PREPARE){}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleAnimationFinishReq
	{
		GameBattleAnimationFinishReq() : header(MT_GAME_BATTLE_ANIMATION_FINISH_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleCountDownFinishReq
	{
		GameBattleCountDownFinishReq() : header(MT_GAME_BATTLE_ROLE_FINISH_COUNTDOWN) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGameRoleRunAway : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameRoleRunAway() : header(MT_BATTLE_GAME_ROLE_RUN_AWAY) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		int battle_id;
		int battle_mode;
		int uid;
		short reserve_sh;
		short is_leave_team;
		BattleData_Business business_data;	// 业务数据
		BattleData_Statistic statistic_data;
		BattleGameSpecialBusinessData special_business_data; // 必须放在协议最后
	};

	struct GameBattleForceFinishFight
	{
		GameBattleForceFinishFight() : header(MT_GAME_BATTLE_FORCE_FINISH_FIGHT) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct BattleGameForceFinishFight : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameForceFinishFight() : header(MT_BATTLE_GAME_FORCE_FINISH_FIGHT) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct RoleInfo
		{
			int uid;
			char is_flyaway;
			char is_runaway;
			char is_leader;
			char pet_num;
			int pet_id_list[ROLE_BATTLE_PET_NUM];
			int pet_lv_list[ROLE_BATTLE_PET_NUM];
			long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole pet_stat_list[ROLE_BATTLE_PET_NUM];
			BattleData_StatisticRole role_statistic;
			BattleData_BusinessRole role_business;
		};

		struct RoleList
		{
			int role_count;
			RoleInfo role_list[SIDE_MAX_ROLE_NUM];
		};

		int battle_id;
		int battle_mode;
		int monster_group_id;
		int result_type;

		RoleList attacker_list;
		RoleList defender_list;

		BattleData_Business business_data;			// 单人与多人的业务数据结构可能不同， 后面看需求，可以传char[]
		BattleData_Statistic statistic_data;		// 统计战斗数据

		BattleGameSpecialBusinessData special_business_data; // 必须放在协议最后
	};

	struct GameBattleRoleOperate
	{
		GameBattleRoleOperate() : header(MT_GAME_BATTLE_ROLE_OPERATE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int operate_type;
		int param1;
		int param2;
		int param3;
		int param4;
	};

	struct BattleGameRoleSetAuto
	{
		BattleGameRoleSetAuto() : header(MT_BATTLE_GAME_ROLE_SET_AUTO) {}
		MessageHeader header;

		int role_id;
		int is_set_auto;
	};

	struct GameBattleRoleLogin
	{
		GameBattleRoleLogin() : header(MT_GAME_BATTLE_ROLE_LOGIN) {}
		MessageHeader header;

		int role_id;
		BGNetID bg_netid;
		short is_ob;
		short reserve_sh;
	};

	struct GameBattleRoleLogout
	{
		GameBattleRoleLogout() : header(MT_GAME_BATTLE_ROLE_LOGOUT) {}
		MessageHeader header;

		int role_id;
		int battle_id;
	};

	struct BattleGameRoleSetAutoMove
	{
		BattleGameRoleSetAutoMove() : header(MT_BATTLE_GAME_ROLE_SET_AUTO_MOVE) {}
		MessageHeader header;

		int role_id;

		short first_move_type;
		short first_skill_id;
		short first_skill_level;

		short second_move_type;
		short second_skill_id;
		short second_skill_level;

		int second_move_pet_id;
		long long second_move_pet_unique_id;

		short reserve_sh;
		short pet_num;
		int pet_id_list[ROLE_BATTLE_PET_NUM];
		long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_move_type_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_level_list[ROLE_BATTLE_PET_NUM];
		short reserve_sh1;
		
		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);
	};

	struct BattleGameOnlySyncPetAutoMove
	{
		BattleGameOnlySyncPetAutoMove() : header(MT_BATTLE_GAME_ONLY_SET_PET_AUTO_MOVE) {}
		MessageHeader header;

		int role_id;
		int pet_num;
		int pet_id_list[ROLE_BATTLE_PET_NUM];	
		long long pet_unique_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_move_type_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_id_list[ROLE_BATTLE_PET_NUM];
		short pet_auto_skill_level_list[ROLE_BATTLE_PET_NUM];
		short reserve_sh;

		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);
	};

	struct GameBattleRoleUseHealItem
	{
		GameBattleRoleUseHealItem() : header(MT_GAME_BATTLE_ROLE_USE_HEAL_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		short item_count;
		int heal_num;
		short purify_percent;
		short is_realive;
		short limit_level;
		short reply_percent;		//回复对象生命上限百分比
		short is_percent;			//是否是百分比回复
		short reserve_sh;
	};

	struct GameBattleRoleUseMpItem
	{
		GameBattleRoleUseMpItem() : header(MT_GAME_BATTLE_ROLE_USE_MP_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		short item_count;
		short limit_level;
		short mp_value;
		short is_percent;		//是否是百分比回复
		short reserve_sh;
	};

	struct GameBattleRoleUseEquipment
	{
		GameBattleRoleUseEquipment() : header(MT_GAME_BATTLE_ROLE_USE_EQUIPMENT) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short item_column;
		short item_index;
		ItemID item_id;
		
		int equip_attr_num;
		AttrInfo equip_attr_list[BATTLE_WEAPON_ATTR_NUM];

		int equip_sp_num;
		int equip_sp_list[BATTLE_WEAPON_EFFECT_NUM];
	};

	struct BattleGameRoleUseItem
	{
		BattleGameRoleUseItem() : header(MT_BATTLE_GAME_ROLE_USE_ITEM) { }
		MessageHeader header;

		int role_id;

		short item_column;
		short item_index;
		ItemID item_id;
		short reserve_sh;
	};

	struct GameBattleRoleSealPet
	{
		GameBattleRoleSealPet() : header(MT_GAME_BATTLE_ROLE_SEAL_PET) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		ItemID seal_card_item_id;
		short succ_rate;
		int monster_id;
		int pet_id;
		short pr_up_on_fail;
		short fixed_times_must_succ;
	};

	struct BattleGameRoleSealPet
	{
		BattleGameRoleSealPet() : header(MT_BATTLE_GAME_ROLE_SEAL_PET) {}
		MessageHeader header;

		int role_id;

		int monster_id;
		int pet_id;
		ItemID seal_card_item_id;
		char is_succ;
		char is_sure_give_pet_on_succ;
		short is_auto_seal_pet;			// 是否自动抓宠
		short is_bind_pet;
		int monster_group_id;
		int battle_mode;				// 战斗玩法
	};

	struct GameBattleRoleUseInstKillMonsterItem
	{
		GameBattleRoleUseInstKillMonsterItem() : header(MT_GAME_BATTLE_ROLE_USE_INST_KILL_MONSTER_ITEM) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		short move_idx;
		short target_grid_idx;
		ItemID instant_kill_item_id;
		short succ_rate;
		int monster_id;
	};

	struct BattleGameRoleInstantKillMonster
	{
		BattleGameRoleInstantKillMonster() : header(MT_BATTLE_GAME_ROLE_INST_KILL_MONSTER) {}
		MessageHeader header;

		int role_id;
		int monster_id;
		ItemID instant_kill_item_id;
		char is_succ;
		char reserve_ch;
	};

	struct GameBattleReloadConfig
	{
		GameBattleReloadConfig() : header(MT_GAME_BATTLE_RELOAD_CONFIG_REQ) {}
		MessageHeader header;

		int config_type;
		int param1;
		int param2;
	};

	struct BattleGameInstanceReleaseNotice
	{
		BattleGameInstanceReleaseNotice() : header(MT_BATTLE_GAME_BATTLE_INSTANCE_RELEASED) {}
		MessageHeader header;

		int battle_id;
	};

	struct BattleGameSyncWorldBossHp
	{
		BattleGameSyncWorldBossHp() : header(MT_BATTLE_GAME_WORLD_BOSS_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct BattleGameSyncWorldBoss2Hp
	{
		BattleGameSyncWorldBoss2Hp() : header(MT_BATTLE_GAME_WORLD_BOSS_2_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct BattleGameSyncWorld3BossHp
	{
		BattleGameSyncWorld3BossHp() : header(MT_BATTLE_GAME_WORLD_BOSS_3_HP_SYNC) {}
		MessageHeader header;

		int cur_hp;
		int round_modify_hp;
	};

	struct GameBattleCheckRoleReallyInBattleReq
	{
		GameBattleCheckRoleReallyInBattleReq() : header(MT_GAME_BATTLE_ROLE_REALLY_IN_BATTLE_REQ){}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct BattleGameCheckRoleReallyInBattleResp
	{
		BattleGameCheckRoleReallyInBattleResp() : header(MT_BATTLE_GAME_ROLE_REALLY_IN_BATTLE_RESP) {}
		MessageHeader header;

		int uid;
		short is_in_battle;
		short reserve_sh;
		int battle_id;
		int battle_server_idx;
	};

	struct GameBattleStartOBReq
	{
		GameBattleStartOBReq() : header(MT_GAME_BATTLE_START_OB_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int battle_id;
		int watch_uid;
		int forbid_quit_ob_flag;	// 当该参数为1时，禁止退出观战
	};

	struct GameBattleStartOBSideReq
	{
		GameBattleStartOBSideReq() : header(MT_GAME_BATTLE_START_OB_SIDE_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int battle_id;
		short watch_side;		// 0攻方  1守方
		short reserve_sh;
	};

	struct BattleGameGuildHonorFightRequestBackup
	{
		BattleGameGuildHonorFightRequestBackup() : header(MT_BATTLE_GAME_GUILD_HONOR_FIGHT_REQUEST_BACKUP) {}
		MessageHeader header;

		int battle_id;				//!< 战斗标识ID
		short attacker_need_num;	//!< 攻方需要多少人
		short defender_need_num;	//!< 守方需要多少人
		BattleData_Business business_data;	//!< 业务数据
	};

	struct GameBattleGuildHonorFightSendBackup : public IMessageSerializer, public IMessageDeserializer
	{
		GameBattleGuildHonorFightSendBackup() : header(MT_GAME_BATTLE_GUILD_HONOR_FIGHT_SEND_BACKUP) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			RoleBattleData role_battle_data;
		};

		int battle_id;
		short attacker_num;
		short defender_num;
		MemberData attacker_data_list[SIDE_MAX_ROLE_NUM];
		MemberData defender_data_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameGuildHonorRoleDieNotice
	{
		BattleGameGuildHonorRoleDieNotice() : header(MT_BATTLE_GAME_GUILD_HONOR_ROLE_DIE_LIST) {}
		MessageHeader header;

		int battle_id;
		int attacker_queue_num;
		int defender_queue_num;
		BattleData_Business business_data;	//!< 业务数据
	};

	struct GameBattleSetStrategyReq
	{
		GameBattleSetStrategyReq() : header(MT_GAME_BATTLE_SET_STRATEGY_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short strategy_type;
		short target_grid_idx;
	};

	struct GameBattleClearStrategyReq
	{
		GameBattleClearStrategyReq() : header(MT_GAME_BATTLE_CLEAR_STRATEGY_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short grid_idx;
		short is_clear_all;
	};

	struct GameBattleLeaveOBReq
	{
		GameBattleLeaveOBReq() : header(MT_GAME_BATTLE_LEAVE_OB_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int battle_id;

		int is_leave_team;
	};

	struct GameBattleForceFinishBattleModeReq
	{
		GameBattleForceFinishBattleModeReq() : header(MT_GAME_BATTLE_FORCE_FINISH_BATTLE_MODE_REQ) {}
		MessageHeader header;

		int battle_mode;
		int param1;			// 将会原样返回
		int param2;			// 将会原样返回
		int param3;			// 将会原样返回
	};

	struct BattleGameForceFinishBattleModeResp
	{
		BattleGameForceFinishBattleModeResp() : header(MT_BATTLE_GAME_FORCE_FINISH_BATTLE_MODE_RESP) {}
		MessageHeader header;

		int battle_server_idx;
		int battle_mode;
		int param1;
		int param2;
		int param3;
	};

	struct BattleGameRoleReinforceBattle
	{
		BattleGameRoleReinforceBattle() : header(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE) {}
		MessageHeader header;

		int uid;
		int battle_id;
	};

	struct BattleGameRoleReinforceBattleEnd
	{
		BattleGameRoleReinforceBattleEnd() : header(MT_BATTLE_GAME_ROLE_REINFORCE_BATTLE_END) {}
		MessageHeader header;

		int uid;
		int battle_id;
	};

	struct GameBattleRolePauseBattle
	{
		GameBattleRolePauseBattle() : header(MT_GAME_BATTLE_PAUSE_BATTLE_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		int is_pause;
	};

	struct GameBattleRoleRealTimeComment
	{
		GameBattleRoleRealTimeComment() : header(MT_GAME_BATTLE_ROLE_CHAT_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		unsigned int msg_length;

		// 以下紧接聊天内容
		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	struct GameBattleRoleLeaveTeamAfterBattle
	{
		GameBattleRoleLeaveTeamAfterBattle() : header(MT_GAME_BATTLE_ROLE_LEAVE_TEAM_AFTER_BATTLE) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGameRoleSyncOBStatus
	{
		BattleGameRoleSyncOBStatus() : header(MT_BATTLE_GAME_ROLE_SYNC_OB_STATUS) {}
		MessageHeader header;

		enum OB_STATUS
		{
			OB_STATUS_STOP_OB = 0,
			OB_STATUS_START_OB = 1,
			OB_STATUS_STOP_FIGHT_START_OB = 2,
			OB_STATUS_START_FIGHT_STOP_OB = 3,
		};

		int uid;
		int battle_id;
		int ob_status;
	};

	struct BattleGameStartFightFailed
	{
		BattleGameStartFightFailed() : header(MT_BATTLE_GAME_START_FIGHT_FAILED) {}
		MessageHeader header;

		int battle_id;
		BattleData_Business business_data;
	};

	struct GameBattleCheckBattleHasFail
	{
		GameBattleCheckBattleHasFail() : header(MT_GAME_BATTLE_CHECK_BATTLE_START_FAILED) {}
		MessageHeader header;

		int battle_id;
	};

	struct BattleGameCheckBattleHasFailAck
	{
		BattleGameCheckBattleHasFailAck() : header(MT_BATTLE_GAME_CHECK_BATTLE_START_FAILED_ACK) {}
		MessageHeader header;

		int battle_id;
		int is_failed;
	};

	struct GameBattleJoySeeksJavorsOperate
	{
		GameBattleJoySeeksJavorsOperate() : header(MT_GAME_BATTLE_START_FIGHT_SPECIAL_OPERATE_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int operate_type;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
	};

	struct BattleGameSpecialFightResultAck : public IMessageSerializer, public IMessageDeserializer
	{
		BattleGameSpecialFightResultAck() : header(MT_GAME_BATTLE_SPECIAL_RESULT_NOTICE) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length);
		virtual bool Deserialize(const char* data, int data_length, int* out_offset);

		struct MemberData
		{
			BGNetID bg_netid;
			int uid;
			GameName			role_name;
			short				m_avatar_type;									// 虚拟人物类型
			short				reserve_sh;
			long long			m_avatar_timestamp;								// 玩家自上传头像
		};

		short battle_mode;		// 战斗模式标识 
		int battle_id;			// 战斗ID标识
		int battle_result;

		int attacker_num;
		int defender_num;
		MemberData attacker_data[SIDE_MAX_ROLE_NUM];
		MemberData defender_data[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleBattleGiveup
	{
		GameBattleBattleGiveup() : header(MT_GAME_BATTLE_BATTLE_GIVEUP) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleBattleSurrender
	{
		GameBattleBattleSurrender() : header(MT_GAME_BATTLE_BATTLE_SURRENDER) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleRoleRejoinBattleScene
	{
		GameBattleRoleRejoinBattleScene() : header(MT_GAME_BATTLE_ROLE_REJOIN_BATTLE_SCENE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
		GameName role_name;

		int battle_id;
	};

	struct GameBattleForceStopOBNotice
	{
		GameBattleForceStopOBNotice() : header(MT_GAME_BATTLE_FORCE_STOP_OB_NOTICE) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
		int battle_id;
	};

	struct GameBattleRoleUndoReq
	{
		GameBattleRoleUndoReq() : header(MT_GAME_BATTLE_ROLE_UNDO_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;
	};

	struct GameBattleModifyAnimationSpeedReq
	{
		GameBattleModifyAnimationSpeedReq() : header(MT_GAME_BATTLE_MODIFY_ANIMATION_SPEED_REQ) {}
		MessageHeader header;

		int uid;
		BGNetID bg_netid;

		int speed_type;
	};

	struct BattleGameModifyAnimationSpeedResp
	{
		BattleGameModifyAnimationSpeedResp() : header(MT_BATTLE_GAME_MODIFY_ANIMATION_SPEED_RESP) {}
		MessageHeader header;

		int uid;
		short speed_type;
		short is_team;
		int battle_mode;
		int teammate_uid_list[4];
	};

	struct BattleGameGuildHonorRoleStateChangeNotice
	{
		BattleGameGuildHonorRoleStateChangeNotice() : header(MT_BATTLE_GAME_GUILD_HONOR_ROLE_STATE_CHANGE_NOTICE) {}
		MessageHeader header;

		int battle_id;
		int zone_id;
		int is_attacker_side;
		short start_role_num;
		short end_role_num;
		int start_role_list[SIDE_MAX_ROLE_NUM];
		int end_role_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameGuildHonorSyncTotalDamage
	{
		BattleGameGuildHonorSyncTotalDamage() : header(MT_BATTLE_GAME_GUILD_HONOR_SYNC_ROUND_TOTAL_DAMAGE) {}
		MessageHeader header;

		int battle_id;
		int zone_id;
		long long atk_total_damage;
		long long def_total_damage;
	};

	struct GameBattleHotfix
	{
		GameBattleHotfix() : header(MT_GAME_BATTLE_HOTFIX) {}
		MessageHeader header;

		char so_file_name[128];
		char symbol_str[512];
	};

	struct BattleGameWorldBossSyncRoleStat
	{
		BattleGameWorldBossSyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_SYNC_ROLE_STAT) {}
		MessageHeader header;
	
		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameWorldBoss2SyncRoleStat
	{
		BattleGameWorldBoss2SyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_2_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct BattleGameWorldBoss3SyncRoleStat
	{
		BattleGameWorldBoss3SyncRoleStat() : header(MT_BATTLE_GAME_WORLD_BOSS_3_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		int role_count;
		DeltaStat delta_stat_list[SIDE_MAX_ROLE_NUM];
	};

	struct GameBattleWorldBossChangeHp
	{
		GameBattleWorldBossChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;		
	};

	struct GameBattleWorldBoss2ChangeHp
	{
		GameBattleWorldBoss2ChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_2_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;
	};

	struct GameBattleWorldBoss3ChangeHp
	{
		GameBattleWorldBoss3ChangeHp() : header(MT_GAME_BATTLE_WORLD_BOSS_3_CHANGE_HP) {}
		MessageHeader header;

		long long cur_hp;
		long long change_value;
	};

	struct GameBattleSetCustomInstruction
	{
		GameBattleSetCustomInstruction() : header(MT_GAME_BATTLE_SET_CUSTOM_INSTRUCTION) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
		short reserve_sh;
		short grid_idx;
		InstructionStr istr;
	};

	struct GameBattleSkipBattleReq
	{
		GameBattleSkipBattleReq() : header(MT_GAME_BATTLE_SKIP_BATTLE_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct GameBattleFastForwardBattleReq
	{
		GameBattleFastForwardBattleReq() : header(MT_GAME_BATTLE_FAST_FORWARD_REQ) {}
		MessageHeader header;

		BGNetID bg_netid;
		int uid;
	};

	struct BattleGamePerformChangeCardSkill
	{
		BattleGamePerformChangeCardSkill() : header(MT_BATTLE_GAME_PERFORM_CHANGE_CARD_SKILL) {}
		MessageHeader header;

		int role_id;
		int change_card_seq;
	};

	struct BattleGameSyncUTARoleStat
	{
		BattleGameSyncUTARoleStat() : header(MT_BATTLE_GAME_UTA_SYNC_ROLE_STAT) {}
		MessageHeader header;

		struct DeltaStat
		{
			short side;
			short reserve_sh;
			int role_id;
			long long delta_damage;
			long long delta_injure;
			long long delta_heal;
		};

		long long schedule_id;
		short round;
		short role_count;
		DeltaStat delta_stat_list[BATTLE_MAX_ROLE_NUM];
	};

	struct BattleGameSyncUTARoundStartData : public IMessageDeserializer, public IMessageSerializer
	{
		BattleGameSyncUTARoundStartData() : header(MT_BATTLE_GAME_UTA_SYNC_ROUND_START_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(schedule_id)
				&& WRITE_INT(length_sc_battle_info)
				&& WRITE_INT(length_sc_buff_list)
				&& WRITE_INT(length_sc_special_effect_list)
				&& WRITE_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = WRITE_STRN(sc_battle_info_data, length_sc_battle_info)
				&& WRITE_STRN(sc_buff_list_data, length_sc_buff_list)
				&& WRITE_STRN(sc_special_effect_list_data, length_sc_special_effect_list)
				&& WRITE_STRN(sc_halo_list_data, length_sc_halo_list);
			if (!ret) return false;

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(schedule_id)
				&& READ_INT(length_sc_battle_info)
				&& READ_INT(length_sc_buff_list)
				&& READ_INT(length_sc_special_effect_list)
				&& READ_INT(length_sc_halo_list);
			if (!ret) return false;

			ret = READ_STRN(sc_battle_info_data, length_sc_battle_info, sizeof(sc_battle_info_data))
				&& READ_STRN(sc_buff_list_data, length_sc_buff_list, sizeof(sc_buff_list_data))
				&& READ_STRN(sc_special_effect_list_data, length_sc_special_effect_list, sizeof(sc_special_effect_list_data))
				&& READ_STRN(sc_halo_list_data, length_sc_halo_list, sizeof(sc_halo_list_data));
			if (!ret) return false;

			return ret;
		}

		long long schedule_id;

		int length_sc_battle_info;
		char sc_battle_info_data[5000];

		int length_sc_buff_list;
		char sc_buff_list_data[2000];

		int length_sc_special_effect_list;
		char sc_special_effect_list_data[2000];

		int length_sc_halo_list;
		char sc_halo_list_data[2000];
	};

	struct BattleGameSyncUTARoundResultData
	{
		BattleGameSyncUTARoundResultData() : header(MT_BATTLE_GAME_UTA_SYNC_ROUND_RESULT_DATA) {}
		MessageHeader header;

		long long schedule_id;
		int data_length;
		char data_buffer[100000];
	};
}


#pragma pack(pop)


#endif