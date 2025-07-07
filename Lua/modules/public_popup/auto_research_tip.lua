AutoResearchTip = AutoResearchTip or DeclareView("AutoResearchTip", "public_popup/auto_research_tip")
function AutoResearchTip:LoadCallback()
    -- body
end
function AutoResearchTip:OnFlush()
    self.Content.text = ErrorText[118]
    self.Obj1:SetActive(true)
    self.Obj2:SetActive(false)
    --启动倒计时60秒
    self.TimeM:SetCallBack(BindTool.Bind(self.GoTo, self))
    self.TimeM:TotalTime(60, TimeType.Type_Special_2,Language.Finish.Confirm)
end
function AutoResearchTip:Close()
    ViewMgr:CloseView(AutoResearchTip)
end
function AutoResearchTip:Cancel()
    --不进行自动并设置是否继续检查
    ViewMgr:CloseView(AutoResearchTip)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_AUTO_RESEARCH_TASK, self.Gou.activeSelf == true and 0 or 1)
    RoleSettingCtrl.Instance:SaveRoleSettingInfo()
end
function AutoResearchTip:GoTo()
    --发协议变状态，改设置
    ResearchTaskCtrl.SendResearchTaskAuto()
    self.Content.text = ErrorText[119]
    self.Obj1:SetActive(false)
    self.Obj2:SetActive(true)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_AUTO_RESEARCH_TASK, self.Gou.activeSelf == true and 0 or 1)
    RoleSettingCtrl.Instance:SaveRoleSettingInfo()
end
function AutoResearchTip:Change()
    self.Gou:SetActive(not self.Gou.activeSelf)
end
