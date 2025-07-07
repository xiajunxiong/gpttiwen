
function FightData:FightDataCrystal()
	self.fight_crystal_info = SmartData.New()
	self.fight_crystal_objs = SmartData.New()
	self.fight_crystal_sweep = SmartData.New(FightData.DefaultFightCrystalSweep())
	self.fight_crystal_sweep.has_view_opened = false 		--记录精英副本界面是否打开过用于表现红点

	self.fight_crystal_all_objs = {}

	self.fight_crystal_list = {}
	for _, v in pairs(self.crystal_config_crystal_config) do
		self.fight_crystal_list[v.seq] = v
	end
	
	self.fight_crystal_scenes = {}
	for _, v in pairs(self.crystal_config_crystal_config) do
		self.fight_crystal_scenes[v.scene_id] = true
	end
	for _, v in pairs(self.crystal_config_scene_group) do
		self.fight_crystal_scenes[v.scene_id] = true
	end

	self.crystal_single_beh = {}

	self.crystal_use_data = SmartData.New({info = {}})
end

function FightData:ClearFightDataCrystal()
	self.fight_crystal_sweep.has_view_opened = false
end

function FightData:FightSingleBeh(beh)
	self.crystal_single_beh = beh or self.crystal_single_beh
	if beh then self.cur_sel_dungeon:Set(self.dungeon_seq_list[self.crystal_single_beh.param2]) end
	return self.crystal_single_beh
end

function FightData:CheckCrystalSceneName(scene_name, layer, seq)
	if not self.fight_crystal_scenes[SceneData.Instance:SceneId()] then return scene_name end
	local max_layer = self.fight_crystal_list[seq] and self.fight_crystal_list[seq].layer or 1
	return Language.Fight.CrystalDungen.layer_door[layer < max_layer and (layer < max_layer - 1 and "next" or "last") or "exit"] or scene_name
end

function FightData:SetCrystalFbSceneInfo(fight_crystal_info)
	self.fight_crystal_info:Set(fight_crystal_info)
	-- self.fight_crystal_objs:Set({})
end

function FightData:GetFightCrystalInfo()
	return self.fight_crystal_info
end

function FightData:SetCrystalFbAllObjInfo(protocol)
	self.fight_crystal_all_objs = protocol.obj_list
end

function FightData:GetFightCrystalObjList()
	local obj_list = {}
	local objs = self.fight_crystal_all_objs
	for _, obj in pairs(objs) do
		if obj.sort_type then
			table.insert(obj_list, obj)
		end
	end
	table.sort(obj_list, function (a, b)
		if not a or not b then return a end
		if a.sort_type == b.sort_type then
			return a.index < b.index
		end
		return a.sort_type < b.sort_type
	end)
	
	local obj_num = #obj_list
	local is_odd_all = 1 == obj_num % 2
	for i = 1, #obj_list do
		local show_line1 = false
		local show_line2 = false
		local is_odd = 1 == i % 2
		if is_odd_all then
			show_line1 = is_odd and i ~= obj_num
			show_line2 = not is_odd
		else
			show_line1 = is_odd and i ~= (obj_num - 1)
			show_line2 = not is_odd and i ~= obj_num
		end
		obj_list[i].show_line1 = show_line1
		obj_list[i].show_line2 = show_line2
	end

	return obj_list
end

--规定先收info 包再收obj包
function FightData:SetCrystalFbObjInfo(protocol)
	self.fight_crystal_objs:Set(protocol.obj_list)
	-- local crystal_info = self:GetFightCrystalInfo()
	-- if crystal_info ~= nil and 
	-- 	crystal_info.layer == protocol.layer and 
	-- 	crystal_info.level_seq == protocol.level_seq then
	-- 	SceneModeFightCrystal.OnSceneLoaded()
	-- end
	if SceneData.Instance:GetMode() == SceneModeType.FightCrystal then
		-- LogError("LLLLLLLLLLLLLLLLLLL")
		SceneModeFightCrystal.OnSceneLoaded()
	end
