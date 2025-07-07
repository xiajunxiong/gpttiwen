RewardOnlineView = RewardOnlineView or DeclareMono("RewardOnlineView", UIWFlushPanel)

function RewardOnlineView:RewardOnlineView()
	self.data = RewardCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.online_change,  func = self.onFlush}
	}
end

function RewardOnlineView:onFlush()
	self.RewardList:SetData(self.data:GetRewardOnlineList())
end

--------------------------------RewardOnlineItem---------------------------------
RewardOnlineItem = DeclareMono("RewardOnlineItem", UIWidgetBaseItem)

function RewardOnlineItem:RewardOnlineItem()
	self.reward_data = RewardCtrl.Instance:Data()
	self.language = Language.Reward.RewardOnline
	self.btn_state = Language.Reward.BtnState
	self.is_care = false
end

function RewardOnlineItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local today_online_time = self.reward_data:TodayOnlineTime()
	local online_protocol_time = self.reward_data:OnlineProtocolTime()
	local real_online_time = TimeHelper:RealTime() - online_protocol_time + today_online_time
	local condition_desc = self.language.condition_desc
	self.Condition.text = string.format(condition_desc, data.time / 60)
	self.RewardGrid:SetData(data.rewards)
	local is_get = data.get_flag == 1
	local value = not is_get and data.time < real_online_time
	if value then
		self:PlayEffect()
	else
		self:StopEffect()
	end
	self.BtnGetInter.Interactable = value
	self.BtnGet.text = is_get and self.btn_state.Geted or self.btn_state.Get
	TimeHelper:CancelTimer(self.timer_ct)
	if data.time > real_online_time then
		self.total_time = data.time - real_online_time
		self.timer_ct = TimeHelper:AddCountDownTT(BindTool.Bind(self.TimerTTUpdate, self), BindTool.Bind(self.TimerTTComplete, self), self.total_time, 1)
	else
		self.BtnGet.text = is_get and self.btn_state.Geted or self.btn_state.Get
	end
end

function RewardOnlineItem:OnlineRedPoint()
    self.red_point_online = self.red_point_online or UiRedPoint.New(self.BtnGetObj, Vector3.New(77.7,24,0))
	self.red_point_online:SetNum(self.data.online_red_point.val and 1 or 0)
end

function RewardOnlineItem:OnClickGet()
	RewardCtrl.Instance:SendOnlineRewardReq(self.data.index)
end

function RewardOnlineItem:OnDestroy()
	TimeHelper:CancelTimer(self.timer_ct)
end

function RewardOnlineItem:PlayEffect()
	self:StopEffect()
	self.eff_handle = self.EffectTool:Play("10010237")
end

function RewardOnlineItem:StopEffect()
	if self.eff_handle ~= nil then
		self.EffectTool:Stop(self.eff_handle)
		self.eff_handle = nil
	end
end

function RewardOnlineItem:TimerTTUpdate()
	local last_time = self.total_time - 1
	self.total_time = last_time
	local ft = TimeHelper.FormatDHMS(last_time)
	self.BtnGet.text = string.format(self.language.time_format, ft.hour, ft.min, ft.s)
end

function RewardOnlineItem:TimerTTComplete()
	local is_get = self.data.get_flag == 1
	self.BtnGet.text = is_get and self.btn_state.Geted or self.btn_state.Get
	self.BtnGetInter.Interactable = not is_get
end