FILE_REQ("modules/serveractivity/duanyangfuli/duanyangfuli_data")
VIEW_REQ("modules/serveractivity/duanyangfuli/duanyangfuli_view")
-- VIEW_REQ("modules/serveractivity/duanyangfuli/duanyangfuli_reward_view")
ActDuanYangFuLiCtrl = ActDuanYangFuLiCtrl or BaseClass(BaseCtrl)

ActDuanYangFuLiCtrl.REQ_TYPE = {
	REQ_INFO = 0,                   -- 0 请求数据信息
	REQ_BUY = 1,                    -- 1 购买
	REQ_GET_REWARD = 2,        -- 2 领取奖励
}

function ActDuanYangFuLiCtrl:__init()
	if ActDuanYangFuLiCtrl.Instance ~= nil then
		Debuger.LogError("[ActDuanYangFuLiCtrl] attempt to create singleton twice!")
		return
	end
	ActDuanYangFuLiCtrl.Instance = self
	self.data = ActDuanYangFuLiData.New()
	-- 注册协议
    self:RegisterAllProtocols()
end

function ActDuanYangFuLiCtrl:RegisterAllProtocols()
    -- self:RegisterProtocol(CSRADuanYangFuLiReq)
    self:RegisterProtocol(SCRADuanYangFuLiInfo, "OnSCRADuanYangFuLiInfo")
	--红点刷了的话调用那个函数
	Remind.Instance:Register(Mod.DragonBoat.DuanYangFuLi,self.data.remind_data, function () 
		return self.data:RemindNum() 
	end)
end



function ActDuanYangFuLiCtrl:SendDuanYangFuLiReq(req_type,param_1,param_2)
	LOG("端阳福礼请求：",req_type,param_1,param_2)
	ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_DUANYANGFULI, opera_type = req_type, param_1 = param_1 or 0,param_2 = param_2 or 0})
	-- local protocol = ProtocolPool.Instance:GetProtocol(CSRADuanYangFuLiReq)
	-- protocol.req_type = req_type or 0
	-- protocol.param_1 = param_1 or 0
	-- protocol.param_2 = param_2 or 0
	-- SendProtocol(protocol)
end

function ActDuanYangFuLiCtrl:OnSCRADuanYangFuLiInfo(protocol)
    self.data:SetDuanYangFuLiInfo(protocol)
end



function ActDuanYangFuLiCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActDuanYangFuLiCtrl.Instance = nil
end

function ActDuanYangFuLiCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActDuanYangFuLiCtrl:OnInit()
end


function ActDuanYangFuLiCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSRADuanYangFuLiReq)
    self:UnRegisterProtocol(SCRADuanYangFuLiInfo)
end