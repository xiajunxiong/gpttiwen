MoneyExchangeView = MoneyExchangeView or DeclareView("MoneyExchangeView", "mall/MoneyExchangeView")

function MoneyExchangeView:MoneyExchangeView()
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickNumSelect, self))
end

function MoneyExchangeView:LoadCallback(param_t)
    self.data = param_t.config or {}
    self.has_times = param_t.has_times
    self.comfirm_func = param_t.comfirm_func
    self.make_num = MallData.JumpItemNum or 1
    self:FlushItemView(Item.New({item_id = self.data.item_id}))
    local name = Language.Common.HuoBi[param_t.money_type]
    UH.SetText(self.Title,name.. Language.Exchange.Title)
    MallData.JumpItemNum = nil
    self.Text:SetActive(self.has_times ~= nil)

    self:OpenOther(param_t)
end

function MoneyExchangeView:OpenOther(param_t)
    self.ObjImmortalCoinShow:SetActive(false)
    if param_t.money_type == CurrencyType.ImmortalCoin then
        -- 灵玉基金跳转显示
        local is_show, desc = RewardData.Instance:GetImmortalFundIsShowInChange()
        if is_show then
            self.ObjImmortalCoinShow:SetActive(true)
            UH.SetText(self.TxtImmortalCoinTip, Format(Language.Exchange.ImmortalCoinTip, desc or 0))
        end
    end
end

function MoneyExchangeView:OnGoImmortalFundClick()
    ViewMgr:OpenViewByKey(Mod.Reward.ImmortalFund)
    ViewMgr:CloseView(MoneyExchangeView)
end

function MoneyExchangeView:FlushItemView(item)
    for i=1,2 do
        UH.SetGold(self.Gold[i],self.data.price_type)
    end
    self.Call:SetData(item)
    UH.SetText(self.Name,item:QuaName())
    UH.SetText(self.Content,item:Desc())
    UH.SetText(self.Price,self.data.price)
    UH.SetText(self.TotalPrice,self.data.price * self.make_num)
    UH.SetText(self.Num,self.make_num)
end

function MoneyExchangeView:FlushNumView(make_num)
    self.make_num = make_num or 1
    UH.SetText(self.Num,self.make_num)
    UH.SetText(self.TotalPrice,self.data.price * self.make_num)
end

function MoneyExchangeView:OnClickNumSelect()
    if self.Num.text ~= "" then
        self:OnClickAddNum(tonumber(self.Num.text) - self.make_num)
    end
end

function MoneyExchangeView:OnClickAddNum(num)
    if self.make_num + num <= 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[19])
        self:FlushNumView()
        return
    end
    if self.has_times and self.make_num + num > self.has_times then
        if self.has_times == 0 then
            PublicPopupCtrl.Instance:Center(Language.Shop.NotNumTip)
            return
        else
            PublicPopupCtrl.Instance:Center(Language.Shop.LimitTimesMaxTip)
            self:FlushNumView(self.has_times)
            return
        end
    end
    if self.make_num + num > MARKET_BUY_MAX_NUM then
        if self.IsMaxLimitNumTip == nil then
            self.IsMaxLimitNumTip = true
        else
            PublicPopupCtrl.Instance:Center(Language.Market.MaxLimitNumTip)
        end
        self:FlushNumView(MARKET_BUY_MAX_NUM)
        return
    end
    self:FlushNumView(self.make_num + num)
end

function MoneyExchangeView:OnClickPurchase()
    if self.has_times and self.has_times == 0 then
        PublicPopupCtrl.Instance:Center(Language.Shop.NotNumTip)
        return
    end
    if not MallCtrl.IsNotCurrency(self.data.price_type,self.data.price * self.make_num) then
        return
    end
    if self.comfirm_func then
        self.data.make_num = self.make_num
        self.comfirm_func(self.data)
    end
end

function MoneyExchangeView:OnClickClose()
    ViewMgr:CloseView(MoneyExchangeView)
end

function MoneyExchangeView:CloseCallback()

end