--Ctrl在 module_manager 注册 table.insert(self.ctrl_list,ChongJiFanLiCtrl.New())
--Ctrl在 modules_require_list 加载 "modules/template/template_ctrl",

FILE_REQ("modules/serveractivity/chongjifanli/chongjifanli_data")
VIEW_REQ("modules/serveractivity/chongjifanli/chongjifanli_view")

ChongJiFanLiCtrl = ChongJiFanLiCtrl or BaseClass(BaseCtrl)

ChongJiFanLiCtrl.REQ_TYPE = {
	REQ_INFO = 0,                   -- 0 请求数据信息
	REQ_GET_REWARD = 1,        -- 1 领取奖励
}


function ChongJiFanLiCtrl:__init()
	if ChongJiFanLiCtrl.Instance ~= nil then
		Debuger.LogError("[ChongJiFanLiCtrl] attempt to create singleton twice!")
		return
	end
	ChongJiFanLiCtrl.Instance = self
	self.data = ChongJiFanLiData.New()
    self:RegisterAllProtocols()
end


--这里注册协议 请求协议 不需要带回调 如下 
function ChongJiFanLiCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCChongJiFanLiInfo, "OnChongJiFanLiInfo")
end

function ChongJiFanLiCtrl:SendChongJiFanLiReq(req_type,param_1,param_2)
	--LOG("宠技返利请求：",req_type,param_1,param_2)
	ServerActivityCtrl.Instance:SendRandActivityOperaReq({rand_activity_type = ACTIVITY_TYPE.RAND_CHONGJIFANLI, opera_type = req_type, param_1 = param_1 or 0,param_2 = param_2 or 0})
end

function ChongJiFanLiCtrl:OnChongJiFanLiInfo(protocol)
    self.data:SetChongJiFanLiInfo(protocol)
end



function ChongJiFanLiCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCChongJiFanLiInfo)
end


function ChongJiFanLiCtrl:__delete()
    self:UnRegisterAllProtocols()
    ChongJiFanLiCtrl.Instance = nil
end


function ChongJiFanLiCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end
