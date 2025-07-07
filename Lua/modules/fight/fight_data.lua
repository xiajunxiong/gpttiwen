
FightData = FightData or BaseClass()

FightData.OPEN_AUTO_SEAL_INFO_TYPE = {
	ACCOMPLISH = 0, --已完成
	INTERRUPT = 1,  --中断
	LOOK = 2,       --查看
}
--挂机上限公式参数
FightData.GUA_JI_LIMIT_PARAM=8640
function FightData:__init()
	if FightData.Instance ~= nil then
		Debuger.LogError("[FightData] attempt to create singleton twice!")
		return
	end
	FightData.Instance = self

	self:FightConfig()
	self:FightData()
	--自动封印数据
	self.auto_seal_pet_data = SmartData.New({})
	self.auto_seal_flag = {}
	self.sc_auto_seal_info = SmartData.New({info = {}})    --服务端下发的封印成果信息
	self.is_guide_seal = false -- 是否是通过指引打开的封印设置界面
	self.today_add_guaji_exp=SmartData.New({exp=0}) --今日挂机经验
end

function FightData:CheckDungeonRedPonits()
	local role_level = RoleData.Instance:GetRoleLevel()
	local red_points = {}
	for type, dungeons in pairs(self.dungeon_list) do
		for id, info in pairs(dungeons.details) do
			if role_level >= info.level_min and role_level <= info.level_max then
				red_points[info.scene_seq] = false
			end
		end
	end
	self.dungeon_red_points:Set(red_points)
end

function FightData:FlushDungeonRedPonits(scene_seq)
	TimeHelper:AddDelayTimer(function ()
		if self.dungeon_red_points[scene_seq] then self.dungeon_red_points[scene_seq] = false end
	end, 1)
end

function FightData:TypeItemRedPoint(type)
	for scene_seq, v in pairs(self.dungeon_red_points) do
		if v then
			if type == self.dungeon_seq_list[scene_seq].scene_type_1 then return true end
		end
	end
	return false
end

function FightData:FightConfig()
	self.config = Config.scene_config_auto
	self.config_exp_scene = self.config.exp_scene
	self.config_fb1_scene = self.config.fb1_scene
	self.config_stone_scene = self.config.stone_scene

	self.crystal_config = Config.crystal_scene_auto
	self.crystal_config_crystal_config = self.crystal_config.crystal_config
	self.crystal_config_box_key = self.crystal_config.box_key
	self.crystal_config_scene_group = self.crystal_config.scene_group
	self.crystal_config_other = self.crystal_config.other[1]
	self.crystal_config_monster_dorp_list = self.crystal_config.monster_dorp_list
	self.crystal_config_exp = self.crystal_config.exp
	self.crystal_config_buy_power = self.crystal_config.buy_power

	self.collect_config = Config.collect_scene_auto
	self.collect_config_gather_scene = self.collect_config.gather_scene
	self.collect_config_other = self.collect_config.other[1]

	self.fantasy_config = Config.monster_dreamland_auto
	self.fantasy_config_cave_content = self.fantasy_config.cave_content
	self.fantasy_config_reward = self.fantasy_config.reward
	self.fantasy_config_boss_reward = self.fantasy_config.boss_reward
	self.fantasy_config_back = self.fantasy_config.reward_find
	self.fantasy_config_back_group = self.fantasy_config.reward_find_group

	self.pet_god_config = Config.pet_god_scene_auto
	self.pet_god_config_crystal = self.pet_god_config.crystal_config
	self.pet_god_config_monster_att_group = self.pet_god_config.monster_att_group
	self.pet_god_config_exp = self.pet_god_config.exp
	self.pet_god_config_buy_power = self.pet_god_config.buy_power
	self.pet_god_config_main_drop_show = self.pet_god_config.main_drop_show
	self.pet_god_config_other = self.pet_god_config.other[1]
	self.pet_god_config_rate_show = self.pet_god_config.rate_show

	self.patrol_task_config = Config.sundries_auto.patrol_task[1]

	self.guild_fb_config = Config.function_guide_auto.scene_other[1]

	self.advancements_config = KeyList(Config.career_advancements_scene_auto.career,"scene_id")

	Cfg.Register("CrystalSceneNameBySeq",Cfg.RealTimeSearch(self.config_fb1_scene,"scene_seq"))
	Cfg.Register("CrystalConfigBySeq",Cfg.RealTimeSearch(self.crystal_config_crystal_config,"seq"))
	Cfg.Register("CrystalMonsterDorpListBySeq",Cfg.RealTimeSearch(self.crystal_config_monster_dorp_list,"box_num_group"))
	Cfg.Register("CrystalBuyPowerByBuyTimes",Cfg.RealTimeSearch(self.crystal_config_buy_power,"buy_times"))
	Cfg.Register("CrystalSceneNameByTeamTarget",Cfg.RealTimeSearch(self.config_fb1_scene,"team_target"))

	self.special_reward = string.split(self.crystal_config_other.special_reward_1, ":")
    self.special_reward2 = string.split(self.crystal_config_other.special_reward_2, ":")

	self.advancefb_task = SmartData.New()
