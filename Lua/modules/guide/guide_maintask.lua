
GuideMainTask = GuideMainTask or BaseClass(GuideBase)
function GuideMainTask:Start(step_cfg,callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    self:CheckButton()
end
function GuideMainTask:CheckButton()
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    local ui, clickFunc
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(function ()
        if self.view == nil then
            return false
        end
        ui, clickFunc = self.mgr:GetGuideUi(self.step_cfg.step_param_1)
        if ui ~= nil then
            self.view:AddEvent(BindTool.Bind(self.OnClick, self))
            self:ClickRange(ui, clickFunc)
            return true
        end
        return false
    end)
end
function GuideMainTask:OnClick()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    self.view:Hide()
    self.main_task = TaskData.Instance:GetMainlineAccepted()
    TaskCtrl.Instance:GoTask(self.main_task)
    if self.callback ~= nil then
        self.callback()
    end
    self.callback = nil
end

function GuideMainTask:ClickRange(ui,func)
    self.view:Show(ui, 2)
    if self.step_cfg.arrow_tip ~= "" then
        self.view:ShowDesc(self.step_cfg)
     end
end
function GuideMainTask:Finish()
    self.callback = nil
    self.step_cfg = nil
end