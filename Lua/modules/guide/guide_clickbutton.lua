GuideClickBtn = GuideClickBtn or BaseClass(GuideBase)
-- view参数只限定为baseview sub参数为替身或者flushpanel (后面可能要拓展成多个) btn可以作为指引注册key使用（使用时subView为空，可在panel上注册，关界面时要删除注册）
-- 用法1 [key] = { view = "string"（BaseView), sub = "string" (UIWSubstitute/UIWFlushPanel), btn = "string"(具体序列化按钮名) }
-- 用法2 [key] = { view = "string"（BaseView), sub = {"string","sring","string"...}(UIWFlushPanel,UIWFlushPanel,UIWFlushPanel...), btn = "string"(具体序列化按钮名)}
-- 用法3 [key] = { view = "string" (BaseView), sub = nil, btn = "string"（GuideManger里注册功能,注册的key）}
-- Key 是和配置表里对应的
-- ！！！！注意使用RegisterGetGuideUi 进行注册获取ui的方式 subView = nil 不要填其他等于空就行了
-- 列表item的获取 注意填写判断 SetupCompleted 判断是否创建完成
-- RegisterGetGuideUi 注册的方式 非永久存在的ui 需要注销！！！！
-- view字段是btn所在的view名
-- sub 是view的序列化字段，sub为table时 第2个为第1个的序列化字段 （序列化对象为UIWSubstitute/UIWFlushPanel）
GuideClickBtn.btn_config = {
    ["RoleView"] = {view = "MainView", subView = "TopRight", btn = "RoleHeadObj", func = nil},--右上角角色
    ["OnWorldBossBtn"] = {view = "MainView", subView = "TopRight", btn = "OnWorldBossBtn", func = nil}, --右上首领按钮
    ["AddPointBtn"] = {view = "RoleView", subView = "RoleAttr", btn = "AddPointClick", func = nil},--角色界面加点按钮
    ["AddPointBest"] = {view = "RoleAddPointView", subView = "RoleAddPointFlushView", btn = "AddPointBestBtn", func = nil},--加点界面推荐按钮
    ["AddPointConfirm"] = {view = "RoleAddPointView", subView = "RoleAddPointFlushView", btn = "AddPointConfirmBtn", func = nil},--加点界面确认按钮
    ["RoleChangeCardMainBtn"] = {view = "RoleView", subView = "RoleAttr", btn = "RoleChangeCardMainBtn", func = nil},--角色界面幻化按钮
    ["RoleChangeCardBtn"] = {view = "RoleChangeCardView", subView = nil, btn = "RoleChangeCardBtn", func = nil},--幻化确定按钮

    -- 生活系统
    ["LiveMain"] = {view = "MainView", subView = nil, btn = "LiveMain", func = nil},--主界面灵骑按钮
    ["LiveCurrInfo"] = {view = "LiveView", subView = nil, btn = "LiveCurrInfo", func = nil},    -- 生活活跃度信息
    ["LiveTabListInfo"] = {view = "LiveView", subView = nil, btn = "TabListInfo", func = nil},  -- 全部标签的框框
    ["LiveTab6"] = {view = "LiveView", subView = nil, btn = "LiveTab6", func = nil},            -- 厨师标签
    ["LiveContentInfo"] = {view = "LiveView", subView = nil, btn = "LiveContentInfo", func = nil},  -- 介绍功能信息
    ["LiveUseBtn2"] = {view = "LiveView", subView = nil, btn = "LiveUseBtn2", func = nil},  -- 点击烹饪按钮
    ["LiveRefineInfo"] = {view = "LiveRefineView", subView = nil, btn = "LiveRefineInfo", func = nil},  -- 介绍烹饪
    ["LiveRefineBtn"] = {view = "LiveRefineView", subView = nil, btn = "LiveRefineBtn", func = nil},  -- 开始烹饪按钮
    ["LiveRefineResult"] = {view = "LiveRefineResultView", subView = nil, btn = "LiveRefineResult", func = nil},  -- 烹饪结果
    ["LiveContentClose"] = {view = "LiveView", subView = nil, btn = "LiveContentClose", func = nil},  -- 退出厨师界面
    

    ["BattleAutoBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "AutoBtn", func = nil},--自动战斗按钮
    ["BattleSkillBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "SkillBtn", func = nil},--选择技能
    ["BattleSealBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "SealBtn", func = nil},--封印
    ["BattleItemBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "ItemBtn", func = nil},--使用道具
    ["BattleFabaoBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "FabaoBtn", func = nil},--使用法宝
    ["BattleFangYuBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "FangYuBtn", func = nil},--防御按钮
    ["BattlePetBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "PetBtn", func = nil},--宠物按钮
    ["BattleEscapeBtn"] = {view = "MainView", subView = {"MainViewBattle","GuideRects"}, btn = "EscapeBtn", func = nil},--逃跑按钮

    --新模式
    ["BattleAutoBtn2"] = {view = "MainView", btn = "BattleAutoBtn2"},--自动
    ["BattleAutoBeh1"] = {view = "MainView", btn = "BattleAutoBeh1"},--一动
    ["BattleAutoBeh2"] = {view = "MainView", btn = "BattleAutoBeh2"},--二动
    ["BattleSealBtn2"] = {view = "MainView", btn = "BattleSealBtn2"},--封印
    ["BattleItemBtn2"] = {view = "MainView", btn = "BattleItemBtn2"},--物品
    ["BattleFabaoBtn2"] = {view = "MainView", btn = "BattleFabaoBtn2"},--法宝
    ["BattlePetBtn2"] = {view = "MainView", btn = "BattlePetBtn2"},--宠物
    ["BattleEscapeBtn2"] = {view = "MainView", btn = "BattleEscapeBtn2"},--逃跑
    ["BattleFangYuBtn2"] = {view = "MainView", btn = "BattleFangYuBtn2"},--防御
    ["BattleFirstSkill2"] = {view = "MainView", btn = "BattleFirstSkill2"},--第一个技能
    ["BattleSecondSkill2"] = {view = "MainView", btn = "BattleSecondSkill2"},--第二个技能
    ["BattleOtherBtn"] = {view = "MainView", btn = "BattleOtherBtn"},--指引切换到其他
    ["BattleFirstPetSkill2"] = {view = "MainView", btn = "BattleFirstPetSkill2"},--第一个宠物技能
    ["BattleNormalAttack2"] = {view = "MainView", btn = "BattleNormalAttack2"},--拖拽普攻
    --

    ["RunSpeedBtn"] = {view = "MainView", subView = nil, btn = "RunSpeedBtn", func = nil},--变速按钮

    ["BattleFirstSeal"] = {view = "MainView", subView = nil--[[  {"MainViewBattle","PetSealMono"} ]], btn = "FirstSeal", func = nil},--第一个封印卡按钮
    ["BattleSilverCardSeal"] = {view = "MainView", subView = nil, btn = "SilverCardSeal", func = nil},--银色封印卡按钮
    ["BattleFirstSkill"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "FirstSkill", func = nil},--第一个技能按钮
    ["BattleSecondSkill"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "SecondSkill", func = nil},--第二个技能按钮
    ["BattleFirstPetSkill"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "FirstPetSkill", func = nil},--第一个技能按钮
    ["BattleReviveItem"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattleReviveItem", func = nil},--复活道具
    ["BattleFirstFabao"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattleFirstFabao", func = nil},--第一个法宝
    ["BattleMPItem"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattleMPItem", func = nil},--回蓝道具
    ["BattleHpItem"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattleHpItem", func = nil},--战斗中3000加血药
    ["BattleJieFengItem"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattleJieFengItem", func = nil},--战斗中解封丸
    
    
    ["BattlePetItem1"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattlePetItem1", func = nil},--第一只宠物
    ["BattlePetItem2"] = {view = "MainView", subView = nil--[[ {"MainViewBattle","SkillSelMono"} ]], btn = "BattlePetItem2", func = nil},--第二只宠物

    ["BattleEnemy19"] = {view = "MainView", subView = nil, btn = "Enemy19", func = nil},--19号敌人 敌方前排倒数第二
    ["BattleEnemy18"] = {view = "MainView", subView = nil, btn = "Enemy18", func = nil},--18号敌人 敌方前排中间
    ["BattleEnemy13"] = {view = "MainView", subView = nil, btn = "Enemy13", func = nil},--13号敌人 敌方后排中间
    ["BattleEnemy3"] = {view = "MainView", subView = nil, btn = "Enemy3", func = nil},--3号自己人 我方后排中间
    ["BattleEnemy5"] = {view = "MainView", subView = nil, btn = "Enemy5", func = nil},--5号自己人 我方后排最右
    ["BattleEnemy8"] = {view = "MainView", subView = nil, btn = "Enemy8", func = nil},--8号自己人 我方前排中间
    ["BattleMyPos"] = {view = "MainView", subView = nil, btn = "BattleMyPos", func = nil},--自己的位置 智能

    ["PartnerView"] = {view = "MainView", subView = nil, btn = "PartnerBtn",func = nil}, --打开伙伴界面
    ["PartnerViewItemFirst"] = {view = "PartnerView", subView = nil, btn = "PartnerViewItemFirst",func = nil}, --选中第一个伙伴
    ["PartnerViewUp"] = {view = "PartnerView", subView = nil, btn = "PartnerViewTabUp",func = nil}, --选中伙伴进阶标签
    ["PartnerViewUpBtn"] = {view = "PartnerView", subView = "PartnerUp", btn = "PartnerUpClick",func = nil}, --选中伙伴进阶页面进阶按钮

    ["PartnerViewQuality"] = {view = "PartnerView", subView = nil, btn = "PartnerViewTabQuality",func = nil}, --选中伙伴觉醒标签
    ["PartnerViewQualityBtn"] = {view = "PartnerView", subView = "PartnerQuality", btn = "PartnerQualityClick",func = nil}, --选中伙伴觉醒页面觉醒按钮

    ["PartnerViewActiveBtn"] = {view = "PartnerView", subView = "PartnerAttr", btn = "PartnerActiveClick",func = nil}, --选中伙伴属性页面激活按钮
    ["PartnerViewActiveUp10Btn"] = {view = "PartnerView", subView = "PartnerAttr", btn = "PartnerActiveUp10",func = nil}, --选中伙伴属性提升10级按钮
    ["PartnerViewClose"] = {view = "PartnerView", subView = "Board", btn = "BtnCloseClick",func = nil}, --伙伴界面关闭

    ["PartnerViewItemLY"] = {view = "PartnerView", subView = nil, btn = "PartnerViewItemLY",func = nil}, --选中鹿尧
    ["PartnerViewItemH"] = {view = "PartnerView", subView = nil, btn = "PartnerViewItemH",func = nil}, --选中凰
    ["PartnerViewItemML"] = {view = "PartnerView", subView = nil, btn = "PartnerViewItemML",func = nil}, --选中沫璃

    ["PartnerViewLife"] = {view = "PartnerView", subView = "PartnerAttr", btn = "PartnerViewLifeClick",func = nil}, --选中伙伴命格标签
    ["PartnerViewTianBtn"] = {view = "PartnerLifeView", subView = "PartnerLifeViewF", btn = "PartnerTianClick",func = nil}, --选中伙伴命格天格位
    ["PartnerLifeViewEquipBtn"] = {view = "PartnerLifeEquipView", subView = nil, btn = "PartnerLifeEquipClick",func = nil},  --装备伙伴天格位命格

    ["PartnerViewSoarWeaponBtn"] = {view = "PartnerView", subView = "PartnerAttr", btn = "PartnerViewSoarWeaponBtn",func = nil}, --选中伙伴飞升武器按钮
    ["PartnerViewSoarWeaponClick1"] = {view = "PartnerSoarWeaponView", subView = "PartnerSoarWeaponViewF", btn = "GuideObj1",func = nil}, --选中伙伴飞升武器区域1
    ["PartnerViewSoarWeaponClick2"] = {view = "PartnerSoarWeaponView", subView = "PartnerSoarWeaponViewF", btn = "GuideObj2",func = nil}, --选中伙伴飞升武器区域2
    ["PartnerViewSoarWeaponClick3"] = {view = "PartnerSoarWeaponView", subView = "PartnerSoarWeaponViewF", btn = "GuideObj3",func = nil}, --选中伙伴飞升武器区域3

    -- ["PartnerQuaViewEquipBtn"] = {view = "PartnerLifeEquipView", subView = nil, btn = "PartnerLifeEquipClick",func = nil},  --装备伙伴天格位命格

    ["FightView"] = {view = "MainView", subView = nil, btn = "FightBtn",func = nil}, --打开打怪界面
    ["FightViewDungeon"] = {view = "FightView", subView = nil, btn = "FightViewTabDungeon",func = nil}, --选中打怪副本标签
    ["FightViewDungeonBtn"] = {view = "FightView", subView = "FightDungeon", btn = "FightDungeonClick",func = nil}, --选中打怪副本页面开始战斗按钮

    ["FightViewDungeonItem"] = {view = "FightView", subView = "FightDungeon", btn = "FightViewDungeonItem",func = nil}, --选中打怪副本页面南丘墓地
    ["FightViewDungeonSweepBtn"] = {view = "FightView", subView = "FightDungeon", btn = "FightViewDungeonSweepClick",func = nil}, --选中打怪副本页面快速扫荡按钮
    ["FightCrystalSweepViewConfirmBtn"] = {view = "FightCrystalSweepView", subView = nil, btn = "FightCrystalSweepConfirmClick",func = nil}, --选中副本扫荡页面确定

    ["SocietyView"] = {view = "MainView", subView = nil, btn = "SocietyBtn",func = nil}, --打开社交界面
    ["SocietyViewMasterApprentice"] = {view = "SocietyView", subView = nil, btn = "SocietyViewTabMA",func = nil}, --选中社交师徒标签

    ["PetView"] = {view = "MainView", subView = "TopRight", btn = "PetHeadObjClick", func = nil},   --打开宠物界面
    ["PetViewClose"] = {view = "PetView", subView = "Board", btn = "BtnCloseClick", func = nil},  --宠物界面关闭按钮
    ["PetExpAdd"] = {view = "PetView", subView = "PetAttr", btn = "BtnAddExp",func = nil},  --打开经验界面
    ["PetExpUse"] = {view = "PetExp", subView = nil, btn = "BtnUseOne",func = nil}, --使用经验
    ["PetExpUseOneKey"] = {view = "PetExp", subView = nil, btn = "BtnUseOneKey",func = nil}, --使用经验
    -- ["PetSkill"] = {view = "PetView", subView = nil, btn = "PetTabSkill", func = nil}, -- 跳转技能标签
    -- ["PetSkillPlus"] = {view = "PetView", subView = "PetSkill", btn ="BtnPlus", func = nil}, -- 点击技能加号
    -- ["PetSkillPopSelect"] = {view = "PetSkillChoose", subView = nil , btn = "PetSkillPopFirst", func = nil}, -- 选中第一个技能
    -- ["PetSkillStudy"] = {view = "PetView", subView = "PetSkill", btn = "BtnChange", func = nil}, -- 点击学习
    ["PetHitBookSkill"] = {view = "PetView", subView = nil, btn = "PetTabGetBook", func = nil},   --跳转打书标签（新技能标签）
    ["PetHitBookSkillSelect"] = {view = "PetView", subView = "PetGetBook",btn = "PetHitBookSelectHave", func = nil},    --选中已经拥有技能书的一个技能
    ["PetHitBookskillTip"] = {view = "PetSkillTip", subView = nil, btn = "BtnLearnEvent", func = nil},          --技能描述tip界面的学习按钮
    ["PetXilianTab"] = {view = "PetView", subView = nil, btn = "PetTabXilian", func = nil},     -- 跳转洗炼标签
    ["PetXilianBtn"] = {view = "PetView", subView = "PetXilian", btn = "BtnXilianEvent",func = nil},     --点击洗炼
    ["PetXilianKeepBtn"] = {view = "PetView", subView = "PetXilian", btn = "BtnKeepEvent",func = nil},     --点击保存
    ["PetStrengthenOpen"] = {view = "PetView", subView = "PetAttr", btn = "PetStrengthenOpen",func = nil},  --属性界面宠物强化按钮
    ["PetStrengthenBtn"] = {view = "PetView", subView = nil, btn = "PetStrengthenBtn",func = nil},  --宠物强化界面强化按钮
    ["PetStrengthenSelectBtn"] = {view = "PetView", subView = nil, btn = "PetStrengthenSelectBtn",func = nil},  --宠物强化界面选择按钮
    ["PetStrengthenSelectItem"] = {view = "PetStrSelectCopet", subView = nil, btn = "PetStrengthenSelectItem",func = nil},  --宠物强化选择界面选择宠物
    ["PetStrengthenSelectOk"] = {view = "PetStrSelectCopet", subView = nil, btn = "PetStrengthenSelectOk",func = nil},  --宠物强化选择界面选择宠物确认按钮
    ["PetPiecesComposeBtn"] = {view = "PetPieces", subView = nil, btn = "PetPiecesComposeBtn", func = nil}, -- 宠物碎片合成界面 合成按钮
    ["PetFreeLifeBtn"] = {view = "PetView", subView = "PetAttr", btn = "PetFreeLifeBtn", func = nil},             -- 宠物属性界面放生按钮
    ["PetSingleFreeBtn"] = {view = "PetView", subView = "PetAttr", btn = "PetSingleFreeBtn", func = nil},         -- 宠物属性界面单独放生按钮
    ["FreeLifeConfirmBtn"] = {view = "PetFreelifeTip2", subView = nil, btn = "FreeLifeConfirmBtn", func = nil},     --宠物单独放生界面确认按钮
    ["PetFreeLifeChoose"] = {view = "PetView", subView = "PetAttr", btn = "PetFreeLifeChoose", func = nil},         --宠物界面选择对应宠物
    ["PetReinforceItem"] = {view = "PetView", subView = "PetAttr", btn = "PetReinforceItem", func = nil},         --宠物界面选择强化的宠物
    ["PetFightToggleBtn"] = {view = "PetView", subView = "PetAttr", btn = "PetFightToggleBtn", func = nil},                  --宠物属性界面出战按钮
    ["PetThirdLingWu"] = {view = "PetView", subView = "PetGetBook", btn = "PetThirdLingWu", func = nil},                --打书界面第三个技能格子
    ["PetHitBookHighLevelGift"] = {view = "PetView", subView = "PetGetBook", btn = "HighLevelGiftBtn", func = nil},     --打书界面高级技能书标签
    ["PetSkillReplaceLearnBtn"] = {view = "PetGiftReplace",subView = "PetGiftReplacePanel", btn = "PetSkillReplaceLearnBtn", func = nil},              --宠物技能学习界面学习按钮
    ["PetLingWuConfirm"] = {view = "PetLingwuSkillTip", subView = nil, btn = "PetLingWuConfirm", func = nil},                --领悟tip确认按钮
    ["PetTabStrengthen"] = {view = "PetView", subView = nil, btn = "PetTabStrengthen", func = nil},               --强化标签页按钮
    ["PetTabStamp"] = {view = "PetView", subView = "PetStampPanel", btn = "PetTabStamp", func = nil},               --神印标签页按钮
    ["PetStampPrayOpenBtn"] = {view = "PetView", subView = nil, btn = "PetStampPrayOpenBtn", func = nil},               --神印入口按钮
    ["PetStampPrayBtn"] = {view = "PetStampPrayView", subView = nil, btn = "PetStampPrayBtn", func = nil},               --神印单次祈神按钮
    ["PetHitBookReplaceConfirm"] = {view = "PetGiftReplaceResultView", subview = nil, btn = "ConfirmBtn", func = nil},      --打书结果保存界面保存按钮
    ["PetHitBookReplaceDialogConfirm"] = {view = "PetGiftReplaceResultDialog", subview = nil, btn = "ConfirmClick", func = nil}, --打书结果保存提示框确认按钮

    ["PetZhiZhi"] = {view = "PetView", btn = "PetZhiZhi", func = nil},
    ["PetGaiMingBtn"] = {view = "PetView", btn = "PetGaiMingBtn", func = nil},

    ["GaiMingUpOne"] = {view = "PetGaiMing", btn = "GaiMingUpOne"},
    ["GaiMingKuang"] = {view = "PetGaiMing", btn = "GaiMingKuang"},

    ["Workshop"] = {view = "MainView", subView = nil, btn = "WorkshopBtn",func = nil},  --打开工坊
    ["Equip"] = {view = "MainView", subView = nil, btn = "EquipBtn",func = nil},  --打开装备
    ["WorkshopInset"] = {view = "EquipView", subView = nil, btn = "WorkshopTabInset",func = nil}, --跳转标签
    ["WorkshopStone"] = {view = "EquipView", subView = nil, btn = "WorkshopStoneUp",func = nil}, --穿宝石
    ["WorkshopStoneTarget"] = {view = "EquipView", subView = nil, btn = "WorkshopStoneTarget",func = nil}, --选装备
    ["WorkshopStoneSell"] = {view = "EquipView", subView = "InsetTab", btn = "EquipInsetBtnSell",func = nil}, --卖宝石

    ["WorkshopTabJewelry"] = {view = "Workshop", subView = nil, btn = "WorkshopTabJewelry",func = nil}, --跳转标签灵饰
    ["WorkshopJewelryMake"] = {view = "Workshop", subView = "JewelryTab", btn = "BtnMake2",func = nil}, --做灵饰

    ["WorkshopNeoCompose"] = {view = "Workshop", subView = nil, btn = "WorkshopTabNeoCompose",func = nil}, -- 跳转标签合成
    ["WorkshopNeoComposeSelect"] = {view = "Workshop", subView = nil,btn = "ComposeItem", func = nil}, -- 选中第一个
    ["WorkshopNeoComposeCrystal"] = {view = "Workshop", subView = "NeoComposeTab",btn = "BtnCrystal", func = nil}, -- 水晶制造

    ["WorkshopForingSelect"] = {view = "Workshop", subView = nil, btn = "ForingItem",func = nil}, --选中要锻造的对象
    ["WorkshopForing"] = {view = "Workshop", subView = "Foring", btn = "BtnCreate",func = nil}, --锻造
    ["WorkshopForingSingle"] = {view = "Workshop", subView = "Foring", btn = "BtnCreateSingle",func = nil}, --锻造
    ["WorkshopForingStuffCell2"] = {view = "Workshop", subView = "Foring", btn = "StuffCell2",func = nil}, --选中第二个锻造材料
    ["GetWayGotoMark"] = {view = "ItemInfoView", subView = nil, btn = "GetWayGotoMark",func = nil}, --选中前往市场
    ["GetWayGotoMY"] = {view = "GetWayView", subView = nil, btn = "GetWayGotoMY",func = nil}, --选中前往梦渊谷
    ["GetWayGotoSH"] = {view = "GetWayView", subView = nil, btn = "GetWayGotoSH",func = nil}, --选中前往商会
    
    
    ["WorkshopUpgrade"] = {view = "EquipView", subView = nil, btn = "WorkshopTabUpgrade",func = nil}, -- 跳转装备的标签强化 
    ["WorkshopUpgradeSelect"] = {view = "EquipView", subView = nil,btn = "UpgradeWeaponSelect", func = nil}, -- 选中武器
    ["WorkshopUpgradeClick"] = {view = "EquipView", subView = "Foring",btn = "BtnUpGrade", func = nil}, -- 强化按钮

    ["WorkshopNeveal"] = {view = "EquipView", subView = nil, btn = "WorkshopTabNeveal",func = nil}, -- 跳转装备的标签点化 
    ["WorkshopNevealSelect"] = {view = "EquipView", subView = nil,btn = "NevealItemSelect", func = nil}, -- 选中第一个
    ["WorkshopNevealFlush"] = {view = "EquipView", subView = "NevealTab",btn = "NevealFlushShow", func = nil}, -- 更新框
    ["WorkshopNevealCur"] = {view = "EquipView", subView = "NevealTab",btn = "NevealCurShow", func = nil}, -- 现存框
    ["WorkshopNevealClick"] = {view = "EquipView", subView = "NevealTab",btn = "BtnNeveal", func = nil}, -- 点化按钮

    ----学第一个技能
    ["LearSkill1"] = {view = "RoleView", subView = nil, btn = "LSTabbar",func = nil}, --点击角色/技能标签页
    ["LearSkill2"] = {view = "RoleView", subView = nil, btn = "LSSecondSkill",func = nil}, --点击第二个技能
    ["LearSkill3"] = {view = "RoleView", subView = "RoleSkill", btn = "BtnLearn",func = nil}, --点击学习（旧，已失效）
    ["LearSkill4"] = {view = "RoleView", subView = "RoleSkill", btn = "LearSkill4",func = nil}, --点击学习（新）
    ["AllSkillLearn"] = {view = "RoleView", subView = "RoleSkill", btn = "AllSkillLearn",func = nil}, --一键升级（新）
    ["TrainBtnObj"] = {view = "RoleView", subView = "RoleSkill", btn = "TrainBtnObj",func = nil}, --点击修炼
    ["LSSpecialSkill"] = {view = "RoleView", subView = nil, btn = "LSSpecialSkill",func = nil},  --学习特定技能（根据指引表skill_id分表）
    ["RoleViewClose"] = {view = "RoleView", subView = "Board", btn = "BtnCloseClick",func = nil}, --角色界面关闭

    ["IllustratedFirst"] = {view = "MainView", subView = nil, btn = "IllustratedBtn",func = nil},  --打开图鉴
    ["IllustratedOpen"] = {view = "IllustrateView",subView = nil,btn = "BtnIllustrated",func = nil}, --图鉴激活

    ["ActivityView"] = {view = "MainView", subView = nil, btn = "ActivityViewBtn",func = nil},  --打开活跃

    ["Challenge"] = {view = "MainView", subView = nil, btn = "ChallengeBtn",func = nil},  --打开挑战
    ["CourageChallengeSelect"] = {view = "MainView",subView = nil,btn = "CourageChallengeSelectBtn",func = nil},--锢魔之塔选择
    ["GuMoNPC"] = {view = "NpcDialogView", subView = nil, btn = "GuildTaskNPC", func = nil},--锢魔NPC
    ["CourageChallenge"] = {view = "CourageChallengeView",subView = nil,btn = "BtnEnter",func = nil},--锢魔之塔打开

    ["SchoolExerciseBtn"] = {view = "MainView",subView = nil,btn = "SchoolExerciseBtn",func = nil},--校场演武选择
    ["SchoolExerciseFirstBtn"] = {view = "SchoolExerciseView",subView = nil,btn = "SchoolExerciseFirstBtn",func = nil},--校场演武简单
    ["SchoolExerciseFightBtn"] = {view = "SchoolExerciseView",subView = nil,btn = "SchoolExerciseFightBtn",func = nil},--校场演武演练

    ["Athletics"] = {view = "MainView", subView = nil, btn = "AthleticsBtn",func = nil},  --打开竞技
    
    ["ArenaSelect"] = {view = "AthleticsView",subView = nil,btn = "ArenaSelectBtn",func = nil}, --竞技场选择
    ["ArenaView"] = {view = "ArenaView",subView = nil,btn = "ArenaBtn",func = nil},--古仙宝库打开
    ["LeiTaiZhanBtn"] = {view = "AthleticsView",subView = nil,btn = "LeiTaiZhanBtn",func = nil}, --擂台战选择
    ["XiFangQiSuBtn"] = {view = "ArenaMatchView",subView = nil,btn = "XiFangQiSuBtn",func = nil}, --擂台战选择

    ["DialogTipsConfirm"] = {view = "DialogTipsView",subView = nil,btn = "ConfirmClick",func = nil}, --提示窗确认

    ["GuildTask"] = {view = "ActiveView",subView = nil,btn = "GuildTaskBtn",func = nil},--门派任务选择
    ["ResearchTask"] = {view = "ActiveView",subView = nil,btn = "ResearchTaskBtn",func = nil},--三界除魔选择
    ["TreasureMap"] = {view = "ActiveView",subView = nil,btn = "TreasureMapBtn",func = nil},--缉捕盗贼
    ["WSTQuesten"] = {view = "ActiveView",subView = nil,btn = "WSTQuestenBtn",func = nil},--神算子的考验
    ["FightDen"] = {view = "ActiveView",subView = nil,btn = "FightDenBtn",func = nil},--万妖谷
    ["WLStorybookBtn"] = {view = "ActiveView",subView = nil,btn = "WLStorybookBtn",func = nil},--万灵话本
    ["TreasureTaskBtn"] = {view = "ActiveView",subView = nil,btn = "TreasureTaskBtn",func = nil},--九州密宝
    ["TwiceDailyBtn"] = {view = "ActiveView",subView = nil,btn = "TwiceDailyBtn",func = nil},--万灵话本
    ["FourDailyBtn"] = {view = "ActiveView",subView = nil,btn = "FourDailyBtn",func = nil},--万灵话本
    ["ActiveDailyRightArea"] = {view = "ActiveView", subView = nil, btn = "ActiveDailyRightArea", func = nil},  --活跃日常界面右侧区域

    ["BagViewOpen"] = {view = "MainView", subView = nil, btn = "BagViewOpenBtn",func = nil},  --打开背包
    ["BagEquipSelect"] = {view = "BagView", subView = nil, btn = "BagEquipSelectBtn",func = nil},  --背包装备选择
    ["BagEquipUse"] = {view = "ItemInfoView", subView = nil, btn = "BagEquipUseBtn",func = nil},  --背包装备使用
    ["MapViewOpen"] = {view = "MainView",subView = "TopLeft" , btn = "MapIcon",func = function()
        ViewMgr:OpenView(MapView,{show_world = true})
    end}, --点击地图 
    ["MapViewFightSelect"] = {view = "FightMapView",subView = nil , btn = "ButtonNextFB",func = nil}, --点击地图上的传说点（仅限副本
    ["MapXuanDuNanJiao"] = {view = "MapView",subView = nil , btn = "MapXuanDuNanJiao",func = nil}, --点击地图
    ["MapWanCun"] = {view = "MapView",subView = nil , btn = "MapWanCun",func = nil}, --点击地图
    ["WorldMapWanCun"] = {view = "MapView",subView = nil , btn = "WorldMapWanCun",func = nil}, --世界地图上的宛村

    ["BagResolveEquip"] = {view = "BagView", subView = nil, btn = "BagResolveEquip", func = nil},    -- 装备分解选中
    ["BagResolveEquipBtn"] = {view = "ItemInfoView", subView = nil, btn = "BagResolveEquipBtn",func = nil},  --背包装备使用

    ["ReputationSelect"] = {view = "ReputationShopView", subView = nil, btn = "ReputationSelectBtn",func = nil}, --百货铺选择
    ["ReputationBuy"] = {view = "ReputationShopView", subView = "ReputationShopPanel", btn = "BtnPurchase", func = nil},--百货铺购买
    ["ReputationLuYaoSelectItem"] = {view = "ReputationShopView", subView = nil, btn = "ReputationLuYaoSelectItem",func = nil}, --百货铺选选择鹿尧碎片
    ["ReputationClose"] = {view = "ReputationShopView", subView = nil, btn = "ReputationClose",func = nil}, --百货铺关闭

    ["DialogTipsOK"] = {view = "DialogTipsView", subView = nil, btn = "DialogTipsOK",func = nil}, --弹框确认

    ["GuildTaskNPC"] = {view = "NpcDialogView", subView = nil, btn = "GuildTaskNPC", func = nil},--门派任务NPC
    ["GuildTaskTakeAll"] = {view = "DailyGuildTask", subView = "DailyGuildTaskFlushView", btn = "BtnAllTake", func = nil},--门派任务全部领取

    ["MainViewUpgrade"] = {view = "MainView", subView = "TopRight", btn = "UpgradeButton", func = nil},--提升按钮
    ["MainViewUpgradeRoleAttrImprove"] = {view = "MainView", subView = nil, btn = "RoleAttrImprove", func = nil},--人物加点
    ["MainViewUpgradeNewSkillUnlocked"] = {view = "MainView", subView = nil, btn = "NewSkillUnlocked", func = nil},--人物加点

    ["FightDenEnter"] = {view = "FightDenReward2",subView =nil , btn = "FightDenBtnEnter", func = nil}, --万妖谷 进入
    ["FightDenTeamMatch"] = {view = "TeamPlatformView",subView = nil, btn = "BtnTeamMatch",func = nil}, --万妖谷 匹配

    ["ShiLianTeamMatch"] = {view = "TeamPlatformView",subView = nil, btn = "BtnTeamMatch2",func = nil}, --试炼之地 匹配

    ["EquipDonationSelect"] = {view = "BagView", subView = nil, btn = "EquipDonationSelectBtn",func = nil},  --捐赠装备选中
    ["EquipDonationUse"] = {view = "ItemInfoView", subView = nil, btn = "EquipDonationUseBtn",func = nil},   --装备选择捐赠
    ["DonationVillage"] = {view = "ReputationDonationView",subView = nil,btn = "DonationVillageBtn",func = nil},--选中村子
    ["DonationConfirm"] = {view = "ReputationDonationView", subView = "ReputationDonationPanel", btn = "DonationConfirmBtn", func = nil},--点击捐赠
    ["BagPetPiecesCompose"] = {view = "BagView", subView = nil, btn = "BagPetPiecesCompose", func = nil},    --宠物碎片合成选中

    -- 选中使用银蛋
    ["SilverEggSelect"] = {view = "BagView", subView = nil, btn = "SilverEggSelect", func = nil},    --宠物银蛋选中
    ["PetEggClose"] = {view = "PetGain", subView = nil, btn = "PetEggClose", func = nil},    --宠物蛋界面关闭
    ["PetStrSuccClose"] = {view = "PetStrSucc", subView = nil, btn = "PetStrSuccClose", func = nil},    --宠物蛋界面关闭

    ["MainTask"] = {view = "MainView", btn = "MainTask"},--任务栏第一个
    ["MainViewMainTask"] = {view = "MainView", subView = nil, btn = "BtnMainTask",func = nil},  --主线任务
    ["MainTaskEliteBtn"] = {view = "MainTaskView", subView = nil, btn = "BtnEliteChange",func = nil},  --主线任务 精英按钮
    ["MainTaskFirstEliteBoss"] = {view = "MainTaskView", subView = nil, btn = "FirstEliteBoss",func = nil},  --主线任务 第一只boss（精英
    ["TaskBox"] = {view = "MainView", btn = "TaskBox"},--任务框整个
    ["SecondTaskItem"] = {view = "MainView", btn = "SecondTaskItem"},
    
    ["BtnStrategy"] = {view = "MainView", subView = nil, btn = "BtnStrategy", func = nil},--战术按钮
    ["BtnJiHuo"] = {view = "MainView", subView = nil, btn = "BtnJiHuo", func = nil},--战术按钮
    ["BtnStrategy2"] = {view = "MainView", subView = nil, btn = "BtnStrategy2", func = nil},--战术按钮

    ["FabaoBtn"] = {view = "MainView", subView = nil, btn = "FabaoBtn", func = nil},--主界面法宝按钮
    ["FabaoCreateBtn"] = {view = "FabaoNeoView", subView = nil, btn = "FabaoCreateBtn", func = nil}, --法宝界面炼化按钮

    ["SocietyChatQuickCreate"] = {view = "SocietyView", subView = "SocietyFriendGroup", btn = "GroupCreateClick",func = nil}, --社交聊天创建群组
    ["SocietyChatQuickSel"] = {view = "SocietyView", subView = "SocietyFriendGroup", btn = "SocietyChatQuickSel",func = nil}, --社交聊天群组菜单
    ["SocietyChatQuickAdd"] = {view = "RoleInfoView", subView = nil, btn = "SocietyChatQuickAdd",func = nil}, --社交聊天快捷添加
    ["SocietyChatQuickItem"] = {view = "MainView", subView = "Chat", btn = "SocietyChatQuickItem", func = nil},--主界面快捷聊天

    ["MainViewTreasureHunt"] = {view = "MainView", subView = nil, btn = "TreasureHuntViewBtn", func = nil},
    ["TreasureHuntPetHunt"] = {view = "TreasureHuntView", subView = nil, btn = "TreasureHuntPetHunt", func = nil},
    ["TreasureHuntPartnerHunt"] = {view = "TreasureHuntView", subView = nil, btn = "TreasureHuntPartnerHunt", func = nil},
    ["OnceButtonObj"] = {view = "TreasureHuntView", subView = nil, btn = "OnceButtonObj", func = nil},
    ["FiveButtonObj"] = {view = "TreasureHuntView", subView = nil, btn = "FiveButtonObj", func = nil},
    ["OnceButtonNormal"] = {view = "TreasureHuntView", subView = nil, btn = "OnceButtonNormal", func = nil},
    ["FiveButton"] = {view = "TreasureHuntView", subView = "TreasureHuntFlush", btn = "FiveButton", func = nil},


    ["DreamNotesSelect"] = {view = "FightFBView",subView = nil,btn = "DreamNotesBtn",func = nil},              --秘境 梦渊谷 选择
    ["DreamNotesLevel1Select"] = {view = "FightFBView",subView = nil,btn = "DreamNotesViewLevel1",func = nil}, --梦渊笔录界面 选择第一关
    ["DreamNotesLevel2Select"] = {view = "FightFBView",subView = nil,btn = "DreamNotesViewLevel2",func = nil}, --梦渊笔录界面 选择第二关
    ["DreamNotesLeve13Select"] = {view = "FightFBView",subView = nil,btn = "DreamNotesViewLevel3",func = nil}, --梦渊笔录界面 选择第三关
    ["DreamNotesLeve14Select"] = {view = "FightFBView",subView = nil,btn = "DreamNotesViewLevel4",func = nil}, --梦渊笔录界面 选择第四关
    ["DreamNotesChallenge"] = {view = "FightFBView", subView = nil, btn = "DreamNotesChallengeBtn",func = nil},--梦渊笔录界面 点击挑战
    ["DreamNotesSweep"] = {view = "FightFBView", subView = nil, btn = "DreamNotesSweepBtn",func = nil},--梦渊笔录界面 点击扫荡
    ["DreamNotesFinish"] = {view = "FinishHurtView", subView = nil, btn = "DreamNotesFinishBtn",func = nil},   --梦渊笔录结算界面
    ["DreamNotesLevelSelect"] = {view = "FightFBView",subView = nil,btn = "DreamNotesViewLevel",func = nil}, --梦渊笔录界面 选择

    ["OnHook"] = {view = "MainView",subView = nil,btn = "OnHookBtn",func = nil},--挂机按钮
    ["FightScene"] = {view = "FightView",subView = nil,btn = "FightSceneBtn",func = nil},--秘境当前场景第一个 
    ["AutoSeal"] = {view = "FightView",subView = nil,btn = "AutoSealBtn",func = nil},--勾选自动封印
    ["EnterAutoSeal"] = {view = "FightAutoSealSetView",subView = nil,btn = "EnterAutoSealBtn",func = nil},--前往自动封印

    ["BagTreasureMap"] = {view = "BagView",subView = nil,btn = "BagTreasureMap",func = nil},--选中高级藏宝图
    ["TreasureUseBtn"] = {view = "ItemInfoView",subView = nil,btn = "TreasureUseBtn",func = nil},--点击使用藏宝图
    ["KeyUseBtn"] = {view = "KeyUseView",subView = nil,btn = "KeyUseBtn",func = nil},--点击挖掘 小面板通用

    ["BicycleMain"] = {view = "MainView", subView = nil, btn = "BicycleMain", func = nil},--主界面灵骑按钮
    ["BicycleTab1"] = {view = "BicycleView",subView = nil,btn = "BicycleTab1",func = nil}, --灵骑基础标签
    ["BicycleTab2"] = {view = "BicycleView",subView = nil,btn = "BicycleTab2",func = nil}, --灵骑培养标签
    ["BicycleTab3"] = {view = "BicycleView",subView = nil,btn = "BicycleTab3",func = nil}, --灵骑技能标签
    ["BicycleTab4"] = {view = "BicycleView",subView = nil,btn = "BicycleTab4",func = nil}, --灵骑御宠标签
    ["BicycleFirst"] = {view = "BicycleView",subView = nil,btn = "BicycleFirst",func = nil}, --灵骑选中第一个
    ["BicycleActive"] = {view = "BicycleView",subView = nil,btn = "BicycleActive",func = nil}, --灵骑激活第一个
    ["BicycleShowCloseBtn"] = {view = "BicycleShowView",subView = nil,btn = "BicycleShowCloseBtn",func = nil}, --灵骑展示界面关闭
    ["BicycleFosterBtn"] = {view = "BicycleView",subView = "FosterPanelSub",btn = "BicycleFosterBtn",func = nil}, --灵骑培养按钮
    ["BicycleAttriGuide"] = {view = "BicycleView",subView = "FosterPanelSub",btn = "BicycleAttriGuide",func = nil}, --灵骑培养属性展示
    ["BicycleStuffGuide"] = {view = "BicycleView",subView = "FosterPanelSub",btn = "BicycleStuffGuide",func = nil}, --灵骑培养材料展示
    ["BicycleGuideAddPetBtn"] = {view = "BicycleView",subView = nil,btn = "BicycleGuideAddPetBtn",func = nil}, --灵骑御宠添加按钮
    ["BicycleSelectPetBtn"] = {view = "BicycleSelectPetView",subView = nil,btn = "BicycleSelectPetBtn",func = nil}, --灵骑御宠选择按钮
    ["BicycleSelectPetOKBtn"] = {view = "BicycleSelectPetView",subView = nil,btn = "BicycleSelectPetOKBtn",func = nil}, --灵骑御宠确认按钮
    ["BicycleRideOKBtn"] = {view = "BicycleView",subView = nil,btn = "BicycleRideOKBtn",func = nil}, --灵骑基础界面-骑乘按钮
    ["BicycleSkillGuideFrame"] = {view = "BicycleView",subView = nil,btn = "BicycleSkillGuideFrame",func = nil}, --灵技界面-灵技信息介绍
    ["BicycleCloseBtn"] = {view = "BicycleView",subView = nil,btn = "BicycleCloseBtn",func = nil}, --灵骑关闭

    ["OfflineRewardMain"] = {view = "MainView", subView = nil, btn = "OfflineRewardMain", func = nil},--主界面境界按钮
    ["OfflineRewardBreathing"] = {view = "OfflineRewardView", subView = nil, btn = "OfflineRewardBreathing", func = nil},--境界吐纳按钮
    ["OfflineRewardBreak"] = {view = "OfflineRewardView", subView = nil, btn = "OfflineRewardBreak", func = nil},--境界突破按钮
    ["OfflineRewardFightSoul"] = {view = "OfflineRewardView", subView = nil, btn = "OfflineRewardFightSoul", func = nil},--境界挑战按钮
    ["OfflineRewardFightSoulOne"] = {view = "FightSoulView", subView = nil, btn = "OfflineRewardFightSoulOne", func = nil},--境界中心灵之境左侧第一个按钮
    ["OfflineRewardFightSoulTwo"] = {view = "FightSoulView", subView = nil, btn = "OfflineRewardFightSoulTwo", func = nil},--境界中心灵之境左侧第二个按钮
    ["OfflineRewardFightSoulFight"] = {view = "FightSoulView", subView = nil, btn = "OfflineRewardFightSoulFight", func = nil},--境界中心灵之境挑战按钮
    ["OfflineRewardFightSoulClose"] = {view = "FightSoulView", subView = nil, btn = "OfflineRewardFightSoulClose", func = nil},--境界中心灵之境关闭按钮

    ["TowerAdventureLevelCenterItems1"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems1", func = nil},--小游戏塔楼冒险选关第一关
    ["TowerAdventureLevelCenterItems2"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems2", func = nil},--小游戏塔楼冒险选关第二关
    ["TowerAdventureLevelCenterItems3"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems3", func = nil},--小游戏塔楼冒险选关第三关
    ["TowerAdventureLevelCenterItems4"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems4", func = nil},--小游戏塔楼冒险选关第四关
    ["TowerAdventureLevelCenterItems5"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems5", func = nil},--小游戏塔楼冒险选关第五关
    ["TowerAdventureLevelCenterItems6"] = {view = "TowerAdventureLevelView", subView = nil, btn = "TowerAdventureLevelCenterItems6", func = nil},--小游戏塔楼冒险选关第六关

    ["CowBoyLevelCenterItems1"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems1", func = nil},--小游戏牛郎织女选关第一关
    ["CowBoyLevelCenterItems2"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems2", func = nil},--小游戏牛郎织女选关第二关
    ["CowBoyLevelCenterItems3"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems3", func = nil},--小游戏牛郎织女选关第三关
    ["CowBoyLevelCenterItems4"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems4", func = nil},--小游戏牛郎织女选关第四关
    ["CowBoyLevelCenterItems5"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems5", func = nil},--小游戏牛郎织女选关第五关
    ["CowBoyLevelCenterItems6"] = {view = "CowBoyLevelView", subView = nil, btn = "CowBoyLevelCenterItems6", func = nil},--小游戏牛郎织女选关第六关

    ["MaterialSelect"] = {view = "FightFBView", subView = nil, btn = "MaterialBtn", func = nil},                       --秘境 玉虚境 选择
    ["MaterialLevelSelect"] = {view = "FightFBView", subView = nil, btn = "MaterialLevelSelectBtn", func = nil},       --玉虚境 混沌时空按钮                       
    ["MaterialChallengeSelect"] = {view = "MaterialChallengeView", subView = nil , btn = "MaterialChallengeSelectBtn"},--玉虚境 选择第一关
    ["MaterialChallenge"] = {view = "MaterialChallengeView", subView = "MaterialChallengePanel", btn = "BtnChallenge"},--玉虚境 点击挑战

    ["PetIslandBtn"] = {view = "FightFBView", subView = nil, btn = "PetIslandBtn",func = nil}, --选中灵宠岛标签
    ["PetIslandDice"] = {view = "FightFBView", subView = nil, btn = "PetIslandDice"},     --灵宠岛骰子按钮
    ["PetIslandSeal"] = {view = "FightFBView", subView = nil, btn = "PetIslandSeal"},       --灵宠岛封印按钮

    ["FightFBYMZBtn"] = {view = "FightFBView", subView = nil, btn = "FightFBYMZBtn",func = nil}, --选中云梦泽标签
    ["FightFBYMZSeq5"] = {view = "FightFBView", subView = nil, btn = "FightFBYMZSeq5",func = nil}, --选中深海洞窟
    ["FightFBYMZFight"] = {view = "FightFBView", subView = "FightFBYMZ", btn = "FightClick",func = nil}, --开始战斗
    ["FightFBYMZSweep"] = {view = "FightFBView", subView = "FightFBYMZ", btn = "SweepClick",func = nil}, --快速扫荡

    ["ChallengeTestFormationBtn"] = {view = "ChallengeTestView", subView = "ChallengeTestViewF", btn = "ChallengeTestFormationBtn",func = nil}, --选中轮回忘川布阵按钮

    ["FormationViewBtn"] = {view = "MainView", subView = nil, btn = "FormationViewBtn",func = nil},  --打开布阵
    ["FormationPartnerSY"] = {view = "FormationView", subView = "FormationViewF", btn = "FormationPartnerSY",func = nil}, --选中水月
    ["Formation4Click"] = {view = "FormationView", subView = "FormationViewF", btn = "Formation4Click",func = nil}, --选中4号位

    ["FormationPatnerFirst"] = {view = "FormationView", subView = "FormationViewF", btn = "FormationPatnerFirst",func = nil}, --选中轮回忘川布阵按钮
    ["FormationCrystalBtn"] = {view = "FormationView", subView = "FormationViewF", btn = "FormationCrystalBtn",func = nil}, --选中轮回忘川布阵按钮
    ["FormationPetBtn"] = {view = "FormationView", subView = "FormationViewF", btn = "FormationPetBtn",func = nil}, --选中轮回忘川布阵按钮

    ["FabaoLotteryBtn"] = {view = "FabaoView", subView = nil, btn = "LotBtn"},          --法宝界面聚灵炼器按钮
    ["FabaoLotteryStove"] = {view = "FabaoLottery", subView = nil, btn = "Stove1Btn"},   --法宝抽奖炉子1
    ["FabaoLotteryFilterOK"] = {view = "FabaoFiller", subView = nil, btn = "OneKeyBtn"}, --法定抽奖填充材料一键
    ["FabaoLotteryFilterStart"] = {view = "FabaoFiller", subView = nil, btn = "StartBtn"}, --法定抽奖开始按钮

    ["CloudArenaTargeItem"] = {view = "CloudArenaView", subView = nil, btn = "CloudArenaTargeItem"}, --指引对手列表
    ["CloudArenaTargetInfo"] = {view = "CloudArenaView", subView = nil, btn = "CloudArenaTargetInfo"}, --对手信息
    ["CloudArenaChallengeBtn"] = {view = "CloudArenaView", subView = nil, btn = "CloudArenaChallengeBtn"}, --挑战按钮
    ["CloudArenaSelfInfo"] = {view = "CloudArenaView", subView = nil, btn = "CloudArenaSelfInfo"}, --个人信息
    ["CloudArenaFormationBtn"] = {view = "CloudArenaView", subView = nil, btn = "CloudArenaFormationBtn"},  --布阵按钮

    ["CloudArenaFormationAssistObj"] = {view = "ActFormationView", subView = nil, btn = "ActFormationAssistObj"}, --宠物助阵信息
    ["CloudArenaFormationPetToggle"] = {view = "ActFormationView", subView = "ActFormationPanel", btn = "PetToggleClick"}, --宠物标签
    ["CloudArenaFormationChallengeBtn"] = {view = "ActFormationView", subView = "ActFormationPanel", btn = "ChallengetBtnClick"}, --挑战按钮
    ["ActFormationGuidePet"] = {view = "ActFormationView", subView = nil, btn = "ActFormationGuidePet"}, --指引对应id的宠物
    ["ActFormationEmptyPetSlot"] = {view = "ActFormationView", subView = nil, btn = "ActFormationEmptyPetSlot"}, --默认空栏位
    
    ["ChallengeNavi_30002"] = {view = "ExperienceView", subView = nil, btn = "ChallengeNavi_30002"}, --历练界面导航，锢魔之塔
    ["ChallengeNavi_30005"] = {view = "ExperienceView", subView = nil, btn = "ChallengeNavi_30005"}, --历练界面导航，校场演武
    ["ChallengeNavi_30006"] = {view = "ExperienceView", subView = nil, btn = "ChallengeNavi_30006"}, --历练界面导航，轮回忘川
    ["ChallengeNavi_30014"] = {view = "ChallengeNaviView", subView = nil, btn = "ChallengeNavi_30014"}, --挑战界面导航，小游戏塔楼冒险
    ["ChallengeNavi_30015"] = {view = "ChallengeNaviView", subView = nil, btn = "ChallengeNavi_30015"}, --挑战界面导航，小游戏牛郎织女
    ["ExperienceViewBtn"] = {view = "MainView", subView = nil, btn = "ExperienceViewBtn",func = nil},  --历练界面主界面按钮

    ["AutoBeh1"] = {view = "MainView",  subView = nil, btn = "AutoBeh1"},--人物自动技能
    ["AutoBeh2"] = {view = "MainView",  subView = nil, btn = "AutoBeh2"},--人物二动/宠物自动技能

    ["AISKillBtn"] = {view = "MainView", subView = nil , btn = "AISKillBtn"},--人物只能施法

    ["YinYangEyeBtn"] = {view = "MainView", subView = nil, btn = "YinYangEyeBtn"},--阴阳眼开关按钮

    ["RolePromoteBtn"] = {view = "MainView", subView = nil, btn = "RolePromoteBtn",func = nil},  --晋阶主界面按钮
    ["RolePromoteBtn2"] = {view = "MainView", subView = nil, btn = "RolePromoteBtn2",func = nil},  --晋阶主界面按钮非展开
    ["RolePromoteUp"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteUpBtn", func = nil},--晋阶晋升按钮
    ["RolePromoteCond"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteCond", func = nil},--晋阶条件介绍
    ["RolePromoteSkin"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteSkin", func = nil},--晋阶皮肤介绍
    ["RolePromoteClose"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteCloseBtn", func = nil},--晋阶关闭按钮
    ["RolePromoteGo"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteGoBtn", func = nil},--晋阶前往按钮
    ["RolePromoteLevel"] = {view = "RolePromoteView", subView = "RolePromotePanel", btn = "RolePromoteLevel", func = nil},--晋阶信息介绍

    ["RolePromoteSucceCloseBtn"] = {view = "RolePromoteSucceView", subView = nil, btn = "RolePromoteSucceCloseBtn", func = nil},--晋阶成功关闭按钮（全屏)
    
    ["RoleFlyUpPopupCond"] = {view = "FlyUpPopupView", subView = "FlyUpPopupPanel", btn = "RoleFlyUpPopupCond", func = nil},--飞升条件介绍
    ["RoleFlyUpPopupDesc"] = {view = "FlyUpPopupView", subView = "FlyUpPopupPanel", btn = "RoleFlyUpPopupDesc", func = nil},--飞升奖励介绍
    ["RoleFlyUpPopupGo"] = {view = "FlyUpPopupView", subView = "FlyUpPopupPanel", btn = "RoleFlyUpPopupGo", func = nil},    --飞升前往按钮

    ["RoleFlyUpCond"] = {view = "FlyUpView", subView = "FlyUpPanel", btn = "RoleFlyUpCond", func = nil},  --飞升试炼完成条件介绍
    ["RoleFlyUpClose"] = {view = "FlyUpView", subView = "FlyUpPanel", btn = "RoleFlyUpClose", func = nil},--飞升试炼关闭按钮
    ["RoleFlyUpEnter"] = {view = "FlyUpView", subView = "FlyUpPanel", btn = "RoleFlyUpEnter", func = nil},    --飞升凝练按钮
    ["MainRoleFlyUpDesc"] = {view = "MainView", subView = nil, btn = "MainRoleFlyUpDescBtn"},             --再次进入飞升入口介绍

    ["AllSoulsStorybookItem"] = {view = "AllSoulsStorybook", subview = nil, btn = "AllSoulsStorybookItem", func = nil}, --万灵话本第一个关卡
    ["AllSoulsStorybookBtn"] = {view = "AllSoulsStorybook", subview = nil, btn = "AllSoulsStorybookBtn", func = nil}, --万灵话本前往挑战按钮

    ["GuideCleanBtn"] = {view = "GuideClearEffectView",subview = nil, btn = "GuideCleanBtn", func = nil},   --净化阵法界面增加净化指引

    ["TradeRoadMoney"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadMoney", func = nil},   --丝绸之路 货币栏资金
    ["TradeRoadBag"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadBag", func = nil},       --货币栏背包
    ["TradeRoadSupply"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadSupply", func = nil}, --货币栏补给
    ["TradeRoadMarketShop"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadMarketShop", func = nil}, --货币栏补给
    ["TradeRoadMarketBag"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadMarketBag", func = nil}, --货币栏补给
    ["TradeRoadTabTavern"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadTabTavern", func = nil}, --酒馆按钮
    ["TradeRoadTavernDesc"] = {view = "TradeRoadView", subview = nil, btn = "TradeRoadTavernDesc", func = nil}, --酒馆说明

    ["ActiveActivityBackSelect"] = {view = "ActiveView",subView = nil,btn = "ActiveActivityBackSelect",func = nil},--选择找回按钮
    ["ActiveActivityBackBtn"] = {view = "ActiveView", subView = nil, btn = "ActiveActivityBackBtn", func = nil},--一键找回按钮
    ["LevelComplementView"] = {view = "MainView", subview = nil, btn = "LevelComplementBtn", func = nil},    --助力经验按钮

    -- ["Mall"]
    ["MarketSellSelectStone"] = {view = "MallView", subView = nil, btn = "MarketSelectSellStone", func = nil},    --市场界面选择要卖出的宝石
    ["MarketViewBtnOperate"] = {view = "MallView", subView = "MarketTab", btn ="MarketViewBtnPurchase", func = nil},    --市场界面的操作按钮
    ["MallViewBtnClose"] = {view = "MallView", subView = "Board", btn ="BtnClose", func = nil},    --市场界面的关闭按钮
    ["MallCurrencyLingYu"] = {view = "MallView", subView = nil, btn = "MallCurrencyLingYu", func = nil},        --市场界面灵玉提示性指引无实际操作

    ["GiveNoticeCloseBtn"] = {view = "GivePetNoticeView", subview = nil, btn = "GiveNoticeCloseBtn", func = nil}, -- 刑天领取界面关闭按钮
    ["GiveNoticePanel"] = {view = "GivePetNoticeView", subview = nil, btn = "GiveNoticePanel", func = nil}, -- 刑天领取界面关闭按钮
    ["LoginRewardGetBtn"] = {view = "LoginRewardView", subview = nil, btn = "LoginRewardGetBtn", func = nil}, -- 28天登陆有礼领取第一天奖励按钮
    ["XingTianCell"] = {view = "LoginRewardView", subview = nil, btn = "XingTianCell", func = nil}, -- 28天登陆有礼领取第一天奖励按钮
    ["LoginRewardActBtn"] = {view = "MainView", subview = nil, btn = "LoginRewardActBtn", func = nil}, -- 28天登陆活动按钮

    ["AnecdoteLeftArea"] = {view = "AnecdoteHintView",subview = nil, btn = "AnecdoteLeftArea", func = nil}, --万灵大事件左侧指引区
    ["AnecdoteRightArea"] = {view = "AnecdoteHintView",subview = nil, btn = "AnecdoteRightArea", func = nil}, --万灵大事件右侧指引区

    ["HundredGodReward1"] = {view = "HundredGodBoss", subView = nil, btn = "HundredGodReward1"},--百神之师第一个奖励
    ["HundredGodClose"] = {view = "HundredGodBoss", subView = nil, btn = "HundredGodBossClose"},--百神关闭

    ["DivineConsumeList"] = {view = "DivineCostumeView", subview = nil, btn = "DivineConsumeList", func = nil}, --神装列表
    ["DivineConsumeAttr"] = {view = "DivineCostumeView", subview = nil, btn = "DivineConsumeAttr", func = nil}, --神装属性
    ["DivineConsumeGongMingBtn"] = {view = "DivineCostumeView", subview = nil, btn = "DivineConsumeGongMingBtn", func = nil}, --共鸣按钮
    ["DivineConsumeGongMingAttr"] = {view = "DivineCostumeView", subview = nil, btn = "DivineConsumeGongMingAttr", func = nil}, --共鸣属性

    ["LianYao"] = {view = "MainView", subView = nil, btn = "LianYaoBtn", func = nil},   --主界面炼妖功能按钮
    ["LianYaoDesc"] = {view = "PetRefining", subView = nil, btn = "ClickFrame", func = nil}, --炼妖介绍
    ["LianYaoStart"] = {view = "PetRefining", subView = nil, btn = "BtnStart", func = nil}, --开始炼妖
    ["LianYaoZhu"] = {view = "PetRefining", subView = nil, btn = "LianYaoZhu", func = nil}, --开始炼妖
    ["LianYaoFu"] = {view = "PetRefining", subView = nil, btn = "LianYaoFu", func = nil}, --开始炼妖
    ["LianYaoPreview"] = {view = "PetRefining", subView = nil, btn = "PreviewBtn", func = nil}, --介绍预览按钮

    ["ZaoHua"] = {view = "MainView", subView = nil, btn = "ZaoHuaBtn", func = nil},   --主界面造化装备按钮
    ["MarbasEquipSelect"] = {view = "MarbasView", subView = nil, btn = "MarbasEquipSelect", func = nil},   --造化基础选择装备孔
    ["MarbasBagSelect"] = {view = "MarbasBagView", subView = nil, btn = "MarbasBagSelect", func = nil},   --造化背包选择装备
    ["MarbasEquipEquip"] = {view = "ItemInfoView", subView = nil, btn = "MarbasEquipEquip", func = nil},   --造化装备物品信息穿着

    ["BatchBuyBuyBtn"] = {view = "BatchBuyView", subView = nil, btn = "BtnBuyObj", func = nil},     --商会购买按钮
 
    ["MiniMapWanCunShengWang"] = {view = "MapMiniView", subView = nil, btn = "MiniMapWanCunShengWang", func = nil},     --小地图宛村声望NPC
    ["LineQuickBuySure"] = {view = "LiveQuickBuy",subView = nil,btn = "BtnSure",func = nil},--选择找回按钮

    -- BagEquipUse 使用
    ["ShengQiMain"] = {view = "MainView", subView = nil, btn = "ShengQiMain", func = nil},--主界面圣器按钮
    ["ShengQiBag"] = {view = "ShengQiView", subView = nil, btn = "ShengQiBag", func = nil},   --圣器打开背包
    ["ShengQiBagItem"] = {view = "ShengQiBagView", subView = nil, btn = "ShengQiBagItem", func = nil},   --选中圣器第一个
    ["ShengQiListBagItem"] = {view = "ShengQiBagView", subView = nil, btn = "ShengQiListBagItem", func = nil},   --新列表背包选中圣器第一个
    ["ShengQiBagItemUse"] = {view = "ShengQiBagView", subView = nil, btn = "ShengQiBagItemUse", func = nil},   --新列表背包第一个出战
    ["ShengQiBagClose"] = {view = "ShengQiBagView", subView = "Board", btn = "BtnCloseClick", func = nil},  --背包界面关闭
    ["ShengQiAttriGuide"] = {view = "ShengQiView", subView = nil, btn = "ShengQiAttriGuide", func = nil},   --圣器信息
    ["ShengQiSkillGuide"] = {view = "ShengQiView", subView = nil, btn = "ShengQiSkillGuide", func = nil},   --圣器技能
    ["ShengQiTabAttri"] = {view = "ShengQiView", subView = nil, btn = "ShengQiTabAttri", func = nil},   --圣器属性标签页按钮
    ["ShengQiTabLevel"] = {view = "ShengQiView", subView = nil, btn = "ShengQiTabLevel", func = nil},   --圣器升级标签页按钮
    ["ShengQiTabFuShi"] = {view = "ShengQiView", subView = nil, btn = "ShengQiTabFuShi", func = nil},   --圣器符石标签页按钮
    ["ShengQiBlessPet"] = {view = "ShengQiView", subView = nil, btn = "ShengQiBlessPet", func = nil},   --圣佑入口按钮
    ["ShengQiSelectPetToggle"] = {view = "ShengQiSelectPetView", subView = nil, btn = "ShengQiSelectPetToggle", func = nil},    --圣佑第一个宠物
    ["ShengQiSelectPetOKBtn"] = {view = "ShengQiSelectPetView", subView = nil, btn = "ShengQiSelectPetOKBtn", func = nil},      --圣佑确定按钮
    ["ShengQiLevelStuff"] = {view = "ShengQiView", subView = nil, btn = "ShengQiLevelStuff", func = nil},   --选择经验材料
    ["ShengQiUpLevelBtn"] = {view = "ShengQiView", subView = nil, btn = "ShengQiUpLevelBtn", func = nil},   --圣器升级

    --侠义界面指引
    ["XiaYiMain"] = {view = "MainView", btn = "KnightMain"},--侠义主界面
    ["XiaYiHuoBi"] = {view = "KnightlyView", btn = "XiaYiHuoBi"},--侠义货币介绍
    ["XiaYiGuiZe"] = {view = "KnightlyView", btn = "XiaYiGuiZe"},--侠义规则介绍
    ["XiaYiJiangLi"] = {view = "KnightlyView", btn = "XiaYiJiangLi"},--侠义奖励介绍
    ["XiaYiShangDian"] = {view = "KnightlyView", btn = "XiaYiShangDian"},--侠义商店按钮
    ["ShangChengGuangBi"] = {view = "StoreView", subView = "Board", btn = "BtnClose"},--商城关闭按钮
    ["XiaYiGuiZeBtn"] = {view = "KnightlyView", btn = "XiaYiGuiZeBtn"},--侠义规则小按钮
    ["XiaYiGuiZeCloseBtn"] = {view = "TeamKnightlyView", btn = "XiaYiGuiZeCloseBtn"},--侠义规则小按钮
}

--常量指引ID
ConstGuideId = {
    DailyActResearchTask = 134, --从活跃界面打开三界除魔时的指引
    DailyActAllSoulsBook = 135, --从活跃界面打开万灵话本时的指引 
    LianYaoFirstOpen = 140,
    XiaYiFirstOpen = 148,
    TaskBoxGuide = 150,--#59775 
    BattleModeOpGuide = 3000,
    GaiMing = 168,
}

function GuideClickBtn:Start(step_cfg, callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    self.listener = nil
    --[[ TimeHelper:AddDelayTimer(function ()
    end,0.5) ]]
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    if self.view and self:Check() then
        self.view:BtnClickReady()
    end
    self:CheckAndCloseNpcTalk()
    self:CheckButton()
    if self.auto_click_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_click_handle)
        self.auto_click_handle = nil
    end
    if type(self.step_cfg.guide_auto) == "number" and self.step_cfg.guide_auto > 0 then
        self.auto_click_handle = TimeHelper:AddDelayTimer(function ()
            self:OnClick()
        end, self.step_cfg.guide_auto)
    end
    if not StringIsEmpty(self.step_cfg.step_param_2) then
        if self.step_cfg.step_param_2 == "StartRollingTask" then
            GuideData.Instance:StartRollingTask()
        end
    end
end
function GuideClickBtn:CheckAndCloseNpcTalk()
    if self.step_cfg.step_id == 15 or self.step_cfg.step_id == 16 or self.step_cfg.step_id == 30 then
        return
    end
    TimeHelper:CancelTimer(self.handle_runtime)
    self.handle_runtime = TimeHelper:AddRunTimer(function ()
        if ViewMgr:IsOpen(NpcDialogView) and GuideManager.Instance:IsGuide() then
            ViewMgr:CloseView(NpcDialogView)
            TimeHelper:CancelTimer(self.handle_runtime)
        end
    end,1,5)
end
--不连贯指引
function GuideClickBtn:Check()
    if self.step_cfg.step_id == 15 and self.step_cfg.index == 4 then
        return false
    end
    if self.step_cfg.step_id == 34 and self.step_cfg.index == 4 then
        return false
    end
    return true
end
function GuideClickBtn:CheckButton()
    local ui, clickFunc, config
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(
        function()
            ui, config = self:GetGuideUi(self.step_cfg.step_param_1)
            if self.view == nil then
                return false
            end
            if ui ~= nil and ui.gameObject ~= nil then
                self.view:AddEvent(BindTool.Bind(self.OnClick, self))
                if config.func == nil then
                    if ui:GetType().BaseType == typeof(Game.LuaUIEvent) then
                        self.listener = function()
                            ui:Trigger()
                        end
                    elseif not ui.gameObject:Equals(nil) and ui.gameObject:GetComponent(typeof(Game.LuaUIEvent)) then
                        self.listener = function()
                            if not ui.gameObject:Equals(nil) and ui.gameObject:GetComponent(typeof(Game.LuaUIEvent))  then
                                ui.gameObject:GetComponent(typeof(Game.LuaUIEvent)):Trigger()
                            else
                                BuglyReport("guide ui is nil or component is nil step_id = ", self.step_cfg.step_id, " index = ", self.step_cfg.index)
                            end
                        end
                    end
                else
                    self.listener = config.func
                end
                self:ClickRange(ui.gameObject)
                return true
            end
            return false
        end
    )
end
function GuideClickBtn:GetGuideUi(key)
    local btn
    local clickFunc
    local config = GuideClickBtn.btn_config[key]
    if config then
        local view = MonoTypeOf(config.view)
        if ViewMgr:IsOpen(view) then
            if config.subView then--这里也是可以多层的
                if type(config.subView) == "string" then
                    if ViewMgr:getViewInfo(view).view_mono[config.subView]:IsLoaded() then
                        btn = ViewMgr:getViewInfo(view).view_mono[config.subView][config.btn]
                    end
                else
                    btn = ViewMgr:getViewInfo(view).view_mono
                    for index, value in ipairs(config.subView) do
                        btn = btn[value]
                    end
                    btn = btn[config.btn]
                end
            else
                btn = ViewMgr:getViewInfo(view).view_mono[config.btn]
            end
            if btn ~= nil then
                return btn, config
            else
                --如果没找到就通过注册寻找
                btn, clickFunc = self.mgr:GetGuideUi(config.btn)
                config.func = clickFunc
                return btn, config
            end
        --[[ else
            print("界面未打开"..config.view) ]]
        end
    end
    return nil, nil
end
function GuideClickBtn:OnClick()
    -- LogError("GuideClickBtn OnClick",self.step_cfg)
    GuideData.Instance:StopRollingTask()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.auto_click_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_click_handle)
        self.auto_click_handle = nil
    end
    TimeHelper:CancelTimer(self.handle_runtime)
    self.view:Hide()
    if self.listener then
        self.listener()
    end
    self.listener = nil
    if self.callback then
        self.callback()
    end
    --触发下一步
end

function GuideClickBtn:ClickRange(ui)
    if not GuideManager.Instance:IsGuide() then return end 
    self.view:Show(ui, 1)
    if self.step_cfg.arrow_tip ~= "" then
        self.view:ShowDesc(self.step_cfg)
    end
end

function GuideClickBtn:Finish()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if self.auto_click_handle ~= nil then
        TimeHelper:CancelTimer(self.auto_click_handle)
        self.auto_click_handle = nil
    end
    TimeHelper:CancelTimer(self.handle_runtime)
    -- self.callback = nil
    self.step_cfg = nil
end
--开始一段手指漂移
function GuideClickBtn:StartFingerAction(p1, P2)
   self.view:StartFingerAction(p1, P2)
end