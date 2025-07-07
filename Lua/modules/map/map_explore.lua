MapExplorePanel = MapExplorePanel or DeclareMono("MapExplorePanel", UIWFlushPanel)
function MapExplorePanel:MapExplorePanel()
    self.data = MapExploreData.Instance
    self.data_cares = {
        {data = self.data.explore_data, func = self.OnShowChanged},
    }
    self.is_playing_eff = false
end

function MapExplorePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

end

function MapExplorePanel:OnShowChanged()
    local flag = self.data.explore_data.reward_show > -1
    local cfg = self.data:GetBoxMapExploreCfg(self.data.explore_data.reward_show,SceneData.Instance:SceneId())
    self.BtnCatch:SetActive(flag and cfg ~= nil)
    if cfg == nil then 
        if self.keep_effect ~= nil then 
            self.EffectTool:Stop(self.keep_effect) 
            self.keep_effect = nil
        end
        return 
    end
     
    if flag then 
        UH.SetText(self.btn_text,cfg.type_btn)
    end 

    if self.keep_effect == nil and flag and not self.is_playing_eff then 
        self.keep_effect = self.EffectTool:Play("10170003")
    elseif not flag and self.keep_effect ~= nil then
        self.EffectTool:Stop(self.keep_effect)
        self.keep_effect = nil
    end 
end

function MapExplorePanel:OnClickCache()

    self.EffectTool:Play("10170004")
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if self.keep_effect then 
        self.EffectTool:Stop(self.keep_effect)
    end 
    self.is_playing_eff = true

end

function MapExplorePanel:EndCacheEffect()
    self.is_playing_eff = false

    MapCtrl.Instance:SendMapExplorationReq({req_type = 1,
        param = self.data.explore_data.reward_show})

    self.data:SetMark( self.data.explore_data.reward_show)
end
