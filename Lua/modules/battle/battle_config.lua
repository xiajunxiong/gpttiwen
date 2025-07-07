ANIMATION_SPEED = {
    NORMAL = 0,
    DOUBLE = 1,
    HALF = 2
}
BATTLE_ROUND_PHASE = {
	None = 0,
	Command = 1,		--玩家输入指令
	Server = 2,			--服务器计算
	Client = 3,			--播放动画

	CommandDone = 100,	--命令输完后可能还需要等一段时间，比如组队时
}

BATTLE_ROUND_OPERATE_TYPE = {
	SET_AUTO = 1,					--!< 1 设置自动    \param param1 是否自动[0:否 1:是]

	SET_AUTO_NORMAL_ATTACK = 2,	 	--!< 2 设置自动普攻 	\param param1 行为格[0,1]
	SET_AUTO_DEFENSE = 3,			--!< 3 设置自动防御		\param param1 行为格[0,1]
	SET_AUTO_PERFORM_SKILL = 4,	 	--!< 4 设置自动放技能   \param param1 行为格[0,1] \param param2 技能ID  \param param3 技能等级
	SET_AUTO_RUNAWAY = 5,			--!< 5 设置自动逃跑		\param param1 行为格[0,1]
	SET_AUTO_CHANGE_POSITION = 6,	--!< 6 设置自动换位		\param param1 行为格[0,1]
	SET_AUTO_AI_SKILL = 7,			--!< 7 智能施法

	SET_MOVE_NORMAL_ATTACK = 10,	--!< 10 普攻  \param param1 行为格[0,1]  \param param2 目标格点[0,20)
	SET_MOVE_DEFENSE = 11,			--!< 11 防御  \param param1 行为格[0,1]
	SET_MOVE_PERFORM_SKILL = 12,	--!< 12 放技能 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 技能id  \param param4 技能等级

	SET_MOVE_CHANGE_POSITION = 13,
	SET_MOVE_RUNAWAY = 14,

	SET_MOVE_CHANGE_PET = 15,	--!< 15 换宠 param1 行为格[0,1] param2 换上的宠物序号[0, 3)∪[-1, 0) 其中-1为只下宠不上宠 

	SET_MOVE_USE_ITEM = 16,			--!< 16 使用物品 param1 行为格[0,1] param2 目标格点[0,20) param3 物品栏号column param4 物品序号index 

	SET_MOVE_SEAL_PET = 17,			--!< 17 封印宠物 param1 行为格[0,1] param2 目标格点[0,20) param3 封印卡类型[0:普卡 1:银卡 2:金卡] param4 封印的怪物ID（用于提取宠物ID以及成功率上传到战斗服)
	
	SET_MOVE_USE_FABAO = 18,			--!< 18 使用法宝 param1 行为格[0,1] param2 目标格点[0,20) param3 法宝序号[0,fabao_num)
	
	SET_MOVE_USE_INST_KILL_ITEM = 19,	--!< 19 使用降妖鞭 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 瞬杀的怪物ID（用于成功率上传到战斗服)

	SET_MOVE_USE_FAKE_POTION = 20,	 --!< 20 使用假药 \param param1 行为格[0,1] \param param2 目标格点[0,20) \param param3 假药item_id 

	SET_MOVE_PERFORM_HEART_SKILL = 21,	--!< 21 使用心法技能

	SET_MOVE_CARD_SKILL = 22,			--!< 22 释放变身卡技能 \param param1 行为格[0,1] \param param2 目标格点[0,20)  \param param3 技能ID  \param param4  技能等级

	SET_MOVE_HUAN_SHOU = 23, 			--!< 23 释放 幻兽技能 \param param1 行为格[0,1] \param param2 目标格点[0,20)  \param param3 技能等级 

	SET_MOVE_SEAL_YOUMING = 24,			--!< 24 封印幽冥  \param param1 行为格[0,1]  \param param2 目标格点[0,20)  \param param3 \see BATTLE_SEAL_NETHER_WORLD_CARD_TYPE_MAX  \param param4 封印的怪物ID（用于提取幽冥ID以及成功率上传到战斗服)
}

