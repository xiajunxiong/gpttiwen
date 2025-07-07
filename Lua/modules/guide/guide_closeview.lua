GuideCloseView = GuideCloseView or BaseClass(GuideBase)
function GuideCloseView:Start(step_cfg, callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    ViewMgr:CloseViewByKey(tonumber(step_cfg.step_param_1))
    self:Continue()
end
function GuideCloseView:Continue()
    if self.callback then
        self.callback()
    end
    self.callback = nil
end
function GuideCloseView:Finish()
    self.callback = nil
    self.step_cfg = nil
end