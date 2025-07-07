LandsBetweenBoard = LandsBetweenBoard or DeclareMono("LandsBetweenBoard")

function LandsBetweenBoard:LandsBetweenBoard()
	self.view_type = nil
end

function LandsBetweenBoard:OnCloseClick()
	if self.view_type ~= nil then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
		ViewMgr:CloseView(self.view_type)
	end
end

function LandsBetweenBoard:SetData(view_type, title_str)
	self.view_type = view_type
    UH.SetText(self.TxtTitle, title_str or "")
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
end

function LandsBetweenBoard:SetTabbarVisible(visible)
    self.Tabbar1.gameObject:SetActive(visible)
end