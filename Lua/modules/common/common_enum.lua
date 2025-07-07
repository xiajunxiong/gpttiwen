-----------------------------------------------------
-- 游戏中的枚举
-----------------------------------------------------
--聊天默认等级
TalkLevel = {
    Default = 15,
}
--颜色枚举
ItemColor = {
    None = 0,
    White = 1,
    Green = 2,
    Blue = 3,
    Purple = 4,
    Orange = 5,
    Red = 6,
    Star = 7,
    God = 8,
    Gold = 9,
    Black = 10,
}

--武器类型
WEAPON_TYPE = {
    NONE = 0,
    BOW = 1, --弓
    SWORD = 2, --剑
    AXE = 3, --斧
    CANE = 4, --杖
    SPEAR = 5, --长枪
    GLOVES = 6, --拳套
    LANTERN = 7, --灯笼
    SHAN = 8, --扇
    SHIELD = 9,--盾
    FISHROD = 12,--鱼竿
    SUB_START = 100 -- 副武器起始位
}

PROF2WEAPON = {
    [1] = WEAPON_TYPE.GLOVES,
    [2] = WEAPON_TYPE.SWORD,
    [3] = WEAPON_TYPE.AXE,
    [4] = WEAPON_TYPE.SPEAR,
    [5] = WEAPON_TYPE.BOW,
    [6] = WEAPON_TYPE.CANE,
    [7] = WEAPON_TYPE.LANTERN,
    [8] = WEAPON_TYPE.SHAN,
}

-- 小游戏枚举
MINIGAME_TYPE = {
    TOWER_ADVENTURE = 1,   --塔楼冒险（该小游戏嵌入游戏内）
    COW_BOY = 2,           --牛郎织女（该小游戏嵌入游戏内）
}

-- 审核服的小游戏枚举
AUDIT_MINIGAME_TYPE = {
    SANXIANGUIDONG = 1,    --三仙归洞（对应渠道加载完资源后只能进入小游戏）
    TAOQANNAJI = 2,        --套圈纳吉（对应渠道加载完资源后只能进入小游戏）
    YINYOU = 3,            --音游（对应渠道加载完资源后只能进入小游戏）
}

--特殊动作武器类型
SP_WEAPON_TYPE = {
    AXE = 3, --斧
    CANE = 4 --杖
}

--装备类型
EquipmentIndex = {
    IndexHelmet = 0, --头盔
    IndexArmour = 1, --盔甲
    IndexMainWeapon = 2, --主武器
    IndexSubWeapon = 3, --副武器
    IndexShoe = 4 --鞋子
}

--装备排序
EquipmentSort = {
    [0] = 7,--0,
    [1] = 6,--1,
    [2] = 5,--3,
    [3] = 4,--4,
    [4] = 3,--2,
}

-- 灵饰排序
JewerlySort = {
    [0] = 2,--5,
    [1] = 1,--6,
    [2] = 0,--7,
}

--主角角色类型
RoleAvatarType = {
    YueZhaoJun = 1, --月照君
    HuaOu = 2, --花偶
    MaoErNiang = 3, --猫耳娘
    TuHun = 4, --涂魂
    BanYue = 5, --半月
    ShangLing = 6 --殇灵
}

--职业枚举
RoleProfessionType = {
    None = 0,
    QiGongShi = 1, --气功师
    JianXiaKe = 2, --剑客
    KuangZhanShi = 3, --狂战士
    TieWei = 4, --铁卫
    ShenSheShou = 5, --神射手
    XingShuShi = 6, --星术师
    YiShuShi = 7, --医师
    FuZhouShi = 8, --巫师
    ProfessionTypeMax = 9
}

-- 初始武器ID
RoleProfessionSimpleWeaponId = {
    [0] = 0,
    [1] = 2101,
    [2] = 2201,
    [3] = 2301,
    [4] = 2401,
    [5] = 2501,
    [6] = 2601,
    [7] = 2701,
    [8] = 2801
}

ItemType = {
    Task = 0, --任务道具
    Weapon = 1, --武器
    Armor = 2, --防具
    Normal = 3, --普通道具
    Ore = 4, --矿石
    Wood = 5, --木材
    Plants = 6, --花草
    Cloth = 7, --布料
    Medicine = 8, --药品
    Gem = 9, --宝石
    Key = 10, --钥匙
    Dimensity = 11, --魔石
    EleCrystal = 12, --水晶
    SkillGrass = 13, --技能草
    PveCrystal = 14, --副本水晶
    CrystalChip = 15, --水晶碎片
    Gift = 16, --礼包
    AttriMedicine = 17, --属性药
    Illustration = 18, --图鉴
    Skill = 19, --技能书
    Jewelry = 20, --灵饰
    Fashion = 21, --幻化时装
    ExpCrystal = 22, --打卡水晶
    Medal = 23, --勋章
    PartnerEquip = 24, --伙伴装备
    PetEquip=25,--宠物装备
    MedalFuling = 26,--勋章附灵石
    Appearance = 27, --颜值物品（头衔、气泡框、尾标）
    Skin = 28,      --皮肤（宠物、伙伴）
    ExpPercent = 30,-- 百分比经验丹
    PetPieces = 31, -- 宠物碎片
    CrystalEquip = 32, -- 秘境神秘紫装
    FaBao = 34,     -- 法宝
    PetWeapon = 35, -- 宠物魂器
    EquipSpirit = 36, --铸灵
    PetStamp = 37,  --宠物神印
    AttributeDan = 38,  --属性丹
    PartnerPact = 39,  --伙伴元神契约
    Accrssory = 40, --配饰
    Neveal = 41, --点化石
    PetBagde = 42, --徽章
    PeakToken = 43, --天下至尊
    Marbas = 45, -- 造化装备
    MarbasGem = 46,  -- 元魂珠
    ShengQi = 47,   -- 圣器
    FuShi = 48,     -- 符石
    ShengQiSkillBook = 49,  -- 圣器技能书
    FuYu = 50, --符玉
    BicycleSkillBook = 51,  --灵骑技能卷
    HuanShouSkillBook = 52, --幻兽技能书
    FairyDragonSkin = 53,   -- 神龙皮肤
    FairyDragonExp = 54,    -- 龙魂经验道具
    LingBao = 55,--灵宝
    FairyDragonSoul = 56,   -- 神龙龙魂
    FairyDragonSpirit = 57, -- 神龙龙魄
    YouMing = 58,   --幽冥
    YouMingEquip = 59,--幽冥装备
}

ItemUseType = {
    Gift = 2,       --礼包
    HPMedicine = 9, --生命回复药
    MPMedicine = 12,  --法力回复药
    HPMedicineRate = 54,    --百分比回血
    MPMedicineRate = 55,    --百分比回蓝
    PrestigeCard = 14,--声望卡
    CurrencyItem = 39,  --货币道具自动使用
    ReduceKillValue = 43, --减少杀戮值
    LuckyMaze = 44,     --幸运迷宫
    MoneyBox = 50,  --货币红包
    Illusion = 8,--幻化 p1 幻化id
    ParnerSkin = 29, --伙伴皮肤 p1 伙伴id p2 皮肤id
}

-- 客户端用物品类型（用于显示查询）
ShowType = {
    Medicine = 1, --药品
    DungeonSealStone = 2, --副本封印石
    Normal = 3, --杂物
    KeyItem = 4, --钥匙
    FiveTreasure = 5, --五宝
    SealCard = 6, --封印卡
    MineStone = 7, --矿石
    Wood = 8, --木材
    Flower = 9, --花草
    Material = 10, --材料
    ElementCrystal = 11, --元素水晶
    Diamond = 12, --宝石
    FabaoSprite = 13, --法宝精华
    IllustrateCard = 14, --图鉴卡
    PartnerPieces = 15, --伙伴碎片
    TreasureMap = 16, --藏宝图
    Formula = 17, --配方
    PetSoul = 18, --宠物魂魄
    PetSkillBook = 19, --宠物技能书
    PetEgg = 20, --宠物蛋
    GiftBag = 21, --礼包
    Fashion = 22, --时装
    Medal = 23, --勋章
    Pet = 24, --宠物
    PartnerLiftCard = 25, --伙伴命格卡
    PetPieces = 26, --宠物碎片
    JewelryMaterial = 27,  -- 灵饰材料
    PartnerWakeStone = 28,  -- 伙伴觉醒石
    ActItem = 29, -- 活动物品
    FuLingShi = 30, --附灵石
    PetEquip = 31,  --宠物护符
    PetSkin = 32,   --宠物皮肤
    PartnerSkin = 33, --伙伴皮肤
    Face = 34,         --颜值系统物品（气泡框、尾标、称号）
    ExpDan = 35,    --经验丹
    CrystalEquip = 36,    --秘境神秘紫装
    Partner = 38,    --伙伴
    PartnerPage = 39,--伙伴残页
    PetWeapon = 40,  --魂器
    EquipSpirit = 41, --铸灵
    PetStamp = 42,  --宠物神印
    BicycleEquip = 43, -- 灵骑装备
    FaBao = 44, -- 法宝激活令,

    HeartSkill = 45,--心法
    HeartSkillChip = 46,   --心法残页
    HeartSkillVolume = 47, --心法转换卷
    Title = 48, -- 称号
    AttributeDan = 49,--属性丹
    GlassTripod = 50, --天引琉璃
    RareMaterial = 51, -- 珍稀材料
    PreciousItem = 52, -- 珍贵道具
    PartnerPact = 53, -- 伙伴元神契约
    Accrssory = 54, --配饰
    GiftFlower = 55,    --鲜花礼物
    PetCommonPieces = 56, -- 宠物通用碎片
    Neveal= 57, --点化石
    LiveFood = 58,  --生活食物
    LiveCard = 59,  --生活变身卡
    FlyUp = 60, --飞升凝练道具
    Halidom = 61,--灵骑圣物
    PetBagde = 62,  -- 宠物徽章
    PetBagdeClip = 63, -- 宠物徽章碎片

    MoneyBox1 = 64, --师门红包
    MoneyBox2 = 65, --桃李红包
    SpecialCompose = 66,    --特殊合成材料，（背包中点击物品合成直接发协议合成的物品）
    PeakToken = 67, --天下至尊
    VitalityFirework = 68,  --元气烟火秀专用道具
    ShengQi = 69,   --圣器
    FuShi = 70,     --符石（圣器上的）
    Marbas = 71,    -- 造化装备
    MarbasGem = 72, -- 造化装备·元魂珠
    ShengQiYaoShi = 73,--圣器钥匙，单抽和5连
    ShengQiSkillBook = 74,  -- 圣器技能书
    LingQiChangeEquip = 75,--灵骑装备转换卷
    LingQiChangeHalidom = 76,--灵骑圣物转换卷
    LingBao = 77,--灵宝    
    FairyDragonSkin = 78,   -- 神龙皮肤
    FairyDragonExp = 79,    -- 神龙龙魂经验道具
    PetItemShow = 80,   --宠物物品展示
    BicycleOrnament = 81,-- 灵骑饰品
    BicycleAct = 82,-- 激活灵骑道具
    IllustrateRare = 83,-- 珍品图鉴卡
    BicycleSkillBook = 84,  --灵骑技能卷
    IllusionChange = 85,--时装，武器，坐骑转换后道具
    HuanShouSkillBook = 86, --幻兽技能书
    HuanShouSoul = 87,  --幻兽魂魄
    HuanShouSPEgg = 88, --幻兽特殊蛋
    FairyDragonSoul = 89,   -- 神龙龙魂
    FairyDragonSpirit = 90, -- 神龙龙魄
    YouMing = 91,   -- 幽冥
    YouMingEquip = 92,  --幽冥冥装
    YouMingClip = 93,   --幽冥碎片
    YouMingSkillBook = 94,  --幽冥技能书
}

--货币类型
CurrencyType = {
    Gold = 0, --元宝
    CoinBind = 2,   --铜币
    GuildCotribution = 3,--公会贡献
    Chivalrous = 4, --侠义值
    Colosseum = 5,  --竞技场积分
    FaceScore = 6,  --颜值币
    Fashion = 7,    --时装币
    Arena = 8,      --擂台积分
    BattlefieldScore = 10,  --幻界战场积分
    LuckCoin = 11,  --气运值
    Battlefield = 12,--幻界古币
    PartnerLevelItem=13,--元灵之髓
    LeaderCoin = 14, --队长工资
    CloudArena = 15, --穹顶之争
    ImmortalCoin = 16,--银币（灵玉）
    DustCoin = 17,--粉尘（铸灵分解获得）
    CanHun = 19, -- 元魂珠残魂 
    FairyDragon = 20,  -- 龙魂碎片
    GodSpaceExploits = 21, -- 神域战功
    GodSpaceEssence = 22, -- 神域精华
    Diamond = 23,           --钻石
    --其他
    Treasure = 101, --寻宝钥匙1
    Treasures = 102, --寻宝钥匙5
    PartnerCall = 103, --伙伴招募令
    PartnerCalls = 104, --伙伴招募令5
    TiePetRope = 105,  --缚宠金绳
    TiePetRopes = 106, --缚宠金绳5
    Blessing = 107,    --祈福币
    SkyGlass = 108,    --天引琉璃
    MysteryFlush = 109,--刷新令
    JadePendant = 110, --琉璃玉
    XunQiZhouFu = 114, --寻器咒符
    XunQiZhouFus = 115, --5连寻器咒符
    Anecdote = 116,    --奇闻令
    SummerMazeTicket = 120,--迷宫令
    HorcruxTianchengKey = 121,--花魂令
    GuessCoin = 122, --竞猜币(天下第一比武竞猜)
    MayDayKey = 123, --元气(元气庆典)
    ShengQiYaoShi = 124, --圣器唤灵钥匙1
    ShengQiYaoShis = 125, --圣器唤灵钥匙5
    PetStampPrayItem = 126,     --宠物神印祺神道具

    Exp = 998, --经验
    RenMinBi = 999, ---人民币
}
ActHuoBi = {
    [0] = 65534,
    [1] = 65532
}
FindHuoBi = {
    [1] = 65534,
    [0] = 65532
}
--货币
HuoBi = {
    [0] = 65534,--元宝
    [2] = 65532,--铜钱
    [3] = 65536,--家族贡献
    [4] = 65537,--狭义
    [5] = 65567,--竞技场积分
    [7] = 27249,--时装币(时装碎片)
    [8] = 65568,--擂台积分
    [11] = 65607,--气运值
    [12] = 65604,--幻界古币
    [13] = 65620,--元灵之髓
    [14] = 65622,--队长工资
    [15] = 65682,--跨服竞技（穹顶之争）
    [16] = 65698,--银币（灵玉）
    [17] = 65716,--粉尘（铸灵分解获得）
    [18] = 65716,--竞猜币（废弃）
    [19] = 65878,--元魂珠残魂
    [20] = 65905,--龙魂碎片
    [21] = 65906,--神域战功
    [22] = 65907,--神域精华
    [23] = 65954,--钻石
    --其他客户端自定
    [101] = 26812,--寻宝钥匙1
    [102] = 26813,--寻宝钥匙5
    [103] = 27402,--伙伴招募令
    [104] = 27403,--伙伴招募令5
    [105] = 27540,--缚宠金绳
    [106] = 27541,--缚宠金绳5
    [107] = 27397,--祈福币
    [108] = 26073,--天引琉璃
    [109] = 27575,--刷新令
    [110] = 26315,--琉璃玉
    [111] = 26412,--蓝色原石
    [112] = 26413,--紫色原石
    [113] = 26414,--橙色原石
    [114] = 27664,--寻器咒符
    [115] = 27665,--寻器咒符5
    [116] = 27842,--奇闻令
    [120] = 27611,--迷宫令
    [121] = 26345,--花魂令
    [122] = 20001,--天下第一比武大会竞猜币
    [124] = 26774,--圣器唤灵钥匙1
    [125] = 26775,--圣器唤灵钥匙5
    [126] = 56447,--宠物神印祺神道具
    [998] = 65535, --经验
    [999] = 99999, --人民币（纯客户端自己定义）
}

AttributeDanType = {
    Attack = 0, 
    Defense = 1, 
    Agility = 2, 
    Recovery = 3, 
    Mental = 4, 
    MaxHp = 5,
    MaxMp = 6,
}

--属性枚举
BattleAttriType = {
    NormalValueBegin = 0, --基础属性值开始
    Attack = 0, --攻击(默认*10,要精确到小数点后一位)
    Defense = 1, --防御(默认*10,要精确到小数点后一位)
    Agility = 2, --速度(默认*10,要精确到小数点后一位)
    Recovery = 3, --回复(默认*10,要精确到小数点后一位)
    Mental = 4, --精神(默认*10,要精确到小数点后一位)
    CurHp = 5,
    MaxHp = 6,
    CurMp = 7,
    MaxMp = 8,
    PhysicsReduce = 9, --物伤减免
    MagicReduce = 10, --法伤减免
    PhysicsIncrease = 11, --物理增伤
    MagicIncrease = 12, --法术增伤
    HealIncrease = 13, -- 治疗加成
    GeDang = 14,  -- 格挡
    NormalValueEnd = 15,
    CrrectedValueBegin = 15,
    --修正属性开始
    Critical = 15, --必杀率（百分比）
    Hit = 16, --命中率（百分比）
    Dodge = 17, --闪避（百分比）
    CounterAttack = 18, --反击率（百分比）
    MagicAttack = 19, --魔攻
    MagicDefense = 20, --魔防
    CriticalIncValue = 21, --必杀伤害值
    CriticalDecValue = 22, --坚韧值
    XiaoGuoMingZhong = 23, --效果命中（百分比）
    ZhenShiShangHai = 24, --真实伤害
    MagicPntrtn = 25, --法术穿透
    AntiEffect = 26, --效果抵抗（百分比）
    FaBao=27, --法爆（百分比）
    FaBaoSahnghai=28, --法爆伤害 （百分比）

    CrrectedValueEnd = 30, --修正值结束
    AntiValueBegin = 30, --抵抗值开始
    AntiPosition = 30, --中毒抵抗 毒药的英语是poison！！！
    AntiSleep = 31, --睡眠抵抗(千分比)(改为冰封抵抗)
    AntiPetrify = 32, --石化抵抗(千分比)
    AntiChaos = 33, --混乱抵抗(千分比)
    AntiDrunk = 34, --酒醉抵抗(千分比)
    AntiSilence = 35, --沉默抵抗(千分比)
    -- AntiCurse = 36, --诅咒抵抗(千分比)
    -- AntiSinkHole = 37, --地陷抵抗(千分比)
    AntiSevereInjuer = 38, --重伤抵抗(千分比)
    -- AntiDefenceCracked = 39, --破防抵抗(千分比)
    AntiValueEnd = 40,
    ElementBegin = 40,
    ElementWater = 40, -- 水元素值
    ElementFire = 41, -- 火元素值
    ElementWind = 42, -- 风元素值
    ElementEarth = 43, -- 地元素值
    VITALITY = 45,--体力
    STRENGTH = 46,--力量
    POWER = 47, --强度
    AGILITY = 48, --敏捷
    MAGIC = 49, --法术
    ATTRMAX = 50,
    AntiAll = 100,--全抗性
    Damage = 102,--伤害
}

