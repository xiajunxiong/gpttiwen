FILE_REQ("modules/pet/pet_data")
FILE_REQ("modules/pet/pet_inborn/pet_inborn_ctrl")
FILE_REQ("modules/pet/pet_tale/pet_tale_ctrl")
FILE_REQ("modules/pet/pet_resonance_data")
VIEW_REQ("modules/pet/pet_view")
VIEW_REQ("modules/pet/pet_name")
VIEW_REQ("modules/pet/pet_gain")
VIEW_REQ("modules/pet/pet_egg")
VIEW_REQ("modules/pet/pet_freelife_tip")
VIEW_REQ("modules/pet/pet_quickbuy")
VIEW_REQ("modules/pet/pet_restraint_view")
VIEW_REQ("modules/pet/pet_select")
VIEW_REQ("modules/pet/pet_convert")
VIEW_REQ("modules/pet/pet_storage")
VIEW_REQ("modules/pet/pet_equip_select")
VIEW_REQ("modules/pet/pet_breach_view")
VIEW_REQ("modules/pet/pet_str_select_copet")
VIEW_REQ("modules/pet/pet_special_get_way")
VIEW_REQ("modules/pet/pet_book_change_view")
VIEW_REQ("modules/pet/pet_pray_info")
VIEW_REQ("modules/pet/pet_skill_lvup_tip")
VIEW_REQ("modules/pet/pet_multi_pieces")
VIEW_REQ("modules/pet/pet_stuff_buy_dialog")
VIEW_REQ("modules/pet/pet_resonance_view")
VIEW_REQ("modules/pet/pet_sp_show")
VIEW_REQ("modules/pet/pet_gai_ming")
VIEW_REQ("modules/pet/pet_lian_yao_mutant_view")

