
FightRewardView = FightRewardView or DeclareView("FightRewardView", "fight/fight_reward")
VIEW_DECLARE_MASK(FightRewardView, ViewMask.BlockClose)

function FightRewardView:FightRewardView()
    self.data = FightCtrl.Instance:Data()
end

function FightRewardView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Fight.RewardDropPreview, Vector2.New(450, 370))
    local battle_mode = BattleData.Instance:BattleMode()
    local rewards = self.data:GetFightRewardData(battle_mode)
    self.Rewards:SetData(rewards)
end