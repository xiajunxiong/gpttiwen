
PetEquipSelectView = PetEquipSelectView or DeclareView("PetEquipSelectView", "pet/pet_equip_select")
-- item_array = Array(PetEquipItem)
function PetEquipSelectView:PetEquipSelectView()
	self.lang = Language.PetEquipSelect
end

function PetEquipSelectView:LoadCallback()
	self.Board:SetData(self:GetType(), self.lang.Title,Vector2.New(1088, 598))
end

function PetEquipSelectView:CloseCallback()
	PetData.Instance.pet_equip_select:Set({item_type = 1, skill_type = 0, item_index = -1, equip = 1}) 
end
-----------PetEquipSelectFlush--------------
PetEquipSelectFlush = PetEquipSelectFlush or DeclareMono("PetEquipSelectFlush", UIWFlushPanel)
function PetEquipSelectFlush:PetEquipSelectFlush()
	self.pet_data = PetData.Instance.pet_equipment_select_another_open and AnotherData.Instance or PetData.Instance
	self.lang = Language.PetEquipSelect
	self.data_cares = {
		{data = self.pet_data.pet_equip_select, func = self.FlushEquipList, keys = {"item_type","skill_type"}},
	}
	self.show_skill_type_list = false
	self.show_item_type_list = false
end

function PetEquipSelectFlush:FlushEquipList()
	local item_type = self.pet_data.pet_equip_select.item_type
	UH.SetText(self.ItemTypeText, PetData.Instance:GetEquipItemTypeList()[item_type].name)
	UH.SetText(self.SkillTypeText, self.lang.SkillTypeList[self.pet_data.pet_equip_select.skill_type])

	local equip_list
	if self.pet_data.pet_equip_select.equip ~= 4 then
		equip_list = PetData.Instance.pet_equipment_select_another_open and AnotherData.Instance:GetEquipShowList() or PetData.Instance:GetEquipShowList()
	end
	
	--是合成界面过来的筛选一下
	if self.pet_data.pet_equip_select.equip == 3 then
		for i=#equip_list,1,-1 do
			if not AmuletData.Instance:CheckIsCanAssembly(equip_list[i]) then
				table.remove(equip_list,i)
			end
		end
	elseif self.pet_data.pet_equip_select.equip == 4 then
		local pos = self.pet_data.pet_equip_select.pos
		local skill_type = self.pet_data.pet_equip_select.skill_type
		if skill_type == 1 then
			equip_list = {}
		else
			local item_id = self.pet_data.pet_equip_select.item_type == 1 and 0 or self.pet_data:GetPetItemTypeId()
			equip_list = AmuletData.Instance:GetIntensifyStuffList(pos,item_id)
		end
	end
	local show_list = {}

	--拿到列表后每4个物品分为一个显示组
	if equip_list ~= nil and next(equip_list)~=nil then
		if #equip_list > 0 then
			local t = {}
			local title_index = 1
			for i=1,#equip_list do
				show_list[title_index] = show_list[title_index] or {}
				show_list[title_index].equip_list = show_list[title_index].equip_list or {}
				if i == 1 then
					show_list[title_index].first = true
				end
				table.insert(show_list[title_index].equip_list, equip_list[i])
				if i%4 == 0 then
					title_index = title_index + 1
				end
			end
		end
	else
		UH.SetText(self.BtnText, self.lang.BtnText[self.pet_data.pet_equip_select.equip])
	end
	self.NoEquip:SetActive(equip_list == nil or next(equip_list)==nil)
	self.EquipList:SetData(show_list)
	self.ItemChooseList:SetData({})
	self.SkillChooseList:SetData({})
	self.show_skill_type_list = false
	self.show_item_type_list = false
end

function PetEquipSelectFlush:OnClickItemType()
	self.show_item_type_list = not self.show_item_type_list
	local data_list = {}
	if self.show_item_type_list then
		data_list = PetData.Instance:GetEquipItemTypeList()
	end
	self.ItemChooseList:SetData(data_list)
end

function PetEquipSelectFlush:OnClickSkillType()
	self.show_skill_type_list = not self.show_skill_type_list
	local data_list = {}
	if self.show_skill_type_list then
		data_list = PetData.Instance:GetEquipSkillTypeList()
	end
	self.SkillChooseList:SetData(data_list)
