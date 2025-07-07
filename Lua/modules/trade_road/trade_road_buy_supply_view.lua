--购买补给：购买数量 100/每次|购买数量|花费金额|补给剩余数量
TradeRoadBuySupplyView = TradeRoadBuySupplyView or DeclareView("TradeRoadBuySupplyView", TradeRoadConfig.ResPath .. "trade_road_buy_supply")
function TradeRoadBuySupplyView:TradeRoadBuySupplyView()
    self.select_num, self.min_num, self.max_num = 1, 1, 1
    self.buy_supply = TradeRoadData.Instance:OtherConfig().buy_supply
    self.max_num = TradeRoadData.Instance:MaxBuySupplyTimes() - TradeRoadData.Instance:CurBuySupplyTimes()
end

function TradeRoadBuySupplyView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.TradeRoad.BuySupply, Vector2.New(505, 530))
    self.ItemCell:SetData({icon = TradeRoadConfig.SupplyIcon})

    UH.SetText(self.TopValue0, TradeRoadData.Instance:CurSupply())
    UH.SetText(self.TopValue1, Format(Language.TradeRoad.BuySupplyNum, self.buy_supply))
    self:OnNumChange()
end

function TradeRoadBuySupplyView:OnClickNum(param)
    if param < 0 and self.select_num == self.min_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MinNum)
        return
    elseif param > 0 and self.select_num == self.max_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum)
        return
    end
    self.select_num = self.select_num + param
    self.select_num = math.max(self.select_num, self.min_num)
    self.select_num = math.min(self.select_num, self.max_num)
    self:OnNumChange()
end

function TradeRoadBuySupplyView:OnNumChange()
    UH.SetText(self.NumText, self.select_num)
    UH.SetText(self.MiddleValue1, self.select_num * self.buy_supply)
    UH.SetText(self.MiddleValue2, TradeRoadData.Instance:GetBuySupplyPrice(self.select_num))
end

function TradeRoadBuySupplyView:OnClickEvent()
    if self.select_num > self.max_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum)
        return
    end
    if TradeRoadCtrl.IsEnough(TradeRoadData.Instance:GetBuySupplyPrice(self.select_num)) then
        TradeRoadCtrl.Instance:SendBuySupplyReq(self.select_num)
        ViewMgr:CloseView(TradeRoadBuySupplyView)
    end
end
