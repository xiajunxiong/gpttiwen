
PartnerData = PartnerData or BaseClass()
-- 审核服保留显示的伙伴id
PartnerData.partner_audit_show = {
	[1] = {
		[1] = true,
		[7] = true,
		[21] = true,
	},
	[2] = {
		[10] = true,
		[16] = true,
		[20] = true,
	},
	[3] = {
		[1] = true,
		[3] = true,
		[12] = true,
	},
}

PartnerData.CryStalOpenFormEmun = {
	Normal = 0,
	Mirage = 1,
}

function PartnerData:__init()
	if PartnerData.Instance ~= nil then
		Debuger.LogError("[PartnerData] attempt to create singleton twice!")
		return
    end
    PartnerData.Instance = self

	self:InitConfig()
	self:InitPartnerList()
	self:InitGiftList()

	self:PartnerDataLifeHandBook()

	self.cur_sel_partner = -1
	self.item_flush = SmartData.New{ val = false }
	self.quality_flush = SmartData.New{ val = false }
	self.shut_eff_single = SmartData.New{ val = false }
	Cfg.Register("PartnerScoreInfoByJobLevel", Cfg.RealTimeSearch2Key(Config.score_cfg_auto.partner_score, "job", "level"))
	Cfg.Register("PartnerHoleDescCoByHoleLevel", Cfg.RealTimeSearch(Config.partner_equipment_cfg_auto.hole_desc, "hole_level"))
	Cfg.Register("PartnerLevelUpByIdLevel", Cfg.RealTimeSearch2Key(Config.partner_cfg_auto.partner_levelup, "partner_id", "level"))
    Cfg.Register("PartnerSkinById",Cfg.RealTimeSearch(Config.skin_auto.partner_skin,"partner_skin_id"))
    Cfg.Register("PartnerSoarArmsByIdLevel",Cfg.RealTimeSearch2Key(Config.partner_cfg_auto.soar_arms,"arms_id","arms_level"))
    Cfg.Register("PartnerSoarArmsByIdSkillLevel",Cfg.RealTimeSearch2Key(Config.partner_cfg_auto.soar_arms,"arms_id","skill_level"))
    Cfg.Register("ParnterIntensifyByIdLevel",Cfg.RealTimeSearch2Key(Config.partner_cfg_auto.parnter_intensify,"partner_id","intensify_level"))
	Cfg.Register("ParnterFuYuByLevel", Cfg.RealTimeSearch2Key(Config.fuyu_item_auto, "fuyu_level", "fuyu_type"))
	Cfg.Register("ParnterFuYuByLevel2", Cfg.RealTimeSearch(Config.fuyu_auto.target, "rank"))
	Cfg.Register("ParnterFuYuEffect", Cfg.RealTimeSearch(Config.fuyu_auto.sp_group, "sp_id"))
	Cfg.Register("ParnterFuYuEffectLevel", Cfg.RealTimeSearch2Key(Config.fuyu_auto.sp_group, "sp_group","sp_level"))
	Cfg.Register("ParnterFuYuBelong", Cfg.RealTimeSearch(Config.fuyu_auto.belong_fuyu, "id"))
	Cfg.Register("ParnterFuYuBelongSpId", Cfg.RealTimeSearch(Config.fuyu_auto.belong_fuyu, "sp_id"))
	Cfg.Register("PartnerFuYuDecompose", Cfg.RealTimeSearch(Config.fuyu_auto.fuyu_decompose, "item_id"))
    self.partner_crystal_change = SmartData.New({element_changed = 0})
	self.is_partner_quality_up = false
	self.quality_item_config = KeyList(Config.partner_cfg_auto.parnter_quality_upgrade,"need_item")
	self.is_cur_accpet_partner_soar_task = 0

	self.fuyu_other = Config.fuyu_auto.other[1]
	self.fuyu_flush = SmartData.New()
	self.fuyu_decom_opera = SmartData.New({select = {}})
	self.fuyu_change = SmartData.New()
	self.chang_fuyu_item = SmartData.New()
	self.chang_fuyu_target_item = SmartData.New({item1 = nil, item2 = nil})

	self.sub_fuyu_item = SmartData.New()
	self.sub_fuyu_target_item = SmartData.New({item1 = nil, item2 = nil})

	self.gift_flush = SmartData.New{cur_select = 0,call = 0}
	self.strength_items = SmartData.New{
		index = 1,
		sel_item = nil,
		items = {},
		flush = false,
	}
	

	self.gift_eff_mark = {}
	self.gift_all_info = {}

	self.pact_up_tips_show = true
	self.crystal_open_form = 0
end

function PartnerData:__delete()
end

function PartnerData:ClearData()
	TimeHelper:CancelTimer(self.qua_red_timer)
	TimeHelper:CancelTimer(self.soar_red_timer)
	for _, partner in pairs(self.partner_list) do
		partner:ClearRedPointShow()
	end
end

function PartnerData:InitConfig()
	self.config = Config.partner_cfg_auto

	self.config_partner_info = self.config.partner_info
	self.config_gift_list = self.config.gift_list
	self.config_parnter_quality_upgrade = self.config.parnter_quality_upgrade
	self.config_partner_friendly = self.config.partner_friendly
	self.config_soar = self.config.soar
	self.config_other = self.config.other[1]

	self.pe_config = Config.partner_equipment_cfg_auto
	self.pe_config_partner_equipment = self.pe_config.partner_equipment
	self.pe_config_rand_group = self.pe_config.rand_group
	self.pe_config_att_value = self.pe_config.att_value
	self.pe_config_suit = self.pe_config.suit
	self.pe_config_hole = self.pe_config.partner_hole

	self.skin_config = Config.skin_auto.partner_skin

	self.quality_up_info = {}

	self.activation_item_config = KeyList(Config.partner_cfg_auto.partner_info,"activation_item")
	self.info_from_client_list = {}
	self.partner_id_config = {}
	self.partner_soar_task_ids = {}
	self.partner_soar_task_ids2 = {}
	for k, info in pairs(self.config_partner_info) do
		local config_info = {}
		config_info.activation_item = info.activation_item
		config_info.partner_id = info.partner_id
		config_info.partner_quality = info.partner_quality
		config_info.activation_num = info.activation_num
		table.insert(self.partner_id_config,config_info)
		table.insert(self.partner_soar_task_ids, info.soar_fin_task)
		self.partner_soar_task_ids2[info.soar_fin_task] = info.partner_id
	end

	self.partner_quality_us_config = {}
	self.partner_quality_sl_config = {}
	for _, info in pairs(self.config_parnter_quality_upgrade) do
		self.partner_quality_us_config[info.partner_id] = self.partner_quality_us_config[info.partner_id] or {}
		self.partner_quality_us_config[info.partner_id][info.quality_upgrade] = self.partner_quality_us_config[info.partner_id][info.quality_upgrade] or {}
		self.partner_quality_us_config[info.partner_id][info.quality_upgrade][info.quality_segment] = info
		for i = 1, PartnerConfig.PARTNER_SKILL_AN_MAX do
			self.partner_quality_sl_config[info.partner_id] = self.partner_quality_sl_config[info.partner_id] or {}
			self.partner_quality_sl_config[info.partner_id][i] = self.partner_quality_sl_config[info.partner_id][i] or {}
			local sl_key = "skill_level_" .. i
			if self.partner_quality_sl_config[info.partner_id][i][info[sl_key]] then
				local sl = self.partner_quality_sl_config[info.partner_id][i][info[sl_key]]
				if info.quality_upgrade < sl.quality_upgrade and sl.quality_upgrade < sl.quality_segment then
					self.partner_quality_sl_config[info.partner_id][i][info[sl_key]] = info
				end
			else
				self.partner_quality_sl_config[info.partner_id][i][info[sl_key]] = info
			end
		end
	end

	self.partner_friendly_seq_config = {}
	self.partner_friendly_pl_config = {}
	self.partner_friendly_max_level = 0
	for _, info in pairs(self.config_partner_friendly) do
		self.partner_friendly_seq_config[info.seq] = info
		self.partner_friendly_pl_config[info.partner_id] = self.partner_friendly_pl_config[info.partner_id] or {}
		self.partner_friendly_pl_config[info.partner_id][info.level] = info
		self.partner_friendly_max_level = math.max(self.partner_friendly_max_level, info.level)
	end

	self.partner_aio_list = {}
	for _, info in pairs(self.config.partner_info) do
		self.partner_aio_list[info.activation_item_norex] = info
	end

	self.partner_soar_count_list = {}
	for _, info in pairs(self.config_soar) do
		self.partner_soar_count_list[info.soar_frequency] = info
	end

	Cfg.Register("PEByRandGroupCoAttrGroup",Cfg.RealTimeSearch(self.pe_config_rand_group,"att_group"))

	self.inborn_base_list = {}
	for k,v in pairs(Config.partner_inborn_auto.inborn_base) do 
		if self.inborn_base_list[v.inborn_type] == nil then 
			self.inborn_base_list[v.inborn_type] = {}
		end 
		if self.inborn_base_list[v.inborn_type][v.inborn_hole_id] == nil then 
		    self.inborn_base_list[v.inborn_type][v.inborn_hole_id] = {}
		end 
		self.inborn_base_list[v.inborn_type][v.inborn_hole_id].info = v
	end 

	for k,v in pairs(Config.partner_inborn_auto.inborn_level) do 
		if self.inborn_base_list[v.inborn_type] == nil then 
			self.inborn_base_list[v.inborn_type] = {}
		end 
		if self.inborn_base_list[v.inborn_type][v.inborn_hole_id] == nil then 
			self.inborn_base_list[v.inborn_type][v.inborn_hole_id] = {}
		end 
		if self.inborn_base_list[v.inborn_type][v.inborn_hole_id].level_list == nil then 
			self.inborn_base_list[v.inborn_type][v.inborn_hole_id].level_list = {}
		end 

		self.inborn_base_list[v.inborn_type][v.inborn_hole_id].level_list[v.inborn_hole_level] = v
	end 

	self.inborn_sp_effect= {}
	for k,v in pairs(Config.partner_inborn_auto.inborn_effect) do
		if self.inborn_sp_effect[v.inborn_type] == nil then 
			self.inborn_sp_effect[v.inborn_type] = {}
		end 
		self.inborn_sp_effect[v.inborn_type][v.sp_hole] = v
    end

	self.partner_intensify_pre_show_list = {}
	self.partner_intensify_add_show_list = {}

	self.partner_soar_weapon_consumes = {}
	for k, v in ipairs(Config.partner_cfg_auto.soar_arms) do
		if v.item_list then
			for y, z in pairs(v.item_list) do
				self.partner_soar_weapon_consumes[z.item_id] = true
			end
		end
	end
	self.partner_fuyu_exc_list = {}
	self.partner_fuyu_prof_list = {}
	local prof = 0
	for i, v in ipairs(Config.fuyu_auto.sp_group) do
		if v.limit_type == 2 then
			table.insert(self.partner_fuyu_exc_list, v)
		elseif v.limit_type == 0 then --通用
			if self.partner_fuyu_prof_list[0] == nil then
				self.partner_fuyu_prof_list[0] = {}
			end
			table.insert(self.partner_fuyu_prof_list[0], v)
		elseif v.limit_type == 1 then
			prof = v.param1 / 100
			if self.partner_fuyu_prof_list[prof] == nil then
				self.partner_fuyu_prof_list[prof] = {}
			end
			table.insert(self.partner_fuyu_prof_list[prof], v)
		end
	end
