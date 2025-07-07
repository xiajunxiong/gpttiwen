--邀请列表
SwornInviteView = SwornInviteView or DeclareView("SwornInviteView", SwornConfig.ResPath .. "sworn_invite")
function SwornInviteView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Sworn.SwornInvited, Vector2.New(810, 421))
end

function SwornInviteView:OnClickClose()
    ViewMgr:CloseView(SwornInviteView)
end

function SwornInviteView:CloseCallback()
    SwornData.Instance:ResetInvitedList()
end

SwornInvitePanel = SwornInvitePanel or DeclareMono("SwornInvitePanel", UIWFlushPanel)
function SwornInvitePanel:SwornInvitePanel()
    self.data_cares = {
        {data = TeamData.Instance:GetTeamInfo(), func = self.onFlush},
        {data = SwornData.Instance.sworn_invite_care, func = self.onFlush, init_call = false},
        {data = SwornData.Instance.sworn_info, func = self.onFlush, init_call = false}
    }
    -- source：来源 0=无，1=队伍，2=邀请，3=结义
    self.show_list = {}
    for i = 1, 4 do
        self.show_list[i] = {index = i, source = 0, item_info = nil}
    end
end

function SwornInvitePanel:onFlush()
    if SwornData.Instance:IsHaveSworn() then
        self:FlushAddInfo()
    else
        self:FlushTeamList()
    end
end

--队伍邀请
function SwornInvitePanel:FlushTeamList()
    local team_list = TableCopy(TeamData.Instance:GetMemberRoleList())
    local invite_list = TableCopy(SwornData.Instance.invited_list)
    local function func_team(uid)
        for i, v in ipairs(team_list) do
            if v.info.uid == uid then
                return table.remove(team_list, i)
            end
        end
    end
    func_team(RoleData.Instance:GetRoleId()) --先从队伍去掉自己

    --先处理掉已退队或取消邀请，改source=0
    for i = 1, 4 do
        local info = self.show_list[i]
        if info.source == 1 then
            info.item_info = func_team(info.uid)
            info.source = info.item_info == nil and 0 or 1
        elseif info.source == 2 then
            invite_list[info.uid] = nil
            info.item_info = func_team(info.uid)
            if info.item_info ~= nil then
                info.source = 1
                SwornData.Instance:CancelInvited(info.uid)
            elseif SwornData.Instance:IsInInvitedList(info.uid) == nil then
                info.source = 0
            end
        end
    end
    --加入有具体位置的邀请
    for i, v in pairs(invite_list) do
        if self.show_list[v].source == 0 then
            self.show_list[v].source = 2
            self.show_list[v].uid = i
        end
    end
    --放入之后自己加入的队员, 先往空处填，再往已邀请里填
    if #team_list > 0 then
        for i = 1, 4 do
            local info = self.show_list[i]
            if info.source == 0 then
                info.item_info = table.remove(team_list, 1)
                info.source = 1
                info.uid = info.item_info.info.uid
            end
            if #team_list == 0 then
                break
            end
        end
    end
    if #team_list > 0 then
        for i = 1, 4 do
            local info = self.show_list[i]
            if info.source == 2 then
                SwornData.Instance:CancelInvited(info.uid)
                info.item_info = table.remove(team_list, 1)
                info.source = 1
                info.uid = info.item_info.info.uid
            end
            if #team_list == 0 then
                break
            end
        end
    end
    self.RoleList:SetData(self.show_list)
end

--已结义邀请
function SwornInvitePanel:FlushAddInfo()
    local team_list = TableCopy(SwornData.Instance:GetMemberList())
    local invite_list = TableCopy(SwornData.Instance.invited_list)
    local function func_team(uid)
        for i, v in ipairs(team_list) do
            if v.uid == uid then
                return table.remove(team_list, i)
            end
        end
    end
    func_team(RoleData.Instance:GetRoleId()) --先从结义队伍去掉自己

    --先处理掉取消邀请，改source=0, 这里不踢人
    for i = 1, 4 do
        local info = self.show_list[i]
        if info.source == 3 then
            info.item_info = func_team(info.uid)
            info.source = info.item_info == nil and 0 or 3
        elseif info.source == 2 then
            invite_list[info.uid] = nil
            info.item_info = func_team(info.uid)
            if info.item_info ~= nil then
                info.source = 3
                SwornData.Instance:CancelInvited(info.uid)
            elseif SwornData.Instance:IsInInvitedList(info.uid) == nil then
                info.source = 0
            end
        end
    end
    --加入有具体位置的邀请
    for i, v in pairs(invite_list) do
        if self.show_list[v].source == 0 then
            self.show_list[v].source = 2
            self.show_list[v].uid = i
        end
    end
    --放入之后自己加入的队员, 先往空处填，再往已邀请里填
    if #team_list > 0 then
        for i = 1, 4 do
            local info = self.show_list[i]
            if info.source == 0 then
                info.item_info = table.remove(team_list, 1)
                info.source = 3
                info.uid = info.item_info.uid
            end
            if #team_list == 0 then
                break
            end
        end
    end
    if #team_list > 0 then
        for i = 1, 4 do
            local info = self.show_list[i]
            if info.source == 2 then
                SwornData.Instance:CancelInvited(info.uid)
                info.item_info = table.remove(team_list, 1)
                info.source = 3
                info.uid = info.item_info.info.uid
            end
            if #team_list == 0 then
                break
            end
        end
    end
    self.RoleList:SetData(self.show_list)
