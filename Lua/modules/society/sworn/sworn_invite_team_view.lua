SwornInviteTeamView = SwornInviteTeamView or DeclareView("SwornInviteTeamView", SwornConfig.ResPath .. "sworn_invite_team")
function SwornInviteTeamView:OnClickClose()
    ViewMgr:CloseView(SwornInviteTeamView)
end

function SwornInviteTeamView:LoadCallback()
    local up_list = {}
    table.insert(list, {type = TeamConfig.InviteType.friend, name = Language.Team.InviteTypeNames[TeamConfig.InviteType.friend]})
    self.UpList:SetData(up_list)
    self.UpList:ClickItemData(up_list[1])
end

function SwornInviteTeamView:OnClickInviteType(data)
end

SwornInviteTeamTypeItem = SwornInviteTeamTypeItem or DeclareMono("SwornInviteTeamTypeItem", UIWidgetBaseItem)
function SwornInviteTeamTypeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function SwornInviteTeamTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TypeName, data.name)
end

SwornInviteTeamList = SwornInviteTeamList or DeclareMono("SwornInviteTeamList", UIWFlushPanel)
function SwornInviteTeamList:SwornInviteTeamList()
    self.data = TeamCtrl.Instance:Data()
    self.friend_data = SocietyCtrl.Instance:Data()
end

function SwornInviteTeamList:Awake()
    UIWFlushPanel.Awake(self)
    local role_list = self.friend_data:GetFriendListFriendTeamInvite()
    local show_list = {}
    for i, v in ipairs(role_list) do
        if SwornData.Instance:IsSwornLeader() then
            if not SwornData.Instance:MemberInTeam(v.info.uid) then
                if not SwornData.Instance:IsInInvitedList(v.info.uid) then
                    table.insert(show_list, v)
                end
            end
        else
            if not TeamData.Instance:MemberInTeam(v.info.name) then
                if not SwornData.Instance:IsInInvitedList(v.info.uid) then
                    table.insert(show_list, v)
                end
            end
        end
    end
    self.EmptyObj:SetActive(#show_list == 0)
    self.RoleList:SetData(show_list)
end

SwornInviteTeamItem = SwornInviteTeamItem or DeclareMono("SwornInviteTeamItem", UIWidgetBaseItem)
function SwornInviteTeamItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.info = data.info or data
    UH.SetText(self.Name, self.info.name)
    UH.SetText(self.Level, "Lv." .. self.info.level)
    local job = self.info.prof or self.info.job
    local prof_sp = RoleData.GetProfSp(job or 0)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)

    local appearance = self.info.appearance or {}
    UH.SetAvatar(self.IconSp, self.info.avatar_type or appearance.avatar_type, self.info.avatar_id or appearance.avatar_id,self.info.avatar_quality or appearance.avatar_quality)
    self:CanInvite()
end

function SwornInviteTeamItem:CanInvite()
    local value = SwornData.Instance:IsInInvitedList(self.info.uid)
    self.BtnInviteObj:SetActive(not value)
    self.InvitedObj:SetActive(value)
end

function SwornInviteTeamItem:OnClickInvite()
    if SocietyData.Instance:GetFriendIntimacyVal(self.info.uid) < SwornData.Instance:GetSwornNeedIntimacy() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornIntimacyNo)
        return
    end
    if self.info.level < FunOpen.Instance:GetFunOpenLevel(Mod.Society.Sworn) then
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornLevelNo)
        return
    end
    SwornData.Instance:AddToInvitedList(self.info.uid)
    if SwornData.Instance:IsSwornLeader() then
        SwornCtrl.Instance:SendInviteSwron(self.info.uid)
    else
        TeamCtrl.Instance:OnSendCreateTeam(function()
			TeamCtrl.Instance:SendTeamInvitationReq(self.info.uid, 1)
		end)
    end
    ViewMgr:CloseView(SwornInviteTeamView)
    SwornData.Instance.sworn_invite_care.flush = not SwornData.Instance.sworn_invite_care.flush
end
