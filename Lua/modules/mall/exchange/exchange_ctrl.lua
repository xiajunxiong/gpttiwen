FILE_REQ("modules/mall/exchange/exchange_data")
VIEW_REQ("modules/mall/exchange/exchange_view")
--兑换
ExchangeCtrl = ExchangeCtrl or BaseClass(BaseCtrl)

function ExchangeCtrl:__init()
	if ExchangeCtrl.Instance ~= nil then
		Debuger.LogError("[ExchangeCtrl] attempt to create singleton twice!")
		return
	end	
	ExchangeCtrl.Instance = self
	self.data = ExchangeData.New()
	self:RegisterAllProtocols()
end

function ExchangeCtrl:__delete()
	self:UnRegisterAllProtocols()
	ExchangeCtrl.Instance = nil
	Delete(self,"data")
end

function ExchangeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSExchangeReq)
	self:RegisterProtocol(SCExchangeAllInfo,"OnExchangeAllInfo")
	self:RegisterProtocol(SCExchangeSingleInfo,"OnExchangeSingleInfo")
	-- BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))
end

function ExchangeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSAddItemOnShelve)
	self:UnRegisterProtocol(SCExchangeAllInfo)
	self:UnRegisterProtocol(SCExchangeSingleInfo)
end

--请求购买商品
function ExchangeCtrl:SendExchangeReq(seq,num)
	local protocol = GetProtocol(CSExchangeReq)
	protocol.seq = seq or 0
	protocol.num = num or 1
	SendProtocol(protocol)
end

--下发全部信息
function ExchangeCtrl:OnExchangeAllInfo(protocol)
	self.data:SetExchangeAllInfo(protocol)
end

--下发单个信息
function ExchangeCtrl:OnExchangeSingleInfo(protocol)
	self.data:SetExchangeSingleInfo(protocol)
end

function ExchangeCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self.data:GetNpcId() then
		table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.Exchange.PrestigeTitle,
		function()
			self.data:SetSelectType(ExchangePage.Prestige)
			ViewMgr:OpenViewByKey(Mod.Mall.Exchange)
		end))
	end
end

function ExchangeCtrl:SetSelectIndex(item_id)
	local type,index = self.data:GetItemIndexByType(item_id)
	self.data:SetSelectType(type or 1)
	self.data:SetSelctItemIndex(type or 1,index or 1)
end

function ExchangeCtrl:OpenSelectIndex(item_id)
	self:SetSelectIndex(item_id)
	ViewMgr:OpenViewByKey(Mod.Mall.Exchange)
end