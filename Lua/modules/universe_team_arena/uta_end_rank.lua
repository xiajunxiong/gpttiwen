-- =========== 天下第一比武大会：最终排名榜 ==========
UtaEndRank = UtaEndRank or DeclareView("UtaEndRank", "universe_team_arena/uta_end_rank")
VIEW_DECLARE_LEVEL(UtaEndRank, ViewLevel.L0)
function UtaEndRank:OnCloseClick()
    ViewMgr:CloseView(UtaEndRank)
end

function UtaEndRank:OnOpenStoreClick()
    ViewMgr:OpenView(UTAGuessStore)
end

function UtaEndRank:OnOpenRewardClick()
    ViewMgr:OpenView(UniverseTeamArenaReward)
end

function UtaEndRank:OnGuessRecordClick()
    ViewMgr:OpenView(UTAGuessRecord)
end

function UtaEndRank:OnOpenPreviewClick()
    ViewMgr:OpenView(UTAAllMatch)
end

function UtaEndRank:OnOpenChampionClick()
    ViewMgr:OpenView(UtaChampionTeam)
end

function UtaEndRank:OnHelpClick()

end

UtaEndRankPanel = UtaEndRankPanel or DeclareMono("UtaEndRankPanel", UIWFlushPanel)
function UtaEndRankPanel:UtaEndRankPanel()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.end_rank_data, func = self.FlushList, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false},
    }

end

function UtaEndRankPanel:Awake()
    UIWFlushPanel.Awake(self)
    UniverseTeamArenaCtrl.Instance:SendCSUTAFinalRankList()
    local start_time, end_time = self.data:GetStartAndEndTime()
    end_time = end_time + 86400 * 3
    local time_info = TimeManager.FormatUnixTime2Date(end_time)
    local time_str = ColorStr(string.format(Language.UTAPanel.Text9, time_info.year, time_info.month, time_info.day), COLORSTR.Yellow16)
    local tip = string.format(Language.UTAPanel.Text10, time_str)
    UH.SetText(self.Tip, tip)
    self:FlushItem()
end

function UtaEndRankPanel:FlushPanel()
    self:FlushItem()
end

function UtaEndRankPanel:FlushList()
    local list_data = self.data:GetEndRankData()
    self.RankList:SetData(list_data)
end

function UtaEndRankPanel:FlushItem()
    local item_id = self.data:GetGuessItemId()
    local num = Item.GetNum(item_id)
    UH.SetText(self.ItemNum, num)
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end



UtaEndRankItem = UtaEndRankItem or DeclareMono("UtaEndRankItem", UIWidgetBaseItem)
-- data 来源 decode_uta_team_info_func
function UtaEndRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local rank = data.rank
    local info = data
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
    UH.SetText(self.Name, data.team_name)
    local server_info = LoginData.Instance:GetServerInfoById(data.server_id, data.plat_id) or {}
    UH.SetText(self.ServerName, server_info.name or "")
    local rate = info.total_battle_num == 0 and "0" or math.floor(info.total_win_num / info.total_battle_num * 100)
    local rate_str = string.format("%s%% (%s/%s)",rate, info.total_win_num, info.total_battle_num)
    UH.SetText(self.Rate, rate_str)

    local reward_list = UniverseTeamArenaData.Instance:GetRankReward(1, data.rank)
    self.RewardList:SetData(reward_list)
end

function UtaEndRankItem:OnLookInfo()
    ViewMgr:OpenView(UTABattleArray,{team_id_t = self.data.team_id_t, team_name = self.data.team_name})
end