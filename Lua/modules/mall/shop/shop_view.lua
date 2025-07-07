ShopView = ShopView or DeclareMono("ShopView", UIWFlushPanel)

function ShopView:ShopView()
    self.data = ShopData.Instance
    self.data_cares = {
        {data = self.data.count_data,  func = self.FlushShopTimes,  init_call = false},
        {data = self.data.limit_buy_data,func = self.FlushShopTimes,init_call = false},
        {data = self.data.notify_data, func = self.FlushShopItem,   init_call = false},
        {data = self.data.select_data, func = self.FlushShopType,   init_call = true, keys = {"type"}},
        {data = self.data.select_data, func = self.FlushShopItem,   init_call = false,keys = {"list"}},
    }
end

--初始化
function ShopView:Awake()
    UIWFlushPanel.Awake(self)
    local data = self.data:GetTypeData()
    self.TypeList:SetData(data)
	self.TypeList:ClickItemData(data[self.data.select_data.type])
end

--刷新标签页
function ShopView:FlushShopType()
    local type = self.data:GetSelectType()
    local index = self.data:GetSelctItemIndex(type)
    self.ShopList:SetDataList(self.data:GetShopData(type),index)
    if index > 2 then
        self.ShopList:JumpVerticalIndex(math.ceil(index / 2),131)
    end
end

--刷新购买页面
function ShopView:FlushShopItem()
    local data = self.data:GetSelectData()
    UH.SetText(self.Name, data.item:Name())
    UH.SetText(self.Content,Format(Language.Mall.Content,data.item:Desc()))
    local gold_name = Language.Common.HuoBi[data.money_type]
    if data.old_price then
        UH.SetText(self.PPrice, Format(Language.Guild.Cost,data.price,gold_name))
        UH.SetText(self.OPrice, Format(Language.Guild.Price,data.old_price,gold_name))
    end
    for i=1,2 do
        UH.SetGold(self.Gold[i],data.money_type)
    end
    self.money_type,self.make_price,self.make_num = data.money_type,data.price,1
    UH.SetText(self.Price, self.make_price)
    self.Discount:SetActive(data.discount and data.discount > 0)
    self:FlushShopTimes()
    self:FlushNumView()
end

--刷新次数页面
function ShopView:FlushShopTimes()
    local data = self.data:GetSelectData()
    if data.limit_times > 0 then
        local has_times = data.limit_times - self.data:GetLimitTimes()
        local has_color = has_times == 0 and COLORSTR.Red12 or COLORSTR.Yellow13
        local limit_type = Language.Shop.LimitType[data.limit_type]
        UH.SetText(self.Times, Format("<color=#%s>%s：%s</color>",has_color,limit_type,has_times))
        self.TimesRect.sizeDelta = Vector2.New(335,data.discount > 0 and 77 or 38.5)
    end
    self.TimeView:SetActive(data.limit_times > 0)
    self.limit_times = data.limit_times
end

--刷新数量
function ShopView:FlushNumView()
    local total_price = self.make_num * self.make_price
    UH.SetText(self.Num, self.make_num)
    UH.SetText(self.TotalPrice, total_price)
    --是否不够元宝
    local num = RoleData.Instance:GetCurrency(self.money_type)
    UH.Color(self.TotalPrice, num < total_price and COLORS.Red10 or COLORS.White)
end

--数量改变
function ShopView:OnClickAddNum(num)
    if self.make_num + num <= 0 then return end
    if self.limit_times > 0 and self.make_num + num > self.limit_times - self.data:GetLimitTimes() then
        PublicPopupCtrl.Instance:Center(Language.Reputation.LimitNumTip)
        return
    end
    if self.make_num + num > GameEnum.SHOP_BUY_MAX_LIMIT_NUM then
        PublicPopupCtrl.Instance:Center(Language.Shop.LimitNumTip)
        return
    end
    self.make_num = self.make_num + num
    self:FlushNumView()
end

