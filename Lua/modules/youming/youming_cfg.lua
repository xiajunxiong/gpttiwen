YouMingCfg = {}

YouMingCfg.ReqType =
{
    ALL_INFO = 0,					--//!< 请求幽冥系统所有信息
    BAG_ALL_INFO = 1,				--//!< 请求幽冥系统背包所有信息
    HUNT_TREASURE = 2,				--//!< 请求幽冥系统探宝 P1:treasure_id P2:(0:单抽)
    FETCH_LUCKY_REWARD = 3,		    --//!< 请求领取探宝幸运值奖励 P1:treasure_id P2:reward_index
    POSSESS = 4,					--//!< 请求幽冥附身 P1:bag_index P2:pet_index
    SOUL_LEVEL_UP = 5,				--//!< 请求升级三魂 P1:bag_index P2: \see NETHER_WORLD_SOUL_TYPE_MAX
    RESET_SOUL = 6,				    --//!< 请求重置三魂 P1:bag_index
    SPIRITS_LEVEL_UP = 7,			--//!< 请求七魄升级 P1:bag_index P2: \see NETHER_WORLD_SPIRITS_TYPE_MAX
    AWAKEN_LEVEL_UP = 8,			--//!< 请求觉醒升级 P1:bag_index        --废弃
    PUT_ON_EQUIP = 9,				--//!< 请求穿戴冥装 P1:bag_index P2:equip_bag_index(冥装背包下标)
    TAKE_OFF_EQUIP = 10,			--//!< 请求脱下冥装 P1:bag_index P2:NETHER_WORLD_EQUIP_GRID_TYPE_MAX
    REFORM = 11,					--//!< 请求幽冥改造 P1:bag_index P2:reform_index(改造下标) P3:item_id
    USE_FRUIT = 12,				    --//!< 请求使用果实 P1:bag_index P2:item_id
    REMODELING = 13,				--//!< 请求幽冥重塑 P1:bag_index 
    COMPOSE = 14,					--//!< 请求合成幽冥 P1:item_id
    DECOMPOSE = 15,				    --//!< 请求分解幽冥 P1:bag_index
    STR_BAG_EQUIP = 16,			    --//!< 请求强化背包冥装 P1:equip_bag_index  --废弃，背包不给操作
    STR_WEAR_EQUIP = 17,			--//!< 请求强化穿戴冥装 P1:bag_index P2:NETHER_WORLD_EQUIP_GRID_TYPE_MAX
    ATTR_INFO = 18,				    --//!< 请求幽冥属性 P1:bag_index
    CANCEL_POSSESS = 19,			--//!< 请求取消附身 P1:bag_index
    SAVE_OR_CANCEL_REFORM = 20,     --//!< 请求保存取消幽冥改造结果 P1:bag_index P2:flag(0:cancel)
    SAVE_OR_CANCEL_REMODELING = 21, --//!< 请求保存取消幽冥重塑结果 P1:bag_index P2:flag(0:cancel)
    POSSESS_INFO = 22,              --//!< 请求幽冥系统附身列表信息
    SINGLE_INFO = 23,               --//!< 请求单条幽冥/冥装信息下发 P1:bag_type P2:bag_index
    ADD_GROWTH = 24,                --//!< 请求分配自由天资点 P1 bag_index P2:ADD_POINT_TYPE_MAX
    REPEAT_REFORM = 25,             --//!< 请求幽冥重新改造 P1:bag_index P2:reform_index(改造下标) P3:item_id
    CHECK_TREASURE = 26,            -- 请求服服务端检查探宝
}

