
StoryConfig = {
    StoryInfoNone = 0,

    UIResPath = "story/",

    OptionReqType = {
		info = 0,       -- 下发信息
		record = 1,     -- 记录 param1记录值
	},

    State = {
        Loading = 1,
        Loaded = 2,
        Playing = 3,
        End = 4,
    },

    OptionsState = {
        Start = 1,
        Ready = 2,
        Record = 3,
    },

    --多参数在timeline里有,隔开
    CustomEventType = {
        Pause = "Pause",
        Stop = "Stop",
        ShowBoss = "ShowBoss",
        SetParent = "SetParent",
        Black = "Black",
        NpcDialog = "NpcDialog",        --p1:talk_id p2:自动关闭时间(毫秒,为空或为0表示不自动关闭) p3:是否暂停剧情，默认为暂停
        AudioEff = "AudioEff",          --p1:audioPath 以RunTimeRes p2:path是否要拼avatar_type(1表示要)
        XinMoPre = {
            Caption = "XinMoCaption",   --字幕trigger p1:languageKey--Language.Story.XinMoPre p2:持续时长(毫秒)
            GotoBattle = "XinMoGoBattle",   --继续主线进行心魔战斗
        },
        GameStart = "GameStart",
    },

    StoryType = {
        show_boss = 1,  -- boss展示 p1=monster_group_id
        task = 2,       -- 任务剧情 p1=task_id
        hh_show = 101,    -- 鸿蒙天界进入副本 p1=scene_id
    },

    TriggerType = {
        None = 0,           -- 默认 调用就播 不然就算调用了触发时机不对也不播
        BattleStart = 1,    -- 战斗一开始
        BattleTrans = 2,    -- 战斗中boss变身后,
        BattleStartNew = 3  -- 新战斗一开始
    },

    OriginBattleProf2Seq = {
        [1] = 22,
        [2] = 23,
        [3] = 24,
        [4] = 25,
        [5] = 26,
        [6] = 27,
        [7] = 28,
        [8] = 29,
    },
    ID = {
        XinmoPre = 108,     --第一次境界提升显示心魔剧情
    },


    SpecialStory = {
        [121] = true,
        [122] = true,
        [123] = true,
        [124] = true,
        [125] = true,
        [126] = true,
        [127] = true,
        [128] = true,
        [129] = true,
        [130] = true,
        [131] = true,
        [132] = true,
	[133] = true,
        [134] = true,
        [135] = true,
        [136] = true,
        [137] = true,
	[138] = true,
    },
    SpecialStoryM2I = {
        [1717] = 121,
        [1711] = 122,
        [1708] = 123,
        [1702] = 124,
        [1705] = 125,
        [1746] = 126,
        [1740] = 127,
        [1737] = 128,
        [1731] = 129,
        [1734] = 130,
        [1714] = 131,
        [1720] = 132,
        [1723] = 133,
        [1726] = 134,
        [1743] = 135,
        [1749] = 136,
        [1752] = 137,
        [1755] = 138,
    }
}