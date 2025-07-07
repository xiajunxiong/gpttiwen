

KnightlyView = KnightlyView or DeclareView("KnightlyView", KnightlyConfig.ResPath .. "knightly", Mod.Knight.Main)
VIEW_DECLARE_LEVEL(KnightlyView,ViewLevel.L0)

function KnightlyView:KnightlyView()
	self.data = KnightlyCtrl.Instance:Data()
	self.language = Language.Knightly
	KnightlyCtrl.Instance:SendChivalrousTeamListReq()
end

function KnightlyView:LoadCallback()
	self.Currency:SetCurrency(CurrencyType.Chivalrous)
end

function KnightlyView:OnClickReturn()
	ViewMgr:CloseView(KnightlyView)
end


KnightlyViewF = KnightlyViewF or DeclareMono("KnightlyViewF", UIWFlushPanel)

function KnightlyViewF:KnightlyViewF()
	self.data = KnightlyCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Knightly

	self.data_cares = {
		{data = self.data.team_list, func = self.FlushTeamList},
		{data = self.data.knightly_info, func = self.FlushInfo},
		{data = self.data.knightly_reward_red_point, func = self.FlushRewardRedPoint},
		{data = self.data.flush_grey, func = self.FlushGrey},
		{data = self.team_data:GetKnightlyInfo(), func = self.FlushWeekInfo},
	}
	GuideManager.Instance:RegisterGetGuideUi("XiaYiHuoBi",function ()
		return self.Guide1, function ()
			
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("XiaYiGuiZe",function ()
		return self.Guide2, function ()
			
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("XiaYiJiangLi",function ()
		return self.Guide3, function ()
			
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("XiaYiShangDian",function ()
		return self.BtnShop
	end)
	GuideManager.Instance:RegisterGetGuideUi("XiaYiGuiZeBtn",function ()
		return self.GuideTips
	end)
	--[[ if UnityPlayerPrefs.GetInt(PrefKeys.XiaYiFirstGuide()) == 0 then
		UnityPlayerPrefs.SetInt(PrefKeys.XiaYiFirstGuide(), 1)
	end ]]
	-- GuideManager.Instance:Start(ConstGuideId.XiaYiFirstOpen)
end

function KnightlyViewF:FlushTeamList()
	local team_list = self.data:GetTeamList()
	self.TeamList:SetData(team_list)
	self.EmptyObj:SetActive(self.data.team_list_empty)
end

function KnightlyViewF:FlushInfo()
	local reward = self.data:GetHistoryInfo()
	self.HistoryObj:SetActive(nil ~= reward)
	if reward then
		local history_chivalrous = self.data.knightly_info.history_chivalrous
		self.NumNext.text = string.format(self.language.HistoryDesc, reward.chivalrous_value - history_chivalrous)
		self.HistoryCell:SetData(reward.rewards[1])
	end
end

function KnightlyViewF:FlushRewardRedPoint()
	self.red_point_reward = self.red_point_reward or UiRedPoint.New(self.BtnRewardObj, Vector3.New(77.7,24))
	self.red_point_reward:SetNum(self.data.knightly_reward_red_point.val and 1 or 0)
end

function KnightlyViewF:FlushWeekInfo()
	local knightly_info = self.team_data:GetKnightlyInfo()
	self.NumCur.text = knightly_info.today
	self.NumTotal.text = knightly_info.today_a
end

function KnightlyViewF:FlushGrey()
	local value = self.data.flush_grey.val
	self.BtnFlushInter.Interactable = 0 == value
	self.BtnFlush.text = 0 == value and self.language.Flush or string.format(self.language.FlushGrey, value)
end

function KnightlyViewF:OnClickTips()
	ViewMgr:OpenView(TeamKnightlyView)
end

function KnightlyViewF:OnClickFlush()
	local value = self.data.flush_grey.val
	if value > 0 then
		PublicPopupCtrl.Instance:Center(self.language.FlushTips)
		return
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	KnightlyCtrl.Instance:SendChivalrousTeamListReq()
	self.data:FlushGrey()
end

function KnightlyViewF:OnClickTips()
	ViewMgr:OpenView(TeamKnightlyView)
end

function KnightlyViewF:OnClickShop()
	StoreData.Instance:SetJumpIndex({conver_type = 1})
    ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function KnightlyViewF:OnClickRank()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	ViewMgr:OpenView(KnightlyRankView)
end

function KnightlyViewF:OnClickReward()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	ViewMgr:OpenView(KnightlyRewardView)
end

KnightlyViewItem = DeclareMono("KnightlyViewItem", UIWidgetBaseItem)

function KnightlyViewItem:KnightlyViewItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Knightly
end

function KnightlyViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local team_goal = self.team_data:GetTeamGoalWithId(info.team_type)
	self.Name.text = team_goal.desc
	self.Knight.text = string.format(self.language.KnightlyDesc, info.add_chivalrous_value or 0)
	self.IsCrossObj:SetActive(TeamConfig.CrossType.is_cross == info.is_cross)
	
	local member_list = data.member_list
	for i = 1, self.Members:Length() do
		self.Members[i]:SetData(member_list[i])
	end
end

function KnightlyViewItem:OnClickApply()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	TeamCtrl.Instance:SendJoinTeam(self.data.info.team_index, self.data.info.is_cross)
	KnightlyCtrl.Instance:SendChivalrousTeamListReq()
end