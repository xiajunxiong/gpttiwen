PetHuntExchange = PetHuntExchange or DeclareView("PetHuntExchange", "treasure_hunt/treasure_hunt_pet_exchange")
VIEW_DECLARE_MASK(PetHuntExchange, ViewMask.BgBlock)
-- BtnForget
function PetHuntExchange:PetHuntExchange()
	TreasureHuntData.Instance:ResetExchangeSmart()
end
function PetHuntExchange:LoadCallback()
	TreasureHuntData.Instance.exchange_request.sended_req = 0
	UH.SetText(self.NoticeInfo, TreasureHuntData.Instance:GetCurSelectCfg().displace_tips)
end
function PetHuntExchange:OnCloseClick()
	ViewMgr:CloseView(PetHuntExchange)
end
--------------------- 
PetHuntExchangePanel = PetHuntExchangePanel or DeclareMono("PetHuntExchangePanel",UIWFlushPanel)
function PetHuntExchangePanel:PetHuntExchangePanel()
	self.pet_data = PetData.Instance
	self.hunt_data = TreasureHuntData.Instance
	self.cur_select_cfg = self.hunt_data:GetCurSelectCfg()
	self.cumsume_strengthen_num1 = self.hunt_data.all_config.dis_material[self.cur_select_cfg.material_id1].strengthen_num
	self.cumsume_strengthen_num2 = self.hunt_data.all_config.dis_material[self.cur_select_cfg.material_id2].strengthen_num
	self.get_cfg_list = self.hunt_data.all_config.dis_acquire[self.cur_select_cfg.get_id]
	self.data_cares = {
        {data = TreasureHuntData.Instance.exchange_smart, func = self.SetComsume1, keys = {"comsume_type_index1"},},
        {data = TreasureHuntData.Instance.exchange_smart, func = self.SetComsume2, keys = {"comsume_type_index2"}},
        {data = TreasureHuntData.Instance.exchange_smart, func = self.SetCompose, keys = {"compose_index"}},
    }
end

function PetHuntExchangePanel:OnDestroy()
	if self.hunt_data.exchange_request.sended_req ~= 1 then
		self.hunt_data:ResetExchangeSmart()
	end
	UIWFlushPanel.OnDestroy(self)
end

function PetHuntExchangePanel:SetCompose()
	local t = {}
	t.clicked_index = self.cur_select_cfg.get_id
	t.str_level = self.get_cfg_list.strengthen_num
	local get_cfg_list = self.hunt_data:GetExchangeAcqList(self.cur_select_cfg.get_id)
	local config = self.pet_data:GetPetCfg(get_cfg_list[1].pet_id)
	t.quality = config.quality
	if self.hunt_data.exchange_smart.compose_index > 0 then
		local pet_cfg = self.pet_data:GetPetCfg(self.hunt_data.exchange_smart.compose_pet_id)
		t.icon_id = pet_cfg.icon_id
		t.quality = pet_cfg.quality
		t.level = 1
	end
	self.ComposePet:SetData(t)
end

function PetHuntExchangePanel:SetComsume1()
	local t = {}
	t.clicked_index = -1
	t.str_level = self.cumsume_strengthen_num1
	local mat_cfg_list = self.hunt_data.all_config.dis_material[self.cur_select_cfg.material_id1]
	for k,v in pairs(mat_cfg_list) do
		if type(k) ~= "string" then
			t.quality = self.pet_data:GetPetCfg(k).quality
			break
		end
	end
	if self.hunt_data.exchange_smart.comsume_type_index1 >= 0 then
		self.comsume_pet1 = self.pet_data:GetPetInfoByType(self.hunt_data.exchange_smart.comsume_type1, self.hunt_data.exchange_smart.comsume_type_index1)
		t.icon_id = self.comsume_pet1:IconId()
		t.quality = self.comsume_pet1.config.quality
		t.level = self.comsume_pet1.level
	end
	self.ConsumePet1:SetData(t)
end

function PetHuntExchangePanel:SetComsume2()
	local t = {}
	t.clicked_index = -2
	t.str_level = self.cumsume_strengthen_num2
	local mat_cfg_list = self.hunt_data.all_config.dis_material[self.cur_select_cfg.material_id2]
	for k,v in pairs(mat_cfg_list) do
		if type(k) ~= "string" then
			t.quality = self.pet_data:GetPetCfg(k).quality
			break
		end
	end
	if self.hunt_data.exchange_smart.comsume_type_index2 >= 0 then
		self.comsume_pet2 = self.pet_data:GetPetInfoByType(self.hunt_data.exchange_smart.comsume_type2, self.hunt_data.exchange_smart.comsume_type_index2)
		t.icon_id = self.comsume_pet2:IconId()
		t.quality = self.comsume_pet2.config.quality
		t.level = self.comsume_pet2.level
	end
	self.ConsumePet2:SetData(t)