end

function FightData:GetCrystalFbObjInfo()
	return self.fight_crystal_objs
end

function FightData:GetFightCrystalConfigBySeq(seq)
	return self.fight_crystal_list[seq]
end

function FightData:SetCrystalFbUseItemInfo(protocol)
	self.crystal_use_data.info = protocol.use_info
end

--限时组次数
function FightData:GetFbTimes(item_id)
	local info = self:GetFightCrystalInfo()
	if info ~= nil then
		local config = self.fight_crystal_list[info.level_seq] or {}
		local use_type = Item.GetUseType(item_id)
		local limit_times = self:GetLimitItemTimes(config.item_num or 0,use_type)
		return limit_times - self:GetUseTimes(use_type), limit_times
	end
	return 0
end

--获取限制组次数
function FightData:GetLimitItemTimes(group_id,use_type)
	for i,v in ipairs(Config.crystal_scene_auto.fb_expend_num) do
		if v.limit_item_group_id == group_id and v.use_type == use_type then
			return v.xz_num
		end
	end
	return 0
end

function FightData:GetUseTimes(use_type)
	for i,v in ipairs(self.crystal_use_data.info or {}) do
		if v.use_type == use_type then
			return v.use_times or 0
		end
	end
	return 0
end

function FightData:SetCrystalRewardData(data, exp_num)
	if nil == data then
		self.crystal_reward_data = nil
		self.crystal_reward_others = nil
		return
	end
	local rewards, others = {}, {
		item_s1_num = 0,
		exp_num = 0,
	}
	for _, vo in ipairs(data) do
		local item_id = vo.item_id
		if CommonItemId.SkyGlass == item_id then
			others.item_s1_num = vo.num
		else
			table.insert(rewards, vo)
		end
	end
	if exp_num and exp_num > 0 then
		others.exp_num = exp_num
	end
	self.crystal_reward_data = rewards
	self.crystal_reward_others = others
end
--获取副本奖励
function FightData:GetCrystalRewardData()
	local reward = self.crystal_reward_data
	self.crystal_reward_data = nil
	return reward
end

function FightData:IsCrystalReward(battle_mode)
	return battle_mode == BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB and self.crystal_reward_data == nil
end

function FightData:IsCrystalCommon()
	local fight_crystal_info = self:GetFightCrystalInfo()
	local cur_seq_info = self:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
	if cur_seq_info then
		return cur_seq_info.type ~= 1
	end
	return false
end

function FightData:AutoToNpc()
	local mode = SceneData.Instance:GetMode()
	if mode == SceneModeType.FightCrystal or mode == SceneModeType.PetGod or mode == SceneModeType.HarmonyHeaven then
		local objs_holder = SceneCtrl.Instance:Objs()
		local net_npcs = objs_holder.net_npcs
		local net_monsters = objs_holder.net_monsters
		local pos_t = MainPlayer:GetPos()
		for _, obj in pairs(net_npcs) do
			local npc_point = obj:GetPos()
			if GameMath.GetDistance(pos_t.x, pos_t.z, npc_point.x, npc_point.z, false) < 10 then
				obj:LookAt()
				obj:InteractionBehavior()
				return
			end
		end
		for _, obj in pairs(net_monsters) do
			local monster_point = obj:GetPos()
			if GameMath.GetDistance(pos_t.x, pos_t.z, monster_point.x, monster_point.z, false) < 10 then
				obj:LookAt()
				obj:InteractionBehavior()
				return
			end
		end
	end
end

