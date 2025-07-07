GuideFuben = GuideFuben or DeclareView("GuideFuben", "guide/guide_fuben")
VIEW_DECLARE_MASK(GuideFuben,ViewMask.BgBlockClose)
function GuideFuben:GuideFuben()
    self.mapdata = MapData.Instance
end
function GuideFuben:CloseCallback()
    self.mapdata:SetSingleFirstEnterInfo(SCENE_TYPE.CRYSTAL_FB,1)
    -- GuideManager.Instance:Start(24)
end