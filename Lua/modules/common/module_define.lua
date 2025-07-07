Mod = Mod or {}

Mod = {
    Role = {
        RoleAttr = 1001,        --角色属性
        RoleSkill = 1002,       --角色技能
        RoleSetting = 1003,     --角色设置
        RolePrestige = 1004,    --角色声望
        RoleHeartSkill = 1005,  --角色心法
        TabUniqueSkill = 1006,  --角色绝招
        RoleAddPoint = 1007,    --角色加点
    },
    Pet = {
        PetAttr = 2001,         --宠物属性
        PetBook = 2003,         --宠物图鉴
        PetGift = 2004,         --宠物天赋
        PetStrChange = 2005,    --宠物强化
        PetExp = 2006,          --宠物经验药
        PetGetBook = 2007,      --宠物打书（当前宠物技能标签）
        PetXilian = 2008,       --宠物洗炼
        PetConvert = 2009,      --宠物神兽兑换
        PetAddPoint = 2010,     --宠物加点
        PetStorage = 2011,      --宠物仓库
        PetWeapon = 2012,       --宠物魂器
        PetStamp = 2013,        --宠物神印
        PetPray = 2014,         --宠物祝福
        PetFeiSheng = 2015,     --宠物飞升
        PetGaiMing = 2016,      --宠物改命
    },
    Partner = {
        PartnerAttr = 3001,     --伙伴属性
        PartnerUp = 3002,       --伙伴升级
        PartnerQuality = 3003,  --伙伴品质
        PartnerLife = 3004,     --伙伴命格
        PartnerGift = 3005,     --伙伴天赋
        PartnerContract = 3006, --伙伴契约
        PartnerStrengthen = 3007,--伙伴强化

        PartnerLock = 3010,     --伙伴解锁
        PartnerFuyu = 3011,     --伙伴符玉
        FuyuChange = 3012,      --符玉转换
    },
    Workshop = {
        Forging = 4001,         --工坊锻造
        Jewelry = 4002,         --工坊灵饰
        -- Upgrade = 4003,      --工坊强化（已修改到装备系统上）
        -- Smelt = 4004,        --工坊熔炼（已修改到装备系统上）
        -- Inser = 4005,        --工坊镶嵌（已修改到装备系统上）
        NeoCompose = 4006,      --工坊组合
        -- Neveal = 4007,          --工坊点化
        BlazeStar = 4008,       --工坊铸星
    },
    Guild = {
        GuildInfo = 5001,            --家族信息
        -- GuildMission = 5002,      --家族任务
        GuildSkill = 5002,           --家族技能
        GuildMember = 5003,          --家族成员
        GuildShop = 5004,            --家族商店
        GuildPet = 5005,             --家族训宠
        GuildJiYuan = 5006,          --家族集愿
        GuildRedbag = 5007,          --家族红包
        GuildHof = 5008,             --家族名人堂
        GuildBuilding = 5009,        --家族内政建设
        GuildCooperateTask = 5010,   --家族每日任务
    },
    Answer = {
        AnswerView = 6001,      -- 答题界面
    },
    Professtion = {
        Select = 7001,          -- 选择职业
    },
    CloudArena = {
        Main = 8001,            --跨服竞技（穹顶之争）
    },
    Conspiracy = {
        Main = 9001,            --密医的阴谋进度
    },
    Bag = {
        Main = 10001,           --背包
    },
    Team = {
        Main = 11001,           --组队,
        Approve = 11001,        --审批,
    },
    FlyUp = {
        Main = 12001,           --飞升
    },
    Medal = {
        Aterlier = 14001,           --勋章分解
        Build = 14002,              --勋章抽奖
    },
    SuperMap = {
        Main = 15001,           --高级宝图
        Compass = 15002,        --罗盘
    },
    GuildTask = {
        Main = 16001,           --门派任务
    },
    Reputation = {
        Main = 17001,           --声望商店
    },
    Mall = {
        Trade = 18001,          --交易所
        Shop = 18002,           --商会
        Market = 18003,         --市场
        Exchange = 18004,       --市场兑换
    },
    Fight = {
        Field = 19001,          --野外
        Dungeon = 19002,        --副本
        Gather = 19003,         --采集
        Exp = 19004,            --经验
        Team = 19005,           --副本组队
        PatrolTask = 19010,     --维护治安
        Soul = 19011,           --心灵之境
    },
    TeamSport = {
        Main = 20001,
    },
    Activity = {                --活跃
        Daily = 21001,          --活跃日常
        TimeLimit = 21002,      --活跃限时
        ActivityBack = 21003,   --活跃找回
        WeekReward = 21004,     --活跃周赏
        ActiveRoller = 21005,   --活跃转盘
    },
    Love = {
        Marry = 22001,         
        Puzzle = 22002,        --心心相应小游戏
        Answer = 22003,        --枣生桂子小游戏
    },
    Reward = {                  --奖励 奖励福利主界面图标开启id 45001
        Online = 23001,         --在线奖励
        DailySignIn = 23002,    --每日签到
        ActivityBack = 23003,   --活动找回（废弃）转移至 21003
        Upgrade = 23004,        --升级奖励
        Rebate = 23005,         --充值奖励
        Score = 23006,          --评分奖励
        Invest = 23007,         --月度投资（废弃）
        Roller = 23008,         --消费转盘
        FortuneBag = 23009,     --福袋
        FundExchange = 23010,   --基金兑换
        ImmortalFund = 23011,   --灵玉基金
        SuperVip = 23012,       --超级vip
        MonthCard = 23013,      --月卡
        AntiFraud = 23014,      --预防诈骗
        LunchSupper = 23015,    --每日体力领取
        SportsCards = 23016,    --竞技月卡
        GodToken = 23017,       -- 封神战令
        MarryRecharge = 23018,  --情缘累充
        PetIntensify = 23019,   --宠物强化
        GodBeastGrace = 23020,  --神兽恩泽
        ChongJiFanLi = 23021,   --宠技返利
        PetWeaponFanLi = 23022, --魂器返利
    },
    Fabao = {
        Main = 24001,            --法宝
        Lottery = 24002,         --法宝抽奖天玑鼎
        Qiling = 24003,          --法宝器灵
    },
    HuanHua = {                 --幻化
        Main = 25001
    },
    Achieve = {                 -- 成就
        Main = 26001,
        Target = 26002,         -- 目标
        Course = 26003,         -- 历程
    },
    Setting = {
        Main = 27001            --设置
    },
    Society = {
        Friend = 28001,         --好友
        Mail = 28002,           --邮件
        Master = 28003,         --师徒
        Sworn = 28004,          --结义
        Marry = 28005,          --情缘
        Share = 28011,          --分享
        Send = 28012,           --赠送
    },
    Chat = {
        Main = 29001,           --聊天
    },
    Challenge = {           
        ShiLian = 72001,           --试炼之地(远征魔冢)
        RushDungeon = 82001,       --勇闯地宫
        RareDungeon = 30007,       --稀有副本
        BigDipper = 30008,         --北斗七星
        GoThrough = 30009,         --渡劫
        FightElite = 30010,        --上古遗迹
        FallenGod = 30013,         --殒神之地
        TowerAdventure = 30014,    --塔楼冒险（小游戏）*xiaoyouxi*
        CowBoy = 30015,            --牛郎织女（小游戏）
        FightElite2 = 30016,       --圣器秘境
    },
    TreasureHunt = {
        Main = 31001,            --寻宝
        XunQi = 31002,           --寻器
        Partner = 31003,        --普通伙伴唤灵
    },
    Appearance = {              --颜值外观
        Main = 32001,
        Bubbling = 32002,       --冒泡
        Title = 32003,          --头衔
        TailTag = 32004,        --尾标
    },
    Store = {
        Main = 33001,           --百货商店  
        Gold = 33002,           --元宝商城   
        Recharge = 33003,       --充值界面
        Diamond = 33004,        --钻石商城
        Sports = 33012,         --竞技商店
        Arena = 33013,          --擂台商店
    },
    ServerActivity = {
        Main = 34001,            --开服活动
        LuckRoller = 34002,      --幸运转盘
        WordGift = 34003,        --集字好礼
        Athletics = 34004,       --新服竞技
        LimitShop = 34005,       --折扣商店
        FashionShop = 34006,     --时装商店
        CompetitionRank = 34007, --新服比拼(废弃)
        LuckyCow = 34008,        --福牛报喜
        LuckyTiger = 34009,      --寅虎纳福
        LoveFashion = 34010,     --情缘时装
        PetStr = 34011,          --宠物强化
        NewServerPet = 34012,    --新服灵宠
        SpecialPower360 = 34013, --360大厅特权界面
        SpecialPowerQQ = 34014,   --QQ大厅特权界面
        DoublePlat360 = 34015,   --360双端奖励界面
        SpecialPowerShunWang = 34016,   --顺网大厅特权界面
        DoublePlatShunWang = 34017,     --顺网双端奖励界面
        
    },
    PetPiecesCompose = {
        Main = 35001,           --宠物碎片合成弹框
    },
    GuideRestraint = {
        Main = 36001,           --指引克制
    },
    SilkRoad = {
        Main = 37001,           --远洋贸易
    },
    Arena = {
        Main = 38001,           --竞技场 (古仙宝库) 
        Reward = 38002,         --竞技场 奖励标签
        Rank = 38003,           --竞技场 排行标签
    },
    RolePromote = {
        Main = 39001,           --人物进阶
    },
    RoleAppear = {
        Main = 40001,           --更换形象
    },
    Acts = {
        trevi_fountain = 41001, --许愿池
    },
    PetPiecesShenshouCompose = {
        Main = 42001,           -- 宠物神兽碎片合成
    },
    Experience = {
        Main = 43001, --历练
        Task = 30001,              --挑战任务
        HuanShi = 30003,           --幻世挑战
        GuMo = 30002,              --锢魔之塔
        
        SchoolExercise = 30005,    --校场演武
        ChallengeTest = 30006,     --轮回忘川
        EndlessTower = 30011,      --无尽之塔
        MirageTerritory = 30012,   --琉璃蜃境
        ShiShen = 30020,           --弑神之塔
    },
    GlassTripod = {
        Main = 44001,           --琉璃鼎界面
    },
    --奖励福利开启图标入口
    RewardOpen = {
        Main = 45001,
    },
    --新春活动-温情端午-万圣节
    SpringFestival = {
        Main = 46001,
        FarewellAskNeo = 46002,    -- 辞旧迎新-端午知多少
        CowGoingLuck = 46003,      -- 牛行大运-艾香满城
        Store = 46004,             -- 年货节  -粽香采购
        CowOrigin = 46005,         -- 牛运当头-金龙穿花
        CowConvert = 46006,        -- 牛运兑换-金龙兑换
        CowGoodLuckShop = 46007,   -- 牛运商城-粽情商城-万圣商城
        CowGoodLuck = 46008,       -- 牛运无边-粽情诗韵-南瓜魔法
        LittleYearGift=46009,      -- 小年送礼-鸢飞送礼
    },
    LabourDay = {
        Main = 47001,
        SpringClimb = 47002,       -- 春日登峰
        LabourGift = 47003,        -- 劳动好礼
        CombineGift = 47004,       -- 组合贺礼
    },
    StarTreasureMap = {
        Exchange = 48001,         --星辰宝图
    },
    --周期活动
    PeriodActivity = {
        Main = 49000,           --周期活动
        LoversMatch = 49001,    --情缘对对碰
        FairyWhere = 49002,     --仙子去哪儿
        ConcentricKnot = 49003, --缘牵同心结
        RabbitRace = 49004,     --兔兔快跑
        SanXianGuiDong = 49005, --灵兔招福 三仙归洞
        LuckyRing = 49006,      --灵兔招福 套圈纳吉
        MSLantern = 49007,      --山海灯会 
        SkyTreasureHunt = 49008,--天宫寻宝
        LoversShop = 49010,     --情缘小铺
        ImperialExam = 49011,   -- 科举考试
        School = 49012,         -- 我要上学
        SheepTeam = 49013,      -- 羊羊总动员
        BookStore = 49014,      -- 文昌书局
    },
    Illustrate = {
        Main = 50001,           --图鉴
        Collect = 50002,        --收集
        Rare = 50003,           --珍品
    },
    Task = {
        Accepted = 51001,       --已接
        Acceptable = 51002,     --可接
        Adventure = 51003,      --奇遇
    },
    ChatTrumpet = {
        Main = 52001,           --聊天喇叭
    },
    ItemsSub = {
        Main = 53001,           --任务道具提交
    },
    PetSelect = {
        Main = 54001,           --宠物选择界面
    },
    ProfesstionChange = {
        Change = 55001,         --转职界面
        ColorChange = 55002,
    },
    RandActivity = {
        SevenDay = 56001,        -- 神秘试炼（七天活动）
        FirstCharge = 56002,     -- 首充界面
        LoginReward = 56003,     -- 28天登陆
        LustrateNian = 56004,    -- 驱除妖兽
        ThrivingShop = 56005,    -- 欣欣向荣
        JoySeeksFavors = 56009,  -- 欢乐寻宠
        SealPromote = 56010,     -- 封印提升
        DropDouble = 56011,      -- 掉落双倍
        TaxDiscount = 56013,     -- 税率优惠
        LinkageGift = 56015,     -- 联动送礼
        AdventureGift = 56016,   -- 奇遇礼包
        PetExchange = 56017,     -- 宠物兑换
        SwordZither = 56020,     -- 剑胆琴心
        AnimalRace = 56021,      -- 神兽径赛
        Divination = 56022,      -- 天命卜卦
        IllustrateTreasure=56024,-- 图鉴夺宝
        ActPetSearchView = 56028,-- 送寻宝
        ExclusiveGift = 56029,   -- 专属礼包
        SpSecretTreasure = 56030,-- 特惠秘宝
        GiveSecretKey = 56034,   -- 赠送寻宝钥匙
        AccRecharge = 56035,     -- 累计充值
        HeavenSent = 56039,      -- 令牌天赐
        OneYuanBuy = 56040,      -- 一元购
        FirstChargeExclusive=56041,-- 首充专属
        HeartSkill = 56042,        -- 心法无量
        PaiMaiHang = 56043,        -- 拍卖行
        WorldSoul = 56044,         -- 赋世魂器
        RegressReward = 56045,     -- 回归豪礼
        HappyLinkUp = 56046,       -- 连连乐翻天
        HorcruxTiancheng = 56047,  -- 魂器天成
        SoulsAnecdote = 56048,     -- 万灵奇闻
        ZhaoCaiJinBao = 56049,     -- 招财进宝
        CloudSwamp = 56050,        -- 再战云泽
        FlowerTouch = 56053,       -- 云泽花繁录
        JiXingGaoZhao = 56054,     -- 吉星高照
        WeekGift = 56055,          -- 周末限好礼
        LimitRecharge = 56056,     -- 限时累充
        ChaoZhiGift = 56057,       -- 超值好礼
        ActEquipSmelt = 56058,     -- 熔炼有礼
        CompetitionRank = 56059,   -- 新服比拼
        ClothAroma = 56060,        -- 锦衣天香
        LuckBox = 56061,           -- 幸运盲盒
        TianYiYaoShi = 56062,       -- 天衣耀世
        GodClothes = 56063,        -- 神赐华服
        Precious = 113007,         -- 珍品行铺
        SingleRebate = 56064,      --单笔返利
        GiveHundred = 56065,       -- 赠100抽
        InviteFriend = 56066,       --微信邀请好友（H5线的活动，R线占位）
        WeekendOverCharge = 56067, -- 周末连充礼
        DaZhanHongTu = 56068,       --大展宏兔
    },
    MedalTask = {
        Main = 57001,           -- 勋章任务
    },
    CutChapter = {
        Main = 58001,           -- 断章
        BlackWhite = 58002,     -- 独白
    },
    MapView = {
        Main = 59001,           -- 地图
    },
    MainTask = {
        Main = 60001,           -- 主线推图
        EliteLock = 60002,      -- 精英解锁
        Task = 60003,           -- 任务页面
    },
    WanYaoGu = {
        Mian = 61001,           -- 万妖谷
    },
    MapGuideView = {
        Main = 62001,           -- 地图指引
    },
    ExchangeTask = {
        Main = 63001,           -- 物品兑换
    },
    --放在精彩活动图标里面的红点
    Splendid = {
        Main = 64001,              -- 精彩活动用的

        ColorfulIllustrate = 56007,-- 七彩图鉴
        HeavenGift = 56012,        -- 天降好礼
        PartnerExchange = 56014,   -- 伙伴兑换
        QmDiscount = 56018,        -- 全民团购
        DiscountCarnival = 56019,  -- 折扣狂欢
        LuckyDiscount = 56023,     -- 幸运折扣
        ElementCards = 56026,      -- 元素翻牌
        SignInContinuity = 56027,  -- 连登好礼
        ExpPurchase = 56030,       -- 经验飙升
        GiftBox = 56031,           -- 限时宝盒
        CustomizedTreasure=56032,  -- 定制宝箱
        ChainBox = 56033,          -- 限时宝袋
        PetLoot = 56036,           -- 宠物夺宝
        GiftRotation = 56037,      -- 好礼转动
        LuckyMaze = 56038,         -- 幸运迷宫

        HeritageTreasure = 64002,  -- 传承瑰宝 
        WealthCornucopia = 64003,  -- 财神聚宝
        DealDiscount = 64004,      -- 交易狂欢日
        PetBefall = 64005,         -- 神兽降临
        SignHappy = 64006,         -- 登陆连好礼       
    },
    IllusionExchange = {
        Main = 65001,           --幻化商店
    },
    FightFieldBoss = {          
        Main = 66001,             --野外boss
        Shxy = 66002              --山海降妖
    },
    ArenaMatch = {
        Arena = 67001,          --擂台战擂台
        Team = 67002,           --擂台战队伍
        Reward = 67003,         --擂台战奖励
    },
    Illusion = {
        HuanHua = 68001,        --幻化（霓裳）
        Appearance = 68002,     --外观
    },
    Rank = {
        Main = 69001,           --排行榜
        FirstKill = 69002,      --首杀
    },
    Battlefield = {
        Main = 70001,           --幻界战场
        Shop = 70002,           --幻界商店
    },
    RechargeTip = {
        Main = 71001,           --首充tip
    },
    Modrons = {
        Main = 72001,           --远征魔冢
    },
    MedalStart = {
        Main = 73001,           --勋章任务
    },
    --百鬼夜行
    GhostNight = {
        Main = 74001,
    },
    --排期活动（万灵系列）
    PlanActivity = { 
        Main = 75001,              -- 万灵印象
        AllSoulsFull = 75002,      -- 万灵共盈
        HelpOthers = 75003,         --义胆助人
        CowOrigin = 46005,         -- 牛运当头-金龙穿花
        LustrateNian = 56004,      -- 驱除妖兽
        ThrivingShop = 56005,      -- 欣欣向荣
        SaleShop = 56006,          -- 特卖商店
        ColorfulIllustrate = 56007,-- 七彩图鉴
        QmDiscount = 56018,        -- 全民团购
        DiscountCarnival = 56019,  -- 折扣狂欢
        Divination = 56022,        -- 天命卜卦
        LuckyDiscount = 56023,     -- 幸运折扣
        LuckyMaze = 56038,         -- 幸运迷宫
        GiftBox = 56031,           -- 限时宝盒
        CustomizedTreasure=56032,  -- 定制宝箱
        ChainBox = 56033,          -- 限时宝袋
        HeritageTreasure = 64002,  -- 传承瑰宝
        WealthCornucopia = 64003,  -- 财神聚宝
        PetBefall = 64005,         -- 神兽降临
        FourSeasonCard = 76001,    -- 四季花牌
        FourSeasonBox = 76002,     -- 四季宝盒
        FourSeasonChest = 76003,   -- 四季宝箱
    },
    --功能预告
    FunOpenNotice = {
        CourageChallenge=80001,--固魔
        Battlefield = 80002,   --战场
        Arena = 80003,         --竞技场
        Ring = 80004,          --擂台
        Modrons = 80005,       --魔冢
        FieldBoss = 80006,     --野外首领
        RushDungeon = 80007,   --勇闯地宫
        CloudArena = 80008,     --跨服竞技（穹顶之争）
        ChallengeTest = 80009,  --轮回忘川
        Bicycle = 80010,  --灵骑
        PetIsland = 80011,  --灵宠岛
        DeepDarkFantasy = 80012,  --幽冥炼狱
        PetWeapon = 80013,  --宠物魂器
        PetStamp = 80014, -- 宠物神印
        RoleHeartSkill = 80015, -- 心法
        GivePetNotice = 80016,  --赠送宠物
        PartnerGift = 80017, -- 伙伴天赋
        Accrssory = 80018, -- 配饰
        PetPray = 80019, -- 宠物祝福
        GoThrough = 80020, -- 渡劫
        PartnerContract = 80021, -- 伙伴契约
        UnLimitTower = 80022, -- 无尽之塔
        MirageTerritory = 80023, -- 琉璃蜃境
        FlyUp = 80024, -- 飞升
        PetRefining = 80025, -- 宠物炼妖
        BlazeStar = 80026, -- 橙装
        Smelt = 80027, -- 熔炼
        GhostBust = 80028, -- 妖行试炼
        HarmonyHeaven = 80029, -- 造化·鸿蒙
        StarMap = 80030,  -- 星图
        HuanShou = 80031,  -- 幻兽
        TeamChallenge = 80032,  -- 幻世挑战
        YouMing = 80033,  -- 幽冥附魂
    },
    GuildTrialNew = {
        Main = 81001,--家族试炼
    },
    RushDungeon = {
        Main = 82001,--勇闯地宫
    },
    Formation = {
        Main = 83001,   -- 阵容
    },
    MysteryStore = {
        Main = 84001,   -- 神秘商店
    },
    PartnerEquiqBreak = { --伙伴装备分解
        Main = 85001,
    },
    Anecdote = {
        Hint = 86000,   --万灵大事件
        SudokuBook=86001,-- 洛书奇门
        Puzzle = 86002,  -- 九宫拼图
        Vision = 86003,  -- 眼力训练
        Couplet = 86004, -- 天下绝对
        Poetry = 86005,  -- 游历诗人
        StarLife = 86006,-- 司命星君
        Antique = 86007, -- 古董图鉴
        FishAnswer = 86008,-- 鱼王眼泪
        Klotski = 86009, -- 数字华容道
        Decrypt = 86010, -- 宝箱解密
        CookSort = 86011,-- 寻味美食
        PackPoetry = 86012,  -- 诗词歌赋
        VisionDiscern = 86013, -- 真假画作
        PuzzleStorehouse = 86014, -- 神秘宝藏
        PuzzleMechanical = 86015, -- 机关巧解
        PuzzleStorehouseMap = 86016, -- 神秘宝藏地图
        MemoryMaster = 86017,-- 记忆大师
        ImmortalRecruitStudent = 86018,-- 仙师收徒
        MillenniumPeachBlossom = 86019, -- 千年桃花开（阴阳眼）
        SoulBack = 86020,               -- 魂归故里（阴阳眼）
        DesertDragon = 86021,           -- 沙漠怪龙（阴阳眼）
        PoetrySoul = 86022,             -- 诗者之魂（阴阳眼）
        EmperorPortrait = 86023,        -- 帝王画像（阴阳眼）
        FishMessenger = 86024,          -- 海鱼使者（阴阳眼）
        MusicAnecdote = 86025,          -- 奇闻音游
        FunRiddle = 86026,              -- 趣味答题
        Poetry2 = 86027,                -- 游历诗人2(沙漠)
        Decrypt2 = 86028,               -- 宝箱解密2(沙漠)
        Decrypt3 = 86029,               -- 宝箱解密3（海底）
        Couplet2 = 86030,               -- 天下绝对2
        PackPoetry2 = 86031,            -- 诗词歌赋2
        Couplet3 = 86032,               -- 天下绝对3
    },
    TaskWXLInfo = {
        Main = 87001,   -- 温香楼查看信息
    },
    TaskWXLCut = {
        Main = 88001,   -- 温香楼黑幕
    },
    LevelComplement = {
        Main = 89001,   --等级助力
    },
    Equip = {
        Upgrade = 4003,       --装备强化（原工坊强化）
        Ulti = 90002,          --装备精工
        Inser = 4005,         --装备镶嵌（原工坊镶嵌）
        Smelt = 4004,         --装备熔炼（原工坊熔炼）
        Spirit = 90005,        --装备铸灵
        Neveal = 4007,         --装点点化(原工坊点化)
        Conversion = 4009,      --装备转换
    },
    Resolve = { --装备分解
        Main = 91001,
    },
    DivineCostume = {
        Main = 92001,   --神装
    },
    Athletics = {
        Main = 93001,   --竞技
    },
    FightEnergy = {
        Main = 94001,   --秘境体力使用界面
    },
    TaskPreviewMainLine = {
        Main = 95001,   --主线任务预告
    },
    RunSpeed = {
        Main = 96001,   --调速功能
    },
    TombRaider = {
        Main = 97001,   --遗迹夺宝
    },
    PartnerBiography = {
        Main = 98001,   --伙伴传记界面
    },
    OffLineReward = {
        Main = 99001,   --境界界面
    },
    Bicycle = {
        Main = 100001,   -- 灵骑
        Basice = 100002, -- 基础
        Foster = 100003, -- 培养
        Skill  = 100004, -- 灵技
        PetPanel = 100005,  -- 御宠
        DianHua = 100006,   -- 点化
    },
    SplaceAnecdote = {
        Shop = 101001,  -- 秘境奇闻的商店
        Quest = 101002, -- 秘境奇闻的答题
    },
    LevelGiftNotice = {
        Main = 102001, --等级奖励预告
    },
    Knight = {
        Main = 103001,  --侠义值
        Rank = 103002,  --侠义值排行
        Reward = 103003,  --侠义值奖励
    },
    --活动预告
    ActivityRemind = {
        ChiefElection = 104001,  --首席
        GuildMelee = 104002,     --家族大乱斗
        GuildEmbattle = 104003,  --家族荣誉战
        ArenaMatch = 104004,    -- 擂台结算提醒
    },
    DeepDarkFantasy = {
        Main = 105001, -- 幽冥炼狱
    },
    PetStampPray = {
        Main = 106001, -- 宠物祈神
        RedConvert = 106002, -- 红色神印兑换
    },
    PetWeapon = {
        Main = 107001, -- 宠物魂器
        Intensify = 107002, -- 魂器强化
        Refine = 107003,    -- 魂器精炼
        Change = 107004,    -- 魂器兑换
    },
    GodLandTraces = {
        Main = 108001,  -- 天地道痕
    },
    BianQiang = {
        Main = 109001,  --变强
        TongJi = 109002, --今日统计
    },
    FightFB = {
        Material = 110001,       --玉虚境
        Fight = 110002,          --云梦泽
        PetIsland = 110003,      --灵兽岛
        PetGod = 110004,         --神印地
        DreamNotes = 110005,     --梦渊谷 原梦渊笔录(49001废弃) 迁移到 110005 梦渊谷
        Energy = 110010,         --体力
    },
    PrestigeTask = {
        Main = 111001,  --声望任务
    },
    PetWeaponConvert = {
        Main = 112001,  --魂器兑换
        Qua6 = 112002,  --红色品质兑换
    },
    NationalDay = {
        Main = 113001,              --国庆活动
        SummerExperience = 113002,  --露月争锋（初夏历练换皮）
        FlowerLamp = 113003,        --多彩河灯（多彩花灯换皮）
        LuckySign = 113004,         --好运花签（好运花签换皮）
        Lotus = 113005,             --桂香觅宝（荷香觅宝换皮）
        MiBao = 113006,             --觅宝坊店
        Precious = 113007,          --珍品行铺
    },
    TreasureMap = {
        Show = 114001, --宝图奖励展示
    },
    Accrssory = {
        Main = 115001,          --配饰
        Inherit = 115002,       --配饰继承
        Destiny = 115003,       --天命
        Illustration = 115004,  --配饰图鉴
        Strength = 115005,      --配饰强化
        Change = 115006,        --配饰置换
    },
    YinYangEye = {
        Main = 116001,--阴阳眼
    },
    MedalSpChange = {
        Main = 117001, -- 天赐令牌转换
    },
    MountainSea = {                     --山海系列
        Main = 118001,                  --山海回想
        Question = 118002,              --山海问答
        WindMoon = 118003,              --山海山海争锋风月
        SummerExperienceTwo = 118004,   --
        FlowerLampMS = 118005,          --山海花灯
        LotusTwo = 118006,              --山海觅宝
        FirstMeet = 118007,             --山海初遇
        Task = 118008,                  --山海缘起（跑环任务）
        Meet = 118009,                  --山海相逢
        MineCollect = 118010,           --山海聚宝
        SeaChainBox = 118011,           --山海宝袋(限时宝袋复刻)
        SeaGiftBox = 118012,            --山海宝盒(限时宝盒复刻)
        SeaCustomizedBox = 118013,      --山海宝箱(定制宝箱复刻)
        FourSeasonCard = 76001,         -- 四季花牌
        FourSeasonBox = 76002,          -- 四季宝盒
        FourSeasonChest = 76003,        -- 四季宝箱
    },
    --指引相关小界面
    Guide = {
        ClearEffect = 119001,       --法阵净化特效UI界面
        ElementGame = 119002,       --元素克制小游戏
    },
    SummerFirst = {
        Main = 120001,
        SummerExperience = 120002,  --初夏历练
        FlowerLamp = 120003,        --多彩花灯
        SummerFirstShop = 120004,   --夏日商城
        LuckySign = 120005,         --好运花签
        SummerDiscountShop = 120006,--超级折扣商城
        Banquet = 120007,           --琼楼玉宴
        SummerMaze = 120008,        --夏日迷宫
        Lotus = 120009,             --荷香觅宝
        CoolCard = 120010,          --元气翻牌
        CoolBreeze = 120011,        --凉风送爽
    },
    YunZe = {                       -- 云泽系列
        Main = 121001,              -- 云泽梦册
        Territory = 121002,         -- 云泽画境
        SuperValueEnjoy = 121003,   -- 超值纷享
        SameFlower = 121004,        -- 分享同花礼
        YunZeChainBox = 121005,     -- 云泽宝袋(限时宝袋复刻)
        YunZeGiftBox = 121006,      -- 云泽宝盒(限时宝盒复刻)
        YunZeCustomizedBox = 121007,-- 云泽宝箱(定制宝箱复刻)
        EnjoyRedouble = 121008,     -- 纷享加倍
        ElvesCatch = 121009,        -- 精灵大抓捕
        YzWish = 121010,            -- 云泽祈愿
        SaleShop = 56006,           -- 特卖商店
        YunZeErSanShi = 56052,      -- 云泽二三事
        MoZuZaiNaLi = 56051,        -- 魔族在哪里
        YunZeNiceGift = 121011,     -- 云泽好礼
        MengChongJiaDao = 121012,   -- 萌宠驾到
        FourSeasonCard = 76001,     -- 四季花牌
        FourSeasonBox = 76002,      -- 四季宝盒
        FourSeasonChest = 76003,    -- 四季宝箱
    },
    AllSoulsStorybook = {
        Main = 122001,
    },
    Live = {
        Main = 123001,  --生活
    },
    TradeRoad = {
        Main = 124001,       --丝绸之路坊市
        Rank = 124002,       --排行
        Tavern = 124003,     --酒馆
    },
    TreasureTask = {
        Main = 125001,      --九州秘宝转盘
        Special = 125002,   --九州秘宝神秘大奖
    },
    --心法兑换界面
    HeartSkillChang = {
        Main = 126001
    },
    --虎年活动
    TigerYear = {
        Main = 127001,
        TigerBorn = 127002,             --寅虎问世
        TigerGreatLuck = 127003,        --虎行大运
        TigerThank = 127004,            --寅虎拜岁
        NianNianYouYu = 127005,         --年年有鱼
        TigerMora = 127006,             --寅虎利是(寅虎逗新春)
        TigerGiftMoney = 127007,        --寅虎压岁
        TigerSpringTravel = 127008,     --寅虎探春
        AuspiciousSnow = 127009,        --瑞雪丰年
        TigerNewCowOld = 127010,        --辞牛迎新虎
    },
    --九州论剑
    LunJian = {
        Main = 128001,
        Signup = 128002,
    },
    -- 变身卡
    ChangeCard = {
        Main = 129001,
    },
    BountyTask = {
        Array = 130001,         --门派法阵
        Practice = 130002,      --门派修行
        Letter = 130003,        --门派飞天鼠
    },
    HeroVanBattle = {
        Main = 131001,              --英雄会武
        TeamView = 131002,          --英雄会武·队伍界面
        ApplyView = 131003,         --英雄会武·报名界面
        GloryView = 131004,         --英雄会武·荣誉排名
        PreShow = 131005,           --英雄会武·海布界面
        Rule = 131006,              --英雄会武·英雄帖
    },
    UniverTeamArena = {
        Main = 132001,          -- 天下第一比武大会
        Store = 132002,         -- 武会宝阁
    },
    PetBagde = {
        Main = 133001,          -- 宠物徽章
        Resolve = 133002,       -- 徽章分解
    },
    PetRefining = {
        Main = 134001,          -- 宠物炼妖
    },
    --情缘盛会
    MarryParty = {
        Notice = 135001,        --情缘盛会预告
    },
    PetInborn = {
        Main = 136001,          -- 宠物天赋
    },
    GhostBust = {
        Main = 137001,          -- 妖行试炼
        Achieve = 137002,       -- 妖行试炼·成就
    },
    HundredGodBoss = {
        Main = 138001,  --百神之师
    },
    MayDay = {
        Main = 139001,            -- 五一庆典
        VitalityHelpOthers=139002,-- 元气助人
        SkyLantern = 139003,      -- 祈愿孔明灯
        Butterfly = 139004,       -- 寻梦蝴蝶
        VitalityFlop = 139005,    -- 元气翻牌
        SowReward = 139006,       -- 播种有礼
        Articulately = 139007,    -- 秒笔生花
        AllSoulCast = 139008,     -- 万灵筑礼
        BunCome = 139009,         -- 包子来了
        VitalityFirework = 139010,-- 元气烟火秀
        Braving = 139011,         -- 乘风破浪
        YuanQiSpeed = 139012,     -- 元气竞速
        VitalityNote = 139013,    -- 元气记事簿
        
        SaleShop = 56006,          -- 特卖商店
        QmDiscount = 56018,        -- 全民团购
        LuckyDiscount = 56023,     -- 幸运折扣
        GiftBox = 56031,           -- 限时宝盒
        CustomizedTreasure=56032,  -- 定制宝箱
        ChainBox = 56033,          -- 限时宝袋
    },
    Butterfly = {
        Capturing = 140001,     -- 抓蝴蝶界面
    },
    AnimalWorld = {
        Main = 141001,          -- 异兽临世
        Shop = 141002,          -- 兑换商城
        Task = 141003,          -- 任务详情
    },
    BranchSerialTask = {
        Main = 142001,          -- 连环支线任务
    },
    HarmonyHeaven = {
        Main = 143001,          -- 鸿蒙天界
        Shop = 143002,          -- 天界宝库
        Record = 143003,        -- 首通队伍
    },
    Marbas = {
        Base = 146001,          -- 造化装备·基础信息
        Build = 146002,         -- 造化装备·打造功能
        Refine = 146003,        -- 造化装备·精炼功能
        Bathe = 146004,         -- 造化装备·珑铸功能
        Blood = 146005,         -- 造化装备·血祭功能
        Vigorous = 146006,      -- 造化装备·罡气功能
        Bag = 146007,           -- 造化装备·背包
        Demons = 146008,        -- 造化装备·入魔
        Resolve = 146009,       -- 造化装备·分解
        GemResolve = 146010,    -- 造化装备·批量分解元魂
    },
    ShengQi = {
        Main = 144001,          -- 圣器
        Attri = 144002,         -- 属性
        Up = 144003,            -- 升级
        Skill = 144004,         -- 技能
        FuShi = 144005,         -- 符文
        Book = 144006,          -- 图鉴
        FuShiFusion = 144007,   -- 符石合成
    },
    BranchMarbasTask = {
        Main = 145001,          -- 造化装备任务
    },
    BranchRelicTask = {
        Main = 147001,          -- 圣器环式任务
    },
    DragonBoat = {
        Main = 148001,              -- 端午活动
        ReedIncenseCity = 148002,   -- 粽香满城
        SweetSaltyWar = 148003,     -- 甜咸之争
        SoupBath = 148004,          -- 兰汤沐浴
        DuanYangZiXuanLi = 148005,  -- 端阳自选礼
        DragonCollect = 148006,     -- 苍龙赐福
        NoonSachet = 148007,        -- 端阳香囊
        DuanYangFuLi = 148008,      -- 端阳福礼

        GiftBox = 56031,           -- 限时宝盒
        CustomizedTreasure=56032,  -- 定制宝箱
        ChainBox = 56033,          -- 限时宝袋
        SaleShop = 56006,          -- 特卖商店
        LuckyDiscount = 56023,     -- 幸运折扣
        QmDiscount = 56018,        -- 全民团购
    },
    ThreeHundredSkin = {
        Main = 150001, --300抽皮肤
    },
    GoldRMineC = {
        Main = 151001, --金龙聚宝
    },
    QixiFestival = {
        Main = 149001,      --七夕汇总
        SevenBuy = 149002,  --七巧特惠
        MeiLiQiXi = 149003, --魅力七夕
    },
    KFHeroRecord = {
        Main = 152001,      --跨服英雄录
    },
    BattleOpTip = {
        Main = 153001,--战斗模式选择界面
    },
    RewardOnlineGift = {
        Main = 154001,--在线礼包
    },
    NevealExtraPlus = {
        Main = 155001,--附魔石强化
    },
    StarMap = {
        Main = 156001,  --星图
        Role = 156002,  --角色
        Pet = 156003,   --宠物
        Partner = 156004,   --伙伴
        Super = 156005, --超星系
    },
     MidAutumnFestival = {
        Main = 157001, -- 月满中秋 中秋活动页
        Yxdm = 157002, -- 月夕灯谜
        Yxlc = 157003, -- 月夕累充
        Zysc = 157004, -- 追月商城
        Zqzy = 157005, -- 追月商城-仲秋追月子页面
    },
    HuanShou = {
        Main = 158001,  --幻兽
        Attri = 158002, --属性
        Skill = 158003, --打书
        Star = 158004,  --天赋
        Egg = 158005,   --幻兽蛋
        SoulResolve = 158006,   -- 灵魄分解
        RefineStar = 158007,    -- 炼丹炉
    },
    LingBao = {
        Main = 159001, --灵宝
    },
    FairyDragon = {
        Main = 160001,      --神龙
        Foster = 160002,    --神龙培养
        Star = 160003,      --神龙升星
        Skin = 160004,      --神龙皮肤
        Soul = 160005,      --神龙龙魂
        Pray = 160006,      --神龙祈愿           
    },
    LandsBetween = {
        Main = 161001,          --神域 主界面
        AlterInfo = 161002,     --神域 神坛信息
        Report = 161003,        --神域 战报
        City = 161004,          --神域 主城
        Map = 161005,           --神域 地图
        MeetGift = 161006,      --神域 见面礼
        Moving = 161007,        --神域 行动丹
        Poster = 161008,        --神域 海布界面
        Produce = 161009,       --神域 产出界面
        Season = 161010,        --神域 赛季界面
        ReportDetail = 161011,  --神域 战报详细界面
        Explain = 161012,       --神域 说明
    },
    LandsBetweenExtra = {
        GodPos = 162001,        --神域-神位
        DragonPluse = 162002,   --神域-龙脉
        HandBooks = 162003,     --神域-指南
        Rank = 162004,          --神域-排行榜
        Shop = 162005,          --神域-商店 
    },
    GodAreasActs = {
        Ruins = 163001,--归墟之境
        Palaces = 163002,--云顶天宫
        Animals = 163003,--阵营神兽
        RuinsScene = 163004,--归墟场景界面
        PalacesHall = 163005,--殿堂
        PalacesScene = 163006,--云顶场景界面
    },
    ShenYuProbe = {
        Main = 164001,       -- 神域探索
    },
    LBFestival = {
        main = 165001, -- 神域狂欢
        Treasure = 165002,   -- 神域狂欢·神域宝藏
    },
    Act_Halloween={
        Main=166001,     --万圣活动主界面
        LeiChong=166002, --万圣活动累充界面
        Shop=166003,  --万圣商城界面
        Pray=166004,--万圣祈福
        LuckNum = 166005,   -- 幸运数字
        ShanDanMiLing = 166006, -- 圣诞密令
    },
    -- 新版挑战类奇闻
    AnecdoteNew = {
        Vision = 167001,    -- 眼力训练玩法
        Puzzle = 167002,    -- 九宫拼图玩法
        Poetry = 167003,    -- 游历诗人玩法
        Couplet = 167004,   -- 天下绝对玩法
        Decrypt = 165005,   -- 宝箱解密玩法
        Memory = 86017,     -- 记忆大师 ：还是用原来的就行
        Riddle = 86026,     -- 趣味答题 ：还是用原来的就行
    },
    QQBigPlayer = {
        Main = 168001,      --QQ大厅大玩咖
    },
    YouMing = {
        Main = 169001,      -- 幽冥系统
        Lottery = 169002,   -- 探宝
        Compound = 169004,  --幽冥合成
        Seal = 169012,      --幽冥抓捕
        Reslove = 169013,   --幽冥分解
    },
    FavorNpc={
        Main = 170001,      -- 友人志
    },
    YouMingPractice={
        Base = 171001,      -- 修炼基础
        SanHun = 171002,    -- 修炼三魂
        QiPo = 171003,      -- 修炼七魄
        GaiZao = 171004,    -- 修炼改造
        JueXing = 171005,   -- 修炼觉醒
        MingZhuang = 171006,-- 修炼冥装
        GaiZao1 = 171007,   -- 改造基础
        GaiZao2 = 171008,   -- 改造技能领悟
        GaiZao3 = 171009,   -- 改造天资重置
    },
    RabbitYear = {
        Main = 172001,      --兔年新春活动汇总
        XingXingDianDeng = 172002,--星星电灯
        FuYunShangCheng = 172003, --福运商城
        YingChunNaFu = 172004,   --迎春纳福
        HongYunLeiChong = 172005, --鸿运累充
    },
}
-- 小游戏枚举对应模块id
MINIGAME_MOD = {
    [MINIGAME_TYPE.TOWER_ADVENTURE] = Mod.Challenge.TowerAdventure,
    [MINIGAME_TYPE.COW_BOY] = Mod.Challenge.CowBoy,
}

