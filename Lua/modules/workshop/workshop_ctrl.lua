FILE_REQ("modules/workshop/workshop_data")
FILE_REQ("modules/workshop/workshop_plus_data")
VIEW_REQ("modules/workshop/workshop")
VIEW_REQ("modules/workshop/workshop_getway")
VIEW_REQ("modules/workshop/workshop_quick_buy")
VIEW_REQ("modules/workshop/workshop_selectskill")
VIEW_REQ("modules/workshop/workshop_jewelry_neo")
VIEW_REQ("modules/workshop/workshop_forging_neo")
VIEW_REQ("modules/workshop/workshop_inset_neo")
VIEW_REQ("modules/workshop/workshop_smelt_neo")
VIEW_REQ("modules/workshop/workshop_smelt_plus")
VIEW_REQ("modules/workshop/smelt_pre_range_view")
VIEW_REQ("modules/workshop/workshop_ultiwork")
VIEW_REQ("modules/workshop/workshop_upgrade")
VIEW_REQ("modules/workshop/workshop_smelt_smelt_plus")
VIEW_REQ("modules/workshop/workshop_neveal")
VIEW_REQ("modules/workshop/workshop_neveal_atlas")
VIEW_REQ("modules/workshop/workshop_neveal_extract")
VIEW_REQ("modules/workshop/workshop_neveal_select")
VIEW_REQ("modules/workshop/workshop_neveal_skill_show_view")
VIEW_REQ("modules/workshop/workshop_neveal_skill_tip")
VIEW_REQ("modules/workshop/workshop_neveal_resolve_view")
VIEW_REQ("modules/workshop/workshop_quick_forgebuy")
VIEW_REQ("modules/workshop/workshop_blazestar")
VIEW_REQ("modules/workshop/blazestar_done_view")
VIEW_REQ("modules/workshop/neveal_extral_plus")
VIEW_REQ("modules/workshop/workshop_neveal_change")
WorkshopCtrl = WorkshopCtrl or BaseClass(BaseCtrl)
-- 0 是装备着，1 是在背包
WORLSHOP_OP = {
    WORKSHOP_OP_COMPOUND_REQ = 0, --请求合成物品
    WORKSHOP_OP_GEM_MOUNTING = 1, --装备宝石镶嵌
    WORKSHOP_OP_GEM_DISMOUNT = 2, --装备宝石拆卸
    WORKSHOP_OP_MAKE_JEWELTY = 3, --请求打造灵饰
    WORKSHOP_OP_JEWE_REFORGE = 4, --请求灵饰重锻
    WORKSHOP_OP_GAIN_REFORGE = 5, --选择重锻结果
    WORKSHOP_OP_GEM_UPGRADES = 6, --装备宝石升级
    WORKSHOP_OP_GEM_CONF_UGD = 7, --确认宝石升级
    WORKSHOP_OP_PET_CARD_EXC = 8, --宠物封印卡兑换
    WORKSHOP_OP_FABAO_SP_EXC = 9, --法宝碎片兑换成仙气
    WORKSHOP_OP_BATCH_COMP_REQ = 10, --请求批量合成装备
    WORKSHOP_OP_AVTIVE_EQUIP = 11, --请求激活孔位属性
    WORKSHOP_OP_REFRESH_EQUIP = 12, --请求洗练未锁定孔位属性
    WORKSHOP_OP_SAVE_EQUIP_REFRESH = 13, --请求保存洗练后的属性
    WORKSHOP_OP_ULTIMATE_WORK = 14, --请求精工
    WORKSHOP_OP_GOD_STRONGTHEN = 15, --请求神装强化
    WORKSHOP_OP_DIAN_HUA = 16, -- 装备点化
    WORKSHOP_OP_SAVE_DIAN_HUA = 17, -- 保存点化
    WORKSHOP_OP_FU_MO = 18, -- 装备附魔
    WORKSHOP_OP_XI_FU = 19, -- 装备吸附
    WORKSHOP_OP_DISCOMPOSE = 20, -- 点化石分解
    WORKSHOP_OP_UP_STAR = 21, -- 装备熔铸
    WORKSHOP_OP_UP_LEVEL_DIANHUA =22, -- 点化石强化
    WORKSHOP_OP_SWITCH_EQUIP_JEWELRY1 = 23,--装备转换穿戴
    WORKSHOP_OP_SWITCH_EQUIP_JEWELRY2 = 24,--装备转换背包
    WORKSHOP_OP_OPEN_DIAN_HUA_PLAN_HOLE = 25, --!< 解锁点化方案 param1 是否穿戴(state 0装备 1背包) param2 下标idx(穿戴/背包)
    WORKSHOP_OP_SWITCH_DIAN_HUA_PLAN = 26,	--!< 切换点化方案 param1 是否穿戴 param2 下标idx(穿戴/背包) param3 新方案序号(0-3)
}

