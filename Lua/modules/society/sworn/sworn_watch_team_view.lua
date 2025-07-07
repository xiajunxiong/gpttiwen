-----查看结义成员
SwornWatchTeamView = SwornWatchTeamView or DeclareView("SwornWatchTeamView", SwornConfig.ResPath .. "sworn_watch_team")
VIEW_DECLARE_MASK(SwornWatchTeamView, ViewMask.BlockClose)
function SwornWatchTeamView:LoadCallback(param_t)
    local member_list = param_t.member_list
    self.MemberList:SetData(member_list)
end

SwornWatchTeam = SwornWatchTeam or DeclareMono("SwornWatchTeam", UIWidgetBaseItem)
function SwornWatchTeam:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local prof_sp = RoleData.GetProfSp(data.prof)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetText(self.Name, data.name)
    -- if data.top_level and data.top_level > 0 then
    --     UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, data.top_level))
    -- else
    UH.SetText(self.Level, "Lv." .. data.level)
    -- end
    self.SwornAvatar:SetData({avatar_type = data.avatar_type, avatar_id = data.avatar_id,avatar_quality = data.avatar_quality, level = data.level})
    self.SwornAvatar:SetLevelTextActive(false)
end