end

function PetEquipSelectFlush:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.pet_data.pet_equip_select.item_index = -1
	self.pet_data.cur_select_pet_equip = nil
end
-----------PetEquipRightInfo-----------------
-- ItemName = Text
-- SkillName1 = Text
-- SkillName2 = Text
-- SkillDesc1 = Text
-- SkillDesc1 = Text
-- BtnText = Text
PetEquipRightInfo = PetEquipRightInfo or DeclareMono("PetEquipRightInfo", UIWFlushPanel)
function PetEquipRightInfo:PetEquipRightInfo()
	self.pet_data = PetData.Instance.pet_equipment_select_another_open and AnotherData.Instance or PetData.Instance-- PetData.Instance
	self.lang = Language.PetEquipSelect
	self.data_cares = {
		{data = self.pet_data.pet_equip_select, func = self.FlushEquipInfo, keys = {"item_index"}, init_call = false},
		{data = BagData.Instance.item_list[ItemColumnType.Item], func = self.FlushAssemblyData,init_call = false},
	}
end

function PetEquipRightInfo:FlushEquipInfo()
	local item_data = self.pet_data.cur_select_pet_equip
	UH.SetText(self.ItemName, item_data:Name())
	for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
		local skill_name = ""
		local skill_desc = ""
		local skill_id = item_data.param.equipment_skill_list[i]
		if skill_id ~= 0 then
			local level = AmuletData.Instance:GetIntensifySkillLevel(skill_id)
			local skill_cfg = PetData.Instance:GetGiftDetail(skill_id, level)
			if skill_cfg ~= nil then
				skill_name = skill_cfg.name
				skill_desc = skill_cfg.desc_1
			end
			if level and level > 0 then
				skill_name = string.format(Language.Pet.PetEquip.LevelShow,skill_name, level)
			end
		end
		UH.SetText(self["SkillName"..tostring(i)], skill_name)
		UH.SetText(self["SkillDesc"..tostring(i)], skill_desc)
	end
	UH.SetText(self.BtnText, self.lang.BtnText[self.pet_data.pet_equip_select.equip])
	UH.SetIcon(self.Icon, item_data:IconId())
end

function PetEquipRightInfo:OnClickEquipBtn()
	local item_data = self.pet_data.cur_select_pet_equip
	if item_data then
		if self.pet_data.pet_equip_select.equip == 1 or self.pet_data.pet_equip_select.equip == 2 then
			if self.pet_data.cur_select_pet_equip ~= nil then
				--pet_index>0说明这个装备被其它宠物已经装备过了
				if (item_data.param.pet_index or -1) >= 0 then
					-- 如果替换宠物自己的装备，则不操作
					if item_data.param.pet_index ~=nil and item_data.param.pet_index ==  self.pet_data:GetCurSelect().index then
						PublicPopupCtrl.Instance:Center(Language.PetEquipSelect.NotCanEquip)
						return
					end
					if not PetData.Instance.pet_equipment_select_another_open then
						PetCtrl.SendPetOp(PET_OP_TYPE.PetEquipmentChange, self.pet_data:GetCurSelect().index, item_data.param.pet_index)
					else
						AnotherCtrl.SendPetOp(AnotherPetOpType.PetEquipmentPutOn, self.pet_data:GetCurSelect().index, item_data.index)
					end
				else
					if not PetData.Instance.pet_equipment_select_another_open then
						PetCtrl.SendPetOp(PET_OP_TYPE.PetEquipmentPutOn, self.pet_data:GetCurSelect().index, self.pet_data.cur_select_pet_equip.index)
					else
						AnotherCtrl.SendPetOp(AnotherPetOpType.PetEquipmentPutOn, self.pet_data:GetCurSelect().index, item_data.index)
					end
				end
				if MainViewData.Instance:IsBattle() then
					PublicPopupCtrl.Instance:Center(ErrorText[110])
				end
			else
				PublicPopupCtrl.Instance:Center(self.lang.NoEquip)
				return
			end
		elseif self.pet_data.pet_equip_select.equip == 3 then
			if (item_data.param.pet_index or -1) >= 0 then
				self.select_item = item_data
				PetCtrl.SendPetOp(PET_OP_TYPE.PetEquipmentPutOff,item_data.param.pet_index)
				if MainViewData.Instance:IsBattle() then
					PublicPopupCtrl.Instance:Center(ErrorText[110])
				end
				return
			elseif item_data.index~=nil then
				AmuletData.Instance:SetAssemblyData(item_data)
			end
		elseif self.pet_data.pet_equip_select.equip == 4 then
			local pos = self.pet_data.pet_equip_select.pos
			AmuletData.Instance:SetAmuletIntensifyData(pos, item_data)
		end
		ViewMgr:CloseView(PetEquipSelectView)
	end
