
ItemHealView = ItemHealView or DeclareView("ItemHealView","item_info/item_heal")
VIEW_DECLARE_MASK(ItemHealView, ViewMask.None)
function ItemHealView:ItemHealView()
	self.data = ItemInfoCtrl.Instance:Data()
	self.info = self.data:ItemInfo()
end

function ItemHealView:LoadCallback(data)
	self.InfoPosition.localPosition = data.pos + Vector2.New(213,-4)
end

ItemHealViewList = ItemHealViewList or DeclareMono("ItemHealViewList", UIWFlushPanel)

function ItemHealViewList:ItemHealViewList()
	self.data = ItemInfoCtrl.Instance:Data()

	self.data_cares = {
		{data = TeamData.Instance:GetTeamInfo(), func = self.onFlush},
		{data = PetData.Instance:GetPetList(), func = self.onFlush, init_call = false},
		{data = TeamData.Instance.flush_menu, func = self.onFlush, init_call = false},
	}
end

function ItemHealViewList:onFlush()
	local heal_list = self.data:GetHealList()
	self.HealList:SetData(heal_list)
end

ItemInfoHealItem = ItemInfoHealItem or DeclareMono("ItemInfoHealItem", UIWidgetBaseItem)

function ItemInfoHealItem:ItemInfoHealItem()
	self.item_info_data = ItemInfoCtrl.Instance:Data()
	self.info = self.item_info_data:ItemInfo()
	self.item = self.info.item_list[1]

	self.HP:SetProgWidth(185)
    self.MP:SetProgWidth(185)
end

function ItemInfoHealItem:SetData(data)
	self.data = self.data or data
	self.RoleIcon:SetObjActive(data.is_role)
	self.PetIcon:SetObjActive(data.is_pet)
	self.PartnerIcon:SetObjActive(data.is_partner)
	if data.is_pet then
		self.PetIcon:SetData(data.pet)
		self.ZhanDai.SpriteName = data.pp_state_sp
	elseif data.is_partner then
		UH.SetIcon(self.PartnerIcon, data.icon_id, ICON_TYPE.ITEM)
		self.ZhanDai.SpriteName = PPStateSp.zhan
	else
		UH.SetAvatar(self.RoleIcon, data.avatar_type,data.avatar_id,data.avatar_quality)
	end
	self.Name.text = data.name
	self.HP:SetProgress(data.cur_hp / data.max_hp, true)
	UH.AnchoredPosition(self.HpEffParRect, Vector2.New(data.cur_hp / data.max_hp * 190))
	self.HP:SetText(string.format(Language.Common.Progress2TxtColorDFormat, data.cur_hp, data.max_hp))
	self.MP:SetProgress(data.cur_mp / data.max_mp, true)
	UH.AnchoredPosition(self.MpEffParRect, Vector2.New(data.cur_mp / data.max_mp * 190))
	self.MP:SetText(string.format(Language.Common.Progress2TxtColorDFormat, data.cur_mp, data.max_mp))
	if self.canPlayEffect then
		if self.data.cur_hp ~= data.cur_hp then
			self.HpEffectTool:Play("10010251")
		end
		if self.data.cur_mp ~= data.cur_mp then
			self.MpEffectTool:Play("10010251")
		end
		self.canPlayEffect = false
	end
end

function ItemInfoHealItem:OnClickItem()
	if ItemInfoData.IsMedicinal(self.item.item_id) then
		if ItemInfoData.GetMedicinalTimes(self.item.item_id) == 0 then
			PublicPopupCtrl.Instance:Center(Language.ItemInfo.CrystalUseTimesTip)
			return
		end
	end
	if 0 == BagData.Instance:GetNumByItemId(self.item.item_id) then
		PublicPopupCtrl.Instance:Center(Language.ItemInfo.HealEmptyTips)
		return
	end
	self.canPlayEffect = true
	BagCtrl.Instance:SendUseItem{
		column = self.item.column_type,
		index = self.item.index,
		param1 = self.data.param1 or 0,
		param2 = self.data.param2 or 0,
		param3 = self.data.param3 or 0,
	}
	if SceneData.Instance:GetMode() == SceneModeType.FightCrystal then
		FightCtrl.Instance:SendCrystalFbReq()
	end
end

function ItemInfoHealItem:OnUpItem()
	TimeHelper:CancelTimer(self.timer_rt)
end

function ItemInfoHealItem:OnDownItem()
	TimeHelper:CancelTimer(self.timer_rt)
	self.timer_rt = TimeHelper:AddRunTimer(function ()
		self:OnClickItem()
	end, 0.5, nil, false)
end

function ItemInfoHealItem:OnDestroy()
	TimeHelper:CancelTimer(self.timer_rt)
end

ItemInfoHealPetIconItem = DeclareMono("ItemInfoHealPetIconItem", UIWidgetBaseItem)

function ItemInfoHealPetIconItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Level, data:Level())
    PetData.SetStrLevel(nil, self.StrLevel, data:StrLevel())
end