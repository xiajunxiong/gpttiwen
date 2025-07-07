ServiceInviteRewardPanel = ServiceInviteRewardPanel or DeclareMono("ServiceInviteRewardPanel",UIWFlushPanel)

function ServiceInviteRewardPanel:ServiceInviteRewardPanel()
	-- body
end

function ServiceInviteRewardPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	UH.SetText(self.Desc,Language.Service.ServiceInviteDesc)
	self:FlushPanel()
end

function ServiceInviteRewardPanel:FlushPanel()
	-- body
end

function ServiceInviteRewardPanel:OnGetClick()
	-- body
end



ServiceShareRewardPanel = ServiceShareRewardPanel or DeclareMono("ServiceShareRewardPanel",UIWFlushPanel)

function ServiceShareRewardPanel:ServiceShareRewardPanel()
	-- body
end

function ServiceShareRewardPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushPanel()
end

function ServiceShareRewardPanel:FlushPanel()
	-- body
end

ServiceShareRewardItem = ServiceShareRewardItem or DeclareMono("ServiceShareRewardItem",UIWidgetBaseItem)
function ServiceShareRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
end

function ServiceShareRewardItem:OnGetClick()
	-- body
end