DivineCostumeData = DivineCostumeData or BaseClass()


--选中的标签页 ToggleList的下标对应
DivineCostumeData.SELECT_TAB = {
	EQUIPMENT = 1,
	SPIRITUALISM = 2,
	CHANGEGOD = 3,
	RESONANCE = 4,
}

--共鸣满阶等级
DivineCostumeData.RESONACE_FULL = 14

function DivineCostumeData:__init()
	if DivineCostumeData.Instance ~= nil then
		Debuger.LogError("[DivineCostumeData] attempt to create singleton twice!")
		return
	end	
	DivineCostumeData.Instance = self
	self.tab_index_data = SmartData.New()
	self.red_point = SmartData.New({count = 0})
	self.red_flag = SmartData.New({flag = false})
	self.items_is_full = true
	self.items_is_full_itemid = 0
	self.items_is_full_itemid_num = 0
	self.is_have_show_red = false
	self.is_first_red = false
	self.pop_tab_index_data = SmartData.New()
	self.now_select_equiptype = 0
	self.now_select_equiptype_strengthen_level = 0
	self.now_select_equiptype_color = 0
	self.resonace_flag = SmartData.New({count = 0})
	RemindCtrl.Instance:Remind():Register(Mod.DivineCostume.Main, self.red_point, BindTool.Bind(self.FlushRemind, self))
	LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))
	BagData.Instance:Care(BindTool.Bind(self.OnChangeItemNotice, self))
end

function DivineCostumeData:__delete()

end

function DivineCostumeData:SetResonaceInfo(protocol)
	self.resonace_flag.count = protocol.resonace_flag
	if FunOpen.Instance:GetFunIsOpened(Mod.DivineCostume.Main) then
		self:FreshRedPointCount()
	end
end

function DivineCostumeData:GetResonaceFlagCount()
	return self.resonace_flag.count
end

function DivineCostumeData:OnChangeItemNotice()
	if FunOpen.Instance:GetFunIsOpened(Mod.DivineCostume.Main) then
		self:FreshRedPointCount()
	end
end

function DivineCostumeData:OnLoginEvent()
    if LoginData.Instance:LoginState() == LoginStateType.Logined then
		self:FreshRedPointCount()
    end
	if LoginCtrl.Instance.data:LoginState() == LoginStateType.RoleSelecting then
		self.is_have_show_red = false
		self.is_first_red = false
	end
end

function DivineCostumeData:GetIsHaveRedPointByType(s_type)
	if s_type == 1 then
		local show_equipment_list = self:GetEquipmentShowList()
		for k,v in pairs(show_equipment_list) do 
			local cell_data = self:GetEquipmentData(v.id)
			if cell_data then
				if v.equip_type == 2 then
					local role_id = RoleData.Instance:GetRoleId()
					if UnityPlayerPrefs.GetInt(role_id.."god_equip") == 0 then
						return true
					end
				end
			else
				if v.equip_type ~= 2 then
					local items = self:GetNeedItemsById(v.id)
					local have_full = true
					for k1,v1 in pairs(items) do 
						local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
						if have_item < v1.num then
							have_full = false
							break
						end
					end
					if have_full then
						return true
					end
				end
			end
		end
	elseif s_type == 2 then
		local show_spiritualism_list = self:GetSpiritualismList()
		for k,v in pairs(show_spiritualism_list) do 
			local cell_data = self:GetSpiritualismData(v.id)
			if not cell_data then
				local items = self:GetNeedItemsById(v.id)
				local have_full = true
				for k1,v1 in pairs(items) do 
					local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
					if have_item < v1.num then
						have_full = false
						break
					end
				end
				if have_full then
					return true
				end
			end
		end
	else
		local show_equipment_list = self:GetEquipmentShowList()
		for k,v in pairs(show_equipment_list) do 
			local cell_data = self:GetEquipmentData(v.id)
			if cell_data then	
				local color = self:GetColor(cell_data,Config.godequip_up_auto.godequip_strengthen)
				local level,strengthen_id = self:GetChangeLevelAndId(cell_data,Config.godequip_up_auto.godequip_strengthen)
				if color == 7 and cell_data.param.strengthen_level >= level then
					local items = self:GetChangeNeedItemsById(v.id,Config.godequip_up_auto.godequip_strengthen)
					local have_full = true
					for k1,v1 in pairs(items) do 
						local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
						if have_item < v1.num then
							have_full = false
							break
						end
					end
					if have_full then
						return true
					end
				end
				
			end
		end
		
		
		local show_spiritualism_list = self:GetSpiritualismList()
		for k,v in pairs(show_spiritualism_list) do 
			local cell_data = self:GetSpiritualismData(v.id)
			if cell_data then
				local color = self:GetColor(cell_data,Config.godequip_up_auto.godjewelry_strengthen)
				local level,strengthen_id = self:GetChangeLevelAndId(cell_data,Config.godequip_up_auto.godjewelry_strengthen)
				if color == 7 and cell_data.param.strengthen_level >= level then
					local items = self:GetChangeNeedItemsById(v.id,Config.godequip_up_auto.godjewelry_strengthen)
					local have_full = true
					for k1,v1 in pairs(items) do 
						local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
						if have_item < v1.num then
							have_full = false
							break
						end
					end
					if have_full then
						return true
					end
				end
			end
		end
			
	end
	return false
