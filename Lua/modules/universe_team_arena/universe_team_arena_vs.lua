-- =========== 天下第一比武大会：对阵界面 ==========
UniverseTeamArenaVs = UniverseTeamArenaVs or DeclareView("UniverseTeamArenaVs", "universe_team_arena/universe_team_arena_vs")
function UniverseTeamArenaVs:UniverseTeamArenaVs()
    
end

function UniverseTeamArenaVs:OnCloseClick()
    ViewMgr:CloseView(UniverseTeamArenaVs)
end

UniverseTeamArenaVsPanel = UniverseTeamArenaVsPanel or DeclareMono("UniverseTeamArenaVsPanel", UIWFlushPanel)
function UniverseTeamArenaVsPanel:UniverseTeamArenaVsPanel()
    self.data = UniverseTeamArenaData.Instance
    self.vs_data = self.data.vs_data
end

function UniverseTeamArenaVsPanel:Awake()
    UIWFlushPanel.Awake(self)
    TimeHelper:CancelTimer(self.time_ht)
    self.time_count = 5
    self.TimeImg:SetObjActive(false)
    self.time_ht = TimeHelper:AddRunTimer(function ()
        if self.time_count <= 3 and self.time_count >= 1 then
            UH.SpriteName(self.TimeImg, "DaoJiShiShuZi" .. self.time_count)
            self.TimeImg:SetObjActive(true)
        end
        self.time_count = self.time_count - 1
        if self.time_count == -1 then
            -- 进入战斗
            ViewMgr:CloseView(UniverseTeamArenaVs)
        end
    end, 1, 6)

    UH.SetText(self.LCampName, self.vs_data.self_team_name)
    UH.SetText(self.RCampName, self.vs_data.side_team_name)
    local round_index = self.vs_data.round_index
    if self.vs_data.match_type == UTACfg.MatchType.Out then
        round_index = UniverseTeamArenaData.Instance:ConvertRoundIndex(round_index)
    end
    local match_name = self.data:FormatMatchName(self.vs_data.match_type, round_index)
    UH.SetText(self.MatchName, match_name)

    for i = 1, self.LItemList:Length() do
        local item = self.LItemList[i]
        local info = self.vs_data.self_member_list[i]
        item:SetObjActive(not TableIsEmpty(info))
        if not TableIsEmpty(info)then
            item:SetData(info)
        end
    end
    for i = 1, self.RItemList:Length() do
        local item = self.RItemList[i]
        local info = self.vs_data.side_member_list[i]
        item:SetObjActive(not TableIsEmpty(info))
        if not TableIsEmpty(info)then
            item:SetData(info)
        end
    end
end

function UniverseTeamArenaVsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end



UniverseTeamArenaVsInfoItem = UniverseTeamArenaVsInfoItem or DeclareMono("UniverseTeamArenaVsInfoItem")
-- data == protocol_050.lua => decode_uta_member_info_func
function UniverseTeamArenaVsInfoItem:SetData(data)
    self.data = data
    UH.SetAvatarp(self.HeadIcon, data.avatar_type, data.head_id, 0)
    UH.SetText(self.Name, data.role_name)
    local prof_name = Language.Common.Prof[data.prof]
    UH.SetText(self.Level, string.format(Language.UTAPanel.Text4, data.level, prof_name))
end 