QualityCtrl = QualityCtrl or BaseClass(BaseCtrl)

QUALITY_LEVEL = {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
}

QUALITY_SCENE_BLOCK_TAG = {
    MIDDLE = "QualitySceneBlockMiddle",
    HIGH = "QualitySceneBlockHigh"
}

function QualityCtrl:__init()
    if QuaCtrl ~= nil then
        Debuger.LogError("[QualityCtrl] attempt to create singleton twice!")
        return
    end
    QuaCtrl = self
    self.quality = nil
end

function QualityCtrl:__delete()
    if QuaCtrl == self then
        QuaCtrl = nil
    end
end

function QualityCtrl.SetUnityQualityLevel(quality)
    if quality == QUALITY_LEVEL.LOW then        --红魔设备闪退，因为QualitySettings的Low配置里TextureQuality为HalfRes，暂时舍弃设置QS为Low，改用Medium+细节参数的方式代替
        UnityQualitySettings.SetQualityLevel(QUALITY_LEVEL.MEDIUM)
        UnityQualitySettings.lodBias = 0.4
        UnityQualitySettings.particleRaycastBudget = 16
        UnityQualitySettings.blendWeights = 2
        UnityQualitySettings.vSyncCount = 0
        UnityQualitySettings.anisotropicFiltering = 0
    else
        UnityQualitySettings.SetQualityLevel(quality)
        if quality == QUALITY_LEVEL.MEDIUM then
            UnityQualitySettings.lodBias = 0.7
            UnityQualitySettings.particleRaycastBudget = 64
            UnityQualitySettings.blendWeights = 4
            UnityQualitySettings.vSyncCount = 2
            UnityQualitySettings.anisotropicFiltering = 1          
        end
    end
end


function QualityCtrl:SetQuality(quality)
    if not self.quality or self.quality ~= quality then
        self.quality = quality
        -- UnityQualitySettings.masterTextureLimit = 0;--红魔闪退，因为QualitySettings里TextureQuality为HalfRes，暂改成全是FullRes
        -- UnityQualitySettings.SetQualityLevel(quality)
        QualityCtrl.SetUnityQualityLevel(quality)
        local cur_scene = SceneCtrl.Instance:CurScene()
        -- if cur_scene and (cur_scene:GetClassType() == GameScene or cur_scene:GetClassType() == BattleScene) then
        --     cur_scene:SetQualitySceneBlock(quality)
        -- end
        SceneMgr:SetQualityLevel(quality)
        if quality > 2 then
            quality = 2
        end
        Crystal.Quality.CurrentQuality = Crystal.Quality.QualityNames[quality]
        Game.EffectLevelFilter.RefreshAll()
        if quality == QUALITY_LEVEL.LOW then
            local ph, pw = ScreenResolution:GetPhysicalHW()
            if ph > 1280 and pw > 720 then
                ScreenResolution:SetMaxScreenResolution(1280, 720)
            end
            GpQualityMgr:SetShadowEnable(false)
            self.role_shadow_enabled = true
        elseif quality == QUALITY_LEVEL.MEDIUM then
            ScreenResolution:RestoreScreenResolution()
            GpQualityMgr:SetShadowEnable(true)
            GpQualityMgr:SetRoleShadowEnable(true)
            GpQualityMgr:SetSceneShadowEnable(false)
            GpQualityMgr:SetShadowMixEnable(false)
            GpQualityMgr:SetShadowSize(1024, 1024)
            self.role_shadow_enabled = false
        else
            ScreenResolution:RestoreScreenResolution()
            GpQualityMgr:SetShadowEnable(true)
            GpQualityMgr:SetRoleShadowEnable(true)
            GpQualityMgr:SetSceneShadowEnable(true)
            GpQualityMgr:SetShadowMixEnable(true)
            GpQualityMgr:SetShadowSize(1536, 1536)
            self.role_shadow_enabled = false
        end
        SceneCtrl.Instance:Objs():EnableAllObjShadow(self.role_shadow_enabled)
        BattleCtrl.Instance:EnableAllObjShadow(self.role_shadow_enabled)
    end
end

function QualityCtrl:GetQuality()
    return self.quality
end

function QualityCtrl:RoleShadowEnabled()
    return self.role_shadow_enabled
end