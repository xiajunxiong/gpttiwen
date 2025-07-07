SwornEffectView = SwornEffectView or DeclareView("SwornEffectView", SwornConfig.ResPath .. "sworn_effect")
VIEW_DECLARE_MASK(SwornEffectView, ViewMask.Block)
function SwornEffectView:LoadCallback()
end

function SwornEffectView:OpenCallback()
    self.Effect1:SetActive(true)
    if self.effect_timer == nil then
        self.effect_timer = Invoke(BindTool.Bind1(self.PlayEffect2, self), 12)
    end
end

function SwornEffectView:CloseCallback()
    TimeHelper:CancelTimer(self.effect_timer)
    self.effect_timer = nil
end

function SwornEffectView:PlayEffect2()
    self.Effect1:SetActive(false)
    self.Effect2:SetActive(true)
    self.effect_timer = Invoke(BindTool.Bind1(self.PlayEffect3, self), 3)
end

function SwornEffectView:PlayEffect3()
    ViewMgr:CloseView(SwornEffectView)
end