end

function FightData:FightData()
	self:FightDataField()
	self:FightDataDungeon()
	self:FightDataGather()
	self:FightDataSkill()
	self:FightDataCrystal()
	self:FightDataCollect()
	self:FightDataGuild()
	self:FightDataFantasy()
	self:FightDataSplaceAnecdote()
	self:FightDataPetIsland()
	self:FightDataSoul()

	self:FightFBData()
	
	self.cur_sel_types = SmartData.New({
		field_type = 1,
		dungeon_type = 1,
		gather_type = 1,
	})

	self.fight_shop_info = SmartData.New()
	self.cur_sel_shop_item = SmartData.New()

	self.exp_crystal_info = SmartData.New{
		is_open = false,
		count = 0,
	}
	self.patrol_task_info = SmartData.New{
		today_flag = 0,
        today_reward = 0,
        task_progress = 0,
        task_status = 0,
	}
	self.dungeon_red_points = SmartData.New()

	self.cur_sel_tabbar = SmartData.New{ val = 0 }
	self.energy_effect = SmartData.New{ val = false }
end

function FightData:Clear()
	self:ClearFightFBData()
	self:ClearFightDataCrystal()
	self:ResetPetMonopolyData()
	self:ClearSoulData()
end

function FightData:SetRoleCompleteData()
	self:ResetFightRemindData()
end

function FightData:FightDataField()
	self.field_list = self.FightListCreate(self.config_exp_scene, FightConfig.FightTabType.field)
	self.cur_sel_field = SmartData.New()
	self.meet_monster_flag = SmartData.New{
		flag = 0
	}
end
function FightData:AutoToGuaJi(setting)
	--先设置目标
	-- config_exp_scene
	local role_level = RoleData.Instance:GetRoleLevel()
	local item = nil
	for _, v in pairs(self.config_exp_scene) do
		if 0 ~= v.is_show_always then
			if v.scene_type_1 == 2 and role_level >= v.level_min and role_level <= v.level_max then
				item = v
				break
			end
		end
	end
	if item ~= nil then
		if setting then
			TeamCtrl.Instance:SendChangeTeamLimit(item.level_min, item.level_max, 1)
		else
			BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(item.began_to_fight, true))
		end
	end
end

function FightData:GetFieldTypeList()
	return self.field_list
end

function FightData:GetFieldListWithType(type)
	return FightData.SortTypeList(self.field_list[type or self.cur_sel_types.field_type].details)
end

function FightData:SetMeetMonsterFlag(flag)
	self.meet_monster_flag.flag = flag
end

function FightData:GetMeetMonsterFlag()
	return self.meet_monster_flag.flag
end

function FightData:FightDataDungeon()
	self.dungeon_seq_list = {}
	self.dungeon_consume_items = {}
	self.dungeon_list = self.FightListCreate(self.config_fb1_scene, FightConfig.FightTabType.dungeon, {dungeon_seq_list = self.dungeon_seq_list, fight_data = self})
	self.cur_sel_dungeon = SmartData.New()

	self.dungeon_rare_is_list = {}
	for _, v in pairs(self.crystal_config_crystal_config) do
		if 3 == v.scene_type_1 then
			self.dungeon_rare_is_list[v.fb_index] = v.seq
		end
	end
end

function FightData:GetDungeonTypeList()
	return self.dungeon_list
end

function FightData:GetDungeonListWithType(type)
	return FightData.SortTypeList(self.dungeon_list[type or self.cur_sel_types.dungeon_type].details)
end

function FightData:FightDataGather()
	self.gather_list = self.FightListCreate(self.config_stone_scene, FightConfig.FightTabType.gather)
	self.cur_sel_gather = SmartData.New()
	self.gather_energy = SmartData.New{
		val = 0,
		total = self.collect_config_other.power_max,
		per = self.collect_config_other.power_expend
	}

	self.gather_scene_list = {}
	for _, v in pairs(self.collect_config_gather_scene) do
		self.gather_scene_list[v.scene_id] = v
	end
end

function FightData:GetGatherTypeList()
	return self.gather_list
end

function FightData:GetGatherListWithType(type)
	return FightData.SortTypeList(self.gather_list[type or self.cur_sel_types.gather_type].details)
end

function FightData:SetGatherSelectType(type)
	self.cur_sel_types.gather_type = type
end

function FightData:GetGatherFindType(item_id)
	for k,v in pairs(self.config_stone_scene) do
		if string.find(tostring(v.reward_view), tostring(item_id)) ~= nil then
			return v
		end
	end
end

function FightData:GetStoneSceneDataBySeq(seq)
	for k,v in pairs(self.config_stone_scene) do
		if v.scene_seq == seq then
			return v
		end
	end
end

function FightData:GetItemWithType(list, type)
	self.cur_sel_type = nil
	for _, v in pairs(list) do
		if v.seq == type then
			return v
		end
	end
	return list[next(list)]
end

