ItemPreRangeaView = ItemPreRangeaView or DeclareView("ItemPreRangeaView","item_info/item_pre_rangea")

VIEW_DECLARE_MASK(ItemPreRangeaView, ViewMask.None)
function ItemPreRangeaView:ItemPreRangeaView()
	self.ht = EquipData.Instance.equip_detail_data:Care(BindTool.Bind(self.FlushShow, self), "info")
end

function ItemPreRangeaView:LoadCallback(data)
	self.InfoPosition.localPosition = data.pos + Vector2.New(408, 0)
	EquipData.Instance.equip_detail_data.info = data.cell
	self:FlushShow()
end

function ItemPreRangeaView:CloseCallback()
	EquipData.Instance.equip_detail_data:Uncare(self.ht)
	EquipData.Instance.equip_detail_data.is_on_1 = false
	EquipData.Instance.equip_detail_data.is_on_2 = false
end

function ItemPreRangeaView:FlushShow()
	local combine_data = EquipData.Instance.equip_detail_data.info:GetSpiritResonance()
	self.ObjHasReasonance:SetActive(combine_data ~= nil)
	local spirit_list = EquipData.Instance.equip_detail_data.info:GetHasSpiritList()
	-- 计算显示铸灵属性和共鸣属性
	local spirit_attr_config = EquipData.Instance:TotalSpiritListAttr(spirit_list)
	local spirit_attr_data = {}
	if not TableIsEmpty(combine_data) then
		UH.SetText(self.TxtName, combine_data.combine_config.group_name)
		UH.SetText(self.TxtLevel, Format(Language.Equip.EquipStoneLevel2, combine_data.combine_attr.resonance_level + 1))
			local spirit_combine_attr = EquipData.Instance:SpiritCombineAttr(
				EquipData.Instance.equip_detail_data.info:EquipType(),
				combine_data.combine_attr.resonance_group_id,
				combine_data.combine_attr.resonance_level
			)
			EquipData.Instance:TotalSpiritCombineAttr(spirit_combine_attr)
			spirit_attr_data = EquipData.Instance:DealSpiritByCombineAttrToTxt2(
				spirit_attr_config,
				spirit_combine_attr,
				Language.Equip.SingleAttr[8],
				Language.Equip.SingleAttr[9]
			)
			local spirit_combine_attr_data = EquipData.Instance:DealCombineAttrToTxt(spirit_combine_attr, Language.Equip.SingleAttr[7], Language.Equip.CombineAttr[4])
			self.ResonanceAttrList:SetData(spirit_combine_attr_data)
	else
		spirit_attr_data = EquipData.Instance:DealSpiritAttrToTxt(spirit_attr_config, Language.Equip.SingleAttr[8])
	end
	self.SpiritAttrList:SetData(spirit_attr_data)
end

function ItemPreRangeaView:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end