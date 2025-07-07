LingBaoGetWayView = LingBaoGetWayView or DeclareView("LingBaoGetWayView", "ling_bao/ling_bao_get_way")
VIEW_DECLARE_MASK(LingBaoGetWayView, ViewMask.BlockClose)
function LingBaoGetWayView:LingBaoGetWayView()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoGetWayView:LoadCallback(param_t)
    self.List:SetData(self.Data:GetWayData())
end

--------------LingBaoGetWayItem--------------
LingBaoGetWayItem = LingBaoGetWayItem or DeclareMono("LingBaoGetWayItem", UIWidgetBaseItem)
function LingBaoGetWayItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.name)
end

function LingBaoGetWayItem:OnBtnClick()
    if self.data.open_panel then
        if self.data.open_panel == Mod.LandsBetween.Main then
            ViewMgr:OpenView(LandsBetweenPosterView)
        else
            ViewMgr:OpenViewByKey(self.data.open_panel)
        end
        ViewMgr:CloseView(LingBaoGetWayView)
        ViewMgr:CloseView(LingBaoBagView)
        ViewMgr:CloseView(LingBaoView)
    end
end