end

function DivineCostumeData:GetIsHaveRedPoint(v)
	if self.is_have_show_red then
		return false
	end
	if v.equip_type <= 4 then
		local cell_data = self:GetEquipmentData(v.id)
		if cell_data then
			if v.equip_type == 2 then
				local role_id = RoleData.Instance:GetRoleId()
				if UnityPlayerPrefs.GetInt(role_id.."god_equip") == 0 then
					return true
				end
			end
		else
			if v.equip_type ~= 2 then
				local items = self:GetNeedItemsById(v.id)
				local have_full = true
				for k1,v1 in pairs(items) do 
					local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
					if have_item < v1.num then
						have_full = false
						break
					end
				end
				if have_full then
					return true
				end
			end
		end
	else
		local cell_data = self:GetSpiritualismData(v.id)
		if not cell_data then
			local items = self:GetNeedItemsById(v.id)
			local have_full = true
			for k1,v1 in pairs(items) do 
				local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
				if have_item < v1.num then
					have_full = false
					break
				end
			end
			if have_full then
				return true
			end
		end
		
	end
	
	return false
end

function DivineCostumeData:GetChangeNeedItemsById(id,config)
	local godequip_consume_item = {}
	for k,v in pairs(config) do 
		if v.id == id and table.nums(v.break_consume_item) > 0 then
			godequip_consume_item = v.break_consume_item
			break
		end
	end
	return godequip_consume_item
end

function DivineCostumeData:GetIsHaveChangeRedPoint(v)
	if self.is_have_show_red then
		return false
	end
	if v.equip_type <= 4 then
		local cell_data = self:GetEquipmentData(v.id)
		if cell_data then
			local color = self:GetColor(cell_data,Config.godequip_up_auto.godequip_strengthen)
			local level,strengthen_id = self:GetChangeLevelAndId(cell_data,Config.godequip_up_auto.godequip_strengthen)
			if color == 7 and cell_data.param.strengthen_level >= level then
				local items = self:GetChangeNeedItemsById(v.id,Config.godequip_up_auto.godequip_strengthen)
				local have_full = true
				for k1,v1 in pairs(items) do 
					local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
					if have_item < v1.num then
						have_full = false
						break
					end
				end
				if have_full then
					return true
				end
			end
		end
	else
		local cell_data = self:GetSpiritualismData(v.id)
		if cell_data then
			local color = self:GetColor(cell_data,Config.godequip_up_auto.godjewelry_strengthen)
			local level,strengthen_id = self:GetChangeLevelAndId(cell_data,Config.godequip_up_auto.godjewelry_strengthen)
			if color == 7 and cell_data.param.strengthen_level >= level then
				local items = self:GetChangeNeedItemsById(v.id,Config.godequip_up_auto.godjewelry_strengthen)
				local have_full = true
				for k1,v1 in pairs(items) do 
					local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
					if have_item < v1.num then
						have_full = false
						break
					end
				end
				if have_full then
					return true
				end
			end
		end
		
	end
	
	return false
end

function DivineCostumeData:FreshRedPointCount()
	self.red_flag.flag = not self.red_flag.flag
	local show_equipment_list = self:GetEquipmentShowList()
	for k,v in pairs(show_equipment_list) do 
		local cell_data = self:GetEquipmentData(v.id)
		if cell_data then
			if v.equip_type == 2 then
				local role_id = RoleData.Instance:GetRoleId()
				if UnityPlayerPrefs.GetInt(role_id.."god_equip") == 0 then
					self.red_point.count = 1
					return
				end
			end
		else
			if v.equip_type ~= 2 then
				local items = self:GetNeedItemsById(v.id)
				local have_full = true
				for k1,v1 in pairs(items) do 
					local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
					if have_item < v1.num then
						have_full = false
						break
					end
				end
				if have_full then
					self.red_point.count = 1
					return
				end
			end
		end
	end

	local show_spiritualism_list = self:GetSpiritualismList()
	for k,v in pairs(show_spiritualism_list) do 
		local cell_data = self:GetSpiritualismData(v.id)
		if not cell_data then
			local items = self:GetNeedItemsById(v.id)
			local have_full = true
			for k1,v1 in pairs(items) do 
				local have_item = BagData.Instance:GetNumByItemId(v1.item_id)
				if have_item < v1.num then
					have_full = false
					break
				end
			end
			if have_full then
				self.red_point.count = 1
				return
			end
		end
	end
	
	if self:GetIsHaveResonanceRedPoint() then
		self.red_point.count = 1
		return
	end
	self.red_point.count = 0
