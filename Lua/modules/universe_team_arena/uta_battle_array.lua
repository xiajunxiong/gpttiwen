-- ================= 天下第一比武大会：选手阵容 ================
UTABattleArray = UTABattleArray or DeclareView("UTABattleArray", "universe_team_arena/uta_battle_array")
function UTABattleArray:UTABattleArray()
    
end

function UTABattleArray:LoadCallback(param_t)
    param_t = param_t or {}
    if TableIsEmpty(param_t) then
        return
    end
    self.Panel:SetTeamInfo(param_t)
    UH.SetText(self.Name, param_t.team_name)
end

function UTABattleArray:OnCloseClick()
    ViewMgr:CloseView(UTABattleArray)
end


UTABattleArrayPanel = UTABattleArrayPanel or DeclareMono("UTABattleArrayPanel", UIWFlushPanel)
function UTABattleArrayPanel:UTABattleArrayPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.battle_array_data, func = self.FlushPanel, init_call = false},
    }
end

function UTABattleArrayPanel:SetTeamInfo(param_t)
    local team_id_t = param_t.team_id_t
    UniverseTeamArenaCtrl.Instance:SendCSUTATeamDetailReq(team_id_t)
end

function UTABattleArrayPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function UTABattleArrayPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:BattleArrayData({})
end

function UTABattleArrayPanel:FlushPanel()
    local list_data = self.data:BattleArrayData().member_list or {}
    self.List:SetData(list_data)
end


UTABattleArrayItem = UTABattleArrayItem or DeclareMono("UTABattleArrayItem", UIWidgetBaseItem)
-- data 来源 decode_uta_member_info_func
function UTABattleArrayItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.Prof,ProfSettingB[data.prof_base_type])
    UH.SetText(self.Level, data.level)
    UH.SetText(self.Name, data.role_name)
    local prof_rank_str = data.rank_prof < 1 and Language.UTAPanel.Text5 or string.format(Language.UTAPanel.Text6, data.rank_prof)
    local level_rank_str = data.rank_level < 1 and Language.UTAPanel.Text5 or string.format(Language.UTAPanel.Text6, data.rank_level)
    local score_rank_str = data.rank_capability < 1 and Language.UTAPanel.Text5 or string.format(Language.UTAPanel.Text6, data.rank_capability)
    UH.SetText(self.RankProf, prof_rank_str)
    UH.SetText(self.RankLevel, level_rank_str)
    UH.SetText(self.RankCapability, score_rank_str)
    UH.SpriteName(self.Prof, UTACfg.ProfIcon[data.prof_base_type])
    UH.SetAvatarp(self.Avatar, data.avatar_type,data.headshot_id,0)
    UH.SetText(self.ProfName, string.format(Language.UTAPanel.Text22, Language.Common.Prof[data.prof_base_type]))
end