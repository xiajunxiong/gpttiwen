ShengQiCfg = {
    IS_OPEN_LOG = false, --是否开启log
    
    MAX_HOLY_EQUIP_BASE_VOC_NUM = 7,    -- 圣器基础属性条数
    MAX_ACTIVE_SKILLS_NUM = 5,          -- 主动技能条数
    MAX_TALENT_SKILLS_NUM = 2,          -- 天赋技能条数
    MAX_FU_SHI_NUM = 5,                 -- 最大镶嵌符石数量
    MAX_HOLY_EQUIP_EXTRA_VOC_NUM = 2,   -- 属性额外随机基础属性词条   --(目前没用)
    MAX_SHENG_QI_STUFF_COUNT = 30,       -- 最多可放多少个圣器当材料
    MAX_FUSHI_COUNT= 500,               -- 符石最大数量

    State = {
        Rest = 0,       -- 休息
        Ready = 1,      -- 待机
        Fight = 2,      -- 出战
    },

    StateImg = {
        [0] = "_LocXiu",
        [1] = "_LocDai",
        [2] = "_LocZhan",
    },

    -- 圣器主动技能分类
    InitativeType = {
        Attack = 1,     -- 攻击类
        Help = 2,       -- 辅助类
        Shoot = 3,      -- 打宝类
    },

    -- 圣器天赋技能分类
    TalentType = {
        General = 1,    -- 普通
        Advanced = 2,   -- 高级
        SP = 3,         -- 超级
    },

    BagType = {
        ShengQi = 1,
        FuShi = 2,
    },

    -- 圣器/符石改变的原因
    SendReason = {
        Normal = 0,     -- 正常下发（上线或客户端请求）	
        GetIn = 1,      -- 1 外部获得飘字
        ShengQiLvUp = 2,-- 2 圣器升级消耗
        ChangeData = 3, -- 3 道具数据改变	符石/圣器
        ShengQiState = 4,   -- 4 圣器状态改变
        FuShiFusion = 5,    -- 符石合成
        FuShiReset = 6,     --符石重铸
        FuShiUse = 7,       --符石使用
        HuangLing = 8,      --圣器唤灵
        BlessPet = 9,       --圣佑
        SkillUpdate = 10,   --学习技能变化
        Sell = 11,          --市场出售符石
    },

    -- 属性字段名
    AttriFieldName = {
        [BattleAttriType.Attack] = "attack",
        [BattleAttriType.Defense] = "defense",
        [BattleAttriType.Mental] = "spirit",
        [BattleAttriType.Recovery] = "restore",
        [BattleAttriType.MaxMp] = "mp",
        [BattleAttriType.MaxHp] = "hp",
        [BattleAttriType.Agility] = "speed",
        [BattleAttriType.AntiPosition] = "de_zhongdu",
        [BattleAttriType.AntiSleep] = "de_bingfeng",
        [BattleAttriType.AntiPetrify] = "de_shihua",
        [BattleAttriType.AntiChaos] = "de_hunluan",
        [BattleAttriType.AntiDrunk] = "de_jiuzui",
        [BattleAttriType.AntiSilence] = "de_chenmo",
        [BattleAttriType.AntiSevereInjuer] = "de_zhongshang",
        [BattleAttriType.MagicAttack] = "magic_attack",
        [BattleAttriType.ZhenShiShangHai] = "real_injury",
    },

    SkillQua = {
        [3] = "HuoBanPinZhiKuangLan",
        [4] = "HuoBanPinZhiKuangZi",
        [5] = "HuoBanPinZhiKuangCheng",
        [6] = "HuoBanPinZhiKuangHong",
    },

    ShengQiItemQua = {
        [0] = "ShengQiKuang",
        [1] = "ShengQiKuang",
        [2] = "ShengQiKuang",
        [3] = "ShengQiKuang",
        [4] = "ShengQiItemQua4",
        [5] = "ShengQiItemQua5",
        [6] = "ShengQiItemQua6",
    }
}

-- 圣器类型
SHENG_QI_TYPE = {
    COMMON = 0,     -- 通用类圣器
    ATTACK = 1,     -- 攻击类圣器
    HELP = 2,       -- 辅助类圣器
    SHOOT = 3,      -- 落宝类圣器
}

-- 圣器技能类型
SHENG_QI_SKILL_TYPE = {  
    INITIATIVE = 1, -- 主动技能
    TALENT = 2,     -- 天赋技能
}

-- 圣器属性类型
SHENG_QI_ATTRI_TYPE = {
    ACTIVENESS = 0, -- 活跃度
    ATTACK = 1,     -- 伤害能力
    SPEED = 2,      -- 速度
    REPLY = 3,      -- 回复
    HELP = 4,       -- 援助
    FALL = 5,       -- 落宝
    COPE_FALL = 6,  -- 抗落宝
    MAX = 6,
}

--- 圣器属性评分字段
SHENG_QI_ATTRI_SCORE_NAME = {
    [SHENG_QI_ATTRI_TYPE.ACTIVENESS] = "huoyue",
    [SHENG_QI_ATTRI_TYPE.ATTACK] = "damage",
    [SHENG_QI_ATTRI_TYPE.SPEED] = "speed",
    [SHENG_QI_ATTRI_TYPE.REPLY] = "huifu",
    [SHENG_QI_ATTRI_TYPE.HELP] = "yuanzhu",
    [SHENG_QI_ATTRI_TYPE.FALL] = "luobao",
    [SHENG_QI_ATTRI_TYPE.COPE_FALL] = "kangluobao",
}

-- 符石类型
FU_SHI_TYPE = {
    Common = 0,     -- 全部
    QingLong = 1,   -- 青龙
    BaiHu = 2,      -- 白虎
    ZhuQu = 3,      -- 朱雀
    XuanWU = 4,     -- 玄武
    QiLing = 5,     -- 麒麟
}


-- 圣器飘字哪些Reason 飘字
SHENGQI_NOT_NOTICE_REASON = {
    [ShengQiCfg.SendReason.GetIn] = true,
}
