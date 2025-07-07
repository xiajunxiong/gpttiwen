AttributeDanView = AttributeDanView or DeclareView("AttributeDanView", "attributedan/attribute_dan")
function AttributeDanView:AttributeDanView()
    self.lang = Language.AttributeDan
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
end

function AttributeDanView:CloseCallback()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function AttributeDanView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.TiTle, Vector2.New(968, 606))
end

AttributeDanPanel = AttributeDanPanel or DeclareMono("AttributeDanPanel", UIWFlushPanel)
function AttributeDanPanel:AttributeDanPanel()
    self.data = AttributeDanData.Instance
	self.lang = Language.AttributeDan
	local battle_attr = RoleData.Instance:AllBattleAttri()
	self.data_cares = {
        {data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.FlushAttributeDanView, init_call = true},
		{data = battle_attr, func = self.BaseAttr},
		{data = self.data.use_list_change, func = self.BaseAttr},
		{data = PetData.Instance.pet_select, func = self.BaseAttr, keys = {"index"}, init_call = false}
    }
	self.item_num_show = 0
	self.item_num_max = 0
	self.item_data = nil
	self.value_param3 = 0
end

function AttributeDanPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_func_reduce)
    TimeHelper:CancelTimer(self.time_func_add)
end

function AttributeDanPanel:BaseAttr()
	if self.data.now_attributedan_type == 1 then 
		self:RoleBaseAttr()
		local role_id = RoleData.Instance:GetRoleId()
		if UnityPlayerPrefs.GetInt(role_id.."role_attribute_dan") == 1 then
			self.IsSendMessage.isOn  = false
		end
	end
	
	if self.data.now_attributedan_type == 2 then 
		local pet = PetData.Instance:GetCurSelect()
		if pet then
			local role_id = RoleData.Instance:GetRoleId()
			local red_sign = UnityPlayerPrefs.GetInt(role_id.."pet_attribute_dan"..pet.index) 
			if red_sign == 0 then
				local state = pet:State()
				if state ~= PetState.Fight and state ~= PetState.StandBy then
					self.IsSendMessage.isOn  = false
				end
			end			
			if red_sign == 1 then
				self.IsSendMessage.isOn  = false
			end
			if red_sign == 2 then
				self.IsSendMessage.isOn  = true
			end
		end

		self:PetBaseAttr()
	end
	
	if self.data.now_attributedan_type == 3 then 
		local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(self.data.cur_sel_partner)
		if data then
			local role_id = RoleData.Instance:GetRoleId()
			local red_sign = UnityPlayerPrefs.GetInt(role_id.."partner_attribute_dan"..self.data.cur_sel_partner)
			if red_sign == 0 then
				if data.vo.fight_state ~= 1 then
					self.IsSendMessage.isOn  = false
				end
			end
			if red_sign == 1 then
				self.IsSendMessage.isOn  = false
			end
			if red_sign == 2 then
				self.IsSendMessage.isOn  = true
			end
			
		end
		self:PartnerBaseAttr()
	end
	
	if self.item_data then
		self:ItemNumMaxChange(self.item_data)
	end
end

