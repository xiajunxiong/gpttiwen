FightDenData = FightDenData or BaseClass()
FightDenData.AchievementState = {
    NotEnough = 0,
    HasGet = 1,
    CanGet = 2
}
function FightDenData:__init()
    if FightDenData.Instance ~= nil then
        Debuger.LogError("[FightDenData] attempt to create singleton twice!")
        return
    end
    FightDenData.Instance = self
    self.monster_cave_auto = Config.monster_cave_auto
    self.cave_content = self.monster_cave_auto.cave_content
    self.other_info = self.monster_cave_auto.other[1]
    self.reward_random = self.monster_cave_auto.reward_random

    self.role_level = self.other_info.open_level
    self.wave = SmartData.New({num = 1})
    self.achievement_smart = SmartData.New({achieve_state = false})
    self.reward_list = nil
    self.reward_random_list = {}
    self.finish_reward = {}
    self.battle_map_id = nil
    self.cur_view_select_mod = 0
    Cfg.Register("CaveCfgByMonsterId", Cfg.RealTimeSearch(Config.monster_cave_auto.cave_content,"monster_group"))
    self:CheckConfig()
end

function FightDenData:CheckConfig()
    for k,v in ipairs(self.reward_random) do
        if self.reward_random_list[v.random] == nil then
            self.reward_random_list[v.random] = {}
        end
        table.insert(self.reward_random_list[v.random], v.reward)
    end
end

function FightDenData:CheckCaveReward()
    self.reward_list = {}
    for key, value in pairs(self.cave_content) do
        if self.reward_list[value.monster_difficulty] == nil then
            self.reward_list[value.monster_difficulty] = {}
        end
        if self.reward_list[value.monster_difficulty][value.level] == nil then
            self.reward_list[value.monster_difficulty][value.level] = {}
        end
        self.reward_list[value.monster_difficulty][value.level][value.num] = value
        -- 将奖励转换成Item
        -- local temp = {}
        -- local group_list = string.split(value.show_item,"|")
        -- for index, valu in ipairs(group_list) do
        --     local item_struct = string.split(valu,":")
        --     table.insert(temp, Item.Create({item_id = tonumber(item_struct[1]), num = tonumber(item_struct[2])}))
        -- end
        -- self.reward_list[value.level][value.num].reward_item_list = temp
        -- self.reward_list[value.level][value.num].num = value.num
    end
    --LogError("self.reward_list",self.reward_list)
end

function FightDenData:GetMaxWave()
    return self.other_info.max_num
end

function FightDenData:GetOpenLevel()
    return self.other_info.open_level
end

function FightDenData:SetLevel(role_level)
    self.role_level = role_level
end

function FightDenData:SetWave(wave)
    self.wave.num = wave
end

function FightDenData:GetShowReward(role_level, mod, in_battle)
    if self.reward_list == nil then
        self:CheckCaveReward()
    end
    mod = mod or self:GetMonsterMod(in_battle)
    --LogError("GetShowReward mod",mod)
    local base_cfg_list = self.reward_list[mod][role_level or self.role_level]
    local t = {}
    for k, v in pairs(base_cfg_list) do
        t[k] = {}
        t[k].cfg = v
        local temp = {}
        local group_list = string.split(v.show_item,",")
        for _, valu in ipairs(group_list) do
            local item_struct = string.split(valu,":")
            -- table.insert(temp, Item.Create({item_id = tonumber(item_struct[1]), num = tonumber(item_struct[2])}))
            table.insert(temp, item_struct)
        end
        if not StringIsEmpty(v.show_R_item) then
            local rate_group_list = string.split(v.show_R_item, ",")
            for _,valu in ipairs(rate_group_list) do
                local item_struct = string.split(valu,":")
                item_struct.rate = (v.show_probability / 100).."%"
                table.insert(temp, item_struct)
            end
        end
        t[k].reward_item_list = temp
        t[k].num = v.num
    end
    -- 根据等级显示总波数奖励
    return t
end
--获取当前的难度接口（0：普通，1：困难）
function FightDenData:GetMonsterMod(in_battle)
    local temp = self.cur_view_select_mod
    if in_battle then
        local monster_group_id = BattleData.Instance:BaseData().monster_group_id
        if monster_group_id ~= -1 then
            local cfg = Cfg.CaveCfgByMonsterId(monster_group_id)
            if cfg and cfg.monster_difficulty then
                temp = cfg.monster_difficulty
            end
        end
    end
    return temp
