-- 灵骑数据配置，常量和枚举那些
BicycleCfg = {}

BicycleCfg.MAX_BASE_SKILL_CUR_NUM = 3               -- 灵骑最大基础技能数量
BicycleCfg.MAX_PET_NUM = 2                          -- 灵骑最大御宠数量
BicycleCfg.MAX_ATTR_NUM = 5                         -- 灵骑最大属性类型
BicycleCfg.MAX_EXCLUSIVE_SKILL_NUM = 2              -- 灵骑最大专属技能数量
BicycleCfg.MAX_EQUIP_NUM = 2                        -- 灵骑最大的装备数量

-- 请求类型 -- 对应2100 协议
BicycleCfg.OP_TYPE = {
    ALL_INFO = 0,				--!< 请求灵骑信息
    SINGLE_INFO = 1,			--!< 请求灵骑单个信息 param1 : index
    ACTIVE = 2,					--!< 请求激活灵骑		param1 : index
    LEVEL_UP = 3,				--!< 请求升级灵骑		param1 : index
    BREAK_LEVEL = 4,			--!< 请求突破等级		param1 : index
    REFINE_NATURE = 5,			--!< 请求洗练灵性		param1 : index
    SKILL_UP = 6,				--!< 请求升级技能		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(技能格子)  【*屏蔽*】
    REFINE_SKILL = 7,			--!< 请求洗练技能		param1 : index param2 : 上锁 [0-2bit]二进制 置0:没上锁, 置1:上锁    【*屏蔽*】
    SAVE_REFINE_SKILL = 8,		--!< 请求保存洗练		param1 : index    【*屏蔽*】
    CANCEL_REFINE_SKILL = 9,	--!< 请求取消洗练		param1 : index    【*屏蔽*】
    WEAR_ORNAMENTS = 10,		--!< 请求穿戴饰品		param1 : index 
    TAKE_OFF = 11,				--!< 请求脱下饰品		param1 : index    【*弃用*】
    ADD_ENERGY = 12,			--!< 请求补充精力		param1 : index
    RESET_SKILL = 13,			--!< 请求技能重置		param1 : index param2 : base_or_exclusive[0, 1] param3 : skill_index(技能格子)  【*屏蔽*】
    DOMINANCE_PET = 14,			--!< 请求统御宠物		param1 : index param2 : pet_index param3 : dominance_index[0, 1]
    CANCEL_DOMINANCE = 15,		--!< 请求取消统御		param1 :　index param2 : dominance_index[0, 1]
    ATTR_REQ = 16,				--!< 请求单个灵骑属性 param1 : index
    FINISH_FUN_GUIDE = 17,		--!< 请求完成指引	
    RIDE = 18,					--!< 请求骑乘		param1 : index
    CANCEL_RIDE = 19,			--!< 请求取消骑乘
    SET_MODEL = 20,             -- 设置形象 p1 : index p2 : 0:普通形象 1:进阶形象
    ACTIVE_EQUIP = 21,          -- 激活装备 p1 : index p2 : equip_index[0,1]
    UP_EQUIP = 22,              -- 装备升级 p1 : index p2 : equip_index[0,1]
    JEWELRY_STREGTH = 23,		-- 灵饰强化 p1 : index
    ACTIVE_HALIDOM = 24,        -- 激活圣物 p1 : index
    UP_HALIDOM = 25,            -- 圣物升级 p1 : index
    UP_DIAN_HUA = 26,           -- 灵骑点化 p1 : index
    CHANGE_ITEM = 27,           -- 请求转换灵骑装备圣物 p1：item_id，p2:item_id
    USE_ACT_ITEM = 28,          -- 请求使用激活灵骑道具转换 p1：column，p2:index
    SKILL_ITEM_COMPOSE = 29,    -- 灵技合成 p1 : main_item_id   p2 : stuff_item_id  p3:target_item_id
    OPEN_AUTO_RECOVER = 30,     -- 请求开启自动回复 p1:index
    CALCEL_AUTO_RECOVER = 31,   -- 请求关闭自动回复 p1:index
    HOLY_CONVERT = 32,          -- 圣物等级转换 p1:source_index p2:target_index(转换为哪个灵骑圣物)
}

-- 技能相关请求
BicycleCfg.OP_TYPE_SKILL = {
    LEAEN_SKILL = 0,    -- 请求学习技能     p1:index p2:skill_index p3:bag_colume p4:bag_index
    SKILL_UP = 1,       -- 请求升级灵骑技能 p1:index p2:skill_index p3:bag_colume p4:bag_index
    SKILL_REPLACE = 2,  -- 请求置换灵骑技能 p1:index p2:skill_index p3:bag_colume p4:bag_index
}

