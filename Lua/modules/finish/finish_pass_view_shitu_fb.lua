
FinishPassViewShiTuFB = FinishPassViewShiTuFB or DeclareView("FinishPassViewShiTuFB", "finish/finish_pass_shitu_fb")

function FinishPassViewShiTuFB:OnClickClose()
	ViewMgr:CloseView(FinishPassViewShiTuFB)
end

FinishPassViewShiTuFBF = FinishPassViewShiTuFBF or DeclareMono("FinishPassViewShiTuFBF", UIWFlushPanel)

function FinishPassViewShiTuFBF:FinishPassViewShiTuFBF()
	self.data = SocietyCtrl.Instance:Data()
end

function FinishPassViewShiTuFBF:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushTimeView()
	self:FlushShowDesc()
end

--刷新倒计时
function FinishPassViewShiTuFBF:FlushTimeView()
	self.ConfirmTime:SetCallBack(function()
		ViewMgr:CloseView(FinishPassViewShiTuFB)
	end)
	self.ConfirmTime:TotalTime(5, TimeType.Type_Special_2, Language.Finish.Confirm)
end

function FinishPassViewShiTuFBF:FlushShowDesc()
	local info = self.data:GetShiTuFbBattleResult()
	if nil == info then return end
	self.ShowDesc.text = self.data:GetShiTuFbShowDesc(info.group, info.index)
end

function FinishPassViewShiTuFBF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	self.ConfirmTime:CloseCountDownTime()
end