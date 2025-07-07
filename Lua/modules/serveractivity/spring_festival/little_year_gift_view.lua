LittleYearGiftView = LittleYearGiftView or DeclareView("LittleYearGiftView", "serveractivity/spring_festival/little_year_gift_view",Mod.SpringFestival.LittleYearGift)

VIEW_DECLARE_LEVEL(LittleYearGiftView,ViewLevel.L1)

function LittleYearGiftView:LittleYearGiftView()
    self.data = ServerActivityData.Instance
end

function LittleYearGiftView:OpenCallback()
    self.data.little_year_gift_data.val = false
end

function LittleYearGiftView:LoadCallback()
    local thread_id = self.data:GetLittleYearGiftThreadId()
    local config = self.data:GetLittleYearGiftThread(thread_id)
    UH.SetText(self.Desc,"          ".. config.desc)
    UH.SetText(self.Title,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_LITTLE_YEAR_GIFT))
end

function LittleYearGiftView:OnClickClose()
    ViewMgr:CloseView(LittleYearGiftView)
end