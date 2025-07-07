VIEW_REQ("modules/serveractivity/openserver/act_lucky_tiger/act_lucky_tiger_view")

-- 福牛报喜请求类型
ActLuckyTigerReqType = {
    Buy = 0,        --购买
    GetUserInfo = 1,--获取获奖名单
}

-- 福牛报喜状态
ActLuckyTigerState = {
    ActNotOpen = 0, --活动未开
    CanBuy = 1,     --可以购买
    BuyEnd = 2,     --购买结束
    Show = 3,       --公布获奖
    ActEnd = 4,     --活动结束
}

ActLuckyTigerCtrl = ActLuckyTigerCtrl or BaseClass(BaseCtrl)

function ActLuckyTigerCtrl:__init()
	if ActLuckyTigerCtrl.Instance ~= nil then
		Debuger.LogError("[ActLuckyTigerCtrl] attempt to create singleton twice!")
		return
	end	
	ActLuckyTigerCtrl.Instance = self
	self.data = ActLuckyTigerData.New()
	self:RegisterAllProtocols()
end

function ActLuckyTigerCtrl:__delete()
	self:UnRegisterAllProtocols()
	ActLuckyTigerCtrl.Instance = nil
	Delete(self, "data")
end

function ActLuckyTigerCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTigerBringsBlessingClientReq)
    self:RegisterProtocol(SCTigerBringsBlessingRewardListInfo, "OnSCTigerBringsBlessingRewardListInfo")
    self:RegisterProtocol(SCTigerBringsBlessingInfo, "OnSCTigerBringsBlessingInfo")
    self:RegisterProtocol(SCTigerBringsBlessingRoleBuyInfo, "OnSCTigerBringsBlessingRoleBuyInfo")
end

function ActLuckyTigerCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSTigerBringsBlessingClientReq)
	self:UnRegisterProtocol(SCTigerBringsBlessingRewardListInfo)
	self:UnRegisterProtocol(SCTigerBringsBlessingInfo)
	self:UnRegisterProtocol(SCTigerBringsBlessingRoleBuyInfo)
end

function ActLuckyTigerCtrl:OnInit()

end

function ActLuckyTigerCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function ActLuckyTigerCtrl:SendReq(req_type)
    local protocol = ProtocolPool.Instance:GetProtocol(CSTigerBringsBlessingClientReq)
    protocol.req_type = req_type
    SendProtocol(protocol)
end

function ActLuckyTigerCtrl:OnSCTigerBringsBlessingRewardListInfo(protocol)
    self.data:SetUserInfo(protocol)
end

function ActLuckyTigerCtrl:OnSCTigerBringsBlessingInfo(protocol)
    self.data:SetStateInfo(protocol)
end

function ActLuckyTigerCtrl:OnSCTigerBringsBlessingRoleBuyInfo(protocol)
    self.data:SetBuyInfo(protocol)
end


--------------------ActLuckyTigerData--------------------
ActLuckyTigerData = ActLuckyTigerData or BaseClass()

function ActLuckyTigerData:__init()
	if ActLuckyTigerData.Instance ~= nil then
		Debuger.LogError("[ActLuckyTigerData] attempt to create singleton twice!")
		return
	end
	ActLuckyTigerData.Instance = self
    self.lucky_tiger_data = SmartData.New()
    self:ResetLuckyTigerData()
end

function ActLuckyTigerData:LuckyTigerIsOpen()
    if FunOpen.Instance:IsFunOpen(Mod.ServerActivity.LuckyTiger) then
        return self.lucky_tiger_data.state ~= ActLuckyTigerState.ActNotOpen and self.lucky_tiger_data.state ~= ActLuckyTigerState.ActEnd
    end
    return false
end

function ActLuckyTigerData:ResetData()
    self:ResetLuckyTigerData()
end

function ActLuckyTigerData:ResetLuckyTigerData()
    self.lucky_tiger_data:Set({
        is_buy = false,
        state = ActLuckyTigerState.ActNotOpen,
        time = {},
        user_info = {},
    })
end

function ActLuckyTigerData:SetUserInfo(protocol)
    self.lucky_tiger_data.user_info = protocol.user_info
end

function ActLuckyTigerData:SetStateInfo(protocol)
    self.lucky_tiger_data.state = protocol.status
    self.lucky_tiger_data.time = protocol.time
    -- 公布获奖期间如果没有名单数据就手动获取一次
    if protocol.status == ActLuckyTigerState.Show and TableIsEmpty(self.lucky_tiger_data.user_info) then
        ActLuckyTigerCtrl.Instance:SendReq(ActLuckyTigerReqType.GetUserInfo)
    end
    ActivityRandData.Instance:CheckRandOpenData()
    ServerActivityData.Instance:CheckOpenServerData()
end

function ActLuckyTigerData:SetBuyInfo(protocol)
    self.lucky_tiger_data.is_buy = protocol.is_buy == 1
end

function ActLuckyTigerData:GetStateTime(state)
    state = state or self.lucky_tiger_data.state
    return self.lucky_tiger_data.time[state] and self.lucky_tiger_data.time[state] or 0
end