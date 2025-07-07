--收到邀请，接受邀请界面
SwornInvitedView = SwornInvitedView or DeclareView("SwornInvitedView", SwornConfig.ResPath .. "sworn_invited")
function SwornInvitedView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Sworn.SwornInvited, Vector2.New(475, 362))

    self.invited_info = param_t
    self.is_team_invited = param_t.team_info ~= nil
    local role_info = nil
    if self.is_team_invited then
        role_info = SocietyData.Instance:GetFriendInfoByUid(param_t.team_info.leader.uid)
    else
        role_info = SocietyData.Instance:GetFriendInfoByUid(param_t.uid)
    end
    if role_info == nil then
        return
    end
    if self.is_team_invited then
        UH.SetText(self.Desc, Format(Language.Sworn.SwornInvitedStr1, role_info.name))
    else
        UH.SetText(self.Desc, Format(Language.Sworn.SwornInvitedStr2, role_info.name))
    end
    local avatar_data = {
        avatar_type = role_info.avatar_type,
        avatar_id = role_info.avatar_id,
        avatar_quality = role_info.avatar_quality,
        level = role_info.level
    }
    self.SwornAvatar:SetData(avatar_data)
    local prof_sp = RoleData.GetProfSp(role_info.prof)
    UH.SetText(self.Name, role_info.name)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetText(self.IntimacyText, SocietyData.Instance:GetFriendIntimacyVal(role_info.uid))
end

function SwornInvitedView:OnClickClose()
    if self.is_team_invited then
        local info = self.invited_info.team_info
        TeamCtrl.Instance:SendTeamInvationRet(info.leader.uid, false, info.is_cross, info.team_index)
    else
        SwornCtrl.Instance:SendInviteApply(self.invited_info.jie_yi_zu_id, 0)
    end
    ViewMgr:CloseView(SwornInvitedView)
end

function SwornInvitedView:OnClickApply()
    if self.is_team_invited then
        local info = self.invited_info.team_info
        TeamCtrl.Instance:SendTeamInvationRet(info.leader.uid, true, info.is_cross, info.team_index)
    else
        SwornCtrl.Instance:SendInviteApply(self.invited_info.jie_yi_zu_id, 1)
    end
    ViewMgr:CloseView(SwornInvitedView)
end
