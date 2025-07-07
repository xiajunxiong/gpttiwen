SocietyDialogView = SocietyDialogView or DeclareView("SocietyDialogView", SocietyConfig.ResPath .. "society_dialog",nil,ViewLayer.Top + 2)
VIEW_DECLARE_CACHE(SocietyDialogView)

function SocietyDialogView:SocietyDialogView()
    self.data = SocietyCtrl.Instance:Data()
end

function SocietyDialogView:LoadCallback()
	self.society_dialog_info = self.data.society_dialog_info
	self:FlushInfo()
end

function SocietyDialogView:FlushInfo()
	local info = self.society_dialog_info.info
    local prof_sp = RoleData.GetProfSp(info.prof)
	self.Name.text = info.name
	self.Level.text = "Lv." .. info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	self.Uid.text = "ID:" .. info.uid
	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
	
	local confirm = self.society_dialog_info.confirm
 	self.ConfirmName.text = confirm.name
end

function SocietyDialogView:OnClickCancel()
    ViewMgr:CloseView(SocietyDialogView)
end

function SocietyDialogView:OnClickConfirm()
	local confirm = self.society_dialog_info.confirm
 	confirm.func()
    ViewMgr:CloseView(SocietyDialogView)
end