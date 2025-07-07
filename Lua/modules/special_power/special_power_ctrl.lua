VIEW_REQ("modules/special_power/special_power_qq_view")
VIEW_REQ("modules/special_power/special_power_360_view")

SpecialPowerCtrl = SpecialPowerCtrl or BaseClass(BaseCtrl)
--特权大厅通用ctrl，360、腾讯特权大厅，所有PCsdk特权大厅的收发包都写在这里
function SpecialPowerCtrl:__init()
    if SpecialPowerCtrl.Instance ~= nil then
        Debuger.LogError("[SpecialPowerCtrl] attempt to create singleton twice!")
        return
    end
    SpecialPowerCtrl.Instance = self
    self.data = SpecialPowerData.New()
    self:RegisterProtocol(CSSpid360Req)
    self:RegisterProtocol(SCSpid360Info, "OnSCSpid360Info")
    self:RegisterProtocol(SCRATengXunDaTingInfo, "OnSCRATengXunDaTingInfo")
    EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.OnRoleNoticeComplete, self))
    FunOpen.Instance:SelfCondition(Mod.ServerActivity.SpecialPowerQQ,function()
        return QQBigPlayerCtrl.IsQQPC()
    end)
    FunOpen.Instance:SelfCondition(Mod.ServerActivity.SpecialPower360,function()
        if ChannelAgent.PCAgentType and ChannelAgent.PCAgentType() == "_360PC" then
            return true
        else
            return false
        end
    end)
    FunOpen.Instance:SelfCondition(Mod.ServerActivity.DoublePlat360,function()
        if ChannelAgent.PCAgentType and ChannelAgent.PCAgentType() == "_360PC" then
            return true
        else
            return false
        end
    end)
    FunOpen.Instance:SelfCondition(Mod.ServerActivity.SpecialPowerShunWang,function()
        if ChannelAgent.PCAgentType and ChannelAgent.PCAgentType() == "ShunWang" then
            return true
        else
            return false
        end
    end)
    FunOpen.Instance:SelfCondition(Mod.ServerActivity.DoublePlatShunWang,function()
        if ChannelAgent.PCAgentType and ChannelAgent.PCAgentType() == "ShunWang" then
            return true
        else
            return false
        end
    end)
end

function SpecialPowerCtrl:OnRoleNoticeComplete()
    if QQBigPlayerCtrl.IsQQPC() then
        self:SendQQSpecialReq(0)
    elseif GLOBAL_CONFIG:PkgInfo().agent_id == "pa1" or GLOBAL_CONFIG:PkgInfo().agent_id == "ps1" then
        self:Send360SpecialReq(0, 1)
    elseif GLOBAL_CONFIG:PkgInfo().agent_id == "pa3" or GLOBAL_CONFIG:PkgInfo().agent_id == "ps2" then
        self:Send360SpecialReq(0, 0)
    end
end
function SpecialPowerCtrl:OnSCSpid360Info(protocol)
    self.data:Set360SpecialInfo(protocol)
end

function SpecialPowerCtrl:OnSCRATengXunDaTingInfo(protocol)
    self.data:SetQQSpecialInfo(protocol)
end
--op_type 0、请求下发 1、领取特权奖励 2、领取每日特权奖励 3、领取等级奖励，p1 reward_seq
function SpecialPowerCtrl:SendQQSpecialReq(op_type, p1)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_TX_TeQuanDaTing
    param_t.opera_type = op_type
    param_t.param_1 = p1 or 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

--req_type 0、上报登录平台login_flag p1:0安卓 1pc， 1、领取奖励，fetch_flag p1:奖励seq，0:每日 1特权 2双端
function SpecialPowerCtrl:Send360SpecialReq(req_type, p1)
    local protocol = GetProtocol(CSSpid360Req)
    protocol.req_type = req_type
    protocol.param1 = p1
    protocol.spid = GLOBAL_CONFIG:PkgInfo().agent_id
    SendProtocol(protocol)
end

-----------------------------------------------------
SpecialPowerData = SpecialPowerData or BaseClass()
SpecialPowerData.ErWeiMaName = {
    pa1 = "double_plat_360",
    ps1 = "double_plat_shunwang",
}

