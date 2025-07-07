
MapGuideView = MapGuideView or DeclareView("MapGuideView", MapConfig.ResPath .. "map_guide", Mod.MapGuideView.Main)
VIEW_DECLARE_MASK(MapGuideView, ViewMask.BlockClose)

function MapGuideView:MapGuideView()
	self.data = MapCtrl.Instance:Data()
end

function MapGuideView:LoadCallback()
	self.need_guide = false
end

function MapGuideView:CloseCallback()
	self.data.map_guide.state = self.need_guide and MapConfig.GuideState.guide or MapConfig.GuideState.show
end

function MapGuideView:OnClickMap()
	-- self.need_guide = true
	ViewMgr:CloseView(MapGuideView)
end