#ifndef __BATTLE_MSG_CODE_H__
#define __BATTLE_MSG_CODE_H__

namespace Protocol
{
	// 此处定义的是玩家与Battle的交互协议

	enum
	{
		BATTLE_SERVER_MSG_BEGIN = 11000,
		BATTLE_SERVER_MSG_END = 12000,
	};

	enum _MsgTypeBattle
	{
		MT_BATTLE_START_SC = 11001,				// 战斗开始通知
		MT_BATTLE_RESULT_SC = 11002,			// 战斗最终结果
		MT_BATTLE_INFO_SC = 11003,				// 对战双方信息
		MT_BATTLE_ROUND_RESULT_SC = 11004,		// 单回合结果
		MT_BATTLE_ROUND_PHASE_SC = 11005,		// 阶段
		MT_BATTLE_ROLE_INFO_SC = 11006,			// 角色数据
		MT_BATTLE_SET_AUTO_RESP_SC = 11007,		// 自动设置返回
		MT_BATTLE_FORCE_FINISH_FIGHT_SC = 11008,// 强制结束战斗返回or通知
		MT_BATTLE_SET_AUTO_MOVE_SC = 11009,		// 自动释放行为设置回包
		MT_BATTLE_DEMON_CAVE_WAVE_SC = 11010,	// 魔窟当前波数
		MT_BATTLE_ROLE_OPERATE_SC = 11011,		// 设置行动的返回结果
		MT_BATTLE_ROLE_OB_SWITCH_SC = 11012,	// OB开关
		MT_BATTLE_SPECIAL_START_SC = 11013,		// 特殊战斗信息返回
		MT_BATTLE_SPECIAL_ONE_MAP_CHANGE_SC = 11014,// 特殊战斗-单面地图改变
		MT_BATTLE_SPECIAL_FINISH_SC = 11015,	// 欢乐寻宠结果
		MT_BATTLE_SPECIAL_JOY_REQ_CS = 11016,	// 欢乐寻宠操作请求
		MT_BATTLE_CHARACTER_BUFF_LIST_SC = 11017,	// 角色buff列表
		MT_BATTLE_REPORT_SC = 11018,			// 战报
		MT_BATTLE_CHARACTER_SPECIAL_EFFECT_LIST_SC = 11019,	// 角色特效列表
		MT_BATTLE_CHARACTER_HALO_LIST_SC = 11020,	// 角色光环列表
		MT_BATTLE_HALLOW_LIST_SC = 11021,			// 圣器列表

		MT_BATTLE_START_CS = 11051,
		MT_BATTLE_DEMO_START_CS = 11052,
		MT_BATTLE_ANIMATION_FINISH_CS = 11053,	// 播动画结束
		MT_BATTLE_ROLE_OPERATE_CS = 11054,		// 角色战斗操作
		MT_BATTLE_FORCE_FINISH_FIGHT_CS = 11055,// 强制结束战斗(调试用)
		MT_BATTLE_COUNTDOWN_FINISH_CS = 11056,	// 倒计时结束
		MT_BATTLE_FIGHT_MONSTER_CS = 11057,		// 请求打某个怪物
		MT_BATTLE_MODE_FIGHT_CS = 11058,		// 按战斗模式开始战斗
		MT_BATTLE_FIGHT_OTHERS_CS = 11059,		// 强制 PK 其它玩家(强P，慎用)

		MT_BATTLE_QIE_CUO_REQ_CS = 11060,		// 请求与其它玩家切磋
		MT_BATTLE_QIE_CUO_REQ_SC = 11061,		// 其它玩家请求与你切磋
		MT_BATTLE_QIE_CUO_REP_CS = 11062,		// 切磋请求回复

		MT_BATTLE_SET_STRATEGY_CS = 11071,		// 战术指挥设置
		MT_BATTLE_CLEAR_STRATEGY_CS = 11072,	// 清除战术指挥
		MT_BATTLE_STRATEGY_INFO_SC = 11073,		// 战术指挥信息
		MT_SYNC_WORLD_BOSS_CUR_HP_SC = 11074,	// 同步世界boss当前血量
		MT_BATTLE_ROLE_FINISH_INPUT_NOTICE_SC = 11075, // 主角完成输入
		MT_BATTLE_ROLE_START_INPUT_SC = 11076,	// 主角开始输入

