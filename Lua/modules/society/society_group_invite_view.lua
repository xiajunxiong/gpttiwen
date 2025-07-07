
SocietyGroupInviteView = SocietyGroupInviteView or DeclareView("SocietyGroupInviteView", SocietyConfig.ResPath .. "society_group_invite")

function SocietyGroupInviteView:SocietyGroupInviteView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyGroupInviteView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyGroupInvite, Vector2.New(905, 602))
end

SocietyGroupInviteViewF = SocietyGroupInviteViewF or DeclareMono("SocietyGroupInviteViewF", UIWFlushPanel)

function SocietyGroupInviteViewF:SocietyGroupInviteViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.ChatGroupInfo
	
	self.data_cares = {
		{data = self.data.chat_group_detail, func = self.FlushRoleList},
		{data = self.data.chat_group_sels, func = self.FlushNumSel, init_call = false},
	}
end

function SocietyGroupInviteViewF:FlushRoleList()
	-- if self.data.chat_group_detail.group_id ~= self.data.cur_sel_chat_group:GroupId() then return end
	self.data:ResetFriendChatGroupSels()
	local role_list = self.data:GetFriendChatGroupInviteList()
	local str = self.InputField.text
	if not StringIsEmpty(str) then role_list = self.data.SearchOne(role_list, str) end
	self.RoleList:SetData(role_list)
end

function SocietyGroupInviteViewF:FlushNumSel()
	self.NumSel.text = string.format(self.language.InviteNum, self.data.chat_group_sels.num)
end

function SocietyGroupInviteViewF:OnClickSearch()
	self:FlushRoleList()
end

function SocietyGroupInviteViewF:OnClickInvite()
	local uid_list = self.data.chat_group_sels.uid_list
	for _, uid in pairs(uid_list) do
        SocietyCtrl.Instance:SendRoleInviteEnterGroup(self.data.chat_group_detail.group_id, uid)
	end
	-- ViewMgr:CloseView(SocietyGroupInviteView)
end

SocietyGroupInviteViewItem = DeclareMono("SocietyGroupInviteViewItem", UIWidgetBaseItem)

function SocietyGroupInviteViewItem:SocietyGroupInviteViewItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyGroupInviteViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	local is_online = info.is_online >= 1
	self.Name.text = info.name
	self.Level.text = info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI:SetObjActive(nil ~= prof_sp)
	self.Toggle.isOn = false
	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_quality)
	self.MaskObj:SetActive(not is_online)
	self.RoleState.text = is_online and Language.Society.RoleOnline or TimeHelper.Ago(info.last_online_timestamp)
end

function SocietyGroupInviteViewItem:OnClickSel()
    local info = self.data.info
	self.society_data:SetFriendChatGroupSels(info.uid, self.Toggle.isOn)
end

function SocietyGroupInviteViewItem:OnClickTips()
    PublicPopupCtrl.Instance:Center(Language.Society.InviteError)
end

