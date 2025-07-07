
IllusionData = IllusionData or BaseClass()

function IllusionData:__init()
	if IllusionData.Instance ~= nil then
        Debuger.LogError("[IllusionData] attempt to create singleton twice!")
        return
    end
    IllusionData.Instance = self

	self:IllusionConfig()
	self:IllusionData()
end

function IllusionData:__delete()
end

function IllusionData:IllusionConfig()
	self.config = Config.surface_cfg_auto
	self.config_surface_list = self.config.surface_list
	self.config_suit_list = self.config.suit_list
	self.config_surface_value = self.config.surface_value
	self.config_exchange_shop = self.config.exchange_shop
	self.config_surface_name = self.config.surface_name

	self.config2 = Config.fashion_auto
	self.other2 = self.config2.other[1]

	Cfg.Register("SurfaceInfoById",Cfg.RealTimeSearch(Config.surface_cfg_auto.surface_list,"id"))
	Cfg.Register("SurfaceInfoByResolveItemId",Cfg.RealTimeSearch(Config.surface_cfg_auto.surface_list,"decompose_item"))
	--通过ResId找到坐骑配置，目前用于主角播放移动音效
	Cfg.Register("SurfaceInfoByResId",Cfg.RealTimeSearch(Config.surface_cfg_auto.surface_list,"res_id"))
	Cfg.Register("SurfaceValue",Cfg.RealTimeSearch2Key(self.config_surface_value,"surface_id", "job"))
	Cfg.Register("SurfaceInfoByActiveStuffId",Cfg.RealTimeSearch(Config.surface_cfg_auto.surface_list,"active_stuff_id"))
	Cfg.Register("SkinPetInfoByActiveStuffId",Cfg.RealTimeSearch(Config.skin_auto.pet_skin,"active_stuff_id"))
	Cfg.Register("SkinPartnerInfoByActiveStuffId",Cfg.RealTimeSearch(Config.skin_auto.partner_skin,"active_stuff_id"))

	self.level_up_cfg = DataHelper.Subdivision(self.config2.fashion_level_up, {"fashion_id", "limit_prof", "fashion_level"})
	self.sp_level_up_cfg = DataHelper.Subdivision(self.config2.star_add, {"limit_prof", "level"})
	-- LogError("self.sp_level_up_cfg",self.sp_level_up_cfg == nil)
	Cfg.Register("IllusionResolveId", Cfg.RealTimeSearch(self.config2.fashion_resolve, "item_id"))
end

function IllusionData:IllusionData()
	self:FashionList()
	self:SuitList()
	self:FashionWearList()
	self:ExchangeShopList()
	self:FashionTypeList()

	self.cur_model_info = SmartData.New{
		avatar_type = 0,
		type = 1,
		suit_id = 0,
		prof_type = 0,
		fashion = SmartData.New(),
		default_prof = {
			trans = false,
			prof = 0,
		},
		is_ride = false,
	}

	self.cur_exchange_info = SmartData.New{
		type = 1,
	}
	self.red_points = SmartData.New{
		[IllusionConfig.FashionType.cloth] = false,
		[IllusionConfig.FashionType.weapon] = false,
		[IllusionConfig.FashionType.mount] = false,
	}

	self.illusion_lv_info = SmartData.New()
	local info = {}
	info.sp_level = 0
	info.levels = {}
	for i = 0, SURFACE_MAX_NUM - 1 do
		info.levels[i] = 0
	end
	info.total_level = 0
	self.illusion_lv_info.info = info

	self.lv_need_info = SmartData.New()
	self.lv_need_info.id = 1
	self.lv_need_info.pos = Vector2.New(0, 0)

	self.sp_lv_up_select = SmartData.New()
	self.sp_lv_up_select.index = 1
	self.sp_lv_up_select.page = 1
end

function IllusionData:FashionList()
	self.fashion_list = {}
	for _, info in pairs(self.config_surface_list)	do
		self.fashion_list[info.id] = IllusionInfo.New{info = info}
	end

	self.fashion_names = {}
	for _, info in pairs(self.config_surface_name)	do
		self.fashion_names[info.surface_id] = self.fashion_names[info.surface_id] or {}
		self.fashion_names[info.surface_id][info.role_id] = info.name
	end

	self:FashionShowList()
