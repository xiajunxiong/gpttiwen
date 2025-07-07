FightMod = {
	[1] = Mod.FightFB.Material,
	[2] = Mod.FightFB.Fight,
	[3] = Mod.FightFB.PetIsland,
	[4] = Mod.FightFB.PetGod,
	[5] = Mod.FightFB.DreamNotes,
}

function FightData:FightFBData()
	self.fight_fb_sm = SmartData.New({
		sel_tab = 1,
	})

	self.fight_fb_energy_show = SmartData.New{is_show = true}

	self.fight_fb_ymz = SmartData.New{
		type = 1,
		page = 1,
		info = SmartData.New(),
		flush_sel = {move = false},
		normal_fb_record_list = SmartData.New(),
		normal_fb_record_bt_list = {},
	}
	
	self.fight_rare_dungeon = SmartData.New{
		info = SmartData.New(),
	}

	self.fight_elite_dungeon = SmartData.New{
		info = SmartData.New(),
	}
	
	self.fight_fb_ymz_type_item_flush = SmartData.New{val = false}

	local ymz_page_list = {
		[1] = {},
		[2] = {},
		[3] = {},
		[4] = {},
		[5] = {},
	}
	local ymz_type_list = {
		[1] = {},
		[2] = {},
		[3] = {},
		[4] = {},
		[5] = {},
	}
	for _, v in pairs(self.dungeon_list) do
		local details = v.details
		for _, info in pairs(details) do
			if 1 == info.is_show then
				info.ymz_type = FightConfig.YMZST2T[v.type]
				info.up_types = {}
				for rt, rn in pairs(FightConfig.YMZRewards) do
					info[rn] = {}
					if not StringIsEmpty(info[rt]) then
						for y, z in pairs(string.split(info[rt], ":")) do
							local item_id = tonumber(z)
							local item_data = Item.New()
							item_data.old_id = Cfg.RewardJobBagHasId(item_id, false) and item_id or nil
							item_data.item_id = item_id
							item_data.num = 1
							item_data.is_bind = 0
							item_data.is_rare_dw = info.rewards_rare[item_id] or false
							item_data.is_unique_formula = info.unique_formula[item_id] or false
							item_data.is_up_dw = info.rewards_up[item_id] or false
							item_data.is_rate_dw = info.rewards_rate[item_id] or false
							item_data.block_pre_range = true
							table.insert(info[rn], item_data)
							if info.rewards_up[item_id] then
								info.up_types[FightConfig.YMZRewardsIndex[rt]] = true
							end
						end
					end
				end
				table.insert(ymz_page_list[info.ymz_type], info)
				table.insert(ymz_type_list[v.type], info)
			end
		end
	end
	for _, v in pairs(ymz_page_list) do
		table.sort(v, function(a, b)
			if not a or not b then return a end
			if a.sort == b.sort then
				return a.scene_type_1 > b.scene_type_1
			end
			return a.sort < b.sort
		end)
	end
	-- for index, info in ipairs(ymz_page_list[2]) do
	-- 	info.icon_index = index
	-- end
	for index, info in ipairs(ymz_page_list[3]) do
		info.show_index = index
	end
	self.ymz_page_list = ymz_page_list
	self.ymz_type_list = ymz_type_list


	self.pet_god_list = {}
	self.pet_god_type_level_list = {}
	self.pet_god_seq_list = {}
	self.pet_god_rate_list = {}
	for k, v in ipairs(self.pet_god_config_main_drop_show) do
		local rewards = {}
		for y, z in pairs(string.split(v.drop_show, "|")) do
			table.insert(rewards, Item.Init(tonumber(z), 1, 0))
		end
		v.rewards = rewards
		self.pet_god_list[v.seq] = v
	end
	for k, v in ipairs(self.pet_god_config_crystal) do
		local rewards = {}
		local monsters = {}
		for y, z in pairs(string.split(v.drop_show, "|")) do
			table.insert(rewards, Item.Init(tonumber(z), 1, 0))
		end
		for y, z in pairs(string.split(v.boss_show, "|")) do
			local monster_id = tonumber(z)
			local icon_id = 0
			local co = Cfg.MonsterById(monster_id)
			if co then icon_id = co.icon_id end
			table.insert(monsters, {icon_id = icon_id})
		end
		v.rewards = rewards
		v.monsters = monsters
		self.pet_god_seq_list[v.seq] = v
		self.pet_god_type_level_list[v.fb_type] = self.pet_god_type_level_list[v.fb_type] or {chapter_name = v.chapter_name, levels = {}}
		self.pet_god_type_level_list[v.fb_type].levels[v.level] = v
	end
	for k,v in pairs(self.pet_god_config_rate_show) do
        self.pet_god_rate_list[v.reward_group] = self.pet_god_rate_list[v.reward_group] or {}
        local item_list = {}
		for i = 0, #v.reward_id do
            local item_data = v.reward_id[i]
            item_list[i + 1] = Item.Init(item_data.item_id, item_data.num, item_data.is_bind)
        end
        self.pet_god_rate_list[v.reward_group][v.seq + 1] = {cfg = v, item_list = item_list}
    end
	self.pet_god_info = SmartData.New({
		type = 0,
		info = SmartData.New(),
		times = SmartData.New{
			day_pass_times = 0,
			day_buy_times = 0,
			day_remain_time = 0,
		},
		scene = SmartData.New{
			fb_seq = 1,
			is_finish = 0,
			boss_npc_id = 0,
		},
		max_levels = SmartData.New()
	})
