DivineCostumeView = DivineCostumeView or DeclareView("DivineCostumeView", "divine_costume/divine_costume_view",Mod.DivineCostume.Main)
VIEW_DECLARE_MASK(DivineCostumeView, ViewMask.Block)
VIEW_DECLARE_LEVEL(DivineCostumeView, ViewLevel.L0)
function DivineCostumeView:DivineCostumeView()
	self.data = DivineCostumeData.Instance
	self.tab_handle = self.data.tab_index_data:Care(BindTool.Bind(self.FlushSelectToggle,self))
	self.remind_handle = self.data.red_flag:Care(BindTool.Bind(self.FlushRemind,self))
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
	self.is_play_open_audio = true
end

function DivineCostumeView:LoadCallback(param_t)
	self.data:SetSelectTab(DivineCostumeData.SELECT_TAB.EQUIPMENT)
	if self.data:GetIsHaveRedPointByType(2) or self.data:GetIsHaveRedPointByType(1) or self.data:GetIsHaveRedPointByType(3) then
		self.data.is_first_red = true
		if self.data:GetIsHaveRedPointByType(2) then
			if not self.data.is_have_show_red  then
				self.RedObjects[DivineCostumeData.SELECT_TAB.SPIRITUALISM]:SetNum(1)
			end
		end
		
		if self.data:GetIsHaveRedPointByType(3) then
			if not self.data.is_have_show_red  then
				self.RedObjects[DivineCostumeData.SELECT_TAB.CHANGEGOD]:SetNum(1)
			end
		end
	end
	
	if self.data:GetIsHaveResonanceRedPoint() then
		self.RedObjects[DivineCostumeData.SELECT_TAB.RESONANCE]:SetNum(1)
		self.ResonanceRed:SetNum(1)
	end

	if param_t ~= nil then 
		if param_t.item and param_t.item then 
			local flag = param_t.item:IsJewelry()
			local type = param_t.item:EquipType()
			self:OnTabClick(flag and 2 or 1)
			self.panel.now_select_equipment = not flag and type + 1 or 1
			self.panel.now_select_spiritualism = flag and type - 4 or 1
		end 
	end

	GuideManager.Instance:RegisterGetGuideUi("DivineConsumeGongMingBtn", function()
        return self.ToggleList[4], BindTool.Bind2(self.OnTabClick, self, 4)
    end)
end

function DivineCostumeView:OpenCallback()
	DivineCostumeCtrl.Instance:CheckGuideStart()
	-- 这个界面屏蔽右下角快速使用弹窗
	MainOtherCtrl.Instance.stop = true
end

function DivineCostumeView:CloseCallback()
	self.data.tab_index_data:Uncare(self.tab_handle)
	self.data.red_flag:Uncare(self.remind_handle)
	if self.data.is_first_red  then
		self.data.is_have_show_red  = true
		self.data.red_point.count = 0
	end
	GuideManager.Instance:UnRegisterGetGuideUi("DivineConsumeGongMingBtn")
	-- 这个界面屏蔽右下角快速使用弹窗
	MainOtherCtrl.Instance.stop = false
end

function DivineCostumeView:FlushRemind()
	if self.data:GetIsHaveRedPointByType(2) then
		if not self.data.is_have_show_red  then
			self.RedObjects[DivineCostumeData.SELECT_TAB.SPIRITUALISM]:SetNum(1)
		end
	else
		self.RedObjects[DivineCostumeData.SELECT_TAB.SPIRITUALISM]:SetNum(0)
	end
	
	if self.data:GetIsHaveRedPointByType(3) then
		if not self.data.is_have_show_red  then
			self.RedObjects[DivineCostumeData.SELECT_TAB.CHANGEGOD]:SetNum(1)
		end
	else
		self.RedObjects[DivineCostumeData.SELECT_TAB.CHANGEGOD]:SetNum(0)
	end
	
	if self.data:GetIsHaveResonanceRedPoint() then
		self.RedObjects[DivineCostumeData.SELECT_TAB.RESONANCE]:SetNum(1)
		self.ResonanceRed:SetNum(1)
	else
		self.RedObjects[DivineCostumeData.SELECT_TAB.RESONANCE]:SetNum(0)
		self.ResonanceRed:SetNum(0)
	end
end

function DivineCostumeView:FlushSelectToggle()
	local index = self.data.tab_index_data.index or DivineCostumeData.SELECT_TAB.EQUIPMENT
	if self.ToggleList[index].isOn == false then
		self.ToggleList[index].isOn = true
	end
	if index ~= DivineCostumeData.SELECT_TAB.RESONANCE then
		self.RedObjects[index]:SetNum(0)
	end
end