PetCtrl = PetCtrl or BaseClass(BaseCtrl)
--宠物操作
PET_OP_TYPE = {
    AbandonmentPet = 1, --遗弃宠物	遗弃index为p1的宠物
    ConversionExp = 2, --转换经验	将index为p1的宠物等级降至p2级
    Rebirth = 3, --返生		将index为p1的宠物返生
    Lock = 4, --上锁		给index为p1的宠物上锁或解锁
    GetSkill = 5, --学习技能	给index为p1的宠物的第p2个技能格使用技能id为p3的技能书
    RefreshGift = 6, --刷新天赋	给index为p1的宠物的第p2个天赋格刷新一个新天赋
    SaveGift = 7, --保存天赋	将index为p1的宠物的第p2个天赋格的新天赋覆盖到旧的上
    Breakthroughs = 8, --强化		强化index为p1的宠物 消耗index为p2~4的宠物,p5是否炼妖
    TransferBreakthroughs = 9, --强化转移	将index为p2的宠物的强化等级转换到index为p1的宠物上
    SetPetState = 10, --设置宠物状态 将index为p1的宠物设置成p2 [1:出战 2：跟随 3：待机1 4：待机2 5:待机3 6:休息 7:取消跟随]等状态
    PetAttrReq = 11, --请求宠物属性 请求index为p1
    AddExp = 12, --使用经验药水 给宠物index为p1的宠物使用item_id 为p2的道具 p3个
    RefreshPoints = 13, --洗点 index
    RemoveSkill = 14, --遗忘技能 把宠物index为p1的宠物的skill_index为p2的技能遗忘
    Refined = 15, --洗炼 p1 宠物index P2 是否技能也一起洗炼
    ComprehendGift = 16, --领悟 p1 宠物index
    Compose = 17, --合成 p1 合成配置里的seq
    RefinedSave = 18, --洗炼保存
    Convert = 19,   --神兽兑换 p1 seq
    RefinedGiveUp = 20, --洗炼放弃
    --神兽兑换 p1 合成配置里的seq 默认传45
    MoveStorage = 21,--移动到仓库 p1 背包index p2索引
    MovePetGrid = 22,--移动到背包 p1 背包index p2索引
    ExpandBag = 23,--拓展背包
    PetEquipmentPutOn = 24,     --宠物穿戴装备 p1 宠物index  p2 背包格子（装备的index）
    PetEquipmentPutOff = 25,    --宠物卸下装备 p1 宠物index 
    PetEquipmentChange = 26,    --宠物装备更替 p1获得装备的宠物index p2 失去装备的宠物index
    UsePetSkin = 27,            --宠物皮肤 p1 宠物index p2 皮肤id
    UpLevelBreak = 28,          --宠物升级突破，p1宠物index
    UpLevelBreakAttr = 29,      --请求宠物突破属性 p1 宠物index p2 宠物等级 
    GuideReplacePassiveSkill = 30, --指引打书 p1 宠物index
    GuideComprehendPassiveSkill = 31, --指引领悟 p1 宠物index 
    GuideRefined = 32,                  --指引洗练， p1 宠物index p2 是否洗炼技能
    ReplacePassiveSkillSave = 33,       --保存打书技能 p1 宠物index p2 0保存 1放弃
    OnPetWeapon = 34,           --穿戴魂器  p1宠物下标，p2背包index, p3 魂器格子index 0/1
    UnPetWeapon = 35,           --脱下魂器  p1宠物下标, p2 魂器格子index 0/1
    BreachWeapon = 36,          --魂器突破 p1 是否背包, p2 (背包下标/宠物index) p3 魂器格子index 0/1
    RefineWeapon = 37,          --魂器精炼 p1 是否背包, p2 (背包下标/宠物index) p3 p4 魂器格子index 0/1
    LockWeapon = 38,            --魂器锁定 p1 是否背包, p2 (背包下标/宠物index) p3：是否锁定 p4 魂器格子index 0/1

    -- 神印
    PetStampUse = 39,           --穿戴神印 p1宠物index  p2 神印背包index [1 -100]
    PetStampUnfix = 40,         --脱下神印 p1宠物index  p2 神印位置
    PetStampIntensify = 41,     --升级神印 p1神印背包下标 p2 经验值
    PetStampReslove = 42,       --分解神印 p1神印背包下标
    PetStampPray = 43,          --!< 祈神 p1 货币类型（0铜币，1元宝） p2 消耗总数, p2是否自动分解材料
    PetPetStampLogList = 44,    --!< 祈神日志列表
    PetPetStampLogDetail = 45,  --!< 祈神日志详细 p1 日志下标index
    PetPetStampExchange = 46,   --!< 祈神兑换 p1 seq
    PetStampLock = 47,          --!< 神印锁定 p1背包下标， p2是否锁定[1/0]
    PetStampExpStuffIntensify = 48, -- 神印强化，使用神印经验材料,  p1神印背包下标 p2 背包类型，p3 背包下标, p4 消耗数量
    PetStampUseBlueprint = 49,  -- 直接替换方案里面的神印， p1宠物index, p2方案index
    PetStampExpStuffReslove = 50,-- 分解材料经验， p1背包类型，p2背包下标，p3消耗数量

    PetAutoUpLevel = 51,        -- 宠物一键升级

    PetWeaponConvert = 52,      -- 魂器兑换 p1:兑换seq
    PetWeaponChange = 53,       -- 魂器转化 p1 是否背包, p2 (背包下标/宠物index), p3 转化道具id p4 魂器格子index 0/1

    PetMutant = 54,             -- 宠物使用物品变异 p1:pet_index
    PetBookChange = 55,         -- 超级技能书置换 p1 item_id, p2 item_id

    PetPray = 56,               -- 宠物祝福 p1:mian_index(主宠), p2:pet_index(副宠)
    PetPrayCancel = 57,         -- 取消宠物祝福 p1:pet_index(副宠)-
    StartPetFeiShengTask = 58,  -- 开始宠物飞升任务，p1:pet_index
    UpLevelPassiveSkill = 59,   -- 宠物被动技能升级，p1:pet_index, p2:被动技能index p3是否炼妖
    StartPetFeiSheng = 60,      -- 选择飞升技能 p1:pet_index, p2:选择的技能index
    FeiShengSkillChange = 61,   -- 切换飞升技能 p1:pet_index, p2:选择的技能index
    TaleReward = 62,            -- 领取传记奖励 p1:mutant_id, p2:第几次飞升奖励
    FSSkillUpLv = 63, --飞升技能升级
    PetStampChange = 64,        -- 神印置换 p1:index, p2:index

    PetBagdePutOn = 65,         -- 徽章穿戴 p1:pet_index p2:col_type p3:bag_index
    PetBagdeTakeOff = 66,       -- 徽章脱下 p1:pet_index
    PetBagdeExChange = 67,      -- 徽章替换 p1:pet_index p2:pet_index
    PetBagdeCompose = 68,       -- 徽章合成 p1:item_id

    PetLianYao = 69,            -- 炼妖 p1:主宠pet_index p2:副宠pet_index p3:炉子id
    ComprehendYao = 70,         -- 炼妖技能领悟 p1: pet_index

    PetUnlockInborn = 71,       -- 解锁宠物天赋 p1:pet_index p2:天赋类型 p3:天赋位置
    PetInbornSkillLevelUp = 72, -- 宠物天赋技能升级 p1:pet_index p2:天赋类型
    PetInbornSkillReplace = 73, -- 宠物天赋技能替换 p1:pet_index p2:天赋类型 p3:天赋位置
    PetInbornSkillReset = 74,   -- 宠物天赋重置 p1:pet_index

    PetResonanceSet = 75,       -- 强化共鸣设置 p1:格子类型[0 - 3]  p2格子index[0 - 8] p3 pet_index
    PetUnResonance = 76,        -- 强化共鸣卸下 p1:格子类型[0 - 3]  p2格子index[0 - 8]
    PetGodPrintDevour = 77,     -- 神印吞噬     p1:吞噬神印index    p2:被吞噬神印index  p3:吞噬词条index
    PetStampDrop = 78,          -- 神印丢弃 p1:背包index（该神印必须非绑，非穿戴中）

    PetGaiMingUpLevel = 79,     -- 改命 p1 index P2 次数
    PetGaiMingAttrUpLevel = 80, -- 改命加属性 p1 index p2 属性index
    PetGaiMingAttrReset = 81,   -- 重置 p1 index
    PetMutantLianYao = 82,      -- 副宠变异 p1:pet_index
    PetAmuletInstensify = 83,   -- 护符强化 p1:index p2:column p3:stuff_index p4:是否是穿戴的（0：没穿戴） (p1根据p4转换为pet_index or bag_index)
}
PET_STATE_OP = {
    FIGHT = 1,
    FOLLOW = 2,
    WAIT1 = 3,
    WAIT2 = 4,
    --舍弃不用
    WAIT3 = 5,
    --舍弃不用
    RELAX = 6,
    NOFOLLOW = 7
}
TRADE_PET_INDEX_ORDER = {
    STR_LEVEL_ORDER_UP = 0, --强化等级
    STR_LEVEL_ORDER_DOWN = 1, --强化等级
    PRICE_UP = 2, --价格不同
    PRICE_DOWN = 3 --价格
}
--- 宠物信息下发枚举
PetNotieType = {
    PET_NOTIE_OLD_TYPE = -1, --已过时标签
    PET_NOTIE_NO_TYPE = 0, -- 无标记下发
    PET_NOTIE_TYPE_EXP = 1, -- 经验值增加下发
    PET_NOTIE_TYPE_REBIRTH = 2, -- 宠物返生(经验还原到1级)
    PET_NOTIE_TYPE_RECOVER = 3, -- 状态复原（满血满蓝
    PET_NOTIE_TYPE_HPMP = 4, -- 回血回蓝
    PET_NOTIE_TYPE_ASSIGN_POINTS = 5, -- 属性点
    PET_NOTIE_TYPE_SKILL = 6, -- 技能变更
    PET_NOTIE_TYPE_GIFT = 7, -- 天赋变更
    PET_NOTIE_TYPE_RENAME = 8, -- 重命名
    PET_NOTIE_TYPE_LOCK = 9, -- 锁定
    PET_NOTIE_TYPE_STATUS = 10, -- 宠物状态变更
    PET_NOTIE_TYPE_BREAK_THROUGHS = 11, -- 宠物强化
    PET_NOTIE_TYPE_FABAO = 12, -- 法宝
    PET_NOTIE_TYPE_REFINED = 13, --洗炼下发
    PET_NOTIE_TYPE_COMPREHEND_GIFT = 14, --领悟技能
    PET_NOTIE_TYPE_REPLACE_GIFT = 15, --打书
    PET_NOTIE_TYPE_REFINED_SAVE = 16, --洗炼保存下发
    PET_NOTIE_TYPE_BATTLE_ADD_EXP = 17, --战斗结束增加经验
    PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE = 18, --宠物装备穿脱变更
    PET_NOTIE_TYPE_PET_USE_SKIN = 19,           --宠物穿戴皮肤
    PET_NOTIE_TYPE_PET_BREACH = 20,             --宠物突破
    PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS = 21,     --自动分配属性点
    PET_NOTIE_TYPE_DOMINANCE_CHANGE = 22,       --灵骑统御变更
    PET_NOTIE_TYPE_SAVE_HIT_BOOK = 23,          --保存打书
    PET_NOTIE_TYPE_ECHO = 24,                   --共鸣变化
    PET_NOTIE_TYPE_SET_ADD_POINT_INFO = 25,     --设置自动加点信息
    PET_NOTIE_TYPE_WEAPON_CHANGE = 26,          --魂器穿脱变更
    PET_NOTIE_TYPE_WEAPON_LOCK= 27,             --魂器锁定
    PET_NOTIE_TYPE_WEAPON_DATA = 28,            --魂器数值变化
    PET_NOTIE_TYPE_STAMP_DATA = 29,             --神印数值变化
    PET_NOTIE_TYPE_AUTO_UP_LEVEL = 30,          --宠物一键升级
    PET_NOTIE_TYPE_WEAPON_EXCHANGE = 32,        --宠物魂器转换
    PET_NOTIE_TYPE_PRAY_CHANGE = 33,            --祝福变更
    PET_NOTIE_TYPE_FEISHENG = 34,               --宠物飞升
    PET_NOTIE_TYPE_LIAN_YAO = 40,               --宠物炼妖
    PET_NOTIE_TYPE_UNLOCK_INBORN = 42,          --解锁宠物天赋
    PET_NOTIE_TYPE_INBOTN_SKILL_UP = 43,        --宠物天赋技能升级
    PET_NOTIE_TYPE_INBORN_REPLACE = 44,         --宠物天赋技能替换
    PET_NOTIE_TYPE_INBORN_RESET = 45,           --宠物天赋重置
    PET_NOTIE_TYPE_SHENG_QI = 46,               --圣器圣佑宠物
    PET_NOTIE_TYPE_STRENGTH_BUFF = 47,                     --宠物共鸣
    PET_NOTIE_TYPE_SHENG_HEN_INFO_CHANGE = 48,  --宠物圣痕信息变化
    PET_NOTIE_TYPE_GAI_MING = 49,               --宠物改命
    PET_NOTIE_TYPE_GAI_MING_ATTR = 50,          --宠物改命属性
    PET_NOTIE_TYPE_GAI_MING_ATTR_RESET = 51,    --宠物改命属性重置
    PET_MUTANT_LIAN_YAO_MUTANT = 52,                   --炼妖副宠变异
    PET_NOTIE_AMULET_INSTENSIFY = 53,           --宠物护符强化
    PET_NOTIE_YOUMING_BLESS = 54,               --幽冥附身
    PET_NOTIE_TYPE_TRAIN_SKILL = 99, --宠物修炼技能升级（从所有宠物变化的属性中选一个播放）c纯客户端逻辑
    PET_NOTIE_TYPE_GUILD_INBORN = 100, --家族灵脉学习
    PET_NOTIE_TYPE_STAR_MAP = 101,
}
PetAllInfoReason = {
    DEFAULT = 0,    --登录
    TRAIN_SKILL = 1, --宠物修炼技能
    GUILD_INBORN = 2,--家族灵脉技能
    XIU_WEI = 3,    --修为
    STAR_MAP = 4,   --星图
}