BATTLE_MOVE_TYPE = {
	NONE = 0,
	PERFORM_SKILL = 1,		--释放技能
	NORMAL_ATTACK = 2,		--攻击 
	NORMAL_DEFENCE = 3,		--防御
	USE_ITEM = 4,			--使用道具  使用物品 grid_idx 目标位置 param1 物品类型[0:回血药 1:武器] (找到缓存下来的物品信息并使用之) 
	RUN_AWAY = 5,			--逃跑
	CHANGE_POSITION = 6,	--换位
	CHANGE_PET = 7,			--换宠物
	SEAL_PET_SER = 8,			--封印宠物
	USE_FABAO_SER = 9,			--使用法宝(服务端)
	INSTANT_NORMAL_ATTACK = 10, --瞬发普攻
	AI_SKILL = 11,			--智能技能

	HEART_SKILL = 12,		--用心法技能
	CARD_SKILL = 13,		--用变身卡技能
	HUANSHOU_SKILL = 14,	--用幻兽技能

	USE_FAKE_POTION = 20,	--使用假药 p1 行为格[0,1] p2目标[0,20) p3 假药item_id
	SUMMON_MONSTER = 21,	--召唤怪物
	SELF_DESTRUCT = 22,		--自爆

	-- 客户端自定的，不用来自动的
	SEAL_PET = 101,			--封印怪物
	USE_FABAO = 102,		--使用法宝
	STRATEGY = 103,			--选择战术
	SEAL_YOUMING = 104,		--抓捕幽冥
}


