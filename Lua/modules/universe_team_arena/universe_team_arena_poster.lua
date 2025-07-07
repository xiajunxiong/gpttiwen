-- ========== 天下第一比武海报 =============
UniverseTeamArenaPoster = UniverseTeamArenaPoster or DeclareView("UniverseTeamArenaPoster", "universe_team_arena/universe_team_arena_poster")
function UniverseTeamArenaPoster:LoadCallback()
    local flag = UniverseTeamArenaData.Instance:GetPosterPrefFlag()
    self.Toggle.isOn = flag
end

function UniverseTeamArenaPoster:CloseCallback()
    UniverseTeamArenaData.Instance:SetPosterPrefFlag(self.Toggle.isOn)
end

function UniverseTeamArenaPoster:OnCloseClick()
    ViewMgr:CloseView(UniverseTeamArenaPoster)
end

function UniverseTeamArenaPoster:OnBtnClick()
    UniverseTeamArenaCtrl.Instance:OpenUta()
    ViewMgr:CloseView(UniverseTeamArenaPoster)
end

function UniverseTeamArenaPoster:OnToggleClick(value)
    --value = value or false
    --UniverseTeamArenaData.Instance:SetPosterPrefFlag(value)
end