function PetCtrl:__init()
    if PetCtrl.Instance ~= nil then
        Debuger.LogError("[PetCtrl] attempt to create singleton twice!")
        return
    end
    PetCtrl.Instance = self
    self.data = PetData.New()
    PetInbornCtrl.New()
    PetTaleCtrl.New()
    PetResonanceData.New()
    self:RegisterProtocol(SCSendPetAllInfo, "RecvPetAllInfo", true)
    self:RegisterProtocol(SCSendPetSingleInfo, "RecvPetSingleInfo", true)
    self:RegisterProtocol(SCSendPetOtherInfo, "RecvPetOtherInfo", true)
    self:RegisterProtocol(SCSendPetDeleteInfo, "RecvPetDeleteInfo", true)
    self:RegisterProtocol(SCSendPetAttrListInfo, "RecvPetAttrListInfo", true)
    self:RegisterProtocol(SCSendPetBreakthrougtsRet, "RecvPetBreakthrougtsRet", true)
    self:RegisterProtocol(SCSendPetGetRet, "RecvSendPetGetRet")
    self:RegisterProtocol(SCTradeMarketSearchPetForQuickBuy, "RecvTradeMarketSearchPetForQuickBuy")
    self:RegisterProtocol(SCSealPetGetItemInfo, "OnSealPetGetItemInfo")
    self:RegisterProtocol(SCPetIdRecordList, "OnSCPetIdRecordList")
    self:RegisterProtocol(SCMovePetGridNotice, "OnMovePetGridNotice")
    self:RegisterProtocol(SCPetSkinInfo, "OnSCPetSkinInfo")
    self:RegisterProtocol(SCPetBreachAttri,"OnSCPetBreachAttri")
    self:RegisterProtocol(SCPetPrayListInfo,"OnSCPetPrayListInfo")
    self:RegisterProtocol(SCPetPrayInfo,"OnSCPetPrayInfo")
    self:RegisterProtocol(SCPetComposeBatchRet,"OnSCPetComposeBatchRet")
    self:RegisterProtocol(SCPetResonanceInfo,"OnSCPetResonanceInfo")
    self:RegisterProtocol(SCPetTopRankInfo, "OnSCPetTopRankInfo")
    self:RegisterProtocol(SCPetAddExpNotice, "OnSCPetAddExpNotice")

    self:RegisterProtocol(CSPetOp)
    self:RegisterProtocol(CSPetRename)
    self:RegisterProtocol(CSPetAssignAttributePoints)
    self:RegisterProtocol(CSTradeMarketSearchPetForQuickBuy)
    self:RegisterProtocol(CSAbandonmentPetByList)
    self:RegisterProtocol(CSPetComposeBatch)
    self:RegisterProtocol(CSPetComposeBatchAndDec)
    
    

    self.delay_pet_list = {}
    
    self.is_str_yao = false     -- 是否强化了炼妖
end
local npcSeq = 65001
local npcId = 65041
function PetCtrl:OnInit()
    --宠物潜能点
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        self.data:GetPetList(),
        BindTool.Bind(self.FlushPointRemind, self)  
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        self.data:GetPetList(),
        BindTool.Bind(self.FlushGaiMingRemind, self)  
    )

    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Pet.PetAttr,
    --     self.data:GetPetList(),
    --     BindTool.Bind(self.FlushGiftRemind, self)
    -- )

    --宠物升级
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        self.data:GetPetList(),
        BindTool.Bind(self.FlushUpgradeRemind, self) 
    )

	--属性丹
	RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        AttributeDanData.Instance.attribute_red_piont_pet,
        BindTool.Bind(self.FlushAttributeRemind, self)
    )

    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetStrChange,
        self.data:GetPetList(),
        function ()
            local num = self.data:AllStrengthenRemind()
            return num
        end
    )

    -- 魂器的
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        PetWeaponData.Instance.remind_data,
        function ()
            local num = PetWeaponData.Instance:GetRemind()
            return num or 0
        end
    )
    BagData.Instance.item_grid[ItemColumnType.PetWeapon]:Care(function ()
        PetWeaponData.Instance.remind_data:Notify()
    end)
    BagData.Instance:ListenItems(PetWeaponData.Instance:GetStuffItemList(),function ()
        PetWeaponData.Instance.remind_data:Notify()
    end)
	
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetGetBook,
        self.data.pet_lingwu_remind,
        BindTool.Bind(self.FlushLingWuRemind, self)
    )
    --主界面提升 宠物加点
    --[[ MainViewData.Instance:RegisterUpgradeRemind(
        Language.Main.UpgradeRemind.PetAttrImprove,
        self.data:GetPetList(),
        function()
            return PetData.Instance:CheckFightPointRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or
                MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
        end
    ) ]]
    --主界面提升 宠物升级
    --[[ MainViewData.Instance:RegisterUpgradeRemind(
        Language.Main.UpgradeRemind.PetUpgrade,
        self.data:GetPetList(),
        function()
            return PetData.Instance:CheckFightUpgradePointRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or
                MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
        end
    ) ]]
    --主界面提升 宠物突破
    -- self.pet_breach_remind_handle = BagData.Instance:ListenItems({27577},BindTool.Bind(self.data.TriggerBreachRemind,self.data))
    -- self.data.pet_list:Care(BindTool.Bind(self.data.TriggerBreachRemind,self.data))
    --[[ MainViewData.Instance:RegisterUpgradeRemind(
        Language.Main.UpgradeRemind.PetBreach,
        self.data.pet_breach_remind_data,
        function()
            local remind_num,remind_pet = PetData.Instance:CheckBreachRemind(true)
            if remind_num > 0 then
                return remind_num > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
            else
                remind_num,remind_pet = PetData.Instance:CheckBreachRemind(false)
                return remind_num > 0 and MainViewUpgradeRemindType.WithoutRedPoint or MainViewUpgradeRemindType.Remove
            end
        end,
        function()
            local remind_num,remind_pet = PetData.Instance:CheckBreachRemind(true)
            if remind_num > 0 and remind_pet ~= nil then
                self.data:SetSelect(remind_pet.index)
            end
            ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
        end
    ) ]]
    --主界面提升 宠物护符
    -- BagData.Instance.item_grid:Care(BindTool.Bind(self.ResetEquipFlag, self))
    -- self.data.pet_list:Care(BindTool.Bind(self.ResetEquipFlag, self))
   --[[  MainViewData.Instance:RegisterUpgradeRemind(
        Language.Main.UpgradeRemind.PetEquipment,
        self.data.pet_equipment_remind,
        function()
            return PetData.Instance:CheckPetEquipmentRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or
                MainViewUpgradeRemindType.Remove
        end,
        function()
            ViewMgr:OpenViewByKey(Mod.Pet.PetGift)
        end
    ) ]]
    --主界面提升 宠物领悟技能
    BagData.Instance.item_grid:Care(BindTool.Bind(self.ResetPetLingWu, self))
    self.data.pet_list:Care(BindTool.Bind(self.ResetPetLingWu, self))
   --[[  MainViewData.Instance:RegisterUpgradeRemind(
        Language.Main.UpgradeRemind.PetLingWu,
        self.data.pet_lingwu_remind,
        function ()
            return PetData.Instance:CheckPetLingWuRemind() > 0 and MainViewUpgradeRemindType.WithRedPoint or MainViewUpgradeRemindType.Remove
        end,
        function ()
            ViewMgr:OpenViewByKey(Mod.Pet.PetGetBook)
        end
        ) ]]
    -- BehaviorData.Instance:CustomNpcTalkBtnsId(npcId, BindTool.Bind(self.SetupBehaviourBtn, self), true)
end

function PetCtrl:ResetPetLingWu()
    self.data.pet_lingwu_remind.flag = not self.data.pet_lingwu_remind.flag
end

function PetCtrl:ResetEquipFlag()
    self.data.pet_equipment_remind.flag = not self.data.pet_equipment_remind.flag
end

function PetCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.id == npcId then
        table.insert(
            btn_list,
            NpcDialogBehBtnData.FuncCreate(
                Language.Task.TaskBtn.ConvertPet,
                function()
                    -- --判断
                    -- local info = PetData.PetComposeInfoSeq(45)
                    -- local param_t = {
                    --     content = string.format(Language.Pet.ConvertTip, info.patch_num, Item.GetName(info.patch_id)),
                    --     confirm = {
                    --         func = function()
                    --             PublicPopupCtrl.Instance:CloseDialogTips()
                    --             local num = Item.GetNum(info.patch_id)
                    --             if num < info.patch_num then
                    --                 local item_info = {}
                    --                 item_info.item_id = info.patch_id
                    --                 item_info.content = Language.Pet.ConvertTip2
                    --                 item_info.num = info.patch_num - num
                    --                 item_info.fly_icon = false
                    --                 PublicPopupCtrl.Instance:CenterI(item_info)
                    --             else
                    --                 PetCtrl.SendPetOp(PET_OP_TYPE.Convert, 45)
                    --             end
                    --         end
                    --     }
                    -- }
                    -- PublicPopupCtrl.Instance:DialogTips(param_t)
                    ViewMgr:OpenView(PetPiecesShenshou)
                end
            )
        )
    end
    -- body
end
function PetCtrl:OnUnloadGameLogic()
    self.data:Clear()
    PetInbornCtrl.Instance:OnUnloadGameLogic()
    PetResonanceData.Instance:ClearData()
end
function PetCtrl:FlushPointRemind()
    local num = self.data:CheckPetPointRemind()
    return num
end
function PetCtrl:FlushGaiMingRemind()
    local num = self.data:CheckGaiMingPointRemind()
    return num
end
--天赋解锁红点（已废弃）
-- function PetCtrl:FlushGiftRemind()
--     local num = self.data:CheckPetGiftRemind()
--     return num
-- end

--宠物升级红点
function PetCtrl:FlushUpgradeRemind()
    local num = self.data:CheckAllUpgradePointRemind()
    return num
end

--属性丹
function PetCtrl:FlushAttributeRemind()
    local num = self.data:CheckAttributePointRemind()
    return num
end

function PetCtrl:FlushEquipmentRemind()
    local num = self.data:CheckPetEquipmentRemind()
    return num
end
function PetCtrl:FlushLingWuRemind()
    local num = self.data:CheckPetLingWuRemind() or 0
    if num == 0 then
        num = self.data:CheckPetEquipRemind() or 0
    end
    if num == 0 then
        num = self.data:PetBagdeAllRemind() or 0
    end
    return num
end
function PetCtrl:__delete()
    self:UnRegisterProtocol(SCSendPetAllInfo)
    self:UnRegisterProtocol(SCSendPetSingleInfo)
    self:UnRegisterProtocol(SCSendPetOtherInfo)
    self:UnRegisterProtocol(SCSendPetDeleteInfo)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Pet.PetAttr)
    -- BagData.Instance:RemoveListenRecvItem(self.pet_breach_remind_handle)
end

function PetCtrl:RecvPetAllInfo(protocol)
    if protocol.reason == PetAllInfoReason.GUILD_INBORN or protocol.reason == PetAllInfoReason.STAR_MAP then
        self.data:SetShowAttrIndex()
        local first_index = self.data.trian_skill_attr_change_index
        if first_index ~= -1 and protocol.pet_list[first_index] ~= nil then
            local pet = PetData.Instance:GetPetInfo(first_index)
            if pet ~= nil then
                local old_score = pet:GetScore()
                local new_score = protocol.pet_list[first_index]:GetScore()
                if old_score ~= new_score then
                    self.data:SetScoreChange(old_score, new_score)
                    local gain_time = GuildData.Instance:GetDelayTime(2) + 0.02
                    TimeHelper:AddDelayTimer(function ()
                        ViewMgr:OpenView(PetScore)
                    end, gain_time)
                end
            end
        end
    end
    self.data:SetPetList(protocol.pet_list)
    self.data:SetPetStorageList(protocol.storage_list)
    if protocol.reason == PetAllInfoReason.TRAIN_SKILL then
        self.data:SetShowAttrIndex()
        self.data:SetPetInfoNotie(PetNotieType.PET_NOTIE_TYPE_TRAIN_SKILL)
        for k,v in pairs(protocol.pet_list) do
            self.SendPetOp(PET_OP_TYPE.PetAttrReq, v.info.index)
        end
        -- self.SendPetOp(PET_OP_TYPE.PetAttrReq, self.data.trian_skill_attr_change_index)
    elseif protocol.reason == PetAllInfoReason.GUILD_INBORN then
        self.data:SetPetInfoNotie(PetNotieType.PET_NOTIE_TYPE_GUILD_INBORN)
        for k,v in pairs(protocol.pet_list) do
            self.SendPetOp(PET_OP_TYPE.PetAttrReq, v.info.index)
        end
    elseif protocol.reason == PetAllInfoReason.DEFAULT then
        for k,v in pairs(protocol.pet_list) do
            self.SendPetOp(PET_OP_TYPE.PetAttrReq, v.info.index)
        end
    elseif protocol.reason == PetAllInfoReason.STAR_MAP then
        self.data:SetPetInfoNotie(PetNotieType.PET_NOTIE_TYPE_STAR_MAP)
        for k,v in pairs(protocol.pet_list) do
            self.SendPetOp(PET_OP_TYPE.PetAttrReq, v.info.index)
        end
    end
    self.data:TriggerBreachRemind()
end

