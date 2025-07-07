SceneConfig = {}

-- 场景对象类型
SceneObjType = {
    None = 0,
    MainRole = 1, -- 主角
    Role = 2, -- 角色
    Npc = 3, -- NPC
    Door = 4, -- 场景跳转门
    Monster = 5, -- NPC怪物
    Box = 6, -- 宝箱
    Pet = 7, -- 跟随宠物
    EscortCar = 8, -- 镖车
    Statue = 9, -- 雕像
    Animal = 11, --神兽径赛
    DuChuangZeiYingPatrolMonster = 12, --独闯贼营巡逻怪
    CatchFox = 13, --捉拿狐妖的狐妖
    MapExploreBox = 14, -- 地图探索的宝箱
    YinYangObj = 15,--阴阳眼影响对象
    TreasureTaskObj = 16,--九州秘宝的宝箱
    WeddingBaoWeiXiuQiuMonster = 17,--婚宴副本保卫绣球熊孩子
    ParadeFloat = 18,--巡游花车
    WeddingLookLove = 19,--婚宴副本寻爱礼盒采集物
    PeakToken = 20,--天下至尊令 雕像
    Soldier = 21, --士兵巡逻
    RabbitRace = 22,    -- 兔子快跑的兔子
    UTAStatue = 23,   -- 天下第一冠军雕像
    SoulBoss = 24,     -- 心灵之境BOSS
    SchoolBoy = 25,     -- “我要上学”的小屁孩
    ShengQi = 26,   -- 圣器
    PalaceStatus = 27,  --殿堂雕像
    RuinsBoss = 28,--归墟boss
    PalacesBoss = 29,--天宫boss
}
--场景模型不可点击的
SceneConfig.NotTouchType = {
    [SceneObjType.MainRole] = true,
    [SceneObjType.Door] = true,
    [SceneObjType.Pet] = true,
    [SceneObjType.EscortCar] = true,
    [SceneObjType.Statue] = true,
    [SceneObjType.ParadeFloat] = true,
    [SceneObjType.CatchFox] = true,
    [SceneObjType.MapExploreBox] = true,
    [SceneObjType.Soldier] = true,
    [SceneObjType.Animal] = true,
    [SceneObjType.YinYangObj] = true,
    [SceneObjType.TreasureTaskObj] = true,
    [SceneObjType.ShengQi] = true,
}

--场景模型没有阴影的
SceneConfig.NotShadowType = {
    [SceneObjType.CatchFox] = true,
    [SceneObjType.MapExploreBox] = true,
    [SceneObjType.Soldier] = true,
    [SceneObjType.Animal] = true,
    [SceneObjType.YinYangObj] = true,
    [SceneObjType.TreasureTaskObj] = true,
}

--场景模型对象类 新加的特殊场景模型 必须要加到这里 脚本加载在 scene_requre_list
SceneConfig.NewType = {
    [SceneObjType.MainRole] = "MainCh",
    [SceneObjType.Box] = "BoxCh",
    [SceneObjType.Soldier] = "SoldierCh",
    [SceneObjType.Animal] = "AnimalCh",
    [SceneObjType.CatchFox] = "FoxCh",
    [SceneObjType.YinYangObj] = "YinYangObj",
    [SceneObjType.MapExploreBox] = "BoxMapExplore",
    [SceneObjType.TreasureTaskObj] = "BoxTreasureTask",
    [SceneObjType.DuChuangZeiYingPatrolMonster] = "DuChuangZeiYingPatrolMonsterCh",
    [SceneObjType.WeddingBaoWeiXiuQiuMonster] = "WeddingBaoWeiXiuQiuCh",
    [SceneObjType.WeddingLookLove] = "WeddingLookLoveCh",
    [SceneObjType.ParadeFloat] = "ParadeFloatCh",
    [SceneObjType.PeakToken] = "PeakTokenCh",
    [SceneObjType.RabbitRace] = "RabbitRaceObjCh",
    [SceneObjType.UTAStatue] = "UTAStatueCh",
    [SceneObjType.SchoolBoy] = "GotoSchoolObjCh",
}

--是否是场景人物模型
SceneConfig.RoleType = {
    [SceneObjType.Role] = true,
    [SceneObjType.MainRole] = true,
    [SceneObjType.Statue] = true,
    [SceneObjType.PeakToken] = true,
    [SceneObjType.UTAStatue] = true,
    [SceneObjType.PalaceStatus] = true,
}

--需要用于头顶显示Npc名字的
SceneConfig.NpcBoardType = {
    [SceneObjType.Npc] = true,
    [SceneObjType.Box] = true,
    [SceneObjType.Monster] = true,
    [SceneObjType.RabbitRace] = true,
    [SceneObjType.SoulBoss] = true,
    [SceneObjType.SchoolBoy] = true,
}

--需要用于头顶显示名字的
SceneConfig.NameBoardType = {
    [SceneObjType.Door] = true,
    [SceneObjType.Pet] = true,
    [SceneObjType.Soldier] = true,
    [SceneObjType.DuChuangZeiYingPatrolMonster] = true,
    [SceneObjType.WeddingBaoWeiXiuQiuMonster] = true,
}

--走通用协议下发的 
SceneConfig.NetNpcsType = {
    [SceneObjType.Box] = true,
    [SceneObjType.Npc] = true,
    [SceneObjType.ParadeFloat] = true,
    [SceneObjType.PeakToken] = true,
    [SceneObjType.RabbitRace] = true,
}

--无显示的资源id
SceneConfig.EmptyResId = 109 