-- 这些属性计算角色评分所用公式不一样
AttrScoreSpecial = {
    [BattleAttriType.ZhenShiShangHai] = true,
}

--属性排列顺序（根据界面上的显示顺序来定的）
AttrTypeSort = {
    [BattleAttriType.MaxHp] = 0,
    [BattleAttriType.MaxMp] = 1,
    [BattleAttriType.Attack] = 2,
    [BattleAttriType.Defense] = 3,
    [BattleAttriType.Agility] = 4,
    [BattleAttriType.Mental] = 5,
    [BattleAttriType.Recovery] = 6,
    [BattleAttriType.Critical] = 7,
    [BattleAttriType.Hit] = 8,
    [BattleAttriType.Dodge] = 9,
    [BattleAttriType.CounterAttack] = 10,
    [BattleAttriType.MagicAttack] = 11,
    [BattleAttriType.MagicDefense] = 12,
    [BattleAttriType.CriticalDecValue] = 13,
    [BattleAttriType.CriticalIncValue] = 14,
    [BattleAttriType.AntiPosition] = 15,
    [BattleAttriType.AntiPetrify] = 16,
    [BattleAttriType.AntiSleep] = 17,
    [BattleAttriType.AntiChaos] = 18,
    [BattleAttriType.AntiDrunk] = 19,
    [BattleAttriType.AntiSilence] = 20,
    [BattleAttriType.AntiSevereInjuer] = 21,
}

QUERY_ATTR_TYPE = {
    [0] = BattleAttriType.MaxHp,
    [1] = BattleAttriType.MaxMp,
    [2] = BattleAttriType.Attack,
    [3] = BattleAttriType.Defense,
    [4] = BattleAttriType.Agility,
    [5] = BattleAttriType.Mental,
    [6] = BattleAttriType.Recovery,
    [7] = BattleAttriType.Critical,
}

-- 元素枚举
ElementType = {
    element_water = 40,-- 水元素值
    element_fire = 41, -- 火元素值
    element_wind = 42, -- 风元素值
    element_land = 43 -- 地元素值
}

ElementFactor = {
    [40] = {[40] = 0, [41] = 1, [42] = 0, [43] = -1},
    [41] = {[40] = -1, [41] = 0, [42] = 1, [43] = 0},
    [42] = {[40] = 0, [41] = -1, [42] = 0, [43] = 1},
    [43] = {[40] = 1, [41] = 0, [42] = -1, [43] = 0}
}

ElementSpriteName = {
    [BattleAttriType.ElementWater] = "ShuXing_Shui",
    [BattleAttriType.ElementFire] = "ShuXing_Huo",
    [BattleAttriType.ElementWind] = "ShuXing_Feng",
    [BattleAttriType.ElementEarth] = "ShuXing_Tu"
}

ElementSpriteType = {
    ShuXing_Shui = BattleAttriType.ElementWater,
    ShuXing_Huo = BattleAttriType.ElementFire,
    ShuXing_Feng = BattleAttriType.ElementWind,
    ShuXing_Tu = BattleAttriType.ElementEarth
}
Correction = {
    BattleAttriType.Critical,
    BattleAttriType.Hit,
    BattleAttriType.Dodge,
    BattleAttriType.CounterAttack,
    BattleAttriType.MagicAttack,
    BattleAttriType.MagicDefense,
    BattleAttriType.CriticalIncValue,
    BattleAttriType.CriticalDecValue
}

Resistance = {
    BattleAttriType.AntiPosition,
    BattleAttriType.AntiSleep,
    BattleAttriType.AntiPetrify,
    BattleAttriType.AntiChaos,
    BattleAttriType.AntiDrunk,
    BattleAttriType.AntiSilence,
    BattleAttriType.AntiSevereInjuer,
}

BattleAttriElement = {
    Water = BattleAttriType.ElementWater,
    Fire = BattleAttriType.ElementFire,
    Wind = BattleAttriType.ElementWind,
    Earth = BattleAttriType.ElementEarth
}

-- 克制该灵珠的灵珠属性，用于针对怪物属性的推荐
BattleRestraintElement = {
    [40] = BattleAttriType.ElementEarth,
    [41] = BattleAttriType.ElementWater,
    [42] = BattleAttriType.ElementFire,
    [43] = BattleAttriType.ElementWind
}

TIAN_MING_ADD_ATTR_SYS_TYPE ={
    INVALID = 0,
    PEI_SHI_HU_JING = 1,     -- 配饰护镜
    PEI_SHI_HUA_DAN = 2,     -- 配饰花钿
    PEI_SHI_JING_DAI = 3,    -- 配饰锦带
    PEI_SHI_XIANG_NANG = 4,  -- 配饰香囊
    PEI_SHI_XIU_JIAN = 5,    -- 配饰袖箭
    EQUIP_STRENGTHEN = 6,    -- 配饰全身装备强化
    GEM = 7,                 -- 配饰全身宝石
    EQUIPMENT = 8,           -- 配饰全身装备
    MAX = 8,
}

RoleSelectSurface = {
    None = 0,
    Body = 1,
    Arm = 2,
    Wing = 3,
    Mount = 4,
}

--加点属性
POINT_TYPE = {
    VITALITY = 0, --体力
    STRENGTH = 1, --力量
    POWER = 2, --强度
    AGILITY = 3, --敏捷
    MAGIC = 4 --魔法
}

--角色评分系数枚举，分为4类按照职业不同取不同类的系数
--物理系（剑、弓、气功、斧）
ROLE_SCORE_RATE = {
    --气功师
    [RoleProfessionType.QiGongShi] = {
        [BattleAttriType.Attack] = 0.71, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 0.3, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.06,
        [BattleAttriType.Critical] = 1.3, --必杀率（百分比）
        [BattleAttriType.Hit] = 1.1, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 1.1, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.12, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 1.3, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 1.86,--体力
        -- [BattleAttriType.STRENGTH] = 2.66,--力量
        -- [BattleAttriType.POWER] = 1.2,   --强度
        -- [BattleAttriType.AGILITY] = 2.4, --敏捷
        -- [BattleAttriType.MAGIC] = 0.26,   --魔法
    },
    --剑客
    [RoleProfessionType.JianXiaKe] = {
        [BattleAttriType.Attack] = 0.71, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 0.3, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.06,
        [BattleAttriType.Critical] = 1.3, --必杀率（百分比）
        [BattleAttriType.Hit] = 1.1, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 1.1, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.12, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 1.3, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 1.86,--体力
        -- [BattleAttriType.STRENGTH] = 2.66,--力量
        -- [BattleAttriType.POWER] = 1.2,   --强度
        -- [BattleAttriType.AGILITY] = 2.4, --敏捷
        -- [BattleAttriType.MAGIC] = 0.26,   --魔法
    },
    --狂战士
    [RoleProfessionType.KuangZhanShi] = {
        [BattleAttriType.Attack] = 0.71, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 0.3, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.06,
        [BattleAttriType.Critical] = 1.3, --必杀率（百分比）
        [BattleAttriType.Hit] = 1.1, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 1.1, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.12, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 1.3, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 1.86,--体力
        -- [BattleAttriType.STRENGTH] = 2.66,--力量
        -- [BattleAttriType.POWER] = 1.2,   --强度
        -- [BattleAttriType.AGILITY] = 2.4, --敏捷
        -- [BattleAttriType.MAGIC] = 0.26,   --魔法
    },
    --铁卫（长枪兵）
    [RoleProfessionType.TieWei] = {
        [BattleAttriType.Attack] = 0.71, --攻击
        [BattleAttriType.Defense] = 0.5, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 0.3, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.06,
        [BattleAttriType.Critical] = 1.3, --必杀率（百分比）
        [BattleAttriType.Hit] = 1.1, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 1.3, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.12, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 1.3, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 2.26,  --体力
        -- [BattleAttriType.STRENGTH] = 1.2,   --力量
        -- [BattleAttriType.POWER] = 2.66,     --强度
        -- [BattleAttriType.AGILITY] = 0.93,    --敏捷
        -- [BattleAttriType.MAGIC] = 0.26,     --魔法
    },
    --神射手
    [RoleProfessionType.ShenSheShou] = {
        [BattleAttriType.Attack] = 0.71, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 0.3, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.06,
        [BattleAttriType.Critical] = 1.3, --必杀率（百分比）
        [BattleAttriType.Hit] = 1.1, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 1.1, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.12, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 1.3, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 1.86,--体力
        -- [BattleAttriType.STRENGTH] = 2.66,--力量
        -- [BattleAttriType.POWER] = 1.2,   --强度
        -- [BattleAttriType.AGILITY] = 2.4, --敏捷
        -- [BattleAttriType.MAGIC] = 0.26,   --魔法
    },
    --星术师
    [RoleProfessionType.XingShuShi] = {
        [BattleAttriType.Attack] = 0.1, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 0.6, --回复
        [BattleAttriType.Mental] = 1.2, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.18,
        [BattleAttriType.Critical] = 0.2, --必杀率（百分比）
        [BattleAttriType.Hit] = 0.2, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 0.2, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.8, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 0.2, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 0.26,  --体力
        -- [BattleAttriType.STRENGTH] = 0.26,   --力量
        -- [BattleAttriType.POWER] = 1.86,     --强度
        -- [BattleAttriType.AGILITY] = 2.13,    --敏捷
        -- [BattleAttriType.MAGIC] = 2.53,     --魔法
    },
    --医师
    [RoleProfessionType.YiShuShi] = {
        [BattleAttriType.Attack] = 0.3, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 1.2, --回复
        [BattleAttriType.Mental] = 1, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.18,
        [BattleAttriType.Critical] = 0.2, --必杀率（百分比）
        [BattleAttriType.Hit] = 0.2, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 0.2, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.8, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 0.2, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 0.26,  --体力
        -- [BattleAttriType.STRENGTH] = 0.26,   --力量
        -- [BattleAttriType.POWER] = 1.86,     --强度
        -- [BattleAttriType.AGILITY] = 2.13,    --敏捷
        -- [BattleAttriType.MAGIC] = 2.53,     --魔法
    },
    --巫师
    [RoleProfessionType.FuZhouShi] = {
        [BattleAttriType.Attack] = 0.3, --攻击
        [BattleAttriType.Defense] = 0.45, --防御
        [BattleAttriType.Agility] = 0.45, --敏捷
        [BattleAttriType.Recovery] = 1.2, --回复
        [BattleAttriType.Mental] = 1, --精神
        [BattleAttriType.MaxHp] = 0.08,
        [BattleAttriType.MaxMp] = 0.18,
        [BattleAttriType.Critical] = 0.2, --必杀率（百分比）
        [BattleAttriType.Hit] = 0.2, --命中率（百分比）
        [BattleAttriType.Dodge] = 1.1, --闪避（百分比）
        [BattleAttriType.CounterAttack] = 0.2, --反击率（百分比）
        [BattleAttriType.MagicAttack] = 0.8, --魔攻
        [BattleAttriType.MagicDefense] = 0.45, --魔防
        [BattleAttriType.CriticalIncValue] = 0.2, --必杀伤害值
        [BattleAttriType.CriticalDecValue] = 1.1, --坚韧值
        [BattleAttriType.AntiPosition] = 0.5, --中毒抵抗 毒药的英语是poison！！！
        [BattleAttriType.AntiSleep] = 0.5, --睡眠抵抗(改为冰封抵抗)
        [BattleAttriType.AntiPetrify] = 0.5, --石化抵抗
        [BattleAttriType.AntiChaos] = 0.5, --混乱抵抗
        [BattleAttriType.AntiDrunk] = 0.5 --酒醉抵抗

        -- [BattleAttriType.VITALITY] = 0.26,  --体力
        -- [BattleAttriType.STRENGTH] = 0.26,   --力量
        -- [BattleAttriType.POWER] = 1.86,     --强度
        -- [BattleAttriType.AGILITY] = 2.13,    --敏捷
        -- [BattleAttriType.MAGIC] = 2.53,     --魔法
    }
}

--======================================场景枚举======================================
BoneName = {
    RightHand = "Bip001 R Hand",
    LeftHand = "Bip001 L Hand",
}

GameObjLayer = {
    Default = 0,
    UI = 5,
    QualityScene = 8,
    QualityRole = 9,
    ColliderScene = 10,
    ColliderRole = 11
}

SPECIAL_BATTLE = {
    Default = 0,
    --北斗七星
    BIG_DIPPER = 1,
    --殒神之地
    FALLEN_GOD = 2,
}

--11059协议用到
BATTLE_MODE = {
    --NULL ↓ 注释一般下一行为对应的ID
    BATTLE_MODE_NULL = 0,
    -- DEMO
    BATTLE_MODE_DEMO = 1,
    -- 普通怪物
    BATTLE_MODE_NORMAL_MONSTER = 2,
    -- 迷宫
    BATTLE_MODE_MAZE = 3,
    -- 魔窟
    BATTLE_MODE_WAVE = 4,
    -- 暗雷怪
    BATTLE_MODE_MINE_MONSTER = 5,
    -- 世界BOSS（在野外，可以锁定来打的）
    BATTLE_MODE_WORLD_BOSS = 6,
    -- 全服BOSS（讨伐玩法）
    BATTLE_MODE_SERVER_BOSS = 7,
    -- 真中的研究
    BATTLE_MODE_ZHEN_ZHONG = 8,
    -- 勇气的挑战(锢魔之塔)
    BATTLE_MODE_COURAGE_CHALLENGE = 9,
    -- 家族试炼BOSS p1: boss_seq
    BATTLE_MODE_GUILD_BOSS = 10,
    -- 副本（要记录血量的那种）
    BATTLE_MODE_CRYSTAL_FB = 11,
    -- 单人PVP切磋
    BATTLE_MODE_SINGLE_PVP_PRACTICE = 12,
    -- 宝图任务
    BATTLE_MODE_TREASURE_MAP = 13,
    -- 任务链
    BATTLE_MODE_TASK_CHAIN = 14,
    -- 运镖
    BATTLE_MODE_ESCORT = 15,
    -- 勋章任务
    BATTLE_MODE_MEDAL_TASK = 16,
    -- 高级藏宝图
    BATTLE_MODE_SUPER_MAP = 17,
    --密医 试炼之地 远征魔冢
    BATTLE_MODE_MI_YI = 18,
    --小队竞技战
    BATTLE_MODE_TEAM_SPORT = 19,
    --勇闯地宫
    BATTLE_MODE_BRAVE_GTOUND = 20,
    -- 家族大乱斗
    BATTLE_MODE_GUILD_FIGHT = 23,
    -- 竞技场
    BATTLE_MODE_ARENA = 24,
    --首席竞技
    BATTLE_MODE_CHIEF_ELECTION = 25,
    --家族荣誉战
    BATTLE_MODE_GUILD_HONOR_BATTLE = 27,
    --首席巅峰战
    BATTLE_MODE_CHIEF_PEAK_BATTLE = 28,
    --擂台战
    BATTLE_MODE_PLATFORM_BATTLE = 30,
    --挑战副本
    BATTLE_MODE_CHALLENGE_FB = 31,
    --成就晋升
    BATTLE_MODE_ACHIEVE = 29,
    --巨龙来袭
    BATTLE_MODE_SERVER_BOSS2 = 34,
    --与NPC操作战斗
    BATTLE_MODE_NPC_FIGHT = 35,
    --剧情战斗
    BATTLE_MODE_STORY = 36,
    --勋章副本
    BATTLE_MODE_MEDAL_FB = 37,
    --测试玩家增援
    BATTLE_MODE_TEST_BACKUP = 38,
    --四象幻境
    BATTLE_MODE_FANTASY_FB = 39,
    BATTLE_MODE_EXPEL_MONSTER = 40,
    BATTLE_MODE_JOYSEEKS = 41,
    --剑胆琴心
    BATTLE_MODE_JIAN_DAN_QIN_XIN = 42,
    BATTLE_MODE_ONLY_FIGHT = 43,
    --复活指引
    BATTLE_MODE_REALIVE_GUIDE = 44,
    --幻界战场
    BATTLE_MODE_BATTLEFIELD = 45,
    -- 师徒副本
    BATTLE_MODE_SHITU_FB = 47, 
    -- 首席预选
    BATTLE_MODE_CHIEF_PRIMARY = 48, 
    -- 山海降妖
    BATTLE_MODE_SHXY = 49, 
    -- 独闯贼营
    BATTLE_MODE_DU_CHUANG_ZEI_YING = 50,
    -- 捉拿狐妖
    BATTLE_MODE_CATCH_FOX = 51,
    -- 牛运当头
    BATTLE_MODE_COW_ORIGIN = 52,
    -- 梦渊笔录
    BATTLE_MODE_DREAM_NOTES_CHALLENGE = 53,
    -- 校场演练
    BATTLE_MODE_SCHOOL_EXERCISE = 54,
    -- 秘境奇闻
    BATTLE_MODE_RA_MIJINGQIWEN = 55,
    -- 境界突破
    BATTLE_MODE_REALM_BREAK = 56,
    -- 玉虚境
    BATTLE_MODE_MATERIAL = 57,
    -- 灵宠秘境
    BATTLE_MODE_PET_MONOPOLY = 58,
    -- 轮回忘川
    BATTLE_MODE_CHALLENGE_TEST = 59,
    -- 云端竞技（穹顶之争）
    BATTLE_MODE_CLOUD_ARENA = 60,
    -- 初始战斗
    BATTLE_MODE_INITIAL_FIGHT = 61,
    -- 幽冥炼狱
    BATTLE_MODE_NETHER_WORLD_PURGATORY = 62,
    -- 神印地
    BATTLE_MODE_PET_GOD = 63,
    --百鬼夜行
    BATTLE_MODE_GHOST_NIGHT = 64,
    --北斗七星
    BATTLE_MODE_BIG_DIPPER = 65,
    -- 初夏历练
    BATTLE_MODE_SUMMER_LI_LIAN = 66,
    -- 夏日迷宫
    BATTLE_MODE_MAZE_SUMMER = 67,
    -- 决斗
    BATTLE_MODE_DUEL = 68,
    -- 渡劫 九重天劫
    BATTLE_MODE_GO_THROUGH = 69,
    -- 境界战斗(心灵之境)
    BATTLE_MODE_REALM_BREAK_FIGHT = 71,
    --万灵奇闻
    BATTLE_MODE_SOULS_ANECDOTE = 72,
    --无尽之塔
    BATTLE_MODE_ENDLESS_TOWER = 73,
    --再战云泽
    BATTLE_MODE_CLOUD_SWAMP = 74,
    -- 山海争锋
    BATTLE_MODE_SHAN_HAI_ZHENG_FENG = 76,
    --万灵话本
    BATTLE_MODE_ALL_SOULS = 77,
    --琉璃蜃境
    BATTLE_MODE_MIRAGE_TERRITORY = 78,
    --婚宴副本
    BATTLE_MODE_WEDDING_FB = 79,
    --婚宴 保卫绣球
    BATTLE_MODE_BAO_WEI_XIU_QIU = 81,
    --殒神之地
    BATTLE_MODE_FALLEN_GOD = 82,
    --论剑九州
    BATTLE_MODE_WORLD_ARENA = 80,
    --英雄会武
    BATTLE_MODE_WORLD_TEAM_AREMA = 83,
    --妖行试炼
    BATTLE_MODE_YAO_XING_SHI_LIAN = 85,
    --山海灯会
    BATTLE_MODE_ML_LANTERN = 86,
    --天下第一
    BATTLE_MODE_UTA = 87,
    --鸿蒙天界
    BATTLE_MODE_HARMONY_HEAVEN = 88,
    --科举考试
    BATTLE_MODE_IMPERIAL_EXAM = 89,
    --我要上学
    BATTLE_GOTO_SCHOOL = 90,
    -- 上古遗迹
    BATTLE_MODE_ANCIENT_RUINS = 91,
    -- 精英试炼
    BATTLE_MODE_ELITE_TRAILS = 92,
    -- 弑神之塔
    BATTLE_MODE_NEW_COURAGE_CHALLENGE = 95,
    -- 小游戏战斗
    BATTLE_MODE_MINI_GAME_BATTLE = 96,
    -- 星官降临(世界boss3、天神降临)
    BATTLE_MODE_SERVER_BOSS3 = 97,
    -- 挑战类奇闻任务
    BATTLE_MODE_SPECIAL_ANECDOTE_TASK = 98,
    -- 组队挑战任务
    BATTLE_MODE_TEAM_CHALLENGE_TASK = 99,
	--神域占领神坛
    GOD_SPACE_OCCUPY_ALTAR = 101,
    --阵营神兽
    ZHEN_YING_SHEN_SHOU = 102,
    --归墟打玩家
    GUI_XU_ZHI_JING_PVP = 103,
    --归墟打BOSS
    GUI_XU_ZHI_JING_BOSS = 104,
    --龙脉普通小怪
    DRAGON_VEIN_MONSTER = 105,
    --龙脉BOSS怪保存血量
    DRAGON_VEIN_BOSS = 106,
    --云顶打玩家
    YUN_DING_TIAN_GONG_PVP = 107,
    --云顶打BOSS
    YUN_DING_TIAN_GONG_ATTACK_BOSS = 108,


	--三仙归洞(客户端自定)
    BATTLE_MODE_SANXIANGUIDONG = 998,
    --套圈纳吉（客户端自定）
    BATTLE_MODE_TAOQUANNAJI = 999,
}

