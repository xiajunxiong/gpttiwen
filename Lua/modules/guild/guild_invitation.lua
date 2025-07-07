GuildInvitation = GuildInvitation or DeclareView("GuildInvitation", "guild/guild_invitation")

function GuildInvitation:GuildInvitation()
    self.data = GuildData.Instance
end

function GuildInvitation:LoadCallback()
    local data = self.data:GetInvitationInfo()
    UH.SetText(self.Title,Language.Guild.GuildDear .. RoleData.Instance:GetRoleName())
    UH.SetText(self.Desc,Language.Guild.InvitationDesc)
    UH.SetText(self.GuildName,Language.Guild.GuildName .. data.guild_name)
    UH.SetText(self.Invitationer,Language.Guild.GuildPlayer .. data.user_name)
    self.user_name = data.user_name
end

function GuildInvitation:OnClickClose()
    ViewMgr:CloseView(GuildInvitation)
    GuildCtrl.InvitationReply(0)
    self.data:SetInvitationFlag()
end

function GuildInvitation:OnClickCancel()
    ViewMgr:CloseView(GuildInvitation)
    GuildCtrl.InvitationReply(0)
    self.data:SetInvitationFlag()
end

function GuildInvitation:OnClickAccept()
    GuildCtrl.InvitationReply(1)
    ViewMgr:CloseView(GuildInvitation)
    self.data:SetInvitationFlag(true)
end