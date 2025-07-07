SceneEffectConfig = SceneEffectConfig or {}
-- 是否自动销毁 是否是缓存默认
SceneEffectConfig.EffectList = {
    ClickEffect = {path = "effect/10020201.prefab", auto_release = true, resident = true},--地面点击特效
    SelectEffect = {path = "effect/10020202.prefab", auto_release = true, resident = true},--选中场景对象特效
    LevelEffect = {path = "effect/10020203.prefab", auto_release = true, resident = false},--升级特效
    PetSealEffect0 = {path = "effect/10060100.prefab", auto_release = false, resident = false},--宠物封印特效0
    PetSealEffect1 = {path = "effect/10060104.prefab", auto_release = false, resident = false},--宠物封印特效1
    PetSealEffect2 = {path = "effect/10060105.prefab", auto_release = false, resident = false},--宠物封印特效2
    CurCmdEffect = {path = "effect/10020204.prefab", auto_release = false, resident = true},--战斗显示操作中特效
    GuildAnswer1 = {path = "effect/10020002.prefab", auto_release = false, resident = false},--家族答题特效1
    GuildAnswer2 = {path = "effect/10020003.prefab", auto_release = false, resident = false},--家族答题特效2
    GuildAnswer3 = {path = "effect/10020004.prefab", auto_release = false, resident = false},--家族答题特效3
    GuildAnswer4 = {path = "effect/10020005.prefab", auto_release = false, resident = false},--家族答题特效4
    RoleCreate = {path = "effect/10020205.prefab", auto_release = true, resident = true},--角色出现时特效
    Death = {path = "effect/10070501.prefab", auto_release = true, resident = false},--战斗死亡时特效
    TaskGatherRole = {path = "effect/10150002.prefab", auto_release = true, resident = false},--血脉测试1
    TaskGatherMono = {path = "effect/10150001.prefab", auto_release = true, resident = false},--血脉测试2
    HCHuoYan = {path = "effect/10020009.prefab", auto_release = false, resident = false},--皇城战斗火焰特效
    BianShen = {path = "effect/10035001.prefab", auto_release = true, resident = false},--战斗变身特效
    AngryEffect = {path = "effect/10020801.prefab", auto_release = true, resident = false},--头顶生气特效
    SpyEffect = {path = "effect/10020807.prefab", auto_release = true, resident = false},--铁卫营奸细特效
    GGInitEffect = {path = "effect/10150836.prefab", auto_release = true, resident = false},--共工出场特效
    XTInitEffect = {path = "effect/10150835.prefab", auto_release = true, resident = false},--刑天出场特效
    LuoShiEffect = {path = "effect/10150839.prefab", auto_release = true, resident = false},--刑天出场特效
    FishTearShowEffect = {path = "effect/10170001.prefab", auto_release = true, resident = false},--鱼王睁眼特效
    FishTearKeepEffect = {path = "effect/10170002.prefab", auto_release = true, resident = false},--鱼王睁眼特效
    FishTearcatchEffect = {path = "effect/10170015.prefab", auto_release = true, resident = false},--鱼王拾取
    FoxCatchedEffect = {path = "effect/10170008.prefab", auto_release = true, resident = false},--狐妖出生
    FoxSearchEffect = {path = "effect/10170009.prefab", auto_release = true, resident = false},--法阵生成
    StarLifeEffect1 = {path = "effect/10170017.prefab", auto_release = false, resident = false}, --司命星君拖尾特效
    StarLifeEffect2 = {path = "effect/10170018.prefab", auto_release = true, resident = false}, --司命星君瞬间特效
    StarLifeEffect3 = {path = "effect/10170019.prefab", auto_release = false, resident = false}, --司命星君人物身上持续特效
    TaskWXLItemMono = {path = "effect/10170022.prefab", auto_release = false, resident = false},--温香楼线索
    JinGeTieMa = {path = "effect/10080804.prefab", auto_release = false, resident = false},--金戈铁马护卫特效
    Storehouse = {path = "effect/10020012.prefab", auto_release = true, resident = false}, --神秘宝藏特效
    StarTreasureMap = {path = "effect/30000871.prefab", auto_release = false, resident = false},--星辰宝图特效
    XianLingJiaXue = {path = "effect/10045557.prefab", auto_release = true, resident = false},--仙灵祝福加血特效
    FightFieldBoss = {path = "effect/3247000.prefab", auto_release = false, resident = false},--野外首领特效
    HillSea1 = {path = "effect/2171002.prefab", auto_release = false, resident = false},--山海降妖玩法普通特效
    HillSea2 = {path = "effect/3175100.prefab", auto_release = false, resident = false},--山海降妖玩法困难特效
    HillSea3 = {path = "effect/2171001.prefab", auto_release = false, resident = false},--山海降妖玩法地狱特效
    HillSea4 = {path = "effect/2171000.prefab", auto_release = false, resident = false},--山海降妖玩法噩梦特效
    TempCatchEffect = {path = "effect/2015004.prefab", auto_release = true, resident = false},--临时抓捕特效范围测试
    SuperMapEffect = {path = "effect/2176000.prefab", auto_release = false, resident = false}, --高级藏宝图特效
    SuperMapEffect1 = {path = "effect/2176001.prefab", auto_release = true, resident = false}, --高级藏宝图特效1
    SuperMapEffect2 = {path = "effect/2176002.prefab", auto_release = true, resident = false}, --高级藏宝图特效2
    SuperMapEffect3 = {path = "effect/2176003.prefab", auto_release = true, resident = false}, --高级藏宝图特效3
    BoxMapExploreEffect1 = {path = "effect/5175000.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果1
    BoxMapExploreEffect2 = {path = "effect/5175002.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果2
    BoxMapExploreEffect3 = {path = "effect/5175003.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果3
    BoxMapExploreEffect4 = {path = "effect/5175004.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果4
    BoxMapExploreEffect5 = {path = "effect/5175011.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果5
    PeakLevelUp = {path = "effect/3175006.prefab", auto_release = true, resident = false}, -- 地图探索宝箱效果5
    LinkChains = {path = "effect/10040213.prefab", auto_release = false, resident = false}, -- 链接锁链特效
    YinYanEye = {path = "effect/30001000.prefab", auto_release = false, resident = false}, -- 阴阳眼场景特效
    DanLanHuoYan = {path = "effect/4171003.prefab", auto_release = false, resident = false}, --阴阳人身上的火焰
    YinYangObjShow = {path = "effect/4171004.prefab",auto_release = true, resident = false}, --阴阳玩意阴阳眼打开时的特效
    YinYangObjHide = {path = "effect/4171005.prefab",auto_release = true, resident = false}, --阴阳玩意阴阳眼打开时的特效
    YinYangBox1 = {path = "effect/4171001.prefab",auto_release = false, resident = false}, --阴阳玩意阴阳眼打开时的特效
    YinYangBox2 = {path = "effect/4171002.prefab",auto_release = false, resident = false}, --阴阳玩意阴阳眼打开时的特效
    LiveModelCard = {path = "effect/2015014.prefab",auto_release = false, resident = false}, --生活变身卡特效
    LiveStove = {path = "effect/3171004.prefab",auto_release = false, resident = false}, --生活丹炉特效
    LiveKitchen = {path = "effect/3171005.prefab",auto_release = false, resident = false}, --生活灶台特效
    BoxTreasureTaskEffect1 = {path = "effect/7171005.prefab", auto_release = true, resident = false}, -- 九州秘宝宝箱效果
    BoxTreasureTaskEffect2 = {path = "effect/2171004.prefab", auto_release = true, resident = false}, -- 九州秘宝宝箱效果
    BountyTaskArrayEffect1 = {path = "effect/3175101.prefab", auto_release = true, resident = false}, -- 门派任务法阵特效未激活
    BountyTaskArrayEffect2 = {path = "effect/3175102.prefab", auto_release = true, resident = false}, -- 门派任务法阵特效已激活
    ChFlyUpEffect1 = {path = "effect/2171005.prefab", auto_release = false, resident = false},       -- 飞升1段模型特效
    ChFlyUpEffect2 = {path = "effect/2171006.prefab", auto_release = false, resident = false},       -- 飞升2段模型特效
    ParadeFloatEffect1 = {path = "effect/7171002.prefab", auto_release = false, resident = false}, --花车特效1
    ParadeFloatEffect2 = {path = "effect/7171006.prefab", auto_release = false, resident = false}, --花车特效2
    PeakTokenEffect = {path = "effect/7121001.prefab", auto_release = false, resident = false},    --天下至尊令
    RabbitRaceDes = {path = "effect/7175000.prefab", auto_release = false, resident = false},      --兔子快跑到达终点
    RabbitRaceWin = {path = "effect/7175001.prefab", auto_release = false, resident = false},      --兔子胜利
    LuckyRingPoint = {path = "effect/7175001.prefab", auto_release = false, resident = false},      --套圈得分
    JiangZiYaEffect = {path = "effect/2047000.prefab", auto_release = false, resident = false},     --姜子牙脚底特效1009怪物组专用
    MarbasVSE1 = {path = "effect/3017000.prefab", auto_release = false, resident = false},          --造化罡气·简易流光
    MarbasVSE2 = {path = "effect/3017001.prefab", auto_release = false, resident = false},          --造化罡气·复杂流光
    MarbasVSE3 = {path = "effect/3121004.prefab", auto_release = false, resident = false},          --造化罡气·光球拖尾
    MarbasVSE4 = {path = "effect/3271000.prefab", auto_release = false, resident = false},          --造化罡气·元素法球
    MarbasVSE5 = {path = "effect/3271101.prefab", auto_release = false, resident = false},          --造化罡气·飞剑
    MarbasVSE6 = {path = "effect/3271102.prefab", auto_release = false, resident = false},          --造化罡气·火焰飞羽
    MarbasVSE7 = {path = "effect/3271103.prefab", auto_release = false, resident = false},          --造化罡气·凤鸣九天
    MarbasVSE8 = {path = "effect/3121005.prefab", auto_release = false, resident = false},          --造化罡气·龙啸天地
    MarbasVSE9 = {path = "effect/3017002.prefab", auto_release = false, resident = false},          --造化罡气·神兵护体
    SoulBossShow1 = {path = "effect/3175019.prefab", auto_release = false, resident = false},          --旋转法阵
    SoulBossShow2 = {path = "effect/3175020.prefab", auto_release = false, resident = false},          -- 蓝色拖尾条
    SoulBossShow3 = {path = "effect/3175021.prefab", auto_release = false, resident = false},          --溶解动画的光
    SoulBossShow4 = {path = "effect/3175022.prefab", auto_release = false, resident = false},          --boss出现候残留法阵
    SoulBossShow5 = {path = "effect/3175023.prefab", auto_release = false, resident = false},          --黑色拖尾条
    SchoolRangeGreen = {path = "effect/9999888.prefab", auto_release = false, resident = false},           --我要上学的绿圈
    SchoolRangeRed = {path = "effect/9999889.prefab", auto_release = false, resident = false},             --我要上学的红圈
    HallowActive = {path = "effect/3171006.prefab", auto_release = false, resident = false},             --圣器激活特效
    HallowUnactive = {path = "effect/3171007.prefab", auto_release = false, resident = false},             --圣器激活特效
    RewardBoxShow = {path = "effect/3225028.prefab", auto_release = false, resident = false},           --宝箱创建出现特效
    RewardBoxZi = {path = "effect/3225022.prefab", auto_release = false, resident = false},           --宝箱打开特效紫
    RewardBoxCheng = {path = "effect/3225023.prefab", auto_release = false, resident = false},           --宝箱打开特效橙
    RewardBoxHong = {path = "effect/3225024.prefab", auto_release = false, resident = false},           --宝箱打开特效红
    RewardBoxCai = {path = "effect/3225025.prefab", auto_release = false, resident = false},           --宝箱打开特效彩
    EleBoxOpenEffect = {path = "effect/3225031.prefab", auto_release = false, resident = false},        --精灵宝箱吐金币特效
    RabbitShieldCreate = {path = "effect/7074001.prefab", auto_release = false, resident = false},      --偃月兔护盾生成
    RabbitShieldBeAttacked = {path = "effect/7074002.prefab", auto_release = false, resident = false},  --偃月兔护盾受击
    RabbitShieldDestroy = {path = "effect/7074003.prefab", auto_release = false, resident = false},     --偃月兔护盾消失
}