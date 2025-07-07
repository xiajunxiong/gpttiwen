AppearanceTailTagView = AppearanceTailTagView or DeclareMono("AppearanceTailTagView", UIWFlushPanel)

function AppearanceTailTagView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	AppearanceData.Instance:SetSelectType(AppearanceConfig.Type.TailTag)
end