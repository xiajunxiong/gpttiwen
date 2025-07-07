
FILE_REQ("modules/serveractivity/first_charge/first_charge_data")

VIEW_REQ("modules/serveractivity/first_charge/first_charge_view")
VIEW_REQ("modules/serveractivity/first_charge/first_charge_exclusive_view")
FirstChargeCtrl = FirstChargeCtrl or BaseClass(BaseCtrl)

function FirstChargeCtrl:__init()
	if FirstChargeCtrl.Instance ~= nil then
		Debuger.LogError("[FirstChargeCtrl] attempt to create singleton twice!")
		return
	end	
	FirstChargeCtrl.Instance = self
	self.data = FirstChargeData.New()
	self:RegisterAllProtocols()
end

function FirstChargeCtrl:OnInit()
	-- 首充
	local first_charge_remind_func = function ()
		if FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.FirstCharge) then
			return self.data.first_charge_red_point.val and 1 or 0
		else
			return 0
		end
	end
	-- 首充
	RemindCtrl.Instance:Remind():Register(Mod.RandActivity.FirstCharge, self.data.first_charge_red_point, first_charge_remind_func)
	
	-- 首充专属
	local exclusive_remind_func = function ()
		return self.data:GetExclusiveRemind()
	end
	RemindCtrl.Instance:Remind():Register(Mod.RandActivity.FirstChargeExclusive, self.data.first_charge_data, exclusive_remind_func)
	RemindCtrl.Instance:Remind():Register(Mod.RandActivity.FirstChargeExclusive, self.data.exclusive_info, exclusive_remind_func)
	RemindCtrl.Instance:Remind():Register(Mod.RandActivity.FirstChargeExclusive, RoleData.Instance.base_data, exclusive_remind_func, "level")
	
	--注册自定义监听方式
	-- 首充
	ActivityRandData.Instance:Register(Mod.RandActivity.FirstCharge, function()
		return FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.FirstCharge) and self.data:GetFirstChargeFlag() == 0
	end, self.data.first_charge_data)
	-- 首充专属
	ActivityRandData.Instance:Register(Mod.RandActivity.FirstChargeExclusive, function()
		return self.data:GetExclusiveIsOpen()
	end, self.data.first_charge_data)
	ActivityRandData.Instance:Register(Mod.RandActivity.FirstChargeExclusive, function()
		return self.data:GetExclusiveIsOpen()
	end, self.data.exclusive_info)
end

function FirstChargeCtrl:__delete()
	FirstChargeCtrl.Instance = nil
	self:UnRegisterAllProtocols()
end

function FirstChargeCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function FirstChargeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSRAFirstChargeReq)
	self:RegisterProtocol(SCRAFirstChargeInfo,"OnFirstChargeInfo")
	
end

function FirstChargeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSRAFirstChargeReq)
	self:UnRegisterProtocol(SCRAFirstChargeInfo)

	RemindCtrl.Instance:Remind():UnRegister(Mod.RandActivity.FirstCharge)
	RemindCtrl.Instance:Remind():UnRegister(Mod.RandActivity.FirstChargeExclusive)
end

function FirstChargeCtrl:SendFirstChargeReq(req_type, param_1, param_2)
	local protocol = GetProtocol(CSRAFirstChargeReq)
	protocol.req_type = req_type or 0
	protocol.param_1 = param_1 or 0
	protocol.param_2 = param_2 or 0
	SendProtocol(protocol)
end

function FirstChargeCtrl:OnFirstChargeInfo(protocol)
	self.data:SetFirstChargeInfo(protocol)
end
