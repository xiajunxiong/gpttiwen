ItemGetWayView = ItemGetWayView or DeclareView("ItemGetWayView", "item_info/item_getway")

VIEW_DECLARE_MASK(ItemGetWayView, ViewMask.None)

--item_id + pos 必须都有
function ItemGetWayView:LoadCallback(param_t)
    self:SetGetWayList(param_t.item_id)
    self:SetPosition(param_t.pos)
end

function ItemGetWayView:SetGetWayList(item_id)
    local get_list = Item.GetWayList(item_id) or {}
    if #get_list == 0 then 
        PublicPopupCtrl.Instance:Center(Language.ItemInfo.GetWayNil)
        ViewMgr:CloseView(ItemGetWayView)
    end
    self.BtnList:SetData(get_list)
end

function ItemGetWayView:SetPosition(pos)
    UH.LocalPos(self.Pos,pos)
end

function ItemGetWayView:OnClickBlank()
    ItemInfoCtrl.Instance:ItemDeriveView()
    local info = ItemInfoData.Instance:ItemInfo()
    if info and info.show_ways == true and not DEBUG_MODE then
        ViewMgr:CloseView(ItemInfoView)
    end
end

function ItemGetWayView:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end