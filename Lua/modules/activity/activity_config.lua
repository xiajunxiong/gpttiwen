-- 日常活动开启配置 Config.daily_activity_auto.daily
ActType = {
	WorldBoss = 1,									--魔将入侵
	WorldBoss2 = 10,								--巨龙来袭
	TeamSport = 2,									--小队竞技
    MzeRun = 3,										--迷宫竞速
    RushDungeon = 4,                                --勇闯地宫
	GuildMelee = 5,									--家族大乱斗
	ChiefElection = 6,                              --首席竞选
	ChiefPeakBattle = 7,                            --首席巅峰战
	GuildEmbattle = 8,								--家族荣誉战
	GuildAnswer = 9,								--家族答题
	OnlyFight = 11,									--以一敌百
	Battlefield = 12,							    --幻界战场
    TombRaider = 13,                                --遗迹夺宝
    GhostNight = 14,                                --百鬼夜行
    TradeRoad = 15,                                 --丝绸之路
    HeroVanBattle = 16,                             --英雄会武
    MarryParty = 17,                                --情缘盛会
    WorldBoss3 = 18,                                --星官降临
    FightSoul = 19,                                 --心灵之境
	
	TreasureMap = 10001,							--宝图
	GuildTask = 10002,								--门派任务
	FightDen = 10004,								--讨伐魔窟
	WSTQuesten = 10005,								--万事通
	TreviFountain = 10006,							--许愿池
	CourageChalleng = 10007,                        --锢魔之塔
	Escort = 10008,									--运镖
	ResearchTask = 10009,							--三界除魔
	Conspiracy = 10010,                             --试炼之地
	TaskChain = 10011,								--任务链
	ChallengeFb = 10014,                            --挑战副本
	ChallengeTask = 10015,                          --挑战任务
	Arena = 10016,									--古神宝库
	LoveDaily = 10017,                              --情缘日常
	ArenaMatch = 10018,                          	--擂台战
	GuildWeekTask = 10019,							--家族周任务
	BountyTask = 10020,								--赏金任务
	SilkRoad = 10021,                               --远洋贸易
	FantasyFB = 10022,	                            --四象幻境
	Patrol = 10023,	                            	--巡逻
	GuidlTrial = 10024,								--家族试炼
	ArenaAgain = 10025,							    --竞技场
	BattlefieldAgain = 10026,						--幻界战场
	FightFieldBoss = 10027,                         --野外首领
	-- RushDungeon = 10028,                            --勇闯地宫
    DreamNotes = 10030,                             --梦渊笔录玩法
    Strength = 10031,                               --体力
    CloudArena = 10032,                             --跨服竞技（穹顶之争）
    ChallengeTest = 10033,                          --轮回忘川
    CloudArenaDaily = 10034,                        --跨服竞技(日常界面入口)
    FightElite = 10035,                             --精英试炼
    DeepDarkFantasy = 10036,                        --幽冥炼狱
    ShxyBoss = 10037,                               --山海降妖录
    Prestige = 10039,                               --声望任务
	BigDipper = 10040,                              --北斗七星
    AllSoulsFB = 10041,                             --万灵话本
    TreasureTask = 10042,                           --九州秘宝
    FlyUpDaily= 10043,                              --飞升日常
    Hint= 10044,                                    --万灵大事件
    WorldArena = 10045,                             --论剑九州
    FallenGod = 10046,                              --殒神之地
    UTArena = 10047,                                --天下第一
    HeroVanBattleAgain = 10048,                     --英雄会武(额外)
    UTArena2 = 10049,                               --天下第一（额外）
    HarmonyHeaven = 10050,                          --鸿蒙天界

    Ruins = 10052,
    Animals = 10053,
    Palaces = 10054,
    DragonPluse = 10055,
    Probe = 10056,
}
-- 活动状态
ActStatusType = {
	Close = 0,										-- 活动关闭状态
	Standy = 1,										-- 活动准备状态
	Open = 2,										-- 活动进行中
}

ActivityConfig = {
    ResPath = "activity/",

    ACTIVITY_ROOM_MAX = 8,

    ActivityGroupType = {
        daily = 1,
        time_limit = 2,
        challenge = 3,
        future = 4,
        match = 5,
    },
    ActivityGroupKey = {
        [1] = "is_in_daily",
        [2] = "is_in_time_limit",
        [3] = "is_in_challenge",
        [4] = "is_in_future",
    },
    
    ActivityActType = {
        bg_sp = {
            [1] = "XinXi_Di2",
            [2] = "XinXi_Di3",
            [3] = "XinXi_Di4",
            [4] = "XinXi_Di5",
            [5] = "XinXi_Di6",
        },
        label = {
            [1] = COLORS.Yellow21,
            [2] = COLORS.Green8,
            [3] = COLORS.Yellow1,
            [4] = COLORS.Yellow13,
            [5] = COLORS.Yellow1,
        },
        outline = {
            [1] = COLORS.Yellow14,
            [2] = COLORS.Green11,
            [3] = COLORS.Red14,
            [4] = COLORS.Red14,
            [5] = COLORS.Red14,
        },
    },

    ActiveActList = {
        [ActType.CourageChalleng] = true,
        [ActType.ArenaAgain] = true,
        [ActType.SilkRoad] = true,
    },

    ColorfulIllustrateColor = {
        [0] = {[0] = _NC(231,74,63),_NC(250,89,71),_NC(252,114,96)},
        [1] = {[0] = _NC(225,105,49),_NC(242,122,63),_NC(252,166,106)},
        [2] = {[0] = _NC(252,196,66),_NC(252,215,112),_NC(255,242,186)},
        [3] = {[0] = _NC(126,177,60),_NC(176,202,120),_NC(201,218,153)},
        [4] = {[0] = _NC(87,210,206),_NC(107,230,225),_NC(156,247,247)},
        [5] = {[0] = _NC(82,165,222),_NC(101,178,231),_NC(137,206,255)},
        [6] = {[0] = _NC(165,94,222),_NC(184,118,239),_NC(211,162,252)},
    },

    LabelName = {
        [1] = "TuiJianDi_1",
        [2] = "TuiJianDi_2",
        [3] = "TuiJianDi_1",
        [4] = "TuiJianDi_2",
        [5] = "TuiJianDi_3",
        [6] = "TuiJianDi_4",
    },
    LabelOtherName = "TuiJianDi_3",

    NO_SHOW_TIP = {
        [ActType.GuildMelee] = 1,--家族大乱斗
        [ActType.ChiefElection] = 1,--首席竞选
        [ActType.OnlyFight] = 1,--以一敌百
    },
    TIME_LIMIT_REWARD_GET_OP = {
        JOIN_AWARD = 0,
        SURVIVAL_AWARD = 1,
    },
}