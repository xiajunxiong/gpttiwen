LBFTreasureSurplusRewardView = LBFTreasureSurplusRewardView or DeclareView("LBFTreasureSurplusRewardView", "lb_festival/lb_f_treasure_surplus_reward")
VIEW_DECLARE_MASK(LBFTreasureSurplusRewardView,ViewMask.BgBlock)

function LBFTreasureSurplusRewardView:GodTokenSurplusRewardView()
	self.Title.text = Language.GodToken.SurplusReward
end

function LBFTreasureSurplusRewardView:LoadCallback()

end

function LBFTreasureSurplusRewardView:CloseCallback()
	-- body
end

function LBFTreasureSurplusRewardView:OnCloseClick()
	ViewMgr:CloseView(LBFTreasureSurplusRewardView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

LBFTreasureSurplusRewardPanel = LBFTreasureSurplusRewardPanel or DeclareMono("LBFTreasureSurplusRewardPanel",UIWFlushPanel)
function LBFTreasureSurplusRewardPanel:LBFTreasureSurplusRewardPanel()
	self.data = LBFestivalData.Instance
	self.data_cares = {
        {data = self.data.season_pass_info, func = self.FlushPanel},
    }
end

function LBFTreasureSurplusRewardPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushPanel()
end

function LBFTreasureSurplusRewardPanel:FlushPanel()
    local param = self.data:GetSurplusRewardParam()

	self.GetBtnInter.Interactable = param.surplus_receive_count > 0
	self.OrdinaryRewardList:SetData(param.nomal_list)
	self.SeniorRewardList:SetData(param.senior_list)
	UH.SetText(self.GetNum,string.format(Language.GodToken.GetNum,param.surplus_receive_count))
	UH.SetText(self.ProgressText,string.format(Language.GodToken.SurplusExp,param.surplus_exp,param.total_exp))

    self.flag_receive = param.surplus_receive_count
end

function LBFTreasureSurplusRewardPanel:OnGetClick()
	if self.flag_receive then
		LBFestivalCtrl.Instance:ReqTreasureBuySurplus()
	end
end

GodTokenSurplusRewardItem = GodTokenSurplusRewardItem or DeclareMono("GodTokenSurplusRewardItem",UIWidgetBaseItem)
function GodTokenSurplusRewardItem:SetData(data)
	self.Cell:SetData(data)
	self.BlockObj:SetActive(not LBFestivalData.Instance:IsBuySenior())
end