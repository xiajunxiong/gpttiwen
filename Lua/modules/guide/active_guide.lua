ActiveGuide = ActiveGuide or DeclareView("ActiveGuide","guide/active_guide")
VIEW_DECLARE_MASK(ActiveGuide, ViewMask.None)
function ActiveGuide:ActiveGuide()
   self.handle_care = ViewMgr:CareTouchDownEvent(function ()
        if ViewMgr.touch_down_data.type ~= ViewManager.TouchDownType.MainTask then
            ViewMgr:CloseView(ActiveGuide)
        end
    end)
end
function ActiveGuide:LoadCallback(param_t)
    GuideData.Instance.open_active_guide.open = true
    self.param_t = param_t
    GuideData.Instance.open_active_guide.step_id = param_t.step_id
end
function ActiveGuide:OnFlush()
    if self.param_t and self.param_t.desc then
        UH.SetText(self.Desc, self.param_t.desc)
    else
        UH.SetText(self.Desc, Language.Guide.ActiveGuide)
    end
end
function ActiveGuide:OnClick()
    -- ViewMgr:CloseView(ActiveGuide)
end
function ActiveGuide:CloseCallback()
    if self.handle_care then
        ViewMgr:UncareTouchDownEvent(self.handle_care)
        self.handle_care = nil
    end
    GuideData.Instance.open_active_guide.open = false
end