function FightData.SortTypeList(t)
	local scene_id = SceneData.Instance:SceneId()
	local role_level = RoleData.Instance:GetRoleLevel()
	local list = {}
	for _, v in pairs(t) do
		if 0 ~= v.is_show_always or scene_id == v.scene_id then
			v.value_cur = scene_id == v.scene_id
			v.value_reco = role_level >= v.level_min and role_level <= v.level_max and (not v.value_cur)
			table.insert(list, v)
		end
	end
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local ac = a.value_cur and 1 or 0
		local bc = b.value_cur and 1 or 0
		if ac == bc then
			local ar = a.value_reco and 1 or 0
			local br = b.value_reco and 1 or 0
			if ar == br then
				local ai = a.index or a.scene_seq
				local bi = b.index or b.scene_seq
				return ai < bi
			end
			return ar > br
		end
		return ac > bc
	end)
	return list
end

function FightData.FightListCreate(config, type, params)
	local t = {}
	for _, v in pairs(config) do
		t[v.scene_type_1] = t[v.scene_type_1] or FightData.CreateType(type, v.scene_type_1)
		v._scene = SceneData.Instance:GetSceneCfg(v.scene_id)
		v.map_id = MapData.MapIdToIcon(v.scene_id)
		local rewards = {}
		local rewards_rare = {}
		local unique_formula = {}
		local rewards_up = {}
		local rewards_rate = {}
		if v.reward_view_xy then
			for y, z in pairs(string.split(v.reward_view_xy, ":")) do
				rewards_rare[tonumber(z)] = "_LocXiYou"
			end
		end
		if v.reward_view_zc then
			for y, z in pairs(string.split(v.reward_view_zc, ":")) do
				rewards_rare[tonumber(z)] = "_LocZhenCang"
			end
		end
		if v.reward_view_ss then
			for y, z in pairs(string.split(v.reward_view_ss, ":")) do
				rewards_rare[tonumber(z)] = "_LocShiShi"
			end
		end
		if v.reward_view_wy then
			for y, z in pairs(string.split(v.reward_view_wy, ":")) do
				unique_formula[tonumber(z)] = true
			end
		end
		if v.reward_view_up then
			for y, z in pairs(string.split(v.reward_view_up, ":")) do
				rewards_up[tonumber(z)] = true
			end
		end
		if not StringIsEmpty(v.reward_view_rate) then
			for y, z in pairs(string.split(v.reward_view_rate, ":")) do
				local str_rate = string.split(z, "|")
				rewards_rate[tonumber(str_rate[1])] = tonumber(str_rate[2])
			end
		end
		v.rewards_rare = rewards_rare
		v.unique_formula = unique_formula
		v.rewards_up = rewards_up
		v.rewards_rate = rewards_rate
		local rate = {}
		if not StringIsEmpty(v.rate_view) then
			rate = string.split(v.rate_view, ":")
		end
		for y, z in pairs(string.split(v.reward_view, ":")) do
			local item_id = tonumber(z)
			local item_data = Item.New()
			item_data.old_id = Cfg.RewardJobBagHasId(item_id, false) and item_id or nil
			item_data.item_id = item_id
			item_data.num = 1
			item_data.is_bind = 0
			item_data.is_rare_dw = rewards_rare[item_id] or false
			item_data.is_unique_formula = unique_formula[item_id] or false
			item_data.is_up_dw = rewards_up[item_id] or false
			item_data.is_rate_dw = rewards_rate[item_id] or false
			item_data.rate = rate[y]
			item_data.tradable = item_id == CommonItemId.XingChenShi or item_id == CommonItemId.ShanHaiHuiJuan
			if nil == item_data.old_id then
				local cfg = Item.GetConfig(item_id)
				item_data.tradable = item_data.tradable or (cfg and cfg.show_type == ShowType.IllustrateCard)
			end
			table.insert(rewards, item_data)
		end
		if not StringIsEmpty(v.reward_button) then
			v.rewards_button = {}
			for y, z in pairs(string.split(v.reward_button, ":")) do
				local item_id = tonumber(z)
				local item_data = Item.New()
				item_data.old_id = Cfg.RewardJobBagHasId(item_id, false) and item_id or nil
				item_data.item_id = item_id
				item_data.num = 1
				item_data.is_bind = 0
				item_data.is_rare_dw = rewards_rare[item_id] or false
				item_data.is_unique_formula = unique_formula[item_id] or false
				item_data.is_up_dw = rewards_up[item_id] or false
				item_data.is_rate_dw = rewards_rate[item_id] or false
				table.insert(v.rewards_button, item_data)
			end
		end
		v.monsters = {}
		local m_t = string.split(v.monter_id, "|")
		for i = 1, #m_t do
			local monster_id = tonumber(m_t[i])
			local config_m = Cfg.MonsterById(monster_id, false)
			if config_m then
				local icon_id = config_m.icon_id
				if StringIsEmpty(icon_id) then Debuger.LogError(string.format("打怪 monster:%s 没有怪物图标!", monster_id)) end
				table.insert(v.monsters, {
					monster_id = monster_id,
					is_boss = 1 == i,
					tab_type = type,
					icon_id = icon_id,
					name = config_m.name,
				})
			else
				Debuger.LogError(string.format("打怪 monster:%s 没有配置!", monster_id))
			end
		end
		if v.expend_item then
			local item_data = Item.New()
			item_data.item_id = tonumber(v.expend_item)
			item_data.num = 1
			item_data.is_bind = 0
			v.consume = item_data
		end
		local beh_id = v.began_to_fight
		if beh_id > 0 then
			v.click_handle = function ()
				BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(beh_id, true))
				ViewMgr:CloseView(FightView)
				ViewMgr:CloseView(MapView)
				-- ViewMgr:CloseView(FightFBView)
			end
		end
		if type == FightConfig.FightTabType.dungeon and v.expend_item then
			params.fight_data.dungeon_consume_items[v.expend_item] = v.click_handle
		end

		if FightConfig.FightTabType.dungeon == type then
			params.dungeon_seq_list[v.scene_seq] = v
			v.reward_list = rewards
			if 1 == v.scene_type_1 then
				v.rewards = rewards
			else
				v.rewards = {}
				local per_page_num = 16
				for index, item in ipairs(rewards) do
					local page = math.floor((index - 1) / per_page_num) + 1
					v.rewards[page] = v.rewards[page] or {}
					table.insert(v.rewards[page], item)
				end
			end
			local crystal_co = Cfg.CrystalConfigBySeq(v.scene_seq)
			v.exp_num = 0
			v.item_s1_num = 0
			if crystal_co and 1 == v.scene_type_1 then
				local drop_co = Cfg.CrystalMonsterDorpListBySeq(crystal_co.boss_id)
				if drop_co then
					v.exp_num = drop_co.exp
					-- v.item_s1_num = drop_co.reward[0].num
					v.item_s1_num = 0
				end
			end
		else
			v.rewards = rewards
		end

		table.insert(t[v.scene_type_1].details, v)
	end
	return t
