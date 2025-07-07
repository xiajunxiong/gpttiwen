
TeamGoalView = TeamGoalView or DeclareView("TeamGoalView", TeamConfig.ResPath .. "team_goal")

function TeamGoalView:TeamGoalView()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team
end

function TeamGoalView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.TeamGoalSel, Vector2.New(865, 570))
end

TeamGoalViewOffset = TeamGoalViewOffset or DeclareMono("TeamGoalViewOffset", UIWFlushPanel)

function TeamGoalViewOffset:TeamGoalViewOffset()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team
	self.data:ReplaceGoalCurToReal()

	self.data_cares = {
		{data = self.data.cur_goal_type, func = self.FlushTeamGoalList, init_call = false},
		{data = self.data.cur_goal_goal, func = self.FlushTeamLevelList, init_call = false},
	}

	TeamGoalViewOffset.is_first_init1 = false
	TeamGoalViewOffset.is_first_init2 = false

	self.flush_item_handle = nil
end

function TeamGoalViewOffset:Awake()
	UIWFlushPanel.Awake(self)
	local is_cross = self.data:IsCross()
	self:FlushTeamTypeList()
	self:FlushTeamGoalList()
	self:FlushTeamLevelList()
	self.GoalList:ClickItemData(self.data.real_goal_goal)
	self.IsCrossSel:SetActive(is_cross)
end

function TeamGoalViewOffset:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	if self.flush_item_handle ~= nil then
		TimeHelper:CancelTimer(self.flush_item_handle)
		self.flush_item_handle = nil
	end
end

function TeamGoalViewOffset:FlushTeamTypeList()
	local team_type_list = self.data:GetTeamTypeList()
	self.TypeList:SetData(team_type_list)
	self.TypeList:ClickItemData(self.data.cur_goal_type:Val())
end

function TeamGoalViewOffset:FlushTeamGoalList()
	local team_goal_type_list = self.data:GetTeamGoalListWithType(self.data.cur_goal_type.team_type, true)
	self.GoalList:SetDataList(team_goal_type_list)
end

function TeamGoalViewOffset:FlushTeamLevelList()
	local team_level_list, t_low, t_high =self.data:GetTeamLevelList()
	self.LevelLList:SetData(team_level_list)
	self.LevelLList:ClickItemData(t_low)
	self.LevelHList:SetData(team_level_list)
	self.LevelHList:ClickItemData(t_high)
	-- self.LevelHList:JumpToDown()
	if self.flush_item_handle ~= nil then
		TimeHelper:CancelTimer(self.flush_item_handle)
		self.flush_item_handle = nil
	end

	self.flush_item_handle = TimeHelper:AddDelayFrameTimer(function ()
		self.flush_item_handle = nil
		if t_low and t_high then
			self.LevelLList:JumpVerticalIndex(t_low.level - self.data.cur_goal_goal.low_level,35)
			self.LevelHList:JumpVerticalIndex(t_high.level - self.data.cur_goal_goal.low_level,35)
		end
	end)
end

function TeamGoalViewOffset:OnClickMatch()
	self.data:ReplaceGoalRealToCur()
	local in_team = self.data:InTeam()
	local is_leader = self.data:IsLeader()
	local is_cross = self.data:IsCross()
	if in_team and is_leader then
		TeamCtrl.Instance:SendChangeTeamLimitAuto()
		if not is_cross and self.IsCrossSel.activeSelf then
			TeamCtrl.Instance:SendTeamSwitchToCross()
		end
	end
	ViewMgr:CloseView(TeamGoalView)
end

function TeamGoalViewOffset:OnClickIsCross()
	local is_cross = self.data:IsCross()
	if is_cross then
		PublicPopupCtrl.Instance:Center(self.language.TipsCrossSwitch)
	else
		self.IsCrossSel:SetActive(not self.IsCrossSel.activeSelf)
	end
end

TeamGoalViewItem = DeclareMono("TeamGoalViewItem", UIWidgetBaseItem)

function TeamGoalViewItem:TeamGoalViewItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamGoalViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

TeamGoalViewItemType = DeclareMono("TeamGoalViewItemType", TeamGoalViewItem)

function TeamGoalViewItemType:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = data.team_target
end

function TeamGoalViewItemType:OnClickItem()
	if not TeamGoalViewOffset.is_first_init1 then
		TeamGoalViewOffset.is_first_init1 = true
		return
	end
	self.ToggleGroupGoal:SetAllTogglesOff()
	self.team_data.cur_goal_type:Set(self.data)
	self.team_data:ResetCurGoalGoal()
	self.team_data:ResetCurGoalLevel()
end

TeamViewGoalItemGoal = DeclareMono("TeamViewGoalItemGoal", TeamGoalViewItem)

function TeamViewGoalItemGoal:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.GoalName.text = (data.desc_fb or data.desc) .. (data.is_complete and RichTextHelper.SizeStr(RichTextHelper.ColorStr(Language.Team.TargetComplete, COLORSTR.Green6), 20) or "")
	self.GoalInter.Interactable = not LoginData.Instance:IsOnCrossSever() or TeamConfig.CrossType.is_cross == data.is_cross
end

function TeamViewGoalItemGoal:OnClickItem()
	if not TeamGoalViewOffset.is_first_init2 then
		TeamGoalViewOffset.is_first_init2 = true
		return
	end
	self.team_data.cur_goal_goal:Set(self.data)
	self.team_data:ResetCurGoalLevel()
	self.IsCrossObj:SetActive(TeamConfig.CrossType.is_cross == self.data.is_cross)
end

TeamViewGoalItemLevel = DeclareMono("TeamViewGoalItemLevel", TeamGoalViewItem)

function TeamViewGoalItemLevel:Click()
	if self.Toggle then
		self.Toggle:SetObjActive(true)
		self.Toggle:Trigger()
	end
end

function TeamViewGoalItemLevel:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.LevelName.text = data.level
end

TeamViewGoalItemLevelL = DeclareMono("TeamViewGoalItemLevelL", TeamViewGoalItemLevel)

function TeamViewGoalItemLevelL:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.LevelName.text = data.level
	self.LevelInter.Interactable = data.level <= RoleData.Instance:GetRoleLevel()
end

function TeamViewGoalItemLevelL:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
		self.team_data.cur_goal_level_low = self.data.level <= self.team_data.cur_goal_level_high and self.data.level or self.team_data.cur_goal_level_high
		self.team_data.cur_goal_level_high = self.data.level <= self.team_data.cur_goal_level_high and self.team_data.cur_goal_level_high or self.data.level
    end
end

TeamViewGoalItemLevelH = DeclareMono("TeamViewGoalItemLevelH", TeamViewGoalItemLevel)

function TeamViewGoalItemLevelH:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
		self.team_data.cur_goal_level_high = self.data.level <= self.team_data.cur_goal_level_low and self.team_data.cur_goal_level_low or self.data.level
		self.team_data.cur_goal_level_low = self.data.level <= self.team_data.cur_goal_level_low and self.data.level or self.team_data.cur_goal_level_low
    end
end