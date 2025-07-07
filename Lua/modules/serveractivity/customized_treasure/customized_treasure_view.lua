CustomizedTreasureView = CustomizedTreasureView or DeclareView("CustomizedTreasureView", "serveractivity/customized_treasure_view", Mod.Splendid.CustomizedTreasure)
VIEW_DECLARE_LEVEL(CustomizedTreasureView, ViewLevel.L0)
function CustomizedTreasureView:CustomizedTreasureView()
    local style_type = CustomizedTreasureData.Instance:GetShowStyle()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE), style_type)
    self:FlushTopView(style_type)
end

function CustomizedTreasureView:FlushTopView(index)
    for i = 1, self.Tops:Length() do
        self.Tops[i]:SetActive((index + 1) == i)
    end
end

function CustomizedTreasureView:OnClickClose()
	ViewMgr:CloseView(CustomizedTreasureView)
end

CustomizedTreasurePanel = CustomizedTreasurePanel or DeclareMono("CustomizedTreasurePanel", UIWFlushPanel)
function CustomizedTreasurePanel:CustomizedTreasurePanel()
	self.cur_page, self.min_page, self.max_page = 1, 1, 1
	self.data_cares = {
		{data = CustomizedTreasureData.Instance.buy_count_change, func = self.FreshTreasureListList, init_call = true},
		{data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
	}
	local function func_delay()
		self.open_timer = nil
		self:DoShowItem()
	end
	self.open_timer = Invoke(func_delay, 0.1)
end

function CustomizedTreasurePanel:Awake()
	UIWFlushPanel.Awake(self)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE, TimeType.Type_Time_3, Language.RAUstomizedBox.Txt2)
end

function CustomizedTreasurePanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.open_timer ~= nil then
		TimeHelper:CancelTimer(self.open_timer)
		self.open_timer = nil
	end
	if self.show_timer ~= nil then
		TimeHelper:CancelTimer(self.show_timer)
		self.show_timer = nil
	end
end

function CustomizedTreasurePanel:FreshTreasureListList()
	local base_list = CustomizedTreasureData.Instance:SortTreasureList()
	self.first_data = base_list[1]
	self.show_list = DataHelper.FormatList(base_list, 2)
	self.max_page = math.ceil(#self.show_list / 2)
	self:FlushPageItem()
	self:FlushFreeGift()
end

function CustomizedTreasurePanel:FlushPageItem()
	if self.show_list ~= nil then
		self.HotItem:SetData(self.first_data)
		local show_list = {}
		for i = 1, 2 do
			local data = self.show_list[(self.cur_page - 1) * 2 + i]
			if data ~= nil then
				table.insert(show_list, data)
			end
		end
		self.TreasureList:SetData(show_list)
	end
	self.PageLeft:SetActive(self.cur_page ~= self.min_page)
	self.PageRight:SetActive(self.cur_page ~= self.max_page)
end

function CustomizedTreasurePanel:OnClickPage(param)
	if (param == -1 and self.cur_page == self.min_page) or (param == 1 and self.cur_page == self.max_page) then
		return
	end
	self.cur_page = self.cur_page + param
	self:DoShowItem()
	self:FreshTreasureListList()
end

--播放物品动效
function CustomizedTreasurePanel:DoShowItem()
	if self.show_timer ~= nil then
		TimeHelper:CancelTimer(self.show_timer)
		self.show_timer = nil
	end
	for i = 1, #self.TreasureList.item_list do
		local item = self.TreasureList.item_list[i]
		if item ~= nil then
			item:DoShowNormal()
		end
	end
	self.show_index = 1
	self.show_timer = Invoke(BindTool.Bind(self.DoShowNext, self), 0.2)
end
function CustomizedTreasurePanel:DoShowNext()
	local item = self.TreasureList.item_list[self.show_index]
	if item ~= nil then
		item:DoShow()
	end
	self.show_index = self.show_index + 1
	if self.show_index <= #self.TreasureList.item_list then
		self.show_timer = Invoke(BindTool.Bind(self.DoShowNext, self), 0.1)
	else
		--隐藏显示其他物体，RectMask2D刷新，选中特效会闪, 所以在此显示出来
	end
end

function CustomizedTreasurePanel:SendBuy()
	ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_CUSTOMIZED_TREASURE_NEW, self.buy_money)
end

function CustomizedTreasurePanel:SaveMoney(buy_money)
	self.buy_money = buy_money
end

function CustomizedTreasurePanel:FlushFreeGift()
    self.FreeGiftNode:SetActive(not CustomizedTreasureData.Instance:IsFreeGiftFetched())
end

function CustomizedTreasurePanel:OnClickFreeGift()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE, 2)
end

TreasureListItem = TreasureListItem or DeclareMono("TreasureListItem", UIWidgetBaseItem)
function TreasureListItem:TreasureListItem()
	self.last_seq = -1
end

function TreasureListItem:OnDestroy()
	UIWidgetBaseItem.OnDestroy(self)
	if self.num_timer ~= nil then
		TimeHelper:CancelTimer(self.num_timer)
		self.num_timer = nil
	end
