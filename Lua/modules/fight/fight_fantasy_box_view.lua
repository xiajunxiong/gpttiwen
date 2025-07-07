FightFantasyBoxView = FightFantasyBoxView or DeclareView("FightFantasyBoxView", FightConfig.ResPath .. "fight_fantasy_box")

function FightFantasyBoxView:FightFantasyBoxView() 

end 

function FightFantasyBoxView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Fight.FantasyDungen.RewardTitle, Vector2.New(387,328))
    local data_list = FightData.Instance:GetFantasyFBRewardInfo()
    self.RewardList:SetData(data_list)
end 