end

function DivineCostumeData:GetNeedItemsById(id)
	local godequip_consume_item = {}
	local godequip_consume = Config.compose_auto.godequip_consume
	for k,v in pairs(godequip_consume) do 
		if v.id == id then
			godequip_consume_item = v.godequip_consume_item
			break
		end
	end
	return godequip_consume_item
end

function DivineCostumeData:FlushRemind()
	local num = self.red_point.count
	if self.is_have_show_red then
		num = 0
	end
	if self:GetIsHaveResonanceRedPoint() then
		num = 1
	end
    return num
end

function DivineCostumeData:SetSelectTab(ind)
	self.tab_index_data:Set({index = ind})
end


function DivineCostumeData:GetEquipmentShowList()
	local config = Config.godequip_up_auto.godequip_strengthen
	local xing_list_id = {}
	local shen_list_id = {}
	local show_list_id = {}
	local pro = RoleData.Instance:GetProfession() * 100
	for i = 0, 4 do
		for k,v in pairs(config) do 
			if v.equip_type == i and pro == v.limit_prof and v.color == 7 then
				local add = {equip_type = v.equip_type ,id = v.id}
				table.insert(xing_list_id,add)
				break
			end
		end
	end 
	
	for i = 0, 4 do
		for k,v in pairs(config) do 
			if v.equip_type == i and pro == v.limit_prof and v.color == 8 then
				local add = {equip_type = v.equip_type ,id = v.id}
				table.insert(shen_list_id,add)
				break
			end
		end
	end 

	for k,v in pairs(shen_list_id) do 
		local need_add = false
		local add = {equip_type = v.equip_type ,id = v.id}
		local list = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor})
		for k1,v1 in pairs(list) do 
			if v.id == v1.item_id then
				need_add = true
				break
			end
		end
		if need_add then
			table.insert(show_list_id,add)
		else
			local need_add2 = false
			local list1 = BagData.Instance:GetEquipList()
			for k2,v2 in pairs(list1) do 
			if v.id == v2.item_id then
				need_add2 = true
				break
				end
			end
			if need_add2 then
				table.insert(show_list_id,add)
			end
		end
	end

	if #show_list_id > 0 then
		local add_list = {}
		for i = 0, 4 do
			local need_add = true 
			for k,v in pairs(show_list_id) do 
				if v.equip_type == i then
					need_add = false
					break
				end
			end
			if need_add then
				for k,v in pairs(xing_list_id) do 
					if v.equip_type == i then
						local add = {equip_type = v.equip_type ,id = v.id}
						table.insert(add_list,add)
						break
					end
				end
			end
		end
		if #add_list > 0 then
			for k,v in pairs(add_list) do 
				table.insert(show_list_id,v)
			end
		end
	else
		show_list_id = xing_list_id
	end
	table.sort(show_list_id,function(a,b)return a.equip_type < b.equip_type end)
	return show_list_id
end

function DivineCostumeData:GetSpiritualismList()
	local config = Config.godequip_up_auto.godjewelry_strengthen
	local xing_list_id = {}
	local shen_list_id = {}
	local show_list_id = {}
	local pro = RoleData.Instance:GetProfession() * 100
	for i = 5, 7 do
		for k,v in pairs(config) do 
			if v.equip_type == i and pro == v.limit_prof and v.color == 7 then
				local add = {equip_type = v.equip_type ,id = v.id}
				table.insert(xing_list_id,add)
				break
			end
		end
	end 
	
	for i = 5, 7 do
		for k,v in pairs(config) do 
			if v.equip_type == i and pro == v.limit_prof and v.color == 8 then
				local add = {equip_type = v.equip_type ,id = v.id}
				table.insert(shen_list_id,add)
				break
			end
		end
	end 

	for k,v in pairs(shen_list_id) do 
		local need_add = false
		local add = {equip_type = v.equip_type ,id = v.id}
		local list = BagData.Instance:GetGridByType({ItemType.Jewelry})
		for k1,v1 in pairs(list) do 
			if v.id == v1.item_id then
				need_add = true
				break
			end
		end
		if need_add then
			table.insert(show_list_id,add)
		else
			local need_add2 = false
			local list1 = BagData.Instance:GetJewelryList()
			for k2,v2 in pairs(list1) do 
			if v.id == v2.item_id then
				need_add2 = true
				break
				end
			end
			if need_add2 then
				table.insert(show_list_id,add)
			end
		end
	end

	if #show_list_id > 0 then
		local add_list = {}
		for i = 5,7 do
			local need_add = true 
			for k,v in pairs(show_list_id) do 
				if v.equip_type == i then
					need_add = false
					break
				end
			end
			if need_add then
				for k,v in pairs(xing_list_id) do 
					if v.equip_type == i then
						local add = {equip_type = v.equip_type ,id = v.id}
						table.insert(add_list,add)
						break
					end
				end
			end
		end
		if #add_list > 0 then
			for k,v in pairs(add_list) do 
				table.insert(show_list_id,v)
			end
		end
	else
		show_list_id = xing_list_id
	end
	table.sort(show_list_id,function(a,b)return a.equip_type < b.equip_type end)
	return show_list_id
