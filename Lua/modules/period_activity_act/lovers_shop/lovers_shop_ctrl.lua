FILE_REQ("modules/period_activity_act/lovers_shop/lovers_shop_data")
VIEW_REQ("modules/period_activity_act/lovers_shop/lovers_shop_view")

local LoversShopReqType = {
    TYPE_INFO = 0,
    TYPE_BUY = 1,           --请求购买
}

LoversShopCtrl = LoversShopCtrl or BaseClass(BaseCtrl)
function LoversShopCtrl:__init()
    if LoversShopCtrl.Instance ~= nil then
        Debuger.LogError("[LoversShopCtrl] attempt to create singleton twice!")
        return
    end
    LoversShopCtrl.Instance = self
    self.data = LoversShopData.New()
    self:RegisterAllProtocols()
end

function LoversShopCtrl:__delete()
    LoversShopCtrl.Instance = nil
    Delete(self, "data")
end

function LoversShopCtrl:OnInit()
    --初始化完毕操作
end

function LoversShopCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCLoversMarketInfo, "OnSCLoversMarketInfo")
    self:RegisterProtocol(CSLoversMarketReq)
end

function LoversShopCtrl:SendReq(req_type, param1)
    local protocol = GetProtocol(CSLoversMarketReq)
    protocol.req_type = req_type
    protocol.param1 = param1
    SendProtocol(protocol)
end

function LoversShopCtrl:SendBuyItem(seq)
    self:SendReq(LoversShopReqType.TYPE_BUY, seq)
end

function LoversShopCtrl:OnSCLoversMarketInfo(protocol)
    self.data:SetShopInfo(protocol)
end


