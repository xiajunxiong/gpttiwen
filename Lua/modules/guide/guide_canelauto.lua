GuideCancelAuto = GuideCancelAuto or BaseClass(GuideBase)
function GuideCancelAuto:Start(step_cfg, callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    if BattleData.Instance:BaseData().auto == true then
        --取消自动战斗
        BattleCtrl.Instance:SendAutoBattle(false)
    end
    self:Continue()
end
function GuideCancelAuto:Continue()
    if self.callback then
        self.callback()
    end
    self.callback = nil
end
function GuideCancelAuto:Finish()
    self.callback = nil
    self.step_cfg = nil
end

GuideForceAuto = GuideForceAuto or BaseClass(GuideBase)
function GuideForceAuto:Start(step_cfg, callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    if BattleData.Instance:BaseData().auto == false then
        -- LogDG("没有自动，发送自动")
        BattleCtrl.Instance:SendAutoBattle(true)
    end
    self:Continue()
end
function GuideForceAuto:Continue()
    if self.callback then
        self.callback()
    end
    self.callback = nil
end
function GuideForceAuto:Finish()
    self.callback = nil
    self.step_cfg = nil
end