-- 通知类型 -- 对应 2105 协议
BicycleCfg.NOTICE_TYPE = {
    ACTIVE = 0,				    -- 激活灵骑		param1:smart_mounts_level param2:smart_nature_value (灵性值需要除100)
    LEVEL_UP = 1,				-- 等级提升		param1:smart_mounts_level param2:remain_skill_point 
    BASE_SKILL_UP = 2,			-- 基础技能提升		param1:skill_level param2:skill_index
    EXCLUSIVE_SKILL_UP = 3,     -- 专属技能提升     param1:skill_level param2:skill_index    <<废弃>>
    BREAK_SUCC = 4,				-- 突破成功		
    ENERGY_VALUE = 5,			-- 灵骑精力不足	param1:energy_value
    ACTIVE_EQUIP = 6,           -- 激活装备 p1:equip_index,p2:level
    UP_EQUIP = 7,               -- 升级装备 p1:equip_index,p2:level
    ACTIVE_HALIDOM = 8,         -- 激活圣物 p1:level
    UP_HALIDOM = 9,             -- 圣物升级 p1:level
    UP_DIAN_HUA = 10,           -- 灵骑点化
    LEARA_SKILL = 11,           -- 学习技能 p1:skill_index p2: skill_id p3:skill_level
    REPLACE_SKILL = 12,         -- 置换技能 p1:skill_index p2: skill_id p3:skill_level
    HOLY_CONVERT = 13,          -- 圣物等级转换 p1:target_str_level p2:source_index p3:source_str_level
}
-- 变化类型 -- 对应 2108 协议
BicycleCfg.CHANGE_TYPE = {
    ENERGY = 0,			        --精力值变化  param1:精力值
    NATURE = 1,			        --灵性值变化  param1:灵性值 (灵性值需要除100)
    ORNAMENTS = 2,			    --饰品穿戴变化 param1:穿戴的饰品ID
    UP_ORNAMENT = 3,            --升级饰品 param1: 饰品强化等级
    MODEL_CHANGE = 4,           --灵骑形象变化 p1: 0 普通形象，1：进阶形象
    AUTO_RECOVER_FLAG = 5,      --自动进食变化 p1: 是否自动进食
}

-- 技能类别
BicycleCfg.SKILL_TYPE = {
    BASE = 0,       -- 基础技能
    EXCLUSIVE = 1,  -- 专属技能
}

-- 途径|解锁条件类型
BicycleCfg.CONDITION_TYPE = {
    ROLE_LEVEL = 1,      -- 角色等级条件
    BICYCLE_LEVEL = 2,   -- 灵骑等级条件
    BICYCLE_SKILL_LEVEL = 3,        -- 灵骑技能等级条件
    BICYCLE_SKILL_MAX_LEVEL = 4,    -- 满级的灵骑技能条件
    ACTIVITY = 5,                   -- 运营活动获得条件
}

-- 灵骑展示界面显示类型
-- 通知类型 -- 对应 2105 协议
BicycleCfg.SHOW_TYPE = {
    Not = -1,                   -- 无
    ACTIVE_BICYCLE = 1,		    -- 激活灵骑
    ACTIVE_SKILL = 2,		    -- 激活技能     -- 弃用
    UP_BICYCLE = 4,			    -- 灵骑提升
    UP_SKILL = 3,               -- 技能提升    
    BREAK_SUCC = 5,				-- 突破成功
    EQUIP = 6,                  -- 穿戴装备成功  -- 未开发
    HALIDOM_ACTIVE = 7,         -- 圣物激活
    HALIDOM_UP = 8,             -- 圣物提升
    DIAN_HUA_SUCC = 9,          -- 点化成功
    SKILL_CONVERT = 10,         -- 技能置换
    SKILL_STUDY = 11,           -- 技能学习
}

-- 展示开关配置
BicycleCfg.SHOW_TYPE_CARE = {
    [BicycleCfg.SHOW_TYPE.ACTIVE_BICYCLE] = true,
    [BicycleCfg.SHOW_TYPE.UP_BICYCLE] = true,
    [BicycleCfg.SHOW_TYPE.HALIDOM_ACTIVE] = true,
    [BicycleCfg.SHOW_TYPE.HALIDOM_UP] = true,
    [BicycleCfg.SHOW_TYPE.DIAN_HUA_SUCC] = true,
    [BicycleCfg.SHOW_TYPE.SKILL_STUDY] = true,
    [BicycleCfg.SHOW_TYPE.SKILL_CONVERT] = true,
}
-- 灵骑特效
BicycleCfg.EFFECT = {
    NULL = "",
    UP_LEVEL = "2165011",
    UP_SKILL_START = "2165023",
    UP_SKILL_END = "2165024",
    UP_DIAN_HUA = "3165186",
}

BicycleCfg.QUA = {
    [1] = "PingZhi2",
    [2] = "PingZhi2",
    [3] = "PingZhi3",
    [4] = "PingZhi4",
    [5] = "PingZhi5",
    [6] = "PingZhi6",
}
BicycleCfg.SkillQua = {
    [3] = "HuoBanPinZhiKuangLan",
    [4] = "HuoBanPinZhiKuangZi",
    [5] = "HuoBanPinZhiKuangCheng",
    [6] = "HuoBanPinZhiKuangHong",
}
-- 灵骑装备圣物置换
BicycleCfg.ChangeItem = {
    Equip = 1,
    Halidom = 2,
}