end

function PartnerData:InitPartnerList()
	self.partner_list = {}
	
	self.fight_order_list = SmartData.New()
	for _, info in pairs(self.config_partner_info) do
		self.partner_list[info.partner_id] = SmartData.New(PartnerInfo.New{info = info})
	end
	
	self.partner_pe_detail_list = {}
	for _, info in pairs(self.pe_config_partner_equipment) do
		self.partner_pe_detail_list[info.item_id] = info
	end
	self.partner_pe_suit_list = {}
	for _, info in pairs(self.pe_config_suit) do
		self.partner_pe_suit_list[info.index] = info
	end
	self.partner_pe_hole_list = {}
	for _, info in pairs(self.pe_config_hole) do
		self.partner_pe_hole_list[info.partner_id] = self.partner_pe_hole_list[info.partner_id] or {}
		self.partner_pe_hole_list[info.partner_id][info.hole_postion] = self.partner_pe_hole_list[info.partner_id][info.hole_postion] or {}
		self.partner_pe_hole_list[info.partner_id][info.hole_postion][info.hole_level] = info
	end

	self.cur_sel_tabbar = SmartData.New{ val = PartnerConfig.PartnerViewTabs.None }
	self.partner_acti_red_point = SmartData.New{ val = false }
	self.partner_up_red_point = SmartData.New{ val = false }
	self.partner_crystal_red_point = SmartData.New{ val = false }
	self.partner_skill_red_point = SmartData.New{ val = false }
	self.partner_life_red_point = SmartData.New{ val = false }
	self.partner_qua_red_point = SmartData.New{ val = false }
	self.partner_attr_level_red_point = SmartData.New{ val = false }
	self.partner_gift_red_point = SmartData.New{ val = false } 
	self.partner_biography_red_point = SmartData.New{ val = false }
	self.partner_life_effect = SmartData.New{ pos = 0 }
	self.partner_up_skillf_effect = SmartData.New{val = false}
	self.partner_attr_up_level_effect = SmartData.New{val = false}
	self.partner_pact_equip_red_point = SmartData.New{ val = false }
	self.partner_pact_up_red_point = SmartData.New{ val = false }
	self.partner_soar_red_point = SmartData.New{ val = false }
	self.partner_soar_weapon_red_point = SmartData.New{ val = false }
	self.partner_strengthen_red_point = SmartData.New{ val = false }

	self.partner_pe_list = {}
	for pt = 1, 8 do
		self.partner_pe_list[pt] = {}
		for i = 1, PartnerConfig.PARTNER_PE_MAX do
			self.partner_pe_list[pt][i] = {
				num = 0,
				min_level = 0,
			}
		end
	end

	self.partner_quality_list = {}
	self.partner_quality_max = {}
	for k ,v in pairs(self.config.parnter_quality_upgrade) do 
		if self.partner_quality_list[v.partner_id] == nil then 
			self.partner_quality_list[v.partner_id] = {}
		end 
		-- if v.need_item > 0 then 
			if self.partner_quality_list[v.partner_id][v.quality_upgrade] == nil then 
				self.partner_quality_list[v.partner_id][v.quality_upgrade] = {}
			end 
		    self.partner_quality_list[v.partner_id][v.quality_upgrade][v.quality_segment] = v

			self.partner_quality_max.upgrade = v.quality_upgrade + PartnerConfig.QuaFetter.Grassy
			self.partner_quality_max.segment = v.quality_segment
		-- end 
	end


	self.partner_crystal_list = {[1] = {item_id = 0, name = Language.Partner.Crystal.all}}
	for _, co in pairs(Config.element_crystal_cfg_auto) do
		if co.id <= 40003 then		--TEMPCODE 临时代码去掉混合水晶在伙伴水晶选择界面上的显示
			table.insert(self.partner_crystal_list, {
				item_id = co.id,
				name = co.name,
			})
		end
	end
	table.sort(self.partner_crystal_list, function (a, b)
		if not a or not b then return a end
		return a.item_id < b.item_id
	end)
	self.cur_sel_crystal = SmartData.New{
		item_id = 0,
		flush = false,
		partner = -1,
		role = 0,
	}
	self.arena_match_id_list = {}

	self.partner_skins = {}
	self.partner_skin_ids = {}
	self.partner_skin_actived = {}
 	for k, v in ipairs(self.skin_config) do
		self.partner_skins[v.partner_id] = self.partner_skins[v.partner_id] or {}
		table.insert(self.partner_skins[v.partner_id], v)
		self.partner_skin_ids[v.partner_skin_id] = v
	end
	self.cur_sel_skin = SmartData.New{
		skin_id = 0
	}
	self.cur_biography_info = SmartData.New{
		biography = 0,
		page = 0,
		flush_get = false,
	}

	self.biography_show = SmartData.New{
		partner = 0,
		biography = 0,
	}

	self.partner_pact_items = SmartData.New{}
	self.partner_pact_show_info = SmartData.New{
		sort_type = 1,
		sel_pos = 0,
		sel_item = SmartData.New{},
	}
	self.partner_pact_up_effect = SmartData.New{
		type = 0,
		partner = 0,
		pos = 0,
	}
	self.partner_soar_weapon_effect = SmartData.New{val = false}

	self.partner_strengthen_up_effect = SmartData.New{partner = 0}
end

function PartnerData:InitGiftList()
	self.gift_list = {}
	for _, v in pairs(self.config_gift_list) do
		self.gift_list[v.gift_id] = self.gift_list[v.gift_id] or {}
		self.gift_list[v.gift_id][v.gift_level] = self.gift_list[v.gift_id][v.gift_level] or {}
		self.gift_list[v.gift_id][v.gift_level] = v
	end
end

function PartnerData:SetPartnerAllInfo(protocol)
	-- for i = 1, protocol.count do
	-- 	local vo = protocol.partner_list[i]
	-- 	if self.partner_list[vo.partner] ~= nil then self.partner_list[vo.partner]:PartnerVo(vo) end
	-- end
	for _, partner in pairs(self.partner_list) do
		local vo = protocol.partner_list[partner.id]
		if vo == nil then
			vo = partner:EmptyVo()
			self.info_from_client_list[partner.id] = true
		end
		self.partner_list[partner.id]:PartnerVo(vo)
	end
	self:PartnerActiRedPoint()
	self:PartnerUpRedPoint()
	self:PartnerCrystalRedPoint()
	self:PartnerSkillRedPoint()
	self:PartnerLifeRedPoint()
	self:PartnerQuaRedPoint()
	self:PartnerGiftRedPoint()
	self:PartnerAttrLevelRedPoint()
	self:PartnerBiographyRedPoint()
	self:PartnerPactEquipRedPoint()
	self:PartnerSoarRedPoint()
	self:PartnerSoarWeaponRedPoint()
	self:SetArenaMatchIdList(self.arena_match_id_list)
	-- self:PartnerGuide()
	TeamData.Instance:NeedFlushMenu()
	self.guide_partner = 0
end

function PartnerData:SetPartnerSingleInfo(protocol)
	local vo = protocol.partner_info
	self.info_from_client_list[vo.partner] = nil
	self.partner_list[vo.partner]:PartnerVo(vo)
	self:PartnerUpRedPoint()
	self:PartnerAttrLevelRedPoint()
	self:PartnerBiographyRedPoint()
	self:PartnerSoarRedPoint()
	self:PartnerStrengthenRedPoint()
	-- self:PartnerSoarWeaponRedPoint()
end

function PartnerData:IsInfoFromClient(partner_id)
	return self.info_from_client_list[partner_id] == true
end