end

function IllusionData:FashionTypeList()
	self.fashion_type_list = {
		{type = IllusionConfig.FashionType.cloth},
		{type = IllusionConfig.FashionType.weapon},
		{type = IllusionConfig.FashionType.mount},
		{type = IllusionConfig.FashionType.partner},
		{type = IllusionConfig.FashionType.pet},
	}
end

function IllusionData:GetFashionTypeList()
	return self.fashion_type_list
end

function IllusionData:GetNeedJumpItemId(item_id)
	return self.need_jump_item_id
end

function IllusionData:SetNeedJumpItemId(item_id)
	if not item_id then
		if self:JumpItemIdPageFlag() and self:JumpItemIdItemFlag() then
			self.need_jump_item_id = item_id
		end
	else
		self.need_jump_item_id = item_id
		self:JumpItemIdPageFlag(false)
		self:JumpItemIdItemFlag(false)
	end
end

function IllusionData:JumpItemIdPageFlag(value)
	if value ~= nil then
		self.jump_item_id_page_flag = value
	else
		return self.jump_item_id_page_flag or false
	end
end

function IllusionData:JumpItemIdItemFlag(value)
	if value ~= nil then
		self.jump_item_id_item_flag = value
	else
		return self.jump_item_id_item_flag or false
	end
end

