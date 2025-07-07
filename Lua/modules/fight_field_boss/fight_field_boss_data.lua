FightFieldBossColor = {
	[1] = COLORSTR.Blue2,
	[2] = COLORSTR.Purple1,
	[3] = COLORSTR.Yellow6,
	[4] = COLORSTR.Red8,
}

FightFieldBossData = FightFieldBossData or BaseClass()

function FightFieldBossData:__init()
    if FightFieldBossData.Instance ~= nil then
        Debuger.LogError("[FightFieldBossData] attempt to create singleton twice!")
        return
    end
	FightFieldBossData.Instance = self
	self.auto_to_boss_smdata = SmartData.New({auto_to_seq = -1, boss_pos = {}})
	self:InitFieldBossData()
	self:Initialization()
end

function FightFieldBossData:Initialization()
	self.field_boss_data = SmartData.New({type = 1,val = false,wild_number = 0})
end

function FightFieldBossData:InitFieldBossData()
	self.field_boss_config = KeyList(Config.monster_yewaiboss_auto.monster_list,"npc_id")
	self.hill_sea_config = KeyList(Config.monster_shxyboss_auto.monster,"npc_id")
	self.wild_boss_data = SmartData.New({today_times = 0,next_refresh_time = {},wild_number_list = {}})
	self.hill_sea_data = SmartData.New({today_times = 0,next_refresh_time = 0,info_list = {}})
	self.field_boss_list = self:GetFieldBossConfig(Config.monster_yewaiboss_auto)
	self.boss_config = KeyList(Config.monster_yewaiboss_auto.monster_list,"npc_number")
	self.boss_id_config = KeyList(Config.monster_yewaiboss_auto.monster_list,"npc_id")
	self.boss_poker_info = SmartData.New()
end

function FightFieldBossData:GetFieldBossConfig(config)
	local boss_list,page_list = {},{}
	for k,v in pairs(config.monster) do
		if boss_list[v.level_group] == nil then
			boss_list[v.level_group] = {}
			page_list[v.level_group] = config.level_group[v.level_group]
			page_list[v.level_group].wild_number = v.wild_number
			local show_config = config.level_group[v.level_group]
		end
		table.insert(boss_list[v.level_group],v)
	end
	for k,v in pairs(boss_list) do
		table.sort(v,function(a,b)return a.seq < b.seq end)
	end
	return {field_boss_list = boss_list,field_boss_page = page_list}
end

--野外首领
function FightFieldBossData:SetWildBossInfo(protocol)
	self.wild_boss_data.today_times = protocol.today_times
	self.wild_boss_data.next_refresh_time = protocol.next_refresh_time
	self.wild_boss_data.wild_number_list = protocol.wild_number_list
end

function FightFieldBossData:SetWildBossSingleInfo(protocol)
	self.wild_boss_data.next_refresh_time[protocol.index] = protocol.next_refresh_time
	self.wild_boss_data.wild_number_list[protocol.index] = protocol.wild_number
end

function FightFieldBossData:GetNextRefreshTime(seq)
	return self.wild_boss_data.next_refresh_time[seq] or 0
end

function FightFieldBossData:SetFieldBossType(data)
	self.field_boss_data.type = data.id
	self.field_boss_data.wild_number = data.wild_number
	self.field_boss_data.val = not self.field_boss_data.val
end

function FightFieldBossData:GetFieldBossType()
	return self.field_boss_data.type
end

function FightFieldBossData:GetFieldBossRewardData(page)
	local group_id = BattleData.Instance:GetBossGroupId()
	local config = MonsterData.Instance:GetGroupConfig(group_id) or {}
	local reward_cfg = page == 1 and self.field_boss_config[config.id or 0] or self.hill_sea_config[config.id or 0]
	return reward_cfg and DataHelper.FormatItemList(reward_cfg.reward) or {}
end

--获取副本结算信息下发
function FightFieldBossData:SetWorldBossRewardData(data)
	self.world_boss_reward_data = data
end

function FightFieldBossData:GetWorldBossRewardData()
	return self.world_boss_reward_data
end

function FightFieldBossData:GetBossConfig(npc_number)
	return self.boss_config[npc_number]
end
function FightFieldBossData:GetBossConfigById(npc_id)
	return self.boss_id_config[npc_id]
