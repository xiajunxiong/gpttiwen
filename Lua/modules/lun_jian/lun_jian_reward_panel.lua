
LunJianRewardPanel = LunJianRewardPanel or DeclareMono("LunJianRewardPanel", UIWFlushPanel)
--MyScore========Text
--MyRank=========Text
--ScoreList==========UIWidgetList[LunJianRewardListItem(LunJianRewardListItem.Type.Score)]
--WinList============UIWidgetList[LunJianRewardListItem(LunJianRewardListItem.Type.Win)]
function LunJianRewardPanel:LunJianRewardPanel()
    UH.SetText(self.MyScore,tostring(LunJianData.Instance:MyScore()))
    local rank = LunJianData.Instance:MyRank()
    UH.SetText(self.MyRank,tostring(rank<= 0 and "--" or rank))


    local score_reward_data = Config.worldarena_auto.season_rank_reward
    self.ScoreList:SetData(score_reward_data)
    local win_reward_data = LunJianData.WinRewardCfg()
    self.WinList:SetData(win_reward_data)
   
end

function LunJianRewardPanel:IsLoaded()
    return self.ScoreList:IsLoaded() and self.WinList:IsLoaded()
end

---------------------------------------
LunJianRewardListItem = LunJianRewardListItem or DeclareMono("LunJianRewardListItem", UIWidgetBaseItem)
LunJianRewardListItem.Type = {
    Score = 1,  --积分奖励
    Win = 2,    --胜场奖励
}
--ItemType====int(LunJianRewardListItem.Type)
--RewardItems====UIWidgetList[UIWItemCell]

--RankImg========UIImageSpriteSet(Score类型专属)
--RankTxt========Text(Score类型专属)

--WinNum=========Text(Win类型专属)


function LunJianRewardListItem:SetData(data)
    local item_data = nil
    local idx = data.sn or 0
    if self.ItemType == LunJianRewardListItem.Type.Score then
        local show_rank_img = data.score_rank <= 3
        self.RankTxt:SetObjActive(not show_rank_img)
        self.RankImg:SetObjActive(show_rank_img)
        if show_rank_img then
            UH.SpriteName(self.RankImg,LunJianMainView.RankSpriteName(data.score_rank))
        else
            UH.SetText(self.RankTxt,tostring(data.score_rank))
        end
        item_data = data.rank_reward
    elseif self.ItemType == LunJianRewardListItem.Type.Win then
        UH.SetText(self.WinNum,string.format(Language.LunJian.RewardWinNum,data.victory_times))
        item_data =  data.win_times
    else
        item_data = {}
    end
    self.gameObject.name = string.format("item (%d)",idx)
    local items = {}
    for _,v in pairs(item_data) do
        table.insert(items,Item.Create(v))
    end 
    self.RewardItems:SetData(items)
end


