

KnightlyRewardView = KnightlyRewardView or DeclareView("KnightlyRewardView", KnightlyConfig.ResPath .. "knightly_reward", Mod.Knight.Reward)
-- VIEW_DECLARE_LEVEL(KnightlyView,ViewLevel.L1)

function KnightlyRewardView:KnightlyRewardView()
	self.data = KnightlyCtrl.Instance:Data()
	self.language = Language.Knightly
end

function KnightlyRewardView:LoadCallback()
	
end

function KnightlyRewardView:OnClickClose()
	ViewMgr:CloseView(KnightlyRewardView)
end

KnightlyRewardViewF = KnightlyRewardViewF or DeclareMono("KnightlyRewardViewF", UIWFlushPanel)

function KnightlyRewardViewF:KnightlyRewardViewF()
	self.data = KnightlyCtrl.Instance:Data()
	self.language = Language.Knightly

	self.data_cares = {
		{data = self.data.knightly_info, func = self.FlushShow},
	}
end

function KnightlyRewardViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

function KnightlyRewardViewF:FlushShow()
	self.HistoryDesc.text = string.format(self.language.HistoryDesc2, self.data.knightly_info.history_chivalrous)
	self.RewardList:SetData(self.data:GetRewardList())
end

KnightlyRewardViewItem = DeclareMono("KnightlyRewardViewItem", UIWidgetBaseItem)

function KnightlyRewardViewItem:KnightlyRewardViewItem()
	self.knightly_data = KnightlyCtrl.Instance:Data()
	self.language = Language.Knightly
end

function KnightlyRewardViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local history_chivalrous = self.knightly_data.knightly_info.history_chivalrous
	self.Num.text = data.chivalrous_value
	self.RewardGrid:SetData(data.rewards)
	self.BtnGetInter.Interactable = history_chivalrous >= data.chivalrous_value
	self.BtnGetInter:SetObjActive(0 == data.get_flag)
	self.GetedObj:SetActive(1 == data.get_flag)
end

function KnightlyRewardViewItem:OnDestroy()
end

function KnightlyRewardViewItem:OnClickGet()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	KnightlyCtrl.Instance:SendRoleHistoryChivalrousRewardFetch(self.data.seq)
end