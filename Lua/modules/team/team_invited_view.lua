
TeamInvitedView = TeamInvitedView or DeclareView("TeamInvitedView", TeamConfig.ResPath .. "team_invited")

function TeamInvitedView:TeamInvitedView()
	self.data = TeamCtrl.Instance:Data()
end

function TeamInvitedView:LoadCallback()
	-- self.data:TeamInvitedListFlush()
end

function TeamInvitedView:OnClickClose()
	ViewMgr:CloseView(TeamInvitedView)
end

TeamInvitedViewF = TeamInvitedViewF or DeclareMono("TeamInvitedViewF", UIWFlushPanel)

function TeamInvitedViewF:TeamInvitedViewF()
	self.data = TeamCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.team_invited_list, func = self.FlushShow},
	}
end

function TeamInvitedViewF:FlushShow()
	local list = self.data:GetTeamInvitedList()
	self.ShowList:SetData(list)
end

TeamInvitedViewItem = DeclareMono("TeamInvitedViewItem", UIWidgetBaseItem)

function TeamInvitedViewItem:TeamInvitedViewItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.language = Language.Knightly
end

function TeamInvitedViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	local team_goal = self.team_data:GetTeamGoalWithId(data.team_type)
	local add_chivalrous_value = data.add_chivalrous_value or 0
	self.TargetDesc.text = string.format(self.language.TargetDesc, team_goal.desc, self.data.team_member_num)
	self.Knight.text = string.format(self.language.KnightlyDesc2, add_chivalrous_value)
	self.IsCrossObj:SetActive(TeamConfig.CrossType.is_cross == data.is_cross)
	self.Knight:SetObjActive(add_chivalrous_value > 0)
	self.TargetDescPos.localPosition = Vector2.New(20, add_chivalrous_value > 0 and 17 or 0)

	local leader = self.data.leader
	local prof_b = RoleData.ProfIdToProfType(leader.prof)
	self.Name.text = leader.name
	self.ProfSp.SpriteName = RoleData.GetProfSp(prof_b)
	self.Level.text = string.format(self.language.LevelDesc, leader.level)
	UH.SetAvatar(self.IconSp, leader.avatar_type, leader.avatar_id,leader.avatar_quality)
end

function TeamInvitedViewItem:OnClickRefuse()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.team_data:RemoveTeamInvitedInfoByTeamIndex(self.data.team_index)
	TeamCtrl.Instance:SendTeamInvationRet(self.data.leader.uid, false, self.data.is_cross, self.data.team_index)
end

function TeamInvitedViewItem:OnClickAccpet()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.team_data:RemoveTeamInvitedInfoByTeamIndex(self.data.team_index)
	TeamCtrl.Instance:SendTeamInvationRet(self.data.leader.uid, true, self.data.is_cross, self.data.team_index)
end