TradeRoadBeginView = TradeRoadBeginView or DeclareView("TradeRoadBeginView", TradeRoadConfig.ResPath .. "trade_road_begin")
function TradeRoadBeginView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.TradeRoad.Name, Vector2.New(546, 374))
end

TradeRoadBeginPanel = TradeRoadBeginPanel or DeclareMono("TradeRoadBeginPanel", UIWFlushPanel)
function TradeRoadBeginPanel:TradeRoadBeginPanel()
    self.data_cares = {
        {data = TradeRoadData.Instance.road_info, func = self.FlushView}
    }
end

function TradeRoadBeginPanel:FlushView()
    local buff_cfg = TradeRoadData.Instance:GetBuffCfg(TradeRoadData.Instance:CurBuffId())
    if buff_cfg == nil then
        return
    end
    UH.SetText(self.BuffText, buff_cfg.name)
    UH.SetText(self.DescText, buff_cfg.txt)
    UH.SetText(self.MoneyText, TradeRoadData.Instance:CurMoney())
    UH.SetText(self.BagText, TradeRoadData.Instance:CurBagMaxSize())
end

function TradeRoadBeginPanel:OnClickBegin()
    ViewMgr:CloseView(TradeRoadBeginView)
    TradeRoadCtrl.Instance:SendStartOutset()
end
