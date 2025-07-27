#ifndef __MSG_BATTLE_H__
#define __MSG_BATTLE_H__

#include "battlemsgcode.h"
#include "msgheader.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/battle/battle_ai_def.hpp"
#include "servercommon/struct/battle/battle_event_def.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	class SCBattleStart			//!< 11001 战斗开始通知
	{
	public:
		SCBattleStart() : header(MT_BATTLE_START_SC) {}
		MessageHeader header;

		struct PvpBuff
		{
			short buff_id;
			short buff_level;
		};

		short is_boss_fight;	//!< 1:是boss战   0:不是boss战
		short is_mine_monster;	//!< 1:是暗怪     0:不是暗怪
		int monster_group_id;	//!< 怪物组ID
		short can_fast_forward;	//!< 能否速战 (1:可以速战 0:不能速战)
		short reserve_sh_2;		//!< 保留字
		short max_round;		//!< 最大回合数
		short pvp_buff_num;		//!< pvp_buff数量
		PvpBuff pvp_buff_list[10];	//!< pvp_buff列表 按实际数量来读
	};

	class SCBattleResult		//!< 11002 战斗结果通知
	{
	public:
		SCBattleResult() : header(MT_BATTLE_RESULT_SC) {}
		MessageHeader header;

		char is_win;			//!< [0:负  1:胜]
		char result_type;		//!< 结果类型 \see RESULT_TYPE
		char battle_mode;		//!< 战斗模式
		char reserve_ch;		//!< 保留字
	};

	class CSBattleDemoStart		 //!< 11052 开始战斗demo
	{
	public:
		CSBattleDemoStart() : header(MT_BATTLE_DEMO_START_CS) {}
		MessageHeader header;
	};

	class CSBattleFightMonsterReq //!< 11057 请求打某个怪物
	{
	public:
		CSBattleFightMonsterReq() : header(MT_BATTLE_FIGHT_MONSTER_CS) {}
		MessageHeader header;

		int monster_group_id;		//!< 怪物组id
	};

	struct MsgCharacterBuffInfo
	{
		short buff_id;			//!< buffid
		short buff_level;		//!< buff等级
		short sustain_num;		//!< 持续的回合数或行动数
		short grid_idx;			//!< 角色的格点
		unsigned int buff_key;	//!< buff的标识
	};

	struct MsgCharacterSpecialEffect
	{
		short spe_id;			//!< 特效ID
		short grid_idx;			//!< 角色的格点
	};

	struct MsgCharacterHalo
	{
		short grid_id;			//!< 光环所有者所在格点
		short halo_id;			//!< 光环ID
		char halo_cover_grid_list[MAX_CHARACTER_NUM]; //!< 光环覆盖的格点
	};

	struct MsgHallowInfo
	{
		short grid_idx;
		short reserve_sh;
		int hallow_id;
		int owner_uid;
	};

	class SCBattleInfo			//!< 11003 战斗信息下发
	{
	public:
		SCBattleInfo() : header(MT_BATTLE_INFO_SC) {}
		MessageHeader header;

		static const UInt16 VERSION = 3;

		void* operator new(size_t c);
		void operator delete(void* m);

		short battle_mode;					//!< 战斗玩法类型  \see BATTLE_MODE
		char character_num;					//!< 角色数量
		char can_modify_speed;				//!< 是否能修改观看战报速度 （0：否  1:是)
		char is_ob;							//!< 是否观战(0:非观战  1:击飞观战  2:一直都观战）
		char ob_side;						//!< 观战主视觉（0：攻方  1：守方）
		char is_pvp;						//!< 是否PVP（0：否  1:是)
		char is_team_up;					//!< 是否组队（0：否  1:是)
		unsigned int limit_timestamp;		//!< 限制时间（0:不限制  >0:战斗强制结束时间戳）
		int monster_group_id;				//!< 怪物组ID
		BattleCharacterInfo character_info_list[MAX_CHARACTER_NUM];  //!< 角色信息列表  按character_num数量循环读取
	};

	class SCBattleRoundPhase				//!< 11005 战斗阶段信息下发
	{
	public:
		SCBattleRoundPhase() : header(MT_BATTLE_ROUND_PHASE_SC) {}
		MessageHeader header;

		short round;						//!< 回合数
		short round_phase;					//!< 回合阶段 [1:等待玩家输入指令阶段(倒计时3秒或30秒取决于玩家是不是自动的)  2:服务器计算阶段  3:播放动画阶段]
		unsigned int next_round_phase_time; //!< 进入下个阶段的时间戳 (用于倒计时)

		char is_second_move_avaliable[MAX_CHARACTER_NUM]; //!< 下标是grid_idx  \see  MAX_CHARACTER_NUM
	};

	class SCBattleRoundResult				//!< 11004 单回合战斗数据下发
	{
	public:
		SCBattleRoundResult() : header(MT_BATTLE_ROUND_RESULT_SC) {}
		MessageHeader header;

		static const UInt16 VERSION = 0;

		void* operator new(size_t c);
		void operator delete(void* m);

		int round;							//!< 当前回合
		int tree_num;						//!< 事件树数量
		int total_data_len;					//!< 数据总长度
		char result_buffer[MAX_UINT16 + 1];	//!< 数据缓冲区
	};

	class SCBattleRoleInfo : public IMessageSerializer	//!< 11006 玩家在战斗中的数据
	{
	public:
		SCBattleRoleInfo() : header(MT_BATTLE_ROLE_INFO_SC) {}
		virtual ~SCBattleRoleInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteShort(role_skill_num, buffer, buffer_size, out_length);

			for (int i = 0; i < role_skill_num; ++i)
			{
				result = result && WriteShort(role_skill_list[i].skill_id, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].skill_level, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].skill_index, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].cooldown_num, buffer, buffer_size, out_length);
				result = result && WriteShort(role_skill_list[i].remain_cast_times, buffer, buffer_size, out_length);
			}

			result = result && WriteShort(pet_num, buffer, buffer_size, out_length);

			for (int i = 0; i < pet_num; ++i)
			{
				result = result && WriteInt(pet_list[i].pet_id, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].pet_level, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].pet_str_level, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].pet_skin_id, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].maxhp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].maxmp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].curhp, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].curmp, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_water, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_fire, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_wind, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].element_earth, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].status, buffer, buffer_size, out_length);
				result = result && WriteShort(pet_list[i].skill_num, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].dec_reamin_hp_shield, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].fly_flag, buffer, buffer_size, out_length);
				result = result && WriteInt(pet_list[i].lianyao_pet_id, buffer, buffer_size, out_length);

				for (int k = 0; k < pet_list[i].skill_num; ++k)
				{
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_id, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_level, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].skill_index, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].cooldown_num, buffer, buffer_size, out_length);
					result = result && WriteShort(pet_list[i].pet_skill_list[k].remain_cast_times, buffer, buffer_size, out_length);
				}
			}

			result = result && WriteChar(is_auto, buffer, buffer_size, out_length);
			result = result && WriteChar(role_side, buffer, buffer_size, out_length);
			result = result && WriteShort(first_move_type, buffer, buffer_size, out_length);
			result = result && WriteShort(first_skill_id, buffer, buffer_size, out_length);
			result = result && WriteShort(first_skill_level, buffer, buffer_size, out_length);
			result = result && WriteShort(second_move_type, buffer, buffer_size, out_length);
			result = result && WriteShort(second_skill_id, buffer, buffer_size, out_length);
			result = result && WriteShort(second_skill_level, buffer, buffer_size, out_length);
			result = result && WriteShort(animation_speed, buffer, buffer_size, out_length);

			result = result && WriteShort(fabao_cd, buffer, buffer_size, out_length);
			result = result && WriteShort(fabao_remain_times, buffer, buffer_size, out_length);
			result = result && WriteShort(fabao_num, buffer, buffer_size, out_length);

			for (int i = 0; i < fabao_num; ++i)
			{
				result = result && WriteShort(fabao_list[i].fabao_id, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_level, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_skill_type, buffer, buffer_size, out_length);
				result = result && WriteShort(fabao_list[i].fabao_grade, buffer, buffer_size, out_length);
			}

			result = result && WRITE_SHORT(used_mp_item_times);
			result = result && WRITE_SHORT(max_mp_item_times);

			result = result && WRITE_INT(fake_potion_num);
			for (int i = 0; i < fake_potion_num; ++i)
			{
				result = result && WRITE_USHORT(fake_potion_list[i].fake_potion_item_id);
				result = result && WRITE_SHORT(fake_potion_list[i].potion_num);
			}

			result = result && WRITE_SHORT(guild_buff_id_1);
			result = result && WRITE_SHORT(guild_buff_id_2);

			result = result && WRITE_CHAR(heart_skill_is_vaild);
			result = result && WRITE_CHAR(heart_skill_count);
			for (int i = 0; i < heart_skill_count && i < ARRAY_LENGTH(heart_skill_list); ++i)
			{
				result = result && WRITE_SHORT(heart_skill_list[i].skill_id);
				result = result && WRITE_SHORT(heart_skill_list[i].level);
				result = result && WRITE_SHORT(heart_skill_list[i].remain_times);
			}

			result = result && WRITE_INT(dec_reamin_hp_shield);
			result = result && WRITE_USHORT(soaring_skill_id);
			result = result && WRITE_SHORT(soaring_skill_lv);
			result = result && WRITE_INT(change_card_skill_id);
			result = result && WRITE_INT(change_card_seq);
			result = result && WRITE_USHORT(huanshou_skill_info.skill_id);
			result = result && WRITE_SHORT(huanshou_skill_info.skill_level);
			result = result && WRITE_SHORT(huanshou_skill_info.cd_perform);

			return result;
		}

		struct SkillInfo
		{
			short skill_id;			//!< 技能id
			short skill_level;		//!< 技能等级
			short skill_index;		//!< 技能序号
			short cooldown_num;		//!< 技能CD
			short remain_cast_times;//!< 剩余释放次数 -1为不限次数
		};

		struct PetInfo
		{
			int pet_id;				//!< 宠物ID
			short pet_level;		//!< 宠物等级
			short pet_str_level;	//!< 宠物强化等级	
			int pet_skin_id;		//!< 宠物皮肤

			Attribute maxhp;		//!< 最大血量
			Attribute maxmp;		//!< 最大魔量
			Attribute curhp;		//!< 当前血量
			Attribute curmp;		//!< 当前魔量

			short element_water;	//!< 水元素值
			short element_fire;		//!< 火元素值
			short element_wind;		//!< 风元素值
			short element_earth;	//!< 土元素值

			short status;			//!< 状态  [0:出战 1:待机]
			short skill_num;		//!< 技能数
			SkillInfo pet_skill_list[MAX_BATTLE_SKILL_NUM];	//!< 技能列表
			int dec_reamin_hp_shield;						//!< 剩余护盾值
			int fly_flag;			//!< 飞升阶段/次数
			int lianyao_pet_id;		//!< 副宠ID
		};

		struct FakePotionInfo
		{
			ItemID fake_potion_item_id;
			short potion_num;
		};

		short role_skill_num;								//!< 角色技能数量
		SkillInfo role_skill_list[MAX_BATTLE_SKILL_NUM];	//!< 角色技能列表

		short pet_num;										//!< 宠物数量
		PetInfo pet_list[ROLE_BATTLE_PET_NUM];				//!< 宠物列表
		UNSTD_STATIC_CHECK(5 == ROLE_BATTLE_PET_NUM);

		char is_auto;										//!< [0:否 1:是]
		char role_side;										//!< [0:攻方 1:守方]
		short first_move_type;								//!< 第一个行为的类型
		short first_skill_id;								//!< 第一个行为的技能序号 (行为是技能时有效）
		short first_skill_level;							//!< 第一个行为的技能等级 (行为是技能时有效）
		short second_move_type;								//!< 第二个行为的类型
		short second_skill_id;								//!< 第二个行为的技能序号 (行为是技能时有效）
		short second_skill_level;							//!< 第二个行为的技能等级 (行为是技能时有效）
		short animation_speed;								//!< 动画速度 \see ANIMATION_SPEED

		short fabao_cd;										//!< 法宝剩余CD
		short fabao_remain_times;							//!< 法宝剩余释放次数
		short fabao_num;									//!< 法宝数量
		BattleFabao fabao_list[BATTLE_FABAO_NUM_MAX];		//!< 法宝列表

		short used_mp_item_times;							//!< 使用过了多少次蓝药
		short max_mp_item_times;							//!< 最大使用蓝药次数

		int fake_potion_num;
		FakePotionInfo fake_potion_list[BATTLE_NPC_DEMO_POTION_NUM]; //!< 假药列表

		short guild_buff_id_1;								//!< 家族BUFF1
		short guild_buff_id_2;								//!< 家族BUFF2
		short reserve_sh1;
		char heart_skill_is_vaild;											// 心法技能是否生效 0:失效 1:生效
		char heart_skill_count;
		BattleHeartSkill heart_skill_list[BATTLE_HEART_SKILL_NUM];			//!< 心法技能列表
		int dec_reamin_hp_shield;							//!< 剩余护盾值

		UInt16 soaring_skill_id;							//!< 飞升技能ID
		short soaring_skill_lv;								//!< 飞升技能等级

		int change_card_skill_id;							//!< 变身卡技能ID
		int change_card_seq;								//!< 变身卡seq
		BattleHuanShouSkillInfo huanshou_skill_info;		//!< 幻兽技能
	};

	class CSAnimationFinish
	{
	public:
		CSAnimationFinish() : header(MT_BATTLE_ANIMATION_FINISH_CS) {}
		MessageHeader header;
	};

	class CSBattleCountDownFinish				//!< 11056 
	{
	public:
		CSBattleCountDownFinish() : header(MT_BATTLE_COUNTDOWN_FINISH_CS) {}
		MessageHeader header;
	};

	class CSBattleRoleOperate			//!< 11054 战斗操作请求
	{
	public:
		CSBattleRoleOperate() : header(MT_BATTLE_ROLE_OPERATE_CS) {}
		MessageHeader header;

		int operate_type;				//!< 操作请求类型 \see BATTLE_ROLE_OPERATE_TYPE_SET_AUTO
		int param1;						//!< 参数1
		int param2;						//!< 参数2
		int param3;						//!< 参数3
		int param4;						//!< 参数4
	};

	class CSBattleForceFinishFight
	{
	public:
		CSBattleForceFinishFight() : header(MT_BATTLE_FORCE_FINISH_FIGHT_CS) {}
		MessageHeader header;
	};

	class SCBattleSetAutoResp		//!< 11007 设置自动的返回
	{
	public:
		SCBattleSetAutoResp() : header(MT_BATTLE_SET_AUTO_RESP_SC) {}
		MessageHeader header;

		int is_auto;				//!< [0:否 1:是]
	};

	class SCBattleForceFinishFight	//!< 11008 强制结束战斗的返回或通知
	{
	public:
		SCBattleForceFinishFight() : header(MT_BATTLE_FORCE_FINISH_FIGHT_SC) {}
		MessageHeader header;
	};

	class SCBattleSetAutoMoveResp	//!< 11009 设置自动行为的回包
	{
	public:
		SCBattleSetAutoMoveResp() : header(MT_BATTLE_SET_AUTO_MOVE_SC), notify_reason(REASON_TYPE_NORMAL){}
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,
			REASON_TYPE_EMPTY_MP_RESET_MOVE = 1, // 空蓝重置自动行为
			REASON_TYPE_AI_SKILL = 2,			 // 智能施法	
		};

		short first_move_type;		//!< 第一个行为的类型
		short first_skill_id;		//!< 第一个行为的技能ID (行为是技能时有效）
		short first_skill_level;	//!< 第一个行为的技能等级 (行为是技能时有效）
		short second_move_type;		//!< 第二个行为的类型
		short second_skill_id;		//!< 第二个行为的技能ID (行为是技能时有效）
		short second_skill_level;	//!< 第二个行为的技能等级 (行为是技能时有效）
		int notify_reason;			//!< 下发原因 \see REASON_TYPE
	};

	class CSBattleModeFightReq		//!< 11058  按战斗模式开始战斗 
	{
	public:
		CSBattleModeFightReq() : header(MT_BATTLE_MODE_FIGHT_CS) {}
		MessageHeader header;

		short battle_mode;			//!< 战斗模式 \see  BATTLE_MODE
		short reserve_sh;
		int param1;
		int param2;
		int param3;
	};

	class CSChallengeOthersReq		//!< 11059 强制 PK 其它玩家(强P，慎用)
	{
	public:
		CSChallengeOthersReq() : header(MT_BATTLE_FIGHT_OTHERS_CS) {}
		MessageHeader header;

		int battle_mode;		//!< 战斗模式 \see  BATTLE_MODE
		int role_id;			//!< 角色ID
	};

	class CSBattleQieCuoWithOthers		//!< 11060 请求与其它玩家切磋
	{
	public:
		CSBattleQieCuoWithOthers() : header(MT_BATTLE_QIE_CUO_REQ_CS) {}
		MessageHeader header;

		int role_id;		//!< 目标角色ID
	};

	class SCOthersReqQieCuoWithYou		//!< 11061 其它玩家请求与我切磋通知
	{
	public:
		SCOthersReqQieCuoWithYou() : header(MT_BATTLE_QIE_CUO_REQ_SC) {}
		MessageHeader header;

		int role_id;				//!< 发起者id
		GameName sponsor_name;		//!< 发起者姓名
	};

	class CSBattleQieCuoMessageRep		//!< 11062 切磋请求回复
	{
	public:
		CSBattleQieCuoMessageRep() : header(MT_BATTLE_QIE_CUO_REP_CS) {}
		MessageHeader header;

		int reply_role_id;		//!< 回复的玩家id
		int rep_result;			//!< 回复的结果： 为0 - 拒绝；非0 - 同意
	};

	class SCBattleDemonCaveWave		//!< 11010 魔窟当前波数信息
	{
	public:
		SCBattleDemonCaveWave() : header(MT_BATTLE_DEMON_CAVE_WAVE_SC) {}
		MessageHeader header;

		int cur_wave;				//!< 当前波数
	};

	enum ROLE_OPER_REASON
	{
		ROLE_OPER_REASON_SUCC = 0,						//!< 0 成功
		ROLE_OPER_REASON_BATTLE_SERVER_NOT_CONNECT = 1, //!< 1 战斗服未连上
		ROLE_OPER_REASON_NOT_IN_BATTLE = 2,				//!< 2 角色不在战斗中
		ROLE_OPER_REASON_INPUT_PARAM_ERROR = 3,			//!< 3 输入参数错误
		ROLE_OPER_REASON_KNAPSACK_LOAD_ERROR = 4,		//!< 4 读取物品错误
		ROLE_OPER_REASON_ITEM_USE_ERROR = 5,			//!< 5 使用物品失败
		ROLE_OPER_REASON_UNKNOWN_ERROR = 6,				//!< 6 未知错误
		ROLE_OPER_REASON_MOVE_INVALID = 7,				//!< 7 行动不合法
	};

	class SCBattleRoleOperate		//!< 11011 玩家操作返回结果
	{
	public:
		SCBattleRoleOperate() : header(MT_BATTLE_ROLE_OPERATE_SC) {}
		MessageHeader header;

		short is_succ;				//!< 是否操作成功
		short reserve_sh;			//!< 保留字
	};

	class SCBattleRoleOBSwitch		//!< 11012 战斗中 观战模式和战斗模式的切换
	{
	public:
		SCBattleRoleOBSwitch() : header(MT_BATTLE_ROLE_OB_SWITCH_SC) {}
		MessageHeader header;

		short is_ob;				//!< 是否观战模式（0:战斗模式 1:击飞观战模式  2:一直观战模式）
		short reserve_sh;			//!< 保留字
	};

	class CSWatchBattleReq		 //!< 11200 开始观战
	{
	public:
		CSWatchBattleReq() : header(MT_BATTLE_WATCH_BATTLE_CS) {}
		MessageHeader header;

		short watch_side;	//观看 0是左边，1是右边
		short id_type;		//0 uid ，1是battle-id
		int uid;			//<! 观看的角色id
	};

	class CSBattleSetStrategy	//!< 11071 设置战术指挥
	{
	public:
		CSBattleSetStrategy() : header(MT_BATTLE_SET_STRATEGY_CS) {}
		MessageHeader header;

		short strategy_type;	//!< \see  BATTLE_STRATEGY
		short target_grid_idx;	//!< 格点[0, 20)
	};

	class CSBattleClearStrategy	//!< 11072 清除战术指挥
	{
	public:
		CSBattleClearStrategy() : header(MT_BATTLE_CLEAR_STRATEGY_CS) {}
		MessageHeader header;

		short grid_idx;			//!< 格点 [0,20)
		short is_clear_all;		//!< 是否清除全部 [0:否 1:是]
	};

	class SCBattleStrategyInfo : public IMessageSerializer	//!< 11073 战术指挥信息
	{
	public:
		SCBattleStrategyInfo() : header(MT_BATTLE_STRATEGY_INFO_SC) {}
		MessageHeader header;

		enum REASON_TYPE
		{
			REASON_TYPE_NORMAL = 0,		//!< 0 列表
			REASON_TYPE_ADD = 1,		//!< 1 加一个
			REASON_TYPE_REMOVE = 2,		//!< 2 去掉一个
		};

		struct StrategyInfo
		{
			short strategy_type;		//!< \see  BATTLE_STRATEGY
			short target_grid_idx;		//!< 格点[0, 20)
			GameName role_name;
			InstructionStr istr;
		};

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			ret = ret && WRITE_SHORT(count);
			ret = ret && WRITE_SHORT(reason);
			for (int i = 0; i < count; ++i)
			{
				ret = ret && WRITE_SHORT(info_list[i].strategy_type);
				ret = ret && WRITE_SHORT(info_list[i].target_grid_idx);
				ret = ret && WRITE_STRN(info_list[i].role_name, sizeof(GameName));
				if (0 == info_list[i].strategy_type)
				{
					ret = ret && WRITE_STRN(info_list[i].istr, sizeof(InstructionStr));
				}
			}
			
			return ret;
		}

		short count;					//!< 数量
		short reason;					//!< 原因 \see REASON_TYPE
		StrategyInfo info_list[MAX_CHARACTER_NUM];	//!< 战术列表
	};

	class SCBattleSyncWorldBossCurHp
	{
	public:
		SCBattleSyncWorldBossCurHp() : header(MT_SYNC_WORLD_BOSS_CUR_HP_SC) {}
		MessageHeader header;

		int cur_hp;
		int monster_id;
	};

	class SCBattleRoleFinishInputNotice	// 11075 玩家完成输入通知
	{
	public:
		SCBattleRoleFinishInputNotice() : header(MT_BATTLE_ROLE_FINISH_INPUT_NOTICE_SC) {}
		MessageHeader header;

		int role_id;
		short grid_idx;
		short reserve_sh;
	};

	class SCBattleRoleStartInput		// 11076 主角开始输入通知
	{
	public:
		SCBattleRoleStartInput() : header(MT_BATTLE_ROLE_START_INPUT_SC) {}
		MessageHeader header;

		short is_first_input_finish;	// 一动输入操作是否完成
		short is_second_input_finish;	// 二动/宠物一动输入操作是否完成
	};

	class CSBattleLeaveOB	// 11077 离开观战（击飞玩家或普通观战玩家）
	{
	public:
		CSBattleLeaveOB() : header(MT_BATTLE_LEAVE_OB_CS) {}
		MessageHeader header;

		int is_leave_team;				// [0:暂离 1:退队]
	};

	class SCBattleEndForAudience	// 11078 观众战斗结束
	{
	public:
		SCBattleEndForAudience() : header(MT_BATTLE_END_FOR_AUDIENCE_SC) {}
		MessageHeader header;
	};

	class CSBattlePause			//!< 11079 请求暂停/取消暂停战斗
	{
	public:
		CSBattlePause() : header(MT_BATTLE_PAUSE_CS) {}
		MessageHeader header;

		int is_pause;			//!< 0:取消暂停  1:暂停
	};

	class SCBattlePause			//!< 11080 暂停结果反馈
	{
	public:
		SCBattlePause() : header(MT_BATTLE_PAUSE_SC) {}
		MessageHeader header;

		int is_succ;			//!< 0:失败  1:成功
	};

	class CSBattleChat			//!< 11201 发送弹幕
	{
	public:
		CSBattleChat() : header(MT_BATTLE_CHAT_CS) {}
		MessageHeader header;

		unsigned int msg_length;
		// 紧接聊天内容

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class SCBattleChat			//!< 11202 发送弹幕回包
	{
	public:
		SCBattleChat() : header(MT_BATTLE_CHAT_SC) {}
		MessageHeader header;

		GameName sender_name;	//!< 发送者名称
		int sender_uid;			//!< 发送者uid

		unsigned int msg_length;
		// 紧接聊天内容

		char msg_content[REAL_TIME_COMMENT_LEN];
	};

	class CSBattlePrepareFinish	 //!< 11081 战斗准备完毕
	{
	public:
		CSBattlePrepareFinish() : header(MT_BATTLE_PREPARE_FINISH_CS) {}
		MessageHeader header;
	};

	class SCBattleSpecialInfo	 //!< 11013 特殊战斗下发
	{
	public:
		SCBattleSpecialInfo() : header(MT_BATTLE_SPECIAL_START_SC) {}
		MessageHeader header;

		long long ready_tamp;				//!< 设置结束时间戳

		JoySeeksFavorsMap myself_map;		//!< 自己的地图信息
		JoySeeksFavorsMap enemy_map;		//!< 对手的地图信息
	};

	class SCBattleSpecialOneMapChangeInfo	 //!< 11014 单地图改变
	{
	public:
		SCBattleSpecialOneMapChangeInfo() : header(MT_BATTLE_SPECIAL_ONE_MAP_CHANGE_SC) {}
		MessageHeader header;

		long long show_tamp;				//!< 展示时间戳
		long long next_move_tamp;			//!< 本回合结束时间戳

		char has_find;						//!< [0-2] 0:无，1找到了，2被找到了
		char reserve_ch;
		short joy_seq;						//!< [0-4] 0:无，1小蛋，2中蛋，3大蛋 
		JoySeeksFavorsMap map_info;
	};

	class SCBattleSpecialFinishInfo	 //!< 11015 欢乐寻宠战斗结果
	{
	public:
		SCBattleSpecialFinishInfo() : header(MT_BATTLE_SPECIAL_FINISH_SC) {}
		MessageHeader header;

		int winer_id;
	};

	class CSBattleSpecialJoyReqInfo	 //!< 11016 欢乐寻宠操作请求
	{
	public:
		CSBattleSpecialJoyReqInfo() : header(MT_BATTLE_SPECIAL_JOY_REQ_CS) {}
		MessageHeader header;

		int operate_type;				//!< 操作请求类型 \see BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE_OPEN
		int param1;						//!< 参数1
		int param2;						//!< 参数2
		int param3;						//!< 参数3
		int param4;						//!< 参数4
		int param5;						//!< 参数5
	};

	class CSBattleGiveup	//!< 11082 放弃战斗
	{
	public:
		CSBattleGiveup() : header(MT_BATTLE_GIVEUP_CS) {}
		MessageHeader header;
	};

	class CSBattleSurrender	//!< 11102 投降
	{
	public:
		CSBattleSurrender() : header(MT_BATTLE_SURRENDER_CS) {}
		MessageHeader header;
	};

	class CSBattleReportSimpleInfo	//!< 11083 请求战报基本信息
	{
	public:
		CSBattleReportSimpleInfo() : header(MT_BATTLE_REPORT_SIMPLE_INFO_CS) {}
		MessageHeader header;

		int monster_group_id;		//!< 怪物组ID
		int param1;					//!< 原样返回 
		int param2;					//!< 原样返回
		int param3;					//!< 原样返回
		int is_server_first_kill;	//!< 是否首杀
	};

	class SCBattleReportSimpleInfo	//!< 11084 返回战报基本信息
	{
	public:
		SCBattleReportSimpleInfo() : header(MT_BATTLE_REPORT_SIMPLE_INFO_SC) {}
		MessageHeader header;

		int monster_group_id;		//!< 怪物组ID
		int param1;					//!< 原样返回 
		int param2;					//!< 原样返回
		int param3;					//!< 原样返回
		long long report_file_id;	//!< 战报ID   0就是没有
		int is_server_first_kill;	//!< 是否首杀
	};

	class CSBattleReportDetailInfo //!< 11085 观看录像请求
	{
	public:
		CSBattleReportDetailInfo() : header(MT_BATTLE_REPORT_DETAIL_INFO_CS) {}
		MessageHeader header;

		int monster_group_id;
		int is_server_first_kill;
	};

	class CSBattleReportParseFailed	//!< 11097 解析战报录像失败上报
	{
	public:
		CSBattleReportParseFailed() : header(MT_BATTLE_REPORT_PARSE_FAILED){}
		MessageHeader header;

		int monster_group_id;
		int is_server_first_kill;
	};

	class CSBattleReportStopWatching //!< 11086 暂停观看战报（队长可请求or单人时可请求）
	{
	public:
		CSBattleReportStopWatching() : header(MT_BATTLE_REPORT_STOP_WATCHING_CS) {}
		MessageHeader header;
	};

	class SCBattleReportStopWatching //!< 11087 暂停观看战报返回 （队长和队员都会收到）
	{
	public:
		SCBattleReportStopWatching() : header(MT_BATTLE_REPORT_STOP_WATCHING_SC) {}
		MessageHeader header;
	};

	class SCBattleAutoInfoList	// 11088 自动信息列表
	{
	public:
		SCBattleAutoInfoList() : header(MT_BATTLE_AUTO_INFO_LIST_SC) {}
		MessageHeader header;

		struct AutoInfo
		{
			int role_id;		// 角色ID
			short is_auto;		// 0:手动 1:自动
			short reserve_sh;	// 保留字
		};

		int num;
		AutoInfo info_list[SIDE_MAX_ROLE_NUM];
	};

	class SCBattleAutoInfoChangeNotice	// 11089 自动信息改变通知
	{
	public:
		SCBattleAutoInfoChangeNotice() : header(MT_BATTLE_AUTO_INFO_CHANGE_NOTICE_SC) {}
		MessageHeader header;

		int role_id;	// 角色ID
		int is_auto;	// 0:手动 1:自动
	};

	class CSBattleStartNpcDemoFight	// 11090 开始特殊主角配置战斗
	{
	public:
		CSBattleStartNpcDemoFight() : header(MT_BATTLE_START_NPC_DEMO_FIGHT_CS) {}
		MessageHeader header;

		int npc_demo_seq;	
		int is_initial_fight;		//!< 是否初始战斗
	};

	class SCBattleRoleInBattleNotice // 11091 收到该通知后请求11092
	{
	public:
		SCBattleRoleInBattleNotice() : header(MT_BATTLE_ROLE_IN_BATTLE_NOTICE_SC) {}
		MessageHeader header;

		int battle_server_idx;		//!< 战斗服序号，用于11092请求
		int battle_id;				//!< 战斗实例ID，用于11092请求
	};

	class CSRoleRejoinBattleScene	// 11092 客户端请求重进战斗
	{
	public:
		CSRoleRejoinBattleScene() : header(MT_BATTLE_ROLE_REJOIN_BATTLE_SCENE_CS) {}
		MessageHeader header;

		int battle_server_idx;		//!< 战斗服序号
		int battle_id;				//!< 战斗实例ID
	};

	class SCBattleStatisticResult : public IMessageSerializer // 11093 玩家战斗统计数据
	{
	public:
		SCBattleStatisticResult() : header(MT_BATTLE_STATISTIC_RESULT_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			ret = ret && WRITE_INT(role_num);
			for (int i = 0; i < role_num; ++i)
			{
				ret = ret && WRITE_SHORT(role_stat_list[i].avatar_type);
				ret = ret && WRITE_SHORT(role_stat_list[i].head_id);
				ret = ret && WRITE_STRN(role_stat_list[i].role_name, sizeof(GameName));
				ret = ret && WRITE_SHORT(role_stat_list[i].reserve_sh);
				ret = ret && WRITE_SHORT(role_stat_list[i].special_appearance_type);
				ret = ret && WRITE_INT(role_stat_list[i].special_appearance_param);
				ret = ret && WRITE_LL(role_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(role_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(role_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(role_stat_list[i].fly_flag);
			}
			if (!ret) return false;

			ret = ret && WRITE_INT(partner_num);
			for (int i = 0; i < partner_num; ++i)
			{
				ret = ret && WRITE_INT(partner_stat_list[i].partner_id);
				ret = ret && WRITE_INT(partner_stat_list[i].skin_id);
				ret = ret && WRITE_LL(partner_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(partner_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(partner_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(partner_stat_list[i].fly_flag);
			}
			if (!ret) return false;

			ret = ret && WRITE_INT(pet_num);
			for (int i = 0; i < pet_num; ++i)
			{
				ret = ret && WRITE_INT(pet_stat_list[i].pet_id);
				ret = ret && WRITE_INT(pet_stat_list[i].skin_id);
				ret = ret && WRITE_SHORT(pet_stat_list[i].level);
				ret = ret && WRITE_CHAR(pet_stat_list[i].strength_level);
				ret = ret && WRITE_CHAR(pet_stat_list[i].role_stat_list_idx);
				ret = ret && WRITE_STRN(pet_stat_list[i].pet_name, sizeof(GameName));
				ret = ret && WRITE_LL(pet_stat_list[i].total_damage_to_other);
				ret = ret && WRITE_LL(pet_stat_list[i].total_suffer_injure);
				ret = ret && WRITE_LL(pet_stat_list[i].total_heal_num);
				ret = ret && WRITE_INT(pet_stat_list[i].fly_flag);
			}

			return true;
		}

		struct RoleStatistic
		{
			short avatar_type;		//!< 模型
			short head_id;			//!< 头像id
			GameName role_name;		//!< 玩家名字

			short reserve_sh;		//!< 保留字
			short special_appearance_type;	//!< 特殊形象类型
			int special_appearance_param;	//!< 特殊形象参数

			long long total_damage_to_other; //!< 总伤害
			long long total_suffer_injure;	 //!< 总承伤
			long long total_heal_num;		 //!< 总治疗
			int fly_flag;					 //!< 飞升标记
		};

		struct PartnerStatistic
		{
			int partner_id;
			int skin_id;					 //!< 皮肤ID
			long long total_damage_to_other; //!< 总伤害
			long long total_suffer_injure;	 //!< 总承伤
			long long total_heal_num;		 //!< 总治疗
			int fly_flag;					 //!< 飞升标记
		};

		struct PetStatistic
		{
			int pet_id;							//!< 宠物ID
			int skin_id;						//!< 皮肤ID
			short level;						//!< 宠物等级
			char strength_level;				//!< 强化等级
			char role_stat_list_idx;			//!< 宠物对应玩家在role_stat_list的下标
			GameName pet_name;					//!< 宠物名
			long long total_damage_to_other;	//!< 总伤害
			long long total_suffer_injure;		//!< 总承伤
			long long total_heal_num;			//!< 总治疗
			int fly_flag;						//!< 飞升标记
		};

		int role_num;
		RoleStatistic role_stat_list[SIDE_MAX_ROLE_NUM];

		int partner_num;
		PartnerStatistic partner_stat_list[SIDE_MAX_ROLE_NUM];

		int pet_num;
		PetStatistic pet_stat_list[SIDE_MAX_ROLE_NUM * ROLE_BATTLE_PET_NUM]; //!< 根据pet_num数量读pet_stat_list即可
	};

	class CSRoleUndo		//!< 11203 撤销一动操作
	{
	public:
		CSRoleUndo() : header(MT_BATTLE_UNDO_CS) {}
		MessageHeader header;
	};

	class SCBattleChiefElectFieldBuffShow	// 11094 首席竞选战场BUFF显示
	{
	public:
		SCBattleChiefElectFieldBuffShow() : header(MT_BATTLE_CHIEF_ELECT_FIELD_BUFF_SHOW) {}
		MessageHeader header;

		short buff_id;
		short buff_value;
	};

	class CSBattleAnimationSpeedModify	// 11204 战斗中修改倍速请求
	{
	public:
		CSBattleAnimationSpeedModify() : header(MT_BATTLE_ANIMATION_SPEED_MODIFY_CS) {}
		MessageHeader header;

		int speed_type;	// 要改的速度  [0:一倍速 1:二倍速 2:半倍速]
	};

	class SCBattleAnimationSpeedModify	// 11205 战斗中修改倍速返回
	{
	public:
		SCBattleAnimationSpeedModify() : header(MT_BATTLE_ANIMATION_SPEED_MODIFY_SC) {}
		MessageHeader header;

		int speed_type;	// 改后的速度 [0:一倍速 1:二倍速 2:半倍速]
	};

	class SCBattleAISkillResult		// 11095 智能施法结果
	{
	public:
		SCBattleAISkillResult() : header(MT_BATTLE_AI_SKILL_RESULT) {}
		MessageHeader header;

		short move_type;			// 行为类型 （目前有1:释放技能 和 2:普攻）
		UInt16 skill_id;			// 技能ID
		short skill_level;			// 技能等级
		short reserve_sh;			// 保留字
	};

	class CSBattleSetCustomInstruction	//!< 11096 设置自定义指令
	{
	public:
		CSBattleSetCustomInstruction() : header(MT_BATTLE_SET_CUSTOM_INSTRUCTION_CS) {}
		MessageHeader header;

		short index;					//!< 自定义指令序号[0,8) 
		short grid_idx;					//!< 格子[0,20)
	};

	class CSBattleSkipReq	//!< 11098 跳过战斗
	{
	public:
		CSBattleSkipReq() : header(MT_BATTLE_SKIP_CS) {}
		MessageHeader header;
	};

	class CSBattleFastForwardReq	//!< 11099 速战
	{
	public:
		CSBattleFastForwardReq() : header(MT_BATTLE_FAST_FORWARD_CS) {}
		MessageHeader header;
	};

	class SCBattleFastForwardResp  //!< 11100 速战结果下发
	{
	public:
		SCBattleFastForwardResp() : header(MT_BATTLE_FAST_FORWARD_SC) {}
		MessageHeader header;

		int result;	// 0:失败  1:成功
	};

	class SCBattleGuildBossTotalDamage	// 11101 速战后下发对家族BOSS总伤害
	{
	public:
		SCBattleGuildBossTotalDamage() : header(MT_BATTLE_GUILD_BOSS_DAMAGE_SC) {}
		MessageHeader header;

		long long total_damage;
	};

	class SCBattleMoveBoardcast	// 11206 战斗指令广播
	{
	public:
		SCBattleMoveBoardcast() : header(MT_BATTLE_MOVE_BOARDCAST_SC) {}
		MessageHeader header;

		short grid_idx;			// 哪个格点上的角色的行为
		short target_grid_idx;	// 目标的格点
		short move_type;		// 行为类型
		short param1;			// 当行为是放技能时，为技能ID；当行为是法宝时，为法宝序号[0,fabao_num)
		short param2;			// 当行为是放技能时，为技能等级  
		short param3;
	};
}

#pragma pack(pop)


namespace Protocol
{
	class SCBattleCharacterBuffList : public IMessageSerializer	//!< 11017 角色buff列表
	{
	public:
		SCBattleCharacterBuffList() : header(MT_BATTLE_CHARACTER_BUFF_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)character_buff_list.size());
			for (size_t i = 0; i < character_buff_list.size(); ++i)
			{
				MsgCharacterBuffInfo& info = character_buff_list[i];
				result = result && WRITE_SHORT(info.buff_id);
				result = result && WRITE_SHORT(info.buff_level);
				result = result && WRITE_SHORT(info.sustain_num);
				result = result && WRITE_SHORT(info.grid_idx);
				result = result && WRITE_INT(info.buff_key);
			}

			return result;
		}

		std::vector<MsgCharacterBuffInfo> character_buff_list;

		//相当于一个数量和一个数组，如下：
		//int count;
		//CharacterBuffInfo buff_list[];
	};

	class SCBattleCharacterSpecialEffectList : public IMessageSerializer	//!< 11019 角色特效列表
	{
	public:
		SCBattleCharacterSpecialEffectList() : header(MT_BATTLE_CHARACTER_SPECIAL_EFFECT_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)spe_list.size());
			for (size_t i = 0; i < spe_list.size(); ++i)
			{
				MsgCharacterSpecialEffect& info = spe_list[i];
				result = result && WRITE_SHORT(info.spe_id);
				result = result && WRITE_SHORT(info.grid_idx);
			}

			return result;
		}

		std::vector<MsgCharacterSpecialEffect> spe_list;
	};

	class SCBattleCharacterHaloList : public IMessageSerializer	//!< 11020 角色光环列表
	{
	public:
		SCBattleCharacterHaloList() : header(MT_BATTLE_CHARACTER_HALO_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)halo_list.size());
			for (size_t i = 0; i < halo_list.size(); ++i)
			{
				MsgCharacterHalo& info = halo_list[i];
				result = result && WRITE_SHORT(info.grid_id);
				result = result && WRITE_SHORT(info.halo_id);
				for (int k = 0; k < ARRAY_ITEM_COUNT(info.halo_cover_grid_list); ++k)
				{
					result = result && WRITE_CHAR(info.halo_cover_grid_list[k]);
				}
			}

			return result;
		}

		std::vector<MsgCharacterHalo> halo_list;
	};

	class SCBattleHallowList : public IMessageSerializer, public IMessageDeserializer	//!< 11021 圣器列表
	{
	public:
		SCBattleHallowList() : header(MT_BATTLE_HALLOW_LIST_SC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_INT((int)hallow_list.size()); // 数量
			for (size_t i = 0; i < hallow_list.size(); ++i)
			{
				MsgHallowInfo& info = hallow_list[i];
				result = result && WRITE_SHORT(info.grid_idx); // 圣器格点[20,30)
				result = result && WRITE_INT(info.hallow_id);  // 圣器ID
				result = result && WRITE_INT(info.owner_uid);  // 拥有者uid
			}

			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool result = true;
			int count = 0;
			result = result && READ_BEGIN(header.msg_type);
			result = result && READ_INT(count); // 数量
			if (!result) return false;

			for (int i = 0; i < count; ++i)
			{
				MsgHallowInfo info;
				result = result && READ_SHORT(info.grid_idx); // 圣器格点[20,30)
				result = result && READ_INT(info.hallow_id);  // 圣器ID
				result = result && READ_INT(info.owner_uid);  // 拥有者uid
				hallow_list.push_back(info);
				if (!result) return false;
			}

			return result;
		}

		std::vector<MsgHallowInfo> hallow_list;
	};

	class SCBattleReport : public IMessageSerializer		//!< 11018 战报
	{
	public:
		SCBattleReport() : header(MT_BATTLE_REPORT_SC), battle_info(NULL) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			// 如果改了SCBattleInfo或SCBattleRoundResult，要修改version
			UNSTD_STATIC_CHECK(sizeof(SCBattleInfo) == 3940);
			UNSTD_STATIC_CHECK(sizeof(SCBattleRoundResult) == 65552);
			UNSTD_STATIC_CHECK(SCBattleInfo::VERSION == 3);
			UNSTD_STATIC_CHECK(SCBattleRoundResult::VERSION == 0);

			bool ret = true;
			ret = ret && WRITE_BEGIN(header.msg_type);

			// EventVersion
			ret = ret && WRITE_USHORT(sc_battleinfo_version);
			ret = ret && WRITE_USHORT(sc_battleroundresult_version);
			ret = ret && WRITE_UINT(event_version_list.size());
			for (size_t i = 0; i < event_version_list.size(); ++i)
			{
				ret = ret && WRITE_SHORT(event_version_list[i].event_type);
				ret = ret && WRITE_USHORT(event_version_list[i].event_version);
			}
			if (!ret)
			{
				return false;
			}

			// battle info
			ret = ret && WRITE_BEGIN(battle_info->header.msg_type);
			ret = ret && WRITE_SHORT(battle_info->battle_mode);
			ret = ret && WRITE_CHAR(battle_info->character_num);
			ret = ret && WRITE_CHAR(battle_info->can_modify_speed);
			ret = ret && WRITE_CHAR(battle_info->is_ob);
			ret = ret && WRITE_CHAR(battle_info->ob_side);
			ret = ret && WRITE_CHAR(battle_info->is_pvp);
			ret = ret && WRITE_CHAR(battle_info->is_team_up);
			ret = ret && WRITE_UINT(battle_info->limit_timestamp);
			ret = ret && WRITE_INT(battle_info->monster_group_id);
			for (int i = 0; i < battle_info->character_num; ++i)
			{
				ret = ret && WRITE_STRN((const char*)&battle_info->character_info_list[i], sizeof(battle_info->character_info_list[i]));
			}
			if (!ret)
			{
				return false;
			}

			if (!hallow_list.Serialize(buffer, buffer_size, out_length))
			{
				return false;
			}

			ret = ret && WRITE_INT(static_cast<int>(round_result_list.size()));
			for (size_t i = 0; i < round_result_list.size(); ++i)
			{
				RoundResult& rr = round_result_list[i];

				ret = ret && WRITE_SHORT(rr.round);
				ret = ret && WRITE_CHAR(rr.has_prepare_result);
				ret = ret && WRITE_CHAR(rr.has_formal_result);
				if (1 == rr.has_prepare_result)
				{
					ret = ret && WRITE_BEGIN(rr.prepare_result->header.msg_type);
					ret = ret && WRITE_INT(rr.prepare_result->round);
					ret = ret && WRITE_INT(rr.prepare_result->tree_num);
					ret = ret && WRITE_INT(rr.prepare_result->total_data_len);
					ret = ret && WRITE_STRN(rr.prepare_result->result_buffer, rr.prepare_result->total_data_len);
					if (!ret)
					{
						return false;
					}
				}
				if (1 == rr.has_formal_result)
				{
					ret = ret && WRITE_BEGIN(rr.formal_result->header.msg_type);
					ret = ret && WRITE_INT(rr.formal_result->round);
					ret = ret && WRITE_INT(rr.formal_result->tree_num);
					ret = ret && WRITE_INT(rr.formal_result->total_data_len);
					ret = ret && WRITE_STRN(rr.formal_result->result_buffer, rr.formal_result->total_data_len);
					if (!ret)
					{
						return false;
					}
				}
			}

			return true;
		}

		bool CheckDeserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = true;
			ret = ret && READ_BEGIN(header.msg_type);

			// EventVersion
			ret = ret && READ_USHORT(sc_battleinfo_version);
			ret = ret && READ_USHORT(sc_battleroundresult_version);

			unsigned int event_version_num = 0;
			ret = ret && READ_UINT(event_version_num);
			event_version_num = GetMin(static_cast<unsigned int>(BATTLE_EVENT_MAX), event_version_num);
			event_version_list.reserve(event_version_num);
			for (unsigned int i = 0; i < event_version_num; ++i)
			{
				static EventVersion version;
				ret = ret && READ_SHORT(version.event_type);
				ret = ret && READ_USHORT(version.event_version);
				event_version_list.push_back(version);
			}
			if (!ret)
			{
				return false;
			}

			// battle info
			static Protocol::SCBattleInfo battle_info;
			ret = ret && READ_BEGIN(battle_info.header.msg_type);
			ret = ret && READ_SHORT(battle_info.battle_mode);
			ret = ret && READ_CHAR(battle_info.character_num);
			ret = ret && READ_CHAR(battle_info.can_modify_speed);
			ret = ret && READ_CHAR(battle_info.is_ob);
			ret = ret && READ_CHAR(battle_info.ob_side);
			ret = ret && READ_CHAR(battle_info.is_pvp);
			ret = ret && READ_CHAR(battle_info.is_team_up);
			ret = ret && READ_UINT(battle_info.limit_timestamp);
			ret = ret && READ_INT(battle_info.monster_group_id);
			
			if (battle_info.character_num <= 0 || battle_info.character_num > ARRAY_ITEM_COUNT(battle_info.character_info_list))
			{
				return false;
			}

			for (int i = 0; i < battle_info.character_num && i < ARRAY_ITEM_COUNT(battle_info.character_info_list); ++i)
			{
				ret = ret && READ_STRN((char*)&battle_info.character_info_list[i], sizeof(battle_info.character_info_list[i]), sizeof(battle_info.character_info_list[i]));
			}
			if (!ret)
			{
				return false;
			}

			if (!hallow_list.Deserialize(data, data_length, out_offset))
			{
				return false;
			}

			int round_result_num = 0;
			ret = ret && READ_INT(round_result_num);
			if (round_result_num > 10000)
			{
				return false;
			}
			for (int i = 0; i < round_result_num; ++i)
			{
				short round = 0;
				char has_prepare_result = 0;
				char has_formal_result = 0;
				ret = ret && READ_SHORT(round);
				ret = ret && READ_CHAR(has_prepare_result);
				ret = ret && READ_CHAR(has_formal_result);
				if (1 == has_prepare_result)
				{
					static Protocol::SCBattleRoundResult prepare_result;
					ret = ret && READ_BEGIN(prepare_result.header.msg_type);
					ret = ret && READ_INT(prepare_result.round);
					ret = ret && READ_INT(prepare_result.tree_num);
					ret = ret && READ_INT(prepare_result.total_data_len);
					ret = ret && READ_STRN(prepare_result.result_buffer, prepare_result.total_data_len, sizeof(prepare_result.result_buffer));
					if (!ret)
					{
						return false;
					}
				}

				if (1 == has_formal_result)
				{
					static Protocol::SCBattleRoundResult formal_result;
					ret = ret && READ_BEGIN(formal_result.header.msg_type);
					ret = ret && READ_INT(formal_result.round);
					ret = ret && READ_INT(formal_result.tree_num);
					ret = ret && READ_INT(formal_result.total_data_len);
					ret = ret && READ_STRN(formal_result.result_buffer, formal_result.total_data_len, sizeof(formal_result.result_buffer));
					if (!ret)
					{
						return false;
					}
				}
			}

			return true;
		}

		struct RoundResult
		{
			RoundResult() : round(0), has_prepare_result(0),
				has_formal_result(0), prepare_result(NULL),
				formal_result(NULL) {}

			short round;
			char has_prepare_result;
			char has_formal_result;
			SCBattleRoundResult* prepare_result;
			SCBattleRoundResult* formal_result;
		};

		struct EventVersion
		{
			short event_type;
			UInt16 event_version;
		};

		UInt16 sc_battleinfo_version;
		UInt16 sc_battleroundresult_version;		
		std::vector<EventVersion> event_version_list;

		SCBattleInfo* battle_info;
		SCBattleHallowList hallow_list;
		std::vector<RoundResult> round_result_list;

		// 这个vector相当于：
		// int round_num;
		// RoundResult round_list[];
	};
}

#endif