function PetCtrl:RecvPetSingleInfo(protocol)
    -- if protocol.pet_info:Index() == 31 then
    --     return
    -- end
    --LogError("RecvPetSingleInfo", protocol.pet_info:Index(),protocol.pet_info:FeiShengTimes(),protocol.pet_info:FeiShengJieDuan(),protocol.notie_type)
    local audioName = nil
    local need_flush_attr = true
    local delay_set = false
    local need_show_score = false
    local old_pet = self.data:GetPetInfo(protocol.pet_info.index)
    if old_pet ~= nil then
        if old_pet.info.capaility ~= protocol.pet_info.info.capaility then
            self.data:SetScoreChange(old_pet.info.capaility, protocol.pet_info.info.capaility)
            need_show_score = true
        end
        PetInbornCtrl.Instance:HandelPetChange(protocol.pet_info, old_pet,protocol.notie_type)
        self.data:SetBicycle(protocol.pet_info,old_pet:Bicycle())
    end
    local function showPetScore()
        ViewMgr:OpenView(PetScore)
    end
    if protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_BATTLE_ADD_EXP
        or protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_EXP or
            protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_AUTO_UP_LEVEL then       --战斗结束增加经验、吃经验药、一键升级经验增加
        local old = self.data:GetPetInfo(protocol.pet_info.index)
        local num = protocol.add_exp
        if num ~= 0 and type(num) == "number" then
            local name = string.gsub(protocol.pet_info:Name(), "%%" , "@")
            PublicPopupCtrl.Instance:CenterV {
                item_id = CommonItemId.Exp,
                num = num,
                content = string.format(Language.PublicPopup.PetExpTip, name, "%s", "%s")
            }
            local item = Item.Init(CommonItemId.Exp, num)
            ChatCtrl.Instance:ChannelChatCustom {pet_exp = {item = item, num = num, name = protocol.pet_info:Name()}}
        end
        if old then
            if old.level == protocol.pet_info.level then        --经验变化但没升级不刷新属性
                need_flush_attr = false
            else
                --升级检查自动加点
                local auto_info = protocol.pet_info:AutoAddInfo()
                if auto_info.index ~= 0 then
                    local add_point = self.data:GetAutoAddPoint(protocol.pet_info)
                    PetCtrl.SendPetAssignAttributePoints(protocol.pet_info.index, add_point)
                end
            end
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_COMPREHEND_GIFT then--or     --领悟
            -- protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_REPLACE_GIFT then     --打书（学技能）
        local new_gift_config = self.data:GetNewGiftConfig(protocol.pet_info)
        if new_gift_config.name ~= nil then
            PublicPopupCtrl.Instance:Center(string.format(Language.Pet.GetNewGiftFormat, new_gift_config.name))
        end
        audioName = protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_REPLACE_GIFT and CommonAudio.JinJie1 or CommonAudio.JinJie2
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_REFINED then          --洗炼
        audioName = CommonAudio.ZhuangBeiFuShuJian
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_REFINED_SAVE then     --洗炼结果保存
        audioName = CommonAudio.JinJie2
        self.data:CheckNewFlag(protocol.pet_info)
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE then     --脱穿装备
        local add_list, lost_list = self.data:GetEquipSkillChange(protocol.pet_info)
        if next(add_list) then
            for k,v in pairs(add_list) do
                PublicPopupCtrl.Instance:Center(string.format(Language.Pet.GetNewEquipSkillFormat, v.name))
            end
        end
        if next(lost_list) then
            for k,v in pairs(lost_list) do
                PublicPopupCtrl.Instance:Center(string.format(Language.Pet.LoseEquipSkillFormat, v.name))
            end
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_RECOVER then
        --通过GM命令新获得的宠物也是下发这条协议（不排除其他方式获得宠物是否也是这个type）
        --当前没有这个宠物时判断为新宠物，需要请求属性
        local old = self.data:GetPetInfo(protocol.pet_info.index)
        if old ~= nil then
            need_flush_attr = false
        end
 
    elseif protocol.notie_type ==  PetNotieType.PET_NOTIE_TYPE_ASSIGN_POINTS then
        local old = self.data:GetPetInfo(protocol.pet_info.index)
        local _, old_added = old:PointNum()
        local _, added = protocol.pet_info:PointNum()
        if old_added ~= added and added == 0 then --洗点后取消自动加点
            self:SendPetAutoAddPoint(0)
        end
        if ViewMgr:IsOpen(PetAddPoint) then
            ViewMgr:FlushView(PetAddPoint)
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_LOCK then  --锁定
        -- if ViewMgr:IsOpen(PetStrengthen) then
        --     ViewMgr:FlushView(PetStrengthen)
        -- end
        if ViewMgr:IsOpen(PetFreeLifeSeveral) then
            ViewMgr:FlushView(PetFreeLifeSeveral)
        end
    -- elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_PET_USE_SKIN then
    --     local growth_t = self.data:GetSkinGrowthChange(protocol.pet_info.info)
    --     for k,v in pairs(growth_t) do
    --         local index = PetData.GetZZIndex(k)
    --         if v > 0 then
    --             PublicPopupCtrl.Instance:Center(Language.Pet.AddPointAttrName[index]..string.format(Language.Common.Xstr, COLORSTR.Green3,"+"..tostring(v)))
    --         elseif v < 0 then
    --             PublicPopupCtrl.Instance:Center(Language.Pet.AddPointAttrName[index]..string.format(Language.Common.Xstr, COLORSTR.Red5, tostring(v)))
    --         end
    --     end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_NO_TYPE then --无标记下发
        need_flush_attr = false
        delay_set = true
        self.data.pet_notice_no_type = protocol.pet_info
        if need_show_score then
            TimeHelper:AddDelayTimer(function ()
                showPetScore()
                self.data:ChangePetList(protocol.pet_info)
            end,3)
        else
            self.data:ChangePetList(protocol.pet_info)
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_BREAK_THROUGHS or protocol.notie_type == PetNotieType.PET_MUTANT_LIAN_YAO_MUTANT then --强化
        delay_set = true
        ---LogError("single set str_result pet_info ",protocol.pet_info:Name())
        self.data.strengthen_result = protocol.pet_info
        if self.is_str_yao and protocol.pet_info:IsLianYao() then
            self.data.strengthen_result.is_show_yao = true
            self.is_str_yao = false
        end
        if protocol.notie_type == PetNotieType.PET_MUTANT_LIAN_YAO_MUTANT then
            self.data.strengthen_result.is_yao_mutant = true
            self.data.strengthen_result.is_show_yao = true
            self.data:SetOldStrLevel(protocol.pet_info.yao_str_level, 1)
            local yao = protocol.pet_info:YaoPet()
            local skill_list = yao:SkillList()
            local old_yao = old_pet:YaoPet()
            local old_skill_list = old_yao:SkillList()
            for i,v in pairs(skill_list) do
                if old_skill_list[i] then
                    v.old_skill_id = old_skill_list[i].skill_id
                end
            end
        end
        ViewMgr:OpenView(PetStrSucc, {close_callback_func = function ()
            TimeHelper:AddDelayFrameTimer(function ()
                self.data.pet_select:SetDirty("index")
            end,10)
        end})
        TimeHelper:AddDelayTimer(function ()
            if need_show_score then
                showPetScore()
            end
            -- LogError("PET_NOTIE_TYPE_BREAK_THROUGHS delay_set",self.data.strengthen_result.name)
            self.data:ChangePetList(protocol.pet_info)
        end,3)
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_SET_ADD_POINT_INFO then --自动加点设置后（也有可能是取消）
        local auto_info = protocol.pet_info:AutoAddInfo()
        if auto_info.index ~= 0 then
            local add_point = self.data:GetAutoAddPoint(protocol.pet_info)
            PetCtrl.SendPetAssignAttributePoints(protocol.pet_info.index, add_point)
        end
        if ViewMgr:IsOpen(PetAddPoint) then
            ViewMgr:FlushView(PetAddPoint)
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_WEAPON_EXCHANGE then
        if protocol.pet_info:Weapon() then
            local weapon = protocol.pet_info:Weapon()
            ViewMgr:OpenView(GiftOpenView,{item = weapon,gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS})
        end
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_PRAY_CHANGE then
        self:CheckPrayActiveTip(protocol.pet_info)
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_LIAN_YAO then
        local old = self.data:GetPetInfo(protocol.pet_info.index)
        local new = protocol.temp_pet
        local old_list = old:LianYaoSkillList({})
        local new_list = new:LianYaoSkillList({})
        for i = #old_list + 1, PetRefiningData.Instance.other_info.addskill_max do
            if new_list[i] then
                new.info.lian_yao_passive_list[i].new_flag = true
            end
        end
        PetRefiningData.Instance:ViewData().has_result = not PetRefiningData.Instance:ViewData().has_result
        -- PetRefiningData.Instance:SetResultPet(new)
        -- ViewMgr:OpenView(PetRefiningResult)
        TimeHelper:AddDelayTimer(function ()
            PetRefiningData.Instance:SetResultPet(new)
            ViewMgr:OpenView(PetRefiningResult)
        end, 9.3)
        audioName = CommonAudio.JinJie2
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_AMULET_INSTENSIFY then
        local item = protocol.pet_info.equip_item
        ViewMgr:OpenView(AmuletAssemblyInfoView,{item = item})
        AmuletData.Instance:SetAmuletIntensifyData(1, item)
    elseif protocol.notie_type == PetNotieType.PET_NOTIE_TYPE_FEISHENG then
        local pet = protocol.pet_info
        if PetFeiShengData.Instance:IsInTask(pet) then
            local target_fs_lv = pet:FeiShengTimes() + 1
            local can_feisheng = pet:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT >= target_fs_lv
            if not can_feisheng then
                PetFeiShengCtrl.Instance:RequestTask(pet:Index())
            end
        end
    end

    -- 判断这个是不是副宠
    -- local main_pet_index = protocol.pet_info:PrayMainIndex()
    -- if main_pet_index ~= nil then
    --     local pet = self.data:GetPetInfo(main_pet_index)
    --     self:CheckPrayActiveTip(pet)
    -- end

    if audioName ~= nil then
        AudioMgr:PlayEffect(AudioType.UI, audioName)
    end
    if need_flush_attr then
        PetCtrl.SendPetOp(PET_OP_TYPE.PetAttrReq, protocol.pet_info.index)
        self.data:SetPetInfoNotie(protocol.notie_type)
    end
    if not delay_set then
        if need_show_score then
            showPetScore()
        end
        self.data:ChangePetList(protocol.pet_info)
    end
    local pet = self.data:GetFollowPetInfo()
    if pet then
        local follow = MainPlayer:Vo().follow_pet
        if follow then
            if pet:Name() ~= follow.name or pet:SkinId() ~= follow.skin_id then
                MainPlayer:Vo().follow_pet = {
                    pet_id = pet:ID(),
                    name = pet:Name(),
                    skin_id = pet:SkinId(),
                    fly_flag = pet:FeiShengTimes(),
                    rank = pet:Rank()
                }
                MainPlayer:FlushFollow()
            end
        end
    end
    --[[ if self.data:IsFightIndex(protocol.pet_info.index) then
        RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
    end ]]

 

    -- ViewMgr:FlushView(PetGiftReplace2)
    ViewMgr:FlushView(PetExp)
    ViewMgr:FlushView(PetReLife)
    if TeamData.Instance.heal_menu.object > 0 then
        TeamData.Instance:NeedFlushMenu()
    end