SpecialPowerData.TipsIndex = {
    pa1 = 1,
    ps1 = 2,
}
function SpecialPowerData:__init()
    if SpecialPowerData.Instance ~= nil then
        Debuger.LogError("[SpecialPowerData] attempt to create singleton twice!")
        return
    end
    SpecialPowerData.Instance = self
    self.sm_data = SmartData.New({view_index = 1})
    self.is_fetch_reward = {[0]=0}
    self.is_fetch_day_reward = {[0]=0}
    self.level_reward_fetch_flag = {}
end

function SpecialPowerData:SetIndex(index)
    self.sm_data.view_index = index
end

function SpecialPowerData:Set360SpecialInfo(protocol)
    self.is_360_fetch_reward = protocol.fetch_flag
    self.is_360_login_flag = protocol.login_flag
    self.sm_data:SetDirty("view_index")
end

function SpecialPowerData:SetQQSpecialInfo(protocol)
    self.is_fetch_reward = protocol.is_fetch_reward
    self.is_fetch_day_reward = protocol.is_fetch_day_reward
    self.level_reward_fetch_flag = protocol.level_reward_fetch_flag
    self.sm_data:SetDirty("view_index")
end

function SpecialPowerData:GetQQSpecialLevelReward()
    local t = {}
    for _ , v in pairs(Config.txtequan_auto.level_reward) do
        local level_t = {item_list = {}}
        for _ , value in pairs(v.reward_item) do
            table.insert(level_t.item_list, Item.Init(value.item_id, value.num, value.is_bind))
        end
        level_t.level = v.level
        level_t.seq = v.seq
        level_t.get_state = self.level_reward_fetch_flag[v.seq]
        table.insert(t,level_t)
    end
    table.sort(t, function (a,b)
        if self.level_reward_fetch_flag[a.seq] == 0 then
            if self.level_reward_fetch_flag[b.seq] == 0 then
                return a.seq < b.seq
            else
                return true
            end
        else
            if self.level_reward_fetch_flag[b.seq] == 0 then
                return false
            else
                return a.seq < b.seq
            end
        end
    end)
    return t
end

function SpecialPowerData:GetQQSpecialReward()
    local reward_items = {}
    local base_t = self.sm_data.view_index == 1 and Config.txtequan_auto.other[1].privilege_reward_item or Config.txtequan_auto.other[1].day_reward_item
    for k,v in pairs(base_t) do
        if k == 0 then
            table.insert(reward_items, 1, Item.Init(v.item_id,v.num,v.is_bind))
        else
            table.insert(reward_items, Item.Init(v.item_id,v.num,v.is_bind))
        end
        
    end
    return reward_items
end

function SpecialPowerData:CanGetQQReward()
    if self.sm_data.view_index == 1 then
        return self.is_fetch_reward[0] == 0
    else
        return self.is_fetch_day_reward[0] == 0
    end
end

function SpecialPowerData:Get360SpecialReward()
    local reward_items = {}
    local base_config = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
    if not TableIsEmpty(base_config) then
        local base_t = self.sm_data.view_index == 1 and base_config.special_reward_1 or base_config.special_reward_0
        for _, v in pairs(base_t) do
            table.insert(reward_items, Item.Init(v.item_id, v.num, v.is_bind))
        end
    end
    return reward_items
end

function SpecialPowerData:Get360DoublePlatReward()
    local reward_items = {}
    local base_config = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
    if not TableIsEmpty(base_config) then
        for _, v in pairs(base_config.special_reward_2) do
            table.insert(reward_items, Item.Init(v.item_id, v.num, v.is_bind))
        end
    end
    return reward_items
end

function SpecialPowerData:CanGet360Reward()
    local reward_seq = self.sm_data.view_index == 1 and 1 or 0
    return self.is_360_fetch_reward[reward_seq] == 0
end

function SpecialPowerData:DoublePlatRewardState360()
    if self.is_360_fetch_reward[2] == 1 then
        return 1, Language.SpecialPower.HasGet
    elseif self.is_360_login_flag[0] == 0 or self.is_360_login_flag[1] == 0 then
        return 2, Language.SpecialPower.NotLoginBothPlat
    end
    return 0
end