
ShiTuFBMenuView = ShiTuFBMenuView or DeclareMono("ShiTuFBMenuView", UIWFlushPanel)

function ShiTuFBMenuView:ShiTuFBMenuView()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.ShiTuFB
	self.data_cares = {
		{data = self.data.shitu_fb_sm, func = self.FlushMenu},
	}
end

function ShiTuFBMenuView:FlushMenu()
	TimeHelper:CancelTimer(self.timer_ct)
	self.timer_ct = TimeHelper:AddCountDownCT(function ()
		local t = TimeHelper.FormatDHMS(self.data.shitu_fb_sm.end_time - TimeHelper.GetServerTime())
		self.RemainTime.text = string.format(self.language.RemainTime, t.min, t.s)
	end,
	function ()
		-- AnimalRaceCtrl.Instance:SendAnimalRaceOperActivityInfo()
	end, self.data.shitu_fb_sm.end_time)

	local progress = self.data.shitu_fb_sm.progress
	if progress > 9 then
		self.Num1Sp.SpriteName = math.floor(progress / 10)
		self.Num2Sp.SpriteName = progress % 10
		self.Num2Sp:SetObjActive(true)
	else
		self.Num2Sp:SetObjActive(false)
		self.Num1Sp.SpriteName = progress
	end
end

function ShiTuFBMenuView:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushMenu()
end

function ShiTuFBMenuView:OnDisable()
	UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function ShiTuFBMenuView:OnClickProgress()
	ViewMgr:OpenView(SocietyShiTuFBProgressView)
end

function ShiTuFBMenuView:OnClickLeave()
	PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. Language.Society.ShiTuFB.LevelNotComplete, function()
		SocietyCtrl.Instance:SendShiTuReqFinish()
		SceneCtrl.Instance:RequestLeaveFb()
	end)
end