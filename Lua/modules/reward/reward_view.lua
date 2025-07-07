RewardView = RewardView or DeclareView("RewardView", RewardConfig.ResPath .. "reward",Mod.Reward.Online)

VIEW_DECLARE_LEVEL(RewardView,ViewLevel.L0)

function RewardView:RewardView()
	self.Currency:DefaultSet()
    SuperVipCtrl.Instance:OnRoleNoticeComplete()
end

function RewardView:LoadCallback(param_t)
	RewardData.Instance:OpenPanelData(param_t.mod_key)
end

function RewardView:OnClickClose()
	local god_token_index = GodTokenData.Instance:GetSelectTab()
	if god_token_index and god_token_index ~= GodTokenData.SELECT_TAB.REWARD then
		GodTokenData.Instance:SetSelectTab(GodTokenData.SELECT_TAB.REWARD)
		return
	end
	ViewMgr:CloseView(RewardView)
end

function RewardView:OnClickRecharge()
	ViewMgr:OpenViewByKey(Mod.Store.Recharge)
	ViewMgr:CloseView(RewardView)
end

function RewardView:SelectTabbar(modKey)
	BaseView.SelectTabbar(self, modKey)
	RewardData.Instance:OpenPanelData(modKey)
	RewardData.Instance.open_reward_data:Notify()
end

MainViewRewardName = MainViewRewardName or DeclareMono("MainViewRewardName", UIWFlushPanel)

function MainViewRewardName:OnEnable()
	UIWFlushPanel.OnEnable(self)
	local config = ActivityRandData.GetBtnConfig(Mod.RewardOpen.Main)
	if config ~= nil then
		UH.SetText(self.Name,config.text)
	end
end