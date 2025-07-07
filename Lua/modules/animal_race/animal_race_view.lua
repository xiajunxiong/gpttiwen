
AnimalRaceView = AnimalRaceView or DeclareView("AnimalRaceView", AnimalRaceConfig.ResPath .. "animal_race")
VIEW_DECLARE_MASK(AnimalRaceView, ViewMask.None)

function AnimalRaceView:OnClickSociety()
	ViewMgr:OpenView(SocietyView)
end

function AnimalRaceView:OnClickChat()
	ViewMgr:OpenView(ChatView)
end

AnimalRaceViewF = AnimalRaceViewF or DeclareMono("AnimalRaceViewF", UIWFlushPanel)

function AnimalRaceViewF:AnimalRaceViewF()
	self.data = AnimalRaceCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.ar_info, func = self.FlushTime},
	}
end

function AnimalRaceViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function AnimalRaceViewF:FlushTime(cdf)
	cdf = cdf or Language.AnimalRace.CountDownFormat
	local ai = self.data.ar_info
	if AnimalRaceConfig.NONE == ai.info_type then return end
	TimeHelper:CancelTimer(self.timer_ct)
	self.timer_ct = TimeHelper:AddCountDownCT(function ()
		self.RemainTime.text = string.format(cdf[ai.info_type], TimeHelper.TimeFormat1(TimeHelper.FormatDHMS(ai.timestamp - TimeHelper.GetServerTime())))
	end,
	function ()
		AnimalRaceCtrl.Instance:SendAnimalRaceOperActivityInfo()
	end, self.data.ar_info.timestamp)
end

function AnimalRaceViewF:OnClickDetail(value)
	local it = AnimalRaceConfig.InfoType
	local info_type = self.data.ar_info.info_type
	if value and (AnimalRaceConfig.NONE == info_type or it.NotTime == info_type) then return end
	if value then
		if it.RoleReady == info_type then
			AnimalRaceCtrl.Instance:SendAnimalRaceOperRoleInfo()
			self.SupportObj:SetActive(true)
		elseif it.RoleDoing == info_type then
			self.ResultObj:SetActive(true)
		end
	else
		self.SupportObj:SetActive(false)
		self.ResultObj:SetActive(false)
	end
	self.DetailObj:SetActive(value)
end

function AnimalRaceViewF:OnClickTips()
	ViewMgr:OpenView(AnimalRaceRuleView)
end

function AnimalRaceViewF:OnClickBack()
	PublicPopupCtrl.Instance:DialogTips{
		content = Language.AnimalRace.BackTips,
		confirm = {
			func = function()
				SceneCtrl.Instance:RequestLeaveFb()
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end


AnimalRaceViewResult = AnimalRaceViewResult or DeclareMono("AnimalRaceViewResult", UIWFlushPanel)

function AnimalRaceViewResult:AnimalRaceViewResult()
	self.data = AnimalRaceCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.ar_info, func = self.FlushTime},
		{data = self.data.ar_info, func = self.FlushResult},
	}
end

function AnimalRaceViewResult:OnDestroy()
	AnimalRaceViewF.OnDestroy(self)
end

function AnimalRaceViewResult:FlushTime()
	AnimalRaceViewF.FlushTime(self, Language.AnimalRace.CountDownFormat2)
end

function AnimalRaceViewResult:FlushResult()
	self.ResultList:SetData(self.data:GetResultList())
end

function AnimalRaceViewResult:OnClickTips()
	ViewMgr:OpenView(AnimalRaceRuleView)
end


AnimalRaceViewResultItem = DeclareMono("AnimalRaceViewResultItem", UIWidgetBaseItem)

function AnimalRaceViewResultItem:AnimalRaceViewResultItem()
end

function AnimalRaceViewResultItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetIcon(self.IconSp, data.picture_id)
	self.Name.text = data.name
	self.RankSp.SpriteName = string.format("Di%s", data.rank)
end

AnimalRaceViewSupport = AnimalRaceViewSupport or DeclareMono("AnimalRaceViewSupport", UIWFlushPanel)

function AnimalRaceViewSupport:AnimalRaceViewSupport()
	self.data = AnimalRaceCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.ar_info, func = self.FlushTime},
		{data = self.data.detail_info, func = self.FlushSupport},
		{data = BagData.Instance.item_grid, func = self.FlushCoin}
	}
end

function AnimalRaceViewSupport:OnDestroy()
	AnimalRaceViewF.OnDestroy(self)
end

function AnimalRaceViewSupport:FlushTime()
	AnimalRaceViewF.FlushTime(self, Language.AnimalRace.CountDownFormat2)
end

function AnimalRaceViewSupport:FlushSupport()
	self.SupportList:SetData(self.data.player_list)
end

function AnimalRaceViewSupport:FlushCoin()
	self.HaveNum.text = Item.GetNum(AnimalRaceConfig.AnimalCoin)
end

function AnimalRaceViewSupport:OnClickTips()
	ViewMgr:OpenView(AnimalRaceRuleView)
end

AnimalRaceViewSupportItem = DeclareMono("AnimalRaceViewSupportItem", UIWidgetBaseItem)

function AnimalRaceViewSupportItem:AnimalRaceViewSupportItem()
	self.ar_data = AnimalRaceCtrl.Instance:Data()
	self.language = Language.AnimalRace.Support
end

function AnimalRaceViewSupportItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local rate_return = self.ar_data:GetTypeResponseRateById(data.id)
	local support_num = self.ar_data:GetEverythingBetTypeCountById(data.id)
	UH.SetIcon(self.IconSp, data.picture_id)
	self.Name.text = data.name
	self.RateReturn.text = string.format(self.language.RateReturn, rate_return)
	self.SupportNum.text = string.format(self.language.SupportNum, support_num)
end

function AnimalRaceViewSupportItem:OnClickSupport()
	self.ar_data.cur_sel_player = self.data
	ViewMgr:OpenView(AnimalRaceSupportView)
end