function DivineCostumeView:OnBackClick()
	ViewMgr:CloseView(DivineCostumeView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function DivineCostumeView:OnTabClick(index)
	self.data:SetSelectTab(index)
	if self.is_play_open_audio_ing == false then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
	else
		self.is_play_open_audio_ing = false
	end
end

function DivineCostumeView:OnclickResonanceTips()
	ViewMgr:OpenView(ResonanceShowView)
end

DivineCostumePanel = DivineCostumePanel or DeclareMono("DivineCostumePanel", UIWFlushPanel)
function DivineCostumePanel:DivineCostumePanel()
	self.data = DivineCostumeData.Instance
	self.data_cares = {
		{data = self.data.tab_index_data, func = self.FlushShowPanel, init_call = false},
		{data = BagData.Instance.item_grid, func = self.FreshAllPanel, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.EquipmentChange, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.SpiritualismChange, init_call = false},
		{data = self.data.resonace_flag, func = self.FreshResonance, init_call = false}
    }
	self.is_first_equipment = true
	self.is_first_spiritualism = true
	self.is_first_changegod = true
	self.now_select_equipment = 1
	self.now_select_spiritualism = 1
	self.now_select_changegod = 1
	self.is_tupo = false
	self.change_level_enough = false
	self.need_level = 0
	self.change_level = 0
	self.current_id = 0
	self.after_id = 0
	self.show_model_index = 0
	self.is_begin_model = false
	self.uptate_model_time = 20

	GuideManager.Instance:RegisterGetGuideUi("DivineConsumeList", function()
        return self.GuideEquipList, function() end
    end)
	GuideManager.Instance:RegisterGetGuideUi("DivineConsumeAttr", function()
        return self.GuideAttrBtn, function() end
    end)
	GuideManager.Instance:RegisterGetGuideUi("DivineConsumeGongMingAttr", function()
        return self.GuideGongMing, function() end
	end)
	
	self.ObjBlock:SetActive(false)
end 

function DivineCostumePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	if self.create_handle ~= nil then
        TimeHelper:CancelTimer(self.create_handle)
        self.create_handle = nil
    end 
	
	if self.active_handle ~= nil then
        TimeHelper:CancelTimer(self.active_handle)
        self.active_handle = nil
    end 
	
	if self.create_handle_q ~= nil then
		TimeHelper:CancelTimer(self.create_handle_q)
		self.create_handle_q = nil
	end 
	
	if self.create_handle_q_block ~= nil then
		TimeHelper:CancelTimer(self.create_handle_q_block)
		self.create_handle_q_block = nil
	end
	
	if self.time_func ~= nil then
		TimeHelper:CancelTimer(self.time_func)
		self.time_func = nil
	end
	
	if self.resonance_handle ~= nil then
        TimeHelper:CancelTimer(self.resonance_handle)
        self.resonance_handle = nil
    end 
	GuideManager.Instance:UnRegisterGetGuideUi("DivineConsumeList")
	GuideManager.Instance:UnRegisterGetGuideUi("DivineConsumeAttr")
	GuideManager.Instance:UnRegisterGetGuideUi("DivineConsumeGongMingAttr")
	
end

function DivineCostumePanel:FreshResonance()
	--LogError("FreshResonance")
	ViewMgr:OpenView(ResonanceSuccessView)
	self:FreshResonancePanel()
end

function DivineCostumePanel:FreshResonancePanel()
	self:FreshResonanceList()
	self:FreshResonanceLevel()
end

function DivineCostumePanel:FreshResonanceLevel()
	local current_level = self.data:GetResonaceFlagCount()
	UH.SetText(self.ResonanceLevel,string.format(Language.DivineCostume.Grade,tostring(current_level)))
	if current_level < DivineCostumeData.RESONACE_FULL then
		local contidition_progress = self.data:GetNextLevelCondition(current_level)
		UH.SetText(self.CurrentResonance,tostring(contidition_progress).."/8")
		self.ResonanceProgress:SetProgress(contidition_progress/8)	
		UH.SetText(self.ResonanceCondition,string.format(Language.DivineCostume.ResonanceCondition,self.data:GetNextResonanceLevel(current_level),current_level + 1))
		self.ResonanceNofullObj:SetActive(true)
		self.ResonancefullObj:SetActive(false)
	else
		UH.SetText(self.ResonanceCondition,Language.DivineCostume.ResonanceFull)
		self.ResonanceNofullObj:SetActive(false)
		self.ResonancefullObj:SetActive(true)
	end
	local resonancedata = self.data:GetAllResonanceData(current_level)
	if table.nums(resonancedata) == 0 then
		self.ResonanceTextObj:SetActive(true)
	else
		self.ResonanceTextObj:SetActive(false)
	end
	self.ResonanceDataList:SetData(resonancedata)
end

function DivineCostumePanel:FreshResonanceList()
	local show_list = DivineCostumeData.Instance:GetResonanceList()
	for k,v in pairs(show_list) do 
		self.ResonanceList[k]:SetData(v)
		self.ResonanceEffect[k]:SetActive(false)
	end
	if not self.is_begin_model then
		self.is_begin_model = true
		self:AutoToNext(true)
		self.time_func = Invoke(BindTool.Bind(self.AutoToNext, self), self.uptate_model_time)
	end
end

function DivineCostumePanel:AutoToNext(is_first)
    self.show_model_index = self.show_model_index + 1
	if self.show_model_index > 8 then
		self.show_model_index = 1
	end
	local cell_data
	local config
	local show_list = DivineCostumeData.Instance:GetResonanceList()
	for k,v in pairs(show_list) do 
		if k == self.show_model_index then
			cell_data = v
			if k > 5 then
				config = Config.godequip_up_auto.godjewelry_strengthen
			else
				config = Config.godequip_up_auto.godequip_strengthen
			end
			break
		end
	end
	if cell_data and config then
		self:FreshResonanceModel(cell_data,config)
	end
	
	if not is_first then
		TimeHelper:CancelTimer(self.time_func)
		self.time_func = Invoke(BindTool.Bind(self.AutoToNext, self), self.uptate_model_time)
	end
end

function DivineCostumePanel:FreshResonanceModel(cell_data,config)
	local show_current_id = DivineCostumeData.Instance:GetModelShowId(cell_data.item_id,cell_data.param.strengthen_level,config)
	local path = DrawerHelper.GetWeaponPath(show_current_id)
	if not self.ui_obj_resonance then
		self.ui_obj_resonance = UIChDrawer.New()
		self.ui_obj_resonance:SetMainSolo(path)
		self.ModelShowResonance:SetShowData({ui_obj = self.ui_obj_resonance, view = self})
	else
		 self.ui_obj_resonance:SetMainSolo(path)
	end
end

function DivineCostumePanel:FlushShowPanel()
	local show_type = true

	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.RESONANCE then
		show_type  =false
		self:FreshResonancePanel()
	end	
	self.yulanobj:SetActive(show_type)
	self.left_type_1:SetActive(show_type)
	self.left_type_2:SetActive(not show_type)
	self.mid_type_1:SetActive(show_type)
	self.mid_type_2:SetActive(not show_type)
	
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		if self.is_first_equipment then
			self:FreshEquipment()
			self.EquipmentToggle[self.now_select_equipment].isOn = true
			self.is_first_equipment =false
		else
			self:OnEquipmentClick(self.now_select_equipment,true)
		end
	end
	
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.SPIRITUALISM then
		if self.is_first_spiritualism then
			self:FreshSpiritualism()
			self.SpiritualismToggle[self.now_select_spiritualism].isOn = true
			self.is_first_spiritualism =false
		else
			self:OnSpiritualismClick(self.now_select_spiritualism,true)
		end
	end

	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.CHANGEGOD then
		if self.is_first_changegod then
			self:FreshChangegod()
			self.ChangeGodToggle[self.now_select_changegod].isOn = true
			self.is_first_changegod =false
		else
			self:OnChangeGodClick(self.now_select_changegod,true)
		end
	end	
	
	if self.cur_effect_dazao == nil then
		if self.EffectTool.isActiveAndEnabled then
			self.cur_effect_dazao = self.EffectTool:Play(100108211)
		end
	end
end 

function DivineCostumePanel:FreshAllPanel()
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		self:FreshEquipmentPanel()
	elseif self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.SPIRITUALISM then
		self:FreshSpiritualismPanel()
	elseif self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.CHANGEGOD then
		self:FreshChangegodPanel()
	end
end 

function DivineCostumePanel:EquipmentChange()
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		self:FreshChangegod()
		self:FreshEquipmentPanel()
		DivineCostumeData.Instance:FreshRedPointCount()
	else
		self:FreshEquipment()
		self:FreshChangegodPanel()
	end
end

function DivineCostumePanel:SpiritualismChange()
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.SPIRITUALISM then
		self:FreshChangegod()
		self:FreshSpiritualismPanel()
		DivineCostumeData.Instance:FreshRedPointCount()
	else
		self:FreshSpiritualism()
		self:FreshChangegodPanel()
	end
end

function DivineCostumePanel:FreshChangegodPanel()
	self:FreshChangegod()
	self:OnChangeGodClick(self.now_select_changegod,true)
end 

function DivineCostumePanel:FreshEquipmentPanel()
	self:FreshEquipment()
	self:OnEquipmentClick(self.now_select_equipment,true)
end 

function DivineCostumePanel:FreshSpiritualismPanel()
	self:FreshSpiritualism()
	self:OnSpiritualismClick(self.now_select_spiritualism,true)
end 

function DivineCostumePanel:FreshEquipment()
	local show_list = DivineCostumeData.Instance:GetEquipmentShowList()
    for i = 1, 5 do
        local vo = show_list[i]
        self.EquipmentList[i]:SetData(vo)
		if DivineCostumeData.Instance:GetIsHaveRedPoint(vo) then
			if not self.data.is_have_show_red  then
				self.RedPointObjects[i]:SetNum(1)
			end
		else
			self.RedPointObjects[i]:SetNum(0)
		end
    end 
end 

function DivineCostumePanel:FreshSpiritualism()
	local show_list = DivineCostumeData.Instance:GetSpiritualismList()
    for i = 1, 3 do
        local vo = show_list[i]
        self.SpiritualismList[i]:SetData(vo)
		if DivineCostumeData.Instance:GetIsHaveRedPoint(vo) then
			if not self.data.is_have_show_red  then
				self.SRedPointObjects[i]:SetNum(1)
			end
		else
			self.SRedPointObjects[i]:SetNum(0)
		end
    end 
end

function DivineCostumePanel:FreshChangegod()
	self:FreshChangegodEquipment()
	self:FreshChangegodSpiritualism()
end

function DivineCostumePanel:FreshChangegodEquipment()
	local show_list = DivineCostumeData.Instance:GetChangeGodList()
    for i = 1, 5 do
        local vo = show_list[i]
        self.ChangeGodList[i]:SetData(vo)
		if DivineCostumeData.Instance:GetIsHaveChangeRedPoint(vo) then
			if not self.data.is_have_show_red  then
				self.CRedPointObjects[i]:SetNum(1)
			end
		else
			self.CRedPointObjects[i]:SetNum(0)
		end
    end 
end

function DivineCostumePanel:FreshChangegodSpiritualism()
	local show_list = DivineCostumeData.Instance:GetChangeGodList()
    for i = 6, 8 do
        local vo = show_list[i]
        self.ChangeGodList[i]:SetData(vo)
		if DivineCostumeData.Instance:GetIsHaveChangeRedPoint(vo) then
			if not self.data.is_have_show_red  then
				self.CRedPointObjects[i]:SetNum(1)
			end
		else
			self.CRedPointObjects[i]:SetNum(0)
		end
    end 
end

function DivineCostumePanel:OnChangeGodClick(index,noclick)
	if not noclick then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	end
	self.CRedPointObjects[index]:SetNum(0)
	self.now_select_changegod = index
	if index < 6 then
		self:OnEquipmentClick(index,noclick,true)
	else
		index = index - 5
		self:OnSpiritualismClick(index,noclick,true)
	end
end

function DivineCostumePanel:OnEquipmentClick(index,noclick,is_change)
	if not noclick then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	end
	if not is_change then
		self.RedPointObjects[index]:SetNum(0)
		self.now_select_equipment = index
	end
	local show_list = DivineCostumeData.Instance:GetEquipmentShowList()
	local id = show_list[index].id
	local cell_data = DivineCostumeData.Instance:GetEquipmentData(id)
	local is_equip = DivineCostumeData.Instance:GetIsEquip(id,1)
	local config = Config.godequip_up_auto.godequip_strengthen
	local equip_type = DivineCostumeData.Instance:GetEquipType(id,1,config)
	self.data.now_select_equiptype = equip_type
	self.data.now_select_equiptype_strengthen_level = 0
	local is_show = true
	if equip_type == 2 then
		local role_id = RoleData.Instance:GetRoleId()
		if UnityPlayerPrefs.GetInt(role_id.."god_equip") == 0 then
			is_show = false
		end
	end
	local equip_list = BagData.Instance:GetEquipList(equip_type)
	if cell_data and is_show then
		self:FreshQiangHuaView(cell_data,1,is_change)
		self.QiangHuaObj:SetActive(true)
		self.DaZaoObj:SetActive(false)
		self.equip = cell_data
		self.ButtonCompare:SetActive(false)
		self.ButtonYuLan:SetActive(true)
		if not is_equip then
			if equip_list and equip_list.item_id ~=0 then
				self.ButtonCompare:SetActive(true)
				self.ButtonYuLan:SetActive(false)
			end
		end
		self.AttrInfo:SetActive(false)
		self.data.now_select_equiptype_strengthen_level = cell_data.param.strengthen_level
		self.data.now_select_equiptype_color = DivineCostumeData.Instance:GetColor(cell_data,config) 
	else
		self:FreshDaZaoView(id,1,is_change)
		self.QiangHuaObj:SetActive(false)
		self.DaZaoObj:SetActive(true)
		if equip_list and equip_list.item_id ~=0 then
			self.ButtonYuLan:SetActive(false)
			self.ButtonCompare:SetActive(true)
		else
			self.ButtonCompare:SetActive(false)
			self.ButtonYuLan:SetActive(true)
		end
		self.AttrInfo:SetActive(true)
	end
end 

function DivineCostumePanel:OnSpiritualismClick(index,noclick,is_change)
	if not noclick then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	end
	if not is_change then
		self.SRedPointObjects[index]:SetNum(0)
		self.now_select_spiritualism = index
	end
	local show_list = DivineCostumeData.Instance:GetSpiritualismList()
	local id = show_list[index].id
	local cell_data = DivineCostumeData.Instance:GetSpiritualismData(id)
	local config = Config.godequip_up_auto.godjewelry_strengthen
	local equip_type = DivineCostumeData.Instance:GetEquipType(id,1,config)
	local equip_list = BagData.Instance:GetJewelryList(equip_type - 5)
	local is_equip = DivineCostumeData.Instance:GetIsEquip(id,2)
	self.data.now_select_equiptype = equip_type
	self.data.now_select_equiptype_strengthen_level = 0
	if cell_data then
		self:FreshQiangHuaView(cell_data,2,is_change)
		self.QiangHuaObj:SetActive(true)
		self.DaZaoObj:SetActive(false)
		self.equip = cell_data
		self.ButtonCompare:SetActive(false)
		self.ButtonYuLan:SetActive(true)
		if not is_equip then
			if equip_list and equip_list.item_id ~=0 then
				self.ButtonCompare:SetActive(true)
				self.ButtonYuLan:SetActive(false)
			end
		end
		self.AttrInfo:SetActive(false)
		self.data.now_select_equiptype_strengthen_level = cell_data.param.strengthen_level
		self.data.now_select_equiptype_color = DivineCostumeData.Instance:GetColor(cell_data,config)
	else
		self:FreshDaZaoView(id,2,is_change)
		self.QiangHuaObj:SetActive(false)
		self.DaZaoObj:SetActive(true)
		if equip_list and equip_list.item_id ~=0 then
			self.ButtonYuLan:SetActive(false)
			self.ButtonCompare:SetActive(true)
		else
			self.ButtonCompare:SetActive(false)
			self.ButtonYuLan:SetActive(true)
		end
		self.AttrInfo:SetActive(true)
	end
end 

function DivineCostumePanel:FreshQiangHuaView(cell_data,t_type,is_change)
	local config 
	if t_type == 1 then
		config = Config.godequip_up_auto.godequip_strengthen
	else
		config = Config.godequip_up_auto.godjewelry_strengthen
	end	
	self.is_tupo = false
	self.change_level_enough = false
	local level,strengthen_id = DivineCostumeData.Instance:GetChangeLevelAndId(cell_data,config)
	local color = DivineCostumeData.Instance:GetColor(cell_data,config)
	self.QInteractorHandler.Interactable = true
	if is_change then
		if color == 7 then
			self.is_tupo = true
			if cell_data.param.strengthen_level >= level then
				self.change_level_enough = true
			end
			self.change_level = level
			self.QInteractorHandler.Interactable = self.change_level_enough
		end
	end
	self.strengthen_consume_item = {}
	local attr_list ,full = DivineCostumeData.Instance:GetAttrInfo(cell_data,config,is_change,level)
	for k,v in pairs(config) do 
		if not self.is_tupo then
			if v.id == cell_data.item_id and v.level == cell_data.param.strengthen_level + 1 then
				self.strengthen_consume_item = v.strengthen_consume_item
				break
			end
		else
			self.strengthen_consume_item = DivineCostumeData.Instance:GetChangeNeedItemsById(cell_data.item_id,config)
			break
		
		end
	end
	self.NoFullObj:SetActive(not full)
	self.FullObj:SetActive(full)
	self.NormalImage:SetActive(true)
	self.ChangeImage:SetActive(false)
	if full then
		if is_change then
			self.NormalImage:SetActive(false)
			self.ChangeImage:SetActive(true)
			self.gradechangeobj:SetActive(true)
			self.NoHaveObj:SetActive(false)
			self.ChangeObj:SetActive(true)
			self.FullImage1:SetActive(false)
			self.FullImage2:SetActive(false)
		else
			self.gradechangeobj:SetActive(false)
			self.NoHaveObj:SetActive(true)
			self.ChangeObj:SetActive(false)
			self.FullImage1:SetActive(true)
			self.FullImage2:SetActive(true)
		end
	end
	UH.LocalPosG(self.listpos, Vector3.New(0,0,0))
	UH.LocalPosG(self.addlistpos, Vector3.New(-115,0,0))
	UH.LocalPosG(self.addlistposafter, Vector3.New(115,0,0))
	UH.LocalPosG(self.addlistposimage, Vector3.New(0,0,0))
	if not full then
		for i = 1, 2 do
			local vo = attr_list[i]
			if vo then
				self.AttrAddList[i]:SetData(vo)
				self.AttrAddListObjects[i]:SetActive(true)
			else
				self.AttrAddListObjects[i]:SetActive(false)
				if is_change then
					UH.LocalPosG(self.addlistpos, Vector3.New(-115, -20,0))
					UH.LocalPosG(self.addlistposafter, Vector3.New(115, -20,0))
					UH.LocalPosG(self.addlistposimage, Vector3.New(0, -20,0))
				end
			end
		end 
	else
		for i = 1, 2 do
			local vo = attr_list[i]
			if vo then
				self.FullList[i]:SetData(vo)
				self.FullListObjects[i]:SetActive(true)
			else
				self.FullListObjects[i]:SetActive(false)
				if is_change then
					UH.LocalPosG(self.listpos, Vector3.New(0, -20,0))
				end
			end
		end 
	
	end
	DivineCostumeData.Instance.items_is_full = true
	DivineCostumeData.Instance.items_is_full_itemid = 0
	DivineCostumeData.Instance.items_is_full_itemid_num = 0
	self.QiangHuaNeedList:SetData(self.strengthen_consume_item)
	self.need_level = DivineCostumeData.Instance:GetNeedLevel(cell_data, config)
	self.show_tupo = false
	local c_grade = cell_data.param.strengthen_level
	local a_grade = c_grade
	if not self.is_tupo then
		a_grade = c_grade + 1
	else
		if c_grade < level then
			c_grade = level
			a_grade = c_grade
		end
	end
	local button_text = Language.DivineCostume.Strengthen
	if c_grade >= 5 and c_grade % 5 == 0 then
		button_text = Language.DivineCostume.Breakt
		self.show_tupo = true
	end
	UH.SpriteName(self.GradeImage,"bottom2")
	if color ~= 7 then
		UH.SpriteName(self.GradeImage,"bottom")
	end
	local grade_cfg = DivineCostumeData.Instance:GetGradeCfg(cell_data.item_id, color, c_grade)
	local t_level = grade_cfg.ordinary_equip or 0
	if t_level > 0 then
		UH.SetText(self.GradeTip,string.format(Language.DivineCostume.GradeTip, t_level))
	else
		UH.SetText(self.GradeTip,"")
	end

	local equip_level = DivineCostumeData.Instance:GetChangeShowLevel(cell_data)
	UH.SetText(self.GradeText, equip_level .. Language.Common.Level)

	UH.LocalPosG(self.GradeObjPos, Vector3.New(340, 50, 0))
	self.AGradeObj:SetActive(false)
	self.Nofullgrade:SetActive(true)
	self.Nofullgradechange:SetActive(false)
	self.ChangeTip:SetActive(false)
	UH.LocalPosG(self.NeedShowPos, Vector3.New(70, -70, 0))
	self.gradeobj:SetActive(true)
	self.cgradeobj:SetActive(false)
	UH.LocalScale(self.ContentRect,Vector3.New(1,1,1))
	UH.LocalPosG(self.ContentObj, Vector3.New(160, -260, 0))
	UH.LocalPosG(self.ButtonObj, Vector3.New(-70, 70, 0))
	UH.LocalPosG(self.CTextObj, Vector3.New(-140, -255, 0))
	UH.LocalScale(self.TextRect,Vector3.New(1,1,1))
	self.ShowitemObj:SetActive(false)
	self.NofullImage1:SetActive(true)
	self.NofullImage2:SetActive(true)
	if self.is_tupo then
		UH.LocalPosG(self.GradeObjPos, Vector3.New(0, -70, 0))
		UH.LocalPosG(self.AGradeObjPos, Vector3.New(450, -70, 0))
		self.AGradeObj:SetActive(false)
		UH.SetText(self.AGradeText,string.format(Language.DivineCostume.Grade,tostring(a_grade)))
		button_text = Language.DivineCostume.Change
		self.Nofullgrade:SetActive(false)
		self.Nofullgradechange:SetActive(true)
		self.ChangeTip:SetActive(true)
		UH.LocalPosG(self.NeedShowPos, Vector3.New(0, -80, 0))
		self.gradeobj:SetActive(false)
		self.cgradeobj:SetActive(true)
		UH.SetText(self.changegradetext1,string.format(Language.DivineCostume.ChangeGrade1,tostring(c_grade)))
		UH.SetText(self.changegradetext2,string.format(Language.DivineCostume.ChangeGrade2,tostring(c_grade)))
		self.NormalImage:SetActive(false)
		self.ChangeImage:SetActive(true)
		UH.LocalScale(self.ContentRect,Vector3.New(0.8,0.8,1))
		UH.LocalPosG(self.ContentObj, Vector3.New(120, -260, 0))
		UH.LocalPosG(self.ButtonObj, Vector3.New(-110, 70, 0))
		UH.LocalPosG(self.CTextObj, Vector3.New(-130, -255, 0))
		UH.LocalScale(self.TextRect,Vector3.New(0.8,0.8,1))
		local c_data,a_data = self:GetChangeItemData(cell_data.item_id,strengthen_id,c_grade)
		self.ItemCurrent:SetData(c_data)
		self.ItemChange:SetData(a_data)
		self.ShowitemObj:SetActive(true)
		self.NofullImage1:SetActive(false)
		self.NofullImage2:SetActive(false)
	end
	UH.SetText(self.cgrade,string.format(Language.DivineCostume.Grade,tostring(c_grade)))
	UH.SetText(self.agrade,string.format(Language.DivineCostume.Grade,tostring(a_grade)))
	UH.SetText(self.buttontext,button_text)
	self:FreshQiangHuaModel(cell_data,config,strengthen_id)
	if self.cur_effect_qianghuan_1 == nil then
		if self.EffectTool.isActiveAndEnabled then 
			self.cur_effect_qianghuan_1 = self.EffectTool:Play(100108212)
		end
	end
	
	if self.is_tupo then
		if self.cur_effect_qianghuan_2 == nil then
			if self.EffectToolQ.isActiveAndEnabled then
				self.cur_effect_qianghuan_2 = self.EffectToolQ:Play(100108212)
			end
		end
	end
end 

function DivineCostumePanel:GetChangeItemData(c_id,a_id,level)
	local c_data = nil
	local a_data = nil
	local index = self.now_select_changegod
	if index > 4 then
		c_data = DivineCostumeData.Instance:GetSpiritualismData(c_id)
		a_data = DivineCostumeData.Instance:GetSpiritualismData(a_id)
	else
		c_data = DivineCostumeData.Instance:GetEquipmentData(c_id)
		a_data = DivineCostumeData.Instance:GetEquipmentData(a_id)
	end
	if c_data == nil then
		c_data = Item.Create({item_id = c_id,param = self:EquipmentParam(c_id,level)})
	else
		if c_data.vo.param and c_data.vo.param.strengthen_level < level then
			c_data = Item.Create({item_id = c_id,param = self:EquipmentParam(c_id,level)})
		end
	end
	if a_data == nil then
		a_data = Item.Create({item_id = a_id,param = self:EquipmentParam(a_id,level)})
	end
	
	return c_data,a_data
end


function DivineCostumePanel:FreshQiangHuaModel(cell_data,config,strengthen_id)
	local show_current_id = DivineCostumeData.Instance:GetModelShowId(cell_data.item_id,cell_data.param.strengthen_level,config)
	if self.current_id ~= show_current_id then 
		local path = DrawerHelper.GetWeaponPath(show_current_id)
		if not self.ui_obj_current then
			self.ui_obj_current = UIChDrawer.New()
			self.ui_obj_current:SetMainSolo(path)
			self.ModelShowCurrent:SetShowData({ui_obj = self.ui_obj_current, view = self})
		else
			 self.ui_obj_current:SetMainSolo(path)
		end
	end
	self.ModelShowAfterObj:SetActive(false)
	UH.LocalPosG(self.ModelShowCurrentPos, Vector3.New(75, 45, 0))
	self.Arrow:SetActive(self.is_tupo)
	self.ButtonYuLan:SetActive(not self.is_tupo)
	self.current_id = show_current_id
	if self.is_tupo then
		local show_after_id = DivineCostumeData.Instance:GetModelShowId(strengthen_id,1,config)
		if self.after_id ~= show_after_id then 
			local path = DrawerHelper.GetWeaponPath(show_after_id)
			if not self.ui_obj_after then
				self.ui_obj_after = UIChDrawer.New()
				self.ui_obj_after:SetMainSolo(path)
				self.ModelShowAfter:SetShowData({ui_obj = self.ui_obj_after, view = self})
			else
				self.ui_obj_after:SetMainSolo(path)
			end
			self.after_id = show_after_id 
		end
		UH.LocalPosG(self.ModelShowCurrentPos, Vector3.New(-150, 45, 0))
		UH.LocalPosG(self.ModelShowAfterPos, Vector3.New(308, 45, 0))
		self.ModelShowAfterObj:SetActive(true)
	end
end

function DivineCostumePanel:OnClickQiangHua()
	if self.is_tupo and not self.change_level_enough then
		PublicPopupCtrl.Instance:Error(string.format(Language.DivineCostume.TipsItem4,self.change_level))
		return
	end
	
	local role_level = RoleData.Instance:GetRoleLevel()
	if not self.is_tupo and role_level < self.need_level then	
		PublicPopupCtrl.Instance:Error(string.format(Language.DivineCostume.TipsLevel,self.need_level))
		return
	end
	if not DivineCostumeData.Instance.items_is_full then
	    local str = Language.DivineCostume.TipsItem
		if self.show_tupo then
			str = Language.DivineCostume.TipsItem1
		end	
		if self.is_tupo then
			str = Language.DivineCostume.TipsItem5
		end
		PublicPopupCtrl.Instance:Error(str)
		local need_item =Item.Create({item_id = DivineCostumeData.Instance.items_is_full_itemid})
		need_item.vo.jump_need_num = DivineCostumeData.Instance.items_is_full_itemid_num
		MainOtherCtrl.Instance:GetWayView({item =need_item})
		return
	end
	local storagenum = BagData.Instance:GetStorageNumByItemId(self.equip.item_id)
	if storagenum > 0 then
		PublicPopupCtrl.Instance:Error(Language.DivineCostume.StorageTips)
		return
	end
	self:ShowQEffect()
end

function DivineCostumePanel:ShowQEffect()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
   	if self.cur_effect_show_q ~= nil then
		self.EffectTool:Stop(self.cur_effect_show_q)
		self.cur_effect_show_q = nil
	end
	self.QBlock:SetActive(true)
	self.cur_effect_show_q = self.EffectTool:Play(10010355)

    if self.create_handle_q ~= nil then
        TimeHelper:CancelTimer(self.create_handle_q)
        self.create_handle_q = nil
    end 
	
	for k,v in pairs(self.strengthen_consume_item) do 
		v.show_effect = true
	end
	self.QiangHuaNeedList:SetData(self.strengthen_consume_item)
	self.need_qblock = true
	if self.is_tupo then
		self.ObjBlock:SetActive(true)
	end
    self.create_handle_q = TimeHelper:AddDelayTimer(function()
        self.create_handle_q = nil 
		for k,v in pairs(self.strengthen_consume_item) do 
			v.show_effect = false
		end
		self.QiangHuaNeedList:SetData(self.strengthen_consume_item)
		--self.QBlock:SetActive(false)
		self.need_qblock = false
		WorkshopCtrl.Instance:SendOperateReq(
			WORLSHOP_OP.WORKSHOP_OP_GOD_STRONGTHEN,
			self.is_tupo and 2 or 1,
			self.equip:IsEquip() and 0 or 1,
			self.equip.in_bag == true and 1 or 0,
			self.equip.index
		)
		-- 神装化神才展示
		if self.is_tupo then
			ViewMgr:OpenView(DivineCostumeResultView, {equip_type = self.data.now_select_equiptype, level = self.data.now_select_equiptype_strengthen_level})
		end
		self.ObjBlock:SetActive(false)
	end , 3)
	
	if self.create_handle_q_block ~= nil then
		TimeHelper:CancelTimer(self.create_handle_q_block)
		self.create_handle_q_block = nil
	end 
			
	self.create_handle_q_block  = TimeHelper:AddDelayTimer(function()
		self.create_handle_q_block = nil
		self.QBlock:SetActive(false)
	end ,3)
end

function DivineCostumePanel:OnClickEffectShowQ()
	-- 化神不允许快速点
	if self.is_tupo then
		return
	end
	if self.cur_effect_show_q ~= nil then
		self.EffectTool:Stop(self.cur_effect_show_q)
		self.cur_effect_show_q = nil
	end

	if self.create_handle_q ~= nil then
		TimeHelper:CancelTimer(self.create_handle_q)
		self.create_handle_q = nil
	end 

	if self.create_handle_q_block ~= nil then
		TimeHelper:CancelTimer(self.create_handle_q_block)
		self.create_handle_q_block = nil
	end 	
	self.QBlock:SetActive(false)
	if self.need_qblock then
		for k,v in pairs(self.strengthen_consume_item) do 
			v.show_effect = false
		end
		self.QiangHuaNeedList:SetData(self.strengthen_consume_item)
		self.QBlock:SetActive(false)
		WorkshopCtrl.Instance:SendOperateReq(
			WORLSHOP_OP.WORKSHOP_OP_GOD_STRONGTHEN,
			self.is_tupo and 2 or 1,
			self.equip:IsEquip() and 0 or 1,
			self.equip.in_bag == true and 1 or 0,
			self.equip.index
		)
	end
end

function DivineCostumePanel:OnClickDaZao()
	if not DivineCostumeData.Instance.items_is_full then
		PublicPopupCtrl.Instance:Error(Language.DivineCostume.TipsItem2)
		local need_item =Item.Create({item_id = DivineCostumeData.Instance.items_is_full_itemid})
		need_item.vo.jump_need_num = DivineCostumeData.Instance.items_is_full_itemid_num
		MainOtherCtrl.Instance:GetWayView({item =need_item})
		return
	end
	if BagData.Instance:GetEmptyNumInBag() == 0 then
		PublicPopupCtrl.Instance:Center(ErrorInfo[110])
		return
	end
	self:ShowCreatingEffect()
end

function DivineCostumePanel:ShowCreatingEffect()
	AudioMgr:PlayEffect(AudioType.UI, "pet_egg1")
	self.effect_type = 1
   	if self.cur_effect_show ~= nil then
		self.EffectTool:Stop(self.cur_effect_show)
		self.cur_effect_show = nil
	end
	self.Block:SetActive(true)
	self.cur_effect_show = self.EffectTool:Play(100108210)

    if self.create_handle ~= nil then
        TimeHelper:CancelTimer(self.create_handle)
        self.create_handle = nil
    end 

	self.ObjBlock:SetActive(true)
    self.create_handle = TimeHelper:AddDelayTimer(function()
        self.create_handle = nil 
		self.Block:SetActive(false)
		WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ,self.seq,1)
		ViewMgr:OpenView(DivineCostumeResultView, {equip_type = self.data.now_select_equiptype, level = 1, is_active = true})
		self.ObjBlock:SetActive(false)
    end ,8)
