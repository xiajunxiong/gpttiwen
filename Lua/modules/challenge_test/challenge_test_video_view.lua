
ChallengeTestVideoView = ChallengeTestVideoView or DeclareView("ChallengeTestVideoView", ChallengeTestConfig.ResPath .. "challenge_test_video", 0, ViewLayer.Top)

function ChallengeTestVideoView:ChallengeTestVideoView()
	self.data = ChallengeTestCtrl.Instance:Data()
end

function ChallengeTestVideoView:LoadCallback()
	self.end_func = BindTool.Bind(ViewMgr.CloseView, ViewMgr, ChallengeTestVideoView)
	if VersionTag.HasTag(VersionTag.ShowCGSupport) then
		self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached + self.end_func
	else
		self.timer_dt = TimeHelper:AddDelayTimer(self.end_func, 23)
	end
end

function ChallengeTestVideoView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_dt)
	if VersionTag.HasTag(VersionTag.ShowCGSupport) then
		self.VideoPlayer.loopPointReached = self.VideoPlayer.loopPointReached - self.end_func
	end
	self.data:ShowButterfly()
end