function FightData:GetFightCrystalRewards(level_seq)
	local rewards = {}
	local dungeon =  self.dungeon_seq_list[level_seq]
	local prof_type = RoleData.Instance:ProfType()
	if dungeon then
		-- local per_page_num = 8
		-- for index, item in ipairs(dungeon.reward_list) do
		-- 	local page = math.floor((index - 1) / per_page_num) + 1
		-- 	rewards[page] = rewards[page] or {}
		-- 	table.insert(rewards[page], item)
		-- end
		for index, item in ipairs(dungeon.reward_list) do
			if item.old_id then
				local co = Cfg.RewardJobBagByIdType(item.old_id, prof_type)
				item.item_id = co and co.new_id or item.item_id
			end
			table.insert(rewards, item)
		end
		if dungeon.item_s1_num > 0 then table.insert(rewards, 1, Item.Init(CommonItemId.SkyGlass, dungeon.item_s1_num)) end
		if dungeon.exp_num > 0 then table.insert(rewards, 1, Item.Init(CommonItemId.Exp, dungeon.exp_num)) end
	end
	return rewards
end

function FightData:SetCrystalFbEnergyInfo(protocol)
	local info = protocol.info
	info.total_energy_reply_tamp = self:GetCrystalFbEneryTotalTimeStamp(info)
	info.new_jingying_play_times_remain = math.max(0, self:GetCrystalFbNormalRewardTime() - info.new_jingying_play_times)
	info.new_xiyou_play_times_remain = math.max(0, info.new_xiyou_play_times - info.use_xiyou_play_times)
	local flag_cache = self.fight_crystal_sweep.has_view_opened
	if flag_cache == nil then flag_cache = false end
	self.fight_crystal_sweep:Set(info)
	self.fight_crystal_sweep.has_view_opened = flag_cache
end

function FightData.DefaultFightCrystalSweep()
	local info = {
		role_enter_energy = 0,
        next_energy_reply_tamp = 0,
        first_pass_flag = {},
        first_pass_reward = {},
		new_jingying_play_times_remain = 0,
		new_xiyou_play_times_remain = 0,
	}
    for i = 0, 4 do
        info.first_pass_flag[i] = 0
	end
	for i = 0, 4 do
        info.first_pass_reward[i] = 0
	end
	return info
end

function FightData:GetCrystalFbFightState(scene_seq, scene_type_1, expend_item)
	local fts = FightConfig.FightTipsState
	local state = fts.none
	if 1 == scene_type_1 then
		local index = math.floor(scene_seq / 32)
		local seq = scene_seq % 32
		local flag = self.fight_crystal_sweep.first_pass_flag[index]
		local role_enter_energy = self:GetFightStrengthNum()
		if role_enter_energy >= FightConfig.CRYSTAL_FB_ENERGY_PER or Item.GetNum(expend_item) > 0 then
			state = (bit:uint_and(flag, (bit:_lshift(1, seq)))) > 0 and fts.sweep or fts.none
		end
	elseif Item.GetNum(expend_item) > 0 then
		state = fts.challenge
	end
	return state
end

function FightData:GetCrystalFbEneryTotalTimeStamp(info)
	if 0 == info.next_energy_reply_tamp then return 0 end
	local co = self.crystal_config_other
	return (self:GetCrystalFbSweepMaxPower() - info.role_enter_energy - 1) / co.recovery_power * co.recovery_time + info.next_energy_reply_tamp
end

function FightData:GetCrystalFbSweepUsePower()
	return self.crystal_config_other.use_power
end

function FightData:GetCrystalFbSweepExtraPowerLimit()
	return self.crystal_config_other.extra_power_limit
end

function FightData:GetCrystalFbPassTimeMax()
	return self.crystal_config_other.pass_time_max
end

function FightData:GetCrystalFbExtraBuyTimes()
	return self.crystal_config_other.extra_buy_times or 0
end

function FightData:GetCrystalFbExtraBuyConsume()
	return self.crystal_config_other.extra_buy_consume or 0
end

function FightData:GetCrystalFbNormalRewardTime()
	return self.crystal_config_other.normal_reward_time
end

function FightData:SetFightSweepRewardInfo(protocol)
    local info = {
		sweep_num = protocol.param2,
		exp_num = protocol.param1,
		item_s1_num = 0,
		rewards = {}
	}
	for _, vo in ipairs(protocol.item_list) do
		local item_id = vo.item_id
		if CommonItemId.SkyGlass == item_id then
			info.item_s1_num = vo.num
		else
			local item = Item.New(vo)
			table.insert(info.rewards, item)
		end
	end
	self.fight_sweep_reward_info = info
