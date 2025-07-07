GuideOnlyDesc = GuideOnlyDesc or BaseClass(GuideBase)
function GuideOnlyDesc:Start(step_cfg,callback)
    self:View()
    self.step_cfg = step_cfg
    self.callback = callback
    if not StringIsEmpty(self.step_cfg.audio) then
        AudioMgr:PlayEffect(AudioType.UI, self.step_cfg.audio)
    end
    self:CheckDesc()
end
function GuideOnlyDesc:CheckDesc()
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
    if not StringIsEmpty(self.step_cfg.arrow_tip) then
        self.handle_runner = Runner.Instance:RunUntilTrue(function ()
            local con = self.mgr:GetShowConditon(self.step_cfg.step_id,self.step_cfg.index)
            if con == nil then
                self:Show()
                return true
            elseif con() == true then
                self:Show()
                return true
            end
            return false
        end)
    else
        LogError("没填描述")
        self:Finish()
    end
end
function GuideOnlyDesc:Show()
    self.view:ShowOnlyDesc(self.step_cfg, true)
    self.view:AddEvent(BindTool.Bind(self.OnClick, self))
end
function GuideOnlyDesc:OnClick()
    self.view:Hide()
    self.callback()
end
function GuideOnlyDesc:Finish()
    self.callback = nil
    self.step_cfg = nil
    self.mgr:ForceStop()
end