
PartnerInfo = PartnerInfo or BaseClass()

function PartnerInfo:__init(param_t)
	self.info = param_t.info
	self.vo = {attr_stage = 0,is_unlocked = 0, init = true}
	self.quality = {
		quality = self.info.partner_quality, 
		quality_segment = 1, 
		operate_qua = PartnerConfig.ChangeServerQua[self.info.partner_quality],
		init = false}
	
	self.id = self.info.partner_id
	self.partner_quality = self.info.partner_quality

	self:PartnerInfo()
end

function PartnerInfo:PartnerInfo()
	self.info.element = self:CheckElement(self.info)
	self.info.up_info, self.info.skill_lock_info, self.info.up_skills_il_info, self.info.up_skillf_info = self:CheckUpInfo()
	self.info.qingyuan_list, self.info.qingyuan_type_list = self:CheckQingYuanList()
	self.info.other = self:CheckOther()
	self.info.skill_list, self.info.skill_list2 = self:CheckSkillInfo()
	self.info.activation_info, self.info.activation_item = self:CheckActivationInfo()
	self.get_way_tips = self:CheckGetWayTips()
	self.info.up_skills_info = self:CheckUpSkillInfo()
	self.biographys = self:CheckBiography()

	self.sel_skill_list = {}
	self:InitCurSkillList()

	self.fetter_cur = {}
	self.fetter_wash = {}
	self.fetter_wash_type = {}

	self.quality_skill_list = {}

	self.fuyu_item_list = {}

	self.up_red_point = SmartData.New{ val = false }
	self.crystal_red_point = SmartData.New{ val = false }
	self.acti_red_point = SmartData.New{ val = false }
	self.skill_red_point = SmartData.New{ val = false }
	self.quality_red_point = SmartData.New{ val = false }
	self.gift_red_point = SmartData.New{ val = false }
	self.life_active_red_point = SmartData.New{ val = false }
	self.life_equip_red_point = SmartData.New{ val = false }
	self.attr_level_red_point = SmartData.New{ val = false }
	self.biography_red_point = SmartData.New{ val = false }
	self.pact_equip_red_point = SmartData.New{ val = false }
	self.pact_up_red_point = SmartData.New{ val = false }
	self.soar_red_point = SmartData.New{ val = false }
	self.soar_weapon_red_point = SmartData.New{ val = false }
	self.strengthen_red_point = SmartData.New{ val = false }

	self.flush_keys = SmartData.New {
		flush_cap = false,
		flush_skin = false,
	}

	self.cap_sp = "scoreLevel0"
	self.cap_level = 0
	self.quality_up = {is_show = false}

	self.crystal_red_point_show = true
	self.life_active_red_point_show = true
	self.life_equip_red_point_show = true
	self.pact_equip_red_point_show = true
	-- self.fuyu_put_on_red_point_show = true
	-- self.fuyu_change_better_red_show = true
	-- self.strengthen_red_point_show = true
end

function PartnerInfo:CheckElement(info)
	local t = {}

	local function CheckEle(ele, type)
		if ele ~= 0 then
			local data = {
				type = type,
				num = ele
			}
			table.insert(t, data)
		end
	end
	
	CheckEle(info.element_water, "ShuXing_Shui")
	CheckEle(info.element_fire, "ShuXing_Huo")
	CheckEle(info.element_wind, "ShuXing_Feng")
	CheckEle(info.element_land, "ShuXing_Tu")

	return t
end

function PartnerInfo:CheckUpInfo()
	local t1, t2, t3, t4 = {}, {}, {}, {}
	local t3t = {}
	for i = 1, PartnerConfig.PARTNER_SKILL_AN_MAX do
		t2[i] = PartnerConfig.UP_LEVEL_MAX
	end
	for k, v in pairs(Config.partner_cfg_auto.partner_upgrade) do
		if v.partner_id == self.id then
			t1[v.level] = v
			for i = 1, PartnerConfig.PARTNER_SKILL_AN_MAX do
				t2[i] = v["skill_level_"..i] > 0 and v.level < t2[i] and v.level or t2[i]
				t3t[v.level] = t3t[v.level] or {}
				t3t[v.level][i] = v["skill_level_"..i]
			end
			t4[v.skill_friendly] = math.min(v.level, t4[v.skill_friendly] or v.level)
		end
	end
	for k, v in pairs(Config.partner_cfg_auto.partner_upgrade) do
		if v.partner_id == self.id then
			t3[v.level] = {}
			local t = t3t[v.level]
			for i = 1, PartnerConfig.PARTNER_SKILL_AN_MAX do
				if t3t[v.level + 1] and t3t[v.level + 1][i] > t3t[v.level][i] then
					t3[v.level][i] = t3t[v.level + 1][i]
				end
			end
		end
	end
	return t1, t2, t3, t4
end

function PartnerInfo:CheckQingYuanList()
	local t1 = {}
	local t2 = {}
	-- for k, v in pairs(Config.partner_cfg_auto.qingyuan_list) do
	-- 	t1[v.qingyuan_id] = v
	-- 	t2[v.qingyuan_type] = t2[v.qingyuan_type] or {}
	-- 	local data = {
	-- 		name = v.name,
	-- 		desc = v.desc,
	-- 		color = v.color,
	-- 	}
	-- 	table.insert(t2[v.qingyuan_type], data)
	-- end
	return t1, t2
end

function PartnerInfo:CheckOther()
	return Config.partner_cfg_auto.other[1]
end

function PartnerInfo:CheckSkillInfo()
	local t1, t2 = {}, {}
	for _, v in pairs(Config.partner_cfg_auto.skill_list) do
		if v.partner_id == self.id then
			local need_level = self.info.skill_lock_info[v.skill_index]
			local min_skill_level = self.info.up_info[need_level]["skill_level_"..v.skill_index]
			local skill_info = PartnerSkillInfo.New{skill_id = v.skill_id, skill_index = v.skill_index , need_level = need_level, min_skill_level = min_skill_level}
			t1[v.skill_id] = skill_info
			t2[v.skill_index] = skill_info
		end
	end
	return t1, t2
end

function PartnerInfo:CheckActivationInfo()
	local activation_type = self.info.activation_type or 0
	for _, v in pairs(Config.partner_cfg_auto.partner_activation) do
		if v.activation_type == activation_type then
			if 5 == activation_type then
				local item_data = Item.New()
				item_data.item_id = self.info.activation_item
				item_data.cost_num = self.info.activation_num
				return v, item_data
			end
			return v
		end
	end
end

function PartnerInfo:CheckGetWayTips()
	local tips_store, tips_normal
	if self.info.activation_item then
		local get_way = self.info.activation_item:GetWay() or {}
		for _, id in pairs(get_way) do
			local co = BagData.Instance:GetWayCfg(id)
			if nil == tips_store and string.find(co.desc or "", Language.Partner.GetWayStore) then
				tips_store = string.format(Language.Partner.GetWayTips1, co.desc)
			end
			tips_normal = tips_normal or string.format(Language.Partner.GetWayTips2, co.desc)
		end
	end
	return tips_store or tips_normal or ""
end

function PartnerInfo:CheckUpSkillInfo()
	local t1, t2 = {}, {}
	for k, v in pairs(self.info.skill_list) do
		t2[v.skill_index] = v
	end
	for k, v in pairs(self.info.up_skills_il_info) do
		t1[k] = {}
		for y, z in pairs(v) do
			table.insert(t1[k], {
				id = t2[y].id,
				info = t2[y].info[z],
				per_info = t2[y].info[z-1],
			})
		end
	end
	return t1
end

