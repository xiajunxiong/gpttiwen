TradeRoadMapView = TradeRoadMapView or DeclareView("TradeRoadMapView", TradeRoadConfig.ResPath .. "trade_road_map")
VIEW_DECLARE_LEVEL(TradeRoadMapView, ViewLevel.Lt) --防止Npc对话框关闭界面

function TradeRoadMapView:LoadCallback()
    local last_val = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS)
    if last_val > 10 then
        self.last_val = last_val
        RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 10)
        RoleSettingCtrl.Instance:SaveRoleSettingInfo()
    end
end

function TradeRoadMapView:CloseCallback()
    if self.last_val ~= nil then
        RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, self.last_val)
        RoleSettingCtrl.Instance:SaveRoleSettingInfo()
        self.last_val = nil
    end
end

function TradeRoadMapView:OnClickClose()
    ViewMgr:CloseView(TradeRoadMapView)
end

function TradeRoadMapView:OnClickBack()
    ViewMgr:OpenView(TradeRoadView)
end

TradeRoadMapPanel = TradeRoadMapPanel or DeclareMono("TradeRoadMapPanel", UIWFlushPanel)
function TradeRoadMapPanel:TradeRoadMapPanel()
    self.data_cares = {
        {data = TradeRoadData.Instance.road_info, func = self.FlushView},
        {data = TradeRoadData.Instance.map_move, func = self.ToNextCity, init_call = false}
    }
end

function TradeRoadMapPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:DrawLines()
end

function TradeRoadMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.single_runner ~= nil then
        TimeHelper:CancelTimer(self.single_runner)
        self.single_runner = nil
    end
end

function TradeRoadMapPanel:FlushView()
    self.CityList:SetData(TradeRoadData.Instance:GetMapAllList())
end

function TradeRoadMapPanel:DrawLines()
    local pos_list = {}
    --如果不显示全部线，可考虑只画目标线段。
    local show_list = TradeRoadData.Instance:GetMapList()
    for i = 1, #show_list - 1 do
        local data = {from = show_list[i], to = show_list[i + 1]}
        local city_cfg = TradeRoadData.Instance:GetCityCfg(show_list[i])
        data.start_pos = TradeRoadData.ConverPos(Vector3.New(city_cfg.coord_x, city_cfg.coord_y, 0))
        city_cfg = TradeRoadData.Instance:GetCityCfg(show_list[i + 1])
        data.end_pos = TradeRoadData.ConverPos(Vector3.New(city_cfg.coord_x, city_cfg.coord_y, 0))

        table.insert(pos_list, data)
    end
    self.LineList:SetData(pos_list)

    local cur_seq = TradeRoadData.Instance:CurCitySeq()
    local city_cfg = TradeRoadData.Instance:GetCityCfg(cur_seq)
    local pos = TradeRoadData.ConverPos(Vector3.New(city_cfg.coord_x, city_cfg.coord_y, 0))
    self.Anim:SetLocalPosition(Vector3.New(pos.x, pos.y + 80, 0))
    self.Anim:SetLocalScale(Vector3.New(1, 1, 1))
    self.Anim:SetActive(true)
end

function TradeRoadMapPanel:ToNextCity()
    if not self.LineList:SetupCompleted() then
        return
    end
    self.is_send = false
    local list_index, city_seq = TradeRoadData.Instance:GetNextCitySeq()
    if list_index ~= nil then
        self:MoveNext(self.LineList:GetItem(list_index - 1))
    end
    self.BtnBack:SetActive(false)
end

function TradeRoadMapPanel:MoveNext(item)
    if item == nil then
        return
    end
    local data = item:GetData()
    self.Anim:SetLocalPosition(data.start_pos)
    self.Anim:SetLocalScale(Vector3.New(data.start_pos.x < data.end_pos.x and 1 or -1, 1, 1))
    self.Anim:SetActive(true)
    self.step_list = table.rearrange2(item:GetDrawLines())
    self:ToNext(data.start_pos, self.step_list[1].gameObject:GetLocalPosition())
end

function TradeRoadMapPanel:ToNext(start_pos, end_pos)
    if self.single_runner ~= nil then
        TimeHelper:CancelTimer(self.single_runner)
        self.single_runner = nil
    end
    self.step_cur, self.step_max = 0, 5
    self.move_dis = (end_pos - start_pos) / self.step_max
    local function func_next()
        self.Anim:SetLocalPosition(start_pos + self.move_dis * self.step_cur)
        self.step_cur = self.step_cur + 1
        if self.step_cur >= self.step_max then
            if #self.step_list ~= 1 then
                self.step_list[1].gameObject:SetActive(true)
            end
            table.remove(self.step_list, 1)
            if #self.step_list > 0 then
                if #self.step_list <= 2 then
                    self:OnMoveSend()
                end
                self:ToNext(end_pos, self.step_list[1].gameObject:GetLocalPosition())
            else
                self:OnMoveOver()
            end
        end
    end
    self.single_runner = TimeHelper:AddRunFrameTimer(func_next, 1, self.step_max)
