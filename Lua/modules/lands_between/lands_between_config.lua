GoldLaw = {
    OutLimit = 110,
    InLimit = 150,
    Operate = {
        [101] = 2,
        [102] = 2,
        [103] = 2,
        [104] = 2,
        [105] = 2,
        [106] = 2,
        [107] = 2,
        [108] = 2,
        [109] = 2,
        [201] = 1,
        [202] = 1,
        [203] = 1,
    },
    VisiLength = 5,
    Around = 1,
    ResAround = 2,

    -- 要注意数组的标头
    ApTemp = {
        [1] = 65909,      -- 小行动丹
        [2] = 65908,      -- 中行动丹
        [3] = 65910,      -- 大行动丹
    },

    ResPath = "actor/shenyu/%s.prefab",
    
    ActOpen = 2,
    ActWait = 1,

    MapColorShow = {
        [0] = {
            [0] = COLORS.Orange1, -- 占领
            [1] = COLORS.Red7, -- 测试颜色
            [2] = COLORS.Orange1, -- 测试颜色
            [3] = COLORS.Green7, -- 测试颜色
        },
        [1] = {
            [0] = COLORS.Blue8, -- 水占领
            [1] = COLORS.Red7, -- 火占领
            [2] = COLORS.Orange1, -- 风占领
            [3] = COLORS.Green7, -- 地占领
        }
    },
    
    CallBackType = {
        Open = 1,
        Close = 2,
    }
}

LandsBetweenAlterTag = {
    [-1] = {index =  -1},
    [1] = {index =  1},
    [2] = {index =  2},
    [3] = {index =  3},
    [4] = {index =  4},
    [5] = {index =  5},
    [6] = {index =  6},
    [7] = {index =  7},
    [8] = {index =  8},
    [9] = {index =  9},
}

LandsLimitClickView = {
    [1] = LandsBetweenMapView,       
    [2] = LandsBetweenAltarInfoView,       
}

ExtraMod = {
    [1] = {
        sort=1,
        id=1,
        name="主城",
        icon = 49002,
        mod=154004,
        label_type=0,
        is_team=1,
        is_guide=0,
    },
    [2] = {
        sort=1,
        id=1,
        name="地图",
        icon = 61008,
        mod=154005,
        label_type=0,
        is_team=1,
        is_guide=0,
    },
}

MapSimulate = {
    
}

MapResOutFloor = {
    [101] = 1,
    [102] = 1,
    [103] = 1,
    [104] = 1,
    [105] = 1,
    [106] = 1,
    [107] = 1,
    [108] = 1,
    [109] = 1,
    [302] = 1,
    [303] = 1,
    [304] = 1,
    [305] = 1,
    [306] = 1,
    [307] = 1,
    [308] = 1,
    [309] = 1,
    [310] = 1,
    [401] = 1,
    [411] = 1,
    [421] = 1,
    [431] = 1,
    [801] = 1,
    [802] = 1,
    [803] = 1,
    [804] = 1,
}

MapResPath = {
    [1] = "",
}

-- 设置为自定义的活动配置 act_seq
LandsBetweenExtralCustom = {
    [4] = true,
}

LandsBetweenConfig = {
    ModPath = "modules/lands_between/",
    PrefabPath = "lands_between/",
}