-- 1 开启 0 不开启
MINIGAME_OPEN_MOD = {
    [Mod.Challenge.TowerAdventure] = 0,
    [Mod.Challenge.CowBoy] = 0,
}

-- 这些模块审核版本不开启
ModAuditClose = {
    [Mod.Reward.GodToken] = true,
    [Mod.Splendid.Main] = true,
    [Mod.SpringFestival.Main] = true,
    [Mod.LabourDay.Main] = true,
    [Mod.RandActivity.AccRecharge] = true,
    [Mod.RandActivity.ExclusiveGift] = true,
    [Mod.RandActivity.SevenDay] = true,
    [Mod.DivineCostume.Main] = true,   --神装
    [Mod.OffLineReward.Main] = true,   --境界界面
    [Mod.Formation.Main] = true,        --布阵
    
    --group
    [Mod.Activity] = true,   --活跃
    [Mod.Challenge] = true,  --挑战
    [Mod.Guild] = true,     --家族
    [Mod.LunJian] = true,   --论剑九州
    [Mod.ServerActivity] = true, --新服活动
    [Mod.Mall] = true,            --交易
    [Mod.FightFB] = true,         --秘境
    [Mod.TreasureHunt] = true,      --唤灵
    [Mod.RolePromote] = true,       --进阶
    [Mod.HeroVanBattle] = true,     --英雄会武
    [Mod.Workshop] = true,          --工坊
    [Mod.Knight] = true,          --侠义
}