BATTLE_DATA_EVENT =
{
	INVALID = 0,		 
	PERFORM_SKILL = 1,				-- 释放技能 
	DODGE = 2,						-- 闪避
	MODIFY_HP = 3,					-- HP变化
	COMBINE_ATTACK = 4,				-- 合击
	RUN_AWAY = 5,					-- 逃跑
	REPLACE_BUFF = 6,				-- BUFF替换
	MERGE_BUFF = 7,					-- BUFF合并
	ADD_BUFF_TO_LIST = 8,			-- BUFF新增到角色上
	REMOVE_BUFF = 9,				-- BUFF去除
	SELF_DESTRUCT = 10,				-- 自爆
	CHARACTER_DIE = 11,				-- 角色死亡
	COUNTER_ATTACK = 12,			-- 反击
	BE_GUARD = 13,					-- 被攻击时，被其他角色护卫
	CHANGE_POSITION = 14,			-- 换位
	USE_ITEM = 15,					-- 使用物品
	CHANGE_EQUIPMENT = 16,  		-- 替换装备
	MODIFY_CD = 17,					-- 技能CD时间改
	ROUND_END = 18,					-- 回合结束
	SUMMON_BATTLE_CHARACTER = 19, 	-- 召唤战斗单位
	SUMMON = 20,					-- 用于播放召唤动作
	RETRIEVE_PET = 21,				-- 用于播放召回动作
	COMBINE_ATTACK_PERFORM_ONE_HIT=22,--合击,播放一次攻击
	MODIFY_MP = 23,					--MP变化
	COMBINE_ATTACK_TOTAL = 24,		--合击总伤害
	PERFORM_COMBO_SKILL = 25,		--25 释放连击技能 
	COMBO_SKILL_ONE_HIT = 26,		--26 连击技能的一次攻击 
	SEAL_PET = 27,					--27 宠物封印 
	USE_FABAO = 28,					--28 使用法宝 
	ELEMENT_CHANGE = 29,			--元素改变
	VANISH = 30,					--原地消失
	SKILL_PRE_EFFECT = 31,          --技能前置效果事件
	SKILL_POST_EFFECT = 32,         --技能后置效果事件
	SKILL_EFFECT = 33,          	--技能效果事件
	
	ABSORB_DAMAGE = 34,          	--（反伤盾）吸收伤害事件

	SKILL_SPECIAL_EFFECT = 35,      --特殊技能效果（如被动技能、装备特效）
	SKILL_HIT = 36,          		--技能命中（包含普攻）
	IN_VAIN = 37,          			--破空无效
	TALKING = 38,					--对话
	SHOT_EFFECT = 39,				--镜头特效（切换镜头）
	CLIENT_GUIDE = 40,				--客户端指引
	MONSTER_TRANSFORM = 41,			--怪物变身
	BUFF_MISS = 42,					--buff未生效0或者免疫1
	USE_MPITEM = 43,				--使用法药次数变化
	BUFF_SUSTAIN_NUM_CHANG = 44,	--BUFF持续时间改变事件
	SKILL_PERFORM_FAIL = 45,		--技能使用失败
	MODIFY_RAGE_POINTS = 46,		--怒气值改变
	NOTICE_NUM = 47,				--errorcode提示
	CLIENT_CUSTOM = 48,				--客户端自定事件
	SIDE_SHOW_HP = 49,				--全队显示HP
	FABAO_INFO_CHANGE = 50,		    --法宝信息变化
	ADD_HALO = 51,					--光环增加
	REMOVE_HALO = 52,				--删除光环
	HALO_CHANGE_POSITION = 53,			--光环位置变化
	IMPRISON_PERFORM_FAILED = 54,		--角色因特殊状态导致技能技能、行动失败
	SKILL_REMAIN_CAST_TIMES = 55, --技能剩余次数改变
	HEART_SKILL_INFO_CHANGE = 56,	--心法技能信息改变
	LINK_TOGETHER = 57,    --事件57 链接在一起
	LINK_BREAK = 58,      --事件58 断开链接
	SKILL_INSTANT_KILL = 59,	--秒杀
	CHARACTER_SPECIAL_EFFECT = 60,--特效列表
	ADD_SINGLE_SP = 61,
	REMOVE_SINGLE_SP = 62,
	SPIRIT_POINT_CHANGE = 63, 		--怒气值sp变化事件
	SEED_EXPLODE = 64, 		--爆种
	SPECIAL_EFFECT_TARGET = 65,	--特效作用目标
	SPREAD_SKILL = 66,			--扩散技能事件（一般在掉血前）
	HALLOW_PERFORM_SKILL = 67,	--圣器释放技能
	HALLOW_SKILL_HIT = 68,		--圣器技能命中
	HALLOW_SKILL_EFFECT = 69,	--圣器技能效果
	HALLOW_FLY_AWAY = 70,		--圣器被打飞
	HALLOW_FLY_WORDS = 71,		--圣器飘字
	HALLOW_ACTIVATE_LIST = 72,	--圣器激活列表
	HALLOW_INTERVENE = 73,		--圣器援护上场
	HALLOW_GIFT_TRIGGER = 74,	--圣器天赋触发
	HUANSHOU_SKILL_CD = 75,		--幻兽技能冷却通知

	DRAGON_SKILL_PERFORM = 76,  --神龙释放技能
	DRAGON_SKILL_HIT = 77,		--神龙技能命中
	DRAGON_SKILL_EFFECT = 78,  	--神龙技能效果事件
	DRAGON_PASSIVE_EFFECT = 79,	--神龙被动效果触发
	DRAGON_STAR_CHANGE = 80,	--神龙星体变化

	RABBIT_SHIELD_GRID_INFO_CHANGE = 81,	--护盾格子信息变更
	RABBIT_SHIELD_GRID_FLICKER = 82,		--护盾发光
	SEAL_NETHER_WORLD = 83,					--封印幽冥
	-- 客户端自用
	PLAY_ALL_BUFF = 10001,			--播放所有buff的某一阶段
	PLAY_ONE_BUFF = 10002,			--播放某种buff的某一阶段
};
BATTLE_DATA_EVENT_DESC = {
	[1] = 				"释放技能",
	[2] = 				"闪避",
	[3] = 				"HP变化",
	[4] = 				"合击",
	[5] = 				"逃跑",
	[6] = 				"BUFF替换",
	[7] = 				"BUFF合并",
	[8] = 				"BUFF新增到角色上",
	[9] = 				"BUFF去除",
	[10] = 				"自爆",
	[11] = 				"角色死亡",
	[12] = 				"反击",
	[13] = 				"被攻击时，被其他角色护卫",
	[14] = 				"换位",
	[15] = 				"使用物品",
	[16] =   			"替换装备",
	[17] = 				"技能CD时间改",
	[18] = 				"回合结束",
	[19] =  			"召唤战斗单位",
	[20] = 				"用于播放召唤动作",
	[21] = 				"用于播放召回动作",
	[22] = 				"合击播放一次攻击",
	[23] = 				"MP变化",
	[24] = 				"合击总伤害",
	[25] = 				"释放连击技能",
	[26] = 				"连击技能的一次攻击",
	[27] = 				"宠物封印 ",
	[28] = 				"使用法宝 ",
	[29] = 				"元素改变",
	[30] =				"原地消失",
	[31] =           	"技能前置效果事件",
	[32] =          	"技能后置效果事件",
	[33] =           	"技能效果事件",

	[34] =           	"（反伤盾）吸收伤害事件",

	[35] =       		"特殊技能效果（如被动技能、装备特效）",
	[36] =           	"技能命中（包含普攻）",
	[37] =				"破空无效",
	[38] = 				"对话",
	[39] = 				"镜头特效",
	[40] = 				"客户端指引",
	[41] = 				"变身",
	[42] =				"buff未生效0或者免疫1",
	[43] =				"使用法药次数变化",
	[44] =				"BUFF持续时间改变事件",
	[45] = 				"技能使用失败",
	[46] =				"怒气值改变",
	[47] =				"errorcode提示",
	[48] =				"客户端自定事件",
	[49] = 				"全队显示HP",
	[50] =				"法宝数据变化",
	[51] = 				"光环添加",
	[52] =				"光环删除",
	[53] =				"光环位置变化",
	[54] =				"行动失败=0找服务端",
	[55] = 				"技能次数改变",
	[56] =				"心法信息改变",
	[57] =				"链接在一起",
	[58] =				"链接断开",
	[59] =				"秒杀",
	[60] =				"角色特效列表",
	[61] =				"角色添加特效",
	[62] =				"角色移除特效",
	[63] =				"角色怒气变化",
	[64] =				"爆种",
	[65] =				"特效作用目标",
	[66] =				"扩散技能事件",
	[67] =				"圣器释放技能",
	[68] =				"圣器技能命中目标",
	[69] =				"圣器技能效果",
	[70] = 				"圣器被打飞",
	[71] =				"圣器飘字",
	[72] =				"圣器激活列表",
	[73] =				"圣器援护上场",
	[74] =				"圣器天赋触发",
	[75] = 				"幻兽技能冷却通知",
	[76] =				"神龙释放技能",
	[77] = 				"神龙技能命中",
	[78] = 				"神龙技能效果事件",
	[79] = 				"神龙被动效果触发",
	[80] = 				"神龙星体变化",
	[81] = 				"偃月兔护盾格子信息变更",
	[82] = 				"偃月兔护盾发光",
}
BATTLE_DIE_DESC = {
	[0] ="死亡倒地",
	"死亡倒地原地消失",
	"死亡击飞消失",
	"复活（凤凰涅槃）"
}
BATTLE_HP_DESC = {
	NORMAL = 0,			--普通扣血
	CRITICAL = 1,		--暴击
	-- DEFENSE = 2,		--防御
	SHIELD = 3,			--护盾
	POISON = 4,			--中毒
	COMBINE_ATTACK = 5,	--合击
	ADD_HP = 6,			--加血
	DAMAGE_ABSORB_HP = 7,--吸血
	IN_VAIN = 9,		--无效
	CRACK_DEFENCE = 10, --破防
	HP_CRITICAL = 12,	--暴击加血
	REAL_DAMAGE = 13,	--真实伤害
	SACRIFICE_HP = 14, 	--献祭自身血量
	DEATH_SICKIE = 15,	--死神之镰
	DESPERATE = 16, --摄魂夺魄
	BURN = 17,		--灼烧

	--下面都是客户端显示加的
	SP_EFF = 113,		--特效
	ACTION_FAIL = 114,	--XX中，无法释放
	BUFF_SHOW = 115,	--显示buff名称
	HP_END = 99,

	BUFF = 100,
	DODGE = 101,
	SKILL = 102,
	SEAL_PET = 103,
	MP_DECREASE = 104,	--攻击消蓝
	BUBBLE = 105,	--NPC泡泡框
	ATTACK_BACK = 106,	--反击
	
	BUFF_INVALID = 107,	--未生效
	BUFF_IMMUNE = 108,	--免疫
	BUFF_OPPOSE = 109,	--抵抗成功
	MIAO_SHA = 116, 	--秒杀
	SEED_EXPLODE = 117, --爆种
	XISHOU_DAMAGE = 118,--吸收伤害
	INTERVENE = 119,	--圣器闪现
	HALLOW_GIFT = 120,	--圣器天赋触发

	-- 服务器调试临时用
	BUFF_REMOVE = 110,
	BUFF_MERGE = 111,
	BUFF_REPLACE = 112,
}
BUFF_MISS_REASON = {
	[0] = BATTLE_HP_DESC.BUFF_INVALID,
	[1] = BATTLE_HP_DESC.BUFF_IMMUNE,
	[2] = BATTLE_HP_DESC.BUFF_OPPOSE,
}
BATTLE_BE_HIT_DESC = {
	NORMAL = 0,			--普通
	DEFENCE = 1,		--防御
	SHIELD = 2,			--护盾
	BLOCK = 3,			--格挡
	GUARD = 4,			--护卫
	SHARE_DAMAGE = 5,	--分担伤害
	DEATH_SICKLE = 6,	--死神之镰
	BLOCK_1 = 7,		--格挡(伪)(连击超有效次数)
}
BATTLE_MODIFY_MP_REASON = {
	SKILL_CONSUME = 0, 	--技能消耗
	DRUNK_STATE = 1,	--酒醉状态
	SELF_DESTRUCT = 2,	--自爆耗蓝
	SPECIAL_EFFECT = 3,	--特效削蓝
	BURN_MP_SKILL = 4,	--技能削蓝
	SYSTEM_SCRIPT = 5,	--系统脚本
	USE_MP_ITEM = 6,	--使用蓝药
	XI_LAN = 7,			--吸蓝
	NV_WA_SHI = 8,		--女娲石复活回蓝
	HALLOW_ADD_MP = 9,	--圣器加蓝
}
--战斗子界面
BattleSubView = {
	None = 0,
	Skill = 1,
	Item = 2,
	Pet = 3,
	Seal = 4,
	Fabao = 5,
}

