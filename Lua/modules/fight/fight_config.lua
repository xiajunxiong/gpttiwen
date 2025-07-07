
FightConfig = {
    ResPath = "fight/",
    PngPath = "fight/%s.png",

    CRYSTAL_FB_XI_YOU_MAX_LEVEL = 52,
    GATHER_ENERGY_REMIND_VAL = 1800,
    SINGLE_TOTAL_TIME = 60,
    SWEEP_NUM_MAX = 10,
    SWEEP_ITEM_NUM_MAX = 20,

    CRYSTAL_FB_ENERGY_PER = 20,     -- 水晶副本体力

    PET_GOD_FB_MAX_TYPE_NUM = 4,
    FIGHT_SOUL_GUIDE_LEVEL = 30,    -- 心灵之境指引等级

    FightTabType = {
        field = 1,
        dungeon = 2,
        gather = 3,
        irony = 7,
    },

    IconSpNameRole = "Di_RenWu",
    IconSpNamePet = "Di_ChongWu",
    TiLiZhi = "TiLiZhi",

    GatherSkillId = {
        [1] = 1000,
        [2] = 1001,
        [3] = 1002,
    },

    GatherPicNum = {
        [1] = 5,
        [2] = 4,
        [3] = 6,
    },

    GatherTitleSp = {
        [1] = "_LocWaJueJiangLi",
        [2] = "_LocFaMuJiangLi",
        [3] = "_LocCaiJiJiangLi",
    },

    CrystalObjType = {
        Monster = 1,
        Npc = 2,
        Boss = 3,
        Bonfire = 4,
        Coin = 5,
        Gather1 = 6,
        Gather2 = 7,
        Gather3 = 8,
        Box = 9,
        Door = 10,
    },

    CrystalMenuObjShow = {
        [1] = 3,
        [5] = 2,
        [9] = 1,
        [10] = 4,
    },

    CrystalIconsS = {
        [1] = "monster",
        [2] = "npc",
        [3] = "boss",
        [4] = "bonfire",
        [5] = "coin",
        [6] = "gather",
        [7] = "gather",
        [8] = "gather",
        [9] = "box",
        [10] = "door",
    },
    CrystalIconsC = {
        [1] = {"coin"},
        [2] = {"box"},
        [3] = {"npc"},
        [4] = {"monster"},
        [5] = {"boss"},
        [6] = {"bonfire"},
        [7] = {"door"},
    },
    CrystalIconsC2 = {
        [1] = {"npc"},
        [2] = {"boss"},
    },
    SplaceAnecdoteIconsC = {
        [1] = {"box"},
        [2] = {"npc"},
        [3] = {"monster"},
        [4] = {"boss"},
        [5] = {"bonfire"},
        [6] = {"door"},
        -- [7] = {"quest"},
        -- [8] = {"shop"},
    },
    SplaceAnecdoteIconsS = {
        [1] = "monster",
        [2] = "npc",
        [3] = "boss",
        [4] = "bonfire",
        [5] = "coin",
        [6] = "gather",
        [7] = "gather",
        [8] = "gather",
        [9] = "box",
        [10] = "door",
        [11] = "npc",
    },
    AdvancedIcons = {
        [0] = {"boss"},
        [1] = {"door"},
        [2] = {"npc"},
    },
    ConspiracyIcons = {
        [0] = {"boss"},
    },
    ArenaMatchIcons = {
        [1] = {"am_1"},
        [2] = {"am_0"},
        -- [3] = {"am_2"},
        -- [4] = {"am_3"},
        -- [5] = {"am_4"},
    },
    ShiTuFBIcons = {
        [1] = {"monster"},
    },
    HarmonyHeavenIcons = {
        [1] = {"boss"},
    },
    FantasyIconsC = {
        [1] = {"boss"},
        [2] = {"door"},
    },

    MapNameBgSp = {
        normal = "PuTongDi",
        special = "JingYingDi",
    },

    MapNameTypeSp = {
        [2] = "_LocJingYing",
        [3] = "_LocXiYou",
    },
    FightTipsState = {
        none = 1,
        challenge = 2,
        sweep = 3,
    },
    FightTipsSp = { "", "KeTiaoZhanDi", "KeSaoDangDi"},
    --属于副本的场景
    IS_IN_FIGHTFB = {
        [SceneModeType.FightCrystal] = true,
        [SceneModeType.AdvancedFB] = true,
        [SceneModeType.FightGather] = true,
        [SceneModeType.MedalFB] = true,
        [SceneModeType.FightFantasy] = true,
        [SceneModeType.ArenaMatch] = true,
        [SceneModeType.Conspiracy] = true,
        [SceneModeType.OnlyFight] = true,
        [SceneModeType.ShiTuFB] = true,
        [SceneModeType.WenXiangLou] = true,
        [SceneModeType.SplaceAnecdote] = true,
        [SceneModeType.SoulsAnecdote] = true,
        [SceneModeType.FightFly2] = true,
        [SceneModeType.HarmonyHeaven] = true,
    },
    --活动副本,不允许在活动副本里传送进其他副本或退出队伍的
    IS_IN_ACTFB = {
        [SceneModeType.LoversMatch] = true,
    },
    --属于慎重考虑退出副本的场景 提示用
    IS_IN_ONCE = {
        [SceneModeType.GuildMelee] = ActType.GuildMelee,
        [SceneModeType.OnlyFight] = ActType.OnlyFight,
        [SceneModeType.ChiefElelction] = ActType.ChiefElection,
        [SceneModeType.ChiefPrimary] = ActType.ChiefElection,
        [SceneModeType.ChiefPeakBattle] = ActType.ChiefPeakBattle,
        [SceneModeType.RushDungeon] = ActType.RushDungeon,
    },
    --不属于副本场景
    IS_IN_NONE = {
        [SceneModeType.None] = true,
    },
    --需要战斗继续寻路的副本
    IS_NEED_CONTINUE = {
        [SceneModeType.FightCrystal] = true,
        [SceneModeType.AdvancedFB] = true,
        [SceneModeType.MedalFB] = true,
    },
    --单人副本模块
    IS_SINGLE_MOD = {
        [Mod.Experience.EndlessTower] = true,
        [Mod.Experience.ChallengeTest] = true
    },

    FBTextDesc = {
        [1] = 86,
        [2] = 120,
        [3] = 121,
        [4] = 144,
        [5] = 104,
    },

    YMZBgs = {
        [1] = "BaiBeiJing",
        [2] = "HeiBeiJing",
    },
    
    YMZST2T = {
        [1] = 1,
        [2] = 2,
        [3] = 3,
        [4] = 4,
        [5] = 5,
    },

    YMZRewards = {
        reward_view_e = "rewards_e",
        reward_view_m = "rewards_m",
        reward_view_i = "rewards_i",
    },

    YMZRewardsIndex = {
        reward_view_e = 1,
        reward_view_m = 2,
        reward_view_i = 3,
    },

    YMZTypeBgs = {
        [1] = "HuangDi",
        [2] = "HuiDi"
    },

    YMZItemBgs = {
        [1] = {
            sp1 = "DengJiXuanZhongDi1",
            sp2 = "DengJiXuanZhongDi2",
        },
        [2] = {
            sp1 = "DengJiWeiXuanZhongDi1",
            sp2 = "DengJiWeiXuanZhongDi2",
        },
    }
}