BATTLE_MONSTER_GROUP = {
    FirstXinMo = 1006,  --第一次心魔战斗
    FirstRealmBreak = 40401,--心灵之境第一境
}

-- 副本类型和服务端对应 用于进出副本
FB_TYPE = {
    COURAGE_CHALLENGE = 1, -- 勇气的挑战
    MIYI = 2, -- 密医的阴谋
    CRYSTAL_FB = 3, -- 水晶副本
    MEDAL = 4, --勋章副本
    FIGHT_COLLECT = 5, -- 战斗采集
    MAZE_RUN = 6, -- 迷宫竞速
    RUSH_DUNGEON = 7, -- 勇闯地宫
    GUILD_FIGHT = 8, -- 家族大乱斗
    CHALLENGE_FB = 9, -- 挑战副本
    GUILD_ANSWER = 10, -- 家族答题
    ADVANCED_FB = 11,--进阶副本
    GUILD_HOME = 12, -- 家族驻地
    ARENA_MATCH = 13, -- 擂台战
    GuildFB = 14, -- 指引副本
    FANTASYFB = 15, -- 四象幻境
    ANIMAL_RACE = 16, --神兽径赛
    ONLY_FIGHT = 17,  --以一敌百
    SHITU_FB = 18, --师徒副本
    SHXY_FB = 19, --山海降妖副本
    DU_CHUANG_ZEI_YING = 20, --独闯贼营
    WEN_XIANG_LOU = 21, --温香楼
    ZHUA_NA_HU_YAO = 22, --抓拿狐妖
    SI_MING_XING_JUN = 23, --司命星君
    DREAM_NOTES = 24,--梦渊笔录
    TOMB_RAIDER = 28, -- 遗迹夺宝
    MAIN_CATCH_FOX = 29, --抓拿狐妖 主线
    PET_GOD = 31, --神印地
    GO_THROUGH = 32,-- 渡劫
    MI_JING_QI_WEN2 = 33,
    FISHING_GROUND = 34,--钓鱼佬的天堂
    ALL_SOULS_FB = 35,--万灵副本
    WEDDING_FB = 36,--婚宴副本
    HERO_VAN_BATTLE_FB = 37,--英雄会武
    SHI_TU_CHUANG_GONG = 38, -- 师徒传功
    MARRY_PARTY = 39, -- 情缘盛会
    LOVERS_MATCH = 40, --情缘对对碰
    SAN_XIAN_GUI_DONG = 41, -- 三仙归洞
    RABBIT_RACE = 42,   --兔子快跑
    TAO_QUAN_NA_JI = 43, -- 套圈纳吉
    U_T_ARENA = 44, -- 兔子快跑
    HARMONY_HEAVEN = 45, --鸿蒙天界
    GOTO_SCHOOL = 46,   --我要上学
}
--场景类型 与服务端对应
SCENE_TYPE = {
    COMMON_SCENE = 0, --普通（不销毁）
    GUAJI = 1, --挂机场景
    COURAGE_CHALLENGE = 2, --勇气挑战
    MIYI = 3, --密医
    CRYSTAL_FB = 4,--水晶
    TEAM_FIGHT = 5,--小队竞技
    MAZE_RACER = 6,--竞速迷宫
    GATHER_FB = 7,--采集场
    MAZE_WAIT = 8,--竞技等待
    BRAVE_GROUND = 9,--地宫
    GUILD_FIGHT = 10,--家族大乱斗
    CHIEF_ELECTION = 11,
    CHIEF_PEAK_BATTLE = 12,
    CHIEF_HONOR = 13,--荣誉战
    GUILD_ANSWER = 14,--家族答题
    CHALLENGE_FB = 15,--挑战副本
    ADVANCE_FB = 16,--觉醒
    PLATFORM_BATTLE = 17,--擂台
    GUILD_STATION = 18,--家族驻地
    GUIDE_FB = 19,-- 四象幻境
    FANTASY_FB = 21,--指引副本
    ONLY_FIGHT = 23,
    SHITU_FB = 24,  --师徒副本
    CHIEF_PRIMARY = 25,--首席预选
    DU_CHUANG_ZEI_YING = 27, --独闯贼营
    WEN_XIANG_LOU = 28, --温香楼
    SI_MING_XING_JUN = 30, --司命星君
    DREAM_NOTES = 31,--梦渊笔录
    SPLACE_ANECDOTE = 33, --秘境奇闻
    TOMB_RAIDER = 35,  -- 遗迹夺宝
    PET_GOD = 37,   -- 神印地
    GO_THROUGH = 38,-- 渡劫
    SOULS_ANECDOTE = 39,--万灵奇闻
    FISHING_GROUND = 40,--钓鱼
    ALL_SOULS_FB = 41,--万灵话本
    WEDDING_FB = 42,--婚宴副本
    HERO_VAN_BATTLE_FB = 45, -- 英雄会武
    MARRY_PARTY = 46, -- 情缘盛会
    SHI_TU_CHUANG_GONG = 47, -- 师徒传功
    LOVERS_MATCH = 48, --情缘对对碰
    SAN_XIAN_GUI_DONG = 49,--三仙归洞
    LUCK_RING = 51,--套圈纳吉
    --52 天下第一比武大会
    HARMONY_HEAVEN = 53, --鸿蒙天界
}
--9000~9999 为服务器按照普通场景处理、客户端需要特殊处理一些显示效果时用到的类型,
--！！！！！客户端特殊scene_type不能写到上面的SCENE_TYPE枚举中！！！
SCENE_TYPE_CLIENT_SPECIAL = {
        YouMingSeal = 9000,       --幽冥抓捕
}

--7、17、22可使用
GET_REASON_TYPE = {
    INVALID = 0,
    MONSTER_WAVE = 1,      --魔窟
    TEAMSURE_MAP_TASK = 2, --宝图任务完成获得物品
    CRYSTAL_FB_BOSS = 3,   --水晶副本 boss 奖励
    FANTASY_FB = 4,        --四象副本 boss 奖励
    XUN_BAO = 5,           --寻宝物品
    TASK = 6,              --任务道具
    RAND_GIFT = 8,         --随机礼包道具
    WILD_BOSS = 9,         --野外BOSS
    CRYSTAL_SWEEP = 10,    --扫荡水晶副本 p1经验 p2次数
    SHXY_BOSS = 11,        --山海降妖BOSS
    ANECDOTE_TASK = 12,    --奇闻任务
    DREAMNOTES_PASS = 13,  --梦渊挑战
    DREAMNOTES_SWEEP = 14, --梦渊扫荡
    TASK_CHAIN_REWARD = 15,--商人的烦恼
    EQUIP_DECOMPOSE = 16,  --装备分解
    PARTNER_LIENCE = 18,   --伙伴凭证
    MATERIAL = 19,         --玉虚境 param1:1.通关 0.扫荡
    FABAO_ACTED = 20,      --已经激活法宝获得道具
    PET_STAMP = 21,        --神印
    TIAN_DI_DAO_HEN = 23,  --天地道痕
    PRESTIGE_TASK = 24,    --声望任务
    GhostNight = 25,       --百鬼夜行
    RAND_ACTIVITY = 26,    --运营活动，p1:活动id
    ACTIVE_ROLLER = 27,    --活跃转盘
    TaskChain = 28,        --商人的烦恼阶段奖励
    GO_THROUGH = 29,       --渡劫
    REALM_BREAK_FIGHT = 30,--境界战斗(心灵之境)
    HORCRUX_TIANCHENG = 31,--魂器天成
    ENDLESS_TOWER = 32,    --无尽之塔
    CLOUD_SWAMP = 33,      --再战云泽
    YUNZEERSANSHI = 34,    --云泽二三事(抽奖结果)
    JIXINGGAOZHAO = 35,    --吉星高照
    ALL_SOULS_FB = 36,     --万灵话本
    BOUNTY_TASK_REWARD = 38,--赏金转盘结束总奖励展示
    TREASURE_TASK = 41,     --九州秘宝
    TREASURE_TASK_ALL = 42, --九州秘宝最后结算
    FALLEN_GOD = 43,        --殒神之地
    XIN_XIN_XIANG_YING = 44,--心心相印
    PARTNER_PACT = 45,      --伙伴元神契约获得提示
    SHITU_CHUANGONG = 46,   --师徒传功
    PUZZLE_FAIRY = 47,      --仙图重铸
    MS_LANTERN = 48,        --山海灯会
    MONEY_BOX = 49,         --货币红包
    LIANHUAN_TASK = 50,     --连环支线
    BUN_COME = 54,          --包子来了
    VITALITY_FIREWORK = 52, --元气烟火秀开启烟火物品
    VITALITY_CARD = 53,     --元气翻牌
    HARMONY_HEAVEN = 55,    --鸿蒙天界 p1:1boss奖励 2助战奖励
    GET_REASON_ZAO_HUA_DISCOMPOSE = 56, --造化装备分解
    DUAN_YANG_XIANG_NANG = 57,-- 端阳香囊
    SOUP_BATH = 58,          -- 兰汤初沐
    IMPERIAL_EXAM = 59,      --科举考试
    LING_BAO_DECOMPOSE = 60, --灵宝分解
    TIAN_YI_YAO_SHI = 60,   --天衣耀世
    LING_QI_ACT_CHANGE = 61,--激活灵骑道具转换
    YUE_XI_DENG_MI = 62, -- 月夕灯谜
    TEAM_CHALLENGE_REWARD = 63, -- 组队挑战胜利
    HUANSHOU_RESOLVE = 64,  --幻兽系统分解
}

CANCEL_REASONS = {
    CANCEL_REASON_FAIL = 0,
    CANCEL_REASON_SUCC = 1
}

--加点属性
RENDERER_MATERIAL_LAYER = {
    BASE = 0, --主材质，勿改（放在主材质上的为正数，反之为负数）
    XRAY = 10 --主角的X光遮罩
}
--======================================系统枚举======================================
ROLE_SETTING_TYPE = {
    SET_RESET = 0,
    SET_TOTAL_VOLUME = 1, -- 总音量 正数表示开，负数表示关
    SET_MUSIC_VOLUME = 2, -- 背景音乐音量 正数表示开，负数表示关
    SET_SOUND_VOLUME = 3, -- 音效音量 正数表示开，负数表示关
    SET_PICTUER_QUALITY = 4, -- 画质
    SET_NUMBER_OF_SCREENS = 5, -- 同屏人数
    SET_SHOW_OTHER_PLAYER = 6, -- 是否显示其他玩家
    SET_SHOW_OTHER_PLAYER_NAME = 7, -- 是否显示其他玩家名字
    SET_SHOW_ACTIVITY_MSG = 8, -- 是否开启限时活动推送
    -- SET_SHOW_SYSTEM_MSG = 9, -- 是否显示系统推送信息
    SET_SHOW_REFUSE_TEAM_REQUEST = 10, -- 是否拒绝组队邀请
    SET_SHOW_REFUSE_STRANGER_MSG = 11, -- 是否拒绝陌生人信息
    SET_AUTO_RESEARCH_TASK = 12, -- 是否自动三界
    SET_INFO_NOTICE_CHAT_GROUP_1 = 13, -- 是否提示群组1信息
    SET_INFO_NOTICE_CHAT_GROUP_2 = 14, -- 是否提示群组2信息
    SET_INFO_NOTICE_CHAT_GROUP_3 = 15, -- 是否提示群组3信息
    SET_INFO_NOTICE_CHAT_GROUP_4 = 16, -- 是否提示群组4信息
    SET_INFO_NOTICE_CHAT_GROUP_5 = 17, -- 是否提示群组5信息
    SET_INFO_IS_JOY = 18, -- 是否使用摇杆
    SET_INFO_IS_POWER = 19, -- 是否省电模式
    SYSTEM_SET_INFO_SHIELDING_FLOWERS = 20, -- 屏蔽送花特效        默认不屏蔽
    GUILD_ANSWER_SHOW_ROLE_NUM = 21,--家族答题显示人数
    SYSTEM_SET_INFO_BATTLE_CONTROL_MODE = 22,  -- 选择战斗控制模式  经典0 滑动1
    ROLE_SETTING_TYPE_MAX = 22,
}
SYSTEM_SET_BATTLE_CONTROL_MODE =
  {
    SYSTEM_SET_BATTLE_CONTROL_MODE_CLASSIC = 0,    -- 经典
    SYSTEM_SET_BATTLE_CONTROL_MODE_SLIDE = 1,    -- 滑动
    SYSTEM_SET_BATTLE_CONTROL_MODE_NUM = 2,
  }

RIDE_TYPE = {
    TYPE1 = 1,      --主角跨骑的坐骑
    TYPE2 = 2,      --暂无
    TYPE3 = 3,      --角色站姿的坐骑
    TYPE5 = 5,      --角色盘腿坐的坐骑
}

ICON_TYPE = {
    ITEM = 1,  --物品
    SKILL = 2, --技能
    MAP = 3,   --地图
    FABAO = 4, --法宝
    AVATAR = 5,--成就
    BUFF = 6,  --BUFF
    TITLE = 7, --头衔
    ACT = 8,   --活动图标
    BUBBLE = 9,--冒泡框
    TAIL = 10,--尾标
    HEAD = 11, --头衔
    ANECDOTE = 12, --奇闻华容道、拼图相关图片
    BIGFABAO = 13,--法宝大图标
    PARTNERGIFT = 14, -- 伙伴天赋
}

BETTER_CHECK_REASON = {
    BAG_CHANGE = 0,
    BAG_IN_CHANGE = 1,
    BAG_OUT_CHANGE = 2,
    EQUIP_CHANGE = 3,
    JEWERLY_CHANGE = 4,
    MEDAL_CHANGE = 5,
}

BAGITEM_CHANGEREASON = {
    NOLMAL_OR_PARAM = 0,--首次收包或者数据改变
    OUTSIDE_IN = 1, --外部获得 飘字
    INSIDE_OUT = 2, --内部消耗
    INSIDE_MOVE = 3, --内部移动
    CLEARUP = 4, --整理
    XUNBAO = 39
}

ROLE_STATUS = {
    INVALID = 0,
    IDLE = 1,
    MOVE = 2,
    FIGHT = 3,
    GATHER = 4,
    -- FOLLOW = 5
    FISHING = 5,
}

SYS_MSG_TYPE = {
    SYS_MSG_FLOAT = 0, -- 飘字
    SYS_MSG_ROLLING_NOTICE = 1, -- 滚动公告(跑马灯)
    SYS_MSG_SYSTEM_CHAT = 2, -- 系统聊天栏
    SYS_MSG_RECRUIT = 3, -- 招募栏
    SYS_MSG_WORLD_CHAT = 4, -- 世界聊天栏
    SYS_MSG_PROF_CHAT = 5, -- 职业频道
}

NPC_EXTRA_INFO_TYPE = {
    BRAVE_GROUND = 1 -- param1是使用次数
}

--======================================其他枚举======================================
AptitudeType = {
    ["str"] = 2,
    ["magic"] = 5,
    ["def"] = 3,
    ["con"] = 1,
    ["agi"] = 4
}

