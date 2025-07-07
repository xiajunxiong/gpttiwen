GuideOpenView = GuideOpenView or BaseClass(GuideBase)
function GuideOpenView:Start(step_cfg, callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    if StringIsEmpty(step_cfg.step_param_1) then
        self:Continue()
        return
    end
    
    local mod_key = tonumber(step_cfg.step_param_1)
    ViewMgr:OpenViewByKey(mod_key, true)
    --[[ local key = Mod.ParseKey(mod_key)
    LogError(ViewMgr:IsOpen(ModToView[key])) ]]
    -- 当前断章位置
    if mod_key == Mod.CutChapter.Main or  mod_key == Mod.CutChapter.BlackWhite then
        CutChapterCtrl.Instance:SetCheckPos(step_cfg.step_param_2)
    end 

    if mod_key == Mod.Reputation.Main then
        local id = 1
        if step_cfg.step_param_2 and step_cfg.step_param_2 ~= "" then
            id = step_cfg.step_param_2
        end
        PrestigeData.Instance:SetVillageID(id)
    end 

    if self.step_cfg.arrow_tip ~= "" then
        self.view:ShowDesc(self.step_cfg)
    end
    --这里可以多加一步是否已经打开的判断 暂时不加
    if mod_key ~= Mod.GuideRestraint.Main then
        self:Continue()
    end
end
function GuideOpenView:Continue()
    if self.callback then
        self.callback()
    end
    self.callback = nil
    self.step_cfg = nil
end
function GuideOpenView:Finish()
    self.callback = nil
    self.step_cfg = nil
end