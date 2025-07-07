ExchangeView = ExchangeView or DeclareMono("ExchangeView", UIWFlushPanel)

function ExchangeView:ExchangeView()
    self.data = ExchangeData.Instance
    self.data_cares = {
        {data = self.data.count_data,  func = self.FlushTimesView,   init_call = false},
        {data = self.data.select_data, func = self.FlushExchangeType,init_call = true, keys = {"type"}},
        {data = self.data.select_data, func = self.FlushExchangeItem,init_call = false,keys = {"list"}},
        {data = RoleData.Instance.currencys,func = self.FlushCurrencyView},
        {data = PrestigeData.Instance.shop_info,func = self.FlushTimesView,init_call = false},
        {data = PrestigeData.Instance.common_shop_info,func = self.FlushTimesView, init_call = false},
    }
end

--初始化
function ExchangeView:Awake()
    UIWFlushPanel.Awake(self)
    self.TypeList:SetDataList(self.data:GetTypeData(),self.data:GetSelectType())
    self:FlushChivalrousView()
end

--切换按钮
function ExchangeView:FlushExchangeType()
    local type = self.data:GetSelectType()
    local index = self.data:GetSelctItemIndex(type)
    self.ExchangeList:SetDataList(self.data:GetPageDataList(type),index)
    if index > 2 then
        self.ExchangeList:JumpVerticalIndex(math.ceil(index / 2),118)
    end
    self.ChivalrousView:SetActive(type == ExchangePage.Chivalrous)
    UH.SetText(self.ShowText,Language.Exchange.ShowText[type])
    self.ListRect.sizeDelta = Vector2.New(740,type == ExchangePage.Chivalrous and 468 or 524)
end

--刷新兑换页面
function ExchangeView:FlushExchangeItem()
    local data = self.data:GetSelectData()
    UH.SetText(self.Name, data.item:Name())
    UH.SetText(self.Content,Format(Language.Mall.Content,data.item:Desc()))
    for i=1,2 do
        UH.SetGold(self.Gold[i],data.integral_type or data.money_type,data.integral_type and {[0] = 65537})
    end
    self.make_price,self.make_num = data.integral_price or data.price,1
    self.money_type = data.integral_type or data.money_type
    UH.SetText(self.Price, self.make_price)
    self:FlushTimesView()
    self:FlushNumView()
end

--刷新次数页面
function ExchangeView:FlushTimesView()
    local data = self.data:GetSelectData()
    if data.limit_times > 0 then
        local has_times = data.limit_times - self.data:GetLimitTimes()
        local has_color = has_times == 0 and COLORSTR.Red10 or COLORSTR.Yellow13
        local limit_type = Language.Shop.LimitType[data.limit_type]
        UH.SetText(self.Times, Format("<color=#%s>%s：%s</color>",has_color,limit_type,has_times))
        self.TimesRect.sizeDelta = Vector2.New(335,data.relation and 77 or 38.5)
    end
    if data.relation then
        local pData = PrestigeData.Instance:GetPrestigeUpgradeData(data.relation,1)
        local pLevel = PrestigeData.Instance:GetPrestigeLevel(1)
        local pColor = pLevel >= data.relation and COLORSTR.Yellow13 or COLORSTR.Red12
        UH.SetText(self.LimitLevel,Format(Language.Exchange.LimitLevel,pColor,pData.name or ""))
    end
    self.RequireView:SetActive(data.relation ~= nil)
    self.TimesView:SetActive(data.limit_times > 0)
    self.limit_times = data.limit_times
end

--刷新数量
function ExchangeView:FlushNumView()
    local total_price = self.make_num * self.make_price
    UH.SetText(self.Num, self.make_num)
    UH.SetText(self.TotalPrice, total_price)
    --是否不够元宝
    if self.money_type == 0 then
        self.money_type = CurrencyType.Chivalrous
    end
    local num = RoleData.Instance:GetCurrency(self.money_type)
    UH.Color(self.TotalPrice, num < total_price and COLORS.Red10 or COLORS.White)
end

--刷新侠义值相关
function ExchangeView:FlushChivalrousView()
    UH.SetText(self.Chivalrous, self.data:GetChivalrous())
    local knightly_list = TeamCtrl.Instance:Data():GetKnightlyList()
    for i=1,3 do
        local data = knightly_list[i] or {}
        local knightly = Language.Exchange.Knightly[i]
        UH.SetText(self.Knightly[i],Format(knightly,data.vo.times,data.info.day_limit))
    end
end

function ExchangeView:FlushCurrencyView()
    UH.SetText(self.ChivalrouValue, RoleData.Instance:GetCurrency(CurrencyType.Chivalrous))
end