function AttributeDanPanel:PetBaseAttr()
	local pet = PetData.Instance:GetCurSelect()
	if pet == nil then
        return
    end
	self.value_param3 = PetData.Instance:GetSelectPetIndex()
	local total_attr = PetData.Instance:RealAttr()
	local Hptext 
	local Mptext 
	local Atktext 
	local Deftext 
	local Agilitytext 
	local Recoverytext 
	local Mentaltext 
	for k, v in pairs(total_attr) do
        if tostring(k) == "maxhp" then
			Hptext = tostring(v)
        elseif tostring(k) == "maxmp" then
			Mptext = tostring(v)
        elseif tostring(k) == "gongji" then
			Atktext = tostring(v)
		elseif tostring(k) == "fangyu" then
			Deftext = tostring(v)
		elseif tostring(k) == "minjie" then
			Agilitytext = tostring(v)
		elseif tostring(k) == "jingshen" then
			Mentaltext = tostring(v)
		elseif tostring(k) == "huifu" then
			Recoverytext = tostring(v)
        end
    end	
	self.Hp.text = Hptext
	self.Mp.text = Mptext
	self.Atk.text = Atktext
	self.Def.text = Deftext
	self.Agility.text = Agilitytext
	self.Mental.text = Mentaltext
	self.Recovery.text = Recoverytext
	
	if self.item_data then	
		local attr_value = self.data:GetAttibuteDanByItemId(self.item_data.attribute_pellet_id).attr_value
		local add_value = attr_value * self.item_num_show
		if add_value > 0 then
			if self.item_data.attr_type == 0 then
				self.Atk.text = string.format(self.lang.AttrStr2,Atktext,add_value)
			end
			if self.item_data.attr_type == 1 then
				self.Def.text = string.format(self.lang.AttrStr2,Deftext,add_value)
			end
			if self.item_data.attr_type == 2 then
				self.Agility.text = string.format(self.lang.AttrStr2,Agilitytext,add_value)
			end
			if self.item_data.attr_type == 3 then
				self.Recovery.text = string.format(self.lang.AttrStr2,Recoverytext,add_value)
			end
			if self.item_data.attr_type == 4 then
				self.Mental.text = string.format(self.lang.AttrStr2,Mentaltext,add_value)
			end
			if self.item_data.attr_type == 6 then
				self.Hp.text = string.format(self.lang.AttrStr2,Hptext,add_value)
			end
			if self.item_data.attr_type == 8 then
				self.Mp.text = string.format(self.lang.AttrStr2,Mptext,add_value)
			end
		end
	end

	local limit_count = self.data:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = self.data:GetPetAttibuteDanUseInfo(i)
		local color = COLORS.Yellow13
		if use_count >= limit_count then
			color = COLORS.Red12
		end
		if i == AttributeDanType.Attack then
			self.AtkUse.text = use_count.."/"..limit_count
			UH.Color(self.AtkUse,color)
		end
		
		if i == AttributeDanType.Defense then
			self.DefUse.text = use_count.."/"..limit_count
			UH.Color(self.DefUse,color)
		end
		
		if i == AttributeDanType.Agility then
			self.AgilityUse.text = use_count.."/"..limit_count
			UH.Color(self.AgilityUse,color)
		end
		
		if i == AttributeDanType.Recovery then
			self.RecoveryUse.text = use_count.."/"..limit_count
			UH.Color(self.RecoveryUse,color)
		end
		
		if i == AttributeDanType.Mental then
			self.MentalUse.text = use_count.."/"..limit_count
			UH.Color(self.MentalUse,color)
		end
		
		if i == AttributeDanType.MaxHp then
			self.HpUse.text = use_count.."/"..limit_count
			UH.Color(self.HpUse,color)
		end
		
		if i == AttributeDanType.MaxMp then
			self.MpUse.text = use_count.."/"..limit_count
			UH.Color(self.MpUse,color)
		end
	end
	
end

