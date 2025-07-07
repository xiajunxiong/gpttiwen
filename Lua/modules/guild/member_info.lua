MemberInfo = MemberInfo or DeclareView("MemberInfo", "guild/member_info")
function MemberInfo:MemberInfo()
    self.lang = Language.Guild
end
function MemberInfo:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab7, Vector2.New(497, 582))
end
function MemberInfo:OnFlush()
    Debuger.LogErrorSD("【家族】查看角色信息接口，暂无")
end