end

function FightData.CreateType(tab_type, scene_type)
	return {
		type = scene_type,
		name = Language.Fight.TypeNames[tab_type][scene_type],
		details = {}
	}
end

function FightData:SetBattleAutoInfo(protocol)
	self.skill_protocol = protocol
	local detail1 = protocol.first_skill_id > 0 and SkillData.GetDecConfig(protocol.first_skill_id, protocol.first_skill_level) or self.base_skills[protocol.first_move_type]
	local detail2 = protocol.second_skill_id > 0 and SkillData.GetDecConfig(protocol.second_skill_id, protocol.second_skill_level) or self.base_skills[protocol.second_move_type]
	if nil == detail1 then LogError(string.format("msg 2007 error first_skill_id:%s first_move_type:%s", protocol.first_skill_id, protocol.first_move_type)) end
	if nil == detail2 then LogError(string.format("msg 2007 error second_skill_id:%s second_move_type:%s", protocol.second_skill_id, protocol.second_move_type)) end
	self.cur_sel_skills[1].detail:Set(detail1 or {})
	self.cur_sel_skills[2].detail:Set(detail2 or {})
	if ViewMgr:IsOpen(FormationView) then
		FormationData.Instance:SetFormation0AutoInfo(protocol)
	end
end

function FightData:SetCrystalFbInfo(protocol)
	for i = 0, FightConfig.CRYSTAL_FB_XI_YOU_MAX_LEVEL - 1 do
		if nil ~= self.dungeon_rare_is_list[i] then
        	self.dungeon_seq_list[self.dungeon_rare_is_list[i]].use_time = protocol.xi_you_fb_play_times[i]
		end
	end
	self.fight_fb_ymz.normal_fb_record_bt_list = protocol.normal_record_bt_list
	self.fight_fb_ymz.normal_fb_record_list:Set(protocol.normal_record_list)
end

function FightData:FightDataSkill()
	self.skill_data = SkillData.Instance
	self.cur_sel_skills = SmartData.New({SmartData.New({auto_seq = 1, detail = SmartData.New()}), SmartData.New({auto_seq = 2, detail = SmartData.New()})})

	self.base_skills = {
		[BATTLE_MOVE_TYPE.NORMAL_ATTACK] = {skill_id = BATTLE_MOVE_TYPE.NORMAL_ATTACK, icon_id=1, skill_name = Language.Main.Battle.Attack},
		[BATTLE_MOVE_TYPE.NORMAL_DEFENCE] = {skill_id = BATTLE_MOVE_TYPE.NORMAL_DEFENCE, icon_id=31, skill_name = Language.Main.Battle.Defence},
		[BATTLE_MOVE_TYPE.AI_SKILL] = {skill_id = BATTLE_MOVE_TYPE.AI_SKILL, icon_id=81, skill_name = Language.Main.Battle.AISkill},
		-- [BATTLE_MOVE_TYPE.CHANGE_POSITION] = {skill_id = BATTLE_MOVE_TYPE.CHANGE_POSITION, icon_id=41, skill_name = Language.Main.Battle.Position},
		-- [BATTLE_MOVE_TYPE.RUN_AWAY] = {skill_id = BATTLE_MOVE_TYPE.RUN_AWAY, icon_id=51, skill_name = Language.Main.Battle.Escape},
	}