end

function DivineCostumeData:GetChangeGodList()
	local c_list = {} 
	local e_list = self:GetEquipmentShowList()
	local s_list = self:GetSpiritualismList()
	for k,v in pairs(e_list) do 
		table.insert(c_list,v)
	end
	for k,v in pairs(s_list) do 
		table.insert(c_list,v)
	end
	return c_list
end

function DivineCostumeData:GetResonanceList()
	local c_list = {} 
	local e_list = self:GetEquipmentShowList()
	local s_list = self:GetSpiritualismList()
	for k,v in pairs(e_list) do 
		table.insert(c_list,v)
	end
	for k,v in pairs(s_list) do 
		table.insert(c_list,v)
	end
	local data_list = {} 
	for k,v in pairs(c_list) do 
		local cell_data
		if k > 5 then
			cell_data = self:GetSpiritualismData(v.id)
		else
			cell_data = self:GetEquipmentData(v.id)
		end
		if cell_data == nil then
			cell_data = Item.Create({item_id = v.id,param = self:EquipmentParam(v.id,1,k)})
		end
		table.insert(data_list,cell_data)
	end
	return data_list
end

function DivineCostumeData:EquipmentParam(item_id,level,index)
	local item_type = ItemType.Jewelry
	local show_level = level or 0
	if index <6 then
		item_type = 1
	end
	local attr_list = self:GetBaseAttrByIdAndLevel(item_id,show_level,item_type)
	local t = {}
	t.has_random = 0
	t.is_treasure = 0 --0ͨ�� 1��Ʒ
	t.attr_num = 0
	t.attr_list = attr_list
	t.creater_name = ""
	t.sp_id = 0
	t.inc_num = 0
	t.attr_extra_inc = {}
	t.refresh_attr_extra_inc = {}
	t.ultimate_attr_list = nil
	t.suit_effect_id = Item.GetConfig(item_id).suitgroup_id
	t.is_strong_create = 0
	t.strengthen_level = show_level
	t.star_level = 0
	return t
end

function DivineCostumeData:GetEquipmentData(id)
	local cell_data = nil
	local list = BagData.Instance:GetItemsByItemId(id)--BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor})
	for k,v in pairs(list) do 
		if id == v.item_id then
			cell_data = v
			break
		end
	end
	if cell_data == nil then
		local list1 = BagData.Instance:GetEquipList()
		for k,v in pairs(list1) do 
			if id == v.item_id then
				cell_data = v
				break
			end
		end
	end
	return cell_data
end

function DivineCostumeData:GetIsEquip(id,e_type)
	local is_equip = false
	local list = {}
	e_type = e_type or 1
	if e_type == 1 then
		list = BagData.Instance:GetEquipList()
	else
		list = BagData.Instance:GetJewelryList()
	end
	for k,v in pairs(list) do 
		if id == v.item_id then
			is_equip = true
			break
		end
	end
	return is_equip
end

function DivineCostumeData:GetSpiritualismData(id)
	local cell_data = nil
	local list = BagData.Instance:GetItemsByItemId(id)--BagData.Instance:GetGridByType({ItemType.Jewelry})
	for k,v in pairs(list) do 
		if id == v.item_id then
			cell_data = v
			break
		end
	end
	if cell_data == nil then
		local list1 = BagData.Instance:GetJewelryList()
		for k,v in pairs(list1) do 
			if id == v.item_id then
				cell_data = v
				break
			end
		end
	end
	return cell_data
end