end

function DivineCostumePanel:OnClickEffectShow()
	if self.cur_effect_show ~= nil then
		self.EffectTool:Stop(self.cur_effect_show)
		self.cur_effect_show = nil
	end
	if self.effect_type == 1 then		
		if self.create_handle ~= nil then
			TimeHelper:CancelTimer(self.create_handle)
			self.create_handle = nil
		end 
		self.Block:SetActive(false)
		WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ,self.seq,1)
	else
	    if self.active_handle ~= nil then
			TimeHelper:CancelTimer(self.active_handle)
			self.active_handle = nil
		end 
		self.Block:SetActive(false)
		local role_id = RoleData.Instance:GetRoleId()
		UnityPlayerPrefs.SetInt(role_id.."god_equip",1)
		self:FreshEquipmentPanel()
	end
end

function DivineCostumePanel:OnClickActive()
	if not self.is_have_weapon then
		PublicPopupCtrl.Instance:Error(Language.DivineCostume.TipsItem3)
	else
		self:ShowActiveEffect()
	end
end

function DivineCostumePanel:OnClickGet()
	ViewMgr:OpenView(AccRechargeView)
end

function DivineCostumePanel:ShowActiveEffect()
	self.effect_type = 2
   	if self.cur_effect_show ~= nil then
		self.EffectTool:Stop(self.cur_effect_show)
		self.cur_effect_show = nil
	end
	self.Block:SetActive(true)
	self.cur_effect_show = self.EffectTool:Play(100108210)

    if self.active_handle ~= nil then
        TimeHelper:CancelTimer(self.active_handle)
        self.active_handle = nil
    end 

	self.ObjBlock:SetActive(true)
    self.active_handle = TimeHelper:AddDelayTimer(function()
        self.active_handle = nil 
		self.Block:SetActive(false)
		local role_id = RoleData.Instance:GetRoleId()
		UnityPlayerPrefs.SetInt(role_id.."god_equip",1)
		self:FreshEquipmentPanel()
		ViewMgr:OpenView(DivineCostumeResultView, {equip_type = self.data.now_select_equiptype, level = 1, is_active = true})
		self.ObjBlock:SetActive(false)
    end ,8)
