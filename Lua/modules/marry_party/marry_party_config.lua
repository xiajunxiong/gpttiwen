
MarryPartyConfig = {
    ResPath = "marry_party/",
    OPPONENT_NUM = 2,

    FBReqType = {
        regist = 0,     -- 预约
        enter = 1,      -- 进入场景
        gather = 2,     -- 采集
        
        question_open = 3,          -- 打开答题界面
        question_answer = 4,        -- 答题 p1:选项
        question_close = 5,         -- 关闭答题界面

        match_open = 6,     -- 打开匹配界面
        match_close = 7,    -- 关闭匹配界面
        match = 8,          -- 开始匹配 p1:1开始匹配 0取消匹配

        bless = 9,      -- 祈愿
        battle_reward = 10, -- 领取战斗奖励 p1:次数
        order_info = 11,    -- 预告信息 4658下发
    },

    LinkType = {
        fight = 3,
    }
}