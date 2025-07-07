BattlefieldShopView = BattlefieldShopView or DeclareView("BattlefieldShopView", "battlefield/battlefield_shop",Mod.Battlefield.Shop)

function BattlefieldShopView:OnClickClose()
    ViewMgr:CloseView(BattlefieldShopView)
end

function BattlefieldShopView:CloseCallback()
    BattlefieldData.Instance:InitShopSmartData()
end

--===================================BattlefieldShopPanel===================================
BattlefieldShopPanel = BattlefieldShopPanel or DeclareMono("BattlefieldShopPanel", UIWFlushPanel)

function BattlefieldShopPanel:BattlefieldShopPanel()
    self.data = BattlefieldData.Instance
    self.data_cares = {
        {data = RoleData.Instance.currencys,func = self.FlushItemView,init_call = false},
        {data = self.data.shop_select_data, func = self.FlushItemView, init_call = false,  keys = {"list"}},
        {data = StoreData.Instance.count_data,func = self.FlushItemView, init_call = false,keys = {"list"}}
    }
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickNumSelect, self))
end

function BattlefieldShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushListView()
end

function BattlefieldShopPanel:FlushListView()
    local list = self.data:GetShopListData()
    self.List:SetDataList(list,self:GetSelectIndex(list))
end

function BattlefieldShopPanel:GetSelectIndex(list)
    local data = self.data:GetShopSelectData()
    for k,v in pairs(list) do
        if v.seq == data.seq then
            return k
        end
    end
end

function BattlefieldShopPanel:FlushItemView()
    local data = self.data:GetShopSelectData()
    UH.SetText(self.Name, data.item:QuaName(true))
    UH.SetText(self.Content, Format(Language.Battlefield.ShopContent, data.item:Desc()))
    self.make_price,self.make_num = data.price,1
    self:FlushTimesView()
    self:FlushNumView()
end

function BattlefieldShopPanel:FlushTimesView()
    local data = self.data:GetShopSelectData()
    self.has_times = data.limit_convert_count - StoreData.Instance:GetLimitTimes(data.conver_type,data.seq)
    if data.limit_type > 0 then
        local has_color = self.has_times == 0 and COLORSTR.Red7 or COLORSTR.Green2
        local limit_type = Language.Battlefield.ShopLimitTips[data.limit_type]
        UH.SetText(self.Times, Format("%s：<color=#%s>%s</color>", limit_type,has_color,self.has_times))
        if self.has_times == 0 then
            self:FlushListView()
        end
    end
    local is_limit = MallCtrl.IsCanRequire(data.require_type,data.require_value)
    if is_limit then
        UH.SetText(self.Ranking,MallData.GetRequireValue(data.require_type,data.require_value))
    end
    self.Ranking:SetObjActive(is_limit == true)
    self.Times:SetObjActive(data.limit_type > 0)
    self.limit_type = data.limit_type
end

function BattlefieldShopPanel:FlushNumView()
    local total_price = self.make_num * self.make_price
    UH.SetText(self.Num, self.make_num)
    UH.SetText(self.Price, total_price)
    local num = RoleData.Instance:GetCurrency(CurrencyType.Battlefield)
    UH.SetText(self.TotalPrice, num)
    UH.Color(self.TotalPrice, num < total_price and COLORS.Red7 or COLORS.Green2)
end

function BattlefieldShopPanel:OnClickAddNum(num)
    if self.make_num + num <= 0 then
        return
    end
    if self.limit_type > 0 and self.make_num + num > self.has_times then
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
function BattlefieldShopPanel:OnClickNumSelect()
    if self.Num.text ~= "" then
        local num = tonumber(self.Num.text) or 0
        if num > 0 then
            if self.limit_type > 0 and num > self.has_times then
                local limit_num = self.has_times
                if limit_num <= 0 then
                    self.Num.text = 1
                else
                    self.Num.text = limit_num
                end
                return
            end
            if num > GameEnum.SHOP_BUY_MAX_LIMIT_NUM then
                self.Num.text = GameEnum.SHOP_BUY_MAX_LIMIT_NUM
                PublicPopupCtrl.Instance:Center(Language.Shop.LimitNumTip)
                return
            end
            self.make_num = num
            self:FlushNumView()
        else
            self.Num.text = 1
        end
    end
end

--点击购买
function BattlefieldShopPanel:OnClickPurchase()
    local data = self.data:GetShopSelectData()
    if data == nil then return end
    if MallCtrl.IsCanRequire(data.require_type,data.require_value) then
        PublicPopupCtrl.Instance:Center(Language.BattlefieldShop.RequireError[data.require_type])
        return
    end
    if self.limit_type > 0 and self.has_times == 0 then
        local limit_type = Language.Battlefield.ShopLimitTips[data.limit_type]
        PublicPopupCtrl.Instance:Center(limit_type .. Language.Shop.NotHasTip)
        return
    end
    data.make_num = self.make_num or 1
    local total_price = self.make_num * self.make_price
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Reputation.ReputationTip,total_price,Language.Battlefield.ShopGoldTip,self.make_num,data.item:QuaName()),
    function()
        ShoppingCtrl.Instance:SendStoreBuyReq(data)
        ViewMgr:CloseView(ReputationShopView)
    end)
end

----------------------------BattlefieldShopItem----------------------------
BattlefieldShopItem = BattlefieldShopItem or DeclareMono("BattlefieldShopItem", UIWidgetBaseItem)

function BattlefieldShopItem:SetData(data)
    data.item = Item.New(data)
    UH.SetText(self.Name, data.item:QuaName(true))
    UH.SetText(self.Num, data.price)
    self.Limit:SetActive(data.limit_type > 0)
    if data.limit_type > 0 then
        self.Inter.Interactable = data.has_times > 0
        self.Panel:SetActive(data.has_times > 0)
    else
        self.Inter.Interactable = true
        self.Panel:SetActive(true)
    end
    local is_limit = MallCtrl.IsCanRequire(data.require_type,data.require_value)
    if is_limit then
        UH.SetText(self.LimitName,Language.BattlefieldShop.RequireTips[data.require_type])
    end
    self.Ranking:SetActive(is_limit)
    data.item.is_grey = is_limit or (data.limit_type > 0 and data.has_times == 0)
    self.Call:SetData(data.item)
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldShopItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        BattlefieldData.Instance:SetShopSelectData(self.data)
    end
end