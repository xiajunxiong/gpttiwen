
TeamPlatformView = TeamPlatformView or DeclareView("TeamPlatformView", TeamConfig.ResPath .. "team_platform")
VIEW_DECLARE_LEVEL(TeamPlatformView,ViewLevel.L1)

function TeamPlatformView:TeamPlatformView()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team
	self.data.platform_show_all = false
end

function TeamPlatformView:LoadCallback()
end

function TeamPlatformView:CloseCallback()
	self.data.auto_sel_target = false
end

function TeamPlatformView:OnClickClose()
	if not self.data:IsAutoMatch().value then
		MainViewData.Instance:ToggleTTTask()
	end
	ViewMgr:CloseView(TeamPlatformView)
end


TeamPlatformViewOffset = TeamPlatformViewOffset or DeclareMono("TeamPlatformViewOffset", UIWFlushPanel)

function TeamPlatformViewOffset:TeamPlatformViewOffset()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team

	self.data_cares = {
		{data = self.data:IsAutoMatch(), func = self.AutoMatching},
		{data = self.data.team_list, func = self.FlushTeamList},
		{data = self.data:GetSalaryList(), func = self.FlushLeaderProgress, init_call = true},
	}

	GuideManager.Instance:RegisterGetGuideUi("BtnTeamMatch", function()
        return self.BtnMatch, function ()
            self:OnClickMatch()
        end
	end)
	GuideManager.Instance:RegisterGetGuideUi("BtnTeamMatch2", function()
		self.IsCrossToggle.isOn = false
        return self.BtnMatch, function ()
            self:OnClickMatch()
        end
	end)
	
	self.data.platform_click_num = 0
end

function TeamPlatformViewOffset:Awake()
	UIWFlushPanel.Awake(self)
	local is_auto_match = self.data:IsAutoMatch()
	if not GuideManager.Instance:IsGuide() and SceneData.Instance:GetMode() ~= SCENE_TYPE.GUILD_FIGHT then
		if not is_auto_match.value and not self.data.auto_sel_target then self.data.cur_platform_target_id = nil end
	end
	local team_platform_show_list, target, cp, pp = self.data:GetTeamPlatformShowList(self.data.cur_platform_target_id or 1)
	self.ShowList:SetData(team_platform_show_list, true)
	self.ShowList:ClickChild(target)
	self.ShowList:JumpVerticalIndex(cp,66,pp,60)
end

function TeamPlatformViewOffset:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.matching_runner ~= nil then
		TimeHelper:CancelTimer(self.matching_runner)
	end
end

function TeamPlatformViewOffset:FlushTeamList()
	local team_list = self.data:GetTeamList()
	self.TeamList:SetData(team_list)
	self.EmptyObj:SetActive(self.data.team_list_empty)
end

-- 刷新队长工资进度条
function TeamPlatformViewOffset:FlushLeaderProgress()
	local now_reward = 0
	local total_reward = 0
	for i, v in pairs(self.data:GetSalaryList()) do
		now_reward = now_reward + v.vo.times * v.info.reward
		total_reward = total_reward + v.info.week_limit * v.info.reward
	end
	self.Progress:SetProgress(now_reward / total_reward)
	self.Progress:SetText(Format("%s/%s", now_reward, total_reward))
end

function TeamPlatformViewOffset:OpenSalaryView()
	ViewMgr:OpenView(TeamSalaryView)
end

function TeamPlatformViewOffset:AutoMatching()
	local is_auto_match = self.data:IsAutoMatch()
	self.MatchingObj:SetActive(is_auto_match.value)
	if is_auto_match.value then
		self.runner_count = 1
		self.matching_runner = TimeHelper:AddRunTimer(function ()
			local str = Language.Team.TeamBtnName.matching
			for i = 1, self.runner_count do
				str = str .. ". "
			end
			UH.SetText(self.MatchBtnTxt, str)
			self.runner_count = self.runner_count + 1
			if self.runner_count > 3 then
				self.runner_count = 1
			end
		end)
	else
		if self.matching_runner then
			TimeHelper:CancelTimer(self.matching_runner)
		end
		UH.SetText(self.MatchBtnTxt, Language.Team.TeamBtnName.auto_match)
	end
end

function TeamPlatformViewOffset:OnClickFlush()
	TeamCtrl.Instance:SendUnfullTeamListByType(self.data.cur_sel_target.target_id)
end