end

function FightData:GetRoleSkillList()
	local skill_list = {}
	for k, v in pairs(self.skill_data.smart_data) do
		table.insert(skill_list, SkillData.GetDecConfig(v.skill_id,v.skill_level))
	end
	-- for k, v in pairs(self.base_skills) do
	-- 	v.is_base = true
	-- 	table.insert(skill_list, v)
	-- end
	self.SortSkill(skill_list)
	return {skills = skill_list, type = 1}
end

function FightData:GetBaseSkillList(is_pet)
	local skill_list = {}
	for k, v in pairs(self.base_skills) do
		-- if BATTLE_MOVE_TYPE.CHANGE_POSITION ~= k and BATTLE_MOVE_TYPE.RUN_AWAY ~= k then
			v.is_base = true
			-- 宠物不能智能施法
			if not is_pet or BATTLE_MOVE_TYPE.AI_SKILL ~= v.skill_id then
				table.insert(skill_list, v)
			end
		-- end
	end
	self.SortSkill(skill_list)
	return {skills = skill_list, type = 2}
end

function FightData:GetPetSkillList()
	local skill_list = {}
	local fight_index = PetData.Instance:GetFightIndex()
	local fight_pet = PetData.Instance:GetPetInfo(fight_index)
	if nil ~= fight_pet then
		local pet_skill_data = fight_pet:HadSkillList()
		for k,v in pairs(pet_skill_data) do
			v.name = v.skill_cfg.skill_name
			local skill_type = SkillData.GetSkillType(v.skill_id)
			-- 筛除宠物被动技能
			if 7 ~= skill_type and 8 ~= skill_type then
				table.insert(skill_list,v)
			end
		end
		-- for k, v in pairs(self.base_skills) do
		-- 	-- if BATTLE_MOVE_TYPE.CHANGE_POSITION ~= k and BATTLE_MOVE_TYPE.RUN_AWAY ~= k then
		-- 		v.is_base = true
		-- 		table.insert(skill_list, v)
		-- 	-- end
		-- end
		self.SortSkill(skill_list)
	end
	return {skills = skill_list, type = 3}
end

function FightData.SortSkill(list)
	table.sort(list, function(a, b)
		if not a or not b then return a end
        local as = a.is_base and 1 or 0
		local bs = b.is_base and 1 or 0
		if as == bs then return a.skill_id < b.skill_id end
        return as < bs
	end)
end

function FightData:GetSkillList()
	local show_pet_icon = PetData.Instance:GetFightIndex() > -1
	if show_pet_icon then
		if 1 == self.cur_auto_seq then
			return {self:GetRoleSkillList(), self:GetBaseSkillList()}
		else
			return {self:GetPetSkillList(), self:GetBaseSkillList(true)}
		end
	else
		if self.skill_protocol.first_skill_id > 0 then
			if 1 == self.cur_auto_seq then
				return {self:GetRoleSkillList(), self:GetBaseSkillList()}
			else
				return {self:GetBaseSkillList()}
			end
		elseif self.skill_protocol.second_skill_id > 0 then
			if 2 == self.cur_auto_seq then
				return {self:GetRoleSkillList(), self:GetBaseSkillList()}
			else
				return {self:GetBaseSkillList()}
			end
		else
			return {self:GetRoleSkillList(), self:GetBaseSkillList()}
		end
	end
end

-- function FightData:GetSkillList()
-- 	local show_pet_icon = PetData.Instance:GetFightIndex() > -1
-- 	if show_pet_icon then
-- 		if 1 == self.cur_auto_seq then
-- 			return self:GetRoleSkillList()
-- 		else
-- 			return self:GetPetSkillList()
-- 		end
-- 	else
-- 		if self.skill_protocol.first_skill_id > 0 then
-- 			return 1 == self.cur_auto_seq and self:GetRoleSkillList() or self:GetBaseSkillList()
-- 		elseif self.skill_protocol.second_skill_id > 0 then
-- 			return 2 == self.cur_auto_seq and self:GetRoleSkillList() or self:GetBaseSkillList()
-- 		else
-- 			return self:GetRoleSkillList()
-- 		end
-- 	end
-- end

function FightData:SetGatherFbInfo(protocol)
	self.gather_energy.val = protocol.gather_power
	LiveData.Instance:SetGatherInfo(protocol)
end

function FightData:GetGatherRewards()
	local t = {}
	local items = string.split(self.gather_scene_list[SceneData.Instance:SceneId()].show_item, "|")
	for _, v in pairs(items) do
        local item_data = Item.New()
        item_data.item_id = v
        item_data.num = 1
        item_data.is_bind = 0
        table.insert(t, item_data)
	end
	return t
end

function FightData:GetDungeonBoxKeyByBoxId(box)
	local key = 0
	for k, v in pairs(self.crystal_config_box_key) do
		if v.box == box then
			key = v.key
		end
	end
	return key
