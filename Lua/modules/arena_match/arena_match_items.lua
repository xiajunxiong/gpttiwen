
ArenaMatchPetIconItem = DeclareMono("ArenaMatchPetIconItem", UIWidgetBaseItem)

function ArenaMatchPetIconItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local pet = data.pet
	local vo = data.vo
    self.Cell:SetData(pet)
    UH.SetText(self.Level, vo.level or pet:Level())
    PetData.SetStrLevel(nil, self.StrLevel, vo.str_level or pet:StrLevel())
end

ArenaMatchPartnerIconItem = DeclareMono("ArenaMatchPartnerIconItem", UIWidgetBaseItem)

function ArenaMatchPartnerIconItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local partner = data.partner
	UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ITEM)
	if self.PartnerQua then
		-- PartnerInfo.SetQuality(self.PartnerQua, partner.partner_quality)
		local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(partner.partner_id)
		PartnerInfo.SetQuality(self.PartnerQua, info_quality.quality)
	end
	if self.StrLevel then
		UH.SpriteName(self.StrLevel, PartnerInfo.StrLevelSp(data.vo.str_level <= 0 and 1 or data.vo.str_level))
	end
end