function IllusionData:FashionShowList()
	self.fashion_show_list = {}
	-- local count = {}
	local function to_on(list)
		for i = 1, IllusionConfig.FASHION_PER_ROW do
			list[#list][i] = list[#list][i] or {is_empty = true}
		end
	end
	local role_level = RoleData.Instance:GetRoleLevel()
	local page = 1
	local index = 1
	for _, item in ipairs(self.config_surface_list)	do--根据配置表顺序遍历，取值时按照实例化好的fashion_list取
		if role_level >= item.show_level and item.is_display == 1 then
			if self.fashion_show_list[item.type] == nil then
				self.fashion_show_list[item.type] = {}
				page = 1
				index = 1
			end
			self.fashion_show_list[item.type][page] = self.fashion_show_list[item.type][page] or {}
			self.fashion_show_list[item.type][page][index] = self.fashion_list[item.id]
			index = index + 1
			if index > IllusionConfig.FASHION_PER_ROW then
				page = page + 1
				index = 1
			end
		end
	end
	
	for _, list in pairs(self.fashion_show_list) do
		to_on(list)
	end
end

function IllusionData:SuitList()
	self.suit_list = {}
	for _, info in pairs(self.config_suit_list)	do
		self.suit_list[info.suit_id] = info
		self.suit_list[info.suit_id].attr_show = self:SuitListAttrShow(info)
		self.suit_list[info.suit_id].suit_state = {}
		self.suit_list[info.suit_id].suit_state_actived = false
	end
	for _, info in pairs(self.config_surface_list) do
		if info.suit_id ~= 0 then
			self.suit_list[info.suit_id].suit_state[info.type] = {
				name = info.surface_name,
				actived = false
			}
		end
	end
end

function IllusionData:SuitListAttrShow(info)
	local list = {}
	for k, v in pairs(info) do
		local suit_attr = Language.Illusion.AttrNames[k]
		if nil ~= suit_attr and v ~= 0 then
			table.insert(list, {
				suit_id = info.suit_id,
				content = suit_attr[1] .. v,
				seq = suit_attr[2]
			})
		end
	end
	table.sort(list, function(a, b)
		return a.seq < b.seq
	end)
	return list
end

function IllusionData:SuitListState(info)
	if nil ~= info then
		self.suit_list[info.suit_id].suit_state[info.type].actived = info.level > 0
		self.suit_list[info.suit_id].suit_state_actived = true
		for _, suit_state in pairs(self.suit_list[info.suit_id].suit_state) do
			self.suit_list[info.suit_id].suit_state_actived = self.suit_list[info.suit_id].suit_state_actived and suit_state.actived
		end
	else
		for _, info in pairs(self.fashion_list) do
			if info.suit_id ~= 0 then
				self.suit_list[info.suit_id].suit_state[info.type].actived = info.level > 0
			end
		end
		for key, info in pairs(self.suit_list) do
			self.suit_list[key].suit_state_actived = true
			for _, suit_state in pairs(info.suit_state) do
				self.suit_list[key].suit_state_actived = self.suit_list[key].suit_state_actived and suit_state.actived
			end
		end
	end
end

function IllusionData:FashionWearList()
	self.fashion_wear_list = {}
end

function IllusionData:ExchangeShopList()
	self.exchange_shop_list = {}
	for _, info in ipairs(self.config_exchange_shop) do
		local type = IllusionConfig.PageToType[info.paging_id]
		self.exchange_shop_list[type] = self.exchange_shop_list[type] or {}
		table.insert(self.exchange_shop_list[type], {
			seq = info.seq,
			consume_item = info.consume_item,
			consume_num = info.use_universal_patch,
			exchange_item = info.exchange_item.item_id,
			is_time_limit = 1 == info.is_time,
			limit_time = info.time,
			id = info.id,
			type = type,
			name_sign = info.name_sign,
			surface = IllusionConfig.FashionType.partner == type and Cfg.SkinPartnerInfoByActiveStuffId(info.id)
				or (IllusionConfig.FashionType.pet == type and Cfg.SkinPetInfoByActiveStuffId(info.id)
				or Cfg.SurfaceInfoById(info.id))
		})
	end
end

function IllusionData:GetExchangeShopList(type)
	local list = {}
	for _, info in ipairs(self.exchange_shop_list[type]) do
		if not info.is_time_limit or (info.limit_time - TimeHelper.GetServerTime()) then
			if IllusionConfig.FashionType.partner == info.type then
				info.fce = self:GetPartnerSkinCanExchange(info.surface.partner_skin_id, info.exchange_item)
			elseif IllusionConfig.FashionType.pet == info.type then
				info.fce = self:GetPetSkinCanExchange(info.surface.pet_skin_id, info.exchange_item)
			else
				info.fce = self:GetFashionCanExchange(info.id, info.exchange_item)
			end
			table.insert(list, info)
		end
	end
	if IllusionConfig.FashionType.partner == type or IllusionConfig.FashionType.pet == type then
		table.sort(list, function (a, b)
			if not a or not b then return a end
			return a.seq < b.seq
		end)
	else
		table.sort(list, function (a, b)
			if not a or not b then return a end
			local ace =	a.fce and 0 or 1
			local bce =	b.fce and 0 or 1
			if ace == bce then
				return a.id < b.id
			end
			return ace < bce
		end)
	end
	return list
end

function IllusionData:SetSurfaceAllInfo(protocol)
	for _, info in pairs(self.fashion_list) do
		info:Reset()
	end
	for i = 1, protocol.surface_count do
		local vo = protocol.surface_list[i]
		if self.fashion_list[vo.id] ~= nil then
			self.fashion_list[vo.id]:IllusionVo(vo)
		end
	end
	self.fashion_wear_list = protocol.wear_surface
	self:FashionShowList()
	self:SuitListState()
	self:FlushRedPoints()
end

function IllusionData:SetSurfaceSingleInfo(protocol)
	local vo = protocol.surface_info
	if self.fashion_list[vo.id] ~= nil then
		if 1 == protocol.reason then
			local info = self.fashion_list[vo.id]
			-- 取消本次登录新获得的时装红点
			-- info.red_point = true
			local item = Item.Init(info.active_stuff_id)
			if item and not AppearanceCtrl.Instance.is_shield_appear_show then
				local cs = item:ColorStr()
				local get_tips = string.format(Language.Illusion.GetTips, cs, info.name)
				PublicPopupCtrl.Instance:Center(get_tips)
				ChatCtrl.Instance:ChannelChatCustom{content = get_tips}
			end
			self:FlushRedPoints()
			if info.type == 1 then
				RoleSettingData.Instance:SetHeadTipsFlag(ChangeHeadType.Surface,vo.id,true)
			end
			AppearanceCtrl.Instance:OpenAppearanceShowIllusion(vo.id)
		end
		self.fashion_list[vo.id]:IllusionVo(vo)
		if self.fashion_list[vo.id].suit_id ~= 0 then
			self:SuitListState(self.fashion_list[vo.id])
		end
	end
end

function IllusionData:SetSurfaceWearInfo(protocol)
	self.fashion_wear_list = protocol.wear_surface
end

function IllusionData:GetFashionInfoById(id)
	return self.fashion_list[id]
end

function IllusionData:GetFashionCanExchange(id, exchange_item)
	local value = false
	if not self.fashion_list[id] then return value end
	value = 0 == self.fashion_list[id].level and 0 == Item.GetNum(exchange_item)
	return value
end

function IllusionData:GetPartnerSkinCanExchange(partner_skin_id, exchange_item)
	local value = false
	value = not PartnerData.Instance.partner_skin_actived[partner_skin_id]
	return value
end

function IllusionData:GetPetSkinCanExchange(pet_skin_id, exchange_item)
	local value = false
	value = nil == PetData.Instance.pet_has_skin_list[pet_skin_id]
	return value
end

function IllusionData:GetFashionShowListWithType(type)
	return self.fashion_show_list[type]
end

function IllusionData:GetSuitList()
	return self.suit_list
end

function IllusionData:GetSuitInfoWithSuitId(suit_id)
	return self.suit_list[suit_id]
end

function IllusionData:GetFashionWearIdWithType(type)
	return self.fashion_wear_list[type]
end

function IllusionData:ResetCurModelInfo(is_all)
	self.cur_model_info.avatar_type = RoleData.Instance:GetRoleAvatarType()
	self.cur_model_info.suit_id = 0
	self.cur_model_info.prof_type = RoleData.Instance:ProfType()
	self.cur_model_info.default_prof = {trans = false, prof = RoleData.Instance:ProfType()}
	self.cur_model_info.fashion[IllusionConfig.FashionType.cloth] = nil
	self.cur_model_info.fashion[IllusionConfig.FashionType.weapon] = nil
	self.cur_model_info.fashion[IllusionConfig.FashionType.mount] = self.fashion_show_list[IllusionConfig.FashionType.mount][1][1]
	if is_all then
		self.cur_model_info.type = 1
		self.cur_model_info.is_ride = false
	end
end

function IllusionData:CurSelAvatarType(avatar_type)
	self.cur_model_info.avatar_type = avatar_type
end

function IllusionData:CurSelFashion(fashion, selected)
	local value, red_point = false, false
	if selected then
		self.cur_model_info.suit_id = fashion.suit_id
		self.cur_model_info.fashion[fashion.type] = fashion
		if self.fashion_list[fashion.id].red_point then
			-- 取消本次登录新获得的时装红点
			-- red_point = true
			self.fashion_list[fashion.id].red_point = false
			self:FlushRedPoints()
		end
		if IllusionConfig.FashionType.cloth == self.cur_model_info.type then
			self:FashionGroupSelect(fashion.info.group, IllusionConfig.FashionType.weapon)
		end
	elseif fashion == self.cur_model_info.fashion[fashion.type] then
		if IllusionConfig.FashionType.mount == self.cur_model_info.type then
			-- 锁死坐骑不能取消选中
			value = true
		else
			self.cur_model_info.suit_id = 0
			self.cur_model_info.fashion[fashion.type] = nil
		end
	end
	return value, red_point
end

function IllusionData:FashionGroupSelect(group, fashion_type)
	if 0 == group then return end
	local list = self:GetFashionShowListWithType(fashion_type)
	for _, row in pairs(list) do
		for i = 1, IllusionConfig.FASHION_PER_ROW do
			local info = row[i]
			if not info.is_empty and info.info.group == group then
				self.cur_model_info.fashion[fashion_type] = self.fashion_list[info.id]
				break
			end
		end
	end
end

function IllusionData:CurSelProfIndex()
	local prof_type = self.cur_model_info.default_prof.prof
	-- local prof_type = RoleData.Instance:ProfType()
	local index = 1
	for k, v in pairs(RoleCreateData.Instance:GetRoleProfList(self.cur_model_info.avatar_type).prof_list) do
		local pt = RoleData.ProfIdToProfType(v.prof_id)
		if prof_type == pt then index = k end
	end
	return index
end

function IllusionData:GetShowPngPath(info)
    if IllusionConfig.FashionType.cloth == info.type then
        return string.format(IllusionConfig.PngPath, string.format("%s%s", info.id, info.avatar_type))
    elseif IllusionConfig.FashionType.weapon == info.type then
        return string.format(IllusionConfig.PngPath, string.format("%s%s", info.id, info.prof_type))
    elseif IllusionConfig.FashionType.mount == info.type then
        return string.format(IllusionConfig.PngPath, info.id * 10)
    end
end

function IllusionData:GetShowPngPathByItemId(item_id)
    local config = Item.GetConfig(item_id)
    if config and config.use_type and config.use_type == ItemUseType.Illusion then
        local co = Cfg.SurfaceInfoByActiveStuffId(item_id)
        if co then
            return self:GetShowPngPath({id = co.id, type = co.type, avatar_type = RoleData.Instance:GetRoleAvatarType(), prof_type = RoleData.Instance:ProfType()})
        end
    end
    return nil
end

-- 整合的，可能不全
function IllusionData.GetLiHuiByItemId(item_id)
    local item_cfg = Item.Init(item_id):Config()
    if item_cfg.item_type == ItemType.Fashion then
        local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
            if surface_cfg.type == IllusionConfig.FashionType.mount 
			or surface_cfg.type == IllusionConfig.FashionType.cloth 
			or surface_cfg.type == IllusionConfig.FashionType.weapon then
                local res_info = Cfg.SurfaceInfoByActiveStuffId(item_id)
				return IllusionData.Instance:GetShowPngPath{id = res_info.id, type = surface_cfg.type, 
				avatar_type = RoleData.Instance:GetRoleAvatarType(), prof_type = RoleData.Instance:ProfType()}
			end
        end
    elseif item_cfg.item_type == ItemType.Skin then
        local skin_cfg = nil
        if item_cfg.show_type == ShowType.PetSkin then
            local  res_info = Cfg.SkinPetInfoByActiveStuffId(item_id)
			return  string.format("pet_skin/%s.png",  res_info.res_id)
        elseif item_cfg.show_type == ShowType.PartnerSkin then
            local res_info = Cfg.SkinPartnerInfoByActiveStuffId(item_id)
			return string.format(PartnerConfig.PngPath,  res_info.partner_skin_id)
        end
	elseif item_cfg.show_type==ShowType.Pet then
		return string.format("pet/%s.png","pet_"..item_id)
    end
	return
end

function IllusionData:GetAttrList(id)
	local list = {}
	for i = 1, 8 do
		local t = {}
		t.prof = i
		t.prof_name = Language.Common.Prof[i]
		local lv = self:IllusionLv(id)
		lv = lv == 0 and 1 or lv
		local cfg = self:GetLvUpCfg(id, i * 100, lv)
		t.attrs = DataHelper.GetAttrList(cfg)
		for k, v in pairs(t.attrs) do
			v.prof = i
		end
		table.insert(list, t)
	end

	local prof_type = RoleData.Instance:ProfType()
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local af = prof_type == a.prof and 0 or 1
		local bf = prof_type == b.prof and 0 or 1
		if af == bf then return a.prof < b.prof end
		return af < bf
	end)
	return list