function AttributeDanPanel:PartnerBaseAttr()
	self.value_param3 = self.data.cur_sel_partner
	local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(self.data.cur_sel_partner)
	local attrlist = data:GetAttrList()
	local Hptext = attrlist[6]
	local Mptext = attrlist[8]
	local Atktext = attrlist[0]
	local Deftext = attrlist[1]
	local Agilitytext = attrlist[2]
	local Recoverytext = attrlist[3]
	local Mentaltext = attrlist[4]
	
	self.Hp.text = Hptext
	self.Mp.text = Mptext
	self.Atk.text = Atktext
	self.Def.text = Deftext
	self.Agility.text = Agilitytext
	self.Mental.text = Mentaltext
	self.Recovery.text = Recoverytext
	
	if self.item_data  then
		local attr_value = self.data:GetAttibuteDanByItemId(self.item_data.attribute_pellet_id,self.data.cur_sel_partner).attr_value
		local add_value = attr_value * self.item_num_show
		if add_value > 0 then
			if self.item_data.attr_type == 0 then
				self.Atk.text = string.format(self.lang.AttrStr2,Atktext,add_value)
			end
			if self.item_data.attr_type == 1 then
				self.Def.text = string.format(self.lang.AttrStr2,Deftext,add_value)
			end
			if self.item_data.attr_type == 2 then
				self.Agility.text = string.format(self.lang.AttrStr2,Agilitytext,add_value)
			end
			if self.item_data.attr_type == 3 then
				self.Recovery.text = string.format(self.lang.AttrStr2,Recoverytext,add_value)
			end
			if self.item_data.attr_type == 4 then
				self.Mental.text = string.format(self.lang.AttrStr2,Mentaltext,add_value)
			end
			if self.item_data.attr_type == 6 then
				self.Hp.text = string.format(self.lang.AttrStr2,Hptext,add_value)
			end
			if self.item_data.attr_type == 8 then
				self.Mp.text = string.format(self.lang.AttrStr2,Mptext,add_value)
			end
		end
	end
	
	local limit_count = self.data:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = self.data:GetPartnerAttibuteDanUseInfo(self.data.cur_sel_partner,i)
		local color = COLORS.Yellow13
		if use_count >= limit_count then
			color = COLORS.Red12
		end
		if i == AttributeDanType.Attack then
			self.AtkUse.text = use_count.."/"..limit_count
			UH.Color(self.AtkUse,color)
		end
		
		if i == AttributeDanType.Defense then
			self.DefUse.text = use_count.."/"..limit_count
			UH.Color(self.DefUse,color)
		end
		
		if i == AttributeDanType.Agility then
			self.AgilityUse.text = use_count.."/"..limit_count
			UH.Color(self.AgilityUse,color)
		end
		
		if i == AttributeDanType.Recovery then
			self.RecoveryUse.text = use_count.."/"..limit_count
			UH.Color(self.RecoveryUse,color)
		end
		
		if i == AttributeDanType.Mental then
			self.MentalUse.text = use_count.."/"..limit_count
			UH.Color(self.MentalUse,color)
		end
		
		if i == AttributeDanType.MaxHp then
			self.HpUse.text = use_count.."/"..limit_count
			UH.Color(self.HpUse,color)
		end
		
		if i == AttributeDanType.MaxMp then
			self.MpUse.text = use_count.."/"..limit_count
			UH.Color(self.MpUse,color)
		end
	end

end

function AttributeDanPanel:RoleBaseAttr()
	self.value_param3 = 0
	local data = RoleData.Instance
	local Hptext = tostring(data:GetBattleAttri(BattleAttriType.MaxHp))
	local Mptext = tostring(data:GetBattleAttri(BattleAttriType.MaxMp))
	local Atktext = tostring(data:GetBattleAttri(BattleAttriType.Attack))
	local Deftext = tostring(data:GetBattleAttri(BattleAttriType.Defense))
	local Agilitytext = tostring(data:GetBattleAttri(BattleAttriType.Agility))
	local Mentaltext = tostring(data:GetBattleAttri(BattleAttriType.Mental))
	local Recoverytext = tostring(data:GetBattleAttri(BattleAttriType.Recovery))	
	
	self.Hp.text = Hptext
	self.Mp.text = Mptext
	self.Atk.text = Atktext
	self.Def.text = Deftext
	self.Agility.text = Agilitytext
	self.Mental.text = Mentaltext
	self.Recovery.text = Recoverytext

	if self.item_data then
		local attr_value = self.data:GetAttibuteDanByItemId(self.item_data.attribute_pellet_id).attr_value
		local add_value = attr_value * self.item_num_show
		if add_value > 0 then
			if self.item_data.attr_type == 0 then
				self.Atk.text = string.format(self.lang.AttrStr2,Atktext,add_value)
			end
			if self.item_data.attr_type == 1 then
				self.Def.text = string.format(self.lang.AttrStr2,Deftext,add_value)
			end
			if self.item_data.attr_type == 2 then
				self.Agility.text = string.format(self.lang.AttrStr2,Agilitytext,add_value)
			end
			if self.item_data.attr_type == 3 then
				self.Recovery.text = string.format(self.lang.AttrStr2,Recoverytext,add_value)
			end
			if self.item_data.attr_type == 4 then
				self.Mental.text = string.format(self.lang.AttrStr2,Mentaltext,add_value)
			end
			if self.item_data.attr_type == 6 then
				self.Hp.text = string.format(self.lang.AttrStr2,Hptext,add_value)
			end
			if self.item_data.attr_type == 8 then
				self.Mp.text = string.format(self.lang.AttrStr2,Mptext,add_value)
			end
		end
	end
	local limit_count = self.data:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = self.data:GetRoleAttibuteDanUseInfo(i)
		local color = COLORS.Yellow13
		if use_count >= limit_count then
			color = COLORS.Red12
		end
		if i == AttributeDanType.Attack then
			self.AtkUse.text = use_count.."/"..limit_count
			UH.Color(self.AtkUse,color)
		end
		
		if i == AttributeDanType.Defense then
			self.DefUse.text = use_count.."/"..limit_count
			UH.Color(self.DefUse,color)
		end
		
		if i == AttributeDanType.Agility then
			self.AgilityUse.text = use_count.."/"..limit_count
			UH.Color(self.AgilityUse,color)
		end
		
		if i == AttributeDanType.Recovery then
			self.RecoveryUse.text = use_count.."/"..limit_count
			UH.Color(self.RecoveryUse,color)
		end
		
		if i == AttributeDanType.Mental then
			self.MentalUse.text = use_count.."/"..limit_count
			UH.Color(self.MentalUse,color)
		end
		
		if i == AttributeDanType.MaxHp then
			self.HpUse.text = use_count.."/"..limit_count
			UH.Color(self.HpUse,color)
		end
		
		if i == AttributeDanType.MaxMp then
			self.MpUse.text = use_count.."/"..limit_count
			UH.Color(self.MpUse,color)
		end
	end