--屏蔽充值的包需要不开启的模块
ModBlockRecharge = {
    [Mod.RewardOpen] = true,
    [Mod.Reward] = true,
    [Mod.PeriodActivity] = true,
    [Mod.GodLandTraces] = true,
    [Mod.SpringFestival] = true,
    [Mod.LabourDay] = true,
    [Mod.SummerFirst] = true,
    [Mod.NationalDay] = true,
    [Mod.PlanActivity] = true,
    [Mod.MountainSea] = true,
    [Mod.YunZe] = true,
    [Mod.TigerYear] = true,
    [Mod.Splendid] = true,  
    [Mod.ServerActivity] = true, --新服活动
    [Mod.RandActivity] = true,
}

-- 添加mod_key转换
ModuleChangeConfig = {
    [Mod.NationalDay.SummerExperience] = Mod.SummerFirst.SummerExperience,
    [Mod.NationalDay.FlowerLamp]  = Mod.SummerFirst.FlowerLamp,
    [Mod.NationalDay.Lotus]       = Mod.SummerFirst.Lotus,
    [Mod.NationalDay.LuckySign]   = Mod.SummerFirst.LuckySign,
}

TableMod = {}

function TabMod(modkey)
    TableMod[modkey] = TableMod[modkey] or {modkey}
    return TableMod[modkey]