function PartnerInfo:CheckBiography()
	local t = {}
	for _, co in pairs(Config.partner_cfg_auto.partner_biography) do
		if co.partner_id == self.id then
			local rewards = {}
			table.insert(rewards, Item.Init(CommonItemId.Coin, co.coin, 0))
			for i = 0, #co.reward_item do
				local item_data = co.reward_item[i]
				table.insert(rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
			end
			local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
			co.desc_f = tab .. string.gsub(co.desc, "\n", "\n" .. tab)
			co.letter_desc_f = tab .. string.gsub(co.letter_desc, "\n", "\n" .. tab)
			co.rewards = rewards
			co.flag_open = false
			co.flag_get = false
			co.flag_unlock = false
			t[co.biography] = co
		end
	end
	return t
end

function PartnerInfo:CheckBiographyUp(check_show)
	if nil == self.biographys or nil == self.vo then return end
	for _, v in ipairs(self.biographys) do
		local flag_unlock = v.flag_unlock
		v.flag_open = 1 == bit:_rshift(self.vo.biography_open_flag, v.biography) % 2
		v.flag_get = 0 == bit:_rshift(self.vo.biography_fetch_flag, v.biography) % 2
		v.flag_unlock = self.vo.up_level >= v.upgrade
		if check_show and v.flag_unlock and not flag_unlock then
			PartnerData.Instance:PartnerBiographyInfoShow(self.id, v.biography)
		end
	end
end

function PartnerInfo:CheckContractInfo()
	self.pact_items = {}
	local contracts = self.vo and self.vo.contracts or {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        local t = {
			item_id = contracts[i],
			item = Item.Init(contracts[i]),
			pos = i - 1,
			partner = self.id,
		}
		t.item.show_stars = true
		self.pact_items[i] = t
    end
end

function PartnerInfo:InitCurSkillList()
	self.cur_skill_list = {}
	for k, v in pairs(self.info.skill_list) do
		self.cur_skill_list[k] = {}
		self.cur_skill_list[k].info = {}
	end
end

function PartnerInfo:PartnerVo(vo)
	if self.vo.level and vo.level > self.vo.level then PartnerData.Instance.partner_attr_up_level_effect:Set{val = true} end
	local is_arena_match = self.vo.is_arena_match

	self.vo = vo
	self.vo.is_arena_match = is_arena_match
	
	self:CheckAll()
	self.flush_keys.flush_cap = not self.flush_keys.flush_cap
	PartnerData.Instance:PartnerBiographyInfoFlushGet()
end

function PartnerInfo:ChangeFightState(protocol)
	if 1 == self.vo.fight_state and 0 == protocol.is_fight then
		PartnerData.Instance:PartnerShowListJump(true)
		PartnerData.Instance.cur_sel_partner = -1
	end
	self.vo.fight_state = protocol.is_fight
	self.vo.is_unlocked = PartnerConfig.LockState.unlock
	-- self.vo.is_arena_match = protocol.is_arena_match

	if protocol.is_fight > 0 then 
		PartnerData.Instance:PartnerShowListJump(true)
	end 

    -- 当切换为出战时，则打钩 20211221R
	if protocol.is_fight > 0 then 
		self:SetGiftRemind(self:IsFight())
	end 
end

function PartnerInfo:ChangeIsArenaMatchArena(is_arena_match)
	self.vo.is_arena_match = is_arena_match
end

function PartnerInfo:GetAttrList()
	-- if self.vo.skin_id > 0 and PartnerData.Instance.partner_skin_ids[self.vo.skin_id] then
	-- 	local attr_list = TableCopy(self.vo.attr_list)
	-- 	local skin_info = PartnerData.Instance.partner_skin_ids[self.vo.skin_id]
	-- 	local attr_special = {}
	-- 	for k, v in pairs(AttrTypeForName) do
	-- 		if skin_info[k] and skin_info[k] > 0 then
	-- 			attr_special[v] = true
	-- 		end
	-- 	end
	-- 	for k, v in pairs(attr_list) do
	-- 		if attr_special[k] then
	-- 			attr_list[k] = RichTextHelper.ColorStr(attr_list[k], COLORSTR.Green3)
	-- 		end
	-- 	end
	-- 	return attr_list
	-- else
	-- 	return self.vo.attr_list
	-- end
	return self.vo.attr_list
end

function PartnerInfo:PartnerAttrListInfo(protocol)
	local cap = self.vo.cap
	self.vo.cap = protocol.cap
	local change_list = {}
	self.attr_change = not self.attr_change
	for k, v in pairs(protocol.attr_list or {}) do
		if self.vo.attr_list == nil then
			self.vo.attr_list = {}
		end
		change_list[k] = {
			-- old_val = self.vo.attr_list[k] or 0,
			old_val = v,
			change_val = v - (self.vo.attr_list[k] or 0),
		}
		self.vo.attr_list[k] = v
	end
	local nt = PartnerConfig.NoticeType
	if nt.pact_change == protocol.notice_type then
		local title_info = {fromType = AttrChangeData.ChangeFromType.Partner, showName = self.info.name}
		MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
		PartnerData.Instance:SetScoreChange(cap, self.vo.cap)
		ViewMgr:OpenView(PartnerScore)
	elseif nt.role_quality_up == protocol.notice_type then 
		-- self.quality_up.is_show = true
		-- self.quality_up.change_list = change_list
		-- PartnerData.Instance.new_partner = self.id
		-- PartnerData.Instance.is_quality = true
		-- ViewMgr:OpenView(PartnerViewLock)
	-- elseif nt.role_level_up ~= protocol.notice_type and nt.role_train_up ~= protocol.notice_type and nt.skin_no_ac ~= protocol.notice_type then
	elseif nt.guild_inborn == protocol.notice_type then
		local data, first_partner = PartnerData.Instance:GetPartnerAllInfoLockSort()
		if self.id == first_partner then
			local delay_time = GuildData.Instance:GetDelayTime(1)
			local function showAttr()
				local title_info = {fromType = AttrChangeData.ChangeFromType.Partner, showName = self.info.name}
				MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
				if cap ~= self.vo.cap then
					PartnerData.Instance:SetScoreChange(cap, self.vo.cap)
					ViewMgr:OpenView(PartnerScore)
				end
			end
			--灵脉某些技能人物伙伴宠物同时加属性，延迟下。。。
			TimeHelper:AddDelayTimer(showAttr, delay_time)
		end
	elseif nt.fu_yu == protocol.notice_type or nt.star_map == protocol.notice_type then
		local data, first_partner = PartnerData.Instance:GetPartnerAllInfoLockSort()
		if self.id == first_partner then
			-- local delay_time = GuildData.Instance:GetDelayTime(1)
			local function showAttr()
				local title_info = {fromType = AttrChangeData.ChangeFromType.Partner, showName = self.info.name}
				MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
				if cap ~= self.vo.cap then
					PartnerData.Instance:SetScoreChange(cap, self.vo.cap)
					ViewMgr:OpenView(PartnerScore)
				end
			end
			--灵脉某些技能人物伙伴宠物同时加属性，延迟下。。。
			if nt.star_map == protocol.notice_type then
				showAttr()
			else
				TimeHelper:AddDelayTimer(showAttr, 3.5)
			end
		end
	elseif nt.role_level_up ~= protocol.notice_type and nt.skin_no_ac ~= protocol.notice_type and nt.pact ~= protocol.notice_type
	and nt.fu_yu ~= protocol.notice_type then
		local data, first_partner = PartnerData.Instance:GetPartnerAllInfoLockSort()
		if self.id == first_partner then
			local title_info = {fromType = AttrChangeData.ChangeFromType.Partner, showName = self.info.name}
			MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
		end
		-- MainOtherCtrl.Instance:OpenAttrChange(change_list, title_info)
	end

	if cap ~= 0 and cap ~= self.vo.cap then
		local cap_show = true
		if nt.role_train_up == protocol.notice_type then
			cap_show = 1 == self.id
		elseif nt.guild_inborn == protocol.notice_type then
			cap_show = false
		elseif nt.fu_yu == protocol.notice_type then
			cap_show = false
		end
		if cap_show then
			PartnerData.Instance:SetScoreChange(cap, self.vo.cap)
			ViewMgr:OpenView(PartnerScore)
		end
		PartnerData.Instance:PactUpTipsShow(self.id)
	end
	self:CheckCap()
	self.flush_keys.flush_cap = not self.flush_keys.flush_cap
end

function PartnerInfo:PartnerFetter(protocol)
	self.vo.fetter_cur = protocol.fetter_cur
	self.vo.fetter_wash = protocol.fetter_wash

	self:CheckFetter()
end

function PartnerInfo:PartnerBaseGradeInfo(protocol)
	local up_level = self.vo.up_level
	self.vo.up_level = protocol.up_level
	self.vo.attr_stage = protocol.attr_stage
	self.vo.task_finish = protocol.task_finish
	
	self:CheckUpLoop()
	self:CheckCurUpInfo()
	self:CheckSkillF()
	self:CheckSkillList{level = true}
	self:CheckBiographyUp(true)

	if up_level < self.vo.up_level then
		self.up_lock_skills = {}
		for _, skill in pairs(self.cur_skill_list_sort) do
			if self.vo.up_level >= skill.info.need_level then
				table.insert(self.up_lock_skills, skill)
			end
		end
		-- PartnerData.Instance.new_partner = self.id
		-- PartnerData.Instance.lock_special = true
		-- ViewMgr:OpenView(PartnerViewLock)
	end
end

function PartnerInfo:PartnerSkill(protocol)
	self.vo.skill_list = protocol.skill_list

	self:CheckSkillList{state = true}
	self:CheckQualitySkill()
end

function PartnerInfo:PartnerEquip(protocol)
	self.vo.pes[protocol.equip_slot + 1] = protocol.pe

	self:CheckPEList()
end

function PartnerInfo:PartnerUseElementCrystal(protocol)
	self.vo.crystal = protocol.crystal
	self.vo.used_times = protocol.used_times
end

function PartnerInfo:SetPartnerSkinId(skin_id)
	self.vo.skin_id = skin_id
	self.skin_id_try = 0
	self.flush_keys.flush_skin = not self.flush_keys.flush_skin
end

function PartnerInfo:SetPartnerQualityInfo(protocol)
	self.quality.init = false
	self.quality.quality = protocol.quality
	self.quality.operate_qua = PartnerConfig.ChangeServerQua[protocol.quality]
	self.quality.quality_segment = protocol.quality_segment

	self:CheckQualitySkill()
end

function PartnerInfo:CheckUpLoop()
	self.up_loop = {}
	self.up_loop_t = {}

	local config_partner_star_point = Config.partner_cfg_auto.partner_star_point
	local level = self.vo.up_level

	for k, v in pairs(config_partner_star_point) do
		if v.partner_id == self.id and v.level == level then
			local data = {}
			local seq = v.seq + 1
			for i = 1, 1 do
				local item_data = Item.New()
				item_data.item_id = v["stuff_need_"..i]
				item_data.cost_num = v["stuff_num_"..i]
				item_data.is_bind = 0
				table.insert(data, item_data)
			end
			self.up_loop[seq] = {
				seq = seq,
				att_type = v.att_type,
				att_value = v.att_value,
				attr_stage = self.vo.attr_stage,
				items = data,
			}
		end
		if v.partner_id == self.id then
			self.up_loop_t[v.level] = self.up_loop_t[v.level] or {}
			self.up_loop_t[v.level][v.seq + 1] = {
				att_type = v.att_type,
				att_value = v.att_value,
			}
		end
	end
end

function PartnerInfo:CheckCurUpInfo()
	local up_info = self.info.up_info
	local level = self.vo.up_level
	
	self.cur_up_info = up_info[level]
	self.pre_up_info = up_info[level - 1]
end

function PartnerInfo:CheckSkillF()
	local skillf_seq = (self.info.up_info[self.vo.up_level] or {}).skill_friendly or 0
	local is_unlock = PartnerConfig.LockState.unlock == self.vo.is_unlocked
	if skillf_seq > 0 then
		local pfc = PartnerData.Instance.partner_friendly_seq_config
		local pfc2 = PartnerData.Instance.partner_friendly_pl_config
		if pfc[skillf_seq] then
			local skill_info_cur = pfc[skillf_seq]
			local skill_info_next = pfc2[self.id][skill_info_cur.level + 1]
			local skill_level_cur = skill_info_cur and skill_info_cur.level or 0
			if self.skill_f and self.skill_f.skill_level_cur and skill_level_cur > self.skill_f.skill_level_cur then
				PartnerData.Instance.partner_up_skillf_effect:Set{val = true}
			end
			self.skill_f = {
				index = 0,
				info = skill_info_cur,
				is_lock = not is_unlock or nil == skill_info_cur,
				is_equip = true,
				skill_cur = skill_info_cur,
				skill_next = skill_info_next,
				need_level = skill_info_next and self.info.up_skillf_info[skill_info_next.seq] or 0,
				partner_name = self.info.name,
				attrs_cur = self:GetSkillFAttrs(skill_info_cur),
				attrs_next = self:GetSkillFAttrs(skill_info_next),
				skill_type = PartnerConfig.SkillType.friendly,
				skill_level_cur = skill_level_cur,
			}
		else
			LogError(string.format("Config.partner_cfg_auto.partner_friendly seq=%s 缺少配置！！", skillf_seq))
		end
	else
		local pfc = PartnerData.Instance.partner_friendly_pl_config
		if pfc[self.id] and pfc[self.id][1] then
			local skill_info_cur = nil
			local skill_info_next = pfc[self.id][1]
			self.skill_f = {
				index = 0,
				info = pfc[self.id][1],
				is_lock = true,
				is_equip = true,
				skill_cur = skill_info_cur,
				skill_next = skill_info_next,
				need_level = skill_info_next and self.info.up_skillf_info[skill_info_next.seq] or 0,
				partner_name = self.info.name,
				attrs_cur = self:GetSkillFAttrs(skill_info_cur),
				attrs_next = self:GetSkillFAttrs(skill_info_next),
				skill_type = PartnerConfig.SkillType.friendly,
				skill_level_cur = 0,
			}
		else
			LogError(string.format("Config.partner_cfg_auto.partner_friendly partner_id=%s level=%s 缺少配置！！", self.id, 1))
		end
	end
end

function PartnerInfo:GetSkillFAttrs(skill_info)
	if nil == skill_info then return end
	local list = {}
	for k, v in pairs(skill_info) do
		local skill_attr = Language.Partner.AttrNames[k]
		if nil ~= skill_attr and v ~= 0 then
			table.insert(list, {
				seq = skill_attr[2],
				attr_name = skill_attr[1],
				attr_value = v,
			})
		end
	end
	return list
end

function PartnerInfo:CheckFetter()
	local qingyuan_list = self.info.qingyuan_list
	local qingyuan_type_list = self.info.qingyuan_type_list

	local fetter_cur = self.vo.fetter_cur
	local fetter_wash = self.vo.fetter_wash

	local function sort_color(a, b)
		return a.color < b.color
	end
	
	for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        self.fetter_cur[i] = qingyuan_list[fetter_cur[i]]
		self.fetter_wash[i] = qingyuan_list[fetter_wash[i]]
		if self.fetter_wash[i] then
			self.fetter_wash[i].fetter_wash_type = qingyuan_type_list[self.fetter_wash[i].qingyuan_type]
			if self.fetter_wash[i].fetter_wash_type then table.sort(self.fetter_wash[i].fetter_wash_type, sort_color) end
		end
	end
end

function PartnerInfo:CheckSkillList(param_t)
	-- for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
	-- 	local skill = self.info.skill_list[self.vo.skill_list[i]]
	-- 	if skill then
	-- 		self.sel_skill_list[i] = skill.info[self.cur_up_info["skill_level_"..skill.skill_index]]
	-- 	elseif -1 == self.vo.skill_list[i] then
	-- 		self.sel_skill_list[i] = {is_lock = true}
	-- 	end
	-- end

	local is_unlock = PartnerConfig.LockState.unlock == self.vo.is_unlocked
	for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
		local skill = self.info.skill_list2[i]
		local equip = 1 == self.vo.skill_list[i]
		if skill then
			local skill_info = skill.info[self.cur_up_info["skill_level_"..skill.skill_index]]
			self.sel_skill_list[i] = {is_equip = is_unlock and 1 == self.vo.skill_list[i], skill = skill_info or skill.info[1], is_lock = not is_unlock or nil == skill_info, need_level = skill.need_level, index = i}
		end
	end
	
	local function CheckSkillState(skill_id)
		for k, v in pairs(self.vo.skill_list) do
			if v == skill_id then return true end
		end
		return false
	end

	for k, v in pairs(self.info.skill_list) do
		if not param_t or param_t.level then
			self.cur_skill_list[k].info = v.info[self.cur_up_info["skill_level_"..v.skill_index]] or v.info[1]
			self.cur_skill_list[k].info.cur_level = self.vo.up_level
		end
		if not param_t or param_t.state then
			self.cur_skill_list[k].is_using = CheckSkillState(k)
		end
	end

	self.cur_skill_list_sort = {}
	for k, v in pairs(self.cur_skill_list) do
		table.insert(self.cur_skill_list_sort, v)
	end
	table.sort(self.cur_skill_list_sort, function(a, b)
		-- if a.info.need_level == b.info.need_level then
		-- 	return a.info.skill_id < b.info.skill_id
		-- else
		-- 	return a.info.need_level < b.info.need_level
		-- end
		if not a or not b then return a end
		return a.info.skill_index < b.info.skill_index
	end)
end

function PartnerInfo:CheckPEList()
	self.pe_list = {}
	local pe_suit_list1 = {}
	local is_a = true
	local suit_id = nil
	local color = nil
	local sc = {}
	for i = 1, PartnerConfig.PARTNER_PE_MAX do
		local pe = self.vo.pes[i]
		local item_data = pe.item_id ~= 0 and PartnerEquipment.New{
			item_id = pe.item_id,
			num = 1,
			is_bind = 0,
			param = {attr_list = pe.attr_list},
			unlock_level = pe.unlock_level
		} or PartnerEquipment.New{unlock_level = pe.unlock_level}
		self.pe_list[i] = item_data
		if 0 == pe.item_id then
			is_a = false
		else
			local detail, suit = PartnerData.Instance:GetPEConfigByItemId(self.pe_list[i].item_id)
			suit_id = suit_id or suit.suit_id
			color = color or suit.suit_color
			if suit.suit_id ~= suit_id then is_a = false end
			local key = suit.suit_id .. "_" .. suit.suit_color
			-- if nil == pe_suit_list1[suit.suit_id] or pe_suit_list1[suit.suit_id].suit_level > suit.suit_level then
			-- 	pe_suit_list1[suit.suit_id] = {
			-- 		suit_id = suit.suit_id,
			-- 		name = suit.name,
			-- 		desc = suit.desc,
			-- 		suit_level = suit.suit_level,
			-- 	}
			-- end
			if nil == sc[key] then
				sc[key] = i
				pe_suit_list1[i] = {
					suit_id = suit.suit_id,
					name = suit.name,
					desc = suit.desc,
					suit_level = suit.suit_level,
					color = suit.suit_color,
				}
			else
				local t = pe_suit_list1[sc[key]]
				if suit.suit_level < t.suit_level then
					pe_suit_list1[sc[key]] = {
						suit_id = suit.suit_id,
						name = suit.name,
						desc = suit.desc,
						suit_level = suit.suit_level,
						color = suit.suit_color,
					}
				end
			end
		end
	end
	for i = 1, PartnerConfig.PARTNER_PE_MAX do
		self.pe_list[i].pe_actived = is_a
	end
	local pe_suit_list2 = {}
	if is_a then
		for _, suit in pairs(pe_suit_list1) do
			suit.is_a = is_a
			pe_suit_list2[1] = pe_suit_list2[1] or suit
			pe_suit_list2[1] = (suit.suit_level < pe_suit_list2[1].suit_level or (suit.suit_level == pe_suit_list2[1].suit_level and suit.color < pe_suit_list2[1].color)) and suit or pe_suit_list2[1]
		end
	else
		for _, suit in pairs(pe_suit_list1) do
			suit.is_a = is_a
			table.insert(pe_suit_list2, suit)
		end
	end
	-- table.sort(pe_suit_list2, function (a, b)
	-- 	if not a or not b then return a end
	-- 	return a.suit_id < b.suit_id
	-- end)
	self.pe_suit_list = pe_suit_list2
end

function PartnerInfo:HPPMPP()
	local al = self.vo.attr_list
	return al[BattleAttriType.CurHp] / al[BattleAttriType.MaxHp], al[BattleAttriType.CurMp] / al[BattleAttriType.MaxMp]
end

function PartnerInfo:CheckAll()
	self:CheckUpLoop()
	self:CheckCurUpInfo()
	self:CheckSkillF()
	self:CheckFetter()
	self:CheckSkillList()
	self:CheckPEList()
	self:CheckCap()
	self:CheckUpLevelCost()
	self:CheckQualitySkill()
	self:CheckBiographyUp()
	self:CheckContractInfo()
	self:CheckPartnerGift()
end

function PartnerInfo:ClearRedPointShow()
	self.crystal_red_point_show = true
	self.life_active_red_point_show = true
	self.life_equip_red_point_show = true
	self.pact_equip_red_point_show = true
	-- self.fuyu_put_on_red_point_show = true
	-- self.fuyu_change_better_red_show = true
	-- self.strengthen_red_point_show = true
end

function PartnerInfo:CheckCap()
	local co = Cfg.PartnerScoreInfoByJobLevel(self.info.job, RoleData.Instance:GetRoleLevel())
	if co == nil then
		return
	end
	local cap = self.vo.cap
	if co.score_s <= cap then
		self.cap_sp = "scoreLevel3"
		self.cap_level = 3
	elseif co.score_a <= cap then
		self.cap_sp = "scoreLevel2"
		self.cap_level = 2
	elseif co.score_b <= cap then
		self.cap_sp = "scoreLevel1"
		self.cap_level = 1
	else
		self.cap_sp = "scoreLevel0"
		self.cap_level = 0
	end
end

function PartnerInfo:UpRedPoint(check_fight)
	local urp = true
	-- if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
	if PartnerConfig.LockState.lock == self.vo.is_unlocked  then
		urp = false
	elseif self.up_loop then
		if self.up_loop[self.vo.attr_stage + 1] then
			local items = self.up_loop[self.vo.attr_stage + 1].items
			for _, item in pairs(items) do
				urp = urp and (item:Num() >= item.cost_num)
			end
		else
			urp = urp and (self.cur_up_info.need_rolelevel <= RoleData.Instance:GetRoleLevel()) and (self.cur_up_info.level < #self.up_loop_t) and self:PartnerUpCanSoar()
			-- urp = urp and (self.cur_up_info.need_rolelevel <= self.vo.level) and (self.cur_up_info.level < #self.up_loop_t) and self:PartnerUpCanSoar()
		end
	end
	if check_fight == true and not self:IsFight() then
		urp = false
	end
	self.up_red_point.val = urp
	return urp
end

function PartnerInfo:CrystalRedPoint()
	local crp = true
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		crp = false
	else
		crp = 0 == self.vo.crystal and BagData.Instance:GetHaveCrystal()
	end
	self.crystal_red_point.val = crp
	return crp and self.crystal_red_point_show
end


function PartnerInfo:ActiRedPoint()
	local arp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked then
		if nil ~= self.info.activation_item and self.info.activation_item:Num() >= self.info.activation_item.cost_num then
			arp = true
		end
	end
	self.acti_red_point.val = arp
	return arp
end

function PartnerInfo:AttrLevelRedPoint(check_fight)
	local alrp = true
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state or self.vo.level >= RoleData.Instance:GetRoleLevel() then
		alrp = false
	else
		alrp = self.info.cost_up_level1 > 0 and RoleData.Instance:GetCurrency(CurrencyType.PartnerLevelItem) >= self.info.cost_up_level1
	end
	if check_fight == true and not self:IsFight() then
		alrp = false
	end
	self.attr_level_red_point.val = alrp
	return alrp
end

function PartnerInfo:BiographyRedPoint()
	local brp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or nil == self.biographys or RoleData.Instance:GetRoleLevel() < 31 then
		brp = false
	else
		for _, v in ipairs(self.biographys) do
			brp = brp or (v.flag_unlock and v.flag_get)
		end
	end
	self.biography_red_point.val = brp
	return brp
end

function PartnerInfo:SkillRedPoint()
	local srp = false
	if self.vo.up_level then
		for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
			local is_lock = self.vo.up_level + 1 < i
			srp = (nil == self.sel_skill_list[i] and not is_lock and PartnerConfig.LockState.unlock == self.vo.is_unlocked) or srp
		end
	end
	self.skill_red_point.val = srp
	return srp
end

function PartnerInfo:LifeActiveRedPoint()
	if nil == self.pe_list then return false end 
	local lrp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		lrp = false
	else
		local partner_pe_list = PartnerData.Instance:GetPartnerPEList()
		for i = 1, PartnerConfig.PARTNER_PE_MAX do
			lrp = (self.life_active_red_point_show and 0 == self.pe_list[i].vo.unlock_level and RoleData.Instance:GetCurrency(CurrencyType.LuckCoin) >= 160) or lrp
		end
	end
	self.life_active_red_point.val = lrp
	return lrp
end

function PartnerInfo:LifeEquipRedPoint()
	if nil == self.pe_list then return false end 
	local lrp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		lrp = false
	else
		local partner_pe_list = PartnerData.Instance:GetPartnerPEList(self.info.job)
		for i = 1, PartnerConfig.PARTNER_PE_MAX do
			local pe = self.pe_list[i]
			local ppe = partner_pe_list[i]
			if self.life_equip_red_point_show and ppe.num > 0 and pe.vo.unlock_level >= ppe.min_level then
				if 0 == pe.item_id then
					lrp = true
				-- elseif ppe.max_level > pe.limit_level then
				-- 	lrp = true
				-- elseif pe:PESuitProf() ~= RoleData.Instance:ProfType() and ppe.max_level == pe.limit_level then
				-- 	lrp = true
				end
			end
		end
	end
	self.life_equip_red_point.val = lrp
	return lrp
end

function PartnerInfo:QualityRedPoint(check_fight)
    local info,cfg = PartnerData.Instance:GetPartnerQualityInfoById( self.id,false )
	local is_unlock = PartnerConfig.LockState.unlock == self.vo.is_unlocked
	if cfg == nil then 
		self.quality_red_point.val = false
		return false 
	end 
	local is_level =  cfg.o_cfg.level_limit <= RoleData.Instance:GetRoleLevel() 
	local red = Item.GetNum(cfg.o_cfg.need_item) >= cfg.o_cfg.need_num and is_unlock and is_level and not cfg.is_max and not self:ParnetQualityUpNeedSoar()
	if check_fight == true and not self:IsFight() then
		red = false
	end
	self.quality_red_point.val = red
	return red
end

function PartnerInfo:GiftRedPoint()
	local is_unlock = PartnerConfig.LockState.unlock == self.vo.is_unlocked
	if not is_unlock then 
		self.gift_red_point.val = false
		return false
	end 

	-- 非出战则无视勾选，一律没有红点 20211221R
	if (self:IsFight() and not self:GetGiftRemind()) or not self:IsFight() then -- not self:GetGiftRemind() then 
		self.gift_red_point.val = false
		return false
	end 

	local red_num = 0
	for i = 1,10 do 
		if PartnerData.Instance:CheckGiftPointRed(self.id,i) then 
			red_num = red_num + 1
		end 
	end 

	local red = red_num > 0
	self.gift_red_point.val = red
	return red
end

function PartnerInfo:CheckPartnerGift()
	local role_id = RoleData.Instance:GetRoleId()
	local save_int = UnityPlayerPrefs.GetInt(role_id.."partner_gfit"..self.id)
	-- 信息清算
	if save_int == 0 then 
		-- 非出战状态的默认不勾
		self:SetGiftRemind(self:IsFight())
	end 
	if PartnerData.Instance:GetMarkGiftRedRemind() == self.id then 
		self:SetGiftRemind(true)
		PartnerData.Instance:MarkGiftRedRemind(-1)
	end 
end

function PartnerInfo:GetGiftRemind()
	local role_id = RoleData.Instance:GetRoleId()
	return UnityPlayerPrefs.GetInt(role_id.."partner_gfit"..self.id) > 0
end

function PartnerInfo:SetGiftRemind(flag)
	local role_id = RoleData.Instance:GetRoleId()
	UnityPlayerPrefs.SetInt(role_id.."partner_gfit"..self.id,flag and 1 or -1)
end

function PartnerInfo:NeedGuide()
	if 5 == self.info.activation_type then
		return PartnerConfig.LockState.lock == self.vo.is_unlocked
	end
	return true
end

function PartnerInfo:Guide()
	return 5 == self.info.activation_type and self:NeedGuide() and (self.info.activation_item and Item.GetNum(self.info.activation_item.item_id) or 0) >= self.info.activation_num
end

function PartnerInfo.SetQuality(sp, qua)
	sp.SpriteName = PartnerConfig.QuaSp[qua]
end

function PartnerInfo:IconId()
	if self:SkinIconId() then
		return self:SkinIconId()
	end
	return self.info.icon_id or 0
end

function PartnerInfo:SkinId()
	return self.vo.skin_id
end

function PartnerInfo:SkinIconId()
	if self:SkinId() ~= 0 then
		local co = Cfg.PartnerSkinById(self:SkinId())
        return co and co.head_portrait or nil
	end
end

function PartnerInfo:CheckUpLevelCost()
	local co = Cfg.PartnerLevelUpByIdLevel(self.id, self.vo.level)
	local cost_up_level1, cost_up_level10 = 0, 0
	local reset_up_level = 0
	if co then
		cost_up_level1, cost_up_level10 = co.price, co.price
		for i = self.vo.level + 1, self.vo.level + 9 do
			if i > 120 then break end
			local co_level_up = Cfg.PartnerLevelUpByIdLevel(self.id, i)
			if co_level_up then
				cost_up_level10 = cost_up_level10 + co_level_up.price
			else
				LogError(string.format("Config.partner_cfg_auto.partner_levelup id=%s level=%s 缺少配置！！", self.id, i))
				break
			end
		end
		for i = 1, self.vo.level - 1 do
			local co_level_up = Cfg.PartnerLevelUpByIdLevel(self.id, i)
			if co_level_up then
				reset_up_level = reset_up_level + co_level_up.price
			else
				LogError(string.format("Config.partner_cfg_auto.partner_levelup id=%s level=%s 缺少配置！！", self.id, i))
				break
			end
		end
	end
	self.info.cost_up_level1 = cost_up_level1
	self.info.cost_up_level10 = cost_up_level10
	self.info.reset_up_level = reset_up_level
end

function PartnerInfo:UpLevelIsEnough(up_level)
	local have_num = RoleData.Instance:GetCurrency(CurrencyType.PartnerLevelItem)
	local is_succ = true
	local role_level = RoleData.Instance:GetRoleLevel()
	if self.vo.level < role_level then
		if 10 == up_level then
			if self.info.cost_up_level10 <= have_num then
				PartnerCtrl.Instance:SendPartnerUpLevel(self.id, math.min(up_level, role_level - self.vo.level))
			elseif self.info.cost_up_level1 <= have_num then
				local cost_up_level = self.info.cost_up_level1
				local can_up_level = 1
				for i = self.vo.level + 1, self.vo.level + 9 do
					if i > 120 then break end
					local co_level_up = Cfg.PartnerLevelUpByIdLevel(self.id, i)
					if co_level_up and i < role_level then
						if cost_up_level + co_level_up.price <= have_num then
							can_up_level = can_up_level + 1
							cost_up_level = cost_up_level + co_level_up.price
						end
					else
						break
					end
				end
				PartnerCtrl.Instance:SendPartnerUpLevel(self.id, can_up_level)
			else
				is_succ = false
				ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.PartnerLevelItem]})})
			end
		else
			if self.info.cost_up_level1 <= have_num then
				PartnerCtrl.Instance:SendPartnerUpLevel(self.id, 1)
			else
				is_succ = false
				ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.PartnerLevelItem]})})
			end 
		end
	else
		is_succ = false
		PublicPopupCtrl.Instance:Center(Language.Partner.Attr.level_up_role)
	end
	return is_succ
