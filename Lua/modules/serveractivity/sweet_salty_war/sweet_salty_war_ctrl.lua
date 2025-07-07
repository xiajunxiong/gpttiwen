VIEW_REQ("modules/serveractivity/sweet_salty_war/sweet_salty_war_view")
-- 甜咸之争
SweetSaltyWarCtrl = SweetSaltyWarCtrl or BaseClass(BaseCtrl)

function SweetSaltyWarCtrl:__init()
    if SweetSaltyWarCtrl.Instance ~= nil then
        Debuger.LogError("[SweetSaltyWarCtrl] attempt to create singleton twice!")
        return
    end
    SweetSaltyWarCtrl.Instance = self
    self.data = SweetSaltyWarData.New()
end

function SweetSaltyWarCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function SweetSaltyWarCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.DragonBoat.SweetSaltyWar,
        self.data.info_data,
        remind_func
    )
    BagData.Instance:ListenItems({self.data:ConsumeItem()}, function()
        self.data.info_data:Notify()
    end)
    FunOpen.Instance:SelfCondition(Mod.DragonBoat.SweetSaltyWar,function ()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR), Language.Activity.NotOpenTip
    end)
end

function SweetSaltyWarCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRASaltySweetBattleInfo, "OnSCRASaltySweetBattleInfo")
    self:RegisterProtocol(SCRASaltySweetBattleRoleInfo, "OnSCRASaltySweetBattleRoleInfo")
end

function SweetSaltyWarCtrl:__delete()
    if SweetSaltyWarCtrl.Instance == self then
        SweetSaltyWarCtrl.Instance = nil
    end
end

function SweetSaltyWarCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function SweetSaltyWarCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function SweetSaltyWarCtrl:SendReq(req_type, param1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR, req_type, param1, 1)
end

function SweetSaltyWarCtrl:OnSCRASaltySweetBattleInfo(protocol)
    self.data:SetInfo(protocol)
end

function SweetSaltyWarCtrl:OnSCRASaltySweetBattleRoleInfo(protocol)
    self.data:SetRoleInfo(protocol)
end

-------------------------SweetSaltyWarData-------------------------
SweetSaltyWarData = SweetSaltyWarData or BaseClass()

-- 请求类型
SweetSaltyWarReq = {
    Info = 0,
    Vote = 1,   --投票，p1：投票类型，p2：票数
    Reward = 2, --领取结束奖励
}

-- 投票类型
SweetSaltyWarVote = {
    None = 0,
    Salty = 1,
    Sweet = 2,
}

-- 活动状态
SweetSaltyWarState = {
    Vote = 0,   --投票中
    Count = 1,  --结算中
    Result = 2, --结束
}

function SweetSaltyWarData:__init()
    if SweetSaltyWarData.Instance ~= nil then
        Debuger.LogError("[SweetSaltyWarData] attempt to create singleton twice!")
        return
    end
    SweetSaltyWarData.Instance = self
    self.config = Config.tianxianzhizheng_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    
    self:ResetData()
    self:InitData()
end

function SweetSaltyWarData:__delete()
    SweetSaltyWarData.Instance = nil
end

function SweetSaltyWarData:InitData()
    
end

function SweetSaltyWarData:ResetData()
    local info = {}
    info.value = {}
    info.value[1] = 0    --咸的票数
    info.value[2] = 0    --甜的票数
    info.value[0] = 0    --总票数
    info.state = SweetSaltyWarState.Result          --投票状态
    self.info_data.info = info
    self.info_data.select_type = SweetSaltyWarVote.None  --当前阵营
    self.info_data.reward_flag = 1  --是否领取奖励标志位
end

function SweetSaltyWarData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR) or self.info_data.info.state == SweetSaltyWarState.Count then
        return 0
    end
    if self:SingleRemind(SweetSaltyWarVote.Sweet) > 0 then
        return 1
    elseif self:SingleRemind(SweetSaltyWarVote.Salty) > 0 then
        return 1
    end
    return 0
end

function SweetSaltyWarData:SingleRemind(select_type)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR) or self.info_data.info.state == SweetSaltyWarState.Count then
        return 0
    end
    if self.info_data.info.state == SweetSaltyWarState.Vote then
        if self.info_data.select_type == 0 or self.info_data.select_type == select_type then
            local has_num = Item.GetNum(self:ConsumeItem())
            return has_num > 0 and 1 or 0
        end
    elseif self.info_data.info.state == SweetSaltyWarState.Result then
        if self.info_data.select_type == select_type and self.info_data.reward_flag == 0 then
            return 1
        end
    end
    return 0
end

function SweetSaltyWarData:SetInfo(protocol)
    self.info_data.info = protocol.info
end

function SweetSaltyWarData:SetRoleInfo(protocol)
    self.info_data.select_type = protocol.select_type
    self.info_data.reward_flag = protocol.reward_flag
end

function SweetSaltyWarData:ConsumeItem()
    return self.other.consume_item
end

function SweetSaltyWarData:TopDesc()
    return self.other.activity_desc
end

function SweetSaltyWarData:BaseCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SWEET_SALTY_WAR, self.config.basis_configuration)
    return cfg[1]
end