end

--特殊条件的界面
ModOpenViewFunc = {
    [Mod.Guild.GuildInfo] = function()
        --#62480
        local flag, _ = FunOpen.Instance:GetFunIsOpened(Mod.Guild.GuildInfo)
        if flag then
            if GuildData.Instance:GetGuildID() <= 0 then
                GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
                ViewMgr:OpenView(GuildList)
                return true
            end
        end
    end,
    [Mod.Guild.GuildJiYuan] = function()
        if GuildData.Instance:GetGuildID() <= 0 then
            PublicPopupCtrl.Instance:Center(Language.Guild.JiYuanQuitGuild)
            return true
        end
    end,
    [Mod.TreasureHunt.Main] = function()
        if TreasureHuntData.Instance:NoOneHasOpen() then
            PublicPopupCtrl.Instance:Center(Language.TreasureHunt.NoOneHasOpend)
            return true
        end
    end,
    [Mod.PeriodActivity.ConcentricKnot] = function()
        ConcentricKnotCtrl.Instance:OnClickActEvent()
        return true
    end,
    [Mod.Challenge.FightElite2] = function()
        ShengQiCtrl.GetShengQiJump()
        return true
    end,
    [Mod.RewardOpen.Main] = function()
        ViewMgr:OpenView(RewardView)
        return true
    end,
    [Mod.Experience.ShiShen] = function()
        ViewMgr:OpenView(CourageChallengeView,{type = CourageChallengeData.PagodaType.ShiShen})
        return true
    end
}

