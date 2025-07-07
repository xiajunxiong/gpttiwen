MazeRunData = MazeRunData or BaseClass()

--符文信息
MazeRunData.Rune = {
	SpeedUp = 0,
	SpeedDown = 1,
	MoveFreeze = 2,
	BattleAvoid = 3,
	BattleOften = 4,
}
MazeRunData.MaxRank = 50	--排名最多显示50名
function MazeRunData:__init()
	if MazeRunData.Instance ~= nil then
		Debuger.LogError("[MazeRunData] attempt to create singleton twice!")
		return
	end
	MazeRunData.Instance = self

	self.wait_data = SmartData.New({
		begin_timestamp = 0,
		wait_num = 0,
		min_lv = 0,
		max_level = 0,
	})
	
	self.running_data = SmartData.New({
		is_timing = false,
		begin_timestamp = 0,
		cur_floor = 0,
		cur_rank = 0,	--本队当前排名
		rank_list = {},
	})
	---Key: rune id,value: num
	self.rune_bag = SmartData.New({
		[MazeRunData.Rune.SpeedUp] = 0,
		[MazeRunData.Rune.SpeedDown] = 0,
		[MazeRunData.Rune.MoveFreeze] = 0,
		[MazeRunData.Rune.BattleAvoid] = 0,
		[MazeRunData.Rune.BattleOften] = 0,
	}) 

	--SCActivityMazeRacerPassdInfo
	self.pass_data = {	
		pass_time = 0,
		rank_posi = 0,
		total_encounter = 0,
		award_list = {},
	}

	--key:SCActivityMazeRacerRankLists.maze_id val:SCActivityMazeRacerRankLists.rank_item
	self.rank_data = SmartData.New()

	--SCActivityMazeRacerRankLists.rank_item
	-- self.my_rank_data = SmartData.New()

	self.view_data = SmartData.New({
			rank_maze_id = 1,
		})

	--key: obj_id, value:MazeRunData.Rune
	self.obj_rune_curse_info = SmartData.New()

	self.level_cfg = Config.maze_cfg_auto.scene
	self.level_config = KeyList(self.level_cfg, "scene_id")
	self.reward_config = Config.maze_cfg_auto.act_rank_reward
	self.maze_reward_config = Config.maze_cfg_auto.maze_rank_reward

	self.cur_sel_preview = SmartData.New({val = 1})
	self.max_layer_cache = {} ---key:maze_id,value:max_layer
	self:InitRewardPreviewData()

	-- 通关次数记录
	self.pass_count = SmartData.New({count = 0})
end

function MazeRunData:__delete()

end
function MazeRunData:Clear()
	self.wait_data.begin_timestamp = 0
	self.running_data.begin_timestamp = 0
	self.running_data.cur_floor = 0
end

function MazeRunData:WaitData()
	return self.wait_data
end

function MazeRunData:SetRuneNum(amulet_num)
	for rid,_ in pairs(self.rune_bag) do
		self.rune_bag[rid] = 0
	end
	for _,item_info in pairs(amulet_num) do
		self.rune_bag[item_info.item_id] = item_info.item_num
	end
end

function MazeRunData:RuneBag()
	return self.rune_bag
end

function MazeRunData:RunningData()
	return self.running_data
end

function MazeRunData:PassData()
	return self.pass_data
end

function MazeRunData:SetPassData(protocol)
	self.pass_data = protocol
end

function MazeRunData:AllRankData()
	return self.rank_data
end

function MazeRunData:RankData(maze_id)
	return self.rank_data[maze_id] or {}
end

function MazeRunData:SetRankData(maze_id,rank_list)
	self.rank_data[maze_id] = rank_list
	--排行信息是否是自己的
	-- for _,rl in pairs(self.rank_data) do
	-- 	for _,rank_info in pairs(rl) do
	-- 		for _,mem_info in pairs(rank_info.mem_list) do
	-- 			Debuger.LogError("%s=======%s",mem_info.role_name ,RoleData.Instance:GetBaseData().name)
	-- 			if mem_info.role_name == RoleData.Instance:GetBaseData().name then
	-- 				self:SetMyPassRank(rank_info)
	-- 				return
	-- 			end
	-- 		end
	-- 	end	
	-- end
	-- self:SetMyPassRank(nil)

	-- for i=#rank_list,MazeRunData.MaxRank+1,-1 do
	-- 	table.remove(rank_list,i)
	-- end

