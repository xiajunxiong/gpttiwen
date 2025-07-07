
WorldBossData = WorldBossData or BaseClass()

function WorldBossData:__init()
	if WorldBossData.Instance ~= nil then
        Debuger.LogError("[WorldBossData] attempt to create singleton twice!")
        return
	end
	WorldBossData.Instance = self
	self:WorldBossConfig()
	self:WorldBossData()
end

function WorldBossData:WorldBossConfig()
	self.config = Config.monster_taofaboss_auto
	self.config3 = Config.monster_tianshen_auto
	self.config_monster = self.config.monster
	self.config3_monster = self.config3.monster
	self.config_reward_show = self.config.reward_show
	self.config_reward_show2 = self.config.reward_show2
	self.config_reward_show3 = self.config3.reward_show2
	self.config_reward_show_3 = self.config3.reward_show
end

function WorldBossData:WorldBossData()
	self:RankList()
	self.boss_info = SmartData.New()
	self.boss_info2 = SmartData.New()
	self.boss_info3 = SmartData.New()
	self.pos_info = {}
	self.pos_info2 = {}
	self.pos_info3 = {}
	self.my_rank_info = SmartData.New({shang_hai = 0, zhiliao = 0, rank0 = 0, rank1 = 0})
	self.boss_rank_info = SmartData.New({list0 = {}, list1 = {}})
end

function WorldBossData:RankList()
	self.rank_list = {}
	self.rank_type_list = {}
	for _, v in pairs(WorldBossConfig.RankType) do
		self.rank_type_list[v] = {
			type = v,
			title = Language.WorldBoss.RankType[v],
		}
	end
end

function WorldBossData:SetWorldBossInfo(protocol)
	self.boss_info:Set(protocol.boss_info)
end

function WorldBossData:SetWorldBoss2Info(protocol)
	self.boss_info2:Set(protocol.boss_info)
end

function WorldBossData:SetWorldBoss3Info(protocol)
	self.boss_info3:Set(protocol.boss_info)
end

function WorldBossData:GetBossInfo()
	return self.boss_info
end

function WorldBossData:GetBossInfo2()
	return self.boss_info2
end

function WorldBossData:GetBossInfo3()
	return self.boss_info3
end

function WorldBossData:SetWorldBossResult(protocol)
	self.rank_list = protocol.rank_list
end

function WorldBossData:SetWorldBossPosInfo(protocol)
	self.pos_info = protocol.pos_info
end

function WorldBossData:SetWorldBoss2PosInfo(protocol)
	self.pos_info2 = protocol.pos_info
end

function WorldBossData:SetWorldBoss3PosInfo(protocol)
	self.pos_info3 = protocol.pos_info
end

function WorldBossData:GetPosInfo()
	return self.pos_info
end

function WorldBossData:GetPosInfo2()
	return self.pos_info2
end

function WorldBossData:GetPosInfo3()
	return self.pos_info3
end

function WorldBossData:GetActType()
	local act_type = 0
	for k, v in pairs(self.config_monster) do
		if self.pos_info.id == v.npc_id then
			act_type = v.act_type
		end
	end
	if act_type == 0 then 
	    for k,v in pairs(self.config3_monster) do 
			if self.pos_info3.id == v.npc_id then
				act_type = v.act_type
			end
	    end 
    end 
	return act_type
end

function WorldBossData:GetRankListWithType(type)
	return self.rank_list[type] or {}
end

function WorldBossData:GetRankTypeList()
	return self.rank_type_list
end

function WorldBossData:GetRewardList()
	local reward_list = {}
	local role_level = RoleData.Instance:GetRoleLevel()
	local act_type = self.result_act_type or ActType.WorldBoss
	for k, v in pairs(self.config_reward_show) do
		if v.act_type == act_type and v.type <= 3 and (role_level >= v.min_level and role_level <= v.max_level) then
			local t = {}
			t.rewards = {}
			t.sp_name = WorldBossConfig.RewardNameSp[v.type]
			for _, item_id in pairs(string.split(v.reward, "|")) do
				table.insert(t.rewards, Item.Init(tonumber(item_id)))
			end
			reward_list[v.type] = t
		end
	end
	for k, v in pairs(self.config_reward_show_3) do
		if v.act_type == act_type and v.type <= 3 and (role_level >= v.min_level and role_level <= v.max_level) then
			local t = {}
			t.rewards = {}
			t.sp_name = WorldBossConfig.RewardNameSp[v.type]
			for _, item_id in pairs(string.split(v.reward, "|")) do
				table.insert(t.rewards, Item.Init(tonumber(item_id)))
			end
			reward_list[v.type] = t
		end
	end
	return reward_list