end
function PetCtrl:RecvPetOtherInfo(protocol)
    local old_info = self.data:GetOtherInfo()
    local flush_pet_attr = false
    for k, v in pairs(old_info) do
        if type(v) ~= "table" and protocol.pet_other_info[k] ~= nil and v ~= protocol.pet_other_info[k] then
            AudioMgr:PlayEffect(AudioType.UI, "click_tongyong3")
            --出战及待机状态宠物发生变化时，所有出战及待机宠物需要申请刷新一次属性（共鸣属性可能变化）
            if k ~= "extend_grid_num" and k ~= "extend_times" and k ~= "follow_index" then
                flush_pet_attr = true
            end
            break
        end
    end
    if flush_pet_attr then
        if protocol.pet_other_info.fight_index ~= -1 then
            self.SendPetOp(PET_OP_TYPE.PetAttrReq, protocol.pet_other_info.fight_index)
        end
        for i,v in ipairs(protocol.pet_other_info.standby_list) do
            if v ~= -1 then
                self.SendPetOp(PET_OP_TYPE.PetAttrReq, v)
            end
        end
        if ViewMgr:IsOpen(PetView) then
            self.data.jump_top = true
        end
        -- RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
    else
        self.data.jump_top = false
    end
    local old_pet = nil
    if protocol.pet_other_info.fight_index ~= -1 and protocol.pet_other_info.fight_index ~= old_info.fight_index and old_info.fight_index ~= nil then
        old_pet = old_info.fight_index >= 0 and self.data:GetPetInfo(old_info.fight_index) or {pet_id = old_info.fight_index}
    else
        self.data.change_state_cache_index = nil
    end
    self.data:SetOtherInfo(protocol.pet_other_info)
    -- local locked_pet = self.data:GetLockedPet()
    -- for k,v in pairs(locked_pet) do
    --     PetCtrl.SendPetOp(PET_OP_TYPE.Lock, k)
    -- end
    self.data:SetShowAttrIndex()
    local pet = self.data:GetFollowPetInfo()
    if pet and MainPlayer then
        MainPlayer:Vo().follow_pet = {
            pet_id = pet:ID(),
            name = pet:Name(),
            skin_id = pet:SkinId(),
            fly_flag = pet:FeiShengTimes(),
            rank = pet:Rank()}
    else
        if MainPlayer then
            MainPlayer:Vo().follow_pet = {pet_id = 0, name = ""}
        end
    end
    if MainPlayer then
        MainPlayer:FlushFollow()
    end
    if TeamData.Instance.heal_menu.object > 0 then
        TeamData.Instance:NeedFlushMenu()
    end
    --LogError("old_pet", old_pet and old_pet.pet_id or "is nil")
    if old_pet ~= nil then
        if self.data.change_state_cache_index == protocol.pet_other_info.fight_index then
            FormationData.Instance:TopLayerShowPet(old_pet.pet_id)
        else
            TimeHelper:AddDelayTimer(function ()
                FormationData.Instance:TopLayerShowPet(old_pet.pet_id)
            end, 14)
        end
    end
end

function PetCtrl:RecvPetDeleteInfo(protocol)
    --LogError("RecvPetDeleteInfo",protocol)
    if protocol.pet_index == self.data.pet_strengthen_eat_index then
        local pet_index = self.data.pet_strengthen_eat_index
        self.data.pet_strengthen_eat_index = 0
        TimeHelper:AddDelayTimer(function ()
            self.data:DeletePetList(pet_index)
        end,3)
    else
        self.data:DeletePetList(protocol.pet_index)
    end
end

function PetCtrl:RecvPetAttrListInfo(protocol)
    --LogError("RecvPetAttrListInfo 宠物属性list",PetData.Instance.pet_single_info_notie_type,PetData.Instance:GetIsPetInfoNotieShowAttrChange(),protocol)
    if PetData.Instance:GetIsPetInfoNotieShowAttrChange() then
        -- LogError("ShowAttrChange ")
        local old_attr = self.data:GetPetAttrInfo(protocol.index)
        local change_list = {}
        for k, v in pairs(protocol.attr_list or {}) do
            if old_attr == nil then
                old_attr = {}
            end
            if old_attr[k] ~= nil and (v ~= 0 or old_attr[k] ~= 0) and old_attr[k] ~= v and k < 40 then
                change_list[k] = {
                    --2月27修改，显示旧值修改为显示变化结果（胡总要求）
                    old_val = v,
                    --old_attr[k],
                    change_val = v - old_attr[k]
                }
            end
        end
        --LogError("RecvPetAttrListInfo change_list=",change_list)

        local is_play = true
        --灵脉技能播放第一个宠物属性
        if PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GUILD_INBORN or PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_STAR_MAP then
            -- local show_list, first_index = self.data:GetPetShowList()
            is_play = protocol.index == PetData.Instance.trian_skill_attr_change_index
        elseif PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_TRAIN_SKILL and 
                protocol.index ~= PetData.Instance.trian_skill_attr_change_index then
            is_play = false
        end

        local pet_info = self.data:GetPetInfo(protocol.index)
        -- LogDG(pet_info.info.capaility, protocol.capaility)
        if pet_info ~= nil and is_play then
            pet_info.info.capaility = protocol.capaility
            -- if self.data:IsFightIndex(protocol.index) then
            --     RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
            -- end
            local title_info = {fromType = AttrChangeData.ChangeFromType.Pet, showName = pet_info.name}
            local function playAttrChange()
                MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
                self.data:SetPetInfoNotie(PetNotieType.PET_NOTIE_OLD_TYPE)
            end
            --非宠物强化的属性变化直接播放，宠物强化的要延迟播放，等界面UI逻辑
            -- LogError("notie_type",PetData.Instance.pet_single_info_notie_type)
            local delay_time = 0
            if PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_BREAK_THROUGHS then
                delay_time = 3
            elseif PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_REPLACE_GIFT and not self.data.pet_gift_replace_s.jump_anim then
                delay_time = 3
            elseif PetData.Instance.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GUILD_INBORN then
                delay_time = GuildData.Instance:GetDelayTime(2)
            end
            if delay_time > 0 then
                TimeHelper:AddDelayTimer(
                    function ()
                        playAttrChange()
                    end, delay_time)
            else
                playAttrChange()
            end
        end
    end
    self.data:SetPetAttrList(protocol.index, protocol.attr_list)
    self.data.pet_select:SetDirty("index")
    RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
end
function PetCtrl:RecvPetBreakthrougtsRet(protocol)
    -- Debuger.LogError("RecvPetBreakthrougtsRet ",protocol.index.."|".. protocol.is_succ.."|"..protocol.old_str_level)
    self.is_str_yao = protocol.is_lian_yao == 1
    self.data:SetOldStrLevel(protocol.old_str_level, protocol.is_succ)
    -- ViewMgr:OpenView(PetStrSucc)
