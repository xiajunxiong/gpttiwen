VIEW_REQ("modules/serveractivity/openserver/act_lucky_cow/act_lucky_cow_view")

-- 福牛报喜请求类型
ActLuckyCowReqType = {
    Buy = 0,        --购买
    GetUserInfo = 1,--获取获奖名单
}

-- 福牛报喜状态
ActLuckyCowState = {
    ActNotOpen = 0, --活动未开
    CanBuy = 1,     --可以购买
    BuyEnd = 2,     --购买结束
    Show = 3,       --公布获奖
    ActEnd = 4,     --活动结束
}

ActLuckyCowCtrl = ActLuckyCowCtrl or BaseClass(BaseCtrl)

function ActLuckyCowCtrl:__init()
	if ActLuckyCowCtrl.Instance ~= nil then
		Debuger.LogError("[ActLuckyCowCtrl] attempt to create singleton twice!")
		return
	end	
	ActLuckyCowCtrl.Instance = self
	self.data = ActLuckyCowData.New()
	self:RegisterAllProtocols()
end

function ActLuckyCowCtrl:__delete()
	self:UnRegisterAllProtocols()
	ActLuckyCowCtrl.Instance = nil
	Delete(self, "data")
end

function ActLuckyCowCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSCowReportGoodNewsClientReq)
    self:RegisterProtocol(SCCowReportGoodNewsRewardListInfo, "OnSCCowReportGoodNewsRewardListInfo")
    self:RegisterProtocol(SCCowReportGoodNewsInfo, "OnSCCowReportGoodNewsInfo")
    self:RegisterProtocol(SCCowReportGoodNewsRoleBuyInfo, "OnSCCowReportGoodNewsRoleBuyInfo")
end

function ActLuckyCowCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSCowReportGoodNewsClientReq)
	self:UnRegisterProtocol(SCCowReportGoodNewsRewardListInfo)
	self:UnRegisterProtocol(SCCowReportGoodNewsInfo)
	self:UnRegisterProtocol(SCCowReportGoodNewsRoleBuyInfo)
end

function ActLuckyCowCtrl:OnInit()

end

function ActLuckyCowCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function ActLuckyCowCtrl:SendReq(req_type)
    local protocol = ProtocolPool.Instance:GetProtocol(CSCowReportGoodNewsClientReq)
    protocol.req_type = req_type
    SendProtocol(protocol)
end

function ActLuckyCowCtrl:OnSCCowReportGoodNewsRewardListInfo(protocol)
    self.data:SetUserInfo(protocol)
end

function ActLuckyCowCtrl:OnSCCowReportGoodNewsInfo(protocol)
    self.data:SetStateInfo(protocol)
end

function ActLuckyCowCtrl:OnSCCowReportGoodNewsRoleBuyInfo(protocol)
    self.data:SetBuyInfo(protocol)
end


--------------------ActLuckyCowData--------------------
ActLuckyCowData = ActLuckyCowData or BaseClass()

function ActLuckyCowData:__init()
	if ActLuckyCowData.Instance ~= nil then
		Debuger.LogError("[ActLuckyCowData] attempt to create singleton twice!")
		return
	end
	ActLuckyCowData.Instance = self
    self.lucky_cow_data = SmartData.New()
    self:ResetLuckyCowData()
end

function ActLuckyCowData:LuckyCowIsOpen()
    if FunOpen.Instance:GetFunIsOpened(Mod.ServerActivity.LuckyCow) then
        return self.lucky_cow_data.state ~= ActLuckyCowState.ActNotOpen and self.lucky_cow_data.state ~= ActLuckyCowState.ActEnd
    end
    return false
end

function ActLuckyCowData:ResetData()
    self:ResetLuckyCowData()
end

function ActLuckyCowData:ResetLuckyCowData()
    self.lucky_cow_data:Set({
        is_buy = false,
        state = ActLuckyCowState.ActNotOpen,
        time = {},
        user_info = {},
    })
end

function ActLuckyCowData:SetUserInfo(protocol)
    self.lucky_cow_data.user_info = protocol.user_info
end

function ActLuckyCowData:SetStateInfo(protocol)
    self.lucky_cow_data.state = protocol.status
    self.lucky_cow_data.time = protocol.time
    -- 公布获奖期间如果没有名单数据就手动获取一次
    if protocol.status == ActLuckyCowState.Show and TableIsEmpty(self.lucky_cow_data.user_info) then
        ActLuckyCowCtrl.Instance:SendReq(ActLuckyCowReqType.GetUserInfo)
    end
    ActivityRandData.Instance:CheckRandOpenData()
    ServerActivityData.Instance:CheckOpenServerData()
end

function ActLuckyCowData:SetBuyInfo(protocol)
    self.lucky_cow_data.is_buy = protocol.is_buy == 1
end

function ActLuckyCowData:GetStateTime(state)
    state = state or self.lucky_cow_data.state
    return self.lucky_cow_data.time[state] and self.lucky_cow_data.time[state] or 0
end