WorkshopGetWay = WorkshopGetWay or DeclareView("WorkshopGetWay", "workshop/workshop_getway")
-- 传个item_id应该就可以了
-- 途径id->对应执行方法
WorkshopGetWayOperaFunc = {

}
--VIEW_DECLARE_LEVEL(WorkshopGetWay,ViewLevel.L1)
VIEW_DECLARE_MASK(WorkshopGetWay, ViewMask.BlockClose)
function WorkshopGetWay:WorkshopGetWay()
    self.data = ItemInfoCtrl.Instance:Data()
    self.info = self.data:GetWayInfo() or {}
    self.item_info = self.data:ItemInfo()
end
function WorkshopGetWay:LoadCallback(param)
    self.info = param or self.info

    -- self.Pos.localPosition = Vector2.New(self.item_info.pos.x + 408, self.item_info.pos.y)
end
function WorkshopGetWay:OnFlush()
    if self.info.pos then UH.LocalPos(self.Pos, self.info.pos) end
    if self.info.id_list then
        self.BtnList:SetData(self.info.id_list)
    else
        local item = Item.Create({item_id = self.info.item_id})
        self.BtnList:SetData(item:GetWay() or {})
    end
end

WayBtnItem = WayBtnItem or DeclareMono("WayBtnItem", UIWidgetBaseItem)

function WayBtnItem:SetData(data)
    self.data = data
    self.config_data = BagData.Instance:GetWayCfg(self.data)
    if self.config_data == nil then return end 
    UH.SetText(self.Name,self.config_data.desc)

    if not FunOpen.Instance:GetFunIsOpened(self.config_data.open_panel) then
        self.Interactor.Interactable = false
    end 

end 

function WayBtnItem:OnClickButton()
    if self.config_data.npc ~= "" then 
        SceneLogic.Instance:AutoToNpc(self.config_data.npc)
        ViewMgr:CloseView(WorkshopGetWay)
        ViewMgr:CloseView(Workshop)
        return 
    end 

    ViewMgr:OpenViewByKey(self.config_data.open_panel)
end 