end

function FightData:ClearFightFBData()

end

function FightData:FightFBYMZAutoSel()
	local fb_type = self.fight_fb_ymz.type
	local role_level = RoleData.Instance:GetRoleLevel()
	local list = self.ymz_page_list[fb_type]
	if nil == list then return end
	local info_sel = list[1]
	if self.ymz_open_seq then
		info_sel = self.dungeon_seq_list[self.ymz_open_seq] or info_sel
		-- self.ymz_open_seq = nil
	else
		for _, info in ipairs(list) do
			if role_level >= info.level_min then
				info_sel = ((info_sel.level_min == info.level_min and info.scene_type_1 > info_sel.scene_type_1) or info_sel.level_min < info.level_min) and info or info_sel
			end
		end
	end
	if info_sel.ymz_type == self.fight_fb_ymz.type then
		self.ymz_open_seq = nil
	end
	self.fight_fb_ymz.type = FlyUpData.Instance:IsFinish() and info_sel.ymz_type or 1
	self.fight_fb_ymz.page = info_sel.ymz_page
	self.fight_fb_ymz.info:Set(info_sel)
	self:FightFBYMZFlushSel(false)
end

function FightData:GetClickTips()
	return FightConfig.FBTextDesc[self.fight_fb_sm.sel_tab] or 0
end

function FightData:SetFightSelectData(value)
	self.fight_fb_sm.sel_tab = value
end

function FightData:FightFBYMZTypeItemFlush()
	self.fight_fb_ymz_type_item_flush.val = not self.fight_fb_ymz_type_item_flush.val
end

function FightData:FightFBEnergyShow(is_show)
	self.fight_fb_energy_show:Set{is_show = is_show}
end

function FightData:FightFBYMZHaveNumAll(type)
	local num = 0
	local fb_type = type or self.fight_fb_ymz.type
	local list = self.ymz_page_list[fb_type]
	if list then
		for _, info in ipairs(list) do
			num = num + BagData.Instance:GetNumByItemId(info.expend_item)
		end
	end
	return num
end

function FightData:FightFBYMZHaveNumAllElite()
	local num = 0
	local fb_type = 2
	local list = self.ymz_page_list[fb_type]
	for _, info in ipairs(list) do
		num = num + BagData.Instance:GetNumByItemId(info.expend_item)
		break
	end
	return num
end

function FightData:FightFBYMZFlushSel(value)
	self.fight_fb_ymz.flush_sel = {is_move = value}
end

function FightData:FightFBYMZOpenSel(seq)
	self.ymz_open_seq = tonumber(seq)
end

function FightData:FightFBRareDungeonOpenSel(seq)
	self.rare_dungeon_open_seq = tonumber(seq)
end

function FightData:FightFBRareDungeonAutoSel()
	local list = self.ymz_page_list[3]
	local role_level = RoleData.Instance:GetRoleLevel()
	local info_sel = list[1]
	if self.rare_dungeon_open_seq then
		info_sel = self.dungeon_seq_list[self.rare_dungeon_open_seq] or info_sel
		self.rare_dungeon_open_seq = nil
	else
		for _, info in ipairs(list) do
			if role_level >= info.level_min then
				info_sel = info
				if not self:IsFightCrystalPassFb(info.scene_seq) then
					break
				end
				-- info_sel = ((info_sel.level_min == info.level_min and info.scene_type_1 > info_sel.scene_type_1) or info_sel.level_min < info.level_min) and info or info_sel
			end
		end
	end
	self.fight_rare_dungeon.info:Set(info_sel)
end

function FightData:GetRareDungeonRemindNum()
	for _, info in ipairs(self.ymz_page_list[3]) do
		local is_pass = self:IsFightCrystalPassFb(info.scene_seq)
		local is_geted = self:IsFightCrystalRewardGetFb(info.scene_seq)
		if is_pass and not is_geted then
			return 1
		end
	end
	--有挑战次数的时候只显示一次红点，界面被打开后红点就消失
	local num = self.fight_crystal_sweep.new_xiyou_play_times_remain
	if num > 0 then
		if self.fight_crystal_sweep.has_view_opened then
			return 0
		else
			return 1
		end
	end
	return 0