BATTLE_SUMMON_REASON = {
	BATTLE_SUMMON_REASON_ROLE_SUMMON_PET = 0,	--召唤宠物
	BATTLE_SUMMON_REASON_MONSTER_SUMMON = 1,	--怪物召唤
	BATTLE_SUMMON_REASON_SCRIPT_SUMMON = 2,		--脚本召唤
	BATTLE_SUMMON_REASON_MONSTER_REINFORCE = 3,	--怪物增援
	BATTLE_SUMMON_REASON_ROLE_REINFORCE = 4,	--主角增援
	BATTLE_SUMMON_REASON_PET_REINFORCE = 5,		--宠物增援
	BATTLE_SUMMON_REASON_PET_SUMMON_PET = 6,	--宠物自己召唤宠物
	BATTLE_SUMMON_REASON_YOU_MING = 7,			--幽冥出现
}


BATTLE_CH_TYPE = {
	ROLE = 0,
	PET = 1,
	MONSTER = 2,
	PARTNER = 3,
	ROLE_SHADOW = 4,
	PET_SHADOW = 5,
	PARTNER_ROBOT = 6,
	NPC_HELPER = 7,
	YOU_MING = 8,		-- 幽冥
}
FLY_WORDS_TYPE = 
{
  FLY_WORDS_TYPE_INVALID = 0,
  FLY_WORDS_TYPE_DODGE = 1,    -- 闪避
  FLY_WORDS_TYPE_USE_FAIL = 2,--没对象释放失败
  FLY_WORDS_TYPE_ALL_IN_CD = 3,--CD中释放失败
  FLY_WORDS_TYPE_DEAD_ABOR = 4,--主人死了释放失败
  FLY_WORDS_TYPE_NUM = 3,
}