function TeamPlatformViewOffset:OnClickMatch()
	local is_auto_match = self.data:IsAutoMatch()
	local target = self.data:GetTeamGoalWithId(self.data.platform_fb_goal.target_id) or {}
	-- local is_show_match_type = 2 == target.target_id or TeamConfig.TeamTargetType.Storybook == target.team_type
	local is_show_match_type = false
	if not is_auto_match.value then
		if is_show_match_type then
			self.MatchTypeObj:SetActive(true)
		else
			self.data.cur_platform_target_id = self.data.cur_sel_target.target_id
			TeamCtrl.Instance:SendTeamMatch(self.data.cur_sel_target.target_id, self.data.cur_sel_cross)		
			MainViewData.Instance:ToggleTTTeam()
		end
	else
		self.data.cur_platform_target_id = nil
		TeamCtrl.Instance:SendTeamMatch(self.data.cur_sel_target.target_id, self.data.cur_sel_cross)
	end
end

function TeamPlatformViewOffset:OnClickMatchLeader()
	self.MatchTypeObj:SetActive(false)
	local is_open, str = FunOpen.Instance:GetFunIsOpened(Mod.Team.Main)
	if not is_open then
		PublicPopupCtrl.Instance:Center(str)
	elseif BattleData.Instance:IsServerFighting() then
		PublicPopupCtrl.Instance:Center(ErrorText[156])
	else
		if MainPlayer then
			MainPlayer:StopMove()
		end
		TeamCtrl.Instance:SendCreateTeam(self.data.cur_sel_cross, 1)
		local co = Config.team_cfg_auto.special_team[self.data.cur_sel_target.target_id]
		if co then
			SceneLogic.Instance:AutoToNpc(co.npc_seq, nil, nil, true)
		end
	end
end

function TeamPlatformViewOffset:OnClickMatchMember()
	self.MatchTypeObj:SetActive(false)
	self.data.cur_platform_target_id = self.data.cur_sel_target.target_id
	TeamCtrl.Instance:SendTeamMatch(self.data.cur_sel_target.target_id, self.data.cur_sel_cross, nil, 0)
	MainViewData.Instance:ToggleTTTeam()
end

function TeamPlatformViewOffset:OnClickCreate()
	local is_open, str = FunOpen.Instance:GetFunIsOpened(Mod.Team.Main)
	if not is_open then
		PublicPopupCtrl.Instance:Center(str)
	elseif BattleData.Instance:IsServerFighting() then
		PublicPopupCtrl.Instance:Center(ErrorText[156])
	else
		if MainPlayer then
			MainPlayer:StopMove()
		end
		TeamCtrl.Instance:SendCreateTeam(self.data.cur_sel_cross, self.data.platform_click_num > 1 and 1 or 0)
	end
end

function TeamPlatformViewOffset:OnClickShowAll()
	self.data.platform_show_all = true
	TeamCtrl.Instance:SendUnfullTeamListByType(0)
end

function TeamPlatformViewOffset:OnClickGoalItem(data)
	self.ButtonList:SetObjActive(nil ~= data.details and nil == self.data.cur_platform_target_id)
	if data.details then
		if nil == self.data.cur_platform_target_id then
			self.ButtonList:SetData(data.details)
		else
			local value = TeamConfig.CrossType.is_cross == data.is_cross
			self.IsCrossObj:SetActive(value)
			self.data.cur_sel_cross = value and self.IsCrossToggle.isOn and TeamConfig.CrossType.is_cross or TeamConfig.CrossType.not_cross
		end
		self.data.cur_platform_target_id = nil
		return
	end
	self.data.platform_click_num = (self.data.platform_click_num or 0) + 1
	self.data.platform_show_all = false
	local is_auto_match = self.data:IsAutoMatch()
	if is_auto_match.value and self.pre_data and data ~= self.pre_data then
		self.ShowList:ClickChild(self.pre_data)
		PublicPopupCtrl.Instance:Center(self.language.TipsMatch)
		return
	end
	self.pre_data = data
	self.data.cur_sel_target = data
	TeamCtrl.Instance:SendUnfullTeamListByType(data.target_id)

	local value = TeamConfig.CrossType.is_cross == data.is_cross
	self.IsCrossObj:SetActive(value)
	self.data.cur_sel_cross = value and self.IsCrossToggle.isOn and TeamConfig.CrossType.is_cross or TeamConfig.CrossType.not_cross
	-- if value then self.IsCrossToggle.isOn = true end
	self.data.platform_fb_goal:Set(data)
	-- if not self.data:IsAutoMatch() then self.data.cur_platform_target_id = nil end
end

function TeamPlatformViewOffset:OnClickGoalItemClick(data)
	if data.details then self:OnClickGoalItem(data) end
end

function TeamPlatformViewOffset:OnClickButtonsBlock()
	self:OnClickGoalItem(self.ButtonList:GetData()[1])
	self.ButtonList:SetObjActive(false)
end

