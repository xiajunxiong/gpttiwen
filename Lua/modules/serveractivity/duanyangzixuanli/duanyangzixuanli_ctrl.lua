FILE_REQ("modules/serveractivity/duanyangzixuanli/duanyangzixuanli_data")
VIEW_REQ("modules/serveractivity/duanyangzixuanli/duanyangzixuanli_view")

ActDuanYangZiXuanLiCtrl = ActDuanYangZiXuanLiCtrl or BaseClass(BaseCtrl)

-- 请求类型
ActDuanYangZiXuanLiCtrl.REQ_TYPE = {
	REQ_INFO = 0,                   -- 0 请求数据信息
	REQ_BUY = 1,                    -- 1 购买
	REQ_GET_REWARD = 2,        -- 2 领取奖励
}

-- 初始化
function ActDuanYangZiXuanLiCtrl:__init()
	if ActDuanYangZiXuanLiCtrl.Instance ~= nil then
		Debuger.LogError("[ActDuanYangZiXuanLiCtrl] attempt to create singleton twice!")
		return
	end
	ActDuanYangZiXuanLiCtrl.Instance = self
	self.data = ActDuanYangZiXuanLiData.New()




	-- 注册协议
	-- self:RegisterProtocol(SCRADuanYangZiXuanLiInfo, "OnSCRADuanYangZiXuanLiInfo")
    self:RegisterAllProtocols()
end

-- 删除
function ActDuanYangZiXuanLiCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActDuanYangZiXuanLiCtrl.Instance = nil
end

function ActDuanYangZiXuanLiCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRADuanYangZiXuanLiInfo, "OnSCRADuanYangZiXuanLiInfo")


	--红点刷了的话调用那个函数
	Remind.Instance:Register(Mod.DragonBoat.DuanYangZiXuanLi,self.data.remind_data, function () 
        return self.data:RemindNum() 
    end)

	
    -- self:RegisterProtocol(CSRADuanYangZiXuanLiReq)
    -- self:RegisterProtocol(SCRADuanYangZiXuanLiInfo, "OnRADuanYangZiXuanLiInfo")
end

-- 发送请求
function ActDuanYangZiXuanLiCtrl:SendDuanYangZiXuanLiReq(req_type,param_1,param_2)
	LOG("端阳自选礼请求：",req_type,param_1,param_2)
	ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, opera_type = req_type, param_1 = param_1 or 0,param_2 = param_2 or 0})
	-- local protocol = ProtocolPool.Instance:GetProtocol(CSRADuanYangZiXuanLiReq)
	-- protocol.req_type = req_type or 0
	-- protocol.param_1 = param_1 or 0
	-- protocol.param_2 = param_2 or 0
	-- SendProtocol(protocol)
end

-- 接受信息
function ActDuanYangZiXuanLiCtrl:OnSCRADuanYangZiXuanLiInfo(protocol)
	-- 接受协议后data设置信息
    self.data:SetDuanYangZiXuanLiInfo(protocol)

end


















function ActDuanYangZiXuanLiCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActDuanYangZiXuanLiCtrl:OnInit()
	-- self.data:initConfig()
end



function ActDuanYangZiXuanLiCtrl:UnRegisterAllProtocols()
    -- self:UnRegisterProtocol(CSRADuanYangZiXuanLiReq)
    -- self:UnRegisterProtocol(SCRADuanYangZiXuanLiInfo)
end