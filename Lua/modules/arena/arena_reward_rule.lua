------------排行奖励规则窗口--------------------
-- ArenaRewardRuleView = ArenaRewardRuleView or DeclareView("ArenaRewardRuleView","arena/arena_reward_rule")
-- function ArenaRewardRuleView:ArenaRewardRuleView()

-- end

-- function ArenaRewardRuleView:LoadCallback()
--     self.TextList:SetData(ArenaData.RewardRuleInfoList())
-- end

-- function ArenaRewardRuleView:OnClose()
--     ViewMgr:CloseView(ArenaRewardRuleView)
-- end
--------------------排行奖励规则tab-----------------------
ArenaRewardRuleTab = ArenaRewardRuleTab or DeclareMono("ArenaRewardRuleTab", UIWFlushPanelGroup)
function ArenaRewardRuleTab:ArenaRewardRuleTab()
end

function ArenaRewardRuleTab:Awake()
	UIWFlushPanelGroup.Awake(self)
	self.TextList:SetData(ArenaData.RewardRuleInfoList())
end

---------------------------
ArenaRewardRuleTxtItem = ArenaRewardRuleTxtItem or DeclareMono("ArenaRewardRuleTxtItem",UIWidgetBaseItem)
-- RankText = Text
-- RewardText = Text
function ArenaRewardRuleTxtItem:ArenaRewardRuleTxtItem()
    
end

function ArenaRewardRuleTxtItem:SetData(data)
	UH.SpriteName(self.BgImg, data.seq % 2 ~= 0 and "qianqian" or "shenshen")
    UH.SetText(self.RankText, string.format(Language.Arena.RewardRuleRankFormat, data.min_rank, data.max_rank))
    UH.SetText(self.ScoreText,  string.format(Language.Arena.RuleScore, data.rank_price))
    UH.SetText(self.CoinText,  string.format(Language.Arena.RuleCoin, data.bind_coin))
    UIWidgetBaseItem.SetData(self,data)
end