LoginBanDialog = LoginBanDialog or DeclareView("LoginBanDialog", "login/login_ban_dialog", 0,  ViewLayer.Top - 2)

function LoginBanDialog:LoginBanDialog()
    self.data = LoginData.Instance
end 

function LoginBanDialog:LoadCallback()
    local firbid_str = self.data.role_forbid_use_php and self.data.role_forbid_tip or Language.Login.DefaultForbidText
    UH.SetText(self.info, firbid_str)
    UH.SetText(self.RoleInfoText, self.data:GetForbidRoleInfo())
    RichTextHelper.Bind(self.ClickText, BindTool.Bind(self.OnClickCopy, self))
    local copyStr = RichTextHelper.LinkStr(Language.Login.ClickCopy, "_ul",COLORSTR.Green5)
    UH.SetText(self.ClickText, copyStr)
end

function LoginBanDialog:OnClickCopy()
    UnityEngine.GUIUtility.systemCopyBuffer = self.data:GetForbidRoleInfo()
    PublicPopupCtrl.Instance:Center(Language.Login.CopyIdSuccess)
end

function LoginBanDialog:CloseCallback()
end 

function LoginBanDialog:OnClickConfirm()
    ViewMgr:CloseView(LoginBanDialog)
end