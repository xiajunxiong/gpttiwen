
TeamViewNotice = TeamViewNotice or DeclareView("TeamViewNotice", TeamConfig.ResPath .. "team_notice")

function TeamViewNotice:TeamViewNotice()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team
end

function TeamViewNotice:LoadCallback()
end

function TeamViewNotice:OnFlush()
	local team_detail = self.language.TeamDetail
    self.Goal.text = RichTextHelper.ColorStr(team_detail.goal .. "：", COLORSTR.Red7) .. self.data.real_goal_goal.desc
    self.Level.text = RichTextHelper.ColorStr(team_detail.level .. "：", COLORSTR.Red7) .. self.data.real_goal_level_low .. "-" .. self.data.real_goal_level_high
end

function TeamViewNotice:OnClickClose()
	ViewMgr:CloseView(TeamViewNotice)
end

function TeamViewNotice:OnClickSend()
end

function TeamViewNotice:OnClickCancel()
	ViewMgr:CloseView(TeamViewNotice)
end