AppearanceBubblingView = AppearanceBubblingView or DeclareMono("AppearanceBubblingView", UIWFlushPanel)

function AppearanceBubblingView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	AppearanceData.Instance:SetSelectType(AppearanceConfig.Type.Bubble)
end