ChAnimType = {
    Idle = "idle",
    Run = "run",
    Death = "death",
    Behit = "behit",
    Attack = "attack",
    Skill = "skill",
    Defence = "defence",
    BattleRun = "battle_run",
    CounterAttack = "counter_attack",
    Gather = "gather",
    FallDown = "fall_down", --摔倒
    Run2 = "run_2", --刹车
    Show = "show",
    Show2 = "show2",
    Show3 = "show3",
    FaMu = "famu",
     --伐木
    WaKuang = "wakuang",
     --挖矿
    WeaponShow = "weapon_show", --武器展示
    ShouShang = "shoushang",--主角受伤
    XinMoIdle = "xinmoidle",--心魔待机
    XinMoPreWeak = "xinmopreweak",  --心魔出现剧情虚弱动画
    BianShen = "bianshen",
    Skill1 = "skill1",
    Skill2 = "skill2",
    Skill3 = "skill3",
    KunBang = "kunbang",
    --钓鱼
    FishIdle = "yugan",--2
    PaoGan = "paogan",
    FishWait = "fishwait",--3
    Lagan = "lagan",
    LaganIdle = "lagan_idle",--4
    ShouGan = "shougan"
}
FishAnim = {
    ["yugan"] = true,
    ["paogan"] = true,
    ["fishwait"] = true,
    ["lagan"] = true,
    ["lagan_idle"] = true,
    ["shougan"] = true,
}
--idle->paogan->wait->lagan->lagan_idle->shougan
AnimClip = {
    Loop = "d_loop",
    LoopPD = "d_piaodai_loop",
    Unloop = "d_unloop",
    UnloopPD= "d_piaodai_unloop"
}
AttrTypeForName = {
    ["gongji"] = BattleAttriType.Attack,    --攻击0
    ["fangyu"] = BattleAttriType.Defense,   --防御1
    ["minjie"] = BattleAttriType.Agility,   --速度2
    ["huifu"] = BattleAttriType.Recovery,   --回复3
    ["jingshen"] = BattleAttriType.Mental,  --精神4
    ["maxhp"] = BattleAttriType.MaxHp,  --最大hp 6
    ["maxmp"] = BattleAttriType.MaxMp,  --最大mp 8

    ["gedang"] = BattleAttriType.GeDang,    --格挡14
    ["bisha"] = BattleAttriType.Critical,   --必杀率（百分比）15
    ["fanji"] = BattleAttriType.CounterAttack,  --反击率（百分比）18
    ["mingzhong"] = BattleAttriType.Hit,    --命中率（百分比）16
    ["shanduo"] = BattleAttriType.Dodge,    --闪避（百分比）17

    ["def_zhongdu"] = BattleAttriType.AntiPosition, --中毒抵抗(千分比)30
    ["def_shihua"] = BattleAttriType.AntiPetrify,   --石化抵抗(千分比)32
    ["def_hunshui"] = BattleAttriType.AntiSleep,    --睡眠抵抗(千分比)(改为冰封抵抗)31
    ["def_jiuzui"] = BattleAttriType.AntiDrunk,     --酒醉抵抗(千分比)34
    ["def_hunluan"] = BattleAttriType.AntiChaos,    --混乱抵抗(千分比)33
    ["def_chenmo"] = BattleAttriType.AntiSilence,   --沉默抵抗(千分比)35
    ["def_zhongshang"] = BattleAttriType.AntiSevereInjuer,  --重伤抵抗(千分比)38

    ["mogong"] = BattleAttriType.MagicAttack,   --魔攻19
    ["mokang"] = BattleAttriType.MagicDefense,  --魔防20
    ["bisha_dmg"] = BattleAttriType.CriticalIncValue,   --必杀伤害值21
    ["jianren"] = BattleAttriType.CriticalDecValue, --坚韧值22

    ["vit"] = BattleAttriType.VITALITY, --体力45
    ["str"] = BattleAttriType.STRENGTH, --力量46
    ["pow"] = BattleAttriType.POWER,    --强度47
    ["agi"] = BattleAttriType.AGILITY,  --敏捷48
    ["mag"] = BattleAttriType.MAGIC,    --法术49

    ["con"] = BattleAttriType.VITALITY, --体力45
    ["def"] = BattleAttriType.POWER,    --强度47
    ["magic"] = BattleAttriType.MAGIC,    --法术49

    ["phy_inc"] = BattleAttriType.PhysicsIncrease,  --物理增伤11
    ["mag_inc"] = BattleAttriType.MagicIncrease,    --法术增伤12
    ["zhiliao"] = BattleAttriType.HealIncrease,     --治疗 13
    ["phy_red"] = BattleAttriType.PhysicsReduce,    --物伤减免9
    ["mag_red"] = BattleAttriType.MagicReduce,  --法伤减免10
    ["xiaozhong"] = BattleAttriType.XiaoGuoMingZhong,   --效果命中（百分比）23
    ["xiaodi"] = BattleAttriType.AntiEffect,   --效果抵抗（百分比）26
    ["zhenshang"] = BattleAttriType.ZhenShiShangHai,    --真实伤害24
    ["fabao"] = BattleAttriType.FaBao,  --法爆
    ["fabao_dmg"] = BattleAttriType.FaBaoSahnghai,  --法爆伤害
}
AttrNameForType = {
    [BattleAttriType.Attack] = "gongji",
    [BattleAttriType.Defense] = "fangyu",
    [BattleAttriType.Agility] = "minjie",
    [BattleAttriType.Recovery] = "huifu",
    [BattleAttriType.Mental] = "jingshen",
    [BattleAttriType.MaxHp] = "maxhp",
    [BattleAttriType.MaxMp] = "maxmp",
    [BattleAttriType.Critical] = "bisha",
    [BattleAttriType.CounterAttack] = "fanji",
    [BattleAttriType.Hit] = "mingzhong",
    [BattleAttriType.Dodge] = "shanduo",

    [BattleAttriType.AntiPosition] = "def_zhongdu",
    [BattleAttriType.AntiPetrify] = "def_shihua",
    [BattleAttriType.AntiSleep] = "def_hunshui",
    [BattleAttriType.AntiDrunk] = "def_jiuzui",
    [BattleAttriType.AntiChaos] = "def_hunluan",
    [BattleAttriType.AntiSilence] = "def_chenmo",
    [BattleAttriType.AntiSevereInjuer] = "def_zhongshang",

    [BattleAttriType.MagicAttack] = "mogong",
    [BattleAttriType.MagicDefense] = "mokang",
    [BattleAttriType.CriticalIncValue] = "bisha_dmg",
    [BattleAttriType.CriticalDecValue] = "jianren",

    [BattleAttriType.PhysicsIncrease] = "phy_inc",
    [BattleAttriType.MagicIncrease] = "mag_inc",
    [BattleAttriType.PhysicsReduce] = "phy_red",
    [BattleAttriType.MagicReduce] = "mag_inc",
    [BattleAttriType.XiaoGuoMingZhong] = "xiaozhong",
    [BattleAttriType.ZhenShiShangHai] = "zhenshang",
}
SimpleAttrNameForType = {
    [BattleAttriType.Attack] = "gongji",
    [BattleAttriType.Defense] = "fangyu",
    [BattleAttriType.Agility] = "minjie",
    [BattleAttriType.Recovery] = "huifu",
    [BattleAttriType.Mental] = "jingshen",
    [BattleAttriType.MaxHp] = "maxhp",
    [BattleAttriType.MaxMp] = "maxmp",
}
function ChAnimType.IsLoopAnim(anim)
    return anim == ChAnimType.Idle or anim == ChAnimType.Run or anim == ChAnimType.Death or anim == ChAnimType.Gather or anim == ChAnimType.BattleRun or anim == ChAnimType.WaKuang or
        anim == ChAnimType.FaMu or anim == ChAnimType.FishIdle or anim == ChAnimType.FishWait or anim == ChAnimType.LaganIdle
end
function ChAnimType.IsDLoopAnim(anim)
    return anim == ChAnimType.KunBang or anim == ChAnimType.ShouShang or anim == ChAnimType.XinMoIdle or anim == ChAnimType.XinMoPreWeak
end

-----------------------------------------------------
-- 游戏中的枚举
-----------------------------------------------------
GameEnum = {
    TRADE_LIST_SHOW_MAX_NUM = 8, --交易列表显示最大数量
    SHOP_BUY_MAX_LIMIT_NUM = 99, --商店购买最大数量
    APPEARANCE_MAX_LEVEL_NUM = 6, --外观最大颜值等级
    TIPS_POPUP_MAX_SHOW_NUM = 1024---1344 --玩法说明最大显示多少个字符
}

ACTIVITY_SYSTEM_TYPE = {
    ACTIVE_SYSTEM_TYPE_SCHOOL_TASK = 1,         -- 1  学院任务
    ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG = 2,          -- 2  真中的研究(三界除魔)
    ACTIVE_SYSTEM_TYPE_ARRESTING_THIEVES = 3,      -- 3  追缉盗贼
    ACTIVE_SYSTEM_TYPE_WST_QUESTION = 4,        -- 4  万事通的考验 / 神算子的考验
    ACTIVE_SYSTEM_TYPE_TREVI_FOUNTAIN = 5,        -- 5  法兰城许愿池 / 许愿池
    ACTIVE_SYSTEM_TYPE_DEMON_CAVE = 6,          -- 6  魔窟（万妖谷)
    ACTIVE_SYSTEM_TYPE_ESCORT = 7,            -- 7  运镖
    ACTIVE_SYSTEM_TYPE_BOUNTY_TASK = 8,          -- 8  赏金任务（门派任务）
    ACTIVE_SYSTEM_TYPE_TRANSACTION_TASK = 9,      -- 9  购买任务 - 丝绸之路
    ACTIVE_SYSTEM_TYPE_FANTASY_FB = 10,          -- 10 四象幻境
    ACTIVE_SYSTEM_TYPE_JINGJICHANG = 11,          -- 11 参与竞技场
    ACTIVE_SYSTEM_TYPE_COURAGE_CHALLENGE = 12,      -- 12 参与锢魔之塔
    ACTIVE_SYSTEM_TYPE_MIYI = 13,            -- 13 远征魔冢(试炼之地)
    ACTIVE_SYSTEM_TYPE_ENERGY = 14,            -- 14 体力消耗
    ACTIVE_SYSTEM_TYPE_CLOUD_ARENA = 15,          -- 15 云端竞技
    ACTIVE_SYSTEM_TYPE_LUNHUIWANGCHUAN = 16,        -- 16 轮回忘川
    ACTIVE_SYSTEM_TYPE_DREAM_NOTES_CHALLENGE = 17,    -- 17 梦渊笔录
    ACTIVE_SYSTEM_TYPE_SHAN_HAI_BOSS = 18,        -- 18 山海降妖
    ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU = 19,        -- 19 幻境试炼(精英试炼)
    ACTIVE_SYSTEM_TYPE_NETHER_WORLD_PURGATORY = 20,  --幽冥炼狱
    ACTIVE_SYSTEM_TYPE_PERSTIGE_TASK = 21,          --声望任务
    ACTIVE_SYSTEM_TYPE_BIG_DIPPER = 22,             --北斗七星
    ACTIVE_SYSTEM_TYPE_WAN_LING_FB = 23,            --万灵话本
    ACTIVE_SYSTEM_TYPE_TREASURE_TASK = 24,          --九州秘宝
    ACTIVE_SYSTEM_TYPE_FLY_UP_TASK = 25,            --飞升任务          
}

