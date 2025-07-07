FILE_REQ("modules/equip/equip_data")

VIEW_REQ("modules/equip/equip_view")
VIEW_REQ("modules/equip/equip_spirit_view")
VIEW_REQ("modules/equip/spirit_resonance_view")
VIEW_REQ("modules/equip/small_spirit_resonance_view")
VIEW_REQ("modules/equip/spirit_level_view")
VIEW_REQ("modules/equip/spirit_level_view")
VIEW_REQ("modules/equip/spirit_resolve_view")
VIEW_REQ("modules/equip/spirit_quick_buy_view")
VIEW_REQ("modules/equip/equip_conversion")

-- 铸灵请求参数
SPIRIT_REQ_TYPE = {
	REQ_TYPE_LEVEL = 0,		--升级
	REQ_TYPE_PUT_ON = 1,	--穿上
	REQ_TYPE_TAKE_OFF = 2,	--脱下
}

-- 铸灵请求SRC参数
SPIRIT_SRC_TYPE = {
	SRC_TYPE_BAG = 0,		--背包里
	SRC_TYPE_EQUIP = 1,		--装备里
	SRC_TYPE_JEWELRY = 2,	--灵饰里
}

EquipCtrl = EquipCtrl or BaseClass(BaseCtrl)

function EquipCtrl:__init()
    if EquipCtrl.Instance ~= nil then
        Debuger.LogError("[EquipCtrl] attempt to create singleton twice!")
        return
    end
    EquipCtrl.Instance = self
    self.data = EquipData.New()

    self:OnRemind()
    self:RegisterAllProtocols()
end

function EquipCtrl:OnInit()
    FunOpen.Instance:Register(Mod.Equip.Smelt, BindTool.Bind(self.SmeltFunCall, self))
end

function EquipCtrl:OnRemind()
    -- 涉及多个监听红点的情况下需要单独开一个绑定，防止重复注册
    -- 装备强化红点
    local upgrade_func = BindTool.Bind(self.FlushUpgradeRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Upgrade,
        BagData.Instance.item_grid,
        upgrade_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Upgrade,
        BagData.Instance.equip_sort_upgrade,
        upgrade_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Upgrade,
        WorkshopData.Instance.upgrade_remind_sort,
        upgrade_func
    )
    -- 装备镶嵌红点
    local stone_func = BindTool.Bind(self.FlushStoneRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Inser,
        BagData.Instance.item_grid,
        stone_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Inser,
        RoleData.Instance.base_data,
        stone_func
    )
    -- 装备熔炼红点
    local flush_func = BindTool.Bind(self.FlushSmeltRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Smelt,
        BagData.Instance.item_grid,
        flush_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Smelt,
        BagData.Instance.jewelry_list,
        flush_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Smelt,
        BagData.Instance.equipment_list,
        flush_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Smelt,
        BagData.Instance.equipment_voc_list,
        flush_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Smelt,
        WorkshopData.Instance.smelt_func_open,
        flush_func
    )
    -- 装备铸灵红点
    local spirit_func = BindTool.Bind(self.FlushSpiritRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Spirit,
        BagData.Instance:GetColumnGrids(ItemColumnType.Item),
        spirit_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Spirit,
        RoleData.Instance.base_data,
        spirit_func,
        "level"
    )
    
    -- 点化红点
    local neveal_func = BindTool.Bind(self.FlushNevealRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Neveal,
        BagData.Instance.item_grid,
        neveal_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.Equip.Neveal,
        BagData.Instance.equipment_list,
        neveal_func
    )
end

function EquipCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSZhuLingReq)
    self:RegisterProtocol(CSZhuLingDecomposeInfo)
    self:RegisterProtocol(CSZhuLingUpLevelReq)
    self:RegisterProtocol(SCZhuLingChangeInfo, "GetSpiritChangeInfo")
    self:RegisterProtocol(SCZhuLingDecomposeInfo, "GetSpiritResolveInfo")
    self:RegisterProtocol(SCZhuLingPutOnSuccNotice, "GetEquipNoticeInfo")
