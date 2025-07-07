UIWCanvasGroupDelay = UIWCanvasGroupDelay or DeclareMono("UIWCanvasGroupDelay")
----CanvasGroup======CanvasGroup
----DelayFrames======Int

function UIWCanvasGroupDelay:Awake()
	self.CanvasGroup.alpha = 0
	self.CanvasGroup.interactable = false

	self.timer = TimeHelper:AddDelayFrameTimer(
		function () 
			self.timer = nil
			self.CanvasGroup.alpha = 1
			self.CanvasGroup.interactable = true
			end,
		self.DelayFrames)
end




function UIWCanvasGroupDelay:OnDestroy()
	if self.timer ~= nil then
		TimeHelper:CancelTimer(self.timer)
		self.timer = nil
	end
end