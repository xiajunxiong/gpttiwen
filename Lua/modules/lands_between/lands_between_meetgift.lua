LandsBetweenMeetGiftView = LandsBetweenMeetGiftView or DeclareView("LandsBetweenMeetGiftView", "lands_between/lands_between_meetgift",Mod.LandsBetween.MeetGift)
function LandsBetweenMeetGiftView:LandsBetweenMeetGiftView()

end

function LandsBetweenMeetGiftView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenMeetGiftView)
end

LandsBetweenMeetGiftPanel = LandsBetweenMeetGiftPanel or DeclareMono("LandsBetweenMeetGiftPanel", UIWFlushPanel)
function LandsBetweenMeetGiftPanel:LandsBetweenMeetGiftPanel()
    self.data = LandsBetweenData.Instance
end 

function LandsBetweenMeetGiftPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.close_timer ~= nil then
        TimeHelper:CancelTimer(self.close_timer)
        self.close_timer = nil
    end
end

function LandsBetweenMeetGiftPanel:Awake()
    UIWFlushPanel.Awake(self)
    
    local reward_list = self.data:GetMeetGiftReward()
    self.reward_list:SetData(reward_list)
end 

function LandsBetweenMeetGiftPanel:OnClickCatch()
    self.opned_box:SetActive(true)
    self.BtnCatch:SetActive(false)
    
    LandsBetweenCtrl.Instance:CatchMeetGift()

    self.close_timer = TimeHelper:AddDelayTimer(function() 
        self.close_timer = nil
        ViewMgr:CloseView(LandsBetweenMeetGiftView)
    end,3)
end