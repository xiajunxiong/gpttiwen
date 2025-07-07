PublicWarningView = PublicWarningView or DeclareView("PublicWarningView", "public_popup/public_warning")
VIEW_DECLARE_MASK(PublicWarningView,ViewMask.None)
function PublicWarningView:LoadCallback(param_t)
    self.data = PublicPopupData.Instance:GetWarningData()
    UH.SetText(self.LblDesc, self.data.desc)
    UH.SetText(self.LblWarning, self.data.warning)
    UH.AnchoredPosition(self.ShowPos, param_t.pos or Vector2.zero)
    self.ShopRect.sizeDelta = Vector2.New(1024,param_t.height or 250)
end
function PublicWarningView:OnClickClose()
    ViewMgr:CloseView(PublicWarningView)
end