-----------------------------------------------------
-- 活动枚举 KF开服 / RAND随机 / CROSS跨服
-----------------------------------------------------
ACTIVITY_TYPE = {
    KF_OPEN_ROLLER = 2051,        --开服转盘(屏蔽)
    KF_LIMIT_SHOP = 2052,         --折扣商店
    KF_WORD_GIFT = 2068,          --集字活动
    KF_LUCK_ROLLER = 2076,        --幸运转盘
    KF_ATHLETICS = 2079,          --新服竞技
    KF_COMPETITION_RANK = 2133,   --新服比拼

    RAND_TIME_LIMIT_BUY = 2049,   --商店限时购买(废弃)
    RAND_CONSUMER_CAROUSEL = 2055,--消费转盘
    RAND_LUSTRATE_NIAN = 2056,    --驱除妖兽
    RAND_HERITAGE_TREASURE = 2057,--传承瑰宝
    RAND_COLORFUL_ILLUSTRATE=2058,--七彩图鉴
    RAND_SALE_SHOP = 2059,        --特卖商店
    RAND_THRIVING_SHOP = 2060,    --欣欣向荣
    RAND_ANIMAL_RACE = 2061,      --神兽径赛
    RAND_FORTUNE_BAG = 2062,      --奖励福袋
    RAND_TAX_DISCOUNT= 2063,      --税率优惠
    RAND_HEAVEN_GIFT = 2064,      --天降好礼
    RAND_SEAL_PROMOTE= 2065,      --封印提升
    RAND_DROP_DOUBLE = 2066,      --掉落双倍
    RAND_DISCOUNT_CARNIVAL = 2069,--折扣狂欢
    RAND_PARTNER_EXCHANGE = 2070, --伙伴兑换
    RAND_LUCKY_DISCOUNT = 2071,   --幸运折扣    
    RAND_PET_EXCHANGE = 2072,     --宠物兑换
    RAND_SWORD_ZITHER = 2073,     --剑胆琴心(幻境奇遇)
    RAND_ADVENTURE_GIFT = 2074,   --奇遇礼包
    RAND_LINKAGE_GIFT = 2075,     --联动奖励
    RAND_DIVINATION = 2077,       --天命卜卦
    RAND_QM_DISCOUNT = 2078,      --全民团购
    RAND_ILLUSTRATE_TREASURE=2080,--图鉴寻宝
    RAND_GOD_TOKEN = 2081,        --封神战令
    RAND_ELEMENT_CARDS = 2082,    --元素翻牌
    RAND_SIGN_IN_CONTINUITY=2083, --连登好礼
    RAND_WEALTH_CORNUCOPIA = 2084,--财神聚宝
    RAND_CUSTOMIZED_TREASURE=2085,--定制宝箱
    RAND_EXCLUSIVE_GIFT = 2086,   --专属礼包
    RAND_EXP_PURCHASE = 2087,     --经验飙升
    RAND_GIFT_BOX = 2088,         --限时宝盒
    RAND_CHAIN_BOX = 2089,        --限时宝袋
    RAND_GIVE_PET_DRAW = 2090,    --赠30连抽
    RAND_FAREWELL_ASK_NEO = 2091, --辞旧迎新
    RAND_COW_GOING_LUCK = 2092,   --牛行大运
    RAND_SPRING_STORE = 2093,     --年货节
    RAND_COW_ORIGIN = 2094,       --牛运当头
    RAND_COW_CONVERT = 2095,      --牛运兑换
    RAND_COW_STORE= 2096,         --牛运商城
    RAND_COW_LUCKY_EXCHANGE= 2097,--牛运无边
    RAND_LITTLE_YEAR_GIFT = 2098, --小年送礼
    RAND_GIVE_SECRET_KEY = 2099,  --赠寻宝钥匙
    RAND_ACC_RECHARGE = 2100,     --累计充值
    RAND_PET_LOOT = 2101,         --宠物夺宝
    RAND_DEAL_DISCOUNT = 2102,    --交易行折扣
    RAND_PET_BEFALL = 2103,       --神兽降临
    RAND_GIFT_ROTATION = 2104,    --好礼转动
    RAND_SECRET_TREASURE = 2105,  --特惠秘宝
    -- RAND_HELP_OTHERS = 2106,   --义胆助人
    RAND_LUCKY_MAZE = 2107,       --幸运迷宫
    RAND_SPRING_CLIMB = 2108,     --春日登峰
    RAND_SPlace_Anecdote = 2109,  --秘境奇闻
    RAND_LABOUR_GIFT = 2110,      --劳动好礼
    RAND_COMBINE_GIFT = 2111,     --组合贺礼
    RAND_HEAVEN_SENT = 2112,      --令牌天赐
    RAND_FASHION_SHOP = 2113,     --时装商店
    RAND_ONE_YUAN_BUY = 2114,     --一元购
    RAND_IMMORTAL_FUND = 2115,    --灵玉基金
    RAND_HEART_SKILL = 2116,      --心法无量
    RAND_LUCKY_SIGN = 2117,       --好运花签
    RAND_BANQUET = 2118,          --琼楼玉宴
    RAND_SUMMER_DISCOUNT_SHOP=2119,--超级折扣商城
    RAND_YUAN_QI_TURNING = 2120,  --元气翻牌
    RAND_COOL_BREEZE = 2121,      --凉风送爽
    RAND_LOTUS = 2122,            --荷香觅宝
    RAND_SUMMER_MAZE = 2123,      --夏日迷宫
    RAND_SUMMER_EXPERIENCE = 2124,--初夏历练
    RAND_FLOWER_LAMP = 2125,      --多彩花灯
    RAND_NATIONAL_MIBAO_STORE=2126,--觅宝坊店
    RAND_SUMMER_FIRST_SHOP = 2127,--夏日商城
    RAND_PRECIOUS_STORE = 2128,   --珍品行铺
    RAND_PAI_MAI_HANG = 2129,     --拍卖行
    RAND_WORLD_SOUL = 2130,       --赋世魂器
    RAND_FUND_EXCHANGE = 2131,    --灵玉基金兑换
    RAND_REGRESS_REWARD = 2132,   --回归豪礼
    RAND_HAPPY_LINKUP = 2134,     --连连乐翻天
    RAND_SIGN_HAPPY = 2135,       --登陆连好礼
    RAND_WAN_LING_GONG_YING =2136,--万灵共盈
    RAND_HELP_OTHERS = 2137,      --桃园一三事
    RAND_SOULS_ANECDOTE = 2138,   --万灵奇闻
    RAND_HORCRUX_TIANCHENG = 2139,--魂器天成

    RAND_MOUNTAIN_SEA_WIND_MOON = 2140, --山海风月
    RAND_MOUNTAIN_SEA_FIRST_MEET = 2141,--山海初遇
    RAND_MOUNTAIN_SEA_QUESTION = 2142,  --山海问答
    RAND_MOUNTAIN_SEA_MINEC = 2143,     --山海聚宝
    RAND_MOUNTAIN_SEA_MEET = 2144,      --山海相逢
    RAND_MOUNTAIN_SEA_TASK = 2145 ,     --山海缘起

    RAND_ZHAOCAIJINBAO = 2146,       --招财进宝
    RAND_YUN_ZE_TERRITORY = 2147,    --云泽画境
    RAND_YUN_ZE_BLOOMING_RECORD=2148,--云泽繁华录
    RAND_SAME_FLOWER = 2149,         --分享同花礼
    RAND_CLOUD_SWAMP = 2150,         --再战云泽
    RAND_MOZUZAINALI = 2151,         --魔族在哪里
    RAND_SUPER_VALUE_ENJOY = 2152,   --超值纷享
    RAND_YUN_ZE_ER_SAN_SHI = 2153,   --云泽二三事
    RAND_ENJOY_REDOUBLE = 2154,      --纷享加倍
    RAND_SUMMER_EXPERIENCE_TWO=2155, --山海争锋
    RAND_LOTUS_MS = 2156,            --山海觅宝
    RAND_SEA_CUSTOMIZED_BOX = 2157,  --山海宝箱
    RAND_SEA_GIFT_BOX = 2158,        --山海宝盒
    RAND_SEA_CHAIN_BOX = 2159,       --山海宝袋
    RAND_MS_FLOWER_LAMP = 2160,      --山海花灯
    
    RAND_YUN_ZE_CUSTOMIZED_BOX=2161, --云泽宝箱
    RAND_YUN_ZE_GIFT_BOX = 2162,     --云泽宝盒
    RAND_YUN_ZE_CHAIN_BOX = 2163,    --云泽宝袋
    RAND_JIXINGGAOZHAO = 2164,       --吉星高照
    RAND_YZ_WISH = 2165,             --云泽祈愿
    RAND_YZ_NICE_GIFT = 2166,        --云泽好礼
    RAND_MENG_CHONG_JIA_DAO = 2167,  --萌宠驾到
    RAND_ELVES_CATCH = 2168,         --精灵大抓捕
    RANK_LIMIT_RECHARGE = 2169,      --限时累充
    RANK_WEEK_GIFT = 2170,           --周末限好礼
    RANK_CHAOZHI_GIFT = 2171,        --超值献礼
    RAND_JING_JI_YUE_KA = 2172,      --竞技月卡
    RAND_NIAN_NIAN_YOU_YU = 2173,    --年年有鱼
    RAND_TIGER_FIND_SPRING = 2174,   --寅虎探春
    RAND_TIGER_MORA = 2175,          --寅虎利是(寅虎逗新春)

    RAND_TIGER_GIFT_MONEY = 2176,    --寅虎压岁
    RAND_TIGER_BORN = 2177,          --寅虎问世
    RABD_TIGER_NEW_COW_OLD = 2178,   --辞牛迎新虎
    RAND_TIGER_GREAT_LUCK = 2179,    --虎行大运
    RAND_TIGER_THANK = 2180,         --寅虎拜岁
    RAND_AUSPICIOUS_SNOW_YEAR = 2181,--瑞雪丰年
    RAND_EQUIP_SMELT = 2182,         --熔炼有礼
    KF_LOVE_FASHION = 2183,          --情缘时装
    RAND_MARRY_RECHARGE = 2184,      --情缘累充
    RAND_ANIMAL_WORLD = 2185,        --异兽临世
    RAND_SKY_LANTERN = 2186,         --祈愿孔明灯
    RAND_YUAN_QI_SPEED = 2187,       --元气竞速
    RAND_VITALITY_HELP_OTHERS=2188,  --元气助人
    CATCH_BUTTERFLY = 2189,          --寻梦蝴蝶
    RAND_SOW_REWARD = 2190,          --播种有礼
    VITALITY_FLOP = 2191,            --元气翻牌
    RAND_BUN_COME = 2192,            --包子来了
    RAND_ALL_SOUL_COST = 2193,       --万灵筑礼
    RAND_JIN_YI_TIAN_XIANG = 2194,   --锦衣天香
    RAND_ARTICULATELY = 2195,        --妙笔生花
    RAND_VITALITY_FIREWORK = 2196,   --元气烟火秀
    RAND_VITALITY_NOTE = 2197,       --元气记事簿
    RAND_BRAVING = 2198,             --乘风破浪
    RAND_REED_INCENSE_CITY = 2199,   --粽香满城
    DRAGON_COLLECT = 2203,          -- 苍龙赐福
    LUCK_DRAW_BOX = 2204,           --幸运盲盒
    RAND_SWEET_SALTY_WAR = 2200,     --甜咸之争
    RAND_DUAN_YANG_XIANG_NANG = 2201,-- 端阳香囊
    RAND_DUANYANGZIXUANLI = 2202,    -- 端阳自选礼
    RAND_SOUP_BATH = 2205,           --兰汤沐浴
    RAND_THREE_HUNDRED_SKIN = 2206,  --300抽皮肤
    RAND_DUANYANGFULI = 2207,        -- 端阳福礼
    RAND_JIN_LONG_JU_BAO = 2208,     --金龙聚宝
    RAND_CHONGJIFANLI = 2209,        --宠技返利
    RAND_GOD_BEAST_GRACE = 2210,     --神兽恩泽
    RAND_PET_STR = 2211,             --宠物强化
    RAND_SEVEN_BUY = 2212,           --七巧特惠
    RAND_MEILIQIXI = 2213,           --魅力七夕
    RAND_FOUR_SEASON_CARD = 2214,    --四季花牌
    RAND_FOUR_SEASON_BOX = 2215,     --四季宝盒
    RAND_FOUR_SEASON_CHEST = 2216,   --四季宝箱
    RAND_NEW_SERVER_PET = 2217,      --新服灵宠
    RAND_GOD_CLOTHES = 2218,         --神赐华服
    RAND_TIAN_YI_YAO_SHI = 2219,     -- 添衣耀世
    Rand_HUN_QI_FAN_LI = 2220,       --魂器返利
    --中秋活动
    RAND_YUE_XI_DENG_MI = 2221,         -- 月夕灯谜
    RAND_YUE_XI_LEI_CHONG = 2222,       -- 月夕累充
    RAND_ZHUI_YUE_SHANG_CHENG = 2224,   -- 追月商城
    RAND_SINGLE_REBATE = 2223,          -- 单笔返利
    RAND_GIVE_HUNDRED = 2225,           -- 百抽提示
    --万圣活动
    RAND_HALLOWEEN_PRAY = 2226,         -- 万圣祈福
    RAND_WAN_SHENG_LEI_CHONG = 2227,    -- 万圣累充
    RAND_LUCK_NUM = 2228,               -- 幸运数字
    RAND_WAN_SHENG_SHOP = 2229,         -- 万圣商城

    RAND_TX_TeQuanDaTing = 2230,        -- 腾讯特权大厅
    RAND_SHENG_DAN_MI_LING = 2231,      -- 圣诞密令
    RAND_STAR_LIGHTING = 2235,          -- 星星点灯
    RAND_ZHOU_MO_LIAN_CHONG_LI = 2233,      -- 周末连充礼

    RAND_YING_CHUN_NA_FU = 2236,   --迎春纳福
    RAND_FU_YUN_SHANG_CHENG = 2234,     --福运商城

    RAND_DA_ZHAN_HONG_TU = 2237     --大展宏图（兔）
}

-- 活动对应的虚拟道具
ACT_VIRTUAL_ITEM = {
    [ACTIVITY_TYPE.RAND_COW_CONVERT] = 65624,           -- 牛运积分
    [ACTIVITY_TYPE.RAND_COW_LUCKY_EXCHANGE] = 65625,    -- 牛运玫瑰
}
-- 夏日活动显示获取奖励弹窗的活动
ACT_SUMMER_SHOW_ITEM_DIALOG = {
    [ACTIVITY_TYPE.RAND_LOTUS] = 1
}

-- 山海活动显示奖励弹窗的活动
ACT_M_S_SHOW_ITEM_DIALOG = {
    -- [ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_WIND_MOON] = 1,
    [ACTIVITY_TYPE.RAND_LOTUS_MS] = 1
}
----------------------
--不阻碍自动任务的界面
NotHinderList = {
    ["LevelUpView"] = 1,
    ["AttrChange"] = 1,
    ["UnlockNewMapView"] = 1,
    ["AchievementPopupView"] = 1,
    ["PowerSavingView"] = 1,
}
--阻碍自动任务的界面
HinderList = {
    ["NpcDialogView"] = 1,
    ["RoleSelectProfessionView"] = 1,
    ["PetGain"] = 1,
    ["GatherView"] = 1,
    ["WaitView"] = 1,
    ["DialogTipsView"] = 1,
    ["StoreView"] = 1,
    ["FinishFailView"] = 1,
    -- ["FightView"] = 1,
    ["ItemsSubView"] = 1,
    ["ArenaMatchRankView"] = 1,
}

--不因Lu打开被关闭的Lt界面*xiaoyouxi*
LtNotJoinLuLogic = {
    ["GuideView"] = 1,
    ["LevelUpView"] = 1,
    ["PowerSavingView"] = 1,
    ["GuildHongBaoTipView"] = 1,
    ["OpenRedbagView"] = 1,
    ["TradeRoadView"] = 1,
    ["TradeRoadItemShowView"] = 1,
    ["TradeRoadMapView"] = 1,
    ["TowerAdventureLevelView"] = 1,
    ["TowerAdventureGameView"] = 1,
    ["TowerAdventureResultView"] = 1,
    ["CowBoyLevelView"] = 1,
    ["CowBoyGameView"] = 1,
}

-- 小游戏的所有界面*xiaoyouxi*
MiniGameAllView = {
    ["TowerAdventureLevelView"] = 1,
    ["TowerAdventureGameView"] = 1,
    ["TowerAdventureProfSelectView"] = 1,
    ["TowerAdventureResultView"] = 1,
    ["CowBoyLevelView"] = 1,
    ["CowBoyGameView"] = 1,
    ["CowBoyWinView"] = 1,
    ["CowBoyResultView"] = 1,
    ["CowBoyGetView"] = 1,
    ["CowBoyReturnView"] = 1,
    ["CowBoyProfSelectView"] = 1,
}

-- 这些Lt界面打开时不会自动任务*xiaoyouxi*
LtNotAutoTask = {
    ["TowerAdventureLevelView"] = 1,
    ["TowerAdventureGameView"] = 1,
    ["TowerAdventureResultView"] = 1,
    ["CowBoyLevelView"] = 1,
    ["CowBoyGameView"] = 1,
    ["NpcDialogView"] = 1,
}

NoCloseInBattle = {
    ["MainView"] = 1,
    ["GuideView"] = 1,
    ["TopLayerView"] = 1,
    ["LoadingView"] = 1,
    ["WaitView"] = 1,
    ["ChatView"] = 1,
    ["AttrChange"] = 1,
    ["DialogTipsView"] = 1,
    ["BagView"] = 1,
    --["Workshop"] = 1,
    ["GuildView"] = 1,
    ["PetView"] = 1,
    ["SocietyView"] = 1,
    ["RoleView"] = 1,
    --["PartnerView"] = 1,
    ["AchieveNewView"] = 1,
    ["StoreView"] = 1,
    ["MallView"] = 1,
    ["KeyUseView"] = 1,
    ["PetSelectView"] = 1,
    ["ChallengeTestView"] = 1,
    ["PowerSavingView"] = 1,
    ["DeepDarkFantasyView"] = 1,
    ["GuildHongBaoTipView"] = 1,
    ["OpenRedbagView"] = 1,
    ["HeroVanBattleStart"] = 1,
    ["EquipView"] = 1,
    ["GuildList"] = 1,
    --["IllustrateView"] = 1,
}
CloseInBattle = {
    ["FinishFailView"] = 1,
    ["FinishHurtView"] = 1,
    ["FinishPassViewAM"] = 1,
    ["FinishPassViewCT"] = 1,
    ["FinishPassViewGH"] = 1,
    ["FinishPassViewShiTuFB"] = 1,
    ["FinishPassView"] = 1,
    ["FinishFailNew"] = 1,
    ["ConspiracyFinish"] = 1,
    ["RushDungeonFinish"] = 1,
    ["CourageChallengeFinishAutoView"] = 1,
    ["ActFormationView"] = 1,
    ["YouMingSealView"] = 1,
}

NoCloseInStory = {
    ["MainView"] = 1,
    ["GuideView"] = 1,
    ["TopLayerView"] = 1,
    ["LoadingView"] = 1,
    ["WaitView"] = 1,
    ["AttrChange"] = 1,
    ["DialogTipsView"] = 1,
    ["GuildView"] = 1,
    ["StoryShowBossView"] = 1,
    ["PowerSavingView"] = 1,
    ["GuildHongBaoTipView"] = 1,
    ["OpenRedbagView"] = 1,
}
NoCloseInGuide = {
    ["MainView"] = 1,
    ["GuideView"] = 1,
    ["TopLayerView"] = 1,
    ["LoadingView"] = 1,
    ["WaitView"] = 1,
    ["ChatView"] = 1,
    ["AttrChange"] = 1,
    ["GuildHongBaoTipView"] = 1,
    ["OpenRedbagView"] = 1,
    ["BattleTips"] = 1,
    --["DialogTipsView"] = 1,
    --["BagView"] = 1,
    --["Workshop"] = 1,
    --["GuildView"] = 1,
    --["PetView"] = 1,
    --["SocietyView"] = 1,
    --["RoleView"] = 1,
    --["PartnerView"] = 1,
    --["AchieveNewView"] = 1,
    --["StoreView"] = 1,
    --["MallView"] = 1,
    ["KeyUseView"] = 1,
    --["PetSelectView"] = 1,
    --["IllustrateView"] = 1,
    ["PowerSavingView"] = 1,
    ["AnecdoteHintView"] = 1,
    ["TowerAdventureLevelView"] = 1,
    ["CowBoyLevelView"] = 1,
    ["ActiveView"] = 1,
}
NoCloseInGuideId = {
    [55] = 1,
    [83] = 1,
    [84] = 1,
    [90] = 1,
    [95] = 1,
    [103] = 1,
    [110] = 1,
    [111] = 1,
    [117] = 1,
    [123] = 1,
    [136] = 1,
    [139] = 1,
    [140] = 1,
    [148] = 1,
}
--通用物品ID
CommonItemId = {
    Exp = 65535, --经验
    Coin = 65532, --铜币
    SkillItem = 26073,--人物修炼丹
    GongXian = 65536,--家族贡献
    ChengHao = 65551, --称号
    BaoTu = 22966,
    FabaoSP = 26369,
    SealStone = 65550, --封印石
    ChangeAppear = 26682, --角色形象更换劵
    ChangeGuildName = 26057,
    XiangYaoBian = 24544,   --降妖鞭
    ShenShouSoul = 27187,  --神兽之魂
    JiuZhuanHuiHun = 23025, --九转回魂丹
    XuePing3000 = 22972, --3000生命药
    JieFengWan = 23026, --解封丸
    NomalKey = 26061, -- 小钥匙
    SilverKey = 26062, --银钥匙
    GoldKey = 26064, --金钥匙
    Gold = 65534,   --元宝
    Prestige = 65561,   --声望
    ArenaPoint = 65567,   --竞技场积分
    ArenaMatchPoint = 65568, --擂台战积分
    BattlefieldPoint = 65604,--幻界古币
    XianFen = 27478, --勋章分解道具
    Immortal = 65533, --仙币
    MapItem = 24556,--藏宝图
    SuperMapItem = 24557,--高级藏宝图
    QingXinDan = 23026,--清心丹
    PetExp = 65608,--宠物经验
    SilverCard = 26079,--纯银封印卡
    SingleDraw = 26812, -- 寻宝秘钥
    FifthCompany = 26813, -- 五连寻宝秘钥
    TiePetRope = 27540,-- 缚宠金绳
    TiePetRopes = 27541,-- 缚宠金绳5
    SkyGlass = 26073, -- 天引琉璃
    SkyGlassHundred = 22020, -- 天引琉璃 *100
    JadePendant = 26315, --琉璃玉
    SummerMazeTicket = 27611,--迷宫令
    LuckCoin = 65607, -- 气运值
    PartnerLevelItem = 65620, -- 元灵之髓
    LevelComplementExp = 65621, --等级助力
    FightCrystalEnergyItem = 25517, --秘境体力药剂
    ElementWater = 40000,--水灵珠
    ElementFire = 40001,--火灵珠
    ElementWind = 40002,--风灵珠
    ElementEarth = 40003,--地灵珠
    GuildMoney = 65605,--家族资金
    LeaderCoin = 65622,--队长工资
    MysteryFlush = 27575,--刷新令
    ActivePoint = 65626,--活跃值
    StrengthPoint = 65627,--体力
    BlueStone = 26412,--蓝色原石
    PurpleStone = 26413,--紫色原石
    OrangeStone = 26414,--橙色原石
    CloudArena = 65682,     --跨服竞技（穹顶之争）
    ImmortalCoin = 65698,--银币（灵玉）
    DustCoin = 65716,--粉尘（铸灵分解获得）
    SoulStoneId = 22007,--灵玉道具卡
    ImmortalCoinId = 22013,--灵玉道具卡
    MedalSpecialSpChange = 26492,  -- 天赐令牌转换券
    SilverFantasyKey = 65726, -- 银曜密令
    GoldFantasyKey = 65727, -- 金曜密令
    PartnerGiftNomal = 26499, -- 初级天赋精魂
    PartnerGiftNomalTen = 22055, -- 初级天赋精魂 *10
    PartnerGiftRare = 26500, -- 高级天赋精魂 
    PartnerGiftRareTen = 22057, -- 高级天赋精魂  *10
    LingLiZhi = 65733,--灵力值
    DestinyItem = 27830, -- 天命丹
    Anecdote = 27842, --奇闻令
    HorcruxTianchengKey = 26345,--花魂令
    RenMinBi = 99999,
    DianHuaShi = 27850, --点化石
    PartnerPactNormal = 65848,  --伙伴通用元神
    MayDayKey = 26359,  --元气(元气庆典)
    CanHun = 65878, -- 元魂残魂
    XingChenShi = 27594, --星辰石
    ShanHaiHuiJuan = 27595,--山海绘卷
    PetStampPrayItem = 56447, --宠物神印祺神道具
    RabbitCoinId = 56451, --追月商城抽奖道具
    PetGaiMingReset = 56503,--宠物改名重置丹
    GodSpaceMovition = 65919,-- 神域行动力
    GodSpaceExploits = 65906,-- 神域战功
    GodSpaceEssence = 65907,-- 神域精华
    HalloweenCoinId=26705,--万圣活动抽奖道具
    FairyDragon=65905, -- 龙魂碎片
    QiangHuaShi=56318, -- 强化石
    Diamond=65954,  --钻石
}

