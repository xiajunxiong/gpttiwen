CourageChallengeData = CourageChallengeData or BaseClass()
CourageChallengeData.NPC = 40018
CourageChallengeData.PagodaType = {
	GuMo = 1,
	ShiShen = 2
}
function CourageChallengeData:__init()
	if CourageChallengeData.Instance ~= nil then
		Debuger.LogError("[CourageChallengeData] attempt to create singleton twice!")
		return
	end	
	CourageChallengeData.Instance = self
	self.challenge_config = Config.courage_challenge_auto.challenge
	self.new_challenge_config = Config.courage_challenge_auto.new_challenge
	self.other_info = Config.courage_challenge_auto.other[1]

	self.new_select_data = SmartData.New()
	self.final_select_data = nil
	self.achi_data = SmartData.New({common_reward_flag = {}, token_reward_flag = {}, token_is_buy = 0})
	self.new_rank_list = {}
	self:Initialization()
	self:InitCourageChallengeData()
	self.reward_data_list = {}
end

function CourageChallengeData:Initialization()
	self.select_data = SmartData.New()
	self.new_select_data:Set({})
end

function CourageChallengeData:GetChallengeConfig(seq)
	return self.challenge_config[seq]
end
function CourageChallengeData:GetChallengeConfig2(seq)
	return self.new_challenge_config[seq]
end

function CourageChallengeData:InitCourageChallengeData()
	self.layer_list,self.layer_config = {},{}
	for k,v in pairs(self.challenge_config) do
		if self.layer_config[v.layer_level] == nil then
			self.layer_config[v.layer_level] = {}
			table.insert(self.layer_list,{layer_level = v.layer_level,level = v.level, type = CourageChallengeData.PagodaType.GuMo})
		end
		v.type = CourageChallengeData.PagodaType.GuMo
		table.insert(self.layer_config[v.layer_level],v)
	end
	for k,v in pairs(self.layer_config) do
		table.sort(v,function(a,b)return a.level_seq < b.level_seq end)
	end
	self.score_reward_list = {}
	for k,v in pairs(Config.courage_challenge_auto.score_reward) do
		if self.score_reward_list[v.layer_level] == nil then
			self.score_reward_list[v.layer_level] = {}
		end
		table.insert(self.score_reward_list[v.layer_level],v)
	end
	for k,v in pairs(self.score_reward_list) do
		table.sort(v,function(a,b)return a.seq < b.seq end)
	end

	self.new_layer_list, self.new_layer_config = {}, {}
	for k, v in pairs(self.new_challenge_config) do
		if self.new_layer_config[v.layer_level] == nil then
			self.new_layer_config[v.layer_level] = {}
			table.insert(self.new_layer_list, {layer_level = v.layer_level, level = v.level, type = CourageChallengeData.PagodaType.ShiShen})
		end
		v.type = CourageChallengeData.PagodaType.ShiShen
		table.insert(self.new_layer_config[v.layer_level], v)
	end
	for k, v in pairs(self.new_layer_config) do
		table.sort(v,function(a,b)return a.level_seq < b.level_seq end)
	end


	self.layer_data = SmartData.New{list = {},val = false}
	self.reward_data = SmartData.New{list = {}}
	self.reward_red_piont = SmartData.New{val = false}
	self.rank_data = SmartData.New{ranks_list = {},val = false}
	self.red_point_data = SmartData.New({val = false})
	
	self.new_layer_data = SmartData.New{list = {}, val = false}
	self.new_reward_data = SmartData.New{list = {}, token_is_buy = 0}
end

function CourageChallengeData:GetLayerList()
	local list = {}
	for i=#self.layer_list,1,-1 do
		table.insert(list,self.layer_list[i])
	end
	return list
end

function CourageChallengeData:GetLayerList2()
	local list = {}
	for i=#self.new_layer_list,2,-1 do
		table.insert(list,self.new_layer_list[i])
	end
	return list, self.new_layer_list[1]
end

function CourageChallengeData:GetLayerBossList(layer_level)
	return self.layer_config[layer_level]
