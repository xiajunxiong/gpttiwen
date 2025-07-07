OnlyFightRank = OnlyFightRank or DeclareView("OnlyFightRank", "only_fight/only_fight_rank")
function OnlyFightRank:OnlyFightRank()
    self.lang = Language.OnlyFight
    self.data = OnlyFightData.Instance
end
function OnlyFightRank:LoadCallback()
    self.Board:SetData(
        self:GetType(),
        string.format(self.lang.RankTitle, self.data:MatchLevel()),
        Vector2.New(1006, 527)
    )
end
function OnlyFightRank:CloseCallback()
    OnlyFightCtrl.ActivityOnlyFightOperatorReq(1)
end

OnlyFightRankPanel = OnlyFightRankPanel or DeclareMono("OnlyFightRankPanel", UIWFlushPanel)
function OnlyFightRankPanel:OnlyFightRankPanel()
    self.data = OnlyFightData.Instance
    self.data_cares = {
        {data = self.data:RankAllInfo(), func = BindTool.Bind(self.FlushRankItem, self)},
        {data = self.data:MyRankInfo(), func = BindTool.Bind(self.FlushMyRankInfo, self)}
    }
end
function OnlyFightRankPanel:FlushRankItem()
    local info = self.data:RankAllInfo()
    self.RankList:SetData(info.list_info or {})
    self.None:SetActive(table.nums(info.list_info) == 0)
end
function OnlyFightRankPanel:FlushMyRankInfo()
    local info = self.data:MyRankInfo()
    if next(info) == nil then
        return
    end
    UH.SetText(
        self.MyRank,
        string.format(Language.OnlyFight.MyRank, info.rank == 0 and Language.MazeRun.NoRank or info.rank)
    )
    UH.SetText(self.MyScore, string.format(Language.OnlyFight.MyScore, info.total_score))
    UH.SetText(
        self.Succ,
        string.format(
            Language.Guild.MeleeRate,
            info.battle_times == 0 and 0 or (info.win_times / info.battle_times * 100),
            info.win_times,
            info.battle_times
        )
    )
end

OnlyFightRankItem = OnlyFightRankItem or DeclareMono("OnlyFightRankItem", UIWidgetBaseItem)
function OnlyFightRankItem:OnlyFightRankItem()
    -- body
end
function OnlyFightRankItem:SetData(data)
    if data.rank < 4 then
        UH.SpriteName(self.RankSp, data.rank)
        UH.SetText(self.Rank, "")
    else
        UH.SpriteName(self.RankSp, "")
        UH.SetText(self.Rank, data.rank)
    end
    UH.Alpha(self.Bg, data.rank % 2 == 0 and 0.3 or 0.7)
    UH.SetText(self.Name, data.role_name)
    UH.SetText(self.Score, data.total_score)
    UH.SetText(
        self.Rate,
        string.format(
            "%d%%(%d/%d)",
            data.battle_times == 0 and 0 or (data.win_times / data.battle_times * 100),
            data.win_times,
            data.battle_times
        )
    )
    local info = OnlyFightData.Instance:GetReward(data.rank)
    if next(info) ~= nil then
        local new_list = {}
        for key, value in pairs(info.reward) do
            table.insert(new_list, Item.Create(value))
        end
        --[[ if info.title > 0 then
            table.insert(new_list, Item.Create({item_id = CommonItemId.ChengHao}))
        end ]]
        if info.coin > 0 then
            table.insert(new_list, Item.Create({item_id = CommonItemId.Coin, num = info.coin}))
        end
        if info.exp > 0 then
            table.insert(new_list, Item.Create({item_id = CommonItemId.Exp, num = 0}))
        end
        self.RewardList:SetData(new_list)
    end
    UIWidgetBaseItem.SetData(self, data)
end