end

--翻牌信息======================================
function FightFieldBossData:SetBossPokerInfo(info)
	self.boss_poker_info:Set(info or {})
end

function FightFieldBossData:GetBossPokerInfo()
	return self.boss_poker_info
end

function FightFieldBossData:GetPokerList()
	return self.boss_poker_info.mypoker or {}
end

function FightFieldBossData:GetWildNumber(seq)
	return self.wild_boss_data.wild_number_list[seq] or 0
end

function FightFieldBossData:IsSceneWild(id)
	if self.field_boss_config[id] then
		--return self.field_boss_config[id].npc_number > self:GetMaxNumber()
		local npc_number = self.field_boss_config[id].npc_number
		for i,v in pairs(Config.monster_yewaiboss_auto.monster) do
			if npc_number == v.wild_number then
				return true
			end
		end
	end
	return false
end

function FightFieldBossData:GetMaxNumber()
	local config = Config.monster_yewaiboss_auto.monster
	return config[#config].ordinary_number
end

--======================================
-- 山海降妖玩法
function FightFieldBossData:SetShanHaiBossInfo(protocol)
	self.hill_sea_data.today_times = protocol.challenge_count
	self.hill_sea_data.next_refresh_time = protocol.next_refresh_time
	self.hill_sea_data.info_list = protocol.info_list
end

function FightFieldBossData:SetShanHaiBossRoleInfo(protocol)
	self.hill_sea_data.today_times = protocol.challenge_count
	self.hill_sea_data.next_refresh_time = protocol.next_refresh_time
end

function FightFieldBossData:SetShanHaiBossSingleInfo(protocol)
	self.hill_sea_data.info_list[protocol.npc_id] = protocol.info
end

function FightFieldBossData:GetSingleBossInfo(npc_id)
	return self.hill_sea_data.info_list[npc_id]
end

function FightFieldBossData:GetGetHillSeaType(npc_id)
	local co = self.hill_sea_config[npc_id]
	return co and co.grade_type or nil
end

function FightFieldBossData:GetHillSeaRefreshTime()
	return self.hill_sea_data.next_refresh_time
end

function FightFieldBossData:GetSceneId(npc_id)
	local info = self:GetSingleBossInfo(npc_id)
	return info and info.scene_id or 0
end

function FightFieldBossData:GetHillSeaName(npc_id,not_color)
	local config = self.hill_sea_config[npc_id]
	if config ~= nil then
		local name = Language.FightFieldBoss.BossName[config.grade_type] or ""
		return not_color == nil and ColorStr(name,FightFieldBossColor[config.grade_type]) or name
	end
	return ""
end

function FightFieldBossData:IsDeeathNum(data)
	if data.fight_times == nil then
		return false
	end
	local info = self:GetSingleBossInfo(data.npc_id)
	if info ~= nil then
		return info.death_num >= data.fight_times
	end
	return false
end

function FightFieldBossData:GetBtnList()
	return {{name = Language.FightFieldBoss.Btns[1],mod = Mod.FightFieldBoss.Main},{name = Language.FightFieldBoss.Btns[2],mod = Mod.FightFieldBoss.Shxy}}
end

function FightFieldBossData:GetFieldBossPageList()
	return self.field_boss_list.field_boss_page
end

function FightFieldBossData:GetFieldBossList(level_group)
	return self.field_boss_list.field_boss_list[level_group] or {}
end

function FightFieldBossData:GetFieldBossTimes()
	return self.wild_boss_data.today_times or 0,Config.monster_yewaiboss_auto.other[1].weekly_time
end

function FightFieldBossData:GetHillSeaTimes()
	return self.hill_sea_data.today_times or 0,Config.monster_shxyboss_auto.other[1].day_time
end

function FightFieldBossData:GetHillSeaList()
	local list = {}
	for k,v in pairs(self.hill_sea_data.info_list) do
		table.insert(list,self.hill_sea_config[k])
	end
	table.sort(list,DataHelper.SortFunc([[seq]]))
	return list
end

function FightFieldBossData:SetBossPos(protocol)
	self.auto_to_boss_smdata.boss_pos = {x = protocol.pos.x, y = protocol.pos.y}
	self.auto_to_boss_smdata:SetDirty("boss_pos")
end