--不做表现的技能特效
--[[ BATTLE_SKILL_SHOW_IGNORE = {
	[2] = true,	--防御
} ]]
--技能表现不在原地的不显示被击
--[[ BATTILE_SKILL_NO_BEHIT = {
	["MFDanTiFengRenMoFaXuLi"] = true,
	["HuanJingZhiYi"] = true,
	["HunLuanZhiFeng"] = true,
	["ChenMoFengBao"] = true,
	["MFQunTiFengRenMoFaXuLi"] = true,
} ]]

--[[ function BATTLE_SKILL_SHOW_IGNORE.Ignore(skill_id)
	return BATTLE_SKILL_SHOW_IGNORE[skill_id] == true
end
 ]]
--护卫技能（不移动
--[[ BATTLE_SKILL_GUARD = {
	["HuWeiXuLi"] = true,
	["QiShiZhiShiShouJi"] = true,
} ]]
--自身反噬的技能不显示被击
--[[ BATTLE_HIT_SELF_NOSHOW = {
	["BuffWLHunLuanGongJi"] = true,
	["SheShenQuYi"] = true,
	["YiShenPoDi"] = true,
	["SheMingYiJi"] = true,
	["PuGongLianJiShouJi"] = true,
	["SheMingYiJi2"] = true,
} ]]
--冲刺类不需要额外的移动逻辑
--[[ BATTLE_CHONGCHI_SKILL = {
	["Assassinate"] = true,
	["PoJiaChongQuan2"] = true,
	-- ["LiYuTouChui"] = true,
	["Charge"] = true,
	["MultipleCharge"] = true,
	["PoJiaChongQuan"] = true,
	["TieBiChongYa"] = true,
	["SheMingYiJi2"] = true,
} ]]
--释放冲刺不需要前置移动技能释放完后要移动
--[[ BATTLE_CHONGCHI_PRE_SKILL = {
	["LiYuTouChui"] = true,
	["SheMingYiJi"] = true,
} ]]