function DivineCostumeData:GetAttrInfo(cell_data,config,is_change,level)
	local attr_list = {}
	local strengthen_id = 0
	local full = true
	for k,v in pairs(config) do 
		local strengthen_level = cell_data.param.strengthen_level
		if is_change then
			if strengthen_level < level then
				strengthen_level = level
			end
		end
		if v.id == cell_data.item_id and v.level == strengthen_level then
			if v.attr_add_1 > 0 then
				local add_info = {addtype = v.attr_type_1 ,addnum = v.attr_add_1}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_2 > 0 then
				local add_info = {addtype = v.attr_type_2 ,addnum = v.attr_add_2}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_3 > 0 then
				local add_info = {addtype = v.attr_type_3 ,addnum = v.attr_add_3}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_4 > 0 then
				local add_info = {addtype = v.attr_type_4 ,addnum = v.attr_add_4}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_5 > 0 then
				local add_info = {addtype = v.attr_type_5 ,addnum = v.attr_add_5}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_6 > 0 then
				local add_info = {addtype = v.attr_type_6 ,addnum = v.attr_add_6}
				table.insert(attr_list,add_info)
			end
			break
		end
	end
	
	if is_change then
		local level,strengthen_id = self:GetChangeLevelAndId(cell_data,config)
		local color = self:GetColor(cell_data,config)
		if color == 7 then
			for k,v in pairs(config) do 
				local strengthen_level = cell_data.param.strengthen_level
				if is_change then
					if strengthen_level < level then
						strengthen_level = level
					end
				end
				if v.id == strengthen_id and v.level == strengthen_level then
					full = false
					for k1,v1 in pairs(attr_list) do 
						if  v.attr_type_1 == v1.addtype then
							if v.attr_add_1 > 0 then
								v1.addnum2 = v.attr_add_1
							end
						end
						if  v.attr_type_2 == v1.addtype then
							if v.attr_add_2 > 0 then
								v1.addnum2 = v.attr_add_2
							end
						end
						if  v.attr_type_3 == v1.addtype then
							if v.attr_add_3 > 0 then
								v1.addnum2 = v.attr_add_3
							end
						end
						if  v.attr_type_4 == v1.addtype then
							if v.attr_add_4 > 0 then
								v1.addnum2 = v.attr_add_4
							end
						end
						if  v.attr_type_5 == v1.addtype then
							if v.attr_add_5 > 0 then
								v1.addnum2 = v.attr_add_5
							end
						end
						if  v.attr_type_6 == v1.addtype then
							if v.attr_add_6 > 0 then
								v1.addnum2 = v.attr_add_6
							end
						end
					end	
					break
				end
			end
		end
	else
		for k,v in pairs(config) do 
			if v.id == cell_data.item_id and v.level == (cell_data.param.strengthen_level + 1) then
				full = false
				for k1,v1 in pairs(attr_list) do 
					if  v.attr_type_1 == v1.addtype then
						if v.attr_add_1 > 0 then
							v1.addnum2 = v.attr_add_1
						end
					end
					if  v.attr_type_2 == v1.addtype then
						if v.attr_add_2 > 0 then
							v1.addnum2 = v.attr_add_2
						end
					end
					if  v.attr_type_3 == v1.addtype then
						if v.attr_add_3 > 0 then
							v1.addnum2 = v.attr_add_3
						end
					end
					if  v.attr_type_4 == v1.addtype then
						if v.attr_add_4 > 0 then
							v1.addnum2 = v.attr_add_4
						end
					end
					if  v.attr_type_5 == v1.addtype then
						if v.attr_add_5 > 0 then
							v1.addnum2 = v.attr_add_5
						end
					end
					if  v.attr_type_6 == v1.addtype then
						if v.attr_add_6 > 0 then
							v1.addnum2 = v.attr_add_6
						end
					end
				end	
				break
			end
		end
	end	
	return attr_list,full
end

function DivineCostumeData:GetIsTuPo(cell_data,config)
	local is_tupo = false
	local strengthen_id = 0
	for k,v in pairs(config) do 
		if v.id == cell_data.item_id and v.level == cell_data.param.strengthen_level then
			if v.strengthen_id ~= 0 then
				is_tupo = true
				strengthen_id = v.strengthen_id
			end
			break
		end
	end
	return is_tupo,strengthen_id
end


function DivineCostumeData:GetChangeLevelAndId(cell_data,config)
	local level = 0
	local strengthen_id = 0
	for k,v in pairs(config) do 
		if v.id == cell_data.item_id and v.strengthen_id ~= 0 then
			level = v.level
			strengthen_id = v.strengthen_id
			break
		end
	end
	return level,strengthen_id
end

function DivineCostumeData:GetColor(cell_data,config)
	local color = 7
	for k,v in pairs(config) do 
		if v.id == cell_data.item_id and v.level == cell_data.param.strengthen_level then
			color = v.color
			break
		end
	end
	return color
end

function DivineCostumeData:GetNeedLevel(cell_data,config)
	local need_level = 0
	for k,v in pairs(config) do 
		if v.id == cell_data.item_id and v.level == cell_data.param.strengthen_level + 1 then
			if v.need_rolelevel ~= 0 then
				need_level = v.need_rolelevel
			end
			break
		end
	end
	return need_level
end

function DivineCostumeData:GetModelShowId(item_id,strengthen_level,config)
	local model_id = 0
	for k,v in pairs(config) do 
		if v.id == item_id and v.level == strengthen_level then
			model_id = v.model_id
			break
		end
	end
	return model_id
end