end

function FightData:GetFightSweepRewardInfo()
    return self.fight_sweep_reward_info
end

function FightData:IsFightCrystalEnergyEnough()
    return self:GetFightStrengthNum() >= self:GetCrystalFbSweepUsePower()
end

function FightData:IsFightCrystalPassFb(scene_seq)
	scene_seq = scene_seq or 0
	local index = math.floor(scene_seq / 32)
	local seq = scene_seq % 32
	local flag = self.fight_crystal_sweep.first_pass_flag[index]
	return (bit:uint_and(flag, (bit:_lshift(1, seq)))) > 0
end

function FightData:IsFightCrystalRewardGetFb(scene_seq)
	local index = math.floor(scene_seq / 32)
	local seq = scene_seq % 32
	local flag = self.fight_crystal_sweep.first_pass_reward[index]
	return (bit:uint_and(flag, (bit:_lshift(1, seq)))) > 0
end

function FightData:IsCanFightCrystalFbSweep(cur_sel_dungeon)
	local value, tips = true, ""
	local cur_sel_dungeon = cur_sel_dungeon or self.cur_sel_dungeon
	local pass_time_max = self:GetCrystalFbPassTimeMax()
    local num = pass_time_max + (self.fight_fb_ymz.normal_fb_record_bt_list[cur_sel_dungeon.scene_seq] or 0) - (self.fight_fb_ymz.normal_fb_record_list[cur_sel_dungeon.scene_seq] or 0)
	if not self:IsFightCrystalPassFb(cur_sel_dungeon.scene_seq) then
		value = false
		tips = string.format(Language.Fight.Sweep.sweep_tips1, cur_sel_dungeon._scene.name)
	elseif SCENE_TYPE.COMMON_SCENE ~= SceneData.Instance:GetMode() then
		value = false
		tips = Language.Fight.Sweep.sweep_tips2
	elseif MainViewData.Instance:IsBattle() then
		value = false
		tips = Language.Fight.Sweep.sweep_tips3
	elseif 0 == num then
		value = false
		tips = Language.Fight.Sweep.sweep_tips5
	-- elseif 0 == BagData.Instance:GetNumByItemId(cur_sel_dungeon.consume.item_id) and not self:IsFightCrystalEnergyEnough() then
	elseif not self:IsFightCrystalEnergyEnough() then
		value = false
		tips = string.format(Language.Fight.Sweep.sweep_tips4, self:GetCrystalFbSweepUsePower())
		if self:GetSweepBuyTimes() - self.fight_crystal_sweep.buy_times > 0 then
			ViewMgr:OpenView(FightCrystalSweepItemView)
		end
	end
	return value, tips
end

function FightData:IsFightCrystalEnergyMaxRedPoint()
    return RoleData.Instance:GetRoleLevel() >= self.crystal_config_other.red_point_level and self:GetFightStrengthNum() == self:GetCrystalFbSweepMaxPower()
end

function FightData:GetFightCrystalExpNum()
	local role_level = RoleData.Instance:GetRoleLevel()
	local exp_num = 0
	for _, v in ipairs(self.crystal_config_exp) do
		if role_level == v.level then
			exp_num = v.exp * self:GetCrystalFbSweepUsePower()
			break
		end
	end
	return exp_num
end

function FightData:GetCompleteBackTips()
	local fight_crystal_info = FightData.Instance:GetFightCrystalInfo()
	local cur_seq_info = FightData.Instance:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
	local is_complete = false
	if cur_seq_info then
		local last_layer = fight_crystal_info.layer == cur_seq_info.layer
		is_complete = last_layer and fight_crystal_info.is_finish
	end
	return is_complete and "" or Language.Fight.CrystalDungen.level_not_complete
end

function FightData:GetEnergyExpNum()
	local co = Config.crystal_scene_auto.exp[RoleData.Instance:GetRoleLevel()]
    return co and co.exp or 0
end