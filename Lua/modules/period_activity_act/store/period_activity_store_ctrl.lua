FILE_REQ("modules/period_activity_act/store/period_activity_store_data")

VIEW_REQ("modules/period_activity_act/store/wen_cang_book_store")

PeriodActivityStoreCtrl = PeriodActivityStoreCtrl or BaseClass(BaseCtrl)

PeriodActivityStoreCtrl.ReqType = {
    TYPE_INFO = 0,
    TYPE_BUY = 1,           --请求购买
}

function PeriodActivityStoreCtrl:__init()
	if PeriodActivityStoreCtrl.Instance ~= nil then
		Debuger.LogError("[PeriodActivityStoreCtrl] attempt to create singleton twice!")
		return
	end
	PeriodActivityStoreCtrl.Instance = self
    self.data = PeriodActivityStoreData.New()
    self.data = PeriodActivityStoreData.Instance
    self:RegisterAllProtocols()
end

function PeriodActivityStoreCtrl:__delete()
    PeriodActivityStoreCtrl.Instance = nil
end

function PeriodActivityStoreCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function PeriodActivityStoreCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSWenCangReq)
    self:RegisterProtocol(SCWenCangInfo, "OnSCWenCangInfo")
end

function PeriodActivityStoreCtrl:SendReq(req_type, param1)
    local protocol = GetProtocol(CSWenCangReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

-- 文昌书局
function PeriodActivityStoreCtrl:OnSCWenCangInfo(protocol)
    self.wen_cang_shop_data:Set(protocol.store_list)
end