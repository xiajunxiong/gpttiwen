
PartnerFetterInfoView = PartnerFetterInfoView or DeclareView("PartnerFetterInfoView", "item_info/partner_fetter_info")

function PartnerFetterInfoView:PartnerFetterInfoView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.FetterInfo
end

function PartnerFetterInfoView:LoadCallback()
	local show_fetter_cell = self.data.show_fetter_cell
	local state = self.data:GetPartnerInfoById(show_fetter_cell.need_partner).vo.is_unlocked
	self.Name.text = show_fetter_cell.name
	self.State.text = self.language.state[state]
	self.Desc.text = string.format(self.language.desc_name, QualityColorStr[show_fetter_cell.color], show_fetter_cell.desc)
end

function PartnerFetterInfoView:OnClickClose()
    ViewMgr:CloseView(PartnerFetterInfoView)
end
