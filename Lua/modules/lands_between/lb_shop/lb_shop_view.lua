LBShopView = LBShopView or DeclareView("LBShopView", "lb_shop/lb_shop",Mod.LandsBetweenExtra.Shop)
function LBShopView:LBShopView()

end

function LBShopView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(),Language.LBShop.ViewTitle)
	self.Board:SetTabbarVisible(true)
    local tabs = {}
    local params = {
        {name = Language.LBShop.Tag[0], tab_objs = {self.TabShop}},
        {name = Language.LBShop.Tag[1], tab_objs = {self.TabBuffShop}},
        {name = Language.LBShop.Tag[2], tab_objs = {self.tabExchangeShop}},
    }

    if  param_t.is_out then LBShopData.Instance:SetOpenMark(true) end 
    local is_open = LandsBetweenData.Instance:IsOpen()

    for i = 1,3 do 
        if not param_t.is_out and is_open and i < 3 then 
            table.insert(tabs,params[i])
        elseif i == 3 then 
            table.insert(tabs,params[i])
        end 
    end
    self.Board.Tabbar1:SetData(tabs)
    self.Board.Tabbar1:ClickItem(1)
    self.Currency:SetData(CurrencyType.ImmortalCoin, CurrencyType.Gold)
end 

function LBShopView:CloseCallback()
    LBShopData.Instance:SetOpenMark(false)
end

function LBShopView:OnClickTabbar(data)
    if data.name == Language.LBShop.Tag[0] or data.name == Language.LBShop.Tag[1] then 
        self.Currency:SetData(CurrencyType.ImmortalCoin, CurrencyType.Gold)
    end 
end

------------------- 商店分页 -----------------------
LBShopPanel = LBShopPanel or DeclareMono("LBShopPanel", UIWFlushPanel)
function LBShopPanel:LBShopPanel()
    self.data = LBShopData.Instance

    self.data_cares = {
        {data = self.data.shop_info, func = self.FlushPanel},
    }

    self.box_red = self.box_red or UiRedPoint.New(self.BoxRedPos, Vector3.New(0,0,0))
end 

function LBShopPanel:Awake()
    UIWFlushPanel.Awake(self)

   
end

function LBShopPanel:FlushPanel()
    local param = self.data:GetShopParam()

    self.ItemList:SetData(param.shop_list)
    UH.SetText(self.FlushCost, param.flush_cost)
    UH.SetText(self.FlushTime, param.flush_time)
    UH.SetIcon(self.FlushIcon, param.flush_icon)

    self.FlushComplete:SetActive(param.flush_done)
    self.BtnFlush:SetActive(not param.flush_done)

    self.flush_cost = param.flush_cost
    self.flush_type = param.flush_type
    self.flush_done = param.flush_done
    self.FlushTimer:StampTime(param.next_flush_time,TimeType.Type_Time_0,Language.LBShop.FlushTimerDesc)
    
    self.box_red:SetNum(param.reward_num)
    self.box_enable = param.reward_num == 1
end

function LBShopPanel:OnClickShopFlush()
    if self.flush_done then 
        PublicPopupCtrl.Instance:Center(Language.LBShop.FlushDoneTips)
        return 
    end 

    -- 不够钱
    if RoleData.Instance:GetCurrency(self.flush_type) < self.flush_cost then 
        PublicPopupCtrl.Instance:Center(FormatEnum.CurrencyType[self.flush_type]..Language.LBShop.CurrencyLackTips)
        return 
    end 

    LBShopCtrl.Instance:ReqFlushShop()
end

function LBShopPanel:OnClickReward()
    if not self.box_enable then 
        PublicPopupCtrl.Instance:Center(Language.LBShop.DailyRewardTips)
        return 
    end 

    LBShopCtrl.Instance:ReqDailyShopReward()
end

function LBShopPanel:OnClickShopCell(data)
    if data.buyed then 
        PublicPopupCtrl.Instance:Center(Language.LBShop.ShopItemBuyedTips)
        return 
    end 

    -- 不够钱
    if RoleData.Instance:GetCurrency(data.price_type) < data.now_price then 
        PublicPopupCtrl.Instance:Center(FormatEnum.CurrencyType[data.price_type]..Language.LBShop.CurrencyLackTips)
        return 
    end 

    LBShopCtrl.Instance:ReqBuyShopItem({index = data.index})
