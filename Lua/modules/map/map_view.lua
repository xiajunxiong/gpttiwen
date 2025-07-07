
FILE_REQ("modules/map/map_view_local")
FILE_REQ("modules/map/map_view_world")
FILE_REQ("modules/map/map_view_list")
FILE_REQ("modules/map/map_view_mini")

MapView = MapView or DeclareView("MapView", MapConfig.ResPath .. "map", Mod.MapView.Main)
VIEW_DECLARE_LEVEL(MapView,ViewLevel.L0)

function MapView:MapView()
	self.data = MapCtrl.Instance:Data()
	self.language = Language.Map
	self.mini = MapConfig.Mini

	self.close_map_mini_event = EventSys:Bind(GlobalEventKeys.CloseMapMini, function ()
		self.ToggleCur.isOn = false
	end)

	GuideManager.Instance:RegisterGetGuideUi("WorldMapWanCun",function ()
		if not self.NameLList:SetupCompleted() then
			return
		end
		for i = 1,self.NameLList:GetItemCount() do
			local mono = self.NameLList:GetItem(i)
			if mono ~= nil and mono:GetData().id == 5 then
				return mono
			end
		end
	end)
end

function MapView:LoadCallback(param_t)
	self:ViewRect()
	self:MiniViewRect()
	self:FlushWorldNameIList()
	self:FlushWorldNameLList()
	if param_t and param_t.show_world then
		self.ToggleWorld.isOn = true
	else
		self.ToggleCur.isOn = param_t == nil or param_t.need_click == nil or param_t.need_click == true
	end
end

function MapView:OnClickReturn()
	ViewMgr:CloseView(MapView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function MapView:ViewRect()
	self.data.view_rect:Set{
		width = self.ViewSize.rect.width,
		height = self.ViewSize.rect.height,
	}
	if IS_IPHONEX or EditorSettings:SimularIphoneX() then
		self.LocalRect:SetSizeWithCurrentAnchors(0, self.LocalRect.rect.width + 100)
		self.WorldRect:SetSizeWithCurrentAnchors(0, self.WorldRect.rect.width + 100)
	end
	self.data.view_rect_ready = true
end

function MapView:MiniViewRect()
	self.data.mini_view_rect:Set(Vector2.New(self.ViewSize.rect.width / self.mini.max_size_w * self.mini.width, self.ViewSize.rect.height / self.mini.max_size_h * self.mini.height))
end

function MapView:FlushWorldNameIList()
	local world_namei_list = self.data:GetWorldNameIList()
	self.NameIList:SetData(world_namei_list)
end

function MapView:FlushWorldNameLList()
	local world_namel_list = self.data:GetWorldNameLList()
	self.NameLList:SetData(world_namel_list)
end

function MapView:CloseCallback()
	self.data.mini_view_rect:Set(Vector2.zero)
	self.data.view_rect_ready = false

	EventSys:UnBind(self.close_map_mini_event)
end

function MapView:OnClickItemCur()
	local sceneid = SceneData.Instance:SceneId()
    if SceneData.Instance:GetMode() ~= SceneModeType.None then
        return
    end
    if not MapData.Instance:IsHasMiniMap(sceneid) then
        return
    end
    if not UnityGameObj.FindGameObjectWithTag("MinimapCamera") then
        LogError("当前场景没有MinimapCamera " .. sceneid)
        return
    end
	MapData.Instance:SetSelectSceneId(sceneid)
	if GuideManager.Instance:IsGuide() then
		local guide_id = GuideManager.Instance.guide.id
		return
	end
    ViewMgr:OpenView(MapMiniView, sceneid)
end