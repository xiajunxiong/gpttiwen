GuideNpcTalkShow = GuideNpcTalkShow or BaseClass(GuideBase)
function GuideNpcTalkShow:Start(step_cfg,callback)
    self:View()
    self.view:Hide()
    self.step_cfg = step_cfg
    self.callback = callback
    self.temp_beh = -1
    if not StringIsEmpty(self.step_cfg.step_param_2) then
        self.temp_beh = BehaviorData.Instance.auto_behavior
        BehaviorData.Instance.auto_behavior = self.step_cfg.step_param_2
    end
    NpcDialogCtrl.Instance:NpcDialogView({talk_id = tonumber(self.step_cfg.step_param_1)}, function ()
        if self.temp_beh > -1 then
            BehaviorData.Instance.auto_behavior = self.temp_beh
        end
        if self.callback then
            self.callback()
        end
    end)
end

function GuideNpcTalkShow:Finish()
    self.callback = nil
    self.step_cfg = nil
    if ViewMgr:IsOpen(NpcDialogView) then
        ViewMgr:CloseView(NpcDialogView)
    end
    self.mgr:ForceStop()
end