end

function FightData:SetNPCShopInfo(protocol)
	local item_list = {}
	for i = 1, protocol.count do
		if protocol.item_list[i].shop_info.buy_num > 0 then
			local item = Item.New(protocol.item_list[i])
			item.shop_info = protocol.item_list[i].shop_info
			item.index = #item_list + 1
			table.insert(item_list, item)
		end
	end
	if self.cur_sel_shop_item and (self.cur_sel_shop_item.index or 0) > #item_list then self.cur_sel_shop_item:Set(item_list[1] or {}) end
	self.fight_shop_info:Set(item_list)
end

function FightData:GetNPCShopInfo()
	return self.fight_shop_info:Val()
end

function FightData:SetExpCrystalInfo(protocol)
	self.exp_crystal_info.is_open = 1 == protocol.is_open
	self.exp_crystal_info.count = protocol.count
end

function FightData:PatrolTaskTotal()
	return self.patrol_task_config.kill_num
end

function FightData:SetPatrolTaskInfo(protocol)
	self.patrol_task_info:Set(protocol.info)
end

--进阶副本
function FightData:GetAdvancementsSceneData(scene_id)
	return self.advancements_config[scene_id]
end

function FightData:SetAdvanceFbObjInfo(protocol)
	self.advance_obj_info = protocol.obj_list
end

function FightData:SetAdvanceFbTaskInfo(protocol)
	self.advancefb_task:Set(protocol.task_info)
end

function FightData:GetAdvanceFbTaskInfo()
	return self.advancefb_task or {}
end

function FightData:GetAdvanceFbObjInfo()
	return self.advance_obj_info or {}
end

function FightData:IsCanOpenMapAdvancedFB()
    local scene_id = SceneData.Instance:SceneId()
	local config = self:GetAdvancementsSceneData(scene_id) or {}
	return config.is_map and config.is_map == 1 or false
end

function FightData:GetGatherEnergy()
	return math.floor(self.gather_energy.val / self.gather_energy.per)
end

function FightData.IsDropDouble(seq)
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DROP_DOUBLE) then
		return false
	end
	local config = Config.drop_double_auto.burst_rate_double
	for k,v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DROP_DOUBLE,config)) do
		if v.fb_id == seq then
			return true
		end
	end
	return false
end

function FightData:GetAdvancementsData(adv,level)
	for k,v in pairs(Config.career_advancements_scene_auto.career) do
		if v.adv == adv and v.level == level then
			return v
		end
	end
end

function FightData:GetFightBoardSingleTitle2()
	local title2 = ""
	local beh_cfg = FightData.Instance:FightSingleBeh()
	if FB_TYPE.CRYSTAL_FB == beh_cfg.param1 then
		local co = Cfg.CrystalSceneNameBySeq(beh_cfg.param2)
		if co then title2 = string.format("(%s)", co.scene_name) end
	end
	return title2
end

function FightData:GetFightBoardTeamTitle()
	local title = Language.Fight.FightTeam
	local enter_fb_info = TeamData.Instance.enter_fb_info
	if 1 == enter_fb_info.enter_type then
		if FB_TYPE.CRYSTAL_FB == enter_fb_info.enter_type_param then
			local co = Cfg.CrystalSceneNameBySeq(enter_fb_info.param1)
			if co and 3 == co.scene_type_1 then title = Language.Fight.FightTeamTitles.RareDungeon1 end
		elseif FB_TYPE.MIYI == enter_fb_info.enter_type_param then
			title = Language.Fight.FightTeamTitles.MiYi1
		elseif FB_TYPE.RUSH_DUNGEON == enter_fb_info.enter_type_param then
			title = Language.Fight.FightTeamTitles.RushDungeon1
		elseif enter_fb_info.enter_type_param == FB_TYPE.GO_THROUGH then
			title = Language.GoThrough.EnterTeamTitle
		elseif FB_TYPE.SHI_TU_CHUANG_GONG == enter_fb_info.enter_type_param then
			title = Language.Fight.FightTeamTitles.ShiTuChuanGong1
		elseif FB_TYPE.LOVERS_MATCH == enter_fb_info.enter_type_param then
			title = Language.LoversMatch.FBName
		elseif FB_TYPE.RABBIT_RACE == enter_fb_info.enter_type_param then
			title = Language.RabbitRun.Title
		elseif FB_TYPE.HARMONY_HEAVEN == enter_fb_info.enter_type_param then
			title = Language.Fight.FightTeamHarmonyHeaven
		end
	elseif  2 == enter_fb_info.enter_type then
		title = Language.Fight.FightTeamTitles.ChallengeTask1
    elseif 3 == enter_fb_info.enter_type then
        if BATTLE_MODE.BATTLE_MODE_TEAM_CHALLENGE_TASK == enter_fb_info.enter_type_param then
            title = Language.Fight.FightTeamTitles.Team_Challenge
        end
	end
	return title
end

