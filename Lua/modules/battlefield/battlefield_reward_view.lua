BattlefieldRewardView = BattlefieldRewardView or DeclareView("BattlefieldRewardView", "battlefield/battlefield_reward")

function BattlefieldRewardView:BattlefieldRewardView()
    self.data = BattlefieldData.Instance
end

function BattlefieldRewardView:LoadCallback()
    self.List:SetData(Config.illusory_battlefield_auto.ranking_reward)
    self:FlushRoleView()
    self:FlushRankView()
end

function BattlefieldRewardView:FlushRoleView()
    local config = BattlefieldData.Instance:GetRankingRewardList() or {}
    local reward_list = DataHelper.FormatItemList(config.ranking_reward_show or {})
    self.Grid:SetData(reward_list)
    self.NotHasTip:SetActive(#reward_list == 0)
    UH.SetText(self.Rank,self:GetRankName(config.min_rank_pos,config.max_rank_pos))
end

function BattlefieldRewardView:GetRankName(min_rank_pos,max_rank_pos)
    if min_rank_pos == nil then
        return Language.Battlefield.NotHasRankTip
    end
    local RewardRank = Language.Battlefield.RewardRank
    return Format(RewardRank[1],min_rank_pos)
end

function BattlefieldRewardView:OnClickClose()
    ViewMgr:CloseView(BattlefieldRewardView)
end

--刷新段位奖励
function BattlefieldRewardView:FlushRankView()
    self.RankList:SetData(self.data:GetRewardRankList())
    local ranking = self.data:GetRankingNum() or 1
    local cur_config = self.data:GetRankingConfig(ranking)
    local next_config = self.data:GetRankingConfig(ranking + 1)
    if next_config then
        local cur_score = self.data:GetRankScore()
        local curr_score = cur_score - cur_config.score
        local next_scroe = next_config.score - cur_config.score
        next_scroe = next_scroe <= 0 and curr_score or next_scroe
        local need_scroe = next_scroe - curr_score
        UH.SetText(self.RankTip,Format(Language.Battlefield.RewardRankTip,cur_score,cur_config.ranking_list))
        UH.SetText(self.RankDesc,Format(Language.Battlefield.RewardRankTips,need_scroe,BattlefieldData.QuaName(next_config,true)))
    end
    self.FhishTip:SetActive(next_config == nil)
    if self.data:GetTopRace() == 0 then
        UH.SetText(self.RankTip)
        UH.SetText(self.RankDesc)
        self.FhishTip:SetActive(false)
    end
    self.TopRaceTip:SetActive(self.data:GetTopRace() == 0)
end

----------------------------BattlefieldRewardItem----------------------------
BattlefieldRewardItem = BattlefieldRewardItem or DeclareMono("BattlefieldRewardItem", UIWidgetBaseItem)

function BattlefieldRewardItem:SetData(data)
    if data.min_rank_pos > 3 then
        UH.SetText(self.RinkNum,Format(Language.Battlefield.Rankings,data.min_rank_pos,data.max_rank_pos))
        UH.SpriteName(self.ImgRink,data.seq % 2 == 0 and "WengZiDi1" or "WengZiDi2")
    else
        UH.SpriteName(self.ImgRink,"RankBg"..data.min_rank_pos)
        UH.SpriteName(self.ImgNum,"RankNum"..data.min_rank_pos)
    end
    self.RinkNum:SetObjActive(data.min_rank_pos > 3)
    self.Grid:SetData(DataHelper.FormatItemList(data.ranking_reward_show))
end

----------------------------BattlefieldRankRewardItem----------------------------
BattlefieldRankRewardItem = BattlefieldRankRewardItem or DeclareMono("BattlefieldRankRewardItem", UIWidgetBaseItem)

function BattlefieldRankRewardItem:SetData(data)
    local config = BattlefieldData.Instance:GetRankingConfig(data.ranking)
    UH.SpriteName(self.Rank,"Rank" .. config.rank)
    UH.SetText(self.Name,BattlefieldData.QuaName(config,true))
    UH.SetText(self.Score,Format(Language.Battlefield.ScoreTip,config.score))
    UH.SpriteName(self.ImgBG,data.ranking % 2 == 0 and "WengZiDi1" or "WengZiDi2")
    self.Grid:SetData(data.reward_list)
    local ranking = BattlefieldData.Instance:GetRankingNum() or 1
    self.Finish:SetActive(ranking >= data.ranking and BattlefieldData.Instance:GetTopRace() == 1)
    if #data.reward_list > 3 then
		self.Content.pivot = Vector2.New(0,0.5)
    else
        self.Content.pivot = Vector2.New(0.5,0.5)
    end
end