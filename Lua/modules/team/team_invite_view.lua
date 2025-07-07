
TeamInviteView = TeamInviteView or DeclareView("TeamInviteView", TeamConfig.ResPath .. "team_invite")

function TeamInviteView:LoadCallback()
	local is_target = TeamData.Instance.is_world_notice == true
	self.BtnNotice:SetActive(is_target) --世界喊话按钮
	self.ListRect.sizeDelta = Vector2.New(435,is_target and 292 or 380)
end

function TeamInviteView:OnClickClose()
	ViewMgr:CloseView(TeamInviteView)
end

function TeamInviteView:OnClickNotice()
	TeamCtrl.Instance:OnSendNotice()
end

TeamInviteViewUpList = TeamInviteViewUpList or DeclareMono("TeamInviteViewUpList", UIWFlushPanel)

function TeamInviteViewUpList:TeamInviteViewUpList()
	self.data = TeamCtrl.Instance:Data()
end

function TeamInviteViewUpList:Awake()
	UIWFlushPanel.Awake(self)
	local up_list = self.data:GetInviteTypeList()
	self.UpList:SetData(up_list)
	self.UpList:ClickItemData(up_list[1])
	self:OnClickInviteType(up_list[1])
end

function TeamInviteViewUpList:OnClickInviteType(data)
	self.data.invite_type.val = data.type
end

TeamInviteViewTypeItem = DeclareMono("TeamInviteViewTypeItem", UIWidgetBaseItem)

function TeamInviteViewTypeItem:TeamInviteViewTypeItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamInviteViewTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function TeamInviteViewTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = data.name
end

TeamInviteViewList = TeamInviteViewList or DeclareMono("TeamInviteViewList", UIWFlushPanel)

function TeamInviteViewList:TeamInviteViewList()
	self.data = TeamCtrl.Instance:Data()
	self.friend_data = SocietyCtrl.Instance:Data()
	self.family_data = GuildCtrl.Instance:Data()
	self.partner_data = PartnerCtrl.Instance:Data()

	self.data:ResetInvitedList()

	self.data_cares = {
        {data = self.data.invite_type, func = self.onFlush },
        {data = self.partner_data:GetPartnerFightOrderList(), func = self.onFlush, init_call = false},
	}
end

function TeamInviteViewList:onFlush()
	local role_list = {}
	if TeamConfig.InviteType.friend == self.data.invite_type.val then
		role_list = self.friend_data:GetFriendListFriendTeamInvite()
	elseif TeamConfig.InviteType.family == self.data.invite_type.val then
		role_list = self.family_data:GetMemberListOnline()
	elseif TeamConfig.InviteType.partner == self.data.invite_type.val then
		role_list = self.data:GetTeamPartnerList()
	end
	self.EmptyObj:SetActive(#role_list == 0)
    self.RoleList:SetData(role_list)
end

TeamInviteViewItem = DeclareMono("TeamInviteViewItem", UIWidgetBaseItem)

function TeamInviteViewItem:TeamInviteViewItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamInviteViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.info = data.info or data
	local role_level = RoleData.Instance:GetRoleLevel()
	local job = self.info.prof or self.info.job
	local prof_sp = RoleData.GetProfSp(job or 0)
	self.Name.text = self.info.name
	self.Level.text = "Lv." .. (self.info.level or role_level)
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	if self.info.partner_id then
		UH.SetIcon(self.IconSp, self.info.icon_id, ICON_TYPE.ITEM)
	else
		local appearance = self.info.appearance or {}
		UH.SetAvatar(self.IconSp, self.info.avatar_type or appearance.avatar_type, self.info.avatar_id or appearance.avatar_id,self.info.avatar_quality or appearance.avatar_quality)
	end
	self:CanInvite()
end

function TeamInviteViewItem:CanInvite()
	local value = self.team_data:IsInInvitedList(self.team_data.invite_type.val, self.info.id or self.info.uid)
	self.BtnInviteObj:SetActive(not value)
	self.InvitedObj:SetActive(value)
end

function TeamInviteViewItem:OnClickInvite()
	if TeamConfig.InviteType.partner == self.team_data.invite_type.val then
		PartnerCtrl.Instance:SendPartnerFight(self.data.id)
	else
		TeamCtrl.Instance:OnTeamInvitationReq(self.info.uid)
	end
	self.team_data:AddToInvitedList(self.team_data.invite_type.val, self.info.id or self.info.uid)
	self:CanInvite()
end



TeamInviteShiTuView = TeamInviteShiTuView or DeclareView("TeamInviteShiTuView", TeamConfig.ResPath .. "team_invite_shitu")

function TeamInviteShiTuView:TeamInviteShiTuView()
	self.language = Language.Society.ShiTuInvite
end

function TeamInviteShiTuView:LoadCallback()
	local seek_master = SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type
	self.TitleShow.text = seek_master and self.language.InviteMaster or self.language.InviteApprentice

	if not TeamData.Instance:InTeam() then
		TeamCtrl.Instance:SendCreateTeam()
	end
end

function TeamInviteShiTuView:OnClickClose()
	ViewMgr:CloseView(TeamInviteShiTuView)
end

TeamInviteShiTuViewF = TeamInviteShiTuViewF or DeclareMono("TeamInviteShiTuViewF", UIWFlushPanel)

function TeamInviteShiTuViewF:TeamInviteShiTuViewF()
	self.data = TeamCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()

	self.data:ResetInvitedList()
end

function TeamInviteShiTuViewF:Awake()
	UIWFlushPanel.Awake(self)
	
	self:FlushShow()
end

function TeamInviteShiTuViewF:FlushShow()
	local seek_master = SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type
	local role_list = self.society_data:GetShiTuListTeamInvite(seek_master)
	self.EmptyObj:SetActive(#role_list == 0)
	self.RoleList:SetData(role_list)
end

TeamInviteViewShiTuItem = DeclareMono("TeamInviteViewShiTuItem", UIWidgetBaseItem)

function TeamInviteViewShiTuItem:TeamInviteViewShiTuItem()
	self.team_data = TeamCtrl.Instance:Data()
end

function TeamInviteViewShiTuItem:SetData(data)
	self.info = data.info
	local role_level = RoleData.Instance:GetRoleLevel()
	local job = self.info.prof
	local prof_sp = RoleData.GetProfSp(job or 0)
	self.Name.text = self.info.name
	self.Level.text = "Lv." .. (self.info.level or role_level)
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)

	local appearance = self.info.appearance or {}
	UH.SetAvatar(self.IconSp, self.info.avatar_type or appearance.avatar_type, self.info.avatar_id or appearance.avatar_id,self.info.avatar_quality or appearance.avatar_quality)

	self:CanInvite()
end

function TeamInviteViewShiTuItem:CanInvite()
	local value = self.team_data:IsInInvitedList(TeamConfig.InviteType.shitu, self.info.uid)
	self.BtnInviteObj:SetActive(not value)
	self.InvitedObj:SetActive(value)
end

function TeamInviteViewShiTuItem:OnClickInvite()
	TeamCtrl.Instance:OnTeamInvitationReq(self.info.uid)
	self.team_data:AddToInvitedList(TeamConfig.InviteType.shitu, self.info.uid)
	self:CanInvite()
end