end
--@TODO 2019-06-03 15:08:37 获得宠物
function PetCtrl:RecvSendPetGetRet(protocol, is_delay)
    --LogError("RecvSendPetGetRet ",protocol.reason)
    local pet_info = self.data:GetPetInfo(protocol.index)
    if pet_info == nil then
        return
    end
    local gift_list = pet_info.gift_list
    --新手宠物获得后自动跟随
    if gift_list[1].gift_id == 0 then
        self.SendPetOp(PET_OP_TYPE.SetPetState, protocol.index, 2)
    end
    self.data:SetNewPet(pet_info)
    if protocol.reason == PetGetReason.DEFAULT then
        if MainViewData.Instance:IsBattle() and not is_delay then
            local info = {
                index = protocol.index,
                reason = protocol.reason
            }
            table.insert(self.delay_pet_list, info)
            return
        else
            ViewMgr:OpenView(PetGain, {is_xilian = false})
        end
    elseif protocol.reason == PetGetReason.REFINED_MUTANT_PET or protocol.reason == PetGetReason.ITEM_MUTANT_PET then
        if self.data.pet_notice_no_type ~= nil and protocol.index == self.data.pet_notice_no_type.index then
            self.data:SetNewPet(self.data.pet_notice_no_type)
        end
        ViewMgr:OpenView(PetGain, {is_xilian = true})
    elseif protocol.reason == PetGetReason.COMPOSE_PET then
        -- end
        --碎片合成获得的宠物判断是否是变异的，如果是变异的则播放砸蛋获取动画
        -- if pet_info:IsMutant()then
        --     ViewMgr:OpenView(PetEgg)
        -- else
        -- 神兽碎片合成获得的神兽不立即打开宠物获得界面，神兽碎片合成界面有动效
        if pet_info:Quality() ~= 5 then
            ViewMgr:OpenView(PetGain,{is_xilian = false})
        end
    elseif protocol.reason == PetGetReason.XUN_BAO_EXCHANGE then
        -- ViewMgr:OpenView(PetGain, {is_xilian = false})
        if ViewMgr:IsOpen(TreasureHuntView) then
            TreasureHuntData.Instance.exchange_request.sended_req = 2
        else
            ViewMgr:OpenView(PetGain, {is_xilian = false})
        end
    elseif protocol.reason == PetGetReason.PET_BEFALL then
        -- 神兽降临活动不作操作
    elseif protocol.reason == PetGetReason.REALM_BREAK then
        -- 心灵之境获得的宠物需要存下来自己弹窗
        FightData.Instance:SetFightSoulRewardPetData(pet_info)
    elseif protocol.reason ~= PetGetReason.SPECIAL_PET and protocol.reason ~= PetGetReason.MULTI_PIECES then
        ViewMgr:OpenView(PetEgg)
    end
end

function PetCtrl:RecvTradeMarketSearchPetForQuickBuy(protocol)
    -- LogError("【宠物快捷】下发宠物数量"..protocol.item_count)
    self.data:SetShopPetInfo(protocol)
end

function PetCtrl:OnSealPetGetItemInfo(protocol)
    if protocol.is_get_pet == 0 then
        ChatCtrl.Instance:ChannelChatCustom {
            pet_seal_get = {pet = Item.Init(protocol.pet_id), item = Item.Init(protocol.item_id, protocol.num)}
        }
    else
        TimeHelper:AddDelayTimer(function ()
            ChatCtrl.Instance:ChannelChatCustom {seal_get = {name = Pet.New({pet_id = protocol.pet_id}):Name()}}
            TaskData.Instance:TaskChainSubContinue(true)
            GuildData.Instance:PetSealCountinue()
        end, 5)
    end
end

function PetCtrl:OnSCPetIdRecordList(protocol)
    self.data:SetHasGetList(protocol)
end

function PetCtrl:OnMovePetGridNotice(protocol)
    -- LogError("OnMovePetGridNotice")
    if protocol.type == 1 then
        --存入仓库
        local pet = self.data:GetPetInfo(protocol.pet_index)
        pet.index = protocol.pet_storage_index
        self.data:ChangeStorageList(pet, protocol.pet_storage_index)
        self.data:DeletePetList(protocol.pet_index)
    elseif protocol.type == 2 then
        --取回背包
        local pet = self.data:GetStorageInfo(protocol.pet_storage_index)
        pet.index = protocol.pet_index
        --先从列表里拿一次的原因是仓库内的宠物经验在放进去时可能是超标的（等级被玩家等级压制）
        --取出时玩家已经升级这时服务器会下发宠物升级协议，升级协议可能会比宠物取出协议早到，导致此处赋值时宠物等级数据错误
        local pet_info = self.data:GetPetInfo(pet.index)
        if pet_info == nil then
            self.data:ChangePetList(pet)
        end
        self.data:DeleteStorageList(protocol.pet_storage_index)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[159])
    end
end

function PetCtrl:OnSCPetSkinInfo(protocol)
    local old_list = self.data:GetPetActivedSkinList()
    if self.data.pet_skin_count ~= -1 then
        local skin_cfg = nil
        for k,v in pairs(protocol.pet_skin_list) do
            if old_list[k] == nil then
                skin_cfg = Cfg.PetSkinAttr(k)
                break
            end
        end
        if skin_cfg then
            local function showActivePetSkin()
                local pets = string.split(skin_cfg.pet_id,"|")
                local pet_cfg = self.data:GetPetCfg(tonumber(pets[1]))
                if pet_cfg ~= nil then
                    PublicPopupCtrl.Instance:Center(string.format(Language.Pet.PetSkinActivePublicText, pet_cfg.name, skin_cfg.name))
                end
                AppearanceCtrl.Instance:OpenAppearanceShowPet(skin_cfg)
            end
            if TreasureHuntData.Instance.not_show_anim or not ViewMgr:IsOpen(TreasureHuntDetailView) then
                showActivePetSkin()
            else
                TimeHelper:AddDelayTimer(showActivePetSkin,9)
            end
        end
    end
    self.data:SetPetSkinList(protocol.pet_skin_list)
    self.data.pet_skin_count = protocol.count
end

function PetCtrl:PopDelayPetShow()
    if self.delay_pet_list[1] then
        self:RecvSendPetGetRet(table.remove(self.delay_pet_list, 1), true)
    end
end

function PetCtrl:SendRemoveEquip()
    local pet = self.data:GetCurSelect()
    PetCtrl.SendPetOp(PET_OP_TYPE.PetEquipmentPutOff, pet.index)
end

function PetCtrl.SendPetOp(op_type, p1, p2, p3, p4, p5)
    -- LogError("OP ",tostring(op_type),"|index ",tostring(p1),"|",tostring(p2),"|",tostring(p3),"|",tostring(p4))
    local protocol = GetProtocol(CSPetOp)
    protocol.op_type = op_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    protocol.p3 = p3 or 0
    protocol.p4 = p4 or 0
    protocol.p5 = p5 or 0
    SendProtocol(protocol)
end
function PetCtrl.SendPetReName(index, name)
    local protocol = GetProtocol(CSPetRename)
    protocol.index = index
    protocol.name = name
    SendProtocol(protocol)
end
function PetCtrl.SendPetAssignAttributePoints(index, param)
    -- LogError("index",index,"|",param)
    local protocol = GetProtocol(CSPetAssignAttributePoints)
    protocol.index = index
    protocol.con = param.con or 0
    protocol.str = param.str or 0
    protocol.def = param.def or 0
    protocol.agi = param.agi or 0
    protocol.magic = param.magic or 0
    SendProtocol(protocol)
end

function PetCtrl.SendTradeMarketSearchPetForQuickBuy(param_t)
    local protocol = GetProtocol(CSTradeMarketSearchPetForQuickBuy)
    protocol.order_type = param_t.order_type
    protocol.item_id = param_t.item_id
    protocol.last_seller_uid = param_t.last_seller_uid
    protocol.last_price = param_t.last_price
    protocol.last_put_on_shelve_time = param_t.last_put_on_shelve_time
    protocol.last_str_level = param_t.last_str_level
    protocol.quality = param_t.quality
    SendProtocol(protocol)
end
function PetCtrl.SearchPetForQuickBuyOrder(order_type, item_id, quality)
    local protocol = GetProtocol(CSTradeMarketSearchPetForQuickBuy)
    protocol.order_type = order_type
    protocol.item_id = item_id
    protocol.last_seller_uid = 0
    protocol.last_price = 0
    protocol.last_put_on_shelve_time = 0
    protocol.last_str_level = 0
    protocol.quality = quality or 0
    SendProtocol(protocol)
