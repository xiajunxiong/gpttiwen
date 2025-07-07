ItemInfoView = ItemInfoView or DeclareView("ItemInfoView","item_info/item_info")
VIEW_DECLARE_MASK(ItemInfoView,ViewMask.None)
VIEW_DECLARE_CACHE(ItemInfoView)

function ItemInfoView:ItemInfoView()
	self.data = ItemInfoData.Instance
	self.Offset = ItemInfoConfig.AutoPosConfig.Offset
	self:RegisterGetGuideUi()
end

function ItemInfoView:RegisterGetGuideUi()
	GuideManager.Instance:RegisterGetGuideUi("BagEquipUseBtn", function()
		if self.FunctionList and self.FunctionList.item_list and self.FunctionList.item_list[1] then
			return self.FunctionList.item_list[1].BtnObj
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("EquipDonationUseBtn", function()
		if self.FunctionList and self.FunctionList.item_list and self.FunctionList.item_list[2] then
			return self.FunctionList.item_list[2].BtnObj
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("TreasureUseBtn", function()
		if self.FunctionList and self.FunctionList.item_list and self.FunctionList.item_list[1] then
			return self.FunctionList.item_list[1].BtnObj,function ()
				if ViewMgr:IsOpen(KeyUseView) then
					MainOtherCtrl.Instance:CloseKeyUse()
				end
				self.FunctionList.item_list[1]:OnClick()
				local info = self.data:ItemInfo()
				TreasureMapData.Instance:SetMapInfo(info.item_list[1])
				ViewMgr:OpenView(Compass)
				SceneLogic.Instance:ClearMoveCache()
				SceneLogic.Instance:AutoToPos(info.item_list[1].param.scene_id, info.item_list[1].param, BehaviorFactory.TreasureMap(info.item_list[1]), true, true, nil)
			end
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("BagResolveEquipBtn", function()
		if self.FunctionList and self.FunctionList.item_list and self.FunctionList.item_list[3] then
			return self.FunctionList.item_list[3].BtnObj
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("MarbasEquipEquip", function()
		if self.FunctionList and self.FunctionList.item_list and self.FunctionList.item_list[1] then
			return self.FunctionList.item_list[1].BtnObj
		end
	end)

	GuideManager.Instance:RegisterGetGuideUi("GetWayGotoMark", function()
		if self.GetWay1.GetGotoMarkBtn1 == nil then	--ItemInfoViewGetWay
			return
		end
		return self.GetWay1:GetGotoMarkBtn1()
	end)
end

function ItemInfoView:LoadCallback()
	local info = self.data:ItemInfo()
	if nil == info.item_list then return end
	self.item_list = info.item_list or {}
	local item = self.item_list[1] or Item.New()
	self.Info2:SetActive(self.item_list[2] ~= nil)
	if item.Type == nil then
		BuglyReport("Item Info Open Cfg Error id =", item.item_id)
	else
		self.fixed_pos = self.Offset[item:Type()]
		self.model_pos = self.Offset.Model[item:Type()]
	end
	self:FlushListView(info.func,item)
	self:FlushItemTypeView(item.item_id)
	self.need_calcu_pos = self:GetNeedCalcPos(item)
	self:FlushELement()
	self:AutoToPos()
end
function ItemInfoView:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end
function ItemInfoView:OpenCallback()
	self:AutoToPos(true)
	self.handle_care = ViewMgr:CareTouchDownEvent(function ()
		if GuideManager.Instance:IsGuide() then
			return
		end
        if ViewMgr.touch_down_data.type ~= ViewManager.TouchDownType.ItemInfo then
            ViewMgr:CloseView(ItemInfoView)
        end
    end)
end

function ItemInfoView:OnClickBlank()
	if nil ~= self.data.derive_view then
		ItemInfoCtrl.Instance:ItemDeriveView()
	else
		ViewMgr:CloseView(ItemInfoView)
	end
end

function ItemInfoView:CloseCallback()
	if self.handle_care then
        ViewMgr:UncareTouchDownEvent(self.handle_care)
        self.handle_care = nil
    end
	self.Info2:SetActive(false)
	self.data:SetClickDataPos()
	ItemInfoCtrl.Instance:ItemDeriveView()
	for i=1,self.ELement:Length() do
		self.ELement[i].enabled = false
		self.ScrollList[i].enabled = false
	end
	self.InfoPosition.localPosition = Vector2.zero
	self.InfoPosition.sizeDelta = Vector2.zero
	self.RightSize.sizeDelta = Vector2.zero
	self.Left:SetActive(false)
	self.RightHeight.minHeight = 0
	self.CrystalEquip:End()
end

function ItemInfoView:OnFlush()
	self.FunctionList:SetObjActive(nil ~= next(self.funcs or {})and nil == self.data.derive_view)
end

function ItemInfoView:FlushListView(func,item)
	self.funcs = func and func(item) or {}
	self.FunctionList:SetData(self.funcs)
end

function ItemInfoView:FlushItemTypeView(item_id)
	local is_pre_range = self.item_list[1]:PreRange()
	local is_sell = DataHelper.SimpleIndex(self.funcs,"name","Sell") ~= nil
	self.ModelShow.localPosition = self.model_pos or Vector2.zero
	self.RightSize.sizeDelta = self.fixed_pos and Vector2.zero or 
	(is_sell and self.Offset.SellAutoSize or 
	(is_pre_range and self.Offset.PreRangeSize or self.Offset.GetWaySize))
	self.Left:SetActive(Item.GetType(item_id) == ItemType.Illustration)
	self.RightHeight.minHeight = is_sell and 362 or (is_pre_range and 490 or 0)
	local way_list = Item.GetWayList(item_id) or {}
	if #way_list > 0 then
		local height = self.Offset.MinHeight + self.data.get_way_show.start + 320
		height = height > 490 and 490 or height
		if height >= self.RightHeight.minHeight then
			self.RightHeight.minHeight = height
		end
	end
end

function ItemInfoView:IsEdgePosition()
	local IsEdgePosition = false
	for i=1,self.ELement:Length() do
		if self.item_list and self.item_list[i] ~= nil then
			local height = self.Content[i].sizeDelta.y
			local max_height = self.ELement[i].minHeight
			self.ELement[i].enabled = height > max_height
			self.ScrollList[i].enabled = height > max_height
			IsEdgePosition = IsEdgePosition or height > max_height
		end
	end
	return IsEdgePosition
end

function ItemInfoView:FlushELement()
	for i=1,self.ELement:Length() do
		self.ELement[i].minHeight = self.Root.rect.height - self.Offset.MinHeight
		self.ELement[i].preferredHeight = self.Root.rect.height - self.Offset.MinHeight
	end
end

-- 新增加的ItemInfoViewItem需要在预设里的OpenWait中增加对应obj
function ItemInfoView:AutoToPos(is_flush,is_unchange)
	-- 增加新参数，持有该参数下不主动读取点击信息 20220628c
	local pos = is_unchange == nil and (self.fixed_pos or self.data:GetClickDataPos() or 
	GameMath.Vector2(PublicPopupData.Instance:ClickData().position)) or 
	GameMath.Vector2(self.InfoPosition.localPosition)

	local s_flag = self:IsEdgePosition() or not self.need_calcu_pos
	if s_flag then
		pos.y = 0

	-- 不加该参数会导致小型物品信息在开关获取途径时产生位移 20220629c
	elseif not is_unchange then 
		pos = pos + Vector2.New(self.Offset[self.InfoPosition.sizeDelta.x] or 0,-50)
	end
	if #self.item_list == 2 then
		pos = self.Offset.ItemList
	end
	self.InfoPosition.localPosition = pos
	UH.SetEdgePosition(self.InfoPosition,self.Root)
	if s_flag then 
		self.InfoPosition.localPosition = GameMath.Vector2({x=self.InfoPosition.localPosition.x,y=0})
	end
	if IS_EDITOR and is_flush and not s_flag and #self.item_list == 1 then
		self:FlushIsEdgePosition(self.InfoPosition,self.Root)
	end
	self.data.item_info.pos = GameMath.Vector2(self.InfoPosition.localPosition)
	if self.data.effect_show then
		self.data.effect_show = false
		self.CrystalEquip:Start()
	end
end

function ItemInfoView:FlushIsEdgePosition(obj,root)
	local edge_height = 40
    local pos = obj.localPosition
	if pos.y > (root.rect.height / 2 - obj.rect.height / 2 - edge_height) then
		UH.SetRectPosition(self.InfoPosition,{y = self.InfoPosition.localPosition.y - edge_height})
		UH.SetEdgePosition(self.InfoPosition,self.Root)
		self.data.item_info.pos = GameMath.Vector2(self.InfoPosition.localPosition)
	end
end

function ItemInfoView:GetNeedCalcPos(item)
	if item:IsStarGod() then
		return false
	else
		local item_type = item:Type()
		if item_type and (item_type == ItemType.Fashion or item_type == ItemType.Skin or item_type == ItemType.Appearance) then
			return false
		end
	end
	return true
end