AttributeDanData = AttributeDanData or BaseClass()

--���Ե�����
AttributeDanData.Type = {
	Role = 1,
	Pet = 2,
	Partner = 3,
}
AttributeDanData.UnlockType = {
	Default = 0,	--������
	FeiSheng = 1,--�������� param1:��������
}


function AttributeDanData:__init()
	if AttributeDanData.Instance ~= nil then
		Debuger.LogError("[AttributeDanData] attempt to create singleton twice!")
		return
	end	
	AttributeDanData.Instance = self
	self.now_attributedan_type = 1
	self.cur_sel_partner = -1
	self.role_use_attr_list = SmartData.New()
	self.partner_use_attr_list = SmartData.New()
	self.use_list_change = SmartData.New({flag = false})
	self.attribute_red_piont = SmartData.New({val = false,count = 0})	
	self.attribute_red_piont_pet = SmartData.New({val = false,count = 0})
	self.attribute_red_piont_partner = SmartData.New({val = false,count = 0})
end

function AttributeDanData:__delete()

end

function AttributeDanData:GetCanUse()
	return RoleLevel() >= FunOpen.GetConfog().attr_dan_use_level , FunOpen.GetConfog().attr_dan_use_level
end

function AttributeDanData:SetAttributeDanType(m_type)
	self.now_attributedan_type = m_type
end

function AttributeDanData:SetCurSelPartner(cur_sel)
	self.cur_sel_partner = cur_sel
end

function AttributeDanData:GetAttibuteDanByItemId(item_id,cur_sel_partner)
	local attributedan = nil
	local prof_type = RoleData.Instance:ProfType()
	for k,v in pairs(Config.attribute_pellet_auto.attribute_list) do 
		if v.attribute_pellet_type == 0 then
			if v.attribute_pellet_id == item_id and prof_type == v.type_param then
				attributedan = v
				break
			end
		end
		
		if v.attribute_pellet_type == 1 then
			local pet = PetData.Instance:GetCurSelect()
			if pet == nil then
				if v.attribute_pellet_id == item_id then
					attributedan = v
					break
				end
			else
				local index = pet:MainAttr()
				if v.attribute_pellet_id == item_id and index == v.type_param then
					attributedan = v
					break
				end
			end	

		end	
		if v.attribute_pellet_type == 2 then	
			if cur_sel_partner == nil then
				if v.attribute_pellet_id == item_id then
					attributedan = v
					break
				end
			else
				local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(cur_sel_partner)
				local info = data.info
				if v.attribute_pellet_id == item_id and info.job == v.type_param then
					attributedan = v
					break
				end
			end
		end
	end
	return attributedan
end

function AttributeDanData:GetAttibuteDanAllByType(m_type)
	local list = {}
	m_type = m_type - 1
	local has_add_id = {}
	local prof_type = RoleData.Instance:ProfType()
	for k,v in pairs(Config.attribute_pellet_auto.attribute_list) do 
		if v.attribute_pellet_type == m_type then
			if m_type == 0 then
				if prof_type == v.type_param then
					table.insert(list,v)
				end
			else
				local is_add = true
				for k1,v1 in pairs(has_add_id) do 
					if v1 == v.attribute_pellet_id then
						is_add = false
						break
					end
				end
				if is_add then
					table.insert(list,v)
					table.insert(has_add_id,v.attribute_pellet_id)
				end
			end
		end
	end

	for k,v in pairs(list) do 
		local num = BagData.Instance:GetNumByItemId(v.attribute_pellet_id)
		v.num = num
	end	
	
	return list
end

function AttributeDanData:foreachTypeCfg(dan_type,func)
	for _,v in pairs(Config.attribute_pellet_auto.other) do
		if v.type == dan_type then
			func(v)
		end
	end
end