end
function CourageChallengeData:GetLayerBossList2(layer_level)
	return self.new_layer_config[layer_level]
end

function CourageChallengeData:GetLayerNum(layer_level)
	return self:GetLayerBossFlagNum(layer_level),#self.layer_config[layer_level] * 3
end
function CourageChallengeData:GetLayerNum2(layer_level)
	return self:GetLayerBossFlagNum2(layer_level),#self.new_layer_config[layer_level] * 3
end

--当前层星数
function CourageChallengeData:GetLayerBossFlagNum(layer_level)
	local num = 0
	for k,v in pairs(self.layer_data.list[layer_level] or {}) do
		num = num + v.flag_num
	end
	return num
end
function CourageChallengeData:GetLayerBossFlagNum2(layer_level)
	local num = 0
	for k, v in pairs(self.new_layer_data.list[layer_level] or {}) do
		num = num + v.flag_num
	end
	return num
end
function CourageChallengeData:GetTotalStar2()
	local list = self.new_layer_data.list or {}
	local num = 0
	for k, v in pairs(list) do
		for k, v2 in pairs(v) do
			num = num + v2.flag_num
		end
	end
	return num
end

function CourageChallengeData:GetLayerBossConfig(layer_level,level_seq)
	for k,v in pairs(self.layer_config[layer_level] or {}) do
		if v.level_seq == level_seq then
			return v
		end
	end
end
function CourageChallengeData:GetLayerBossConfig2(layer_level,level_seq)
	for k,v in pairs(self.new_layer_config[layer_level] or {}) do
		if v.level_seq == level_seq then
			return v
		end
	end
end

--解析条件内容
function CourageChallengeData:GetConditionList(data)
	local list = {}
	for i=1,3 do
		local key = data["condition_" .. i]
		list[i] = self:GetConditionDesc(key)
	end
	return list
end

--获取条件对应评分内容
function CourageChallengeData:GetConditionDesc(condition_id)
	local config = Config.score_condition_auto.condition[condition_id]
	return config and config.desc or ""
end

--获取怪物config
function CourageChallengeData:GetMonsterConfig(monster_group_id)
	local config = Config.monster_group_auto.monster_group[monster_group_id]
	return Cfg.MonsterById(config and config.show_monster or 0) or {}
end

--锢魔之塔全部信息---------------------------------------------
function CourageChallengeData:SetCourageChallengeAllInfo(protocol)
	self.layer_data.list = protocol.layer_list
	self.reward_data.list = protocol.reward_list
end

--锢魔之塔单个信息
function CourageChallengeData:SetCourageChallengeSingleInfo(protocol)
	if self.layer_data.list[protocol.level] == nil then
		self.layer_data.list[protocol.level] = {}
	end
	self.layer_data.list[protocol.level][protocol.seq] = protocol.layer_info
	self.layer_data.val = not self.layer_data.val
end

--锢魔之塔评分奖励
function CourageChallengeData:SetCourageChallengeScoreInfo(protocol)
	self.reward_data.list[protocol.layer_level] = protocol.reward_info
	self.reward_red_piont.val = not self.reward_red_piont.val
end

--锢魔之塔排行榜
function CourageChallengeData:SetCourageChallengeRankListInfo(protocol)
	self.rank_data.ranks_list[protocol.rank_type] = protocol.rank_list
	self:CheckFriendList(protocol.rank_list,CourageChallengeRankType.Friends)
	self:CheckGuildList(protocol.rank_list,CourageChallengeRankType.Guild)
	self.rank_data.val = not self.rank_data.val
	-- CompetitionRanklData.Instance:CheckRankListFlush()
end
--special_type 0无 1好友 
function CourageChallengeData:SetNewCourageChallengeRankListInfo(protocol)
	local list = {}
	for i, v in ipairs(protocol.info.rank_infos) do
		list[i] = self:RankinfoStruct(v)
	end 
	self.new_rank_list[protocol.info.special_type] = list
	self.rank_data.val = not self.rank_data.val