end

LBShopCell = LBShopCell or DeclareMono("LBShopCell", UIWidgetBaseItem)
function LBShopCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.Calls:SetData(data.item_call)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.OriginPrice, "x"..data.origin_price)
    UH.SetText(self.NowPrice, "x"..data.now_price)


    UH.SpriteName(self.OriginPriceType, Item.GetIconId(data.origin_price_type))
    UH.SpriteName(self.NowPriceType, Item.GetIconId(data.now_price_type))

    self.BtnBuy.Interactable = not data.buyed

    self.ShowOriginPrice:SetActive(data.with_discount)
    self.Discount:SetActive(data.with_discount)
    UH.SetText(self.DiscountText, data.discount_str)

end

----------- buff商店分页 ------------------
LBBuffShopPanel = LBBuffShopPanel or DeclareMono("LBBuffShopPanel", UIWFlushPanel)
function LBBuffShopPanel:LBBuffShopPanel()
    self.data = LBShopData.Instance

    self.data_cares = {
        {data = self.data.buff_shop_info, func = self.FlushPanel},
        {data = self.data.buff_shop_oper, func = self.FlushPanel,init_call = false},
    }

    self.BuffList:SetCompleteCallBack(function()
        if self.init == nil then 
            self.BuffList:ClickItem(1)
            self.init = true
        end 
    end)

    self.mark = nil
end 

function LBBuffShopPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function LBBuffShopPanel:FlushPanel()
    if self.mark ~= nil then 
        PublicPopupCtrl.Instance:Center(string.format(Language.LBShop.BuffMark,self.mark.name))
        self.mark = nil
    end 
    local param = self.data:GetBuffShopParam()

    self.OperShow:SetActive(param.can_show)
    self.OperEmpty:SetActive(not param.can_show)

    if param.can_show then 
        self.Calls:SetData(param.item_call)
        UH.SetText(self.Name, param.name)
        UH.SetText(self.Desc, param.desc)
        UH.SetText(self.Times, param.times_str)
        UH.SetText(self.BuyTimes, param.buy_times)
        UH.SetIcon(self.PriceType, Item.GetIconId(param.price_type))
        UH.SetText(self.Class,param.class_name)
        UH.SetText(self.BuyPrice,param.buy_price)
    end 

    self.BuffList:SetData(param.buff_list)
end 

function LBBuffShopPanel:OnClickAdd(detla)
    self.data:ChangeBuffBuyTimes(detla)
end

function LBBuffShopPanel:OnClickBuy()
    local send_data = self.data:GetBuyBuffParam()

    if send_data == nil then
        return 
    end

    if send_data.all_time_done then 
        PublicPopupCtrl.Instance:Center(send_data.limit_str)
        return 
    end 

    -- 不够钱
    if RoleData.Instance:GetCurrency(send_data.price_type) < send_data.price then 
        PublicPopupCtrl.Instance:Center(FormatEnum.CurrencyType[send_data.price_type]..Language.LBShop.CurrencyLackTips)
        return 
    end 

    self.mark = send_data
    LBShopCtrl.Instance:ReqBuyBuff({seq = send_data.seq , num = send_data.num})
end

function LBBuffShopPanel:OnClickBuffCell(data)
    self.data:SetOperItem(data)
end

LBShopBuffCell = LBShopBuffCell or DeclareMono("LBShopBuffCell", UIWidgetBaseItem)
function LBShopBuffCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.Calls:SetData(data.item_call)
    UH.SetText(self.Name, data.name)
    UH.SetIcon(self.PriceType, Item.GetIconId(data.price_type))
    UH.SetText(self.LimitStr, data.limit_str)
    self.Done:SetActive(data.all_time_done)
    self.LimitStr.gameObject:SetActive(not data.all_time_done)
    UH.SetText(self.PriceStr, "x"..data.price)

end

function LBShopBuffCell:Click()
    if self.ItemClick ~= nil then 
        self.ItemClick:Trigger()
    end 
end


