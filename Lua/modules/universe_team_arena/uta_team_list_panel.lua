-- ==================== 天下第一比武大会参赛名单 ====================
UTATeamListPanel = UTATeamListPanel or DeclareMono("UTATeamListPanel", UIWFlushPanel)
function UTATeamListPanel:UTATeamListPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.team_all_list_data, func = self.FlushPanel, init_call = false},
    }
end

function UTATeamListPanel:Awake()
    UIWFlushPanel.Awake(self)

    UniverseTeamArenaCtrl.Instance:SendCSUTATeamListReq()

    self:FlushPanel()
    local start_time = self.data:GetKnockoutTime(0).start_time
    local time_info = TimeManager.FormatUnixTime2Date(start_time)
    local time_str = string.format(Language.UTAPanel.ListPanel.Text1, time_info.year, time_info.month, time_info.day, time_info.hour, time_info.minute >= 10 and time_info.minute or time_info.minute .. "0")
    time_str = ColorStr(time_str, COLORSTR.Yellow15)
    UH.SetText(self.Tip, string.format(Language.UTAPanel.ListPanel.Text2,time_str))
    
    self.TimeTip:StampTime(start_time,TimeType.Type_Special_9,Language.UTAPanel.ListPanel.Text3)
end

function UTATeamListPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.TimeTip:CloseCountDownTime()
end

function UTATeamListPanel:FlushPanel()
    local list_data = self.data.team_all_list_data or {}
    self.List:SetData(list_data)
end



UTATeamListPanelItem = UTATeamListPanelItem or DeclareMono("UTATeamListPanelItem", UIWidgetBaseItem)
-- data 来源 decode_uta_team_info_func
function UTATeamListPanelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank = data.rank
    self.BG:SetObjActive(rank <= 3)
    self.RankImg:SetObjActive(rank <= 3)
    if rank <= 3 then
        local bg_img = "PaiMingDi" .. rank
        UH.SpriteName(self.BG, bg_img)
        local rank_img = string.format("PaiMing_%sXiao", rank)
        UH.SpriteName(self.RankImg, rank_img)
        UH.SetText(self.RankText, "")
    else
        UH.SetText(self.RankText, rank)
    end

    local team_name
    if data.team_name and data.team_name == UniverseTeamArenaData.Instance:GetSelfTeamName() then
        team_name = ColorStr(data.team_name, COLORSTR.Yellow1)
    else
        team_name = data.team_name
    end
    UH.SetText(self.Name, team_name)
    local server_info = LoginData.Instance:GetServerInfoById(data.server_id, data.plat_type)
    UH.SetText(self.ServerName, server_info.name)
    UH.SetText(self.Score1, data.hero_team_arena_score)
    --UH.SetText(self.Score2, "")
    local score3 = data.qualification_score < 0 and Language.UTAPanel.Text5 or data.qualification_score
    UH.SetText(self.Score3, score3)

    local condition = data.record_type == 0 and string.format(Language.UTAPanel.ListPanel.Text4, data.record_value) or Language.UTAPanel.MainRank[data.record_value] or ""
    UH.SetText(self.Condition, condition)
end

function UTATeamListPanelItem:OnLookInfo()
    ViewMgr:OpenView(UTABattleArray,{team_id_t = self.data.team_id_t, team_name = self.data.team_name})
end