function AttributeDanData:GetAttibuteDanLimit()
	local limit = Config.attribute_pellet_auto.other[1].use_limit
	if self.now_attributedan_type == 1 then
		local stage = FlyUpData.Instance:GetStage()
		local count = Config.role_feisheng_auto.other[1].use_limit_up
		limit = limit + stage*count
	elseif self.now_attributedan_type == AttributeDanData.Type.Pet then
		self:foreachTypeCfg(self.now_attributedan_type,function(cfg) 
			if limit == 0 or cfg.use_limit < limit then
				limit = cfg.use_limit
			end
		end)
		local pet = PetData.Instance:GetCurSelect()
		if pet ~= nil then
			limit = limit + Config.pet_cfg_auto.other[1]["pellet_fs" .. tostring(pet:FeiShengTimes())]
		end
	elseif self.now_attributedan_type == 3 then
		local partner = PartnerData.Instance:GetPartnerInfoById(self.cur_sel_partner)
		limit = partner:GetAttibuteDanLimit(limit)
	end 
	return	limit
end

function AttributeDanData:GetAttibuteDanUseInfo(m_type,param3)
	local use_count = 0
	if self.now_attributedan_type == 1 then
		use_count = self:GetRoleAttibuteDanUseInfo(m_type)
	end 
	if self.now_attributedan_type == 2 then
		use_count = self:GetPetAttibuteDanUseInfo(m_type)
	end 
	
	if self.now_attributedan_type == 3 then
		use_count = self:GetPartnerAttibuteDanUseInfo(param3,m_type)
	end 
	return use_count
end

function AttributeDanData:GetRoleAttibuteDanUseInfo(m_type)
	local use_count = 0
	for k,v in pairs(self.role_use_attr_list) do 
		if m_type == k - 1 then
			use_count =v
			break
		end
	end
	return use_count
end

function AttributeDanData:GetPetAttibuteDanUseInfo(m_type)
	local use_count = 0
	local pet = PetData.Instance:GetCurSelect()
	if pet then
		use_count = pet.info.use_attr_pellet_list[m_type + 1]
    end
	return use_count
end

function AttributeDanData:GetPartnerAttibuteDanUseInfo(sel_partner,m_type)
	local use_count = 0
	for k,v in pairs(self.partner_use_attr_list) do 
		if sel_partner == k - 1  then
			for k1,v1 in pairs(v) do 
				if m_type == k1 - 1 then
					use_count = v1
					break
				end
			end
			break
		end
	end
	return use_count
end

function AttributeDanData:ChangeConfigTypeToUseType(c_type)
	local m_type = 0
	if c_type <= 4 then
		m_type = c_type
	else
		if c_type == 6 then
			m_type = 5
		end
		if c_type == 8 then
			m_type = 6
		end
	end
	
	return m_type
end


function AttributeDanData:ChangeAttributeUseList(m_info)
	self.use_list_change.flag = not self.use_list_change.flag
	if m_info.role_attr_pellet_type == 0 then
		local count = -1
		for k,v in pairs(self.role_use_attr_list) do 
			if m_info.attr_pellet_type == k - 1 then
				count = m_info.use_count
				break
			end
		end
		if count ~= -1 then
			self.role_use_attr_list[m_info.attr_pellet_type + 1] = count
		end
	end
	
	if m_info.role_attr_pellet_type == 2 then
		local list = self.partner_use_attr_list[m_info.param3 + 1]
		local count = -1
		for k,v in pairs(list) do 
			if m_info.attr_pellet_type == k - 1 then
				count = m_info.use_count
				break
			end
		end
		if count ~= -1 then
			list[m_info.attr_pellet_type + 1] = count
		end
	end
end

function AttributeDanData:GetPartnerRedAttibutePiontNum()
	if not self:GetCanUse() then
		return 0
	end
	self.attribute_red_piont_partner.count = 0
	local show_red = false
	local list = PartnerData.Instance:GetPartnerAllInfo()
	for k,v in pairs(list) do 
		if self:GetOnePartnerPiontNum(v.vo.partner) > 0 then
			show_red = true
			self.attribute_red_piont_partner.count = 1
			break
		end	
	end
	return show_red and 1 or 0
end


function AttributeDanData:GetOnePartnerPiont(data)
	if not self:GetCanUse() then
		return false
	end
	local role_id = RoleData.Instance:GetRoleId()
	local red_sign = UnityPlayerPrefs.GetInt(role_id.."partner_attribute_dan"..data.id)
	if red_sign == 0 then
		if data.vo.fight_state ~= 1 then
			return false
		end
	end
	if red_sign == 1 then
		return false
	end
	local show_red = false
	local limit_count = self:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = self:GetPartnerAttibuteDanUseInfo(data.id,i)
		if use_count < limit_count then
			local item_id_index = 0
			if i > 4 then
				item_id_index = 27814 + i - 5
			else
				item_id_index = 27816 + i
			end
			if self:GetAttibuteDanByItemId(item_id_index) then
				local attribute_pellet_id = self:GetAttibuteDanByItemId(item_id_index).attribute_pellet_id
				local num = BagData.Instance:GetNumByItemId(attribute_pellet_id)
				if num > 0 then
					show_red = true
					break
				end
			end
		end
	end	
	
	return show_red 
