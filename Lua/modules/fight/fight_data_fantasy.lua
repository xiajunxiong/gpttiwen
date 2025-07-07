function FightData:FightDataFantasy()
	self.fight_fantasy_info = SmartData.New()
    self.fight_fantasy_objs = SmartData.New()
	self.fb_reward_data = SmartData.New()

	self.fight_fantasy_list = {}
	for _, v in pairs(self.fantasy_config_cave_content) do
		self.fight_fantasy_list[v.seq] = v
	end

    self.fight_fantasy_scenes = {}
	for _, v in pairs(self.fantasy_config_cave_content) do
		self.fight_fantasy_scenes[v.scene_id] = true
	end

	self.is_fantasy_complete = nil
	self.fb_reward_data.item_list = {}
	self.fb_reward_data.flush_flag = -1

	-- self.is_auto_fantasy = false
	self.fantasy_fb_npc_id = Config.monster_dreamland_auto.other[1].npc_id

	self.fantasy_reward_find = Config.monster_dreamland_auto.reward_find
end
function FightData:FantasyReward()
	local total_num= 4 - self:GetTodayFantasyFetch()
	local list = {}
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.fantasy_reward_find) do
        if level >= value.min_level and level <= value.max_level then
            if value.exp > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Exp, num = value.exp * total_num}))
            end
            if value.coin > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Coin, num = value.coin * total_num}))
            end
            break
        end
    end
    return list
end
function FightData:FantasyCostType()
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.fantasy_reward_find) do
        if level >= value.min_level and level <= value.max_level then
            return value.find_type
        end
    end
end

function FightData:SetFantasyFbSceneInfo(protocol)
	self.fight_fantasy_info:Set(protocol.fight_fantasy_info)
end

function FightData:GetFightFantasyInfo()
	return self.fight_fantasy_info
end

function FightData:SetFantasyFbObjInfo(protocol)
	self.fight_fantasy_objs:Set(protocol.obj_list)
end

function FightData:GetFantasyFbObjInfo()
	return self.fight_fantasy_objs
end

function FightData:CheckFantasySceneName(scene_name, layer, seq)
	if not self.fight_fantasy_scenes[SceneData.Instance:SceneId()] then return scene_name end
	local max_layer = self.fight_fantasy_list[seq] and self.fight_fantasy_list[seq].layer or 1
	return Language.Fight.FantasyDungen.layer_door[layer < max_layer and (layer < max_layer - 1 and "next" or "last") or "exit"] or scene_name
end

function FightData:CheckFantasySceneNameS(scene_name, layer, seq)
	if not self.fight_fantasy_scenes[SceneData.Instance:SceneId()] or self.fight_fantasy_list[seq].target_scene == nil or self.fight_fantasy_list[seq].target_scene == "" then return scene_name end
	local cfg = SceneData.Instance:GetSceneCfg(self.fight_fantasy_list[seq].target_scene)
	return nil ~= cfg and cfg.name or scene_name
end

function FightData:GetFantasyLayer()
	if not self.fight_fantasy_scenes[SceneData.Instance:SceneId()] then return 0,0 end
	local max_layer = self.fight_fantasy_list[self.fight_fantasy_info.level_seq] and self.fight_fantasy_list[self.fight_fantasy_info.level_seq].layer or 1
	return self.fight_fantasy_info.layer,max_layer
end

function FightData:GetFantasySceneDesc()
	if not self.fight_fantasy_scenes[SceneData.Instance:SceneId()] then
		return Language.Fight.FantasyDungen.DangeonDescDefault 
	end
	if self.fight_fantasy_list[self.fight_fantasy_info.level_seq] then
		return self.fight_fantasy_list[self.fight_fantasy_info.level_seq].txt
	end
    return ""
end

function FightData:GetFantasyLayerInfo(_scene_id)
	local target_scene = _scene_id == nil and SceneData.Instance:SceneId() or _scene_id
	for k,v in pairs(self.fight_fantasy_list) do 
		if v.scene_id == target_scene then 
			return v
		end 
	end 
	return nil
end

