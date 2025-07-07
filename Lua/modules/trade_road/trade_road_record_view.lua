TradeRoadRecordView = TradeRoadRecordView or DeclareView("TradeRoadRecordView", TradeRoadConfig.ResPath .. "trade_road_record")
VIEW_DECLARE_MASK(TradeRoadRecordView, ViewMask.BgBlockClose)

function TradeRoadRecordView:Awake()
    BaseView.Awake(self)
    self.handle_info = TradeRoadData.Instance.record_info:Care(BindTool.Bind1(self.FlushView, self))
    TradeRoadCtrl.Instance:SendRecordListReq()
end

function TradeRoadRecordView:OnDestroy()
    BaseView.OnDestroy(self)
    TradeRoadData.Instance.record_info:Uncare(self.handle_info)
    self.handle_info = nil
end

function TradeRoadRecordView:FlushView()
    local show_list = table.rearrange2(TradeRoadData.Instance:GetRecordList())
    local count = #show_list
    for i, v in ipairs(show_list) do
        v.index = count
        count = count - 1
        v.is_pass = TradeRoadData.Instance:IsCityPass(v.target_city_map_seq)
    end
    table.sort(show_list, DataHelper.WeightSort("index", function(data)
        if data.is_pass then return 1000 end
    end))
    self.EmptyObj:SetActive(#show_list < 1)
    self.RecordList:SetData(show_list)
end

TradeRoadRecordItem = TradeRoadRecordItem or DeclareMono("TradeRoadRecordItem", UIWidgetBaseItem)
function TradeRoadRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local city_cfg = TradeRoadData.Instance:GetCityCfg(data.from_city_map_seq)
    UH.SetText(self.NameText, city_cfg.city_name)
    UH.SetText(self.MessageText, self:ConverMessage(data))
    --已经经过的城镇需要换个颜色显示，增加灰色蒙版，然后放在最下方
    self.MaskObj:SetActive(data.is_pass)
end

function TradeRoadRecordItem:ConverMessage(info)
    local item_cfg = TradeRoadData.Instance:GetItemCfg(info.commodity_seq)
    local frieight_cfg = TradeRoadData.Instance:GetFreightCfg(info.log_type, info.commodity_seq)
    local traget_cfg = TradeRoadData.Instance:GetCityCfg(info.target_city_map_seq)
    if item_cfg == nil or frieight_cfg == nil or traget_cfg == nil then
        return ""
    end
    if info.log_type == SILK_ROADS_LOG_TYPE.TYPE_PRICE_UP then
        local name_str = ColorStr(traget_cfg.city_name, COLORSTR.Green6)
        local price_str = ColorStr(info.price, COLORSTR.Purple5)
        local item_str = ColorStr(item_cfg.freight_name, COLORSTR.Red1)
        return Format(frieight_cfg.freight_txt, name_str, price_str, item_str)
    elseif info.log_type == SILK_ROADS_LOG_TYPE.TYPE_DOUBLE_UP then
        local rate_val = TradeRoadData.Instance:GetItemPriceRate(info.target_city_map_seq, info.commodity_seq)
        local name_str = ColorStr(traget_cfg.city_name, COLORSTR.Green6)
        local item_str = ColorStr(item_cfg.freight_name, COLORSTR.Red1)
        local rate_str = ColorStr(rate_val .. "%", COLORSTR.Purple5)
        return Format(frieight_cfg.freight_txt, name_str, item_str, rate_str)
    end
    return ""
end
