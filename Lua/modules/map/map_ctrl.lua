
FILE_REQ("modules/map/map_config")
FILE_REQ("modules/map/map_data")
FILE_REQ("modules/map/map_data_local")
FILE_REQ("modules/map/map_data_world")
FILE_REQ("modules/map/map_data_list")
FILE_REQ("modules/map/map_data_mini")
FILE_REQ("modules/map/map_explore_data")
VIEW_REQ("modules/map/map_view")
VIEW_REQ("modules/map/map_mini_view")
VIEW_REQ("modules/map/map_guide_view")
VIEW_REQ("modules/map/map_explore")
VIEW_REQ("modules/map/map_explore_reward")
VIEW_REQ("modules/map/map_exprole_reward_view")

MapCtrl = MapCtrl or BaseClass(BaseCtrl)

function MapCtrl:__init()
	if MapCtrl.Instance ~= nil then
		Debuger.LogError("[MapCtrl] attempt to create singleton twice!")
		return
	end	
	MapCtrl.Instance = self
	self.data = MapData.New()
    self.explore_data = MapExploreData.New()
	self:RegisterAllProtocols()
end

function MapCtrl:__delete()
	if MapCtrl.Instance == self then
		MapCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function MapCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSMapUnlockRemoveAnim)
    self:RegisterProtocol(CSMapExplorationReq)
	self:RegisterProtocol(SCMapUnlockAllListInfo, "OnMapUnlockAllListInfo")
	self:RegisterProtocol(SCMapUnlockFirstEnterFbType, "OnMapUnlockFirstEnterFbType")
    self:RegisterProtocol(SCMapExplorationInfo,"OnSCMapExplorationInfo")

    self:RegisterProtocol(SCMapExplorationRewardInfo,"OnMapExplorationRewardInfo")
    self:RegisterProtocol(SCMapExplorationSingleRewardInfo,"OnMapExplorationSingleRewardInfo")
end

function MapCtrl:OnMapUnlockFirstEnterFbType(protocol)
    self.data:SetFirstEnterInfo(protocol.fb_type_enter_flag)
end

function MapCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCMapUnlockAllListInfo)
    self:UnRegisterProtocol(SCMapExplorationInfo)
end

function MapCtrl:OnUnloadGameLogic()
    self.explore_data:CancelExplore()
end

function MapCtrl:OnSCMapExplorationInfo(protocol)
    self.explore_data:SetMapExplorationInfo(protocol)
end

function MapCtrl:OnMapExplorationRewardInfo(protocol)
    LogDG("杰哥哥，这是地图探索的奖励下发哦",protocol)
    self.explore_data:SetMapExplorationRewardInfo(protocol)
end

function MapCtrl:OnMapExplorationSingleRewardInfo(protocol)
    LogDG("杰哥哥，这是地图探索的单条奖励下发哦",protocol)
    self.explore_data:SetMapExplorationSingleRewardInfo(protocol)
end

function MapCtrl:AutoToPos(scene_id)
	ViewMgr:CloseView(MapView)
	SceneLogic.Instance:AutoToPos(scene_id,nil,nil,nil,true)
end

function MapCtrl:OnMapUnlockAllListInfo(protocol)
    self.data:SetMapUnlockAllListInfo(protocol)
end

function MapCtrl:SendMapUnlockRemoveAnim(remove_irregular_id)
    local protocol = GetProtocol(CSMapUnlockRemoveAnim)
	protocol.remove_irregular_id = remove_irregular_id or 0
end

function MapCtrl:SendMapExplorationReq(param_t)
    local protocol = GetProtocol(CSMapExplorationReq)
    protocol.req_type = param_t.req_type
    protocol.param = param_t.param or 0
    protocol.param_1 = param_t.param_1 or 0
    SendProtocol(protocol)
end

