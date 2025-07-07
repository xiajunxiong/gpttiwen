
FightShopView = FightShopView or DeclareView("FightShopView", FightConfig.ResPath .. "fight_shop")
VIEW_DECLARE_LEVEL(FightShopView,ViewLevel.L1)

function FightShopView:FightShopView()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight
end

function FightShopView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), self.language.FightShop.ShopName)
	self.Currency:DefaultSet()

	self.TypeList:SetDataList({Language.Reputation.TitleBtnList})
end

function FightShopView:CloseCallback()
	self.data.cur_sel_shop_item:Set{}
end

FightShopViewList = FightShopViewList or DeclareMono("FightShopViewList", UIWFlushPanel)

function FightShopViewList:FightShopViewList()
    self.data = FightCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data.fight_shop_info, func = self.FlushItems},
	}
end

function FightShopViewList:FlushItems()
	local item_list = self.data:GetNPCShopInfo()
	local is_empty = 0 == #item_list
	self.EmptyObj:SetActive(is_empty)
	self.DetailObj:SetActive(not is_empty)
	self.ItemList:SetDataList(item_list, self.data.cur_sel_shop_item.index or 1)
end

FightShopViewItem = DeclareMono("FightShopViewItem", UIWidgetBaseItem)

function FightShopViewItem:FightShopViewItem()
	self.fight_data = FightData.Instance
end

function FightShopViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Cell:SetData(data)
	local shop_info = data.shop_info
	self.Name.text = data:Name()
	self.Price.text = shop_info.price
	self.LimitNum.text = string.format(Language.Shop.LimitNum, shop_info.max_num)
	self.Discount.text = string.format(Language.Shop.Discount, shop_info.discount / 10)
	UH.SetGold(self.Type, shop_info.money)
end

function FightShopViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.fight_data.cur_sel_shop_item:Set(self.data)
    end
end

FightShopViewTypeItem = FightShopViewTypeItem or DeclareMono("FightShopViewTypeItem", UIWidgetBaseItem)

function FightShopViewTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.Type.text = data
end

function FightShopViewTypeItem:OnClickItem()
end

function FightShopViewTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

FightShopViewDetail = FightShopViewDetail or DeclareMono("FightShopViewDetail", UIWFlushPanel)

function FightShopViewDetail:FightShopViewDetail()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight.FightShop

    self.data_cares = {
		{data = self.data.cur_sel_shop_item, func = self.FlushDetails, init_call = false},
	}

	self.InputField.onValueChanged:AddListener(function ()
		local num = tonumber(self.InputField.text) or 1
		num = num < 1 and 1 or num
		if self.max_num >= num then
			self.cur_num = num
		elseif num > 1 then
			PublicPopupCtrl.Instance:Center(Language.Reputation.LimitNumTip)
			self.cur_num = self.max_num
		end
		self:FlushCurNum()
	end)
end

function FightShopViewDetail:FlushDetails()
	local item = self.data.cur_sel_shop_item:Val()
	if nil == next(item) then return end
	local shop_info = item.shop_info
	self.Name.text = item:QuaName()
	self.Desc.text = string.format(Language.Mall.Content, item:Desc())
	self.LimitNum.text = shop_info.buy_num > 0 and RichTextHelper.ColorStr(string.format(self.language.LimitNum, Language.Shop.LimitType[1], shop_info.buy_num), COLORSTR.Yellow13) or RichTextHelper.ColorStr(self.language.LimitEmpty, COLORSTR.Red10)
	self.price = shop_info.price
	self.cur_num = 1
	self.max_num = shop_info.buy_num
	self.PerPrice.text = self.price
	self:FlushCurNum()
	for i = 1, 2 do
        UH.SetGold(self.Types[i], shop_info.money)
    end
end

function FightShopViewDetail:FlushCurNum()
	self.InputField.text = self.cur_num
	self.TotalPrice.text = self.cur_num * self.price
end

function FightShopViewDetail:OnClickAdd()
	if self.max_num > self.cur_num then
		self.cur_num = self.cur_num + 1
		self:FlushCurNum()
	else
		PublicPopupCtrl.Instance:Center(Language.Reputation.LimitNumTip)
	end
end

function FightShopViewDetail:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		self:FlushCurNum()
	end
end

function FightShopViewDetail:OnClickBuy()
	local item = self.data.cur_sel_shop_item:Val()
	local shop_info = item.shop_info
	if shop_info then
		SceneCtrl.Instance:SendNPCShowBuyReq(shop_info.obj_id, shop_info.buy_index, self.cur_num)
	end
end