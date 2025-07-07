PopupLifeMsg = PopupLifeMsg or DeclareView("PopupLifeMsg", "fish/popup_life_msg")
VIEW_DECLARE_MASK(PopupLifeMsg, ViewMask.None)

function PopupLifeMsg:LoadCallback(param_t)
    UH.SetText(self.Title, param_t.title or "")
    UH.SetText(self.Desc, param_t.desc or "")
    self.handle_delay = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickClose, self), 2)
end
function PopupLifeMsg:OnClickClose()
    ViewMgr:CloseView(PopupLifeMsg)
end
function PopupLifeMsg:CloseCallback()
    TimeHelper:CancelTimer(self.handle_delay)
end