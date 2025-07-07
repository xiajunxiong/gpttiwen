-- ==================== 天下第一比武大会配置 =====================
UTACfg = {
    IS_LOG = false,  -- log开关

    -- 活动状态
    ActStatus = {
        Nothing = 0,
        Ready = 1,      -- 准备
        Knockout = 2,   -- 淘汰赛
        Second = 3,     -- 晋级赛
        End = 4,        -- 结算
        Pause = 5,      -- 休赛
    },

    -- 比赛类型
    MatchType = {
        Out = 1,        -- 资格淘汰赛
        MainWin = 2,    -- 正赛晋级赛
        MainFail = 3,   -- 正赛败者组
    },

    -- 竞猜状态
    GuessStatus = {
        Bet = 0,        -- 可以下注
        StopBet = 1,    -- 停止下注
        ResultWin = 2,  -- 开奖结果：获胜
        ResultFail = 3, -- 开奖结果：失败
        Draw = 4,       -- 开奖平局
    },

    -- 竞猜请求
    GuessReqType = {
        GetInfo = 0,    -- //!< 获取单场竞猜的信息 param1: match_idx比赛序号
        Bet = 1,        -- //!< 竞猜下注 param1: match_idx比赛序号 param2: team_idx队伍序号 param3: 下注的金额
        GetRecord = 2,  -- //!< 获取自己下注过的比赛记录 返回：4970协议
        GetReward = 3,  -- //!< 获取竞猜奖励 //!< param1:[-1:一键领取 ,非-1为match_idx比赛序号 ] //!< param2:team_idx队伍序号[param1非-1时有用]
        GetProfit = 4,  -- 获取竞猜币累计收益
    },

    -- 晋级赛类型
    MAIN_OVERVIEW_TYPE = {
        TOP32 = 0,      --//!< 三十二强
        TOP16 = 1,      --//!< 十六强
        TOP8_TO_FINAL = 2,      --//!< 八强至总决赛
        FAIL_GROUP_1 = 3,       --//!< 败者组第一轮
        FAIL_GROUP_2 = 4,       --//!< 败者组第二轮
        FAIL_GROUP_3 = 5,       --//!< 败者组第三轮
        FAIL_GROUP_4 = 6,       --//!< 败者组第四轮
    },

    -- 晋升赛轮数
    MAIN_ROUND_TYPE = {
        TOP32 = 0,      --//!< 三十二强
        TOP16 = 1,      --//!< 十六强
        TOP8 = 2,       --//!< 八强
        TOP4 = 3,       --//!< 四强
        TOP2 = 4,       --//!< 决赛
    },

    -- 晋级赛类型转比赛类型和轮数
    MAIN_TYPE_CONVERT = {
        [0] = {
            match_type = 2,  --UTACfg.MatchType.MainWin,
            round_index = 0, --UTACfg.MAIN_ROUND_TYPE.TOP32,
        },
        [1] = {
            match_type = 2,     --UTACfg.MatchType.MainWin,
            round_index = 1,    --UTACfg.MAIN_ROUND_TYPE.TOP16,
        },
        [2] = {
            match_type = 2,     --UTACfg.MatchType.MainWin,
            round_index = 2,    --UTACfg.MAIN_ROUND_TYPE.TOP8,
        },
        [3] = {
            match_type = 3,     --UTACfg.MatchType.MainFail,
            round_index = 1,    --UTACfg.MAIN_ROUND_TYPE.TOP16,
        },
        [4] = {
            match_type = 3,     --UTACfg.MatchType.MainFail,
            round_index = 2,    --UTACfg.MAIN_ROUND_TYPE.TOP8,
        },
        [5] = {
            match_type = 3,     --UTACfg.MatchType.MainFail,
            round_index = 3,    --UTACfg.MAIN_ROUND_TYPE.TOP4,
        },
        [6] = {
            match_type = 3,     --UTACfg.MatchType.MainFail,
            round_index = 4,    --UTACfg.MAIN_ROUND_TYPE.TOP2,
        },
    },

    -- 比赛战斗结果 0:未打  1:攻方胜  2:守方胜  3:在打 4 平局
    FightResult = {
        Nothing = 0,
        AtkWin = 1,
        DefWin = 2,
        Fighting = 3,
        Draw = 4,
    },

    -- 职业图标
    ProfIcon = {
        [0] = "TongYong1";
        [1] = "QiGongShi1";
        [2] = "JianXiaKe1";
        [3] = "KuangZhanShi1";
        [4] = "TieWei1";
        [5] = "ShenSheShou1";
        [6] = "XingShuShi1";
        [7] = "YiShuShi1";
        [8] = "FuZhouShi1";
    },

    -- 冠军雕像位置 4-2-1-3-5 这样摆放
    StatuePos = {
        {
            seq=3,
            x=267.55,
            y=74.5,
            z=157.403,
            dir_y=-90,
            mod_size=1
        },
        {
            seq=2,
            x=267.55,
            y=74.5,
            z=162.81,
            dir_y=-90,
            mod_size=1
        },
        {
            seq=4,
            x=267.55,
            y=74.5,
            z=152,
            dir_y=-90,
            mod_size=1
        },
        {
            seq=1,
            x=267.55,
            y=74.5,
            z=168.1,
            dir_y=-90,
            mod_size=1
        },
        {
            seq=5,
            x=267.55,
            y=74.5,
            z=146.8,
            dir_y=-90,
            mod_size=1
        },
    },

    ModelShowCfg = {
        [1] = {
            reward_id = 429,
            reward_type = 1,
        },
        [2] = {
            reward_id = 50240,
            reward_type = 2,
        },
    },
}