--护卫需要短暂停顿
--[[ BATTLE_HUWEI = {
	["HuWei"] = true
} ]]
--石化
--[[ SKILL_SHIHUA = {
	["BuffMFShiHuaMoFa"] = true,
	["BuffWLShiHuaGongJiShouJi"] = true,
} ]]
--延迟buff表现
--[[ DELAY_PALY_BUFF = {
    -- [420] = true,
    [303] = true,
    [3] = true,
	[4] = true,
}
 ]]
BATTLE_MODIFY_RAGE_POINT_REASON = 
{
  ROUND = 0,      -- 0 回合开始
  NORMAL_ATTACK = 1,  -- 1 普攻
  NORMAL_DEFENSE = 2,  -- 2 防御
  PERFORM_SKILL = 3,  -- 3 释放技能
}
--吸血类技能
--[[ BATTLE_HUIXUE = {
	""
} ]]
--溅射
--[[ SKILL_JIANSHE = {
	["LuoYingLianFu"] = true,
	["TengBian"] = true,
} ]]

--艺术字显示技能名称通用逻辑忽略
--[[ SKILLNAME_BIG_SHOW = {
	["BaTianHuiXuan"] = true,
	["KuangNuJianTa"] = true,
} ]]
--技能伤害延迟显示
--[[ DAMAGE_DELAY_SHOWTIME = {
	["WuYingQuan"] = 0.4,
	-- ["PuGongZhuRenShouJi"] = 0.1
} ]]
--忽略演员限制
--[[ IGNORE_ACTOR_LIMIT = {
	["XuanZhuanFeiFu2"] = true,
	["BaTianHuiXuan"] = true,
	["XTXuanZhuanFeiFu"] = true,

} ]]
--闪避忽略攻击者
--[[ IGNORE_ATKER_DODGE = {
	["SheMingYiJi"] = true
} ]]

--战斗地图
ModeToMap = {
	[BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE] = "PTZD_1",
	[BATTLE_MODE.BATTLE_MODE_STORY] = "SDZD_3",
	[BATTLE_MODE.BATTLE_MODE_ARENA] = "JJC_1",
	[BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE] = "PTZD_2"
}

--忽略战斗连续播放队列的战斗模式
--[[ BATTLE_IGNORE_SEQUENCE = {
	[BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT] = true,
} ]]

