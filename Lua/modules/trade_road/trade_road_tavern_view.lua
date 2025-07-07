TradeRoadTavernPanel = TradeRoadTavernPanel or DeclareMono("TradeRoadTavernPanel", UIWFlushPanel)
function TradeRoadTavernPanel:TradeRoadTavernPanel()
    self.cur_page, self.min_page, self.max_page = 1, 1, 1
    self.data_cares = {
        {data = TradeRoadData.Instance.road_info, func = self.FlushView, init_call = false},
        {data = TradeRoadData.Instance.record_info, func = self.FlushView}
    }
    GuideManager.Instance:RegisterGetGuideUi("TradeRoadTavernDesc", function()
        return self.GuideDesc, function() end
    end)
end

function TradeRoadTavernPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("TradeRoadTavernDesc")
end

function TradeRoadTavernPanel:FlushView()
    if not TradeRoadData.Instance:IsJoined() then
        return
    end
    local city_cfg = TradeRoadData.Instance:GetCurCityCfg()
    UH.SetText(self.NameText, Format(Language.TradeRoad.TavernName, city_cfg.city_name))
    self.NpcSetter:SetByPath(Format(TradeRoadConfig.PngPath, city_cfg.city_tavern_npc))

    self.record_list = {}
    table.insert(self.record_list, city_cfg.tavern_txt)
    local record_list = TradeRoadData.Instance:GetCurCityRecordList()
    for i, v in ipairs(record_list) do
        table.insert(self.record_list, self:ConverMessage(v))
    end

    --最新一条在最后
    if self.max_page ~= #self.record_list then
        self.cur_page = #self.record_list
    end
    self.max_page = #self.record_list
    self:FlushPage()

    UH.SetInter(self.BtnBuyInter, TradeRoadData.Instance:IsCanBuyMessage() and TradeRoadData.Instance:GetRemainNews() > 0)
    UH.SetText(self.BtnBuyText, TradeRoadData.Instance:GetBuyMessagePrice())
end

function TradeRoadTavernPanel:FlushPage()
    UH.SetText(self.MessageText, self.record_list[self.cur_page] or "")
    self.PageLeft:SetActive(self.cur_page ~= self.min_page)
    self.PageRight:SetActive(self.cur_page ~= self.max_page)
end

function TradeRoadTavernPanel:OnClickPage(param)
    if (param == -1 and self.cur_page == self.min_page) or (param == 1 and self.cur_page == self.max_page) then
        return
    end
    self.cur_page = self.cur_page + param
    self:FlushPage()
end

function TradeRoadTavernPanel:OnClickBuy()
    TradeRoadCtrl.Instance:SendBuyMessage()
end

function TradeRoadTavernPanel:ConverMessage(info)
    local item_cfg = TradeRoadData.Instance:GetItemCfg(info.commodity_seq)
    local frieight_cfg = TradeRoadData.Instance:GetFreightCfg(info.log_type, info.commodity_seq)
    local traget_cfg = TradeRoadData.Instance:GetCityCfg(info.target_city_map_seq)
    if item_cfg == nil or frieight_cfg == nil or traget_cfg == nil then
        return ""
    end
    if info.log_type == SILK_ROADS_LOG_TYPE.TYPE_PRICE_UP then
        return Format(frieight_cfg.freight_txt, traget_cfg.city_name, info.price, item_cfg.freight_name)
    elseif info.log_type == SILK_ROADS_LOG_TYPE.TYPE_DOUBLE_UP then
        local rate_val = TradeRoadData.Instance:GetItemPriceRate(info.target_city_map_seq, info.commodity_seq)
        return Format(frieight_cfg.freight_txt, traget_cfg.city_name, item_cfg.freight_name, rate_val .. "%")
    end
    return ""
end