end

function IllusionData:TransFashionToAppearance()
	local appearance = TableCopy(RoleData.Instance:GetApearance())
	local fashion = self.cur_model_info.fashion
	appearance.avatar_type = self.cur_model_info.avatar_type
	if fashion[1] then
		appearance.color = 0
		appearance.fashion_body_id = fashion[1].info.res_id
		appearance.special_appearance_type = 0
        appearance.special_appearance_param = 0
	end
	if fashion[2] then
		appearance.weapon_id = 2001 + self.cur_model_info.prof_type * 100
		appearance.fashion_weapon_id = fashion[2].info.res_id
	end
	if fashion[4] then
		appearance.ride_id = fashion[4].info.res_id
	end
	return appearance
end

function IllusionData:FashionIdToResId(id)
	return self.fashion_list[id] and self.fashion_list[id].res_id or 0
end

function IllusionData:FashionIconId(id, icon_id)
	local co = Cfg.SurfaceInfoById(id)
	if co then
		if IllusionConfig.FashionType.cloth == co.type then
			icon_id = icon_id .. (RoleData.Instance:GetRoleAvatarType() or 1)
		elseif IllusionConfig.FashionType.weapon == co.type then
			local prof_type = RoleData.Instance:ProfType()
			icon_id = icon_id .. prof_type
		end
	end
	return icon_id