end
function CourageChallengeData:RankinfoStruct(info)
    local t = {}	
    t.rank = info.rank
    t.role_id = info.role.uid
    t.name = info.role.name
    t.avatar_type = info.role.avatar_type
    t.avatar_id = info.role.avatar_id
    t.prof = info.role.prof
    t.star_num = info.rank_value
    t.layer_level = info.flexible_int or 0
    t.rank_value = t.star_num
    return t
end
--获取排行榜
function CourageChallengeData:GetRankList(rank_type)
	return self.rank_data.ranks_list[rank_type] or {}
end
function CourageChallengeData:GetRankList2(special_type)--0 1好友2家族
	return self.new_rank_list[special_type] or {}
end

--获取当前Boss通关信息
function CourageChallengeData:GetLayerBossInfo(layer_level,level_seq)
	if self.layer_data.list[layer_level] then
		return self.layer_data.list[layer_level][level_seq]
	end
end
--获取当前Boss通关信息
function CourageChallengeData:GetLayerBossInfo2(layer_level,level_seq)
	if self.new_layer_data.list[layer_level] then
		return self.new_layer_data.list[layer_level][level_seq]
	end
end

--是否通关
function CourageChallengeData:GetFirstKill(level,seq)
	local info = self:GetLayerBossInfo(level,seq) or {}
	return info.first_kill or 0
end
function CourageChallengeData:GetFirstKill2(level, seq)
	local info = self:GetLayerBossInfo2(level, seq) or {}
	return info.first_kill or 0
end
function CourageChallengeData:GetRewardList(data,info)
	local reward_list = {}
	if data.extra_reward then
		for _, v in pairs(data.extra_reward) do
			v.is_extra_reward = true
			v.is_grey = (info.get_extra or 0) == 1 or (RoleData.Instance:GetRoleLevel() > self:GetExtraRewardLevel() + data.level)
			v.has_overdue = (info.get_extra or 0) == 0 and (RoleData.Instance:GetRoleLevel() > self:GetExtraRewardLevel() + data.level) 
			table.insert(reward_list,v)
		end
	end
	for _, v in pairs(data.clear_reward) do
		v.is_grey = (info.first_kill or 0) == 1
		table.insert(reward_list,v)
	end
	if data.coin > 0 then
		table.insert(reward_list,{item_id = CommonItemId.Coin,num = data.coin,is_grey = (info.first_kill or 0) == 1})
	end
	return reward_list
end

--获取当前层奖励列表
function CourageChallengeData:GetLayerRewardList(layer_level)
	return self.score_reward_list[layer_level] or {}
end

function CourageChallengeData:GetStarRewardConfig(layer_level)
	return Config.courage_challenge_auto.star_reward[layer_level]
end

--奖励领取
function CourageChallengeData:GetLayerRewardFlag(layer_level,seq)
	if self.reward_data.list[layer_level] then
		local flag_list = self.reward_data.list[layer_level].flag or {}
		return flag_list[seq] or 0
	end
	return 0
end

--最终奖励领取
function CourageChallengeData:GetLayerRewardEndFlag(layer_level)
	if self.reward_data.list[layer_level] then
		local flag_list = self.reward_data.list[layer_level].flag or {}
		return flag_list[63] or 0
	end
	return 0
end

