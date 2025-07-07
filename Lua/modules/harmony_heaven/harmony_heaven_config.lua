
HarmonyHeavenConfig = {
    ResPath = "harmony_heaven/",

    PngPath = "harmony_heaven/%s.png",

    MAX_HONGMENG_TIANJIE_BOSS_NUM = 3,
    MAX_LIMIT_SEQ_NUM = 10,
    MAX_FB_INDEX_NUM = 64,

    BOSS_NUM_MAX = 3,

    ReqType = {
        talk = 0,           -- 广播对话
        role_info = 1,      -- 请求个人信息
        send_reward = 2,    -- 请求副本奖励记录
        shop = 3,           -- 请求购买道具 p1:seq p2:num
        world_pass = 4,     -- 请求全服首通信息
        pass_team = 5,      -- 请求首通队伍 p1:fb_id
    }
}
