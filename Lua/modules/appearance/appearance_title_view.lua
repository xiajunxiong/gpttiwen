AppearanceTitleView = AppearanceTitleView or DeclareMono("AppearanceTitleView", UIWFlushPanel)

function AppearanceTitleView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	AppearanceData.Instance:SetSelectType(AppearanceConfig.Type.Title)
end

function AppearanceTitleView:OnClickGoWay()
	PublicPopupCtrl.Instance:Center(Language.Appearance.GoWayTip)
end