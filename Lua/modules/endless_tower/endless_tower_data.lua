EndlessTowerData = EndlessTowerData or BaseClass()

function EndlessTowerData:__init()
    if EndlessTowerData.Instance ~= nil then
        Debuger.LogError("[EndlessTowerData] attempt to create singleton twice!")
        return
    end
    EndlessTowerData.Instance = self
	self.rank_data = SmartData.New{ranks_list = {}}
	self.info_data = SmartData.New{pass_level = 1}
	self.opera_data = SmartData.New{cur_state = 0}
	self.anims_data = SmartData.New{battle_state = 0}
	self.config = Config.endless_tower_auto
	self.reward_data_list = {}
end

function EndlessTowerData:GetOutBattle()
	return self.anims_data.battle_state
end

function EndlessTowerData:Initialization()
	self.opera_data:Set({cur_state = 0})
	self.anims_data.battle_state = 0
end

function EndlessTowerData:SetSelectData(data)
	self.select_data = data
end

function EndlessTowerData:GetSelectData()
	return self.select_data
end

function EndlessTowerData:SetFinishRewardData(data)
	self.reward_data = data
end

function EndlessTowerData:GetFinishRewardData()
	return self.reward_data
end

function EndlessTowerData:SetEndlessTowerInfo(protocol)
	self.info_data.pass_level = protocol.pass_level
	self.info_data.battle_seq = protocol.battle_seq
end

function EndlessTowerData:GetPassLevel()
	return self.info_data.pass_level
end

function EndlessTowerData:GetBattleLevel()
	return self.info_data.battle_seq
end

function EndlessTowerData:GetPassLevelData(pass_level)
	pass_level = pass_level or self:GetPassLevel()
	return self.config.challenge[pass_level]
end

function EndlessTowerData:EndPassLevelData()
	return self.config.challenge[#self.config.challenge]
end

function EndlessTowerData:GetRewardData(reward_id)
	local list = {}
	for _,v in pairs(self.config.reward) do
		if v.seq == reward_id then
			for k,item in pairs(v.reward_show) do
				table.insert(list,item)
			end
		end
	end
	return list
end

--当前最大可助战宠物数量
function EndlessTowerData:GetPetNum()
	return Config.endless_tower_auto.other[1].pet_num
end

function EndlessTowerData:InitRankInfo()
	local rank_info = SocietyData.Instance:GetRankInfo(PERSON_RANK_TYPE.ENDLESS_TOWER)
	self.rank_data.ranks_list[CourageChallengeRankType.Main] = rank_info
	self:CheckFriendList(rank_info,CourageChallengeRankType.Friends)
	self:CheckGuildList(rank_info,CourageChallengeRankType.Guild)
end

function EndlessTowerData:GetRankInfo(rank_type)
    return self.rank_data.ranks_list[rank_type] or {}
end

--好友排行榜
function EndlessTowerData:CheckFriendList(rank_list,rank_type)
	self.rank_data.ranks_list[rank_type] = nil
	local friends_list = {}
	for k,v in pairs(rank_list or {}) do
		if SocietyData.Instance:IsFriend(v.role.uid) or v.role.uid == RoleData.Instance:GetRoleId()then
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
function EndlessTowerData:CheckGuildList(rank_list,rank_type)
	self.rank_data.ranks_list[rank_type] = nil
	local guild_list = {}
	if GuildData.Instance:GetGuildID() > 0 then
		for k,v in pairs(rank_list or {}) do
			if GuildData.Instance:GetMemberVo(v.role.uid) ~= nil or v.role.uid == RoleData.Instance:GetRoleId()then
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

function EndlessTowerData:OperaState(state)
	if state then 
		self.opera_data.cur_state = state
	else
		return self.opera_data.cur_state
	end
end

function EndlessTowerData:GetRankCrossList()
	return SocietyData.Instance:GetRankCrossInfo(PERSON_RANK_TYPE.ENDLESS_TOWER)
end

--获取个人排名奖励
function EndlessTowerData:GetCrossRoleRankData()
	local _,rank_info_m = self:GetRankCrossList()
	local my_rank_index = rank_info_m.rank or 0
	return self:GetRewardRankConfig(my_rank_index) or {rank = my_rank_index}
end

--根据排名获取奖励
function EndlessTowerData:GetRewardRankData(rank)
	for k,v in pairs(self.config.rank_reward) do
		if IsRange(rank,v.min_rank,v.max_rank) then
			return v.reward
		end
	end
end

function EndlessTowerData:GetRewardRankConfig(rank)
	for k,v in pairs(self.config.rank_reward) do
		if IsRange(rank,v.min_rank,v.max_rank) then
			return v
		end
	end
end

--奖励列表
function EndlessTowerData:GetRewardConfig()
	return self.config.rank_reward
end

function EndlessTowerData.IsEndlessTower()
	return BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER == FinishData.Instance:GetBattleMode()
end

function EndlessTowerData:SetRewardDataList(reward_list,title_text)
	if #reward_list ~= 0 then
		table.insert(self.reward_data_list,{
			list = DataHelper.FormatItemList(reward_list),
			title_text = title_text
		})
	end
end

function EndlessTowerData:ResetRewardData()
	self.reward_data_list = {}
end

function EndlessTowerData:GetRewardDataList()
	return self.reward_data_list or {}
end