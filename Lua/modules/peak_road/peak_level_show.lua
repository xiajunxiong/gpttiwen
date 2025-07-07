PeakLevelShow = PeakLevelShow or DeclareView("PeakLevelShow", "peak_road/peak_level_show")
VIEW_DECLARE_MASK(PeakLevelShow, ViewMask.None)
function PeakLevelShow:OnFlush()
    self.Level.text = PeakRoadData.Instance:PeakData().level
    TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(PeakLevelShow)
    end,3)
end