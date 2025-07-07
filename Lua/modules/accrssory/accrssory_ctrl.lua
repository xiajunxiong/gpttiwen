FILE_REQ("modules/accrssory/accrssory_data")
FILE_REQ("modules/accrssory/accrssory")

VIEW_REQ("modules/accrssory/accrssory_view")
VIEW_REQ("modules/accrssory/accrssory_main_view")
VIEW_REQ("modules/accrssory/accrssory_inherit_view")
VIEW_REQ("modules/accrssory/accrssory_destiny_view")
VIEW_REQ("modules/accrssory/accrssory_illustration_view")
VIEW_REQ("modules/accrssory/accrssory_resolve_view")
VIEW_REQ("modules/accrssory/accrssory_skill_show_view")
VIEW_REQ("modules/accrssory/accrssory_select_view")
VIEW_REQ("modules/accrssory/accrssory_strength_view")
VIEW_REQ("modules/accrssory/accrssory_change_view")
VIEW_REQ("modules/accrssory/accrssory_item_info")
VIEW_REQ("modules/accrssory/accrssory_extral_view")

AccrssoryCtrl = AccrssoryCtrl or BaseClass(BaseCtrl)

function AccrssoryCtrl:__init()
    if AccrssoryCtrl.Instance ~= nil then
        Debuger.LogError("[AccrssoryCtrl] attempt to create singleton twice!")
        return
    end
    AccrssoryCtrl.Instance = self
    self.data = AccrssoryData.New()
    self:OnRemind()
    self:RegisterAllProtocols()
end

function AccrssoryCtrl:OnRemind()
    BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory):Care(function()
        self.data.remind_data:Notify()
        BagData.Instance.crystal_red_piont:Notify()
    end)
    -- 配饰界面红点
    local main_func = BindTool.Bind(self.FlushMainRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Accrssory.Main,
        self.data.remind_data,
        main_func
    )
    -- 配饰继承界面红点
    local inherit_func = BindTool.Bind(self.FlushInheritRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Accrssory.Inherit,
        self.data.remind_data,
        inherit_func
    )
    -- 配饰强化界面红点
    local strength_func = BindTool.Bind(self.FlushStrengthRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.Accrssory.Strength,
        self.data.remind_data,
        strength_func
    )
    -- 等级改变或者转职的时候会去算一遍推荐套装和技能
    self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SetRecommendSuit, self))
    self.level_handle = EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.LevelChange, self))
    self.prof_change_handle = EventSys:Bind(GlobalEventKeys.RoleProfessionChange, BindTool.Bind(self.SetRecommendSuit, self))
end

function AccrssoryCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSAccessoriesMentOperaReq)
    self:RegisterProtocol(SCAccessoriesMentInfo, "SetAllAccrssoryInfo")
    self:RegisterProtocol(SCAccessoriesMentChange, "SetAccrssoryInfo")
    self:RegisterProtocol(CSAccessoriesMentVocComposeReq)
    self:RegisterProtocol(SCAccessoriesSpecialEquip, "OnAccessoriesSpecialEquip")

    self:RegisterProtocol(SCTianMingInfo, "OnTianMingInfo")
end

function AccrssoryCtrl:__delete()
    --注销网络事件
    self:UnRegisterProtocol(CSAccessoriesMentOperaReq)
    self:UnRegisterProtocol(SCAccessoriesMentInfo)
    self:UnRegisterProtocol(SCAccessoriesMentChange)
    self:UnRegisterProtocol(CSAccessoriesMentVocComposeReq)
    self:UnRegisterProtocol(SCAccessoriesSpecialEquip)
    
    self:UnRegisterProtocol(SCTianMingInfo)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Accrssory.Main)
    RemindCtrl.Instance:Remind():UnRegister(Mod.Accrssory.Inherit)
    EventSys:UnBind(self.login_handle)
    EventSys:UnBind(self.level_handle)
    EventSys:UnBind(self.prof_change_handle)
    if AccrssoryCtrl.Instance == self then
        AccrssoryCtrl.Instance = nil
    end
end

function AccrssoryCtrl:OnUnloadGameLogic()
    
end

-- 打开技能展示面板
function AccrssoryCtrl:OpenSkillShowView(skill_config)
    ViewMgr:OpenView(AccrssorySkillShowView, {skill_config = skill_config})
end

function AccrssoryCtrl:OnTianMingInfo(protocol)
	self.data:SetTianMingInfo(protocol)
end

-- 配饰操作请求（穿戴、卸下、分解、置换）
function AccrssoryCtrl:SendAccrssoryReq(param_t)
    -- LogError("健哥哥，协议要请求了！",param_t)
    local protocol = ProtocolPool.Instance:GetProtocol(CSAccessoriesMentOperaReq)
    protocol.req_type = param_t.req_type or 0
    protocol.param_1 = param_t.param_1 or ItemColumnType.Accrssory
    protocol.param_2 = param_t.param_2 or 1
    protocol.param_3 = param_t.param_3 or 1
    protocol.param_4 = param_t.param_4 or 0
    SendProtocol(protocol)
end

function AccrssoryCtrl:SetAllAccrssoryInfo(protocol)
    self.data:SetAllAccrssoryInfo(protocol)
end

function AccrssoryCtrl:SetAccrssoryInfo(protocol)
    self.data:SetAccrssoryInfo(protocol)
end

function AccrssoryCtrl:OnAccessoriesSpecialEquip(protocol)
    -- LogError("?健哥哥 这是仙饰的下发哦！!",protocol)
    self.data:SetAccrssoryExtral(protocol)
end

-- 配饰词条继承
-- is_wear 继承者是否是已穿戴的
-- index 如果继承者是已穿戴的，那这里就是1~5，否则就是背包的index
-- replace_index 被继承者在背包中的index
-- replace_list 需要替换的词条的数组，每一组格式{attr_index = -1, replace_attr_index = -1}
function AccrssoryCtrl:SendInheritReq(param_t)
    local protocol = ProtocolPool.Instance:GetProtocol(CSAccessoriesMentVocComposeReq)
    protocol.is_wear = param_t.is_wear or 1
    protocol.index = param_t.index or 0
    protocol.replace_index = param_t.replace_index or 0
    protocol.replace_list = param_t.replace_list or {}
    SendProtocol(protocol)
end

function AccrssoryCtrl:FlushMainRemind()
    local num = self.data:CheckMainRemind()
    return num
end

function AccrssoryCtrl:FlushInheritRemind()
    local num = self.data:CheckInheritRemind()
    return num
end

function AccrssoryCtrl:FlushStrengthRemind()
    local num = self.data:CheckStrengthRemind()
    return num
end

-- 获取配饰系统红点
function AccrssoryCtrl:GetAccrssoryRemind()
    if self:FlushMainRemind() > 0 then
        return 1
    elseif self:FlushInheritRemind() > 0 then
        return 1
    elseif self:FlushStrengthRemind() > 0 then
        return 1
    end
    return 0
end

-- 重算推荐套装后
function AccrssoryCtrl:SetRecommendSuit()
    self.data:SetRecommendSuit()
end

-- 等级改变后需要重算红点
function AccrssoryCtrl:LevelChange()
    self.data:SetRecommendSuit()
    self.data.remind_data:Notify()
end