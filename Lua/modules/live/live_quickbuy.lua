LiveQuickBuy = LiveQuickBuy or DeclareView("LiveQuickBuy", "live/live_quickbuy")
function LiveQuickBuy:LiveQuickBuy()

end

-- param_t : {item_list}
function LiveQuickBuy:LoadCallback(param_t)
    param_t = param_t or {}
    local item_list = param_t.item_list or {}
    self.CellList:SetData(item_list)
    local tip = self:GetTipStr(item_list)
    UH.SetText(self.Tip, tip)

    if param_t.wait_quick then 
        self:OnOkClick()
    end 
end

function LiveQuickBuy:GetTipStr(item_list)
    local tip = ""
    local item_infos = {}
    for i,v in pairs(item_list) do
        if not item_infos[v.item_id] then
            item_infos[v.item_id] = {}
            item_infos[v.item_id].num = v:Num()
            item_infos[v.item_id].name = v:QuaName()
            item_infos[v.item_id].item_id = v.item_id
        else
            item_infos[v.item_id].num = item_infos[v.item_id].num + v:Num()
        end
    end
    local curr_type_list = {}
    for i,v in pairs(item_infos) do
        local str = string.format(Language.Live.Quickbuy.Txt1, v.num, v.name)
        tip = (tip ~= "" ) and tip..Language.Live.Quickbuy.Txt3 or tip..Language.Live.Quickbuy.Txt4
        tip = tip .. str
        local info = ShopData.GetFastBuyData(v.item_id)
        if info then
            if not curr_type_list[info.money_type] then
                curr_type_list[info.money_type] = 0
            end
            if info.money_type == CurrencyType.ImmortalCoin then 
                curr_type_list[info.money_type] = curr_type_list[info.money_type] + (MarketData.Instance:GetItemPrice(info.item_id) * v.num)
            else 
                curr_type_list[info.money_type] = curr_type_list[info.money_type] + (info.price * v.num)
            end 
            
        end
    end

    local need_str = ""
    for i,v in pairs(curr_type_list) do
        local curr_name = FormatEnum.CurrencyType[i]
        local num = v
        need_str = need_str .. string.format("%s%s ", num, curr_name)
    end
    tip = tip .. string.format(Language.Live.Quickbuy.Txt2, need_str)
    self.item_infos = item_infos
    return tip
end

function LiveQuickBuy:OnCloseClick()
    ViewMgr:CloseView(LiveQuickBuy)
end

function LiveQuickBuy:OnCancelClick()
    ViewMgr:CloseView(LiveQuickBuy)
end

function LiveQuickBuy:OnOkClick()
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
    for i,v in pairs(self.item_infos) do
        local info = ShopData.GetFastBuyData(v.item_id)
        if info and info.money_type == CurrencyType.ImmortalCoin then 
            total_price = MarketData.Instance:GetItemPrice(info.item_id) * v.num + total_price
        elseif info and info.money_type == CurrencyType.CoinBind then 
            total_coin = info.price * v.num + total_coin
        end 
    end 

    if total_price > 0 and need_market_fun(total_price) then 
        LiveData.Instance.live_quick_buy_flag = false
        return 
    end 

    if total_coin > 0 and need_shop_fun(total_coin) then 
        LiveData.Instance.live_quick_buy_flag = false
        return 
    end 

    for i,v in pairs(self.item_infos) do
        local info = ShopData.GetFastBuyData(v.item_id)
        if info and info.money_type == CurrencyType.ImmortalCoin then 
            MarketCtrl.Instance:SendMarketReq(1,v.item_id,v.num)
        elseif info then 
            RoleCtrl.FastPurchase(v.item_id, v.num)
        end
    end
    ViewMgr:CloseView(LiveQuickBuy)
end

function LiveQuickBuy:OnToggleClick(value)
    LiveData.Instance.live_quick_buy_flag = value
end