YouMingCfg.NOTICE_TYPE =
{
    NOTICE_TYPE_SOUL_LEVEL_UP = 0,			--// 三魂升级 P1:bag_index P2:\see NETHER_WORLD_SOUL_TYPE_MAX P3:level
    NOTICE_TYPE_RESET_SOUL = 1,				--// 三魂重置 P1:bag_index																		 --发
    NOTICE_TYPE_SOUL_UNLOCK = 2,			--// 三魂解锁 P1:bag_index P2:\see NETHER_WORLD_SOUL_TYPE_MAX P3:level:
    NOTICE_TYPE_SPIRITS_LEVEL_UP = 3,		--// 七魄升级 P1:bag_index P2:\see NETHER_WORLD_SPIRITS_TYPE_MAX P3:level
    NOTICE_TYPE_AWAKEN_LEVEL_UP = 4,		--// 觉醒提升 P1:bag_index P2:level
    NOTICE_TYPE_STR_EQUIP_UP = 5,			--// 强化冥装 P1:equip_grid_index P2:str_level
    NOTICE_TYPE_STR_WEAR_EQUIP_UP = 6,		--// 强化穿戴冥装 P1:bag_index P2:NETHER_WORLD_EQUIP_GRID_TYPE_MAX P3:str_level
    NOTICE_TYPE_WEAR_EQUIP_ADD_EXP = 7,		--// 转换材料添加经验冥装 P1:bag_index P2:NETHER_WORLD_EQUIP_GRID_TYPE_MAX P3:level P4:cur_exp
    NOTICE_TYPE_POSSESS_CHANGE = 8,			--// 幽冥附身变化 p1:bag_index p4:pet_item_unique_id
    NOTICE_TYPE_REFORM = 9,					--// 幽冥改造 P1:bag_index P2:reform_index														--发
    NOTICE_TYPE_SAVE_OR_CANCEL_REFORM = 10,	--// 幽冥保存/取消改造结果 P1:bag_index P2:reform_index											--发
    NOTICE_TYPE_USE_FRUIT = 11,				--// 幽冥使用水果 P1:bag_index P2:reform_index	P3:attr_index(哪个资质） P4:growth(该改造对应资质此时的值)								
    NOTICE_TYPE_USE_FRUIT_1 = 12,			--// 幽冥使用水果 P1:bag_index P2:reform_index	P3:reform_add_growth(自由点）  //11与12连发								
    NOTICE_TYPE_REMODELING = 13,			--// 幽冥重塑	P1:bag_index 																	--发
    NOTICE_TYPE_SAVE_OR_CANCEL_REMODELING = 14,	--// 幽冥保存/取消重塑结果	P1:bag_index 													--发
    NOTICE_TYPE_GRASP_SKILL = 15,			--// 幽冥领悟技能成功 P1:bag_index P2:reform_index P3:skill_index P4:skill_id
    NOTICE_TYPE_ADD_GROWTH = 16,            --// 幽冥分配自由天资点 P1 bag_index P2 reform_index P3:ADD_POINT_TYPE_MAX -- 发
    NOTICE_TYPE_GRASP_SKILL_FALIED = 17,    --// 幽冥领悟技能失败 P1:bag_index P2:reform_index P3:skill_index P4:skill_id --发
    NOTICE_TYPE_TURN_TREASURE = 18,         --转动探宝转盘 p1,treasure_id, p2 value
    NOTICE_TYPE_TREASURE_INFO_CHANGE = 19,  --探宝信息变动 p1,treasure_id, p2 value p3:fetch_flag

    --发 指发5442
}

YouMingCfg.SkillType = {
    Initiative = 1,     -- 主动技能
    Appendage = 2,      -- 附身技能
    Inborn = 3,         -- 出场技能
}
YouMingCfg.SoulType = {
    HEAVENLY = 0,	--//天魂
	EARTHLY = 1,	--//地魂
	LIFE = 2,		--//命魂
}
YouMingCfg.SpiritsType = {
    TYPE_0 = 0,
	TYPE_1 = 1,
	TYPE_2 = 2,
	TYPE_3 = 3,
	TYPE_4 = 4,
	TYPE_5 = 5,
	TYPE_6 = 6,
}
YouMingCfg.GridType = {
    GOLD = 0,
	WOOD = 1,
	WATER = 2,
	FIRE = 3,
	SOIL = 4,
}

