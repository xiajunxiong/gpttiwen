
ChatConfig = {
    ResPath = "chat/",

    CHAT_ROLE_NUM_MAX = 20,
    CHAT_MSG_ITEM_INFO_NUM_MAX = 3,

    VOICE_SENSITIVITY = 100,
    VOICE_READY_TIME = 0.5,
    VOICE_MIN_INTERVAL = 2,
    RECORD_NUM_MAX = 8,
    SIMPLE_RECORD_NUM_MAX = 5,
    COPY_READY_TIME = 0.7,

    CHAT_CHACHE_WORD_NUM = 18,
    CHAT_CHECK_CHACHE_NUM_MAX = 5,
    CHAT_BLOCK_CHACHE_NUM_MAX = 10,

    ITEM_WORD_NUM = 5,

    IS_BLOCK = false,

    MsgType = {
        normal = 0,
        recruit = 1,
        bullet = 2,
        trumpet = 3,

        group_notice = 100,
        temp_group_notice = 101,
        intimacy_add_notice =  102,
    },

    CommentType = {
        COMMENT_TYPE_BATTLE = 0,			-- 战斗
        COMMENT_TYPE_ACT_RACE_HORSE = 1,	-- 跑马
        COMMENT_TYPE_WEDDING_FB = 2,        --  婚宴副本
    },

    ItemTrumpet = {
        is_cross = 26059,
        not_cross = 26060,
    },

    Trumpet2SpeakerType = {
        [26060] = 0,
        [26059] = 1,
    },

    EMOJ_NUM_MAX = 20,
    EMOJD_NUM_MAX = 20,

    ChatContentType = {
        text = 0,
        audio = 1,
    },

    ChannelId = {
        world = 1,
        scene = 2,
        guild = 3,
        team = 4,
        recruit = 5,
        prof = 6,
        system = 7,
        trumpet = 8,
        general = 9,
        group = 10,
        landsbetween = 11,

        single = 100,
        bullet = 200,
    },

    ChannelType = {
        world = 0,
        guild = 3,
        team = 2,
        group = 5,
        general = 6,
        landsbetween = 7,
    },

    ChannelIdToType = {
        [1] = 0,
        [2] = 1,
        [3] = 3,
        [4] = 2,
        [6] = 4,
        [9] = 6,
        [10] = 5,
        [11] = 7,
    },

    ChannelTypeToId = {
        [0] = 1,
        [1] = 2,
        [3] = 3,
        [2] = 4,
        [4] = 6,
        [5] = 10,
        [6] = 9,
        [7] = 11,
    },

    EmojSize = {
        w = 90,
        h = 70,
        dw = 110,
        dh = 80,
    },

    TeamChannelReason = {
        normal = 0,             -- 正常情况(整个列表下发)
        change = 1,             -- 信息改变
        add = 2,                -- 新增队伍
        remove = 3              -- 从列表中去除队伍
    },

    ChatItemType = {
        Item = 0,
        Pet = 1,
        Equip = 2,
        Jewelry = 3,
        Medal = 4,
        Emoj = 5,
        ShengQi = 6,
    },

    ChatQuickType = {
        emoj = 1,
        task = 2,
        chat = 3,
        item = 4,
        pet = 5,
        title = 6,
        sheng_qi = 7,
    },

    BubbleDeco = {
        [1] = "lt",
        [2] = "ld",
        [3] = "rd",
        [4] = "rt",
    },

    EmojDPicNum = {
        [1] = 6,
        [2] = 4,
        [3] = 11,
        [4] = 5,
        [5] = 3,
        [6] = 20,
        [7] = 6,
        [8] = 2,
        [9] = 33,
        [10] = 8,
        [11] = 10,
        [12] = 10,
        [13] = 6,
        [14] = 15,
        [15] = 2,
        [16] = 35,
        [17] = 131,
        [18] = 15,
        [19] = 12,
        [20] = 12,
    },
    EmojDPicSpeed = {
        [1] = 0.07,
        [2] = 0.13,
        [3] = 0.05,
        [4] = 0.07,
        [5] = 0.12,
        [6] = 0.04,
        [7] = 0.05,
        [8] = 0.1,
        [9] = 0.05,
        [10] = 0.06,
        [11] = 0.1,
        [12] = 0.1,
        [13] = 0.04,
        [14] = 0.08,
        [15] = 0.1,
        [16] = 0.07,
        [17] = 0.02,
        [18] = 0.08,
        [19] = 0.1,
        [20] = 0.07,
    },
    EmojDPicWidth = {
        [3] = 130,
        [6] = 130,
        [11] = 140,
        [12] = 130,
        [13] = 110,
        [14] = 150,
        [16] = 130,
        [17] = 130,
    },
    EmojDPicHeight = {
        [13] = 90,
    },

    TailTag = "tail_tag_%s",
    BubbleTag = "bubble_%s",
    BubbleDecoTag = "bubble_%s_%s",
    HideQuestionView = {
        [1] = Mod.CloudArena.Main,
        [2] = Mod.DeepDarkFantasy.Main,
        [3] = Mod.GodAreasActs.RuinsScene,
        [4] = Mod.LandsBetween.Main,
    },

    ItemHeights = {
        AvatarBase = 35,        --基础头像高，要显示头像的sub都要用自己的高加上这个高
        Pet = 161,          --宠物信息
        HongBao = 120,      --红包
        HongBaoGet = 26,    --红包领取信息
    },

    FORBID_TALK_TYPE = {
        FORBID_TALK_TYPE_DEFAULT = 0,   --默认(普通禁言模式，有飘字提示)
        FORBIT_TALK_TYPE_SILENCE = 1,   --静默禁言模式(仅自己可见聊天内容，无提示)
    },
}