end

function PartnerInfo:CheckQualitySkill()
	if self.vo.init or self.quality.init then return end
	local pqc = PartnerData.Instance.partner_quality_us_config
	if pqc[self.id] and pqc[self.id][self.quality.operate_qua] and pqc[self.id][self.quality.operate_qua][self.quality.quality_segment] then
		local skill_co = pqc[self.id][self.quality.operate_qua][self.quality.quality_segment]
		local is_unlock = PartnerConfig.LockState.unlock == self.vo.is_unlocked
		for i = 1, PartnerConfig.PARTNER_SKILL_AN_MAX do
			local skill = self.info.skill_list2[i]
			local equip = 1 == self.vo.skill_list[i]
			if skill then
				local pqc2 = PartnerData.Instance.partner_quality_sl_config
				local skill_info_cur = skill.info[skill_co["skill_level_"..skill.skill_index]]
				local skill_info_next = skill.info[skill_co["skill_level_"..skill.skill_index] + 1]
				local skill_type = SkillData.GetSkillType((skill_info_cur or skill.info[1]).skill_id)
				local co_next
				if skill_info_next and pqc2[self.id][i][skill_info_next.skill_level] then
					co_next =  pqc2[self.id][i][skill_info_next.skill_level]
				end
				self.quality_skill_list[i] = {
					index = i,
					info = skill_info_cur or skill.info[1],
					is_lock = not is_unlock or nil == skill_info_cur,
					is_equip = is_unlock and 1 == self.vo.skill_list[i],
					skill_cur = skill_info_cur,
					skill_next = skill_info_next,
					partner_name = self.info.name,
					need_level = co_next and string.format(Language.Common.Xstr,
						QualityColorStrShallow[co_next.quality_upgrade + PartnerConfig.QuaFetter.Grassy],
						Language.Partner.QualityName[co_next.quality_upgrade] .. string.format(Language.Partner.QualitySeg, DataHelper.GetDaXie(co_next.quality_segment))) or "",
					skill_type = (7 == skill_type or 8 == skill_type) and PartnerConfig.SkillType.passive or PartnerConfig.SkillType.active,
				}
			end
		end
	else
		LogError(string.format("Config.partner_cfg_auto.parnter_quality_upgrade id=%s quality=%s quality_segment=%s 缺少配置！！", self.id, self.vo.quality, self.vo.quality_segment))
	end
