
TeamViewApprove = TeamViewApprove or DeclareView("TeamViewApprove", TeamConfig.ResPath .. "team_approve")

function TeamViewApprove:TeamViewApprove()
	self.language = Language.Team
	self.data = TeamCtrl.Instance:Data()
end

function TeamViewApprove:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.TeamApprove, Vector2.New(479, 506))
	self.data:NewApproveRedPoint(false)
end

TeamViewApproveDown = TeamViewApproveDown or DeclareMono("TeamViewApproveDown", UIWFlushPanel)

function TeamViewApproveDown:TeamViewApproveDown()
	self.data = TeamCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.FlushDown},
	}
end

function TeamViewApproveDown:FlushDown()
	local team_info = self.data:GetTeamInfo()
	local check_type = team_info.info.check_type
	self.AutoJoin:SetActive(check_type == TeamConfig.TeamCheckType.auto_join)
end

function TeamViewApproveDown:OnClickAutoJoin()
	local team_info = self.data:GetTeamInfo()
	local check_type = team_info.info.check_type
	TeamCtrl.Instance:SendChangeMustCheck(TeamConfig.TeamCheckType.need_check == check_type and TeamConfig.TeamCheckType.auto_join or TeamConfig.TeamCheckType.need_check)
end

function TeamViewApproveDown:OnClickClear()
	self.data:ClearApproveList()
	ViewMgr:FlushView(TeamViewApprove, TeamViewApproveList:GetTypeName())
end


TeamViewApproveList = TeamViewApproveList or DeclareMono("TeamViewApproveList", UIWFlushPanel)

function TeamViewApproveList:TeamViewApproveList()
    self.flush_events = {EventProtocol.ToEventKey(SCJoinReq)}

	self.data = TeamCtrl.Instance:Data()
end

function TeamViewApproveList:onFlush()
	local team_join_list = self.data:GetTeamApproveList()
	self.ShowList:SetData(team_join_list)
end

TeamViewApproveItem = DeclareMono("TeamViewApproveItem", UIWidgetBaseItem)

function TeamViewApproveItem:TeamViewApproveItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamViewApproveItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_id,info.avatar_quality)
end

function TeamViewApproveItem:OnClickAccept()
	local uid = self.data.info.uid
	TeamCtrl.Instance:SendJoinTeamReq(uid)
	self.team_data:RemoveApproveMember(uid)
	ViewMgr:FlushView(TeamViewApprove)
end