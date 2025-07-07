-- 显示逻辑配置
local type_parts = {
	defalut = {"Desc","GetWay"},
	[ItemType.Task] = {"Desc","GetWay"},
	[ItemType.Weapon] = {"EquipBase","EquipSmelt","EquipSpecial","EquipStone","CastSoul","EquipSet","Neveal","NevealSkill","Desc","CreateName"},
	[ItemType.Armor] = {"EquipBase","EquipSmelt","EquipSpecial","EquipStone","CastSoul","EquipSet","Neveal","NevealSkill","Desc","CreateName"},
	[ItemType.Jewelry] = {"EquipBase","EquipSmelt","EquipStone","CastSoul","EquipSet","Desc","CreateName"},--{"EquipBase","JewelrySuit","JewelyLegend","Desc","CreateName"},
	[ItemType.ExpCrystal] = {"Desc","CreateName"},
	[ItemType.Medal] = {"EquipBase","EquipSpecial","Desc"},
	[ItemType.PartnerEquip] = {"EquipBase","JewelrySuit","Desc","GetWay"},
	[ItemType.Fashion] = {"Desc","ModelShow","IllusionBase"},
	[ItemType.Skin] = {"Desc","ModelShow","IllusionBase"},
	[ItemType.PetEquip] = {"Desc","PetEquip"},
	[ItemType.Appearance] = {"Desc","ModelShow","IllusionBase"},
	[ItemType.PetWeapon] = {"PetWeapon","BicycleEquip","Desc"},
	[ItemType.EquipSpirit] = {"SpiritStone","Desc","GetWay"},
	[ItemType.PetStamp] = {"PetStampInfo","Desc","GetWay"},
	[ItemType.Accrssory] = {"AccrssoryItemBaseAttr","AccrssoryItemStrengthAttr","AccrssoryItemExtraAttr","AccrssoryItemSuit","Desc","GetWay"},
	[ItemType.Marbas] = {"EquipBase","MarbasBathe","MarbasBlood","MarbasGem","MarbasSuit"},
	[ItemType.MarbasGem] = {"MarbasGemInfo","Desc"},
	[ItemType.FuYu] = {"FuYuAttrInfo", "FuYuEffectInfo", "Desc"},
	[ItemType.MarbasGem] = {"MarbasGemInfo","Desc"},
	[ItemType.LingBao] = {"LingBaoItemBaseAttr","Desc","GetWay"},
}

local show_type_parts = {
	[ShowType.BicycleEquip] = {"Desc", "BicycleEquip"},
	[ShowType.LiveCard] = {"LivePropSub", "Desc"},
	[ShowType.LiveFood] = {"LivePropSub", "Desc"},
	[ShowType.PetBagde] = {"PetBagde", "Desc"},
	[ShowType.ShengQi] = {"ShengQi","Desc"},
	[ShowType.FuShi] = {"FuShi","Desc"},
	[ShowType.Halidom] = {"Desc","BicycleEquip"},
	[ShowType.BicycleOrnament] = {"Desc","BicycleEquip"},
	[ShowType.BicycleOrnament] = {"Desc","BicycleEquip"},
	[ShowType.IllustrateRare] = {"Desc","BicycleEquip"},
	[ShowType.FairyDragonSoul] = {"DragonSoulInfo","Desc"},
	[ShowType.FairyDragonSkin] = {"Desc","FairyDragonSkinInfo","ModelShow"},
}
-- 自定义显示优先使用
local custom_parts = {
	["workshop_equip"] = {"EquipBase","BlazeList","EquipSet","Desc"},
	["workshop_blaze"] = {"EquipBase","BlazeList","Desc"}
}

ItemInfoViewInfo = ItemInfoViewInfo or DeclareMono("ItemInfoViewInfo", UIWFlushPanel)

function ItemInfoViewInfo:ItemInfoViewInfo()
	self.data = ItemInfoCtrl.Instance:Data()
	self.data_cares = {{data = self.data.get_way_show,func = self.FlushGetWayShow,keys = {"val"},init_call = false}}
	self.act_objs = {}
end

function ItemInfoViewInfo:activeObj(obj,sibling_i)
	obj:SetActive(true)
	obj:SetSiblingIndex(sibling_i)
	table.insert(self.act_objs,obj)
	return sibling_i + 1	