end


function AttributeDanPanel:FlushAttributeDanView()
	local attributedan_type = self.data.now_attributedan_type
	local list = self.data:GetAttibuteDanAllByType(attributedan_type)
	--table.sort(list, DataSorter.KeyLowerSorter("attribute_pellet_id"))
	self.ItemList:SetData(list)
	if self.item_data then
		for k,v in pairs(list) do 
			if v.attribute_pellet_id == self.item_data.attribute_pellet_id then
				self:OnClickAttributeDanItem(v)
			end
		end
	end
end

function AttributeDanPanel:ItemNumMaxChange(data)
	self.item_num_max = data.num
	local limit_count = self.data:GetAttibuteDanLimit()
	local m_type = self.data:ChangeConfigTypeToUseType(data.attr_type)
	local use_count = self.data:GetAttibuteDanUseInfo(m_type,self.value_param3)
	local can_add = limit_count - use_count
	if can_add < 0 then
		can_add = 0
	end
	if can_add > data.num then
		can_add = data.num
	end
	self.item_num_max = can_add
end

function AttributeDanPanel:OnClickAttributeDanItem(data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.CountObj:SetActive(data.num > 0)
	local show_num = 0
	if data.num > 0 then
		show_num = 1
	end
	UH.SetText(self.ItemNum,show_num)
	self.item_num_show = show_num
	--self:ItemNumMaxChange(data)
	self.item_data = data
	self:BaseAttr()
end

function AttributeDanPanel:OnClickUse()
	if self.item_data == nil then
		PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error4)
		return
	end
	if self.item_num_show <= 0 then
		PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error3)
		MainOtherCtrl.Instance:GetWayViewById(self.item_data.attribute_pellet_id)
		return
	end
	local can_use, use_level = self.data:GetCanUse()
	if not can_use then
		PublicPopupCtrl.Instance:Error(Format(Language.AttributeDan.Error6, use_level))
		return
	end

	local m_type = self.data:ChangeConfigTypeToUseType(self.item_data.attr_type)
	local use_count = self.data:GetAttibuteDanUseInfo(m_type,self.value_param3)
	local all_count = self.item_num_show + use_count
	local limit_count = self.data:GetAttibuteDanLimit()
	if all_count > limit_count then
		PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error5)
	else
		local op_type = self.data.now_attributedan_type
		local param1 = self.item_data.attribute_pellet_id
		local param2 = self.item_num_show
		local param3 = self.value_param3
		AttributeDanCtrl.Instance:SendRoleAttrPelletReq(op_type,param1,param2,param3)
	end
end

function AttributeDanPanel:OnClickZongLan()
	ViewMgr:OpenView(AttributeZongLanView)
end

function AttributeDanPanel:OnClickReduce()
	if self.item_num_show <= 1 then
		PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error2)	
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		self.item_num_show = self.item_num_show - 1
		UH.SetText(self.ItemNum,self.item_num_show)
		self:BaseAttr()
	end
