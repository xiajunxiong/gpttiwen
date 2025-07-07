ShenYuRankRewardView = ShenYuRankRewardView or DeclareView("ShenYuRankRewardView", "shen_yu_rank/shen_yu_rank_reward")
VIEW_DECLARE_LEVEL(ShenYuRankRewardView, ViewLevel.Lt)
VIEW_DECLARE_MASK(ShenYuRankRewardView, ViewMask.BgBlock)

function ShenYuRankRewardView:ShenYuRankRewardView()
end

function ShenYuRankRewardView:LoadCallback(param_t)
    self.Board3:SetData(ShenYuRankRewardView, Language.GodPos.Title[3], Vector2.New(500, 310))
    local item_list ={}
    for i=1,#param_t.rewards,1 do
        table.insert(item_list,Item.Create (param_t.rewards[i]))
    end
    --  BagData.Instance:ItemListBuild(param_t.rewards)
    self.List:SetData(item_list)
end

function ShenYuRankRewardView:OnCloseClick()
    ViewMgr:CloseView(ShenYuRankRewardView)
end