end


function ItemInfoViewInfo:OnEnable()
	local info = self.data:ItemInfo()
	local item = info.item_list[self.ShowType or 1]

	local sibling_index = 1
	local config
	if item.show_key then
		config = custom_parts[item.show_key]
	else
		config = (item.InfoConfig ~= nil and item:InfoConfig() or nil) or TableCopy(type_parts[item:Type()] or show_type_parts[item:ShowType()] or type_parts.defalut)
		if item:Type() == ItemType.FuYu and next(item.param) == nil then
			config = type_parts.defalut
		end
		--[[ if item:IncAttr() then
			table.insert(config, 2, "PointAttr")
		end ]]
	end
	if self.data:ItemInfo().show_send then
		config = TableCopy(config)
		table.insert(config, "SocietySend")
	end
	sibling_index = self:activeObj(self[config[1]],sibling_index)

	if #config > 1 then
		for i = 2, #config do
			-- sibling_index = self:activeObj(self.Lines[i - 1],sibling_index)
			if self:IsHideItemInfo(info,config[i]) then 
			    sibling_index = self:activeObj(self[config[i]],sibling_index)
			end 
		end
	end
end

function ItemInfoViewInfo:IsHideItemInfo(info,config_name)
	if info.item_list[self.ShowType].param == nil then
		local item_type = info.item_list[self.ShowType]:Type()
	    if config_name == "EquipSet" then 
			return info.item_list[self.ShowType]:Config().suitgroup_id > 0 and EquipSetData.Instance:CheckIsEmptySuit(info.item_list[self.ShowType]:Config().suitgroup_id)
		elseif config_name == "BlazeList" then 
			return info.item_list[self.ShowType]:Config().is_star > 0
		elseif config_name == "SingleBlaze" then 
			return info.item_list[self.ShowType].IsBlazeStar == nil and info.item_list[self.ShowType]:IsBlazeStar()
		elseif config_name == "ModelShow" then
			local show_type = info.item_list[self.ShowType]:ShowType()
			if item_type == ItemType.Skin or item_type == ItemType.Appearance or item_type == ItemType.Fashion or show_type == ShowType.FairyDragonSkin then
				return true
			end
		elseif config_name == "IllusionBase" then
			if item_type == ItemType.Skin or item_type == ItemType.Appearance or item_type == ItemType.Fashion then
				return true
			end
		elseif config_name == "FairyDragonSkinInfo" and info.item_list[self.ShowType]:ShowType() == ShowType.FairyDragonSkin then
			return true
		else
			return false
		end
	end

	if config_name == "EquipSmelt" then 
		local has_inc = info.item_list[self.ShowType]:IncAttr()
		return has_inc
	elseif config_name == "EquipStone" then
		local has_stone = false
		if info.item_list[self.ShowType].param == nil then return has_stone end 
		for i, v in pairs(info.item_list[self.ShowType].param.stone or {}) do
			if v > 0 then 
				has_stone = true 
			    break
		    end 
	    end 
		return has_stone
	elseif config_name == "CastSoul" then
		if info.item_list[self.ShowType].param == nil then
			return false
		end
		if info.item_list[self.ShowType].GetHasSpiritList then
			local spirit_list = info.item_list[self.ShowType]:GetHasSpiritList()
			return not TableIsEmpty(spirit_list)
		end
		return false
	elseif config_name == "EquipSpecial" and info.item_list[self.ShowType].param.sp_id == 0
	and not info.item_list[self.ShowType]:IsMedal() then 
		    return false
	elseif config_name == "EquipSet" then 
		if info.item_list[self.ShowType].param == nil then return false end 
		return info.item_list[self.ShowType].param.new_suit_effect_id and info.item_list[self.ShowType].param.new_suit_effect_id > 0 
		    and EquipSetData.Instance:CheckIsEmptySuit(info.item_list[self.ShowType].param.new_suit_effect_id)
	elseif config_name == "GetWay" then
		if info.item_list[self.ShowType]:GetWay() == nil then return false end 

		self.data.get_way_show.start = info.item_list[self.ShowType]:DescInfoRect(384,22).h + 10
		return self.data.get_way_show.val
	elseif config_name == "AccrssoryItemSuit" then
		if info.item_list[self.ShowType].in_bag then
			return false
		end
	elseif config_name == "AccrssoryItemStrengthAttr" then
		if not info.item_list[self.ShowType].StrengthLevel then
			return false
		end
		if info.item_list[self.ShowType]:StrengthLevel() == 0 then
			return false
		end
	elseif config_name == "Neveal" or config_name == "NevealSkill" then
		local item_data = info.item_list[self.ShowType]
		return item_data ~= nil and item_data.IsNeveal ~= nil and item_data:IsNeveal()
	elseif config_name == "SingleBlaze" then
		return info.item_list[self.ShowType]:IsBlazeStar()
	elseif config_name == "MarbasBlood" then 
		return info.item_list[self.ShowType]:CheckMana() > 0 and MarbasData.Instance:GetBloodPlusAttrs(info.item_list[self.ShowType]).is_act
	elseif config_name == "MarbasGem" then 
		local flag_1 = info.item_list[self.ShowType].CheckMana and info.item_list[self.ShowType]:CheckMana() > 0
		local flag_2 = false 
		for i =1 ,4 do 
			flag_2 = flag_2 or MarbasData.Instance:GetBloodSoltLock(info.item_list[self.ShowType],i)
		end 
		return flag_1 and flag_2
	end 

	return true