end

function PartnerInfo:Level()
	return self.vo.level
end
function PartnerInfo:Name()
	return self.info.name
end

function PartnerInfo:IsUnlocked()
	return self.vo.is_unlocked == PartnerConfig.LockState.unlock
end

function PartnerInfo:Id()
	return self.id
end

function PartnerInfo:Quality()
	return self.quality.quality
end

--最大进阶等级
function PartnerInfo:CanUpMaxGrade()
	local role_level = RoleData.Instance:GetRoleLevel()
	local max_val = 1
	for i, v in ipairs(self.info.up_info) do
		if v.need_rolelevel > role_level then
			max_val = i
			break
		elseif i == #self.info.up_info then
			max_val = i
		end
	end
	return max_val
end

function PartnerInfo:IsFight()
	return self:IsUnlocked() and self.vo.fight_state == PartnerConfig.FightState.fight
end

function PartnerInfo:GetScore()
	return self.vo.cap, self.cap_level
end


function PartnerInfo:GetPactAttrList(attr_up)
	local attr_list = {}
	if nil ~= self.vo then
		local contract_attrs = self.vo.contract_attrs
		for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
			attr_list[i] = {
				attr_type = contract_attrs[i].attr_type,
				attr_value = math.floor(contract_attrs[i].attr_value * attr_up / 100),
			}
		end
	end
	return attr_list