function TeamPlatformViewOffset:OnClickMatchBlock()
	self.MatchTypeObj:SetActive(false)
end

function TeamPlatformViewOffset:OnClickIsCross()
	local is_auto_match = self.data:IsAutoMatch()
	local cur_sel_cross = (self.IsCrossObj.activeSelf and self.IsCrossToggle.isOn) and TeamConfig.CrossType.is_cross or TeamConfig.CrossType.not_cross
	if LoginData.Instance:IsOnCrossSever() and cur_sel_cross ~= self.data.cur_sel_cross then
		self.IsCrossToggle.isOn = true
		PublicPopupCtrl.Instance:Center(self.language.TipsCrossSwitch)
		return
	end
	if is_auto_match.value and cur_sel_cross ~= self.data.cur_sel_cross then
		self.IsCrossToggle.isOn = TeamConfig.CrossType.is_cross == self.data.cur_sel_cross
		PublicPopupCtrl.Instance:Center(self.language.TipsMatch)
		return
	end
	self.data.cur_sel_cross = cur_sel_cross
end

TeamPlatformViewAIP = DeclareMono("TeamPlatformViewAIP", UIWidgetBaseItem)

function TeamPlatformViewAIP:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.name
end

TeamPlatformViewAIC = DeclareMono("TeamPlatformViewAIC", UIWidgetBaseItem)

function TeamPlatformViewAIC:TeamPlatformViewAIC()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamPlatformViewAIC:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	self:uncareData()
	self.care_handles = {}
	self:careData(self.team_data.platform_fb_goal, BindTool.Bind(self.FlushName, self))

	self.GoalName.text = data.desc
	-- self.CompleteObj:SetActive(data.is_complete)
	self:FlushName()
end

function TeamPlatformViewAIC:OnDestroy()
    self:uncareData()
end

function TeamPlatformViewAIC:Click()
    if self.Toggle then
		self.Toggle:Trigger()
	end
end

function TeamPlatformViewAIC:FlushName()
	if self.data and self.data.details then
		local platform_fb_goal = self.team_data.platform_fb_goal
		self.GoalName.text = (platform_fb_goal.team_type2 == self.data.team_type2 and platform_fb_goal.param_1 == self.data.param_1) and string.format(Language.Team.Platform2Front[self.data.team_type2][self.data.param_1], platform_fb_goal.desc_fb or platform_fb_goal.desc) or self.data.desc
	end
end

TeamPlatformViewGoalItem = DeclareMono("TeamPlatformViewGoalItem", UIWidgetBaseItem)

function TeamPlatformViewGoalItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.GoalName.text = data.desc
	self.CompleteObj:SetActive(data.is_complete)
end

TeamPlatformViewItem = DeclareMono("TeamPlatformViewItem", UIWidgetBaseItem)

function TeamPlatformViewItem:TeamPlatformViewItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Team.Platform
end

function TeamPlatformViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local team_goal = self.team_data:GetTeamGoalWithId(info.team_type)
	local level_desc = self.language.level_desc
	self.Name.text = team_goal.desc
	self.Level.text = string.format(level_desc, info.limit_level_low, info.limit_level_high)
	self.IsCrossObj:SetActive(TeamConfig.CrossType.is_cross == info.is_cross)
	
	local member_list = data.member_list
	for i = 1, self.Members:Length() do
		self.Members[i]:SetData(member_list[i])
	end
end

function TeamPlatformViewItem:OnClickApply()
	local is_open, str = FunOpen.Instance:GetFunIsOpened(Mod.Team.Main)
	if not is_open then
		PublicPopupCtrl.Instance:Center(str)
		return
	end
	TeamCtrl.Instance:SendJoinTeam(self.data.info.team_index, self.data.info.is_cross)
	TeamCtrl.Instance:SendUnfullTeamListByType(self.team_data.platform_show_all and 0 or self.team_data.cur_sel_target.target_id)
end

TeamPlatformViewItemM = DeclareMono("TeamPlatformViewItemM", UIWidgetBaseItem)

function TeamPlatformViewItemM:TeamPlatformViewItemM()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Team.Platform
end

function TeamPlatformViewItemM:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	self.EmptyObj:SetActive(nil == info)
	self.InfoObj:SetActive(nil ~= info)
	if nil == info then return end
	local prof_b = RoleData.ProfIdToProfType(info.prof)
	self.Name.text = info.name
	self.ProfSp.SpriteName = RoleData.GetProfSp(prof_b)
	self.Level.text = info.level
	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
end

function TeamPlatformViewItemM:OnClickItem()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.data.info.uid)
    RoleInfoCtrl.Instance:SendReferRoleEquipment(self.data.info.uid)
end