LANDS_BETWEEN_OPERATE_TYPE = {
    OP_OPEN_GOD_SPACE_MAIN_UI = 0,    -- 打开神域主界面（此时会下发玩家信息、地图信息、其他玩家位置等,并会持续收到地图上信息变化的协议下发）
    OP_CLOSE_GOD_SPACE_MAIN_UI = 1,   -- 关闭神域主界面（服务端不再主动同步地图上变化的信息）
    OP_OCCUPY_GRID = 2,               -- 占领格点 p1 行  p2 列  (行和列从0开始）
    OP_QUERY_ALTAR_DEFENDER_INFO = 3, -- 查询神坛防御者信息 p1 行  p2 列  (行和列从0开始）
    OP_PRAY = 4,                      -- 祈祷 p1 行  p2 列  p3 祈祷的伙伴ID p4 宠物index(没有就填-1)
    OP_BUY_AP = 5,                    -- 购买行动力 p1 是否使用元宝
    OP_ALL_REPORT_LIST = 6,           -- 请求全部战报
    OP_STOP_PRAYING = 7,              -- 停止祷告p1 行 p2 列
    OP_GOD_LEVEL_TASK_REWARD = 8,     -- 领取神位任务奖励 p1 task_id (配置中task_id
    OP_GIVEUP_ALTAR = 9,              -- 放弃神坛 p1 行 p2 列
    OP_FETCH_SEASON_GIFT = 10,        -- 领取赛季见面礼
    OP_QUERY_FACTION_USERS = 11,      -- 查询阵营玩家列表 p1 是否重新打开 (0则继续上次的数据下发，1则重新获取数据下发）
    OP_USE_AP_ITEM = 12,              -- 使用行动力物品 p1类型（0 小 1 中 2 大
    OP_DRAGON_VEIN_CALL_HELPER = 13,  -- 龙脉呼叫帮手
    OP_OPEN_DRAGON_VEIN_MAIN_UI = 14, -- 打开龙脉主界面（会下发一次5519）
    OP_READ_REPORT = 15,              -- 读战报 p1 战报时间戳
    OP_DRAGON_VEIN_MONSTER_FIGHT = 16,-- 打龙脉小怪 p1 第x个小怪[0,5)
    OP_DRAGON_VEIN_BOSS_FIGHT = 17,   -- 打龙脉boss怪 p1 第x个boss[0,3) p2 协战单位id
    OP_DELETE_ALL_REPORT = 18,        -- 一键删除所有战报
    OP_UNFETCH_ESSENCE_INFO = 19,     -- 查询未领取的精华奖励信息
    OP_FETCH_ESSENCE_REWARD = 20,     -- 领取精华奖励
    OP_DRAGON_VEIN_SHARE = 21,        -- 龙脉分享 p1 第x个boss[0,3)
    OP_FINISH_GUIDE_TASK = 22,        -- 完成神域指南任务 param1- 任务seq
    OP_RECEIVE_GUIDE_REWARD = 23,     -- 领取神域指南任务奖励，param1-任务seq
    OP_DRAGON_VEIN_INSPIRE = 24,      -- 龙脉鼓舞
    OP_FETCH_PHASE_REWARD = 25,       -- 领取阶段任务奖励 p1 外域:0  内域:1
    OP_RESET_HELPER_COND = 26,        -- 重置龙脉怪物 p1 index
    OP_USER_SHOP_BUY_ITEM = 27,       -- 神域商店购买物品 p1 购买第p1个商品[0,6)
    OP_USER_SHOP_REFRESH = 28,        -- 手动刷新神域商店
    OP_USER_SHOP_FETCH_DAILY = 29,    -- 领取商店每日奖励
    OP_QUERY_RANK = 30,				  -- 查询排行榜, param1-榜单类型(GOD_SPACE_RANK_TARGET_TYPE), param2-排行类型
    OP_BUFF_SHOP_BUY = 31,            -- 购买Buff商店 p1商品seq p2购买数量
    OP_FETCH_SEASON_PASS_FULI = 32,   -- 领取全民福利 p1 福利序号 从0开始
    OP_PALACE_RANK_QUERY = 33,        -- 查询殿堂排行榜 p1 赛季id, p2 从第几名开始查询
    OP_PALACE_FIRST_RANK_QUERY = 34,  -- 查询殿堂场景雕像信息
    OP_PALACE_WORSHIP = 35,           -- 殿堂膜拜

    OP_FETCH_EXPLORE_LV_REWARD = 36,   -- 领取神域探索等级奖励   p1 探索等级 -
    OP_EXPLORE_GATHER = 37,            -- 神域探索-采集 p1 采集类型（0：矿石 1：草药） p2 row  p3 column
    OP_FETCH_EXPLORE_TASK_REWARD = 38, -- 神域探索领取任务奖励 p1 任务类型(0:神域任务 1:阵营任务 2:个人任务) -
    OP_EXPLORE_BOSS_RANK_LIST = 39,    -- 神域探索BOSS排行榜请求 - + *
    OP_EXPLORE_WELL_INFO = 40,         -- 请求封妖井信息 p1 index[0,1] -
    OP_EXPLORE_WELL_SEAL = 41,         -- 封印封妖井 p1 index[0,1] p2 slot_idx[0,max_slot) p3 伙伴ID(没有则填0) p4 宠物index(没有则填-1) -
}    

LandsBetweenEvent = {
    BaseInfoChange = "BaseInfoChange",
}

-- 进入神域时，需要关闭的界面
LandsBetweenCloseViewList = {
    "LingBaoView",
    "LingBaoBagView",
}
