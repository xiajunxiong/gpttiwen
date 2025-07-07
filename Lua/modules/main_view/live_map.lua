
LiveMap = LiveMap or DeclareMono("LiveMap", UIWFlushPanel)
function LiveMap:LiveMap()
    self.on_scene_data_inited = EventSys:Bind(GlobalEventKeys.SceneDataInited,BindTool.Bind(self.OnSceneDataInited, self))
    self.on_scene_data_disposed = EventSys:Bind(GlobalEventKeys.SceneDataDisposed,BindTool.Bind(self.OnSceneDataDisposed, self))
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.onMainRolePosChange, self))

    self:LoadMapInfo()
end 

function LiveMap:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    EventSys:UnBind(self.role_pos_event)
    EventSys:UnBind(self.on_scene_data_disposed)
    EventSys:UnBind(self.on_scene_data_inited)
end 

-- 启动加载
function LiveMap:OnSceneDataInited()
    self:LoadMapInfo()
end 

function LiveMap:OnSceneDataDisposed()

end 

function LiveMap:LoadMapInfo()
    local scene_id = SceneData.Instance:SceneId()
    --加载地图
    -- if self.Texture:TextureIsNil() then
        self.Texture:SetByPath(MapData.Instance:GetMapMiniPath(scene_id))
        self.CamTexture:SetByPath(MapData.Instance:GetMapMiniPath(scene_id))
    -- end
    --加载地图图标
    local icon_list = MapData.Instance:GetMapMiniIconList(scene_id)
    for k,v in pairs(icon_list) do
        local pos = Vector3.New(v.x,v.z,v.y)
        v.pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex,scene_id)
    end
    self.IconList.StepCount = #icon_list
    self.IconList:SetData(icon_list)

    local camicon_list = MapData.Instance:GetMapMiniIconList(scene_id)
    for k,v in pairs(camicon_list) do
        local pos = Vector3.New(v.x,v.z,v.y)
        v.pos = MapCtrl.Instance:WorldToTransformUV(pos,self.CamMapTex,scene_id)
    end
    self.CamIconList.StepCount = #icon_list
    self.CamIconList:SetData(icon_list)

    self:onMainRolePosChange()
end

-- 比例切换
function LiveMap:OnClickToggleOn()
    self.Total:SetLocalScale(Vector3.one)
end

function LiveMap:OnClickToggleOff()
    self.Total:SetLocalScale(Vector3.New(0.5,0.5,0.5))
end

-- 实时刷新
function LiveMap:onMainRolePosChange()
    if MainPlayer == nil then return end
    local scene_id = MapData.Instance:GetSelectSceneId()
    local pos = MainPlayer:GetPos()
    local main_pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex, scene_id)
    UH.LocalPos(self.MainRolePos,main_pos)
   
    local cam_main_pos = MapCtrl.Instance:WorldToTransformUV(pos,self.CamMapTex, scene_id)
    UH.LocalPos(self.CamMainRolePos,cam_main_pos)

    local cam_pos = Vector3.New(-cam_main_pos.x,-cam_main_pos.y,-cam_main_pos.z)
    UH.LocalPos(self.MainCamPos,cam_pos)
end

function LiveMap:onOtherRolePosChange()

end 