end

function DivineCostumePanel:FreshDaZaoView(id,t_type,is_change)	
	if self.cur_effect_dazao_active == nil then
		if self.EffectTool.isActiveAndEnabled then
			self.cur_effect_dazao_active = self.EffectTool:Play(10010354)
		end
	end
	local config
	if t_type == 1 then
		config = Config.godequip_up_auto.godequip_strengthen
	else
		config = Config.godequip_up_auto.godjewelry_strengthen
	end	
	
	if not is_change then
		self.ChangeNoHave:SetActive(false)
		if DivineCostumeData.Instance:GetEquipType(id,1,config) == 2 then
			self.is_have_weapon = false
			local cell_data = DivineCostumeData.Instance:GetEquipmentData(id)
			if cell_data then
				self.is_have_weapon = true
			end
			self.InteractorHandler.Interactable = self.is_have_weapon
			self.NoActive:SetActive(false)
			self.Active:SetActive(true)
			local Str = RichTextHelper.LinkStr(Language.DivineCostume.ClickGo, "_ul",COLORSTR.Yellow1)
			UH.SetText(self.RichText, Str)
			self.TextObj:SetActive(not self.is_have_weapon)
		else
			local godequip_consume_item = {}
			local godequip_consume = Config.compose_auto.godequip_consume
			for k,v in pairs(godequip_consume) do 
				if v.id == id then
					godequip_consume_item = v.godequip_consume_item
					self.seq = v.seq
					break
				end
			end
			DivineCostumeData.Instance.items_is_full = true
			DivineCostumeData.Instance.items_is_full_itemid = 0
			DivineCostumeData.Instance.items_is_full_itemid_num = 0
			self.DaZaoNeedList:SetData(godequip_consume_item)
			self.NoActive:SetActive(true)
			self.Active:SetActive(false)
		end
	else
		self.NoActive:SetActive(false)
		self.Active:SetActive(false)
		self.ChangeNoHave:SetActive(true)
	end

	
	for i = 1, 2 do
		local cell_data = {item_id = id ,param = {strengthen_level = i}}
		local attr_list = DivineCostumeData.Instance:GetAttrInfo(cell_data,config)
		local vo = {attr_list = attr_list,strengthen_level =i}
		self.PopAttrList[i]:SetData(vo)
	end
	
	self:FreshDaZaoModel(id,t_type)