end

function PetCtrl.SendAbandonPets(pets_index)
    -- LogError("pets_index",pets_index)
    local protocol = GetProtocol(CSAbandonmentPetByList)
    protocol.pets_index = {}
    for k,v in pairs(pets_index) do
        table.insert(protocol.pets_index, k)
    end
    SendProtocol(protocol)
end
-- 请求突破属性预览 -- 原则上只能请求选中的宠物
function PetCtrl:SendPetBreachAttri(breach_lv)
    local pet = self.data:GetCurSelect()
    if pet == nil then
        return
    end
    local pet_index = pet.index
    --LogError("请求了突破属性",PET_OP_TYPE.UpLevelBreakAttr,pet_index,breach_lv)
    PetCtrl.SendPetOp(PET_OP_TYPE.UpLevelBreakAttr,pet_index,breach_lv)
end

-- 突破属性预览
function PetCtrl:OnSCPetBreachAttri(protocol)
    --LogError("收到了属性预览",protocol)
    if protocol.info.index == self.data:GetSelectPetIndex() then
        self.data:SetPetBreachAttri(protocol.info)
    end
end

function PetCtrl:SendPetAutoAddPoint(index)
    local t_param = {}
    for i=1,16 do
        if i == index then
            t_param[i] = 1
        else
            t_param[i] = 0
        end
    end
    RoleCtrl.Instance:SendSetAutoAddPoint(SET_AUTO_ADD_POINT_FLAG.SET_AUTO_ADD_POINT_FLAG_PET, {param0 = self.data:GetSelectPetIndex(),param1 = bit:b16d(t_param)})
end

-- 宠物祝福列表信息下发  -- 只是进入游戏下发一次
function PetCtrl:OnSCPetPrayListInfo(protocol)
    --LogError("AAA宠物祝福列表信息下发:", protocol)
    self.data.old_pray_info_data = {}
    self.data.pray_info_data.vice_pet_list = protocol.pray_list
    self:HandlePrayInfoData()
    for i,v in pairs(protocol.pray_list) do
        if v >= 0 then
            self:SetPrayOldData(v)
        end
    end
end

-- 单个宠物祝福信息下发 -- 变化下发
function PetCtrl:OnSCPetPrayInfo(protocol)
    --LogError("BBB单个宠物祝福信息下发:", protocol)
    local info = protocol.info
    local main_inex = self.data.pray_info_data.vice_pet_list[info.pet_index] == -1 and info.main_pet_index or -1
    self:SetPrayOldData(main_inex)
    self.data.pray_info_data.vice_pet_list[info.pet_index] = info.main_pet_index
    self:HandlePrayInfoData()
end

-- 碎片批量分解获得宠物
function PetCtrl:OnSCPetComposeBatchRet(protocol)
    local index_list = protocol.pet_index_list
    ViewMgr:OpenView(PetMultiPiecesResult, {index_list = index_list})
end

-- 批量合成操作
function PetCtrl:SendMultiPieces(info_list, ctrl_type)
    local protocol
    if ctrl_type == 1 then
        protocol = GetProtocol(CSPetComposeBatch)
    else
        protocol = GetProtocol(CSPetComposeBatchAndDec)
    end
    protocol.info_list = info_list
    SendProtocol(protocol)
end

-- 强化共鸣
function PetCtrl:OnSCPetResonanceInfo(protocol)
    PetResonanceData.Instance:SetInfoData(protocol)
end


-- 处理祝福数据
function PetCtrl:HandlePrayInfoData()
    local data = {}
    for i,v in pairs(self.data.pray_info_data.vice_pet_list or {}) do
        if v >= 0 then
            if data[v] == nil then
                data[v] = {}
            end
            table.insert(data[v], i)
        end
    end
    self.data.pray_info_data.pet_list = data
end

-- 保留祝福旧数据
function PetCtrl:SetPrayOldData(pet_index)
    local pet = self.data:GetPetInfo(pet_index)
    if pet == nil then
        return
    end
    local old_pray_cfg = pet:PrayActiveCfg()
    self.data.old_pray_info_data[pet_index] = old_pray_cfg
end

-- 检查祝福激活提示
function PetCtrl:CheckPrayActiveTip(pet)
    -- 延迟半秒，等宠物信息齐全在检查提示
    Invoke(function ()
        local cur_pet = pet
        local pray_cfg = cur_pet:PrayActiveCfg()
        if not TableIsEmpty(pray_cfg) then
            local old_pray_cfg = self.data:GetPrayOldData(cur_pet.index)
            local list_cfg = {}
            -- LogError("AAA",pray_cfg)
            -- LogError("BBB",old_pray_cfg)
            if TableIsEmpty(old_pray_cfg) then
                list_cfg[1] = pray_cfg
            elseif pray_cfg.benediction_level > old_pray_cfg.benediction_level then
                list_cfg[1] = old_pray_cfg
                list_cfg[2] = pray_cfg
            end
            
            if not TableIsEmpty(list_cfg) then
                ViewMgr:OpenView(PetPrayActiveTip, {cfg_list = list_cfg})
            end
        end
        self.data.old_pray_info_data[pet.index] = pray_cfg
    end, 0.5)
end


function PetCtrl:OnSCPetTopRankInfo(protocol)
    self.data:SetPetTopRank(protocol.info)
end

function PetCtrl:SendPatnerPetRankReqPet()
	PartnerCtrl.Instance:SendPatnerPetRankReq(0)
end

-- 传一个item_id然后自动打开宠物详细信息，带专属魂器的
function PetCtrl:OpenPetDetailView(item_id)
    local pet = Pet.New({item_id = item_id})
    pet:Weapon(pet:ExclusiveWeapon())
    ViewMgr:OpenView(PetDetailView, pet)
end

-- 活动专属专属宠物详情
function PetCtrl:OpenActPetDetailView(pet)
    if TableIsEmpty(pet) then
        return
    end
    local limit_type = pet:LimitType()
    if limit_type == PetData.LimitType.Rahi then
        ViewMgr:OpenView(PetSpShow,{pet = pet})
        return
    end

    pet.show_tabs = {5}
    pet.hide_tabs = {2,4}
    pet.hide_weapon = true
    ViewMgr:OpenView(PetDetailView,pet)
end

-- 解锁背包格子
function PetCtrl:UnlockCell(func)
    local cfg = self.data:GetPetOtherCfg()
    local price = self.data:ExpandPrice()
    local other = self.data:GetOtherInfo()
    local max_lock = price[#price].times
    if other.extend_times >= max_lock then
        PublicPopupCtrl.Instance:Center(ErrorText[124])
        return
    end
    local lock_num = max_lock - other.extend_times >= cfg.add_num and cfg.add_num or max_lock - other.extend_times
    if lock_num == 0 then
        return
    end

    local fee = 0
    for i = other.extend_times + 1, other.extend_times + lock_num do
        fee = fee + price[i].bag_pet
    end

    local send_msg = function ()
        PetCtrl.SendPetOp(PET_OP_TYPE.ExpandBag)
        ViewMgr:CloseView(DialogTipsView)
    end
    func = func or send_msg
    local info = {
        content = string.format(Language.Pet.PetBagUnLock,lock_num,fee),
		confirm = {
			name = nil,
			func = func,
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function PetCtrl:OnSCPetAddExpNotice(protocol)
    if protocol.notice_type == 0 then
        local pet = PetData.Instance:GetPetInfo(protocol.pet_index)
        local str = string.format(Language.Pet.FullExpTip, pet:QuaName())
        ChatCtrl.Instance:ChannelChatCustom({content = str}, ChatConfig.ChannelId.system)
    end
end

function PetCtrl:GaiMingUplevel(index, count, is_tupo)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetGaiMingUpLevel, index, count, is_tupo) 
end
function PetCtrl:GaiMingAttrUplevel(index, seq)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetGaiMingAttrUpLevel, index , seq)
end
function PetCtrl:GaiMingAttrReset(index)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetGaiMingAttrReset, index)
end