end

function FightDenData:GetReward()
    local result = {}
    for key, value in pairs(self.finish_reward or {}) do
        table.insert( result, Item.Create(value))
    end
    return result
end

function FightDenData:GetWave()
    return self.wave
end
function FightDenData:SetPassWave(pass_wave, mod)
    if mod then
        self.pass_wave = self.pass_wave or {}
        self.pass_wave[mod] = pass_wave
    else
        self.pass_wave = pass_wave
    end
end

function FightDenData:SetRewardFlag(flag)
    self.reward_flag = flag
end

function FightDenData:GetRewardFlag(mod, num)
    return self.reward_flag and self.reward_flag[mod][num] or 0
end

function FightDenData:PassWave(in_battle)
    return self.pass_wave[self:GetMonsterMod(in_battle)]
end
function FightDenData:SetFinishReward(list)
    self.finish_reward = {}
    -- 堆叠一遍物品
    for index, value in ipairs(list) do
        if self.finish_reward[value.item_id] == nil then
            self.finish_reward[value.item_id] = value
        else
            self.finish_reward[value.item_id].num = self.finish_reward[value.item_id].num + value.num
        end
    end
    ViewMgr:FlushView(FightDenFinish)
end

function FightDenData:ClearFinishReward()
    self.finish_reward = {}
end

function FightDenData:GetBattleWarningText()
    local level = DataHelper.GetDaXie(self.wave.num or 1)
    return Format(Language.FightDen.WaveTip,level)
end

function FightDenData:ResultLevel(level)
    level = level or RoleData.Instance:GetRoleLevel()
    local result = 0
    
    if self.reward_list == nil then
        self:CheckCaveReward()
    end

    for k, v in pairs(self.reward_list[0]) do
        if k <= level then
            result = math.max(k, result)
        end
    end
    return result
end
--跟队长的等级匹配的等级
function FightDenData:GetLeaderLevel(level)
    --[[ if not TeamCtrl.Instance:Data():IsLeader() then
        local info = TeamData.Instance:GetLeaderInfo()
        local result = self:ResultLevel(info.level)
        if result ~= self:ResultLevel() then
            return result
        end
    end ]]
    level = level or RoleData.Instance:GetRoleLevel()
    if TeamCtrl.Instance:Data():InTeam() then
        local list = TeamCtrl.Instance:Data():GetMemberList()
        for index, value in ipairs(list) do
            level = math.max(level, value.info.level)
        end
    end
    if self.reward_list == nil then
        self:CheckCaveReward()
    end
    local result = 0
    for k, v in pairs(self.reward_list[0]) do
        if k <= level then
            result = math.max(k, result)
        end
    end
    return result
end

function FightDenData:SetBattleMapId(role_level)
    local map_id = self.reward_list[self:GetMonsterMod()][self:ResultLevel(role_level)][1].map_id_cfg
    self.battle_map_id = map_id
end

function FightDenData:GetBattleMapId()
    return self.battle_map_id
end

function FightDenData:ClearBattleMapId()
    self.battle_map_id = nil
end

function FightDenData:RemindAchievement()
    if TableIsEmpty(self.achievement_info) or self.achievement_info.pass_wave_cond_times == nil or self.achievement_info.pass_wave_cond_times <= 0 then
        return 0
    end
    for key, value in pairs(Config.monster_cave_auto.achievement) do
        if value.num <= self.achievement_info.pass_wave_cond_times and self.achievement_info.achievement_fecth_flag[value.index] == 0 then
            return 1
        end
    end
    return 0
end

function FightDenData:SetAchievementInfo(protocol)
    self.achievement_info = self.achievement_info or {}
    self.achievement_info.pass_wave_cond_times = protocol.pass_wave_cond_times
    self.achievement_info.achievement_fecth_flag = protocol.achievement_fecth_flag
    self.achievement_smart:SetDirty("achieve_state")
end

function FightDenData:GetRewardState(index, condtion_num)
    if self.achievement_info == nil or condtion_num > self.achievement_info.pass_wave_cond_times then
        return FightDenData.AchievementState.NotEnough
    elseif self.achievement_info.achievement_fecth_flag[index] == 1 then
        return FightDenData.AchievementState.HasGet
    else
        return FightDenData.AchievementState.CanGet
    end
end