end

function MazeRunData:AddRankData(maze_id,rank_data)
	local old_rd = self.rank_data[maze_id] or {}
	if old_rd == nil then
		Debuger.LogErrorSD("MazeRunData:AddRankData ERROR old_rd is NULL,maze_id=%s",tostring(maze_id))
		return
	end
	local new_rd = {}
	for i=1,#old_rd do
		local ri = old_rd[i]
		if ri.rank < rank_data.rank then
		elseif ri.rank == rank_data.rank then
			ri.rank = ri.rank + 1
			table.insert(new_rd,rank_data)
		else
			ri.rank = ri.rank + 1
		end
		table.insert(new_rd,ri)
	end
	if #old_rd == #new_rd then
		rank_data.rank = #new_rd + 1
		table.insert(new_rd,rank_data)
	end
	self.rank_data[maze_id] = new_rd

	for i=#new_rd,MazeRunData.MaxRank+1,-1 do
		table.remove(new_rd,i)
	end
end

function MazeRunData:ViewData()
	return self.view_data
end

-- function MazeRunData:MyPassRank()
-- 	return self.my_rank_data
-- end

-- function MazeRunData:SetMyPassRank(rank_item)
-- 	self.my_rank_data:Set(rank_item or {})
-- end

function MazeRunData:SetRuneCurseInfo(obj_id,rune_id)
	if rune_id < 0 then
		self.obj_rune_curse_info[obj_id] = nil
	else
		self.obj_rune_curse_info[obj_id] = rune_id
	end
end

function MazeRunData:ClearRuneCurseInfo()
	self.obj_rune_curse_info:Set({})
end

function MazeRunData:ObjRuneCurseInfo(obj_id)
	local re_val = self.obj_rune_curse_info[obj_id]
	if re_val == nil or re_val < 0 then
		return nil
	else
		return re_val
	end
end

function MazeRunData:RuneCurseInfo()
	return self.obj_rune_curse_info
end

function MazeRunData:IsMoveFreeze()
	return self.obj_rune_curse_info[RoleData.Instance:GetBaseData().obj_id] == MazeRunData.Rune.MoveFreeze
end

function MazeRunData.RuneName(rune_id)
	local lang_str = Language.MazeRun.RuneName[rune_id]
	if lang_str == nil then
		lang_str = string.format("%s[%s]",Language.MazeRun.UnknowRune,tostring(rune_id))
	end
	return lang_str
end
function MazeRunData:IsInScene()
	return self.level_config[SceneData.Instance:SceneId()]
end

function MazeRunData:MaxLayer()
	local maze_id = 1
	local role_lv = RoleData.Instance:GetRoleLevel()
	for _,v in pairs(Config.maze_cfg_auto.maze) do
		if v.min_level <= role_lv and v.max_level >= role_lv then
			maze_id = v.maze_id
			break
		end
	end
	local re_val = self.max_layer_cache[maze_id]
	if re_val == nil then
		for _,v in pairs(Config.maze_cfg_auto.tier) do
			if v.maze_id == maze_id then
				re_val = v.layer
				self.max_layer_cache[maze_id] = re_val
				break
			end
		end
	end
	return re_val
end

function MazeRunData:DoorName(name)
	if self:IsInScene() then
		local cur_level = self:RunningData().cur_floor
		local max_floor = self:MaxLayer()
		if cur_level == (max_floor - 1) then
			name = Language.MazeRun.DoorNames[2]
		elseif cur_level >= max_floor then
			name = Language.MazeRun.DoorNames[3]
		else
			name = Language.MazeRun.DoorNames[1]
		end
	end
	return name
end

