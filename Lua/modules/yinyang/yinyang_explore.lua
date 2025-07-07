YinYangExplorePanel = YinYangExplorePanel or DeclareMono('YinYangExplorePanel', UIWFlushPanel)
function YinYangExplorePanel:YinYangExplorePanel()
    self.data = YinYangData.Instance
    self.data_cares = {
        {data = self.data.explore_data, func = self.OnShowChanged, keys = {"vo"}}
    }
    self.is_playing_eff = false
end

function YinYangExplorePanel:OnShowChanged()
    local vo = self.data.explore_data.vo
    -- LogDG(vo)
    local flag = self.data.explore_data.vo ~= nil
    if self.data.explore_data.vo then
        flag = self.data.explore_data.vo.show_type == 0
    end
    self.BtnCatch:SetActive(flag)
    if vo == nil then
        if self.keep_effect ~= nil then
            self.EffectTool:Stop(self.keep_effect)
            self.keep_effect = nil
        end
        return
    end

    if flag then
        UH.SetText(self.btn_text, vo.type_btn)
    end

    if self.keep_effect == nil and flag and not self.is_playing_eff then
        self.keep_effect = self.EffectTool:Play('10170003')
    elseif not flag and self.keep_effect ~= nil then
        self.EffectTool:Stop(self.keep_effect)
        self.keep_effect = nil
    end
end

function YinYangExplorePanel:OnClickCache()
    self.EffectTool:Play('10170004')
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if self.keep_effect then
        self.EffectTool:Stop(self.keep_effect)
    end
    self.is_playing_eff = true
end

function YinYangExplorePanel:EndCacheEffect()
    self.is_playing_eff = false
    local vo = self.data.explore_data.vo
    if vo == nil then
        return
    end
    if vo.is_talk == 1 then
        --local npc_cfg = NpcHelper.NpcCo(vo.id)
		--BehaviorCtrl.ExeByCfgVo(npc_cfg, vo)
        BehaviorCtrl.ExeByVo(vo)
        return
    end
    -- MapCtrl.Instance:SendMapExplorationReq({req_type = 1,
    --     param = self.data.explore_data.reward_show})
    YinYangCtrl.CSYinYangEyeOpReq(1, vo.seq)
    -- self.data:SetMark(self.data.explore_data.reward_show)
end
