GuideAnother = GuideAnother or DeclareView("GuideAnother", "guide/guide_another")

VIEW_DECLARE_MASK(GuideAnother, ViewMask.BgBlockClose)

function GuideAnother:GuideAnother()
    self.count = 0
end

function GuideAnother:OnFlush()
    self.count = self.count + 1
    if self.ItemList[self.count] then
        self.ItemList[self.count]:PlayTimer()
    end
end

function GuideAnother:OnClickClose()
    if self.count <= self.ItemList:Length() then
        if self.ItemList[self.count] then
            self.ItemList[self.count]:CancelTimer()
            self:OnFlush()
        end
    else
        ViewMgr:CloseView(GuideAnother)
    end
end

function GuideAnother:CloseCallback()
    ViewMgr:OpenView(AnotherView)
end

----------------------------GuideAnotherItem----------------------------
GuideAnotherItem = GuideAnotherItem or DeclareMono("GuideAnotherItem", UIWidgetBaseItem)

function GuideAnotherItem:PlayTimer()
    self.Root:SetActive(true)
    self.show_time = TimeHelper:AddDelayTimer(function()
        if ViewMgr:IsOpen(GuideAnother) then
            ViewMgr:FlushView(GuideAnother)
        end
    end, 2)
end

function GuideAnotherItem:CancelTimer()
    TimeHelper:CancelTimer(self.show_time)
    self.Root:SetActive(true)
end

function GuideAnotherItem:OnDestroy()
	TimeHelper:CancelTimer(self.show_time)
end
