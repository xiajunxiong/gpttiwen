
MarryConfig = {
    ResPath = "marry/",
    GameImgPngPath = "marry/game%s.png",

    BlessMoneyType = {
        [0] = CommonItemId.Gold,
        [1] = CommonItemId.Immortal,
        [2] = CommonItemId.Coin,
    },

    FBSyncType = {
        bless_shop = 0,     --购买祝福商品通知 p:商品index
        heat = 1,           --热度通知 p:当前热度
    },

    FBReqType = {
        common = 0,         -- 副本公共操作 p1:操作类型 p2:操作参数
        regist = 1,         -- 婚宴-预约 p1:预约时间戳
        regist_off = 2,     -- 婚宴-解除预约 p1:婚宴id
        invite = 3,         -- 婚宴-邀请 p1:目标uid
        apply = 4,          -- 婚宴-申请 p1:婚宴id
        mark = 5,           -- 婚宴-申请处理 p1:目标uid p2:0拒绝1同意

        regist_list = 6,            -- 婚宴-发送预约列表(并订阅)
        sub_regist_list = 7,        -- 婚宴-预约列表订阅 p1:1订阅0取消订阅
        inivite_list = 8,           -- 婚宴-发送邀请列表

        other_info = 9,         --婚宴-其他信息
        invite_size_buy = 10,   --婚宴-扩展邀请上限
        xun_you = 11,           --预定巡游 p1:巡游类型

        wedding_show_info = 12, --婚宴-展示信息 p1:婚宴id
        wedding_open_info = 13, --婚宴-开启信息

        sub_act = 100,           --副本-小游戏操作 p1:小游戏类型 p2:操作参数

    },

    -- ps:配置意义|场景参数|操作参数
    FBCommonOperType = {
        enter_scene = 0,    -- 进入场景         可进入      进入次数        无效 用enterfb
        candies = 1,        -- 发喜糖           可领取      领取次数        无
        rite = 2,           -- 拜堂动画         播放        已播放          场景参数赋值
        meal = 3,           -- 用餐             可领取      领取次数        无
        bless = 4,          -- 祝福             可购买      无              商品index
        barrage = 5,        -- 弹幕             可发送      发出次数        无效 用chat
        heat_reward = 6,    -- 热度奖励         无配置      领取标志(位)    奖励序号
    },

    WeddingChangeType = {
        regist_add = 0,             -- 预约列表 新增 p1:已被预约时间
        regist_remove = 1,          -- 预约列表 移除 p1:已被预约时间
        role_regist_change = 2,     -- 本人预约时间 p1:预约时间
        apply_remove = 3,           -- 申请列表 移除 申请者uid
    },

    FBEventType = {
        none = 0,           -- 无
        broadcast = 1,      -- 宣传阶段 副本创建
        warm = 2,           -- 预热阶段 无
        rite = 3,           -- 拜堂阶段 无
        meal = 4,           -- 用餐阶段 无
        act_xqma = 5,       -- 活动 寻情觅爱
        act_xxxy = 6,       -- 活动 心心相印
        act_bwxq = 7,       -- 活动 保卫绣球
        act_zsgz = 8,       -- 活动 枣生桂子
    },

    FBParseType = {
        broadcast = 1,      -- 宣传阶段 副本创建
        warm = 2,           -- 预热阶段 无
        rise = 3,           -- 拜堂阶段 无
        meal = 4,           -- 用餐阶段 无
        sub_act = 5,        -- 活动阶段 p1   
    },

    FBSubActType = {
        xun_qing_mi_ai = 1,     -- 寻情觅爱
        xin_xin_xiang_yin = 2,  -- 心心相印
        bao_wei_xiu_qiu = 3,    -- 保卫绣球
        zao_sheng_gui_zi = 4,   -- 枣生桂子
    },

    PROPOSE_TYPE = {
        INVITE = 1, --收到邀请
        REPLY = 2,  --收到回应
        WEDDING = 3,--收到婚宴
    },

    REQUEST_TYPE = {
        INVITE = 0,               -- 求婚（param1 = 目标uid，param2 = 求婚类型(1 or 2)）
        INVITE_REPLY = 1,         -- 求婚回应（param1 = 目标uid，param2 = 是否接受 0 or 1）
        GIFT = 2,                 -- 情缘礼物 赠送对方礼物 param1 礼物类型 1 or 2
        GIFT_FETCH = 3,           -- 情缘礼物 领取礼物首次奖励
        GIFT_FETCH_DAY = 4,       -- 情缘礼物 领取礼物每日奖励
        SKILL_NOT_FLAG = 5,       -- 情缘技能设置（发二进制 flag[32-index] = 1 哪个index为1 技能不生效，反之生效）
        DIVORCE = 6,		      -- 双方协议离婚
        DIVORCE_RESPOND = 7,      -- 离婚回应（param1 = 是否接受离婚 0 or 1）
        SINGLE_DIVORCE = 8,       -- 单方面离婚
        FORCE_DIVORCE = 9,	      -- 强制离婚
        CANCEL_FORCE_DIVORCE = 10,-- 取消强制离婚
    },
    FASHION_BODY_ID = 17, --结婚时装的fashion_body_id

    BWXQPatrolState = {
        stop = -2,
        ready = -1,
        patrol_bt1 = 0,
        patrol_1td = 1,
        patrol_dt1 = 2,
        patrol_1tb = 3,
        max = 4,
    },

    InviteType = {
        friend = 1,
        family = 2,
        apply = 3,
    },

    HasWeddingType = {
        none = -1,  --空
        no = 0,     --未预约
        now = 1,    --已预约
        done = 2,   --已举办
    },

    WeddingShowType = {
        none = 0,       -- 无
        apply = 1,      -- 已申请
        invited = 2,    -- 已被邀请
    },

    WeddingStartImg = {
        [5] = "_LocXunQingMiAi",
        [6] = "_LocXinXinXiangYin",
        [7] = "_LocBaoWeiXiuQiu",
        [8] = "_LocZaoShengGuiZi",
    }
}