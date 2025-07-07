FishExplorePanel = FishExplorePanel or DeclareMono("FishExplorePanel", UIWFlushPanel)
function FishExplorePanel:FishExplorePanel()
    self.data = FishData.Instance
    self.data_cares = {
        {data = self.data:Fish(), func = self.FlushFishingState, key = {'fish_area'}},
        {data = SceneData.Instance:Base(), func = self.SceneChange}
    }
    self.standby_dll = false
end
function FishExplorePanel:SceneChange()
    if not FishData.Instance:InFishMap() then
        if self.keep_effect then
            self.EffectTool:Stop(self.keep_effect)
        end
        self.BtnCatch:SetActive(false)
        return
    end
end
function FishExplorePanel:FlushFishingState()
    if ViewMgr:IsOpen(FishView) then
        self.BtnCatch:SetActive(false)
        return
    end
    local fish_area = self.data:Fish().fish_area
    local flag = fish_area ~= nil
    self.BtnCatch:SetActive(flag)
    if fish_area == nil then
        if self.keep_effect ~= nil then
            self.EffectTool:Stop(self.keep_effect)
            self.keep_effect = nil
        end
        return
    end

    if flag then
        UH.SetText(self.btn_text, Language.Live.Fishing)
    end

    if self.keep_effect == nil and flag and not self.is_playing_eff then
        self.keep_effect = self.EffectTool:Play('10170003')
    elseif not flag and self.keep_effect ~= nil then
        self.EffectTool:Stop(self.keep_effect)
        self.keep_effect = nil
    end
end

function FishExplorePanel:OnClickCache()
    if not FishData.Instance:InFishMap() then
        if self.keep_effect then
            self.EffectTool:Stop(self.keep_effect)
        end
        self.BtnCatch:SetActive(false)
        return
    end
    self.EffectTool:Play('10170004')
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if self.keep_effect then
        self.EffectTool:Stop(self.keep_effect)
    end
    self.is_playing_eff = true
    self.BtnCatch:SetActive(false)
end

function FishExplorePanel:EndCacheEffect()
    self.is_playing_eff = false
    local fish_area = self.data:Fish().fish_area
    if fish_area == nil then
        return
    end
    MainPlayer:StopMove()
    self.data:Fish().fish_area = nil
    FishData.Instance:Fish().state = FishData.State.Init
    ViewMgr:OpenView(FishView)
    -- MainPlayer:SetForward(fish_area.transform.localPosition)
    MainPlayer.root_node:SetEulerAngles(fish_area.transform.localEulerAngles)
    self.BtnCatch:SetActive(false)

    if self.standby_dll == false then
        tolua.loadassembly('Cinemachine')
        self.standby_dll = true
    end
--[[     local main_cam = SceneMgr.FindGameObjectWithTag("MainCamera")
    local Brain = typeof('Cinemachine.CinemachineBrain')
    local BlendDefinition = typeof('Cinemachine.CinemachineBlendDefinition')
    local defalut = tolua.getfield(Brain, "m_DefaultBlend")
    local style = tolua.getfield(BlendDefinition,"m_Style")
    local BrainCom = main_cam:GetComponent(Brain)
    local styleValue = defalut:Get(BrainCom)
    style:Set(styleValue, 1)
    defalut:Set(BrainCom, styleValue)
    defalut:Destroy()
    style:Destroy() ]]

    Cinemachine.loadassembly()
    local vir_cam = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FishSceneCamera)
    local eulerAngles = vir_cam.transform.localEulerAngles
    eulerAngles.y = fish_area.transform.localEulerAngles.y - 30
    vir_cam.transform.localEulerAngles = eulerAngles
    local AxisState = typeof('Cinemachine.AxisState')
    local FreeLook = typeof('Cinemachine.CinemachineFreeLook')
    local FreeLookCom = vir_cam:GetComponent(FreeLook)
    --设置角度
    local XAxis = tolua.getfield(FreeLook, 'm_XAxis')
    local Value = tolua.getfield(AxisState, 'Value')
    local XAxisValue = XAxis:Get(FreeLookCom)
    local angle = eulerAngles.y - 135
    Value:Set(XAxisValue, angle)
    XAxis:Set(FreeLookCom, XAxisValue)
    XAxis:Destroy()
    Value:Destroy()
    FreeLookCom.Priority = 6
end