end

function IllusionData:FlushRedPoints()
	-- self.red_points[IllusionConfig.FashionType.cloth] = false
	-- self.red_points[IllusionConfig.FashionType.weapon] = false
	-- self.red_points[IllusionConfig.FashionType.mount] = false
	-- for _, info in pairs(self.fashion_list) do
	-- 	self.red_points[info.type] = self.red_points[info.type] or info.red_point
	-- end
end

function IllusionData:SetLvUpNeedData(id, pos)
	self.lv_need_info.id = id
	self.lv_need_info.pos = pos
end

-- 专精界面当前选择第几个，从1开始
function IllusionData:SpLvUpIndex(index)
	if index then
		self.sp_lv_up_select.index = index
	else
		return self.sp_lv_up_select.index
	end
end

function IllusionData:SpLvUpPage(page)
	if page then
		self.sp_lv_up_select.page = page
	else
		return self.sp_lv_up_select.page
	end
end

function IllusionData:GetLvUpCfg(id, prof, lv)
	prof = prof or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof = prof == 0 and 100 or prof
	lv = lv or self:IllusionLv(id)
	return self.level_up_cfg[id][prof][lv] or {}
end

function IllusionData:GetLvUpAttr(id, prof, lv, is_zero, is_use_type_key)
	local cfg = self:GetLvUpCfg(id, prof, lv)
	if not TableIsEmpty(cfg) then
		return DataHelper.GetAttrList(cfg, is_zero, is_use_type_key)
	end
	return {}
