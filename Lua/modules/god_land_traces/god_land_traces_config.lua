
GodLandTracesConfig = {
    ResPath = "god_land_traces/",

    ITEM_EXCHANGE_MAX = 4,

    ReqType = {
        active = 0, -- param1 0~3(水火风地)
        fetch = 1,  -- 领取4个激活后的奖励
    },
    TipsAtlas = "god_land_traces",
    TipsIcons = {
        [1] = {
            n = "TiLiZhi",
            w = 30,
            h = 27,
        },
        [2] = {
            n = "HuoYue",
            w = 26,
            h = 34,
        },
        [3] = {
            n = "XiaYiZhi",
            w = 36,
            h = 36,
        },
    },

    GuideShow = {
        have_num = {1,1,2,1},
        need_num = {3,3,4,4}
    }
}