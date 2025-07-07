
BoardAM = BoardAM or DeclareMono("BoardAM")

function BoardAM:BoardAM()
	self.view_type = nil
end

function BoardAM:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	end
end

function BoardAM:SetData(view_type)
	self.view_type = view_type
end

function BoardAM:SetTabbarVisible(visible)
	self.Tabbar:SetObjActive(visible)
end

function BoardAM:OnRelease()
	self.Tabbar:SetObjActive(false)
end