
RewardFirstChargeView = RewardFirstChargeView or DeclareMono("RewardFirstChargeView", UIWFlushPanel)

function RewardFirstChargeView:Awake()
    UIWFlushPanel.Awake(self)
    -- 首充已改
    -- self.RewardGrid:SetData(FirstChargeData.Instance:GetRewardFirstChargeList())
end

function RewardFirstChargeView:OnClickCharge()
	ViewMgr:CloseView(RewardView)
	ViewMgr:OpenViewByKey(Mod.Store.Recharge)
end