end

function WorldBossData:GetDetailInfo(act_type, npc_name)
	local rewards = {}
	local co = ActivityData.Instance:GetActRewardData(act_type)
	if co then
		for _, item_id in pairs(string.split(co.reward_id, ":")) do
			table.insert(rewards, Item.Init(tonumber(item_id)))
		end
	end
	local tips = {}
	local detail_tips = Language.WorldBoss.DetailTips
	for i = 1, #detail_tips do
		tips[i] = string.format(detail_tips[i], npc_name)
	end
	return NpcDialogData.CreateDetailInfo1{tips = tips, rewards = rewards}
end

function WorldBossData:GetBattleWarningText()
	return string.format(Language.WorldBoss.BattleWarning, NpcDialogCtrl.Instance:NpcInfo(self.pos_info.id).name)
end

function WorldBossData:GetBattleWarningText2()
	return string.format(Language.WorldBoss.BattleWarning, NpcDialogCtrl.Instance:NpcInfo(self.pos_info2.id).name)
end

function WorldBossData:GetBattleWarningText3()
	return string.format(Language.WorldBoss.BattleWarning, NpcDialogCtrl.Instance:NpcInfo(self.pos_info3.id).name)
end


function WorldBossData:GetWorldBossPreReward(act_type)
	local language = Language.WorldBoss.PreReward
	local reward_list = {
        [1] = {
            toggle_name = language.tab_names[1],
            reward_item_list = {}
		},
		[2] = {
            toggle_name = language.tab_names[2],
            reward_item_list = {}
        },
        [3] = {
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = act_type}}
        }
	}

	table.insert(reward_list[1].reward_item_list, self:GetBossReward(act_type, 5))
	table.insert(reward_list[1].reward_item_list, self:GetBossReward(act_type, 3))
	table.insert(reward_list[1].reward_item_list, self:GetBossReward(act_type, 4))
	table.insert(reward_list[2].reward_item_list, self:GetBossReward(act_type, 1))
	table.insert(reward_list[2].reward_item_list, self:GetBossReward(act_type, 2))
	return reward_list
end
function WorldBossData:BossRankInfo()
	return self.boss_rank_info
end
function WorldBossData:MyRankInfo()
	return self.my_rank_info
end
function WorldBossData:GetBossReward(act_type, type, level)
	local language = Language.WorldBoss.PreReward
	level = level or RoleData.Instance:GetRoleLevel()
	act_type = act_type or self:ActType()
	local t = {
		title_name = language.title_names[type],
		item_list = {},
	}
	for k, v in pairs(self.config_reward_show2) do
		if v.act_type == act_type and v.type == type and (level >= v.min_level and level <= v.max_level) then
			for i = 0, #v.reward_show do
				local reward = v.reward_show[i]
				table.insert(t.item_list, Item.Init(reward.item_id, reward.num, reward.is_bind))
			end
		end
	end
	for k, v in pairs(self.config_reward_show3) do
		if v.act_type == act_type and v.type == type and (level >= v.min_level and level <= v.max_level) then
			for i = 0, #v.reward_show do
				local reward = v.reward_show[i]
			table.insert(t.item_list, Item.Init(reward.item_id, reward.num, reward.is_bind))
			end
		end
	end
	return t
end
function WorldBossData:ActType()
	local act_type = ActType.WorldBoss
	if ActivityData.IsOpen(act_type) then
		act_type = ActType.WorldBoss
	end
	if ActivityData.IsOpen(ActType.WorldBoss2) then
		act_type = ActType.WorldBoss2
	end
	if ActivityData.IsOpen(ActType.WorldBoss3) then
		act_type = ActType.WorldBoss3
	end
	return act_type
end