FILE_REQ("modules/mall/recharge/recharge_data")
VIEW_REQ("modules/mall/recharge/recharge_view")
VIEW_REQ("modules/pc_web_view/pc_web_view")
RechargeCtrl = RechargeCtrl or BaseClass(BaseCtrl)

function RechargeCtrl:__init()
	if RechargeCtrl.Instance ~= nil then
		Debuger.LogError("[RechargeCtrl] attempt to create singleton twice!")
		return
	end	
	RechargeCtrl.Instance = self
	self.data = RechargeData.New()
	self:RegisterAllProtocols()
end

function RechargeCtrl:OnInit()
	FunOpen.Instance:SelfCondition(Mod.Store.Recharge,function ()
		if GLOBAL_CONFIG:Flags().block_recharge then
			return false, Language.Recharge.NotOpenCloseBeta
		else
			local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Store.Recharge)
			if RoleData.Instance:GetRoleLevel() >= open_level then
				return true
			else
				return false, string.format(ErrorText[90], open_level)
			end
		end
	end)
	FunOpen.Instance:SelfCondition(Mod.Reward.FortuneBag,function ()
		if GLOBAL_CONFIG:Flags().block_recharge then
			return false, Language.Recharge.NotOpenCloseBeta
		else
			local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Reward.FortuneBag)
			if RoleData.Instance:GetRoleLevel() >= open_level then
				return true
			else
				return false, string.format(ErrorText[90], open_level)
			end
		end
	end)
	FunOpen.Instance:SelfCondition(Mod.Reward.MonthCard,function ()
		if GLOBAL_CONFIG:Flags().block_recharge then
			return false, Language.Recharge.NotOpenCloseBeta
		else
			local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Reward.MonthCard)
			if RoleData.Instance:GetRoleLevel() >= open_level then
				return true
			else
				return false, string.format(ErrorText[90], open_level)
			end
		end
	end)
	FunOpen.Instance:SelfCondition(Mod.RandActivity.FirstCharge,function ()
		if GLOBAL_CONFIG:Flags().block_recharge then
			return false
		else
			local open_cfg = FunOpen.Instance:GetOpenData(Mod.RandActivity.FirstCharge)
			if open_cfg then
				return FunOpen.Instance:CheckLevel(open_cfg, RoleData.Instance:GetRoleLevel())
			else
				return true
			end
		end
	end)
end

function RechargeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSChongzhiOperateReq)
	self:RegisterProtocol(CSRechargeConfigInfoReq)
	self:RegisterProtocol(SCChongzhiInfo,"OnChongzhiInfo")
	self:RegisterProtocol(SCRechargeConfigInfo,"OnRechargeConfigInfo")
	self:RegisterProtocol(SCRechargeConfigReloadNotice,"OnRechargeConfigReloadNotice")
	RemindCtrl.Instance:Remind():Register(Mod.Reward.Rebate,self.data.reward_data,BindTool.Bind(self.CheckRemind, self))
    self.role_complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SendRechargeInfo, self))
end

function RechargeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSChongzhiOperateReq)
	self:UnRegisterProtocol(CSRechargeConfigInfoReq)
	self:UnRegisterProtocol(SCChongzhiInfo)
	self:UnRegisterProtocol(SCRechargeConfigInfo)
	self:UnRegisterProtocol(SCRechargeConfigReloadNotice)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.Rebate)
	EventSys:UnBind(self.role_complete_handle)
end

function RechargeCtrl:__delete()
	self:UnRegisterAllProtocols()
	RechargeCtrl.Instance = nil
	Delete(self,[[data]])
end

--请求充值信息
function RechargeCtrl:SendRechargeReq(op_type,param1,param2)
	local protocol = GetProtocol(CSChongzhiOperateReq)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

--请求充值
function RechargeCtrl:SendRechargeInfo()
	local protocol = GetProtocol(CSRechargeConfigInfoReq)
	protocol.currency_type = self.data:GetCurrencyType() or 0
	protocol.spid = LoginData.Instance:GetAccountSpid() or 0
	SendProtocol(protocol)
end

--下发全部信息
function RechargeCtrl:OnChongzhiInfo(protocol)
	self.data:SetChongzhiInfo(protocol)
	self.wait_info = false
	RoleCtrl.Instance:PlatReport()
end

--服务端下发充值信息
function RechargeCtrl:OnRechargeConfigInfo(protocol)
	self.data:SetRechargeConfigInfo(protocol)
end

-- 收到这个协议需要请求一遍充值列表2042下发
function RechargeCtrl:OnRechargeConfigReloadNotice(protocol)
	self:SendRechargeInfo()
end

function RechargeCtrl:CheckRemind()
	if not FunOpen.Instance:GetFunIsOpened(Mod.Reward.Rebate) then
		return 0
	end
	return self.data:GetRewardNum()
end

--充值操作
function RechargeCtrl:Recharge(money, gift_id)
	if GLOBAL_CONFIG:Flags().open_chongzhi == false then
		PublicPopupCtrl.Instance:Center(Language.Recharge.NotOpenCloseBeta)
		return
	end
	local role_id = RoleData.Instance:GetRoleId()
	local role_name = RoleData.Instance:GetRoleName()
	local server_id = LoginData.Instance:GetLastLoginServer()
	if money and money ~= 0 and role_id and role_name and server_id then
		ChannelAgent:Mai(role_id, role_name, money, server_id, nil, gift_id or 0)
	else
		PublicPopupCtrl.Instance:Center(Language.Common.ChargeFail)
	end
end