-- 获得物品来源
PUT_REASON_TYPE = {
    PUT_REASON_INVALID = 0, --!< 0 无效
    PUT_REASON_NO_NOTICE = 1, --!< 1 不通知
    PUT_PET_CONVERSION_EXP = 2, --!< 2 宠物经验转化
    PUT_PET_REBIRTH = 3, --!< 3 宠物返生
    PUT_REASON_GM = 4, --!< 4 GM命令
    PUT_REASON_DEPOSIT = 5, --!< 5 存入仓库
    PUT_REASON_WITHDRAW_STORAGE = 6, --!< 6 取出仓库
    PUT_REASON_WITHDRAW_TEMPORARY = 7, --!< 7 取出临时背包
    PUT_REASON_WITHDRAW_ALL_TEMPORARY = 8, --!< 8 取出临时背包全部
    PUT_REASON_PRESTIGE_BUY = 9, --!< 9 声望商店
    PUT_REASON_COLLECTION_ONCONVERT = 10, --!< 10 藏品兑换
    PUT_REASON_GIFT_RAND_ITEM = 11, --!< 11 随机礼包(大)
    PUT_REASON_TASK_REWARD = 12, --!< 12 任务报酬
    PUT_REASON_ACHIEVEMENT_REWARD = 13, --!< 13 成就
    PUT_REASON_DAY_REWARD = 14, --!< 14 奖励界面
    PUT_REASON_WORKSHOP_COMPOUND = 15, --!< 15 工坊合成
    PUT_REASON_GUILD_EXPLORE = 16, --!< 16 家族探险
    PUT_REASON_GUILD_BOSS = 17, --!< 17 家族Boss
    PUT_REASON_MAIL_RECEIVE = 18, --!< 18 邮件
    PUT_REASON_SYSTEM_NOTICE_REWARD = 19, --!< 19 功能预告奖励
    PUT_REASON_SYSTEM_COURAGE_CHALLENGE = 20, --!< 20 勇气挑战
    PUT_REASON_FIGHT_MINE_MONSTER = 21, --!< 21 打暗怪
    PUT_REASON_SYSTEM_MONSTER_WAVE = 22, --!< 22 魔窟
    PUT_REASON_GATHER_OBJECT = 23, --!< 23 采集
    PUT_REASON_GUILD_SHOP = 24, --!< 24 家族商店
    PUT_REASON_WILD_BOSS = 25, --!< 25 野外boss
    PUT_REASON_MEDAL = 26, --!< 26 勋章
    PUT_REASON_MEDAL_PUT_ON = 27, --!< 27 勋章穿戴或脱下
    PUT_REASON_MI_YI = 28, --!< 28 密医
    PUT_REASON_SCHOOL_TASK = 29, --!< 29 学院任务
    PUT_REASON_DROP = 30, --!< 30 掉落
    PUT_REASON_ESCORT = 31, --!< 31 运镖
    PUT_REASON_TREASURE_MAP = 32, --!< 32 宝图
    PUT_REASON_TRADE_MARKET = 33, --!< 33 交易所
    PUT_REASON_FIND_REWARD = 34, --!< 34 奖励找回
    PUT_REASON_TASK_CHAIN = 35, --!< 35 任务链
    PUT_REASON_FABAO_DECOMPOSE = 36, --!< 36 法宝分解
    PUT_REASON_CRYSTAL_FB = 37, --!< 37 水晶副本
    PUT_REASON_SHOP = 38, --!< 38 商店
    PUT_REASON_TREASURE_HUNT = 39, --!< 39 寻宝
    PUT_REASON_EXCHANGE = 40, --!< 40 兑换
    PUT_REASON_BRAVE_GROUND = 41, --!< 41 勇闯地宫
    PUT_REASON_RA_TIME_LIMIT_STORE = 42, --!< 42 随机活动-限时商店
    PUT_REASON_MARRIAGE_TASK = 43, --!< 43 情缘日常任务
    PUT_REASON_GUILD_FIGHT = 44, --!< 44 家族大乱斗
    PUT_REASON_COLOSSEUM = 45, --!< 45 竞技场
    PUT_REASON_GUILD_ANSWER = 46, --!< 46 家族答题
    PUT_REASON_OPTIONAL_GIFT = 47, --!< 47 自选礼包
    PUT_REASON_CHALLENGE_FB = 48, --!< 48 挑战副本
    PUT_REASON_TRADE_MARKET_OTHER = 49, --!< 49 交易所(非购买)
    PUT_REASON_DEF_GIFT = 50, --!< 50 固定礼包
    PUT_REASON_RAND_GIFT = 51, --!< 51 随机礼包
    PUT_REASON_CHONGZHI_FANLI = 52, --!< 52 充值返利
    PUT_REASON_FUNC_GUIDE = 53, --!< 53 功能引导 / 新手事件
    PUT_REASON_GUIDE_FB = 54, --!< 54 指引副本
    PUT_REASON_MAZERACER = 55, --!< 55 迷宫竞速
    PUT_REASON_TREVIFOUNTION = 56, --!< 56 许愿池
    PUT_REASON_LOW_TREASURE_MAP = 57, --!< 57 低级宝图
    PUT_REASON_NPC_SHOP = 58, --!< 58 npc随机商店
    PUT_REASON_CONVERT_SHOP = 59, --!< 59 兑换商店
    PUT_REASON_GIFT_OPTIONAL_ITEM = 60, --!< 60 自选礼包(大)
    PUT_REASON_CATCH_PET = 61, --!< 61 捕获宠物
    PUT_REASON_PET_ABANDONMENT_SHARDS = 62, --!< 62 宠物放生获取的碎片
    PUT_REASON_DECOMPOSE_SURFACE = 63, --!< 63 分解时装
    PUT_REASON_RA_TURN_TABLE = 64, --!< 64 随机活动-开服转盘
    PUT_REASON_RA_DISCOUNT_STORE = 65, --!< 65 随机活动-折扣商店
    PUT_REASON_CATCH_PET_SHARD = 66, --!< 66 捕获宠物得到碎片奖励
    PUT_REASON_RA_MYSTERIOUS_TRIAL = 67, --!<67 随机活动-神秘试炼
    PUT_REASON_GUILD_BOSS_CHAPTER_REWARD = 68, --!<68 家族试炼章节奖励
    PUT_REASON_RAND_CONSUMER_CAROUSEL = 69,--消费转盘
    PUT_REASON_KF_LUCK_ROLLER = 101,--幸运转盘
    -- PUT_REASON_MAX = 69 --!< 最大原因
    PUT_REASON_PARTNER_CRYTALS = 110, --!<110 伙伴元素水晶
    PUT_REASON_PARTNER_CRYTALS_BAD = 113,   --!<113 伙伴元素废水晶
    PUT_REASON_WORKSHOP_BATCH_COMPOUND =120,       --!< 120 工坊批量打造
    PUT_REASON_STAR_TREASURE_MAP = 151, --星辰宝图
    PUT_REASON_NOREXITEM_EQUIP = 156,   --!<156 未鉴定装备
    PUT_REASON_PET_BREAKTHROUGH = 157,  --宠物强化吃掉宠物
    PUT_REASON_REALM_BREAK = 168,       --境界系统突破奖励
    PUT_REASON_FA_BAO_DRAW_REWARD = 176, --法宝抽奖(天玑鼎)
    PUT_REASON_PET_STAMP = 195,         --宠物神印
    PUT_REASON_SPIRIT_RESOLVE = 198,    --铸灵分解
    PUT_REASON_HEART_SKILL_CHIP = 200,  --心法残页
    PUT_REASON_HEART_SKILL_CHANGE = 201,--心法转换
    PUT_REASON_SPIRIT_LEVEL_UP = 202,   --铸灵升级获得
    PUT_REASON_ONE_KEY_GRADE_MEDAL = 208, -- 一键升华令牌
    PUT_REASON_PET_WEAPON_CONVERT = 214,--魂器兑换
    PUT_REASON_PET_WEAPON_CHANGE = 230, --魂器转换
    PUT_REASON_ACTIVE_ROLLER = 232,--活跃转盘
    PUT_REASON_PET_BOOK_CHANGE = 239,--超级技能书置换
    PUT_REASON_MOUNTAIN_SEA_MINEC = 254,--山海聚宝
    PUT_REASON_ENDLESS_TOWER = 258,--无尽之塔
    PUT_REASON_RA_ZHAOCAIJINBAO = 261, --招财进宝抽奖
    PUT_SAME_FLOWER = 264,      --分享同花礼
    PUT_REASON_DIAN_HUA_SHI = 282,-- 点化石吸附
    PUT_REASON_LIVE_FISH = 283, --生活钓鱼
    PUT_REASON_LIVE_MAKE_ITEM = 284,   -- 生活制作药物
    PUT_REASON_AUSPICIOUS_SNOW_YEAR = 296, --瑞雪丰年
    PUT_REASON_FALLEN_GOD = 309, --殒神之地挑战奖励
    PUT_MULTI_PIECES = 312, --宠物碎片批量合成
    PUT_REASON_FALLEN_GOD_REWARD = 317, --殒神之地助战奖励
    PUT_REASON_WORLD_ARENA_WIN_BOX = 318,  --论剑九州胜场宝箱
    PUT_REASON_PET_STAMP_CHANGE = 319,  --神印置换
    PUT_REASON_PET_BAGDE_COM = 324,     --徽章碎片合成
    PUT_REASON_LUCKY_RING = 343,    -- 套圈纳吉
    PUT_REASON_RABBIT_RACE = 344,   -- 兔子快跑
    PUT_REASON_XUN_BAO_GUIDE_PARTNER = 351,     --寻宝伙伴指引
    PUT_REASON_XUN_BAO_GUIDE_PARTNER_2 = 352,   --寻宝伙伴指引2
    PUT_REASON_SKY_LANTERN = 353,       --祈愿孔明灯
    PUT_REASON_COUNT_REWARD = 359,  --元气翻牌次数奖励
    PUT_REASON_CARD_REWARD = 360,   --元气翻牌奖励
    PUT_REASON_JIN_YI_TIAN_XIANG = 363,   --锦衣天香
    PUT_REASON_VITALITY_FIREWORK = 368,   --元气烟火秀放烟火获得道具
    PUT_REASON_ZAO_HUA_DISCOMPOSE = 374,   --造化装备分解
    PUT_REASON_DUAN_YANG_XIANG_NANG = 382,          --端阳香囊
    PUT_REASON_LUCK_BOX = 385,          --幸运盲盒
    PUT_REASON_SOUP_BATH = 386,         --兰汤初沐
    PUT_REASON__DUAN_YANG_FU_LI = 388, --端阳福礼 
    PUT_REASON_LUCK_BOX_REWARD = 389,   --幸运盲盒累计充值奖励和购买的
    PUT_REASON_JIN_LONG_JU_BAO_DRAW = 390, -- 金龙聚宝抽奖
    PUT_REASON_JIN_LONG_JU_BAO_FETCH = 391, -- 金龙聚宝领取
    PUT_REASON_JIN_LONG_JU_BAO_BUY = 392, -- 金龙聚宝购买
    PUT_REASON_SPECIAL_ACTIVE_REWARD = 393, -- 特殊活跃奖励
    PUT_REASON_SCHOOL = 397,        -- 我要上学
    PUT_REASON_GOD_BEAST_GRACE = 400,--神兽恩泽抽奖道具
    PUT_REASON_GOD_EQUIP_WORK_SHOP = 404,--工坊神装产出
    PUT_REASON_RA_SHEN_SHOU_EN_ZE_FETCH = 401, --神兽恩泽领取自选奖励
    PUT_REASON_RA_SHEN_SHOU_EN_ZE_BUY = 402,-- 神兽恩泽购买
    PUT_REASON_RA_XIN_FU_QIANG_HUA = 403,-- 新服强化
    PUT_REASON_RA_QI_XI_TE_HUI = 405, --七夕特惠
    PUT_REASON_CHARISMAITC_TANABATA_REWARD = 406,-- 魅力七夕奖励
    PUT_REASON_XIAN_SHI_BAO_HE_3 = 407,-- 限时宝盒3
    PUT_REASON_XIAN_SHI_BAO_HE_4 = 408,-- 限时宝盒4
    PUT_REASON_RA_PET_EXCHANGE_BUY_ITEM = 409,-- 宠物兑换-商店购买  
    PUT_REASON_SHENG_QI_BOOK_CHANGE = 410,  --圣器技能书置换
    PUT_REASON_SHENGQI_TREASURE_MAP = 411,--圣器宝图


    PUT_REASON_FU_YU_TAKE_OFF = 414,--符玉卸下
    PUT_REASON_FU_YU_UPGRADE_SUCC = 415,--符玉强化成功
    PUT_REASON_FU_YU_UPGRADE_FAIL = 416,--符玉强化失败
    PUT_REASON_FU_YU_USE_GIFT = 417,--符玉礼包使用
    PUT_REASON_ELEMENT_ELF_REWARD = 418,--元素精灵奖励
    PUT_REASON_COLOSSEUM_BATTLE_REWARD = 419,--竞技场战斗结束

    PUT_REASON_PET_WEAPON_FANLI = 421, --魂器返利抽奖奖励

    PUT_REASON_LING_QI_ACT_CHANGE = 427, --激活灵骑道具转换

    PUT_BIYCCLE_SKILL_COMPOSE = 432,    -- 灵骑合成
    PUT_REASON_YUE_XI_DENG_MI = 435, --月夕灯谜
    PUT_REASON_ZHUI_YUE_SHANG_CHENG_BUY= 436, --追月商城 购买
    PUT_REASON_ZHUI_YUE_SHANG_CHENG_DRAW= 437, --追月商城 抽奖

    PUT_REASON_FU_YU_SUBLIME = 438, --符玉升华
    PUT_REASON_FU_YU_SWITCH = 439,  --符玉转化

    PUT_REASON_TEAM_CHALLENGE = 441, -- 组队挑战胜利

    PUT_REASON_HUANSHOU_ACTIVE_CONVERT = 442,   -- 幻兽激活转化
    PUT_REASON_HUANSHOU_LIAN_DAN = 443,         -- 幻兽系统炼丹
    PUT_REASON_HUANSHOU_RESOLVE = 444,          -- 幻兽系统分解
    PUT_REASON_GUILD_INBORN_RESET_RETURN = 449, -- 家族灵脉重置技能返还

    PUT_REASON_LING_BAO_TAKE_OFF = 450,--灵宝脱下
    PUT_REASON_LING_BAO_DECOMPOSE = 451,--灵宝分解
    PUT_REASON_GOD_DRAGON_PRAY = 455,      --神龙祈愿
    PUT_REASON_HALLOWEEN_PRAY = 484,       -- 万圣祈福抽奖
    PUT_REASON_SWITCH_EQUIP_OR_JEWELRY = 490,--装备或灵饰转换
    PUT_REASON_PET_AMULET_INSTENSIFY = 491, -- 护符技能强化
    PUT_REASON_YOU_MING_EQUIP_TAKE_OFF = 500,   --冥装脱下
    PUT_REASON_YOU_MING_COMPOUND = 501,         --幽冥合成
    PUT_REASON_YOU_MING_RESLOVE = 502,          --幽冥分解
    PUT_REASON_YOU_MING_TREASURE = 503,         --幽冥探宝
    PUT_REASON_BICYCLE_BOOK_ALL_UP = 505,       --灵骑技能书一键合成
    PUT_REASON_YOU_MING_SEAL = 507,             --幽冥捕捉
    PUT_REASON_YOU_MING_BATCH_COMPOSE = 510,    --幽冥批量分解
}

--经验变化的原因
ADD_EXP_REASON = {
    NORMAL = 0,
    GM = 1,
    TASK = 2,
    KILL_NORMAL_MONSTER = 3,
    QUESTION = 4,
    MAIL = 5,
    NOREX_ITEM = 6,
    SCHOOL_TASK = 7,
    FIND_REWARD = 8,
    TASK_CHAIN = 9,
    COLOSSEUM = 10,
    QUESTION_BUY_EXP = 11,
    TYPE_LEVEL_COMPLEMENT = 22, --等级助力
    OFFLINE_RECEIVE = 28,             --境界领取经验
    OFFLINE_BREAK = 29,               --境界突破经验
    ACHIEVEMENT_NUM = 33,             --成就阶段
    REALM_BREAK_FIGHT = 40,           --境界-心灵之境
    NEW_LEVEL_COMPLEMENT = 46,        --新等级助力
}

-- 在系统频道屏蔽以下获得物品来源
BlockChannelSystem = {
    [PUT_REASON_TYPE.PUT_REASON_INVALID] = true,
    [PUT_REASON_TYPE.PUT_REASON_NO_NOTICE] = true,
    [PUT_REASON_TYPE.PUT_REASON_DEPOSIT] = true,
    [PUT_REASON_TYPE.PUT_REASON_WITHDRAW_STORAGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_WITHDRAW_TEMPORARY] = true,
    [PUT_REASON_TYPE.PUT_REASON_WITHDRAW_ALL_TEMPORARY] = true,
    [PUT_REASON_TYPE.PUT_REASON_MEDAL_PUT_ON] = true,
    [PUT_REASON_TYPE.PUT_REASON_TRADE_MARKET_OTHER] = true,
    [PUT_REASON_TYPE.PUT_REASON_TEAM_CHALLENGE] = true,
}