-- 额外参数： 0 - 合成表 seq；		1 - 装备所属位置；	2 - 装备所属位置； 3 -灵饰列表id 4 --材料格子1 5 -0-1-2 10 - 合成表seq 11 是否装备     12：是否装备       13：是否装备      14：1普通精工 2强化精工
-- 额外参数： 0 - 合成数量；		1 - 装备所占格子；	2 - 装备所占格子；                 --材料格子2             - 合成数量  11 装备所属位置 12：装备所属位置    13：装备所属位置  14：是否装备
-- 额外参数： 0 - 是否强化打造；	1 - 宝石所占格子；	2 - 宝石镶嵌槽位；                                                    11 装备所占格子 12：装备所占格子    13：装备所占格子  14：装备所属位置
--           0 - 是否化身打开      1 - 是否灵饰       2 - 是否灵饰                                                          11 词条位置     12：锁定格子（二进）13：是否化身      14：装备所占格子(在背包就是背包index,在装备列表就是列表index)
--                                                                                                                        11 是否化身    12：是否化身                         14：是否化身
--                                                                                                                                      12:是否使用元宝代替
-- 16 param1 是否穿戴中 param2 下标 param3 材料背包类型 param4 材料背包下标
-- 17 param1 是否穿戴中 param2 下标
-- 18 param1 是否穿戴中 param2 下标
-- 19 param1 是否穿戴中 param2 下标
-- 21 param1 铸星合成seq param2 是否灵饰 param3 装备所属位置 param4 装备所占格子 param5 消耗道具1位置 param6 消耗道具2位置
-- 22 param1 主点化石index param2 消耗点化石1 param3 消耗点化石2
-- 23 param1 是否灵饰 param2 装备索引 param3 新item_id
-- 24 param1 背包col param2 index param3 新item_id
function WorkshopCtrl:__init()
    if WorkshopCtrl.Instance ~= nil then
        Debuger.LogError("[WorkshopCtrl] attempt to create singleton twice!")
        return
    end
    WorkshopCtrl.Instance = self
    self.data = WorkshopData.New()
	self.popup_list = {}

    self:RegisterProtocol(SCWorkshopAllInfoRespond, "RecvWorkshopAllInfoRespond")
    self:RegisterProtocol(SCSingleRecipeActiveFlag, "RecvSingleRecipeActiveFlag")
    self:RegisterProtocol(SCJewelryReforgeOutcomes, "RecvJewelryReforgeOutcomes")
    self:RegisterProtocol(SCGemOneKeyUpgradesNeeds, "RecvGemOneKeyUpgradesNeeds")
    self:RegisterProtocol(SCWorkshopComposeResults, "RecvWorkshopComposeResults")
    self:RegisterProtocol(SCWorkShopJewelryInfo,"RecvWorkShopJewelryInfo")
    self:RegisterProtocol(CSWorkshopOperateRequire)
    self:RegisterProtocol(CSConvertPetSkillBookSeq)
    self:RegisterProtocol(CSChangeEquipVocReq)
    self:RegisterProtocol(CSWorkshopDecomposeReq)

    local forging_remind_func = BindTool.Bind(self.FlushForgingRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.Forging,
        BagData.Instance.item_grid,
        forging_remind_func
    ) 
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.Forging,
        self.data.remind_block,
        forging_remind_func
    ) 

    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.NeoCompose,
        BagData.Instance.item_grid,
        BindTool.Bind(self.FlushComposeRemind, self)
    )
    local jewerly_remind_func = BindTool.Bind(self.FlushJewelryRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.Jewelry,
        BagData.Instance.item_grid,
        jewerly_remind_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.Jewelry,
        self.data.remind_block,
        jewerly_remind_func
    ) 
    -- local flush_func = BindTool.Bind(self.FlushSmeltRemind, self)
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Smelt,
    --     BagData.Instance.item_grid,
    --     flush_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Smelt,
    --     BagData.Instance.jewelry_list,
    --     flush_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Smelt,
    --     BagData.Instance.equipment_list,
    --     flush_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Smelt,
    --     BagData.Instance.equipment_voc_list,
    --     flush_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Smelt,
    --     WorkshopData.Instance.smelt_func_open,
    --     flush_func
    -- )
    -- local stone_func = BindTool.Bind(self.FlushStoneRemind, self)
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Inser,
    --     BagData.Instance.item_grid,
    --     stone_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Inser,
    --     RoleData.Instance.base_data,
    --     stone_func
    -- )
    -- 涉及多个监听红点的情况下需要单独开一个绑定，防止重复注册
    -- local upgrade_func = BindTool.Bind(self.FlushUpgradeRemind, self)
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Upgrade,
    --     BagData.Instance.item_grid,
    --     upgrade_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Upgrade,
    --     BagData.Instance.equip_sort_upgrade,
    --     upgrade_func
    -- )
    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.Workshop.Upgrade,
    --     WorkshopData.Instance.upgrade_remind_sort,
    --     upgrade_func
    -- )
    --[[ RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.Inser,
        BagData.Instance.equipment_list,
        stone_func
    ) ]]

    local blazestar_func = BindTool.Bind(self.FlushBlazeStarRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.BlazeStar,
        BagData.Instance.item_grid,
        blazestar_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.BlazeStar,
        BagData.Instance.equipment_list,
        blazestar_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Workshop.BlazeStar,
        BagData.Instance.jewelry_list,
        blazestar_func
    )