end

function FightData:ResetPetGoldInfoType()
	self.pet_god_info.type = 0
end

function FightData:ResetPetGoldInfoInfo()
	self.pet_god_info.info:Set{}
end

function FightData:GetFightPetGodExpNum(seq)
	local role_level = RoleData.Instance:GetRoleLevel()
	local exp_num = 0
	local use_power = self.pet_god_seq_list[seq] and self.pet_god_seq_list[seq].power_expend or 0
	for _, v in ipairs(self.pet_god_config_exp) do
		if role_level == v.level then
			exp_num = v.exp * use_power
			break
		end
	end
	return exp_num
end

function FightData:GetFightPetGodNpcInfo(seq)
	local role_level = RoleData.Instance:GetRoleLevel()
	for k, v in ipairs(self.pet_god_config_monster_att_group) do
		if v.seq == seq and v.levels_min <= role_level and v.level_max >= role_level then
			return NpcHelper.NpcCo(v.npc_id)
		end
	end
end

function FightData:SetPetGodFBRoleInfo(protocol)
	-- local info = protocol.info
	-- info.day_remain_time = self.pet_god_config_other.enter_num - info.day_pass_times + info.day_buy_times
	self.pet_god_info.max_levels:Set(protocol.info)
end

function FightData:SetPetGodFBSceneInfo(protocol)
	self.pet_god_info.scene:Set(protocol.info)
end

function FightData:GetPetGodExtraBuyTimes()
	return self.pet_god_config_other.extra_buy_times or 0
end

function FightData:GetPetGodExtraBuyConsume()
	local buy_times = self.pet_god_info.times.day_buy_times + 1
	local price = 0
	for k, v in ipairs(self.pet_god_config_buy_power) do
		if buy_times == v.buy_times then
			price = v.price
			break
		end
	end
	return price
end

function FightData:FightFBPetGodInfoAutoSel()
	local type = self.pet_god_info.type
	if 0 == type then return end
	local info = self.pet_god_type_level_list[type]
	if nil == info then return end
	local max_level = self.pet_god_info.max_levels[type] or 0
	local next_level = max_level + 1
	local sel_info = info.levels[1]
	for k, v in pairs(info.levels) do
		if next_level >= v.level and sel_info.level < v.level then
			sel_info = v
		end
	end
	return sel_info
end

function FightData:FightFBEliteDungeonOpenSel(seq)
	self.elite_dungeon_open_seq = tonumber(seq)
end

function FightData:GetFightElitePage()
	local page = 2
	if self.elite_dungeon_open_seq then
		page = self.dungeon_seq_list[self.elite_dungeon_open_seq] and self.dungeon_seq_list[self.elite_dungeon_open_seq].ymz_type or 2
	end
	return page 
end

function FightData:GetFightEliteList(page)
	local list = self.ymz_page_list[page]
	local role_level = RoleData.Instance:GetRoleLevel()
	local info_sel = list[1]
	local temp = {}
	for k, v in ipairs(list) do
		if v.is_feisheng > 0 then
			if FlyUpData.Instance:GetStage() >= v.is_feisheng then
				table.insert(temp, v)
			end
		else
			table.insert(temp, v)
		end
	end
	list = temp
	if self.elite_dungeon_open_seq then
		info_sel = self.dungeon_seq_list[self.elite_dungeon_open_seq] or info_sel
		self.elite_dungeon_open_seq = nil
	else
		for _, info in ipairs(list) do
			if role_level >= info.level_min  then
				info_sel = (info_sel.level_min < info.level_min) and info or info_sel
			end
		end
	end
	return list, info_sel
end


function FightData:SetFightEliteWaitFunc(click_handle)
	if SceneData.Instance:GetMode() == SceneModeType.FightFly2 then
		Call(click_handle)
	else
		self.fight_elite_wait_func = click_handle
		SceneCtrl.Instance:Logic():AutoFlyToScene(self:ConfigOtherHuashengScene(), {x=self:ConfigOtherHuashengCoordinateX(), y=self:ConfigOtherHuashengCoordinateY()})
	end
end

function FightData:DoFightEliteWaitFunc()
	if self.fight_elite_wait_func then
		Invoke(self.fight_elite_wait_func, 2)
		self.fight_elite_wait_func = nil
	end
end

function FightData:ConfigOtherHuashengScene()
	return Config.scene_config_auto.other[1].huasheng_scene
end

function FightData:ConfigOtherHuashengCoordinateX()
	return Config.scene_config_auto.other[1].coordinate_x
end

function FightData:ConfigOtherHuashengCoordinateY()
	return Config.scene_config_auto.other[1].coordinate_y
end