		MT_BATTLE_LEAVE_OB_CS = 11077,			// 离开观战（包括击飞观战和普通的观战）
		MT_BATTLE_END_FOR_AUDIENCE_SC = 11078,	// 观众战斗结束

		MT_BATTLE_PAUSE_CS = 11079,				// 请求暂停
		MT_BATTLE_PAUSE_SC = 11080,				// 请求暂停结果反馈

		MT_BATTLE_PREPARE_FINISH_CS = 11081,	// 战斗准备完成请求
		MT_BATTLE_GIVEUP_CS = 11082,            // 战斗放弃

		MT_BATTLE_REPORT_SIMPLE_INFO_CS = 11083,// 请求战报基本信息
		MT_BATTLE_REPORT_SIMPLE_INFO_SC = 11084,// 返回战报基本信息

		MT_BATTLE_REPORT_DETAIL_INFO_CS = 11085,// 请求战报详细信息（观看录像）
		MT_BATTLE_REPORT_STOP_WATCHING_CS = 11086, // 战报暂停观看请求
		MT_BATTLE_REPORT_STOP_WATCHING_SC = 11087, // 战报暂停关卡返回
		MT_BATTLE_AUTO_INFO_LIST_SC = 11088,	   // 自动信息列表
		MT_BATTLE_AUTO_INFO_CHANGE_NOTICE_SC = 11089,	// 自动信息改变通知
		MT_BATTLE_START_NPC_DEMO_FIGHT_CS = 11090,	// 开始特殊主角配置战斗
		MT_BATTLE_ROLE_IN_BATTLE_NOTICE_SC = 11091,	// 通知玩家在战斗中（玩家要发请求进入战斗）
		MT_BATTLE_ROLE_REJOIN_BATTLE_SCENE_CS = 11092, // 重新进入战斗场景请求
		MT_BATTLE_STATISTIC_RESULT_SC = 11093,  // 战斗统计数据下发
		MT_BATTLE_CHIEF_ELECT_FIELD_BUFF_SHOW = 11094, // 首席竞选战场BUFF显示
		MT_BATTLE_AI_SKILL_RESULT = 11095,		// 智能施法结果
		MT_BATTLE_SET_CUSTOM_INSTRUCTION_CS = 11096, // 战斗中设置自定义指令
		MT_BATTLE_REPORT_PARSE_FAILED = 11097,	// 战报解析失败上报
		MT_BATTLE_SKIP_CS = 11098,				// 跳过战斗
		MT_BATTLE_FAST_FORWARD_CS = 11099,		// 速战
		MT_BATTLE_FAST_FORWARD_SC = 11100,		// 速战结果下发
		MT_BATTLE_GUILD_BOSS_DAMAGE_SC = 11101,	// 战斗下发家族BOSS总伤害
		MT_BATTLE_SURRENDER_CS = 11102,         // 投降

		MT_BATTLE_WATCH_BATTLE_CS = 11200,		// 请求观战
		MT_BATTLE_CHAT_CS = 11201,				// 请求战斗聊天（弹幕）
		MT_BATTLE_CHAT_SC = 11202,				// 战斗聊天返回(弹幕）

		MT_BATTLE_UNDO_CS = 11203,				// 撤销战斗指令
		MT_BATTLE_ANIMATION_SPEED_MODIFY_CS = 11204, // 观看速度修改请求
		MT_BATTLE_ANIMATION_SPEED_MODIFY_SC = 11205, // 观看速度修改返回
		
		MT_BATTLE_MOVE_BOARDCAST_SC = 11206,	// 战斗指令广播
	};

	static const int BATTLE_MSG_NUM = BATTLE_SERVER_MSG_END - BATTLE_SERVER_MSG_BEGIN;
}

#endif