function FightData:GetFinishPassInFantasy()
	local param = 0
	if self.is_fantasy_complete then 
		param = 2
		self:ClearMarkFantasyCountDown()
		return 1,param
	end 
	param = TeamData.Instance:IsLeader() and 1 or 0
	self:ClearMarkFantasyCountDown()
	return 1,param
end

function FightData:SetMarkFantasyCountDown(flag)
	self.is_fantasy_complete = flag
end

function FightData:ClearMarkFantasyCountDown()
	self.is_fantasy_complete = nil
end

function FightData:SetFantasyRewardData(data)
	self.fantasy_reward_data = data
end

function FightData:GetFantasyCurBossKill()
	local objs = self:GetFantasyFbObjInfo()
	local config = self:GetFantasyLayerInfo()

	if config == nil then return false end 

	for k,v in pairs(objs) do 
		if v.npc_id == config.boss_id then 
			return false
		end 
	end 

	return true
end

--获取副本奖励
function FightData:GetFantasyRewardData()
	return self.fantasy_reward_data or nil
end

function FightData:SetFantasyFBRewardInfo(protocol)
	self.fb_reward_data.is_fetch_reward = bit:d2b(protocol.is_fetch_reward)
	if protocol.reward_count == 0 then 
		self.fb_reward_data.flush_flag = self.fb_reward_data.flush_flag + 1
		self.fb_reward_data.item_list = {} 
		return 
	end 


	for k,v in pairs(protocol.reward_list) do 
		local item_data = Item.New()
		item_data.item_id = v.item_id
		item_data.num = v.num
		item_data.is_bind = v.is_bind
		local flag_ = true
		for i,j in pairs(self.fb_reward_data.item_list) do 
			if item_data.item_id == j.item_id and item_data.num == j.num then 
				flag_ = false
			elseif item_data.item_id == j.item_id and item_data.num ~= j.num then 
				self.fb_reward_data.flush_flag = self.fb_reward_data.flush_flag + 1
				j.num = item_data.num 
				flag_ = false
			end 
		end 
		if flag_ then 
			self.fb_reward_data.flush_flag = self.fb_reward_data.flush_flag + 1
		    table.insert(self.fb_reward_data.item_list, item_data)
		end 
	end 

end


function FightData:GetFantasyFBRewardIsFetch(level)
	if level == nil then return false end 
	return self.fb_reward_data.is_fetch_reward[32 - level ] == 1
end

function FightData:ReastFantasyFBFlushFlag()
	self.fb_reward_data.flush_flag = 0
end

function FightData:GetFantasyFBRewardInfo()
	return self.fb_reward_data.item_list or {}
end

function FightData:GetFantasyPreReward()
	-- fantasy_config_boss_reward
	local role_level = RoleData.Instance:GetRoleLevel()
	local reward_cfg = {}
	local total_num= 4 - self:GetTodayFantasyFetch()
	for k,v in pairs(self.fantasy_config_boss_reward) do 
		if v.min_level <= role_level and role_level <= v.max_level and total_num > 0 then
			table.insert( reward_cfg, v )
			total_num = total_num - 1
		end 
		if total_num == 0 then break end 
	end 

	if #reward_cfg == 0 then 
		return reward_cfg
	end 

	local result = {}
	local item_list = {}
	local item_coin = Item.New()
	local item_exp = Item.New()
	-- coin 
	item_coin.item_id = CommonItemId.Coin
	item_coin.is_bind = 0
	item_exp.item_id = CommonItemId.Exp
	item_exp.is_bind = 0

	for k,v in pairs(reward_cfg) do 
		item_coin.num = item_coin.num + v.bind_coin
		item_exp.num = item_exp.num + v.reward_exp

		for i,j in pairs(v.online_reward) do 
			local item_with = false
			for _i,_j in pairs(item_list) do 
				if _j.item_id == j.item_id then 
					_j.num = _j.num + j.num
					item_with = true
				end 
			end 
			if not item_with then 
				local item_Extra = Item.New(j)
				table.insert( item_list, item_Extra)
			end 
		end 

	end 

	table.insert( result, item_exp )
	table.insert( result, item_coin )

	for k,v in pairs(item_list) do 
		table.insert( result, v )
	end 

	return result