function DivineCostumeData:GetEquipType(item_id,strengthen_level,config)
	local equip_type = 0
	for k,v in pairs(config) do 
		if v.id == item_id and v.level == strengthen_level then
			equip_type = v.equip_type
			break
		end
	end
	return equip_type
end

-- 获取强化配置
function DivineCostumeData:GetGradeCfg(item_id, color, strengthen_level)
	local item_type = Item.GetType(item_id)
	local config = Config.godequip_up_auto.godequip_strengthen
	if item_type == ItemType.Jewelry then
		config = Config.godequip_up_auto.godjewelry_strengthen
	end
	for k,v in pairs(config) do
		if v.color == color and v.level == strengthen_level then
			return v
		end
	end
	return {}
end


function DivineCostumeData:GetBaseAttrByIdAndLevel(item_id,strengthen_level,item_type)
	local attr_list = {}
	local config = Config.godequip_up_auto.godequip_strengthen
	if item_type == ItemType.Jewelry then
		config = Config.godequip_up_auto.godjewelry_strengthen
	end
	for k,v in pairs(config) do 
		if v.id == item_id and v.level == strengthen_level then
			if v.attr_add_1 >= 0 then
				local add_info = {attr_type = v.attr_type_1 ,attr_value = v.attr_add_1}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_2 >= 0 then
				local add_info = {attr_type = v.attr_type_2 ,attr_value = v.attr_add_2}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_3 >= 0 then
				local add_info = {attr_type = v.attr_type_3 ,attr_value = v.attr_add_3}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_4 >= 0 then
				local add_info = {attr_type = v.attr_type_4 ,attr_value = v.attr_add_4}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_5 >= 0 then
				local add_info = {attr_type = v.attr_type_5 ,attr_value = v.attr_add_5}
				table.insert(attr_list,add_info)
			end
			if v.attr_add_6 >= 0 then
				local add_info = {attr_type = v.attr_type_6 ,attr_value = v.attr_add_6}
				table.insert(attr_list,add_info)
			end
			break
		end
	end
	return attr_list
end

function DivineCostumeData.CheckEquip(item_list)
	for k,item in pairs(item_list) do
		if item.old_id then
			local co = Cfg.RewardJobBagByIdType(item.old_id,RoleData.Instance:ProfType())
			item.item_id = co and co.new_id or item.item_id
			if item.old_id == 48806 then
				item.param.attr_list = Equip.GetSingleAttrSection(item.item_id)
			end
		end
	end
	return item_list
end

-- 获取对应神装武器的所有升级属性
function DivineCostumeData:GetAllAttrById()
	local pro = RoleData.Instance:GetProfession() * 100
	if not self.godequip_level_config then
		self.godequip_level_config = {}
	end
	if self.godequip_level_config[pro] then
		return self.godequip_level_config[pro]
	else
		self.godequip_level_config[pro] = {}
		-- 1用于存储曜装阶段的，2用于存储神话阶段的
		self.godequip_level_config[pro][1] = {}
		self.godequip_level_config[pro][2] = {}
	end
	for i, v in pairs(Config.godequip_up_auto.godequip_strengthen) do
		if v.equip_type == 2 and pro == v.limit_prof and v.level ~= 0 then
			local tab = {}
			for i = 1, 6 do
				if v["attr_add_" .. i] > 0 then
					local attr = CommonStruct.AttrStruct(v["attr_type_" .. i], v["attr_add_" .. i])
					table.insert(tab, attr)
				end
			end
			local tab2 = {}
			tab2.attr = tab
			tab2.level = v.level
			tab2.need_rolelevel = v.need_rolelevel
			if v.color == 7 then
				table.insert(self.godequip_level_config[pro][1], tab2)
			else
				table.insert(self.godequip_level_config[pro][2], tab2)
			end
		end
	end
	table.sort(self.godequip_level_config[pro][1], DataSorter.KeyLowerSorter("level"))
	table.sort(self.godequip_level_config[pro][2], DataSorter.KeyLowerSorter("level"))
	return self.godequip_level_config[pro]
end

function DivineCostumeData:GetBtnList()
	local btn_list = {}
	local btn_one = {page_name = Language.DivineCostume.PopAttribute,page_index = 1}
	local btn_two = {page_name = Language.DivineCostume.PopExplain,page_index = 2}
	table.insert(btn_list,btn_one)
	table.insert(btn_list,btn_two)
	return btn_list
end

