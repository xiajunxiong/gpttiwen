
IllusionConfig = {
    ResPath = "illusion/",

    PngPath = "illusion/%s.png",

    FASHION_TYPE_MAX = 4,

    FASHION_PER_ROW = 4,    -- 时装界面每页有几个

    OperType = {
        wear = 1,           -- p1:幻化id 为0时脱下装备 p2:要脱的部位
        exchange = 2,       -- p1:要购买的seq
	},

    FashionType = {
        cloth = 1,
        weapon = 2,
        wing = 3,
        mount = 4,

        partner = 101,
        pet = 102,
    },

    PageToType = {
        [1] = 1,
        [2] = 2,
        [3] = 4,
        [4] = 101,
        [5] = 102,
    },

    -- 时装升级请求
    IllusionUpReqType = {
        Info = 0,
        LvUp = 1,   --时装升级，p1：时装id
        SpLvUp = 2, --专精升级
    },
    IllusionUpStarEffect = {
        [1] = "7165457",
        [2] = "7165458",
        [3] = "7165456",
        [4] = "7165455",
    },
}