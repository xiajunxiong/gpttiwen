TombRaiderRewardView = TombRaiderRewardView or DeclareView("TombRaiderRewardView", "tomb_raider/tomb_raider_reward")
function TombRaiderRewardView:TombRaiderRewardView() 
    self.data = TombRaiderData.Instance
    
    self.RewardList:SetData(self.data:GetRewardCfg())
end

function TombRaiderRewardView:OnClickClose()
    ViewMgr:CloseView(TombRaiderRewardView)
end

--============================TombRewardItem=========================================
TombRewardItem = TombRewardItem or DeclareMono("TombRewardItem", UIWidgetBaseItem)
function TombRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    if data.rank_range_max > 0 then
        UH.SetText(self.RankNum,string.format(Language.TombRaider.RankText,data.rank_range_min.."-"..data.rank_range_max))
    else 
        UH.SetText(self.RankNum,string.format(Language.TombRaider.RankTextEnd,data.rank_range_min))
    end 

    if TombRaiderConfig.RankSp[data.rank_range_min] then 
        UH.SpriteName(self.RankShow, TombRaiderConfig.RankSp[data.rank_range_min])
    end 

    local flag = data.rank_range_min ~= data.rank_range_max
    self.RankNum.gameObject:SetActive(flag)
    self.RankShow.gameObject:SetActive(not flag)

    local list = {}
    for k,v in pairs(data.item_list) do 
        local t = Item.Create(v)
        table.insert( list, t )
    end 
    self.Reward:SetData(list)
end