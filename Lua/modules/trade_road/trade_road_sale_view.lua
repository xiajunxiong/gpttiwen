--卖出：售价|成本|出售数量|出售金额|获得利润
TradeRoadSaleView = TradeRoadSaleView or DeclareView("TradeRoadSaleView", TradeRoadConfig.ResPath .. "trade_road_sale")
function TradeRoadSaleView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.TradeRoad.Sale, Vector2.New(505, 559))
    self.select_num, self.min_num, self.max_num = 1, 1, 9999

    --物品seq,物品数量，物品价格
    self.commodity_seq = param_t.commodity_seq
    self.item_num = param_t.commodity_count
    self.buy_price = param_t.commodity_cost

    self.item_cfg = TradeRoadData.Instance:GetItemCfg(self.commodity_seq)
    local item_info = TradeRoadData.Instance:GetCityItemInfo(nil, self.commodity_seq)
    self.sale_price = item_info ~= nil and item_info.price or self.item_cfg.freight_market_price
end

function TradeRoadSaleView:OnFlush()
    self.ItemCell:SetData({seq = self.item_cfg.seq})
    UH.SetText(self.NameText, self.item_cfg.freight_name)
    UH.Color(self.NameText, QualityColor[self.item_cfg.pinzhi])

    --售价，成本
    UH.SetText(self.TopValue1, self.sale_price)
    UH.SetText(self.TopValue2, self.buy_price)
    self:OnNumChange()
end

function TradeRoadSaleView:OnClickNum(param)
    if param < 0 and self.select_num == self.min_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MinNum)
        return
    elseif param > 0 and self.select_num == self.max_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum)
        return
    end
    if self.select_num + param > self.item_num then
        if self.select_num == self.item_num then
            PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum2)
            return
        end
        self.select_num = self.item_num
    else
        self.select_num = self.select_num + param
    end
    self.select_num = math.max(self.select_num, self.min_num)
    self.select_num = math.min(self.select_num, self.max_num)
    self:OnNumChange()
end

function TradeRoadSaleView:OnNumChange()
    UH.SetText(self.NumText, self.select_num)
    -- 出售数量，出售金额，获得利润
    UH.SetText(self.MiddleValue1, Format(Language.TradeRoad.BuySize, self.select_num, self.item_num))
    local sale_val = self.select_num * self.sale_price
    UH.SetText(self.MiddleValue2, sale_val)
    local profit_val = sale_val - self.select_num * self.buy_price
    UH.SetText(self.MiddleValue3, profit_val)
end

function TradeRoadSaleView:OnClickEvent()
    TradeRoadCtrl.Instance:SendSaleItemReq(self.item_cfg.seq, math.min(self.select_num, self.item_num))
    ViewMgr:CloseView(TradeRoadSaleView)
end
