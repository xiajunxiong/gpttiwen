FILE_REQ("modules/serveractivity/rabbit_year/xingxingdiandeng/xingxingdiandeng_data")
VIEW_REQ("modules/serveractivity/rabbit_year/xingxingdiandeng/xingxingdiandeng_view")

XingXingDianDengCtrl = XingXingDianDengCtrl or BaseClass(BaseCtrl)

function XingXingDianDengCtrl:__init()
    if XingXingDianDengCtrl.Instance ~= nil then
        Debuger.LogError("[XingXingDianDengCtrl] attempt to create singleton twice!")
        return
    end
    XingXingDianDengCtrl.Instance = self
    self.data = XingXingDianDengData.New()
    self:RegisterAllProtocols()
    local remind_func = BindTool.Bind(self.GetXingXingDianDengRed, self)
    RemindCtrl.Instance:Remind():Register(Mod.RabbitYear.XingXingDianDeng, self.data:GetSerInfo(), remind_func)
    BagData.Instance:ListenItems({Config.xingxingdiandeng_auto.other[1].consume_item}, remind_func)
end
function XingXingDianDengCtrl:GetXingXingDianDengRed()
    return self.data:GetXingXingDianDengRed()
end

function XingXingDianDengCtrl:__delete()
    self:UnRegisterAllProtocols()
    XingXingDianDengCtrl.Instance = nil
end

function XingXingDianDengCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

-- function XingXingDianDengCtrl:OnInit()
-- end

function XingXingDianDengCtrl:RegisterAllProtocols()
    -- self:RegisterProtocol(CSTemplateReq)
    self:RegisterProtocol(SCRAStarLightingInfo, "SCRAStarLightingInfo")
end

function XingXingDianDengCtrl:UnRegisterAllProtocols()
    -- self:UnRegisterProtocol(CSTemplateReq)
    -- self:UnRegisterProtocol(SCTemplateInfo)
end

function XingXingDianDengCtrl:SCRAStarLightingInfo(protocol)
    LogDG(protocol)
    self.data:SetSerInfo(protocol)
end
function XingXingDianDengCtrl:SendReq(type, p1)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_STAR_LIGHTING
    param_t.opera_type = type
    param_t.param_1 = p1 or 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

-- function XingXingDianDengCtrl:SendTemplateReq(req_type,param1,param2)
-- 	local protocol = GetProtocol(CSTemplateReq)
-- 	protocol.req_type = req_type or 0
-- 	protocol.param1 = param1 or 0
-- 	protocol.param2 = param2 or 0
-- 	SendProtocol(protocol)
-- end