end

-- function WorkshopCtrl:OnInit()
--     FunOpen.Instance:Register(Mod.Workshop.Smelt, BindTool.Bind(self.SmeltFunCall, self))
-- end
function WorkshopCtrl:FlushForgingRemind()
    local num = self.data:CheckWorkForging()
    return num
end
-- 技能书封印卡
function WorkshopCtrl:FlushComposeRemind()
    local num = self.data:CheckWorkCompose()
    return num
end
function WorkshopCtrl:FlushJewelryRemind()
    local num = self.data:CheckWorkJewelry()
    return num
end
function WorkshopCtrl:FlushStoneRemind()
    local num = self.data:CheckWorkInset()
    return num
end
function WorkshopCtrl:FlushSmeltRemind()
    local num = self.data:CheckNeoSmeltAttr()
    return num
end

function WorkshopCtrl:FlushUpgradeRemind()
    local num = self.data:CheckUpgradeRemind()
    return num 
end

function WorkshopCtrl:FlushBlazeStarRemind()
    local num = self.data:CheckBlazeStarRemind()
    return num 
end

-- function WorkshopCtrl:SmeltFunCall(opened)
--     if opened then 
--         local role_id = RoleData.Instance:GetBaseData().role_id
--         if UnityPlayerPrefs.GetInt(PrefKeys.SmeltFunOpenMark(role_id)) == 0 then
--             UnityPlayerPrefs.SetInt(PrefKeys.SmeltFunOpenMark(role_id), 1)
--             self.data:SetIsFunRed(true)
--         end
--     end 
-- end

function WorkshopCtrl:__delete()
    RemindCtrl.Instance:Remind():UnRegister(Mod.Workshop.Forging)
    -- RemindCtrl.Instance:Remind():UnRegister(Mod.Workshop.Smelt)
    -- RemindCtrl.Instance:Remind():UnRegister(Mod.Workshop.Inser)