-- 飘字屏蔽以下获得物品来源
BlockCenter = {
    -- [PUT_REASON_TYPE.PUT_REASON_INVALID] = true,
    [PUT_REASON_TYPE.PUT_REASON_NO_NOTICE] = true,
    [PUT_REASON_TYPE.PUT_REASON_GUILD_BOSS_CHAPTER_REWARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_CATCH_PET_SHARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_PARTNER_CRYTALS] = true,
    [PUT_REASON_TYPE.PUT_REASON_PARTNER_CRYTALS_BAD] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_BREAKTHROUGH] = true,
    [PUT_REASON_TYPE.PUT_REASON_REALM_BREAK] = true,
    [PUT_REASON_TYPE.PUT_REASON_FA_BAO_DRAW_REWARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_TASK_REWARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_STAMP] = true,
    [PUT_REASON_TYPE.PUT_REASON_SPIRIT_RESOLVE] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_CONVERT] = true,
    [PUT_REASON_TYPE.PUT_REASON_ACTIVE_ROLLER] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_CHANGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_BOOK_CHANGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_RA_ZHAOCAIJINBAO] = true,
    [PUT_REASON_TYPE.PUT_REASON_DIAN_HUA_SHI] = true,
    [PUT_REASON_TYPE.PUT_SAME_FLOWER] = true,
    [PUT_REASON_TYPE.PUT_REASON_AUSPICIOUS_SNOW_YEAR] = true,
    [PUT_REASON_TYPE.PUT_REASON_FALLEN_GOD] = true,
    [PUT_REASON_TYPE.PUT_MULTI_PIECES] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_STAMP_CHANGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_BAGDE_COM] = true,
    [PUT_REASON_TYPE.PUT_REASON_RABBIT_RACE] = true,
    [PUT_REASON_TYPE.PUT_REASON_LUCKY_RING] = true,
    [PUT_REASON_TYPE.PUT_REASON_XUN_BAO_GUIDE_PARTNER] = true,
    [PUT_REASON_TYPE.PUT_REASON_XUN_BAO_GUIDE_PARTNER_2] = true,
    [PUT_REASON_TYPE.PUT_REASON_SKY_LANTERN] = true,
    [PUT_REASON_TYPE.PUT_REASON_CARD_REWARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_COUNT_REWARD] = true,
    [PUT_REASON_TYPE.PUT_REASON_JIN_YI_TIAN_XIANG] = true,
    [PUT_REASON_TYPE.PUT_REASON_VITALITY_FIREWORK] = true,
    [PUT_REASON_TYPE.PUT_REASON_ZAO_HUA_DISCOMPOSE] = true,
    [PUT_REASON_TYPE.PUT_REASON_LUCK_BOX] = true,
    [PUT_REASON_TYPE.PUT_REASON_DUAN_YANG_XIANG_NANG] = true,
    [PUT_REASON_TYPE.PUT_REASON_JIN_LONG_JU_BAO_DRAW] = true,
    [PUT_REASON_TYPE.PUT_REASON__DUAN_YANG_FU_LI] = true,
    [PUT_REASON_TYPE.PUT_REASON_SCHOOL] = true,
    [PUT_REASON_TYPE.PUT_REASON_GOD_BEAST_GRACE] = true,
    [PUT_REASON_TYPE.PUT_REASON_GOD_EQUIP_WORK_SHOP] = true,
    [PUT_REASON_TYPE.PUT_REASON_SHENG_QI_BOOK_CHANGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_SHENGQI_TREASURE_MAP] = true,
    [PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_FANLI] = true,
    [PUT_REASON_TYPE.PUT_REASON_LING_QI_ACT_CHANGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_YUE_XI_DENG_MI] = true,
    [PUT_REASON_TYPE.PUT_REASON_ZHUI_YUE_SHANG_CHENG_DRAW]=true,
    [PUT_REASON_TYPE.PUT_REASON_TEAM_CHALLENGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_LING_BAO_DECOMPOSE] = true,
    [PUT_REASON_TYPE.PUT_REASON_GOD_DRAGON_PRAY] = true,
    [PUT_REASON_TYPE.PUT_REASON_HALLOWEEN_PRAY] = true,
    [PUT_REASON_TYPE.PUT_REASON_SWITCH_EQUIP_OR_JEWELRY] = true,
    [PUT_REASON_TYPE.PUT_REASON_YOU_MING_TREASURE] = true,
}

-- 快速使用屏蔽以下获得物品来源
BlockItemAutoUse = {
    [PUT_REASON_TYPE.PUT_SAME_FLOWER] = true,
    [PUT_REASON_TYPE.PUT_REASON_RABBIT_RACE] = true,
    [PUT_REASON_TYPE.PUT_REASON_XUN_BAO_GUIDE_PARTNER] = true,
    [PUT_REASON_TYPE.PUT_REASON_XUN_BAO_GUIDE_PARTNER_2] = true,
    [PUT_REASON_TYPE.PUT_REASON_JIN_LONG_JU_BAO_DRAW] = true,
    [PUT_REASON_TYPE.PUT_REASON_YUE_XI_DENG_MI] = true,
    [PUT_REASON_TYPE.PUT_REASON_ZHUI_YUE_SHANG_CHENG_DRAW]=true,
    [PUT_REASON_TYPE.PUT_REASON_TEAM_CHALLENGE] = true,
    [PUT_REASON_TYPE.PUT_REASON_HALLOWEEN_PRAY] = true,
}

PPStateSp = {
    zhan = "_LocZhan",      --出战
    dai = "_LocDai",        --待机
    xun = "_LocXun",        --家族训宠
    lei = "_LocLei",        --擂台战
    main = "_LocMain",      --主战（穹顶）
    xiu = "_LocXiu",        --休息（穹顶）
    zhu = "_LocZhu",        --助战（穹顶）
}

EffectEvent = {
    event_start = "start",
    event_end = "end"
}

--战斗返回结果类型
RESULT_TYPE = {
    INVALID = 0,
    ATTACKER_WIN = 1, --攻方胜
    DEFENDER_WIN = 2, --守方胜
    DRAW = 3, --和局（两边同时被打死）
    FORCED_FINISH = 4, --战斗强制结束
    EXCEED_MAX_ROUND = 5,--超过最大回合数
    EXCEED_MAX_TIME = 6, --超过最大时间限制
    RUNAWAY = 7 --逃跑
}

REWARD_VIRTUAL_TYPE = {
    TYPE_PET = 1
}

TextAnchor = {
    UpperLeft = 0,
    UpperCenter = 1,
    UpperRight = 2,
    MiddleLeft = 3,
    MiddleCenter = 4,
    MiddleRight = 5,
    LowerLeft = 6,
    LowerCenter = 7,
    LowerRight = 8
}

LAYER = {
    Default = 0,
    TramsparentFX = 1,
    IgnoreRaycast = 2,
    Water = 4,
    UI = 5,
    QualityScene = 8,
    QualityRole = 9,
    ColliderScene = 10,
    ColliderRole = 11,
    ShadowScene = 12,
    BakeShadowOffScene = 13,
    ShadowOnlyScene = 14
}

RELOGIN_TYPE = {
    WELCOME = 0,
    ROLE_SELECT = 1,
}

-- 地图城镇id
MapItemEnum = {
    YingDu = 55,
    YingDuXiJiao = 52,
}

CHAPTER_BOSS_REQ_TYPE = {
    TYPE_CLAIM_BOSS_REWARD = 0,
    TYPE_CLAIM_CHAPTER_REWARD = 1,
    TYPE_CLAIM_START_BATTLE = 2,
}

MPNEEDVIRTUALCAMERA = {
    [1703] = true,
    [326] = true,
    [342] = true,
    [344] = true,
}

MPNEEDCHANGE = {
    [206] = true,
    --[207] = true,
    [209] = true,
    [213] = true,
}

MPSCENEID = {
    [206] = true,
    [207] = true,
    [208] = true,
    [209] = true,
    [210] = true,
    [211] = true,
    [212] = true,
    [213] = true,
}
MPMAPRES = {
    [206] = "MP_1",
    [207] = "MP_2",
    [208] = "MP_3",
    [209] = "MP_4",
    [210] = "MP_5",
    [211] = "MP_6",
    [212] = "MP_7",
    [213] = "MP_8",
}
LEITAIZHAN = {
    [280] = "LTCJ_2_yuanpan04",
    [281] = "LTCJ_2_yuanpan05",
    [282] = "LTCJ_2_yuanpan03",
    [283] = "LTCJ_2_yuanpan01",
    [284] = "LTCJ_2_yuanpan02",
}
SLCJRES = {
    [305] = {"SLCJ_1_ditan01a","SLCJ_1_yuanpan01a","SLCJ_1_zhanqi01a"},
    [306] = {"SLCJ_1_ditan01b","SLCJ_1_yuanpan01b","SLCJ_1_zhanqi01b"},
    [307] = {"SLCJ_1_ditan01c","SLCJ_1_yuanpan01c","SLCJ_1_zhanqi01c"},
    [308] = {"SLCJ_1_ditan01d","SLCJ_1_yuanpan01d","SLCJ_1_zhanqi01d"},
    [309] = {"SLCJ_1_ditan01e","SLCJ_1_yuanpan01e","SLCJ_1_zhanqi01e"},
    [310] = {"SLCJ_1_ditan01f","SLCJ_1_yuanpan01f","SLCJ_1_zhanqi01f"},
}

SWITCHITEM = {
    [24544] = true
}

BuffType = {
    CHANGE_ATTR = 1,
    EXP_UP = 2,
    CHANGE_MOVE_SPEED = 3,
    RUSH_DUNGEON = 4,
}

SERVER_STATE = {
    [0] = "Maintain",       --关闭
    [1] = "smooth",         --流畅(新服)
    [2] = "Crowded",        --拥挤
    [3] = "full",           --爆满
}

--动画状态机上的参数
MainViewGameState = {
    normal = 0,
    courage_challenge = 1,
    conspiracy = 2,
    escort = 3,
    fight_crystal = 4,
    fight_gather = 5,
    maze_run = 6,       --迷宫竞速状态
    team_sport = 7,     --竞技对抗赛
    rush_dungeon = 8,   --勇闯地宫
    guild_melee = 10,
    chief_elelction = 11, 
    chief_peak_battle = 12,
    guild_embattle = 13,
    guild_fb = 14,
    arena_match = 15,
    advanced = 16,
    madel_fb = 17,    -- 勋章副本
    fight_fantasy = 18,    -- 四象幻境
    animal_race = 19,   --神兽径赛
    only_fight = 20,    --以一敌百
    shitu_fb = 21,      -- 师徒副本
    chief_primary = 25, -- 首席预选
    du_chuang_zei_ying = 27, --独闯贼营
    wen_xiang_lou = 28, --温香楼
    fox_catch = 29, -- 捉拿狐妖
    star_life = 30, -- 司命星君
    dream_notes = 31,--梦渊笔录
    lucky_maze = 32, -- 幸运迷宫
    splace_anecdote = 33, -- 秘境奇闻
    material = 34, -- 秘境奇闻
    tomb_raider = 35, -- 遗迹夺宝
    array_ghost = 36,  -- 阵中之魂
    pet_god = 37,  -- 神印地
    go_through = 38,-- 渡劫
    fish_ground = 39,--钓鱼
    all_souls_storybook = 41,--万灵话本
    wedding_fb = 42,--婚宴副本
    world_arena = 43,    --论剑九州
    hero_van_battle = 45,    --英雄会武
    lovers_match = 46,   --情缘对对碰
    marry_party = 47,   --情缘盛会
    rabbit_race = 50,   --兔兔快跑周期活动
    lucky_ring = 51,   -- 套圈纳吉
    ut_arena = 52,      -- 天下第一比武大会
    harmony_heaven = 53,--鸿蒙天界
    youming_seal = 9000,  --幽冥抓捕
    palace_rank = 10003,--殿堂
}

FreedomCode = {
    NiuYaoSkill = true,
    NiuYaoBianShen = true,
    NpcViewXinMo = true,
    NpcViewBangJia = true,
    BattleXinMoSum = true,
    BattleUINew = true,
}

EquipGuideLevel = {
    [25] = true,
    [31] = true,
    [35] = true,
}

--找回类型                                              --对应服务器的枚举字段：DAY_ACTIVITY_TYPE
FindType = {
    ResearchTask = 1,           --三界除魔（日常/限时）             DAY_ACTIVITY_TYPE_ZHEN_ZHONG
    TreasureMap = 2,            --宝图任务/缉捕盗贼（日常/限时）    DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK
    GuildTask = 3,              --学院任务（日常/限时）             DAY_ACTIVITY_TYPE_SCHOOL_TASK
    WSTQuesten = 4,             --神算子的考验（日常/限时）         DAY_ACTIVITY_TYPE_WST_QUESTION
    TreviFountain = 5,          --许愿池（日常/限时）               DAY_ACTIVITY_TYPE_TREVI_FOUNTAIN
    BountyTask = 6,             --赏金任务/门派任务（日常/限时）    DAY_ACTIVITY_TYPE_BOUNTY_TASK
    FantasyFB = 7,              --四象幻境（日常/限时）             DAY_ACTIVITY_TYPE_FANTASY_FB
    SilkRoad = 8,               --丝绸之路,这是旧版的远洋贸易（日常/限时）DAY_ACTIVITY_TYPE_TRANSACTION_TASK
    GuildAnswer = 9,            --家族答题（日常/限时）             DAY_ACTIVITY_TYPE_GUILD_ANSWER
    GuildMelee = 10,            --家族大乱斗（日常/限时）           DAY_ACTIVITY_TYPE_GUILD_FIGHT
    OnlyFight = 11,             --以一敌百（日常/限时）             DAY_ACTIVITY_TYPE_ONLY_FIGHT
    TeamSport = 12,             --竞技对抗赛（日常/限时）           DAY_ACTIVITY_TYPE_TEAM_FIGHT
    FightEnergy = 13,           --秘境体力溢出（日常/限时）         DAY_ACTIVITY_TYPE_OVERFLOW_ENERGY
    WanLingActive = 14,         --万灵话本（活跃）                  DAY_ACTIVITY_TYPE_WAN_LING_FB_ACTIVE
    CrystalXiYouActive = 15,    --精英试炼（活跃）                  DAY_ACTIVITY_TYPE_CRYSTAL_XI_YOU_ACTIVE
    ClourArenaActive = 16,      --穹顶（活跃）                      DAY_ACTIVITY_TYPE_CLOUD_ARENA_ACTIVE
    ResearchTaskActive = 17,    --三界除魔（活跃）                  DAY_ACTIVITY_TYPE_ZHEN_ZHONG_ACTIVE
    BountyTaskActive = 18,      --赏金任务/门派任务（活跃）         DAY_ACTIVITY_TYPE_BOUNTY_TASK_ACTIVE
    SkillRoadAct = 19,          --丝绸之路（日常/限时）

    -- 大于100的开始为客户端自定义的找回类型，需要特殊处理
    OfflineReward = 101, -- 境界
}

--找回类型中活跃找回对应的活跃枚举值
FindTypeToActivitySystemType = {
    [FindType.WanLingActive] = ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_WAN_LING_FB,
    [FindType.CrystalXiYouActive] = ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_CRYSTAL_XIYOU,
    [FindType.ClourArenaActive] = ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_CLOUD_ARENA,
    [FindType.ResearchTaskActive] = ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_ZHEN_ZHONG,
    [FindType.BountyTaskActive] = ACTIVITY_SYSTEM_TYPE.ACTIVE_SYSTEM_TYPE_BOUNTY_TASK,       
}

OnekeyFinishType = {
    ResearchTask = 0,   --三界除魔
    WSTQuesten = 1,     --神算子的考验
    BountyTask = 2,     --门派任务
    FantasyFB = 3,      --四象幻境
    SilkRoad = 4,       --远洋贸易
    TreasureMap = 5,    --缉捕盗贼
}

AppearanceShowViewType = {
    Illusion = 1,   --外观
    Pet = 2,        --宠物皮肤
    Partner = 3,    --伙伴皮肤
    Face = 4,       --颜值
}

GIFT_TYPE = {

}

HIDETYPE = {
    ShowBack = 1, -- 解除隐身
    HideSelf = 2, -- 我方隐身
    HideEnemy = 3, -- 对我方隐身
}
--BUY_GIFT_ITEM
--直购类型
-- >>> 如需支持万能卡道具直购需添加到 BUY_GIFT_ITEM_TYPE里且与服务端同步 <<<
BUY_ITEM_TYPE = {
    TYPE_GOD_TOKEN = 4,         --封神战令
    TYPE_COW_CONVERT = 10,      --牛运兑换 param1:索引
    TYPE_SPRING_STORE = 11,     --年货节
    TYPE_COW_STORE = 12,        --牛运商城
    --以下新直购类型需要走1251协议
    TYPE_COMBINE_GIFT = 14,     --组合贺礼 param1:多个,按位,配置的type
    TYPE_COMBINE_GIFT_2 = 15,               --组合贺礼 param1 param2
    TYPE_FASHION_SHOP = 16,     --时装商店
    TYPE_ACCRECHARGE = 17,      --累计充值
    TYPE_ONE_YUAN_BUY = 18,     --一元购
    TYPE_IMMORTAL_FUND_BUY = 19,            --灵玉基金
    TYPE_PET_BEFALL_AUTO = 20,              --神兽降临
    TYPE_REWARD_FORTUNE_BAG_NEW = 21,       --奖励福袋
    TYPE_WEALTH_CORNUCOPIA_NEW = 22,        --财神聚宝
    TYPE_CUSTOMIZED_TREASURE_NEW = 23,      --定制宝箱
    TYPE_EXCLUSIVE_GIFT_NEW = 24,           --专属礼包
    TYPE_XIAN_SHI_BAO_HE_NEW = 25,          --限时宝盒
    TYPE_XIAN_SHI_BAO_DAI_NEW = 26,         --限时宝袋
    TYPE_SEVEN_DAY_NEW = 27,                --神秘试炼（非活动）
    TYPE_REWARD_INVEST_NEW = 28,            --奖励投资（非活动）
    TYPE_EXCLUSIVE_GIFT_DAY = 29,           --购买专属礼包每日礼包 param1 param2
    TYPE_FIRST_RECHARGE_EXCLUSIVE = 30,     --首充专属（非活动）
    TYPE_MONTH_CARD = 31,                   --购买周卡/月卡/公用商城 param1 param2
    TYPE_SUMMER_STORE = 32,                 --夏日商店
    TYPE_DISCOUNT_STORE = 33,               --超级折扣商城
    TYPE_NATIONAL_ZHEN_PIN_SHANG_PU = 34,   --珍品行铺      
    TYPE_REWARD_FORTUNE_BAG_NEW_BATCH = 35,   --福袋购买 批量N天 （奖励福袋）     
    TYPE_WAN_LING_GONG_YING = 36,           --万灵共盈
    TYPE_HORCRUX_TIANCHENG = 37,            --魂器天成
    TYPE_MOUNTAIN_SEA_FIRST_MEET = 38,      --山海初遇
    TYPE_MOUNTAIN_SEA_MEET = 39,            --山海相逢
    TYPE_ZHAOCAIJINBAO = 40,                --招财进宝
    TYPE_SCAME_FLOWER = 41,                 --分享同花礼
    TYPE_SUPER_VALUE_ENJOY = 42,            --超值纷享
    TYPE_ENJOY_REDOUBLE = 43,               --纷享加倍
    TYPE_SEA_CUSTOMIZED_BOX = 44,           --山海宝箱(定制)
    TYPE_SEA_GIFT_BOX = 45,                 --山海宝盒
    TYPE_SEA_CHAIN_BOX = 46,                --山海宝袋
    TYPE_YUN_ZE_CUSTOMIZED_BOX = 47,        --云泽宝箱(定制)
    TYPE_YUN_ZE_CHAIN_BOX = 48,             --云泽宝袋
    TYPE_YUN_ZE_GIFT_BOX = 49,              --云泽宝盒
    TYPE_JIXINGGAOZHAO = 50,                --吉星高照
    TYPE_WEEK_GIFT = 51,                    --周末限好礼
    TYPE_CHAOZHI_GIFT = 52,                 --超值献礼
    TYPE_JINGJI_YUEKA = 53,                 --竞技月卡
    TYPE_TIGER_BORN = 54,                   --寅虎问世
    TYPE_TIGER_THANK = 55,                  --寅虎拜岁
    TYPE_LOVE_FASHION = 56,                 --情缘时装
    TYPE_YAO_XING_SHI_LIAN = 57,            --妖行试炼
    TYPE_ANIMAL_WORLD = 58,                 --异兽临世
    TYPE_ALL_SOUL_CAST = 59,                --万灵筑礼
    TYPE_JIN_YI_TIAN_XIANG = 60,            --锦衣天香
    TYPE_VITALITY_FIREWORK = 61,            --元气烟火秀
    TYPE_VITALITY_NOTE = 62,                --元气记事簿
    TYPE_DUANYANGZIXUANLI = 63,             --端阳自选礼
    TYPE_LUCK_BOX = 64,                     --幸运盲盒
    TYPE_DUANYANGFULI = 65,                 --端阳福礼
    TYPE_JIN_LONG_JU_BAO = 66,              --金龙聚宝
    TYPE_GOD_BEAST_GRACE = 67,              --神兽恩泽

    TYPE_FOUR_SEASON_CARD = 68,             --四季花牌
    TYPE_FOUR_SEASON_BOX = 69,              --四季宝盒
    TYPE_PET_EXCHANGE = 70,                 --宠物兑换
    TYPE_SEVEN_BUY = 71,                    --七巧特惠
    TYPE_FOUR_SEASON_CHEST = 72,            --四季宝箱
    TYPE_NEW_SERVER_PET = 73,               --新服灵宠
    TYPE_GOD_CLOTHES = 74,                  --神赐华服
    TYPE_TIAN_YI_YAO_SHI = 75,              --天衣耀世
    TYPE_NEW_COURAGE_CHALLENGE = 76,        --弑神之塔
    TYPE_ZHUI_YUE_SHANG_CHENG=77,           --追月商城
    TYPE_SINGLE_REBATE = 78,                --单笔返利
    TYPE_GOD_SPACE_SEASON_PASS = 79,        -- 神域宝藏 
    TYPE_WAN_SHENG_SHOP=80,                 --万圣商城
    TYPE_NETHER_WORLD_SEAL_CARD = 81,       --幽冥封印卡

    TYPE_FU_YUN_SHANG_CHENG = 82,           --福运商城
    TYPE_RECHARGE = 99999,                  -- 元宝商城
}