end 

function DivineCostumePanel:FreshDaZaoModel(item_id,t_type)
	local config 
	if t_type == 1 then
		config = Config.godequip_up_auto.godequip_strengthen
	else
		config = Config.godequip_up_auto.godjewelry_strengthen
	end	
	local show_current_id = DivineCostumeData.Instance:GetModelShowId(item_id,1,config)
	local path = DrawerHelper.GetWeaponPathGray(show_current_id.."_gary")
	if not self.ui_obj_dazao then
        self.ui_obj_dazao = UIChDrawer.New()
        self.ui_obj_dazao:SetMainSolo(path)
		self.ModelShowDaZao:SetShowData({ui_obj = self.ui_obj_dazao, view = self})
	else
		self.ui_obj_dazao:SetMainSolo(path)
    end
end

function DivineCostumePanel:OnClickCompare()
	local id = 0
	local index  = 0
	local show_list = nil
	local data = nil
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		index = self.now_select_equipment
		show_list = DivineCostumeData.Instance:GetEquipmentShowList()
		id = show_list[index].id
	elseif self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.SPIRITUALISM then
		index = self.now_select_spiritualism
		show_list = DivineCostumeData.Instance:GetSpiritualismList()
		id = show_list[index].id
	else
		index = self.now_select_changegod
		show_list = DivineCostumeData.Instance:GetChangeGodList()
		id = show_list[index].id
	end
	local list = BagData.Instance:GetItemsByItemId(id)
	for k,v in pairs(list) do 
		if id == v.item_id then
			data = v
			break
		end
	end
	if data == nil then
		data = Item.Create({item_id = id,param = self:EquipmentParam(id)})
	end
	local info = {
		item_list = {data, data:Compare()}
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function DivineCostumePanel:OnClickYuLan()
	local data = nil 
	local index  = 0
	local show_list = nil
	local id = 0
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		index = self.now_select_equipment
		show_list = DivineCostumeData.Instance:GetEquipmentShowList()
		id = show_list[index].id
		data = DivineCostumeData.Instance:GetEquipmentData(id)
	elseif self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.SPIRITUALISM then
		index = self.now_select_spiritualism
		show_list = DivineCostumeData.Instance:GetSpiritualismList()
		id = show_list[index].id
		data = DivineCostumeData.Instance:GetSpiritualismData(id)
	else
		index = self.now_select_changegod
		show_list = DivineCostumeData.Instance:GetChangeGodList()
		id = show_list[index].id
		if index > 4 then
			data = DivineCostumeData.Instance:GetSpiritualismData(id)
		else
			data = DivineCostumeData.Instance:GetEquipmentData(id)
		end
	end
	if data == nil then
		data = Item.Create({item_id = id,param = self:EquipmentParam(id)})
	end
	local info = {
		item_list = {data}
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function DivineCostumePanel:OnClickDetail()
	ViewMgr:OpenView(DivineCostumePopupView)
end

function DivineCostumePanel:EquipmentParam(item_id,level)
	local item_type = ItemType.Jewelry
	local show_level = level or 0
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.EQUIPMENT then
		item_type = 1
	end
	if self.data.tab_index_data.index == DivineCostumeData.SELECT_TAB.CHANGEGOD and self.now_select_changegod <= 5 then
		item_type = 1
	end
	local attr_list = DivineCostumeData.Instance:GetBaseAttrByIdAndLevel(item_id,show_level,item_type)
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
	t.new_suit_effect_id = Item.GetConfig(item_id).suitgroup_id
	t.is_strong_create = 0
	t.strengthen_level = show_level
	t.star_level = 0
	return t
end

function DivineCostumePanel:OnClickTips()	
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[101].desc})
end