end

function TreasureListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	if self.last_seq ~= data.seq then
		if self.ItemList:GetData() ~= nil then
			self.ItemList:SelectAll(false)
		end
		self.last_seq = data.seq
	end
	UH.SetText(self.Multiple, tostring(self.data.multiple) .. "%")
	UH.SetText(self.BoxName, self.data.name_box)

	UH.SetText(self.PriceText, RechargeData.Instance:GoldToMoney(self.data.buy_money))
	UH.SetGold(self.PriceIcon, 999)

	local show_list = CustomizedTreasureData.Instance:GetSelectDataList(self.data.item_id)
	self.ItemList:SetData(show_list)
	if show_list[5] ~= nil then
		if self.ItemScrollRect.enabled ~= true then
			self.ItemScrollRect.enabled = true
			self.ItemScrollRect.verticalNormalizedPosition = 1
		end
		self.ItemLayoutGroup.padding.top = 0
	else
		if self.ItemScrollRect.enabled ~= false then
			self.ItemScrollRect.enabled = false
			self.ItemScrollRect.verticalNormalizedPosition = 1
		end
		self.ItemLayoutGroup.padding.top = 30
	end
	local buy_count = CustomizedTreasureData.Instance:GetTreasureBuyCount(self.data.seq)
	local all_count = CustomizedTreasureData.Instance:GetTreasureCanBuyCount(self.data.seq)
	local have_count = all_count - buy_count
	local is_over = have_count <= 0

	local color = is_over and COLORSTR.Red7 or COLORSTR.Green2
	local str = data.limit_type == 1 and Language.RAUstomizedBox.Count1 or Language.RAUstomizedBox.Count2
	UH.SetText(self.Times, string.format(str, color, have_count, all_count))
	self.ImgOver:SetActive(is_over)
	UH.SetInter(self.BtnBuyInter, not is_over)

	UH.SetText(self.MaxNum, Format(Language.RAUstomizedBox.CanSelectNum, self.data.reward_num))
	TimeHelper:CancelTimer(self.num_timer)
	self.num_timer = TimeHelper:AddDelayTimer(BindTool.Bind1(self.DelayFlushNum, self), 0.3)
end

function TreasureListItem:DelayFlushNum()
	self.num_timer = nil
	self:FlushNum()
end

--播放动画
function TreasureListItem:DoShowNormal()
	self.Pos:SetActive(false)
end
function TreasureListItem:DoShow()
	self.Pos:SetActive(true)
end

function TreasureListItem:OnToggleChange(param)
	self:FlushNum()
	local select_num = #self:GetSelectedData()
	if select_num >= self.data.reward_num then
		for i, v in pairs(self.ItemList.item_list) do
			v:SetToggleEnable(v:IsSelected())
		end
	else
		for i, v in pairs(self.ItemList.item_list) do
			v:SetToggleEnable(true)
		end
	end
end

function TreasureListItem:ShowEffect()
	-- effect_id = 10010353
	-- effect_id = 10010352
	-- effect_id = 10010348
	-- effect_id = 10010349
	-- effect_id = 10010350
	-- effect_id = 10010351
end

function TreasureListItem:FlushNum()
	local select_num = #self:GetSelectedData()
	local color = select_num >= self.data.reward_num and COLORSTR.Red7 or COLORSTR.Green2
	UH.SetText(self.SelectNum, Format(Language.RAUstomizedBox.SelectNum, color, select_num, self.data.reward_num))
end

function TreasureListItem:OnClickBuy()
	local have_count = CustomizedTreasureData.Instance:GetTreasureHaveCount(self.data.seq)
	if have_count <= 0 then
		PublicPopupCtrl.Instance:Error(Language.RAUstomizedBox.Limit2)
		return
	end
	local select_data = self:GetSelectedData()
	if #select_data ~= self.data.reward_num then
		PublicPopupCtrl.Instance:Error(Language.RAUstomizedBox.Limit)
		return
	end
	local buy_bit = bit:d2b(0)
	for i, v in pairs(select_data) do
		buy_bit[32 - v.type] = 1
	end
	local param1 = self.data.seq
	local param2 = bit:b2d(buy_bit)
	local fee = self.data.buy_money / 10
	ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_CUSTOMIZED_TREASURE_NEW,fee,ACTIVITY_TYPE.RAND_CUSTOMIZED_TREASURE, 1, param1, param2, 0)
	self.MainPanel:SaveMoney(self.data.buy_money)
end

function TreasureListItem:GetSelectedData()
	return self.ItemList:GetSelectedData()
end

TreasureToggleItem = TreasureToggleItem or DeclareMono("TreasureToggleItem", UIWidgetBaseItem)
function TreasureToggleItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.New(TableCopy(data.reward_item))
	self.ItemCell:SetData(item_data)
end

function TreasureToggleItem:SetToggleEnable(enable)
	UH.SetEnabled(self.Selector, enable)
end