--模块转换 10001 >> 10000
function ModToModule(mod_key)
    return math.ceil(mod_key / 1000) * 1000
end

ModToView = {}
ViewToMod = {}
function Mod.RegisterView(mod_key, view)
    ModToView[mod_key] = view
    ViewToMod[view] = mod_key
end

local modkey_to_tab = {}    
--帮助解开ModTab里对于modKey数字的格式显示，方便做模块迁移。当界面的ModKey为一个Table的时候注册进Map，供Mod.ParseKey查找
function Mod.ReigsterModkeyToModTab(mod_tab)
    for _,mk in pairs(mod_tab) do
        if modkey_to_tab[mk] ~= nil then
            LogError("重复注册ModTabKey!===",mk,mod_tab)
        else
            modkey_to_tab[mk] = mod_tab
        end
    end
end

function Mod.ModtableByModkey(mod_key)
    return modkey_to_tab[mod_key]
end

function Mod.IsView(mod_key)
    return ModToView[mod_key] ~= nil
end

function Mod.OpenView(mod_key,beh_id)
    local view_tab = ModToView[mod_key]
    if view_tab ~= nil then
        ViewMgr:OpenView(view_tab)
    else
        Debuger.LogErrorSD("Mod.OpenView ERROR Unregister mod key====" .. tostring(mod_key))
    end
end

function Mod.CloseView(mod_key)
    local view_tab = ModToView[mod_key]
    if view_tab ~= nil then
        ViewMgr:CloseView(view_tab)
    else
        Debuger.LogErrorSD("Mod.CloseView ERROR Unregister mod key====" .. tostring(mod_key))
    end
end

function Mod.FlushView(param_t)
    if param_t.mod_key == nil then
        return
    end
    local view_tab = ModToView[param_t.mod_key]
    if view_tab ~= nil then
        view_tab[param_t.func](view_tab, param_t)
    end
end

-- 解析并等到标签
function Mod.ParseKey(mod_key)
    if type(mod_key) == "string" and tonumber(mod_key) ~= nil then
        mod_key = tonumber(mod_key)
    end
    local key = mod_key
    local index = mod_key % 1000
    local mod_tab = Mod.ModtableByModkey(mod_key)
    if mod_tab ~= nil then --存在ModTab注册，使用新的mod_key解析方式，不依赖数字格式
        key = mod_tab
    else
        if index > 1 then
            key = mod_key - index + 1
        end
    end
    local param_t = CommonStruct.OpenParam()
    param_t.open_param = index
    param_t.mod_key = mod_key
    return key, param_t
end