end

function AttributeDanData:GetOnePartnerPiontNum(sel_partner)
	if not self:GetCanUse() then
        return 0
    end
	if sel_partner == nil then
	   return 0
	end
	local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(sel_partner)
	local role_id = RoleData.Instance:GetRoleId()
	local red_sign = UnityPlayerPrefs.GetInt(role_id.."partner_attribute_dan"..sel_partner)
	if red_sign == 0 then
		if data.vo.fight_state ~= 1 then
			return 0
		end
	end
	if red_sign == 1 then
		return 0
	end
	local show_red = false
	local limit_count = self:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = self:GetPartnerAttibuteDanUseInfo(sel_partner,i)
		if use_count < limit_count then
			local item_id_index = 0
			if i > 4 then
				item_id_index = 27814 + i - 5
			else
				item_id_index = 27816 + i
			end
			if self:GetAttibuteDanByItemId(item_id_index) then
				local attribute_pellet_id = self:GetAttibuteDanByItemId(item_id_index).attribute_pellet_id
				local num = BagData.Instance:GetNumByItemId(attribute_pellet_id)
				if num > 0 then
					show_red = true
					break
				end
			end
		end
	end
	return show_red and 1 or 0
end


function AttributeDanData:GetRedAttibutePiontNum()
	if not self:GetCanUse() then
		return 0
	end
	self.attribute_red_piont.count = 0
	local is_show = true
	local role_id = RoleData.Instance:GetRoleId()
	if UnityPlayerPrefs.GetInt(role_id.."role_attribute_dan") == 1 then
		is_show = false
	end	
	
	if not is_show then
		return 0
	end

	local limit_count = self:GetAttibuteDanLimit()
	local show_red = false
	
	for i=0,6 do
		local use_count = self:GetRoleAttibuteDanUseInfo(i)
		if use_count < limit_count then
			local item_id_index = 0
			if i > 4 then
				item_id_index = 27800 + i - 5
			else
				item_id_index = 27802 + i
			end
			if self:GetAttibuteDanByItemId(item_id_index) then
				local attribute_pellet_id = self:GetAttibuteDanByItemId(item_id_index).attribute_pellet_id
				local num = BagData.Instance:GetNumByItemId(attribute_pellet_id)
				if num > 0 then
					show_red = true
					self.attribute_red_piont.count = 1
					break
				end
			end
		end
	end	
	
	return show_red and 1 or 0
end


function AttributeDanData:RemindBagDataRole()
	self.attribute_red_piont.val = not self.attribute_red_piont.val
	self:GetRedAttibutePiontNum()
end

function AttributeDanData:RemindPetDataRole()
	self.attribute_red_piont_pet.val = not self.attribute_red_piont_pet.val
end

function AttributeDanData:RemindPartnerDataRole()
	self.attribute_red_piont_partner.val = not self.attribute_red_piont_partner.val
	self:GetPartnerRedAttibutePiontNum()
end

function AttributeDanData:RemindBagData()
	self.attribute_red_piont.val = not self.attribute_red_piont.val
	self.attribute_red_piont_partner.val = not self.attribute_red_piont.val
	self.attribute_red_piont_pet.val = not self.attribute_red_piont_pet.val
	self:GetRedAttibutePiontNum()
	self:GetPartnerRedAttibutePiontNum()
end

function AttributeDanData:RecvRoleAttrPelletAllInfo(protocol)
	self:RemindBagData()
	if self.item_care == nil then
		self.item_care = BagData.Instance.item_grid[ItemColumnType.Item]:Care(BindTool.Bind1(self.RemindBagData, self))
	end
	
	if self.pet_care == nil then
		self.pet_care = PetData.Instance:GetPetList():Care(BindTool.Bind1(self.RemindPetDataRole, self))
	end
end






