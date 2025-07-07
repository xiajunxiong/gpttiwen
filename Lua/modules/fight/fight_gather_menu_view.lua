
FightGatherMenuView = FightGatherMenuView or DeclareMono("FightGatherMenuView", UIWFlushPanel)

function FightGatherMenuView:FightGatherMenuView()
	self.data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.gather_energy, func = self.FlushEnergy},
		{data = self.data:GetCollectGatherList(), func = self.FlushOutcome},
		{data = AutoInvokerCtrl.Instance.auto_state, func = self.FlushBtnAuto},
		{data = self.data.gather_energy, func = self.IsEnoughEnergy, init_call = false},
		{data = SceneData.Instance:Base(), func = self.OnEnable,init_call = false, keys = {"scene_id"}},
	}
end

function FightGatherMenuView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	UH.SetText(self.SceneName,SceneData.Instance:SceneName())
	TimeHelper:CancelTimer(self.show_time)
	self.show_time = TimeHelper:AddDelayTimer(function()
		self:OnClickAuto(true)
	end, 1)
	self.Panel:SetActive(false)
	for i=1,self.ItemList:Length() do
		self.ItemList[i]:SetData({})
	end
end

function FightGatherMenuView:OnDisable()
	UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.show_time)
	if SceneData.Instance:GetMode() ~= SceneModeType.FightGather then
		FightData.Instance:SetCollectGatherList{}
		MainOtherCtrl.Instance:CloseGather()
		for i=1,self.ItemList:Length() do
			self.ItemList[i]:SetData({})
		end
	end
end

function FightGatherMenuView:FlushEnergy()
	local consume = self.data.gather_energy.val / self.data.gather_energy.per
	for i=1,self.Consume:Length() do
		UH.SetText(self.Consume[i],math.floor(consume))
	end
end

function FightGatherMenuView:FlushBtnAuto()
	local auto_state = AutoInvokerCtrl.Instance:AutoState()
	for i=1,self.BtnAuto:Length() do
		UH.SetText(self.BtnAuto[i],Language.Fight.Gather.auto_btns[auto_state])
	end
end

function FightGatherMenuView:FlushOutcome()
	local list = self.data:GetCollectGatherList():Val()
	for i=1,self.ItemList:Length() do
		self.ItemList[i]:SetData(list)
	end
end

--体力不足的时候使用或者跳转购买
function FightGatherMenuView:IsEnoughEnergy()
	local value = self.data.gather_energy.val >= self.data.gather_energy.per
	if not value then
		MainOtherCtrl.Instance:CloseGather()
		ViewMgr:OpenView(ItemConsumeTipView,{item_id = 24391})
	end
	return value
end

--点击自动
function FightGatherMenuView:OnClickAuto(is_enable)
	local auto_state = AutoInvokerCtrl.Instance:AutoState()
	if tonumber(auto_state) == AutoCommandState.None and self:IsEnoughEnergy() then
		local gather_type = self:IsManual()
		if gather_type then
			PublicPopupCtrl.Instance:Center(Language.Fight.Gather.AutoGatherTip)
			return
		end
		local config = self.data.gather_scene_list[SceneData.Instance:SceneId()]
		if config ~= nil then
			AutoInvokerCtrl.Instance:SetCommands({AutoCommandGather.New(config.gather_id)}, 0)
		end
	else
		if is_enable == nil then
			MainOtherCtrl.Instance:CloseGather()
			self:FlushBtnAuto()
		end
	end
end

function FightGatherMenuView:IsManual()
	for k,v in pairs({"Gather","FaMu","WaKuang"}) do
		if MainPlayer:IsInAnim(ChAnimType[v]) then
			return v
		end
	end
end

function FightGatherMenuView:OnClickOut()
	MainOtherCtrl.Instance:CloseGather()
	PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips,function()
		SceneCtrl.Instance:RequestLeaveFb()
	end)
end

FightGatherMenuOutcomeViewItem = FightGatherMenuOutcomeViewItem or DeclareMono("FightGatherMenuOutcomeViewItem",UIWidgetBaseItem)

function FightGatherMenuOutcomeViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
	self.Item:SetData(self.data)
	self.Name.text = self.data:Name()
end

-- --===========================FightGatherMenuOrderView===========================
-- FightGatherMenuOrderView = FightGatherMenuOrderView or DeclareMono("FightGatherMenuOrderView", UIWFlushPanel)

-- function FightGatherMenuOrderView:FightGatherMenuOrderView()
-- 	self.data = FightCtrl.Instance:Data()
-- 	self.data_cares = {
		-- {data = self.data:GetCollectGatherList(), func = self.FlushOrderView,init_call = false},
		-- {data = SilkRoadData.Instance.single_change, func = self.FlushOrderView,init_call = false},
		-- {data = SceneData.Instance:Base(), func = self.FlushOrderView,init_call = false, keys = {"scene_id"}},
-- 	}
-- end

-- function FightGatherMenuOrderView:OnEnable()
	-- UIWFlushPanel.OnEnable(self)
	-- self:FlushOrderView()
-- 	self.Panel:SetActive(false)
-- end

-- function FightGatherMenuOrderView:FlushOrderView()
-- 	local scene_id = SceneData.Instance:SceneId()
-- 	local order_list = SilkRoadData.Instance:GetOrderList(scene_id)
-- 	if order_list ~= nil then
-- 		for i=1,self.ItemList:Length() do
-- 			local config = order_list[i]
-- 			if config ~= nil then
-- 				self.ItemList[i]:SetData(config)
-- 			end
-- 			self.ItemList[i]:SetObjActive(config ~= nil and SilkRoadData.Instance:GetSingleFlag(config.index) == 0)
-- 		end
-- 	end
-- 	self.Panel:SetActive(order_list ~= nil)
-- end

-- ----------------------------FightGatherMenuOrderItem----------------------------
-- FightGatherMenuOrderItem = FightGatherMenuOrderItem or DeclareMono("FightGatherMenuOrderItem", UIWidgetBaseItem)

-- function FightGatherMenuOrderItem:SetData(data)
-- 	local need_item = data.need_item or {}
-- 	local num = Item.GetNum(need_item.item_id)
-- 	local color = num >= need_item.num and COLORSTR.Green10 or COLORSTR.Red10
-- 	UH.SetText(self.Num,Format("%s: <color=#%s>%s</color>/%s",
-- 	Item.GetName(need_item.item_id),color,num,need_item.num))
-- 	self.BtnInter.Interactable = color == COLORSTR.Green10
--     UIWidgetBaseItem.SetData(self, data)
-- end

-- function FightGatherMenuOrderItem:OnClickSubmit()
-- 	if self.BtnInter.Interactable then
-- 		local item_id = self.data.need_item.item_id or 0
-- 		local data = SilkRoadData.Instance:GetSilkRoadSingleData(item_id)
-- 		if data ~= nil then
-- 			SilkRoadData.Instance:SetSelectData(data)
-- 		end
-- 		ViewMgr:OpenView(SilkRoadView)
-- 	else
-- 		PublicPopupCtrl.Instance:Center(Language.Fight.Gather.OrderTip)
-- 	end
-- end