GuideViewEffect = GuideViewEffect or BaseClass(GuideBase)

-- 指引特效
-- 用法：本处的配置仅用于标记检查，实际使用需要每个对应的flushpanel注册方法实现
-- view填入对应flushpanel，
-- 注册方法的时候主要仍然需要返回ui与function
GuideViewEffect.effect_cfg ={
    ["MapYingduEffect"] = {view = "MapViewLocal"},
    ["MapYingduXijiaoEffect"] = {view = "MapViewLocal"},
}

function GuideViewEffect:Start(step_cfg,callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    self:EffectCheck()
end 

function GuideViewEffect:EffectCheck() 
    local ui, effectFunc
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(
        function()
            ui, effectFunc = self.mgr:GetGuideUi(self.step_cfg.step_param_1)

            if ui ~= nil and effectFunc ~= nil then 
                effectFunc()
                self:Continue()
                return true
            end 
            return false
        end
    )
end 

function GuideViewEffect:Continue()
    if self.callback then
        self.callback()
    end
end 

function GuideViewEffect:Finish()
    self.callback = nil
    self.step_cfg = nil
end 