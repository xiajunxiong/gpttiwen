-- ============== 星图重置 =============
StarMapResetView = StarMapResetView or DeclareView("StarMapResetView", "star_map/star_map_reset_view")
function StarMapResetView:StarMapResetView()
    self.data = StarMapData.Instance
end

-- star_type ==== StarMapCtrl.MapType
function StarMapResetView:LoadCallback(param_t)
    local map_type = param_t.map_type or StarMapCtrl.MapType.ROLE
    self.map_type = map_type
    local title = string.format(Language.StarMap.Text2, Language.StarMap.StarMapType[map_type])
    self.Board:SetData(self:GetType(), title ,Vector2.New(752, 460))

    local map_name = Language.StarMap.StarMapType[map_type] .. Language.StarMap.Text4
    local tip = string.format(Language.StarMap.Text3, map_name)
    UH.SetText(self.Tips, tip)

    local stuff_list = self.data:GetMapResetBackStuff(map_type)
    self.stuff_list = stuff_list
    self.StuffList:SetData(stuff_list)

    local coin_num = map_type == StarMapCtrl.MapType.Super and Config.starmap_auto.other[1].reset_cion2 or Config.starmap_auto.other[1].reset_cion1
    self.coin_num = coin_num
    UH.SetText(self.CoinNum, coin_num)
end

function StarMapResetView:OnResetClick()
    if TableIsEmpty(self.stuff_list) then
        PublicPopupCtrl.Instance:Error(Language.StarMap.Tip1)
        return
    end
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.coin_num) then
        if self.map_type == StarMapCtrl.MapType.Super then
            StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.ResetSuper)
        else
            StarMapCtrl.Instance:SendReq(StarMapCtrl.OpType.Reset, self.map_type)
        end
        ViewMgr:CloseView(StarMapResetView)
    end
end


StarMapResetStuffItem = StarMapResetStuffItem or DeclareMono("StarMapResetStuffItem", UIWidgetBaseItem)
function StarMapResetStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:QuaName())
end