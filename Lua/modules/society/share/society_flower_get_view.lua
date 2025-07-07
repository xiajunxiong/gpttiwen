
SocietyFlowerGetView = SocietyFlowerGetView or DeclareView("SocietyFlowerGetView", SocietyConfig.ResPath .. "society_flower_get")

function SocietyFlowerGetView:SocietyFlowerGetView()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.Share
end

function SocietyFlowerGetView:LoadCallback(param_t)
	self.param_t = param_t
	self.ShowDesc.text = string.format(self.language.flower_get_desc, param_t.giver_name .. RichTextHelper.ColorStr("A", COLORSTR.Transparent))
end

function SocietyFlowerGetView:OnClickClose()
	ViewMgr:CloseView(SocietyFlowerGetView)
end

function SocietyFlowerGetView:OnClickBackKiss()
	ViewMgr:CloseView(SocietyFlowerGetView)
	SocietyCtrl.Instance:SendGiftReqBackToKiss(self.param_t.giver_id)
end

function SocietyFlowerGetView:OnClickBackSend()
	ViewMgr:CloseView(SocietyFlowerGetView)
	self.data:SetSocietySendInfo(SocietyConfig.ShareType.GodLandTrace, 0, self.data.item_id)
	ViewMgr:OpenView(SocietySendView)
end