end

function PetEquipRightInfo:FlushAssemblyData()
	if self.pet_data.pet_equip_select.equip == 3 and self.select_item then
		local item = AmuletData.Instance:GetItemIndexByItemIdAndSkill(self.select_item)
		AmuletData.Instance:SetAssemblyData(item)
		ViewMgr:CloseView(PetEquipSelectView)
	end
end
---------------EquipTitleItem----------------------
--因为每行由一条贯穿的横杆子所以要多加一级item
EquipTitleItem = EquipTitleItem or DeclareMono("EquipTitleItem", UIWidgetBaseItem)
function EquipTitleItem:EquipTitleItem()
end

function EquipTitleItem:SetData(data)
	self.data = data
	self.EquipItemList:SetData(data.equip_list)
	if self.data.first then
		self.EquipItemList:ClickItem(1)
	end
end

--------------PetEquipItem--------------------------
PetEquipItem = PetEquipItem or DeclareMono("PetEquipItem", UIWidgetBaseItem)
-- ItemName = Text
function PetEquipItem:PetEquipItem()
	self.pet_data = PetData.Instance.pet_equipment_select_another_open and AnotherData.Instance or PetData.Instance
end

function PetEquipItem:SetData(data)
	self.data = data
	UH.SetText(self.ItemName, data:Name())
	UH.SetIcon(self.Icon, data:IconId())
	local skill_name = ""
	for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
		if data.param.equipment_skill_list[i] ~= 0 then
			if i >= 2 then
				skill_name = skill_name .. "\n"
			end
			local skill_id = data.param.equipment_skill_list[i]
			local level = AmuletData.Instance:GetIntensifySkillLevel(skill_id)
			local skill_cfg = PetData.Instance:GetGiftDetail(skill_id, level)
			if skill_cfg == nil then
				LogError("skill_id",data.param.equipment_skill_list[i],"config is nil")
			else
				local name = skill_cfg.name
				local qua = skill_cfg.page
				if level and level > 0 then
					name = string.format(Language.Pet.PetEquip.LevelShow2,name, level)
					qua = 3
				end
				skill_name = skill_name .. string.format(Language.PetEquipSelect.SkillNameColor, Language.PetEquipSelect.SkillColor[qua], name)
			end
		end
	end
	UH.SetText(self.SkillName, skill_name)
	if data.param.pet_index ~=nil and data.param.pet_index >= 0 then
        self.UsedObj:SetActive(true)
    end
end

function PetEquipItem:Click()
	self.Toggle.isOn = true
	self:OnClickCheckInfo()
end

function PetEquipItem:OnClickCheckInfo()
	self.pet_data.cur_select_pet_equip = self.data
	if self.data.index ~= nil or self.pet_data.pet_equip_select.item_index ~= self.data.index then
		self.pet_data.pet_equip_select.item_index = self.data.index
	else
		self.pet_data.pet_equip_select:SetDirty("item_index")
	end
	-- self.pet_data.pet_equip_select.item_index = self.data.index
end
--------------------PetChooseBtnItem-------------------
-- BtnText = Text
PetChooseBtnItem = PetChooseBtnItem or DeclareMono("PetChooseBtnItem", UIWidgetBaseItem)
function PetChooseBtnItem:PetChooseBtnItem()
	self.pet_data = PetData.Instance.pet_equipment_select_another_open and AnotherData.Instance or PetData.Instance
end

function PetChooseBtnItem:SetData(data)
	self.data = data
	UH.SetText(self.BtnText, data.name)
end

function PetChooseBtnItem:OnClickBaseButton()
	if self.data.skill_type then
		self.pet_data.pet_equip_select.skill_type = self.data.skill_type
	elseif self.data.item_type then
		self.pet_data.pet_equip_select.item_type = self.data.item_type
	end
end