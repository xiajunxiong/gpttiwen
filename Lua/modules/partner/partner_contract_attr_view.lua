
PartnerContractAttrView = PartnerContractAttrView or DeclareView("PartnerContractAttrView", PartnerConfig.ResPath .. "partner_contract_attr")
VIEW_DECLARE_MASK(PartnerContractAttrView, ViewMask.BgBlockCloseNoAnim)

function PartnerContractAttrView:PartnerContractAttrView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Up
end

function PartnerContractAttrView:LoadCallback()
end

function PartnerContractAttrView:OnFlush()
	self.Tips.text = Config.language_cfg_auto.textdesc[160].desc
	if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	local pact_items = {}
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        local pact_item = data.pact_items[i]
		if pact_item.item_id > 0 then
			table.insert(pact_items, pact_item)
		end
    end
	self.ShowList:SetData(pact_items)
	self.EmptyObj:SetActive(0 == #pact_items)
end

PartnerContractAttrViewItem = PartnerContractAttrViewItem or DeclareMono("PartnerContractAttrViewItem", UIWidgetBaseItem)

function PartnerContractAttrViewItem:PartnerContractAttrViewItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Contract
end

function PartnerContractAttrViewItem:SetData(data)
	local item = data.item
	local attr_list = self.partner_data:GetPartnerPactAttrShowOne(data)
	self.Name.text = item:Name()
	for i = 1, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX do
		local attr = attr_list[i - 1]
		self.Attrs[i].text = string.format(self.language.AttrShow2, Language.Common.AttrList[attr.attr_type], DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
	end
end