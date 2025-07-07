
FinishPassViewCT = FinishPassViewCT or DeclareView("FinishPassViewCT", "finish/finish_pass_ct")
VIEW_DECLARE_MASK(FinishPassViewCT,ViewMask.BgBlock)

function FinishPassViewCT:FinishPassViewCT()
	self.data = ChallengeTestCtrl.Instance:Data()
	self.language = Language.ChallengeTest.Finish
end

function FinishPassViewCT:LoadCallback(param_t)
	local is_last_layer = self.data:IsChallengeTestLastLayer()
	self.AutoShow:SetActive(not is_last_layer)
	self.BlockObj:SetActive(is_last_layer)
	local remain = ChallengeTestConfig.FINISH_PASS_CONTINUE
	self.timer_tt = TimeHelper:AddCountDownTT(function ()
		self.CountDown.text = string.format(self.language.CountDown, remain)
		remain = remain - 1
	end, function ()
		if is_last_layer then
			self:OnClickCancel()
		else
			self:OnClickContinue()
		end
		self.CountDown.text = string.format(self.language.CountDown, 0)
	end, ChallengeTestConfig.FINISH_PASS_CONTINUE, 1)
end

function FinishPassViewCT:CloseCallback()
	TimeHelper:CancelTimer(self.timer_tt)
end

function FinishPassViewCT:OnClickCancel()
	ViewMgr:CloseView(FinishPassViewCT)
	ViewMgr:OpenView(ChallengeTestView, {is_fail = false})
end

function FinishPassViewCT:OnClickContinue()
	ChallengeTestCtrl.Instance:SendLunHuiWangChuanBattle()
end