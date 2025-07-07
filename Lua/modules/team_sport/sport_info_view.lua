SportInfoView = SportInfoView or DeclareView("SportInfoView","team_sport/sport_info_view")
VIEW_DECLARE_MASK(SportInfoView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(SportInfoView, ViewLevel.Lu)
function SportInfoView:SportInfoView()
	
end

function SportInfoView:LoadCallback()
	if ViewMgr:IsOpen(TeamSportView) then
		ViewMgr:CloseView(TeamSportView)
	end

	if TombRaiderData.Instance.tomb_matching or ViewMgr:IsOpen(TombRaiderView) then 
		-- ViewMgr:CloseView(TombRaiderView)
		ViewMgr:CloseView(SportInfoView)
		self.tomb_mark = true
	end 
end

function SportInfoView:CloseCallback()
	if self.tomb_mark then 
		if TombRaiderData.Instance.tomb_matching and not ViewMgr:IsOpen(TombRaiderView) then 
			ViewMgr:OpenView(TombRaiderBattleView)
		end 
	    -- ViewMgr:MainViewVisible(false)
	    -- ViewMgr:OpenView(TombRaiderBattleView)
	end 
end

SportInfoPanel = SportInfoPanel or DeclareMono("SportInfoPanel",UIWFlushPanel)
function SportInfoPanel:SportInfoPanel()
	self.data = TeamSportData.Instance
end

function SportInfoPanel:Awake()
	UIWFlushPanel.Awake(self)
	self.remind_time = 5
	self.timer_tt = TimeHelper:AddCountDownTT(BindTool.Bind(self.TimerCTUpdate, self), BindTool.Bind(self.TimerCTComplete, self), self.remind_time, 1)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MatchSuccess)
	self:FlushPanel()
end

function SportInfoPanel:FlushPanel()
	local user_data = self.data:GetAllUserList()
	if user_data == nil or user_data=={} then return end
	for i=1,# user_data.left_user_list do
		self.LeftPlayers[i].gameObject:SetActive(true)
		self.LeftPlayers[i]:SetData(user_data.left_user_list[i])
	end

	for i=1,# user_data.right_user_list do
		self.RightPlayers[i].gameObject:SetActive(true)
		self.RightPlayers[i]:SetData(user_data.right_user_list[i])
	end

	self.data.match_data.is_complete = 0
end

function SportInfoPanel:TimerCTUpdate()
	UH.SetText(self.TimeText,self.remind_time.."")
	self.remind_time = self.remind_time-1
end

function SportInfoPanel:TimerCTComplete()
	ViewMgr:CloseView(SportInfoView)
end

function SportInfoPanel:OnDestroy()
	TimeHelper:CancelTimer(self.timer_tt)
	UIWFlushPanel.OnDestroy(self)
end

SportInfoItem = SportInfoItem or DeclareMono("SportInfoItem",UIWidgetBaseItem)

function SportInfoItem:SetData(data)
	local names = Split(data.name,"_")
	local name = names[1] or ""
	local server_name = names[2] or ""

	if data.rank_config == nil then
		UH.SetText(self.Name,name)
		UH.SetText(self.ServerName,server_name)
	else
		UH.SetText(self.ServerName,ColorStr(name,COLORSTR.Yellow19) .. "_" .. server_name)
		UH.SpriteName(self.Prof, ProfSettingB[data.prof])
		UH.SetText(self.Rank,data.is_top_race == 1 and BattlefieldData.QuaName(data.rank_config) or ColorStr(Language.Battlefield.NotRankTip,COLORSTR.Gray4))
	end
	if data.is_leader ~= nil then
		self.Captain:SetActive(data.is_leader == true)
	end
	UH.SetAvatarp(self.Head,data.avatar_type or 1,data.avatar_id,data.avatar_quality)
end