--红点相关
function CourageChallengeData:GetRewardNum()
	if self.red_point_data.val then
		return 1
	end
	for k,reward_list in pairs(self.score_reward_list) do
		if self:GetLayerRewardEndFlag(k) == 0 then
			if self:GetLayerBossFlagNum(k) >= reward_list[#reward_list].star then
				return 1
			end
		end
		for _,v in pairs(reward_list) do
			if self:GetLayerRewardFlag(k,v.seq) == 0 then
				if self:GetLayerBossFlagNum(k) >= v.star then
					return 1
				end
			end
		end
	end
	local is_open, _ = FunOpen.Instance:GetFunIsOpened(Mod.Experience.ShiShen)
	if is_open then
		if self:AchiCanGet() then
			return 1
		end
	end
	
	return 0
end

--当前层领取红点
function CourageChallengeData:GetlayerRewardNum(layer_level)
	local reward_list = self.score_reward_list[layer_level] or {}
	if self:GetLayerRewardEndFlag(layer_level) == 0 then
		if #reward_list > 0 and self:GetLayerBossFlagNum(layer_level) >= reward_list[#reward_list].star then
			return 1
		end
	end
	for k,v in pairs(reward_list) do
		if self:GetLayerRewardFlag(layer_level,v.seq) == 0 then
			if self:GetLayerBossFlagNum(layer_level) >= v.star then
				return 1
			end
		end
	end
	return 0
end

--跳转选中
function CourageChallengeData:SetSelectData(data)
	self.select_data:Set(data)
end
function CourageChallengeData:SetSelectData2(data)
	self.new_select_data:Set(data)
end
function CourageChallengeData:FinalSelectData(data)
	self.final_select_data = data
end

function CourageChallengeData:GetSelectData()
	return self.select_data
end
function CourageChallengeData:GetSelectData2()
	return self.new_select_data
end
function CourageChallengeData:GetFinalSelectData()
	return self.final_select_data
end

--当前选中的层数 index
function CourageChallengeData:GetSelectIndex(layer_level)
	for k,v in pairs(self:GetLayerList()) do
		if v.layer_level == layer_level then
			return k
		end
	end
end

--当前选中的层数 index
function CourageChallengeData:GetSelectIndex2(layer_level)
	for k,v in pairs(self:GetLayerList2()) do
		if v.layer_level == layer_level then
			return k
		end
	end
	return nil
end

--选中下一关
function CourageChallengeData:GetSelectItemIndex(layer_level,level_seq)
	local Index = function(list,level_seq)
		for k,v in pairs(list) do
			if v.level_seq == level_seq then
				return k
			end
		end
	end
	local item_list = self:GetLayerBossList(layer_level) or {}
	local index = Index(item_list,level_seq) or 1
	if index < #item_list and item_list[index + 1] ~= nil then
		-- local role_level = RoleData.Instance:GetRoleLevel()
		-- if role_level >= item_list[index + 1].level then
			return index + 1
		-- end
	end
	return index
end

--上一只怪是否解锁
function CourageChallengeData:IsLastPass(seq)
	if seq ~= 1 then
		local config = self.challenge_config[seq-1]
		if config ~= nil then
			return self:GetFirstKill(config.layer_level,config.level_seq) == 1
		end
	end
	return true
end

--上一只怪是否解锁
function CourageChallengeData:IsLastPass2(seq)
	if seq ~= 1 then
		local config = self.new_challenge_config[seq-1]
		if config ~= nil then
			return self:GetFirstKill2(config.layer_level,config.level_seq) == 1
		end
	end
	return true
end

--上一层的怪是否通关
function CourageChallengeData:GetLastPassData(layer_level)
	local GetIndex = function()
		for k,v in pairs(self.layer_list) do
			if v.layer_level == layer_level then
				return k
			end
		end
	end
	local index = GetIndex(layer_level)
	if index > 1 then
		local layer_level = self.layer_list[index-1].layer_level
		local config_list = self.layer_config[layer_level]
		return config_list[#config_list]
	end
end

--上一层的怪是否通关
function CourageChallengeData:GetLastPassData2(layer_level)
	local GetIndex = function()
		for k,v in pairs(self.new_layer_list) do
			if v.layer_level == layer_level then
				return k
			end
		end
	end
	local index = GetIndex(layer_level)
	if index > 1 then
		local layer_level = self.new_layer_list[index-1].layer_level
		local config_list = self.new_layer_config[layer_level]
		return config_list[#config_list]
	end
end

--推荐选中
function CourageChallengeData:GetRecommendData()
	local last_level_pos = 0
	for i,v in ipairs(self.challenge_config) do
		if self:GetFirstKill(v.layer_level,v.level_seq) == 0 and v.level <= RoleData.Instance:GetRoleLevel() then
			return v
		else 
			if v.level <= RoleData.Instance:GetRoleLevel() and self:GetFirstKill(v.layer_level,v.level_seq) == 1 then 
				last_level_pos = i
			end 
		end
	end
	return last_level_pos > 0 and self.challenge_config[last_level_pos] or self.challenge_config[#self.challenge_config]
end

function CourageChallengeData:GetRecommendData2()
	local last_level_pos = 0
	for i,v in ipairs(self.new_challenge_config) do
		if self:GetFirstKill2(v.layer_level,v.level_seq) == 0 and v.level <= RoleData.Instance:GetRoleLevel() then
			return v
		else 
			if v.level <= RoleData.Instance:GetRoleLevel() and self:GetFirstKill2(v.layer_level,v.level_seq) == 1 then 
				last_level_pos = i
			end 
		end
	end
	return last_level_pos > 0 and self.new_challenge_config[last_level_pos] or self.new_challenge_config[#self.new_challenge_config]
end

function CourageChallengeData:GetCurrLevel()
	for i,v in ipairs(self.challenge_config) do
		if self:GetFirstKill(v.layer_level,v.level_seq) == 0 then
			return self.challenge_config[i-1] and self.challenge_config[i-1].level or v.level
		end
	end
	return self.challenge_config[#self.challenge_config].level
end

function CourageChallengeData:GetCurrLevel2()
	for i,v in ipairs(self.new_challenge_config) do
		if self:GetFirstKill2(v.layer_level,v.level_seq) == 0 then
			return self.new_challenge_config[i-1] and self.new_challenge_config[i-1].level or v.level
		end
	end
	return self.new_challenge_config[#self.new_challenge_config].level
end

--好友排行榜
function CourageChallengeData:CheckFriendList(rank_list,rank_type)
	self.rank_data.ranks_list[rank_type] = nil
	local friends_list = {}
	for k,v in pairs(rank_list or {}) do
		if SocietyData.Instance:IsFriend(v.role_id) or v.role_id == RoleData.Instance:GetRoleId()then
			table.insert(friends_list,TableCopy(v))
		end
	end
	table.sort(friends_list,function(a,b)return a.rank < b.rank end)
	for i,v in ipairs(friends_list) do
		v.rank_num = i
	end
	self.rank_data.ranks_list[rank_type] = friends_list
end

--家族排行榜
function CourageChallengeData:CheckGuildList(rank_list,rank_type)
	self.rank_data.ranks_list[rank_type] = nil
	local guild_list = {}
	if GuildData.Instance:GetGuildID() > 0 then
		for k,v in pairs(rank_list or {}) do
			if GuildData.Instance:GetMemberVo(v.role_id) ~= nil or v.role_id == RoleData.Instance:GetRoleId()then
				table.insert(guild_list,TableCopy(v))
			end
		end
	end
	table.sort(guild_list,function(a,b)return a.rank < b.rank end)
	for i,v in ipairs(guild_list) do
		v.rank_num = i
	end
	self.rank_data.ranks_list[rank_type] = guild_list
end

function CourageChallengeData:GetRoleRinkInfo()
	for k,v in pairs(self.rank_data.ranks_list[CourageChallengeRankType.Main] or {}) do
		if v.role_id == RoleData.Instance:GetRoleId()then
			return v
		end
	end
	return {star_num = 0,layer_level = self.challenge_config[1].layer_level}
end

function CourageChallengeData:GetRoleRinkInfo2()
	for k,v in pairs(self.new_rank_list[CourageChallengeRankType.Main] or {}) do
		if v.role_id == RoleData.Instance:GetRoleId()then
			return v
		end
	end
	return {star_num = 0,layer_level = self.new_challenge_config[1].layer_level}
end

function CourageChallengeData:GetCourageChallengeTip(level)
	return UnityPlayerPrefs.GetInt(PrefKeys.CourageChallengeTip(level))
end

function CourageChallengeData:SetCourageChallengeTip(level,value)
	UnityPlayerPrefs.SetInt(PrefKeys.CourageChallengeTip(level),value)
end

function CourageChallengeData:GetExtraRewardLevel()
	return Config.courage_challenge_auto.other[1].extra_reward_level
end

--是否显示提示额外奖励
function CourageChallengeData:IsShowExtraTip(data)
	if data == nil then
		return false
	end
	local level = data.level or 0
	if not FunOpen.Instance:GetFunIsOpened(Mod.Experience.GuMo) then
		return false
	end
	if self:GetCourageChallengeTip(level) == 1 then
		return false
	end
	return RoleData.Instance:GetRoleLevel() <= self:GetExtraRewardLevel() + level
end

function CourageChallengeData:GetCurrPetNum()
	local config = Config.courage_challenge_auto.help_fight
	if self.help_fight_list == nil then
		self.help_fight = DataHelper.ReverseList(TableCopy(config))
	end
	local data = self:GetRecommendData()
	for i,v in ipairs(self.help_fight) do
		if data.seq > v.num then
			return v.pet_num
		end
	end
	if data.seq > config[#config].num then
		return config[#config].pet_num
	end
	return 0
end

function CourageChallengeData:GetNextPetConfig(pet_num)
	for k,v in pairs(Config.courage_challenge_auto.help_fight) do
		if v.pet_num == pet_num then
			return self.challenge_config[v.num]
		end
	end
end

function CourageChallengeData:SetRedPoint(flag)
	self.red_point_data.val = flag
end

function CourageChallengeData:SetPetNum(num)
	self.pet_num = num
end

function CourageChallengeData:GetPetNum()
	return self.pet_num
end

--获取结算奖励
function CourageChallengeData:GetFinishRewardData(config,param_t)
	if param_t.first_kill == 0 and param_t.layer_info.first_kill == 1 then
		local reward_list = self:GetRewardList(config,param_t)
		if (param_t.get_extra or 0) == 0 and (RoleData.Instance:GetRoleLevel() > self:GetExtraRewardLevel() + config.level) then
			return DataHelper.FormatList(reward_list,2,#reward_list)
		else
			return reward_list
		end
	end
end

function CourageChallengeData:SetRewardDataList(reward_list,title_text)
	if #reward_list ~= 0 then
		table.insert(self.reward_data_list,{
			list = DataHelper.FormatItemList(reward_list),
			title_text = title_text
		})
	end
end

function CourageChallengeData:ResetRewardData()
	self.reward_data_list = {}
end

function CourageChallengeData:GetRewardDataList()
	return self.reward_data_list or {}
end
function CourageChallengeData:GetAchiList()
	return Config.courage_challenge_auto.achievement_reward
end

function CourageChallengeData:SetNewCourageChallengeAllInfo(protocol)
	-- LogDG(protocol)
	self.new_layer_data.list = protocol.list
end

--锢魔之塔单个信息
function CourageChallengeData:SetNewCourageChallengeSingleInfo(protocol)
	local info = protocol.info
	if self.new_layer_data.list[info.layer_level] == nil then
		self.new_layer_data.list[info.layer_level] = {}
	end
	self.new_layer_data.list[info.layer_level][info.level_seq] = info
	self.new_layer_data.val = not self.new_layer_data.val
end

function CourageChallengeData:SetNewCourageChallengeRewardInfo(procotol)
	self.achi_data:Set(procotol.all_info)
end
function CourageChallengeData:GetAchiData()
	return self.achi_data
end

function CourageChallengeData:IsGet1(req)
	return (self.achi_data.common_reward_flag[req] or 0) == 1
end

function CourageChallengeData:IsGet2(req)
	return (self.achi_data.token_reward_flag[req] or 0) == 1
end

function CourageChallengeData:AchiCanGet()
	local list = self:GetAchiList()
	for i, v in ipairs(list) do
		if self:AchiCanGet1(v) then
			return true	
		end
		if self:AchiCanGet2(v) then
			return true
		end
	end
	return false
end
function CourageChallengeData:AchiCanGet1(data)
	local total = self:GetTotalStar2()
	return total >= data.star_num and not self:IsGet1(data.reward_seq)
end
function CourageChallengeData:AchiCanGet2(data)
	local total = self:GetTotalStar2()
	local is_open = (self.achi_data.token_is_buy or 0) == 1
	return total >= data.star_num and is_open and not self:IsGet2(data.reward_seq)
end