function FightData:GetFightBoardTeamTitle2()
	local title2 = ""
	local enter_fb_info = TeamData.Instance.enter_fb_info
	if 1 == enter_fb_info.enter_type then		-- 副本
		if FB_TYPE.CRYSTAL_FB == enter_fb_info.enter_type_param then
			local co = Cfg.CrystalSceneNameBySeq(enter_fb_info.param1)
			
			if co then
				if 3 == co.scene_type_1 then
					local monster = co.monsters and co.monsters[1] or nil
					if monster then
						title2 = string.format("(%s)", monster.name)
					end
				else
					title2 = string.format("(%s)", co.scene_name)
				end
			end
		elseif FB_TYPE.MIYI == enter_fb_info.enter_type_param then
			local co = SceneData.Instance:GetSceneCfg(enter_fb_info.param1)
			title2 = string.format(Language.Fight.FightTeamTitles.MiYi2, co and co.name or "")
		end
	-- elseif 3 == enter_fb_info.enter_type then
	-- 	local co = Config.challengetask_list_auto.task_list[enter_fb_info.param2]
	-- 	title2 = string.format(Language.Fight.FightTeamTitles.ChallengeTask2, co and co.task_name or "")
	end
	return title2
end

function FightData:CheckAdvancedFBSceneName(scene_name,scene_id,param1)
	if SceneModeType.AdvancedFB == SceneData.Instance:GetMode() then
		if SceneData.Instance:SceneId() == scene_id then
			local cfg = SceneData.Instance:GetSceneCfg(param1)
			return cfg and cfg.name or Language.Fight.Exit
		end
	end
	return scene_name
end

function FightData:GetButtonRewardsBySceneSeq(scene_seq)
	return self.dungeon_seq_list[scene_seq].rewards_button or {}
end

function FightData:GetDungeonInfoBySceneSeq(scene_seq)
	return self.dungeon_seq_list[scene_seq]
end

--设置封印的宠物数据
function FightData:SetAutoSealData(data)
	self.auto_seal_pet_data:Set(data)
end
--获取封印的宠物数据
function FightData:GetAutoSealData()
	return self.auto_seal_pet_data
end

--设置封印的宠物数量数据
function FightData:SetAutoSealPet(pet_id,num)
	if self.auto_seal_pet_data.pet_nums == nil then
		self.auto_seal_pet_data.pet_nums = {}
	end
	self.auto_seal_pet_data.pet_nums[pet_id] = num
end

--获取自动封印最多可封印多少个宠物
function FightData:GetAutoSealPetMaxNum()
	local pet_bag_num = PetData.Instance:GetBagRemainNum()
	local num = self:GetSumSealPetNum()
    return pet_bag_num - num
end

--获取总共选择封印宠物的数量
function FightData:GetSumSealPetNum()
	local num = 0
	for i,v in pairs(self.auto_seal_pet_data.pet_nums or {}) do
    	num = num + v
    end
    return num
end

--设置自动抓宠标记
function FightData:SetAutoSealFlag(flag)
	self.auto_seal_flag.flag = flag or false
end

function FightData:GetAutoSealFlag()
	return self.auto_seal_flag.flag or false
end

--设置自动抓宠的角色id -> 用来判定那个角色设置的，例如切换账号的时候
function FightData:SetAutoSealRoleId(role_id)
	self.auto_seal_flag.role_id = role_id
end

function FightData:GetAutoSealRoleId()
	return self.auto_seal_flag.role_id or 0
end

--设置自动封印的成果信息
function FightData:SetAutoSealInfo(protocol)
	self.sc_auto_seal_info.info = protocol.info
end

--每日挂机经验
function FightData:OnSCGuaJiExpInfo(protocol)
	self.today_add_guaji_exp.exp=protocol.today_add_guaji_exp
end
--获取自动封印的成果信息
function FightData:GetAutoSealInfo()
	return self.sc_auto_seal_info.info
end

--检查是否达到自动封印的要求
function FightData:CheckAccomplishAutoSeal()
	local info = self:GetAutoSealInfo()
	if next(info) == nil then
		return false
	end
	if info.auto_catch_info_num <=0 then
		return false
	end
	if not self:CheckCanSeal() then
		return true
	end

	if info.max_seal_card_common == info.cur_seal_card_common and info.max_seal_card_silver == info.cur_seal_card_silver and info.max_seal_card_gold == info.cur_seal_card_gold then
		return true
	end
	for i=1,info.auto_catch_info_num do
		if info.info_list[i].max_catch_num ~= info.info_list[i].cur_catch_num then
			return false
		end
	end
	return true
end

--检查当前封印卡是否可继续封印 （如我自动封印设置中只剩下金宠，而我只有银卡了，此时停止封印）
function FightData:CheckCanSeal()
	local info = self:GetAutoSealInfo()
	local pet_min_qua = math.huge
	local card_max_qua = 0
	for i,v in pairs(info.info_list) do
		if v.max_catch_num > v.cur_catch_num then
			local monster_cfg = Cfg.MonsterById(v.monster_id)
			local pet_cfg = PetData.Instance:GetPetCfg(monster_cfg.catch_pet_id)
			if pet_cfg.quality < pet_min_qua then
				pet_min_qua = pet_cfg.quality
			end
		end
	end
	for i,v in pairs(info.card_info_list) do
		if v.max_seal_num > v.cur_seal_num then
			if i > card_max_qua then
				card_max_qua = i
			end
		end
	end
	if pet_min_qua <= card_max_qua then
		return true
	else
		return false
	end
