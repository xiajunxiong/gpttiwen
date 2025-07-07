WorkshopQuickForgeBuy = WorkshopQuickForgeBuy or DeclareView("WorkshopQuickForgeBuy","workshop/workshop_quick_forgebuy")

function WorkshopQuickForgeBuy:WorkshopQuickForgeBuy()
    self.Board:SetData(self:GetType(),Language.Workshop.QuickForgeBuy,Vector2.New(678, 500))

end

function WorkshopQuickForgeBuy:LoadCallback(param_t)
    local neo_list = {}
    
    for k,v in pairs(param_t.item_list) do 
        local vo = {}
        vo.info = v.item
        vo.jump_need_num = v.lack
        vo.shop_data,vo.mod = self:CatchBuyWay(vo.info)
        LogDG("vo",vo)
        table.insert( neo_list, vo )
    end 
    self.item_list = neo_list
    self.cell_list:SetData(neo_list)
    
    self.target = Item.Init(param_t.target)

    for k,v in pairs(self.item_list) do 
        local is_require_limit = ReputationCtrl.IsRequire(v.shop_data.require_type,v.shop_data.require_value)
        local is_limit = v.shop_data.relation and (PrestigeData.Instance:GetPrestigeLevel(v.shop_data.village_id) < v.shop_data.relation)
        
        if is_limit or not is_require_limit then 
            PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.QuickForgeBuyReputationLack,v.info:Name()))
            ViewMgr:CloseView(WorkshopQuickForgeBuy)
            return     
        end 
    end 

    if WorkshopData.Instance:GetQuickForgeBuyTips() == true then 
        ViewMgr:CloseView(WorkshopQuickForgeBuy)
        self:OnClickConfirm()
        return 
    end 

    local price = {}
    for k,v in pairs(self.item_list) do 
        if v.shop_data.money_type ~= nil  then 
            if price[v.shop_data.money_type] == nil then price[v.shop_data.money_type] = 0 end 
            price[v.shop_data.money_type] = price[v.shop_data.money_type] + v.shop_data.price * v.jump_need_num
        end 

        if v.shop_data.price_type ~= nil  then 
            if price[v.shop_data.price_type] == nil then price[v.shop_data.price_type] = 0 end
            price[v.shop_data.price_type] = price[v.shop_data.price_type] + v.shop_data.price * v.jump_need_num
        end 
    end 

    local price_str = ""
    for k,v in pairs(price) do 
        price_str = price_str .. v..Language.Common.HuoBi[k]..(",")
    end 

    UH.SetText(self.desc, string.format(Language.Workshop.QuickForgeBuyDesc,ColorStr(price_str,COLORSTR.Green3)))
end 

function WorkshopQuickForgeBuy:CloseCallback()
    -- if self.delay_timer ~= nil then
    --     TimeHelper:CancelTimer(self.delay_timer)
    --     self.delay_timer = nil
    -- end
end 

-- 通过获取途径获取具体的购买信息
function WorkshopQuickForgeBuy:CatchBuyWay(item)
    return WorkshopData.Instance:CatchForgeBuyWay(item)
end

function WorkshopQuickForgeBuy:OnClickConfirm()

    local need_market_fun = function (buy_num)
        if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,buy_num) then
            PublicPopupCtrl.Instance:Center(Language.Common.ImmortalCoinLack)
            MallData.JumpItemNum = MarketData.Instance:GetNeedGoldNum(buy_num)
            return true 
        end
        return false
    end

    local need_shop_fun = function (buy_num)
        if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind,buy_num) then
            PublicPopupCtrl.Instance:Center(Language.Role.PopupTips7)
            return true 
        end
        return false
    end

    local total_price = 0
    local total_coin = 0

    for k,v in pairs(self.item_list) do 
        if v.shop_data.price_type ~= nil then 
            total_price = total_price + MarketData.Instance:GetItemPrice(v.info.item_id) * v.jump_need_num
        elseif v.shop_data.money_type ~= nil then 
            total_coin = total_coin + MarketData.Instance:GetItemPrice(v.info.item_id) * v.jump_need_num
        end 
        
        -- if v.shop_data.price_type ~= nil and need_market_fun(MarketData.Instance:GetItemPrice(v.info.item_id) * v.jump_need_num) then 
        --     return
        -- elseif v.shop_data.money_type ~= nil and need_shop_fun(MarketData.Instance:GetItemPrice(v.info.item_id) * v.jump_need_num) then 
        --     return
        -- end 
    end 

    if total_price > 0 and need_market_fun(total_price) then 
        return 
    end 

    if total_coin > 0 and need_shop_fun(total_coin) then 
        return 
    end 

    for k,v in pairs(self.item_list) do 
        if v.mod == Mod.Mall.Market then 
            MarketCtrl.Instance:SendMarketReq(1,v.info.item_id,v.jump_need_num)
        elseif v.mod == Mod.Mall.Shop then 
            ShopCtrl.Instance:SendGoldShopBuyReq(v.shop_data.index,v.jump_need_num)
        elseif v.mod == Mod.Reputation.Main then 
            RoleCtrl.Instance:CSPrestigeShopingReq(v.shop_data.index,v.jump_need_num)
        end 
    end 
    
    if self.target:IsEquip() then 
        WorkshopData.Instance:ForgingData().auto_create = true
    else
        WorkshopData.Instance:JewerlyData().auto_create = true
    end 

    ViewMgr:CloseView(WorkshopQuickForgeBuy)
end 

function WorkshopQuickForgeBuy:OnClickTick(flag)
    WorkshopData.Instance:SetQuickForgeBuyTips(flag)
end 

WorkshopQuickForgeItem =  WorkshopQuickForgeItem or DeclareMono("WorkshopQuickForgeItem", UIWidgetBaseItem)
function WorkshopQuickForgeItem:WorkshopQuickForgeItem() end

function WorkshopQuickForgeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.cell:SetData(self.data.info)
    UH.SetText(self.num,self.data.jump_need_num * self.data.shop_data.price)
    UH.SetIcon(self.huobi,HuoBi[self.data.shop_data.money_type == nil and self.data.shop_data.price_type or self.data.shop_data.money_type])

    UH.SetText(self.need_num,self.data.jump_need_num)
end