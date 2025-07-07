PeakRoadStart = PeakRoadStart or DeclareView("PeakRoadStart", "peak_road/peak_road_start")
function PeakRoadStart:PeakRoadStart()
    
end
function PeakRoadStart:OnClickOpen()
    PeakRoadCtrl.CSRoleTopLevelReq(0)
    ViewMgr:CloseView(PeakRoadStart)
    ViewMgr:OpenView(PeakRoadReward)
end