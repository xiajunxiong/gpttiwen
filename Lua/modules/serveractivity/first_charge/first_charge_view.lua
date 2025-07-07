FirstChargeView = FirstChargeView or DeclareView("FirstChargeView", "serveractivity/openserver/first_charge_view", Mod.RandActivity.FirstCharge)
VIEW_DECLARE_MASK(FirstChargeView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(FirstChargeView, ViewLevel.L1)

function FirstChargeView:FirstChargeView()
	self.Data = FirstChargeData.Instance
	self.data = self.Data.first_charge_data
	self.language = Language.FirstCharge
	self.ht = self.data:Care(BindTool.Bind(self.FlushRewardView, self))
	self.ht2 = self.Data.first_charge_red_point:Care(BindTool.Bind(self.FlushRewardView, self))
end

function FirstChargeView:LoadCallback()
	if self.ui_obj == nil then
		self.ui_obj = UIChDrawer.New()
	end
	self.ui_obj:SetMainSolo("actor/npc/200.prefab")
	self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self, show_shadow = false})
	self.ui_obj:PlayAnim(ChAnimType.Show)

	local config = FirstChargeData.Instance:GetFirstChargeList()
	self.List:SetData(DataHelper.FormatItemList(config[1]))
	self:FlushRewardView()
end

function FirstChargeView:CloseCallback()
	if self.red_point ~= nil then
		self.red_point:DeleteMe()
		self.red_point = nil
	end
	self.data:Uncare(self.ht)
	self.Data.first_charge_red_point:Uncare(self.ht2)
end

function FirstChargeView:FlushRewardView()
	local num = RechargeData.Instance:GetRechargeNum()
	local flag = FirstChargeData.Instance:GetFirstChargeFlag()
	if flag ~= 0 then
		ViewMgr:CloseView(FirstChargeView)
		ViewMgr:OpenView(FirstChargeExclusiveView)
		return
	end
	self.can_reward = num > 0 and flag == 0
	if self.red_point == nil then
		self.red_point = UiRedPoint.New(self.RedPos, Vector3.zero, false)
	end
	self.red_point:SetNum(num)
	UH.SetText(self.BtnName, self.language.BtnGo[self.can_reward and 2 or 1])
end

function FirstChargeView:OnClickRecharge()
	if IS_AUDIT_VERSION then
		RoleCtrl.Instance:SendAuditBehaviourSubmitReq(0)
	end
	if self.can_reward then
		FirstChargeCtrl.Instance:SendFirstChargeReq(FIRST_RECHARGE_REQ_TYPE.FETCH_FIRST_REWARD, self.Data.first_recharge_config[1].seq)
	else
		ViewMgr:OpenViewByKey(Mod.Store.Recharge)
	end
end

function FirstChargeView:OnOpenInfoClick()
	local partner_id = 5
	local partner_data = PartnerCtrl.Instance:Data()
	partner_data.new_partner = partner_id
	local config = Config.partner_cfg_auto.partner_info[partner_id]
    ViewMgr:OpenView(PartnerViewLock, {partner_name = config.name})
end

function FirstChargeView:OnClickClose()
	ViewMgr:CloseView(FirstChargeView)
end