end

SwornInviteItem = DeclareMono("SwornInviteItem", UIWidgetBaseItem)
function SwornInviteItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.data.source == 0 then
        self.EmptyObj:SetActive(true)
        self.InfoObj:SetActive(false)
        return
    else
        self.EmptyObj:SetActive(false)
        self.InfoObj:SetActive(true)
    end
    self.Invited:SetActive(self.data.source == 2)
    self.InTeam:SetActive(self.data.source == 1 or self.data.source == 3)
    if self.data.source == 1 then
        local info = self.data.item_info.info
        local prof_sp = RoleData.GetProfSp(info.prof)
        UH.SpriteName(self.ProfI, prof_sp)
        self.ProfI.gameObject:SetActive(nil ~= prof_sp)
        UH.SetText(self.Name, info.name)
        UH.SetText(self.Level, "Lv." .. info.level)
        local appearance = info.appearance
        self.SwornAvatar:SetData({level = info.level, avatar_type = appearance.avatar_type, avatar_id = appearance.avatar_id,avatar_quality = appearance.avatar_quality})
        self.BtnReduce:SetActive(true)
    elseif self.data.source == 2 then
        local info = SocietyData.Instance:GetFriendInfoByUid(self.data.uid)
        local prof_sp = RoleData.GetProfSp(info.prof)
        UH.SpriteName(self.ProfI, prof_sp)
        self.ProfI.gameObject:SetActive(nil ~= prof_sp)
        UH.SetText(self.Name, info.name)
        UH.SetText(self.Level, "Lv." .. info.level)
        self.SwornAvatar:SetData({level = info.level, avatar_type = info.avatar_type, avatar_id = info.avatar_id,avatar_quality = info.avatar_quality})
        self.BtnReduce:SetActive(true)
    elseif self.data.source == 3 then
        local info = self.data.item_info
        local prof_sp = RoleData.GetProfSp(info.prof)
        UH.SpriteName(self.ProfI, prof_sp)
        self.ProfI.gameObject:SetActive(nil ~= prof_sp)
        UH.SetText(self.Name, info.name)
        UH.SetText(self.Level, "Lv." .. info.level)
        self.SwornAvatar:SetData({level = info.level, avatar_type = info.avatar_type, avatar_id = info.avatar_id,avatar_quality = info.avatar_quality})
        self.BtnReduce:SetActive(false)
    end
    UH.SetText(self.IntimacyText, SocietyData.Instance:GetFriendIntimacyVal(self.data.uid))
end

function SwornInviteItem:OnClickInvite()
    SwornData.Instance:SetInvitedIndex(self.data.index)
    ViewMgr:OpenView(SwornInviteTeamView)
end

function SwornInviteItem:OnClickReduce()
    if self.data.source == 0 then
        return
    end
    if not SwornData.Instance:IsHaveSworn() then
        if self.data.source == 2 then
            SwornData.Instance:CancelInvited(self.data.uid)
            SwornData.Instance.sworn_invite_care.flush = not SwornData.Instance.sworn_invite_care.flush
        elseif self.data.source == 1 then
            if TeamData.Instance:MemberInTeam(self.data.item_info.info.name) then
                TeamCtrl.Instance:SendKickOutOfTeamReq(self.data.item_info.info.member_index)
            end
        end
    else
        SwornData.Instance:CancelInvited(self.data.uid)
        SwornData.Instance.sworn_invite_care.flush = not SwornData.Instance.sworn_invite_care.flush
    end
end