-- 支持万能卡的直购类型
BUY_GIFT_ITEM_TYPE = {
    [BUY_ITEM_TYPE.TYPE_REWARD_FORTUNE_BAG_NEW] = 1,  -- 福袋
    [BUY_ITEM_TYPE.TYPE_SEVEN_DAY_NEW] = 2,                 -- 神秘试炼
    [BUY_ITEM_TYPE.TYPE_GOD_TOKEN] = 3,                     -- 封神战令
    [BUY_ITEM_TYPE.TYPE_WEALTH_CORNUCOPIA_NEW] = 4,         -- 财神聚宝
    [BUY_ITEM_TYPE.TYPE_CUSTOMIZED_TREASURE_NEW] = 5,       -- 定制宝箱
    [BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_NEW] = 6,            -- 专属礼包
    [BUY_ITEM_TYPE.TYPE_XIAN_SHI_BAO_HE_NEW] = 7,           -- 限时宝盒
    [BUY_ITEM_TYPE.TYPE_XIAN_SHI_BAO_DAI_NEW] = 8,          -- 限时宝袋
    [BUY_ITEM_TYPE.TYPE_COMBINE_GIFT_2] = 9,                -- 组合贺礼
    [BUY_ITEM_TYPE.TYPE_SPRING_STORE] = 10,                 -- 年货节
    --[BUY_ITEM_TYPE.TYPE_COW_STORE] = 11,                    -- 牛运商城         --不支持
    [BUY_ITEM_TYPE.TYPE_FASHION_SHOP] = 12,                 -- 时装商店
    [BUY_ITEM_TYPE.TYPE_IMMORTAL_FUND_BUY] = 13,            -- 灵玉基金
    [BUY_ITEM_TYPE.TYPE_ACCRECHARGE] = 14,                  -- 累计充值
    [BUY_ITEM_TYPE.TYPE_MONTH_CARD] = 15,                   -- 月卡周卡
    [BUY_ITEM_TYPE.TYPE_SEA_CUSTOMIZED_BOX] = 16,           -- 山海宝箱
    [BUY_ITEM_TYPE.TYPE_SEA_CHAIN_BOX] = 17,                -- 山海宝袋 
    [BUY_ITEM_TYPE.TYPE_SEA_GIFT_BOX] = 18,                 -- 山海宝盒
    [BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_FIRST_MEET] = 19,      -- 山海初遇
    [BUY_ITEM_TYPE.TYPE_MOUNTAIN_SEA_MEET] = 20,            -- 山海相逢
    [BUY_ITEM_TYPE.TYPE_ZHAOCAIJINBAO] = 21,                -- 招财进宝
    [BUY_ITEM_TYPE.TYPE_PET_BEFALL_AUTO] = 22,              -- 神兽降临
    [BUY_ITEM_TYPE.TYPE_WAN_LING_GONG_YING] = 23,           -- 万灵共盈
    [BUY_ITEM_TYPE.TYPE_HORCRUX_TIANCHENG] = 24,            -- 魂器天成
    [BUY_ITEM_TYPE.TYPE_SCAME_FLOWER] = 25,                 -- 分享同花礼
    [BUY_ITEM_TYPE.TYPE_SUPER_VALUE_ENJOY] = 26,            -- 超值纷享
    [BUY_ITEM_TYPE.TYPE_YUN_ZE_CUSTOMIZED_BOX] = 27,        -- 云泽宝箱(定制)
    [BUY_ITEM_TYPE.TYPE_YUN_ZE_GIFT_BOX] = 28,              -- 云泽宝盒
    [BUY_ITEM_TYPE.TYPE_YUN_ZE_CHAIN_BOX] = 29,             -- 云泽宝袋
    [BUY_ITEM_TYPE.TYPE_JIXINGGAOZHAO] = 30,                -- 吉星高照
    [BUY_ITEM_TYPE.TYPE_CHAOZHI_GIFT] = 31,                 -- 超值献礼
    [BUY_ITEM_TYPE.TYPE_TIGER_BORN] = 32,                   -- 寅虎问世
    [BUY_ITEM_TYPE.TYPE_TIGER_THANK] = 33,                  -- 寅虎拜岁
    [BUY_ITEM_TYPE.TYPE_EXCLUSIVE_GIFT_DAY] = 34,           -- 购买专属礼包每日礼包 param1 param2
    [BUY_ITEM_TYPE.TYPE_ENJOY_REDOUBLE] = 35,               -- 分享加倍
    [BUY_ITEM_TYPE.TYPE_YAO_XING_SHI_LIAN] = 36,            -- 妖行试炼
    [BUY_ITEM_TYPE.TYPE_ALL_SOUL_CAST] = 37,                -- 万灵筑礼
    [ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG] = 38,            -- 锦衣天香
    [ACTIVITY_TYPE.RAND_VITALITY_FIREWORK] = 39,            -- 元气烟火秀
    [ACTIVITY_TYPE.RAND_VITALITY_NOTE] = 40,                -- 元气记事
    [ACTIVITY_TYPE.RAND_FORTUNE_BAG] = 41,                  -- 奖励福袋
    [BUY_ITEM_TYPE.TYPE_RECHARGE] = 42,                     -- 元宝商城充值 p：元宝数
    [BUY_ITEM_TYPE.TYPE_FIRST_RECHARGE_EXCLUSIVE] = 43,     -- 首充专属 p:first_recharge_cmd_type
    [BUY_ITEM_TYPE.TYPE_LUCK_BOX] = 44,                     -- 幸运盲盒
    [BUY_ITEM_TYPE.TYPE_DUANYANGFULI] = 45,                 -- 端阳福礼
    [BUY_ITEM_TYPE.TYPE_DUANYANGZIXUANLI] = 46,             -- 端阳自选礼
    [BUY_ITEM_TYPE.TYPE_JIN_LONG_JU_BAO] = 47,              -- 金龙聚宝
    [BUY_ITEM_TYPE.TYPE_GOD_BEAST_GRACE] = 48,              -- 神兽恩泽
    [BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CARD] = 49,             -- 四季花牌
    [BUY_ITEM_TYPE.TYPE_FOUR_SEASON_BOX] = 50,              -- 四季宝盒
    [BUY_ITEM_TYPE.TYPE_PET_EXCHANGE] = 51,                 -- 宠物兑换
    [BUY_ITEM_TYPE.TYPE_SEVEN_BUY] = 52,                    -- 七巧特惠
    [BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CHEST] = 53,            -- 四季宝箱
    [BUY_ITEM_TYPE.TYPE_NEW_SERVER_PET] = 54,               -- 新服灵宠
    -- [BUY_ITEM_TYPE.TYPE_LOVE_FASHION] = 55,                 -- 情缘时装（渠道要求屏蔽）
    [BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU] = 56,   -- 珍品行铺(现‘周末限时礼’)
    [BUY_ITEM_TYPE.TYPE_TIAN_YI_YAO_SHI] =  57,             -- 天衣耀世
    [BUY_ITEM_TYPE.TYPE_GOD_CLOTHES] =  58,                 -- 神赐华服
    [BUY_ITEM_TYPE.TYPE_NEW_COURAGE_CHALLENGE] = 59,        -- 弑神之塔
    [BUY_ITEM_TYPE.TYPE_SINGLE_REBATE] =  60,               -- 单笔返利
    [BUY_ITEM_TYPE.TYPE_ZHUI_YUE_SHANG_CHENG] = 61,         -- 追月商店
    [BUY_ITEM_TYPE.TYPE_GOD_SPACE_SEASON_PASS] = 62,        -- 神域宝藏
    [BUY_ITEM_TYPE.TYPE_WAN_SHENG_SHOP] = 63,               -- 万圣商城
    [BUY_ITEM_TYPE.TYPE_FU_YUN_SHANG_CHENG] = 64,           -- 福运商城
}

-- 特殊活跃类型
ACTIVITY_SYSTEM_SPECIAL_TYPE = {
    TYPE_WILD_BOSS = 1,
}

--等级助力操作请求
LEVEL_COMPLEMENT_TYPE = {
    TYPE_0 = 0, --请求下发
    TYPE_1 = 1, --购买经验
}

--副本翻牌请求类型
POKER_REQ_OPER_TYPE = {
    TYPE_MI_YI = 1,     --密医/试炼之地/远征魔冢
    TYPE_WILD_BOSS = 2, --野外boss
    TYPE_SHXY_FB = 3,   --山海降妖
    CRYSTAL = 4,        --水晶副本通关
    CRYSTAL_SWEEP = 5,  --水晶副本扫荡
    PET_GOD = 6,        --神印地
    PET_GOD_SWEEP = 7,  --神印地扫荡
    GHOST_NIGHT = 8,    --百鬼夜行


    NORMAL_FALSE = 999, --通用假翻牌
}

--副本翻牌操作类型
POKER_REQ_REASON_TYPE = {
    TYPE_SELF = 0, --玩家主动翻牌
    TYPE_SYSTEM = 1, --系统帮忙翻牌
}
--主线特殊处理任务150
MAIN_LINE_SP_TASK = 150
MAIN_LINE_SP_MAP = 131

--排行榜类型
PERSON_RANK_TYPE = {
    DREAM_NOTES_CHALLENGE = 20, --梦渊笔录玩法
    ENDLESS_TOWER = 36,         --无尽之塔玩法
    FALLEN_GOD = 37,            --殒神之地
    NEW_COURAGE_CHALLENGE = 39, --弑神之塔
}

BountyTaskRewardGetType = {
    None = 0,
    TaskChain = 1,
}

-- 使用需要特殊逻辑道具
UseSpecialLogicItem = {
    CryStalFB = 0,      --秘境药剂
    BreakItem = 1,      --境界用的灵力丹
}

FuncGuideFlagType = {
    FUNC_GUIDE_NEWBIE_PET_CHOICE = 0,   --寻宝新手宠物2
    FUNC_GUIDE_CHALLENGE_COLOSSEUM = 1, --新手挑战古神宝库
    FUNC_GUIDE_NEWBIE_CATCH_PET = 2,    --新手抓宠指引
    FUNC_GUIDE_NEWBIE_DRAW_PET_2 = 3,   --寻宝新手宠物2
    FUNC_GUIDE_CLOUD_ARENA = 4,         --云端竞技
    FUNC_GUIDE_LUNHUI_WANGCHUAN = 5,    --轮回忘川
    FUNC_GUIDE_NEWBIE_DRAW_PARTNER = 6, --寻宝伙伴指引
    FUNC_GUIDE_NEWBIE_DRAW_PARTNER_2 = 7, --寻宝伙伴指引2
    FUNC_GUIDE_DREAM_NOTES_CHALLENGE = 8,--扫荡梦缘指定层数
    FUNC_GUIDE_PASS_LUN_CHUAN = 9,--通关轮回忘川指定层数
}

--首席预选 1 ~ 8  等于服务端 TPOIC_TYPE
RoleSubscribeTopicType = {
    TeamChannel = 0,
    ChiefPeakBattle = 9,
    Market = 10,
    World_Boss_1 = 11,
    World_Boss_2 = 12,
    TYPE_RA_PAI_MAI_HANG = 13, --运营活动-拍卖行
    World_Boss_3 = 14, 
}

PET_HELPER_FORMATION_TYPE = {
    PET_HELPER_FORMATION_CLOUD_ARENA = 0,           --!< 0 云端竞技（穹顶）
    PET_HELPER_FORMATION_COURAGE_CHALLENGE = 1,     --!< 1 锢魔之塔
    PET_HELPER_FORMATION_NETHERWORLD = 2,           --!< 2 幽冥炼狱
    PET_HELPER_FORMATION_COLOSSEUM = 3,             --!< 3 竞技场
    PET_HELPER_FORMATION_ENDLESS_TOWER = 4,         --!< 4 无尽之塔
    PET_HELPER_FORMATION_GUILD_BOSS = 5,         --!< 5 家族试炼
    PET_HELPER_FORMATION_WORLD_ARENA = 6,       --!<6 论剑九州
    PET_HELPER_FORMATION_YAO_XING_SHI_LIAN = 7,       --!<7 妖行试炼
    PET_HELPER_FORMATION_REALM_BREAK_FIGHT = 8,     --!<8 心灵之境
    PET_HELPER_FORMATION_GOD_SPACE = 9,             --!<9 神域
    PET_HELPER_FORMATION_TYPE_MAX = 9,              --!< 最大值
}

--直接使用的道具 服务端自己自动使用 并且自动转化为货币
DirectUseItemID = {
    [CommonItemId.ImmortalCoinId] = true,
    [CommonItemId.SoulStoneId] = true,
}

-- 根据运营设定的购买类型去返回public图集的货币id去设置
-- 主要是直购的货币图片需要处理
BUY_CURRENCY_TYPE = {
    [3] = CurrencyType.RenMinBi
}

ONLINE_STATUS_TYPE = {
    TYPE_BOTH = -1,
    TYOE_OFFLINE = 0, --离线
    TYPE_ONLINE = 1, --在线
    TYPE_CROSS = 2, --跨服
}

KnapsackGetItemNoticeType = {
    Workshop = 0,
    LiveSkill = 1,
    Deafult = 2,
}

FunPreNoticeEnum = {
    Lock = 0,
    UnLock = 1,
    Rewarded = 2,
}

--用来区分随机活动
SERVER_ACTIVITY_MIN_NUM = 2000

SportsCardEmum = {
    MAX_DAY_TASK_NUM = 20,
    MAX_WEEK_TASK_NUM = 20,
    MAX_SEASON_TASK_NUM = 20,
    MAX_TASK_GROUP_TYPE_NUM = 3,
    MAX_TASK_TYPE_NUM = 75,

    TYPE_WEEK_TASK = 2,
    TYPE_DAY_TASK = 1,
    TYPE_SEASON_TASK = 0,
}

-- 场景对象类型(Server)
SceneObjTypeS = {
    None = 0,
    Role = 1, -- 1 角色
    Monster = 2, -- 2 NPC - 怪物
    Gather = 3, -- 3 NPC - 采集物
    Door = 4, -- 4 传送门
    Box = 5, -- 5 宝箱
    Npc = 6, -- 6 NPC - 功能性NPC | 由服务器生成的NPC商人、分解师，或是观赏性NPC等
    Count = 7,
    FBDoor = 10,--副本内传送门
}

SceneObjNotifyReason = {
    Normal = 0, -- 0 正常下发(通常在进入场景时)
    Add = 1, -- 1 新增obj时下发
    Delete = 2 -- 2 删除obj时下发
}

-- 屏蔽坐骑的场景
ShieldRideScene = {
    [326] = true,
}

AudutSeqIndex = {
    [1] = true,
    [2] = true,
    [3] = true,
}

-- 对应的灵珠道具id
BattleAttriElementId = {
    [40] = CommonItemId.ElementWater,
    [41] = CommonItemId.ElementFire,
    [42] = CommonItemId.ElementWind,
    [43] = CommonItemId.ElementEarth
}
--神域阵营
GOD_SPACE_TOWN = {
    WATER = 0,--水之城
    FIRE = 1,--火之城
    WIND = 2,--风之城
    EARTH = 3,--地之城
}
--神域状态
GOD_SPACE_STATUS = {
    Prepare1 = 1,--准备1
    Prepare2 = 2,--准备2
    OurterOpen = 3,--外域开启
    InnerOpen = 4,--内域开启
    Calcul = 5,--结算
    Close = 6,--休赛
}
--新模式
IS_HEXAGON = false

--PC端新手阶段等级标记
PC_NEW_ROLE_STATE_LEVEL = 32