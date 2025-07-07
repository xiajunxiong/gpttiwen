
TeamViewInvite = TeamViewInvite or DeclareView("TeamViewInvite", TeamConfig.ResPath .. "team_invite")

function TeamViewInvite:TeamViewInvite()
	self.language = Language.Team
	self.data = TeamCtrl.Instance:Data()
end

function TeamViewInvite:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.TeamInvite, Vector2.New(479, 506))
end

function TeamViewInvite:OnClickInviteType(type)
	self.data.invite_type = type
	ViewMgr:FlushView(TeamViewInvite, "TeamViewInviteList")
end

TeamViewInviteList = TeamViewInviteList or DeclareMono("TeamViewInviteList", UIWFlushPanel)

function TeamViewInviteList:TeamViewInviteList()
	self.data = TeamCtrl.Instance:Data()
	self.friend_data = SocietyCtrl.Instance:Data()
	self.family_data = GuildCtrl.Instance:Data()
	self.data.invite_type = TeamConfig.InviteType.friend
end

function TeamViewInviteList:onFlush()
	local role_list = {}
	if TeamConfig.InviteType.friend == self.data.invite_type then
		role_list = self.friend_data:GetFriendListFriendTeamInvite()
	elseif TeamConfig.InviteType.family == self.data.invite_type then
		role_list = self.family_data:GetMemberListOnline()
	end
    self.RoleList:SetData(role_list)
end

function TeamViewInviteList:OnClickItem(data)
    self.data.cur_sel_partner = data.id
    ViewMgr:FlushView(PartnerView, PartnerViewUp:GetTypeName())
    ViewMgr:FlushView(PartnerView, PartnerViewAttr:GetTypeName())
end

TeamViewInviteItem = DeclareMono("TeamViewInviteItem", UIWidgetBaseItem)

function TeamViewInviteItem:TeamViewInviteItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function TeamViewInviteItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.info = data.info or data
	local prof_sp = RoleData.GetProfSp(self.info.prof)
	self.Name.text = self.info.name
	self.Level.text = "Lv." .. self.info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	if nil ~= self.info.uid then self.Uid.text = "ID:" .. self.info.uid end
end

function TeamViewInviteItem:OnClickInvite()
    TeamCtrl.Instance:SendTeamInvitationReq(self.info.uid)
end