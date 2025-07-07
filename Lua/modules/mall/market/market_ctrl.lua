FILE_REQ("modules/mall/market/market_data")
VIEW_REQ("modules/mall/market/market_view")
VIEW_REQ("modules/mall/money_exchange_view")

MarketCtrl = MarketCtrl or BaseClass(BaseCtrl)

function MarketCtrl:__init()
	if MarketCtrl.Instance ~= nil then
		Debuger.LogError("[MarketCtrl] attempt to create singleton twice!")
		return
	end
	MarketCtrl.Instance = self
	self.data = MarketData.New()
	self:RegisterAllProtocols()
end

function MarketCtrl:OnUnloadGameLogic()
	self.data.cache_list = {}
end

function MarketCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSMarketReq)
	self:RegisterProtocol(SCMarketAllInfo,"OnMarketAllInfo")
	self:RegisterProtocol(SCMarketSingleInfo,"OnMarketSingleInfo")
end

function MarketCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSMarketReq)
	self:UnRegisterProtocol(SCMarketAllInfo)
	self:UnRegisterProtocol(SCMarketSingleInfo)
end

function MarketCtrl:__delete()
	MarketCtrl.Instance = nil
end

function MarketCtrl:SendMarketReq(req_type,param1,param2,param3,param4)
	local protocol = GetProtocol(CSMarketReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	protocol.param3 = param3 or 0
	protocol.param4 = param4 or 0
	SendProtocol(protocol)
end

function MarketCtrl:OnMarketAllInfo(protocol)
	self.data:SetMarketAllInfo(protocol)
end

function MarketCtrl:OnMarketSingleInfo(protocol)
	self.data:SetMarketSingleInfo(protocol)
end

function MarketCtrl.IsRequire(require_type,require_value)
    if require_type == 1 then
        return RoleData.Instance:GetRoleLevel() >= require_value,Format(Language.Market.LimitError[require_type],require_value)
    end
    return true
end

function MarketCtrl:OnMarketSell()
	MarketData.Instance:SetSelectType(2)
	ViewMgr:OpenViewByKey(Mod.Mall.Market)
end