function PartnerData:SetChangeFightState(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:ChangeFightState(protocol)
	self:PartnerActiRedPoint()
	self:PartnerSkillRedPoint()
	self:PartnerLifeRedPoint()
	self:PartnerQuaRedPoint()
	self:PartnerGiftRedPoint()
	self:PartnerUpRedPoint()
	self:PartnerCrystalRedPoint()
	self:PartnerAttrLevelRedPoint()
	self:PartnerPactUpRedPoint()
	self:PartnerStrengthenRedPoint()

	if 1 == protocol.is_fight then
		FormationData.Instance:TopLayerShowPartner(protocol.partner)
	end

	-- if self.partner_list[protocol.partner].vo.fight_state == 1 then
		RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
	-- end
end

function PartnerData:SetPartnerAttrListInfo(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerAttrListInfo(protocol)
	TeamData.Instance:NeedFlushMenu()
	if self.partner_list[protocol.partner].vo.fight_state == 1 then
		RoleData.Instance.role_total_score.flush_score = not RoleData.Instance.role_total_score.flush_score
	end
end

function PartnerData:SetPartnerFetter(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerFetter(protocol)
end

function PartnerData:SetPartnerBaseGradeInfo(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerBaseGradeInfo(protocol)
	self.up_success_switch = protocol.up_ret > 0
	self:PartnerUpRedPoint()
	self:PartnerBiographyRedPoint()
end

function PartnerData:SetPartnerSkill(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerSkill(protocol)
	self:PartnerSkillRedPoint()
end

function PartnerData:SetFightOrderList(protocol)
	self.fight_order_list:Set(protocol.fight_list)
	TeamData.Instance.sort_index_first.val = -2
	TeamData.Instance:NeedFlushMenu()
end

function PartnerData:SetPartnerEqSingleSlots(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerEquip(protocol)
	self:PartnerLifeRedPoint()
end

function PartnerData:GetPartnerAllInfo()
	return self.partner_list
end

function PartnerData:GetPartnerAllInfoLockSort()
	local function sort_lock(a,b)
		local av = a.vo
		local bv = b.vo
		if av.is_unlocked == bv.is_unlocked then
			if av.fight_state == bv.fight_state then
				local arp = a.acti_red_point.val and 1 or 0
				local brp = b.acti_red_point.val and 1 or 0
				if arp == brp then
					if a.partner_quality == b.partner_quality then
						return a.info.sort < b.info.sort
					end
					return a.partner_quality > b.partner_quality
				end
				return arp > brp
			end
			return av.fight_state > bv.fight_state
		end
		return av.is_unlocked > bv.is_unlocked
	end
	local list = {}
	for _, info in pairs(self.partner_list) do
		if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
			local audit_seq = PartnerData.partner_audit_show[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
			if PartnerData.partner_audit_show[audit_seq][info.id] then
				table.insert(list, info)
			end
		else
			table.insert(list, info)
		end
	end
	table.sort(list, sort_lock)
	return list, list[1].id
end

function PartnerData:GetPartnerInfoById(id)
	return self.partner_list[id]
end

function PartnerData:GetPartnerGiftWithIdAndLevel(gift_id, level)
	return self.gift_list[gift_id][level]
end

function PartnerData:GetPartnerInfoLockSortPos(item_list,partner_id)
	for k ,v in pairs(item_list) do 
		if v.info.partner_id == partner_id then 
			return k
		end 
	end
	return 1
end

function PartnerData:GetPartnerFightOrderList()
	return self.fight_order_list
end

function PartnerData:GetPEConfigByItemId(item_id)
	if 0 == item_id then return end
	local detail = self.partner_pe_detail_list[item_id]
	local index = detail.index
	local suit = self.partner_pe_suit_list[index]
	return detail, suit
end

function PartnerData:PartnerActiRedPoint()
	local arp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:ActiRedPoint()
		arp = arp or val
	end
	self.partner_acti_red_point.val = arp
end

function PartnerData:PartnerUpRedPoint()
	local urp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:UpRedPoint()
		urp = urp or val
	end
	self.partner_up_red_point.val = urp
	BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.HuoBanKeJinSheng)
end
function PartnerData:FightPartnerUpRedPoint()
	for _, partner in pairs(self.partner_list) do
		if partner:UpRedPoint(true) then
			return 1, partner
		end
	end
	return 0
end


function PartnerData:PartnerCrystalRedPoint()
	local crp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:CrystalRedPoint()
		crp = crp or val
	end
	self.partner_crystal_red_point.val = crp
end

function PartnerData:PartnerAttrLevelRedPoint()
	local alrp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:AttrLevelRedPoint()
		alrp = alrp or val
	end
	self.partner_attr_level_red_point.val = alrp
	BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.HuoBanKeShengJi)
end
function PartnerData:FightPartnerAttrLevelRedPoint()
	local alrp = false
	for _, partner in pairs(self.partner_list) do
		if partner:AttrLevelRedPoint(true) then
			return 1, partner
		end
	end
	return 0
end

function PartnerData:PartnerBiographyRedPoint()
	local brp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:BiographyRedPoint()
		brp = brp or val
	end
	self.partner_biography_red_point.val = brp
end

function PartnerData:PartnerSkillRedPoint()
	local srp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:SkillRedPoint()
		srp = srp or val
	end
	self.partner_skill_red_point.val = srp
end

function PartnerData:PartnerLifeRedPoint()
	local lrp = false
	for _, partner in pairs(self.partner_list) do
		local lrpa = partner:LifeActiveRedPoint()
		local lrpe = partner:LifeEquipRedPoint()
		lrp = lrp or (lrpa or lrpe)
	end
	self.partner_life_red_point.val = lrp
end

function PartnerData:PartnerPactEquipRedPoint()
	local crp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:PactEquipRedPoint()
		crp = crp or val
	end
	self.partner_pact_equip_red_point.val = crp
end

function PartnerData:PartnerPactUpRedPoint()
	local crp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:PactUpRedPoint()
		crp = crp or val
	end
	self.partner_pact_up_red_point.val = crp
end

function PartnerData:PartnerSoarRedPoint()
	local srp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:SoarRedPoint()
		srp = srp or val
	end
	self.partner_soar_red_point.val = srp
	BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.HuoBanKeFeiSheng)
end

function PartnerData:PartnerStrengthenRedPoint()
	local srp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:StrengthenRedPoint()
		srp = srp or val
	end
	if self.partner_strengthen_red_point.val == srp then
		self.partner_strengthen_red_point:SetDirty("val")
	else
		self.partner_strengthen_red_point.val = srp
	end
end

function PartnerData:FightPartnerSoarRedPoint()
	for _, partner in pairs(self.partner_list) do
		if partner:IsFight() then
			if partner:SoarRedPoint() then
				return 1, partner
			end
		end
	end
	return 0
end

function PartnerData:PartnerSoarWeaponRedPoint()
	local swrp = false
	for _, partner in pairs(self.partner_list) do
		local val = partner:SoarWeaponRedPoint()
		swrp = swrp or val
	end
	self.partner_soar_weapon_red_point.val = swrp
end

function PartnerData:PartnerQuaRedPoint()
	local red = false
	for _, partner in pairs(self.partner_list) do
		local red_in = partner:QualityRedPoint()
		red = red or red_in
	end
	self.partner_qua_red_point.val = red
	BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.HuoBanKeJueXing)
end
function PartnerData:FightPartnerQuaRedPoint()
	for _, partner in pairs(self.partner_list) do
		if partner:QualityRedPoint(true) then
			return 1, partner
		end
	end
	return 0
end

function PartnerData:PartnerGiftRedPoint()
	local red = false
	for _, partner in pairs(self.partner_list) do
		local red_in = partner:GiftRedPoint()
		red = red or red_in
	end
	self.partner_gift_red_point.val = red
end

function PartnerData:PartnerLifeCalc()
	for pt = 1, 8 do
		self.partner_pe_list[pt] = {}
		for i = 1, PartnerConfig.PARTNER_PE_MAX do
			local list, min_level, max_level = BagData.Instance:GetPartnerEquipGrid(i, pt)
			self.partner_pe_list[pt][i] = {
				num = #list,
				min_level = min_level,
				max_level = max_level,
			}
		end
	end
end

function PartnerData:GetPartnerPEList(job)
	return self.partner_pe_list[job]
end

function PartnerData:PartnerGuide()
	self.guide_partner = 0
	if self.guide_handle then
		BagData.Instance:UnCare(self.guide_handle)
		self.guide_handle = nil
	end
	self.need_guide = true
	for _, info in pairs(self.partner_list) do
		self.need_guide = self.need_guide and info:NeedGuide()
	end
	if not self.need_guide then return end
	self.guide_handle = BagData.Instance:Care(BindTool.Bind(self.CheckGuide, self))
end

function PartnerData:CheckGuide()
	-- if not self.need_guide then return end
	for _, info in pairs(self.partner_list) do
		if info:Guide() then
			if self.guide_handle then
				BagData.Instance:UnCare(self.guide_handle)
				self.guide_handle = nil
			end
			self.guide_partner = info.id
			GuideManager.Instance:Start(33)
			return
		end
	end
end

function PartnerData:GetPEHoleCo(partner_id, hole_postion, hole_level)
	-- hole_level = 0 == hole_level and 50 or hole_level + 10
	hole_level = PartnerConfig.HOLE_LOCK_LEVEL
	local succ = false
	local pis = self.partner_pe_hole_list[partner_id]
	local partner = self.partner_list[partner_id]
	if partner and PartnerConfig.LockState.lock == partner.vo.is_unlocked then
		PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
		return succ
	end
	if nil == pis then
		LogError(string.format("partner_equipment_cfg_auto hole partner_id=%s 配置不存在！", partner_id))
		return succ
	end
	local hps = pis[hole_postion]
	if nil == hps then
		LogError(string.format("partner_equipment_cfg_auto hole partner_id=%s hole_postion=%s 配置不存在！", partner_id, hole_postion))
		return succ
	end
	local co = hps[hole_level]
	if nil == co then
		LogError(string.format("partner_equipment_cfg_auto hole partner_id=%s hole_postion=%s hole_level=%s 配置不存在！", partner_id, hole_postion, hole_level))
		return succ
	end
	succ = true
	return succ, co
end

function PartnerData:SetPartnerQualityInfo(param_t)
	if param_t.partner_id == nil then return end

	if self.partner_list[param_t.partner_id] then
		self.partner_list[param_t.partner_id]:SetPartnerQualityInfo(param_t)
	end

	if self.quality_up_info[param_t.partner_id] == nil then 
		self.quality_up_info[param_t.partner_id] = {}
	end 

	if param_t.notice_type == 1 then 
		self.quality_up_cache = param_t
	else
		self.quality_up_info[param_t.partner_id].up_type = param_t.up_type
		self.quality_up_info[param_t.partner_id].quality = param_t.quality
		self.quality_up_info[param_t.partner_id].operate_qua = PartnerConfig.ChangeServerQua[param_t.quality]
		self.quality_up_info[param_t.partner_id].attr_list = param_t.attr_list
		self.quality_up_info[param_t.partner_id].quality_segment = param_t.quality_segment
	
		self.quality_flush.val = not self.quality_flush.val
	end 

	if param_t.notice_type == 1 then 
		self.partner_list[param_t.partner_id].quality_up.is_show = true
		self.new_partner = param_t.partner_id
		self.is_quality = true
		ViewMgr:OpenView(PartnerViewLock)
	elseif param_t.notice_type == 2 and not ViewMgr:IsOpen(TreasureHuntView) and not ViewMgr:IsOpen(TreasureHuntDetailView) then
		self.partner_list[param_t.partner_id].quality_up.is_show = false
		self.new_partner = param_t.partner_id
		ViewMgr:OpenView(PartnerViewLock)
	end

	--如果很短时间接到多次下发，以最后一次下发刷新一次,防止激活家族灵脉一下刷新全部伙伴，会卡一会。
	TimeHelper:CancelTimer(self.qua_red_timer)
	self.qua_red_timer = TimeHelper:AddDelayTimer(BindTool.Bind1(self.PartnerQuaRedPoint, self), 0.2)
	-- self:PartnerQuaRedPoint()
	TimeHelper:CancelTimer(self.soar_red_timer)
	self.soar_red_timer = TimeHelper:AddDelayTimer(BindTool.Bind1(self.PartnerSoarRedPoint, self), 0.2)
end

function PartnerData:SetNewPartner(partner_id, unlock)
	self.partner_list[partner_id].quality_up.is_show = false
	self.new_partner = partner_id
	self.force_unlock = unlock or false
end

-- 专用合并逻辑·结算界面
function PartnerData:QualityUpLockInfoShow(partner_id)
	local attr_param = {}

	local info = self.quality_up_info[partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	local cache = self.quality_up_cache
	attr_param.attr_list = {} 
	attr_param.attr_neo_list = {}

	for k,v in pairs(PartnerConfig.QuaAttrType) do 
		attr_param.attr_neo_list[k] = self.quality_up_cache.attr_list[v]
		attr_param.attr_list[k] = info.attr_list[v]
	end 

	local item_show = {}
	item_show.old = {	
		quality = PartnerConfig.ChangeServerQua[info.quality],
		quality_segment = info.quality_segment,
	}

	item_show.new = {	
		quality = PartnerConfig.ChangeServerQua[self.quality_up_cache.quality],
		quality_segment = self.quality_up_cache.quality_segment,
	}

	local skill_list = self:GetPartnerQualityPopSkillShow(self.new_partner,PartnerConfig.ChangeServerQua[self.quality_up_cache.quality],
	    self.quality_up_cache.quality_segment)
	
	if self.qua_up_timer ~= nil then
		TimeHelper:CancelTimer(self.qua_up_timer)
		self.qua_up_timer = nil
	end

	self.qua_up_timer = TimeHelper:AddDelayTimer(function()
	    self:PartnerQualityCacheOut()
    end, 0.2)
	return attr_param,item_show,skill_list
end

function PartnerData:PartnerQualityCacheOut()
	if self.quality_up_cache == nil then return end 
	self.quality_up_info[self.quality_up_cache.partner_id].up_type = self.quality_up_cache.up_type
	self.quality_up_info[self.quality_up_cache.partner_id].quality = self.quality_up_cache.quality
	self.quality_up_info[self.quality_up_cache.partner_id].operate_qua = PartnerConfig.ChangeServerQua[self.quality_up_cache.quality]
	self.quality_up_info[self.quality_up_cache.partner_id].attr_list = self.quality_up_cache.attr_list
	self.quality_up_info[self.quality_up_cache.partner_id].quality_segment = self.quality_up_cache.quality_segment

	self.quality_flush.val = not self.quality_flush.val

	self.quality_up_cache = nil

	self:ShutQuaEff()
	
	self:PartnerQuaRedPoint()
end

function PartnerData:PartnerQualityIsCaching(  )
	return self.quality_up_cache ~= nil
end

function PartnerData:ShutQuaEff()
	self.shut_eff_single.val = not self.shut_eff_single.val 
end

function PartnerData:SetPartnerUseElementCrystal(protocol)
	for i = 1, protocol.count do
		local info = protocol.info_list[i]
		local partner_info = self.partner_list[info.partner]
		if partner_info then
			local changed = false
			if partner_info.vo.crystal ~= info.crystal then
				changed = true
			end
			partner_info:PartnerUseElementCrystal(info)
			if changed then
				self.partner_crystal_change:SetDirty("element_changed")
			end
		end
    end
	self.cur_sel_crystal.flush = not self.cur_sel_crystal.flush
	self:PartnerCrystalRedPoint()
end

function PartnerData:SetPartnerSkinActive(protocol)
	local skin_list = protocol.skin_list
	local partner_skin_actived = {}
	for k, v in pairs(Config.skin_auto.partner_skin) do
		local skin_active_status = skin_list[v.partner_id] or 0
		partner_skin_actived[v.partner_skin_id] = 1 == (bit:_rshift(skin_active_status, v.seq) % 2)
	end
	self.partner_skin_actived = partner_skin_actived
end

function PartnerData:SetPartnerSkinSingleOper(protocol)
	local info = protocol.info
	local partner = self.partner_list[info.partner_id]
	if PartnerConfig.SkinOperType.active == info.type then
		self.partner_skin_actived[info.skin_id] = true
		AppearanceCtrl.Instance:OpenAppearanceShowPartner(info.skin_id)
	elseif PartnerConfig.SkinOperType.equip == info.type then
		if partner then
			partner:SetPartnerSkinId(info.skin_id)
		end
	elseif PartnerConfig.SkinOperType.remove == info.type then
		partner:SetPartnerSkinId(0)
	end
	TeamData.Instance:NeedFlushMenu()
end

function PartnerData:GetPartnerQuaListById(partner_id)
	local qua_info = self.quality_up_info[partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	local qua_list = self.partner_quality_list[partner_id]

	local result = {}
	local index = 1
	for k,v in pairs(qua_list) do 
		for i,j in pairs(v) do 
			local vo = {}
			vo.index = index 

			vo.cfg = j
			vo.is_cur = qua_info.quality_segment == j.quality_segment and qua_info.operate_qua == j.quality_upgrade
			vo.is_pre = qua_info.operate_qua < j.quality_upgrade or (qua_info.operate_qua == j.quality_upgrade and qua_info.quality_segment < j.quality_segment) 
			    or self.partner_list[partner_id].vo.is_unlocked == PartnerConfig.LockState.lock

			vo.is_max = qua_list[j.quality_upgrade+1] == nil and qua_list[j.quality_upgrade][j.quality_segment + 1] == nil 
			table.insert(result,vo)
			index = index + 1
		end 
	end 

	table.sort(result, function(a, b)
        local itemA = a.cfg.quality_segment + a.cfg.quality_upgrade * 10
        local itemB = b.cfg.quality_segment + b.cfg.quality_upgrade * 10
        if itemB > itemA then
			return true
        else
			return false
        end
    end)
	return result
end 

function PartnerData:GetPartnerQuaShowById(partner_id)
	local list = self:GetPartnerQuaListById(partner_id)
	local pos_cur = nil
	local pos_next = nil
	local pos_after = nil
	for k,v in pairs(list) do 
		if v.is_cur then 
			pos_cur = v
			pos_next = list[k + 1]
			pos_after = list[k + 2]
		end 
	end 

	return pos_cur,pos_next,pos_after
end

--返回的配置里是否需要attr_list 默认为false不需要,为配置添加attr_list比较费性能
function PartnerData:GetPartnerQualityInfoById(partner_id,need_attr)

	local qua_info = self.quality_up_info[partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	if qua_info == nil then return nil,nil end 
	local partner_config =  self.partner_list[partner_id]
	local qua_cfg = {}
	-- local attr_list = {}

	qua_cfg.o_cfg = self.partner_quality_list[partner_id][qua_info.operate_qua][qua_info.quality_segment]
	if need_attr then
		qua_cfg.attr_list = self:GetPartnerQualityAttrList(partner_id,qua_info.operate_qua,qua_info.quality_segment)
	end
	qua_cfg.is_max = qua_info.quality == self.partner_quality_max.upgrade and qua_info.quality_segment == self.partner_quality_max.segment

	return qua_info,qua_cfg
end

function PartnerData:GetPartnerQualityAttrList(partner_id,operate_qua,operate_segment)
	local qua_info = self.quality_up_info[partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	local o_cfg = self.partner_quality_list[partner_id][operate_qua][operate_segment]
	local temp_attr_list = DataHelper.ResetAttrList(o_cfg)

	local temp_main_list = Split(o_cfg.main_att,"|")
	local attr_list = {}

	for k,v in pairs(temp_attr_list) do 
		local vo = {}
		vo.flag_main = false
		for i,j in pairs(temp_main_list) do
			if tonumber(j) == k then vo.flag_main = true end 
		end 
		vo.attr_type = k
		vo.attr_plus = v
		vo.attr_total = qua_info.attr_list[PartnerConfig.QuaAttrType[vo.attr_type]]
		vo.is_max = qua_info.quality == self.partner_quality_max.upgrade and 
			qua_info.quality_segment == self.partner_quality_max.segment
		table.insert( attr_list, vo )
	end 

	return attr_list
end

function PartnerData:GetPartnerQualityPopSkillShow(partner_id,next_qua,next_seg)
	local c_cfg = self.partner_quality_list[partner_id][next_qua][next_seg]
	local qua_list = self:GetPartnerQuaListById(partner_id)
	local skill_info_1,skill_info_2 = self:GetPartnerInfoById(partner_id):CheckSkillInfo()

	local list = {}
	local p_cfg = nil 
	for k,v in pairs(qua_list) do 
		if v.cfg.quality_upgrade == c_cfg.quality_upgrade and v.cfg.quality_segment == c_cfg.quality_segment then 
			if k > 1 then p_cfg = qua_list[k - 1].cfg end 
			break
		end 
	end 

	for k,v in pairs(skill_info_2) do 
		local vo = {}
		vo.per_info = p_cfg ~= nil and v.info[p_cfg["skill_level_"..k]] or nil
		vo.info = v.info[c_cfg["skill_level_"..k]]
		if vo.info ~= nil and ((vo.per_info == nil and c_cfg["skill_level_"..k] > 0 ) 
		    or (vo.per_info ~= nil and p_cfg["skill_level_"..k] ~= c_cfg["skill_level_"..k])) then 
			table.insert(list,vo)
		end 
	end 
	return list
end 

-- 注意这里获取的是当前阶和下一阶
function PartnerData:GetPartnerQualitySkillShow(partner_id)
	local qua_info = self.quality_up_info[partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	local qua_list = self:GetPartnerQuaListById(partner_id)
	local skill_info_1,skill_info_2 = self:GetPartnerInfoById(partner_id):CheckSkillInfo()

	local list = {}
	local c_cfg = self.partner_quality_list[partner_id][qua_info.operate_qua][qua_info.quality_segment]
	local n_cfg = nil 
	for k,v in pairs(qua_list) do 
		if v.cfg.quality_upgrade == c_cfg.quality_upgrade and v.cfg.quality_segment == c_cfg.quality_segment then 
		    n_cfg = qua_list[k + 1] ~= nil and qua_list[k + 1].cfg or nil
			break
		end 
	end 
	for k,v in pairs(skill_info_2) do 
		local vo = {}
		vo.per_info = c_cfg ~= nil and v.info[c_cfg["skill_level_"..k]] or nil
		vo.info = n_cfg ~= nil and v.info[n_cfg["skill_level_"..k]] or nil
		if vo.info ~= nil and c_cfg["skill_level_"..k] ~= n_cfg["skill_level_"..k] then 
			table.insert(list,vo)
		end 
	end 
	return list
end

function PartnerData:GetPartnerQualityCfg(partner_id,qua,seg)
	return self.partner_quality_list[partner_id][qua][seg]
end

function PartnerData:GetPartnerQualityUpDesc(partner_id)
	local qua_list = self:GetPartnerQuaListById(partner_id)
	local pos_cur = nil
	local pos_next = nil
	for k,v in pairs(qua_list) do 
		if v.is_cur then 
			pos_cur = v
			pos_next = qua_list[k+1]
			break
		end 
	end 
	local name_cur = string.format( Language.Common.Xstr,
	PartnerQualityDeepColor[pos_cur.cfg.quality_upgrade + PartnerConfig.QuaFetter.Grassy],
	Language.Partner.QualityName[pos_cur.cfg.quality_upgrade]..string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(pos_cur.cfg.quality_segment)))

	local name_next = pos_next ~= nil and string.format( Language.Common.Xstr,
	PartnerQualityDeepColor[pos_next.cfg.quality_upgrade + PartnerConfig.QuaFetter.Grassy],
	Language.Partner.QualityName[pos_next.cfg.quality_upgrade]..string.format(Language.Partner.QualitySeg,
	DataHelper.GetDaXie(pos_next.cfg.quality_segment))) or nil
	return name_cur,name_next
end

function PartnerData.GetPartnerQualityName(quality_upgrade,quality_segment)
	local name = string.format( Language.Common.Xstr,
	PartnerQualityDeepColor[quality_upgrade + PartnerConfig.QuaFetter.Grassy],
	Language.Partner.QualityName[quality_upgrade]..string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(quality_segment)))
	return name or ""
end

function PartnerData:GetPartnerQualityInfoByItemId(item_id)
	if self.quality_item_config[item_id] then 
		return  self.quality_item_config[item_id].partner_id
	end 

	return -1
end

function PartnerData:GetPartnerActivationInfoByItemId(item_id)
	if self.activation_item_config[item_id] then 
		return  self.activation_item_config[item_id].partner_id
	end 

	return -1
end

function PartnerData:GetPartnerQualityShouldJump(item_id)
	local quality_cfg = self.quality_item_config[item_id]
	local quality_info = self.quality_up_info[quality_cfg.partner_id] or PartnerData.EmptyQualityUp(self.partner_list[partner_id])
	local flag_1 = quality_cfg.quality_upgrade == (quality_info.quality - PartnerConfig.QuaFetter.Grassy)
	local flag_2 = self.partner_list[quality_cfg.partner_id].vo.is_unlocked == PartnerConfig.LockState.unlock

	if not flag_2 then return false,Language.Partner.UnLockTips end 
	if not flag_1 then return false,Language.Partner.CantQualityOverTips end 

	return true,""
end

function PartnerData:GetCurPartnerQualityInfo(partner_id)
	if self.quality_up_info[partner_id] == nil then
		local info = self:GetPartnerInfoById(partner_id)
		return {quaility = info.partner_quality}
	else
		return self.quality_up_info[partner_id]
	end
end

function PartnerData:PartnerShowListJump(flag)
	if flag ~= nil then 
		self.partner_show_list_jump = flag
	else 
		return self.partner_show_list_jump or false
	end 
end

function PartnerData:SetArenaMatchIdList(arena_match_id_list)
	self.arena_match_id_list = arena_match_id_list
	for _, partner in pairs(self.partner_list) do
		partner:ChangeIsArenaMatchArena(arena_match_id_list[partner.id] and 1 or 0)
	end
end

function PartnerData:CurSelSkin(skin_id, selected)
	local value = false
	if selected then
		self.cur_sel_skin.skin_id = skin_id
	elseif skin_id == self.cur_sel_skin.skin_id then
		-- self.cur_sel_skin.skin_id = 0
		value = true
	end
	return value
end

function PartnerData:LifeActiveRedPointShow()
	if PartnerConfig.PartnerViewTabs.Life == self.cur_sel_tabbar.val then
		if -1 == self.cur_sel_partner then return end
		local data = self:GetPartnerInfoById(self.cur_sel_partner)
		if nil == data then return end
		if data.life_active_red_point_show then
			data.life_active_red_point_show = false
			self:PartnerLifeRedPoint()
		end
	end
end

function PartnerData:GetOriginalPartnerInfo()
	return self.partner_id_config
end

function PartnerData.IconId(data)
	if data.vo.skin_id ~= 0 then
		local co = Cfg.PartnerSkinById(data.vo.skin_id)
		return co and co.head_portrait or data.info.icon_id
	end
	return data.info.icon_id
end

function PartnerData.GetScore()
	local partner_score = 0
	local partner_sort_list,v = PartnerData.Instance:GetPartnerAllInfoLockSort()
    for _,v in pairs(partner_sort_list) do
        if v.vo.fight_state == 1 then
            partner_score = partner_score + v.vo.cap
        end
	end
	return partner_score
end

function PartnerData:GetPartnerResetCostVal()
	return self.config_other.reset_price
end
function PartnerData:WeaponLimit()
	return self.config_other.soar_arms_limit
end
function PartnerData:PartnerBiographyInfo(biography, page)
	self.cur_biography_info.biography = biography or self.cur_biography_info.biography
	self.cur_biography_info.page = page or self.cur_biography_info.page
end

function PartnerData:PartnerBiographyInfoFlushGet()
	self.cur_biography_info.flush_get = not self.cur_biography_info.flush_get
end

function PartnerData.IsPartnerItem(id)
	return PartnerData.Instance.partner_aio_list[id]
end

function PartnerData:PartnerBiographyInfoShow(partner, biography)
	TimeHelper:CancelTimer(self.timer_dt_partner_biography)
	self.biography_show:Set{
		partner = partner,
		biography = biography,
	}
	if partner > 0 and biography > 0 then
		self.timer_dt_partner_biography = TimeHelper:AddDelayTimer(function ()
			self:PartnerBiographyInfoShow(0, 0)
		end, 3)
	end
end

function PartnerData:SetScoreChange(old_score, new_score)
    self.old_score = old_score
    self.new_score = new_score
end

function PartnerData:GetScoreChange()
    return self.new_score or 0, self.old_score or 0
end

-- 伙伴详细属性数据
function PartnerData:GetOtherAttriListData()
    return PartnerConfig.OtherAttriCfg
end

function PartnerData:GetCurSelect()
	return self:GetPartnerInfoById(self.cur_sel_partner)
end

function PartnerData:GetPartnerAttriValue(attr_type, partner_index)
    partner_index = partner_index or self.cur_sel_partner
    if not partner_index or partner_index == -1 then
        return 0
    end
	local attri_list = self:GetPartnerInfoById(partner_index):GetAttrList()
    if TableIsEmpty(attri_list) then
        return 0
    end
    return attri_list[attr_type]
end


function PartnerData:GetUnlockedPartnerNum()
	local count = 0
	for k,v in pairs(self.partner_list) do
		if v:IsUnlocked() then
			count = count + 1
		end
	end
	return count
end

function PartnerData:GetPartnerGiftBaseCfg(partner_id)
	-- local partner_info = self:GetPartnerInfoById(self.cur_sel_partner)
	if self.inborn_base_list == nil then 
		return nil
	end 
	local base_info = self.inborn_base_list[partner_id == nil and self.cur_sel_partner or partner_id]
	local simple_list = {}
	for k,v in pairs(base_info) do 
		simple_list[k] = v.info
	end 

	return simple_list
end

function PartnerData:GetPartnerGiftLevelCfg(partner_id,index)
		-- local partner_info = self:GetPartnerInfoById(self.cur_sel_partner)
	if self.cur_sel_partner < 0 and partner_id == nil then return self.inborn_base_list[0][1] end 
	local select = index == nil and self.gift_flush.cur_select or index
	return self.inborn_base_list[partner_id == nil and self.cur_sel_partner or partner_id][select].level_list
end

function PartnerData:GetPartnerGiftLevelCfgWithLevel(partner_id,index,level)
	local select = index == nil and self.gift_flush.cur_select or index
	local oper_id = partner_id == nil and self.cur_sel_partner or partner_id
	return self.inborn_base_list[oper_id][select].level_list[level]
end

function PartnerData:GetPartnerGiftSpEffectCfg(partner_id)
	-- local partner_info = self:GetPartnerInfoById(self.cur_sel_partner)
	if self.cur_sel_partner < 0 then return self.inborn_sp_effect[0] end 
	return self.inborn_sp_effect[partner_id == nil and self.cur_sel_partner or partner_id]
end

function PartnerData:SetGiftAllInfo(protocol)
	-- self.gift_all_info = protocol.partner_gift_list
	for k,v in pairs(protocol.partner_gift_list) do 
		self.gift_all_info[v.partner_id] = v.gift_list
	end 
	self:PartnerGiftRedPoint()
	self:GiftFlushCall()

	self:FlushGiftEffMark()
end

function PartnerData:SetSingleGiftInfo(protocol)
	self.gift_all_info[protocol.partner_id] = protocol.gift_list
	self:PartnerGiftRedPoint()
	self:GiftFlushCall()

	self:GiftEfflush()
end 

function PartnerData:GiftEfflush()
	local total = self:GetPartnerTotalLevel()
	for i = 1,3 do 
		if self.inborn_sp_effect[self.cur_sel_partner][i].inborn_level_ask <= total then
			if self:CheckGiftEffMark(self.cur_sel_partner,i) then 
			    ViewMgr:OpenView(PartnerGiftSpShowView)
				break
			end 
		end 
	end 
end

function PartnerData:GiftFlushCall()
	self.gift_flush.call = self.gift_flush.call + 1
end

function PartnerData:GetPartnerGift(partner_id,index)
	local info = self.gift_all_info[partner_id == nil and self.cur_sel_partner or partner_id]
	if info == nil then 
		info = {}
		for i = 1,15 do 
			info[i] = 0
		end
	end 

	if index then 
		return info[index] or 0
	else 
		return info
	end 
end

function PartnerData:GetPartnerTotalLevel(partner_id)
	local info = self:GetPartnerGift(partner_id)

	local total = 0
	for i = 1,10 do total = info[i] + total end 
	return total
end

function PartnerData:GetPartnerTotalGiftAttr(partner_id)
	local info = self:GetPartnerGift(partner_id)
	local level_cfg = self.inborn_base_list[partner_id == nil and self.cur_sel_partner or partner_id]

	local attr_list = {}
	for i = 1,10 do 
		local lv = info[i]
		local cur_cfg = lv > 0 and level_cfg[i].level_list[lv] or nil
		
		if cur_cfg ~= nil then 
			for j = 1,3 do 
				local vo = {
					attr_type = cur_cfg["attr_type_"..j],
					attr_add = cur_cfg["attr_add_"..j],
				}
	
				local had_plus = false
				for k,v in pairs(attr_list) do 
					if v.attr_type == vo.attr_type then 
						v.attr_add = v.attr_add + vo.attr_add
						had_plus = true
					end 
				end 
	
				if not had_plus and vo.attr_add > 0 then 
					table.insert(attr_list,vo)
				end 
			end 
		end 
	end 

	return attr_list
end 

-- 检查够不够
-- true 不够 false 够了
function PartnerData:CheckGiftEnoughWithPop(partner_id,index)
	local oper_id = partner_id == nil and self.cur_sel_partner or partner_id
	local flag_unlock = self.partner_list[oper_id].vo.is_unlocked == PartnerConfig.LockState.lock

	if flag_unlock then 
		PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
		return true
	end 
	
	local info = self:GetPartnerGift(oper_id)
	local select = index == nil and self.gift_flush.cur_select or index
	local level_cfg = self.inborn_base_list[oper_id][select]
	local lv = info[select] == 10 and 10 or info[select] + 1

	local base = self:GetPartnerGiftBaseCfg(oper_id)
	if base == nil then return false end 
	local unlock_flag = (base[select].pre_hole_1 == 0 and base[select].pre_hole_2 == 0 ) or 
	(base[select].pre_hole_1 > 0 and info[base[select].pre_hole_1] >= base[select].pre_level_1) or 
	(base[select].pre_hole_2 > 0 and info[base[select].pre_hole_2] >= base[select].pre_level_2)

	if not unlock_flag then 
		PublicPopupCtrl.Instance:Center(string.format(Language.Partner.GiftPreLimit,base[select].pre_level_1))
		return true
	end 

	local lack_list = {}
	for k,v in pairs(level_cfg.level_list[lv].strengthen_consume_item) do 
		local num = Item.GetNum(v.item_id)
		local need = v.num
		if num < need then 
			table.insert(lack_list,{item_id =v.item_id,need_num = need - num})
		end 
	end 

	if #lack_list >0 then 
	    for _, v in ipairs(lack_list) do
		    PublicPopupCtrl.Instance:Center(string.format(ErrorText[5], Item.GetName(v.item_id), v.need_num))
	    end
		-- 获取途径指向10颗装购买道具
	    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = lack_list[1].item_id,jump_need_num = lack_list[1].need_num})})
    end 

	return #lack_list >0 
end

function PartnerData:CheckGiftPointRed(partner_id,index)
	-- if self.cur_sel_partner < 0 then return false end 
	local oper_id = partner_id == nil and self.cur_sel_partner or partner_id
	local select = index == nil and self.gift_flush.cur_select or index

	-- self.cur_sel_partner == nil 
	if oper_id == nil or select == nil then 
		return false
	end 
	
	if self.partner_list[oper_id] == nil then
		return false
	end 

	local flag_unlock = self.partner_list[oper_id].vo.is_unlocked == PartnerConfig.LockState.lock
	if flag_unlock then 
		return false 
	end 

	local info = self:GetPartnerGift(oper_id)
	local base = self:GetPartnerGiftBaseCfg(oper_id)
	if base == nil then return false end 

	local cur_level = info[select] == 10 and 10 or info[select] + 1
	local level_cfg = self:GetPartnerGiftLevelCfg(oper_id,select)[cur_level]

	if cur_level == 10 then return false end 

	local mater_flag = true
	for k,v in pairs(level_cfg.strengthen_consume_item) do 
		local num = Item.GetNum(v.item_id)
		local need = v.num
		if num < need then 
			mater_flag = false
		end 
	end 

	if not mater_flag then 
		return false 
	end 

	local unlock_flag = (base[select].pre_hole_1 == 0 and base[select].pre_hole_2 == 0 ) or 
	(base[select].pre_hole_1 > 0 and info[base[select].pre_hole_1] >= base[select].pre_level_1) or 
	(base[select].pre_hole_2 > 0 and info[base[select].pre_hole_2] >= base[select].pre_level_2)

	return unlock_flag
end

-- 获取一键强化天赋的次数
function PartnerData:GetOnesTimeGift(partner_id,index)
	local oper_id = partner_id == nil and self.cur_sel_partner or partner_id
	local select = index == nil and self.gift_flush.cur_select or index

	local info = self:GetPartnerGift(oper_id)
	local base = self:GetPartnerGiftBaseCfg(oper_id)
	if base == nil then return 0 end 
	local cur_level = info[select] == 0 and 1 or info[select]
	local level_cfg = self:GetPartnerGiftLevelCfg(oper_id,select)

	local times = {time = 0, need_0 = 0,need_1 = 0}
	for i = cur_level + 1 , 10 do 
		local cfg = level_cfg[i]

		local mater_flag = true
		for k,v in pairs(cfg.strengthen_consume_item) do 
			local num = Item.GetNum(v.item_id)
			local need = v.num + times["need_"..k]
			if num < need then 
				mater_flag = false
			else 
				times["need_"..k] = v.num + times["need_"..k]
			end 
		end

		if mater_flag then 
			times.time = times.time + 1
		else 
			break
		end 
	end 

	return times.time
end

function PartnerData:FlushGiftEffMark()
	for k,v in pairs(self.gift_all_info) do 
		local total = self:GetPartnerTotalLevel(k)
		for i = 1,3 do 
			if self.inborn_sp_effect ~= nil and self.inborn_sp_effect[k] ~= nil and self.inborn_sp_effect[k][i] ~= nil then 
			    if self.inborn_sp_effect[k][i].inborn_level_ask <= total then
				    if self.gift_eff_mark[k] == nil then self.gift_eff_mark[k] = {} end 
				    self.gift_eff_mark[k][i] = {id = k,sp = i}
			    end 
		    end
		end 
	end 
end

function PartnerData:CheckGiftEffMark(p_id,p_sp)
	if self.gift_eff_mark[p_id] == nil or self.gift_eff_mark[p_id][p_sp] == nil then 
		if self.gift_eff_mark[p_id] == nil then self.gift_eff_mark[p_id] = {} end 
		self.gift_eff_mark[p_id][p_sp] =  {id = p_id,sp = p_sp}
		return true
	else 
		return false
	end 
end

function PartnerData:GetResetTotalResult()
	-- self.cur_sel_partner
	-- self.gift_flush.cur_select
	local info = self:GetPartnerGift()
	self.partner_reset_list = {}
	self.partner_reset_limit = 0
	self:CheckGiftNextNeed(self.cur_sel_partner,self.gift_flush.cur_select)

	self.partner_reset_limit = 0

	local result = {coin = 0 ,temp_item_list = {},item_list = {}}
	for k,v in pairs(self.partner_reset_list) do 
		result.coin = result.coin + v.coin
		for k,v in pairs(v.item_list) do 
			if result.temp_item_list[v.item_id] == nil then 
				result.temp_item_list[v.item_id] = v
			else 
				result.temp_item_list[v.item_id].num = result.temp_item_list[v.item_id].num + v.num
			end 
		end 
	end 

	for k,v in pairs (result.temp_item_list) do
		table.insert(result.item_list,v)
    end 

	self.partner_reset_list = {}

	return result
end

function PartnerData:CheckGiftNextNeed(id,index)
	self.partner_reset_limit = self.partner_reset_limit + 1
	if self.partner_reset_limit > 50 then return end 

	local base = self:GetPartnerGiftBaseCfg(id)
	if base == nil then return end 
	for i = 1,3 do 
		if base[index]["pos_hole_"..i] ~= 0 then 
			self:CheckGiftNextNeed(id,base[index]["pos_hole_"..i])
		end 
	end 	

	local level = self:GetPartnerGift(id,index)
	local need = true

	for k,v in pairs(self.partner_reset_list) do 
		if v.mark.pid == id and v.mark.pindex == index then 
			need = false
		end 
	end 

	if level > 0 and need then 
		local total = {coin = 0,item_list = {},mark = {pid = id,pindex = index}}
		total.coin = base[index].reset_consume_coin
		for i = 1,level do 
			local level_cfg = self:GetPartnerGiftLevelCfgWithLevel(id,index,i)
			for k,v in pairs(level_cfg.strengthen_consume_item) do 
				if total.item_list[v.item_id] == nil then 
					total.item_list[v.item_id] = {item_id = v.item_id,num = v.num}
				else
					total.item_list[v.item_id].num = total.item_list[v.item_id].num + v.num
				end 
			end 
		end 
		table.insert(self.partner_reset_list,total)
	end 
end

function PartnerData:CheckGiftCanBeReset()
	local gift_level = PartnerData.Instance:GetPartnerGift(nil,self.gift_flush.cur_select)
	if gift_level == 0 then 
		PublicPopupCtrl.Instance:Center(Language.Partner.GiftHadNotActive)
		return true
	end 

	return false
end

-- 标记解锁后进行天赋红点勾选
function PartnerData:MarkGiftRedRemind(id)
	self.gift_red_mark = id 
end

function PartnerData:GetMarkGiftRedRemind()
	return self.gift_red_mark
end

function PartnerData:SetPartnerSpiritPactList(protocol)
	if 0 == protocol.type then
		self.partner_pact_items:Set(protocol.item_list)
	else
		for save_index, info in pairs(protocol.item_list) do
			self.partner_pact_items[save_index] = info
		end
	end
	self:PartnerPactUpRedPoint()
	self:PartnerPactEquipRedPoint()
	self:PartnerStrengthenRedPoint()
end

function PartnerData:SetPartnerPactBaseAttr(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerPactBaseAttr(protocol)
end

function PartnerData:SetPartnerPactInfo(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerPactInfo(protocol)
	self.item_flush.val = not self.item_flush.val
	self:PartnerPactEquipRedPoint()
end

local partnerPactSorts = {
	[1] = function (a, b)
		if not a or not b then return a end
		local csp =  PartnerData.Instance.cur_sel_partner
		local ai = a.item
		local bi = b.item
		local api = ai:PartnerId() == csp and 0 or 1
		local bpi = bi:PartnerId() == csp and 0 or 1
		if api == bpi then
			local ap = a.partner and (a.partner == csp and 2 or 0) or 1
			local bp = b.partner and (b.partner == csp and 2 or 0) or 1
			if ap == bp then
				local as = ai:StarLevel()
				local bs = bi:StarLevel()
				if as == bs then
					return a.temp_cap > b.temp_cap
				else
					return as > bs
				end
			else
				return ap > bp
			end
		else
			return api > bpi
		end
	end,
	[2] = function (a, b)
		if not a or not b then return a end
		local csp =  PartnerData.Instance.cur_sel_partner
		local ai = a.item
		local bi = b.item
		local api = ai:PartnerId() == csp and 0 or 1
		local bpi = bi:PartnerId() == csp and 0 or 1
		if api == bpi then
			local ap = a.partner and (a.partner == csp and 2 or 0) or 0
			local bp = b.partner and (b.partner == csp and 2 or 0) or 0
			if ap == bp then
				return a.temp_cap > b.temp_cap
			else
				return ap > bp
			end
		else
			return api > bpi
		end
	end,
	[3] = function (a, b)
		if not a or not b then return a end
		local csp =  PartnerData.Instance.cur_sel_partner
		local ppsp =  PartnerData.Instance.partner_pact_sort_prof
		local ai = a.item
		local bi = b.item
		local api = ai:PartnerId() == csp and 0 or 1
		local bpi = bi:PartnerId() == csp and 0 or 1
		if api == bpi then
			local app = ai:PartnerProf() == ppsp and 1 or 0
			local bpp = bi:PartnerProf() == ppsp and 1 or 0
			if app == bpp then
				local asl = a.star_level
				local bsl = b.star_level
				if asl == asl then
					return a.temp_cap > b.temp_cap
				else
					return asl > bsl
				end
			else
				return app > bpp
			end
		else
			return api > bpi
		end
	end,
	[4] = function (a, b)
		if not a or not b then return a end
		local csp =  PartnerData.Instance.cur_sel_partner
		local ai = a.item
		local bi = b.item
		local api = ai:PartnerId() == csp and 0 or 1
		local bpi = bi:PartnerId() == csp and 0 or 1
		if api == bpi then
			local asl = a.star_level
			local bsl = b.star_level
			if asl == bsl then
				return a.temp_cap > b.temp_cap
			else
				return asl > bsl
			end
		else
			return api > bpi
		end
	end,
}


function PartnerData:GetPartnerPactItems()
	local list = {}
	local sel_pos = self.partner_pact_show_info.sel_pos
	local sel_partner = self.partner_list[self.cur_sel_partner]
	self.partner_pact_sort_prof = sel_partner.info.job
	local block_ids = {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
		local pact_item = sel_partner.pact_items[i]
		if pact_item.item_id > 0 then
			pact_item.temp_cap = pact_item.item:ScoreAdd(self.cur_sel_partner)
			if pact_item.pos == sel_pos then
				table.insert(list, pact_item)
			else
				block_ids[pact_item.item:PartnerId()] = true
			end
		end
	end
	for _, partner in pairs(self.partner_list) do
		local pact_items = partner.pact_items
		for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
			local pact_item = pact_items[i]
			if pact_item.item_id > 0 then
				pact_item.temp_cap = pact_item.item:ScoreAdd(self.cur_sel_partner)
				if pact_item.partner ~= self.cur_sel_partner and not block_ids[pact_item.item:PartnerId()] then
					table.insert(list, pact_item)
				end
			end
		end
	end

	for _, info in pairs(self.partner_pact_items:Val()) do
		if info.num > 0 and not block_ids[info.item:PartnerId()] then
			info.temp_cap = info.item:ScoreAdd(self.cur_sel_partner)
			if 1 == info.num then
				table.insert(list, info)
			else
				for i = 1, info.num do
					table.insert(list, TableCopy(info))
				end
			end
		end
	end
	
	table.sort(list, partnerPactSorts[self.partner_pact_show_info.sort_type])
	return list
end

function PartnerData:GetPatnerPactHaveNum(item_id)
	local num = 0
	for _, info in pairs(self.partner_pact_items:Val()) do
		if info.item_id == item_id then
			num = num + info.num
		end
	end
	return num
end

function PartnerData:GetPartnerPactAttrShowAll(pact_items)
	local attr_list = {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        local pact_item = pact_items[i]
        if pact_item.item_id > 0 then
			local item = pact_item.item
			local partner_id = item:PartnerId()
			local partner = self.partner_list[partner_id]
			local attrs = partner:GetPactAttrList(item:AttrUp())
			for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
				if attrs[i] then
					if nil == attr_list[i] then
						attr_list[i] = attrs[i]
					else
						attr_list[i].attr_value = attr_list[i].attr_value + attrs[i].attr_value
					end
				end
			end
		end
    end
	return attr_list
end

function PartnerData:GetPartnerPactAttrShowOne(pact_item)
	local attr_list = {}
	if pact_item.item_id > 0 then
		local item = pact_item.item
		local partner_id = item:PartnerId()
		local partner = self.partner_list[partner_id]
		local attrs = partner:GetPactAttrList(item:AttrUp())
		for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
			if attrs[i] then
				attr_list[i] = attrs[i]
			end
		end
	end
	return attr_list
end

function PartnerData:SetPartnerPactUpEffect(type, partner, pos)
	self.partner_pact_up_effect.type = type
	self.partner_pact_up_effect.partner = partner
	self.partner_pact_up_effect.pos = pos
end

function PartnerData:GetPartnerPactItemNumById(partner_id)
	local num = 0
	for _, partner in pairs(self.partner_list) do
		num = num + partner:PactPartnerNum(partner_id)
	end
	return num
end

function PartnerData:GetPartnerPactItemsEquipCheck(partner_id)
	local value = false
	for _, info in pairs(self.partner_pact_items:Val()) do
		if info.num > 0 then
			if info.item:PartnerId() ~= partner_id and info.item:PartnerIsUnlocked() and self:GetPartnerPactItemNumById(info.item:PartnerId()) < 2 then
				value = true
				break
			end
		end
	end
	return value
end

function PartnerData:GetPartnerPactItemsUpCheck(item)
	local value = false
	if not item:MaxAttrUp() then
		local promote_expens = item:PromoteExpens()
		local expens_num = item:ExpensNum()
		for _, info in pairs(self.partner_pact_items:Val()) do
			if info.item_id == promote_expens and info.num >= expens_num then
				value = true
				break
			end
		end
	end
	return value
end

function PartnerData:SetPartnerSoarNotice(protocol)
	if nil == self.partner_list[protocol.partner] then return end
	self.partner_list[protocol.partner]:PartnerSoarNotice(protocol)
	self.item_flush.val = not self.item_flush.val

	if PartnerConfig.SoarNoticeType.soar == protocol.notice_type then
		self.new_partner = protocol.partner
		ViewMgr:CloseView(PartnerSoarView)
		ViewMgr:OpenView(PartnerSoarSuccView)
	elseif PartnerConfig.SoarNoticeType.soar_weapon == protocol.notice_type then
		self.partner_soar_weapon_effect.val = not self.partner_soar_weapon_effect.val
	end
	self:PartnerUpRedPoint()
	self:PartnerSoarRedPoint()
	self:PartnerSoarWeaponRedPoint()
end

function PartnerData:GetPartnerSoarCoByCount(count)
	return self.partner_soar_count_list[count]
end

function PartnerData:GetPartnerSoarWeaponAttrsShow(attrs, attrs_next)
	if nil == next(attrs_next) then
		return attrs
	end
	for attr_type, attr in pairs(attrs_next) do
		attr.attr_value_now = attrs[attr_type] and attrs[attr_type].attr_value or 0
	end
	return attrs_next
end

function PartnerData:SetNewTaskPartnerSoarInfo(protocol)
    self.is_cur_accpet_partner_soar_task = protocol.is_cur_accpet_partner_soar_task
end

function PartnerData:PactUpTipsShow(partner_id)
    if self.pact_up_tips_show then
		local is_pact = false
		for _, partner in pairs(self.partner_list) do
			is_pact = is_pact or partner:IsPactPartner(partner_id)
		end
		if is_pact then
			self.pact_up_tips_show = false
			PublicPopupCtrl.Instance:Center(Language.Partner.PactUpTipsShow)
			TimeHelper:AddDelayTimer(function ()
				self.pact_up_tips_show = true
			end, 5)
		end
	end
end

function PartnerData:SetPartnerIntensifyInfo(protocol)
    if nil == self.partner_list[protocol.partner_id] then return end
	self.partner_list[protocol.partner_id]:PartnerIntensifyInfo(protocol)
	self.new_partner = protocol.partner_id
	self:SetPartnerStrengthenUpEffect(self.new_partner)
	self:PartnerStrengthenRedPoint()
	TimeHelper:CancelTimer(self.partner_strengthen_up_timer)
	self.partner_strengthen_up_timer = TimeHelper:AddDelayTimer(function ()
		ViewMgr:OpenView(PartnerStrengthenSuccView)
	end, 4)
end

function PartnerData:SetPartnerTopRankInfo(protocol)
	local info = protocol.info
    for _, partner in pairs(self.partner_list) do
		partner:PartnerTopRankInfo(info[partner.id] or 0)
	end
	self.item_flush.val = not self.item_flush.val
end

function PartnerData:PartnerStrAttrsParse(co)
	if nil == co.attrs_show then
		local attrs = {}
		local strs1 = string.split(co.att_up_1, "|")
		for i = 1, #strs1 do
			local strs2 = string.split(strs1[i], ":")
			local attr_type = tonumber(strs2[1])
			local attr_value = tonumber(strs2[2])
			table.insert(attrs, {
				attr_type = attr_type,
				attr_value = attr_value,
				index = i,
				intensify_level = co.intensify_level
			})
			co.attrs_show = attrs
		end
	end
	return co.attrs_show
end

function PartnerData:PartnerStrConsumesParse(co)
	if nil == co.consumes_show then
		local consumes1 = {}
		local consumes2 = {}
		consumes1[1] = Item.Init(co.expend_item_1, co.expend_num_1)
		consumes1[2] = Item.Init(CommonItemId.PartnerPactNormal, co.expend_num_2)
		consumes1[3] = Item.Init(co.expend_item_3, co.expend_num_3)
		if co.expend_num_1 > 0 then
			table.insert(consumes2, consumes1[1])
		end
		if co.expend_num_2 > 0 then
			table.insert(consumes2, consumes1[2])
		end
		if co.expend_num_3 > 0 then
			table.insert(consumes2, consumes1[3])
		end
		co.consumes_show = consumes2
		co.consumes_items = consumes1
	end
	return co.consumes_show, co.consumes_items
end

function PartnerData:PartnerIntensifyPreShowList(id)
	local data = self:GetPartnerInfoById(id)
    local vo = data.vo
	-- if nil == self.partner_intensify_pre_show_list[id] then
		local list = {}
		for k, v in ipairs(Config.partner_cfg_auto.parnter_intensify) do
			if v.partner_id == id and v.intensify_level < PartnerConfig.PARTNER_STRENGTHEN_LEVEL_MAX and vo.soar_count >= v.feisheng_num then
				self:PartnerStrConsumesParse(v)
				table.insert(list, v)
			end
		end
		-- self.partner_intensify_pre_show_list[id] = list
	-- end
	return list
end

function PartnerData:PartnerIntensifyAddShowList(id)
	local data = self:GetPartnerInfoById(id)
    local vo = data.vo
	-- if nil == self.partner_intensify_add_show_list[id] then
		local list = {}
		for k, v in ipairs(Config.partner_cfg_auto.parnter_intensify) do
			if v.partner_id == id and v.intensify_level > 1 and vo.soar_count >= v.feisheng_show then
				self:PartnerStrAttrsParse(v)
				table.insert(list, v)
			end
		end
		-- self.partner_intensify_add_show_list[id] = list
		-- end
	return list
end

function PartnerData:PartnerIntensifyAllShowList(id)
	local data = self:GetPartnerInfoById(id)
    local vo = data.vo
	local list1, list2 = {}, {}
	local index = 0
	local t_index = {}
	for k, v in ipairs(Config.partner_cfg_auto.parnter_intensify) do
		if v.partner_id == id and vo.intensify_level >= v.intensify_level and v.attrs_show then
			for _, attr in ipairs(v.attrs_show) do
				if nil == list1[attr.attr_type] then
					index = index + 1
					t_index[attr.attr_type] = index
					list1[attr.attr_type] = attr.attr_value
				else
					list1[attr.attr_type] = list1[attr.attr_type] + attr.attr_value 
				end 
			end
		end
	end
	for attr_type, attr_value in pairs(list1) do
		list2[t_index[attr_type] or 1] = {
			attr_type = attr_type,
			attr_value = attr_value,
		}
	end
	return list2
end

function PartnerData:SetPartnerStrengthenSel(index, item)
	self.strength_items.index = index
	self.strength_items.sel_item = item
end

function PartnerData:SetPartnerStrengthenSelItems(item_id, sel_num)
	local index = self.strength_items.index
	self.strength_items.items[index] = self.strength_items.items[index] or {}
	if 0 == sel_num then
		self.strength_items.items[index][item_id] = nil
	else
		self.strength_items.items[index][item_id] = sel_num
	end
	self.strength_items.flush = not self.strength_items.flush
end

function PartnerData:GetPartnerStrengthenSelItemsNumByItemId(item_id, index)
	local index = index or self.strength_items.index
	local num = 0
	if self.strength_items.items[index] and self.strength_items.items[index][item_id] then
		num = self.strength_items.items[index][item_id]
	end
	return num
end

function PartnerData:GetPartnerStrengthenSelItemsNums(index)
	local index = index or self.strength_items.index
	local num = 0
	if self.strength_items.items[index] then
		for k, v in pairs(self.strength_items.items[index]) do
			num = num + v
		end
	end
	return num
end

function PartnerData:GetPartnerStrengthenSelItemsCount(index)
	local count = 0
	if self.strength_items.items[index] then
		for k, v in pairs(self.strength_items.items[index]) do
			count = count + 1
		end
	end
	return count
end

function PartnerData:IsPartnerStrengthenSelItemsMaxNum()
	local sel_nums = self:GetPartnerStrengthenSelItemsNums()
	local need_num = self.strength_items.sel_item:Num()
	return sel_nums >= need_num
end


function PartnerData:GetPartnerStrengthenSelShowList()
	local list = {}
	local index = self.strength_items.index
	local is_empty = true
	if 2 == index then
		local temp = {}
		for _, info in pairs(self.partner_pact_items:Val()) do
			local item = info.item
			if 0 == item:StarLevel() and nil == temp[item.item_id] then
				local have_num = self:GetPatnerPactHaveNum(item.item_id)
				have_num = have_num - self:GetPartnerStrengthenSelItemsNumByItemId(item.item_id, 1)
				have_num = have_num - self:GetPartnerStrengthenSelItemsNumByItemId(item.item_id, 3)
				if have_num > 0 then
					is_empty = false
					list[info.item_id] = Item.Init(item.item_id, have_num)
				end
				temp[info.item_id] = true
			end
		end
	else
		local sel_item = self.strength_items.sel_item
		local have_num = self:GetPatnerPactHaveNum(sel_item.item_id)
		have_num = have_num - self:GetPartnerStrengthenSelItemsNumByItemId(sel_item.item_id, 2)
		have_num = have_num - self:GetPartnerStrengthenSelItemsNumByItemId(sel_item.item_id, 1 == index and 3 or 1)
		if have_num > 0 then
			is_empty = false
			list[1] = Item.Init(sel_item.item_id, have_num)
		end
	end
	return list, is_empty
end

function PartnerData:ResetPartnerStrengthenSelItems()
	self.strength_items.items = {}
	-- self.strength_items.flush = not self.strength_items.flush
end

function PartnerData:SubPartnerStrengthenSelItemsByIndex(index)
	self.strength_items.items[index] = {}
	self.strength_items.flush = not self.strength_items.flush
end

function PartnerData:SetPartnerStrengthenUpEffect(partner)
	self.partner_strengthen_up_effect.partner = partner
end

function PartnerData:PartnerSoarWeaponConsume(item_id)
	local value = false
	if 0 == UnityPlayerPrefs.GetInt(PrefKeys.PartnerSoarWeaponConsume()) and self.partner_soar_weapon_consumes[item_id] then
		value = true
		UnityPlayerPrefs.SetInt(PrefKeys.PartnerSoarWeaponConsume(), 1)
		ViewMgr:CloseView(ItemInfoView)
		GuideManager.Instance:Start(115)
	end
	return value
end

function PartnerData.PartnerRankShow(rank)
	return rank > 0 and rank <= PartnerConfig.PARTNER_RANK_MAX_SHOW
end

function PartnerData:PartnerFuYuUp()
	if PartnerData.FuYuIsOpen() then
		return false
	end
	for _, partner in pairs(self.partner_list) do
		if partner:IsFight() then-- and partner:SoarCount() > 0 then
			if partner:FuYuCanPutOn() then
				return true
			end
		end
	end
	return false
end
function PartnerData.FuYuIsOpen()
	local isopen, _ = FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerFuyu)
	return isopen
end

function PartnerData.EmptyQualityUp(partner_info)
	local t = {
		up_type = 1,
		quality = partner_info.partner_quality,
		operate_qua = PartnerConfig.ChangeServerQua[partner_info.partner_quality],
		attr_list = {[0] = 0,[1] = 0,[2] = 0,[3] = 0,[4] = 0,[5] = 0,[6] = 0,},
		quality_segment = 1,
	}
	return t
end