function MazeRunData:InitRewardPreviewData()
	self.reward_preview_list = {}
	for i,v in ipairs(self.reward_config) do
		if self.reward_preview_list[v.maze_id] == nil then
			self.reward_preview_list[v.maze_id] = {}
		end
		table.insert(self.reward_preview_list[v.maze_id],v)
	end
	self.maze_reward_perview_list = {}
	for i,v in pairs(self.maze_reward_config) do
		if self.maze_reward_perview_list[v.maze_id] == nil then
			self.maze_reward_perview_list[v.maze_id] = {}
		end
		table.insert(self.maze_reward_perview_list[v.maze_id],v)
	end
end

function MazeRunData:GetRewardPreviewData(maze_id)
	maze_id = maze_id or self.cur_sel_preview.val
	return self.reward_preview_list[maze_id] or {}
end
function MazeRunData:DoorInfo(info)
	if info then
		self.door_info = info
	else
		return self.door_info
	end
end

function MazeRunData:GetDetailInfoRewards()
	local rewards = {}
	local co = ActivityData.Instance:GetActRewardData(ActType.MzeRun)
	if co then
		for _, item_id in pairs(string.split(co.reward_id, ":")) do
			table.insert(rewards, Item.Init(tonumber(item_id)))
		end
	end
	return rewards
end

-- 判断当前角色所处在的迷宫id
function MazeRunData:GetCurMazeId()
	local level = RoleData.Instance:GetRoleLevel()
	for i,v in pairs(Config.maze_cfg_auto.maze) do
		if level >= v.min_level and level <= v.max_level then
			return v.maze_id
		end
	end
	return 1
end

-- 整理排名奖励信息
function MazeRunData:FormatRankReward(base_reward_cfg,tip)
	local level = RoleData.Instance:GetRoleLevel()
	local maze_id = self:GetCurMazeId()
	local reward_cfg = base_reward_cfg[maze_id] or {}
	local rank_reward_list = {}
	local is_pass = true
	for i = 1, #reward_cfg do
		local v = reward_cfg[i]
		is_pass = true
		if v.min_level and v.max_level then
			if level < v.min_level or level > v.max_level then
				is_pass = false
			end
		end
		if is_pass == true then
			local rank_title
			if v.rank_min == v.rank_max then
				rank_title = string.format(Language.MazeRun.RankStr1,v.rank_max)
			else
				rank_title = string.format(Language.MazeRun.RankStr2,v.rank_min,v.rank_max)
			end
			local info = {}
			info.item_list = DataHelper.FormatItemList(v.reward_show or {})
			info.title_name = rank_title
			info.tip_str = tip
			info.title_type = 2
			table.insert(rank_reward_list,info)
		end
	end
	return rank_reward_list
end

-- 总排名奖励信息
function MazeRunData:GetRankRewardInfo(is_help)
	local info = {}
	info.toggle_name = Language.MazeRun.ToggleName2
	local tip = not is_help and Language.MazeRun.RankRewardTip1 or ""
	info.reward_item_list = self:FormatRankReward(self.reward_preview_list,tip)
	return info
end

-- 迷宫排名奖励信息
function MazeRunData:GetMazeRankRewardInfo(is_help)
	local info = {}
	info.toggle_name = Language.MazeRun.ToggleName1
	local tip = not is_help and Language.MazeRun.RankRewardTip2 or ""
	info.reward_item_list = self:FormatRankReward(self.maze_reward_perview_list,tip)
	return info
end

-- 全部奖励信息
function MazeRunData:GetAllRewardInfo(is_help)
	local reward_list = {}
    local rank_info = self:GetRankRewardInfo(is_help)
	local maze_rank_info = self:GetMazeRankRewardInfo(is_help)
	table.insert(reward_list,maze_rank_info)
	table.insert(reward_list,rank_info)
	local tip_info = {}
    tip_info.toggle_name = Language.Activity.TipsNotice
    tip_info.reward_item_list = {{tips_act_type = ActType.MzeRun}}
    table.insert(reward_list,tip_info)
    return reward_list
end

-- 问号信息
function MazeRunData:GetAllHelpInfo()
	return self:GetAllRewardInfo(true)
end