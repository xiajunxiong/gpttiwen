
function PartnerData:PartnerDataLifeHandBook()
	self.partner_life_handbook_list = {}
	self.partner_life_handbook_attrs = {}
	self.partner_life_handbook_cards = {}
	for _, v in ipairs(self.pe_config_partner_equipment) do
		self.partner_life_handbook_cards[v.index] = self.partner_life_handbook_cards[v.index] or {}
		local co = Item.GetConfig(v.item_id)
		v.limit_level = co.limit_level or 10
		v.color = co.color or 3
		self.partner_life_handbook_cards[v.index][co.partner_equipment_type or 0] = v
	end
	for _, v in ipairs(self.pe_config_att_value) do
		self.partner_life_handbook_attrs[v.max_level] = self.partner_life_handbook_attrs[v.max_level] or {}
		self.partner_life_handbook_attrs[v.max_level][v.color] = self.partner_life_handbook_attrs[v.max_level][v.color] or {}
		self.partner_life_handbook_attrs[v.max_level][v.color][v.att_type] = v
	end
	for k, v in ipairs(Config.partner_equipment_cfg_auto.suit) do
		self.partner_life_handbook_list[v.suit_id] = self.partner_life_handbook_list[v.suit_id] or {prof = PartnerConfig.LifeId2ProfType[v.suit_id], levels = {}}
		if v.suit_level >= 50 and v.suit_color < 6 then
			v.cards = self.partner_life_handbook_cards[v.index]
			self.partner_life_handbook_list[v.suit_id].levels[v.suit_level] = self.partner_life_handbook_list[v.suit_id].levels[v.suit_level] or {level = v.suit_level, suits = {}}
			self.partner_life_handbook_list[v.suit_id].levels[v.suit_level].suits[v.suit_color] = v
		end
	end

	self.partner_life_handbook_info = SmartData.New{
		prof = SmartData.New{},
		level = SmartData.New{},
	}
end

function PartnerData:ResetLifeHandbookInfo()
	self.partner_life_handbook_info.prof:Set{}
	self.partner_life_handbook_info.level:Set{}
end

function PartnerData:GetPartnerLifeHandbookProfListSort()
	if nil == self.partner_life_handbook_list_sort then
		local list = {}
		for k, v in pairs(self.partner_life_handbook_list) do
			table.insert(list, v)
		end
		table.sort(list, function (a, b)
			if not a or not b then return a end
			local ap = a.prof
			local bp = b.prof
			return ap < bp
		end)
		self.partner_life_handbook_list_sort = list
	end
	local prof_sel = self.partner_life_handbook_list_sort[1]
	local prof_type = RoleData.Instance:ProfType()
	local partner = self:GetPartnerInfoById(self.cur_sel_partner)
	if partner then
		prof_type = partner.info.job or prof_type
	end
	for k, v in ipairs(self.partner_life_handbook_list_sort) do
		if v.prof == prof_type then
			prof_sel = v
			break
		end
	end
	return self.partner_life_handbook_list_sort, prof_sel
end

function PartnerData:GetPartnerLifeHandbookLevelListSort()
	local auto_level = PartnerLifeHandbookViewF.AutoLevel
	if nil == self.partner_life_handbook_info.prof.levels_sort then
		local list = {}
		for k, v in pairs(self.partner_life_handbook_info.prof.levels) do
			table.insert(list, v)
		end
		table.sort(list, function (a, b)
			if not a or not b then return a end
			local al = a.level
			local bl = b.level
			return al < bl
		end)
		self.partner_life_handbook_info.prof.levels_sort = list
	else
		PartnerLifeHandbookViewF.AutoLevel = false
	end
	local level_sel = self.partner_life_handbook_info.prof.levels_sort[1]
	local level = auto_level and math.floor(RoleData.Instance:GetRoleLevel() / 10) * 10 or self.partner_life_handbook_info.level.level
	if nil ~= level then
		for k, v in ipairs(self.partner_life_handbook_info.prof.levels_sort) do
			if v.level == level then
				level_sel = v
				break
			end
		end
	end
	return self.partner_life_handbook_info.prof.levels_sort, level_sel
end

function PartnerData:GetPartnerLifeHandbookSuitListSort()
	if nil == self.partner_life_handbook_info.level.suits_sort then
		local list = {}
		for k, v in pairs(self.partner_life_handbook_info.level.suits) do
			table.insert(list, v)
		end
		table.sort(list, function (a, b)
			if not a or not b then return a end
			local ac = a.suit_color
			local bc = b.suit_color
			return ac < bc
		end)
		self.partner_life_handbook_info.level.suits_sort = list
	end
	return self.partner_life_handbook_info.level.suits_sort
end

function PartnerData:GetPartnerLifeHandbookCardInfo(suit, index)
	local card = suit.cards[index]
	if nil == card.attr_list then
		local attr_list = {}
		local plha = self.partner_life_handbook_attrs
		if plha[card.limit_level] and plha[card.limit_level][card.color] then
			local co_attr = plha[card.limit_level][card.color]
			for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
				local key = "att_group" .. (1 == i and "" or ("_" .. i))
				if card[key] > 0 then
					local co_rg = Cfg.PEByRandGroupCoAttrGroup(card[key])
					if co_rg then
						if co_attr[co_rg.att_type] then
							table.insert(attr_list, {
								attr_type = co_attr[co_rg.att_type].att_type,
                				attr_value = co_attr[co_rg.att_type].max,
							})
						end
					end
				end
			end
			card.show_item = PartnerEquipment.New{
				item_id = card.item_id,
				num = 1,
				is_bind = 0,
				param = {attr_list = attr_list},
			}
		end
		card.attr_list = attr_list
	end
	return suit.cards[index]
end