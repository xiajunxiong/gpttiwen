FILE_REQ("modules/illustrate/illustrate_config")
FILE_REQ("modules/illustrate/illustrate_data")
VIEW_REQ("modules/illustrate/illustrate_view")
VIEW_REQ("modules/illustrate/illustrate_main_view")
VIEW_REQ("modules/illustrate/illustrate_collect_view")
VIEW_REQ("modules/illustrate/illustrate_rare")

IllustrateCtrl = IllustrateCtrl or BaseClass(BaseCtrl)

function IllustrateCtrl:__init()
	if IllustrateCtrl.Instance ~= nil then
		Debuger.LogError("[IllustrateCtrl] attempt to create singleton twice!")
		return
	end
	IllustrateCtrl.Instance = self
    self.data = IllustrateData.New()
	self:RegisterAllProtocols()
end

function IllustrateCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSCollectionOp)
	self:RegisterProtocol(SCCollectionAllInfo,"OnCollectionAllInfo")
	self:RegisterProtocol(SCCollectionSingleInfo,"OnCollectionSingleInfo")
	self:RegisterProtocol(SCCollectionIncludedInfo,"OnCollectionIncludedInfo")
	self:RegisterProtocol(SCCollectionIncludedSingle,"OnCollectionIncludedSingle")
	self:RegisterProtocol(SCCollectionAtlasCardInfo,"OnCollectionAtlasCardInfo")
	self:RegisterProtocol(SCCollectionAtlasCardSignleInfo,"OnCollectionAtlasCardSignleInfo")
	RemindCtrl.Instance:Remind():Register(Mod.Illustrate.Main,BagData.Instance.item_grid,BindTool.Bind(self.CheckRemind, self))
	RemindCtrl.Instance:Remind():Register(Mod.Illustrate.Collect,self.data.included_red_piont,BindTool.Bind(self.CheckIncludedRemind, self))
	RemindCtrl.Instance:Remind():Register(Mod.Illustrate.Rare, self.data.rare_data, BindTool.Bind(self.CheckRareRemind, self))
	self.rare_item_listen = BagData.Instance:ListenItems(self.data:RemindRareItemList(), function()
		self.data.rare_data:SetDirty("remind")
	end)
end

function IllustrateCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSCollectionOp)
	self:UnRegisterProtocol(SCCollectionAllInfo)
	self:UnRegisterProtocol(SCCollectionSingleInfo)
	self:UnRegisterProtocol(SCCollectionIncludedInfo)
	self:UnRegisterProtocol(SCCollectionIncludedSingle)
	self:UnRegisterProtocol(SCCollectionAtlasCardInfo)
	self:UnRegisterProtocol(SCCollectionAtlasCardSignleInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Illustrate.Main)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Illustrate.Collect)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Illustrate.Rare)
end

function IllustrateCtrl:__delete()
	self:UnRegisterAllProtocols()
    IllustrateCtrl.Instance = nil
    Delete(self,"data")
end

function IllustrateCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function IllustrateCtrl:SendCollectSeq(op_type,param1)
	local protocol = GetProtocol(CSCollectionOp)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function IllustrateCtrl:OnCollectionAllInfo(protocol)
	self.data:SetCollectionAllInfo(protocol)
end

function IllustrateCtrl:OnCollectionSingleInfo(protocol)
	self.data:SetCollectionSingleInfo(protocol)
end

function IllustrateCtrl:OnCollectionIncludedInfo(protocol)
	self.data:SetCollectionIncludedInfo(protocol)
end

function IllustrateCtrl:OnCollectionIncludedSingle(protocol)
	self.data:SetCollectionIncludedSingle(protocol)
end

function IllustrateCtrl:OnCollectionAtlasCardInfo(protocol)
	self.data:SetRareAllInfo(protocol)
end

function IllustrateCtrl:OnCollectionAtlasCardSignleInfo(protocol)
	self.data:SetRareInfo(protocol)
end

function IllustrateCtrl:CheckRemind()
	if not FunOpen.Instance:GetFunIsOpened(Mod.Illustrate.Main) then
		return 0
	end
	return self.data:GetRemindNum()
end

function IllustrateCtrl:CheckIncludedRemind()
	if not FunOpen.Instance:GetFunIsOpened(Mod.Illustrate.Collect) then
		return 0
	end
	return self.data:GetIncludedRedNum()
end

function IllustrateCtrl:CheckRareRemind()
	return self.data:RareRemind()
end