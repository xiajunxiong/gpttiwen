
WenXiangLouConfig = {
    ResPath = "task/wxl/",
    
    MAX_NPC_NUM = 9,
    MAX_ITEM_NUM = 8,
    KEYS_PAGE_HEIGHT = 516,
    AUTO_SHOUT_TIME = 3,

    JUDGE_NPC = 59037,

    OperType = {
        talk = 0,           -- 交谈 p1:seq
        find_item = 1,      -- 查找道具 p1:item_seq
        identity = 2,       -- 指认 p1:seq
        fight = 3,          -- 发起结算战斗
        npc_info = 4,       -- 请求npc信息
        scene_info = 5,     -- 请求场景信息
    },

    LoadType = {
        none = -1,
        type0 = 0,       --初始 
        type1 = 1,       --查证
        type2 = 2,       --指认
        type3 = 3,       --试探
        type4 = 4,       --结束
    },

    InfoType = {
        check = 1,       --查看
        report = 2,     --举报
    }
}