function DivineCostumePanel:OnClickTips()	
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[101].desc})
end

function DivineCostumePanel:OnClickResonance()	
	if self.data:GetIsHaveResonanceRedPoint() then
		if DivineCostumeData.Instance:IsEquipAllDivineCostume() then
			self:ShowResonanceEffect()
		else
			PublicPopupCtrl.Instance:Error(Language.DivineCostume.ResonanceNotFull)
		end
	else
		local current_level = self.data:GetResonaceFlagCount()
		local text = string.format(Language.DivineCostume.ResonanceCondition,self.data:GetNextResonanceLevel(current_level),current_level + 1)
		PublicPopupCtrl.Instance:Error(text)
	end
	--ViewMgr:OpenView(ResonanceSuccessView)
end

function DivineCostumePanel:ShowResonanceEffect()
	self.ResonanceBlock:SetActive(true)
	for i = 1,8 do
		self.ResonanceEffect[i]:SetActive(true)
	end
    if self.resonance_handle ~= nil then
        TimeHelper:CancelTimer(self.resonance_handle)
        self.resonance_handle = nil
    end 
    self.resonance_handle = TimeHelper:AddDelayTimer(function()
        self.resonance_handle = nil 
		self.ResonanceBlock:SetActive(false)
		for i = 1,8 do
			self.ResonanceEffect[i]:SetActive(false)
		end
		BagCtrl.Instance:SendEquipmentOperaReq({oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_RESONACE})
	end ,2.5)
