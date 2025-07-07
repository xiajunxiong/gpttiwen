
SocietyConfig = {
    ResPath = "society/",

    FRIEND_GROUP_TYPE_MAX = 7,
    FRIEND_GROUP_CUSTOM_MAX = 2,
    FRIEND_GROUP_NAME_NUM_MAX = 4,
    FRIEND_ADD_NUM_MAX = 6,

    CHAT_GROUP_MEMBER_MAX = 10,
    CHAT_GROUP_AVATAR_NUM = 3,
    CHAT_GROUP_NAME_NUM_MAX = 6,
    CHAT_GROUP_NOTICE_NUM_MAX = 20,
    CHAT_GROUP_NUM_MAX = 5,
    CHAT_GROUP_NUM_MAX_NOT_CROSS = 3,
    CHAT_GROUP_NUM_MAX_IS_CROSS = 20,
    
    CHAT_NUM_MAX = 20, 
    DECL_WORD_MAX = 30, 
    
    MASTER_APPRENTICE_MAX = 3,
    MASTER_COUNT_MAX = 2,
    PREFERENCE_NUM = 4,
    
    MAIL_ATTACHMENT_ITEM_NUM = 5,
    MAIL_MAX_NUM = 160,
    MAIL_MAX_DAY = 30,
    
    RANK_TYPE_NUM_MAX = 11,

    RANK_INTERVAL = 0,

    RANK_START = 0,
    RANK_END = 99,

    CHUANG_GONG_OPEN = 30,

    ACHIEVE_TIMES_MAX = 3,

    ChatGroupIconNone = "KongFangXingTouXiangDi",

    ChatGroupSettingKey = "SET_INFO_NOTICE_CHAT_GROUP_",

    FriendOperType = {
        info = 0,           --请求信息
        add_friend = 1,     --添加好友 p1=uid
        reply = 2,          --回复添加好友请求 p1=uid,p2=是否接受
        delete_friend = 3,  --删除好友 p1=uid
        add_black = 4,      --添加到黑名单 p1=uid
        remove_black = 5,   --解除黑名单屏蔽 p1=uid
        delete_group = 6,   --删除分组 p1=group_type
        ask_list = 7,       --添加好友请求列表
        add_recent_chat = 8,--添加最近联系人 p1=uid
        give_flower = 11,   --送花
        is_online = 12,     --检查是否在线 p1=uid
    },

    ShiTuOperType = {
        info = 0,       --获取师徒信息
        invite = 1,     --邀请师徒副本
        finish = 2,     --结束副本
    },

    SocietyType = {
        friend = 1,
        mail = 2,
        master_appearance = 3
    },

    ShiTuSeekingReqType = {
        info = 0,       --请求信息
        insert = 1,     --插入宣言
        remove = 2,     --删除宣言
        preference = 3  --师徒偏好设置
    },

    ShiTuSeekingType = {
        seek_apprentice = 1,
        seek_master = 0,
    },

    ShiTuChuanGongReqType = {
        game_ready = 0,     --就绪
        game_info = 1,      --游戏过程 p1:基础得分 p2:加成后得分
        game_end = 2,       --结算游戏(失效)
        fetch_mile = 3      --领取里程碑奖励 p1:目标uid p2:领取序号
    },

    ShiTuChuanGongInfoType = {
        game_begin = 0,      --游戏开始准备 p:开始时间
        game_score = 1,      --对方游戏得分 p:对方基础得分
    },


    FriendGroupType = {
        recent_contacts = 1,
        group = 2,
        friend = 3,
        black_list = 4,
        group_custom1 = 5,
        group_custom2 = 6,
        recent_team = 7,
    },

    FriendGroupToS = {
        [3] = 0,
        [5] = 1,
        [6] = 2,
    },

    FriendGroupToC= {
        [0] = 3,
        [1] = 5,
        [2] = 6,
    },

    FriendGroupFuncs = {
        [2] = {
            [1] = "AddFriends",
            [2] = "ApplyList"
        },
        [3] = {
            [1] = "AddFriends",
            [2] = "ApplyList"
        },
        [4] = {
            [1] = "Unblock",
        },
        [5] = {
            [1] = "DeleteGroup",
            [2] = "EditGroup"
        },
        [6] = {
            [1] = "DeleteGroup",
            [2] = "EditGroup"
        },
    },

    FriendInfoChangeType = {
        update = 0,
        remove = 1,
    },

    FriendFunctions = {
		[1] = { "ViewEquip", "TeamInvite", "Report", "AddBlackList" },
		[2] = { "ViewEquip", "TeamInvite", "Report", "AddBlackList" },
		[3] = { "ViewEquip", "TeamInvite", "DeleteFriend", "Report", "AddBlackList" },
		[4] = { "ViewEquip", "Report"},
		[5] = { "ViewEquip", "TeamInvite", "DeleteFriend", "RomveGroup", "Report", "AddBlackList" },
        [6] = { "ViewEquip", "TeamInvite", "DeleteFriend", "RomveGroup", "Report", "AddBlackList" },
        [7] = { "ViewEquip", "TeamInvite", "Report", "AddBlackList" },
    },

    RoleFunctions = { "ViewEquip", "Chat", "TeamInvite", "Report", "AddBlackList" },

    ChatGroupFuncs = {
		[1] = { "GroupManage", "GroupInvite", "GroupShare", "GroupDissmiss" },
        [2] = { "GroupInfo", "GroupInvite", "GroupShare" , "GroupExit" },
        [3] = { "ViewEquip", "TeamInvite", "GroupHandover", "GroupKickOut"},
        [4] = { "ViewEquip", "TeamInvite"},
	},

    NoGuildFunctions = { "ViewEquip", "Chat", "TeamInvite", "PK", "Report", "AddBlackList" },
    GuildMeleeFunctions = {"Chat", "TeamInvite", "Report", "AddBlackList",  },
    TeamSport = { "ViewEquip", "Chat", "Report" },
    MasterApprenticeFunctions = {
        [1] = { "ViewEquip", "TeamInvite", "Report", "AddBlackList" },
    },
    GuildMemberFunctios = {
        "ViewEquip","Chat","TeamInvite","ApplyPost","Accuse","KickOutG"
    },
    SwornMemberFunctions = {
        "ViewEquip", "Chat", "TeamInvite"
    },

    MasterApprenticeMax = {
        master = 2,
        apprentice = 3,
    },

    MailState = {
        [1] = "YouJianTuBiao_Kai",
        [2] = "YouJianTuBiao_WeiKai"
    },

    MailKind = {
        personal = 1,           -- 个人邮件
        system = 2,             -- 系统邮件
        guild = 3,              -- 家族邮件
        chongzhi = 4            -- 充值邮件
    },

    MailReason = {
        default = 0,            -- 默认原因
        gm_test = 1,            -- gm测试
        arena_match = 2,        -- 擂台战
    },

    MailVirtualItemType = {
        exp = 1,                -- 经验
        guild_cot = 2,          -- 公会贡献
        immortal_coin = 3,      -- 仙币
        gold = 4,               -- 元宝
        coin_bind = 5           -- 铜币
    },

    MailVirtualItemId = {
       [1] = CommonItemId.Exp,
       [2] = CommonItemId.GongXian,
       [3] = CommonItemId.Immortal,
       [4] = CommonItemId.Gold,
       [5] = CommonItemId.Coin,
       [6] = CommonItemId.ArenaPoint,
       [7] = CommonItemId.ArenaMatchPoint,
       [8] = CommonItemId.BattlefieldPoint,
       [9] = CommonItemId.LuckCoin,
       [10] = CommonItemId.BattlefieldPoint,
       [12] = CommonItemId.PartnerLevelItem,
       [13] = CommonItemId.ImmortalCoin,
       [14] = CommonItemId.CloudArena,
       [15] = CommonItemId.DustCoin,
       [16] = CommonItemId.CanHun,
       [18] = CommonItemId.GodSpaceExploits,
       [19] = CommonItemId.GodSpaceEssence,
    },

    OnlineSp = {
        [1] = "BiaoQian",
        [2] = "BiaoQian2",
    },

    PersonRankType = {
        Level = 0,              -- 全职业等级榜
        Level_1 = 1,            -- 气功师等级榜
        Level_2 = 2,            -- 剑客等级榜
        Level_3 = 3,            -- 狂战士等级榜
        Level_4 = 4,            -- 铁卫等级榜
        Level_5 = 5,            -- 神射手等级榜
        Level_6 = 6,            -- 星术师等级榜
        Level_7 = 7,            -- 医师等级榜
        Level_8 = 8,            -- 巫师等级榜

        Cap = 9,                -- 全职业战力榜
        Cap_1 = 10,             -- 气功师战力榜
        Cap_2 = 11,             -- 剑客战力榜
        Cap_3 = 12,             -- 狂战士战力榜
        Cap_4 = 13,             -- 铁卫战力榜
        Cap_5 = 14,             -- 神射手战力榜
        Cap_6 = 15,             -- 星术师战力榜
        Cap_7 = 16,             -- 医师战力榜
        Cap_8 = 17,             -- 巫师战力榜
    },
    PetRankType = {
        Cap = 0,                -- 宠物战力榜
    },

    RankTypeRoleToS = {
        [1] = 9,
        [2] = 0,
        [4] = 10,
        [5] = 11,
        [6] = 12,
        [7] = 13,
        [8] = 14,
        [9] = 15,
        [10] = 16,
        [11] = 17,
    },
    RankTypePetToS = {
        [3] = 0,
    },
    RankTypeRoleToC = {
        [0] = 2,
        [9] = 1,
        [10] = 4,
        [11] = 5,
        [12] = 6,
        [13] = 7,
        [14] = 8,
        [15] = 9,
        [16] = 10,
        [17] = 11,
    },
    RankTypePetToC = {
        [0] = 3
    },

    RankTitleType = {1, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1},

    RankSp = {
		[1] = "DiYiMing_2",
		[2] = "DiErMing_2",
		[3] = "DiSanMing_2",
		default = "PaiMingDi_4"
    },
    
    ServerSp = {
        all = 1,
        login = 2,
    },

    QuickShowType = {
        role = 1,
        group = 2,
        add = -1,
        all = -2,
        temp = -3,
    },

    ChatQuickFlyState = {
		start = 0,
		flying = 1,
		finish = 2,
    },
    
    ChatQuickCreateState = {
        state0 = 0,
        state1 = 1,
        state2 = 2,
    },

    ChatGroupReason = {
        add = 0,
        remove = 1,
        change = 2,
    },

    ShareType = {
        None = 0,
        GodLandTrace = 1,
        GiftFlowers = 2,
        LiveItem = 3,       -- 生活道具
    },

    GifeItemType = {
        normal = 0,
        flower = 1,
    },

    GiftReqType = {
        give = 0,           -- 赠予 p1=uid p2=item_id p3=num
        fetch = 1,          -- 领取 p1,p2=4052协议的uuid
        send_flower = 2,    -- 送花 p1=uid p2=item_id p3=num p4=is_auto_buy
        back_to_kiss = 3,   -- 回吻 p1=uid
    },

    GiftInfoType = {
        default = 0,
        update = 1,
        remove = 2,
    },

    GiftMsgType = {
        send = 1,
        fetch = 2,
        flower = 3,
    },

    IntimacyMsgType = {
        first_to = 1,
        first_from = 2,
    },

    GiftItemTypeShow = {
        default = {
            bg_height = 320,
            list_height = 335,
            show_flower100 = false,
            show_msg = true,
        },
        flower2 = {
            bg_height = 369,
            list_height = 384,
            show_flower100 = true,
            show_msg = false,
        }
    },

    GiftEffect = {
        [1] = {
            [1] = {5161016},
        },
        [2] = {
            [0] = {7161020},
            [1] = {7161021},
            [2] = {7161021, 7161008},
            [3] = {7161021, 7161012, 5161016},
        },
        [100] = {
            [1] = {30000000}
        },
    }
}