--点击数量
function ShopView:OnClickNumSelect()
    if self.NumSelect.text ~= "" then
        local num = tonumber(self.NumSelect.text)
        if num > 0 then
            if self.limit_times > 0 and num > self.limit_times - self.data:GetLimitTimes()then
                self.NumSelect.text = self.limit_times - self.data:GetLimitTimes()
                PublicPopupCtrl.Instance:Center(Language.Reputation.LimitNumTip)
                return
            end
            if num > GameEnum.SHOP_BUY_MAX_LIMIT_NUM then
                self.NumSelect.text = GameEnum.SHOP_BUY_MAX_LIMIT_NUM
                PublicPopupCtrl.Instance:Center(Language.Shop.LimitNumTip)
                return
            end
            self.make_num = num
            self:FlushNumView()
        else
            self.NumSelect.text = 1
        end
    end
end

--点击购买
function ShopView:OnClickPurchase()
    local data = self.data:GetSelectData()
    if data and self.limit_times > 0 and self.limit_times - self.data:GetLimitTimes() == 0 then
        local limit_type = Language.Shop.LimitType[data.limit_type]
        PublicPopupCtrl.Instance:Center(limit_type .. Language.Shop.NotHasTip)
        return
    end
    if data ~= nil then
        local gold_name = Language.Common.HuoBi[data.money_type]
        local gold_num = data.price * self.make_num
        PublicPopupCtrl.Instance:AlertTip(Format(Language.Shop.BuyItemTip,gold_num,gold_name,data.item:QuaName()),function()
            if self.data:IsLimitType() and self.data:IsTimeLimit() then
                ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIME_LIMIT_BUY,1,data.seq,self.make_num)
            else
                ShopCtrl.Instance:SendGoldShopBuyReq(data.index,self.make_num)
            end
        end,data.money_type)
    end
end

----------------------------ShopItem----------------------------
ShopItem = ShopItem or DeclareMono("ShopItem", UIWidgetBaseItem)
function ShopItem:ShopItem()
    self.shop_data = ShopData.Instance
end

function ShopItem:SetData(data)
    data.item = Item.New(data.reward_item or data)
    self.Call:SetData(data.item)
    UH.SetText(self.Num,data.price)
    UH.SetText(self.Name,data.item:Name())
    UH.SetGold(self.Gold,data.money_type)
    self.Limit:SetActive(data.limit_times > 0)
    if data.limit_times > 0 then
        UH.SetText(self.LimitNum,Format(Language.Shop.LimitNum,data.limit_times))
    end
    self.Label:SetActive(data.discount and data.discount > 0)
    UH.SetText(self.Discount,Format(Language.Shop.Discount,data.discount or 0))
    if self.shop_data:IsLimitType() and self.shop_data:IsTimeLimit() then
        self.Time:CreateActTime(ACTIVITY_TYPE.RAND_TIME_LIMIT_BUY,TimeType.Type_Special_4)
    end
    self.LimitTime:SetActive(self.shop_data:IsLimitType() and self.shop_data:IsTimeLimit())
    --检查并显示红点
    if data.limit_times > 0 then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.Root, Vector3.New(172,54,0)) 
        end
        self.red_point:SetNum(self.shop_data:GetNotifyFlag(data.index))
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ShopItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        self.shop_data:SetSelectData(self.data)
        if self.red_point ~= nil then
            if self.red_point:GetNum() ~= 0 then
                ShopCtrl.Instance:SendGoldShopNotifyCancelReq(self.data.index)
                ShopCtrl.Instance:SendGoldShopNotifyCancelReq(-1)
                self.red_point:SetNum(0)
            end
        end
    end
end

----------------------------ShopBtnItem----------------------------
ShopBtnItem = ShopBtnItem or DeclareMono("ShopBtnItem", UIWidgetBaseItem)

function ShopBtnItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function ShopBtnItem:OnClickItem()
	ShopData.Instance:SetSelectType(self.data.page)
end

function ShopBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end