-- 天资属性
YouMingCfg.Aptitude = {
    ["con"] = BattleAttriType.VITALITY, --体力45
    ["str"] = BattleAttriType.STRENGTH, --力量46
    ["def"] = BattleAttriType.POWER,    --强度47
    ["agi"] = BattleAttriType.AGILITY,  --敏捷48
    ["magic"] = BattleAttriType.MAGIC,    --法术49
}

YouMingCfg.Aptitude2 = {
    [1] = BattleAttriType.VITALITY, --体力45
    [2] = BattleAttriType.STRENGTH, --力量46
    [3] = BattleAttriType.POWER,    --强度47
    [4] = BattleAttriType.AGILITY,  --敏捷48
    [5] = BattleAttriType.MAGIC,    --法术49
}

YouMingCfg.ShowAttriType = {
    BattleAttriType.MaxHp,
    BattleAttriType.MaxMp,
    BattleAttriType.Attack,
    BattleAttriType.Defense,
    BattleAttriType.Agility,
    BattleAttriType.Mental,
    BattleAttriType.Recovery,
}

YouMingCfg.NETHER_WORLD_BAG_TYPE = {
    NETHER_WORLD_BAG_TYPE_ALL = -1,     --全发
    NETHER_WORLD_BAG_TYPE_MIN = 0,
	NETHER_WORLD_BAG_TYPE_NW = 0,		--幽冥背包
	NETHER_WORLD_BAG_TYPE_NWE = 1,		--冥装背包
}

YouMingCfg.MAX_NETHER_WORLD_SYSTEM_BAG_NUM = 300                    -- 背包最大幽冥
YouMingCfg.MAX_NETHER_WORLD_EQUIP_BAG_NUM = 200                     -- 冥装背包数量
YouMingCfg.ADD_POINT_TYPE_MAX = 5                                   -- 最大资质数量
YouMingCfg.PET_PASSIVE_MAX_COUNT = 20                               -- 最大天赋数
YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM = 5	                --// 最多改造上限
YouMingCfg.MAX_NETHER_WORLD_REFORM_PASSIVE_NUM = 5                  -- 单次改造最多可得技能孔位上限
YouMingCfg.MAX_NETHER_WORLD_HUNT_TREASURE_LUCKY_REWARD_NUM = 32 	--//探宝幸运值奖励数量
YouMingCfg.MAX_NETHER_WORLD_SOUL_LEVEL_ATTR_NUM = 4 		        --// 三魂属性数量上限
YouMingCfg.MAX_NETHER_WORLD_SPIRITS_LEVEL_ATTR_NUM = 4	            --// 七魄属性数量上限
YouMingCfg.MAX_NETHER_WORLD_EQUIP_BASE_LEVEL_ATTR_NUM = 3           --// 冥装基础等级属性数量上限
YouMingCfg.MAX_NETHER_WORLD_EQUIP_STR_LEVEL_ATTR_NUM = 3	        --// 冥装强化等级属性数量上限
YouMingCfg.MAX_NETHER_WORLD_EQUIP_GRID_TYPE_NUM = 5                 -- 冥装最大数量
YouMingCfg.MAX_NETHER_WORLD_EQUIP_ATTR_AFFIX_NUM = 4                -- 最大副词条
YouMingCfg.MaxAwakenLevel = 15                                      -- 最大觉醒等级
YouMingCfg.QiPoLevel = 50                                           -- 最大七魄等级
YouMingCfg.SanHunLevel = 40                                           -- 最大三魂等级

YouMingCfg.ChangeItemId = 26392                                     --最low改造书id
YouMingCfg.ChangeItemList = {{item_id = 26392},{item_id = 26393},{item_id = 26394},{item_id = 26395},{item_id = 26396}}


YouMingCfg.TIP_ID = 407     -- 总说明id
YouMingCfg.TIANZI_ID = 408  -- 天资说明id
YouMingCfg.PossessSkillLevel = 11 --激活等级+1