----------- 兑换商店分页 ------------------
LBExchangeShopPanel = LBExchangeShopPanel or DeclareMono("LBExchangeShopPanel", UIWFlushPanel)
function LBExchangeShopPanel:LBExchangeShopPanel()
    self.data = LBShopData.Instance

    self.data_cares = {
        {data = StoreData.Instance.count_data,func = self.FlushPanel, init_call = false},
        {data = self.data.exchange_oper ,func = self.FlushPanel, init_call = false}
    }

    self.ExchangeList:SetCompleteCallBack(function()
        if self.init == nil then 
            self.ExchangeList:ClickItem(1)
            self.init = true
        end 
    end)
end 

function LBExchangeShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    
    local is_open = LandsBetweenData.Instance:IsOpen() and not self.data:GetOpenMark()
    self.TagBtnTypeGod.gameObject:SetActive(is_open )
    self.wait_timer = TimeHelper:AddDelayFrameTimer(function ()
        if is_open then 
            self.TagBtnTypeGod:Trigger()
        else
            self.TagBtnTypeDragon:Trigger()
        end 
    end,1)
end

function LBExchangeShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.wait_timer ~= nil then
        TimeHelper:CancelTimer(self.wait_timer)
        self.wait_timer= nil
    end
end

function LBExchangeShopPanel:FlushPanel()
    local param = self.data:GetExchangeShopParam()

    self.OperShow:SetActive(param.can_show)
    self.OperEmpty:SetActive(not param.can_show)

    if param.can_show then 
        self.Calls:SetData(param.item_call)
        UH.SetText(self.Name, param.name)
        UH.SetText(self.Desc, param.desc)
        UH.SetText(self.Times, param.times_str)
        UH.SetText(self.BuyTimes, param.buy_times)
        UH.SetIcon(self.PriceType, Item.GetIconId(param.price_type))
        UH.SetText(self.ItemDesc,param.item_desc)
        UH.SetText(self.BuyPrice,param.buy_price)
    end 

    self.ExchangeList:SetData(param.exchange_list)
end 

function LBExchangeShopPanel:OnClickTagOne() 
    self.init = nil

    self.Currency:SetData(CurrencyType.ImmortalCoin, CurrencyType.Gold,CurrencyType.GodSpaceExploits,true)
    -- self:OnClickTag(1)
end 
function LBExchangeShopPanel:OnClickTagTwo() 
    self.init = nil

    self.Currency:SetData(CurrencyType.ImmortalCoin, CurrencyType.Gold,CurrencyType.FairyDragon,true)
    -- self:OnClickTag(2)
end 

function LBExchangeShopPanel:OnClickTag(type)
    self.data:ChangeExchangeOperType(type)
end

function LBExchangeShopPanel:OnClickAdd(detla)
    self.data:ChangeExchangeBuyTimes(detla)
end

function LBExchangeShopPanel:OnClickBuy()
    local send_data = self.data:GetBuyExchangeParam()

    if send_data == nil then
        return 
    end

    if send_data.all_time_done then 
        PublicPopupCtrl.Instance:Center(send_data.limit_str)
        return 
    end 

    -- 不够钱
    if RoleData.Instance:GetCurrency(send_data.price_type) < send_data.price then 
        PublicPopupCtrl.Instance:Center(FormatEnum.CurrencyType[send_data.price_type]..Language.LBShop.CurrencyLackTips)
        return 
    end 

    ShoppingCtrl.Instance:SendShopBuyReq(send_data.convert_type,send_data.buy_seq,send_data.item_id,send_data.num)
end

function LBExchangeShopPanel:OnClickExchangeCell(data)
    self.data:SetExchangeOperItem(data)
end

LBShopExchangeCell = LBShopExchangeCell or DeclareMono("LBShopExchangeCell", UIWidgetBaseItem)
function LBShopExchangeCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.Calls:SetData(data.item_call)
    UH.SetText(self.Name, data.name)
    UH.SetIcon(self.PriceType, Item.GetIconId(data.price_type))
    UH.SetText(self.LimitStr, data.limit_str)
    self.Done:SetActive(data.all_time_done)
    self.LimitStr.gameObject:SetActive(not data.all_time_done)
    UH.SetText(self.PriceStr, "x"..data.price)

end


function LBShopExchangeCell:Click()
    if self.ItemClick ~= nil then 
        self.ItemClick:Trigger()
    end 
end

