FILE_REQ("modules/serveractivity/meiliqixi/meiliqixi_data")
VIEW_REQ("modules/serveractivity/meiliqixi/meiliqixi_view")

ActMeiLiQiXiCtrl = ActMeiLiQiXiCtrl or BaseClass(BaseCtrl)


ActMeiLiQiXiCtrl.REQ_TYPE = {
	REQ_INFO = 0,                   -- 0 请求数据信息
	REQ_GET_REWARD = 1,        -- 1 领取奖励
}


function ActMeiLiQiXiCtrl:__init()
	if ActMeiLiQiXiCtrl.Instance ~= nil then
		Debuger.LogError("[ActMeiLiQiXiCtrl] attempt to create singleton twice!")
		return
	end
	ActMeiLiQiXiCtrl.Instance = self
	self.data = ActMeiLiQiXiData.New()
    self:RegisterAllProtocols()

	local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.QixiFestival.MeiLiQiXi,
        self.data.meiliqixi_smart,
        remind_func
    )
end
function ActMeiLiQiXiCtrl:FlushRemind()
    return self.data:Remind()
end

--这里注册协议 请求协议 不需要带回调 如下 
function ActMeiLiQiXiCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAMeiLiQiXiInfo, "OnSCRAMeiLiQiXiInfo")

end

function ActMeiLiQiXiCtrl:SendMeiLiQiXiReq(req_type,param_1,param_2)
	-- LOG("魅力七夕请求：",req_type,param_1,param_2)
	ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_MEILIQIXI, opera_type = req_type, param_1 = param_1 or 0,param_2 = param_2 or 0})
	-- local protocol = ProtocolPool.Instance:GetProtocol(CSRAMeiLiQiXiReq)
	-- protocol.req_type = req_type or 0
	-- protocol.param_1 = param_1 or 0
	-- protocol.param_2 = param_2 or 0
	-- SendProtocol(protocol)
end

function ActMeiLiQiXiCtrl:SendMeiLiQiXiPersonRankReq()
	-- LOG("魅力七夕获得原服排行榜请求：")
	SocietyCtrl.Instance:SendGetPersonRankList(self.data.RANK_TYPE,49,0)

end

function ActMeiLiQiXiCtrl:SendCrossMeiLiQiXiPersonRankReq()
	--LogError("魅力七夕获得跨服排行榜请求：")
	SocietyCtrl.Instance:SendCrossGetPersonRankList(self.data.RANK_TYPE,nil,49)
end


function ActMeiLiQiXiCtrl:OnSCRAMeiLiQiXiInfo(protocol)
    self.data:SetMeiLiQiXiInfo(protocol)
end

function ActMeiLiQiXiCtrl:OnGetMeiLiBangRankList(protocol)
    self.data:SetMeiLiBangRankList(protocol)
end

function ActMeiLiQiXiCtrl:OnGetCrossMeiLiBangRankList(protocol)
    self.data:SetMeiLiBangRankList(protocol)
end

function ActMeiLiQiXiCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActMeiLiQiXiCtrl.Instance = nil
end