--数量改变
function ExchangeView:OnClickAddNum(num)
    if self.make_num + num <= 0 then return end
    if self.limit_times > 0 and self.make_num + num > self.limit_times - self.data:GetLimitTimes()then
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
function ExchangeView:OnClickNumSelect()
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

--点击兑换
function ExchangeView:OnClickPurchase()
    local type,data = self.data:GetSelectType(),self.data:GetSelectData()
    if data == nil then
        return
    end
    if self.limit_times > 0 and self.limit_times - self.data:GetLimitTimes() == 0 then
        local limit_type = Language.Shop.LimitType[data.limit_type]
        PublicPopupCtrl.Instance:Center(limit_type .. Language.Shop.NotHasTip)
        return
    end
    --声望商店需要判断等级才可以购买
    if data.relation and PrestigeData.Instance:GetPrestigeLevel(1) < data.relation then
        PublicPopupCtrl.Instance:Center(Language.Exchange.NotHasLimitLevel)
        return
    end   
    local exchange_type = data.integral_type and Language.Exchange.ExchangeType[data.integral_type] or Language.Common.HuoBi[data.money_type]
    local total_price = self.make_num * self.make_price
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Exchange.ExchangeTip,total_price,exchange_type,Language.Exchange.ShowText[type],self.make_num,data.item:QuaName()),function()
        if type == ExchangePage.Chivalrous then
            ExchangeCtrl.Instance:SendExchangeReq(data.seq,self.make_num)
        elseif type == ExchangePage.Grocery then
            RoleCtrl.Instance:CSShopBuyReq(data.index,self.make_num)
        else
            RoleCtrl.Instance:CSPrestigeShopingReq(data.index,self.make_num)
        end
    end)
end

-- --刷新村子
-- function ExchangeView:FlushExchangeVillage()
--     self.PrestigeList:SetDataList(self.data:GetPrestigeData())
-- end

function ExchangeView:OnClickAdd(type)
end

----------------------------ExchangeBtnItem----------------------------
ExchangeBtnItem = ExchangeBtnItem or DeclareMono("ExchangeBtnItem", UIWidgetBaseItem)
function ExchangeBtnItem:ExchangeBtnItem()
end

function ExchangeBtnItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function ExchangeBtnItem:OnClickItem()
	ExchangeData.Instance:SetSelectType(self.data.page)
end

function ExchangeBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

----------------------------ExchangeItem----------------------------
ExchangeItem = ExchangeItem or DeclareMono("ExchangeItem", UIWidgetBaseItem)
function ExchangeItem:ExchangeItem()
end

function ExchangeItem:SetData(data)
    data.item = Item.New(data)
    self.Call:SetData(data.item)
    UH.SetText(self.Name,data.item:Name())
    UH.SetGold(self.Gold,data.integral_type or data.money_type,data.integral_type and {[0] = 65537})
    UH.SetText(self.Num,data.integral_price or data.price)
    self.Limit:SetActive(data.limit_times > 0)
    if data.limit_times > 0 then
        UH.SetText(self.LimitNum,Format(Language.Shop.LimitNum,data.limit_times))
    end
    UIWidgetBaseItem.SetData(self, data)
end

function ExchangeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        ExchangeData.Instance:SetSelectData(self.data)
    end
end

--=============================村子拓展可用，目前是村子为1的==============================
-- ----------------------------PrestigeBtnItem----------------------------
-- PrestigeBtnItem = PrestigeBtnItem or DeclareMono("PrestigeBtnItem", UIWidgetBaseItem)
-- function PrestigeBtnItem:PrestigeBtnItem()
-- end

-- function PrestigeBtnItem:SetData(data)
--     UH.SetText(self.Name,data.name)
--     UIWidgetBaseItem.SetData(self, data)
-- end

-- function PrestigeBtnItem:OnSelect(selected)
--     UIWidgetBaseItem.OnSelect(self,selected)
--     if selected then
--         ExchangeData.Instance:SetSelectVillage(self.data.village_id)
--     end
-- end

-- ----------------------------PrestigeItem----------------------------
-- PrestigeItem = PrestigeItem or DeclareMono("PrestigeItem", UIWidgetBaseItem)
-- function PrestigeItem:PrestigeItem()
-- end

-- function PrestigeItem:SetData(data)
--     data.item = Item.New(data)
--     self.Call:SetData(data.item)
--     UH.SetText(self.Name,data.item:Name())
--     UH.SetText(self.Num,data.price)
--     UH.SetGold(self.Gold,data.money_type)
--     UIWidgetBaseItem.SetData(self, data)
-- end

-- function PrestigeItem:OnSelect(selected)
--     UIWidgetBaseItem.OnSelect(self,selected)
--     if selected then
--         ExchangeData.Instance:SetSelectData(self.data)
--     end
-- end