end

function DivineCostumePanel:OnClickResonanceEffectShow()
	if self.resonance_handle ~= nil then
		TimeHelper:CancelTimer(self.resonance_handle)
		self.resonance_handle = nil
	end 
	self.ResonanceBlock:SetActive(false)
	for i = 1,8 do
		self.ResonanceEffect[i]:SetActive(false)
	end
	BagCtrl.Instance:SendEquipmentOperaReq({oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_RESONACE})
end


DivineCostumeNeedListItem = DivineCostumeNeedListItem or DeclareMono("DivineCostumeNeedListItem", UIWidgetBaseItem)
function DivineCostumeNeedListItem:DivineCostumeNeedListItem()

end

function DivineCostumeNeedListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.New()
	item_data.item_id = data.item_id
	local have_item = BagData.Instance:GetNumByItemId(data.item_id)
	local need_num = 0
	if have_item < data.num then
		need_num = data.num - have_item
	end
	item_data.vo.jump_need_num = need_num
	local color = have_item >= data.num and COLORSTR.White or COLORSTR.Red8
	item_data.num = string.format(Language.Common.Cstr,color, have_item, data.num)
	self.Cell:SetData(item_data)
	if data.show_effect then
		if self.Qeffect then
			self.Qeffect:SetActive(true)
		end
	else
		if self.Qeffect then
			self.Qeffect:SetActive(false)
		end
	end
	

	if have_item < data.num then
		DivineCostumeData.Instance.items_is_full = false
		if DivineCostumeData.Instance.items_is_full_itemid == 0 then
			DivineCostumeData.Instance.items_is_full_itemid = data.item_id
			DivineCostumeData.Instance.items_is_full_itemid_num = need_num
		end
	end
	-- CellFlushs.SetNum(self.Cell,string.format(Language.Common.Cstr,color, have_item, data.num))
