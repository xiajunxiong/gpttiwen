GraphicQualityManager = GraphicQualityManager or BaseClass()

GraphicQualityManager.default_shadow_dir = Vector3.New(117.4, 327.8, 0)
GraphicQualityManager.default_shadow_strength = 0.23
GraphicQualityManager.scene_shadow_cam_offset = Vector3.New(0, 0, 19.2)
GraphicQualityManager.battle_shadow_cam_offset = Vector3.New(0, 0, 37.2)

QUALITY_SCENE_BLOCK_TAG = {
    MIDDLE = "QualitySceneBlockMiddle",
    HIGH = "QualitySceneBlockHigh"
}

function GraphicQualityManager:__init()
    if GpQualityMgr ~= nil then
        Debuger.LogError("[GraphicQualityManager] attempt to create singleton twice!")
        return
    end
    GpQualityMgr = self
    self.shadow_cam_tran = nil
    self.shadow_cam = nil
    self.shadow_cam_com = nil
    self.shadow_cam_pos_offset = nil
    self.quality = nil
    self.is_lowmem_device = (UnityEngine.SystemInfo.systemMemorySize <= 2500)
end

function GraphicQualityManager:shadowCameraTran()
    if self.shadow_cam_tran == nil then
        local sc_root = UnityGameObj.FindGameObjectWithTag("ShadowCamera")
        self.shadow_cam_tran = sc_root:FindChild("ShadowCam").transform
    end
    return self.shadow_cam_tran
end

function GraphicQualityManager:shadowCamera()
    if self.shadow_cam == nil then
        self.shadow_cam = self:shadowCameraTran():GetComponent(typeof(UnityEngine.Camera))
    end
    return self.shadow_cam
end

function GraphicQualityManager:shadowCameraCom()
    if self.shadow_cam_com == nil then
        self.shadow_cam_com = self:shadowCameraTran():GetComponent(typeof(Crystal.ShadowDepthSampleCamera))
    end
    return self.shadow_cam_com
end

function GraphicQualityManager:shadowCamPosOffSet()
    if self.shadow_cam_pos_offset == nil then
        self.shadow_cam_pos_offset = self:shadowCameraTran():GetComponent(typeof(Game.TransformByTarget))
    end
    return self.shadow_cam_pos_offset
end

function GraphicQualityManager:__delete()
    if GpQualityMgr == self then
        GpQualityMgr = nil
    end
    self.shadow_cam = nil
    self.shadow_cam_com = nil
end

function GraphicQualityManager:SetShadowDirection(euler_angles)
    if euler_angles == Vector3.zero then
        self:shadowCameraTran().eulerAngles = GraphicQualityManager.default_shadow_dir
    else
        self:shadowCameraTran().eulerAngles = euler_angles
    end
end

function GraphicQualityManager:SetShadowEnable(enable)
    self:shadowCameraTran():SetObjActive(enable)
end

function GraphicQualityManager:SetShadowStrength(strength)
    if strength == 0 then
        self:shadowCameraCom().ShadowStrength = GraphicQualityManager.default_shadow_strength
    else
        self:shadowCameraCom().ShadowStrength = strength
    end
end

function GraphicQualityManager:SetShadowBattleMode(enable)
    if enable then
        self:shadowCamPosOffSet().PositionOffset = GraphicQualityManager.battle_shadow_cam_offset
    else
        self:shadowCamPosOffSet().PositionOffset = GraphicQualityManager.scene_shadow_cam_offset
    end
end

function GraphicQualityManager:SetSceneShadowEnable(enable)
    if enable then
        self:shadowCameraCom():AddCullingMask({LAYER.ShadowScene, LAYER.ShadowOnlyScene})
    else
        self:shadowCameraCom():RemoveCullingMask({LAYER.ShadowScene, LAYER.ShadowOnlyScene})
    end
end

function GraphicQualityManager:SetRoleShadowEnable(enable)
    if enable then
        self:shadowCameraCom():AddCullingMask({LAYER.QualityRole})
    else
        self:shadowCameraCom():RemoveCullingMask({LAYER.QualityRole})
    end
end

function GraphicQualityManager:SetShadowMixEnable(enable)
    self:shadowCameraCom():SetShadowMixEnable(enable)
end

function GraphicQualityManager:SetShadowSize(size_w, size_h)
    if VersionTag.HasTag(VersionTag.ShadowSize) then
        self:shadowCameraCom():SetSize(size_w, size_h)
    end
end

function GraphicQualityManager:IsLowMemDevice()
    return self.is_lowmem_device
end

-- function GraphicQualityManager:RefreshScreenResolution(quality)
--     if self.refreshed_screen_resolution and quality == self.refreshed_screen_resolution then
--         return
--     end
--     self.refreshed_screen_resolution = quality

--     local max_w
--     local max_h

--     if quality == GRAPHIC_QUALITY.LOW then
--         max_w = 1280
--         max_h = 720
--         ScreenResolution:SetMaxScreenResolution(max_w, max_h)
--     elseif quality == GRAPHIC_QUALITY.MEDIUM then
--         -- max_w = 1440
--         -- max_h = 800
--         ScreenResolution:RestoreScreenResolution()
--     else
--         -- max_w = 1920
--         -- max_h = 1080
--         ScreenResolution:RestoreScreenResolution()
--     end

-- end