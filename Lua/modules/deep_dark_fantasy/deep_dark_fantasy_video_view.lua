
DeepDarkFantasyVideoView = DeepDarkFantasyVideoView or DeclareView("DeepDarkFantasyVideoView", "deep_dark_fantasy/deep_dark_fantasy_video", 0, ViewLayer.Top)
function DeepDarkFantasyVideoView:DeepDarkFantasyVideoView()
end

function DeepDarkFantasyVideoView:LoadCallback()
	self.end_func = BindTool.Bind(DeepDarkFantasyVideoView.OnClose, DeepDarkFantasyVideoView)
	if VersionTag.HasTag(VersionTag.ShowCGSupport) then
		self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached + self.end_func
	else
		self.timer_dt = TimeHelper:AddDelayTimer(self.end_func, 23)
	end
end

function DeepDarkFantasyVideoView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_dt)
	if VersionTag.HasTag(VersionTag.ShowCGSupport) then
		self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached - self.end_func
	end
end

function DeepDarkFantasyVideoView:OnClose()
	DeepDarkFantasyData.Instance.model_data.start = 1
	ViewMgr:CloseView(DeepDarkFantasyVideoView)
end