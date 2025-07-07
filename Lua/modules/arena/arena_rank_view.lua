-- ArenaRankView = ArenaRankView or DeclareView("ArenaRankView","arena/arena_rank")

-- function ArenaRankView:LoadCallback()
--     self.rank_list:SetData(ArenaData.Instance.rank_list)
--     self.rank.text = ArenaData.Instance.RoleInfo.rank
--     local role_base_data = RoleData.Instance:GetBaseData()
--     self.name.text = role_base_data.name
--     self.level.text = role_base_data.level
--     self.prof.text = Language.Common.Prof[role_base_data.profession]
--     self.Board:SetData(ArenaRankView,Language.ArenaMatch.Rank.Title,nil,true)
-- end

-- function ArenaRankView:OnClose()
--     ViewMgr:CloseView(ArenaRankView)
-- end
-----------------------------------
ArenaRankTab = ArenaRankTab or DeclareMono("ArenaRankTab",UIWFlushPanelGroup)
function ArenaRankTab:ArenaRankTab()
end

function ArenaRankTab:Awake()
    UIWFlushPanelGroup.Awake(self)
    self.rank_list:SetData(ArenaData.Instance.rank_list)
    self.rank.text = ArenaData.Instance.RoleInfo.rank
    local role_base_data = RoleData.Instance:GetBaseData()
    UH.SetText(self.name, role_base_data.name)
    if role_base_data.top_level and role_base_data.top_level > 0 then
        UH.SetText(self.level, string.format(Language.PeakRoad.Level2, role_base_data.top_level))
    else
        UH.SetText(self.level, role_base_data.level)
    end
    UH.SetText(self.prof, Language.Common.Prof[role_base_data.profession])
    UH.SpriteName(self.ProfImg, ProfSettingB[role_base_data.profession])
end
--------------------------------
ArenaRankItem = ArenaRankItem or DeclareMono("ArenaRankItem",UIWidgetBaseItem)
function ArenaRankItem:SetData(data)
    self.no4and5:SetActive(data.rank >= 4) --or data.rank == 5)
    UH.SetText(self.name, data.name)
    UH.SetText(self.prof, Language.Common.Prof[data.profession])
    if data.top_level and data.top_level > 0 then
        UH.SetText(self.level, string.format(Language.PeakRoad.Level2, data.top_level))
    else
        UH.SetText(self.level, data.level)
    end
    UH.SpriteName(self.ProfImg, ProfSettingB[data.profession])
    if data.rank and data.rank > 0 and data.rank <=3 then
        self.no_array[data.rank]:SetActive(true)
    else
        self.rankOj:SetActive(true)
        UH.SetText(self.rank, data.rank)
    end
end