end

function PartnerInfo:GetPartnerScaleScore()
	if nil == self.scale_score then
		local co = Cfg.ScaleScoreCfgByProf(self.info.job * 100)
		local list = {}
		for k, v in pairs(co) do 
			if AttrTypeForName[k]~= nil then 
				list[AttrTypeForName[k]] = v
			end 
		end
		self.scale_score = list
	end
	return self.scale_score
end

function PartnerInfo:GetContractAttrs()
	return self.vo.contract_attrs or {}
end

function PartnerInfo:PartnerPactBaseAttr(protocol)
	self.vo.contract_attrs = protocol.contract_attrs
end

function PartnerInfo:PartnerPactInfo(protocol)
	self.vo.contracts = protocol.contracts
	self:CheckContractInfo()
	local pos = protocol.param + 1
	if 1 == protocol.type then
		PartnerData.Instance:SetPartnerPactUpEffect(1, self.id, protocol.param + 1)
	elseif protocol.contracts[pos] > 0 then
		PartnerData.Instance:SetPartnerPactUpEffect(2, self.id, protocol.param + 1)
	end
end

function PartnerInfo:PactPartnerNum(partner_id)
	local num = 0
	local contracts = self.vo and self.vo.contracts or {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
		local pact_item = self.pact_items[i]
		if pact_item.item_id > 0 then
			if partner_id == pact_item.item:PartnerId() then
				num = num + 1
			end
		end
    end
	return num
end

function PartnerInfo:IsPactPartner(partner_id)
	local is_pact = false
	local contracts = self.vo and self.vo.contracts or {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
		local pact_item = self.pact_items[i]
		if pact_item.item_id > 0 then
			if partner_id == pact_item.item:PartnerId() then
				is_pact = true
				break
			end
		end
    end
	return is_pact
end

function PartnerInfo:PactEquipRedPoint()
	if nil == self.pact_items then return false end 
	local crp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		crp = false
	else
		for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
			local pact_item = self.pact_items[i]
			if self.pact_equip_red_point_show and 0 == pact_item.item_id then
				if PartnerData.Instance:GetPartnerPactItemsEquipCheck(self.id) then
					crp = true
				end
			end
		end
	end
	self.pact_equip_red_point.val = crp
	return crp
end

function PartnerInfo:PactUpRedPoint()
	if nil == self.pact_items then return false end 
	local crp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		crp = false
	else
		for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
			local pact_item = self.pact_items[i]
			if pact_item.item_id > 0 then
				if PartnerData.Instance:GetPartnerPactItemsUpCheck(pact_item.item) then
					crp = true
				end
			end
		end
	end
	self.pact_up_red_point.val = crp
	return crp
end

function PartnerInfo:PartnerSoarNotice(protocol)
	self.vo.soar_count = protocol.soar_count
	self.vo.soar_weapon_level = protocol.soar_weapon_level
end

function PartnerInfo:PartnerSoarBtnShow()
	local value = false
	local soar_count = self.vo.soar_count or 0
	local co = PartnerData.Instance:GetPartnerSoarCoByCount(soar_count + 1)
	if co then
	-- 	value = RoleData.Instance:GetRoleLevel() >= co.soar_level and TaskData.Instance:GetTaskIsCompleted(co.soar_task)
		value = FlyUpData.Instance:GetStage() >= soar_count + 1
	end
	return value, soar_count
end
function PartnerInfo:SoarCount()
	return self.vo.soar_count or 0
end
--是否可激活飞升武器
function PartnerInfo:IsCanUpWeapon()
	return self.quality.operate_qua >= PartnerData.Instance:WeaponLimit()
end
--刚好激活了飞升武器
function PartnerInfo:ShowWeapon()
	return self.quality.operate_qua == PartnerData.Instance:WeaponLimit()
end
--是否未激活飞升武器
function PartnerInfo:IsUnactiveWeapon()
	return self.vo.soar_weapon_level == 0
end


-- true 是需要 false 是不需要
function PartnerInfo:ParnetQualityUpNeedSoar()
	local pos_cur,pos_next,pos_after = PartnerData.Instance:GetPartnerQuaShowById(self.id)	

	local soar_count = self.vo.soar_count or 0
	local co = PartnerData.Instance:GetPartnerSoarCoByCount(soar_count == 0 and 1 or soar_count + 1)
	if pos_next ~= nil and co ~= nil then 
		-- 检查是否抵達飛昇條件
		-- LogError("cur !", pos_cur.cfg.quality_upgrade,pos_cur.cfg.quality_segment)
		-- LogError("cur !check", co.upgrade_condition,co.segment_condition)
		if pos_cur.cfg.quality_upgrade > co.upgrade_condition or 
		    (pos_cur.cfg.quality_upgrade == co.upgrade_condition and pos_cur.cfg.quality_segment >= co.segment_condition ) then 
			-- -- 檢查下一级的觉醒是否在现飞升阶段的上限内
			-- -- 如果在范围内，则不需要飞升
			-- LogError("next !", pos_next.cfg.quality_upgrade,pos_next.cfg.quality_segment)
			-- LogError("next !check", co.quality_upgrade,co.quality_segment)
			-- 佛了 写死算了
			-- 升5-1要飞升1
			-- 升5-4要飞升2
			if self.vo.soar_count == 0 and pos_next.cfg.quality_upgrade == 5 and pos_next.cfg.quality_segment == 1 then 
				return true
			elseif self.vo.soar_count == 1 and pos_next.cfg.quality_upgrade == 5 and pos_next.cfg.quality_segment == 4 then 
				return true
			else 
			    return false
			end
			-- elseif pos_next.cfg.quality_upgrade < co.quality_upgrade or 
			--     ( pos_next.cfg.quality_upgrade == co.quality_upgrade and pos_next.cfg.quality_segment < co.quality_segment) then 
			-- 	return false
			-- elseif pos_next.cfg.quality_upgrade == co.quality_upgrade and pos_next.cfg.quality_segment == co.quality_segment then
			-- 	return co.soar_frequency <= self.vo.soar_count
			-- end 
		else 
			return false
		end 
	else 
		return false
	end 
end

function PartnerInfo:GetPartnerSoarArmsCo(level, need_attrs, type_key)
	level = level or self.vo.soar_weapon_level or 1
	local attrs = {}
	local co = Cfg.PartnerSoarArmsByIdLevel(self.info.arms_id, level, false)
	if co and need_attrs then
		if type_key then
			for k, v in pairs(AttrTypeForName) do
				if co[k] and co[k] > 0 then
					attrs[AttrTypeForName[k]] =  {
						attr_type = AttrTypeForName[k],
						attr_value = co[k],
					}
				end
			end
		else
			for k, v in pairs(AttrTypeForName) do
				if co[k] and co[k] > 0 then
					table.insert(attrs, {
						attr_type = AttrTypeForName[k],
						attr_value = co[k],
					})
				end
			end
		end
	end
	return co, attrs
end

function PartnerInfo:GetAttibuteDanLimit(limit)
	local soar_count = self.vo.soar_count or 0
	if soar_count > 0 then
		local co = PartnerData.Instance:GetPartnerSoarCoByCount(soar_count)
		limit = co and co.attribute_pellet_limit or limit
	end
	return limit
end

function PartnerInfo:PartnerUpCanSoar()
	local up_level_max = PartnerData.Instance.config_other.upgrade_level_limit
	local soar_count = self.vo.soar_count or 0
	local co = PartnerData.Instance:GetPartnerSoarCoByCount(self.vo.soar_count)
	if co then
		up_level_max = co.upgrade_level
	end
	return self.vo.up_level < up_level_max or (self.vo.up_level == up_level_max and self.up_loop and self.up_loop[self.vo.attr_stage + 1])
end

function PartnerInfo:PartnerWeaponUpCanSoar()
	local up_level_max = 0
	local soar_count = self.vo.soar_count or 0
	local co = PartnerData.Instance:GetPartnerSoarCoByCount(self.vo.soar_count)
	if co then
		up_level_max = co.arms_level
	end
	return self.vo.soar_weapon_level < up_level_max
end

function PartnerInfo:SoarRedPoint()
	local srp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked then
		srp = false
	elseif self.vo.init or self.quality.init then
		srp = false
	else
		local soar_count = self.vo.soar_count or 0
		local co = PartnerData.Instance:GetPartnerSoarCoByCount(soar_count + 1)
		if co then
			srp = TaskData.Instance:GetTaskIsCompleted(self.info.soar_fin_task) and FlyUpData.Instance:GetStage() >= soar_count + 1 and self.vo.level >= co.soar_level and ((self.quality.operate_qua > co.upgrade_condition) or ((self.quality.operate_qua == co.upgrade_condition) and (self.quality.quality_segment >= co.segment_condition)))
		end
	end
	self.soar_red_point.val = srp
	return srp
end
function PartnerInfo:FuYuCanPutOn()
	local list = PartnerData.Instance:GetFuYuBagList()
	local list2 = self:FuYuList()
	if table.nums(list) > 0 --and self.fuyu_put_on_red_point_show == true
	and table.nums(list2) ~= 3 then
		return true
	end
	return false
end
function PartnerInfo:FuYuCanChange()
	local list = PartnerData.Instance:GetFuYuBagList()
	local list2 = self:FuYuList()
	local max_level = 0
	local item
	for k, v in pairs(list2) do
		item = Item.Create(v)
		max_level = math.max(max_level, item:FuYuLevel())
	end
	for k, v in pairs(list) do
		if v:FuYuLevel() > max_level and self.fuyu_change_better_red_show == true then
			return true
		end
	end
	return false
end

function PartnerInfo:SoarWeaponRedPoint(is_str)
	local swrp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked then
		swrp = false
	else
		local soar_count = self.vo.soar_count or 0
		if soar_count > 0 then
			local co = PartnerData.Instance:GetPartnerSoarCoByCount(soar_count)
			if co then
				local arms_level_max = co.arms_level
				local soar_weapon_level = self.vo.soar_weapon_level or 0
				if soar_weapon_level < arms_level_max then
					local soar_arms_co = self:GetPartnerSoarArmsCo(soar_weapon_level)
					if co then
						swrp = true
						for i = 0, #soar_arms_co.item_list do
							local item_data = soar_arms_co.item_list[i]
							local have_num = BagData.Instance:GetNumByItemId(item_data.item_id)
							local need_num = item_data.num
							if have_num < need_num then
								swrp = false
								break
							end
						end
					end
				end
			end
			if is_str then
				return swrp
			end
			-- if swrp ==false and self:SoarCount() > 0 and self.vo.fight_state == PartnerConfig.FightState.fight then
			if swrp ==false and self:IsUnlocked() and self.vo.fight_state == PartnerConfig.FightState.fight then
				swrp = self:FuYuCanPutOn()
				--[[ if swrp == false then
					swrp = self:FuYuCanChange()
				end ]]
			end
		end
	end
	self.soar_weapon_red_point.val = swrp
	return swrp
end


function PartnerInfo:StrengthenRedPoint()
	local srp = false
	if PartnerConfig.LockState.lock == self.vo.is_unlocked or PartnerConfig.FightState.rest == self.vo.fight_state then
		srp = false
	else
		if self.vo.intensify_level < PartnerConfig.PARTNER_STRENGTHEN_LEVEL_MAX then--and self.strengthen_red_point_show then
			local co = Cfg.ParnterIntensifyByIdLevel(self.id, self.vo.intensify_level)
			if nil == co.consumes_show then PartnerData:PartnerStrConsumesParse(co) end
			if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= co.coin and self.vo.level >= co.levels and self.vo.soar_count >= co.feisheng_num then
				srp = true
				local need_num_add = 0
				for i = 1, 3 do
					local consume_item = co.consumes_items[i]
					if consume_item.item_id > 0 then
						if CommonItemId.PartnerPactNormal == consume_item.item_id then
						else
							need_num_add = need_num_add + consume_item:Num()
							if PartnerData.Instance:GetPatnerPactHaveNum(consume_item.item_id) < consume_item:Num() then
								srp = false
							end
						end
					end
				end
				local have_num = 0
				local temp = {}
				for _, info in pairs(PartnerData.Instance.partner_pact_items:Val()) do
					local item = info.item
					if 0 == item:StarLevel() and nil == temp[item.item_id] then
						have_num = have_num + PartnerData.Instance:GetPatnerPactHaveNum(item.item_id)
						temp[info.item_id] = true
					end
				end
				if have_num < (co.consumes_items[2]:Num() + need_num_add) then
					srp = false
				end
			end
		end
	end
	self.strengthen_red_point.val = srp
	return srp
end

function PartnerInfo:PartnerQuaValShowUp()
	if nil == self.partner_qua_val_show then
		local qua = self.info.partner_quality
		local qua_val = self.info.qua_value
		self.partner_qua_val_show = string.format(Language.Partner.PartnerQuaValShow, Language.Partner.QuaColor[qua] or "", qua_val or 0)
	end
	return self.partner_qua_val_show
end

function PartnerInfo:PartnerIntensifyInfo(protocol)
	self.vo.intensify_level = protocol.intensify_level
end

function PartnerInfo.StrLevelSp(str_level)
	return string.format(PartnerConfig.StrLevelSp, str_level)
end

function PartnerInfo:PartnerTopRankInfo(rank)
	self.vo.rank = rank
end
function PartnerInfo:SetFuYuInfo(list, info, setnil)
	if list then
		self.fuyu_item_list = list
	end
	if info then
		if info.item_id == 0 then
			self.fuyu_item_list[info.wear_idx] = nil
		else
			self.fuyu_item_list[info.wear_idx] = info
		end
	end
end
function PartnerInfo:FuYuList()
	return self.fuyu_item_list
end
function PartnerInfo:FuYuItemList()
	local list = {}
	local data = self:FuYuList()
	for k, v in pairs(data) do
		table.insert(list, Item.Create(v))
	end
	return list
end
function PartnerInfo:FuYuListDataAndEmpty()
	local temp = {}
	for i = 1, 3 do
		if self.fuyu_item_list[i-1] then
			table.insert(temp, self.fuyu_item_list[i-1])
		else
			table.insert(temp, {is_empty = true, wear_idx = i - 1})
		end
	end
	return temp
end

function PartnerInfo:EmptyVo()
	local t = {}

	--列表
	t.partner = self.id -- 伙伴 ID
	t.is_unlocked = 0 -- 解锁状态: 0 - 锁定; 1 - 解锁
	t.fight_state = 0 -- 战斗状态: 0 - 待机; 1 - 出战
	--进阶
	t.up_level = 0 -- 进阶等级
	t.attr_stage = 0 -- 属性已经突破的阶段, [0, 8],进阶成功后,此数据会被重置为0

	t.skill_list = {}
	for i = 1, PartnerConfig.PARTNER_SKILL_EN_MAX do
		t.skill_list[i] = 0
	end

	--情缘
	t.fetter_cur = {} -- 当前框伙伴情缘
	for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
		t.fetter_cur[i] = 0
	end
	t.fetter_wash = {} -- 洗练框伙伴情缘
	for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
		t.fetter_wash[i] = 0
	end

	--命格
	t.pes = {}
	for i = 1, PartnerConfig.PARTNER_PE_MAX do
		local data = {attr_list = {}}
		data.item_id = 0
		data.unlock_level = 0
		-- data.attr_type = 0
		-- data.attr_value = 0
		for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
			data.attr_list[i] = {
				attr_type = 0,
				attr_value = 0,
			}
		end
		t.pes[i] = data
	end

	--属性
	t.cap = 0

	t.is_arena_match = 0 -- 是否守擂中
	t.task_finish = 1 -- 是否完成进阶所需任务
	t.intensify_level = 1 -- 强化等级
	t.count = 50 -- 改变的属性数量

	t.attr_list = {}
	local attr_co = Cfg.PartnerLevelUpByIdLevel(self.id, RoleData.Instance:GetRoleLevel()) or {}
	for i = 0, t.count - 1 do
		t.attr_list[i] = AttrNameForType[i] and attr_co[AttrNameForType[i]] or 0
	end
	t.attr_list[BattleAttriType.CurHp] = t.attr_list[BattleAttriType.MaxHp]
	t.attr_list[BattleAttriType.CurMp] = t.attr_list[BattleAttriType.MaxMp]

	t.crystal = 0 -- 装备元素水晶id
	t.used_times = 0 -- 装备元素水晶剩余使用次数
	
	t.skin_id = 0    -- 穿戴的皮肤id 未穿戴发0
	t.level = 1    -- 等级

	t.biography_open_flag = 0
	t.biography_fetch_flag = 0
	t.soar_count = 0	-- 伙伴飞升次数

	t.contracts = {}
	for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
		t.contracts[i] = 0
	end

	t.contract_attrs = {}
	for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
		t.contract_attrs[i] = {
			attr_type = PartnerConfig.ContractI2Type[i],
			attr_value = 0,
		}
	end
	t.soar_weapon_level = 0    --伙伴飞升武器阶数
	t.rank = 0

	return t
end