end

function AttributeDanPanel:OnPressReduceDown()
    TimeHelper:CancelTimer(self.time_func_reduce)
	local time = 0
    self.time_func_reduce =
        TimeHelper:AddRunTimer(
        function()
			time = time + 0.1
			if time > 0.3 then
				if self.item_num_show <= 1 then
					--PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error2)
				else
					self.item_num_show = self.item_num_show - 1
					UH.SetText(self.ItemNum,self.item_num_show)
					self:BaseAttr()		
				end
			end
        end,
        0.1
    )
end

function AttributeDanPanel:OnPressReduceUp()
    TimeHelper:CancelTimer(self.time_func_reduce)
end

function AttributeDanPanel:OnClickAdd()
	if self.item_num_show + 1 > self.item_num_max then
		PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error1)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		self.item_num_show = self.item_num_show + 1
		UH.SetText(self.ItemNum,self.item_num_show)
		self:BaseAttr()
	end
end

function AttributeDanPanel:OnPressAddDown()
    TimeHelper:CancelTimer(self.time_func_add)
	local time = 0
    self.time_func_add =
        TimeHelper:AddRunTimer(
        function()
			time = time + 0.1
			if time > 0.3 then
				if self.item_num_show + 1 > self.item_num_max then
					--PublicPopupCtrl.Instance:Error(Language.AttributeDan.Error1)
				else
					self.item_num_show = self.item_num_show + 1
					UH.SetText(self.ItemNum,self.item_num_show)
					self:BaseAttr()
				end
			end
        end,
        0.1
    )
end

function AttributeDanPanel:OnPressAddUp()
    TimeHelper:CancelTimer(self.time_func_add)
end



function AttributeDanPanel:ToggleChange()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	if self.data.now_attributedan_type == 1 then 
		local role_id = RoleData.Instance:GetRoleId()
		if self.IsSendMessage.isOn then
			UnityPlayerPrefs.SetInt(role_id.."role_attribute_dan",0)
		else
			UnityPlayerPrefs.SetInt(role_id.."role_attribute_dan",1)
		end
		self.data:RemindBagDataRole()
	end
	
	if self.data.now_attributedan_type == 2 then
		local pet = PetData.Instance:GetCurSelect()
		local role_id = RoleData.Instance:GetRoleId()
		if self.IsSendMessage.isOn then
			UnityPlayerPrefs.SetInt(role_id.."pet_attribute_dan"..pet.index,2)
		else
			UnityPlayerPrefs.SetInt(role_id.."pet_attribute_dan"..pet.index,1)
		end
		self.data:RemindPetDataRole()
	end
	
	
	if self.data.now_attributedan_type == 3 then
		local role_id = RoleData.Instance:GetRoleId()
		if self.IsSendMessage.isOn then
			UnityPlayerPrefs.SetInt(role_id.."partner_attribute_dan"..self.data.cur_sel_partner,2)
		else
			UnityPlayerPrefs.SetInt(role_id.."partner_attribute_dan"..self.data.cur_sel_partner,1)
		end
		self.data:RemindPartnerDataRole()
	end
end

function AttributeDanPanel:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[159].desc})
end


AttributeDanItem = AttributeDanItem or DeclareMono("AttributeDanItem", UIWidgetBaseItem)
function AttributeDanItem:AttributeDanItem()
    self.lang = Language.AttributeDan
end

function AttributeDanItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_id = data.attribute_pellet_id
	local item_data = Item.New()
    item_data.item_id = item_id
	item_data.num = tostring(data.num)
	local item_name = item_data:Name()
	self.Cell:SetData(item_data)
	UH.SetText(self.Name,item_name)
	local param2 = -1
	if AttributeDanData.Instance.now_attributedan_type == 3 then
		param2 = AttributeDanData.Instance.cur_sel_partner
	end
	local attr_value = AttributeDanData.Instance:GetAttibuteDanByItemId(data.attribute_pellet_id,param2).attr_value
	local value_name = string.format(self.lang.AttrStr, Language.Common.AttrList[data.attr_type], attr_value)
	UH.SetText(self.Value,value_name)
end






