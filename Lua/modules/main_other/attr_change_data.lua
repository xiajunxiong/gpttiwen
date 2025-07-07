AttrChangeData = AttrChangeData or BaseClass()
AttrChangeData.ChangeFromType = {
    None = "none",
    Role = "role",
    Pet = "pet",
    Partner = "partner",
    ShengQi = "sheng_qi",
}
function AttrChangeData:__init()
    if AttrChangeData.Instance ~= nil then
        Debuger.LogError("[AttrChangeData] attempt to create singleton twice!")
        return
    end
    AttrChangeData.Instance = self
    self.gather_info = {}
    self.change_info = SmartData.New({info = {}})
    self.title_info = SmartData.New({fromType = "none", showName = ""})
end
function AttrChangeData:SetChangeInfo(info, title_info)
    if self.change_info.frame ~= Time.frameCount then
        self.change_info.info = {}
        self.change_info.frame = Time.frameCount
        self.title_info.fromType = title_info.fromType
        self.title_info.showName = title_info.showName
    end

    for key, value in pairs(info) do
        table.insert(self.change_info.info, {key = key, change_val = value.change_val, old_val = value.old_val, from_type = value.from_type})
    end
end

function AttrChangeData:GetChangeInfo()
    return self.change_info.info
end

function AttrChangeData:ClearCache()
    self.change_info:Set({info = {}})
end

function AttrChangeData:GetShowTypeStr()
    if self.title_info.fromType == AttrChangeData.ChangeFromType.Role then
        return RolePromoteData.GetProfName(RoleData.Instance:GetRoleProfId())
    else
        return Language.AttrChange.fromTypeFormat[self.title_info.fromType]
    end
end

function AttrChangeData:GetShowName()
    return self.title_info.showName
end