-- 获取对应神装武器的所有升级属性
function DivineCostumeData:GetAllAttrByEquipType(equip_type)
	local pro = RoleData.Instance:GetProfession() * 100
	local godequip_level_config = {}
	godequip_level_config[pro] = {}
	-- 1用于存储曜装阶段的，2用于存储神话阶段的
	godequip_level_config[pro][1] = {}
	godequip_level_config[pro][2] = {}
	local config = Config.godequip_up_auto.godequip_strengthen
	if equip_type > 4 then
		config = Config.godequip_up_auto.godjewelry_strengthen
	end
	for i, v in pairs(config) do
		if v.equip_type == equip_type and pro == v.limit_prof --[[and v.level ~= 0 --]]then
			local tab = {}
			for i = 1, 6 do
				if v["attr_add_" .. i] > 0 then
					local attr = {attr_type = v["attr_type_" .. i], attr_name = Language.Common.AttrList[v["attr_type_" .. i]], attr_value = v["attr_add_" .. i]}
					table.insert(tab, attr)
				end
			end
			local tab2 = {}
			tab2.attr = tab
			tab2.level = v.level
			tab2.need_rolelevel = v.need_rolelevel
			tab2.color = v.color
			if v.color == 7 then
				tab2.index = 1
				table.insert(godequip_level_config[pro][1], tab2)
			else
				tab2.index = 2
				table.insert(godequip_level_config[pro][2], tab2)
			end
		end
	end
	table.sort(godequip_level_config[pro][1], DataSorter.KeyLowerSorter("level"))
	table.sort(godequip_level_config[pro][2], DataSorter.KeyLowerSorter("level"))
	local all_level_config = {}
	if DivineCostumeData.Instance.now_select_equiptype_color == 7 then
		for i, v in pairs(godequip_level_config[pro][1]) do
			table.insert(all_level_config, v)
		end
	else
		for i, v in pairs(godequip_level_config[pro][2]) do
			table.insert(all_level_config, v)
		end
	end
	
	return all_level_config
end

-- 装等
function DivineCostumeData:GetChangeShowLevel(cell_data)
	local is_equip = cell_data:IsEquip()
	local level = 0
	local config = Config.godequip_up_auto.godequip_strengthen
	if not is_equip then
		config = Config.godequip_up_auto.godjewelry_strengthen
	end
	for k,v in pairs(config) do 
		if cell_data and v.id == cell_data.item_id and v.level == cell_data.param.strengthen_level then
			level = v.equip_level
			break
		end
	end
	return level
end

function DivineCostumeData:GetTotleResonance(addtion_level)
	local limit_prof = RoleData.Instance:GetRoleProfId()
	local config = Config.godequip_up_auto.godequip_addition
	
	for k,v in pairs(config) do 
		if v.limit_prof == limit_prof and v.addtion_level <=addtion_level and v.is_addition == 1 then
			
		end
	end
	return level
end

function DivineCostumeData:GetNextResonanceLevel(current_level)
	local limit_prof = RoleData.Instance:GetProfession() * 100
	local config = Config.godequip_up_auto.godequip_addition
	local next_level = current_level + 1
	local level = 0
	for k,v in pairs(config) do 
		if v.limit_prof == limit_prof and v.addition_level == next_level and v.is_addition == 1 then
			level = v.level
			break
		end
	end
	return level
end

function DivineCostumeData:GetNextLevelCondition(current_level)
	local level = self:GetNextResonanceLevel(current_level)
	local list = self:GetResonanceList()
	local progress = 0
	for k,v in pairs(list) do 
		if v.param.strengthen_level >= level then
			progress  = progress + 1
		end
	end
	return progress
end

function DivineCostumeData:GetIsHaveResonanceRedPoint()
	if self.resonace_flag.count < DivineCostumeData.RESONACE_FULL then
		if self:GetNextLevelCondition(self.resonace_flag.count) == 8 then
			return true
		end
	end
	return false
end


function DivineCostumeData:GetAllResonanceData(current_level)
	local limit_prof = RoleData.Instance:GetProfession() * 100
	local config = Config.godequip_up_auto.godequip_addition
	local attr_list = {}
	if current_level > 0 then
		local title_current = {attr_type = -1 ,attr_add = 0}
		table.insert(attr_list,title_current)
	end
	for k,v in pairs(config) do 
		if v.limit_prof == limit_prof and v.addition_level <= current_level and v.is_addition == 1 then
			local attr_data = {attr_type = 0 ,attr_add = 0}
			attr_data.attr_type = v.attr_type_1
			attr_data.attr_add = v.attr_add_1
			attr_list = self:AddResonanceData(attr_data,attr_list)
			local attr_data_2 = {attr_type = 0 ,attr_add = 0}
			attr_data_2.attr_type = v.attr_type_2
			attr_data_2.attr_add = v.attr_add_2
			attr_list = self:AddResonanceData(attr_data_2,attr_list)			
		end
	end

	if DivineCostumeData.RESONACE_FULL ~= current_level then
		local title_next = {attr_type = -1 ,attr_add = 1}
		table.insert(attr_list,title_next)
		
		local next_attr_list = {}
		for k,v in pairs(config) do 
			if v.limit_prof == limit_prof and v.addition_level <= (current_level + 1) and v.is_addition == 1 then
				local attr_data = {attr_type = 0 ,attr_add = 0}
				attr_data.attr_type = v.attr_type_1
				attr_data.attr_add = v.attr_add_1
				next_attr_list = self:AddResonanceData(attr_data,next_attr_list)
				local attr_data_2 = {attr_type = 0 ,attr_add = 0}
				attr_data_2.attr_type = v.attr_type_2
				attr_data_2.attr_add = v.attr_add_2
				next_attr_list = self:AddResonanceData(attr_data_2,next_attr_list)			
			end
		end
		
		for k,v in pairs(next_attr_list) do 
			table.insert(attr_list,v)
		end	
	end

	for k,v in pairs(attr_list) do 
		v.index = k
	end	
	
	return attr_list