end
function WorkshopCtrl:RecvWorkshopAllInfoRespond(protocol)
    -- Debuger.LogError(Serialize(protocol.sp_active_flag))
    -- LogError("!?@!RecvWorkshopAllInfoRespond",protocol)
    self.data:SetActiveInfo(protocol.sp_active_flag)
	self.data:SetCreateInfo(protocol.sp_created_flag)
    self.data:SetCoinSmeltTime(protocol)
end

function WorkshopCtrl:RecvSingleRecipeActiveFlag(protocol)
    self.data:ChangeActiveInfo(protocol.active_id, protocol.is_active,protocol.is_notice)
end
-- 重铸结果
function WorkshopCtrl:RecvJewelryReforgeOutcomes(protocol)
    -- Debuger.LogErrorSD(Serialize(protocol.reforge_list))
    self.data:SetSelectJewelry(protocol.reforge_list)
end

function WorkshopCtrl:SendOperateReqWithParamt(param_t)
    LogDG("!SendOperateReqWithParamt?@",param_t)
    self:SendOperateReq(param_t.op_type, param_t.param1, param_t.param2, param_t.param3,param_t.param4,param_t.param5,param_t.param6)
end

function WorkshopCtrl:SendOperateReq(op_type, param1, param2, param3,param4,param5,param6)
    -- LogError("op_type "..op_type.."|param1 "..(param1 or 0 ).."|param2 "..(param2 or 0 ).."|param3 "..(param3 or 0).."|param4 "..(param4 or 0).."|param5 "..(param5 or 0).."|param6 "..(param6 or 0))
    if op_type == WORLSHOP_OP.WORKSHOP_OP_GEM_UPGRADES and param1 ~= 0 and param1 ~= 1 then
        BuglyReport("1730 op 6 param1 illegal",param1)
        PublicPopupCtrl.Instance:Center(Language.Workshop.GemUpgradesError)
        return
    end
    local protocol = GetProtocol(CSWorkshopOperateRequire)
    protocol.op_type = op_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.param3 = param3 or 0
    protocol.param4 = param4 or 0
    protocol.param5 = param5 or 0
    protocol.param6 = param6 or 0
    protocol.reserve = 0
    SendProtocol(protocol)
end
function WorkshopCtrl:SendPetSkillSeq(seq, num, list)
    local protocol = GetProtocol(CSConvertPetSkillBookSeq)
    protocol.exc_seq = seq
    protocol.list_num = num --几种技能书
    protocol.item_list = list --列表
    SendProtocol(protocol)
end
--熔炼 替换词条
function WorkshopCtrl:ChangeEquipVocReq(param_t)
    -- LogError("ChangeEquipVocReq",param_t)
    local protocol = GetProtocol(CSChangeEquipVocReq)
    protocol.change_voc_type = param_t.change_voc_type or 0--0装备 1灵饰
    protocol.role_type = param_t.role_type or 0 -- 0 本尊 1 化身
    protocol.replacer_posi = param_t.replacer_posi or 0
    protocol.replacer_grid = param_t.replacer_grid
    protocol.replacer_voc_index = param_t.replacer_voc_index
    protocol.bereplacer_posi = param_t.bereplacer_posi or 1
    protocol.bereplacer_grid = param_t.bereplacer_grid
    protocol.bereplacer_voc_index = param_t.bereplacer_voc_index
    SendProtocol(protocol)
end
function WorkshopCtrl:CSWorkshopDecomposeReq(param_t)
    local protocol = GetProtocol(CSWorkshopDecomposeReq)
    protocol.grid_num = param_t.grid_num or 0--分解数量
    protocol.decompose_grid = param_t.decompose_grid or {}
    SendProtocol(protocol)
end
function WorkshopCtrl:GetWayView(info)
    ViewMgr:OpenView(WorkshopGetWay, info)
end

function WorkshopCtrl:RecvGemOneKeyUpgradesNeeds(protocol)
    self.data:SetMateData(protocol.mate_data)
    -- 如果不需要购买就直接升级
    if protocol.mate_data.fin_need_num == 0 then
        -- self:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_CONF_UGD)
        -- LogError(protocol)
        ViewMgr:OpenView(WorkShopQuickBuy, true)
        return
    end
    ViewMgr:OpenView(WorkShopQuickBuy)