end


DivineCostumeShowCell = DivineCostumeShowCell or DeclareMono("DivineCostumeShowCell", UIWidgetBaseItem)
function DivineCostumeShowCell:DivineCostumeShowCell()

end 

function DivineCostumeShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	local cell_data
	local config
	local color = 7
	if data.equip_type > 4 then
		cell_data = DivineCostumeData.Instance:GetSpiritualismData(data.id)
		config = Config.godequip_up_auto.godjewelry_strengthen
	else
		cell_data = DivineCostumeData.Instance:GetEquipmentData(data.id)
		config = Config.godequip_up_auto.godequip_strengthen
	end
	local co = Item.GetConfig(data.id)
	local icon_id = co.icon_id
	local name = co.name
	local is_have = false
	local grade = 1	
	if cell_data and cell_data.param then
		is_have = true
		grade = cell_data.param.strengthen_level
		color = DivineCostumeData.Instance:GetColor(cell_data,config)
	end
	UH.SetIcon(self.icon,icon_id)
	UH.SetText(self.name,name)
	UH.SetText(self.qname,name)
	UH.Color(self.qname,QualityColor[color])
	if color == 7 then
		UH.SpriteName(self.ImageSet,"XingYaoWuPinDi")
	else
		UH.SpriteName(self.ImageSet,"ShenHuaWuPinDi")
	end
	local is_show = true
	if data.equip_type == 2 then
		local role_id = RoleData.Instance:GetRoleId()
		if UnityPlayerPrefs.GetInt(role_id.."god_equip") == 0 then
			-- 强修正 武器存在强化数据的场合下强制设置为已激活
			-- 2022-5-31-R
			if grade > 1 then 
				UnityPlayerPrefs.SetInt(role_id.."god_equip",1)
			else 
				is_show = false
			end 
		end
	end

	if is_have and is_show then
		self.ShowObjs[1]:SetActive(true)
		self.ShowObjs[2]:SetActive(false)
		if color == 7 then
			UH.SetText(self.grade,string.format(Language.DivineCostume.ChangeGrade1,tostring(grade)))
		else
			UH.SetText(self.grade,string.format(Language.DivineCostume.ChangeGrade2,tostring(grade)))
		end
	else
		self.ShowObjs[1]:SetActive(false)
		self.ShowObjs[2]:SetActive(true)
	end
end 

DivineCostumeAttrItem = DivineCostumeAttrItem or DeclareMono("DivineCostumeAttrItem", UIWidgetBaseItem)
function DivineCostumeAttrItem:DivineCostumeAttrItem()

end

function DivineCostumeAttrItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local arrt_name = Language.Common.AttrList[data.addtype]
	local num = DataHelper.IsPercent(data.addtype) and Percent(data.addnum) or data.addnum
	if num then
		UH.SetText(self.current,arrt_name.."    "..num)
	end
	if self.after then
		num = DataHelper.IsPercent(data.addtype) and Percent(data.addnum2) or data.addnum2
		if num then
			UH.SetText(self.after,arrt_name.."    "..num)
		end
	end
end

DivineCostumePopupAttrItem = DivineCostumePopupAttrItem or DeclareMono("DivineCostumePopupAttrItem", UIWidgetBaseItem)
function DivineCostumePopupAttrItem:DivineCostumePopupAttrItem()

end

function DivineCostumePopupAttrItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local attr_one = self.data.attr_list[1]
	local arrt_name = Language.Common.AttrList[attr_one.addtype]
	local num = DataHelper.IsPercent(attr_one.addtype) and Percent(attr_one.addnum) or attr_one.addnum
	UH.SetText(self.attr1,arrt_name.."+"..num)
	UH.SetText(self.attr2,"")
	local attr_two = self.data.attr_list[2]
	if attr_two then
		local arrt_name = Language.Common.AttrList[attr_two.addtype]
		local num = DataHelper.IsPercent(attr_two.addtype) and Percent(attr_two.addnum) or attr_two.addnum
		UH.SetText(self.attr2,arrt_name.."+"..num)
	end
	UH.SetText(self.grade,string.format(Language.DivineCostume.GradeDetail,tostring(self.data.strengthen_level)))
end

ResonanceShowCell = ResonanceShowCell or DeclareMono("ResonanceShowCell", UIWidgetBaseItem)

function ResonanceShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    local with_data = data.item_id > 0 
    if with_data then 
        local str = data:Level()..Language.Common.Level
        if data:Color() == ItemColor.Star or data:Color() == ItemColor.God then
            str = data:StrengthenLevel()..Language.Common.StrengthenLevel
        end
        self.level.text = str
    end 
end 


ResonanceItem = ResonanceItem or DeclareMono("ResonanceItem", UIWidgetBaseItem)

function ResonanceItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	local index = data.index % 2
	local bg_name = "ShuXingTiao1"
	if index == 0 then
		bg_name = "ShuXingTiao2"
	end
	UH.SpriteName(self.Image,bg_name)
	if data.attr_type ~= -1 then
		local attr_name = Language.Common.AttrList[data.attr_type]
		local num = DataHelper.IsPercent(data.attr_type) and Percent(data.attr_add) or data.attr_add
		UH.SetText(self.Name,attr_name)
		UH.SetText(self.Value,tostring(num))
		self.ShowValue:SetActive(true)
		self.Title:SetActive(false)
		UH.SizeDelta(self.ItemRect,Vector2.New(287, 30))
		UH.SizeDelta(self.ImageRect,Vector2.New(287, 30))
	else
		local show_text
		if data.attr_add == 0 then
			show_text = string.format(Language.DivineCostume.ResonanceCurrent,DivineCostumeData.Instance:GetResonaceFlagCount())
		else
			show_text = Language.DivineCostume.ResonanceNext
		end
		self.ShowValue:SetActive(false)
		self.Title:SetActive(true)
		UH.SetText(self.TitleText,show_text)
		UH.SizeDelta(self.ItemRect,Vector2.New(287, 50))
		UH.SizeDelta(self.ImageRect,Vector2.New(287, 50))
	end
end 



