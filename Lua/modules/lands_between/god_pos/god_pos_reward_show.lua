GodPosRewardShowView = GodPosRewardShowView or DeclareView("GodPosRewardShowView", LandsBetweenConfig.PrefabPath.."god_pos/god_pos_reward_show")
VIEW_DECLARE_MASK(GodPosRewardShowView, ViewMask.BgBlock)
function GodPosRewardShowView:GodPosRewardShowView()
    self.Board:SetData(self:GetType(), Language.GodPos.Title[3], Vector2.New(531, 426))
end

function GodPosRewardShowView:LoadCallback(param_t)
    self.List:SetData(DataHelper.FormatItemList(param_t.item_list))
end

function GodPosRewardShowView:CloseCallback()

end

function GodPosRewardShowView:OnCloseClick()
    ViewMgr:CloseView(GodPosRewardShowView)
end