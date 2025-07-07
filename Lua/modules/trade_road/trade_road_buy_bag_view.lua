--仓库扩充：我的仓库|扩充数量|仓库损耗|购买数量|花费金额
TradeRoadBuyBagView = TradeRoadBuyBagView or DeclareView("TradeRoadBuyBagView", TradeRoadConfig.ResPath .. "trade_road_buy_bag")
function TradeRoadBuyBagView:TradeRoadBuyBagView()
    self.select_num, self.min_num, self.max_num = 1, 1, 1
    self.buy_bag = TradeRoadData.Instance:OtherConfig().buy_bag
    self.max_num = TradeRoadData.Instance:MaxBuyBagTimes() - TradeRoadData.Instance:CurBuyBagTimes()
end

function TradeRoadBuyBagView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.TradeRoad.BuyBag, Vector2.New(505, 530))
    self.ItemCell:SetData({icon = TradeRoadConfig.BagIcon})

    local max_size = TradeRoadData.Instance:CurBagMaxSize()
    local use_size = TradeRoadData.Instance:CurBagUseSize()
    UH.SetText(self.TopValue0, Format(Language.TradeRoad.BuySize, use_size, max_size))
    UH.SetText(self.TopValue1, Format(Language.TradeRoad.BuyBagNum, self.buy_bag))
    self:OnNumChange()
end

function TradeRoadBuyBagView:OnClickNum(param)
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

function TradeRoadBuyBagView:OnNumChange()
    UH.SetText(self.NumText, self.select_num)
    UH.SetText(self.MiddleValue1, self.select_num * self.buy_bag)
    UH.SetText(self.MiddleValue2, TradeRoadData.Instance:GetBuyBagPrice(self.select_num))
end

function TradeRoadBuyBagView:OnClickEvent()
    if self.select_num > self.max_num then
        PublicPopupCtrl.Instance:Center(Language.TradeRoad.MaxNum)
        return
    end
    if TradeRoadCtrl.IsEnough(TradeRoadData.Instance:GetBuyBagPrice(self.select_num)) then
        TradeRoadCtrl.Instance:SendBuyBagReq(self.select_num)
        ViewMgr:CloseView(TradeRoadBuyBagView)
    end
end
