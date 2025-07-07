TradeRoadItem = TradeRoadItem or DeclareMono("TradeRoadItem", UIWidgetBaseItem)
function TradeRoadItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local is_spe = data.icon ~= nil
    self.Icon.gameObject:SetActive(not is_spe)
    self.Icon2.gameObject:SetActive(is_spe)

    if is_spe then
        UH.SpriteName(self.QuaIcon, COLOR_FRAME_SPRITE[ItemColor.Blue])
        UH.SpriteName(self.Icon2, data.icon)
    else
        local item_cfg = TradeRoadData.Instance:GetItemCfg(data.seq)
        UH.SetIcon(self.Icon, item_cfg.freight_icon, ICON_TYPE.ITEM)
        UH.SpriteName(self.QuaIcon, COLOR_FRAME_SPRITE[item_cfg.pinzhi])
    end
end

--物品展示界面
TradeRoadItemShowView = TradeRoadItemShowView or DeclareView("TradeRoadItemShowView", TradeRoadConfig.ResPath .. "trade_road_item_show")
VIEW_DECLARE_LEVEL(TradeRoadItemShowView, ViewLevel.Lt)
VIEW_DECLARE_MASK(TradeRoadItemShowView, ViewMask.None)

function TradeRoadItemShowView:LoadCallback()
    self.BtnParam:SetCallBack(BindTool.Bind1(self.OnClickUse, self))

    self:FlushKeyUse(TradeRoadData.Instance:GetOneShowItem())
end

function TradeRoadItemShowView:FlushKeyUse(item)
    local item_cfg = TradeRoadData.Instance:GetItemCfg(item.seq)
    self.ItemCell:SetData({seq = item.seq})
    UH.SetText(self.ItemName, item_cfg.freight_name .. "x" .. item.num)
    self.BtnParam:TotalTime(5, TimeType.Type_Special_2, Language.TradeRoad.KeyUseBtn)
end

function TradeRoadItemShowView:OnClickUse()
    local item = TradeRoadData.Instance:GetOneShowItem()
    if item ~= nil then
        self:FlushKeyUse(item)
    else
        ViewMgr:CloseView(TradeRoadItemShowView)
    end
end

function TradeRoadItemShowView:OnClickClose()
    ViewMgr:CloseView(TradeRoadItemShowView)
end

function TradeRoadItemShowView:CloseCallback()
    TradeRoadData.Instance:ClearShowItem()
end