end

function TradeRoadMapPanel:OnMoveSend()
    if not self.is_send then
        TradeRoadCtrl.Instance:SendGoNextCityReq()
        self.is_send = true
    end
end

function TradeRoadMapPanel:OnMoveOver()
    -- ViewMgr:CloseView(TradeRoadMapView)
    self.BtnBack:SetActive(true)
    ViewMgr:OpenView(TradeRoadView)
    local old_pos = self.Anim:GetLocalPosition()
    self.Anim:SetLocalPosition(Vector3.New(old_pos.x, old_pos.y + 80, 0))
end

TradeRoadMapCityItem = TradeRoadMapCityItem or DeclareMono("TradeRoadMapCityItem", UIWidgetBaseItem)
function TradeRoadMapCityItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local city_cfg = TradeRoadData.Instance:GetCityCfg(self.data)
    if IS_EDITOR then
        self.gameObject.name = city_cfg.city_name .. "_" .. city_cfg.seq
    end
    local state = 1
    if self.data == TradeRoadData.Instance:CurCitySeq() then
        state = 3
    else
        local record_list = TradeRoadData.Instance:GetTargetCityRecordList(self.data)
        if #record_list > 0 then
            state = 2
        end
    end
    UH.SpriteName(self.CityState, "CityName" .. state)
    self.BtnTips:SetActive(state == 2)

    UH.SetText(self.NameText, city_cfg.city_name)
    UH.Color(self.NameText, state == 2 and COLORS.Red2 or COLORS.White)

    local pos = TradeRoadData.ConverPos(Vector3.New(city_cfg.coord_x, city_cfg.coord_y, 0))
    self.ItemRect:SetLocalPosition(pos)
end

function TradeRoadMapCityItem:OnClickMap()
    local record_list = TradeRoadData.Instance:GetTargetCityRecordList(self.data)
    if #record_list > 0 then
        ViewMgr:OpenView(TradeRoadMapRecordView, {seq = self.data, root_pos = self.ItemRect:GetLocalPosition()})
    end
end

TradeRoadMapLineItem = TradeRoadMapLineItem or DeclareMono("TradeRoadMapLineItem", UIWidgetBaseItem)
function TradeRoadMapLineItem:TradeRoadMapLineItem()
    self.line_list = {}
    self.free_list = {}
end

function TradeRoadMapLineItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = data.from .. "_" .. data.to
    self.is_pass = TradeRoadData.Instance:IsCityPass(data.from)
    self:DrawLines()
end

function TradeRoadMapLineItem:DrawLines()
    local temp_pos = self.data.end_pos - self.data.start_pos
    local distance = math.sqrt(math.pow(temp_pos.x, 2) + math.pow(temp_pos.y, 2))
    local times = math.ceil(distance / 20)
    for i = 1, times do
        local clone = self.line_list[i]
        if self.line_list[i] == nil then
            clone = self:AddLine()
            clone.gameObject.name = "Line" .. i
            self.line_list[i] = clone
        end
        clone.gameObject:SetLocalPosition(self.data.start_pos + temp_pos / times * i)
        clone.gameObject:SetActive(self.is_pass)
    end
    for i = times + 1, #self.line_list do
        self:DelLine(table.remove(self.line_list))
    end
end

function TradeRoadMapLineItem:AddLine()
    if #self.free_list > 0 then
        return table.remove(self.free_list)
    end
    local clone = self.Line:Clone(self.gameObject)
    return clone:GetLuaComponent("TradeRoadMapPointItem")
end
function TradeRoadMapLineItem:DelLine(line)
    line.gameObject:SetActive(false)
    table.insert(self.free_list, line)
end
function TradeRoadMapLineItem:GetDrawLines()
    return self.line_list
end

TradeRoadMapPointItem = TradeRoadMapPointItem or DeclareMono("TradeRoadMapPointItem", UIWidgetBaseItem)
function TradeRoadMapPointItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject:SetLocalPosition(data.pos)
end

function TradeRoadMapPointItem:ToMoved()
    self.gameObject:SetActive(true)
end

-----记录
TradeRoadMapRecordView = TradeRoadMapRecordView or DeclareView("TradeRoadMapRecordView", TradeRoadConfig.ResPath .. "trade_road_map_record")
VIEW_DECLARE_MASK(TradeRoadMapRecordView, ViewMask.BlockClose)

function TradeRoadMapRecordView:LoadCallback(param_t)
    self.data = param_t.seq
    local record_list = TradeRoadData.Instance:GetTargetCityRecordList(self.data)
    self.RecordList:SetData(record_list)

    UH.LocalPos(self.Root, param_t.root_pos + Vector3.New(230, 0, 0))
    UH.SetEdgePosition(self.Root, self.Rect)
end

TradeRoadMapRecordItem = TradeRoadMapRecordItem or DeclareMono("TradeRoadMapRecordItem", UIWidgetBaseItem)
function TradeRoadMapRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.DescText, self:ConverMessage(data))
end

function TradeRoadMapRecordItem:ConverMessage(info)
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