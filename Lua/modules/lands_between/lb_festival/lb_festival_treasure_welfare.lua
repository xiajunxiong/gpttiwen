LBFTreasureWelfareView = LBFTreasureWelfareView or DeclareView("LBFTreasureWelfareView", "lb_festival/lb_f_treasure_welfare")
function LBFTreasureWelfareView:LBFTreasureWelfareView()

end

function LBFTreasureWelfareView:LoadCallback(param_t)

end 

function LBFTreasureWelfareView:OnClickClose()
    ViewMgr:CloseView(LBFTreasureWelfareView)
end

LBFTreasureWelfarePanel = LBFTreasureWelfarePanel or DeclareMono("LBFTreasureWelfarePanel", UIWFlushPanel)
function LBFTreasureWelfarePanel:LBFTreasureWelfarePanel()
    self.data = LBFestivalData.Instance
    self.data_cares = {
		{data = self.data.fuli_info, func = self.FlushPanel},
    }
end 


function LBFTreasureWelfarePanel:OnClickReward(data)
    if not data.btn_enable then 
        PublicPopupCtrl.Instance:Center(Language.LBFestival.Treasure.FuliTimeLack)
        return 
    end 

    if not self.data:IsBuySenior() then 
        PublicPopupCtrl.Instance:Center(Language.LBFestival.Treasure.SeniorLack)
        return 
    end 

    -- LBFestivalCtrl.Instance:ReqTreasureBuySurplus()
    LBFestivalCtrl.Instance:ReqTreasureFetchPass(data.index)
end
function LBFTreasureWelfarePanel:FlushPanel()
    local param = self.data:GetWelfareParam()

    self.List:SetData(param.list)
    UH.SetText(self.AllTimes,param.buy_timers)
    self.Timer:StampTime(param.stamp_time,TimeType.Type_Time_3,Language.LBFestival.Treasure.TimeDoneYellow)
end

LBFTreasureWelfareCell = LBFTreasureWelfareCell or DeclareMono("LBFTreasureWelfareCell",UIWidgetBaseItem)
function LBFTreasureWelfareCell:SetData(data)
	UIWidgetBaseItem.SetData(self,data)

    UH.SetText(self.Title,data.title)
    self.Rewards:SetData(data.reward_list)
    self.Done:SetActive(data.is_fetch)
    self.BtnReward:SetActive(not data.is_fetch)
    self.BtnInter.Interactable = data.btn_enable
end 

LBFTreasureWelfareItemCall = LBFTreasureWelfareItemCall or DeclareMono("LBFTreasureWelfareItemCall",UIWidgetBaseItem)
function LBFTreasureWelfareItemCall:SetData(data)
	UIWidgetBaseItem.SetData(self,data)

    self.Call:SetData(data)
end