end

function EquipCtrl:SmeltFunCall(opened)
    if opened then 
        local role_id = RoleData.Instance:GetBaseData().role_id
        if UnityPlayerPrefs.GetInt(PrefKeys.SmeltFunOpenMark(role_id)) == 0 then
            UnityPlayerPrefs.SetInt(PrefKeys.SmeltFunOpenMark(role_id), 1)
            WorkshopData.Instance:SetIsFunRed(true)
        end
    end 
end

function EquipCtrl:__delete()
    RemindCtrl.Instance:Remind():UnRegister(Mod.Equip.Upgrade)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Equip.Inser)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Equip.Smelt)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Equip.Spirit)
end

function EquipCtrl:FlushUpgradeRemind()
    local num = WorkshopData.Instance:CheckUpgradeRemind()
    return num 
end

function EquipCtrl:FlushStoneRemind()
    local num = WorkshopData.Instance:CheckWorkInset()
    return num
end

function EquipCtrl:FlushSmeltRemind()
    local num = WorkshopData.Instance:CheckNeoSmeltAttr()
    return num
end

function EquipCtrl:FlushSpiritRemind()
    local num = self.data:GetSpiritRemind()
    return num
end

function EquipCtrl:OnUnloadGameLogic()
    self.data:ResetAllData()
end

-- 发送铸灵升级（已废弃），穿戴，卸下，替换请求
-- p1 item_src, p2 index, p3 column, p4 slot_index
-- p5 item_src, p6 index, p7 column, p8 slot_index
function EquipCtrl:SendSpiritReq(param_t)
    local protocol = ProtocolPool.Instance:GetProtocol(CSZhuLingReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param1 = param_t.param1 or 0
    protocol.param2 = param_t.param2 or 0
    protocol.param3 = param_t.param3 or 0
    protocol.param4 = param_t.param4 or 0
    protocol.param5 = param_t.param5 or 0
    protocol.param6 = param_t.param6 or 0
    protocol.param7 = param_t.param7 or 0
    protocol.param8 = param_t.param8 or 0
	SendProtocol(protocol)
end

-- 发送分解铸灵请求
function EquipCtrl:SendSpiritResolveReq(param_t)
    if TableIsEmpty(param_t) then
        return
    end
    local protocol = ProtocolPool.Instance:GetProtocol(CSZhuLingDecomposeInfo)
    protocol.item_list = param_t
    SendProtocol(protocol)
end

-- 废弃
function EquipCtrl:GetSpiritChangeInfo(protocol)
    -- self.data:GetSpiritChangeInfo(protocol)
end

function EquipCtrl:GetSpiritResolveInfo(protocol)
    self.data:GetSpiritResolveInfo(protocol)
end

-- 两个装备同时有铸灵或者宝石时，不交换的弹窗
function EquipCtrl:OpenNotAutoExchangeTip()
    local dialog_info = {
        content = Language.Equip.NotAutoExchangeSpiritTip,
        confirm = {
            func = function()
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        new_tnr = DTNewTodayNotRemind.NotAutoExchangeSpirit,
        no_cancel = true,
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

-- 发送铸灵升级请求
function EquipCtrl:SendSpiritLevelUpReq(param_t)
    local protocol = ProtocolPool.Instance:GetProtocol(CSZhuLingUpLevelReq)
    protocol.item_src = param_t.item_src or 0
    protocol.index = param_t.index or 0
    protocol.column = param_t.column or 0
    protocol.slot_index = param_t.slot_index or 0
    protocol.need_buy_num = param_t.need_buy_num or 0
    protocol.now_level = param_t.now_level or 0
    protocol.item_list = param_t.item_list or {}
    SendProtocol(protocol)
end

-- 替换装备时的返回
function EquipCtrl:GetEquipNoticeInfo(protocol)
    self.data:GetEquipNoticeInfo(protocol)
end

function EquipCtrl:FlushNevealRemind()
    local num = WorkshopData.Instance:CheckBlazeStarRemind()
    return num 
end