end
function WorkshopCtrl:RecvWorkshopComposeResults(protocol)
    self.data:SetComposeResult(protocol)
    self.data.complete_item.item = protocol.item
    -- if Item.GetBigType(protocol.item.item_id) == ITEM_BIG_TYPE.PET_EQUIPMENT 
    --     or Item.GetBigType(protocol.item.item_id) == ITEM_BIG_TYPE.ELE_CRYSTAL
    --     or Item.GetBigType(protocol.item.item_id) == ITEM_BIG_TYPE.OTHER 
    --     or Item.GetBigType(protocol.item.item_id) == ITEM_BIG_TYPE.TREASURE_MAP
    --     or Item.GetBigType(protocol.item.item_id) == ITEM_BIG_TYPE.EXPENSE then
    --     return
    -- end
    if Item.GetBigType(protocol.item.item_id) ~= ITEM_BIG_TYPE.EQUIPMENT
        and Item.GetBigType(protocol.item.item_id) ~= ITEM_BIG_TYPE.JEWELRY then 
            return 
    end 
    if protocol.result == 1 then
        TimeHelper:AddDelayTimer(function ()
            ViewMgr:OpenView(ComposeSucc)
        end,0.1)
    else
        TimeHelper:AddDelayTimer(function ()
            ViewMgr:OpenView(ComposeDefeat)
        end,0.1)
    end
end

function WorkshopCtrl:RecvWorkShopJewelryInfo(protocol)
    self.data:SetJewelryQuaInfo(protocol)
end

--请求合成
function WorkshopCtrl:SendCompoundReq(param1,param2,param3)
    local param4 = ViewMgr:IsOpen(AnotherView) and 1 or 0
    self:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ,param1,param2,param3,param4)
end

function WorkshopCtrl:QuickUnFixStone(data)
    local param_t = {}
    param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT
    param_t.param1 = data:IsEquip() and 0 or 1
    param_t.param2 = data.in_bag == true and 1 or 0
    param_t.param3 = data.index
    param_t.param4 = 0
    self:SendOperateReqWithParamt(param_t)

    if data.param.stone[2] > 0 then 
        param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT
        param_t.param1 = data:IsEquip() and 0 or 1
        param_t.param2 = data.in_bag == true and 1 or 0
        param_t.param3 = data.index
        param_t.param4 = 1
        self:SendOperateReqWithParamt(param_t)
    end 
end

function WorkshopCtrl:ChangeEquip1(is_jewel, index, new_item_id)
    local param_t = {}
    param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_SWITCH_EQUIP_JEWELRY1
    param_t.param1 = is_jewel
    param_t.param2 = index
    param_t.param3 = new_item_id
    param_t.param4 = 0
    self:SendOperateReqWithParamt(param_t)
end
function WorkshopCtrl:ChangeEquip2(col, index, new_item_id)
    local param_t = {}
    param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_SWITCH_EQUIP_JEWELRY2
    param_t.param1 = col
    param_t.param2 = index
    param_t.param3 = new_item_id
    param_t.param4 = 0
    self:SendOperateReqWithParamt(param_t)
end

function WorkshopCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function WorkshopCtrl:UnlockDianHuaScheme(in_bag, index)
    local param_t = {}
    param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_OPEN_DIAN_HUA_PLAN_HOLE
    param_t.param1 = in_bag and 1 or 0
    param_t.param2 = index
    param_t.param3 = 0
    param_t.param4 = 0
    self:SendOperateReqWithParamt(param_t)
end
function WorkshopCtrl:UseDianHuaScheme(in_bag, index, seq)
    local param_t = {}
    param_t.op_type = WORLSHOP_OP.WORKSHOP_OP_SWITCH_DIAN_HUA_PLAN
    param_t.param1 = in_bag
    param_t.param2 = index
    param_t.param3 = seq
    param_t.param4 = 0
    self:SendOperateReqWithParamt(param_t)
end