
FILE_REQ("modules/illusion/illusion_config")
FILE_REQ("modules/illusion/illusion_info")
FILE_REQ("modules/illusion/illusion_data")
VIEW_REQ("modules/illusion/illusion_view")
VIEW_REQ("modules/illusion/illusion_exchange_view")
VIEW_REQ("modules/illusion/illusion_view_suit_attr")
VIEW_REQ("modules/illusion/illusion_level_up")
VIEW_REQ("modules/illusion/illusion_sp_level_up")
VIEW_REQ("modules/illusion/illusion_all_attr")

IllusionCtrl = IllusionCtrl or BaseClass(BaseCtrl)

function IllusionCtrl:__init()
	if IllusionCtrl.Instance ~= nil then
		Debuger.LogError("[IllusionCtrl] attempt to create singleton twice!")
		return
	end	
	IllusionCtrl.Instance = self
	self.data = IllusionData.New()

	self:RegisterAllProtocols()

	RemindCtrl.Instance:Remind():Register(Mod.Illusion.HuanHua, self.data.illusion_lv_info, function ()
		self:CheckUpRemind()
		return self.data:GetRemind()
	end)

	-- 霓裳红点道具刷新
	if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.data:RemindItemList(), BindTool.Bind(self.CheckUpRemind, self))
	end
end

function IllusionCtrl:OnInit()
	RoleData.Instance.base_data:KeysCare({"level"},function ()
        self:CheckShowList()
    end)
end

function IllusionCtrl:__delete()
	if IllusionCtrl.Instance == self then
		IllusionCtrl.Instance = nil
	end

	self:UnRegisterAllProtocols()
end

function IllusionCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSSurfaceOp)

	self:RegisterProtocol(SCSurfaceAllInfo, "OnSurfaceAllInfo")
	self:RegisterProtocol(SCSurfaceSingleInfo, "OnSurfaceSingleInfo", true)
	self:RegisterProtocol(SCSurfaceWearInfo, "OnSurfaceWearInfo", true)

	self:RegisterProtocol(CSFashionCollectionRequest)
	self:RegisterProtocol(CSFashionCollectionInfo, "OnCSFashionCollectionInfo")
end

function IllusionCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCSurfaceAllInfo)
	self:UnRegisterProtocol(SCSurfaceSingleInfo)
	self:UnRegisterProtocol(SCSurfaceWearInfo)
end

function IllusionCtrl:OnSurfaceAllInfo(protocol)
	self.data:SetSurfaceAllInfo(protocol)
end

function IllusionCtrl:OnSurfaceSingleInfo(protocol)
	self.data:SetSurfaceSingleInfo(protocol)
end

function IllusionCtrl:OnSurfaceWearInfo(protocol)
	self.data:SetSurfaceWearInfo(protocol)
end

function IllusionCtrl:SendSurfaceOp(param_t)
	local protocol = GetProtocol(CSSurfaceOp)
	protocol.oper_type = param_t.oper_type
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	SendProtocol(protocol)
end

function IllusionCtrl:SendSurfaceWear(id, part)
	local param_t = {
		oper_type = IllusionConfig.OperType.wear,
		param1 = id,
		param2 = part
    }
	self:SendSurfaceOp(param_t)
end

function IllusionCtrl:SendSurfaceExchange(seq)
	local param_t = {
		oper_type = IllusionConfig.OperType.exchange,
		param1 = seq,
    }
	self:SendSurfaceOp(param_t)
end

function IllusionCtrl:CheckShowList()
	self.data:FashionShowList()
end

function IllusionCtrl:SendIllusionUp(oper_type, param1)
	local protocol = GetProtocol(CSFashionCollectionRequest)
	protocol.oper_type = oper_type or IllusionConfig.IllusionUpReqType.Info
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function IllusionCtrl:OnCSFashionCollectionInfo(protocol)
	self.data:SetIllusionLvInfo(protocol)
end

function IllusionCtrl:CheckUpRemind()
	self.data:CheckUpRemind()
end