end

function PetHuntExchangePanel:OnClickExchange()
	if self.hunt_data.exchange_smart.comsume_type1 == -1 
		or self.hunt_data.exchange_smart.comsume_type2 == -1
		or self.hunt_data.exchange_smart.compose_index == 0 then
		PublicPopupCtrl.Instance:Center(Language.TreasureHunt.PetHuntExchangeSelectIlegal)
		return
	end
	if self.comsume_pet1.info.pet_equipment_id ~= 0 or self.comsume_pet2.info.pet_equipment_id ~= 0 then
		PublicPopupCtrl.Instance:Center(Language.TreasureHunt.PetHuntExchangeEquip)
		return
	end
	self.hunt_data.exchange_request.sended_req = 1
	-- TreasureHuntCtrl.Instance:SendCSXunBaoExchange()
	ViewMgr:CloseView(PetHuntExchange)
end
----------------------------
ConsumeItem = ConsumeItem or DeclareMono("ConsumeItem", UIWidgetBaseItem)
function ConsumeItem:ConsumeItem()
	self.hunt_data = TreasureHuntData.Instance
end

function ConsumeItem:SetData(data)
	UH.SpriteName(self.StrLevel, "PetNum"..data.str_level)
	if data.icon_id and data.icon_id ~= 0 then
		if data.level and data.level <= 0 then
			self.LevelObj:SetActive(false)
		else
			self.LevelObj:SetActive(true)
			UH.SetText(self.Level, data.level)
		end
		-- LogError("data.level",data.level)
		UH.SetIcon(self.Icon, data.icon_id)
		self.AddImgObj:SetActive(false)
	else
		self.LevelObj:SetActive(false)
		UH.SetIcon(self.Icon, 0)
		self.AddImgObj:SetActive(true)
	end
	UH.SpriteName(self.QuaFrame, "pet_qua_"..data.quality)
	self.data = data
end

function ConsumeItem:OnClickPet()
	self.hunt_data.exchange_smart.clicked_index = self.data.clicked_index
	ViewMgr:OpenView(PetHuntSelect)
end
--------------------------------------------------------
PetHuntSelect = PetHuntSelect or DeclareView("PetHuntSelect", "treasure_hunt/treasure_hunt_pet_select")
VIEW_DECLARE_MASK(PetHuntSelect, ViewMask.BgBlock)
function PetHuntSelect:PetHuntSelect()
	self.pet_data = PetData.Instance
	self.hunt_data = TreasureHuntData.Instance
end

function PetHuntSelect:LoadCallback()
	self.Board:SetData(self:GetType(), Language.TreasureHunt.PetHuntExchange, Vector2.New(740, 473))
	self.cur_select_cfg = self.hunt_data:GetCurSelectCfg()
	--大于0时为寻宝配置中对应的get_id
	if self.hunt_data.exchange_smart.clicked_index > 0 then
		local get_cfg_list = self.hunt_data:GetExchangeAcqList(self.cur_select_cfg.get_id)
		local pet_list = {}
		for k,v in pairs(get_cfg_list) do
			local pet = Pet.New({pet_id = v.pet_id, str_level = v.strengthen_num})
			pet.seq = v.seq
			table.insert(pet_list, pet)
		end
		self:SetSelectList(pet_list)
	else
		local comsume_id = self.cur_select_cfg["material_id"..math.abs(self.hunt_data.exchange_smart.clicked_index)]
		local pet_list = table.rearrange(self.pet_data:GetComsumePetList(comsume_id))
		self:SetSelectList(pet_list)
	end
end

function PetHuntSelect:SetSelectList(pet_list)
	if #pet_list == 0 then
		self:SetEmpty()
		return
	end
	self.is_empty = false
	self.NoPetChoseObj:SetActive(false)

	function sort_fun(a,b)
		local a_state = PetData.Instance:GetPetState(a.index)
		local b_state = PetData.Instance:GetPetState(b.index)
        local aSort = PetStateSort[a_state]
        local bSort = PetStateSort[b_state]
        if aSort ~= bSort then
        	return aSort < bSort
        elseif a.index ~= b.index then
            return a.index < b.index
        else
        	return a.pet_id < b.pet_id
        end
    end
    table.sort(pet_list, sort_fun)
    self.SelectList:SetData(pet_list)