-- tex == RectTransform
function MapCtrl:TransformUVPos(tex,sceneid)
    local mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    local minimapCamera = UnityGameObj.FindGameObjectWithTag("MinimapCamera")
    local info = self.data:GetMiniCameraCfg(sceneid)
    if mainCanvas then
        mainCanvasCom = mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    else
        LogError("Canvas没有添加Tag MainCanvas")
        return Vector3.zero
    end
    if minimapCamera then
        minimapCameraCom = minimapCamera:GetComponent(typeof(UnityEngine.Camera))
        if info == nil then
            LogError("请在场景里保存mini相机数据并上传")
            info = {size = minimapCameraCom.orthographicSize}
        end
    else
        LogError("当前场景没有MinimapCamera")
        return Vector3.zero
    end
    
	local is_succ, loc_pos
    if mainCanvasCom and minimapCameraCom then
        is_succ, loc_pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(tex, UnityEngine.Input.mousePosition, mainCanvasCom.worldCamera, nil)
    end
    local loc_uv = {}
    loc_uv.x = loc_pos.x / tex.sizeDelta.x
    loc_uv.y = loc_pos.y / tex.sizeDelta.y
    local world_size_y = info.size * 2
    local world_size_x = world_size_y * minimapCameraCom.aspect -- 屏幕宽高比例
    local view_pos = Vector3.New(world_size_x * loc_uv.x, world_size_y * loc_uv.y, 0)
    return view_pos
end

function MapCtrl:TransformUVToWorld(view_pos)
    local minimapCamera = UnityGameObj.FindGameObjectWithTag("MinimapCamera")
    if minimapCamera == nil then
        LogError("当前场景没有MinimapCamera")
        return Vector3.zero
    end
    local world_pos = minimapCamera.transform:TransformPoint(view_pos)
    local ray_succ, hit_info = UnityEngine.Physics.Raycast(world_pos, minimapCamera.transform.forward, nil)
    return hit_info == nil and Vector3.zero or hit_info.point
end

-- 返回tex下的子节点坐标
-- world_pos == v3
-- tex == RectTransform
function MapCtrl:WorldToTransformUV(world_pos, tex, sceneid)
    local info = self.data:GetMiniCameraCfg(sceneid)
    if info == nil then
        LogError("请到该场景点击保存Mini相机数据")
        return Vector3.zero
    end
    local minimapCamera = UnityGameObj.FindGameObjectWithTag("MinimapCamera")
    if minimapCamera then
        minimapCameraCom = minimapCamera:GetComponent(typeof(UnityEngine.Camera))
    else
        LogError("当前场景没有MinimapCamera")
        return Vector3.zero
    end
    UH.LocalPos(minimapCameraCom.transform, Vector3.New(info.x,info.y,info.z))
    if info.r_x == 0 and info.r_y == 0 and info.r_z == 0 then
    else
        minimapCameraCom.gameObject:SetLocalEulerAngles(Vector3.New(info.r_x,info.r_y,info.r_z))
    end
    minimapCameraCom.orthographicSize = info.size
	local view_pos = minimapCameraCom.transform:InverseTransformPoint(world_pos)
    local world_size_y = minimapCameraCom.orthographicSize * 2
    local world_size_x = world_size_y * minimapCameraCom.aspect -- 屏幕宽高比例
	local res_pos = Vector3.New(view_pos.x/world_size_x*tex.sizeDelta.x,view_pos.y/world_size_y*tex.sizeDelta.y,0)
	return res_pos
end

function MapCtrl:CheckMoveToMainCity(data)
    if self.data:GetSelectSceneId() == SceneLogic.Instance:MainCityID() then
        if data.id == nil then
            local dis = 999999
            local MIN = math.min
            local res = 3
            for index, pos in ipairs(MAINCITYVIEWPOS) do
                local dis2 = GameMath.GetDistanceByPoint(pos, data)
                dis = MIN(dis, dis2)
                res = dis == dis2 and index or res
            end
            SceneLogic.Instance:MoveCache().target_pos = MAINCITYWORLDPOS[res]
        else
            SceneLogic.Instance:MoveCache().target_pos = data
        end
    end
end

function MapCtrl:AutoToPosMapView(data)
    local scene_id = self.data:GetSelectSceneId()
    SceneLogic.Instance:AutoToPos(scene_id,nil,function()
        if data.id == nil then
            local local_pos = self:TransformUVToWorld(data)
            SceneLogic.Instance:AutoToPos(scene_id,{x = local_pos.x, y = local_pos.z})
        else
            if data.to_scene_id then
                SceneLogic.Instance:AutoToPos(data.to_scene_id,data)
            else
                SceneLogic.Instance:AutoToNpc(data.seq)
            end
        end
    end,nil,true)
    ViewMgr:CloseView(MapMiniView)
    ViewMgr:CloseView(MapView)
end