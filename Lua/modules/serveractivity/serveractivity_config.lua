ServeractivityConfig = {
    LabourDayPos = {0, -65, 145, 30, -145, 90},
    --专属礼包请求
    ExclusiveGiftReqType = {
        TYPE_INFO = 0, --请求信息
        TYPE_BUY_ITEM = 1, --购买前配置检测 p1:type
        TYPE_DAY_INFO = 2, --请求每日礼包信息
        TYPE_BUY_DAY_ITEM = 3 --购买每日礼包前配置检测 p1:phase p2:type
    },
    ExclusiveGiftType = {
        TYPE_DEFAULT = 1, --默认专属礼包
        TYPE_DAY_GIFT = 2 --每日专属礼包
    },
    NationalDayColor = {
        [0] = COLORS.Gray1,
        [1] = COLORS.Red21,
        [2] = COLORS.Green2,
        [3] = COLORS.Yellow14,
        [4] = COLORS.Purple4,
        [5] = COLORS.Purple3,
        [6] = COLORS.Blue6
    },
    PreciousLimitGiftEffect = {--对应礼包特效
        [1] = 6165065,
        [2] = 6165066,
        [3] = 6165067,
        [4] = 6165068
    },
    RegressReward = {
        ItemPos = {
            [1] = Vector3.New(494, 268, 0),
            [2] = Vector3.New(541, 182, 0),
            [3] = Vector3.New(556, 94, 0),
            [4] = Vector3.New(572, 7, 0),
            [5] = Vector3.New(557, -81, 0),
            [6] = Vector3.New(536, -169, 0),
            [7] = Vector3.New(478, -256, 0)
        },
        Color = {
            [1] = COLORSTR.Green9,
            [2] = COLORSTR.Green9,
            [3] = COLORSTR.Red20
        }
    },
    BackRewardReqType = {
        TYPE_INFO = 0, --0 请求协议
        TYPE_FETCH_DAY = 1, --1 领取登录奖励 param1: day_index [1 - 7]
        TYPE_FETCH_TASK = 2 --2 领取任务奖励 param1: task_index [1 - 64]
    },
    CompetitionRank = {
        Type = {
            -- 0=角色评分比拼
            -- 1=宠物评分比拼
            -- 2=伙伴评分比拼
            -- 3=装备评分比拼
            -- 4=竞技场比拼
            -- 5=锢魔挑战比拼
            Role = 0,
            Pet = 1,
            Partner = 2,
            Equip = 3,
            ArenaRank = 4, --竞技场排行榜
            CourageChallengeRank = 5, ---锢魔之塔请
            Comprehensive = 6,--综合评分
        },
        SendRank = {
            [0] = 32,
            [1] = 35,
            [2] = 33,
            [3] = 34
        },
        RankNum = 9, --请求排行榜数量
        ArenaRankRankNum = 10,
        OffectNum = 60,--68,
        OpenPanel = {
            [0] = 1001,
            [1] = 2001,
            [2] = 3001,
            [3] = 4001,
            [4] = 38001,
            [5] = 30002,
            [6] = 109001,
        }
    },
    SignHappy = {
        UpItemPos = {
            [1] = Vector2.New(84.3, -236),
            [2] = Vector2.New(172, -169),
            [3] = Vector2.New(257.5, -236),
            [4] = Vector2.New(258.9, -102.2),
            [5] = Vector2.New(345, -169),
            [6] = Vector2.New(431.6, -236),
            [7] = Vector2.New(346.1, -38),
            [8] = Vector2.New(432.7, -103),
            [9] = Vector2.New(519.1, -169),
            [10] = Vector2.New(605.5, -236)
        }
    },
    ActRewerdTip = {
        ItemPos = {
            [1] = Vector3.New(-344, -16, 0),
            [2] = Vector3.New(303, -14, 0),
            [3] = Vector3.New(-190, 52, 0),
            [4] = Vector3.New(146, 52, 0),
            [5] = Vector3.New(-20, 75, 0)
        },
        ItemPos2 = {
            [1] = Vector3.New(250, 10, 0),
            [2] = Vector3.New(85, 60, 0),
            [3] = Vector3.New(-85, 60, 0),
            [4] = Vector3.New(-250, 10, 0)
        },
        ItemAngles2 = {
            [1] = Vector3.New(0, 0, -25),
            [2] = Vector3.New(0, 0, -10),
            [3] = Vector3.New(0, 0, 10),
            [4] = Vector3.New(0, 0, 25)
        },
        ItemAngles = {
            [1] = Vector3.New(0, 0, 30),
            [2] = Vector3.New(0, 0, -30),
            [3] = Vector3.New(0, 0, 15),
            [4] = Vector3.New(0, 0, -15),
            [5] = Vector3.New(0, 0, 0)
        },
        RewardTypeToIcon = {
            [1] = 1,
            [2] = 1,
            [3] = 2,
            [4] = 3
        },
        RewardType = {--奖励提示类型
            Success = 1, --通用奖励展示
            Fail = 2,
            ChallengeSuccess = 3,
            ChallengeFail = 4
        }
    },
    YunZeTerritory = {
        ItemPos = {
            {
                [1] = {pos = Vector3.New(-46, 171, 0)},
                [2] = {pos = Vector3.New(-127, 107, 0), scale = Vector3.New(1.5, 1.5, 0)},
                [3] = {pos = Vector3.New(-100, 48, 0)},
                [4] = {pos = Vector3.New(-41, -12, 0)},
                [5] = {pos = Vector3.New(122, 9, 0)},
                [6] = {pos = Vector3.New(13, -120, 0)},
                [7] = {pos = Vector3.New(-127, -201, 0)},
            },
            {
                [1] = {pos = Vector3.New(37, 162, 0)},
                [2] = {pos = Vector3.New(-103, 81, 0)},
                [3] = {pos = Vector3.New(-75, 36, 0)},
                [4] = {pos = Vector3.New(-25, 5, 0)},
                [5] = {pos = Vector3.New(64, 23, 0)},
                [6] = {pos = Vector3.New(109, -46, 0)},
                [7] = {pos = Vector3.New(-106, -145, 0)},
            },
            {
                [1] = {pos = Vector3.New(26, 193, 0)},
                [2] = {pos = Vector3.New(-119, 129, 0)},
                [3] = {pos = Vector3.New(-24, 92, 0)},
                [4] = {pos = Vector3.New(89, 95, 0)},
                [5] = {pos = Vector3.New(137, -13, 0)},
                [6] = {pos = Vector3.New(17, -109, 0), scale = Vector3.New(1.8, 1.8, 0)},
                [7] = {pos = Vector3.New(102, -142, 0), scale = Vector3.New(1.8, 1.8, 0)},
            },
        }
    },
    SaleShop = {
        TopPos = {
            [1] = {
                [1] = Vector3.New(-231, 217, 0),
                [2] = Vector3.New(23, 190, 0)
            },
            [2] = {
                [1] = Vector3.New(-267, 217, 0),
                [2] = Vector3.New(-6, 190, 0)
            },
            [3] = {
                [1] = Vector3.New(-279, 217, 0),
                [2] = Vector3.New(5, 190, 0)
            },            
        },
        Outline = {
            [1] = COLORS.Yellow7,
            [2] = COLORS.Yellow18,
            [3] = COLORS.Yellow20
        },
        Gradient = {
            [1] = {
                [1] = COLORS.Red16,
                [2] = COLORS.Red14
            },
            [2] = {
                [1] = COLORS.White,
                [2] = COLORS.Yellow12
            },
            [3] = {
                [1] = COLORS.White,
                [2] = COLORS.Yellow12
            },         
        }
    }
}