end

function DivineCostumeData:AddResonanceData(attr_data,attr_list)
	local have_add = false
	for k,v in pairs(attr_list) do 
		if v.attr_type == attr_data.attr_type then
			v.attr_add = v.attr_add + attr_data.attr_add
			have_add = true
			break
		end
	end
	if not have_add then
		table.insert(attr_list,attr_data)
	end
	return attr_list
end

function DivineCostumeData:GetConfigResonanceData()
	local limit_prof = RoleData.Instance:GetProfession() * 100
	local config = Config.godequip_up_auto.godequip_addition
	local all_attr_list = {}
	for i=1,DivineCostumeData.RESONACE_FULL do
		for k,v in pairs(config) do 
			if v.limit_prof == limit_prof and v.addition_level == i and v.is_addition == 1 then
				local attr_list = {}
				local attr_data = {attr_type = 0 ,attr_add = 0,index = i}
				attr_data.attr_type = v.attr_type_1
				attr_data.attr_add = v.attr_add_1
				table.insert(attr_list,attr_data)
				local attr_data_2 = {attr_type = 0 ,attr_add = 0,index = i}
				attr_data_2.attr_type = v.attr_type_2
				attr_data_2.attr_add = v.attr_add_2
				table.insert(attr_list,attr_data_2)	
				table.insert(all_attr_list,attr_list)
				break
			end
		end
	end
	return all_attr_list
end

function DivineCostumeData:GetShowChangeData(current_level)
	local limit_prof = RoleData.Instance:GetProfession() * 100
	local config = Config.godequip_up_auto.godequip_addition
	local attr_list = {}
	for k,v in pairs(config) do 
		if v.limit_prof == limit_prof and v.addition_level <= current_level and v.is_addition == 1 then
			local attr_data = {attr_type = 0 ,attr_add = 0}
			attr_data.attr_type = v.attr_type_1
			attr_data.attr_add = v.attr_add_1
			attr_list = self:AddResonanceData(attr_data,attr_list)
			local attr_data_2 = {attr_type = 0 ,attr_add = 0}
			attr_data_2.attr_type = v.attr_type_2
			attr_data_2.attr_add = v.attr_add_2
			attr_list = self:AddResonanceData(attr_data_2,attr_list)			
		end
	end

	local before_attr_list = {}
	for k,v in pairs(config) do 
		if v.limit_prof == limit_prof and v.addition_level <= current_level -1 and v.is_addition == 1 then
			local attr_data = {attr_type = 0 ,attr_add = 0}
			attr_data.attr_type = v.attr_type_1
			attr_data.attr_add = v.attr_add_1
			before_attr_list = self:AddResonanceData(attr_data,before_attr_list)
			local attr_data_2 = {attr_type = 0 ,attr_add = 0}
			attr_data_2.attr_type = v.attr_type_2
			attr_data_2.attr_add = v.attr_add_2
			before_attr_list = self:AddResonanceData(attr_data_2,before_attr_list)			
		end
	end
	
	local final_list = {}
	for k,v in pairs(attr_list) do 
		v.old_value = 0
		v.is_new = true
		for k1,v1 in pairs(before_attr_list) do 
			if v.attr_type == v1.attr_type then
				if v.attr_add ~= v1.attr_add then
					v.old_value = v1.attr_add
				end
				v.is_new = false
			end
		end
		if v.is_new or v.old_value ~= 0 then
			table.insert(final_list,v)
		end
	end
	for k,v in pairs(final_list) do 
		v.index = k
	end
	
	return final_list
end


function DivineCostumeData:IsEquipAllDivineCostume()
	local equip_list = BagData.Instance:GetEquipList()
	local jewelry_list = BagData.Instance:GetJewelryList()
	if table.nums(equip_list) < 5 then
		return false
	else
		for k,v in pairs(equip_list) do 
			if v:StrengthenLevel() == 0 then
				return false
			end
		end
	end
	if table.nums(jewelry_list) < 3 then
		return false
	else
		for k,v in pairs(jewelry_list) do 
			if v:StrengthenLevel() == 0 then
				return false
			end
		end
	end	
	return true
end