end

function FightData:GetFantasySinglePreReward(layer)
	local role_level = RoleData.Instance:GetRoleLevel()
	local reward_cfg = nil
	for k,v in pairs(self.fantasy_config_boss_reward) do 
		if v.min_level <= role_level and role_level <= v.max_level then
			reward_cfg = v 
		end 
	end 

	if reward_cfg == nil then 
		return {}
	end 

	local result = {}
	local item_list = {}
	local item_coin = Item.New()
	local item_exp = Item.New()
	-- coin 
	item_coin.item_id = CommonItemId.Coin
	item_coin.is_bind = 0
	item_exp.item_id = CommonItemId.Exp
	item_exp.is_bind = 0

	-- for k,v in pairs(reward_cfg) do 
	item_coin.num = item_coin.num + reward_cfg.bind_coin
	item_exp.num = item_exp.num + reward_cfg.reward_exp

	for i,j in pairs(reward_cfg.online_reward) do 
		local item_with = false
		for _i,_j in pairs(item_list) do 
			if _j.item_id == j.item_id then 
				_j.num = _j.num + j.num
				item_with = true
			end 
		end 
		if not item_with then 
			local item_Extra = Item.New(j)
			table.insert( item_list, item_Extra)
		end 
	end 

	-- end 

	table.insert( result, item_exp )
	table.insert( result, item_coin )

	for k,v in pairs(item_list) do 
		table.insert( result, v )
	end 

	return result
end

function FightData:GetFantasyFBBackRewardList()
	return RewardData.Instance:GetActivityBackRewardList(self.fantasy_config.reward_find, self.fantasy_config.reward_find_group)
end

function FightData:GetTodayFantasyFetch()
	local result = 0
	for i = 1,4 do 
		if self:GetFantasyFBRewardIsFetch(i) then 
			result = result + 1
		end 

	end 
	return result
end


-- 直接从四象幻境的配置中获取找回内容
function FightData:GetFantasyBackRewardInfo(_num)
	local fetch_reward = bit:d2b(_num)
	local role_level = RoleData.Instance:GetRoleLevel()
	local result = {}
	local result_num = 0
	for i = 1, 4 do 
		if fetch_reward[32 - i ] == 0 then 
			result_num = result_num + 1
			for k,v in pairs(self.fantasy_config_back) do 
				if v.layer == i and role_level >= v.min_level and role_level <= v.max_level then 
					result.exp = v.exp + (result.exp or 0)
					result.coin = v.coin + (result.coin or 0)

					if result.extract == nil and v.group_id ~= -1 then  
						result.extract = {}
						local item = Item.New(self.fantasy_config_back_group[v.group_id].reward)
						table.insert( result.extract, item)
					elseif result.extract ~= nil and v.group_id ~= -1 then 
						local flag_with = false
						local item = Item.New(self.fantasy_config_back_group[v.group_id].reward)
						for _,j in pairs(result.extract) do 
							if j.item_id == item.item_id then 
								j.num = j.num + 1
								flag_with = true
							end 
						end 

						if not flag_with then 
							table.insert( result.extract, item)
						end 
					end 
				end 
			end 
		end 
	end


	return result_num,result
end

function FightData:ChangeFantasyBackRewardInfo(paramt)
	if paramt == nil then return {} end 
	local result = {}
	if paramt.exp and paramt.exp > 0 then 
	    local item_exp = Item.New({item_id = CommonItemId.Exp, num = paramt.exp})
	    table.insert( result, item_exp )
	end 

	if paramt.coin and paramt.coin > 0 then 
	    local item_coin = Item.New({item_id = CommonItemId.Coin, num = paramt.coin})
	    table.insert( result, item_coin )
	end 

	for k, v in pairs(paramt.extract or {}) do 
		local item = Item.New({item_id = v.item_id, num = v.num})
		table.insert( result, item )
	end 

	return result
end

function FightData:CatchFantasyBoxId(vo_id)
	self.fantasy_box_id = vo_id 
end

function FightData:GetFantasyBoxId( )
	return self.fantasy_box_id
end