
SocietyViewFriendEditNewGroup = SocietyViewFriendEditNewGroup or DeclareView("SocietyViewFriendEditNewGroup", SocietyConfig.ResPath .. "society_friend_edit_new_group")

function SocietyViewFriendEditNewGroup:SocietyViewFriendEditNewGroup()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyViewFriendEditNewGroup:LoadCallback()
    local not_create = self.data.friend_cur_sel_group.not_create
	self.Board:SetData(self:GetType(), not_create and self.language.SocietyFriendGroupNew or self.language.SocietyFriendEditGroup, Vector2.New(905, 602))
end

SocietyViewFriendEditNewGroupOther = SocietyViewFriendEditNewGroupOther or DeclareMono("SocietyViewFriendEditNewGroupOther", UIWFlushPanel)

function SocietyViewFriendEditNewGroupOther:SocietyViewFriendEditNewGroupOther()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.FriendEditorNewGroup
end

function SocietyViewFriendEditNewGroupOther:onFlush()
	local friend_editor_new_group = self.data:GetFriendEditorNewGroup()
	self.SelectNum.text = string.format(self.language.sel_num_desc, friend_editor_new_group.num)
	if not self.data.friend_cur_sel_group.not_create then self.InputField.text = self.data.friend_cur_sel_group.group_name end
end

function SocietyViewFriendEditNewGroupOther:OnClickSearch()
	self.data:ResetFriendEditorNewGroup()
	ViewMgr:FlushView(SocietyViewFriendEditNewGroup)
end

function SocietyViewFriendEditNewGroupOther:OnClickConfirm()
	local str = self.InputField.text
	local uid_list = self.data:GetFriendEditorNewGroupUidList()
	local not_create = self.data.friend_cur_sel_group.not_create
	local group_type = self.data.friend_cur_sel_group.group_type
	local group_name = self.data.friend_cur_sel_group.group_name
	if StringIsEmpty(str) and not_create then
        PublicPopupCtrl.Instance:Center(Language.Society.FriendGroup.CreateTips)
        return
	end
	local str_num = DataHelper.GetStringWordNum(str)
	if str_num > SocietyConfig.FRIEND_GROUP_NAME_NUM_MAX then
        PublicPopupCtrl.Instance:Center(string.format(Language.Society.FriendGroup.GroupNameLimit, SocietyConfig.FRIEND_GROUP_NAME_NUM_MAX))
        return
	end
	SocietyCtrl.Instance:SendCreateGroup{group_type = group_type, group_name = StringIsEmpty(str) and group_name or str, uid_list = uid_list}
	ViewMgr:CloseView(SocietyViewFriendEditNewGroup)
end

SocietyViewFriendEditNewGroupList = SocietyViewFriendEditNewGroupList or DeclareMono("SocietyViewFriendEditNewGroupList", UIWFlushPanel)

function SocietyViewFriendEditNewGroupList:SocietyViewFriendEditNewGroupList()
	self.data = SocietyCtrl.Instance:Data()
	self.data:ResetFriendEditorNewGroup()
end

function SocietyViewFriendEditNewGroupList:onFlush()
	local role_list = self.data:GetFriendGroupRoleList()
	local str = self.InputField.text
	if not StringIsEmpty(str) then role_list = self.data.SearchOne(role_list, str) end
    self.RoleList:SetData(role_list)
end

SocietyViewFriendEditNewGroupItem = DeclareMono("SocietyViewFriendEditNewGroupItem", UIWidgetBaseItem)

function SocietyViewFriendEditNewGroupItem:SocietyViewFriendEditNewGroupItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendEditNewGroupItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	self.ProfI.SpriteName = prof_sp
	self.Toggle.isOn = false
	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
end

function SocietyViewFriendEditNewGroupItem:OnClickSel()
    local info = self.data.info
	self.society_data:SetFriendEditorNewGroupInfo(info.uid, self.Toggle.isOn)
	ViewMgr:FlushView(SocietyViewFriendEditNewGroup, "SocietyViewFriendEditNewGroupOther")	
end