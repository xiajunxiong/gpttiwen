GuildMelee = GuildMelee or DeclareView("GuildMelee", "guild/guild_melee")
function GuildMelee:GuildMelee()
    self.lang = Language.Guild
end
function GuildMelee:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.MeleeTitle, Vector2.New(1006, 527))
end
function GuildMelee:CloseCallback()
    GuildCtrl.SendActivityGuildFightOperatorReq(GUILD_FIGHT_OPERATOR_TYPE.CLOSE_RANK)
end
GuildMeleePanel = GuildMeleePanel or DeclareMono("GuildMeleePanel", UIWFlushPanel)
function GuildMeleePanel:GuildMeleePanel()
    self.guild_data = GuildData.Instance
    self.data_cares = {
        {data = self.guild_data:MeleeRankAllInfo(), func = BindTool.Bind(self.FlushRankItem, self)},
        {data = self.guild_data:MeleeMyRankInfo(), func = BindTool.Bind(self.FlushMyRankInfo, self)}
    }
end
function GuildMeleePanel:FlushRankItem()
    local info = self.guild_data:MeleeRankAllInfo()
    self.RankList:SetData(info.list_info or {})
    self.None:SetActive(table.nums(info.list_info) == 0)
end
function GuildMeleePanel:FlushMyRankInfo()
    local info = self.guild_data:MeleeMyRankInfo()
    if next(info) == nil then
        return
    end
    UH.SetText(
        self.MyRank,
        string.format(Language.Guild.MeleeRank, info.rank == 0 and Language.MazeRun.NoRank or info.rank)
    )
    UH.SetText(self.MyScore, string.format(Language.Guild.MeleeScore, info.score))
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

GuidMeleeRankItem = GuidMeleeRankItem or DeclareMono("GuidMeleeRankItem", UIWidgetBaseItem)
function GuidMeleeRankItem:GuidMeleeRankItem()
    -- body
end
function GuidMeleeRankItem:SetData(data)
    if data.rank < 4 then
        UH.SpriteName(self.RankSp, data.rank)
        UH.SetText(self.Rank, "")
    else
        UH.SpriteName(self.RankSp, "")
        UH.SetText(self.Rank, data.rank)
    end
    UH.Alpha(self.Bg, data.rank % 2 == 0 and 0.3 or 0.7)
    UH.SetText(self.Name, data.guild_name)
    UH.SetText(self.Score, data.score)
    UH.SetText(
        self.Rate,
        string.format(
            "%d%%(%d/%d)",
            data.battle_times == 0 and 0 or (data.win_times / data.battle_times * 100),
            data.win_times,
            data.battle_times
        )
    )
    local info = GuildData.Instance:GetMeleeReward(data.rank)
    local new_list = {}
    if info.reward_show then
        for key, value in pairs(info.reward_show) do
            table.insert(new_list, Item.Create(value))
        end
    end
    --[[ if info.title > 0 then
        table.insert(new_list, Item.Create({item_id = CommonItemId.ChengHao}))
    end ]]
    if info.gongxian > 0 then
        table.insert(new_list, Item.Create({item_id = CommonItemId.GongXian, num = info.gongxian}))
    end
    if info.coin > 0 then
        table.insert(new_list, Item.Create({item_id = CommonItemId.Coin, num = info.coin}))
    end
    self.RewardList:SetData(new_list)
    UIWidgetBaseItem.SetData(self, data)
end
