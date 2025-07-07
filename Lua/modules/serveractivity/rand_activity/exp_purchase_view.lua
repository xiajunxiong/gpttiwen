ExpPurchaseView = ExpPurchaseView or DeclareView("ExpPurchaseView", "serveractivity/exp_purchase", Mod.Splendid.ExpPurchase)
VIEW_DECLARE_LEVEL(ExpPurchaseView, ViewLevel.L1)
function ExpPurchaseView:ExpPurchaseView()
	self.server_activity_data = ServerActivityData.Instance
	self.data = self.server_activity_data.exp_purchase_data

	self.info_change_handle = self.data:Care(BindTool.Bind(self.UpdateShow, self))
	self.level_handle = RoleData.Instance.base_data:KeysCare({"level"}, BindTool.Bind(self.UpdateShow, self))
	self.bag_handle = BagData.Instance:Care(BindTool.Bind(self.UpdateShow, self))
end

function ExpPurchaseView:CloseCallback()
	self.data:Uncare(self.info_change_handle)
	RoleData.Instance.base_data:Uncare(self.level_handle)
	BagData.Instance:UnCare(self.bag_handle)
	self.Time:CloseCountDownTime()
end

function ExpPurchaseView:OnCloseClick()
	ViewMgr:CloseView(ExpPurchaseView)
end

function ExpPurchaseView:LoadCallback()
	self.Time:CreateActTime(ACTIVITY_TYPE.RAND_EXP_PURCHASE, TimeType.Type_Special_4)
	self.Time:SetCallBack(function()
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.ActivityCloseTip)
		ViewMgr:CloseView(ExpPurchaseView)
	end)
	UH.SetText(self.LimitTypeTxt, Language.ExpPurchase.LimitType[self.server_activity_data:GetExpPurchaseLimitType()])
	self:UpdateShow()
end

function ExpPurchaseView:UpdateShow()
	local exp_configure = ServerActivityData.Instance:GetExpPurchaseConfigure()
	UH.SetText(self.ExperienceTxt, exp_configure.exp_num)
	UH.SetText(self.GoldTxt, exp_configure.gold_num)
	UH.SetText(self.ItemTxt, exp_configure.item_num)
	UH.SetText(self.LevelTxt, RoleData.Instance:GetRoleLevel())
	UH.SetText(self.LimitTxt, self.server_activity_data:GetExpPurchaseHasLimitTimes())
	
	self:UpdateBtn()
	self:UpdateRedNum()
end

function ExpPurchaseView:UpdateBtn()
	-- 满级或者次数用完就置灰
	local IHandler = self.server_activity_data:GetExpPurchaseIsReachLevelMax() or self.server_activity_data:GetExpPurchaseIsReachLimit()
	self.ItemIHandler.Interactable = not IHandler
	self.YuanBaoIHandler.Interactable = not IHandler
	self.ItemEffectObj:SetActive(not IHandler)
	self.YuanBaoEffectObj:SetActive(not IHandler)
end

function ExpPurchaseView:UpdateRedNum()
	if self.red_pos == nil then
		self.red_pos = UiRedPoint.New(self.RedPos)
	end
	self.red_pos:SetNum(self.server_activity_data:GetExpPurchaseRemind())
end

function ExpPurchaseView:OnItemBuyClick()
	if self.server_activity_data:GetExpPurchaseIsReachLevelMax() then
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.LimitTip3)
	elseif self.server_activity_data:GetExpPurchaseIsReachLimit() then
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.LimitTip)
	elseif not self.server_activity_data:GetExpPurchaseIsReachItem() then
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.LimitTip2)
		PublicPopupCtrl.Instance:Error()
	else
		-- 0就是道具，1是元宝购买
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_EXP_PURCHASE, 0, 0)
	end
end

function ExpPurchaseView:OnYuanBaoBuyClick()
	local exp_configure = ServerActivityData.Instance:GetExpPurchaseConfigure()
	if self.server_activity_data:GetExpPurchaseIsReachLevelMax() then
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.LimitTip3)
	elseif self.server_activity_data:GetExpPurchaseIsReachLimit() then
		PublicPopupCtrl.Instance:Center(Language.ExpPurchase.LimitTip)
	elseif not MallCtrl.IsNotCurrency(CurrencyType.Gold, exp_configure.gold_num) then
		PublicPopupCtrl.Instance:Error()
		return
	else
		-- 0就是道具，1是元宝购买
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_EXP_PURCHASE, 0, 1)
	end
end