end

function PetHuntSelect:SetEmpty()
	self.is_empty = true
	self.NoPetChoseObj:SetActive(true)
end

function PetHuntSelect:OnClickItem(mono,toggle_on)
	if toggle_on then
		local data = mono:GetData()
		local pet = data.index < 0 and data or PetData.Instance:GetPetInfo(data.index)
		if pet:Weapon() or pet:Weapon2() then
			PublicPopupCtrl.Instance:Center(Language.TreasureHunt.PetHuntExchangeWeapon)
			return
		elseif pet:StampBlueprint():IsOk() then
			PublicPopupCtrl.Instance:Center(Language.TreasureHunt.PetHuntExchangeStamp)
			return
		end
		if pet.info.lock == 1 then
			local dialog_info = {
				content = Language.TreasureHunt.PetHuntExchangeLock,
				confirm = {func = function ()
					self.cur_data = data
					PetCtrl.SendPetOp(PET_OP_TYPE.Lock, data.index)
					ViewMgr:CloseView(DialogTipsView)
				end},
				cancel = {func = function ()
					mono.Toggle.isOn = false
					ViewMgr:CloseView(DialogTipsView)
				end}
			}
			PublicPopupCtrl.Instance:DialogTips(dialog_info)
		else
			self.cur_data = data
		end
	else
		self.cur_data = nil
	end
end

function PetHuntSelect:OnClickSelect()
	if not self.is_empty and self.cur_data ~= nil then
		if self.hunt_data.exchange_smart.clicked_index > 0 then
			self.hunt_data.exchange_smart.compose_pet_id = self.cur_data.pet_id
			self.hunt_data.exchange_smart.compose_index = self.cur_data.seq
		elseif self.cur_data.pet_id and self.cur_data.seq and self.cur_data.comsume_type and self.cur_data.index then
			local index = math.abs(self.hunt_data.exchange_smart.clicked_index)
			self.hunt_data.exchange_smart["comsume_type"..index] = self.cur_data.comsume_type
			self.hunt_data.exchange_smart["comsume_pet_id"..index] = self.cur_data.pet_id
			self.hunt_data.exchange_smart["comsume_index"..index] = self.cur_data.seq
			self.hunt_data.exchange_smart["comsume_type_index"..index] = self.cur_data.index
		end
	end
	ViewMgr:CloseView(PetHuntSelect)
end
-----------------------------
PetHuntSelectItem = PetHuntSelectItem or DeclareMono("PetHuntSelectItem",UIWidgetBaseItem)
function PetHuntSelectItem:PetHuntSelectItem()
	self.hunt_data = TreasureHuntData.Instance
	self.pet_data = PetData.Instance
end

function PetHuntSelectItem:FlushData()
	self:SetData(self.pet_data:GetPetInfo(self.data.index))
end

function PetHuntSelectItem:OnDestroy()
	self:uncareData()
end

function PetHuntSelectItem:SetData(data)
	if self.care_handle == nil then
		self.care_handle = self.pet_data.pet_list:KeysCare({data.index}, BindTool.Bind1(self.FlushData,self))
		self.care_handles[self.care_handle] = self.pet_data.pet_list
	end
	local pet = data.index < 0 and data or PetData.Instance:GetPetInfo(data.index)
	if self.hunt_data.exchange_smart.clicked_index < 0 then
		local sp_name = PetData.State[pet:State()]
		if not StringIsEmpty(sp_name) then
			UH.SpriteName(self.PetState, sp_name.."Big")
			self.PetStateObj:SetActive(true)
		else
			self.PetStateObj:SetActive(false)
		end
		UH.SetText(self.PetName, pet.config.name)
	else
		self.PetStateObj:SetActive(false)
		local cfg = self.pet_data:GetPetCfg(data.pet_id)
		if cfg and cfg.name then
			UH.SetText(self.PetName, cfg.name)
		else
			UH.SetText(self.PetName, "")
		end
	end
	self.LockObj:SetActive(pet.info.lock == 1)
	UH.SpriteName(self.QuaFrame, "pet_qua_"..pet.config.quality)
	UH.SpriteName(self.StrLevel, "PetNum"..pet.str_level)
	UH.SetText(self.Level, pet.level)

	UH.SetIcon(self.Icon, pet.config.icon_id)

	self.data = data
end

function PetHuntSelectItem:OnClickIcon()
	if self.data then
		local pet = self.data.index < 0 and self.data or PetData.Instance:GetPetInfo(self.data.index)
		ViewMgr:OpenView(PetDetailView, pet)
	end
end