BATTLE_SHOW_SKIP = {
	[BATTLE_MODE.BATTLE_MODE_ARENA] = true,
	[BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = true,
	[BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = true,
	[BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = true,
}
BATTLE_SHOW_SKIP_DESC = {
	[BATTLE_MODE.BATTLE_MODE_ARENA] = Language.Guide.Skip,
	[BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = Language.Guide.Skip,
	[BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = Language.Guide.Defeat,
	[BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = Language.Guide.Defeat,
}
BATTLE_SPIRIT_POINT_CHANGE_REASON = {
	CHANGE_REASON_DIE_REDUCE = 0,		--死亡减少怒气值
	CHANGE_REASON_ROUND_ADD = 1,		--回合结束加
	CHANGE_REASON_SKILL_COST = 2, 		--技能消耗
}

--特殊自爆
--[[ function BATTLE_SELF_EFFECT(id)
	return (id >= 815 and id <= 840)
end
function BATTLE_XINGTIAN_JUQING(id)
	return id == 1014
end
function BATTLE_XINMO_JUQING(id)
	return id == 1006
end
function BATTLE_NIUYAO_JUQING(id)
	return id == 1002
end
function BATTLE_ORIGIN_JUQING(id)
	return id == 1001
end

COMBO_SKILL_SP = {
	["FenShao"] = true,
	["DongJie"] = true,
	["LianZhuJian"] = true,
	["SanQianLeiDong"] = true,
} ]]
--[[ SP_EFFECT_SHOW_BY_BUFF = {
	["LingGuangPiFeng"] = true,
	["ZhiMingYiJi"] = true,
	["FengHuiLuZhuan"] = true,
} ]]

--当前回合
BATTLE_SHOW_CUR_ROUND = {
	[BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = true,
	[BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION] = true,
	[BATTLE_MODE.BATTLE_MODE_CHIEF_PRIMARY] = true,
	[BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = true,
	[BATTLE_MODE.BATTLE_MODE_WAVE] = true,
	[BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = true,
	[BATTLE_MODE.BATTLE_MODE_GHOST_NIGHT] = true,
	[BATTLE_MODE.BATTLE_MODE_TEAM_SPORT] = true,
	[BATTLE_MODE.BATTLE_MODE_ARENA] = true,
	[BATTLE_MODE.BATTLE_MODE_WORLD_ARENA] = true,
}

--越战越勇
BATTLE_SHOW_CHIEF_BUFF = {
	[BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION] = true,
	[BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = true,
	[BATTLE_MODE.BATTLE_MODE_TEAM_SPORT] = true,
	[BATTLE_MODE.BATTLE_MODE_WORLD_ARENA] = true,
	[BATTLE_MODE.BATTLE_MODE_WORLD_TEAM_AREMA] = true,
	[BATTLE_MODE.BATTLE_MODE_UTA] = true,
}

--奖励预览
BATTLE_SHOW_REWARD_PREVIEW = {
	[BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB] = true,
	[BATTLE_MODE.BATTLE_MODE_WORLD_BOSS] = true,
	[BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND] = true,
	[BATTLE_MODE.BATTLE_MODE_SHXY] = true,
}

--需要有天雷下落的战斗
BATTLE_TOP_RIGHT_CONFIG = {
	[BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY] = true,
	[BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE] = true,
    [BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER] = true ,
    [BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST] = true,
	[BATTLE_MODE.BATTLE_MODE_GUILD_BOSS] = true,
	[BATTLE_MODE.BATTLE_MODE_FALLEN_GOD] = true,
	[BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN] = true,
	[BATTLE_MODE.BATTLE_MODE_HARMONY_HEAVEN] = true,
	[BATTLE_MODE.BATTLE_MODE_REALM_BREAK_FIGHT] = true,
	[BATTLE_MODE.BATTLE_MODE_ELITE_TRAILS] = true,
	[BATTLE_MODE.BATTLE_MODE_MI_YI] = true,
	-- [BATTLE_MODE.BATTLE_MODE_BIG_DIPPER] = true,
	[BATTLE_MODE.BATTLE_MODE_ANCIENT_RUINS] = true,
	[BATTLE_MODE.BATTLE_MODE_GO_THROUGH] = true,
	[BATTLE_MODE.ZHEN_YING_SHEN_SHOU] = true,
	[BATTLE_MODE.DRAGON_VEIN_MONSTER] = true,
	[BATTLE_MODE.DRAGON_VEIN_BOSS] = true,
}

--战斗结束后 主界面不需要自动展开
BATTLE_TOP_MAIN_NOT_SHOW = {
	[SCENE_TYPE.WEDDING_FB] = true,
}

--主界面不显示地图图标
MAIN_NOT_SHOW_MAP_ICON = {
	[MainViewGameState.du_chuang_zei_ying] = true,
	[MainViewGameState.rush_dungeon] = true,
	[MainViewGameState.fight_crystal] = true,
	[MainViewGameState.lucky_maze] = true,
}

--奖励预览
MAIN_SHOW_REWARD_PREVIEW = {
	[MainViewGameState.rush_dungeon] = true,
	[MainViewGameState.fight_crystal] = true,
}

BATTLE_END_SHOW_BOX = {
    [BATTLE_MODE.BATTLE_MODE_FALLEN_GOD] = function(group_id, end_func)
        local item_list = FallenGodData.Instance:GetRewardList()
        if item_list == nil then
            if end_func then
                end_func()
            end
            return
        end
        local cfg = Cfg.FallenGodCfgByGroupId(group_id, false)
        if cfg then
			local info = Config.sundries_auto.box_reward[BATTLE_MODE.BATTLE_MODE_FALLEN_GOD]
			local vo = Config.npc_auto.npc_list[info.box_type]
			if vo then
				if not ViewMgr:IsOpen(GuideBlock) then
					ViewMgr:OpenView(GuideBlock)
				end
				local box_color = ItemColor.Purple
				for k, v in pairs(item_list or {}) do
					box_color = math.max(box_color, v:Color())
				end
				local opos = MainPlayer:GetPos():Clone()
				opos.x = cfg.box_x
				opos.z = cfg.box_y
				vo = TableCopy(vo)
				vo.box_color = box_color
				BattleData.Instance:CreateRewardBox(vo, opos)
				TimeHelper:AddDelayTimer(function()
					if end_func then
						end_func()
					end
					BattleData.Instance:ClearRewardBox()
				end, 8)
			else
				if end_func then
					end_func()
				end
			end
        else
            if end_func then
                end_func()
            end
            return
        end
    end,
    [BATTLE_MODE.BATTLE_MODE_HARMONY_HEAVEN] = function(group_id, end_func)
        local item_list = HarmonyHeavenData.Instance.reward_list_all
        if item_list == nil then
            if end_func then
                end_func()
            end
            return
        end
        local objs = HarmonyHeavenData.Instance:GetRewardMapObjs()
        if objs and next(objs) then
			local info = Config.sundries_auto.box_reward[BATTLE_MODE.BATTLE_MODE_HARMONY_HEAVEN]
			local vo = Config.npc_auto.npc_list[info.box_type]
			if vo then
				if not ViewMgr:IsOpen(GuideBlock) then
					ViewMgr:OpenView(GuideBlock)
				end
				local box_color = ItemColor.Purple
				for k, v in pairs(item_list or {}) do
					box_color = math.max(box_color, v:Color())
				end
				local scene_cfg = SceneData.Instance:SceneConfig()
				local opos = MainPlayer:GetPos():Clone()
				local pos = GameMath.PosToClint(objs[1].pos_x, objs[1].pos_y, scene_cfg.mapid)
				opos.x = pos.x
				opos.z = pos.z
				vo = TableCopy(vo)
				vo.box_color = box_color
				BattleData.Instance:CreateRewardBox(vo, opos)
				-- 播放动作等等
				TimeHelper:AddDelayTimer(function()
					if end_func then
						end_func()
					end
					BattleData.Instance:ClearRewardBox()
				end, 8)
			else
				if end_func then
					end_func()
				end
			end
        else
            if end_func then
                end_func()
            end
            return
        end
    end
}
--不显示战斗过程 战斗结算协议照走，导致重复gotoscene需要判断一下
NO_SHOW_BATTLE_PROCESS = {
	[BATTLE_MODE.GUI_XU_ZHI_JING_BOSS] = true,
	[BATTLE_MODE.GUI_XU_ZHI_JING_PVP] = true,
	[BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS] = true,
	[BATTLE_MODE.YUN_DING_TIAN_GONG_PVP] = true,
}

-- 偃月兔护盾最大格子
RABBIT_SHILED_GRID_MAX = 10
RABBIT_SHIELD_GRID_BEGIN_INDEX = 40;        --//兔子盾牌格子起始(伪)

BATTLE_SEAL_NETHER_WORLD_CARD_TYPE = {
	COMMON = 0, --普通款
	GOLD = 1, --金卡
}