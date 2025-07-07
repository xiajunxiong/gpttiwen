PeakTokenRewardView = PeakTokenRewardView or DeclareView("PeakTokenRewardView", "peak_token/peak_token_reward")

VIEW_DECLARE_MASK(PeakTokenRewardView,ViewMask.BlockClose)

function PeakTokenRewardView:LoadCallback(param_t)
    self.List:SetData(PeakTokenData.Instance:GetRewardList())
end

function PeakTokenRewardView:OnClickComfirm()
    ViewMgr:CloseView(PeakTokenRewardView)
end

function PeakTokenRewardView:CloseCallback()
    ViewMgr:CloseView(PeakTokenView)
end