end

function IllusionData:GetSpLvUpCfg(prof, lv)
	prof = prof or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof = prof == 0 and 100 or prof
	lv = lv or self.illusion_lv_info.info.sp_level
	self.sp_level_up_cfg[prof] = self.sp_level_up_cfg[prof] or {}
	return self.sp_level_up_cfg[prof][lv] or {}
end

-- 获得专精等级的属性，属性是累加的
function IllusionData:GetSpLvUpAttr(prof, lv)
	local list = {}
	lv = lv or self.illusion_lv_info.info.sp_level
	for i = 1, lv do
		local now_data = self:GetSpLvUpCfg(prof, i)
		local one_attr = DataHelper.GetAttrList(now_data, false, true)
		for k, v in pairs(one_attr) do
			if list[k] then
				list[k] = CommonStruct.AttrAdd(k, v.attr_value, list[k].attr_value)
			else
				list[k] = v
			end
		end
	end
	return list
end

-- 获取本职业的外观专精表
function IllusionData:GetSpLvUpData()
	-- 8个一组
	local num = 8
	local list = {}
	prof = prof or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof = prof == 0 and 100 or prof
	for k, v in pairs(self.sp_level_up_cfg[prof]) do
		local k_1 = (k - 1)
		local t1, t2 = math.floor(k_1 / num) + 1, (k_1 % num) + 1
		if not list[t1] then
			list[t1] = {}
		end
		v.my_index = t2
		list[t1][t2] = v
	end
	return list
end

function IllusionData:CheckUpRemind()
	self.red_points[IllusionConfig.FashionType.cloth] = false
	self.red_points[IllusionConfig.FashionType.weapon] = false
	self.red_points[IllusionConfig.FashionType.mount] = false
	for _, info in pairs(self.fashion_list) do
		local value = self:IsCanLvUp(info.id)
		self.red_points[info.type] = self.red_points[info.type] or value
	end
	self.red_points:Notify()
end

-- 时装升级所需材料item_list，红点用
function IllusionData:RemindItemList()
    if not self.lv_up_item_id_list then
        self.lv_up_item_id_list = {}
        local item_id_list = {}
        for k, v in pairs(self.config_surface_list) do
            table.insert(self.lv_up_item_id_list, v.decompose_item)
        end
	end
    return self.lv_up_item_id_list
end

-- 时装是否特殊时装
function IllusionData:IsSpecial(id)
	local cfg = Cfg.SurfaceInfoById(id)
	return cfg.is_limit > 0