end

function ItemInfoViewInfo:OnDisable()
	for _,o in pairs(self.act_objs) do
		o:SetActive(false)
	end
	self.act_objs = {}
	self.data.get_way_show.val = false
	self.data.get_way_show.start = 0
end

function ItemInfoViewInfo:FlushGetWayShow()
	local config
	local item_info = self.data:ItemInfo()
	local item = item_info.item_list[self.ShowType or 1]
	if item.show_key then
		config = custom_parts[item.show_key]
	else
		config = item:InfoConfig() or TableCopy(type_parts[item:Type()] or type_parts.defalut)
	end
	if item_info.show_send then
		config = TableCopy(config)
		table.insert(config, "SocietySend")
	end

	if config[#config] == "GetWay" or item_info.show_send then 
		if self.data.get_way_show.val then 
			self.data.get_way_show.pos = self:activeObj(self.GetWay,#self.act_objs + 1)
		elseif self.data.get_way_show.pos > 0 then 
			self.act_objs[#self.act_objs]:SetActive(false)
			table.remove( self.act_objs,#self.act_objs )
			self.data.get_way_show.pos = 0
		end
		self:HandleGetWatDetails(item, self.data.get_way_show.pos > 0)
	end 
end

function ItemInfoViewInfo:HandleGetWatDetails(item,is_get_way)
	local show_type = item:ShowType()
	if not show_type then
		return
	end
	if show_type == ShowType.BicycleEquip
	or show_type == ShowType.Halidom
	or show_type == ShowType.BicycleOrnament
	or show_type == ShowType.IllustrateRare then
		self.BicycleEquip:SetActive(not is_get_way)
	end
end

ItemInfoViewItem = ItemInfoViewItem or DeclareMono("ItemInfoViewItem", UIWFlushPanel)

function ItemInfoViewItem:ItemInfoViewItem()
	self.data = ItemInfoCtrl.Instance:Data()
	self.info = self.data:ItemInfo()
	self.language = Language.ItemInfo
end

function ItemInfoViewItem:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self.ShowType = ItemInfoConfig.ShowType.Main
	if nil ~= self.ParentMono then
		self.ShowType = self.ParentMono.ShowType
	else
		Debuger.LogError(self._type_tab._type_name .. " ParentMono is nil in item_info_info and can't compare!")
	end
	if ItemInfoConfig.ShowType.Main == self.ShowType then
		self.item1 = self.info.item_list[1]
	elseif ItemInfoConfig.ShowType.Left == self.ShowType then
		self.item1 = self.info.item_list[2]
		self.item2 = self.info.item_list[1]
	end
end

function ItemInfoViewItem:OnClickClose()
	if nil ~= self.data.derive_view then
		ItemInfoCtrl.Instance:ItemDeriveView()
	else
		ViewMgr:CloseView(ItemInfoView)
	end
end