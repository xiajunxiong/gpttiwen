
PartnerLifeSuitView = PartnerLifeSuitView or DeclareView("PartnerLifeSuitView", PartnerConfig.ResPath .. "partner_life_suit")

function PartnerLifeSuitView:PartnerLifeSuitView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerLifeSuitView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.LifeSuit, Vector2.New(372, 455))
end

function PartnerLifeSuitView:OnFlush()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	local vo = data.vo
	local pes = vo.pes
	local life = self.language.Life

	local suit_id, desc = 0, ""
	for i = 1, self.PEItems:Length() do
		local pe = pes[i]
		local detail, suit = self.data:GetPEConfigByItemId(pe.item_id)
		self.PEItems[i].text = life.pe_names[i] .. "：" .. (0 == pe.item_id and life.pe_wu or Language.Common.AttrList[pe.attr_type] .. "+" .. pe.attr_value)
		if 1 == i then
			suit_id = nil ~= detail and detail.suit_id or 0
		else
			suit_id = (nil ~= detail and detail.suit_id == suit_id) and suit_id or 0
		end
		if self.PEItems:Length() == i and 0 ~= suit_id then desc = suit.desc end
	end
	self.LockObj:SetActive(0 == suit_id)
	self.Suit.text = desc
end