end

-- 时装等级
function IllusionData:IllusionLv(id)
	return self.illusion_lv_info.info.levels[id] or 0
end

-- 时装升级最大等级
function IllusionData:IllusionMaxLv()
	return self.other2.max_level
end

-- 时装专精最大等级
function IllusionData:IllusionSpMaxLv()
	return self.other2.max_star_level
end

-- 判断时装是否可以升级
function IllusionData:IsCanLvUp(id)
	local lv = self:IllusionLv(id)
	local str = Language.IllusionUp.Tip
	if self:IsSpecial(id) then
		return false, str[4], 4
	end
	if lv <= 0 then
		return false, str[3], 3
	end
	if lv >= self:IllusionMaxLv() then
		return false, str[1], 1
	end
	local cfg = self:GetLvUpCfg(id)
	local has = Item.GetNum(cfg.strengthen_consume_item[0].item_id)
	if has < cfg.strengthen_consume_item[0].num then
		return false, str[2], 2, cfg.strengthen_consume_item[0].item_id
	end
	return true
end

-- 判断时装专精是否可以升级
function IllusionData:IsCanSpLvUp()
	local lv = self.illusion_lv_info.info.sp_level
	local str = Language.IllusionUp.Tip
	if lv >= self:IllusionSpMaxLv() then
		return false, str[1], 1
	end
	local cfg = self:GetSpLvUpCfg(nil, lv + 1)
	if cfg and cfg.star_level and cfg.star_level > self.illusion_lv_info.info.total_level then
		return false, str[5], 5
	end
	return true
end

-- 霓裳总红点
function IllusionData:GetRemind()
	for k, v in pairs(self.red_points) do
		if v then
			return 1
		end
	end
	if self:IsCanSpLvUp() then
		return 1
	end
	return 0
end

function IllusionData:GetItemName(item_id)
	local co = Cfg.SurfaceInfoByActiveStuffId(item_id)
	if nil == co or nil == self.fashion_names[co.id] then return end
	return self.fashion_names[co.id][RoleData.Instance:GetRoleAvatarType()]
end

function IllusionData:IsSelectId(fashion_type, id)
	local cmi = self.cur_model_info
	local value = false
	if fashion_type == cmi.type and cmi.fashion[fashion_type] then
		value = id == cmi.fashion[fashion_type].id
	end
	return value
end

function IllusionData:SetIllusionLvInfo(protocol)
	self.illusion_lv_info.info = protocol.info
	self:CalculateTotalLv()
	self:CheckUpRemind()
end

function IllusionData:CalculateTotalLv()
	local total = 0
	for k, v in pairs(self.config_surface_list) do
		local lv = self:IllusionLv(v.id)
		if not self:IsSpecial(v.id) then
			total = total + lv
		end
	end
	self.illusion_lv_info.info.total_level = total
end

-- 时装等级的星星数据
function IllusionData:IllusionLvStarData(lv)
	local list = {}
	local max_star_count = 5
	local max_color_star = 4
	if lv <= 0 or lv > max_star_count * max_color_star then
		return list
	end
	local t1, t2 = math.floor(lv / max_star_count) + 1, lv % max_star_count
	for i = 1, max_star_count do
		if i <= t2 then
			table.insert(list, ({star = t1}))
		elseif t2 == 0 then
			table.insert(list, ({star = t1 - 1}))
		end
	end
	return list
end

-- 获取所有时装基础属性
function IllusionData:IllusionLvAllAttr()
	local list = {}
	local attr_by_key = {}
	for k, v in pairs(self.config_surface_list) do
		local attr = self:GetLvUpAttr(v.id, nil, nil, nil, true)
		for k, v in pairs(attr) do
			if not attr_by_key[v.attr_type] then
				attr_by_key[v.attr_type] = v
			else
				attr_by_key[v.attr_type] = CommonStruct.AttrAdd(v.attr_type, attr_by_key[v.attr_type].attr_value, v.attr_value)
			end
		end
	end
	for k, v in pairs(attr_by_key) do
		table.insert(list, v)
	end
	table.sort(list, function(a, b)
		return a.attr_type < b.attr_type
	end)
    return list
end