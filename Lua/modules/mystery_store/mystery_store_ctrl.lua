FILE_REQ("modules/mystery_store/mystery_store_data")
VIEW_REQ("modules/mystery_store/mystery_store_view")
VIEW_REQ("modules/mystery_store/partner_equiq_break_view")
MysteryStoreCtrl = MysteryStoreCtrl or BaseClass(BaseCtrl)

function MysteryStoreCtrl:__init()
	if MysteryStoreCtrl.Instance ~= nil then
		Debuger.LogError("[MysteryStoreCtrl] attempt to create singleton twice!")
		return
	end	
	MysteryStoreCtrl.Instance = self
	self.data = MysteryStoreData.New()
	self.partner_equiq_break_data = PartnerEquiqBreakData.New()
	self:RegisterProtocol(SCMysteryStoreInfo,"OnMysteryStoreInfo")
	self:RegisterProtocol(CSMysteryStoreReq)
end

function MysteryStoreCtrl:__delete()
	self:UnRegisterProtocol(SCMysteryStoreInfo)
	self:UnRegisterProtocol(CSMysteryStoreReq)
	self.data = nil
	MysteryStoreCtrl.Instance = nil
end

function MysteryStoreCtrl:OnUnloadGameLogic()
	self.data.mystery_store_data:Set({next_free_refresh_time = -1})
end


function MysteryStoreCtrl:OnMysteryStoreInfo(protocol)
	self.data:SetItemGroupData(protocol)
end

--req_type: 0 刷新，1购买 && param：传下发的信息的数组下标
function MysteryStoreCtrl:SendMysteryStoreReq(req_type,param)
	local protocol = ProtocolPool.Instance:GetProtocol(CSMysteryStoreReq)
	protocol.req_type = req_type
	protocol.param = param or 0
	SendProtocol(protocol)
end

--分解装备
function MysteryStoreCtrl:Break()
    ViewMgr:OpenView(PartnerEquiqBreakView)
end