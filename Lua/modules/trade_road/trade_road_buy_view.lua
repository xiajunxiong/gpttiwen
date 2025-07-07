--买入：售价|市价|购买数量|花费金额|仓库数量
TradeRoadBuyView = TradeRoadBuyView or DeclareView("TradeRoadBuyView", TradeRoadConfig.ResPath .. "trade_road_buy")
function TradeRoadBuyView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.TradeRoad.Buy, Vector2.New(505, 559))
    self.select_num, self.min_num, self.max_num = 1, 1, 9999

    self.commodity_seq = param_t.seq
    self.item_cfg = TradeRoadData.Instance:GetItemCfg(self.commodity_seq)
    self.item_info = TradeRoadData.Instance:GetCurCityItemInfo(self.commodity_seq)
    self.price = self.item_info.price
end

function TradeRoadBuyView:OnFlush()
    self.ItemCell:SetData({seq = self.item_cfg.seq})
    UH.SetText(self.NameText, self.item_cfg.freight_name)
    UH.Color(self.NameText, QualityColor[self.item_cfg.pinzhi])

    --售价，市价
    UH.SetText(self.TopValue1, self.price)
    UH.SetText(self.TopValue2, self.item_cfg.freight_market_price)
    self:OnNumChange()
end

function TradeRoadBuyView:OnClickNum(param)
    if param < 0 and self.select_num == self.min_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MinNum)
        return
    elseif param > 0 and self.select_num == self.max_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum)
        return
    end
    if self.select_num + param >= self:MaxBuyNums() then
        if self.select_num == self:MaxBuyNums() then
            PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum3)
            return
        end
        self.select_num = self:MaxBuyNums()
        param = 0
    end
    if self.select_num + param >= self:GetBagRemainNum() then
        if self.select_num == self:GetBagRemainNum() then
            PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum1)
            return
        end
        self.select_num = self:GetBagRemainNum()
        param = 0
    end
    self.select_num = self.select_num + param
    self.select_num = math.max(self.select_num, self.min_num)
    self.select_num = math.min(self.select_num, self.max_num)
    self:OnNumChange()
end

function TradeRoadBuyView:OnNumChange()
    UH.SetText(self.NumText, self.select_num)
    --购买数量，花费金额，仓库数量
    UH.SetText(self.MiddleValue1, self.select_num)
    UH.SetText(self.MiddleValue2, self.select_num * self:SurePrice())
    UH.SetText(self.MiddleValue3, Format(Language.TradeRoad.BuySize, self.select_num, self:GetBagRemainNum()))
end

function TradeRoadBuyView:GetBagRemainNum()
    return TradeRoadData.Instance:CurBagMaxSize() - TradeRoadData.Instance:CurBagUseSize()
end

function TradeRoadBuyView:OnClickEvent()
    if self.select_num > self:GetBagRemainNum() then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum1)
        return
    end
    if TradeRoadCtrl.IsEnough(self.select_num * self:SurePrice()) then
        TradeRoadCtrl.Instance:SendBuyItemReq(self.item_cfg.seq, self.select_num)
        ViewMgr:CloseView(TradeRoadBuyView)
    end
end

function TradeRoadBuyView:MaxBuyNums()
    local cur_money = TradeRoadData.Instance:CurMoney()
    return math.floor(cur_money / self.price)
end

function TradeRoadBuyView:SurePrice()
    return TradeRoadData.Instance:GetBuyCost(self.price)
end
