TreasureMapShow = TreasureMapShow or DeclareView("TreasureMapShow", "main_other/treasure_map_show", Mod.TreasureMap.Show)
VIEW_DECLARE_MASK(TreasureMapShow, ViewMask.BlockClose)
function TreasureMapShow:LoadCallback(params)
    local item = params.item
    self.close_func = params.close_func
    self.item_info_is_opened = false
    self.Cell:SetData(item)
    self.Name.text = item:QuaName(true)
    self.handle_count =
        TimeHelper:AddCountDownTT(
        function()
            --检查是否打开了物品详细
            if ViewMgr:IsOpen(ItemInfoView) then
                self.item_info_is_opened = true
            end
        end,
        function()
            if self.item_info_is_opened == false then
                ViewMgr:CloseView(TreasureMapShow)
            end
        end,
        5
    )
end
function TreasureMapShow:CloseCallback()
    TimeHelper:CancelTimer(self.handle_count)
    if self.close_func == nil then
        local map = TreasureMapData.Instance:ViewData().map
    else
        self.close_func()
        self.close_func = nil
    end
end
