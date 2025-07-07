GodTokenSurplusRewardView = GodTokenSurplusRewardView or DeclareView("GodTokenSurplusRewardView", "god_token/god_token_surplus_reward")

VIEW_DECLARE_MASK(GodTokenSurplusRewardView,ViewMask.BgBlock)

function GodTokenSurplusRewardView:GodTokenSurplusRewardView()
	self.Title.text = Language.GodToken.SurplusReward
end

function GodTokenSurplusRewardView:LoadCallback()

end

function GodTokenSurplusRewardView:CloseCallback()
	-- body
end

function GodTokenSurplusRewardView:OnCloseClick()
	ViewMgr:CloseView(GodTokenSurplusRewardView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

GodTokenSurplusRewardPanel = GodTokenSurplusRewardPanel or DeclareMono("GodTokenSurplusRewardPanel",UIWFlushPanel)
function GodTokenSurplusRewardPanel:GodTokenSurplusRewardPanel()
	self.data = GodTokenData.Instance
	self.data_cares = {
		{data = self.data.god_token_data, func = self.FlushPanel, init_call = false},
    }
end

function GodTokenSurplusRewardPanel:Awake()
	UIWFlushPanel.Awake(self)
	local ordinary_reward_data = DataHelper.FormatItemList(self.data:GetOrdinarySurplusReward())
	local senior_reward_data = self.data:GetSeniorSurplusReward()
	self.OrdinaryRewardList:SetData(ordinary_reward_data)
	self.SeniorRewardList:SetData(senior_reward_data)
	self:FlushPanel()
end

function GodTokenSurplusRewardPanel:FlushPanel()
	local god_token_data = self.data.god_token_data
	self.GetBtnInter.Interactable = god_token_data.surplus_receive_count > 0
	UH.SetText(self.GetNum,string.format(Language.GodToken.GetNum,god_token_data.surplus_receive_count))
	UH.SetText(self.ProgressText,string.format(Language.GodToken.SurplusExp,god_token_data.surplus_exp,self.data:GetSurplusNeedExp()))
end

function GodTokenSurplusRewardPanel:OnGetClick()
	if self.data.god_token_data.surplus_receive_count > 0 then
		GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_FETCH_SURPKUS)
	end
end

GodTokenSurplusRewardItem = GodTokenSurplusRewardItem or DeclareMono("GodTokenSurplusRewardItem",UIWidgetBaseItem)
function GodTokenSurplusRewardItem:SetData(data)
	self.Cell:SetData(Item.Create(data))
	self.BlockObj:SetActive(not GodTokenData.Instance:IsBuyToken())
end