end

--获取场景可封印宠物 默认为挂机页面选中的
function FightData:GetCurSceneCanSealPets(scene_id)
	scene_id = scene_id or self.cur_sel_field.scene_id or self:GetFieldListWithType(2)[1].scene_id
	local boss_list = {}
    local scene_config = MapData.Instance:GetMapShowData(scene_id)
    if scene_config ~= nil then
        boss_list = MapData.Instance:GetMapMiniShowList(scene_config)
    end
    for i,v in ipairs(boss_list) do
    	if v.key == "is_pet" then
    		return v.show_list
    	end
    end
    return {}
end

--获取场景中可抓宠物的最低品质
function FightData:GetCurSceneCanSealPetsMinQua()
	local pets = self:GetCurSceneCanSealPets()
	local min_qua =  math.huge
	for _,v in pairs(pets) do
		local pet_cfg = PetData.Instance:GetPetCfg(tonumber(v))
		if pet_cfg.quality < min_qua then
			min_qua = pet_cfg.quality
		end
	end
	return min_qua
end

--通过宠物id获取怪物id 仅适合Fight页面使用
function FightData:GetMonstersIdByPetId(pet_id)
	if self.cur_sel_field.monsters == nil then
		return
	end
	local pet_cfg = PetData.Instance:GetPetCfg(pet_id) or {}
	local icon_id = pet_cfg.icon_id or pet_id
	for _,v in pairs(self.cur_sel_field.monsters) do
		if icon_id == v.icon_id then
			return v.monster_id
		end
	end
end

--通过scene_id获取巡逻场景的data
function FightData:GetFightSceneDataById(scene_id)
 	local list = self.field_list[2].details
 	for _, v in pairs(list) do
		if scene_id == v.scene_id then
		    return v
		end
	end 
end

--是否设置了自动抓宠信息
function FightData:IsSetAutoSealPet()
	local seal_info = self:GetAutoSealInfo() or {}
	if seal_info.scene_id ~= nil and seal_info.scene_id ~= 0 then
		return true
	else
		return false
	end
end

--获取自动封印的目标场景ID
function FightData:GetSealSceneId()
	return self:GetAutoSealInfo().scene_id
end

function FightData:SetSealing(flag)
	self.is_sealing = flag or false
end

-- 是否正在封印
function FightData:IsSealing()
    return self.is_sealing or false
end

function FightData:GetFightRewardData(battle_mode)
	local state = MainViewData.Instance:GetGameState()
	if battle_mode == BATTLE_MODE.BATTLE_MODE_WORLD_BOSS then
		return FightFieldBossData.Instance:GetFieldBossRewardData(1)
	elseif battle_mode == BATTLE_MODE.BATTLE_MODE_SHXY then
		return FightFieldBossData.Instance:GetFieldBossRewardData(2)
	elseif battle_mode == BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND then
		return RushDungeonData.Instance:GetPreviewReward()
	elseif SceneData.Instance:GetMode() == SCENE_TYPE.BRAVE_GROUND or state == MainViewGameState.rush_dungeon then
		return RushDungeonData.Instance:GetPreviewReward()
	else
		local fight_crystal_info = self:GetFightCrystalInfo()
    	return self:GetFightCrystalRewards(fight_crystal_info.level_seq)
	end
end

-- 获取秘境副本名字
function FightData:GetFBName(scene_seq)
	for i,v in pairs(self.config.fb1_scene) do
		if scene_seq == v.scene_seq then
			return v.scene_name
		end
	end
end

function FightData:IsSR(id)
    for k,v in pairs(self.special_reward) do
        if  tonumber(v) == tonumber(id) then
            return true
        end
    end
    return false
end

function FightData:IsSSR(id)
    for k,v in pairs(self.special_reward2) do
        if  tonumber(v) == tonumber(id) then
            return true
        end
    end
    return false
end

function FightData:GetSweepLimitType()
	return self.crystal_config_other.limit_type
end

function FightData:GetSweepBuyTimes()
   	return self.crystal_config_other.buy_times
end

--获取当前秘境体力
function FightData:GetFightStrengthNum()
	return self.fight_crystal_sweep.role_enter_energy,self:GetCrystalFbSweepMaxPower()
end

--获取当前秘境总体力
function FightData:GetCrystalFbSweepMaxPower()
	local config = DataHelper.GetSearch(self.crystal_config.power_up,[[job_id]],RoleData.Instance:ProfStage())
	return config and config.power_up or self.crystal_config_other.max_power
end

function FightData.FightOtherDate()
	return Config.fight_other_auto.other[1]
end